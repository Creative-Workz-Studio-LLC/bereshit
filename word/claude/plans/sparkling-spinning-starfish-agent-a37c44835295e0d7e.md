# Implementation Plan: Universal Base GUI Application

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost, whether he have sufficient to finish it?"* — Luke 14:28

**Planner:** Caleb Navon (plan-builder)
**Date:** 2026-02-24
**Status:** Strategic Plan

---

## Current State

### What Exists Now

**Three dashboard UIs already ship**, all consuming `pkg/dashboard/DashboardService`:

| UI | Module | Go Module | Coupling |
|----|--------|-----------|----------|
| Web (Express+WS) | `dashboard/server/` + `dashboard/frontend/` | `cws.studio/dashboard` | REST/WS via `server.New(svc, port)` |
| TUI (Bubble Tea) | `dashboard/tui/` | `cws.studio/dashboard` | Direct `DashboardService` |
| GTK4 (gotk4) | `dashboard/gtk/` | `cws.studio/dashboard/gtk` (separate module) | Direct gotk4 widgets + `DashboardService` |

**GTK4 Implementation (4 panels, 1 app shell, 1 theme):**
- `app/app.go` (281 lines) — GTK4 Application shell: window, layout, theme, event bridge
- `panels/state_overview.go` (563 lines) — Cube position, Hebrew state, health, CPI, K:ALIGN, session, valence
- `panels/analytics.go` (268 lines) — Session history, trends, patterns, metrics
- `panels/system_data.go` (268 lines) — File browser for `system/data/`
- `panels/message_terminal.go` (219 lines) — Scrolling log viewer with severity coloring
- `styles/theme.go` (317 lines) — Dark theme CSS matching web dashboard palette

**Shared Data Layer (`pkg/dashboard/`):**
- `DashboardService` wraps 5 SQLite databases + fsnotify file watching + EventBus pub/sub
- `StateSnapshot` is the primary data type consumed by all UIs
- `StateChange`, `LogEvent` for real-time distribution
- `AnalyticsBundle` for historical data

**SDK Package (`pkg/sdk/hookoutput/`):**
- 7 files just restructured: substrate detection, events, context, pretool, stop, input, JSON
- This is the hook I/O layer — what every Claude Code hook emits/consumes

**Package Layer Map:**
```
pkg/
  foundation/     — L0: database, result, schema, types
  core/           — L1: statemachine, coordinates, health, cpisi, validation
  orchestration/  — L2: cognition, config, loader, logging, restore
  sdk/            — SDK: hookoutput
  util/           — Utilities: fs, pure, term, transcript, vcs
  agent/          — Agent system
  dashboard/      — Shared DashboardService (all UIs consume this)
```

**Workstation:**
- GTK4 development libraries: Installed and functional
- QEMU: `qemu-system-x86_64` installed (v10.1.0)
- libvirt/virt-manager: NOT installed (needs `pkexec apt install`)
- Go module: gotk4 v0.3.1 bindings already in use

### The Problem

The GTK4 dashboard works, but it is tightly coupled to gotk4 widgets. Every panel directly creates `gtk.Box`, `gtk.Label`, `gtk.ProgressBar`, etc. There is no abstraction layer between panel logic and rendering. Swapping GTK4 for a custom renderer would require rewriting every panel from scratch.

The existing `DashboardService` is well-designed — toolkit-agnostic data layer. The problem is above that layer, at the UI component level.

Additionally, the current GTK4 dashboard is narrowly scoped to CPI-SI state visualization. The vision is a universal application that grows to contain cws-struct, hook monitoring, OmniCode editing, and eventually becomes the MillenniumOS desktop shell.

---

## Scope Assessment

- **Files affected (existing):** 7 GTK files (refactor), 0 pkg files (data layer stays)
- **Files to create:** ~20-25 new files across Phase 0
- **Directories:** New `gui/` top-level directory in `word/claude/`
- **Risk areas:**
  1. gotk4 binding limitations — some GTK4 concepts may not map cleanly to an abstract interface
  2. Performance overhead of an adapter layer on top of GTK4
  3. Module management — new Go module with gotk4 dependency
  4. Scope creep — the vision is enormous; Phase 0 must ship small

---

## Architecture Decision: Renderer Adapter Depth

### The Key Question

How deep should the abstraction go? I see the whole landscape here, and I recommend a layered approach that starts thin and deepens where patterns prove it worthwhile.

