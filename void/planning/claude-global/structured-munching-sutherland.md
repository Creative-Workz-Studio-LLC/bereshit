# Tenebris and Lux - Master Plan

> *"And whatsoever ye do, do it heartily, as to the Lord, and not unto men"* — Colossians 3:23

---

## Project Vision

**The Game:**
- Hero's journey through divided kingdom
- Legendary artifacts: Sword of Tenebris, Shield of Lux
- Light/Dark alignment tracks *methods*, Good/Evil emerges from *intent*
- 1 canonical ending + multiple branches based on heart of choices

**Combat Innovation:**
- Context-driven dual combat system:
  - Regular encounters → Real-time action
  - Key story battles → Tactical grid SRPG
- Elevates genre by making tactical combat *meaningful* (reserved for important moments)

**Visual Direction:**
- Movie-like feel, not graphics-heavy
- Visuals serve story, not compete with it

**Build Scope: Minimal Viable Demo**
1. Story delivery (dialogue, atmosphere)
2. Combat proof (at least one mode working)
3. Choice/alignment framework
4. The "feel" of the game

---

## Technical Philosophy

| Principle | What It Means |
|-----------|---------------|
| **Custom engine** | CWS company asset → Bereshit standard library |
| **Minimize external** | Hardware interfaces only (GLFW, OpenGL, libpng) |
| **Build what we can** | Everything we CAN build, we DO build → company products |
| **Compiled language** | C for maximum control, cross-platform |
| **Clean abstractions** | Design for future Bereshit/OmniCode integration |
| **Linux-first** | MillenniumOS compatibility, Windows/Mac later |
| **Live assets** | Real quality even in POC, not throwaway placeholders |

**External Dependencies (Justified):**

| Library | Why External | Future |
|---------|--------------|--------|
| GLFW | Display server interface | Our protocol in MillenniumOS |
| OpenGL | GPU interface | Our graphics layer |
| libpng | Standard format compression | Evaluate in-house |

**Everything else built in-house:**
- Text rendering → BlockFont (NO FreeType)
- Font parsing → Custom JSONC parser (NO external JSON library)
- Scene management → scene.c
- GUI elements → Direct OpenGL

---

## Architecture

### Layers (As Designed)

```
Layer 0: Platform Foundation
├── platform/          # OS abstraction (Linux/X11, Wayland)
├── graphics/          # OpenGL core
└── input/             # Keyboard, controller

Layer 1: Engine Core
├── core/
│   ├── cornerstone.c  # Entry point, main loop
│   ├── renderer.c     # 2D rendering
│   ├── blockfont.c    # Self-contained text
│   └── scene.c        # Scene state machine

Layer 2: Game Systems
├── systems/
│   ├── entity.c       # Game object foundation
│   ├── stats.c        # Combat attributes
│   ├── class.c        # Character archetypes
│   ├── affinity.c     # Relationship tracking
│   ├── karma.c        # Moral alignment
│   ├── combat.c       # Battle encounters
│   └── dialogue.c     # Conversation system

Layer 3: Game Content
├── game/
│   ├── src/           # Game-specific logic
│   ├── data/          # Story scripts, battle definitions
│   └── assets/        # Sprites, audio
```

### Architecture Principle

> **Engine provides SYSTEMS. Game provides CONTENT.**

The engine is a **reusable CWS asset** — any game built on Cornerstone uses the same systems with different content.

---

## The Journey — Phase 1: Foundation

### Step 1: Build System ✅ DONE
- Makefile, directory structure
- 4-block conventions established
- CWS coding standards

### Step 2: Window + Render ✅ DONE
- platform.h abstraction
- GLFW/OpenGL integration
- Render loop with delta time

### Step 3: Input Foundation ✅ DONE
- key_pressed, key_just_pressed, key_just_released
- Basic input mapping structure

