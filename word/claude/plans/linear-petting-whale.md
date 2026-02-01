# Cornerstone Engine — Master Infrastructure Plan

> *"In the beginning God created the heaven and the earth."* — Genesis 1:1

**Purpose:** Comprehensive plan for Cornerstone engine infrastructure, organization, and future development

---

## Vision

**Cornerstone = Kingdom Technology Game Engine → IDE → OS Layer**

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                             MILLENNIUMOS (Future)                            │
│   OmniCode + CPI-SI integrated into OS — bootable, Kingdom-honoring         │
└──────────────────────────────────────────────────────────────────────────────┘
                                           │ runs on
┌──────────────────────────────────────────▼──────────────────────────────────┐
│                             IDE / EDITOR LAYER                               │
│   Visual editing → Asset management → Live preview → Code generation         │
└──────────────────────────────────────────────────────────────────────────────┘
                                           │ builds with
┌──────────────────────────────────────────▼──────────────────────────────────┐
│                          CORNERSTONE ENGINE                                  │
│   Kernel → HAL → Framework → App → Entry (5-stage boot)                      │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Master Status

| Phase | State | Description |
|-------|-------|-------------|
| A | ⬜ ACTIVE | Kernel-like header ladder — engine/include/ reorganization |
| B | ⬜ PENDING | IDE / Editor Layer integration |
| C | ⬜ PENDING | Core Game Loop completion |
| D | ⬜ PENDING | Service Migration (UI abstraction) |
| E | ⬜ FUTURE | MillenniumOS foundation |

**Current Focus: Phase A** — Kernel-like header ladder

---

# PHASE A: Kernel-like Header Ladder

## WHY THIS REDESIGN

### Current Problems
1. **Mixed concerns** - cpisi/ contains both orchestrator (cpisi.h) and implementation (dar/)
2. **Unclear public interface** - cpisi.h is inside kernel/cpisi/, not at kernel/ level
3. **Confusing naming** - services/ at root vs kernel/service/
4. **Fragile dependencies** - json.h uses ../../../../../ path traversal
5. **No boot sequence clarity** - what loads first?

### The Goal
- Each layer only depends on layers below
- Public interfaces separated from implementation
- Binary distribution at any layer without source leakage
- File names, roles, and positions immediately clear
- Clear kernel-services link

---

## PROPOSED STRUCTURE

