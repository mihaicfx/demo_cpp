cmake_minimum_required(VERSION 3.5)
project (demo_project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable most warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Custom utility functions
include(cmake/conan_config.cmake)
include(cmake/grpc_generate.cmake)


## Generate code based on .proto files
set(GRPC_PROTO_GENS_DIR ${CMAKE_BINARY_DIR}/grpc_generated)
file(MAKE_DIRECTORY ${GRPC_PROTO_GENS_DIR})
set(PROTO_FILE "proto/api.proto")

grpc_generate(${PROTO_FILE} ${GRPC_PROTO_GENS_DIR})

# Add Library target with protobuf sources
add_library(protolib ${GRPC_GENERATED_SOURCES})
target_link_libraries(protolib PUBLIC gRPC::grpc++)
target_include_directories(protolib PUBLIC ${GRPC_PROTO_GENS_DIR})


## Build server lib (needed for tests and benchmarks)
set(SERVER_LIB_SOURCES 
    src/server/dictionary.cpp
    src/server/dictionary.h
)
add_library(serverlib STATIC ${SERVER_LIB_SOURCES})

find_package(robin_hood REQUIRED)
find_package(spdlog REQUIRED)
target_link_libraries(serverlib PUBLIC robin_hood::robin_hood spdlog::spdlog)
target_include_directories(serverlib PRIVATE src/server/extern)

# Build server app
set(SERVER_SOURCES 
    src/server/main.cpp
)
add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE serverlib protolib)


## Build client lib
set(CLIENT_LIB_SOURCES 
    src/client/remotedict.cpp
    src/client/include/remotedict.h
)
add_library(clientlib STATIC ${CLIENT_LIB_SOURCES})

target_link_libraries(clientlib PRIVATE protolib)
target_include_directories(clientlib PUBLIC src/client/include)

# Build client app
set(CLIENT_SOURCES 
    src/client/main.cpp
)
add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client PRIVATE clientlib spdlog::spdlog)


## Testing
enable_testing()

add_executable(unit_tests tests/unit/dictionary_test.cpp)

find_package(GTest REQUIRED)
target_link_libraries(unit_tests PRIVATE serverlib GTest::gtest_main)
target_include_directories(unit_tests PRIVATE src/server)

# Integrate with CMake
include(GoogleTest)
gtest_discover_tests(unit_tests)


## Benchmarking
find_package(benchmark REQUIRED)

add_executable(benchmarks tests/benchmark/dictionary_benchmark.cpp)
target_link_libraries(benchmarks PRIVATE serverlib benchmark::benchmark)
target_include_directories(benchmarks PRIVATE src/server)