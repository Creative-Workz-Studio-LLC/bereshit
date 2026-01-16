# Kingdom Technology Foundation: Cornerstone + OmniCode + Filesystem

## Vision Summary

**Cornerstone IS the OS kernel.** Everything runs through it.

```
MillenniumOS
    └── Cornerstone (THE kernel / application framework)
            │
            ├── Display Modes (switchable):
            │   ├── CLI  (text commands)
            │   ├── TUI  (ncurses terminal)
            │   └── GUI  (graphical window)
            │
            ├── Applications (modules within cornerstone):
            │   ├── IDE       (OmniCode editor)
            │   ├── Browser   (void/word/tov navigation)
            │   ├── Games     (Echo's Becoming, etc.)
            │   └── Tools     (utilities)
            │
            └── Core Systems:
                ├── Config    (TOML specs → typed structs) ✓ DONE
                ├── OmniCode  (parser → compiler → runtime)
                ├── FUSE/VFS  (void/word/tov as mountable FS)
                └── CPI-SI    (state machine / mind)
```

## Decisions Made

| Decision | Choice | Rationale |
|----------|--------|-----------|
| **OmniCode Parser** | C | Matches libtrit, compiles to native, part of cornerstone |
| **Filesystem** | FUSE from start | Real mountable filesystem, not just abstraction |
| **IDE** | Cornerstone module | Application within cornerstone, uses CLI/TUI/GUI display modes |

---

## What Exists (Foundation)

### 1. Cornerstone Game Engine (`/cornerstone/engine/`)
```
engine/
├── core/         # engine.h, game_interface.h, scene.h
├── platform/     # Linux abstraction
├── graphics/     # renderer, texture, text
├── display/      # cli, tui, gui (STUBS - headers only)
├── systems/      # 9 game systems (character, cognition, etc.)
└── Makefile      # Full build orchestration
```
**Status:** Substantial C infrastructure, but display layer is stubs.

### 2. Config System (Just Built)
```
word/work/pkg/trit/
├── include/toml.h, config.h    # Parser + typed structs
├── src/toml.c, config.c        # Implementation
word/core/
├── ternary.toml                # Math constants
├── primitives.toml             # Type definitions
```
**Status:** Complete. Loads TOML specs into typed C structs.

### 3. OmniCode Language (`word/omni/`)
```
word/omni/
├── syntax.omni      # Full syntax specification
├── seed/            # Templates (folder, code, data, documentation)
└── root.omni        # OmniCode folder metadata
```
**Currently:** 63+ .omni files used as metadata. Sentence-based syntax defined.
**Missing:** NO parser, NO compiler, NO execution.

### 4. Filesystem Structure
```
void/ (-1)  Entry gate    planning/, imports/, workflow/
word/ (0)   Processing    core/, glossary/, scripture/, work/
tov/ (+1)   Exit gate     data/, demo/, licenses/
```
**Currently:** Physical directories with root.omni metadata, .health binary files.
**Missing:** NO FUSE/VFS layer, NO enforcement of zone semantics.

---

## Architecture: How It Connects

```
┌──────────────────────────────────────────────────────────────┐
│                     MillenniumOS (Future)                     │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │     IDE     │  │   OmniCode  │  │   Filesystem (VFS)  │  │
│  │  (Editor)   │  │  (Language) │  │   void/word/tov     │  │
│  └──────┬──────┘  └──────┬──────┘  └──────────┬──────────┘  │
│         │                │                     │              │
│         └───────────┬────┴─────────────────────┘              │
│                     │                                         │
│              ┌──────┴──────┐                                  │
│              │ CORNERSTONE │  ← Unified kernel/engine         │
│              │   Engine    │                                  │
│              └──────┬──────┘                                  │
│                     │                                         │
│    ┌────────────────┼────────────────┐                       │
│    │                │                │                       │
│ ┌──┴───┐      ┌─────┴─────┐    ┌─────┴─────┐                │
│ │Config│      │  libtrit  │    │  CPI-SI   │                │
│ │System│      │  (types)  │    │  (mind)   │                │
│ └──────┘      └───────────┘    └───────────┘                │
│                                                              │
│                     TOML Specs                               │
│              (word/core/*.toml)                              │
└──────────────────────────────────────────────────────────────┘
```

---

## Implementation Strategy

### Phase 1: Wire Config to Cornerstone (IMMEDIATE)

**Goal:** Connect the config system we just built to the existing cornerstone engine.

**Tasks:**
1. Create `cornerstone/engine/config/` - Engine config integration
2. Modify engine initialization to load TOML specs
3. Make specs available to all engine subsystems
4. Test with CLI display (simplest first)

**Files to create/modify:**
- `cornerstone/engine/config/engine_config.h` - Engine config interface
- `cornerstone/engine/config/engine_config.c` - Load and expose config
- `cornerstone/engine/core/engine.c` - Integrate config loading
- `cornerstone/Makefile` - Add config sources

### Phase 2: OmniCode Parser (FOUNDATION)

**Goal:** Parse existing .omni files as structured data.

