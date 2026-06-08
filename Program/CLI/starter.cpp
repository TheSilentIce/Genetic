#include "command_functions.h"
#include <cstdlib>
#include <iostream>
#include <string>

Codes convert(char *b) {
  std::string s = b;

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
  clear();
  std::cout << "Genetic CLI started\n";
  bool running = true;
  char *buffer = new char[100];
  while (running) {
    std::cout << "Enter input: ";
    std::cin.getline(buffer, 100);
    switch (convert(buffer)) {
    case Codes::quit:
      running = false;
      std::cout << "Quitting...\n";
      break;
    case Codes::clear:
      clear();
      break;
    case Codes::help:
      help();
      break;
    case Codes::build:
      build();
      break;
    case Codes::run:
      run();
      break;
    default:
      std::cout << "Commando not found\n";
    }
  }

  delete[] buffer;
  return 0;
}
