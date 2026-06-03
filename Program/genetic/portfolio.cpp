#include "portfolio.h"
#include "genetic_util.h"
#include <vector>
#include <string>
#include <sstream>

Portfolio::Portfolio(std::vector<float> stock_map, double balance) {
    this->stock_map = stock_map;
    this->balance = balance;
}

const std::vector<float>& Portfolio::get_stock_map() const { return stock_map; }

float get_close_price(const std::string& csv_line) {
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
    
    for (int i = 0; i < stock_map.size(); ++i) {
        double initial_allocation = balance * stock_map[i];
        
        dollar_values.push_back(initial_allocation);
    }
    return dollar_values;
}
std::vector<float> Portfolio::get_share_counts(const std::vector<std::vector<std::string>>& all_stock_data, int cur_day) const {
    std::vector<float> shares;
    std::vector<float> current_values = get_dollar_values();

    for (int i = 0; i < stock_map.size(); ++i) {
        float current_price = get_close_price(all_stock_data[i][cur_day]);
        shares.push_back(current_values[i] / current_price);
    }
    return shares;
}
std::vector<float> Portfolio::get_percentages(const std::vector<float>& shares, const std::vector<std::vector<std::string>>& all_stock_data, int cur_day) const {
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
            percentages.push_back(val / (float)total_portfolio_value);
        }
    } else {
        percentages.assign(shares.size(), 0.0f);
    }
    return percentages;
}
void Portfolio::mutate(float prob_per_field) {
  i16 size = stock_map.size();

  for (i16 i{0}; i < size; ++i) {
    float chance = random_float();

    if (chance <= prob_per_field) {
      stock_map[i] = stock_map[i] + 0.08; //<=== Mutation
    }
  }

  normalize(stock_map);
}
void Portfolio::advance_time(const std::vector<std::vector<std::string>>& all_stock_data, int cur_day) {
  stock_map = get_percentages(get_share_counts(all_stock_data, cur_day), all_stock_data, cur_day + 1);
}