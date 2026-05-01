#include "world_two.h"
#include "../types.h"
#include "genetic_util.h"
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <thread>
#include <utility>
const i32 NUM_STOCKS = 10;
const i32 BASE_POPULATION = 10;
const i32 BASE_VALUE = 10;
const i32 PRE_CROSS_MUTATION_CHANCE = 1;
const i32 NUM_CROSSOVERS = 2;
const i32 POST_CROSS_MUTATION_CHANCE = 1;
const std::string FILEPATH = "/test";
std::vector<Portfolio> population;
void seed_population(i32 number_of_tickers) {
    for(i32 i = 0; i < BASE_POPULATION; i++) {
      Portfolio x = Portfolio(create_random_array(NUM_STOCKS), BASE_VALUE, 0);
      population.push_back(x);
    }
}
void grow_generation() {
  std::vector<Portfolio> new_pop {};
  std::vector<std::vector<std::string>> stocks = read_stocks(FILEPATH);
  new_pop.reserve(population.size());
  for(int i = 0; i < population.size(); i++){
    Portfolio cur_port = population[i];
    float x = random_float();
    if(x < (PRE_CROSS_MUTATION_CHANCE / 100.0) ){
      mutate(cur_port);
    }
  }
}
std::vector<std::vector<std::string>> read_stocks(const std::string &filepath) {
  std::ifstream data_file(filepath);

  if (!data_file.is_open()) {
    std::cerr << "File Not Open: " << filepath << '\n';
    return {};
  }

  std::string line{};
  std::vector<std::vector<std::string>> data{};

  std::vector<std::string> ticket{};
  bool isFirst = true;

  while (std::getline(data_file, line)) {
    if (isFirst) {
      ticket.push_back(line);
      isFirst = false;
      continue;
    }

    if (line == ",,,,,") {
      data.push_back(std::move(ticket));
      isFirst = true;
      ticket.clear();
    } else {
      if (line.find(",,") == std::string::npos) {
        ticket.push_back(line);
      }
    }
  }
}