#pragma once
// Money: fixed-point currency amount stored in minor units (e.g. paise/cents)
// along with ISO currency code support (Q15).

#include <cstdint>
#include <iosfwd>
#include <stdexcept>
#include <string>

namespace bookmgmt {

class Money {
public:
    Money() = default;

    // Construct from minor units with currency: Money::fromMinor(12550, "USD") == 125.50 USD
    static Money fromMinor(std::int64_t minor, std::string currency = "USD") {
        return Money(minor, std::move(currency));
    }

    // Construct from major + minor parts with currency: Money::of(125, 50, "USD") == 125.50 USD
    static Money of(std::int64_t major, int minor = 0, std::string currency = "USD");

    std::int64_t minorUnits() const { return minor_; }
    const std::string& currency() const { return currency_; }

    double toDouble() const { return static_cast<double>(minor_) / 100.0; }
    std::string toString() const;  // "1234.50" / "-3.05"

    bool isZero() const { return minor_ == 0; }
    bool isNegative() const { return minor_ < 0; }

    Money& operator+=(Money o) {
        checkSameCurrency(o);
        minor_ += o.minor_;
        return *this;
    }

    Money& operator-=(Money o) {
        checkSameCurrency(o);
        minor_ -= o.minor_;
        return *this;
    }

    Money& operator*=(std::int64_t k) {
        minor_ *= k;
        return *this;
    }

    friend Money operator+(Money a, Money b) { return a += b; }
    friend Money operator-(Money a, Money b) { return a -= b; }
    friend Money operator*(Money a, std::int64_t k) { return a *= k; }
    friend Money operator*(std::int64_t k, Money a) { return a *= k; }

    friend bool operator==(Money a, Money b) {
        a.checkSameCurrency(b);
        return a.minor_ == b.minor_;
    }

    friend bool operator!=(Money a, Money b) {
        a.checkSameCurrency(b);
        return a.minor_ != b.minor_;
    }

    friend bool operator<(Money a, Money b) {
        a.checkSameCurrency(b);
        return a.minor_ < b.minor_;
    }

    friend bool operator<=(Money a, Money b) {
        a.checkSameCurrency(b);
        return a.minor_ <= b.minor_;
    }

    friend bool operator>(Money a, Money b) {
        a.checkSameCurrency(b);
        return a.minor_ > b.minor_;
    }

    friend bool operator>=(Money a, Money b) {
        a.checkSameCurrency(b);
        return a.minor_ >= b.minor_;
    }

private:
    explicit Money(std::int64_t minor, std::string currency = "USD")
        : minor_(minor), currency_(std::move(currency)) {
        if (currency_.empty()) {
            throw std::invalid_argument("Currency code cannot be empty");
        }
    }

    void checkSameCurrency(const Money& o) const {
        if (currency_ != o.currency_) {
            throw std::invalid_argument("Currency mismatch: " + currency_ + " vs " + o.currency_);
        }
    }

    std::int64_t minor_ = 0;
    std::string currency_ = "USD";
};

std::ostream& operator<<(std::ostream& os, Money m);

}  // namespace bookmgmt