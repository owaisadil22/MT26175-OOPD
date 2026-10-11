#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include "bookmgmt/Money.h"
#include "bookmgmt/Resource.h"

namespace bookmgmt {

enum class Binding { Paperback, Hardcover };

std::string bindingName(Binding b);
std::string joinAuthors(const std::vector<std::string>& authors);

class Book : public Resource {
public:
    Book(std::string id, std::string title, std::vector<std::string> authors,
         std::string isbn, std::string publisher, int publicationYear,
         Money unitPrice, int edition = 1, Binding binding = Binding::Paperback);

    ResourceCategory category() const override { return ResourceCategory::Book; }
    bool isDigital() const override { return false; }

    const std::vector<std::string>& authors() const { return authors_; }
    const std::string& isbn() const { return isbn_; }
    int edition() const { return edition_; }
    Binding binding() const { return binding_; }

    Money costFor(int quantity) const override;
    void printDetails(std::ostream& os) const override;

protected:
    std::vector<std::string> authors_;
    std::string isbn_;
    int edition_;
    Binding binding_;
};

}  // namespace bookmgmt