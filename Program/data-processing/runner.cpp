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

  data = read_stocks(testpath.string());
  std::string testing = "new_testing.csv";
  create_csv(data, testing);

  std::filesystem::path rawPath =
      std::filesystem::path(PROJECT_ROOT) / "Program" / "data" / "om_nom.csv";
  data = read_stocks(rawPath.string());
  std::string new_raw = "new_omnom.csv";
  create_csv(data, new_raw);

  return 0;
}
