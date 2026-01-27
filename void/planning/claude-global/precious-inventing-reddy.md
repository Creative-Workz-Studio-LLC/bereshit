# Cornerstone Architecture Upgrade

## Overview

Combined upgrade of Display System + Scene Orchestrator:

1. **Display System (Phases 1-6)**: Input extraction, modes, DAR, menu bar
2. **Scene Orchestrator (Phases 7-9)**: Extract monolithic scene.c into services

**The Problem:** scene.c (~3500 lines) contains ALL scene logic inline, duplicating services that already exist. This causes the "dialogue skipping to combat" bug.

**The Solution:** scene.c becomes thin orchestrator (~500 lines), services drive gameplay.

---

## Architecture Vision

```
┌─────────────────────────────────────────────────────────────┐
│                    SCENE (Orchestrator)                      │
├─────────────────────────────────────────────────────────────┤
│  Engine-level scenes only:                                   │
│  ├── title.c      (splash, game selection)                  │
│  ├── pause.c      (overlay)                                 │
│  └── demo_complete.c (endings)                              │
├─────────────────────────────────────────────────────────────┤
│  Gameplay via Services:                                      │
│  ├── DialogueService (owns ALL dialogue logic)              │
│  ├── CombatService (owns ALL combat logic)                  │
│  └── Event-driven chaining via orchestrator                 │
├─────────────────────────────────────────────────────────────┤
│  Display (output-only):                                      │
│  ├── modes/ (cli, tui, gui)                                 │
│  ├── DAR panel                                              │
│  └── Menu bar                                               │
└─────────────────────────────────────────────────────────────┘
```

---

## Display System Phases (1-6)

### Phase 1: Input Extraction ✓ COMPLETE
### Phase 2: Directory Reorganization ✓ COMPLETE
### Phase 3: Scene Stacking ✓ COMPLETE
### Phase 4: DAR Panel Integration ✓ COMPLETE

### Phase 5: Menu Bar Integration (PENDING)

> **Note:** Phase 9 (Bug Fix) was completed ahead of Phases 5-8. The dialogue service
> now correctly progresses through all lines. Two bugs were fixed:
> 1. Input clearing for service activation (cornerstone.c) - prevents ENTER from
>    title screen from immediately advancing dialogue
> 2. State re-read after typewriter update (dialogue.c) - prevents stale state
>    from causing premature INACTIVE transition

**Goal:** Add menu bar as background service

**Files:**
- `engine/include/framework/service/menubar.h` (create)
- `engine/src/framework/service/menubar.c` (create)
- `cornerstone.c` (register service)

**Verification:**
```bash
./cornerstone --gui      # Menu bar visible at top
./cornerstone --tui      # Menu bar visible at top
```

### Phase 6: Final Display Integration (PENDING)

**Goal:** Ensure all display modes unified

**Verification:**
```bash
make test        # All display tests pass
```

---

## Scene Orchestrator Phases (7-9)

### Phase 7: Scene Logic Extraction

**Goal:** Remove duplicate dialogue/combat logic from scene.c

**Current scene.c (~3500 lines):**
| Component | Lines | Action |
|-----------|-------|--------|
| Title scene | ~330 | KEEP |
| Dialogue scene | ~1100 | REMOVE (use DialogueService) |
| Combat scene | ~650 | REMOVE (use CombatService) |
| Demo complete | ~140 | KEEP |
| JSONC helpers | ~220 | REMOVE (duplicate) |
| Transitions | ~150 | REFACTOR |

**Step 7.1: Remove JSONC Helpers**
- Remove `strip_jsonc_comments()`, `parse_json_string()`, etc.
- Already exists in dialogue.c

**Step 7.2: Remove Dialogue Scene Callbacks**
- Remove `scene_dialogue_init/update/render/destroy`
- When SCENE_DIALOGUE requested, push DialogueService instead

**Step 7.3: Remove Combat Scene Callbacks**
- Remove `combat_scene_init/update/render/destroy`
- When SCENE_COMBAT requested, push CombatService instead

