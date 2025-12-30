#include <map>
#ifndef PORTFOLIO_H
#define PORTFOLIO_H

class Portfolio {
private:
  std::vector<float> stock_map;

public:
  Portfolio(std::vector<float> stock_map);
  const std::vector<float> &get_stock_map() const;
};

#endif
