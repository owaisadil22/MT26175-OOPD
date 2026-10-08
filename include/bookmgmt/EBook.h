#ifndef BOOKMGMT_EBOOK_H
#define BOOKMGMT_EBOOK_H

#include "ElectronicResource.h"
#include <vector>
#include <string>

namespace bookmgmt {

// Answer to Q2 Theory Question:
// Code duplicated between Book and EBook:
// Both classes duplicate fields for authors and ISBN (along with their getter methods).
//
// How to avoid duplication:
// Introduce a shared metadata structure or mixin (e.g., BookMetadata) holding 
// 'authors' and 'isbn', or use virtual inheritance / composition to share these fields.

class EBook : public ElectronicResource {
private:
    std::vector<std::string> authors_;
    std::string isbn_;
    std::string format_;
    bool isDrmProtected_;

public:
    EBook(std::string id,
          std::string title,
          std::vector<std::string> authors,
          std::string isbn,
          std::string publisher,
          int year,
          Money pricePerSeat,
          std::string accessUrl,
          LicenseModel licenseModel,
          Money platformFee,
          std::string format,
          bool isDrmProtected)
        : ElectronicResource(std::move(id),
                             std::move(title),
                             std::move(publisher),
                             year,
                             pricePerSeat,
                             std::move(accessUrl),
                             licenseModel,
                             platformFee),
          authors_(std::move(authors)),
          isbn_(std::move(isbn)),
          format_(std::move(format)),
          isDrmProtected_(isDrmProtected) {}

    ResourceCategory category() const override {
        return ResourceCategory::EBook;
    }

    void printDetails(std::ostream& os) const override {
        ElectronicResource::printDetails(os);
        os << "  ISBN: " << isbn_ << "\n"
           << "  format: " << format_ << "\n"
           << "  DRM protected: " << (isDrmProtected_ ? "yes" : "no") << "\n";
    }

    const std::vector<std::string>& authors() const { return authors_; }
    const std::string& isbn() const { return isbn_; }
    const std::string& format() const { return format_; }
    bool isDrmProtected() const { return isDrmProtected_; }
};

} // namespace bookmgmt

#endif // BOOKMGMT_EBOOK_H