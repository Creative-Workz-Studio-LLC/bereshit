# OmniCode IDE Architecture

## Current State Analysis

### File Structure

```
omni_ide.h/c        (837 lines)  - Core logic (FOUNDATION)
omni_ide_panel.h/c  (662 lines)  - Panel system
omni_ide_tui.h/c    (1314 lines) - TUI presentation (ncurses direct)
omni_ide_gui.h/c    (1470 lines) - GUI presentation (display abstraction)
omni_ide_main.c     (197 lines)  - Entry point
```

### Architecture Diagram

```
                    omni_ide_main.c
                         │
          ┌──────────────┼──────────────┐
          │              │              │
          ▼              │              ▼
   omni_ide_tui.c        │       omni_ide_gui.c
   (ncurses direct)      │       (display_*() calls)
          │              │              │
          │              ▼              │
          │      omni_ide.c             │
          │    (core logic)             │
          │              │              │
          ▼              │              ▼
      <ncurses.h>        │        display.h
                         │              │
                         │      ┌───────┼───────┐
                         │      ▼       ▼       ▼
                         │    CLI     TUI     GUI
                         │  backend backend backend
```

### Layer Analysis

| Layer | File | Calls | Should Call |
|-------|------|-------|-------------|
| Core Logic | omni_ide.c | Nothing external | ✓ Correct |
| TUI Presentation | omni_ide_tui.c | ncurses directly | display_*() |
| GUI Presentation | omni_ide_gui.c | display_*() | ✓ Correct |
| Panels | omni_ide_panel.c | ncurses directly | display_*() |

### The Problem

- **TUI** (1314 lines) calls ncurses directly: `mvprintw()`, `attron()`, `getch()`
- **GUI** (1470 lines) calls display abstraction: `display_print()`, `display_wait_key()`
- These are **parallel implementations**, not layered
- TUI has features (panels) that GUI lacks
- GUI has features (mouse) that TUI lacks

### The Solution

**Target Architecture:**

```
                    omni_ide_main.c
                         │
                         ▼
                  omni_ide_ui.c        ← ONE presentation layer
                         │
          ┌──────────────┼──────────────┐
          │              │              │
          ▼              ▼              ▼
     display_*()    omni_ide.c    omni_ide_panel.c
                   (core logic)
          │
          ▼
    display.h (abstraction)
          │
    ┌─────┼─────┐
    ▼     ▼     ▼
   CLI   TUI   GUI
```

**One UI codebase** that:
1. Uses `display_*()` for all output
2. Uses `display_wait_key()` for all input
3. Works identically across CLI/TUI/GUI modes

### Migration Path

1. **Phase 1**: Make `omni_ide_tui.c` the reference (it has all features)
2. **Phase 2**: Gradually replace ncurses calls with display_*() calls
3. **Phase 3**: Merge panel system to use display abstraction
4. **Phase 4**: Deprecate separate `omni_ide_gui.c`
5. **Phase 5**: Rename unified code to `omni_ide_ui.c`

### Function Mapping

| ncurses (current) | display_*() (target) |
|-------------------|----------------------|
| `mvprintw(y, x, text)` | `display_print_at(x, y, text)` |
| `attron(COLOR_PAIR(n))` | `display_print_styled(text, style)` |
| `getch()` | `display_wait_key()` |
| `clear()` | `display_clear()` |
| `refresh()` | `display_refresh()` |
| `getmaxyx(stdscr, h, w)` | `display_get_size(&w, &h)` |
| `move(y, x)` | `display_set_cursor(x, y)` |

### Current Working State

- **TUI (default)**: Working, has all features, uses ncurses directly
- **GUI (--gui flag)**: Partial, uses display abstraction, missing panels
- **Desktop launch**: Opens TUI in terminal, working

### Priority

1. Keep TUI working throughout migration
2. Migrate one function at a time
3. Test after each change
4. No big bang rewrites

