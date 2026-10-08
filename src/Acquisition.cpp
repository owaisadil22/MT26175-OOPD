#include "bookmgmt/Acquisition.h"

#include <cmath>
#include <iomanip>
#include <ostream>

#include "bookmgmt/Exceptions.h"

namespace bookmgmt {

AcquisitionManager::AcquisitionManager(Catalog& catalog, Budget& budget)
    : catalog_(catalog), budget_(budget) {}

Money AcquisitionManager::taxFor(const Resource& r, Money preTax) const {
    double rate = r.isDigital() ? electronicTaxRate_ : printTaxRate_;
    std::int64_t taxMinor = static_cast<std::int64_t>(std::round(preTax.minorUnits() * (rate / 100.0)));
    return Money::fromMinor(taxMinor);
}

Money AcquisitionManager::preTaxCost(const Resource& r, int quantity) const {
    return r.costFor(quantity);
}

Money AcquisitionManager::postTaxCost(const Resource& r, int quantity) const {
    Money base = preTaxCost(r, quantity);
    return base + taxFor(r, base);
}

Money AcquisitionManager::quote(const std::string& id, int quantity) const {
    const Resource& r = catalog_.get(id);
    return preTaxCost(r, quantity);
}

Money AcquisitionManager::quotePostTax(const std::string& id, int quantity) const {
    const Resource& r = catalog_.get(id);
    return postTaxCost(r, quantity);
}

bool AcquisitionManager::canPurchase(const std::string& id, int quantity,
                                     std::string* reason) const {
    if (quantity <= 0) {
        if (reason) *reason = "quantity must be > 0";
        return false;
    }
    const Resource* r = catalog_.find(id);
    if (!r) {
        if (reason) *reason = "Resource not found: " + id;
        return false;
    }

    // Check budget / quota against post-tax cost
    Money cost = postTaxCost(*r, quantity);
    std::string why = budget_.check(r->category(), quantity, cost);
    if (!why.empty()) {
        if (reason) *reason = why;
        return false;
    }
    return true;
}

const PurchaseRecord& AcquisitionManager::purchase(const std::string& id, int quantity) {
    if (quantity <= 0) throw std::invalid_argument("quantity must be > 0");

    const Resource* r = catalog_.find(id);
    if (!r) throw NotFoundError("Resource not found: " + id);

    Money preTax = preTaxCost(*r, quantity);
    Money tax = taxFor(*r, preTax);
    Money postTax = preTax + tax;

    std::string why = budget_.check(r->category(), quantity, postTax);
    if (!why.empty()) {
        // DO NOT add to history_ on exceptions!
        if (why.find("Quota") != std::string::npos || why.find("quota") != std::string::npos) {
            throw QuotaExceededError(why);
        }
        throw BudgetExceededError(why);
    }

    // Commit post-tax cost to budget and update catalog holdings
    budget_.commit(r->category(), quantity, postTax);
    catalog_.addHoldings(id, quantity);

    return record(r, id, quantity, preTax, tax, postTax, true, "");
}

std::vector<PurchaseRecord> AcquisitionManager::processBatch(
    const std::vector<PurchaseRequest>& reqs) {
    std::vector<PurchaseRecord> batchRecords;
    for (const auto& req : reqs) {
        const Resource* r = catalog_.find(req.resourceId);
        if (!r) {
            batchRecords.push_back(record(nullptr, req.resourceId, req.quantity,
                                          Money::fromMinor(0), Money::fromMinor(0), Money::fromMinor(0),
                                          false, "Resource not found: " + req.resourceId));
            continue;
        }

        if (req.quantity <= 0) {
            batchRecords.push_back(record(r, req.resourceId, req.quantity,
                                          Money::fromMinor(0), Money::fromMinor(0), Money::fromMinor(0),
                                          false, "quantity must be > 0"));
            continue;
        }

        Money preTax = preTaxCost(*r, req.quantity);
        Money tax = taxFor(*r, preTax);
        Money postTax = preTax + tax;

        std::string why = budget_.check(r->category(), req.quantity, postTax);
        if (!why.empty()) {
            batchRecords.push_back(record(r, req.resourceId, req.quantity,
                                          preTax, tax, postTax, false, why));
        } else {
            budget_.commit(r->category(), req.quantity, postTax);
            catalog_.addHoldings(req.resourceId, req.quantity);
            batchRecords.push_back(record(r, req.resourceId, req.quantity,
                                          preTax, tax, postTax, true, ""));
        }
    }
    return batchRecords;
}

Money AcquisitionManager::totalSpent() const {
    Money sum;
    for (const auto& rec : history_) {
        if (rec.approved) sum += rec.cost;
    }
    return sum;
}

Money AcquisitionManager::totalSpentPostTax() const {
    Money sum;
    for (const auto& rec : history_) {
        if (rec.approved) sum += rec.postTaxCost;
    }
    return sum;
}

PurchaseRecord& AcquisitionManager::record(const Resource* r, const std::string& id,
                                            int qty, Money preTax, Money tax, Money postTax,
                                            bool approved, std::string reason) {
    history_.push_back({
        nextOrderNo_++,
        id,
        r ? r->title() : "",
        r ? r->category() : ResourceCategory::Book,
        qty,
        preTax,
        preTax,
        tax,
        postTax,
        approved,
        std::move(reason)
    });
    return history_.back();
}

void AcquisitionManager::printReport(std::ostream& os) const {
    os << "=== ACQUISITION REPORT ===\n";
    for (const auto& rec : history_) {
        os << "Order #" << rec.orderNo << " [" << (rec.approved ? "APPROVED" : "REJECTED") << "] "
           << rec.resourceId << " (" << rec.quantity << " units) - "
           << "Pre-tax: " << rec.preTaxCost << ", Tax: " << rec.taxAmount << ", Post-tax: " << rec.postTaxCost;
        if (!rec.approved) os << " Reason: " << rec.reason;
        os << "\n";
    }
    os << "Total Spent (Pre-tax): " << totalSpent() << "\n"
       << "Total Spent (Post-tax): " << totalSpentPostTax() << "\n";
}

}  // namespace bookmgmt