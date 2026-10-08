#pragma once
// Budget: an overall spending limit plus optional per-category purchase quotas.

#include <iosfwd>
#include <map>
#include <optional>
#include <string>

#include "bookmgmt/Money.h"
#include "bookmgmt/Resource.h"

namespace bookmgmt {

struct Quota {
    int maxUnits;                  // maximum copies/seats that may be bought
    Money maxSpend;                // maximum money that may be spent
    std::optional<int> maxTitles;  // Q7: maximum number of distinct titles allowed
};

struct Usage {
    int units = 0;
    Money spent;
    int titles = 0;                 // Q7: distinct titles currently held/purchased
};

class Budget {
public:
    explicit Budget(Money total);

    Money total() const { return total_; }
    Money spent() const { return spent_; }
    Money remaining() const { return total_ - spent_; }

    void setQuota(ResourceCategory c, Quota q);
    void removeQuota(ResourceCategory c);
    std::optional<Quota> quotaFor(ResourceCategory c) const;
    Usage usageFor(ResourceCategory c) const;

    std::optional<int> unitsRemaining(ResourceCategory c) const;
    std::optional<Money> spendRemaining(ResourceCategory c) const;
    std::optional<int> titlesRemaining(ResourceCategory c) const;

    std::string check(ResourceCategory c, int units, Money cost) const;
    std::string check(ResourceCategory c, int units, Money cost, bool isNewTitle) const;

    void commit(ResourceCategory c, int units, Money cost);
    void commit(ResourceCategory c, int units, Money cost, bool isNewTitle);

    // Q8: Refund spent budget and quota usage
    void refund(ResourceCategory c, int units, Money cost, bool titleRemoved = false);

    void print(std::ostream& os) const;

private:
    enum class Failure { None, BadInput, Quota, Overall };
    Failure evaluate(ResourceCategory c, int units, Money cost, std::string& why, bool isNewTitle = false) const;

    Money total_;
    Money spent_;
    std::map<ResourceCategory, Quota> quotas_;
    std::map<ResourceCategory, Usage> usage_;
};

}  // namespace bookmgmt