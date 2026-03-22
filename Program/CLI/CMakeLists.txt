add_library(cli-lib
  command_functions.cpp
  command_functions.h
)
target_link_libraries(genetic-cli cli-lib)
