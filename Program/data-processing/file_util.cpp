#include "file_util.h"
#include "../types.h"
#include "calculations.h"
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

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

void create_csv_faster(std::vector<std::vector<std::string>> &data,
                       std::string &name) {
  std::filesystem::path fp =
      std::filesystem::path(PROJECT_ROOT) / "Program" / "data" / name;

  FILE *fptr;
  fptr = fopen(fp.c_str(), "w");
  std::vector<std::string> names = get_names(data);
  cleaningData(data);
  const char *header =
      "Ticket,Open,Close,High,Low,Volume,OBV,RSI,SMA,EMA,%K,%D,CCI";

  fwrite(header, strlen(header), 1, fptr);

  for (i16 i = 0; i < data.size(); ++i) {
    const std::vector<std::string> vec = data.at(i);
    if (vec.size() < 30) {
      continue;
    }

    std::vector<float> close_percentage = prices_to_percentage(vec, CLOSE);
    std::vector<float> high_percentage = prices_to_percentage(vec, HIGH);
    std::vector<float> low_percentage = prices_to_percentage(vec, LOW);
    std::vector<float> open_percentage = prices_to_percentage(vec, OPEN);
    std::vector<float> volume_percentage = prices_to_percentage(vec, VOLUME);

    std::vector<int> obv = init_on_balance_volumes(vec);
    std::vector<float> rsi = initialize_RSI(vec);
    std::vector<float> sma = init_SMA(vec);
    std::vector<float> ema = init_EMA(vec);
    std::vector<float> so_k = init_SO_K(vec);
    std::vector<float> so_d = init_SO_D(so_k);
    std::vector<float> cci = init_commodity_channel_index(vec);

    for (i16 j = 14; j < vec.size(); ++j) {
      char line[1024];
      float d;
      if (j - 14 < so_d.size() - 1) {
        d = so_d.at(j - 14);
      } else {
        d = so_d.at(so_d.size() - 1);
      }

      int size = snprintf(
          line, sizeof(line),
          "%.2f,%.2f,%.2f,%.2f,%.2f,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,",
          open_percentage.at(j - 14), close_percentage.at(j - 14),
          high_percentage.at(j - 14), low_percentage.at(j - 14),
          volume_percentage.at(j - 14), obv.at(j - 14), rsi.at(j - 14),
          sma.at(j - 14), ema.at(j - 14), so_k.at(j - 14), d, cci.at(j - 14));

      fwrite(line, size, 1, fptr);
      fwrite("\n", 1, 1, fptr);
    }
  }

  fclose(fptr);
}

void create_csv(std::vector<std::vector<std::string>> &data,
                std::string &name) {
  std::filesystem::path fp =
      std::filesystem::path(PROJECT_ROOT) / "Program" / "data" / name;

  std::ofstream file;
  file.open(fp.string());
  std::vector<std::string> names = get_names(data);
  cleaningData(data);

  file << "Ticket,Open,Close,High,Low,Volume,OBV,RSI,SMA,EMA,%K,%D,CCI" << '\n';
  for (i16 i = 0; i < data.size(); ++i) {
    const std::vector<std::string> vec = data.at(i);

    std::vector<float> close_percentage = prices_to_percentage(vec, CLOSE);
    std::vector<float> high_percentage = prices_to_percentage(vec, HIGH);
    std::vector<float> low_percentage = prices_to_percentage(vec, LOW);
    std::vector<float> open_percentage = prices_to_percentage(vec, OPEN);
    std::vector<float> volume_percentage = prices_to_percentage(vec, VOLUME);

    std::vector<int> obv = init_on_balance_volumes(vec);
    std::vector<float> rsi = initialize_RSI(vec);
    std::vector<float> sma = init_SMA(vec);
    std::vector<float> ema = init_EMA(vec);
    std::vector<float> so_k = init_SO_K(vec);
    std::vector<float> so_d = init_SO_D(so_k);
    std::vector<float> cci = init_commodity_channel_index(vec);

    for (i16 j = 14; j < vec.size(); ++j) {
      std::string new_line;

      new_line += names.at(i) + ',';
      new_line += std::to_string(open_percentage.at(j - 14)) + ',';
      new_line += std::to_string(close_percentage.at(j - 14)) + ',';
      new_line += std::to_string(high_percentage.at(j - 14)) + ',';
      new_line += std::to_string(low_percentage.at(j - 14)) + ',';
      new_line += std::to_string(volume_percentage.at(j - 14)) + ',';
      new_line += std::to_string(obv.at(j - 14)) + ',';
      new_line += std::to_string(rsi.at(j - 14)) + ',';
      new_line += std::to_string(sma.at(j - 14)) + ',';
      new_line += std::to_string(ema.at(j - 14)) + ',';
      new_line += std::to_string(so_k.at(j - 14)) + ',';
      if (j - 14 < so_d.size() - 1) {
        new_line += std::to_string(so_d.at(j - 14)) + ',';
      } else {
        new_line += std::to_string(so_d.at(so_d.size() - 1)) + ',';
      }
      new_line += std::to_string(cci.at(j - 14));
      new_line += '\n';

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

  std::string line;
  std::vector<std::vector<std::string>> data{};
  std::vector<std::string> a{};

  while (std::getline(data_file, line)) {
    std::stringstream ss(line);
    std::string field;
    std::vector<std::string> fields;

    // Split line by commas
    while (std::getline(ss, field, ',')) {
      fields.push_back(field);
    }

    // Check if all fields except the first are empty
    bool is_separator = true;
    for (size_t i = 1; i < fields.size(); ++i) {
      if (!fields[i].empty()) {
        is_separator = false;
        break;
      }
    }

    if (!is_separator) {
      a.push_back(line);
    } else {
      if (!a.empty()) {
        data.push_back(std::move(a));
        a.clear();
      }
    }
  }

  // Push any remaining data
  if (!a.empty()) {
    data.push_back(std::move(a));
  }

  data_file.close();
  return data;
}

/**
 * this just gets rid of the name and
 * yfinance's header:
        Date,Open,High,Low,Close,Volume
 */
// void cleaningData(std::vector<std::vector<std::string>> &data) {
//   for (i16 i = 0; i < data.size(); ++i) {
//     data.at(i).erase(data.at(i).begin());
//     data.at(i).erase(data.at(i).begin());
//   }
// }
void cleaningData(std::vector<std::vector<std::string>> &data) {
  for (auto &vec : data) {
    if (vec.size() >= 2) {
      vec.erase(vec.begin()); // remove name
      vec.erase(vec.begin()); // remove yfinance header
    } else if (vec.size() == 1) {
      vec.erase(vec.begin()); // only name, remove it
    }
    // if vec is empty, do nothing
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
std::array<double, 5> split_line(std::string line) {
  std::array<double, 5> info{};
  std::stringstream ss(line);

  std::string temp;

  char del = ',';
  i16 i = 0;

  bool a = true;
  while (std::getline(ss, temp, del)) {
    if (a) {
      a = false;
      continue;
    }

    // info.push_back(std::stod(std::move(temp)));

    auto aaa = temp;
    try {

      float b = std::stod(std::move(temp));
    } catch (const std::invalid_argument &e) {
      std::cout << e.what() << '\n';
      std::cout << aaa << '\n';
      std::exit(1);
    }

    i += 1;
  }
  return info;
}
