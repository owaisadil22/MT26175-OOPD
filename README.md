# MT26175 OOPD - Assignment 2: BookManagement

## Question 1: Journal Resource Class

Implemented the `Journal` class extending the `Resource` abstract base class to support serial publications and subscriptions.

### Implementation Details:
- **`include/bookmgmt/Resource.h` & `src/Resource.cpp`**: Added `Journal` to `ResourceCategory` enum and updated `categoryName()` string mapping.
- **`include/bookmgmt/Journal.h`**:
  - Implemented `Journal` derived from `Resource`.
  - Added member attributes: `issn_` (`std::string`), `issuesPerYear_` (`int`), and `subscriptionYears_` (`int`).
  - Overrode virtual member functions: `category()`, `costFor(quantity)`, and `printDetails(os)`.
  - Enforced constructor validation ensuring `subscriptionYears >= 1`.
- **`include/bookmgmt/bookmgmt.h`**: Included `"Journal.h"` main header.
- **Build & Verification**: Configured with CMake and confirmed test suite execution (`64/64 checks passed`).

---

## Terminal Commands Executed for Q1

```bash
# 1. Unzip and enter project
unzip BookManagement.zip
cd BookManagement

# 2. Configure, build, and test C++ code
cmake -S . -B build
cmake --build build
./build/bookmgmt_tests

# 3. Initialize Git repository and set remote URL with Personal Access Token
git init
git branch -M main
git remote set-url origin https://owaisadil22:<YOUR_GITHUB_TOKEN>@[github.com/owaisadil22/MT26175-OOPD.git](https://github.com/owaisadil22/MT26175-OOPD.git)

# 4. Stage, commit, and push to GitHub
git add include/ src/ CMakeLists.txt README.md
git commit -m "Q1: Implemented Journal resource class and updated documentation"
git push -u origin main --force

---

## Question 2: EBook Resource Class

Implemented the `EBook` class derived from `ElectronicResource` representing digital books with license management.

### Implementation Details:
- **`include/bookmgmt/Resource.h` & `src/Resource.cpp`**: Added `EBook` to `ResourceCategory` enum and updated `categoryName()`.
- **`include/bookmgmt/EBook.h`**:
  - Derived `EBook` from `ElectronicResource`.
  - Matched base constructor arguments: `id`, `title`, `publisher`, `year`, `pricePerSeat`, `accessUrl`, `licenseModel`, `platformFee`.
  - Added fields: `authors_`, `isbn_`, `format_`, and `isDrmProtected_`.
  - Overrode `category()` to return `ResourceCategory::EBook`.
  - Overrode `printDetails(os)` to call `ElectronicResource::printDetails(os)` before outputting EBook details.
  - Added inline commentary on code duplication between `Book` and `EBook`.
- **`include/bookmgmt/bookmgmt.h`**: Included `"EBook.h"`.

### Terminal Commands Executed for Q2
```bash
# Recompile and execute unit tests
cmake -S . -B build
cmake --build build
./build/bookmgmt_tests

# Stage, commit, and push Question 2 implementation and updated README
git add include/ src/ README.md
git commit -m "Q2: Implemented EBook resource class and updated README"
git push origin main

---

## Question 3: AudioBook and Thesis Classes

Implemented `AudioBook` and `Thesis` resource classes with base class choices and justifications.

### Implementation Details:
- **include/bookmgmt/Resource.h & src/Resource.cpp**: Added `AudioBook` and `Thesis` to `ResourceCategory` enum and `categoryName()`.
- **include/bookmgmt/AudioBook.h**:
  - Derived from `ElectronicResource` (justified as digital/streaming content accessed per user seat).
  - Added fields: `narrator_` (`std::string`) and `durationMinutes_` (`int`).
- **include/bookmgmt/Thesis.h**:
  - Derived directly from `Resource` (justified as free/open-access academic publications where unitPrice and `costFor()` are 0).
  - Added fields: `university_`, `degree_`, and `supervisor_`.