```
engine/include/
│
├── types.h                     ← LAYER 0: FOUNDATION (single entry point)
│                                  Public: #include "types.h"
│
├── kernel/                     ← LAYER 1: KERNEL CORE
│   ├── types/                  ← Type definitions (internal structure)
│   │   ├── types.h             ← Master aggregator (includes all below)
│   │   ├── core/               ← Primitive types
│   │   │   ├── flags.h
│   │   │   ├── layer.h
│   │   │   └── timing.h
│   │   ├── config/             ← Configuration types
│   │   │   ├── domain.h
│   │   │   ├── error.h
│   │   │   └── log.h
│   │   └── composite/          ← Complex types
│   │       ├── health.h
│   │       ├── metadata.h
│   │       └── phase.h
│   │
│   ├── dar/                    ← DAR SYSTEM (witness/debug - FOUNDATION)
│   │   ├── dar.h               ← DAR aggregator (public for DAR users)
│   │   ├── detect.h            ← Witness/log
│   │   ├── assess.h            ← Evaluate/debug
│   │   ├── restore.h           ← Recover/fix
│   │   └── rotation.h          ← Log rotation
│   │
│   ├── cpisi.h                 ← CPISI PUBLIC INTERFACE (at kernel/ level!)
│   │                              This is THE public orchestrator API
│   │
│   ├── cpisi/                  ← CPISI IMPLEMENTATION (internal)
│   │   ├── config.h            ← CPISI configuration
│   │   ├── clock.h             ← Internal clock
│   │   └── json.h              ← JSON output
│   │
│   ├── ipc/                    ← IPC SYSTEM
│   │   ├── ipc.h               ← IPC aggregator (public interface)
│   │   ├── server.h
│   │   ├── client.h
│   │   ├── logic.h
│   │   ├── pty.h
│   │   └── vterm.h
│   │
│   ├── memory/
│   │   └── resource.h
│   │
│   ├── platform/
│   │   └── platform.h
│   │
│   ├── subsystem/
│   │   └── asset_manager.h
│   │
│   └── service/                ← KERNEL SERVICE PATTERN
│       ├── service.h           ← Service interface
│       ├── orchestrator.h      ← Service manager
│       ├── data_service.h      ← Data pattern
│       └── cpisi_io_service.h  ← I/O tracking
│
├── hal/                        ← LAYER 2: HARDWARE ABSTRACTION
│   ├── hal.h                   ← HAL aggregator (public interface)
│   ├── display/
│   ├── audio/
│   └── input/
│
├── services/                   ← LAYER 3: SYSTEM SERVICES
│   ├── services.h              ← Aggregator (optional)
│   ├── db/                     ← Database services (grouped)
│   │   ├── database.h          ← Core DB interface
│   │   ├── session_db.h        ← Session persistence
│   │   └── game_db.h           ← Game metadata
│   ├── session.h               ← Session state (uses db/session_db.h)
│   └── config.h                ← Engine config (renamed from engine_config.h)
│
├── framework/                  ← LAYER 4: ENGINE FRAMEWORK
│   ├── framework.h             ← Framework aggregator (optional)
│   ├── core/                   ← Core framework (flattened)
│   │   ├── engine.h            ← Was engine/engine.h
│   │   ├── game.h              ← Was engine/game.h
│   │   ├── game_interface.h
│   │   ├── scene.h             ← Was at framework/ root
│   │   └── loading.h           ← Was at framework/ root
│   │
│   ├── graphics/               ← Graphics subsystem
│   │   ├── graphics.h          ← Aggregator
│   │   ├── render/             ← Rendering
│   │   ├── sprite/             ← Sprites
│   │   ├── font/               ← Fonts
│   │   └── tileset/            ← Tilesets
│   │
│   ├── ui/                     ← UI subsystem
│   │   ├── ui.h                ← Aggregator (already exists)
│   │   ├── menu.h, dialog.h, toast.h, etc.
│   │   └── ...
│   │
│   ├── service/                ← Scene-as-service (GROUPED by domain)
│   │   ├── service.h           ← Aggregator
│   │   ├── scene/              ← Scene services (foreground stack)
│   │   │   ├── dialogue.h, combat.h, pause.h, menu.h, ...
│   │   │   └── title.h, startup.h, loading_service.h
│   │   ├── data/               ← Data services (background)
│   │   │   ├── character_service.h, item_service.h
│   │   │   └── party_service.h, action_service.h
│   │   └── system/             ← System services
│   │       ├── menu_bar_service.h, mode_service.h
│   │       └── dar_panel_service.h, error_service.h
│   │
│   ├── util/                   ← Utilities (flattened where possible)
│   │   ├── util.h              ← Aggregator (already exists)
│   │   ├── format/             ← Format utilities
│   │   ├── fs/                 ← Filesystem utilities
│   │   └── console/            ← Console utilities
│   │
│   └── map/                    ← Map system
│       └── tilemap.h
│
└── app/                        ← LAYER 5: APPLICATION
    └── game/                   ← Game-specific (flattened domains)
        ├── game.h              ← Game aggregator (optional)
        ├── character/          ← Character domain
        │   ├── player.h, enemy.h, entity.h
        │   └── class.h, affinity.h, karma.h
        ├── combat/             ← Combat domain
        │   ├── combat.h, stats.h
        │   └── ...
        ├── narrative/          ← Narrative domain
        │   └── dialogue.h
        ├── items/              ← Items domain
        │   └── item.h
        ├── terrain/            ← Terrain domain
        │   └── terrain.h
        ├── actions/            ← Actions domain
        │   └── action.h
        └── cognition/          ← Cognition domain
            ├── cognition.h
            └── game_state.h
```

---

## DEPENDENCY LADDER (Strict)

