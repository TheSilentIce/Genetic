#include "file_util.h"
#include "../types.h"
#include "calculations.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <filesystem>

constexpr i16 NUM_TICKETS = 500;

/**
 * this generates the new_csv file for the LSTM to read from
 *The first line added to the file is the column names
 * Essentially we iterate over each stock and over each of their lines
 * the various factors are calculated accordingly and added
 *
 * You may notice that we start reading lines from index 14
 * This is because for a lot of the financial indicators, they require an
 * initial 14 days worth of data
 * Which means, we dont have the respective data for those first 14 days
 * thus, they are skipped
 */
void create_csv(std::vector<std::vector<std::string>> &data) {
  std::filesystem::path fp =
      std::filesystem::path(PROJECT_ROOT) / "Program" / "data" / "new_data.csv";
  std::cout << "csv" << '\n';

  std::ofstream file;
  file.open(fp.string());
  std::vector<std::string> names = get_names(data);
  cleaningData(data);

  file << "Ticket,Open,Close,High,Low,Volume,RSI,SMA,EMA,%K,%D" << '\n';
  for (i16 i = 0; i < data.size(); ++i) {
    const std::vector<std::string> vec = data.at(i);

    std::vector<float> rsi = initialize_RSI(vec);
    std::vector<float> sma = init_SMA(vec);
    std::vector<float> ema = init_EMA(vec);
    std::vector<float> so_k = init_SO_K(vec);
    std::vector<float> so_d = init_SO_D(so_k);

    for (i16 j = 14; j < vec.size(); ++j) {
      std::vector<double> line = split_line(vec.at(j));
      std::string new_line;

      new_line += names.at(i) + ',';
      new_line += std::to_string(line.at(OPEN)) + ',';
      new_line += std::to_string(line.at(CLOSE)) + ',';
      new_line += std::to_string(line.at(HIGH)) + ',';
      new_line += std::to_string(line.at(LOW)) + ',';
      new_line += std::to_string(line.at(VOLUME)) + ',';
      new_line += std::to_string(rsi.at(j - 14)) + ',';
      new_line += std::to_string(sma.at(j - 14)) + ',';
      new_line += std::to_string(ema.at(j - 14)) + ',';
      new_line += std::to_string(so_k.at(j - 14)) + ',';
      if (j - 14 < so_d.size() - 1) {
        new_line += std::to_string(so_d.at(j - 14));
      } else {
        new_line += std::to_string(so_d.at(so_d.size() - 1));
      }
      new_line += '\n';
      std::cout << new_line;

      file << new_line;
    }
  }

  file.close();
}

/**
 *It reads the stocks from data.csv
 * exciting
 */
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
    }
  }
  data_file.close();
  return data;
}

/**
 * this just gets rid of the name and
 * yfinance's header:
        Date,Open,High,Low,Close,Volume
 */
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

/**
 * Splits each day's data into respective information represented as a float
 * the indicies to access said data can be found in
 * file_util.h
 */
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
