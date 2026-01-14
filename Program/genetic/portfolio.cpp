#include "portfolio.h"
#include <vector>

Portfolio::Portfolio(std::vector<float> stock_map, double balance) {
  this->stock_map = stock_map;
  this->balance = balance;
}
const std::vector<float> &Portfolio::get_stock_map() const { return stock_map; }
