# Cornerstone Engine — Master Integration Plan v2.0

## Vision

**Cornerstone = Kingdom Technology Game Engine → IDE → OS Layer**

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                             MILLENNIUMOS (Future)                            │
│   OmniCode + CPI-SI integrated into OS — bootable, Kingdom-honoring         │
└──────────────────────────────────────────┬──────────────────────────────────┘
                                           │ runs on
┌──────────────────────────────────────────▼──────────────────────────────────┐
│                             IDE / EDITOR LAYER                               │
│   Visual editing → Asset management → Live preview → Code generation         │
└──────────────────────────────────────────┬──────────────────────────────────┘
                                           │ builds with
┌──────────────────────────────────────────▼──────────────────────────────────┐
│                          CORNERSTONE ENGINE                                  │
│   Kernel → HAL → Framework → App → Entry (5-stage boot)                      │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

# PROGRESS SUMMARY

## ✅ COMPLETED (This Session)

### Phase 1: Logging Foundation (A1-A4) ✅
| Task | Status | Details |
|------|--------|---------|
| Session ID generation | ✅ Done | `YYYYMMDD-HHMMSS-PID-SEQ` format |
| Per-spawn log files | ✅ Done | `cpisi_SESSIONID.jsonl` |
| Session database | ✅ Done | SQLite with full schema |
| Query API | ✅ Done | `session_db_record_log_entry()` |
| **Expanded JSON format** | ✅ Done | Pretty-printed, readable, queryable |
| **Database log indexing** | ✅ Done | All entries stored in `log_entries` table |

### Infrastructure Fixes ✅
| Fix | Details |
|-----|---------|
| Path buffer sizing | `ASSET_PATH_MAX` → 4096, `safe_path_join()` helper |
| Database path resolution | Uses `engine_config_get_data_dir()` |
| Directory creation | `database.c` creates parent dirs automatically |
| Games directory | `bin/games/` created on install |
| Zero warnings | Clean build achieved |

### Graphics Pipeline ✅
| Task | Status |
|------|--------|
| Sprite system | ✅ `RenderQuad`, animation, batch rendering |
| Renderer upgrade | ✅ `renderer_draw_quad()` with transforms |
| Tilemap rendering | ✅ Uses `renderer_draw_quad()` |
| Asset manager | ✅ Full path handling, caching |

---

## 🔄 PENDING TASKS (Carry Forward)

### From Task List
| ID | Task | Status |
|----|------|--------|
| #4 | Phase 3: Core Game Loop (C1-C4) | Pending |
| #3 | Phase 4: Full Service Migration | Pending |
| #7 | Wire Entity Rendering to Sprites | Pending |
| #9 | Add Graphical UI Widgets | Pending |

---

# PART A: KERNEL ARCHITECTURE REORDER

## Current Boot Stages (Working)
```
Stage 1: KERNEL    → cpisi, service orchestrator, memory, subsystems
Stage 2: HAL       → display (GUI/TUI/CLI), input, clipboard
Stage 3: FRAMEWORK → graphics, ui, services, scene, map
Stage 4: APP       → game logic, combat, dialogue, characters
Stage 5: ENTRY     → cornerstone.c main(), config, databases
```

## Architecture Cleanup Needed

### A1: Kernel Layer Audit
**Problem:** Some framework code depends on kernel internals incorrectly.

**Files to audit:**
- `engine/src/kernel/` — Pure kernel, no framework deps
- `engine/src/framework/` — Should only depend on kernel APIs
- `engine/src/app/` — Should only depend on framework APIs

### A2: Service Dependency Graph
```
KERNEL SERVICES (no UI deps):
├── cpisi_io_service    — CPI-SI REPL interface
├── data_service        — File path resolution
└── orchestrator        — Service lifecycle

FRAMEWORK SERVICES (UI-aware):
├── startup_service     → launcher_service → mode_service
├── title_service       → loading_service
├── game_menu_service   → modal_service → choice_service
├── pause_service       → error_service
├── gameplay_service    → dialogue_service → combat_service
├── character_service   → item_service → action_service
├── party_service       → scene_service
└── editor_service      (IDE integration)

DAR PANEL (overlay):
└── dar_panel_service   — Witness system UI
```

