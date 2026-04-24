# grayfi5h Auto-Battler — Design Spec

**Date:** 2026-04-24
**Studio:** grayfi5h studio
**Working title:** TBD (ship-ready name to be chosen before Play Store submission)
**Target platform:** Android (Google Play Store) — v1
**Ship estimate:** 10-12 weeks of focused solo dev
**Engine:** raylib (C), cross-platform build pipeline already operational

---

## 1. Vision

A minimalist, premium-feeling roguelike auto-battler designed to establish grayfi5h studio's visual and quality identity on Play Store. Stack shapes and colors, watch them fight. 15 rounds, 3 bosses, one winner. Each run is ~15 minutes, each run is different, and Ascension mode keeps players coming back for months.

**Core experience pillars:**
- **Shop gambling dopamine** (reroll to chase the 3-star unit)
- **Build-crafting satisfaction** (5 shapes × 5 colors = 25 units with dual-axis synergies)
- **Roguelike run structure** (tower-climb with scripted boss fights)
- **Minimalist-geometric aesthetic** (no sprite art required — pure shapes + colors + glow)

**Inspiration:** Teamfight Tactics (synergy depth), Super Auto Pets (scope/simplicity), Balatro (premium feel, mathematical dopamine), Mini Metro (geometric legibility).

---

## 2. Core Loop (per round)

Each of 15 rounds follows one beat:

1. **Shop phase (15-30 sec)**
   - 5 shop slots populate with units (shape × color combos).
   - Player spends gold to buy units into their bench.
   - Reroll shop for 2g (flat cost).
   - Level up for XP (increases team size + improves shop-tier odds).
   - Drag units from bench onto the 3×3 board; rearrange positions freely.

2. **Combat phase (15-20 sec)**
   - Units auto-fight the enemy's 3×3 board.
   - Front row (closest-to-enemy edge) engages first; back row stays protected.
   - Synergy effects trigger based on active shape counts and color counts.
   - Unit abilities fire as particle effects.

