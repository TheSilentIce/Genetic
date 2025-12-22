#include <map>
#ifndef PORTFOLIO_H
#define PORTFOLIO_H

class Portfolio {
private:
  std::unordered_map<std::string, float> stock_map;
  std::vector<float> props;

public:
  Portfolio(std::unordered_map<std::string, float> stock_map);
  Portfolio(std::vector<float> props);
  const std::unordered_map<std::string, float> &get_stock_map() const;
  const std::vector<float> &get_props() const;
  void insert(std::string key, float value);
};

#endif
