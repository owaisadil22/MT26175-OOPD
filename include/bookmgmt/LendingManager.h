#pragma once

#include <map>
#include <string>

#include "bookmgmt/Catalog.h"

namespace bookmgmt {

class LendingManager {
public:
    explicit LendingManager(Catalog& catalog) : catalog_(catalog) {}

    // Print Copies Lending
    void checkoutPrintCopy(const std::string& patronId, const std::string& resourceId) {
        const Resource& r = catalog_.get(resourceId);
        if (r.isDigital()) {
            throw std::invalid_argument("Cannot checkout print copy for a digital resource: " + resourceId);
        }

        int totalHoldings = catalog_.holdings(resourceId);
        int currentlyBorrowed = totalBorrowedPrintCopies(resourceId);

        if (currentlyBorrowed >= totalHoldings) {
            throw std::runtime_error("No available print copies to checkout for: " + resourceId);
        }

        printLoans_[resourceId][patronId]++;
    }

    void returnPrintCopy(const std::string& patronId, const std::string& resourceId) {
        auto resIt = printLoans_.find(resourceId);
        if (resIt == printLoans_.end() || resIt->second.find(patronId) == resIt->second.end() || resIt->second[patronId] <= 0) {
            throw std::runtime_error("No active print loan found for patron " + patronId + " on resource " + resourceId);
        }

        resIt->second[patronId]--;
        if (resIt->second[patronId] == 0) {
            resIt->second.erase(patronId);
        }
    }

    int activePrintLoans(const std::string& patronId, const std::string& resourceId) const {
        auto resIt = printLoans_.find(resourceId);
        if (resIt == printLoans_.end()) return 0;
        auto patronIt = resIt->second.find(patronId);
        return patronIt == resIt->second.end() ? 0 : patronIt->second;
    }

    int totalBorrowedPrintCopies(const std::string& resourceId) const {
        auto resIt = printLoans_.find(resourceId);
        if (resIt == printLoans_.end()) return 0;
        int total = 0;
        for (const auto& [patron, count] : resIt->second) {
            total += count;
        }
        return total;
    }

    // Electronic Seats Sessions
    void openElectronicSession(const std::string& patronId, const std::string& resourceId) {
        const Resource& r = catalog_.get(resourceId);
        if (!r.isDigital()) {
            throw std::invalid_argument("Cannot open electronic session for non-digital resource: " + resourceId);
        }

        int totalSeats = catalog_.holdings(resourceId);
        int currentlyActiveSessions = totalActiveElectronicSessions(resourceId);

        if (currentlyActiveSessions >= totalSeats) {
            throw std::runtime_error("No available electronic seats for resource: " + resourceId);
        }

        electronicSessions_[resourceId][patronId]++;
    }

    void closeElectronicSession(const std::string& patronId, const std::string& resourceId) {
        auto resIt = electronicSessions_.find(resourceId);
        if (resIt == electronicSessions_.end() || resIt->second.find(patronId) == resIt->second.end() || resIt->second[patronId] <= 0) {
            throw std::runtime_error("No active electronic session found for patron " + patronId + " on resource " + resourceId);
        }

        resIt->second[patronId]--;
        if (resIt->second[patronId] == 0) {
            resIt->second.erase(patronId);
        }
    }

    int activeElectronicSessions(const std::string& patronId, const std::string& resourceId) const {
        auto resIt = electronicSessions_.find(resourceId);
        if (resIt == electronicSessions_.end()) return 0;
        auto patronIt = resIt->second.find(patronId);
        return patronIt == resIt->second.end() ? 0 : patronIt->second;
    }

    int totalActiveElectronicSessions(const std::string& resourceId) const {
        auto resIt = electronicSessions_.find(resourceId);
        if (resIt == electronicSessions_.end()) return 0;
        int total = 0;
        for (const auto& [patron, count] : resIt->second) {
            total += count;
        }
        return total;
    }

private:
    Catalog& catalog_;
    // resourceId -> (patronId -> borrowed copies count)
    std::map<std::string, std::map<std::string, int>> printLoans_;
    // resourceId -> (patronId -> active electronic seats count)
    std::map<std::string, std::map<std::string, int>> electronicSessions_;
};

}  // namespace bookmgmt