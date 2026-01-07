cmake_minimum_required(VERSION 3.23)
project(Genetic)

add_executable(algo Program/genetic/main.cpp)
add_executable(runner Program/data-processing/runner.cpp)

add_subdirectory(Program/data-processing)
add_subdirectory(Program/genetic)
