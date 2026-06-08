#ifndef COMMAND_FUNCTIONS_H
#define COMMAND_FUNCTIONS_H

enum class Codes { help, quit, build, run, predict, clear, error };

void help();
void build();
void clear();
void run();

#endif
