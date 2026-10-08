#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include "bookmgmt/Budget.h"
#include "bookmgmt/Catalog.h"

namespace bookmgmt {

struct PurchaseRequest {
    std::string resourceId;
    int quantity;
};

struct PurchaseRecord {
    int orderNo;
    std::string resourceId;
    std::string title;
    ResourceCategory category;
    int quantity;
    Money cost;          // Pre-tax cost (or primary cost)
    Money preTaxCost;    // Explicit pre-tax cost
    Money taxAmount;     // Tax amount
    Money postTaxCost;   // Post-tax total
    bool approved;
    std::string reason;
};

class AcquisitionManager {
public:
    AcquisitionManager(Catalog& catalog, Budget& budget);

    // Tax configuration (rate in percentage e.g. 5.0 for 5%)
    void setPrintTaxRate(double ratePercent) { printTaxRate_ = ratePercent; }
    void setElectronicTaxRate(double ratePercent) { electronicTaxRate_ = ratePercent; }
    double printTaxRate() const { return printTaxRate_; }
    double electronicTaxRate() const { return electronicTaxRate_; }

    // Cost calculation helpers
    Money taxFor(const Resource& r, Money preTax) const;
    Money preTaxCost(const Resource& r, int quantity) const;
    Money postTaxCost(const Resource& r, int quantity) const;

    Money quote(const std::string& id, int quantity) const;
    Money quotePostTax(const std::string& id, int quantity) const;

    bool canPurchase(const std::string& id, int quantity,
                     std::string* reason = nullptr) const;

    const PurchaseRecord& purchase(const std::string& id, int quantity);

    std::vector<PurchaseRecord> processBatch(const std::vector<PurchaseRequest>& reqs);

    const std::vector<PurchaseRecord>& history() const { return history_; }
    Money totalSpent() const;
    Money totalSpentPostTax() const;

    void printReport(std::ostream& os) const;

private:
    PurchaseRecord& record(const Resource* r, const std::string& id, int qty,
                           Money preTaxCost, Money taxAmount, Money postTaxCost,
                           bool approved, std::string reason);

    Catalog& catalog_;
    Budget& budget_;
    std::vector<PurchaseRecord> history_;
    int nextOrderNo_ = 1;

    double printTaxRate_ = 0.0;       // Default 0%
    double electronicTaxRate_ = 0.0;  // Default 0%
};

}  // namespace bookmgmt