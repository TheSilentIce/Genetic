add_library(data-pro 
	calculations.cpp
	calculations.h
	file_util.cpp
	file_util.h
)

target_link_libraries(runner data-pro)
