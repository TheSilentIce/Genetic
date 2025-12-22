#include <map>
#ifndef PORTFOLIO_H
#define PORTFOLIO_H

class Portfolio {
private:
  std::map<std::string, float> stock_map;

public:
  Portfolio(std::map<std::string, float> stock_map);
  const std::map<std::string, float> &get_stock_map() const;
  void insert(std::string key, float value);
};

#endif