- **include/bookmgmt/bookmgmt.h**: Included `"AudioBook.h"` and `"Thesis.h"`.

### Terminal Commands Executed for Q3:
- `cmake -S . -B build`: Configured CMake build system.
- `cmake --build build`: Recompiled library and test binaries.
- `./build/bookmgmt_tests`: Ran test suite.
- `git add include/ src/ README.md`: Staged header, source, and documentation files.
- `git commit -m "Q3: Implemented AudioBook and Thesis resource classes and updated README"`: Recorded local commit.
- `git push origin main`: Pushed changes to GitHub repository.

---

## Question 4: Hardcover Books Pricing

Implemented 20% surcharge calculation for Hardcover books in `Book::costFor(quantity)`.

### Implementation Details:
- **include/bookmgmt/Book.h**: Added `Money costFor(int quantity) const override;`.
- **src/Book.cpp**: Implemented `costFor(quantity)` to compute base cost (`unitPrice * quantity`) and apply a 20% surcharge using `minorUnits()` and `Money::fromMinor()` when `binding_ == Binding::Hardcover`.

### Terminal Commands Executed for Q4:
- `cmake -S . -B build`: Configured build system.
- `cmake --build build`: Recompiled static library and executables.
- `./build/bookmgmt_tests`: Verified unit test suite execution.
- `git add include/ src/ README.md`: Staged updated files.
- `git commit -m "Q4: Implemented Hardcover 20% surcharge pricing and updated README"`: Saved local commit.
- `git push origin main`: Uploaded commits to GitHub repository.

---

## Question 5: Bulk Discounts

Implemented bulk discount rules for print items and electronic resources.

### Implementation Details:
- **`src/Book.cpp` & `include/bookmgmt/Journal.h`**:
  - Validated `quantity > 0` (throwing `std::invalid_argument` otherwise).
  - Applied a 10% discount in `costFor(quantity)` when purchasing 10 or more copies.
- **`src/ElectronicResource.cpp`**:
  - Validated `quantity > 0` (throwing `std::invalid_argument` otherwise).
  - Updated `costFor(quantity)` so that every seat beyond the 50th receives a 50% discount (half price).

### Terminal Commands Executed for Q5:
- `cmake -S . -B build`: Configured build system.
- `cmake --build build`: Recompiled static library and executables.
- `./build/bookmgmt_tests`: Verified unit test suite execution.
- `git add include/ src/ README.md`: Staged updated files.
- `git commit -m "Q5: Implemented bulk discounts for print items and electronic resources"`: Saved local commit.
- `git push origin main`: Uploaded commits to GitHub repository.

---

## Question 6: Taxes

Implemented configurable tax rates for print items and electronic resources with post-tax quota validation and pre/post-tax reporting.

### Implementation Details:
- **`include/bookmgmt/Acquisition.h` & `src/Acquisition.cpp`**:
  - Added `setPrintTaxRate()` and `setElectronicTaxRate()` to `AcquisitionManager`.
  - Added `taxFor()`, `preTaxCost()`, and `postTaxCost()` helper methods.
  - Updated `canPurchase()` and `purchase()` to check quotas and budget limits against post-tax costs.
  - Updated `processBatch()` to catch invalid requests gracefully without adding unapproved records on thrown exceptions.
  - Extended `PurchaseRecord` and `printReport()` to record and display pre-tax cost, tax amount, and post-tax cost.

### Terminal Commands Executed for Q6:
- `cmake -S . -B build`: Configured build directory.
- `cmake --build build`: Recompiled library and test binaries.
- `./build/bookmgmt_tests`: Verified unit test suite execution (64/64 passed).
- `git add include/ src/ README.md`: Staged updated files.
- `git commit -m "Q6: Implemented print/electronic tax rates with post-tax quota validation"`: Saved local commit.
- `git push origin main`: Uploaded commit to GitHub repository.

---

## Question 7: Distinct Title Limits per Category

Implemented distinct title limits per category quota to control catalog diversity and prevent over-concentration on single resources.

