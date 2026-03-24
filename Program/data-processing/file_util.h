#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include "../types.h"
#include <array>
#include <chrono>
#include <string>
#include <vector>

enum LineIndices {
  OPEN = 0,
  HIGH = 1,
  LOW = 2,
  CLOSE = 3,
  VOLUME = 4,
};

void work(const std::vector<std::vector<std::string>> &data, i32 beg, i32 end,
          std::vector<std::string> &names);
void writeFile(i32 tickets, std::string name);

class Timer {
  std::chrono::time_point<std::chrono::system_clock> start;

public:
  Timer();
  void start_time();
  void end_time(std::string s);
};

void create_csv(std::vector<std::vector<std::string>> &data, std::string &name);
void cleaningData(std::vector<std::vector<std::string>> &data);

std::vector<std::vector<std::string>> read_stocks(const std::string &filepath);
std::array<double, 5> split_line(const std::string_view &line);
std::vector<std::string>
get_names(const std::vector<std::vector<std::string>> &data);
#endif