---

## Migration Plan

### ncurses Call Inventory (91 total)

| Call | Count | Migration |
|------|-------|-----------|
| `init_pair` | 23 | Remove - display layer handles colors |
| `attron` | 15 | → `display_print_styled()` |
| `move` | 14 | → `display_set_cursor()` |
| `attroff` | 13 | → (implicit in display_print_styled) |
| `printw` | 11 | → `display_print()` |
| `mvprintw` | 2 | → `display_print_at()` |
| `curs_set` | 3 | → (handled by display layer) |
| `noecho` | 3 | → (handled by display layer) |
| Lifecycle | 6 | → `display_init()` / `display_shutdown()` |

### Migration Phases

**Phase 1: Lifecycle (6 calls)**
- Replace `initscr()`, `endwin()`, `start_color()`, `keypad()`, `noecho()`, `curs_set()`
- With `display_init()` and `display_shutdown()`
- Remove all `init_pair()` calls (display layer handles colors)

**Phase 2: Output - Cursor (14 calls)**
- Replace `move(y, x)` with `display_set_cursor(x, y)`
- Note: argument order differs!

**Phase 3: Output - Print (13 calls)**
- Replace `printw()` with `display_print()`
- Replace `mvprintw()` with `display_print_at()`

**Phase 4: Output - Styling (28 calls)**
- Replace `attron()/attroff()` pattern with `display_print_styled()`
- Requires restructuring some code

**Phase 5: Input (1 call)**
- Replace `getch()` with `display_wait_key()`
- Map ncurses key codes to DisplayKey enum

**Phase 6: Size (1 call)**
- Replace `getmaxyx()` with `display_get_size()`

**Phase 7: Refresh (1 call)**
- Replace `refresh()` with `display_refresh()`

### Color Mapping

| TUI Color Constant | DisplayColor |
|--------------------|--------------|
| COLOR_LINENO | DISPLAY_COLOR_CYAN |
| COLOR_KEYWORD | DISPLAY_COLOR_BLUE |
| COLOR_STRING | DISPLAY_COLOR_GREEN |
| COLOR_COMMENT | DISPLAY_COLOR_BRIGHT_BLACK |
| COLOR_STATUS | DISPLAY_COLOR_WHITE (bg: BLUE) |
| COLOR_MENU_BAR | DISPLAY_COLOR_BLACK (bg: WHITE) |
| etc. | ... |

### Migration Progress

| Phase | Status | Changes |
|-------|--------|---------|
| **Phase 1: Lifecycle** | ✅ Complete | Replaced `initscr()/raw()/noecho()/keypad()` with `display_init(NULL)`, `endwin()` with `display_shutdown()` |
| **Phase 2: Cursor** | ⏳ Pending | 14 `move()` calls |
| **Phase 3: Print** | ⏳ Pending | 13 `printw()/mvprintw()` calls |
| **Phase 4: Styling** | ⏳ Pending | 28 `attron()/attroff()` calls |
| **Phase 5: Input** | ⏳ Pending | 1 `getch()` call |
| **Phase 6: Size** | ⏳ Pending | 1 `getmaxyx()` call |
| **Phase 7: Refresh** | ⏳ Pending | 1 `refresh()` call |

**Remaining ncurses-specific:**
- Runtime `curs_set()` calls (cursor visibility during focus changes)
- `echo()/noecho()` for text input prompts
- IDE-specific color pairs via `init_pair()` (builds on display layer)

### Testing Checklist

After each phase:
- [x] `make ide-gui` builds without errors (Phase 1 verified)
- [ ] Desktop launch works in TTY context
- [ ] Syntax highlighting works
- [ ] Menu bar displays
- [ ] Keyboard navigation works
- [ ] Save (Ctrl+S) works
- [ ] Quit (Ctrl+Q / Q) works

---

*"Except the LORD build the house, they labour in vain that build it."* — Psalm 127:1
