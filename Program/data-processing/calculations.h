#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include "../types.h"
#include <string>
#include <vector>

std::vector<float>
prices_to_percentage(const std::vector<std::string> &stock_data, i8 key);
std::vector<float> initialize_RSI(const std::vector<std::string> &stock_data);
float calculate_RSI(float &avg_gain, float &avg_loss, float curr, float before);
std::vector<float> init_SMA(const std::vector<std::string> &stock_data);
float calculate_SMA(i16 beg, i16 end,
                    const std::vector<std::string> &stock_data);
std::vector<float> init_EMA(const std::vector<std::string> &stock_data);
float calculate_EMA(float ema, float price);
std::vector<float> init_SO_K(const std::vector<std::string> &stock_data);
float calculate_SO_K(const std::vector<std::string> &stock_data, i16 beginning);
std::vector<float> init_SO_D(const std::vector<float> &k_data);
float calculate_SO_D(const std::vector<float> &k_data, i16 start);

std::vector<int>
init_on_balance_volumes(const std::vector<std::string> &stock_data);

int calculate_on_balance_volume(float curr_price, float prev_price, int obv,
                                int volume);

#endif
