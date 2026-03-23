#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include "../types.h"
#include <string>
#include <vector>

using PriceRow = std::array<double, 5>;

std::vector<PriceRow> break_apart(const std::vector<std::string> &data);

// std::vector<float>
// prices_to_percentage(const std::vector<PriceRow>&vec, i8 key);

std::vector<float> prices_to_percentage(const std::vector<PriceRow> &vec,
                                        i8 key);

std::vector<float> initialize_RSI(const std::vector<PriceRow> &vec);
float calculate_RSI(float &avg_gain, float &avg_loss, float curr, float before);
std::vector<float> init_SMA(const std::vector<PriceRow> &vec);
float calculate_SMA(i16 beg, i16 end, const std::vector<PriceRow> &vec);
float calculate_SMA(i16 beg, i16 end, const std::vector<PriceRow> &vec,
                    i16 period);
std::vector<float> init_EMA(const std::vector<PriceRow> &vec);
float calculate_EMA(float ema, float price);
std::vector<float> init_SO_K(const std::vector<PriceRow> &vec);
float calculate_SO_K(const std::vector<PriceRow> &vec, i16 beginning);
std::vector<float> init_SO_D(const std::vector<float> &k_data);
float calculate_SO_D(const std::vector<float> &k_data, i16 start);

std::vector<int> init_on_balance_volumes(const std::vector<PriceRow> &vec);

int calculate_on_balance_volume(float curr_price, float prev_price, i16 obv,
                                i16 volume);

float calculate_typical_price(const std::vector<PriceRow> &vec, i16 beginning);
float calculate_mean_absolute_deviation(const std::vector<PriceRow> &vec,
                                        i16 beginning);

std::vector<float>
init_commodity_channel_index(const std::vector<PriceRow> &vec);

#endif
