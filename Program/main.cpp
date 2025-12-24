#include "genetic_util.h"
#include "portfolio.h"
#include "world.h"
#include <chrono>
#include <iostream>
#include <map>

void print_portfolio(Portfolio *p, std::vector<std::string> tickets) {
  std::vector<float> props = p->get_stock_map();
  short size = tickets.size();

  float sum = 0;

  for (short i{}; i < size; ++i) {
    std::cout << tickets.at(i) << ": " << props.at(i) << "\n";
    sum += props.at(i);
  }

  std::cout << "SUM: " << sum << '\n';
  std::cout << "\n";
}

int main() {
  // std::map<std::string, float> m{};
  // m.insert_or_assign("MSFT", 0.23);
  // m.insert_or_assign("AAPl", 0.67);
  // m.insert_or_assign("AMZN", .10);
  // Portfolio *p1 = new Portfolio(m);
  //
  // std::map<std::string, float> m1{};
  // m1.insert_or_assign("MSFT", 0.1);
  // m1.insert_or_assign("AAPL", 0.4);
  // m1.insert_or_assign("AMZN", 0.5);
  // Portfolio *p2 = new Portfolio(m1);
  //
  // std::map<std::string, float> a = simulated_binary_crossover(p1, p2, true);
  // mutate(a);
  // Portfolio *p3 = create_child(a);
  //
  // std::map<std::string, float> b = blend_crossover(p1, p2);
  // mutate(b);
  // Portfolio *p4 = create_child(b);
  //
  // print_portfolio(p3);
  // print_portfolio(p4);
  //
  // delete p1;
  // delete p2;
  // delete p3;
  // delete p4;
  short num_stocks = 30;

  std::vector<std::string> tickets{};
  tickets.reserve(num_stocks);

  for (int i = 0; i < num_stocks; ++i) {
    tickets.push_back("T" + std::to_string(i));
  }

  auto start = std::chrono::high_resolution_clock::now();
  initialize_population(tickets);
  auto end = std::chrono::high_resolution_clock::now();

  auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();

  std::vector<Portfolio *> props = get_pop();
  Portfolio *p = props.at(0);
  print_portfolio(p, tickets);

  std::cout << elapsed << " milliseconds" << '\n';

  return 0;
}