3. **Resolution phase (5 sec)**
   - Win/loss shown; HP damage = 1 + (surviving enemy units on winner's board).
   - Gold awarded: base + win/loss streak bonus (capped at +3g).
   - Next round begins.

**Tier merges:** Owning 3 copies of the same unit (same shape + same color) auto-merges them into a 2-star (visually: bigger, glowing border). 3× 2-stars auto-merge into a 3-star. 3-star units are the run's power peak and the primary shop-gambling chase.

---

## 3. Unit System

### Shape = Class (behavior archetype)

| Shape | Class | Behavior |
|-------|-------|----------|
| ▲ Triangle | Attacker | Front-line melee DPS |
| ■ Square | Tank | High HP, absorbs hits |
| ● Circle | Support | Heals / buffs allies |
| ⬢ Hexagon | Caster | Area spells from back row |
| ◆ Diamond | Assassin | Targets enemy backline |

### Color = Faction (synergy)

| Color | Faction | Synergy effect |
|-------|---------|----------------|
| Red | Berserker | Each allied death powers up remaining units |
| Blue | Frozen | Slows enemies, crowd control |
| Green | Verdant | Healing / life steal |
| Yellow | Volt | Chain damage between same-color units |
| Purple | Void | Bonus damage vs bosses |

**Total pool: 25 unique units** (5 × 5).

Synergies activate at thresholds (called "synergy levels" throughout this spec to avoid confusion with shop tiers and star merges):
- 2-of-a-class or 2-of-a-color = synergy level 1
- 4-of-a-class or 4-of-a-color = synergy level 2 (stronger effect)

A single unit contributes to BOTH its shape synergy and its color synergy simultaneously (e.g., a Red Triangle counts toward both Berserker and Attacker synergies).

### Unlock progression

- **10 units unlocked at game start** (balanced starter roster across all shape types)
- **15 units unlocked via boss kills + milestone achievements** (e.g., "Beat Boss 5 unlocks Diamond shapes", "Win a run with 4-Red active unlocks Purple faction", "3-star any unit unlocks the 3-star visual particle effect")

---

## 4. Run Structure

- **Total rounds:** 15 (3 acts of 5 rounds each)
- **Boss rounds:** rounds 5, 10, 15 (every 5th round is a scripted boss encounter)
- **Act structure:**
  - Act 1 (rounds 1-5): trash mobs + mini-boss at round 5
  - Act 2 (rounds 6-10): harder trash + mid-boss at round 10
  - Act 3 (rounds 11-15): elite trash + final boss at round 15
- **Run-ending conditions:**
  - Lose all 30 HP = run over
  - Beat Boss 15 = run won

### Bosses

Each run contains **3 boss encounters** (one each at rounds 5, 10, 15). These are drawn from a **pool of 12 bosses total** — 4 bosses per act pool, 1 randomly selected per act per run. This creates run-to-run variety: a given player sees a different boss lineup almost every run.

Each boss has ONE scripted gimmick that can't be produced by normal trash encounters. Gimmicks are the marketing screenshots.

**Gimmick examples (full list of 12 to be finalized in implementation plan):**
- **The Splitter** — spawns a new small unit every 3 seconds during combat
- **The Mirror** — copies your strongest unit and fights with it
- **The Drought** — all green-faction synergies are disabled this fight
- **The Eraser** — permanently removes one unit from your bench per round until defeated
- **The Timebomb** — takes no damage for 10 seconds, then explodes
- **The Overcharger** — starts with all synergies pre-activated at synergy level 2

For v1 ship, minimum viable is **9 bosses** (3 per act pool = same lineup every run but with variety in which run you meet which). The remaining 3 can ship in a v1.1 content patch post-launch.

---

## 5. Board & Controls

### Board

- **3×3 grid per side** in portrait orientation
- **9 slots per side** — positioning matters (front row = tanks, back row = squishies)
- **Max team size** grows with level: starts at 3, scales to 7 (leaving 2 slots intentionally open for tactical flexibility)

### Screen layout (portrait)

```
┌─────────────────────┐
│ Enemy HP / Round #  │
├─────────────────────┤
│ Enemy 3×3 board     │
├─────────────────────┤
│ Your 3×3 board      │
├─────────────────────┤
│ Active synergy chips│
├─────────────────────┤
│ Shop (5 slots)      │
│ [Reroll 2g] [Lv up] │
│ Gold: 8   HP: 42    │
└─────────────────────┘
```

### Controls

- **Tap-to-buy** from shop
- **Drag-to-place** from bench onto board
- **Drag-to-swap** between board positions
- **Tap unit** for info popup (stats, synergies contributed)
- **One-handed thumb play** — all primary controls in bottom-half of screen

---

## 6. Economy

All numbers are starting points; final tuning happens during playtesting.

| Parameter | Value |
|-----------|-------|
| Starting HP | 30 |
| Starting gold | 4 |
| Starting team size | 3 |
| Shop reroll cost | 2g |
| Level up costs (cumulative XP) | 4 → 8 → 12 → 20 → 36 → 56 → 80 |
| Win gold | 5g base + 1g/streak (cap +3g) |
| Loss gold | 5g base + 1g/streak (cap +3g) |
| HP damage on loss | 1 + (surviving enemy units) |
| 1-star unit cost | 1g / 2g / 3g (Shop Tier I / II / III units) |

### Shop Tiers

Three shop tiers determine unit power/cost in the shop pool:
- **Shop Tier I** (1g) — weakest base stats, easiest to acquire and 3-star
- **Shop Tier II** (2g) — mid-power base stats
- **Shop Tier III** (3g) — strongest base stats, rarest in shop pool

These shop tiers are independent from **synergy levels** (2/4-of-a-color thresholds) and from **stars** (1-star → 2-star → 3-star merges via 3 duplicates).

### Shop-tier odds (percent chance of each tier per shop slot, by player level)

Level-gated so higher-level players see more Tier II and Tier III units. Standard auto-battler pattern; exact percentages tuned in playtesting.

---

## 7. Retention (Meta-Progression)

### Unit unlocks (A)
- Start: 10 units available
- Unlock 15 more via boss kills + milestone achievements
- Visible unlock tracker on main menu — "3/15 units remaining" progress bar

### Ascension tiers (B)
- 20 tiers, locked initially
- Beat the final boss (round 15) on Asc N to unlock Asc N+1
- Each Ascension adds one stacking modifier (full 20-modifier list finalized in implementation plan). Example modifiers:
  - Asc 1: base game
  - Asc 2: reroll costs 3g (+1)
  - Asc 3: start with 1 less HP
  - Asc 4: shop has 4 slots instead of 5
  - Asc 5: bosses have +50% HP
- Ascension progress visible on main menu as "Current Ascension: 7/20"

### NOT in v1
- No daily challenges
- No leaderboards
- No account / cloud save
- No social features
- No cosmetics / skins / battle pass
- No achievement page beyond unlock progress

---

## 8. Monetization

- **Free-to-play.** No IAP. No paywalls. Full game unlocked from install.
- **App-open ad**
  - AdMob app-open ad unit
  - Fires only if ≥4 hours since last session close
  - Skipped on cold starts <4 seconds (Google policy compliance)
- **Run-end interstitial**
  - AdMob interstitial ad unit
  - Fires after the run-end screen is shown, with a **3-second delay** so player sees their final score/result first
  - No other ad placements — no banner ads, no rewarded ads, no every-round interruptions

**Rationale:** minimal ad load protects the "one more run" loop and the grayfi5h brand. Revenue scales with DAU, not ad frequency. If post-launch data shows insufficient revenue, rewarded ads ("watch ad for +1 reroll this round") can be added in v1.1 as opt-in.

---

## 9. Platform & Visual Identity

- **Android only** for v1. iOS port considered for v2 based on Play Store traction.
- **Portrait only.** No landscape support.
- **Phones only** for v1. Tablet-optimized layout in v1.1+.
- **Target specs:** Android 7.0+ (API 24+) — raylib Android build already working at API 29.
- **Performance target:** 30fps locked, 60fps on capable devices. No animation polish pass planned for v1.
- **Visual aesthetic — 2.5D (flat 2D geometry + shader polish):**
  - Shapes rendered as **flat 2D primitives** (raylib `DrawTriangle`, `DrawCircle`, etc.) — NOT true 3D models. This protects portrait-grid legibility and ship speed.
  - **Fragment shader polish:** soft outer glow, additive color bloom on ability fire, outline-pulse on synergy activation, particle trails on attacks.
  - **Subtle idle animation:** units gently bob, scale-pulse (~±3%), and rotate ±3° to feel alive without breaking the minimalist aesthetic.
  - Dark background (~#1a1a1a base, "grayfi5h gray")
  - Neon-glow geometric units — shape conveys class, color conveys faction, glow intensity conveys star level
  - Crisp sans-serif type (a free web font — Inter or similar)
  - Store listing leans hard into this language to stand out from fantasy-medieval thumbnail glut
  - **Reference points:** Balatro's card polish (motion + glow over realism), Mini Metro's geometric clarity, Luck Be a Landlord's tile juice.
- **Brand strategy:** this title establishes grayfi5h visual language. Games #2 and #3 inherit the same dark-neon-geometric identity, building a recognizable studio label.

---

## 10. Technical Architecture (high level)

Detailed implementation plan will be produced by `writing-plans` skill after this spec is approved. This section flags known architectural decisions only.

- **Engine:** raylib in C (existing setup)
- **Build:** existing CMake desktop build for dev/playtest, existing Makefile.Android build for shipping
- **State:** all local, no backend, no cloud sync, no account. Save file = JSON blob in app-private storage.
- **Scenes:** main menu → run (shop/combat loop) → run-end → back to menu
- **Combat simulation:** deterministic turn-based-but-visualized auto-battle. Fixed timestep for reproducibility.
- **Ads:** AdMob SDK integration via Android native hook (raylib's Android build supports JNI bridges)
- **Asset pipeline:** near-zero (shapes are rendered as flat 2D primitives; colors are palette values; particle effects are procedural). One icon font or SVG set for shape silhouettes at most.
- **Shader pipeline:** 3-5 small fragment shaders for the 2.5D polish layer — glow, bloom, outline-pulse, particle trails. Adapted from raylib's bundled example shaders. GLSL ES 2.0 target for Android compatibility. No compute shaders, no post-processing chains.

---

## 11. Scope Protection (OUT of v1)

To protect the 10-12 week ship estimate, these are explicitly deferred:

| Feature | Defer to |
|---------|----------|
| Multiplayer (any form) | Never — not this game |
| Accounts / cloud save | v2 |
| Leaderboards / daily seed | v1.5 (Firebase free tier) |
| Cosmetics / skins | v2 |
| Battle pass / seasons | Never — not this game |
| IAP / ad removal | Not planned |
| Tutorial cinematic | Replaced with 2-round contextual tutorial |
| Localization | Post-launch, based on download geography |
| Google Play Games Services (achievements) | v1.1 |
| Tablet layout | v1.1 |
| 60fps animation polish pass | Not planned for v1 |
| True 3D models / lighting / cameras | Never — not this game. 2.5D flat-primitives + shader polish is the permanent aesthetic. |
| iOS port | v2 if Play Store traction justifies |

---

## 12. Success Criteria

- **Ship:** submitted to Google Play Store within 12 weeks of kickoff
- **Quality:** passes Google Play pre-launch review on first submission
- **Retention signal:** D1 retention >30%, D7 retention >10% (mobile roguelike benchmarks)
- **Brand signal:** game establishes a recognizable grayfi5h visual language that can carry forward to future titles

---

## 13. Open Questions (to resolve during implementation planning)

- Final game title (working under placeholder until Play Store submission)
- Exact 20-Ascension modifier list
- Exact 12-boss gimmick list
- Exact unlock gate conditions for each of the 15 unlockable units
- Shop-tier odds table by player level
- AdMob account setup + ad unit ID provisioning

These are design-detail decisions that can be finalized as part of the implementation plan without reopening the spec.
