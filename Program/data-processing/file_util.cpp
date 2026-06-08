#include "file_util.h"
#include "../types.h"
#include "SafeQueue.h"
#include "calculations.h"
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <thread>
#include <utility>

Timer::Timer() {}

void Timer::start_time() { start = std::chrono::system_clock::now(); }

void Timer::end_time(std::string s) {
  auto end = std::chrono::system_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start)
                  .count();

  std::cout << "It took " << s << " " << time << " µs\n";
}

SafeQueue<std::vector<std::string>> safe_queue{};

void writeFile(i32 tickets, std::string name) {
  std::filesystem::path fp =
      std::filesystem::path(PROJECT_ROOT) / "Program" / "data" / name;

  std::ofstream file;
  file.open(fp.string());

  i32 i{0};

  file << "Ticket,Open,Close,High,Low,Volume,OBV,RSI,SMA,EMA,%K,%D,CCI,ATR"
       << '\n';
  Timer t = Timer();
  while (i != tickets) {
    std::vector<std::string> updated_ticket;
    safe_queue.pop(updated_ticket);
    for (std::string_view s : updated_ticket) {
      file << s;
    }
    i++;
  }
  file.close();
}

void work(const std::vector<std::vector<std::string>> &data, i32 beg, i32 end,
          std::vector<std::string> &names) {
  for (i32 i{beg}; i < end; ++i) {
    const std::vector<std::string> &ticket = data.at(i);
    const std::vector<PriceRow> rows = break_apart(ticket);

    std::vector<float> close_percentage = prices_to_percentage(rows, CLOSE);
    std::vector<float> high_percentage = prices_to_percentage(rows, HIGH);
    std::vector<float> low_percentage = prices_to_percentage(rows, LOW);
    std::vector<float> open_percentage = prices_to_percentage(rows, OPEN);
    std::vector<float> volume_percentage = prices_to_percentage(rows, VOLUME);

    std::vector<int> obv = init_on_balance_volumes(rows);
    std::vector<float> rsi = initialize_RSI(rows);
    std::vector<float> sma = init_SMA(close_percentage);
    std::vector<float> ema = init_EMA(close_percentage);
    std::vector<float> so_k = init_SO_K(rows);
    std::vector<float> so_d = init_SO_D(so_k);
    std::vector<float> cci = init_commodity_channel_index(rows);
    std::vector<float> atr = init_ATR(rows);

    std::vector<std::string> updated_ticket{};

    std::string new_line;
    i16 SIZE = ticket.size();
    updated_ticket.reserve(SIZE);
    i16 MIN_SIZE = std::min({(i16)close_percentage.size(), (i16)sma.size(),
                             (i16)ema.size(), (i16)rsi.size(), (i16)obv.size(),
                             (i16)cci.size(), (i16)atr.size()});

    for (i16 j = 0; j < MIN_SIZE; ++j) {
      new_line += names.at(i) + ',';
      new_line += std::to_string(open_percentage.at(j)) + ',';
      new_line += std::to_string(close_percentage.at(j)) + ',';
      new_line += std::to_string(high_percentage.at(j)) + ',';
      new_line += std::to_string(low_percentage.at(j)) + ',';
      new_line += std::to_string(volume_percentage.at(j)) + ',';
      new_line += std::to_string(obv.at(j)) + ',';
      new_line += std::to_string(rsi.at(j)) + ',';
      new_line += std::to_string(sma.at(j)) + ',';
      new_line += std::to_string(ema.at(j)) + ',';
      new_line += std::to_string(so_k.at(j)) + ',';
      if (j < (i16)so_d.size()) {
        new_line += std::to_string(so_d.at(j)) + ',';
      } else {
        new_line += std::to_string(so_d.back()) + ',';
      }
      new_line += std::to_string(cci.at(j)) + ',';
      new_line += std::to_string(atr.at(j));
      new_line += '\n';
      updated_ticket.push_back(new_line);
    }
    safe_queue.push(updated_ticket);
  }
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

void create_csv(std::vector<std::vector<std::string>> &data,
                std::string &name) {
  cleaningData(data);

  // one is the spawning thread, and one will be the file writer thread
  i32 NUMBER_OF_WORKER_THREADS = std::thread::hardware_concurrency() - 2;
  std::vector<std::string> names = get_names(data);
  i32 WORK_PER_THREAD = names.size() / NUMBER_OF_WORKER_THREADS;

  std::vector<std::thread> worker_threads;

  i32 beg = 0;
  i32 end = WORK_PER_THREAD;
  for (i32 i{0}; i < NUMBER_OF_WORKER_THREADS; ++i) {
    end = (beg + WORK_PER_THREAD) >= names.size() ? names.size()
                                                  : beg + WORK_PER_THREAD;
    end = (i == NUMBER_OF_WORKER_THREADS - 1) ? names.size() : end;
    worker_threads.emplace_back(
        [&data, &names, beg, end]() { work(data, beg, end, names); });

    beg += WORK_PER_THREAD;
  }
  std::thread writer{writeFile, names.size(), name};
  for (auto &t : worker_threads) {
    t.join();
  }

  writer.join();
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
    vec.erase(vec.begin() + 1);
  }
}

std::vector<std::string>
get_names(const std::vector<std::vector<std::string>> &data) {
  std::vector<std::string> tickets{};

  for (const std::vector<std::string> &v : data) {
    std::string ticket = v.at(0);

    ticket.erase(std::remove(ticket.begin(), ticket.end(), ','), ticket.end());

    tickets.push_back(ticket);
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
