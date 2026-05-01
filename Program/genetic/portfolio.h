#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <vector>
#include <string>

class Portfolio {
public:
    std::vector<float> stock_map;
    double balance;
    int day;

public:
    Portfolio(std::vector<float> stock_map, double balance, int day);
    const std::vector<float> &get_stock_map() const;
    double get_balance() const { return balance; }
    std::vector<float> get_dollar_values() const;
    std::vector<float> get_share_counts(const std::vector<std::vector<std::string>>& all_stock_data, int day) const;
    std::vector<float> get_percentages(const std::vector<float>& shares, const std::vector<std::vector<std::string>>& all_stock_data, int cur_day) const;
    void mutate(float prob_per_field);
};

#endif