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






