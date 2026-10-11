#pragma once

#include <iosfwd>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "bookmgmt/Money.h"

namespace bookmgmt {

enum class ResourceCategory {
    Book,
    ElectronicResource,
    Journal,
    EBook,
    AudioBook,
    Thesis
};

std::string categoryName(ResourceCategory cat);
extern const std::vector<ResourceCategory> kAllCategories;

// Q12: Vendor Price Offering Structure
struct VendorOffer {
    std::string vendorName;
    Money unitPrice;
};

class Resource {
public:
    Resource(std::string id, std::string title, std::string publisher,
             int publicationYear, Money baseUnitPrice);

    virtual ~Resource() = default;

    const std::string& id() const { return id_; }
    const std::string& title() const { return title_; }
    const std::string& publisher() const { return publisher_; }
    int publicationYear() const { return publicationYear_; }
    
    Money unitPrice() const { return baseUnitPrice_; }
    Money baseUnitPrice() const { return baseUnitPrice_; }

    virtual ResourceCategory category() const = 0;
    virtual bool isDigital() const = 0;
    virtual Money costFor(int quantity) const;
    
    // Q12 Vendor Methods
    void addVendorOffer(const std::string& vendorName, Money unitPrice);
    std::optional<VendorOffer> cheapestVendor() const;
    Money costForVendor(const std::string& vendorName, int quantity) const;

    virtual void printDetails(std::ostream& os) const;

    friend std::ostream& operator<<(std::ostream& os, const Resource& r);

protected:
    std::string id_;
    std::string title_;
    std::string publisher_;
    int publicationYear_;
    Money baseUnitPrice_;

    // Q12: Map of Vendor Name -> Unit Price
    std::map<std::string, Money> vendorOffers_;
};

}  // namespace bookmgmt