**Tasks:**
1. Create lexer for OmniCode tokens (pragmas, keywords, sentences)
2. Create parser producing AST
3. Parse existing root.omni files to validate
4. Define schema for parsed OmniCode

**Files to create:**
- `word/work/pkg/omni/lexer.c` - Tokenizer
- `word/work/pkg/omni/parser.c` - AST builder
- `word/work/pkg/omni/include/omni.h` - API
- `tov/demo/phase-4/demo-omni-parser/` - Validation demo

### Phase 3: FUSE Filesystem (Mountable void/word/tov)

**Goal:** Real mountable filesystem exposing void/word/tov semantically.

**Tasks:**
1. Implement FUSE operations (readdir, read, write, getattr)
2. Zone-aware path resolution (void=-1, word=0, tov=+1)
3. Health-based access control (read root.omni, enforce gates)
4. Parse root.omni for folder metadata at mount time
5. Mount at `/mnt/bereshit` or similar

**Files to create:**
- `word/work/pkg/fuse/bereshit_fs.c` - FUSE implementation
- `word/work/pkg/fuse/zone.h` - Zone abstraction
- `word/work/pkg/fuse/health.h` - Health enforcement
- `tov/demo/phase-5/demo-fuse-mount/` - Mount validation demo

**Dependencies:** libfuse3-dev

### Phase 4: IDE as Cornerstone Module

**Goal:** OmniCode editor running as cornerstone application.

**Tasks:**
1. Create IDE module interface (cornerstone plugin)
2. Text buffer with OmniCode syntax awareness
3. Block structure validation (3/4/5-block)
4. Template insertion helpers
5. Error reporting with line/column
6. Display through CLI/TUI/GUI modes

**Files to create:**
- `cornerstone/apps/ide/` - IDE module directory
- `cornerstone/apps/ide/editor.c` - Core editor logic
- `cornerstone/apps/ide/omni_syntax.c` - OmniCode highlighting
- `cornerstone/apps/ide/blocks.c` - Block structure validation

---

## Phase 1 Detail: Wire Config to Cornerstone

### Step 1.1: Create Engine Config Interface

```c
// cornerstone/engine/config/engine_config.h

#ifndef ENGINE_CONFIG_H
#define ENGINE_CONFIG_H

#include "config.h"  // From word/work/pkg/trit/

// Initialize config system with TOML spec directory
int engine_config_init(const char* spec_dir);

// Get loaded configuration
const Config* engine_config_get(void);

// Clean up config system
void engine_config_shutdown(void);

#endif
```

### Step 1.2: Integrate into Engine Core

Modify `cornerstone/engine/core/engine.c`:
```c
#include "config/engine_config.h"

int engine_init(const char* config_path) {
    // 1. Load TOML specifications
    if (engine_config_init(config_path) != 0) {
        return -1;  // Config load failed
    }

    // 2. Initialize subsystems using config
    const Config* cfg = engine_config_get();
    // ... subsystem init uses cfg->ternary, cfg->primitives

    return 0;
}
```

### Step 1.3: Update Makefile

Add config sources to cornerstone build:
```makefile
# Config sources from libtrit
CONFIG_SRC = $(BERESHIT)/word/work/pkg/trit/src/toml.c \
             $(BERESHIT)/word/work/pkg/trit/src/config.c
CONFIG_INC = -I$(BERESHIT)/word/work/pkg/trit/include
```

### Step 1.4: Test with CLI Display

Create minimal test:
```c
// cornerstone/demos/config_test.c
int main(int argc, char** argv) {
    engine_config_init("word/core");
    const Config* cfg = engine_config_get();

    printf("Loaded %d primitive types\n", cfg->primitives.count);
    printf("Trit base: %d\n", cfg->ternary.trit.base);

    engine_config_shutdown();
    return 0;
}
```

---

## Verification

### Phase 1 Success Criteria
1. `make` in cornerstone builds with config integration
2. Engine loads TOML specs on initialization
3. Subsystems can access typed config data
4. CLI display shows loaded configuration

### Test Commands
```bash
cd cornerstone
make clean && make
./bin/cornerstone --config=../word/core

# Should output:
# Loading configuration from word/core...
# Loaded ternary.toml: 3 trit values, 243 trit5 states
# Loaded primitives.toml: 7 types defined
# Engine initialized.
```

---

## Immediate Next Steps

**Phase 1 starts now:** Wire config system to cornerstone engine.

1. Create `cornerstone/engine/config/engine_config.h` and `.c`
2. Integrate config loading into `engine.c` initialization
3. Update cornerstone Makefile to include config sources
4. Test with CLI display showing loaded specs

**After Phase 1:** OmniCode parser in C (Phase 2).

---

## Files Reference

| Component | Key Files |
|-----------|-----------|
| **Config (built)** | `word/work/pkg/trit/src/config.c`, `include/config.h` |
| **Cornerstone** | `cornerstone/engine/core/engine.h`, `cornerstone/Makefile` |
| **OmniCode spec** | `word/omni/syntax.omni`, `word/omni/seed/*.omni` |
| **Filesystem** | `void/root.omni`, `word/root.omni`, `tov/root.omni` |
