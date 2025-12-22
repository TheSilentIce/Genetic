#ifndef GENETIC_UTIL_H
#define GENETIC_UTIL_H
#include "portfolio.h"
#include <vector>

std::unordered_map<std::string, float>
simulated_binary_crossover(Portfolio *parent1, Portfolio *parent2,
                           bool positive);
std::unordered_map<std::string, float> blend_crossover(Portfolio *parent1,
                                                       Portfolio *parent2);

void mutate(std::unordered_map<std::string, float> child);
void normalize(std::unordered_map<std::string, float> child);

Portfolio create_child(std::unordered_map<std::string, float> child);

Portfolio *create_random_portfolio(const std::vector<std::string> &keys);

Portfolio *slow(std::vector<std::string> keys);

#endif
