#include <map>
#include <vector>
#ifndef PORTFOLIO_H
#define PORTFOLIO_H

class Portfolio {
private:
  std::vector<float> stock_map;
  std::vector<float> numbers_map;
  double balance;

public:
  Portfolio(std::vector<float> stock_map, double balance);
  const std::vector<float> &get_stock_map() const;
  double &get_porfolio_balance();
  std::vector<float> &get_dollar_values(int days_past_start);
  std::vector<float> &get_dollar_values(std::vector<float> values);
  std::vector<float> &get_stock_numbers();
};

#endif
