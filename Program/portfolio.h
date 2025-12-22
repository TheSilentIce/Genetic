#include <map>
#ifndef PORTFOLIO_H
#define PORTFOLIO_H

class Portfolio {
private:
  std::unordered_map<std::string, float> stock_map;

public:
  Portfolio(std::unordered_map<std::string, float> stock_map);
  const std::unordered_map<std::string, float> &get_stock_map() const;
  void insert(std::string key, float value);
};

#endif