### A3: HAL Abstraction Completion
| Component | Status | Issue |
|-----------|--------|-------|
| Display | ✅ Working | GUI/TUI/CLI backends |
| Input | ✅ Working | Keyboard/mouse abstraction |
| Audio | ❌ Missing | No audio HAL yet |
| Filesystem | ⚠️ Partial | Uses direct libc, needs HAL |

---

# PART B: IDE / EDITOR LAYER

## B1: Editor Service Integration
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

### B1.1: Asset Browser Widget
- Tree view of `assets/` directory
- Preview sprites, tiles, fonts
- Drag-and-drop to scene

### B1.2: Scene Editor
- Visual entity placement
- Transform handles (move, rotate, scale)
- Grid snapping

### B1.3: Properties Panel
- Entity inspector
- Component editing
- Live updates to running game

### B1.4: DAR Witness Panel
- Real-time log view
- Hebrew state visualization
- Cube position display
- Query interface to session database

## B2: UI Widgets Needed (#9)
| Widget | Purpose | Priority |
|--------|---------|----------|
| TreeView | Asset browser, scene hierarchy | ✅ Exists |
| InputField | Property editing | ✅ Exists |
| Slider | Numeric values | ❌ Missing |
| ColorPicker | Color properties | ❌ Missing |
| FileDialog | File selection | ❌ Missing |
| TabBar | Panel switching | ❌ Missing |
| SplitPane | Resizable panels | ❌ Missing |
| Toolbar | Editor tools | ❌ Missing |

## B3: Code Generation
- Export entity definitions as JSONC
- Export tilemap data
- Export dialogue trees
- Generate sprite sheets from editor

---

# PART C: CORE GAME LOOP (Phase 3 - #4)

## C1: Story Branching ⚠️ In Progress
**Problem:** Flags set, triggers found, but conditional files never load.

**Location:** `dialogue.c:515-525`

**Fix:**
```c
if (tc->file[0]) {
    char cond_path[512];
    snprintf(cond_path, sizeof(cond_path), "%s/dialogue/conditional/%s",
             manifest_get_data_path(m), tc->file);
    dialogue_load_conditional(state, cond_path, tc->tag);
}
```

## C2: Multi-Party Combat
**Problem:** Combat is 1v1. Resonant joins but doesn't fight.

**Fix:** Combat iterates party members.
```c
PartyMember* members = party_service_get_active(state->party_svc);
for (int i = 0; i < party_count; i++) {
    if (members[i].active && members[i].hp > 0) {
        combat_execute_ally_turn(state, &members[i]);
    }
}
```

## C3: XP and Progression
**Problem:** Full pipeline broken.

**Fix (3 steps):**
1. Add to CharacterData: `int32_t xp, level, gold, resonance`
2. Parse rewards in character_service
3. Apply rewards in combat completion

## C4: Equipment System
**Problem:** No equipment system exists.

**Solution:** New service for equipment slots and stat modifiers.

**Files:**
- NEW: `engine/include/framework/service/equipment_service.h`
- NEW: `engine/src/framework/service/equipment_service.c`
- NEW: `game/data/service/equipment/*.jsonc`

---

# PART D: SERVICE MIGRATION (Phase 4 - #3)

## Migration Order
| # | Service | Complexity | Status |
|---|---------|------------|--------|
| 1 | loading_service.c | Simple | Pending |
| 2 | modal_service.c | Simple | Pending |
| 3 | error_service.c | Simple | Pending |
| 4 | choice_service.c | Medium | Pending |
| 5 | pause_service.c | Medium | Pending |
| 6 | game_menu_service.c | Complex | Pending |
| 7 | title_service.c | Complex | Pending |
| 8 | startup_service.c | Medium | Pending |
| 9 | launcher_service.c | Medium | Pending |
| 10 | mode_service.c | Complex | Pending |
| 11 | gameplay_service.c | Complex | Pending |
| 12 | dialogue.c | Very Complex | Pending |

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

