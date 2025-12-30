#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include <string>
#include <vector>

std::vector<float> initialize_RSI(const std::vector<std::string> &stock_data);
float calculate_RSI(float &avg_gain, float &avg_loss, float curr, float before);
std::vector<float>
calculate_MOVING_AVERAGE(const std::vector<std::string> &stock_data);
std::vector<float>
calculate_STOCHASTIC_OSCILLATOR(const std::vector<std::string> &stock_data);

#endif
