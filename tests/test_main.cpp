#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "bookmgmt/bookmgmt.h"

using namespace bookmgmt;

static int g_failures = 0;
static int g_checks = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        ++g_checks;                                                              \
        if (!(cond)) {                                                           \
            ++g_failures;                                                        \
            std::cerr << __FILE__ << ":" << __LINE__ << ": CHECK failed: " #cond \
                      << "\n";                                                   \
        }                                                                        \
    } while (0)

#define CHECK_THROWS(expr, ExType)            \
    do {                                      \
        bool thrown_ = false;                 \
        try {                                 \
            (void)(expr);                     \
        } catch (const ExType&) {             \
            thrown_ = true;                   \
        } catch (...) {                       \
        }                                     \
        CHECK(thrown_ && "expected " #ExType); \
    } while (0)

static void testMoney() {
    CHECK(Money::of(12, 5).toString() == "12.05");
    CHECK(Money::of(-3, 50).toString() == "-3.50");
    CHECK(Money::fromMinor(7).toString() == "0.07");
    CHECK(Money::of(10) + Money::of(0, 50) == Money::fromMinor(1050));
    CHECK(Money::of(3) * 4 == Money::of(12));
    CHECK(Money::of(1) < Money::of(2));
    CHECK_THROWS(Money::of(1, 100), std::invalid_argument);
}

static void testResourcesAndCost() {
    Book b("B1", "T", {"A", "B", "C"}, "isbn", "P", 2020, Money::of(100));
    CHECK(b.category() == ResourceCategory::Book);
    CHECK(!b.isDigital());
    CHECK(b.costFor(3) == Money::of(300));
    CHECK_THROWS(b.costFor(0), std::invalid_argument);
    CHECK(joinAuthors(b.authors()) == "A, B and C");

    ElectronicResource e("R1", "DB", "P", 2026, Money::of(10), "url",
                         LicenseModel::AnnualSubscription, Money::of(100));
    CHECK(e.isDigital());
    CHECK(e.costFor(5) == Money::of(150));

    CHECK(e.category() == ResourceCategory::ElectronicResource);

    const Resource& r = e;
    CHECK(r.costFor(1) == Money::of(110));
    std::ostringstream os;
    os << r;
    CHECK(os.str().find("platform fee: 100.00") != std::string::npos);

    CHECK_THROWS(Book("", "T", {}, "", "", 2000, Money::of(1)), std::invalid_argument);
    CHECK_THROWS(Book("B", "T", {}, "", "", 2000, Money::fromMinor(-1)),
                 std::invalid_argument);
}

static void testCatalog() {
    Catalog c;
    c.emplace<Book>("B1", "Clean Code", std::vector<std::string>{"M"}, "i", "P", 2008,
                    Money::of(1));
    c.emplace<Book>("B2", "Clean Architecture", std::vector<std::string>{"M"}, "i", "P",
                    2017, Money::of(1));
    c.emplace<ElectronicResource>("R1", "ACM Digital Library", "ACM", 2026, Money::of(1),
                                  "url");

    CHECK(c.size() == 3);
    CHECK(c.contains("B1"));
    CHECK(c.find("nope") == nullptr);
    CHECK_THROWS(c.get("nope"), NotFoundError);
    CHECK_THROWS(c.emplace<Book>("B1", "dup", std::vector<std::string>{}, "", "", 1,
                                 Money::of(1)),
                 DuplicateIdError);

    CHECK(c.searchTitle("clean").size() == 2);
    CHECK(c.byCategory(ResourceCategory::ElectronicResource).size() == 1);
    CHECK(c.where([](const Resource& r) { return r.isDigital(); }).size() == 1);

    CHECK(c.holdings("B1") == 0);
    c.addHoldings("B1", 3);
    CHECK(c.holdings("B1") == 3);
    CHECK_THROWS(c.addHoldings("B1", -5), std::invalid_argument);

    c.remove("R1");
    CHECK(c.size() == 2);
    CHECK_THROWS(c.remove("R1"), NotFoundError);
}

static void testBudget() {
    Budget b(Money::of(1000));
    b.setQuota(ResourceCategory::Book, {5, Money::of(400)});

    CHECK(b.check(ResourceCategory::Book, 2, Money::of(200)).empty());
    CHECK(!b.check(ResourceCategory::Book, 6, Money::of(10)).empty());
    CHECK(!b.check(ResourceCategory::Book, 1, Money::of(401)).empty());
    CHECK(!b.check(ResourceCategory::ElectronicResource, 1, Money::of(1001)).empty());
    CHECK(b.check(ResourceCategory::ElectronicResource, 1, Money::of(900)).empty());

    b.commit(ResourceCategory::Book, 4, Money::of(300));
    CHECK(b.spent() == Money::of(300));
    CHECK(*b.unitsRemaining(ResourceCategory::Book) == 1);
    CHECK(*b.spendRemaining(ResourceCategory::Book) == Money::of(100));
    CHECK(!b.unitsRemaining(ResourceCategory::ElectronicResource).has_value());

    CHECK_THROWS(b.commit(ResourceCategory::Book, 2, Money::of(10)), QuotaExceededError);
    CHECK_THROWS(b.commit(ResourceCategory::ElectronicResource, 1, Money::of(800)),
                 BudgetExceededError);
    CHECK_THROWS(b.commit(ResourceCategory::ElectronicResource, 0, Money::of(1)),
                 std::invalid_argument);
    CHECK(b.spent() == Money::of(300));
}

static void testAcquisition() {
    Catalog c;
    c.emplace<Book>("B1", "Book", std::vector<std::string>{"A"}, "i", "P", 2020,
                    Money::of(100));
    c.emplace<ElectronicResource>("R1", "DB", "P", 2026, Money::of(10), "url",
                                  LicenseModel::AnnualSubscription, Money::of(50));
    Budget b(Money::of(500));
    b.setQuota(ResourceCategory::Book, {3, Money::of(1000)});
    AcquisitionManager acq(c, b);

    CHECK(acq.quote("R1", 5) == Money::of(100));
    std::string why;
    CHECK(acq.canPurchase("B1", 3, &why) && why.empty());
    CHECK(!acq.canPurchase("B1", 4, &why) && !why.empty());
    CHECK(!acq.canPurchase("nope", 1, &why));

    const auto& rec = acq.purchase("B1", 2);
    CHECK(rec.approved && rec.cost == Money::of(200) && rec.orderNo == 1);
    CHECK(c.holdings("B1") == 2);

    CHECK_THROWS(acq.purchase("B1", 2), QuotaExceededError);
    CHECK_THROWS(acq.purchase("nope", 1), NotFoundError);
    CHECK(acq.history().size() == 1);

    auto res = acq.processBatch({{"R1", 10}, {"R1", 100}, {"B1", 1}, {"zzz", 1}, {"B1", 0}});
    CHECK(res.size() == 5);
    CHECK(res[0].approved && res[0].cost == Money::of(150));
    CHECK(!res[1].approved);
    CHECK(res[2].approved);
    CHECK(!res[3].approved && res[3].reason.find("not found") != std::string::npos);
    CHECK(!res[4].approved);
    CHECK(acq.totalSpent() == Money::of(450));
    CHECK(b.spent() == acq.totalSpent());
    CHECK(c.holdings("R1") == 10 && c.holdings("B1") == 3);
    CHECK(acq.history().size() == 6);
}

static void testCategoryTitleLimits() {
    Catalog c;
    c.emplace<Book>("B1", "Book One", std::vector<std::string>{"Author A"}, "111", "Publisher", 2020, Money::of(100));
    c.emplace<Book>("B2", "Book Two", std::vector<std::string>{"Author B"}, "222", "Publisher", 2021, Money::of(100));

    Budget b(Money::of(1000));
    b.setQuota(ResourceCategory::Book, {10, Money::of(500), 1});
    AcquisitionManager acq(c, b);

    CHECK(acq.canPurchase("B1", 1));
    acq.purchase("B1", 1);
    CHECK(acq.canPurchase("B1", 1));

    std::string reason;
    CHECK(!acq.canPurchase("B2", 1, &reason));
    CHECK(reason.find("title") != std::string::npos);
    CHECK_THROWS(acq.purchase("B2", 1), QuotaExceededError);
}

static void testCancellation() {
    Catalog c;
    c.emplace<Book>("B1", "Book One", std::vector<std::string>{"Author A"}, "111", "Publisher", 2020, Money::of(100));

    Budget b(Money::of(1000));
    b.setQuota(ResourceCategory::Book, {5, Money::of(500)});
    AcquisitionManager acq(c, b);

    const auto& order = acq.purchase("B1", 2);
    std::size_t orderNo = order.orderNo;
    CHECK(c.holdings("B1") == 2);
    CHECK(b.spent() == Money::of(200));

    const auto& cancelRec = acq.cancelOrder(orderNo);
    CHECK(cancelRec.isCancellation);
    CHECK(c.holdings("B1") == 0);
    CHECK(b.spent() == Money::of(0));
    CHECK(acq.totalSpent() == Money::of(0));
    CHECK(acq.history().size() == 2);

    CHECK_THROWS(acq.cancelOrder(999), NotFoundError);
    CHECK_THROWS(acq.cancelOrder(cancelRec.orderNo), std::invalid_argument);
}

static void testDepartmentBudgets() {
    Catalog c;
    c.emplace<Book>("B1", "CS Book", std::vector<std::string>{"Author CS"}, "111", "Publisher", 2020, Money::of(100));

    Budget overallBudget(Money::of(2000));
    overallBudget.setDepartmentBudget("CS", Money::of(500));
    overallBudget.setDepartmentQuota("CS", ResourceCategory::Book, {3, Money::of(300)});

    AcquisitionManager acq(c, overallBudget);

    CHECK(acq.canPurchase("B1", 2, nullptr, "CS"));
    const auto& rec = acq.purchase("B1", 2, "CS");
    CHECK(rec.department == "CS");
    CHECK(overallBudget.spent() == Money::of(200));

    std::string reason;
    CHECK(!acq.canPurchase("B1", 4, &reason, "CS"));
    CHECK(reason.find("CS") != std::string::npos || reason.find("Quota") != std::string::npos);
}

int main() {
    testMoney();
    testResourcesAndCost();
    testCatalog();
    testBudget();
    testAcquisition();
    testCategoryTitleLimits();
    testCancellation();
    testDepartmentBudgets();
    std::cout << (g_checks - g_failures) << "/" << g_checks << " checks passed\n";
    return g_failures == 0 ? 0 : 1;
}