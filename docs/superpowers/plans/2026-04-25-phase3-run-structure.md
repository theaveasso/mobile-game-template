# Phase 3 — Run Structure Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Turn one-off combat into a 15-round run with player HP, round counter, win/loss resolution, rewards, and run-end states.

**Architecture:** Keep run logic in `game.c` so combat remains deterministic and pure. Phase 3 starts with trash-round loop behavior, then adds boss markers for rounds 5/10/15 without boss gimmicks yet. Rendering only displays the state and exposes a continue button after combat.

**Tech Stack:** C11, raylib render/input, existing pure C tests.

---

## Task 1: Run State and Round Resolution

**Files:**
- Modify: `src/game01.h`, `src/game.h`, `src/game.c`, `test/test_game.c`

- [x] Add constants: `STARTING_HP = 30`, `MAX_RUN_ROUNDS = 15`, `WIN_LOSS_GOLD = 5`.
- [x] Add `player_hp`, `round`, `run_won`, `run_lost` to `GameState`.
- [x] Start every run at HP 30 and round 1.
- [x] Add `game_continue_after_result()` to apply result rewards/damage and move to next setup or run end.
- [x] Tests cover win advances round and grants gold, loss damages HP, round 15 win marks run won, HP <= 0 marks run lost.

## Task 2: Preserve Team Across Rounds

**Files:**
- Modify: `src/game.c`, `test/test_game.c`

- [x] Reset enemy board each round.
- [x] Restore living friendly units to full HP before each new combat.
- [x] Keep bench and board ownership between rounds.

## Task 3: Render Round / HP / Continue

**Files:**
- Modify: `src/render.c`, `src/render.h`, `src/main.c`

- [x] Display round and HP in setup/combat.
- [x] Result screen button becomes `CONTINUE` when run is still active.
- [x] Final run end shows `RUN WON` or `RUN LOST`, then reset button returns to new run.

## Task 4: Boss Round Labels

**Files:**
- Modify: `src/game.h`, `src/game.c`, `src/render.c`, `test/test_game.c`

- [x] Add `game_is_boss_round()`.
- [x] Show BOSS label on rounds 5, 10, 15.
- [x] No boss gimmicks yet.

## Task 5: Verification

- [x] Run `.\build.bat debug`.
- [x] Run `.\build\debug\test_runner.exe`.
- [x] Run desktop smoke test.
- [x] Run Android APK build / user build verification.
