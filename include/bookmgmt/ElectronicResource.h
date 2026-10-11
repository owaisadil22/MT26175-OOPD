#pragma once

#include <iosfwd>
#include <string>

#include "bookmgmt/Money.h"
#include "bookmgmt/Resource.h"

namespace bookmgmt {

enum class LicenseModel { AnnualSubscription, PerpetualAccess };

std::string modelName(LicenseModel model);

class ElectronicResource : public Resource {
public:
    ElectronicResource(std::string id, std::string title, std::string publisher,
                       int publicationYear, Money unitPrice, std::string accessUrl,
                       LicenseModel model = LicenseModel::AnnualSubscription,
                       Money platformFee = Money::fromMinor(0));

    ResourceCategory category() const override { return ResourceCategory::ElectronicResource; }
    bool isDigital() const override { return true; }

    const std::string& accessUrl() const { return accessUrl_; }
    LicenseModel licenseModel() const { return licenseModel_; }
    Money platformFee() const { return platformFee_; }

    Money costFor(int quantity) const override;
    void printDetails(std::ostream& os) const override;

protected:
    std::string accessUrl_;
    LicenseModel licenseModel_;
    Money platformFee_;
};

}  // namespace bookmgmt