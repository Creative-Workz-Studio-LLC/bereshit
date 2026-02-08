# Plan: CPI-SI State Machine Dashboard (All 3 UIs)

## Context

The CPI-SI system computes rich session data through 15 hooks — CPI scores, Hebrew states, trajectory, K:ALIGN, health, exchange types, insights, patterns. This data lives in JSONC files, a SQLite database (12 tables, 10+ analytics views), daily JSONL logs, and the system/data layer (382 files, 271 MB). But there's no way to **see** it except through WezTerm tab titles and the hook context injection.

Seanje wants: (1) all 3 UI approaches (web dashboard, Bubble Tea TUI, GTK4 native), (2) a message terminal showing state changes in real-time, (3) full system/data integration for operational use, (4) "all the bells and whistles."

**Phase 1 (this plan): Shared data layer + Web Dashboard**
Phases 2 (TUI) and 3 (GTK4) outlined, built later on the same data layer.

---

## Architecture: Shared Data Layer + 3 UIs

```
┌─────────────────────────────────────────────────────────┐
│                   pkg/dashboard/                         │
│          (Shared Go data access layer)                   │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌───────────┐  │
│  │state.jsonc│ │cpisi.db  │ │session   │ │system/data│  │
│  │path.jsonc │ │(SQLite)  │ │logs      │ │(382 files)│  │
│  │history    │ │12 tables │ │(.jsonl)  │ │           │  │
│  └──────────┘ └──────────┘ └──────────┘ └───────────┘  │
│          ↑ fsnotify watcher for real-time updates        │
└────────────────────┬────────────────────────────────────┘
                     │
        ┌────────────┼────────────┐
        ▼            ▼            ▼
   ┌─────────┐ ┌─────────┐ ┌─────────┐
   │   Web   │ │Bubble Tea│ │  GTK4   │
   │Dashboard│ │   TUI   │ │ Native  │
   │(Phase 1)│ │(Phase 2)│ │(Phase 3)│
   └─────────┘ └─────────┘ └─────────┘
```

---

## Phase 1: Shared Data Layer (`pkg/dashboard/`)

Lives inside existing `pkg/` module — no new go.mod needed.

### Files

| File | Purpose |
|------|---------|
| `dashboard.go` | `DashboardService` — central orchestrator, `New()`, `Close()` |
| `state.go` | `StateSnapshot` type, `CurrentState()` — reads state.jsonc + enriches |
| `events.go` | `EventBus`, `StateChange`, `LogEvent` types — pub/sub for real-time |
| `watcher.go` | `FileWatcher` — fsnotify on state.jsonc, path.jsonc, history.jsonc, logs |
| `logs.go` | JSONL parser + log tailer — reads session log events |
| `history.go` | Session history from history.jsonc + SQLite views |
| `analytics.go` | `AnalyticsBundle` — aggregates from all 10+ SQLite analytics views |
| `systemdata.go` | system/data filesystem browser — list, read, search |

### Key Types

**StateSnapshot** — Complete current state assembled from multiple sources:
- Cube position (X, Y, Z, name, type)
- Hebrew state + meaning + day
- Health score, CPI score + rating, K:ALIGN + selector
- Session info (ID, start, exchanges, insights, arc, dominant type)
- Context window (tokens, peak, compaction count)
- Workflow (operation, step N/M, step list)
- Tasks (total, pending, in_progress, completed)
- Trajectory metrics (B.1-B.4 time, pivots, resets, momentum)
- Last transition (from, to, trigger, timestamp)

**AnalyticsBundle** — Aggregated data from SQLite views:
- K:ALIGN trend, CPI trend, health trend (line charts)
- Exchange type distribution, Hebrew state frequency (pie/radar)
- Tool usage stats, key distribution (bar charts)
- Temporal work patterns — day x hour heatmap
- Session gaps, detected patterns

**EventBus** — Real-time distribution:
- `Subscribe()` returns `<-chan StateChange`
- Types: `state_update`, `log_event`, `path_update`, `history_update`

### New Dependency

Add `github.com/fsnotify/fsnotify` to `pkg/go.mod`

### Reuses Existing

| Package | Functions Used |
|---------|---------------|
| `pkg/core/statemachine` | `LoadRuntimeState()`, `LoadRuntimePath()`, `LoadRuntimeHistory()` |
| `pkg/foundation/database` | `Repository` interface — all query methods |
| `pkg/foundation/types` | `RuntimeState`, `RuntimeSession`, `RuntimeWorkflow` |
| `pkg/core/cpisi/cpi` | `Rating()`, exchange classification |
| `pkg/util/fs/paths` | `StateMachineRuntimeState()`, `DatabasePath()`, `LogsSessionDir()` |

