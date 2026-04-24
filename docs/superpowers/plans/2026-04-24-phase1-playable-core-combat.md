# Phase 1 — Playable Core Combat Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Drop two fixed hardcoded teams onto a 3×3 portrait board, press "FIGHT", watch units auto-battle, see a winner. End of phase = a playable vertical slice on desktop AND Android with no shop, no economy, no synergies — just combat.

**Architecture:** Pure C with raylib. Deterministic data-oriented combat simulator as a separate module (testable without raylib). Rendering and input in raylib layer. Scene enum drives a simple state machine in `main.c`. All pure-logic code (units, board, combat sim) has unit tests; rendering/input smoke-tested by running the game.

**Tech Stack:** C11, raylib (already configured for desktop via vcpkg and Android via Makefile.Android). Custom minimal test macros (no external test framework). CMake for desktop build + test build. Existing Makefile.Android untouched except for adding new source files to `PROJECT_SOURCE_FILES`.

---

## Phase Overview (for context — only Phase 1 is planned in detail)

| Phase | Focus | Milestone |
|-------|-------|-----------|
| **1 (this plan)** | Playable Core Combat | Two hardcoded teams fight on 3×3 board |
| 2 | Shop & Economy | Buy units from 5-slot shop with gold, reroll, level up |
| 3 | Run Structure | Full 15-round run with HP, bosses at rounds 5/10/15 |
| 4 | Synergies & Stars | 5×5 synergy system, 3-star merges |
| 5 | Meta-Progression & Save | Runs persist, unit unlocks, Ascension 1-20 |
| 6 | Shader Polish & Juice | Glow, particles, shake, idle anim, sound |
| 7 | Android Ship Prep | AdMob, Play Store listing, signed APK |

Each later phase becomes its own plan document when Phase N completes.

---

## File Structure (for Phase 1)

**Source files (all in `src/`):**
- `main.c` — raylib entry + main loop + scene dispatch (STRIPPED DOWN from current hello-world)
- `game01.h` — shared enums and structs (Shape, Color, Unit, Board, Team)
- `unit.c` / `unit.h` — unit constructors, damage, queries
- `board.c` / `board.h` — 3×3 board data, place/remove/get cell, living count, is_defeated
- `combat.c` / `combat.h` — deterministic combat stepper (pure logic, no raylib)
- `render.c` / `render.h` — raylib rendering for board + units
- `game.c` / `game.h` — GameState struct, scene enum, update/render dispatch

**Test files (all in `test/`):**
- `test.h` — minimal test assertion macros (no dependencies)
- `test_runner.c` — runs all test suites
- `test_unit.c` — unit model tests
- `test_board.c` — board model tests
- `test_combat.c` — combat simulation tests

**Build files (modified):**
- `CMakeLists.txt` — extend to include all `src/*.c`, add separate `test_runner` executable target
- `src/Makefile.Android` — extend `PROJECT_SOURCE_FILES` line (line 63) to list all source files

**Assets:** none for Phase 1 (all shapes are drawn as raylib primitives, no textures).

---

## Task 1: Project Restructure (CMake + empty source files)

Goal: Prepare the build system for multi-file code without breaking current desktop or Android builds.

**Files:**
- Modify: `CMakeLists.txt`
- Create: `src/game01.h`, `src/unit.c`, `src/unit.h`, `src/board.c`, `src/board.h`, `src/combat.c`, `src/combat.h`, `src/render.c`, `src/render.h`, `src/game.c`, `src/game.h` (all as empty stubs)

- [ ] **Step 1: Create empty header/source stubs**

Create all the files below with minimal placeholder content so they compile:

`src/game01.h`:
```c
#ifndef GAME01_H
#define GAME01_H
#endif
```

`src/unit.h`:
```c
#ifndef GAME_UNIT_H
#define GAME_UNIT_H
#include "game01.h"
#endif
```

`src/unit.c`:
```c
#include "unit.h"
```

Create identical stubs for `board.h/c`, `combat.h/c`, `render.h/c`, `game.h/c` — each header guards its contents with `#ifndef GAME_<NAME>_H` / `#define GAME_<NAME>_H` / `#endif`, and each `.c` file just includes its matching `.h`.

- [ ] **Step 2: Update `CMakeLists.txt` to include all source files**

Replace entire contents of `CMakeLists.txt` with:

```cmake
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
```

- [ ] **Step 3: Verify desktop build still works**

Run: `cmake --preset debug && cmake --build --preset debug`
Expected: build succeeds, produces `build/debug/game01.exe` (or `build/debug/game01` on other platforms).

