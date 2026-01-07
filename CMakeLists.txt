set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_COMPILER "gcc")
cmake_minimum_required(VERSION 3.23)

project(Genetic)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(algo Program/genetic/main.cpp)
add_executable(runner Program/data-processing/runner.cpp)

#pass proj root
add_compile_definitions(PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

add_subdirectory(Program/data-processing)
add_subdirectory(Program/genetic)
