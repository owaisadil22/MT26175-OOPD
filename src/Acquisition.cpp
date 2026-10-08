#include "bookmgmt/Acquisition.h"

#include <algorithm>
#include <iomanip>
#include <ostream>

#include "bookmgmt/Exceptions.h"

namespace bookmgmt {

AcquisitionManager::AcquisitionManager(Catalog& catalog, Budget& budget)
    : catalog_(catalog), budget_(budget) {}

Money AcquisitionManager::quote(const std::string& id, int quantity) const {
    const Resource& r = catalog_.get(id);
    return r.costFor(quantity);
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

    bool isNewTitle = (catalog_.holdings(id) == 0);
    Money cost = r->costFor(quantity);
    std::string why = budget_.check(r->category(), quantity, cost, isNewTitle);
    if (!why.empty()) {
        if (reason) *reason = why;
        return false;
    }
    return true;
}

const PurchaseRecord& AcquisitionManager::purchase(const std::string& id,
                                                   int quantity) {
    if (quantity <= 0) throw std::invalid_argument("quantity must be > 0");

    const Resource* r = catalog_.find(id);
    if (!r) throw NotFoundError("Resource not found: " + id);

    Money cost = r->costFor(quantity);
    bool isNewTitle = (catalog_.holdings(id) == 0);

    std::string why = budget_.check(r->category(), quantity, cost, isNewTitle);
    if (!why.empty()) {
        if (why.find("Quota") != std::string::npos || why.find("quota") != std::string::npos) {
            throw QuotaExceededError(why);
        }
        throw BudgetExceededError(why);
    }

    budget_.commit(r->category(), quantity, cost, isNewTitle);
    catalog_.addHoldings(id, quantity);

    return record(r, id, quantity, cost, true, "");
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

    budget_.refund(it->category, it->quantity, it->cost, titleRemoved);

    const Resource* r = catalog_.find(it->resourceId);
    return record(r, it->resourceId, it->quantity, it->cost, true,
                  "Cancelled Order #" + std::to_string(orderNo), true);
}

std::vector<PurchaseRecord> AcquisitionManager::processBatch(
    const std::vector<PurchaseRequest>& reqs) {
    std::vector<PurchaseRecord> batchRecords;
    for (const auto& req : reqs) {
        const Resource* r = catalog_.find(req.resourceId);
        if (!r) {
            batchRecords.push_back(record(nullptr, req.resourceId, req.quantity,
                                          Money::fromMinor(0), false,
                                          "Resource not found: " + req.resourceId));
            continue;
        }

        if (req.quantity <= 0) {
            batchRecords.push_back(record(r, req.resourceId, req.quantity,
                                          Money::fromMinor(0), false,
                                          "quantity must be > 0"));
            continue;
        }

        Money cost = r->costFor(req.quantity);
        bool isNewTitle = (catalog_.holdings(req.resourceId) == 0);
        std::string why = budget_.check(r->category(), req.quantity, cost, isNewTitle);
        if (!why.empty()) {
            batchRecords.push_back(
                record(r, req.resourceId, req.quantity, cost, false, why));
        } else {
            budget_.commit(r->category(), req.quantity, cost, isNewTitle);
            catalog_.addHoldings(req.resourceId, req.quantity);
            batchRecords.push_back(
                record(r, req.resourceId, req.quantity, cost, true, ""));
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
                                            std::string reason, bool isCancellation) {
    history_.push_back({
        nextOrderNo_++,
        id,
        r ? r->title() : "",
        r ? r->category() : ResourceCategory::Book,
        qty,
        cost,
        approved,
        std::move(reason),
        isCancellation
    });
    return history_.back();
}

void AcquisitionManager::printReport(std::ostream& os) const {
    os << "=== ACQUISITION REPORT ===\n";
    for (const auto& rec : history_) {
        std::string status = rec.isCancellation ? "CANCELLED" : (rec.approved ? "APPROVED" : "REJECTED");
        os << "Order #" << rec.orderNo << " [" << status << "] "
           << rec.resourceId << " (" << rec.quantity << " units) - Cost: " << rec.cost;
        if (!rec.reason.empty()) os << " Reason: " << rec.reason;
        os << "\n";
    }
    os << "Total Spent: " << totalSpent() << "\n";
}

}  // namespace bookmgmt