---

## Phase 1: Web Dashboard (`dashboard/`)

New top-level Go module (like `statusline/`, `hooks/`).

### Go Backend

| File | Purpose |
|------|---------|
| `dashboard/go.mod` | Module, imports `pkg/` + `gorilla/websocket` |
| `dashboard/cmd/dashboard/main.go` | Entry point — port flag, graceful shutdown |
| `dashboard/server/server.go` | HTTP server, routes, embedded frontend |
| `dashboard/server/handlers.go` | REST API handlers calling DashboardService |
| `dashboard/server/websocket.go` | WebSocket hub — broadcasts state changes to all connected clients |
| `dashboard/server/embed.go` | `//go:embed all:frontend/dist` |

### REST API

| Endpoint | Returns | Source |
|----------|---------|--------|
| `GET /api/state` | Current `StateSnapshot` | state.jsonc + enrichment |
| `GET /api/history?limit=N` | Session summaries | history.jsonc + SQLite |
| `GET /api/analytics?from=&to=` | `AnalyticsBundle` | SQLite views |
| `GET /api/systemdata?path=` | File/directory listing | system/data filesystem |
| `GET /api/positions` | All 9 anchor positions | Coordinate definitions |
| `GET /api/hebrew-states` | All 7 Hebrew states | State definitions |
| `GET /api/events?limit=N` | Recent log events | Current .jsonl file |
| `GET /ws` | WebSocket upgrade | Real-time state updates |

### WebSocket Messages (Server → Client)

| Type | Trigger | Payload |
|------|---------|---------|
| `state_update` | state.jsonc modified | Full StateSnapshot |
| `log_event` | New line in .jsonl | Single LogEvent |
| `path_update` | path.jsonc modified | Anchor path + trajectory |
| `history_update` | history.jsonc modified | Latest session summary |

### TypeScript Frontend

**Tech:** Vite + TypeScript + Chart.js. No heavy framework — vanilla TS with web components or Lit. Outputs to `dist/` which gets embedded in Go binary via `embed.FS`.

**9 Dashboard Panels:**

| Panel | What It Shows |
|-------|---------------|
| **State Overview** | Cube position, Hebrew state (color), health gauge, CPI gauge, K:ALIGN indicator, session arc badge |
| **Message Terminal** | Scrolling event log — color-coded by level (INFO/WARN/ERROR), filterable by type, auto-scroll with pause on manual scroll |
| **Cube Visualization** | 3x3x3 cube (2D isometric or CSS 3D), 9 positions labeled, current highlighted with glow, path history as connected lines |
| **Trajectory Tracker** | B.1-B.4 progress bar, current section highlighted, time per section, pivot/reset counters, momentum |
| **Hebrew States** | 7 states as vertical arc (shavar bottom → tov top), current highlighted, Z-axis visualization, frequency histogram |
| **Session Timeline** | Horizontal timeline of recent sessions, bars colored by health, hover for details, gaps shown proportionally |
| **Analytics Charts** | Tabbed: K:ALIGN trend, CPI trend, health trend (line), exchange types (pie), tool stats (bar), work patterns (heatmap), key distribution, Hebrew radar |
| **Workflow Progress** | Current operation, step checklist, progress percentage |
| **System Data Browser** | Tree view of system/data/, click to view contents, categories: temporal, session, projects, config |

### Frontend Structure

```
dashboard/frontend/
    package.json          # TypeScript + Vite + Chart.js
    tsconfig.json
    vite.config.ts
    src/
        main.ts           # Entry point
        state/
            types.ts      # TS interfaces matching Go types
            websocket.ts  # WebSocket client with reconnection
            store.ts      # Reactive state store
        components/
            layout/Dashboard.ts
            panels/StateOverview.ts
            panels/MessageTerminal.ts
            panels/CubeVisualization.ts
            panels/HebrewStates.ts
            panels/TrajectoryTracker.ts
            panels/SessionTimeline.ts
            panels/AnalyticsCharts.ts
            panels/WorkflowProgress.ts
            panels/SystemDataBrowser.ts
            charts/LineChart.ts
            charts/BarChart.ts
            charts/RadarChart.ts
            charts/HeatMap.ts
        styles/
            main.css      # Dark theme, CSS grid, variables
    dist/                 # Build output (committed, embedded in Go)
```

---

