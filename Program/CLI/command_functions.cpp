#include "command_functions.h"
#include <cstdlib>
#include <iostream>

void help() {
  std::cout << "help - list out all commands, enter help [command] for info on "
               "command flags\n";
  std::cout << "quit - quit CLI\n";
  std::cout << "build - (re)build project\n";
  std::cout << "run- run program\n";
  std::cout << "predict - generate predictions\n";
  std::cout << "clear - clear screen\n";
}

void build() {
#if __APPLE__
  std::cout << "Apple system\n";
  system("sh rebuild.sh");
  std::cout << "rebuilt build dir\n";

#elif _WIN32
  std::cout << "Windows System\n";
  system("rebuild.bat");
  std::exit(0);
#else
  std::cout << "idk\n";
  std::exit(0);
#endif
}