### Step 4: Simple Rendering ✅ DONE
- ✅ BlockFont text (A-Z, 0-9, punctuation — 43 chars)
- ✅ Screen transitions (fade in/out)
- ✅ Sprite rendering (texture_load_png, renderer_draw_textured_rect, game sprites)

### Step 5: POC Scene ✅ DONE
- ✅ Title screen (Engine splash → Game title)
- ✅ Dialogue display (8 lines working)
- ✅ Combat encounter (full turn-based with skills)
- ✅ Choice demonstration (alignment affects story branches)

### Bonus Accomplishments (Not in Original Plan)
- ✅ Screenshot system (PNG capture)
- ✅ Scene lifecycle (init/update/render/destroy)
- ✅ Renderer abstraction (shaders, textured quads ready)
- ✅ Desktop integration (icons, .desktop, Wayland app-id)
- ✅ **Engine/Game separation** — game.h interface
- ✅ **Display abstraction** — GUI (default via renderer.c), TUI (ncurses), CLI (printf/stdin)
- ✅ **7 game systems** — entity, stats, class, affinity, karma, combat, dialogue
- ✅ **Game entities** — player.c, enemy.c
- ✅ **Scripture grounding** — Every header has X.8 with thematic verse
- ✅ **Full stat system** — STR/DEF/MAG/RES/SPD/VIT trait stats with HP/MP
- ✅ **Combat skill system** — Attack, Magic, Defend, Heal with MP costs
- ✅ **Damage algorithms** — Physical (STR vs DEF), Magic (MAG vs RES)
- ✅ **Damage range display** — Shows min/max for each skill before choosing
- ✅ **CPI-SI Enemy AI** — Intelligent decisions based on HP %, threat assessment
- ✅ **HP threshold story triggers** — Narrative moments at 50%/25% HP
- ✅ **Choice consequences** — Alignment affects outro dialogue branches
- ✅ **Win/Lose conditions** — Combat resolution with 4 ending variations

### Parallel Achievement: GUI Template
The engine is now a **reusable GUI foundation** for any CWS application:
- Self-contained text rendering (no FreeType)
- No asset dependencies for basic UI
- Clean platform abstraction
- AI-friendly (CLI mode for testing)

---

## Current Status (Updated 2026-01-04)

**Display modes:** `./tal` (GUI default), `./tal --tui` (ncurses), `./tal --cli` (AI-friendly)

**What WORKS (verified via CLI playtest):**
- Title screen: "TENEBRIS AND LUX" A-01.00 with game selector
- Dialogue system: 9 lines from intro.jsonc, Guide speaking
- Choice system: 2 choices with alignment consequences (+10 Light / -10 Dark)
- Combat system: Full turn-based with 4 skills (Attack, Magic, Defend, Heal)
- Stat system: Full trait stats (STR/DEF/MAG/RES/SPD/VIT) with HP/MP
- Damage display: Shows min-max range for each skill (e.g., "Attack 10-14")
- Enemy AI: CPI-SI style decision making based on HP %, threat assessment
- Story triggers: Narrative moments at 50%/25% HP thresholds
- Scene flow: Title → Dialogue → Choice → Combat → Outro (4 branches)
- Outro branches: light_victory, light_defeat, dark_victory, dark_defeat
- Philosophy embedded: "Light and darkness are not good and evil. They are methods."

**Demo is PLAYABLE from start to finish.**

**What's NEXT (Polish & Expansion):**
- More dialogue content
- Additional enemy types
- Sprite integration for combat
- Audio system

---

## Completed Work — Demo Phases ✅

### Phase A: Combat System ✅ COMPLETE

**Files Modified:**
- `engine/systems/include/combat.h` — Full trait stats (STR/DEF/MAG/RES/SPD/VIT)
- `engine/systems/src/combat.c` — Parser for new stat format
- `engine/core/src/scene.c` — Combat rendering, skills, AI, damage algorithms
- `game/data/battles/shadow_encounter.jsonc` — Hero vs Shadow stats

