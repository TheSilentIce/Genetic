#include "../types.h"
#include "genetic_util.h"
#include "portfolio.h"
#include "world.h"
#include <chrono>
#include <iostream>
#include <map>

void print_portfolio(Portfolio *p, std::vector<std::string> tickets) {
  std::vector<float> props = p->get_stock_map();
  i16 size = tickets.size();

  float sum = 0;

  for (i16 i{}; i < size; ++i) {
    std::cout << tickets.at(i) << ": " << props.at(i) << "\n";
    sum += props.at(i);
  }

  std::cout << "SUM: " << sum << '\n';
  std::cout << "\n";
}

int main() {
  i16 num_stocks = 30;

  std::vector<std::string> tickets{};
  tickets.reserve(num_stocks);

  for (i32 i = 0; i < num_stocks; ++i) {
    tickets.push_back("T" + std::to_string(i));
  }

  auto start = std::chrono::high_resolution_clock::now();
  initialize_population(tickets);
  auto end = std::chrono::high_resolution_clock::now();

  auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();

  std::vector<Portfolio *> props = get_pop();
  // Portfolio *p = props.at(0);
  // print_portfolio(p, tickets);

  std::cout << elapsed << " milliseconds" << '\n';

  return 0;
}