```
LAYER 5: app/*
    ↓ depends on
LAYER 4: framework/*
    ↓ depends on
LAYER 3: services/*
    ↓ depends on
LAYER 2: hal/*
    ↓ depends on
LAYER 1: kernel/* (cpisi.h, dar/, ipc/, service/, etc.)
    ↓ depends on
LAYER 0: types.h (root)
    ↓ depends on
SYSTEM HEADERS ONLY
```

**RULE:** Higher layers NEVER depend on lower layers. If they need to communicate upward, use callbacks or events.

---

## KEY CHANGES

### 1. Foundation at Root Level
**Before:** `#include "kernel/types/types.h"`
**After:** `#include "types.h"` (root-level entry point)

The root types.h simply includes kernel/types/types.h, providing a clean single entry point.

### 2. DAR Moved to kernel/ Level (Peer to cpisi/)
**Before:** `kernel/cpisi/dar/` (DAR under cpisi)
**After:** `kernel/dar/` (DAR as peer)

DAR is the witness/debug foundation. CPISI orchestrates but doesn't own DAR.

### 3. cpisi.h at kernel/ Level
**Before:** `kernel/cpisi/cpisi.h` (inside implementation folder)
**After:** `kernel/cpisi.h` (at kernel level as PUBLIC interface)

Implementation stays in `kernel/cpisi/` (config.h, clock.h, json.h).

### 4. Clear Layer Boundaries

| Layer | Directory | Public Interface | Description |
|-------|-----------|------------------|-------------|
| 0 | types.h (root) | `#include "types.h"` | Foundation types |
| 1 | kernel/ | `#include "kernel/cpisi.h"` | Kernel core |
| 2 | hal/ | `#include "hal/hal.h"` | Hardware abstraction |
| 3 | services/ | Direct includes | Persistence layer |
| 4 | framework/ | `#include "framework/scene.h"` | Engine framework |
| 5 | app/ | Direct includes | Application code |

---

## FLATNESS VS NESTEDNESS BALANCE

### Principles

| Principle | Flat | Nested | Balance |
|-----------|------|--------|---------|
| **Findability** | Easy to scan | Can get buried | Max 3 levels deep |
| **Grouping** | Everything visible | Logical domains | Group by domain |
| **Includes** | Long paths | Short paths | Use aggregators |
| **Growth** | Gets cluttered | Stays organized | Split at 8-10 files |

### Rules Applied

1. **Max 3 levels** from include/ root
   - `include/kernel/types/core/flags.h` = 3 levels (ok)
   - `include/kernel/types/core/sub/x.h` = 4 levels (too deep)

2. **Split folders at 8-10 files**
   - framework/service/ has 21 files → split into scene/, data/, system/
   - app/game/ has 7 domains → keep flat at game/ level

3. **Aggregator headers reduce complexity**
   - `#include "framework/graphics/graphics.h"` instead of 5 separate includes
   - `#include "hal/hal.h"` instead of display + audio + input

4. **Domain grouping over type grouping**
   - Group by WHAT it does (db/, scene/, data/)
   - Not by HOW it's implemented (interfaces/, impl/, utils/)

---

## INCLUDE PATTERN FOR EACH LAYER

### Layer 0: Foundation
```c
// types.h (at root)
#ifndef CORNERSTONE_TYPES_H
#define CORNERSTONE_TYPES_H
#include "kernel/types/types.h"  // Delegate to implementation
#endif
```

### Layer 1: Kernel
```c
// kernel/cpisi.h (public orchestrator)
#ifndef KERNEL_CPISI_H
#define KERNEL_CPISI_H
#include "types.h"               // Foundation (at root)
#include "kernel/dar/dar.h"      // DAR system (peer)
#include "kernel/cpisi/config.h" // Internal implementation
#include "kernel/cpisi/clock.h"
#include "kernel/cpisi/json.h"
// ... orchestrator declarations
#endif
```

### Layer 2: HAL
```c
// hal/hal.h (public HAL interface)
#ifndef HAL_H
#define HAL_H
#include "types.h"                // Foundation
#include "hal/display/display.h"
#include "hal/audio/audio.h"
#include "hal/input/input.h"
#endif
```

