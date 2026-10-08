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
