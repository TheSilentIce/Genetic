#include "file_util.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

int main() {
  // filepath is dependent on where Makefile is run
  // TODO standardize filepath inputting

  std::filesystem::path filepath =
      std::filesystem::path(PROJECT_ROOT) / "Program" / "data" / "training.csv";

  auto start = std::chrono::system_clock::now();
  auto data = read_stocks(filepath.string());

  std::string training = "new_training.csv";
  create_csv(data, training);
  auto end = std::chrono::system_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();
  std::cout << "Time it took: " << time << '\n';

  // start = std::chrono::system_clock::now();
  // data = read_stocks(filepath.string());
  // std::string training1 = "new_training1.csv";
  // create_csv_faster(data, training);
  // end = std::chrono::system_clock::now();
  // time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
  //            .count();
  // std::cout << "Time it took: " << time << '\n';

  // std::filesystem::path testpath =
  //     std::filesystem::path(PROJECT_ROOT) / "Program" / "data" /
  //     "testing.csv";
  //
  // data = read_stocks(testpath.string());
  // std::string testing = "new_testing.csv";
  // create_csv(data, testing);
  //
  // std::filesystem::path rawPath =
  //     std::filesystem::path(PROJECT_ROOT) / "Program" / "data" /
  //     "om_nom.csv";
  // data = read_stocks(rawPath.string());
  // std::string new_raw = "new_omnom.csv";
  // create_csv(data, new_raw);

  return 0;
}
