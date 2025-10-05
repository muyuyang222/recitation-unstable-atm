#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif

#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "atm.hpp"
#include "catch.hpp"


// --------------------- Helpers ---------------------
static bool ReadAll(const std::string& path, std::string& out) {
  std::ifstream fin(path);
  if (!fin) return false;
  out.assign((std::istreambuf_iterator<char>(fin)),
             std::istreambuf_iterator<char>());
  return true;
}

// --------------------- Canonical test data ---------------------
static const unsigned kCard = 12345678;
static const unsigned kPin = 1234;

// --------------------- Tests ---------------------

TEST_CASE("RegisterAccount: create and duplicate") {
  Atm atm;
  atm.RegisterAccount(kCard, kPin, "Sam Sepiol", 300.30);

  auto& accounts = atm.GetAccounts();
  auto key = std::make_pair(kCard, kPin);

  REQUIRE(accounts.contains(key));
  REQUIRE(accounts.size() == 1);

  const Account& acc = accounts.at(key);
  REQUIRE(acc.owner_name == "Sam Sepiol");
  REQUIRE(acc.balance == Approx(300.30));

  auto& txs = atm.GetTransactions();
  REQUIRE(txs.contains(key));
  REQUIRE(txs.at(key).empty());

  REQUIRE_THROWS_AS(atm.RegisterAccount(kCard, kPin, "Someone Else", 10.0),
                    std::invalid_argument);
}

TEST_CASE(
    "DepositCash: positive increases balance and records one transaction") {
  Atm atm;
  atm.RegisterAccount(kCard, kPin, "Alice", 100.00);
  double before = atm.CheckBalance(kCard, kPin);

  atm.DepositCash(kCard, kPin, 200.25);
  REQUIRE(atm.CheckBalance(kCard, kPin) == Approx(before + 200.25));

  auto& v = atm.GetTransactions().at({kCard, kPin});
  REQUIRE_FALSE(v.empty());

  REQUIRE(v.back().find("Deposit") != std::string::npos);
  REQUIRE(v.back().find("200.25") != std::string::npos);
}

TEST_CASE("DepositCash: negative amount throws invalid_argument") {
  Atm atm;
  atm.RegisterAccount(kCard, kPin, "Bob", 0.0);
  REQUIRE_THROWS_AS(atm.DepositCash(kCard, kPin, -0.01), std::invalid_argument);
}

TEST_CASE("WithdrawCash: normal flow decreases balance and records") {
  Atm atm;
  atm.RegisterAccount(kCard, kPin, "Carol", 500.00);
  double before = atm.CheckBalance(kCard, kPin);

  atm.WithdrawCash(kCard, kPin, 100.10);
  REQUIRE(atm.CheckBalance(kCard, kPin) == Approx(before - 100.10));

  auto& v = atm.GetTransactions().at({kCard, kPin});
  REQUIRE_FALSE(v.empty());
  REQUIRE(v.back().find("Withdrawal") != std::string::npos);
  REQUIRE(v.back().find("100.10") != std::string::npos);
}

TEST_CASE("WithdrawCash: negative amount throws invalid_argument") {
  Atm atm;
  atm.RegisterAccount(kCard, kPin, "Dave", 50.0);
  REQUIRE_THROWS_AS(atm.WithdrawCash(kCard, kPin, -1.0), std::invalid_argument);
}

TEST_CASE("WithdrawCash: overdraft throws runtime_error") {
  Atm atm;
  atm.RegisterAccount(kCard, kPin, "Eve", 10.0);
  REQUIRE_THROWS_AS(atm.WithdrawCash(kCard, kPin, 10.01), std::runtime_error);
}

TEST_CASE("Nonexistent account: all public APIs throw invalid_argument") {
  Atm atm;
  REQUIRE_THROWS_AS(atm.CheckBalance(1, 1), std::invalid_argument);
  REQUIRE_THROWS_AS(atm.DepositCash(1, 1, 1.0), std::invalid_argument);
  REQUIRE_THROWS_AS(atm.WithdrawCash(1, 1, 1.0), std::invalid_argument);
  REQUIRE_THROWS_AS(atm.PrintLedger("nope.txt", 1, 1), std::invalid_argument);
}

TEST_CASE("PrintLedger: header and transactions formatting & order") {
  Atm atm;
  atm.RegisterAccount(kCard, kPin, "Sam Sepiol", 300.30);

  atm.WithdrawCash(kCard, kPin, 200.40);   // -> 99.90
  atm.DepositCash(kCard, kPin, 40000.00);  // -> 40099.90
  atm.DepositCash(kCard, kPin, 32000.00);  // -> 72099.90

  const std::string out = "ledger_out.txt";
  atm.PrintLedger(out, kCard, kPin);

  std::string text;
  REQUIRE(ReadAll(out, text));
  REQUIRE(text.find("Name: Sam Sepiol") != std::string::npos);
  REQUIRE(text.find("Card Number: 12345678") != std::string::npos);
  REQUIRE(text.find("PIN: 1234") != std::string::npos);
  REQUIRE(text.find("Withdrawal - Amount: $200.40, Updated Balance: $99.90") !=
          std::string::npos);
  REQUIRE(
      text.find("Deposit - Amount: $40000.00, Updated Balance: $40099.90") !=
      std::string::npos);
  REQUIRE(
      text.find("Deposit - Amount: $32000.00, Updated Balance: $72099.90") !=
      std::string::npos);
  auto i1 = text.find("Withdrawal - Amount: $200.40");
  auto i2 = text.find("Deposit - Amount: $40000.00");
  auto i3 = text.find("Deposit - Amount: $32000.00");
  REQUIRE(i1 != std::string::npos);
  REQUIRE(i2 != std::string::npos);
  REQUIRE(i3 != std::string::npos);
  REQUIRE(i1 < i2);
  REQUIRE(i2 < i3);
}