### Implementation Details:
- **`include/bookmgmt/Budget.h` & `src/Budget.cpp`**:
  - Added `std::optional<int> maxTitles` to `Quota` struct.
  - Added `int titles` tracking to `Usage` struct.
  - Updated `Budget::evaluate()` and `Budget::commit()` to enforce and increment distinct title limits.
  - Added `titlesRemaining()` query method.
- **`include/bookmgmt/Acquisition.h` & `src/Acquisition.cpp`**:
  - Updated `canPurchase()`, `purchase()`, and `processBatch()` to detect new title acquisitions (`catalog_.holdings(id) == 0`).
  - Passed `isNewTitle` flag into `Budget::check()` and `Budget::commit()`.
- **`tests/test_main.cpp`**:
  - Added `testCategoryTitleLimits()` verifying title quotas, re-orders of existing titles, and exception throwing.

### Terminal Commands Executed for Q7:
- `cmake -S . -B build`: Configured build directory.
- `cmake --build build`: Recompiled library and test binaries.
- `./build/bookmgmt_tests`: Verified unit test suite execution (69/69 passed).
- `git add include/ src/ tests/ README.md`: Staged updated files.
- `git commit -m "Q7: Implemented distinct title limits per category quota"`: Saved local commit.
- `git push origin main`: Uploaded commit to GitHub repository.

---

## Question 8: Cancellation

Implemented cancellation of approved orders with full refunds to budget and quota usage, reduced holdings, and history auditing.

### Implementation Details:
- **`include/bookmgmt/Budget.h` & `src/Budget.cpp`**:
  - Added `refund()` method to revert spent amount, units, and title counts.
- **`include/bookmgmt/Acquisition.h` & `src/Acquisition.cpp`**:
  - Added `isCancellation` field to `PurchaseRecord`.
  - Implemented `cancelOrder(std::size_t orderNo)` to reduce catalog holdings, invoke budget refunds, and append a cancellation record to history.
  - Updated `totalSpent()` and `printReport()` to accurately calculate and display net expenditure.
- **`tests/test_main.cpp`**:
  - Added `testCancellation()` verifying holding updates, budget refunds, history records, and exception throwing on invalid cancellations.

### Terminal Commands Executed for Q8:
- `cmake -S . -B build`: Configured build directory.
- `cmake --build build`: Recompiled library and test binaries.
- `./build/bookmgmt_tests`: Verified unit test suite execution.
- `git add include/ src/ tests/ README.md`: Staged updated files.
- `git commit -m "Q8: Implemented order cancellation with budget refund and history tracking"`: Saved local commit.
- `git push origin main`: Uploaded commit to GitHub repository.

---

## Question 8: Cancellation

Implemented order cancellation functionality for approved purchases.

### Implementation Details:
- **`src/Budget.cpp` & `include/bookmgmt/Budget.h`**:
  - Implemented `refund()` method to reduce spent money and decrement category usage counters.
- **`src/Acquisition.cpp` & `include/bookmgmt/Acquisition.h`**:
  - Added `cancelOrder(std::size_t orderNo)` to reduce holdings by the purchased quantity, refund spent budget/quotas, and record a separate cancellation record in history.
  - Updated `totalSpent()` to subtract cancelled amounts from history.
- **`tests/test_main.cpp`**:
  - Added `testCancellation()` test suite to verify refunds, catalog updates, and order history tracking.

### Terminal Commands Executed for Q8:
- `cmake -S . -B build`: Configured build system.
- `cmake --build build`: Recompiled static library and executables.
- `./build/bookmgmt_tests`: Verified unit test suite execution.
- `git add include/ src/ tests/ README.md`: Staged updated files.
- `git commit -m "Q8: Implemented cancellation of approved orders with refunds"`: Saved local commit.
- `git push origin main`: Uploaded commits to GitHub repository.

---

## Question 9: Department Budgets

Implemented department-level spending limits and category quotas. Purchase requests name the department to be charged.

