cmake_minimum_required(VERSION 3.0.0)

project(custom_allocator
    VERSION 1.0
    DESCRIPTION "2 custom allocators for KTH assignment"
    LANGUAGES CXX)

add_executable(custom_allocator
    src/main.cpp)

target_compile_options(custom_allocator PRIVATE -Isrc/ -Wall -Wextra -g)