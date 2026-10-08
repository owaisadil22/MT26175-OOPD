#include "bookmgmt/ElectronicResource.h"

#include <ostream>
#include <stdexcept>

namespace bookmgmt {

const char* licenseName(LicenseModel m) {
    switch (m) {
        case LicenseModel::Perpetual: return "Perpetual";
        case LicenseModel::AnnualSubscription: return "Annual subscription";
    }
    return "Unknown";
}

ElectronicResource::ElectronicResource(std::string id, std::string title,
                                       std::string publisher, int year,
                                       Money pricePerSeat, std::string accessUrl,
                                       LicenseModel license, Money platformFee)
    : Resource(std::move(id), std::move(title), std::move(publisher), year, pricePerSeat),
      accessUrl_(std::move(accessUrl)),
      license_(license),
      platformFee_(platformFee) {
    if (platformFee_.isNegative())
        throw std::invalid_argument("platform fee must not be negative");
}

Money ElectronicResource::costFor(int quantity) const {
    if (quantity <= 0) {
        throw std::invalid_argument("quantity must be > 0");
    }

    std::int64_t seatPriceMinor = unitPrice().minorUnits();
    std::int64_t totalSeatMinor = 0;

    if (quantity <= 50) {
        totalSeatMinor = seatPriceMinor * quantity;
    } else {
        // First 50 seats at full price, remaining seats at half price (50% off)
        std::int64_t first50 = seatPriceMinor * 50;
        std::int64_t extraSeats = quantity - 50;
        std::int64_t halfPriceSeat = seatPriceMinor / 2;
        totalSeatMinor = first50 + (extraSeats * halfPriceSeat);
    }

    return platformFee_ + Money::fromMinor(totalSeatMinor);
}

void ElectronicResource::printDetails(std::ostream& os) const {
    os << "  access url: " << accessUrl_ << "\n"
       << "  license: " << licenseName(license_) << "\n"
       << "  platform fee: " << platformFee_ << "\n";
}

}  // namespace bookmgmt