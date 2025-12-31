#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <string>
#include <vector>

void create_csv(const std::vector<std::vector<std::string>> &data);

std::vector<std::vector<std::string>> read_stocks(const std::string &filepath);

std::vector<double> split_line(std::string line);

std::vector<std::string>
get_names(const std::vector<std::vector<std::string>> &data);

void print(std::vector<std::vector<std::string>> a);
#endif
