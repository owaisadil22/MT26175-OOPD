#include "bookmgmt/Acquisition.h"

#include <algorithm>
#include <iomanip>
#include <ostream>

#include "bookmgmt/Exceptions.h"

namespace bookmgmt {

AcquisitionManager::AcquisitionManager(Catalog& catalog, Budget& budget)
    : catalog_(catalog), budget_(budget) {}

Money AcquisitionManager::quote(const std::string& id, int quantity, const std::string& vendor) const {
    const Resource& r = catalog_.get(id);
    if (!vendor.empty() && vendor != "Default") {
        return r.costForVendor(vendor, quantity);
    }
    auto cheapest = r.cheapestVendor();
    if (cheapest.has_value()) {
        return r.costForVendor(cheapest->vendorName, quantity);
    }
    return r.costFor(quantity);
}

bool AcquisitionManager::canPurchase(const std::string& id, int quantity,
                                     std::string* reason, const std::string& dept,
                                     const std::string& vendor) const {
    if (quantity <= 0) {
        if (reason) *reason = "quantity must be > 0";
        return false;
    }
    const Resource* r = catalog_.find(id);
    if (!r) {
        if (reason) *reason = "Resource not found: " + id;
        return false;
    }

    Money cost = quote(id, quantity, vendor);
    bool isNewTitle = (catalog_.holdings(id) == 0);

    std::string why = dept.empty() 
        ? budget_.check(r->category(), quantity, cost, isNewTitle)
        : budget_.checkDepartment(dept, r->category(), quantity, cost, isNewTitle);

    if (!why.empty()) {
        if (reason) *reason = why;
        return false;
    }
    return true;
}

const PurchaseRecord& AcquisitionManager::purchase(const std::string& id, int quantity,
                                                   const std::string& dept, const std::string& vendor) {
    if (quantity <= 0) throw std::invalid_argument("quantity must be > 0");

    const Resource* r = catalog_.find(id);
    if (!r) throw NotFoundError("Resource not found: " + id);

    std::string usedVendor = vendor;
    if (usedVendor.empty() || usedVendor == "Default") {
        auto cheapest = r->cheapestVendor();
        usedVendor = cheapest.has_value() ? cheapest->vendorName : "Default";
    }

    Money cost = quote(id, quantity, usedVendor);
    bool isNewTitle = (catalog_.holdings(id) == 0);

    std::string why = dept.empty() 
        ? budget_.check(r->category(), quantity, cost, isNewTitle)
        : budget_.checkDepartment(dept, r->category(), quantity, cost, isNewTitle);

    if (!why.empty()) {
        if (why.find("Quota") != std::string::npos || why.find("quota") != std::string::npos) {
            throw QuotaExceededError(why);
        }
        throw BudgetExceededError(why);
    }

    if (dept.empty()) {
        budget_.commit(r->category(), quantity, cost, isNewTitle);
    } else {
        budget_.commitDepartment(dept, r->category(), quantity, cost, isNewTitle);
    }
    catalog_.addHoldings(id, quantity);

    return record(r, id, quantity, cost, true, "", false, dept, usedVendor);
}

const PurchaseRecord& AcquisitionManager::cancelOrder(std::size_t orderNo) {
    auto it = std::find_if(history_.begin(), history_.end(),
                           [orderNo](const PurchaseRecord& rec) {
                               return rec.orderNo == orderNo;
                           });

    if (it == history_.end()) {
        throw NotFoundError("Order number not found: " + std::to_string(orderNo));
    }

    if (!it->approved) {
        throw std::invalid_argument("Cannot cancel an unapproved order");
    }

    if (it->isCancellation) {
        throw std::invalid_argument("Cannot cancel a cancellation record");
    }

    int currentHoldings = catalog_.holdings(it->resourceId);
    catalog_.addHoldings(it->resourceId, -it->quantity);
    bool titleRemoved = (currentHoldings - it->quantity == 0);

    if (it->department.empty()) {
        budget_.refund(it->category, it->quantity, it->cost, titleRemoved);
    } else {
        budget_.refundDepartment(it->department, it->category, it->quantity, it->cost, titleRemoved);
    }

    const Resource* r = catalog_.find(it->resourceId);
    return record(r, it->resourceId, it->quantity, it->cost, true,
                  "Cancelled Order #" + std::to_string(orderNo), true, it->department, it->vendor);
}

std::vector<PurchaseRecord> AcquisitionManager::processBatch(
    const std::vector<PurchaseRequest>& reqs, bool allOrNothing) {
    
    if (allOrNothing) {
        Budget tempBudget = budget_;
        bool anyFailed = false;

        for (const auto& req : reqs) {
            if (req.quantity <= 0) {
                anyFailed = true;
                break;
            }
            const Resource* r = catalog_.find(req.resourceId);
            if (!r) {
                anyFailed = true;
                break;
            }

            Money cost = quote(req.resourceId, req.quantity, req.vendor);
            bool isNewTitle = (catalog_.holdings(req.resourceId) == 0);

            std::string why = req.department.empty()
                ? tempBudget.check(r->category(), req.quantity, cost, isNewTitle)
                : tempBudget.checkDepartment(req.department, r->category(), req.quantity, cost, isNewTitle);

            if (!why.empty()) {
                anyFailed = true;
                break;
            }

            if (req.department.empty()) {
                tempBudget.commit(r->category(), req.quantity, cost, isNewTitle);
            } else {
                tempBudget.commitDepartment(req.department, r->category(), req.quantity, cost, isNewTitle);
            }
        }

        if (anyFailed) {
            std::vector<PurchaseRecord> rejectedRecords;
            for (const auto& req : reqs) {
                const Resource* r = catalog_.find(req.resourceId);
                Money cost = (r && req.quantity > 0) ? quote(req.resourceId, req.quantity, req.vendor) : Money::fromMinor(0);
                std::string usedVendor = req.vendor;
                if ((usedVendor.empty() || usedVendor == "Default") && r) {
                    auto cheapest = r->cheapestVendor();
                    usedVendor = cheapest.has_value() ? cheapest->vendorName : "Default";
                }
                rejectedRecords.push_back(
                    record(r, req.resourceId, req.quantity, cost, false,
                           "Batch processing rejected due to all-or-nothing policy", false, req.department, usedVendor));
            }
            return rejectedRecords;
        }
    }

    std::vector<PurchaseRecord> batchRecords;
    for (const auto& req : reqs) {
        const Resource* r = catalog_.find(req.resourceId);
        if (!r) {
            batchRecords.push_back(record(nullptr, req.resourceId, req.quantity,
                                          Money::fromMinor(0), false,
                                          "Resource not found: " + req.resourceId, false, req.department, "Default"));
            continue;
        }

        if (req.quantity <= 0) {
            batchRecords.push_back(record(r, req.resourceId, req.quantity,
                                          Money::fromMinor(0), false,
                                          "quantity must be > 0", false, req.department, "Default"));
            continue;
        }

        std::string usedVendor = req.vendor;
        if (usedVendor.empty() || usedVendor == "Default") {
            auto cheapest = r->cheapestVendor();
            usedVendor = cheapest.has_value() ? cheapest->vendorName : "Default";
        }

        Money cost = quote(req.resourceId, req.quantity, usedVendor);
        bool isNewTitle = (catalog_.holdings(req.resourceId) == 0);
        
        std::string why = req.department.empty()
            ? budget_.check(r->category(), req.quantity, cost, isNewTitle)
            : budget_.checkDepartment(req.department, r->category(), req.quantity, cost, isNewTitle);

        if (!why.empty()) {
            batchRecords.push_back(
                record(r, req.resourceId, req.quantity, cost, false, why, false, req.department, usedVendor));
        } else {
            if (req.department.empty()) {
                budget_.commit(r->category(), req.quantity, cost, isNewTitle);
            } else {
                budget_.commitDepartment(req.department, r->category(), req.quantity, cost, isNewTitle);
            }
            catalog_.addHoldings(req.resourceId, req.quantity);
            batchRecords.push_back(
                record(r, req.resourceId, req.quantity, cost, true, "", false, req.department, usedVendor));
        }
    }
    return batchRecords;
}

Money AcquisitionManager::totalSpent() const {
    Money sum;
    for (const auto& rec : history_) {
        if (rec.approved && !rec.isCancellation) sum += rec.cost;
        if (rec.isCancellation) sum -= rec.cost;
    }
    return sum;
}

PurchaseRecord& AcquisitionManager::record(const Resource* r,
                                            const std::string& id, int qty,
                                            Money cost, bool approved,
                                            std::string reason, bool isCancellation,
                                            std::string dept, std::string vendor) {
    history_.push_back({
        nextOrderNo_++,
        id,
        r ? r->title() : "",
        r ? r->category() : ResourceCategory::Book,
        qty,
        cost,
        approved,
        std::move(reason),
        isCancellation,
        std::move(dept),
        std::move(vendor)
    });
    return history_.back();
}

void AcquisitionManager::printReport(std::ostream& os) const {
    os << "=== ACQUISITION REPORT ===\n";
    for (const auto& rec : history_) {
        std::string status = rec.isCancellation ? "CANCELLED" : (rec.approved ? "APPROVED" : "REJECTED");
        os << "Order #" << rec.orderNo << " [" << status << "] ";
        if (!rec.department.empty()) os << "[" << rec.department << "] ";
        os << "Vendor: " << rec.vendor << " - " << rec.resourceId << " (" << rec.quantity << " units) - Cost: " << rec.cost;
        if (!rec.reason.empty()) os << " Reason: " << rec.reason;
        os << "\n";
    }
    os << "Total Spent: " << totalSpent() << "\n";
}

}  // namespace bookmgmt