#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <string>
#include <vector>

enum LineIndices {
  OPEN = 0,
  HIGH = 1,
  LOW = 2,
  CLOSE = 3,
  VOLUME = 4,
};

void create_csv(std::vector<std::vector<std::string>> &data);
void cleaningData(std::vector<std::vector<std::string>> &data);
std::vector<std::vector<std::string>> read_stocks(const std::string &filepath);
std::vector<double> split_line(std::string line);
std::vector<std::string>
get_names(const std::vector<std::vector<std::string>> &data);
void print(std::vector<std::vector<std::string>> a);
#endif
