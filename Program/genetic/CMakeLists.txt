add_library(gen
genetic_util.cpp
genetic_util.h
portfolio.cpp
portfolio.h
world.cpp
world_two.cpp
world.h
world_two.cpp
world_two.h
)

target_link_libraries(run-algorithm gen)
target_link_libraries(world_two gen)