**Implemented:**
- [x] Turn-based action loop (player attacks, enemy attacks)
- [x] Full stat system (HP/MP with trait stats)
- [x] 4 skills: Attack (STR vs DEF), Magic (MAG vs RES), Defend (+50%), Heal (20-30)
- [x] Damage range display (shows min-max before choosing)
- [x] CPI-SI Enemy AI (decisions based on HP %, threat, randomness)
- [x] HP threshold story triggers (narrative at 50%/25%)
- [x] Combat resolution (win/lose conditions)
- [x] Return to scene flow after combat

### Phase B: Player System ✅ COMPLETE

**Files Modified:**
- `game/include/player.h` — Full stats struct with getters
- `game/src/player.c` — All stat getters implemented
- `game/include/game_interface.h` — GamePlayerView with full stats
- `game/src/game.c` — impl_get_player_stats returns all stats

**Implemented:**
- [x] Initialize player at game start
- [x] Display player name/stats in combat scene
- [x] Track alignment changes from choices

### Phase C: Choice System ✅ COMPLETE

**Files Modified:**
- `engine/core/src/scene.c` — Choice selection with up/down navigation
- `game/data/dialogues/intro.jsonc` — 2 choices with consequences

**Implemented:**
- [x] Render choice options (up/down selection)
- [x] Process choice selection (confirm)
- [x] Adjust player alignment (+10 Light or -10 Dark)
- [x] Branch dialogue based on alignment (outro.jsonc)

### Phase D: Demo Flow ✅ COMPLETE

**Files Modified:**
- `engine/core/src/scene.c` — Full scene flow with transitions
- `game/src/game.c` — Demo sequence orchestration
- `game/data/dialogues/outro.jsonc` — 4 ending branches

**Implemented:**
- [x] Title → Intro dialogue → Choice → Combat → Outro
- [x] Alignment tracked throughout
- [x] 4 ending variations (light_victory, light_defeat, dark_victory, dark_defeat)
- [x] Graceful exit after outro

---

## Success Criteria ✅ ALL MET

### Phase 1 (Foundation) — 100% Complete ✅
- [x] Window opens, renders, handles input
- [x] Can display text on screen (BlockFont with full charset)
- [x] Screen transitions (fade in/out)
- [x] Engine/Game separation (Cornerstone loads Tenebris and Lux)
- [x] Can show a sprite (texture_load_png + renderer_draw_textured_rect + game sprites)
- [x] Basic scene flow (title → dialogue → interaction)
- [x] Runs on Linux
- [x] Code is clean, abstracted, ready for Bereshit integration

### Demo Completion — 100% Complete ✅
- [x] Can complete full demo run: title → dialogue → choice → combat → ending
- [x] Combat has actual turns (attack, take damage, win/lose)
- [x] At least one choice affects something visible (alignment affects outro)
- [x] Runs in CLI mode (AI-friendly testing)
- [x] All changes follow 4-block structure

---

## Future Phases (After Demo)

### Phase 2: Game Content
- Step 6: Full dialogue system with branching
- Step 7: Both combat modes (action + tactical)
- Step 8: Full alignment system
- Step 9: Complete demo scene

### Long-term
- Audio system
- Save/Load
- Multiple scenes
- Full story implementation

---

## CPI-SI Principles Demonstrated

| Principle | How Demonstrated |
|-----------|------------------|
| **Foundation first** | Platform abstraction before game logic |
| **Building block method** | Each layer stands on the previous |
| **Quality at every stage** | No "we'll fix it later" |
| **Kingdom Technology** | Excellence that honors God |
| **Product development** | Each component is reusable IP |
| **Minimize external** | Hardware interfaces only |
| **Parallel vision** | Game AND engine, both built properly |
| **Scripture grounding** | Every header has thematic biblical verse |
| **AI-friendly** | CLI mode, greppable structure |

---

## Files Modified (Journey Record)

