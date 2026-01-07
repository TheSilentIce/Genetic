#include "world.h"
#include "../types.h"
#include "genetic_util.h"
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

// constexpr short GENERATIONS{1000};
constexpr i32 POPULATION{1'000'000};
// constexpr int16_t CHILDREN{500};
// constexpr float TOP_PERCENTAGE{0.1};
// constexpr int16_t TOP{CHILDREN * (short)TOP_PERCENTAGE};

std::vector<Portfolio> population;
std::vector<Portfolio *> mating_pool;

void initialize_population(const std::vector<std::string> &keys) {
  population.reserve(POPULATION);
  for (i32 i{0}; i < POPULATION; ++i) {
    population.push_back(create_random_portfolio(keys));
  }
}

// void repopulate() {
//   static std::random_device rd;
//   static std::default_random_engine eng(rd());
//   static std::uniform_int_distribution<int> distr(0, population.size());
//
//   for (i16 i = 0; i < TOP_PERCENTAGE; ++i) {
//
//   }
// }

/**
 * Calculate fitness
 * Hall of Fame
 * Add to mating pool
 * crossover
 * mutate
 * replace pop
 */
// run_generation
