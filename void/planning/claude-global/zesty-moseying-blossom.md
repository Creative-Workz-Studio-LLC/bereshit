# Engine Updates + Editor Creation Plan

## Overview

Update Cornerstone engine to fully support current data structures, then create a layered editor (CLI → TUI → GUI) where CLI is the truth and visual interfaces are abstractions.

---

## Phase 1: Engine Data Loading Completion

**Goal:** Ensure dialogue system loads ALL fields from data files.

### Files to Modify

| File | Change |
|------|--------|
| `cornerstone/engine/systems/include/narrative/dialogue.h` | Add `portrait`, `emotion`, `delay` fields to DialogueLine struct |
| `cornerstone/engine/systems/src/narrative/dialogue.c` | Update `core_parse_dialogue_line()` to parse new fields |

### Verification

```bash
make clean && make debug
./cornerstone --cli
# Confirm dialogue loading logs show portrait/emotion/delay
```

---

## Phase 2: CLI Editor Foundation

**Goal:** Create standalone CLI commands as the foundation.

### New Directory Structure

```
cornerstone/editor/
├── include/
│   └── editor.h              # Editor API
├── src/
│   ├── editor.c              # Core initialization
│   ├── cmd_list.c            # List data files
│   ├── cmd_show.c            # Show entry content
│   ├── cmd_validate.c        # Validate against schema
│   ├── cmd_create.c          # Create from template
│   └── cmd_edit.c            # Edit field values
└── cli/
    └── editor_main.c         # CLI entry point
```

### CLI Commands

| Command | Purpose |
|---------|---------|
| `editor list <type>` | List data files by type (dialogues, stats, battles) |
| `editor show <path>` | Display formatted file content |
| `editor validate <path>` | Validate against schema |
| `editor create <type> <template> <name>` | Create from template |
| `editor set <path> <field> <value>` | Edit single field |

### Verification

```bash
make editor
./bin/editor list dialogues
./bin/editor show cornerstone/data/dialogues/linear/intro.jsonc
./bin/editor validate cornerstone/data/dialogues/linear/intro.jsonc
```

---

## Phase 3: TUI Layer

**Goal:** Wrap CLI with ncurses visual interface.

### New Files

```
cornerstone/editor/tui/
├── include/editor_tui.h
└── src/
    ├── tui_main.c            # TUI entry point
    ├── tui_browser.c         # File browser panel
    ├── tui_viewer.c          # Content viewer panel
    └── tui_editor.c          # Field editor panel
```

### Key Principle

TUI calls CLI functions internally, renders results in ncurses panels.

### Verification

```bash
make editor-tui
./bin/editor-tui
# Navigate with arrows, edit fields, verify file changes
```

---

## Phase 4: GUI Layer

**Goal:** Wrap CLI with OpenGL visual interface.

### New Files

```
cornerstone/editor/gui/
├── include/editor_gui.h
└── src/
    ├── gui_main.c            # GUI entry point (uses platform.h)
    ├── gui_panels.c          # Panel rendering
    ├── gui_browser.c         # File browser widget
    ├── gui_viewer.c          # Content viewer widget
    └── gui_editor.c          # Field editor widget
```

### Key Principle

GUI uses existing engine infrastructure (platform.h, renderer.h, text.h), calls same CLI functions.

### Verification

```bash
make editor-gui
./bin/editor-gui
# Click to navigate, edit, verify persistence
```

---

## Implementation Order

1. **Phase 1 first** — Complete engine loading so we know data works
2. **Phase 2 next** — CLI foundation becomes the truth
3. **Phase 3** — TUI adds visual navigation
4. **Phase 4** — GUI completes the layer cake

---

## Critical Files Reference

**Engine (Phase 1):**
- `cornerstone/engine/systems/include/narrative/dialogue.h`
- `cornerstone/engine/systems/src/narrative/dialogue.c`

**Data Reference:**
- `cornerstone/data/dialogues/linear/*.jsonc` — Shows full feature depth
- `cornerstone/schemas/dialogue.schema.jsonc` — Validation schema

**Templates:**
- `cornerstone/templates/code/c/source.c` — 4-block pattern for new C files
- `cornerstone/templates/code/c/header.h` — 4-block pattern for headers

---

## What This Plan Does NOT Do

- Over-engineer for features we don't have yet
- Jump to GUI before CLI works
- Create complex abstractions before basic functions work

---

## End-to-End Verification

After all phases:

```bash
# 1. Engine loads dialogues with all fields
./cornerstone --cli
# Verify intro dialogue plays with proper timing

# 2. CLI editor works standalone
./bin/editor list dialogues
./bin/editor show cornerstone/data/dialogues/linear/intro.jsonc
./bin/editor create dialogue linear test_dialogue
./bin/editor validate cornerstone/data/dialogues/linear/test_dialogue.jsonc

# 3. TUI editor wraps CLI
./bin/editor-tui
# Visual navigation, same operations

# 4. GUI editor wraps CLI
./bin/editor-gui
# Full visual editor, same operations
```
