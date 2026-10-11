#include "bookmgmt/ElectronicResource.h"

#include <iostream>
#include <stdexcept>

namespace bookmgmt {

std::string modelName(LicenseModel model) {
    switch (model) {
        case LicenseModel::AnnualSubscription: return "Annual Subscription";
        case LicenseModel::PerpetualAccess: return "Perpetual Access";
    }
    return "Unknown";
}

ElectronicResource::ElectronicResource(std::string id, std::string title, std::string publisher,
                                       int publicationYear, Money unitPrice, std::string accessUrl,
                                       LicenseModel model, Money platformFee)
    : Resource(std::move(id), std::move(title), std::move(publisher), publicationYear, unitPrice),
      accessUrl_(std::move(accessUrl)), licenseModel_(model), platformFee_(platformFee) {
    if (accessUrl_.empty()) throw std::invalid_argument("Access URL cannot be empty");
    if (platformFee_ < Money::fromMinor(0)) throw std::invalid_argument("Platform fee cannot be negative");
}

Money ElectronicResource::costFor(int quantity) const {
    if (quantity <= 0) throw std::invalid_argument("Quantity must be greater than zero");
    return platformFee_ + (baseUnitPrice_ * quantity);
}

void ElectronicResource::printDetails(std::ostream& os) const {
    Resource::printDetails(os);
    os << " | Access URL: " << accessUrl_
       << " | License: " << modelName(licenseModel_)
       << " | platform fee: " << platformFee_;
}

}  // namespace bookmgmt