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
  
  
  
  
  
  
  
  