### Recommendation: Panel Interface + View Model Pattern

**Not Option A (thin adapter), not Option B (widget tree), not Option C (immediate mode).**

Instead: **View Model pattern.** Panels declare what they WANT to display (structured data). Renderers decide HOW to display it.

```
DashboardService → ViewModel → Renderer(GTK4) → Screen
                                Renderer(Custom) → Screen (future)
```

**Why this is better than Options A/B/C:**

| Approach | Problem |
|----------|---------|
| **Option A (thin adapter)** | Panel-level abstraction still means reimplementing all panel layout logic per backend |
| **Option B (widget tree)** | Abstracts widgets but creates a "portable widget toolkit" — we'd be building our own GTK |
| **Option C (immediate mode)** | Fundamentally different paradigm from GTK4's retained mode — hard to bridge |
| **View Model (recommended)** | Panels emit structured data; renderers translate to widgets. Panels written ONCE. Backend-specific only in render layer. |

**How it works:**

1. **Module** — A logical feature area (Dashboard, SDK Inspector, Hooks Monitor, etc.)
2. **View** — A panel within a module that declares its data shape via a ViewModel
3. **ViewModel** — Structured Go data (no widget references) describing what to show
4. **Renderer** — Translates ViewModels to actual widgets (GTK4 now, custom later)
5. **Shell** — The application frame (window, sidebar, header) — thin renderer-specific layer

**The critical insight:** The existing `StateSnapshot` is ALREADY a ViewModel. The panels just translate it to widgets too eagerly. Separate that concern and the architecture opens up.

---

## Architecture

### Directory Structure

```
word/claude/gui/                          # NEW — Universal Base GUI
  cmd/
    cws-gui/
      main.go                             # Entry point

  app/
    app.go                                # Application lifecycle
    module.go                             # Module interface + registry
    shell.go                              # Shell interface (window frame)

  modules/
    dashboard/
      module.go                           # DashboardModule registration
      views.go                            # View declarations + ViewModels
      update.go                           # Data binding (DashboardService -> ViewModels)

    sdkinspector/
      module.go                           # SDKInspectorModule registration
      views.go                            # Hook I/O visualization ViewModels
      update.go                           # Live hook data binding

  render/
    renderer.go                           # Renderer interface
    viewmodel.go                          # Common ViewModel types (Section, Field, Progress, etc.)

    gtk/
      renderer.go                         # GTK4 Renderer implementation
      shell.go                            # GTK4 Shell (window, headerbar, sidebar)
      widgets.go                          # ViewModel → gotk4 widget mapping
      theme.go                            # CSS theme (migrated from existing)

  go.mod                                  # Module: cws.studio/gui
  go.sum
```

### Core Interfaces

```go
// render/viewmodel.go — What panels declare (toolkit-agnostic)

type Section struct {
    Title    string
    Fields   []Field
    Children []Section
}

type Field struct {
    Key       string
    Value     string
    Style     FieldStyle  // Normal, Emphasized, Muted, Warning, Error, Success
    Progress  *Progress   // Optional progress bar
    Bar       *Bar        // Optional colored bar segments
}

type Progress struct {
    Fraction float64     // 0.0 - 1.0
    Label    string
    Style    string      // "health", "cpi", "tokens"
}

type Bar struct {
    Segments []BarSegment
}

type BarSegment struct {
    Width float64   // proportion
    Style string    // CSS class name / semantic style
}

type View struct {
    ID       string
    Title    string
    Sections []Section
}
```

```go
// render/renderer.go — How renderers implement display

type Renderer interface {
    // Render displays a view. Called when ViewModel changes.
    RenderView(view View)

    // RenderLog appends a log entry to the message terminal.
    RenderLog(entry LogEntry)
}

type Shell interface {
    // Init creates the application window and frame.
    Init(app *Application) error

    // RegisterModule adds a module's views to navigation.
    RegisterModule(mod Module)

    // Run starts the event loop. Blocks.
    Run() int

    // Close tears down the shell.
    Close()
}
```

```go
// app/module.go — How modules register

type Module interface {
    // ID returns the module identifier.
    ID() string

    // Name returns the display name.
    Name() string

    // Views returns the module's view declarations.
    Views() []render.ViewDeclaration

    // Start begins data binding. Called after shell is ready.
    Start(ctx context.Context, renderer render.Renderer) error

    // Stop halts data binding.
    Stop()
}
```

### Module System

