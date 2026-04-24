cmake_minimum_required(VERSION 3.20)
project(game01 C)

set(CMAKE_C_STANDARD 11)

set(GAME_SOURCES
    src/main.c
    src/unit.c
    src/board.c
    src/combat.c
    src/render.c
    src/game.c
)

add_executable(game01 ${GAME_SOURCES})

find_package(raylib CONFIG REQUIRED)
target_link_libraries(game01 PRIVATE raylib)
target_include_directories(game01 PRIVATE src)
