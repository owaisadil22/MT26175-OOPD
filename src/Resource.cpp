#include "bookmgmt/Resource.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace bookmgmt {

std::string categoryName(ResourceCategory cat) {
    switch (cat) {
        case ResourceCategory::Book: return "Book";
        case ResourceCategory::ElectronicResource: return "ElectronicResource";
        case ResourceCategory::Journal: return "Journal";
        case ResourceCategory::EBook: return "EBook";
        case ResourceCategory::AudioBook: return "AudioBook";
        case ResourceCategory::Thesis: return "Thesis";
    }
    return "Unknown";
}

const std::vector<ResourceCategory> kAllCategories = {
    ResourceCategory::Book,
    ResourceCategory::ElectronicResource,
    ResourceCategory::Journal,
    ResourceCategory::EBook,
    ResourceCategory::AudioBook,
    ResourceCategory::Thesis
};

Resource::Resource(std::string id, std::string title, std::string publisher,
                   int publicationYear, Money baseUnitPrice)
    : id_(std::move(id)), title_(std::move(title)), publisher_(std::move(publisher)),
      publicationYear_(publicationYear), baseUnitPrice_(baseUnitPrice) {
    if (id_.empty()) throw std::invalid_argument("Resource ID cannot be empty");
    if (title_.empty()) throw std::invalid_argument("Resource title cannot be empty");
    if (baseUnitPrice_ < Money::fromMinor(0)) throw std::invalid_argument("Base unit price cannot be negative");
}

Money Resource::costFor(int quantity) const {
    if (quantity <= 0) throw std::invalid_argument("Quantity must be greater than zero");
    return baseUnitPrice_ * quantity;
}

void Resource::addVendorOffer(const std::string& vendorName, Money unitPrice) {
    if (vendorName.empty()) throw std::invalid_argument("Vendor name cannot be empty");
    if (unitPrice < Money::fromMinor(0)) throw std::invalid_argument("Vendor price cannot be negative");
    vendorOffers_[vendorName] = unitPrice;
}

std::optional<VendorOffer> Resource::cheapestVendor() const {
    if (vendorOffers_.empty()) return std::nullopt;

    auto cheapestIt = std::min_element(
        vendorOffers_.begin(), vendorOffers_.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    return VendorOffer{cheapestIt->first, cheapestIt->second};
}

Money Resource::costForVendor(const std::string& vendorName, int quantity) const {
    if (quantity <= 0) throw std::invalid_argument("Quantity must be greater than zero");
    auto it = vendorOffers_.find(vendorName);
    if (it == vendorOffers_.end()) throw std::invalid_argument("Vendor not found: " + vendorName);
    return it->second * quantity;
}

void Resource::printDetails(std::ostream& os) const {
    os << "[" << categoryName(category()) << "] " << id_ << ": " << title_
       << " | Publisher: " << publisher_ << " (" << publicationYear_ << ")"
       << " | Base Price: " << baseUnitPrice_;
    if (!vendorOffers_.empty()) {
        os << " | Vendors: ";
        for (const auto& [vendor, price] : vendorOffers_) {
            os << vendor << " (" << price << ") ";
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Resource& r) {
    r.printDetails(os);
    return os;
}

}  // namespace bookmgmt