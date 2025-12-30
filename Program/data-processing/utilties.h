#ifndef UTILTIES_H
#define UTILTIES_H

#include <string>
#include <vector>
void create_csv(std::vector<std::vector<std::string>> data,
                std::vector<std::string>);
std::vector<std::vector<std::string>> read_stocks(std::string filepath);

void print(std::vector<std::vector<std::string>> a);

std::vector<std::string>
get_names(const std::vector<std::vector<std::string>> &data);

std::vector<float> initialize_RSI(const std::vector<std::string> &stock_data);
float calculate_RSI(float &avg_gain, float &avg_loss, float curr, float before);
std::vector<float>
calculate_MOVING_AVERAGE(const std::vector<std::string> &stock_data);
std::vector<float>
calculate_STOCHASTIC_OSCILLATOR(const std::vector<std::string> &stock_data);

std::vector<double> split_line(std::string line);

#endif
