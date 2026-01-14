#include <map>
#include <vector>
#ifndef PORTFOLIO_H
#define PORTFOLIO_H

class Portfolio {
private:
  std::vector<float> stock_map;
  double balance;

public:
  Portfolio(std::vector<float> stock_map, double balance);
  const std::vector<float> &get_stock_map() const;
  double &get_porfolio_balance();
};

#endif