---

# PART E: OS LAYER (Future)

## E1: MillenniumOS Vision
- Bootable from disc/USB
- OmniCode as system language
- CPI-SI as system intelligence
- Cornerstone as default app framework

## E2: Boot Sequence
```
BIOS/UEFI → Bootloader → Kernel Init → HAL Init → Framework → Desktop
```

## E3: System Services
- Window manager (using Cornerstone renderer)
- File manager (using asset browser)
- Terminal (using CPI-SI REPL)
- Settings (using editor properties panel)

---

# PART F: DEBUG / POLISH PHASE

## F1: Known Issues to Debug
| Issue | Location | Severity |
|-------|----------|----------|
| Session DB not creating | cornerstone.c:5253 | Medium |
| Games directory scan | discovery.c | Low (fixed) |
| Menu navigation speed | Fixed this session | Done |

## F2: Performance Profiling
- Frame time logging
- Memory usage tracking
- Database query timing
- Asset load timing

## F3: Test Coverage
- Unit tests for kernel services
- Integration tests for service orchestration
- E2E tests for game flow

---

# IMPLEMENTATION ORDER

## Immediate Next Session
1. **IDE Integration** — Wire editor_service fully
2. **UI Widgets** — Add missing widgets for editor
3. **DAR Panel** — Real-time witness display

## Following Sessions
4. **Core Game Loop** — C1-C4 completion
5. **Service Migration** — All 12 services
6. **Entity Rendering** — Wire to sprites (#7)

## Future
7. **OS Layer** — MillenniumOS foundation
8. **Full Debug** — End-to-end testing
9. **Polish** — Performance, UX, stability

---

# CRITICAL FILES

## Kernel Layer
| File | Purpose |
|------|---------|
| `engine/src/kernel/cpisi/dar/detect.c` | DAR logging (expanded JSON) |
| `engine/src/kernel/service/orchestrator.c` | Service lifecycle |
| `engine/src/kernel/subsystem/asset_manager.c` | Asset loading/caching |

## Framework Layer
| File | Purpose |
|------|---------|
| `engine/src/framework/ui/ui*.c` | UI abstraction |
| `engine/src/framework/graphics/sprite/sprite.c` | Sprite system |
| `engine/src/framework/service/*.c` | All game services |

## Services Layer
| File | Purpose |
|------|---------|
| `engine/src/services/session_db.c` | Session database (queryable logs) |
| `engine/src/services/database.c` | SQLite abstraction |
| `engine/src/services/config.c` | Engine configuration |

---

# DATABASE SCHEMA (For Reference)

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
  score_b100 INTEGER, score_b50 INTEGER, score_b20 INTEGER,
  score_b10 INTEGER, score_b5 INTEGER, score_b3 INTEGER,
  ternary_b3 INTEGER, ternary_b9 INTEGER,
  ternary_b27 INTEGER, ternary_b81 INTEGER,
  cube_x INTEGER, cube_y INTEGER, cube_z INTEGER
);

-- Indexes for fast queries
CREATE INDEX idx_logs_session ON log_entries(session_id);
CREATE INDEX idx_logs_level ON log_entries(level);
CREATE INDEX idx_logs_component ON log_entries(component);
CREATE INDEX idx_logs_state ON log_entries(hebrew_state);
CREATE INDEX idx_logs_time ON log_entries(timestamp_ms);
```

---

# VERIFICATION COMMANDS

```bash
# Check per-session logs
ls -la ~/.local/share/cornerstone/data/logs/cpisi_*.jsonl

# Query session database
sqlite3 ~/.local/share/cornerstone/data/sessions.db \
  "SELECT session_id, hebrew_state, COUNT(*) FROM log_entries GROUP BY session_id;"

# Test all display modes
cornerstone --gui
cornerstone --tui
cornerstone --cli

# Check build
make && make update-user
```

---

*Plan updated: 2026-01-29*
*Status: Ready for IDE integration phase*
