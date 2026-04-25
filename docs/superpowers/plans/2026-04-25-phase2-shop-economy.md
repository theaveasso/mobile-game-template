# Phase 2 — Shop & Economy Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add the first playable shop phase: 5 shop slots, starting gold, reroll cost, tap-to-buy, a bench, and simple board placement so the player builds a team before pressing FIGHT.

**Architecture:** Keep shop/economy logic pure C and testable without raylib. `unit` owns unit tier/cost metadata, new `shop` and `bench` modules own shop slots and purchased units, `game` coordinates gold and scene flow, and `render/main` only draw/click UI. Phase 2 deliberately skips XP level-up odds, drag-and-drop polish, star merges, synergies, streaks, and full 15-round run structure; those remain later phases.

**Tech Stack:** C11, raylib for input/render only, existing minimal C test runner, CMake desktop build, existing Android Makefile.

---

## Phase 2 Scope

Included:
- 5 visible shop slots populated with units.
- Starting gold = 4.
- Reroll cost = 2g.
- Tap shop slot to buy if enough gold and bench has room.
- Bench stores purchased units.
- Tap bench slot, then tap friendly board slot to place/swap a unit.
- FIGHT is only available when at least one friendly unit is on board.

Deferred:
- XP/level-up and tier odds by level.
- Star merging.
- Synergies.
- Round rewards, HP damage, streaks.
- Drag-and-drop.
- Save/meta progression.

---

## File Structure

Create:
- `src/shop.h`, `src/shop.c` — 5-slot shop state, deterministic reroll, unit cost lookup.
- `src/bench.h`, `src/bench.c` — fixed-size bench storage and selected bench slot handling.
- `test/test_shop.c`, `test/test_bench.c` — pure logic tests.

Modify:
- `src/game01.h` — add shop tier/cost fields and constants.
- `src/unit.h`, `src/unit.c` — expose unit tier/cost.
- `src/game.h`, `src/game.c` — add gold, shop, bench, selected bench slot, setup-phase actions.
- `src/render.h`, `src/render.c` — draw shop/bench/gold and reusable unit primitive helpers.
- `src/main.c` — handle shop buy, bench selection, board placement, reroll.
- `CMakeLists.txt` — add new logic modules/tests.
- `src/Makefile.Android` — add new source files.

---

## Task 1: Unit Shop Tier and Cost Metadata

**Files:**
- Modify: `src/game01.h`, `src/unit.h`, `src/unit.c`, `test/test_unit.c`

- [x] Add `ShopTier` enum and `shop_tier` / `cost` fields to `Unit`.
- [x] Update `unit_create()` so base units are Tier I and cost 1g.
- [x] Add `unit_create_tiered(UnitShape shape, UnitColor color, ShopTier tier)`.
- [x] Add tests proving Tier I/II/III costs are 1/2/3 and invalid tier falls back to Tier I.

## Task 2: Shop Model

**Files:**
- Create: `src/shop.h`, `src/shop.c`, `test/test_shop.c`
- Modify: `CMakeLists.txt`, `src/Makefile.Android`, `test/test_runner.c`

- [x] Add `SHOP_SLOT_COUNT = 5`.
- [x] Create `Shop` with 5 occupied slots and 5 `Unit` values.
- [x] Implement deterministic `shop_create(seed)` and `shop_reroll(shop)`.
- [x] Tests cover 5 occupied slots, deterministic same-seed output, reroll changes at least one slot, and all slot costs are 1-3g.

## Task 3: Bench Model

**Files:**
- Create: `src/bench.h`, `src/bench.c`, `test/test_bench.c`
- Modify: `CMakeLists.txt`, `src/Makefile.Android`, `test/test_runner.c`

- [x] Add `BENCH_SLOT_COUNT = 6`.
- [x] Implement add/remove/get/first-empty helpers.
- [x] Tests cover empty bench, adding units, full bench rejection, removing units, and preserving unit data.

## Task 4: Game Economy Actions

**Files:**
- Modify: `src/game.h`, `src/game.c`
- Test: add `test/test_game.c`
- Modify: `CMakeLists.txt`, `test/test_runner.c`

- [x] Add `gold`, `shop`, `bench`, `selected_bench_slot` to `GameState`.
- [x] Start setup with 4g, empty board, empty bench, populated shop.
- [x] Implement `game_buy_shop_slot()`, `game_reroll_shop()`, `game_select_bench_slot()`, `game_place_selected_bench_unit()`.
- [x] Tests cover buy cost, insufficient gold, bench full, reroll cost, reroll blocked under 2g, and placing from bench to board.

## Task 5: Setup UI Rendering

**Files:**
- Modify: `src/render.h`, `src/render.c`

- [x] Refactor board layout to make room for shop and bench.
- [x] Draw gold, shop slots, bench slots, reroll button, FIGHT button.
- [x] Highlight selected bench slot.
- [x] Keep combat/result rendering working.

## Task 6: Setup UI Input

**Files:**
- Modify: `src/main.c`

- [x] Tap shop slot buys unit.
- [x] Tap reroll button rerolls.
- [x] Tap bench slot selects it.
- [x] Tap friendly board slot places selected bench unit.
- [x] FIGHT starts combat only when the friendly board has at least one living unit.

## Task 7: Verification

**Files:**
- Verify desktop and Android build.

- [x] Run `.\build.bat debug`.
- [x] Run `.\build\debug\test_runner.exe`.
- [x] Run desktop smoke test.
- [x] Run Android APK build without install.
- [ ] Run Android install when device is available.
- [ ] Manual check: buy, reroll, place, fight, result, reset.
