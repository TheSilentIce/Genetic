#include "portfolio.h"
#include <string>

Portfolio::Portfolio(std::unordered_map<std::string, float> stock_map) {
  this->stock_map = stock_map;
}
const std::unordered_map<std::string, float> &Portfolio::get_stock_map() const {
  return stock_map;
}

void Portfolio::insert(std::string key, float value) {
  stock_map.insert_or_assign(key, value);
}
