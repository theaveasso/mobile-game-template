cmake_minimum_required(VERSION 3.20)
project(game01 C)

set(CMAKE_C_STANDARD 11)

add_executable(game01 src/main.c)

find_package(raylib CONFIG REQUIRED)
target_link_libraries(game01 PRIVATE raylib)