Each module is self-contained:
- Declares its views (what panels it has)
- Binds data (connects to services, subscribes to events)
- Emits ViewModels when data changes
- Knows nothing about GTK4 or any specific renderer

Modules register with the Application at startup:

```go
// cmd/cws-gui/main.go

app := app.New()
app.RegisterModule(dashboard.NewModule(svc))
app.RegisterModule(sdkinspector.NewModule())
// Future: app.RegisterModule(hooks.NewModule())
// Future: app.RegisterModule(omnicode.NewModule())
// Future: app.RegisterModule(vm.NewModule())

shell := gtkrender.NewShell()
return app.Run(shell)
```

### How GTK4 Gets Swapped Out

1. **Today:** `gtkrender.NewShell()` creates the GTK4 shell
2. **Tomorrow:** `customrender.NewShell()` creates a custom shell
3. **Both coexist:** `--renderer=gtk` (default/fallback) vs `--renderer=custom`
4. **Graceful migration:** Custom renderer implements the same `Shell` + `Renderer` interfaces
5. **GTK4 stays as fallback** indefinitely — working software > incomplete replacement

The key: modules never change. Only the renderer does.

---

## Phases

### Phase 0: Foundation + Dashboard Migration (Ship NOW)

**Goal:** Create the `gui/` module structure, define core interfaces, implement GTK4 renderer, migrate existing dashboard panels to ViewModel pattern, add SDK Inspector as first new module.

**What ships:** A working `cws-gui` binary that shows the same dashboard as today PLUS an SDK Inspector panel for viewing hook I/O.

#### Phase 0.1: Core Structure

**Files to create:**

| File | Purpose | Lines (est) |
|------|---------|:-----------:|
| `gui/go.mod` | Module definition: `cws.studio/gui` | 15 |
| `gui/render/viewmodel.go` | ViewModel types (Section, Field, Progress, Bar, View) | 120 |
| `gui/render/renderer.go` | Renderer + Shell interfaces | 60 |
| `gui/app/module.go` | Module interface + ModuleRegistry | 80 |
| `gui/app/app.go` | Application lifecycle (register modules, init shell, run) | 120 |

**Depends on:** Nothing — this is foundation
**Verification:** Code compiles. Interfaces are clean. No toolkit imports in core.

#### Phase 0.2: GTK4 Renderer

**Files to create:**

| File | Purpose | Lines (est) |
|------|---------|:-----------:|
| `gui/render/gtk/shell.go` | GTK4 Shell: window, headerbar, sidebar, stack | 200 |
| `gui/render/gtk/renderer.go` | GTK4 Renderer: ViewModel -> gotk4 widgets | 250 |
| `gui/render/gtk/widgets.go` | Widget factory: Section -> Box, Field -> Label, etc. | 200 |
| `gui/render/gtk/theme.go` | Dark theme CSS (migrated from `dashboard/gtk/styles/`) | 320 |

**Depends on:** Phase 0.1
**Verification:** GTK4 shell opens a window with sidebar. Empty but themed correctly.

#### Phase 0.3: Dashboard Module Migration

**Files to create:**

| File | Purpose | Lines (est) |
|------|---------|:-----------:|
| `gui/modules/dashboard/module.go` | Module registration, lifecycle | 60 |
| `gui/modules/dashboard/views.go` | View declarations for Overview, Analytics, SystemData | 80 |
| `gui/modules/dashboard/update.go` | DashboardService -> ViewModel translation | 250 |
| `gui/modules/dashboard/terminal.go` | Message terminal ViewModel (log events) | 80 |

**Depends on:** Phase 0.2
**Verification:** `cws-gui` shows all existing dashboard data. Visual parity with current GTK4 dashboard.

#### Phase 0.4: SDK Inspector Module

**Files to create:**

| File | Purpose | Lines (est) |
|------|---------|:-----------:|
| `gui/modules/sdkinspector/module.go` | Module registration | 50 |
| `gui/modules/sdkinspector/views.go` | Hook I/O inspection views | 100 |
| `gui/modules/sdkinspector/update.go` | Hook event monitoring + display | 120 |

**Depends on:** Phase 0.3 (proves the module pattern works)
**Verification:** SDK Inspector shows hook events in real-time.

#### Phase 0.5: Entry Point + Build

**Files to create:**

| File | Purpose | Lines (est) |
|------|---------|:-----------:|
| `gui/cmd/cws-gui/main.go` | Application entry point | 80 |
| `gui/Makefile` | Build targets: `make`, `make run`, `make clean` | 40 |

