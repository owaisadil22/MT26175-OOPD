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
