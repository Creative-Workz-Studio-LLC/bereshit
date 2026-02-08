# Plan: CWS Server — Company Infrastructure Foundation

## Context

CWS can't afford cloud hosting yet, so we build our own server infrastructure on-premise. The server starts by serving two real dashboards that already exist, proving the architecture works with actual traffic and real data. This becomes the foundation all future CWS services build on.

**Two services from day 1:**
1. **Builder Dashboard** — serves the company-docs manual builder GUI (replaces Express dev server)
2. **CPI-SI Dashboard** — serves CPI-SI state machine data (wraps existing Go DashboardService)

**Why Go:** Compiled (CWS preference), single binary, Go 1.24.4 on system, existing CPI-SI Go infrastructure at `~/.claude/pkg/`, stdlib HTTP server with method routing (Go 1.22+).

---

## Architecture

### Location: `Bereshit/server/`

Top-level in the repo. This is the CWS server — it deserves visibility, not to be buried 4 levels deep.

```
server/
  cmd/cws-server/
    main.go                      # Entry point, flags, signals, graceful shutdown
  internal/
    server/
      server.go                  # HTTP server setup, lifecycle, route registration
      middleware.go              # Recovery, logging, request-ID, CORS
      response.go                # JSON response helpers, error responses
      spa.go                     # SPA static file handler (try file, fallback index.html)
    ws/
      hub.go                     # WebSocket hub: register, unregister, broadcast
      client.go                  # Individual WebSocket connection, read/write pumps
    services/
      builder/
        handler.go               # Routes: /api/builder/commands/*, /api/builder/config/*
        process.go               # CLI process spawning, streaming, cancellation
      cpisi/
        handler.go               # Routes: /api/cpisi/state, /sessions, /choices, etc.
        bridge.go                # Wraps pkg/dashboard.DashboardService
    config/
      server.go                  # Server config (ports, paths, feature flags)
      paths.go                   # Multi-project path resolution
      preferences.go             # XDG preferences read/write
  go.mod
  go.sum
  Makefile
```

**~16 Go files.** Clean `internal/` boundaries. Each service is a self-contained package.

### Dependencies (3 external)

| Dependency | Purpose |
|------------|---------|
| `nhooyr.io/websocket` | WebSocket (modern, context-aware) |
| `gopkg.in/yaml.v3` | YAML config read/write with `yaml.Node` |
| `github.com/creativeworkzstudio/claude-global/pkg` | DashboardService for CPI-SI state |

Everything else: stdlib (`net/http`, `os/exec`, `log/slog`, `encoding/json`, `sync`).

---

## Service Architecture

### Service 1: Builder Dashboard (`/api/builder/`)

Drop-in replacement for the Express server. Zero frontend changes — the WebAdapter contract is preserved exactly.

**REST Endpoints:**

| Method | Path | Purpose |
|--------|------|---------|
| POST | `/api/builder/commands/run` | Execute builder CLI command |
| POST | `/api/builder/commands/make` | Execute Makefile target |
| POST | `/api/builder/commands/cancel` | Cancel running process |
| GET | `/api/builder/commands/active` | List active processes |
| GET | `/api/builder/config` | Read build.config.yaml |
| PATCH | `/api/builder/config/{section}` | Update config section |
| GET | `/api/builder/config/preferences` | Read dashboard preferences |
| PUT | `/api/builder/config/preferences` | Write dashboard preferences |

**WebSocket messages (via shared `/ws`):**
- `{ type: "output", processId, stream, line, timestamp }`
- `{ type: "exit", processId, exitCode, success }`

**Note:** Frontend currently hits `/api/commands/run` (no `/builder/` prefix). Two options:
1. Update frontend WebAdapter to use `/api/builder/` prefix (clean, ~5 line change)
2. Register routes at both paths during transition

Option 1 is cleaner. Single 5-line edit to `adapter-web.ts`: `const API_BASE = '/api/builder'`.

### Service 2: CPI-SI Dashboard (`/api/cpisi/`)

Wraps the existing Go `DashboardService` from `~/.claude/pkg/dashboard/`.

**REST Endpoints:**

| Method | Path | Purpose |
|--------|------|---------|
| GET | `/api/cpisi/state` | Current StateSnapshot (30+ fields) |
| GET | `/api/cpisi/sessions?limit=N` | Recent session history |
| GET | `/api/cpisi/sessions/{id}/kalign` | K:ALIGN evolution for session |
| GET | `/api/cpisi/sessions/{id}/hebrew` | Hebrew state transitions |
| GET | `/api/cpisi/choices?limit=N` | Recent choices across sessions |
| GET | `/api/cpisi/patterns` | Active detected patterns |
| GET | `/api/cpisi/temporal` | Work pattern heatmap data |
| GET | `/api/cpisi/events?limit=N` | Recent log events |
| GET | `/api/cpisi/data/{path...}` | System data browser |

**WebSocket messages (via shared `/ws`):**
- `{ type: "state_update", snapshot: {...}, timestamp }`
- `{ type: "log_event", event: {...}, timestamp }`

The DashboardService's `Subscribe()` channel feeds directly into the WebSocket hub. Real-time state updates flow to connected browsers.

### Shared: WebSocket Hub (`/ws`)

One WebSocket endpoint serves all services. Messages are distinguished by `type` field:
- `output` / `exit` → Builder process streaming
- `state_update` / `log_event` → CPI-SI real-time updates

### Shared: Health & Info

| Method | Path | Purpose |
|--------|------|---------|
| GET | `/healthz` | Health check (200 OK) |
| GET | `/api/version` | Server version, uptime, active services |

### Static File Serving