**Step 7.4: Remove Duplicate State**
- Remove: `scene_dialogue`, `intro_script`, `current_line_index`
- Remove: `scene_combat`, `scene_battle`, `player_hp`, `enemy_hp`
- Services own their state

**Files to Modify:**
```
engine/src/framework/scene.c          # Major reduction (~2000 lines removed)
engine/include/framework/scene.h      # Simplify API
```

**Verification:**
```bash
./cornerstone --gui   # Dialogue loads via DialogueService
                      # Combat loads via CombatService
```

---

### Phase 8: Event-Driven Flow

**Goal:** Services chain via events, not hardcoded transitions

**Current (problematic):**
```
scene.c → load_current_line() → scene_transition_to(SCENE_COMBAT)
```

**Target (event-driven):**
```
DialogueService.advance_line()
  → emit DIALOGUE_EVENT_COMPLETE {has_encounter, encounter_name}
  → CombatService receives (subscribed)
  → orchestrator_push(combat_service)
```

**Step 8.1: Manifest Queries in Services**
- DialogueService gets path via `dialogue_service_set_path()`
- CombatService gets encounter via `combat_service_set_encounter()`

**Step 8.2: Story Complete Event**
- DialogueService emits `DIALOGUE_EVENT_STORY_COMPLETE`
- Scene subscribes, transitions to SCENE_DEMO_COMPLETE

**Files to Modify:**
```
engine/src/framework/service/dialogue.c   # Event emission
engine/src/framework/service/combat.c     # Event subscription
```

---

### Phase 9: Bug Fix Verification ✓ COMPLETE

**Goal:** Verify "dialogue skipping to combat" is fixed

**Test Flow:**
1. Title → ENTER → DialogueService activates ✓
2. Dialogue typewriter works ✓
3. Dialogue progresses through all 16 lines ✓
4. Act advances after dialogue complete ✓

**Bugs Fixed:**
1. **Input clearing for service activation** (cornerstone.c:1527-1531):
   - When `scene_is_transitioning()` is true, input is cleared before `orchestrator_update()`
   - Prevents ENTER that triggered TITLE→DIALOGUE from immediately advancing dialogue

2. **State re-read after typewriter update** (dialogue.c service:335-338):
   - After `dialogue_update()` runs, `dstate` is re-read to get current state
   - Prevents input handling from using stale state (TYPING) when actual state is WAITING
   - Without this fix, `dialogue_advance()` would be called on what it thought was TYPING
     but was actually WAITING, causing premature INACTIVE transition

**Remaining for full verification:**
- Choices navigate correctly (needs testing with branching dialogue)
- Combat chain (needs dialogue with encounter)
- Resolution dialogue (needs combat completion)

---

## Implementation Order

| Phase | Task | Status |
|-------|------|--------|
| 5 | Menu bar integration | PENDING |
| 6 | Final display integration | PENDING |
| 7 | Scene logic extraction | PENDING |
| 8 | Event-driven flow | PENDING |
| 9 | Bug fix verification | ✓ PARTIAL (linear dialogue works) |

---

## Target scene.c Structure (~500 lines)

```c
// METADATA (125 lines)
// SETUP (75 lines)
//   - Includes, Scene struct, static vars (no dialogue/combat state)
// BODY (250 lines)
//   - title_init/update/render/destroy
//   - demo_complete_init/update/render/destroy
//   - pause_init/update/render/destroy
//   - modal_init/update/render/destroy
//   - transition helpers
//   - scene_change() → delegates to services for gameplay
//   - REPL accessor delegation
// CLOSING (50 lines)
```

---

## Critical Files

| File | Role |
|------|------|
| `engine/src/framework/scene.c` | Refactor from 3500→500 lines |
| `engine/src/framework/service/dialogue.c` | Primary dialogue logic |
| `engine/src/framework/service/combat.c` | Primary combat logic |
| `engine/src/kernel/service/orchestrator.c` | Event bus, service stack |

---

## Verification Commands

```bash
# Build and test
make clean && make -j$(nproc) && make test

# Full playthrough (cornerstone is the ENGINE, loads games)
./cornerstone --gui

# REPL test
./cornerstone --cli --repl
> state
> advance
```
