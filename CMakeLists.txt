cmake_minimum_required(VERSION 3.23)

project(Genetic)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(PROJECT_ROOT=\"${CMAKE_SOURCE_DIR}\")

set(EXECUTABLE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${EXECUTABLE_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${EXECUTABLE_OUTPUT_DIR}")

add_executable(run-algorithm Program/genetic/main.cpp)
add_executable(get-data Program/data-processing/runner.cpp)
add_executable(genetic-cli Program/CLI/starter.cpp)

add_subdirectory(Program/data-processing)
add_subdirectory(Program/genetic)
add_subdirectory(Program/CLI)

set_target_properties(genetic-cli PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_SOURCE_DIR}"
)

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