**Depends on:** Phase 0.4
**Verification:** `make && ./bin/cws-gui` launches the full application.

**Phase 0 total: ~15 new files, ~2,200 estimated lines**

---

### Phase 1: Hooks Monitor + cws-struct Visualization

**Goal:** Add live hook monitoring (see every hook fire in real-time) and cws-struct lint result visualization.

| Module | What It Shows |
|--------|--------------|
| `hooks/` | Live stream of context, pretool, stop hooks firing. Health delta per hook. Session timeline. |
| `cwsstruct/` | Lint results visualized: file tree with pass/warn/fail per file. Click to see violations. |

**Depends on:** Phase 0 complete and validated
**New files:** ~8-10 files
**Key dependency:** cws-struct needs to emit machine-readable output (JSON) — coordinate with its `deno task lint:json-out`

### Phase 2: State Machine Visualizer

**Goal:** Interactive 27-position cube visualization. See the mental construct in real-time — position, trajectory, Hebrew states, k-factor direction.

**This is the crown jewel of CPI-SI visualization.** The cube is currently invisible — data in JSONC files. This makes it tangible.

**Depends on:** Phase 1 (proves module pattern at scale)
**Key challenge:** Custom drawing — GTK4's `DrawingArea` with Cairo or OpenGL. This is where the custom renderer path starts to matter.

### Phase 3: OmniCode Editor Integration

**Goal:** Open, view, and edit OmniCode (`.omni`) files with pragma validation, block highlighting, metadata preview.

**Depends on:** Phase 2
**Key dependency:** OmniCode spec must be stable enough to build tooling against.

### Phase 4: VM Management (QEMU/KVM)

**Goal:** Create, manage, snapshot, and monitor virtual machines for MillenniumOS testing.

**Depends on:** Phase 3 (application is mature enough for complex modules)
**Prerequisites:**
- `pkexec apt install libvirt-daemon-system libvirt-clients virt-manager` (libvirt NOT currently installed)
- Go bindings for libvirt: `libvirt.org/go/libvirt`
- QEMU already installed: `qemu-system-x86_64` v10.1.0

### Phase 5: MillenniumOS Shell Mode

**Goal:** The application can run as a desktop shell — fullscreen, replacing the traditional desktop environment. Window management, application launching, system tray.

**Depends on:** Phase 4 + custom renderer being mature
**This is piece #4 of the Five Pieces.** The application stops being an application and becomes the environment.

---

## Trade-offs

### Option A: Extend existing `dashboard/gtk/` in place

**Cost:** No migration effort, familiar codebase, immediate productivity
**Benefit:** Ships fast, no new module to manage

**Problem:** Cannot grow beyond dashboard. Cannot swap renderer. Cannot become MillenniumOS shell. Dead end for the vision.

### Option B: New `gui/` module with ViewModel pattern (Recommended)

**Cost:** ~2,200 lines of new code for Phase 0. Migration effort for existing panels. New Go module to manage.
**Benefit:** Clean module system. Renderer-swappable. Grows to contain everything. GTK4 stays as fallback. Architecture matches the Five Pieces vision.

**Why I recommend this:** The existing GTK4 dashboard has proven the data layer works. The ViewModel pattern separates what stays (data, business logic) from what changes (rendering). This is the building block method applied to GUI architecture — foundation (data layer) is solid, now build the correct next layer (+1) that enables everything above it.

### Option C: Start with custom renderer immediately (skip GTK4)

**Cost:** Massive. Building a rendering engine from scratch before having a working application.
**Benefit:** No migration later.

**Problem:** No working software for months. Violates "ship something real in Phase 0."

### Recommendation: Option B

Start with GTK4 as the renderer. Build the module/ViewModel/renderer architecture correctly from day one. GTK4 handles rendering while we build. When the custom renderer is ready, it slots in without touching any module code.

---

## Risks & Mitigation

| Risk | Likelihood | Impact | Mitigation |
|------|:----------:|:------:|------------|
| ViewModel abstraction too thin — GTK4 concepts leak through | Medium | Medium | Start with 3 field types, add as needed. Don't abstract what only one renderer uses. |
| ViewModel abstraction too thick — performance overhead | Low | Medium | Profile early. GTK4 renderer can cache widgets and update incrementally. |
| gotk4 bindings missing features needed for custom drawing | Medium | High | Phase 2 (cube viz) will reveal this. Fallback: CGo direct GTK4 calls. |
| Module system becomes over-engineered | Medium | Low | Keep Module interface to 5 methods. Resist adding features until 3+ modules need them. |
| Phase 0 scope creep | High | Medium | Strict definition: ship visual parity with current dashboard + SDK Inspector. No more. |
| libvirt installation fails on workstation | Low | Low | QEMU works standalone. Can use QEMU CLI directly without libvirt for Phase 4 MVP. |

