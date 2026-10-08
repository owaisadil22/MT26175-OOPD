#ifndef BOOKMGMT_JOURNAL_H
#define BOOKMGMT_JOURNAL_H

#include "Resource.h"
#include <string>
#include <stdexcept>

namespace bookmgmt {

class Journal : public Resource {
private:
    std::string issn_;
    int issuesPerYear_;
    int subscriptionYears_;

public:
    Journal(std::string id,
            std::string title,
            std::string publisher,
            int year,
            Money annualUnitPrice,
            std::string issn,
            int issuesPerYear,
            int subscriptionYears = 1)
        : Resource(std::move(id), std::move(title), std::move(publisher), year, annualUnitPrice),
          issn_(std::move(issn)),
          issuesPerYear_(issuesPerYear),
          subscriptionYears_(subscriptionYears) {
        if (subscriptionYears < 1) {
            throw std::invalid_argument("subscription length must be at least 1 year");
        }
    }

    ResourceCategory category() const override {
        return ResourceCategory::Journal;
    }

    Money costFor(int quantity) const override {
        requirePositive(quantity);
        return unitPrice() * quantity * subscriptionYears_;
    }

    void printDetails(std::ostream& os) const override {
        os << "  ISSN: " << issn_ << "\n"
           << "  issues/year: " << issuesPerYear_ << "\n"
           << "  subscription years: " << subscriptionYears_ << "\n";
    }

    const std::string& issn() const { return issn_; }
    int issuesPerYear() const { return issuesPerYear_; }
    int subscriptionYears() const { return subscriptionYears_; }
};

} // namespace bookmgmt

#endif // BOOKMGMT_JOURNAL_H