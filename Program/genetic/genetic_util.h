#ifndef GENETIC_UTIL_H
#define GENETIC_UTIL_H
#include "../types.h"
#include "portfolio.h"
#include <string>
#include <vector>

std::vector<float> simulated_binary_crossover(Portfolio *parent1,
                                              Portfolio *parent2,
                                              bool positive);
std::vector<float> blend_crossover(Portfolio *parent1, Portfolio *parent2);

void mutate(std::vector<float> &child);
void normalize(std::vector<float> &child);

Portfolio create_child(std::vector<float> &child);
Portfolio create_random_portfolio(const i16 size);
#endif