### Implementation Details:
- **`include/bookmgmt/Budget.h` & `src/Budget.cpp`**:
  - Added department budget and quota mapping (`setDepartmentBudget()`, `setDepartmentQuota()`).
  - Added department budget checking, committing, and refunding methods (`checkDepartment()`, `commitDepartment()`, `refundDepartment()`).
- **`include/bookmgmt/Acquisition.h` & `src/Acquisition.cpp`**:
  - Extended `PurchaseRequest` and `PurchaseRecord` with a `department` field.
  - Updated `canPurchase()`, `purchase()`, `processBatch()`, and `cancelOrder()` to handle department-attributed purchases.
- **`tests/test_main.cpp`**:
  - Added `testDepartmentBudgets()` verifying department quota restrictions and global budget updates.

### Terminal Commands Executed for Q9:
- `cmake -S . -B build`: Configured build directory.
- `cmake --build build`: Recompiled library and test binaries.
- `./build/bookmgmt_tests`: Verified unit test suite execution.
- `git add include/ src/ tests/ README.md`: Staged updated files.
- `git commit -m "Q9: Implemented department budgets and quota tracking"`: Saved local commit.
- `git push origin main`: Uploaded commit to GitHub repository.

---

## Question 10: Year-end Rollover

Implemented year-end budget rollover functionality, carrying forward a configurable percentage of unspent funds to next year's budget.

### Implementation Details:
- **`include/bookmgmt/Budget.h` & `src/Budget.cpp`**:
  - Implemented `rollover(Money nextYearBase, double carryoverPercentage)` method.
  - Calculated carryover amount from remaining funds (`unspent * percentage / 100.0`) and added it to the next year's base budget.
  - Carried forward active category quotas and department budgets to the new fiscal year instance.
  - Validated that `carryoverPercentage` is strictly between `0.0` and `100.0`.
- **`tests/test_main.cpp`**:
  - Added `testRollover()` verifying carryover calculation, reset usage counters, preserved quotas, and input bounds checking.

### Terminal Commands Executed for Q10:
- `g++ -std=c++17 -Wall -Wextra -Iinclude src/*.cpp tests/test_main.cpp -o bookmgmt_tests`: Compiled test binary directly using g++.
- `./bookmgmt_tests`: Verified unit test suite execution.
- `git add include/ src/ tests/ README.md`: Staged updated files.
- `git commit -m "Q10: Implemented year-end budget rollover with configurable carryover percentage"`: Saved local commit.
- `git push origin main`: Uploaded commits to GitHub repository.

---

## Question 10: Year-end Rollover

Implemented year-end budget rollover functionality, carrying forward a configurable percentage of unspent funds to next year's budget.

### Implementation Details:
- **`include/bookmgmt/Budget.h` & `src/Budget.cpp`**:
  - Added `rollover(Money nextYearBase, double carryoverPercentage)` method to compute and create next year's budget.
  - Calculated carryover funds from unspent budget (`remaining * percentage / 100.0`) and added them to the base budget.
  - Recursively carried forward active category quotas and department budgets.
  - Added strict bounds validation ensuring `carryoverPercentage` is between `0.0` and `100.0`.
- **`tests/test_main.cpp`**:
  - Added `testRollover()` verifying carryover math, reset spent usage counters, inherited quotas, and invalid percentage handling.

### Terminal Commands Executed for Q10:
- `g++ -std=c++17 -Wall -Wextra -Iinclude src/*.cpp tests/test_main.cpp -o bookmgmt_tests`: Compiled test executable directly.
- `./bookmgmt_tests`: Verified unit test suite execution.
- `git add include/ src/ tests/ README.md`: Staged updated files.
- `git commit -m "Q10: Implemented year-end budget rollover with configurable carryover percentage"`: Saved local commit.
- `git push origin main`: Uploaded commit to GitHub repository.

---

## Question 10: Year-end Rollover

Implemented year-end budget rollover functionality, carrying forward a configurable percentage of unspent funds to next year's budget.

