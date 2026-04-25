# Phase 6 — Mobile Unit Controller Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Improve Android touch control so units can be inspected and later moved with mobile-friendly gestures.

**Architecture:** Keep touch-result state in `GameState` so rendering can show a pure view of the selected/inspected unit. Keep existing tap-to-select and tap-to-place behavior as a fallback while this phase adds richer mobile gestures incrementally. Rendering owns the unit info panel layout only.

**Tech Stack:** C11, raylib render/input, existing pure C tests.

---

## Task 1: Tap Unit Info

**Files:**
- Modify: `src/game.h`, `src/game.c`, `src/render.h`, `src/render.c`, `src/main.c`, `test/test_game.c`

- [x] Add `unit_info_open` and `unit_info` to `GameState`.
- [x] Bench slot tap keeps selecting the bench slot and copies that unit into `unit_info`.
- [x] Friendly board unit tap shows `unit_info` when no bench unit is selected for placement.
- [x] Enemy board unit tap shows `unit_info`.
- [x] Unit info panel displays shape, color, stars, HP, attack, cooldown, tier, and cost.
- [x] Existing buy, sell, level, reroll, fight, and tap-to-place behaviors remain available.
- [x] Unit info panel appears near the top of the screen.
- [x] Tapping outside the unit info panel closes it.
- [x] Shop popup uses the same panel helper as unit info.
- [x] Tapping outside the shop popup closes it without interfering with bottom action buttons.
- [x] Shop popup and unit info popup use the same panel width.
- [x] Opening unit info closes the shop popup so only one popup is open.

## Task 2: Drag From Bench To Board

**Files:**
- Modify later: `src/game.h`, `src/game.c`, `src/render.h`, `src/render.c`, `src/main.c`, `test/test_game.c`

- [x] Track drag origin and current pointer position.
- [x] Drag bench unit onto empty board cell to place.
- [x] Drag bench unit onto occupied board cell to swap.
- [x] Show ghost unit during drag.
- [x] Preserve tap fallback behavior.

## Task 3: Board Drag Move / Swap

**Files:**
- Modify later: `src/game.h`, `src/game.c`, `src/render.h`, `src/render.c`, `src/main.c`, `test/test_game.c`

- [x] Drag board unit to empty board cell to move.
- [x] Drag board unit to occupied board cell to swap.
- [x] Respect team cap when moving from bench to empty board cells.
- [x] Do not allow enemy board edits.

## Task 4: Verification

- [x] Run `.\build.bat debug`.
- [x] Run `.\build\debug\test_runner.exe`.
- [x] Run desktop smoke test.
- [ ] Run Android APK build / user Android verification.
