#include <iostream>

#include "atm.hpp"

int main() {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  atm.WithdrawCash(12345678, 1234, 200.40);
  atm.DepositCash(12345678, 1234, 40000);
  atm.DepositCash(12345678, 1234, 32000);
  atm.PrintLedger("example.txt", 12345678, 1234);
}