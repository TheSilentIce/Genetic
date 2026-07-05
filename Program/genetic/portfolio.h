#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <string>
#include <vector>

class Portfolio {
public:
  std::vector<float> stock_map_;
  double balance_;
  int day_;

public:
  Portfolio() {}
  Portfolio(std::vector<float> stock_map, double balance);

  Portfolio(std::vector<float> stock_map, double balance, int day);
  const std::vector<float> &get_stock_map() const;
  double get_balance() const;
  std::vector<float> get_dollar_values() const;
  std::vector<float>
  get_share_counts(const std::vector<std::vector<std::string>> &all_stock_data,
                   int day) const;
  std::vector<float>
  get_percentages(const std::vector<float> &shares,
                  const std::vector<std::vector<std::string>> &all_stock_data,
                  int cur_day) const;
  void mutate(float prob_per_field);
  void advance_time(const std::vector<std::vector<std::string>> &all_stock_data,
                    int cur_day);
  float get_balance_by_day (
    const std::vector<std::vector<std::string>> &all_stock_data, std::vector<float> shares,
    int cur_day) const;
  void print_portfolio() const;
};

#endif
