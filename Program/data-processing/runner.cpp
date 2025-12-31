#include "calculations.h"
#include "file_util.h"
#include <iostream>
#include <string>

int main() {
  // filepath is dependent on where Makefile is run
  // TODO standardize filepath inputting
  std::string filepath = "./data/data.csv";
  auto data = read_stocks(filepath);

  create_csv(data);

  return 0;
}
