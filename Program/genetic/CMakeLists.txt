include(FetchContent)

add_library(gen 
	genetic_util.cpp
	genetic_util.h
	portfolio.cpp
	portfolio.h
	world.cpp
	world.h
)

FetchContent_Declare(
  metal-cpp
  URL https://developer.apple.com/metal/cpp/files/metal-cpp_macOS15.2_iOS18.2.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(metal-cpp)

add_library(metal INTERFACE)

target_include_directories(metal INTERFACE
	${metal-cpp_SOURCE_DIR})

target_link_libraries(metal INTERFACE
	"-framework Metal"
	"-framework Foundation"
	"-framework QuartzCore")

target_link_libraries(gen PRIVATE metal)
target_link_libraries(algo gen metal)
