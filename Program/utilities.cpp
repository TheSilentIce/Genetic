#include "iostream"
#include "utilties.h"
#include <fstream>
#include <string>

std::vector<std::vector<int>> readData(std::string filepath) {
  std::vector<std::vector<int>> stocks;

  std::string line;
  std::ifstream data("data.csv");

  // format:
  // NAME HIGH LOW CLOSE OPEN ....
  // AAPL 100 50 75 60 ...
  while (std::getline(data, line)) {
    std::cout << line;
  }

  data.close();
  return stocks;
}
