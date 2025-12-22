#include "world.h"
#include "genetic_util.h"
#include <iostream>
#include <random>

constexpr short GENERATIONS{1000};
constexpr long POPULATION{100'000};
constexpr short CHILDREN{500};
constexpr float TOP_PERCENTAGE{0.1};
constexpr short TOP{CHILDREN * (short)TOP_PERCENTAGE};

std::vector<Portfolio *> population;
std::vector<Portfolio *> mating_pool;

void initialize_population(const std::vector<std::string> &keys) {
  population.reserve(POPULATION);
  for (int i{0}; i < POPULATION; ++i) {
    population.push_back(create_vec(keys));
  }

  for (Portfolio *p : population) {
    delete p;
  }
  population.clear();
}

// void repopulate() {
//   static std::random_device rd;
//   static std::default_random_engine eng(rd());
//   static std::uniform_int_distribution<int> distr(0, population.size());
//
//   for (short i = 0; i < TOP_PERCENTAGE; ++i) {
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
