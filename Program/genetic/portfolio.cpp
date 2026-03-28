#include "portfolio.h"
#include <vector>
#include <string>
#include <sstream>

Portfolio::Portfolio(std::vector<float> stock_map, double balance, int day) {
    this->stock_map = stock_map;
    this->balance = balance;
    this->day = day;
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
    
    for (size_t i = 0; i < stock_map.size(); ++i) {
        double initial_allocation = balance * stock_map[i];
        
        dollar_values.push_back(initial_allocation);
    }
    return dollar_values;
}
std::vector<float> Portfolio::get_share_counts(const std::vector<std::vector<std::string>>& all_stock_data, int cur_day) const {
    std::vector<float> shares;
    std::vector<float> current_values = get_dollar_values();

    for (size_t i = 0; i < stock_map.size(); ++i) {
        float current_price = get_close_price(all_stock_data[i][cur_day]);
        shares.push_back(current_values[i] / current_price);
    }
    return shares;
}