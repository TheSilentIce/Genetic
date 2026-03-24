#include "file_util.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

int main() {
  std::filesystem::path filepath =
      std::filesystem::path(PROJECT_ROOT) / "Program" / "data" / "training.csv";

  auto start = std::chrono::system_clock::now();
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

  auto end = std::chrono::system_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();
  std::cout << "Time it took: " << time << '\n';

  return 0;
}
