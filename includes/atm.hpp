#ifndef ATM_HPP
#define ATM_HPP

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

struct Account {
  std::string owner_name;
  double balance = 0;
};

class Atm {
public:
  void RegisterAccount(unsigned int card_num,
                       unsigned int pin,
                       const std::string& owner_name,
                       double balance);
  double CheckBalance(unsigned int card_num, unsigned int pin);
  void WithdrawCash(unsigned int card_num, unsigned int pin, double amount);
  void DepositCash(unsigned int card_num, unsigned int pin, double amount);
  void PrintLedger(const std::string& file_path,
                   unsigned int card_num,
                   unsigned int pin);

  std::map<std::pair<unsigned int, unsigned int>, Account>& GetAccounts();
  std::map<std::pair<unsigned int, unsigned int>, std::vector<std::string>>&
  GetTransactions();

private:
  std::map<std::pair<unsigned int, unsigned int>, Account> stored_accounts_;
  std::map<std::pair<unsigned int, unsigned int>, std::vector<std::string>>
      account_transactions_;
};

#endif  // SOLUTION_HPP
