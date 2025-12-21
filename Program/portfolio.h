#include <map>
#ifndef PORTFOLIO_H
#define PORTFOLIO_H

class Portfolio {
private:
  std::map<std::string, unsigned short> stock_map;

public:
  Portfolio();
  const std::map<std::string, unsigned short> &get_stock_map() const;
};

#endif
