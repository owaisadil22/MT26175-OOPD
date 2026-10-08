#ifndef BOOKMGMT_THESIS_H
#define BOOKMGMT_THESIS_H

#include "Resource.h"
#include <string>

namespace bookmgmt {

// Answer to Q3 Justification for Thesis:
// Thesis inherits directly from Resource because it is generally open-access / free of cost
// (costFor returns Money::of(0)) and does not fit physical-copy pricing or seat licensing.

class Thesis : public Resource {
private:
    std::string university_;
    std::string degree_;
    std::string supervisor_;

public:
    Thesis(std::string id,
           std::string title,
           std::string publisher,
           int year,
           std::string university,
           std::string degree,
           std::string supervisor)
        : Resource(std::move(id), std::move(title), std::move(publisher), year, Money::of(0)),
          university_(std::move(university)),
          degree_(std::move(degree)),
          supervisor_(std::move(supervisor)) {}

    ResourceCategory category() const override {
        return ResourceCategory::Thesis;
    }

    Money costFor(int /*quantity*/) const override {
        return Money::of(0);
    }

    void printDetails(std::ostream& os) const override {
        Resource::printDetails(os);
        os << "  university: " << university_ << "\n"
           << "  degree: " << degree_ << "\n"
           << "  supervisor: " << supervisor_ << "\n";
    }

    const std::string& university() const { return university_; }
    const std::string& degree() const { return degree_; }
    const std::string& supervisor() const { return supervisor_; }
};

} // namespace bookmgmt

#endif // BOOKMGMT_THESIS_H