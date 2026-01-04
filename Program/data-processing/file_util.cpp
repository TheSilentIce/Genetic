#include "file_util.h"
#include "../types.h"
#include "calculations.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

constexpr i16 NUM_TICKETS = 500;

/**
 * Format:
 * Date, Open, High, Low, Close, Volume
 * Line:
 2025-11-28,277.260009765625,279.0,275.989990234375,278.8500061035156,20135600.0

 */

void create_csv(std::vector<std::vector<std::string>> &data) {
  std::ofstream file;
  file.open("new_data.csv");
  std::vector<std::string> names = get_names(data);
  cleaningData(data);

  file << "Ticket,Open,Close,High,Low,RSI,SMA,EMA" << '\n';
  for (i16 i = 0; i < data.size(); ++i) {
    const std::vector<std::string> vec = data.at(i);
    std::vector<float> rsi = initialize_RSI(vec);
    std::vector<float> sma = init_SMA(vec);
    std::vector<float> ema = init_EMA(vec);

    for (i16 j = 14; j < vec.size(); ++j) {
      std::vector<double> line = split_line(vec.at(j));
      std::string new_line;

      new_line += names.at(i) + ',';
      new_line += std::to_string(line.at(OPEN)) + ',';
      new_line += std::to_string(line.at(CLOSE)) + ',';
      new_line += std::to_string(line.at(HIGH)) + ',';
      new_line += std::to_string(line.at(LOW)) + ',';
      new_line += std::to_string(rsi.at(j - 14)) + ',';
      new_line += std::to_string(sma.at(j - 14)) + ',';
      new_line += std::to_string(ema.at(j - 14));
      new_line += '\n';

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
    std::cout << line << '\n';
    if (line != ",,,,,") {
      a.push_back(line);
    } else {
      data.push_back(std::move(a));
      a.clear();
      break;
    }
  }
  data_file.close();
  return data;
}

void cleaningData(std::vector<std::vector<std::string>> &data) {
  for (i16 i = 0; i < data.size(); ++i) {
    data.at(i).erase(data.at(i).begin());
    data.at(i).erase(data.at(i).begin());
  }
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
