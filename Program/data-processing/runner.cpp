#include "calculations.h"
#include "file_util.h"
#include <iostream>
#include <string>

int main() {
  std::string filepath = "./data.csv";
  auto data = read_stocks(filepath);

  create_csv(data);

  return 0;
}
