#ifndef BOOKMGMT_AUDIOBOOK_H
#define BOOKMGMT_AUDIOBOOK_H

#include "ElectronicResource.h"
#include <string>

namespace bookmgmt {

// Answer to Q3 Justification for AudioBook:
// AudioBook inherits from ElectronicResource because audio books are digital media
// accessed electronically per stream/user seat rather than physical copies.

class AudioBook : public ElectronicResource {
private:
    std::string narrator_;
    int durationMinutes_;

public:
    AudioBook(std::string id,
              std::string title,
              std::string publisher,
              int year,
              Money pricePerSeat,
              std::string accessUrl,
              LicenseModel licenseModel,
              Money platformFee,
              std::string narrator,
              int durationMinutes)
        : ElectronicResource(std::move(id),
                             std::move(title),
                             std::move(publisher),
                             year,
                             pricePerSeat,
                             std::move(accessUrl),
                             licenseModel,
                             platformFee),
          narrator_(std::move(narrator)),
          durationMinutes_(durationMinutes) {}

    ResourceCategory category() const override {
        return ResourceCategory::AudioBook;
    }

    void printDetails(std::ostream& os) const override {
        ElectronicResource::printDetails(os);
        os << "  narrator: " << narrator_ << "\n"
           << "  duration: " << durationMinutes_ << " mins\n";
    }

    const std::string& narrator() const { return narrator_; }
    int durationMinutes() const { return durationMinutes_; }
};

} // namespace bookmgmt

#endif // BOOKMGMT_AUDIOBOOK_H