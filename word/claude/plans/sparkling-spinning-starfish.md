# Universal Base GUI — ViewModel Architecture with GTK4

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost?"* — Luke 14:28

## Context

The SDK hookoutput package just got restructured (53 -> 98/100 lint score, 7 focused files). Seanje wants it in a "well designed, fully us GUI" — not a web app, a **legitimate native application**. A universal base that starts as SDK dashboard but grows into MillenniumOS (Piece #4 of Five Pieces). Start with GTK4 (universal), design so renderers can be swapped as things mature. GTK4 stays as permanent fallback.

**What exists:** Three dashboard UIs already ship (Web, TUI, GTK4) consuming `pkg/dashboard/DashboardService`. The GTK4 dashboard (6 files, ~1,700 lines) works but is tightly coupled to gotk4 widgets — every panel directly creates `gtk.Box`, `gtk.Label`, etc. No abstraction between business logic and rendering.

**The problem:** Can't grow beyond dashboard. Can't swap renderer. Can't become MillenniumOS shell. Dead end for the vision.

## Architecture: ViewModel Pattern

**Panels declare what to show. Renderers decide how to show it.**

```
DashboardService -> Module -> ViewModel -> Renderer(GTK4) -> Screen
                                        -> Renderer(Custom) -> Screen (future)
```

Modules never change. Only the renderer does. GTK4 stays forever as `--renderer=gtk` fallback.

### Core Interfaces

**ViewModel types** (`render/viewmodel.go`) — toolkit-agnostic structured data:
- `View` — A panel: ID, Title, Sections
- `Section` — Title + Fields + Children
- `Field` — Key/Value with Style, optional Progress/Bar
- `Progress` — Fraction (0-1) + Label + Style
- `BarSegment` — Width proportion + semantic style

**Renderer interface** (`render/renderer.go`):
- `Renderer.RenderView(view View)` — display a ViewModel
- `Renderer.RenderLog(entry LogEntry)` — append to message terminal
- `Shell.Init(app) / RegisterModule(mod) / Run() / Close()` — application frame

**Module interface** (`app/module.go`):
- `ID() / Name() / Views() / Start(ctx, renderer) / Stop()` — self-contained feature areas

### Directory Structure

```
word/claude/gui/                    # NEW Go module: cws.studio/gui
  cmd/cws-gui/main.go              # Entry point
  app/
    app.go                          # Application lifecycle
    module.go                       # Module interface + registry
  modules/
    dashboard/                      # Migrated from existing GTK4 dashboard
      module.go, views.go, update.go, terminal.go
    sdkinspector/                    # NEW — Hook I/O visualization
      module.go, views.go, update.go
  render/
    viewmodel.go                    # ViewModel types (toolkit-agnostic)
    renderer.go                     # Renderer + Shell interfaces
    gtk/                            # GTK4 renderer implementation
      shell.go, renderer.go, widgets.go, theme.go
  go.mod, Makefile
```

## Phase 0: Foundation + Dashboard Migration (18 files, ~2,225 lines)

### Phase 0.1: Core Structure (foundation — no GTK4 imports)

| File | Purpose | Est Lines |
|------|---------|:---------:|
| `gui/go.mod` | Module: `cws.studio/gui` | 15 |
| `gui/render/viewmodel.go` | ViewModel types (Section, Field, Progress, Bar, View) | 120 |
| `gui/render/renderer.go` | Renderer + Shell interfaces | 60 |
| `gui/app/module.go` | Module interface + ModuleRegistry | 80 |
| `gui/app/app.go` | Application lifecycle (register, init shell, run) | 120 |

**Verification:** `go build ./...` clean. Zero toolkit imports in core.

### Phase 0.2: GTK4 Renderer

| File | Purpose | Est Lines |
|------|---------|:---------:|
| `gui/render/gtk/shell.go` | GTK4 window, headerbar, sidebar, stack | 200 |
| `gui/render/gtk/renderer.go` | ViewModel -> gotk4 widget rendering | 250 |
| `gui/render/gtk/widgets.go` | Widget factory: Section->Box, Field->Label, etc. | 200 |
| `gui/render/gtk/theme.go` | Dark theme CSS (migrated from `dashboard/gtk/styles/`) | 320 |

**Verification:** Shell opens GTK4 window with sidebar. Empty but themed correctly.

### Phase 0.3: Dashboard Module Migration

| File | Purpose | Est Lines |
|------|---------|:---------:|
| `gui/modules/dashboard/module.go` | Module registration, lifecycle | 60 |
| `gui/modules/dashboard/views.go` | View declarations for Overview, Analytics, SystemData | 80 |
| `gui/modules/dashboard/update.go` | StateSnapshot -> ViewModel translation | 250 |
| `gui/modules/dashboard/terminal.go` | Log event ViewModel | 80 |

**Verification:** Visual parity with current `dashboard-gtk`. All 13 sections display correctly.

### Phase 0.4: SDK Inspector Module (first NEW module)

| File | Purpose | Est Lines |
|------|---------|:---------:|
| `gui/modules/sdkinspector/module.go` | Module registration | 50 |
| `gui/modules/sdkinspector/views.go` | Hook I/O inspection views | 100 |
| `gui/modules/sdkinspector/update.go` | Hook event monitoring + display | 120 |

**Verification:** SDK Inspector shows substrate detection, event types, permission values.

### Phase 0.5: Entry Point + Build

| File | Purpose | Est Lines |
|------|---------|:---------:|
| `gui/cmd/cws-gui/main.go` | Entry point (register modules, init GTK4 shell, run) | 80 |
| `gui/Makefile` | Build: `make`, `make run`, `make clean`, `make install` | 40 |

**Verification:** `make && ./bin/cws-gui` launches full application.

## Key Existing Code to Reuse

| File | Reuse |
|------|-------|
| `dashboard/gtk/styles/theme.go:1-317` | Copy CSS to `gui/render/gtk/theme.go` |
| `dashboard/gtk/app/app.go:1-281` | Reference for GTK4 shell patterns |
| `dashboard/gtk/panels/state_overview.go:1-563` | Reference for ViewModel field mapping |
| `pkg/dashboard/dashboard.go` | Consumed directly — shared data layer, no changes |
| `pkg/sdk/hookoutput/*.go` | Consumed by SDK Inspector module |

## Existing Code Disposition

| Code | Action |
|------|--------|
| `dashboard/gtk/` (6 files) | **Keep as reference and working backup** — not deleted until `cws-gui` reaches parity |
| `pkg/dashboard/` | **Keep as-is** — the data layer is solid, all UIs consume it |
| `dashboard/server/`, `dashboard/tui/` | **Keep as-is** — independent UIs unaffected |

## Module Growth Path (Post Phase 0)

| Phase | Module | What It Adds |
|-------|--------|-------------|
| 1 | `hooks` | Live hook monitor, health deltas, session timeline |
| 1 | `cwsstruct` | Lint results visualization, schema browser |
| 2 | `statemachine` | 27-position cube visualizer (Cairo/OpenGL) |
| 3 | `omnicode` | OmniCode file editor with pragma validation |
| 4 | `vm` | QEMU/KVM virtual machine management |
| 5 | Shell mode | MillenniumOS desktop shell (fullscreen, WM, launcher) |

## Migration Path: GTK4 -> Custom Renderer

```
Phase 0-2: GTK4 only
Phase 2: Custom drawing via GTK4 DrawingArea (cube viz = bridge)
Phase 3-4: Custom renderer alongside GTK4 (--renderer=gtk vs --renderer=custom)
Phase 5: Custom renderer default, GTK4 = fallback
MillenniumOS: Custom renderer is the compositing window manager
```

**GTK4 is never removed.** It stays as the known-good path.

## Verification (End-to-End)

1. `cd gui && go build ./...` — compiles with zero warnings
2. Zero GTK4 imports in `render/viewmodel.go`, `render/renderer.go`, `app/module.go`, `app/app.go`
3. `make && ./bin/cws-gui` — launches, shows sidebar with Dashboard + SDK Inspector
4. Dashboard shows all 13 sections with real-time updates (visual parity)
5. SDK Inspector shows hook event types and substrate detection
6. `go.work` updated with `use ./gui`
7. `--renderer=gtk` flag accepted (only option now, but plumbing exists)
8. Graceful shutdown on SIGINT/SIGTERM
