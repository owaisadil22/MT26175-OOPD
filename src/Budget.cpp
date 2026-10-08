#include "bookmgmt/Budget.h"

#include <iomanip>
#include <ostream>

#include "bookmgmt/Exceptions.h"

namespace bookmgmt {

Budget::Budget(Money total) : total_(total) {}

void Budget::setQuota(ResourceCategory c, Quota q) {
    quotas_[c] = q;
}

void Budget::removeQuota(ResourceCategory c) {
    quotas_.erase(c);
}

std::optional<Quota> Budget::quotaFor(ResourceCategory c) const {
    auto it = quotas_.find(c);
    if (it == quotas_.end()) return std::nullopt;
    return it->second;
}

Usage Budget::usageFor(ResourceCategory c) const {
    auto it = usage_.find(c);
    if (it == usage_.end()) return Usage{};
    return it->second;
}

std::optional<int> Budget::unitsRemaining(ResourceCategory c) const {
    auto q = quotaFor(c);
    if (!q) return std::nullopt;
    return q->maxUnits - usageFor(c).units;
}

std::optional<Money> Budget::spendRemaining(ResourceCategory c) const {
    auto q = quotaFor(c);
    if (!q) return std::nullopt;
    return q->maxSpend - usageFor(c).spent;
}

std::optional<int> Budget::titlesRemaining(ResourceCategory c) const {
    auto q = quotaFor(c);
    if (!q || !q->maxTitles.has_value()) return std::nullopt;
    return q->maxTitles.value() - usageFor(c).titles;
}

std::string Budget::check(ResourceCategory c, int units, Money cost) const {
    std::string why;
    evaluate(c, units, cost, why, false);
    return why;
}

std::string Budget::check(ResourceCategory c, int units, Money cost, bool isNewTitle) const {
    std::string why;
    evaluate(c, units, cost, why, isNewTitle);
    return why;
}

Budget::Failure Budget::evaluate(ResourceCategory c, int units, Money cost, std::string& why, bool isNewTitle) const {
    if (units <= 0) {
        why = "quantity must be > 0";
        return Failure::BadInput;
    }
    if (cost > remaining()) {
        why = "Exceeds overall budget";
        return Failure::Overall;
    }

    auto qIt = quotas_.find(c);
    if (qIt != quotas_.end()) {
        const auto& q = qIt->second;
        const auto& u = usageFor(c);
        if (q.maxUnits > 0 && u.units + units > q.maxUnits) {
            why = "Quota exceeded";
            return Failure::Quota;
        }
        if (q.maxSpend.minorUnits() > 0 && u.spent + cost > q.maxSpend) {
            why = "Quota exceeded";
            return Failure::Quota;
        }
        if (isNewTitle && q.maxTitles.has_value() && u.titles + 1 > q.maxTitles.value()) {
            why = "Quota distinct title limit exceeded";
            return Failure::Quota;
        }
    }
    why.clear();
    return Failure::None;
}

void Budget::commit(ResourceCategory c, int units, Money cost) {
    commit(c, units, cost, false);
}

void Budget::commit(ResourceCategory c, int units, Money cost, bool isNewTitle) {
    std::string why;
    Failure f = evaluate(c, units, cost, why, isNewTitle);
    if (f == Failure::Quota) throw QuotaExceededError(why);
    if (f == Failure::Overall) throw BudgetExceededError(why);
    if (f == Failure::BadInput) throw std::invalid_argument(why);

    spent_ += cost;
    auto& u = usage_[c];
    u.units += units;
    u.spent += cost;
    if (isNewTitle) {
        u.titles += 1;
    }
}

void Budget::refund(ResourceCategory c, int units, Money cost, bool titleRemoved) {
    spent_ -= cost;
    auto& u = usage_[c];
    u.units -= units;
    u.spent -= cost;
    if (titleRemoved && u.titles > 0) {
        u.titles -= 1;
    }
}

// Q9 Department Methods
void Budget::setDepartmentBudget(const std::string& dept, Money total) {
    deptBudgets_.emplace(dept, Budget(total));
}

void Budget::setDepartmentQuota(const std::string& dept, ResourceCategory c, Quota q) {
    auto it = deptBudgets_.find(dept);
    if (it != deptBudgets_.end()) {
        it->second.setQuota(c, q);
    }
}

std::string Budget::checkDepartment(const std::string& dept, ResourceCategory c, int units, Money cost, bool isNewTitle) const {
    auto it = deptBudgets_.find(dept);
    if (it == deptBudgets_.end()) {
        return "Department budget not found: " + dept;
    }
    std::string whyDept = it->second.check(c, units, cost, isNewTitle);
    if (!whyDept.empty()) {
        return "Department [" + dept + "]: " + whyDept;
    }
    return check(c, units, cost, isNewTitle);
}

void Budget::commitDepartment(const std::string& dept, ResourceCategory c, int units, Money cost, bool isNewTitle) {
    auto it = deptBudgets_.find(dept);
    if (it == deptBudgets_.end()) {
        throw NotFoundError("Department budget not found: " + dept);
    }
    it->second.commit(c, units, cost, isNewTitle);
    commit(c, units, cost, isNewTitle);
}

void Budget::refundDepartment(const std::string& dept, ResourceCategory c, int units, Money cost, bool titleRemoved) {
    auto it = deptBudgets_.find(dept);
    if (it != deptBudgets_.end()) {
        it->second.refund(c, units, cost, titleRemoved);
    }
    refund(c, units, cost, titleRemoved);
}

void Budget::print(std::ostream& os) const {
    os << "=== BUDGET REPORT ===\n"
       << "Total: " << total_ << " | Spent: " << spent_ << " | Remaining: " << remaining() << "\n";
    for (const auto& [cat, q] : quotas_) {
        const auto& u = usageFor(cat);
        os << "Category [" << categoryName(cat) << "]: "
           << "Units: " << u.units << "/" << q.maxUnits << ", "
           << "Spent: " << u.spent << "/" << q.maxSpend;
        if (q.maxTitles.has_value()) {
            os << ", Titles: " << u.titles << "/" << q.maxTitles.value();
        }
        os << "\n";
    }
}

}  // namespace bookmgmt