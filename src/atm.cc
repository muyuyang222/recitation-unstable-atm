#include "atm.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>

// DO NOT CHANGE, this function is for you to use in your test cases
double Atm::CheckBalance(unsigned int card_num, unsigned int pin) {
  if (!stored_accounts_.contains({card_num, pin}))
    throw std::invalid_argument("Invalid Card Number/PIN");
  return stored_accounts_[{card_num, pin}].balance;
}

// DO NOT CHANGE, this function is for you to use in your test cases
std::map<std::pair<unsigned int, unsigned int>, Account>& Atm::GetAccounts() {
  return stored_accounts_;
}

// DO NOT CHANGE, this function is for you to use in your test cases
std::map<std::pair<unsigned int, unsigned int>, std::vector<std::string>>&
Atm::GetTransactions() {
  return account_transactions_;
}

// DO NOT CHANGE, used to create formatted transactions for testing
std::string CreateTransactionRecord(const std::string& transaction,
                                    double amount,
                                    double balance) {
  std::stringstream record;
  record << transaction << " - Amount: $" << std::fixed << std::setprecision(2)
         << amount << ", Updated Balance: $" << std::fixed
         << std::setprecision(2) << balance;
  return record.str();
}

/////////////////////////////////////////
// Debug the following functions:
/////////////////////////////////////////
void Atm::RegisterAccount(unsigned int card_num,
                          unsigned int pin,
                          const std::string& owner_name,
                          double balance) {
  Account new_account = {owner_name, balance};
  stored_accounts_[{card_num, pin}] = new_account;
  account_transactions_[{card_num, pin}] = {};
}

void Atm::WithdrawCash(unsigned int card_num, unsigned int pin, double amount) {
  if (!stored_accounts_.contains({card_num, pin})) {
    throw std::invalid_argument("Invalid Card Number/PIN");
  }
  Account& account = stored_accounts_[{card_num, pin}];
  if (account.balance - amount >= 0) {
    account.balance -= amount;
    account_transactions_[{card_num, pin}].push_back(
        CreateTransactionRecord("Withdrawal", amount, account.balance));
  } else {
    throw std::runtime_error("Transaction declined: insufficient balance");
  }
}

void Atm::DepositCash(unsigned int card_num, unsigned int pin, double amount) {
  if (!stored_accounts_.contains({card_num, pin})) {
    throw std::invalid_argument("Invalid Card Number/PIN");
  }
  if (amount < 0) {
    throw std::invalid_argument("Cannot deposit a negative amount");
  }
  Account account = stored_accounts_[{card_num, pin}];
  account.balance += amount;
  account_transactions_[{card_num, pin}].push_back(
      CreateTransactionRecord("Deposit", amount, account.balance));
}

void Atm::PrintLedger(const std::string& file_path,
                      unsigned int card_num,
                      unsigned int pin) {
  std::ofstream ofs{file_path};
  const Account& account = stored_accounts_[{card_num, pin}];
  const std::vector<std::string>& transactions =
      account_transactions_[{card_num, pin}];
  ofs << "Name: " << account.owner_name << "\nCard Number: " << card_num
      << "\nPIN: " << pin << "\n----------------------------\n";
  for (unsigned int i = 0; i < transactions.size(); i++) {
    ofs << transactions[i];
    if (i != transactions.size() - 1) {
      ofs << "\n";
    }
  }
}
