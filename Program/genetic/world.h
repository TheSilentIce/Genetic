#ifndef WORLD_H
#define WORLD_H
#include "portfolio.h"
#include <vector>

Portfolio *revolve();
void run_generation();
void repopulate();
void initialize_population(const std::vector<std::string> &keys);
void slow_pop(const std::vector<std::string> &keys);
std::vector<Portfolio *> get_pop();

#endif
