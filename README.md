# demo_cpp
Simple client-server gRPC in C++ demonstrating CMake, Conan, Protobuf, gRPC, robin_hood::unordered_map, spdlog, bloom_filter

Tested on Ubuntu 22.04

## Prerequisites
- GCC 11.3
- Python 3.6
- CMake 3.5
- Conan 1.53
    - run `pip install conan`
    - run `conan profile new default --detect`
    - run `conan profile update settings.compiler.libcxx=libstdc++11 default`