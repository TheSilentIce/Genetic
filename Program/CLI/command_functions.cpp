#include "command_functions.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/_types/_pid_t.h>
#include <unistd.h>

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
  std::string pid = std::to_string(getpid());
  std::cout << "PID: " << pid << '\n';
  std::cout << "Apple system\n";
  std::string s{"./rebuild.sh " + pid};

  system(s.c_str());

#elif _WIN32
  std::cout << "Windows System\n";
  system("rebuild.bat");
  std::exit(0);
#else
  std::cout << "idk\n";
  std::exit(0);
#endif
}
