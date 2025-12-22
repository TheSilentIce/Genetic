#include "genetic_util.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <random>

#define ALPHA 0.5
#define DISTRiBUTION_INDEX 10     // lower index equals greater diversity
#define MUTATION_PROBABILITY 0.05 // low prob
constexpr double POWER = 4.0 / (DISTRiBUTION_INDEX + 1);

/** SBC simulates a single point crossover in a binary encoded chromsome
It uses a Probability Density Function(PDF) to do this
Algorithm:
Select two parents x1 and x2
Generate a random number mu [0,1)
Calculate BETA
If mu <= 0.5
Beta = (2 * mu) ^ (1 / (distribution index + 1))
otherwise
well, you can read the code lol
This crossover actually generates two offspring
offspring1 = 0.5 * [ (1 + BETA)x1 + (1 - BETA)x2 ];
offspring1 = 0.5 * [ (1 - BETA)x1 + (1 + BETA)x2 ];

that bool parameter is to do this, essentially you call this method twice

Larger distrubution index generates children closer to parents
smaller distribution index generates children farther away from parents;
must be a non-negative integer

The reason why we use this is essentially we get the benefits of a
single point crossover with real values instead of binary ones

**/
std::map<std::string, float> simulated_binary_crossover(Portfolio *parent1,
                                                        Portfolio *parent2,
                                                        bool positive) {
  std::map<std::string, float> new_map{};
  short sign = positive ? 1 : -1;

  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<float> distr(0, 1);

  float mu = distr(eng);
  float beta{};

  std::cout << "MU: " << mu << '\n';

  if (mu <= 0.5) {
    beta = pow(2 * mu, POWER);
  } else {
    beta = pow(1 / (2 * (1 - mu)), POWER);
  }

  std::map<std::string, float> parent1_map = parent1->get_stock_map();
  std::map<std::string, float> parent2_map = parent2->get_stock_map();

  auto p1_iter = parent1_map.begin();
  auto p2_iter = parent2_map.begin();

  while (p1_iter != parent1_map.end()) {
    float p1_gene = (1 + sign * beta) * p1_iter->second;
    float p2_gene = (1 + sign * -1 * beta) * p2_iter->second;

    float new_gene = 0.5 * (p1_gene + p2_gene);
    if (new_gene <= 0) {
      new_gene = 0;
    }
    new_map.insert_or_assign(p1_iter->first, new_gene);

    ++p1_iter;
    ++p2_iter;
  }
  normalize(new_map);

  return new_map;
}

// this uses the BLX-Alpha crossover technique
// Essentially, we take the two genes, create a min/max out of them
// and select a random number within that range
std::map<std::string, float> blend_crossover(Portfolio *parent1,
                                             Portfolio *parent2) {
  std::map<std::string, float> new_map{};

  std::random_device rd;
  std::default_random_engine eng(rd());

  std::map<std::string, float> parent1_map = parent1->get_stock_map();
  std::map<std::string, float> parent2_map = parent2->get_stock_map();

  auto p1_iter = parent1_map.begin();
  auto p2_iter = parent2_map.begin();

  while (p1_iter != parent1_map.end()) {
    float p1 = p1_iter->second;
    float p2 = p2_iter->second;
    float max = p1 >= p2 ? p1 : p2;
    float min = p2 <= p1 ? p2 : p1;

    float range_min = max - ALPHA * (min - max);
    float range_max = max + ALPHA * (min - max);

    std::uniform_real_distribution<float> distr(range_min, range_max);
    float new_gene = distr(eng);
    new_map.insert_or_assign(p1_iter->first, new_gene);

    ++p1_iter;
    ++p2_iter;
  }

  normalize(new_map);
  return new_map;
}

// simple normalization, ensuring all percentages = 1
void normalize(std::map<std::string, float> child) {
  float sum = 0;
  for (auto iter = child.begin(); iter != child.end(); iter++) {
    sum += iter->second;
  }

  for (auto iter = child.begin(); iter != child.end(); iter++) {
    float new_prop = iter->second / sum;
    child.insert_or_assign(iter->first, new_prop);
  }
}

// if a gene is chosen to be mutated, just adding 8% to it; arbritary number,
// change as you want
void mutate(std::map<std::string, float> child) {
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<float> distr(0.0, 1.0);

  for (auto iter = child.begin(); iter != child.end(); ++iter) {
    float chance = distr(eng);
    if (chance <= MUTATION_PROBABILITY) {
      child.insert_or_assign(iter->first, iter->second + 0.08);
    }
  }
  normalize(child);
}

Portfolio *create_child(std::map<std::string, float> child) {
  Portfolio *p_child = new Portfolio(child);
  return p_child;
}
