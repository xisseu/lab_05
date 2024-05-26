## Homework

1. Создайте модульные тесты на классы `Transaction` и `Account` и создайте файл `CMakeLists.txt` для библиотеки *banking* и для запуска тестов.

 - `test_account.cpp`
 
 ```sh
 #include <gtest/gtest.h>
#include "gmock/gmock.h"

#include "Account.h"

class MockAccount: public Account {
public:
  MockAccount(int id, int balance): Account(id, balance){}
};

TEST(Account, Init) {
  MockAccount ac(1, 1000);
  EXPECT_EQ(ac.Account::GetBalance(), 1000);
  EXPECT_EQ(ac.Account::id(), 1);
}

TEST(Account, ChangeBalance) {
  MockAccount ac(1, 1000);
  EXPECT_THROW(ac.Account::ChangeBalance(10), std::runtime_error);
  ac.Account::Lock();
  ac.Account::ChangeBalance(10);
  EXPECT_EQ(ac.Account::GetBalance(), 1010);
  ac.Account::ChangeBalance(-20);
  EXPECT_EQ(ac.Account::GetBalance(), 990);
  ac.Account::Unlock();
  EXPECT_THROW(ac.Account::ChangeBalance(20), std::runtime_error);
}

TEST(Account, Lock) {
  MockAccount ac(1, 1000);
  ac.Account::Unlock();
  ac.Account::Lock();
  EXPECT_THROW(ac.Account::Lock(), std::runtime_error);
}
 ```
 - `test_transaction.cpp`
 
 ```sh
 #include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Account.h"
#include "Transaction.h"

class MockAccount: public Account {
public:
  MockAccount(int id, int balance): Account(id, balance){}
};

class MockTransaction : public Transaction {
public:
  MockTransaction() : Transaction(){}
};

TEST(Transaction, Init) {
  MockTransaction tr;
  EXPECT_EQ(tr.Transaction::fee(), 1);
}

TEST(Transaction, Make) {
  MockTransaction tr;
  Account ac_from(1, 1000);
  Account ac_to(2, 1000);
  
  tr.Transaction::set_fee(10);
  EXPECT_EQ(tr.Transaction::fee(), 10);
  
  tr.Transaction::set_fee(1);
  
  EXPECT_THROW(tr.Transaction::Make(ac_from, ac_from, 150), std::logic_error);
  EXPECT_THROW(tr.Transaction::Make(ac_from, ac_to, -50), std::invalid_argument);
  EXPECT_THROW(tr.Transaction::Make(ac_from, ac_to, 50), std::logic_error);
  
  tr.Transaction::set_fee(60);
  EXPECT_EQ(tr.Transaction::Make(ac_from, ac_to, 118), false);
  tr.Transaction::set_fee(1);
  ac_from.Account::Lock();
  EXPECT_THROW(tr.Transaction::Make(ac_from, ac_to, 150), std::runtime_error);
  ac_from.Account::ChangeBalance(-900);
  ac_from.Account::Unlock();
  EXPECT_EQ(tr.Transaction::Make(ac_from, ac_to, 150), false);
  EXPECT_EQ(ac_from.Account::GetBalance(), 100);
  EXPECT_EQ(ac_to.Account::GetBalance(), 1000);
  ac_from.Account::Lock();
  ac_from.Account::ChangeBalance(900);
  ac_from.Account::Unlock();
  EXPECT_EQ(tr.Transaction::Make(ac_from, ac_to, 150), true);
  EXPECT_EQ(ac_from.Account::GetBalance(), 1000 - tr.Transaction::fee() - 150);
  EXPECT_EQ(ac_to.Account::GetBalance(), 1150);
}
 ```
 - `CMakeLists.txt`
 
 ```sh
cmake_minimum_required(VERSION 3.4)
project(Test_banking)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
  add_compile_options(--coverage)
endif()

add_library(banking STATIC ${CMAKE_CURRENT_SOURCE_DIR}/banking/Transaction.cpp ${CMAKE_CURRENT_SOURCE_DIR}/banking/Account.cpp)
target_include_directories(banking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/banking)
target_link_libraries(banking gcov)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(third-party/gtest)
  file(GLOB BANKING_TEST_SOURCES tests/*.cpp)
  add_executable(check ${BANKING_TEST_SOURCES})
  target_link_libraries(check banking gtest_main gmock_main)
  add_test(NAME check COMMAND check)
endif()
 ```


2. Настройте сборочную процедуру на **Github Actions**.

- `.github/workflows/CI.yml`

```sh
$ mkdir .github && cd .github
$ mkdir workflows && cd workflows
$ touch CI.yml
```

- Файл `CI.yml`:

```sh
name: CMake

on:
 push:
  branches: [main]
 pull_request:
  branches: [main]

jobs: 
 build_Linux:

  runs-on: ubuntu-latest

  steps:
  - uses: actions/checkout@v3

  - name: Adding gtest
    run: git clone https://github.com/google/googletest.git third-party/gtest -b release-1.11.0

  - name: Install lcov
    run: sudo apt-get install -y lcov

  - name: Config banking with tests
    run: cmake -H. -B ${{github.workspace}}/build -DBUILD_TESTS=ON

  - name: Build banking
    run: cmake --build ${{github.workspace}}/build

  - name: Run tests
    run: build/check
    
  - name: Do lcov stuff
    run: lcov --directory . --capture --output-file coverage.info
    
  - name: Lcov remove
    run: lcov --remove coverage.info '/usr/*' --output-file coverage.info
    
  - name: Lcov remove 2
    run: lcov --remove coverage.info '${{github.workspace}}/third-party/gtest/*' --output-file coverage.info
    
  - name: Lcov list
    run: lcov --list coverage.info

  - name: Coveralls
    uses: coverallsapp/github-action@master
    with:
      github-token: ${{ secrets.GITHUB_TOKEN }}
      path-to-lcov: ${{ github.workspace }}/coverage.info
```

3. Настройте [Coveralls.io](https://coveralls.io/).



## Links

- [C++ CI: Travis, CMake, GTest, Coveralls & Appveyor](http://david-grs.github.io/cpp-clang-travis-cmake-gtest-coveralls-appveyor/)
- [Boost.Tests](http://www.boost.org/doc/libs/1_63_0/libs/test/doc/html/)
- [Catch](https://github.com/catchorg/Catch2)

```
Copyright (c) 2015-2021 The ISC Authors
```
