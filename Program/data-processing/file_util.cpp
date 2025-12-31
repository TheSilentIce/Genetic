#include "file_util.h"
#include "calculations.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

constexpr short NUM_TICKETS = 500;

/**
 * Format:
 * Date, Open, High, Low, Close, Volume
 * Line:
 2025-11-28,277.260009765625,279.0,275.989990234375,278.8500061035156,20135600.0

 */

void create_csv(const std::vector<std::vector<std::string>> &data) {
  std::ofstream file;
  file.open("new_data.csv");

  std::vector<std::string> names = get_names(data);

  file << "Ticket,Open,Close,High,Low,RSI" << '\n';
  for (short i = 0; i < data.size(); ++i) {
    const std::vector<std::string> vec = data.at(i);
    std::vector<float> rsi = initialize_RSI(vec);

    for (short j = 16; j < vec.size(); ++j) {
      std::vector<double> line = split_line(vec.at(j));
      std::string new_line;

      new_line += names.at(i) + ',';
      new_line += std::to_string(line.at(0)) + ',';
      new_line += std::to_string(line.at(3)) + ',';
      new_line += std::to_string(line.at(1)) + ',';
      new_line += std::to_string(line.at(2)) + ',';
      new_line += std::to_string(rsi.at(j - 16));
      // std::cout << j - 15 << '\n';
      new_line += '\n';

      // std::cout << new_line << '\n';

      file << new_line;
    }
  }

  file.close();
}

std::vector<std::vector<std::string>> read_stocks(const std::string &filepath) {
  std::ifstream data_file(filepath);
  if (!data_file.is_open()) {
    std::cerr << "File not Open" << filepath << '\n';
    return {};
  }

  std::string line;

  std::vector<std::vector<std::string>> data{};

  std::vector<std::string> a{};
  while (std::getline(data_file, line)) {
    if (line != ",,,,,") {
      a.push_back(line);
    } else {
      data.push_back(std::move(a));
      a.clear();
    }
  }
  data_file.close();
  return data;
}

std::vector<std::string>
get_names(const std::vector<std::vector<std::string>> &data) {
  std::vector<std::string> tickets{};
  tickets.reserve(NUM_TICKETS);

  std::string ticket{};

  for (const std::vector<std::string> &v : data) {
    ticket = v.at(0);

    ticket.erase(std::remove(ticket.begin(), ticket.end(), ','), ticket.end());
    tickets.push_back(std::move(ticket));
  }
  return tickets;
}

std::vector<double> split_line(std::string line) {

  std::vector<double> info{};
  info.reserve(5);
  std::stringstream ss(line);

  std::string temp;

  char del = ',';

  bool a = true;
  while (std::getline(ss, temp, del)) {
    if (a) {
      a = false;
      continue;
    }

    info.push_back(std::stod(std::move(temp)));
  }
  return info;
}

void print(const std::vector<std::vector<std::string>> a) {
  for (std::vector<std::string> b : a) {
    for (std::string c : b) {
      std::cout << c << '\n';
    }
    std::cout << '\n';
  }
}
