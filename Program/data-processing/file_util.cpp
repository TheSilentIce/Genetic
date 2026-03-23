#include "file_util.h"
#include "../types.h"
#include "calculations.h"
#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
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

Timer::Timer() {}

void Timer::start_time() { start = std::chrono::system_clock::now(); }

void Timer::end_time(std::string s) {
  auto end = std::chrono::system_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start)
                  .count();

  std::cout << "It took " << s << " " << time << " µs\n";
}
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
  Timer t = Timer();

  for (i16 i = 0; i < data.size(); ++i) {
    const std::vector<std::string> vec = data.at(i);
    const std::vector<PriceRow> rows = break_apart(vec);

    std::vector<float> close_percentage = prices_to_percentage(rows, CLOSE);
    std::vector<float> high_percentage = prices_to_percentage(rows, HIGH);
    std::vector<float> low_percentage = prices_to_percentage(rows, LOW);
    std::vector<float> open_percentage = prices_to_percentage(rows, OPEN);
    std::vector<float> volume_percentage = prices_to_percentage(rows, VOLUME);

    std::vector<int> obv = init_on_balance_volumes(rows);
    std::vector<float> rsi = initialize_RSI(rows);
    std::vector<float> sma = init_SMA(rows);
    std::vector<float> ema = init_EMA(rows);
    std::vector<float> so_k = init_SO_K(rows);
    std::vector<float> so_d = init_SO_D(so_k);
    std::vector<float> cci = init_commodity_channel_index(rows);

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
  Timer t = Timer();
  for (i16 i = 0; i < data.size(); ++i) {
    const std::vector<std::string> vec = data.at(i);

    const std::vector<PriceRow> rows = break_apart(vec);
    std::vector<float> close_percentage = prices_to_percentage(rows, CLOSE);
    std::vector<float> high_percentage = prices_to_percentage(rows, HIGH);
    std::vector<float> low_percentage = prices_to_percentage(rows, LOW);
    std::vector<float> open_percentage = prices_to_percentage(rows, OPEN);
    std::vector<float> volume_percentage = prices_to_percentage(rows, VOLUME);

    std::vector<int> obv = init_on_balance_volumes(rows);
    std::vector<float> rsi = initialize_RSI(rows);
    std::vector<float> sma = init_SMA(rows);
    std::vector<float> ema = init_EMA(rows);
    std::vector<float> so_k = init_SO_K(rows);
    std::vector<float> so_d = init_SO_D(so_k);
    std::vector<float> cci = init_commodity_channel_index(rows);

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

  if (!data_file.is_open()) {
    std::cerr << "File Not Open: " << filepath << '\n';
    return {};
  }

  std::string line{};
  std::vector<std::vector<std::string>> data{};

  std::vector<std::string> ticket{};
  bool isFirst = true;

  while (std::getline(data_file, line)) {
    if (isFirst) {
      ticket.push_back(line);
      isFirst = false;
      continue;
    }

    if (line == ",,,,,") {
      data.push_back(std::move(ticket));
      isFirst = true;
      ticket.clear();
    } else {
      if (line.find(",,") == std::string::npos) {
        ticket.push_back(line);
      }
    }
  }

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
std::array<double, 5> split_line(const std::string_view &line) {
  std::array<double, 5> info{};

  const char *p = line.data();
  const char *end = p + line.size();

  // skip first column
  while (p < end && *p != ',')
    ++p;
  if (p < end)
    ++p;

  for (int i = 0; i < 5 && p < end; ++i) {
    char *col_end;
    info[i] = std::strtod(p, &col_end);
    p = col_end + 1; // skip comma
  }
  return info;
}
