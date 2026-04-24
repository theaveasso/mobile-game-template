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

# Test runner (does NOT link raylib — pure-logic tests only)
enable_testing()

set(TEST_SOURCES
    test/test_runner.c
    test/test_unit.c
    test/test_board.c
    test/test_combat.c
    src/unit.c
    src/board.c
    src/combat.c
)

add_executable(test_runner ${TEST_SOURCES})
target_include_directories(test_runner PRIVATE src test)

add_test(NAME all_tests COMMAND test_runner)
