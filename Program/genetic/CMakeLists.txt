include(FetchContent)

add_library(gen 
	genetic_util.cpp
	genetic_util.h
	portfolio.cpp
	portfolio.h
	world.cpp
	world.h
)


target_link_libraries(gen PRIVATE metal)
target_link_libraries(algo gen metal)
