#include "calculations.h"
#include "file_util.h"
#include <filesystem>
#include <iostream>
#include <string>

int main() {
  // filepath is dependent on where Makefile is run
  // TODO standardize filepath inputting

  std::filesystem::path filepath =
      std::filesystem::path(PROJECT_ROOT) / "Program" / "data" / "data.csv";

  auto data = read_stocks(filepath.string());

  create_csv(data);

  return 0;
}
