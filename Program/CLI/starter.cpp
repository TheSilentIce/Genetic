#include "command_functions.h"
#include <cstdlib>
#include <iostream>
#include <string>

enum class Codes { help, quit, build, run, predict, clear, error };

Codes convert(std::string s) {
  if (s == "quit")
    return Codes::quit;
  if (s == "build")
    return Codes::build;
  if (s == "run")
    return Codes::run;
  if (s == "help")
    return Codes::help;
  if (s == "clear")
    return Codes::clear;
  if (s == "predict")
    return Codes::predict;

  return Codes::error;
}

int main() {
  std::cout << "Genetic CLI started\n";
  bool running = true;
  while (running) {
    std::cout << "Enter input: ";
    std::string s{};
    std::cin >> s;

    switch (convert(s)) {
    case Codes::quit:
      running = false;
      std::cout << "Quitting...\n";
      break;
    case Codes::clear:
      std::cout << "\033[2J\033[1;1H";
      break;
    case Codes::help:
      help();
      break;
    case Codes::build:
      build();
      break;
    default:
      std::cout << "command not found\n";
    }
  }
  return 0;
}