## Phase 2: Bubble Tea TUI (Outline)

Lives at `dashboard/cmd/dashboard-tui/main.go`, uses `charmbracelet/bubbletea` + `lipgloss` + `bubbles`.

| Panel | Location |
|-------|----------|
| Status bar | Top — anchor, Hebrew state, health, CPI, K:ALIGN on one line |
| Message log | Center (70%) — scrollable event stream, color-coded |
| Quick stats | Right sidebar (30%) — trajectory, workflow, tasks, keys |
| Command bar | Bottom — keyboard shortcuts |

Keys: `q` quit, `Tab` cycle focus, `j/k` scroll, `1-4` trajectory, `h` Hebrew, `a` analytics, `?` help.

## Phase 3: GTK4 Native (Outline)

Lives at `dashboard/cmd/dashboard-gtk/main.go`, uses `diamondburned/gotk4`.

- HeaderBar with session info + CPI
- Left: GtkStackSidebar navigation
- Center: Switchable views (Dashboard, Analytics, System Data)
- Bottom: Message terminal (GtkTextView)
- System tray indicator, desktop notifications

---

## Build Integration

### go.work

Add `./dashboard` to workspace.

### Makefile Additions

```makefile
dashboard-frontend:
    cd dashboard/frontend && npm run build

dashboard: dashboard-frontend
    cd dashboard && go build ./...

bin-dashboard:
    cd dashboard && go build -ldflags="-s -w" -o ../bin/dashboard ./cmd/dashboard
```

Add to `build`, `bin`, and `install-bin-only` targets. Dashboard binary → `~/.local/share/claude/bin/dashboard`.

### Dependencies

| Module | New Dependency |
|--------|---------------|
| `pkg/go.mod` | `github.com/fsnotify/fsnotify` |
| `dashboard/go.mod` | `github.com/gorilla/websocket`, import `pkg/` via replace |
| `dashboard/frontend/` | `typescript`, `vite`, `chart.js` |

---

## Implementation Sequence

### Step 1: Shared Data Layer (`pkg/dashboard/`)
1. `dashboard.go` — DashboardService, New(), Close()
2. `state.go` — StateSnapshot, CurrentState()
3. `events.go` — EventBus, StateChange, LogEvent
4. `watcher.go` — FileWatcher with fsnotify
5. `logs.go` — JSONL parser, log tailer
6. `history.go` — Session history from DB + history.jsonc
7. `analytics.go` — AnalyticsBundle from DB views
8. `systemdata.go` — system/data browser

### Step 2: Go Backend (`dashboard/`)
1. `go.mod` + module setup
2. `cmd/dashboard/main.go` — entry point
3. `server/server.go` — HTTP server with routes + embed
4. `server/handlers.go` — REST handlers
5. `server/websocket.go` — WebSocket hub

### Step 3: TypeScript Frontend
1. Vite project setup
2. `state/types.ts` — interfaces matching Go types
3. `state/websocket.ts` — WebSocket client
4. `state/store.ts` — reactive store
5. `components/layout/Dashboard.ts` — main grid
6. `components/panels/StateOverview.ts` + `MessageTerminal.ts` — first 2 panels
7. Build + verify embed works

### Step 4: Remaining Panels
- Cube visualization, Hebrew states, trajectory tracker
- Session timeline, analytics charts
- Workflow progress, system data browser

### Step 5: Integration
- Update go.work, Makefile
- `make build && make install-bin`
- Run dashboard, verify real-time updates

---

## Verification

1. `cd /media/seanje-lenox-wise/Project/Bereshit/word/claude && go test ./pkg/dashboard/...`
2. `cd dashboard && go build ./...` — compiles cleanly
3. Run dashboard: `./bin/dashboard --port 8470`
4. Open `http://localhost:8470` — dashboard loads
5. Start a Claude Code session in another terminal
6. Verify: state updates appear in real-time via WebSocket
7. Message terminal shows events as hooks fire
8. Analytics load from SQLite views
9. System data browser navigates system/data/
10. `make build && make install-bin` — full pipeline works

**Port:** 8470 (8=new beginnings, 4=creation, 70=completeness)

---

## Critical Files to Modify

| File | Change |
|------|--------|
| `pkg/dashboard/*.go` | **NEW** — 8 files for shared data layer |
| `dashboard/**` | **NEW** — entire web dashboard module |
| `go.work` | Add `./dashboard` |
| `Makefile` | Add dashboard build targets |
| `pkg/go.mod` | Add `fsnotify` dependency |