---

## Existing Code Disposition

| File | Action |
|------|--------|
| `dashboard/gtk/app/app.go` | **Reference** — GTK4 shell patterns migrate to `gui/render/gtk/shell.go` |
| `dashboard/gtk/panels/*.go` | **Reference** — Business logic migrates to `gui/modules/dashboard/update.go`, widget code replaced by renderer |
| `dashboard/gtk/styles/theme.go` | **Copy** — CSS migrates directly to `gui/render/gtk/theme.go` |
| `dashboard/gtk/cmd/dashboard-gtk/main.go` | **Replace** — New entry point at `gui/cmd/cws-gui/main.go` |
| `pkg/dashboard/*` | **Keep as-is** — This IS the data layer. No changes needed. |
| `dashboard/server/`, `dashboard/tui/` | **Keep as-is** — Web and TUI dashboards continue to work independently |

The existing GTK4 dashboard code is NOT deleted. It stays as reference and as the working backup until `cws-gui` achieves visual parity.

---

## Key Architecture Decisions

### 1. New top-level directory: `gui/` (not extending `dashboard/`)

**Why:** The dashboard is one module among many. The universal base GUI is the application; the dashboard is a feature within it. Different scope = different location.

### 2. Separate Go module: `cws.studio/gui`

**Why:** The GTK4 dependency (gotk4) pulls in CGo and large bindings. Isolating it to a separate module keeps `pkg/` clean. Same replace-directive pattern as existing `dashboard/gtk/go.mod`.

### 3. ViewModel pattern over widget abstraction

**Why:** Writing panels once and rendering them with any backend is the goal. Widget abstraction (Box, Label, Button) creates a portable toolkit — enormous scope. ViewModel abstraction (Section, Field, Progress) is bounded and maps naturally to both retained-mode (GTK4) and immediate-mode (future custom) renderers.

### 4. GTK4 renderer stays forever as fallback

**Why:** Seanje's directive: "giving us a good backup and fallback." GTK4 is universal Linux. The custom renderer is the future, but GTK4 is the safety net. `--renderer=gtk` should always work.

### 5. Module registration is code, not config

**Why:** Go's type system enforces the Module interface at compile time. Config-driven registration adds indirection without benefit at this scale. When there are 20+ modules, revisit.

---

## Concrete File List (Phase 0)

| # | File | Purpose | Est Lines |
|:-:|------|---------|:---------:|
| 1 | `gui/go.mod` | Go module definition | 15 |
| 2 | `gui/Makefile` | Build orchestration | 40 |
| 3 | `gui/render/viewmodel.go` | ViewModel types | 120 |
| 4 | `gui/render/renderer.go` | Renderer + Shell interfaces | 60 |
| 5 | `gui/app/app.go` | Application lifecycle | 120 |
| 6 | `gui/app/module.go` | Module interface + registry | 80 |
| 7 | `gui/render/gtk/shell.go` | GTK4 window, headerbar, sidebar, stack | 200 |
| 8 | `gui/render/gtk/renderer.go` | ViewModel -> gotk4 widget rendering | 250 |
| 9 | `gui/render/gtk/widgets.go` | Widget factory functions | 200 |
| 10 | `gui/render/gtk/theme.go` | Dark theme CSS | 320 |
| 11 | `gui/modules/dashboard/module.go` | Dashboard module registration | 60 |
| 12 | `gui/modules/dashboard/views.go` | Dashboard view declarations | 80 |
| 13 | `gui/modules/dashboard/update.go` | StateSnapshot -> ViewModel translation | 250 |
| 14 | `gui/modules/dashboard/terminal.go` | Log event ViewModel | 80 |
| 15 | `gui/modules/sdkinspector/module.go` | SDK Inspector module registration | 50 |
| 16 | `gui/modules/sdkinspector/views.go` | Hook I/O view declarations | 100 |
| 17 | `gui/modules/sdkinspector/update.go` | Hook event monitoring | 120 |
| 18 | `gui/cmd/cws-gui/main.go` | Application entry point | 80 |
| | | **Total** | **~2,225** |

