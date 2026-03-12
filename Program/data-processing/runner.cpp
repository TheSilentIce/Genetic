#include "calculations.h"
#include "file_util.h"
#include <filesystem>
#include <iostream>
#include <string>

int main() {
  // filepath is dependent on where Makefile is run
  // TODO standardize filepath inputting

  std::filesystem::path filepath =
      std::filesystem::path(PROJECT_ROOT) / "Program" / "data" / "training.csv";

  auto data = read_stocks(filepath.string());
  std::string training = "new_training.csv";
  create_csv(data, training);

  std::filesystem::path testpath =
      std::filesystem::path(PROJECT_ROOT) / "Program" / "data" / "testing.csv";

  auto data1 = read_stocks(testpath.string());
  std::string testing = "new_testing.csv";
  create_csv(data1, testing);

  return 0;
}