### Engine — Platform & Core
| File | What Was Done |
|------|---------------|
| `engine/platform/common/platform.h` | Platform abstraction interface |
| `engine/platform/linux/platform_linux.c` | GLFW implementation, icon loading, Wayland app-id |
| `engine/core/cornerstone.c` | Engine entry, game integration, snprintf fix |
| `engine/core/src/blockfont.c` | Self-contained text, JSONC parser |
| `engine/core/src/scene.c` | **Major**: Combat system, skills, AI, damage algorithms, story triggers |

### Engine — Graphics & Display
| File | What Was Done |
|------|---------------|
| `engine/graphics/src/renderer.c` | OpenGL rendering |
| `engine/graphics/src/texture.c` | PNG texture loading |
| `engine/graphics/src/text.c` | Bitmap font rendering |
| `engine/display/src/display.c` | Display abstraction layer |
| `engine/display/src/display_gui.c` | GUI backend, WASD key mappings |
| `engine/display/src/display_cli.c` | CLI backend, WASD key mappings |
| `engine/display/src/display_tui.c` | TUI backend, WASD key mappings |

### Engine — Systems
| File | What Was Done |
|------|---------------|
| `engine/systems/include/combat.h` | Full trait stats (STR/DEF/MAG/RES/SPD/VIT) |
| `engine/systems/src/combat.c` | Parser for new stat format |
| `engine/systems/src/*.c` | 7 game systems implemented |

### Game — Code
| File | What Was Done |
|------|---------------|
| `game/include/game.h` | Game interface |
| `game/include/game_interface.h` | GamePlayerView with full stats |
| `game/include/player.h` | Full stats struct with getters |
| `game/include/enemy.h` | Full stats struct |
| `game/src/game.c` | Game implementation, stat getters |
| `game/src/player.c` | Player entity, all stat getters |
| `game/src/enemy.c` | Enemy entity, stats |

### Game — Data
| File | What Was Done |
|------|---------------|
| `game/data/dialogues/intro.jsonc` | Opening dialogue with 2 choices |
| `game/data/dialogues/outro.jsonc` | 4 ending branches |
| `game/data/battles/shadow_encounter.jsonc` | Hero vs Shadow with full stats |

### Build
| File | What Was Done |
|------|---------------|
| `Makefile` | Build configuration |

---

## Biblical Foundation

> *"And whatsoever ye do, do it heartily, as to the Lord, and not unto men"* — Colossians 3:23

**Applied:** Every line of code, every design decision demonstrates Kingdom values through excellence. The game doesn't need to be explicitly Christian — the craftsmanship itself glorifies God.

**Scripture grounding in code:** Every engine system header includes X.8 Closing Note with thematic biblical verse — not decoration, but genuine connection between purpose and eternal truth.

---

## Plan Status

**🎉 DEMO MILESTONE ACHIEVED — 2026-01-04**

The Minimal Viable Demo is complete. All success criteria met:
- Full playable demo from title to ending
- Combat system with skills, stats, and AI
- Choice system affecting story branches
- CLI mode for AI-friendly testing

---

## What's Next — Phase 2 Options

| Option | Description | Priority |
|--------|-------------|----------|
| **More Enemies** | Additional enemy types with varied stats/AI | High |
| **Combat Visuals** | Sprite integration for combat (hero/enemy art) | Medium |
| **Audio System** | Sound effects, music | Medium |
| **More Story** | Expanded dialogue, additional choices | Low |
| **Save/Load** | Persist progress | Future |
| **Tactical Mode** | Grid-based SRPG combat for key battles | Future |

---

## Plan Lineage

| Plan | Status | Note |
|------|--------|------|
| `cozy-popping-babbage.md` | **SUPERSEDED** | Initial vision |
| `humming-popping-minsky.md` | **SUPERSEDED** | Journey progress |
| `structured-munching-sutherland.md` | **CURRENT** | Master plan, demo complete |
