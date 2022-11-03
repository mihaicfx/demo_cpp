cmake_minimum_required(VERSION 3.5)
project (demo_project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)


include(cmake/conan_config.cmake)
include(cmake/grpc_generate.cmake)

# Create directory for generated .proto files
set(GRPC_PROTO_GENS_DIR ${CMAKE_BINARY_DIR}/grpc_generated)
file(MAKE_DIRECTORY ${GRPC_PROTO_GENS_DIR})
set(PROTO_FILE "proto/api.proto")

grpc_generate(${PROTO_FILE} ${GRPC_PROTO_GENS_DIR})

# Add Library target with protobuf sources
add_library(protolib ${GRPC_GENERATED_SOURCES})
target_link_libraries(protolib PUBLIC gRPC::grpc++)
target_include_directories(protolib PUBLIC ${GRPC_PROTO_GENS_DIR})

# Build server app
set(SERVER_SOURCES 
    src/server/main.cpp
    src/server/dictionary.cpp
    src/server/dictionary.h
)
add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE protolib)

find_package(robin_hood REQUIRED)
target_link_libraries(server PRIVATE robin_hood::robin_hood)

# Build client lib
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
target_link_libraries(client PRIVATE clientlib)
