# Phase 4 — Unit Merge / Stars Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add the core auto-battler chase loop where three identical units merge into a stronger star unit.

**Architecture:** Keep unit stat scaling in `unit.c` so star math is reusable and testable. Keep merge orchestration in `game.c` because it must inspect both owned surfaces: friendly board and bench. Rendering only displays existing unit star state; it does not own merge rules.

**Tech Stack:** C11, raylib render/input, existing pure C tests.

---

## Task 1: Unit Star Promotion

**Files:**
- Modify: `src/unit.h`, `src/unit.c`, `test/test_unit.c`

- [x] Add `unit_promote_star(Unit* unit)` that promotes 1-star to 2-star and 2-star to 3-star.
- [x] Promotion returns `1` when it changes the unit and `0` when the unit is already 3-star.
- [x] Promotion increases `max_hp` and `attack` from the original base unit stats using `stars` as the multiplier.
- [x] Promotion heals the unit to full HP and clears attack cooldown.

## Task 2: Owned Unit Merge Scan

**Files:**
- Modify: `src/game.h`, `src/game.c`, `test/test_game.c`

- [x] Add `game_resolve_unit_merges(GameState* g)`.
- [x] Detect three identical owned units by `shape`, `color`, and `stars`.
- [x] Search both `friendly` board and `bench`.
- [x] Promote one kept unit, remove the other two, and return `1` when a merge happens.
- [x] Return `0` when no merge is available.

## Task 3: Merge After Shop Buy

**Files:**
- Modify: `src/game.c`, `test/test_game.c`

- [x] After `game_buy_shop_slot()` adds a unit to the bench, automatically resolve all possible merges.
- [x] Support chain merge: three 1-stars can become a 2-star, and three 2-stars can become a 3-star in the same resolution pass.
- [x] Keep the shop slot consumed and gold spent only when the buy succeeds.

## Task 4: Star Rendering

**Files:**
- Modify: `src/render.c`

- [x] Show stars on board units and unit cards.
- [x] Use readable text markers (`*`, `**`, `***`) for now; no animation or particle polish yet.
- [x] Keep touch targets and current one-handed shop popup layout unchanged.

## Task 5: Verification

- [x] Run `.\build.bat debug`.
- [x] Run `.\build\debug\test_runner.exe`.
- [x] Run desktop smoke test.
- [x] Run Android APK build / APK contents verification. Device install was blocked by `INSTALL_FAILED_USER_RESTRICTED`.
