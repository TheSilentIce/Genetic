cmake_minimum_required(VERSION 3.23)

project(Genetic)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

add_executable(run-algorithm Program/genetic/main.cpp)
add_executable(get-data Program/data-processing/runner.cpp)

add_subdirectory(Program/data-processing)
add_subdirectory(Program/genetic)

if(APPLE)
    include(FetchContent)

    FetchContent_Declare(
      metal-cpp
      URL https://developer.apple.com/metal/cpp/files/metal-cpp_macOS15.2_iOS18.2.zip
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(metal-cpp)

    add_library(metal INTERFACE)
    target_include_directories(metal INTERFACE ${metal-cpp_SOURCE_DIR})
    target_link_libraries(metal INTERFACE
        "-framework Metal"
        "-framework Foundation"
        "-framework QuartzCore"
    )

    add_executable(gpu-accelerated Program/genetic/gpu-accelerated/gpu_util.cpp)
    add_subdirectory(Program/genetic/gpu-accelerated)

    add_custom_command(TARGET gpu-accelerated POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/Program/genetic/gpu-accelerated/shader.metallib
        $<TARGET_FILE_DIR:gpu-accelerated>/shader.metallib
    )
endif()
