#include "portfolio.h"
#include <map>
#include <string>

Portfolio::Portfolio() {};
const std::map<std::string, unsigned short> &Portfolio::get_stock_map() const {
  return stock_map;
}
