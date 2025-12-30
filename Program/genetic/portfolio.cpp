#include "portfolio.h"

Portfolio::Portfolio(std::vector<float> stock_map) {
  this->stock_map = stock_map;
}
const std::vector<float> &Portfolio::get_stock_map() const { return stock_map; }
