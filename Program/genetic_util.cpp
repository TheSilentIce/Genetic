#include "genetic_util.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

constexpr float ALPHA = 0.5;
constexpr int DISTRiBUTION_INDEX = 10; // lower index equals greater diversity
constexpr float MUTATION_PROBABILITY = 0.05; // low prob
constexpr double POWER = 4.0 / (DISTRiBUTION_INDEX + 1);

constexpr short MAC_CACHE_LINE = 128;

std::default_random_engine &get_engine() {
  static std::default_random_engine eng(std::random_device{}());
  return eng;
}

float random_float() {
  static std::uniform_real_distribution<float> distr(0.0, 1.0);
  return distr(get_engine());
}

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
std::vector<float> simulated_binary_crossover(Portfolio *parent1,
                                              Portfolio *parent2,
                                              bool positive) {
  std::vector<float> new_props{};

  short sign = positive ? 1 : -1;
  float mu = random_float();
  float beta{};

  if (mu <= 0.5) {
    beta = pow(2 * mu, POWER);
  } else {
    beta = pow(1 / (2 * (1 - mu)), POWER);
  }

  const std::vector<float> &p1_props = parent1->get_props();
  const std::vector<float> &p2_props = parent2->get_props();
  new_props.reserve(p1_props.size());

  float sign_part = 1 + sign * beta;

  short size = p1_props.size();
  for (short i = 0; i < size; ++i) {
    float p1_gene = sign_part * p1_props.at(i);
    float p2_gene = sign_part * p2_props.at(i);

    float new_gene = 0.5 * (p1_gene + p2_gene);
    if (new_gene < 0) {
      new_gene = 0;
    }

    new_props.push_back(new_gene);
  }

  normalize(new_props);
  return new_props;
}

// this uses the BLX-Alpha crossover technique
// Essentially, we take the two genes, create a min/max out of them
// and select a random number within that range
std::vector<float> blend_crossover(Portfolio *parent1, Portfolio *parent2) {
  std::vector<float> new_props{};

  const std::vector<float> &p1_props = parent1->get_props();
  const std::vector<float> &p2_props = parent2->get_props();

  new_props.reserve(p1_props.size());

  short size = p1_props.size();
  for (short i = 0; i < size; ++i) {
    float p1 = p1_props.at(i);
    float p2 = p2_props.at(i);

    float bigger = p1 >= p2 ? p1 : p2;
    float smaller = p2 <= p1 ? p2 : p1;

    float min = bigger - ALPHA * (smaller - bigger);
    float max = bigger + ALPHA * (smaller - bigger);

    std::uniform_real_distribution<float> distr1(min, max);
    float new_gene = distr1(get_engine());

    new_props.push_back(new_gene);
  }

  normalize(new_props);
  return new_props;
}

// simple normalization, ensuring all percentages = 1
void normalize(std::unordered_map<std::string, float> child) {
  float sum = 0;
  for (auto iter = child.begin(); iter != child.end(); iter++) {
    sum += iter->second;
  }

  for (auto iter = child.begin(); iter != child.end(); iter++) {
    float new_prop = iter->second / sum;
    child[iter->first] = new_prop;
  }
}

void normalize(std::vector<float> child) {
  float sum = 0;
  for (float f : child) {
    sum += f;
  }

  for (short i = 0; i < child.size(); ++i) {
    float new_prop = child.at(i) / sum;
    child[i] = new_prop;
  }
}

// if a gene is chosen to be mutated, just adding 8% to it; arbritary number,
// change as you want
void mutate(std::vector<float> child) {
  short size = child.size();

  for (short i{0}; i < size; ++i) {
    float chance = random_float();

    if (chance <= MUTATION_PROBABILITY) {
      child[i] = child[i] + 0.08;
    }
  }

  normalize(child);
}

Portfolio create_child(std::unordered_map<std::string, float> child) {
  return Portfolio(child);
}

Portfolio *create_random_portfolio(const std::vector<std::string> &keys) {
  std::unordered_map<std::string, float> unordered_map{};
  unordered_map.reserve(keys.size());

  for (const std::string &key : keys) {
    float prop = random_float();
    unordered_map.emplace(key, prop);
  }
  normalize(unordered_map);
  Portfolio *port = new Portfolio(unordered_map);
  return port;
}

Portfolio *create_vec(const std::vector<std::string> &keys) {
  std::vector<float> props{};
  props.reserve(keys.size());

  for (const std::string &key : keys) {
    float prop = random_float();
    props.push_back(prop);
  }
  normalize(props);
  Portfolio *port = new Portfolio(props);
  return port;
}
