# Phase 5 — Player Level / Team Size Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add player level progression so the board starts with a 3-unit cap and grows toward the 7-unit cap defined in the spec.

**Architecture:** Keep level, XP, and cap rules in `game.c` / `game.h` because they are run state, not combat simulation. Keep constants in `game01.h` with the rest of economy tuning. Rendering shows current level/cap and exposes one setup-phase level button; shop tier odds remain deferred to a later phase.

**Tech Stack:** C11, raylib render/input, existing pure C tests.

---

## Task 1: Level State and Team Cap

**Files:**
- Modify: `src/game01.h`, `src/game.h`, `src/game.c`, `test/test_game.c`

- [x] Add constants: `STARTING_PLAYER_LEVEL = 1`, `MAX_PLAYER_LEVEL = 5`, `STARTING_TEAM_SIZE = 3`, `MAX_TEAM_SIZE = 7`.
- [x] Add `player_level` and `xp` to `GameState`.
- [x] Add `game_team_size_cap(const GameState* g)`.
- [x] New runs start at level 1, XP 0, and team cap 3.
- [x] Team cap increases by 1 per level and caps at 7.

## Task 2: XP Purchase and Level Thresholds

**Files:**
- Modify: `src/game01.h`, `src/game.h`, `src/game.c`, `test/test_game.c`

- [x] Add constants: `XP_BUY_COST = 4`, `XP_PER_BUY = 4`.
- [x] Add `game_buy_xp(GameState* g)`.
- [x] Buying XP only works during setup, spends 4 gold, and adds 4 XP.
- [x] Level thresholds use cumulative XP: level 2 at 4, level 3 at 8, level 4 at 12, level 5 at 20.
- [x] Max-level players cannot buy more XP.

## Task 3: Enforce Team Cap on Placement

**Files:**
- Modify: `src/game.c`, `test/test_game.c`

- [x] Placing a bench unit into an empty board cell fails when the board already has `game_team_size_cap(g)` units.
- [x] Swapping a bench unit with an occupied board cell remains allowed because it does not increase board count.
- [x] Existing owned units are preserved when moving to the next round.

## Task 4: Level UI and Button

**Files:**
- Modify: `src/render.h`, `src/render.c`, `src/main.c`

- [x] Display `Lv`, `XP`, and `Team` cap during setup.
- [x] Add a `LV UP` button in the setup action area.
- [x] Hook the button to `game_buy_xp()`.
- [x] Keep the current one-handed shop popup and bench layout intact.

## Task 5: Verification

- [x] Run `.\build.bat debug`.
- [x] Run `.\build\debug\test_runner.exe`.
- [x] Run desktop smoke test.
- [x] Run Android APK build / user Android verification.

## Task 6: Starter Random Player Unit

**Files:**
- Modify: `src/game.c`, `test/test_game.c`

- [x] Start every new run with one random player unit on the friendly board.
- [x] Place the starter unit in the center cell so the player can immediately press `FIGHT`.
- [x] Keep existing round transitions preserving the player's team; only new runs get the starter unit.
- [x] Tests cover one legal friendly starter unit at run creation.

## Task 7: Bench Unit Selling

**Files:**
- Modify: `src/game.h`, `src/game.c`, `src/render.h`, `src/render.c`, `src/main.c`, `test/test_game.c`

- [x] Add `game_sell_selected_bench_unit(GameState* g)`.
- [x] Sell only works in setup phase with a selected occupied bench slot.
- [x] Selling clears the bench slot and resets selection.
- [x] Refund value uses owned copy count: 1-star = 1 copy, 2-star = 3 copies, 3-star = 9 copies.
- [x] Add a one-handed `SELL` button shown when a bench unit is selected.
