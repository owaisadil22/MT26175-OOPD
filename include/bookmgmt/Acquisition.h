#pragma once
// AcquisitionManager: coordinates purchases using Catalog and Budget.

#include <iosfwd>
#include <string>
#include <vector>

#include "bookmgmt/Budget.h"
#include "bookmgmt/Catalog.h"
#include "bookmgmt/Money.h"

namespace bookmgmt {

struct PurchaseRequest {
    std::string resourceId;
    int quantity;
};

struct PurchaseRecord {
    std::size_t orderNo;
    std::string resourceId;
    std::string title;
    ResourceCategory category;
    int quantity;
    Money cost;
    bool approved;
    std::string reason;
    bool isCancellation = false;  // Q8: track if record is a cancellation
};

class AcquisitionManager {
public:
    AcquisitionManager(Catalog& catalog, Budget& budget);

    Money quote(const std::string& id, int quantity) const;

    bool canPurchase(const std::string& id, int quantity,
                    std::string* reason = nullptr) const;

    const PurchaseRecord& purchase(const std::string& id, int quantity);

    // Q8: Cancel an approved order
    const PurchaseRecord& cancelOrder(std::size_t orderNo);

    std::vector<PurchaseRecord> processBatch(
        const std::vector<PurchaseRequest>& reqs);

    const std::vector<PurchaseRecord>& history() const { return history_; }

    Money totalSpent() const;

    void printReport(std::ostream& os) const;

private:
    PurchaseRecord& record(const Resource* r, const std::string& id, int qty,
                          Money cost, bool approved, std::string reason, bool isCancellation = false);

    Catalog& catalog_;
    Budget& budget_;
    std::vector<PurchaseRecord> history_;
    std::size_t nextOrderNo_ = 1;
};

}  // namespace bookmgmt