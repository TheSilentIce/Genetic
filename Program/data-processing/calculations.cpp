#include "utilties.h"
#include <iostream>

int main() {
  std::string fp = "data.csv";
  auto i = read_stocks(std::move(fp));
  // print(i);
  initialize_RSI(i.at(0));

  return 0;
}
