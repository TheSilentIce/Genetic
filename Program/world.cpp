#include "world.h"
#include "genetic_util.h"

constexpr short GENERATIONS = 1000;
constexpr long POPULATION = 10'000;

std::vector<Portfolio *> population;

void initialize_population(const std::vector<std::string> &keys) {
  population.reserve(POPULATION);
  for (int i{0}; i < POPULATION; ++i) {
    population.push_back(create_random_portfolio(keys));
  }

  for (Portfolio *p : population) {
    delete p;
  }

  population.clear();
}

void slow_pop(std::vector<std::string> keys) {
  for (int i{0}; i < POPULATION; ++i) {
    population.push_back(slow(keys));
  }

  for (Portfolio *p : population) {
    delete p;

    population.clear();
  }
}

/**
 * Calculate fitness
 * Hall of Fame
 * Add to mating pool
 * crossover
 * mutate
 * replace pop
 */