Run: `build/debug/game01.exe`
Expected: Hello World window opens (the existing main.c logic still runs because we didn't touch main.c).

- [ ] **Step 4: Update `src/Makefile.Android` to include new source files**

Open `src/Makefile.Android`, locate line 63:
```makefile
PROJECT_SOURCE_FILES   ?= main.c
```

Replace with:
```makefile
PROJECT_SOURCE_FILES   ?= main.c unit.c board.c combat.c render.c game.c
```

- [ ] **Step 5: Verify Android build still works**

Run: `android.bat` (existing automation script from prior session).
Expected: APK builds successfully and installs if a device is connected. The app still shows Hello World (because main.c is unchanged).

- [ ] **Step 6: Commit**

```bash
git add CMakeLists.txt src/*.c src/*.h
git commit -m "chore: scaffold multi-file source layout for Phase 1"
```

---

## Task 2: Test Infrastructure

Goal: A minimal test framework that lets us TDD pure-logic code without any external dependencies.

**Files:**
- Create: `test/test.h`, `test/test_runner.c`, `test/test_unit.c` (empty stub), `test/test_board.c` (empty stub), `test/test_combat.c` (empty stub)
- Modify: `CMakeLists.txt` (add test executable target)

- [ ] **Step 1: Create `test/test.h` with assertion macros**

Create `test/test.h`:

```c
#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <string.h>

// Counters live in test_runner.c. extern here so every TU sees the same ones.
extern int _tests_run;
extern int _tests_failed;

#define TEST_ASSERT(cond) do {                                        \
    if (!(cond)) {                                                    \
        _tests_failed++;                                               \
        fprintf(stderr, "  FAIL: %s:%d: %s\n",                         \
                __FILE__, __LINE__, #cond);                            \
    }                                                                  \
} while (0)

#define TEST_ASSERT_EQ(a, b) do {                                     \
    long long _va = (long long)(a);                                   \
    long long _vb = (long long)(b);                                   \
    if (_va != _vb) {                                                 \
        _tests_failed++;                                               \
        fprintf(stderr, "  FAIL: %s:%d: %s == %s (got %lld vs %lld)\n",\
                __FILE__, __LINE__, #a, #b, _va, _vb);                 \
    }                                                                  \
} while (0)

#define TEST_CASE(name) static void test_##name(void)

// Increment counter once per test case (not per assertion).
#define TEST_RUN(name) do {                                           \
    _tests_run++;                                                     \
    printf("  %s\n", #name);                                           \
    test_##name();                                                     \
} while (0)

#define TEST_SUMMARY_AND_EXIT() do {                                  \
    printf("\n%d tests run, %d failed\n", _tests_run, _tests_failed);  \
    return _tests_failed > 0 ? 1 : 0;                                  \
} while (0)

#endif
```

- [ ] **Step 2: Create test stubs**

Create `test/test_unit.c`:
```c
#include "test.h"
#include "../src/unit.h"

void run_unit_tests(void) {
    printf("Unit tests:\n");
}
```

Create `test/test_board.c`:
```c
#include "test.h"
#include "../src/board.h"

void run_board_tests(void) {
    printf("Board tests:\n");
}
```

Create `test/test_combat.c`:
```c
#include "test.h"
#include "../src/combat.h"

void run_combat_tests(void) {
    printf("Combat tests:\n");
}
```

- [ ] **Step 3: Create `test/test_runner.c`**

```c
#include "test.h"

void run_unit_tests(void);
void run_board_tests(void);
void run_combat_tests(void);

int main(void) {
    run_unit_tests();
    run_board_tests();
    run_combat_tests();
    TEST_SUMMARY_AND_EXIT();
}
```

- [ ] **Step 4: Add test target to `CMakeLists.txt`**

Append to `CMakeLists.txt`:

```cmake
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
```

- [ ] **Step 5: Build and run tests**

```bash
cmake --preset debug
cmake --build --preset debug --target test_runner
build/debug/test_runner.exe
```
Expected output:
```
Unit tests:
Board tests:
Combat tests:

0 tests run, 0 failed
```
Exit code 0.

- [ ] **Step 6: Commit**

```bash
git add test/ CMakeLists.txt
git commit -m "test: add minimal C test framework and empty suite"
```

---

## Task 3: Unit Data Model (TDD)

Goal: Define the Unit struct and basic operations. Pure logic, fully tested.

**Files:**
- Modify: `src/game01.h`, `src/unit.h`, `src/unit.c`, `test/test_unit.c`

- [ ] **Step 1: Write failing tests for unit creation and damage**

Replace `test/test_unit.c` with:

```c
#include "test.h"
#include "../src/unit.h"

TEST_CASE(unit_create_sets_fields) {
    Unit u = unit_create(SHAPE_TRIANGLE, COLOR_RED);
    TEST_ASSERT_EQ(u.shape, SHAPE_TRIANGLE);
    TEST_ASSERT_EQ(u.color, COLOR_RED);
    TEST_ASSERT_EQ(u.stars, 1);
    TEST_ASSERT(u.hp > 0);
    TEST_ASSERT(u.max_hp == u.hp);
    TEST_ASSERT(u.attack > 0);
}

TEST_CASE(unit_take_damage_reduces_hp) {
    Unit u = unit_create(SHAPE_SQUARE, COLOR_BLUE);
    int start_hp = u.hp;
    unit_take_damage(&u, 5);
    TEST_ASSERT_EQ(u.hp, start_hp - 5);
}

TEST_CASE(unit_is_alive_false_when_hp_zero) {
    Unit u = unit_create(SHAPE_CIRCLE, COLOR_GREEN);
    unit_take_damage(&u, 9999);
    TEST_ASSERT(!unit_is_alive(&u));
    TEST_ASSERT_EQ(u.hp, 0);
}

TEST_CASE(unit_square_has_more_hp_than_triangle) {
    Unit tank = unit_create(SHAPE_SQUARE, COLOR_RED);
    Unit atk  = unit_create(SHAPE_TRIANGLE, COLOR_RED);
    TEST_ASSERT(tank.max_hp > atk.max_hp);
}

void run_unit_tests(void) {
    printf("Unit tests:\n");
    TEST_RUN(unit_create_sets_fields);
    TEST_RUN(unit_take_damage_reduces_hp);
    TEST_RUN(unit_is_alive_false_when_hp_zero);
    TEST_RUN(unit_square_has_more_hp_than_triangle);
}
```

- [ ] **Step 2: Build test to verify it fails**

```bash
cmake --build --preset debug --target test_runner
```
Expected: compile error — `unit_create` / `unit_take_damage` / `unit_is_alive` not declared; types `Unit`, `SHAPE_TRIANGLE`, `COLOR_RED` not defined.

- [ ] **Step 3: Define types in `src/game01.h`**

Replace `src/game01.h` with:

```c
#ifndef GAME01_H
#define GAME01_H

typedef enum {
    SHAPE_TRIANGLE,
    SHAPE_SQUARE,
    SHAPE_CIRCLE,
    SHAPE_HEXAGON,
    SHAPE_DIAMOND,
    SHAPE_COUNT
} UnitShape;

typedef enum {
    COLOR_RED,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_PURPLE,
    COLOR_COUNT
} UnitColor;

typedef struct {
    UnitShape shape;
    UnitColor color;
    int stars;      // 1, 2, or 3
    int hp;         // current HP
    int max_hp;
    int attack;     // damage per attack
    int attack_cooldown_ms;  // time between attacks
    int cooldown_remaining_ms; // countdown during combat
} Unit;

#endif
```

- [ ] **Step 4: Declare unit functions in `src/unit.h`**

Replace `src/unit.h` with:

```c
#ifndef GAME_UNIT_H
#define GAME_UNIT_H

#include "game01.h"

Unit unit_create(UnitShape shape, UnitColor color);
void unit_take_damage(Unit* unit, int damage);
int  unit_is_alive(const Unit* unit);

#endif
```

- [ ] **Step 5: Implement `src/unit.c`**

Replace `src/unit.c` with:

```c
#include "unit.h"

// Base stats per shape. Color has no stat effect in Phase 1 (synergies come in Phase 4).
typedef struct {
    int hp;
    int attack;
    int attack_cooldown_ms;
} ShapeStats;

static const ShapeStats SHAPE_STATS[SHAPE_COUNT] = {
    [SHAPE_TRIANGLE] = { .hp = 40,  .attack = 12, .attack_cooldown_ms = 800  },
    [SHAPE_SQUARE]   = { .hp = 80,  .attack = 6,  .attack_cooldown_ms = 1200 },
    [SHAPE_CIRCLE]   = { .hp = 35,  .attack = 4,  .attack_cooldown_ms = 700  },
    [SHAPE_HEXAGON]  = { .hp = 30,  .attack = 15, .attack_cooldown_ms = 1500 },
    [SHAPE_DIAMOND]  = { .hp = 25,  .attack = 18, .attack_cooldown_ms = 900  },
};

Unit unit_create(UnitShape shape, UnitColor color) {
    ShapeStats s = SHAPE_STATS[shape];
    Unit u = {
        .shape = shape,
        .color = color,
        .stars = 1,
        .hp = s.hp,
        .max_hp = s.hp,
        .attack = s.attack,
        .attack_cooldown_ms = s.attack_cooldown_ms,
        .cooldown_remaining_ms = 0,
    };
    return u;
}

void unit_take_damage(Unit* unit, int damage) {
    if (damage < 0) damage = 0;
    unit->hp -= damage;
    if (unit->hp < 0) unit->hp = 0;
}

int unit_is_alive(const Unit* unit) {
    return unit->hp > 0;
}
```

- [ ] **Step 6: Build and run tests**

```bash
cmake --build --preset debug --target test_runner
build/debug/test_runner.exe
```
Expected:
```
Unit tests:
  unit_create_sets_fields
  unit_take_damage_reduces_hp
  unit_is_alive_false_when_hp_zero
  unit_square_has_more_hp_than_triangle
Board tests:
Combat tests:

4 tests run, 0 failed
```

- [ ] **Step 7: Commit**

```bash
git add src/game01.h src/unit.h src/unit.c test/test_unit.c
git commit -m "feat(unit): add Unit data model with per-shape base stats"
```

---

## Task 4: Board Data Model (TDD)

Goal: 3×3 board with place/remove/get operations and win-condition checks.

**Files:**
- Modify: `src/game01.h`, `src/board.h`, `src/board.c`, `test/test_board.c`

- [ ] **Step 1: Write failing tests**

Replace `test/test_board.c` with:

```c
#include "test.h"
#include "../src/board.h"
#include "../src/unit.h"

TEST_CASE(board_starts_empty) {
    Board b = board_create();
    for (int i = 0; i < BOARD_CELLS; i++) {
        TEST_ASSERT(board_get_unit(&b, i) == NULL);
    }
    TEST_ASSERT_EQ(board_count_living(&b), 0);
}

TEST_CASE(board_place_and_get_unit) {
    Board b = board_create();
    Unit u = unit_create(SHAPE_TRIANGLE, COLOR_RED);
    board_place_unit(&b, 4, u);  // center cell
    Unit* got = board_get_unit(&b, 4);
    TEST_ASSERT(got != NULL);
    TEST_ASSERT_EQ(got->shape, SHAPE_TRIANGLE);
}

TEST_CASE(board_remove_unit) {
    Board b = board_create();
    Unit u = unit_create(SHAPE_SQUARE, COLOR_BLUE);
    board_place_unit(&b, 0, u);
    board_remove_unit(&b, 0);
    TEST_ASSERT(board_get_unit(&b, 0) == NULL);
}

TEST_CASE(board_count_living_ignores_dead_units) {
    Board b = board_create();
    board_place_unit(&b, 0, unit_create(SHAPE_TRIANGLE, COLOR_RED));
    board_place_unit(&b, 1, unit_create(SHAPE_SQUARE, COLOR_BLUE));
    TEST_ASSERT_EQ(board_count_living(&b), 2);

    Unit* dead = board_get_unit(&b, 1);
    unit_take_damage(dead, 9999);
    TEST_ASSERT_EQ(board_count_living(&b), 1);
}

TEST_CASE(board_is_defeated_when_no_living_units) {
    Board b = board_create();
    TEST_ASSERT(board_is_defeated(&b));

    board_place_unit(&b, 0, unit_create(SHAPE_TRIANGLE, COLOR_RED));
    TEST_ASSERT(!board_is_defeated(&b));

    Unit* u = board_get_unit(&b, 0);
    unit_take_damage(u, 9999);
    TEST_ASSERT(board_is_defeated(&b));
}

void run_board_tests(void) {
    printf("Board tests:\n");
    TEST_RUN(board_starts_empty);
    TEST_RUN(board_place_and_get_unit);
    TEST_RUN(board_remove_unit);
    TEST_RUN(board_count_living_ignores_dead_units);
    TEST_RUN(board_is_defeated_when_no_living_units);
}
```

- [ ] **Step 2: Build test to verify it fails**

```bash
cmake --build --preset debug --target test_runner
```
Expected: compile errors — `Board`, `BOARD_CELLS`, `board_create`, etc. not defined.

- [ ] **Step 3: Add Board struct to `src/game01.h`**

Append to `src/game01.h` BEFORE `#endif`:

```c
#define BOARD_WIDTH  3
#define BOARD_HEIGHT 3
#define BOARD_CELLS  9

typedef struct {
    Unit cells[BOARD_CELLS];
    int  occupied[BOARD_CELLS];  // 1 if cell has a unit, 0 if empty
} Board;
```

(We use an occupied flag rather than NULL because cells store Unit by value for cache locality and simpler ownership semantics. `Unit*` returned by `board_get_unit` points into the board's own array.)

- [ ] **Step 4: Declare board functions in `src/board.h`**

Replace `src/board.h` with:

```c
#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "game01.h"

Board board_create(void);
void  board_place_unit(Board* board, int cell, Unit unit);
void  board_remove_unit(Board* board, int cell);
Unit* board_get_unit(Board* board, int cell);  // NULL if cell empty
int   board_count_living(const Board* board);
int   board_is_defeated(const Board* board);   // 1 if no living units

#endif
```

- [ ] **Step 5: Implement `src/board.c`**

Replace `src/board.c` with:

```c
#include "board.h"
#include "unit.h"
#include <string.h>

Board board_create(void) {
    Board b;
    memset(&b, 0, sizeof(Board));
    return b;
}

void board_place_unit(Board* board, int cell, Unit unit) {
    if (cell < 0 || cell >= BOARD_CELLS) return;
    board->cells[cell] = unit;
    board->occupied[cell] = 1;
}

void board_remove_unit(Board* board, int cell) {
    if (cell < 0 || cell >= BOARD_CELLS) return;
    board->occupied[cell] = 0;
}

Unit* board_get_unit(Board* board, int cell) {
    if (cell < 0 || cell >= BOARD_CELLS) return NULL;
    if (!board->occupied[cell]) return NULL;
    return &board->cells[cell];
}

int board_count_living(const Board* board) {
    int count = 0;
    for (int i = 0; i < BOARD_CELLS; i++) {
        if (!board->occupied[i]) continue;
        if (unit_is_alive(&board->cells[i])) count++;
    }
    return count;
}

int board_is_defeated(const Board* board) {
    return board_count_living(board) == 0;
}
```

- [ ] **Step 6: Build and run tests**

```bash
cmake --build --preset debug --target test_runner
build/debug/test_runner.exe
```
Expected: all 9 tests pass (4 unit + 5 board), 0 failed.

- [ ] **Step 7: Commit**

```bash
git add src/game01.h src/board.h src/board.c test/test_board.c
git commit -m "feat(board): add 3x3 board with place/remove/get operations"
```

---

## Task 5: Combat Simulation (TDD)

Goal: Deterministic combat stepper. Given two boards and a time delta, advance the simulation one tick. Each living unit targets the nearest living enemy by board cell distance and attacks when its cooldown reaches zero.

**Files:**
- Modify: `src/combat.h`, `src/combat.c`, `test/test_combat.c`

- [ ] **Step 1: Write failing tests**

Replace `test/test_combat.c` with:

```c
#include "test.h"
#include "../src/combat.h"
#include "../src/board.h"
#include "../src/unit.h"

// Helper: build a board with one unit at cell 0 for convenience.
static Board single_unit_board(UnitShape s, UnitColor c) {
    Board b = board_create();
    board_place_unit(&b, 0, unit_create(s, c));
    return b;
}

TEST_CASE(combat_step_reduces_cooldown) {
    Board friendly = single_unit_board(SHAPE_TRIANGLE, COLOR_RED);
    Board enemy    = single_unit_board(SHAPE_SQUARE,   COLOR_BLUE);

    Unit* f = board_get_unit(&friendly, 0);
    f->cooldown_remaining_ms = 500;

    combat_step(&friendly, &enemy, 100);  // 100ms tick

    TEST_ASSERT_EQ(f->cooldown_remaining_ms, 400);
}

TEST_CASE(combat_step_attacks_when_cooldown_zero) {
    Board friendly = single_unit_board(SHAPE_TRIANGLE, COLOR_RED);   // attack=12
    Board enemy    = single_unit_board(SHAPE_SQUARE,   COLOR_BLUE);  // hp=80

    Unit* f = board_get_unit(&friendly, 0);
    f->cooldown_remaining_ms = 0;

    combat_step(&friendly, &enemy, 16);

    Unit* e = board_get_unit(&enemy, 0);
    TEST_ASSERT_EQ(e->hp, 80 - 12);
    // Cooldown resets after attack
    TEST_ASSERT_EQ(f->cooldown_remaining_ms, f->attack_cooldown_ms - 16);
}

TEST_CASE(combat_dead_unit_does_not_attack) {
    Board friendly = single_unit_board(SHAPE_TRIANGLE, COLOR_RED);
    Board enemy    = single_unit_board(SHAPE_SQUARE,   COLOR_BLUE);

    Unit* f = board_get_unit(&friendly, 0);
    unit_take_damage(f, 9999);

    Unit* e = board_get_unit(&enemy, 0);
    int enemy_start_hp = e->hp;

    combat_step(&friendly, &enemy, 2000);
    TEST_ASSERT_EQ(e->hp, enemy_start_hp);
}

TEST_CASE(combat_is_over_when_one_side_wiped) {
    Board friendly = single_unit_board(SHAPE_TRIANGLE, COLOR_RED);
    Board enemy    = single_unit_board(SHAPE_SQUARE,   COLOR_BLUE);

    TEST_ASSERT_EQ(combat_result(&friendly, &enemy), COMBAT_ONGOING);

    Unit* e = board_get_unit(&enemy, 0);
    unit_take_damage(e, 9999);
    TEST_ASSERT_EQ(combat_result(&friendly, &enemy), COMBAT_FRIENDLY_WIN);

    Unit* f = board_get_unit(&friendly, 0);
    unit_take_damage(f, 9999);
    TEST_ASSERT_EQ(combat_result(&friendly, &enemy), COMBAT_DRAW);
}

TEST_CASE(combat_is_deterministic) {
    // Same initial state, same tick sequence → same result.
    Board a1 = board_create();
    Board a2 = board_create();
    board_place_unit(&a1, 0, unit_create(SHAPE_TRIANGLE, COLOR_RED));
    board_place_unit(&a1, 4, unit_create(SHAPE_SQUARE,   COLOR_BLUE));
    board_place_unit(&a2, 0, unit_create(SHAPE_TRIANGLE, COLOR_RED));
    board_place_unit(&a2, 4, unit_create(SHAPE_SQUARE,   COLOR_BLUE));

    Board b1 = board_create();
    Board b2 = board_create();
    board_place_unit(&b1, 0, unit_create(SHAPE_CIRCLE,  COLOR_GREEN));
    board_place_unit(&b1, 8, unit_create(SHAPE_DIAMOND, COLOR_PURPLE));
    board_place_unit(&b2, 0, unit_create(SHAPE_CIRCLE,  COLOR_GREEN));
    board_place_unit(&b2, 8, unit_create(SHAPE_DIAMOND, COLOR_PURPLE));

    for (int i = 0; i < 200; i++) {
        combat_step(&a1, &b1, 16);
        combat_step(&a2, &b2, 16);
    }

    for (int cell = 0; cell < BOARD_CELLS; cell++) {
        Unit* u1 = board_get_unit(&a1, cell);
        Unit* u2 = board_get_unit(&a2, cell);
        if (u1 == NULL) { TEST_ASSERT(u2 == NULL); continue; }
        TEST_ASSERT(u2 != NULL);
        TEST_ASSERT_EQ(u1->hp, u2->hp);
    }
}

void run_combat_tests(void) {
    printf("Combat tests:\n");
    TEST_RUN(combat_step_reduces_cooldown);
    TEST_RUN(combat_step_attacks_when_cooldown_zero);
    TEST_RUN(combat_dead_unit_does_not_attack);
    TEST_RUN(combat_is_over_when_one_side_wiped);
    TEST_RUN(combat_is_deterministic);
}
```

- [ ] **Step 2: Build test to verify it fails**

```bash
cmake --build --preset debug --target test_runner
```
Expected: compile errors — `combat_step`, `combat_result`, `COMBAT_ONGOING`, etc. not defined.

- [ ] **Step 3: Declare combat API in `src/combat.h`**

Replace `src/combat.h` with:

```c
#ifndef GAME_COMBAT_H
#define GAME_COMBAT_H

#include "game01.h"

typedef enum {
    COMBAT_ONGOING,
    COMBAT_FRIENDLY_WIN,
    COMBAT_ENEMY_WIN,
    COMBAT_DRAW,
} CombatResult;

// Advance combat simulation by dt_ms milliseconds.
// Both boards are mutated: cooldowns tick, damage applies, units die.
void combat_step(Board* friendly, Board* enemy, int dt_ms);

// Check outcome without mutating.
CombatResult combat_result(const Board* friendly, const Board* enemy);

#endif
```

- [ ] **Step 4: Implement `src/combat.c`**

Replace `src/combat.c` with:

```c
#include "combat.h"
#include "board.h"
#include "unit.h"
#include <stdlib.h>

// Manhattan distance between two cell indices on a BOARD_WIDTH-wide grid.
static int cell_distance(int a, int b) {
    int ax = a % BOARD_WIDTH, ay = a / BOARD_WIDTH;
    int bx = b % BOARD_WIDTH, by = b / BOARD_WIDTH;
    int dx = ax - bx; if (dx < 0) dx = -dx;
    int dy = ay - by; if (dy < 0) dy = -dy;
    return dx + dy;
}

// Find cell index of nearest living enemy unit. Tie-break: lowest cell index
// (deterministic). Returns -1 if no living enemy.
static int nearest_enemy_cell(int attacker_cell, const Board* enemy) {
    int best_cell = -1;
    int best_dist = 1 << 30;
    for (int i = 0; i < BOARD_CELLS; i++) {
        if (!enemy->occupied[i]) continue;
        if (!unit_is_alive(&enemy->cells[i])) continue;
        int d = cell_distance(attacker_cell, i);
        if (d < best_dist) {
            best_dist = d;
            best_cell = i;
        }
    }
    return best_cell;
}

static void step_side(Board* side, Board* opponent, int dt_ms) {
    for (int i = 0; i < BOARD_CELLS; i++) {
        if (!side->occupied[i]) continue;
        Unit* attacker = &side->cells[i];
        if (!unit_is_alive(attacker)) continue;

        attacker->cooldown_remaining_ms -= dt_ms;
        if (attacker->cooldown_remaining_ms > 0) continue;

        int target_cell = nearest_enemy_cell(i, opponent);
        if (target_cell < 0) {
            attacker->cooldown_remaining_ms = 0;
            continue;
        }

        Unit* target = &opponent->cells[target_cell];
        unit_take_damage(target, attacker->attack);
        attacker->cooldown_remaining_ms += attacker->attack_cooldown_ms;
        if (attacker->cooldown_remaining_ms < 0) {
            attacker->cooldown_remaining_ms = 0;
        }
    }
}

void combat_step(Board* friendly, Board* enemy, int dt_ms) {
    step_side(friendly, enemy, dt_ms);
    step_side(enemy, friendly, dt_ms);
}

CombatResult combat_result(const Board* friendly, const Board* enemy) {
    int f_dead = board_is_defeated(friendly);
    int e_dead = board_is_defeated(enemy);
    if (f_dead && e_dead) return COMBAT_DRAW;
    if (e_dead) return COMBAT_FRIENDLY_WIN;
    if (f_dead) return COMBAT_ENEMY_WIN;
    return COMBAT_ONGOING;
}
```

- [ ] **Step 5: Build and run tests**

```bash
cmake --build --preset debug --target test_runner
build/debug/test_runner.exe
```
Expected: all 14 tests pass (4 unit + 5 board + 5 combat), 0 failed.

- [ ] **Step 6: Commit**

```bash
git add src/combat.h src/combat.c test/test_combat.c
git commit -m "feat(combat): add deterministic combat stepper with nearest-enemy targeting"
```

---

## Task 6: Game State & Scene System

Goal: A minimal state machine so we can switch between "setup" (place units) and "combat" (auto-battle running) scenes. No visuals yet — just state logic.

**Files:**
- Modify: `src/game.h`, `src/game.c`

- [ ] **Step 1: Declare GameState in `src/game.h`**

Replace `src/game.h` with:

```c
#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "game01.h"

typedef enum {
    SCENE_SETUP,       // Player looking at the pre-placed teams, about to press FIGHT.
    SCENE_COMBAT,      // Auto-battle is running.
    SCENE_RESULT,      // Win/lose screen shown.
} Scene;

typedef struct {
    Scene  scene;
    Board  friendly;
    Board  enemy;
    float  combat_elapsed_ms;   // time since combat started
} GameState;

GameState game_create(void);
void      game_start_combat(GameState* g);
void      game_step(GameState* g, int dt_ms);
void      game_reset_to_setup(GameState* g);

#endif
```

- [ ] **Step 2: Implement `src/game.c`**

Replace `src/game.c` with:

```c
#include "game.h"
#include "board.h"
#include "unit.h"
#include "combat.h"

// Phase 1: two hardcoded teams. Phase 2 replaces this with the shop.
static void seed_demo_teams(GameState* g) {
    g->friendly = board_create();
    g->enemy    = board_create();

    // Friendly team (bottom board):
    //   front row: Square(Blue), Square(Red), Triangle(Yellow)
    //   mid:       Triangle(Red)
    //   back:      Hexagon(Purple)
    board_place_unit(&g->friendly, 0, unit_create(SHAPE_SQUARE,   COLOR_BLUE));
    board_place_unit(&g->friendly, 1, unit_create(SHAPE_SQUARE,   COLOR_RED));
    board_place_unit(&g->friendly, 2, unit_create(SHAPE_TRIANGLE, COLOR_YELLOW));
    board_place_unit(&g->friendly, 4, unit_create(SHAPE_TRIANGLE, COLOR_RED));
    board_place_unit(&g->friendly, 7, unit_create(SHAPE_HEXAGON,  COLOR_PURPLE));

    // Enemy team (top board):
    //   front row: Triangle(Green), Triangle(Blue), Diamond(Red)
    //   mid:       Square(Yellow)
    //   back:      Circle(Green)
    board_place_unit(&g->enemy, 0, unit_create(SHAPE_TRIANGLE, COLOR_GREEN));
    board_place_unit(&g->enemy, 1, unit_create(SHAPE_TRIANGLE, COLOR_BLUE));
    board_place_unit(&g->enemy, 2, unit_create(SHAPE_DIAMOND,  COLOR_RED));
    board_place_unit(&g->enemy, 4, unit_create(SHAPE_SQUARE,   COLOR_YELLOW));
    board_place_unit(&g->enemy, 7, unit_create(SHAPE_CIRCLE,   COLOR_GREEN));
}

GameState game_create(void) {
    GameState g;
    g.scene = SCENE_SETUP;
    g.combat_elapsed_ms = 0.0f;
    seed_demo_teams(&g);
    return g;
}

void game_start_combat(GameState* g) {
    g->scene = SCENE_COMBAT;
    g->combat_elapsed_ms = 0.0f;
}

void game_step(GameState* g, int dt_ms) {
    if (g->scene != SCENE_COMBAT) return;

    combat_step(&g->friendly, &g->enemy, dt_ms);
    g->combat_elapsed_ms += (float)dt_ms;

    CombatResult r = combat_result(&g->friendly, &g->enemy);
    if (r != COMBAT_ONGOING) {
        g->scene = SCENE_RESULT;
    }
}

void game_reset_to_setup(GameState* g) {
    g->combat_elapsed_ms = 0.0f;
    seed_demo_teams(g);
    g->scene = SCENE_SETUP;
}
```

- [ ] **Step 3: Add `src/game.c` to CMake test sources for future use, but do NOT test yet**

No code change needed — tests for `game.c` would require mocking raylib input, which we skip for Phase 1. Game-state transitions are covered by playing the game in later tasks.

- [ ] **Step 4: Build the executable to make sure nothing is broken**

```bash
cmake --build --preset debug
```
Expected: `game01.exe` still builds (game.c compiles fine even though main.c doesn't use it yet).

- [ ] **Step 5: Commit**

```bash
git add src/game.h src/game.c
git commit -m "feat(game): add scene state machine with hardcoded demo teams"
```

---

## Task 7: Render Board Grid (smoke-test by running)

Goal: Draw the 3×3 portrait board layout on screen — empty grid only, no units yet.

**Files:**
- Modify: `src/render.h`, `src/render.c`

- [ ] **Step 1: Declare render API in `src/render.h`**

Replace `src/render.h` with:

```c
#ifndef GAME_RENDER_H
#define GAME_RENDER_H

#include "game01.h"
#include "game.h"

// All rendering happens between raylib's BeginDrawing / EndDrawing.
// Caller is responsible for those; render functions only draw.

// Coordinates are in window pixels. Window is assumed portrait 450x900
// for Phase 1 desktop development (scaled on Android to match screen).

#define WINDOW_WIDTH  450
#define WINDOW_HEIGHT 900

void render_background(void);
void render_board(const Board* board, int top_y, int cell_size);
void render_game(const GameState* g);

#endif
```

- [ ] **Step 2: Implement `src/render.c`**

Replace `src/render.c` with:

```c
#include "render.h"
#include "board.h"
#include "raylib.h"

#define GAME01_BG  ((Color){ 26, 26, 26, 255 })
#define GRID_LINE    ((Color){ 60, 60, 60, 255 })

void render_background(void) {
    ClearBackground(GAME01_BG);
}

void render_board(const Board* board, int top_y, int cell_size) {
    (void)board;  // units rendered in Task 8
    int grid_x = (WINDOW_WIDTH - (cell_size * BOARD_WIDTH)) / 2;

    for (int row = 0; row <= BOARD_HEIGHT; row++) {
        int y = top_y + row * cell_size;
        DrawLine(grid_x, y, grid_x + cell_size * BOARD_WIDTH, y, GRID_LINE);
    }
    for (int col = 0; col <= BOARD_WIDTH; col++) {
        int x = grid_x + col * cell_size;
        DrawLine(x, top_y, x, top_y + cell_size * BOARD_HEIGHT, GRID_LINE);
    }
}

void render_game(const GameState* g) {
    render_background();

    int cell_size = 120;
    int enemy_top_y    = 80;
    int friendly_top_y = WINDOW_HEIGHT - 80 - (cell_size * BOARD_HEIGHT);

    render_board(&g->enemy,    enemy_top_y,    cell_size);
    render_board(&g->friendly, friendly_top_y, cell_size);
}
```

- [ ] **Step 3: Wire `render_game` into `src/main.c`**

Replace `src/main.c` with:

```c
#include "raylib.h"
#include "game.h"
#include "render.h"

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME01_TITLE);
    SetTargetFPS(60);

    GameState g = game_create();

    while (!WindowShouldClose()) {
        int dt_ms = (int)(GetFrameTime() * 1000.0f);
        game_step(&g, dt_ms);

        BeginDrawing();
        render_game(&g);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

- [ ] **Step 4: Update CMakeLists.txt so game01 links render.c**

`render.c` is already in `GAME_SOURCES` from Task 1. We need raylib linked to `game01` (already done). No CMake change required.

- [ ] **Step 5: Build and run**

```bash
cmake --build --preset debug
build/debug/game01.exe
```
Expected: A dark gray 450×900 portrait window opens. Two 3×3 grids are visible — one near the top (enemy) and one near the bottom (your side). Empty cells. Window closes with ESC.

- [ ] **Step 6: Commit**

```bash
git add src/render.h src/render.c src/main.c
git commit -m "feat(render): draw empty 3x3 portrait boards on dark background"
```

---

## Task 8: Render Units (smoke-test)

Goal: Draw each unit as a flat 2D shape in its color inside its board cell.

**Files:**
- Modify: `src/render.c`

- [ ] **Step 1: Add a color palette and unit-draw helper to `src/render.c`**

Add to the top of `src/render.c` (after existing `#define` lines):

```c
#define STAR1_SIZE 0.85f   // fraction of cell size

static Color color_for_unit(UnitColor c) {
    switch (c) {
        case COLOR_RED:    return (Color){ 255,  70,  90, 255 };
        case COLOR_BLUE:   return (Color){  80, 170, 255, 255 };
        case COLOR_GREEN:  return (Color){  80, 230, 140, 255 };
        case COLOR_YELLOW: return (Color){ 255, 220,  80, 255 };
        case COLOR_PURPLE: return (Color){ 200, 120, 255, 255 };
        default:           return WHITE;
    }
}

static void draw_unit_shape(UnitShape shape, Color c, int cx, int cy, int size) {
    float r = size * 0.5f;
    switch (shape) {
        case SHAPE_TRIANGLE: {
            Vector2 a = { cx,         cy - r };
            Vector2 b = { cx - r*0.9f, cy + r*0.7f };
            Vector2 d = { cx + r*0.9f, cy + r*0.7f };
            DrawTriangle(a, b, d, c);
            break;
        }
        case SHAPE_SQUARE:
            DrawRectangle(cx - r, cy - r, (int)(r*2), (int)(r*2), c);
            break;
        case SHAPE_CIRCLE:
            DrawCircle(cx, cy, r, c);
            break;
        case SHAPE_HEXAGON:
            DrawPoly((Vector2){ cx, cy }, 6, r, 30, c);
            break;
        case SHAPE_DIAMOND:
            DrawPoly((Vector2){ cx, cy }, 4, r, 45, c);
            break;
        default:
            break;
    }
}
```

- [ ] **Step 2: Draw units on the board inside `render_board`**

Replace the existing `render_board` function with:

```c
void render_board(const Board* board, int top_y, int cell_size) {
    int grid_x = (WINDOW_WIDTH - (cell_size * BOARD_WIDTH)) / 2;

    // Grid lines
    for (int row = 0; row <= BOARD_HEIGHT; row++) {
        int y = top_y + row * cell_size;
        DrawLine(grid_x, y, grid_x + cell_size * BOARD_WIDTH, y, GRID_LINE);
    }
    for (int col = 0; col <= BOARD_WIDTH; col++) {
        int x = grid_x + col * cell_size;
        DrawLine(x, top_y, x, top_y + cell_size * BOARD_HEIGHT, GRID_LINE);
    }

    // Units
    int unit_size = (int)(cell_size * STAR1_SIZE);
    for (int cell = 0; cell < BOARD_CELLS; cell++) {
        if (!board->occupied[cell]) continue;
        const Unit* u = &board->cells[cell];
        if (u->hp <= 0) continue;
        int col = cell % BOARD_WIDTH;
        int row = cell / BOARD_WIDTH;
        int cx = grid_x + col * cell_size + cell_size / 2;
        int cy = top_y + row * cell_size + cell_size / 2;
        Color c = color_for_unit(u->color);
        draw_unit_shape(u->shape, c, cx, cy, unit_size);

        // HP bar under the unit
        int bar_w = cell_size - 20;
        int bar_h = 4;
        int bar_x = cx - bar_w / 2;
        int bar_y = cy + cell_size / 2 - 10;
        DrawRectangle(bar_x, bar_y, bar_w, bar_h, (Color){ 50, 50, 50, 255 });
        int filled = (u->max_hp > 0) ? (bar_w * u->hp / u->max_hp) : 0;
        DrawRectangle(bar_x, bar_y, filled, bar_h, c);
    }
}
```

- [ ] **Step 3: Build and run**

```bash
cmake --build --preset debug
build/debug/game01.exe
```
Expected: Window opens showing enemy team in top grid (5 units: 3 triangles/1 diamond/1 square/1 circle in various colors) and your team in bottom grid (2 squares/2 triangles/1 hexagon in various colors). HP bars visible under each unit.

- [ ] **Step 4: Visual sanity checklist**

Walk through this list manually — if any fails, stop and fix before committing:
- [ ] All 5 shapes (triangle, square, circle, hexagon, diamond) render recognizably at 120×120 cell size
- [ ] All 5 colors (red, blue, green, yellow, purple) are visually distinct on the dark gray background
- [ ] Units fit inside their grid cells (no overflow)
- [ ] HP bars are full width (green-ish against gray) on freshly placed units
- [ ] Empty cells show no visual noise

- [ ] **Step 5: Commit**

```bash
git add src/render.c
git commit -m "feat(render): draw 5 shapes x 5 colors as flat primitives with HP bars"
```

---

## Task 9: Fight Button and Combat Playthrough

Goal: Show a FIGHT button in SCENE_SETUP. Tapping it starts combat. Combat plays out visibly over time.

**Files:**
- Modify: `src/render.c`, `src/render.h`, `src/main.c`

- [ ] **Step 1: Add a button-rendering and hit-testing helper to `src/render.h`**

Append to `src/render.h` BEFORE `#endif`:

```c
typedef struct {
    int x, y, w, h;
} Rect;

// Returns 1 if rect is being clicked this frame.
int render_button(Rect r, const char* label);

extern const Rect FIGHT_BUTTON_RECT;
extern const Rect RESET_BUTTON_RECT;
```

- [ ] **Step 2: Implement the button in `src/render.c`**

Append to `src/render.c`:

```c
const Rect FIGHT_BUTTON_RECT = { 75, WINDOW_HEIGHT - 100, 300, 70 };
const Rect RESET_BUTTON_RECT = { 75, WINDOW_HEIGHT - 100, 300, 70 };

int render_button(Rect r, const char* label) {
    Vector2 m = GetMousePosition();
    int hover = (m.x >= r.x && m.x <= r.x + r.w &&
                 m.y >= r.y && m.y <= r.y + r.h);
    Color fill  = hover ? (Color){ 60, 60, 60, 255 } : (Color){ 40, 40, 40, 255 };
    Color stroke = (Color){ 120, 120, 120, 255 };

    DrawRectangle(r.x, r.y, r.w, r.h, fill);
    DrawRectangleLines(r.x, r.y, r.w, r.h, stroke);

    int text_w = MeasureText(label, 30);
    DrawText(label, r.x + (r.w - text_w) / 2, r.y + (r.h - 30) / 2, 30, RAYWHITE);

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
```

- [ ] **Step 3: Use the button in `render_game` and handle transitions in `main.c`**

Modify `render_game` in `src/render.c` so it returns a command. Easier: keep render side-effect-free and do input handling in `main.c`. Replace `render_game` with:

```c
void render_game(const GameState* g) {
    render_background();

    int cell_size = 120;
    int enemy_top_y    = 80;
    int friendly_top_y = WINDOW_HEIGHT - 80 - (cell_size * BOARD_HEIGHT);

    render_board(&g->enemy,    enemy_top_y,    cell_size);
    render_board(&g->friendly, friendly_top_y, cell_size);

    // UI text for scene context
    const char* header = NULL;
    switch (g->scene) {
        case SCENE_SETUP:   header = "FIGHT TO BEGIN";      break;
        case SCENE_COMBAT:  header = "COMBAT";              break;
        case SCENE_RESULT:  header = NULL;                  break;  // handled by result overlay
    }
    if (header) {
        int w = MeasureText(header, 24);
        DrawText(header, (WINDOW_WIDTH - w) / 2, 30, 24, RAYWHITE);
    }
}
```

Now rewrite `src/main.c`:

```c
#include "raylib.h"
#include "game.h"
#include "render.h"

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME01_TITLE);
    SetTargetFPS(60);

    GameState g = game_create();

    while (!WindowShouldClose()) {
        int dt_ms = (int)(GetFrameTime() * 1000.0f);
        game_step(&g, dt_ms);

        BeginDrawing();
        render_game(&g);

        if (g.scene == SCENE_SETUP) {
            if (render_button(FIGHT_BUTTON_RECT, "FIGHT")) {
                game_start_combat(&g);
            }
        } else if (g.scene == SCENE_RESULT) {
            if (render_button(RESET_BUTTON_RECT, "FIGHT AGAIN")) {
                game_reset_to_setup(&g);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

- [ ] **Step 4: Build and play through**

```bash
cmake --build --preset debug
build/debug/game01.exe
```
Expected: Window opens. FIGHT button at bottom. Clicking FIGHT visibly starts combat — units' HP bars drain over 5-15 seconds. When one side is wiped, FIGHT AGAIN button appears. Clicking it resets units and returns to setup.

- [ ] **Step 5: Visual/UX sanity checklist**

Mark each done:
- [ ] FIGHT button visible, hover feedback works, click starts combat
- [ ] HP bars visibly drain during combat
- [ ] Dead units disappear (or at least show 0 HP)
- [ ] Combat resolves in a reasonable time (5-20 seconds)
- [ ] FIGHT AGAIN resets and is clickable again

- [ ] **Step 6: Commit**

```bash
git add src/render.h src/render.c src/main.c
git commit -m "feat: add FIGHT/FIGHT AGAIN button and combat playthrough flow"
```

---

## Task 10: Result Overlay (Win / Lose / Draw)

Goal: When combat ends, show a centered banner declaring the outcome.

**Files:**
- Modify: `src/render.h`, `src/render.c`, `src/main.c`

- [ ] **Step 1: Add `render_result_overlay` to `src/render.h`**

Append to `src/render.h` BEFORE `#endif`:

```c
void render_result_overlay(const GameState* g);
```

- [ ] **Step 2: Implement in `src/render.c`**

Append to `src/render.c`:

```c
#include "combat.h"

void render_result_overlay(const GameState* g) {
    if (g->scene != SCENE_RESULT) return;

    CombatResult r = combat_result(&g->friendly, &g->enemy);
    const char* msg = "DRAW";
    Color tint = (Color){ 220, 220, 220, 255 };

    if (r == COMBAT_FRIENDLY_WIN) {
        msg = "VICTORY";
        tint = (Color){ 80, 230, 140, 255 };
    } else if (r == COMBAT_ENEMY_WIN) {
        msg = "DEFEAT";
        tint = (Color){ 255, 70, 90, 255 };
    }

    // Darken background
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                  (Color){ 0, 0, 0, 160 });

    int font_size = 60;
    int w = MeasureText(msg, font_size);
    DrawText(msg, (WINDOW_WIDTH - w) / 2, WINDOW_HEIGHT / 2 - font_size,
             font_size, tint);
}
```

- [ ] **Step 3: Call overlay from `src/main.c`**

In `src/main.c`, in the `BeginDrawing()` block, add the overlay call BEFORE the result button:

```c
        BeginDrawing();
        render_game(&g);

        render_result_overlay(&g);

        if (g.scene == SCENE_SETUP) {
            if (render_button(FIGHT_BUTTON_RECT, "FIGHT")) {
                game_start_combat(&g);
            }
        } else if (g.scene == SCENE_RESULT) {
            if (render_button(RESET_BUTTON_RECT, "FIGHT AGAIN")) {
                game_reset_to_setup(&g);
            }
        }

        EndDrawing();
```

- [ ] **Step 4: Build and play through to each outcome**

```bash
cmake --build --preset debug
build/debug/game01.exe
```
Play 3-5 times. The hardcoded teams from Task 6 should produce a mix of outcomes — at least once see VICTORY, at least once see DEFEAT. (DRAW is unlikely but possible.)

- [ ] **Step 5: Commit**

```bash
git add src/render.h src/render.c src/main.c
git commit -m "feat(render): add victory/defeat result overlay"
```

---

## Task 11: Verify Android Build and Install

Goal: Confirm the full multi-file build works on Android end-to-end. This is the moment of truth for Phase 1.

**Files:**
- Verify: `src/Makefile.Android` (already updated in Task 1)

- [ ] **Step 1: Build the Android APK using the existing automation**

```bash
./android.bat
```
Expected: APK builds successfully using the updated `PROJECT_SOURCE_FILES` list. Look for `BUILD OK` or equivalent in output.

- [ ] **Step 2: If build fails on a missing symbol or file path, troubleshoot**

Most likely cause: `PROJECT_SOURCE_FILES` in `src/Makefile.Android` line 63 is not pointing at the right files, or a header path wasn't added to the Android build. If so, verify it includes: `main.c unit.c board.c combat.c render.c game.c`.

If the build asks for a path like `src/game01.h` — it already works because all new headers live in the same `src/` directory as `main.c`.

If test_runner accidentally got pulled into the Android build, verify only desktop CMake references `test/` — Makefile.Android should not.

- [ ] **Step 3: Install and run on device**

```bash
./android.bat
```
(Running again with a device plugged in will install and launch the APK per your existing automation.)

Expected: APK installs, app launches on device, you see the portrait window with two boards, FIGHT button, and the combat playable end-to-end.

- [ ] **Step 4: On-device sanity checklist**

- [ ] App launches without crash
- [ ] Portrait orientation is correct (not sideways)
- [ ] Both boards visible
- [ ] Shapes and colors render correctly
- [ ] Tapping FIGHT starts combat
- [ ] Result overlay appears
- [ ] FIGHT AGAIN resets
- [ ] Touch input registers on the button area

- [ ] **Step 5: If orientation is wrong, fix AndroidManifest.xml**

If the app rotates with the phone or starts in landscape, edit `src/android.game01/AndroidManifest.xml` — find the `<activity>` tag and add/verify:

```xml
android:screenOrientation="portrait"
```

Then rebuild with `./android.bat`.

- [ ] **Step 6: Commit any manifest or Makefile.Android changes**

```bash
git add src/android.game01/AndroidManifest.xml src/Makefile.Android
git commit -m "chore(android): lock to portrait and wire multi-file build"
```

(Skip this step if no changes were needed.)

---

## Task 12: Phase 1 Wrap-Up Commit

Goal: Close the phase with a marker commit so Phase 2 starts from a known-good state.

- [ ] **Step 1: Final test + build sanity pass**

```bash
cmake --build --preset debug --target test_runner
build/debug/test_runner.exe
cmake --build --preset debug
build/debug/game01.exe
```

Expected: all tests pass (should be 14), game plays end-to-end.

- [ ] **Step 2: Tag the phase**

```bash
git tag phase-1-complete
```

- [ ] **Step 3: Update memory**

Write to the auto-memory system a project memory noting: "Phase 1 of grayfi5h auto-battler complete on 2026-04-24 (or current date). Playable vertical slice: 3x3 portrait board, hardcoded teams, deterministic combat simulator, flat 2D shape rendering, Android build verified. Next: Phase 2 (Shop & Economy)."

---

## Self-Review Notes

Spec coverage check against `docs/superpowers/specs/2026-04-24-grayfi5h-autobattler-design.md`:

- §2 Core loop (shop → combat → resolution): **combat phase covered**. Shop + resolution are Phase 2/3.
- §3 Unit system (5 shapes × 5 colors): **covered**. Synergies deliberately deferred to Phase 4.
- §4 Run structure: **deferred to Phase 3** — Phase 1 has just one combat, no rounds/HP/bosses.
- §5 Board & controls (3×3 portrait): **covered**.
- §6 Economy: **deferred to Phase 2**.
- §7 Retention: **deferred to Phase 5**.
- §8 Monetization: **deferred to Phase 7**.
- §9 Visual identity: partially covered — flat primitives + grayfi5h background in Phase 1. Shader polish (glow, particles, motion) is Phase 6.
- §10 Technical architecture: save-file, AdMob, scene system deferred. Scene *stub* is in Phase 1.

Placeholder scan: no TBDs, TODOs, or vague steps in the 12 tasks. Every code block is complete. Every command has expected output.

Type consistency:
- `Unit` struct defined in Task 3 (`src/game01.h`) with fields `shape, color, stars, hp, max_hp, attack, attack_cooldown_ms, cooldown_remaining_ms`
- Used consistently in Tasks 4-10
- `Board` struct defined in Task 4 with `cells[BOARD_CELLS]` and `occupied[BOARD_CELLS]`
- Used consistently in Tasks 5-10
- Function names: `unit_create` / `unit_take_damage` / `unit_is_alive` / `board_create` / `board_place_unit` / `board_remove_unit` / `board_get_unit` / `board_count_living` / `board_is_defeated` / `combat_step` / `combat_result` — consistent naming convention (snake_case, module prefix) throughout

Ambiguity check: `combat_step` is defined as "advance by dt_ms" — tests pin down exact cooldown behavior. No other step is open to multiple interpretations.

---

## End of Phase 1 Plan