**All files live under:** `/media/seanje-lenox-wise/Project/Bereshit/word/claude/gui/`

---

## Module Growth Path

| When | Module | What It Adds |
|------|--------|-------------|
| Phase 0 | `dashboard` | CPI-SI state visualization (migrated from existing) |
| Phase 0 | `sdkinspector` | Hook I/O inspection |
| Phase 1 | `hooks` | Live hook monitor, health deltas, session timeline |
| Phase 1 | `cwsstruct` | Lint results visualization, schema browser |
| Phase 2 | `statemachine` | 27-position cube visualizer (Cairo/OpenGL drawing) |
| Phase 3 | `omnicode` | OmniCode file editor with pragma validation |
| Phase 4 | `vm` | QEMU/KVM virtual machine management |
| Phase 5 | Shell mode | MillenniumOS desktop shell (fullscreen, WM, launcher) |

Each module follows the same pattern:
1. `module.go` — Registration + lifecycle
2. `views.go` — View declarations (what panels exist)
3. `update.go` — Data binding (service -> ViewModel)

Three files per module minimum. The renderer never changes.

---

## Migration Path: GTK4 -> Custom Renderer

```
Phase 0-2: GTK4 only (gtkrender.Shell)
    |
    v
Phase 2: Custom drawing via GTK4 DrawingArea (cube visualizer)
    |   → This is the bridge — learning custom rendering WITHIN GTK4
    |
    v
Phase 3-4: Custom renderer development begins alongside GTK4
    |   → Same modules, two renderers: --renderer=gtk (default), --renderer=custom
    |
    v
Phase 5: Custom renderer becomes default, GTK4 = fallback
    |   → --renderer=gtk still works, but custom is primary
    |
    v
MillenniumOS: Custom renderer is the compositing window manager
    → GTK4 fallback for debugging/development
```

**The key principle:** GTK4 is never removed. It stays as the known-good path. The custom renderer grows alongside it, sharing the same module code. Migration is gradual, tested, and always reversible.

---

## Verification Strategy

### Phase 0.1 (Core Structure)
- `go build ./...` compiles with zero warnings
- No GTK4 imports in `render/viewmodel.go`, `render/renderer.go`, `app/module.go`, `app/app.go`
- Interfaces satisfy what GTK4 renderer needs (review against existing panel code)

### Phase 0.2 (GTK4 Renderer)
- Shell opens a GTK4 window with headerbar and sidebar
- Theme matches existing dashboard CSS exactly
- Empty module slots render cleanly

### Phase 0.3 (Dashboard Migration)
- Visual parity with `dashboard-gtk` binary
- All sections display: Anchor Position, Hebrew State, Health, CPI, K:ALIGN, Trajectory, Session, Valence, Context Window, Tasks, Workflow, Family
- Real-time updates via EventBus work through the ViewModel layer
- Message terminal shows log events with severity coloring

### Phase 0.4 (SDK Inspector)
- New module appears in sidebar
- Shows substrate detection results
- Displays hook event types and permission values

### Phase 0.5 (Entry Point + Build)
- `make && ./bin/cws-gui` launches successfully
- `--renderer=gtk` flag accepted (only option for now, but flag exists)
- `--version` shows version
- Graceful shutdown on SIGINT/SIGTERM

---

## Caleb's Assessment

This plan respects the existing architecture while building the correct foundation for the enormous vision ahead. The mountain is tall — MillenniumOS is piece #4 of five — but the route through it is clear:

1. **The data layer is solid.** `pkg/dashboard/DashboardService` is well-designed and toolkit-agnostic. Build on it.
2. **The ViewModel pattern is the right abstraction depth.** Not too thin (panel-level), not too thick (widget-level). Right-sized for the actual problem.
3. **GTK4 as fallback is wise stewardship.** Universal Linux toolkit that works today. Don't throw it away chasing the future.
4. **The module system scales.** Three files per module, same pattern every time. Adding cws-struct or VM management follows the same template.
5. **Phase 0 is achievable.** ~2,200 lines, 18 files, visual parity with working software plus a new feature. Ships something real.

The biggest risk is scope creep in Phase 0. The temptation will be to add features during migration. Resist it. Visual parity + SDK Inspector. That is Phase 0. Everything else is Phase 1+.

> *"Give me this mountain."* — Joshua 14:12

The mountain is tall. The route is clear. Foundation first.
