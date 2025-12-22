#ifndef GENETIC_UTIL_H
#define GENETIC_UTIL_H
#include "portfolio.h"

std::map<std::string, float> simulated_binary_crossover(Portfolio *parent1,
                                                        Portfolio *parent2,
                                                        bool positive);
std::map<std::string, float> blend_crossover(Portfolio *parent1,
                                             Portfolio *parent2);

void mutate(std::map<std::string, float> child);
void normalize(std::map<std::string, float> child);

Portfolio *create_child(std::map<std::string, float> child);

#endif
