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
// Format right now is to mutate, crossover, remutate, advance time, and then cull. TODO figure out whether to cull first or advance time first
const i32 NUM_STOCKS = 10;
const i32 BASE_POPULATION = 10;
const i32 BASE_VALUE = 10;
const float PRE_CROSS_MUTATION_CHANCE = 0.01;
const float CROSSOVER_SKIP_CHANCE = 0.5;
const float POST_CROSS_MUTATION_CHANCE = 0.01;
const std::string FILEPATH = "/test";
const std::vector<std::vector<std::string>> PRICE_MAP = read_stocks(FILEPATH);
std::vector<Portfolio> population;
i32 day = 0;
void main() {
  seed_population(BASE_POPULATION);
  while(day < PRICE_MAP.size()) {
    mutate_pop(PRE_CROSS_MUTATION_CHANCE);
    add_crossover();
    mutate_pop(POST_CROSS_MUTATION_CHANCE);
    advance_time();
    cull_population();
  }
}
void seed_population(i32 number_of_tickers) {
    for(i32 i = 0; i < BASE_POPULATION; i++) {
      Portfolio x = Portfolio(create_random_array(NUM_STOCKS), BASE_VALUE);
      population.push_back(x);
    }
}
void mutate_pop(float chance) {
  for (i32 i{0}; i < population.size(); i++) {
    float x = random_float();
    while (x < chance) {
      x = random_float();
      mutate(population[i].stock_map);
    }
  }
}
void add_crossover() {
  // Double for loop ensures that the thing is symmetrical
  for (i32 i = 0; i < population.size(); i++) {
    float xi = random_float();
    if(xi < CROSSOVER_SKIP_CHANCE) {
      continue;
    }
    for (i32 j = 0; j < population.size(); j++) {
      float xj = random_float();
      if(xi < CROSSOVER_SKIP_CHANCE) {
        continue;
      }
      float balance = (population[i].balance + population[j].balance) / 2;
      Portfolio x = Portfolio(blend_crossover(&population[i], &population[j]), balance);
    }
  }
}
void cull_population() {
  mergeSort(population, 0, population.size() - 1);
  std::vector<Portfolio> population2;
  if (population.size() < BASE_POPULATION) {
    return;
  }
  for( i32 i = population.size() - BASE_POPULATION; i < population.size(); i++) {
    population2.push_back(population[i]);
    // Kill original population 1
    population = population2;
  }
}
void advance_time() {
  day = day + 1; //TODO add day overflow protection
  for (i32 i = 0; i < population.size(); i++) {
    population[i].advance_time(PRICE_MAP, day);
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
// Mergesort
void merge(std::vector<Portfolio>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    std::vector<Portfolio> L(arr.begin() + left, arr.begin() + mid + 1);
    std::vector<Portfolio> R(arr.begin() + mid + 1, arr.begin() + right + 1);

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].balance < R[j].balance) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(std::vector<Portfolio>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;


        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}