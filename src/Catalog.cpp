#include "bookmgmt/Catalog.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

#include "bookmgmt/Book.h"
#include "bookmgmt/EBook.h"
#include "bookmgmt/Exceptions.h"
#include "bookmgmt/Journal.h"

namespace bookmgmt {

namespace {
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}
}  // namespace

Resource& Catalog::add(std::unique_ptr<Resource> r) {
    if (!r) throw std::invalid_argument("Cannot add null resource");
    std::string id = r->id();
    if (contains(id)) {
        throw DuplicateIdError("Resource already exists: " + id);
    }
    Resource& ref = *r;
    items_[id] = Entry{std::move(r), 0};
    return ref;
}

bool Catalog::contains(const std::string& id) const {
    return items_.find(id) != items_.end();
}

Resource* Catalog::find(const std::string& id) {
    auto it = items_.find(id);
    return it == items_.end() ? nullptr : it->second.resource.get();
}

const Resource* Catalog::find(const std::string& id) const {
    auto it = items_.find(id);
    return it == items_.end() ? nullptr : it->second.resource.get();
}

Resource& Catalog::get(const std::string& id) {
    Resource* r = find(id);
    if (!r) throw NotFoundError("Resource not found: " + id);
    return *r;
}

const Resource& Catalog::get(const std::string& id) const {
    const Resource* r = find(id);
    if (!r) throw NotFoundError("Resource not found: " + id);
    return *r;
}

void Catalog::remove(const std::string& id) {
    auto it = items_.find(id);
    if (it == items_.end()) {
        throw NotFoundError("Cannot remove non-existent resource: " + id);
    }
    items_.erase(it);
}

int Catalog::holdings(const std::string& id) const {
    auto it = items_.find(id);
    if (it == items_.end()) throw NotFoundError("Resource not found: " + id);
    return it->second.holdings;
}

void Catalog::addHoldings(const std::string& id, int units) {
    auto it = items_.find(id);
    if (it == items_.end()) throw NotFoundError("Resource not found: " + id);
    if (it->second.holdings + units < 0) {
        throw std::invalid_argument("Holdings cannot drop below zero");
    }
    it->second.holdings += units;
}

std::vector<const Resource*> Catalog::all() const {
    std::vector<const Resource*> result;
    for (const auto& [id, entry] : items_) {
        result.push_back(entry.resource.get());
    }
    return result;
}

std::vector<const Resource*> Catalog::byCategory(ResourceCategory c) const {
    return where([c](const Resource& r) { return r.category() == c; });
}

std::vector<const Resource*> Catalog::searchTitle(const std::string& text) const {
    std::string q = toLower(text);
    return where([&q](const Resource& r) {
        return toLower(r.title()).find(q) != std::string::npos;
    });
}

std::vector<const Resource*> Catalog::where(
    const std::function<bool(const Resource&)>& pred) const {
    std::vector<const Resource*> result;
    for (const auto& [id, entry] : items_) {
        if (pred(*entry.resource)) {
            result.push_back(entry.resource.get());
        }
    }
    return result;
}

// Q13: Search by Author
std::vector<const Resource*> Catalog::searchByAuthor(const std::string& author) const {
    std::string q = toLower(author);
    return where([&q](const Resource& r) {
        if (auto b = dynamic_cast<const Book*>(&r)) {
            for (const auto& a : b->authors()) {
                if (toLower(a).find(q) != std::string::npos) return true;
            }
        }
        if (auto eb = dynamic_cast<const EBook*>(&r)) {
            for (const auto& a : eb->authors()) {
                if (toLower(a).find(q) != std::string::npos) return true;
            }
        }
        return false;
    });
}

// Q13: Search by ISBN or ISSN
std::vector<const Resource*> Catalog::searchByIsbnOrIssn(const std::string& identifier) const {
    std::string id = toLower(identifier);
    return where([&id](const Resource& r) {
        if (auto b = dynamic_cast<const Book*>(&r)) {
            if (toLower(b->isbn()).find(id) != std::string::npos) return true;
        }
        if (auto eb = dynamic_cast<const EBook*>(&r)) {
            if (toLower(eb->isbn()).find(id) != std::string::npos) return true;
        }
        if (auto j = dynamic_cast<const Journal*>(&r)) {
            if (toLower(j->issn()).find(id) != std::string::npos) return true;
        }
        return false;
    });
}

// Q13: Search by Publication Year Range
std::vector<const Resource*> Catalog::searchByYearRange(int startYear, int endYear) const {
    if (startYear > endYear) {
        throw std::invalid_argument("startYear cannot be greater than endYear");
    }
    return where([startYear, endYear](const Resource& r) {
        int year = r.publicationYear();
        return year >= startYear && year <= endYear;
    });
}

}  // namespace bookmgmt