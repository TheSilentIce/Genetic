#include "portfolio.h"
#include "genetic_util.h"
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
Portfolio::Portfolio(std::vector<float> stock_map, double balance)
    : stock_map_(stock_map), balance_(balance) {}

const std::vector<float> &Portfolio::get_stock_map() const {
  return stock_map_;
}
double Portfolio::get_balance() const { return balance_; }
float get_close_price(const std::string &csv_line) {
  std::stringstream ss(csv_line);
  std::string discard;

  for (int i = 0; i < 4; i++) {
    std::getline(ss, discard, ',');
  }
  std::string close_str;
  if (std::getline(ss, close_str, ',')) {
    return std::stof(close_str);
  }

  return 0.0f;
}

std::vector<float> Portfolio::get_dollar_values() const {
  std::vector<float> dollar_values;

  for (int i = 0; i < stock_map_.size(); ++i) {
    double initial_allocation = balance_ * stock_map_[i];

    dollar_values.push_back(initial_allocation);
  }
  return dollar_values;
}
std::vector<float> Portfolio::get_share_counts(
    const std::vector<std::vector<std::string>> &all_stock_data,
    int cur_day) const {
  std::vector<float> shares;
  std::vector<float> current_values = get_dollar_values();

  for (int i = 0; i < stock_map_.size(); ++i) {
    float current_price = get_close_price(all_stock_data[i][cur_day]);
    shares.push_back(current_values[i] / current_price);
  }
  return shares;
}
std::vector<float> Portfolio::get_percentages(
    const std::vector<float> &shares,
    const std::vector<std::vector<std::string>> &all_stock_data,
    int cur_day) const {
  std::vector<float> current_values;
  double total_portfolio_value = 0.0;
  for (int i = 0; i < shares.size(); i++) {
    float current_price = get_close_price(all_stock_data[i][cur_day]);
    float value = shares[i] * current_price;

    current_values.push_back(value);
    total_portfolio_value += value;
  }
  std::vector<float> percentages;
  if (total_portfolio_value > 0) {
    for (float val : current_values) {
      percentages.push_back(val / static_cast<float>(total_portfolio_value));
    }
  } else {
    percentages.assign(shares.size(), 0.0f);
  }
  return percentages;
}
void Portfolio::mutate(float prob_per_field) {
  i16 size = stock_map_.size();

  for (i16 i{0}; i < size; ++i) {
    float chance = random_float();

    if (chance <= prob_per_field) {
      stock_map_[i] = stock_map_[i] + 0.08; //<=== Mutation
    }
  }

  normalize(stock_map_);
}
void Portfolio::advance_time(
    const std::vector<std::vector<std::string>> &all_stock_data, int cur_day) {
  stock_map_ = get_percentages(get_share_counts(all_stock_data, cur_day),
                               all_stock_data, cur_day + 1);
    float sum = 0;
    for (float val : get_dollar_values()) {
      // std::cout << "Balance: " << val << std::endl;
      sum += val;
    }
    // std::cout << "Balance: " << sum << std::endl;
    balance_ = sum;
}