---

## EXECUTION PHASES (Phase A)

### A.1: DAR Reorganization
1. Move `kernel/cpisi/dar/` to `kernel/dar/`
2. Update all includes from `kernel/cpisi/dar/*.h` to `kernel/dar/*.h`
3. Update cpisi.h to include `kernel/dar/dar.h`

### A.2: cpisi.h Promotion
1. Move `kernel/cpisi/cpisi.h` to `kernel/cpisi.h`
2. Keep implementation files in `kernel/cpisi/` (config.h, clock.h, json.h)
3. Update all includes from `kernel/cpisi/cpisi.h` to `kernel/cpisi.h`

### A.3: Foundation Entry Point
1. Create `include/types.h` that includes `kernel/types/types.h`
2. Update files to use `#include "types.h"` for foundation

### A.4: Services Reorganization
1. Create `services/db/` directory
2. Move database.h, session_db.h, game_db.h to `services/db/`
3. Rename `engine_config.h` to `config.h`
4. Update all includes

### A.5: Framework Core Reorganization
1. Create `framework/core/` directory
2. Move `framework/engine/*.h` to `framework/core/`
3. Move `framework/scene.h`, `framework/loading.h` to `framework/core/`
4. Update all includes

### A.6: Framework Service Grouping
1. Create `framework/service/{scene,data,system}/` directories
2. Move scene services (dialogue, combat, etc.) to `framework/service/scene/`
3. Move data services (character_service, item_service) to `framework/service/data/`
4. Move system services (menu_bar_service, etc.) to `framework/service/system/`
5. Update all includes

### A.7: Fix Fragile Dependencies
1. Fix `json.h` relative path to omni (move omni_cpisi.h or use proper include path)
2. Decouple `ipc/logic.h` from full `cpisi.h` (use forward declarations)
3. Add `ipc/ipc.h` aggregator
4. Add `hal/hal.h` aggregator

### A.8: Aggregator Headers
1. Create `framework/graphics/graphics.h` aggregator
2. Create `framework/service/service.h` aggregator
3. Create `services/services.h` aggregator (optional)

### A.9: Build Verification
1. Run `make clean && make`
2. Check for circular dependencies
3. Verify layer discipline (no upward includes)

---

## FILES TO MOVE (Phase A)

| From | To |
|------|-----|
| `kernel/cpisi/dar/*.h` | `kernel/dar/*.h` |
| `kernel/cpisi/cpisi.h` | `kernel/cpisi.h` |
| `services/{database,session_db,game_db}.h` | `services/db/` |
| `services/engine_config.h` | `services/config.h` |
| `framework/engine/*.h` | `framework/core/` |
| `framework/scene.h` | `framework/core/scene.h` |
| `framework/loading.h` | `framework/core/loading.h` |
| `framework/service/{dialogue,combat,...}.h` | `framework/service/scene/` |
| `framework/service/{character,item,party}_service.h` | `framework/service/data/` |
| `framework/service/{menu_bar,mode,dar_panel}_service.h` | `framework/service/system/` |

---

## BINARY DISTRIBUTION LAYERS

With this structure, we can distribute:

| Distribution | What's Exposed | What's Hidden |
|--------------|----------------|---------------|
| **types.h only** | Foundation types | Everything else |
| **kernel/** | Kernel API | HAL, framework, app |
| **kernel/ + hal/** | Hardware API | Framework, app |
| **Full headers** | Everything | Nothing |

---

# PHASE B: IDE / Editor Layer

## B.1: Editor Service Integration
**Current:** `editor_service.c` exists but not fully wired.
**Goal:** Visual game editing within the engine.

```
┌─────────────────────────────────────────────────────────────────┐
│                         EDITOR MODE                              │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────────────┐  ┌─────────────────┐  │
│  │  Asset      │  │    Scene View       │  │   Properties    │  │
│  │  Browser    │  │    (live preview)   │  │   Panel         │  │
│  │             │  │                     │  │                 │  │
│  │  - Sprites  │  │   [Game running]    │  │  - Entity       │  │
│  │  - Tiles    │  │                     │  │  - Transform    │  │
│  │  - Fonts    │  │                     │  │  - Sprite       │  │
│  │  - Audio    │  │                     │  │  - Collision    │  │
│  └─────────────┘  └─────────────────────┘  └─────────────────┘  │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                    Timeline / Console                     │   │
│  │  [DAR Witness]  [Log Output]  [Performance]              │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

### Components Needed
| Widget | Purpose | Status |
|--------|---------|--------|
| Asset Browser | Tree view of assets/ | TreeView exists |
| Scene Editor | Visual entity placement | Pending |
| Properties Panel | Entity inspector | Pending |
| DAR Witness Panel | Real-time log view | Pending |

---

# PHASE C: Core Game Loop

## C.1: Story Branching
**Problem:** Flags set, triggers found, but conditional files never load.
**Location:** `dialogue.c:515-525`

## C.2: Multi-Party Combat
**Problem:** Combat is 1v1. Resonant joins but doesn't fight.
**Fix:** Combat iterates party members.

## C.3: XP and Progression
**Problem:** Full pipeline broken.
**Fix:** Add to CharacterData, parse rewards, apply in combat completion.

## C.4: Equipment System
**Problem:** No equipment system exists.
**Files needed:** `equipment_service.h`, `equipment_service.c`

---

# PHASE D: Service Migration

## Migration Pattern
**Before:**
```c
if (IS_GUI()) {
    renderer_draw_rect(...);
    blockfont_draw_text(&font, "TEXT", ...);
} else {
    display_clear();
    display_print_centered(5, "TEXT");
}
```

**After:**
```c
ui_draw_rect((UIRect){0, 0, w, h}, ui_style_panel());
ui_draw_text_centered(h/2, "TEXT", ui_style_title());
```

## Migration Order
| # | Service | Complexity |
|---|---------|------------|
| 1 | loading_service.c | Simple |
| 2 | modal_service.c | Simple |
| 3 | error_service.c | Simple |
| 4 | choice_service.c | Medium |
| 5 | pause_service.c | Medium |
| 6 | game_menu_service.c | Complex |
| 7 | title_service.c | Complex |
| 8 | dialogue.c | Very Complex |

---

# PHASE E: MillenniumOS (Future)

## E.1: Vision
- Bootable from disc/USB
- OmniCode as system language
- CPI-SI as system intelligence
- Cornerstone as default app framework

## E.2: Boot Sequence
```
BIOS/UEFI → Bootloader → Kernel Init → HAL Init → Framework → Desktop
```

---

# VERIFICATION

## Phase A Verification
```bash
# Build test
make clean && make

# Include chain check
gcc -H -fsyntax-only -I include engine/cornerstone.c 2>&1 | head -100

# Layer violation check (should return empty)
grep -r "#include.*app/" include/kernel/
grep -r "#include.*framework/" include/kernel/
```

## Critical Files (Phase A)
| File | Action |
|------|--------|
| `kernel/cpisi.h` (new location) | Update its own includes |
| `kernel/dar/dar.h` (new location) | Update path references |
| `Makefile` | Update include paths and source file paths |
| `cornerstone.c` | Verify main entry includes work |

---

# DATABASE SCHEMA (Reference)

```sql
-- Sessions table
CREATE TABLE sessions (
  session_id TEXT PRIMARY KEY,
  start_time_ms INTEGER, end_time_ms INTEGER,
  process_id INTEGER,
  initial_health INTEGER, initial_state TEXT,
  final_health INTEGER, final_state TEXT,
  log_path TEXT, log_entry_count INTEGER
);

-- Log entries table (ALL entries queryable)
CREATE TABLE log_entries (
  id INTEGER PRIMARY KEY,
  session_id TEXT, timestamp_ms INTEGER,
  level INTEGER, component TEXT, message TEXT,
  health INTEGER, hebrew_state TEXT, k_factor TEXT,
  cube_x INTEGER, cube_y INTEGER, cube_z INTEGER
);
```

---

*Plan ready for execution — Start with Phase A (Kernel-like Header Ladder)*