### Implementation Details:
- **`include/bookmgmt/Budget.h` & `src/Budget.cpp`**:
  - Added `rollover(Money nextYearBase, double carryoverPercentage)` method to compute and create next year's budget.
  - Calculated carryover funds from unspent budget (`remaining * percentage / 100.0`) and added them to the base budget.
  - Recursively carried forward active category quotas and department budgets.
  - Added strict bounds validation ensuring `carryoverPercentage` is between `0.0` and `100.0`.
- **`tests/test_main.cpp`**:
  - Added `testRollover()` verifying carryover math, reset spent usage counters, inherited quotas, and invalid percentage handling.

### Terminal Commands Executed for Q10:
- `g++ -std=c++17 -Wall -Wextra -Iinclude src/*.cpp tests/test_main.cpp -o bookmgmt_tests`: Compiled test executable directly.
- `./bookmgmt_tests`: Verified unit test suite execution.
- `git add include/ src/ tests/ README.md`: Staged updated files.
- `git commit -m "Q10: Implemented year-end budget rollover with configurable carryover percentage"`: Saved local commit.
- `git push origin main`: Uploaded commit to GitHub repository.

---

## Question 11: All-or-Nothing Batch Processing

Implemented an optional all-or-nothing policy for batch purchase processing.

### Implementation Details:
- **`include/bookmgmt/Acquisition.h` & `src/Acquisition.cpp`**:
  - Updated `processBatch()` method signature to include a `bool allOrNothing = false` parameter.
  - Implemented pre-validation dry run using a simulated budget when `allOrNothing` is enabled: if any request in the batch would fail (e.g., resource not found, invalid quantity, or exceeding cumulative quotas/budget), the entire batch is rejected with zero holdings added or budget committed.
- **`tests/test_main.cpp`**:
  - Added `testAllOrNothingBatch()` verifying that if a single request in a batch fails, all requests are marked as rejected and no changes persist.

### Terminal Commands Executed for Q11:
- `g++ -std=c++17 -Wall -Wextra -Iinclude src/*.cpp tests/test_main.cpp -o bookmgmt_tests`: Compiled unit test binary.
- `./bookmgmt_tests`: Executed unit test suite and verified 100% check pass rate.
- `git add include/ src/ tests/ README.md`: Staged updated source files and documentation.
- `git commit -m "Q11: Implemented all-or-nothing option for acquisition batch processing"`: Created local commit.
- `git push origin main`: Uploaded commits to GitHub repository.

---

## Question 12: Vendors

Implemented multi-vendor pricing support, enabling resources to record distinct price offers across vendors, automatically selecting the cheapest vendor during acquisitions, and tracking the vendor used in order records.

### Implementation Details:
- **`include/bookmgmt/Resource.h` & `src/Resource.cpp`**:
  - Added `VendorOffer` struct and vendor mapping (`vendorOffers_`) to store vendor unit prices.
  - Added `addVendorOffer()`, `cheapestVendor()`, and `costForVendor()` methods.
  - Updated `printDetails()` to list available vendors and their prices.
- **`include/bookmgmt/Acquisition.h` & `src/Acquisition.cpp`**:
  - Updated `PurchaseRecord` and `PurchaseRequest` to include a `vendor` field.
  - Updated `quote()`, `canPurchase()`, `purchase()`, and `processBatch()` to auto-select the cheapest vendor when none is explicitly specified.
- **`tests/test_main.cpp`**:
  - Added `testVendors()` verifying cheapest-vendor auto-selection and explicit vendor ordering.

### Terminal Commands Executed for Q12:
- `g++ -std=c++17 -Wall -Wextra -Iinclude src/*.cpp tests/test_main.cpp -o bookmgmt_tests`: Compiled test binary.
- `./bookmgmt_tests`: Executed unit test suite and verified 100% pass rate.
- `git add include/ src/ tests/ README.md`: Staged updated files.
- `git commit -m "Q12: Implemented multi-vendor pricing and cheapest vendor auto-selection"`: Saved local commit.
- `git push origin main`: Uploaded changes to GitHub repository.