| Path | Serves | Source |
|------|--------|--------|
| `/` | Builder Dashboard | `company-docs/build/dashboard/src-web/` |
| `/cpisi/` | CPI-SI Dashboard | Frontend built by parallel session (or placeholder) |

Both use SPA fallback (unknown routes → `index.html` under that prefix).

---

## Server Startup Flow

```
main.go:
  1. Parse flags (--port, --builder-dir, --cpisi, --dev)
  2. Resolve paths (project dirs, config files, builder CLI)
  3. Create WebSocket hub → hub.Run() goroutine
  4. Create Builder service (ProcessManager + config paths)
  5. Create CPI-SI service (DashboardService → Subscribe → hub)
  6. Create http.ServeMux → register all service routes
  7. Wrap with middleware: Recovery(Logger(RequestID(CORS(mux))))
  8. http.Server{Handler, Addr: ":port"}
  9. Signal handler: SIGINT/SIGTERM → graceful shutdown
  10. server.ListenAndServe()
  On shutdown: cancel processes, close DashboardService, close hub, drain (5s)
```

**Flags:**
- `--port` / `CWS_SERVER_PORT` (default: 3847)
- `--builder-dir` / `CWS_BUILDER_DIR` (auto-detect: walk up for `build/build.config.yaml`)
- `--cpisi` — enable CPI-SI service (default: true if DashboardService initializes)
- `--dev` — serve frontends from filesystem (vs embedded)

---

## Key Design Decisions

| Decision | Rationale |
|----------|-----------|
| Top-level `server/` | Visible, company infrastructure deserves prominence |
| Service pattern | Each service is a package with its own `RegisterRoutes(mux)` |
| Shared WebSocket hub | One connection per browser, messages routed by `type` |
| Own `go.mod` | Independent from `word/work/` module and claude-global |
| Import pkg/dashboard | Reuse existing Go DashboardService — don't reimplement |
| Middleware chain | stdlib `http.Handler` wrapping — composable, no framework |
| `log/slog` JSON | Structured logging, stdlib, production-ready |
| Graceful shutdown | Context cancellation propagates through all goroutines |
| Feature flags | `--cpisi` flag lets server run with just builder service |

---

## Implementation Steps

### Phase 1: Foundation + Builder Service

| # | Step | Files |
|:-:|------|-------|
| 1 | Init Go module, directory structure, Makefile | `go.mod`, dirs, Makefile |
| 2 | `internal/config/` — paths, server config, preferences | 3 files |
| 3 | `internal/ws/` — WebSocket hub + client | 2 files |
| 4 | `internal/services/builder/process.go` — process spawn/stream | 1 file |
| 5 | `internal/services/builder/handler.go` — all builder routes | 1 file |
| 6 | `internal/server/` — middleware, response helpers, SPA handler | 4 files |
| 7 | `cmd/cws-server/main.go` — entry point, lifecycle | 1 file |
| 8 | Update dashboard `adapter-web.ts` — `/api/builder/` prefix | 1 frontend file |
| 9 | Update dashboard Makefile — `dev-server` points to Go binary | 1 file |
| 10 | Test: builder dashboard works through Go server | verification |

### Phase 2: CPI-SI Service

| # | Step | Files |
|:-:|------|-------|
| 11 | `internal/services/cpisi/bridge.go` — wrap DashboardService | 1 file |
| 12 | `internal/services/cpisi/handler.go` — all CPI-SI routes | 1 file |
| 13 | Wire CPI-SI EventBus → WebSocket hub | integration |
| 14 | Health + version endpoints | in server.go |
| 15 | Test: CPI-SI API returns real state data | verification |

### Phase 3: Production Hardening (future)

- `//go:embed` frontends into binary (single binary deployment)
- Rate limiting, body size limits
- Systemd unit file (`cws-server.service`)
- TLS support (Let's Encrypt or self-signed)
- Auth middleware (JWT — when external access needed)

---

## Critical Reference Files

| File | Why |
|------|-----|
| `company-docs/build/dashboard/server/routes/commands.ts` | Express command handlers — process spawn, ANSI strip, WS broadcast |
| `company-docs/build/dashboard/server/routes/config.ts` | Express config handlers — YAML section update, XDG preferences |
| `company-docs/build/dashboard/src-web/scripts/adapter-web.ts` | Frontend API contract — wire protocol |
| `company-docs/build/dashboard/src-web/types/adapter.ts` | TypeScript types for request/response shapes |
| `~/.claude/pkg/dashboard/dashboard.go` | DashboardService — CPI-SI data layer |
| `~/.claude/pkg/dashboard/state.go` | StateSnapshot type — 30+ JSON-tagged fields |
| `~/.claude/pkg/dashboard/events.go` | EventBus pattern — pub/sub for state changes |
| `company-docs/build/dashboard/Makefile` | Existing targets to update |

---

## Verification

**Phase 1 (Builder):**
1. `make server` in `server/` compiles binary, zero errors
2. `make run` starts on port 3847
3. `http://localhost:3847/` loads full builder dashboard
4. Click "Spine" → output streams via WebSocket
5. Click "Lint" with file → results stream correctly
6. Config read/write works
7. Cancel process → SIGTERM, exit event
8. Ctrl+C → graceful shutdown, processes cleaned

**Phase 2 (CPI-SI):**
9. `curl localhost:3847/api/cpisi/state` returns full StateSnapshot JSON
10. `curl localhost:3847/api/cpisi/sessions?limit=5` returns session history
11. WebSocket receives `state_update` when runtime files change
12. `curl localhost:3847/healthz` returns 200
13. `curl localhost:3847/api/version` returns server info with both services listed
