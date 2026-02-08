# Plan: CWS Server — Deflatten, Document, Expand

## Context

The CWS Go server (14 files, 2,606 lines) is deployed and working. Seanje asked for three things:
1. **README.adoc** following CWS documentation standards
2. **Support docs** (architecture, API, deployment)
3. **Deflatten code** — split flat handler files, extract duplicated helpers, expand docstrings

Order matters: deflatten first (changes file structure), then comments (targets final files), then docs (references final structure).

---

## Phase 1: Deflatten (4 steps, `make check` gate after each)

### 1.1 Extract shared HTTP helpers → `internal/server/response.go` (NEW)

`decodeJSON`, `writeJSON`, `writeError` are identical in both `builder/handler.go:266-285` and `cpisi/handler.go:359-389`. `queryInt` only in cpisi but belongs with the others.

Export as `server.DecodeJSON`, `server.WriteJSON`, `server.WriteError`, `server.QueryInt`. Both service packages import `"cws.studio/server/internal/server"`.

### 1.2 Split `builder/handler.go` (290 lines → 3 files)

| File | Content | Lines from original |
|------|---------|-----|
| `handler.go` (TRIM) | Service struct, NewService, RegisterRoutes | 37-63 |
| `handler_commands.go` (NEW) | runCommandRequest, handleRunCommand, runMakeRequest, handleRunMake, cancelRequest, handleCancelProcess, handleActiveProcesses | 69-138 |
| `handler_config.go` (NEW) | handleReadConfig, handleUpdateConfigSection, handleReadPreferences, handleWritePreferences | 144-260 |

Remove local helpers (replaced by 1.1). `handler_config.go` keeps yaml and config imports.

### 1.3 Split `cpisi/handler.go` (393 lines → 3 files)

| File | Content | Lines from original |
|------|---------|-----|
| `handler.go` (TRIM) | Service struct, NewService, RegisterRoutes | 34-71 |
| `handler_state.go` (NEW) | 9 state handlers (handleState through handleSystemData) | 77-162 |
| `handler_journal.go` (NEW) | 8 journal handlers + importEntry struct + splitTags helper | 168-353 |

Remove local helpers (replaced by 1.1).

### 1.4 Refactor `builder/process.go` — extract `spawnProcess`

`RunCLI` (88-155) and `RunMake` (158-223) share ~65 identical lines (pipe setup, start, stream, wait, exit broadcast). Extract into private `spawnProcess(cmd *exec.Cmd) string`. `RunCLI` and `RunMake` become thin wrappers that build `exec.Cmd` then call `spawnProcess`.

---

## Phase 2: Expand Docstrings & Comments (all 19 files)

Every file gets:
- **Biblical reference** in METADATA (most are missing this — main.go has Nehemiah 2:18, extend to all)
- **Package doc comment** on primary file per package
- **Group-purpose comments** before `───` section dividers
- **Field comments** on struct fields that lack them
- **Inline comments** on non-obvious logic (YAML node manipulation, process group kill, FTS5 triggers, SPA fallback logic)

Key specific additions:

| File | Additions |
|------|-----------|
| `config/paths.go` | Document upward-walk algorithm, XDG compliance |
| `config/preferences.go` | Note "missing file is not error" design choice |
| `server/middleware.go` | Middleware ordering contract, CORS dev-mode note |
| `server/spa.go` | SPA fallback decision tree (file exists → serve; has ext → 404; no ext → index.html) |
| `ws/hub.go` | Channel buffer sizes (256), slow client drop, shutdown |
| `ws/client.go` | Write pump timeout rationale, readPump future expansion |
| `builder/process.go` | Process group management (Setpgid + negative PID), ANSI stripping rationale |
| `cpisi/bridge.go` | Event streaming lifecycle, journal store optionality |
| `cpisi/journal.go` | FTS5 sync trigger pattern, WAL mode choice, ID generation format |

---

## Phase 3: Documentation (4 AsciiDoc files)

### 3.1 `server/README.adoc` — Primary project README

Follow builder README pattern (practical, not full 5-block template):
- AsciiDoc attributes, `.lead` paragraph, biblical sidebar (Nehemiah 2:18)
- Overview (unified server, 3 services, key tech)
- Quick Start (`make build && make run`, `make deploy`)
- Architecture overview (service composition, host routing, WebSocket hub)
- Services section (Builder, CPI-SI, Website — route prefixes, key features)
- API summary table (all endpoints)
- Deployment section (production target, partial deploys, health monitoring)
- Project structure tree (final 19-file layout)

### 3.2 `server/docs/ARCHITECTURE.adoc`

- Service composition diagram (main.go wiring)
- Request flow: HTTP → middleware chain → host router → service handler → response
- WebSocket lifecycle: upgrade → Hub → writePump/readPump → broadcast
- CPI-SI bridge: DashboardService → EventBus → Bridge.streamEvents → Hub
- Process management: spawn → streamPipe → Wait → ExitMessage
- Configuration resolution: flags → env → defaults → auto-detect
- Caching strategy by content type

### 3.3 `server/docs/API.adoc`

Complete endpoint reference:
- Builder: 8 endpoints (commands CRUD, config CRUD, preferences)
- CPI-SI: 9 state endpoints + 8 journal endpoints
- Shared: /healthz, /api/version, /ws
- WebSocket protocol (OutputMessage, ExitMessage, state_update)
- Request/response JSON schemas

### 3.4 `server/docs/DEPLOYMENT.adoc`

- Production server (Dell Inspiron, seanje@192.168.1.99)
- systemd service + Cloudflare tunnel
- Deploy workflow (full + partial)
- Health monitoring (healthcheck.sh, cron, prod-health)
- Troubleshooting guide

---

## Final File Structure (19 Go + 4 docs)

```
server/
├── README.adoc                              # NEW (Phase 3)
├── Makefile
├── go.mod / go.sum
├── cmd/cws-server/
│   └── main.go                              # Phase 2 comments
├── internal/
│   ├── config/
│   │   ├── server.go                        # Phase 2 comments
│   │   ├── paths.go                         # Phase 2 comments
│   │   └── preferences.go                   # Phase 2 comments
│   ├── server/
│   │   ├── hostrouter.go                    # Phase 2 comments
│   │   ├── middleware.go                    # Phase 2 comments
│   │   ├── response.go                      # NEW (Phase 1.1)
│   │   └── spa.go                           # Phase 2 comments
│   ├── services/
│   │   ├── builder/
│   │   │   ├── handler.go                   # TRIMMED (Phase 1.2)
│   │   │   ├── handler_commands.go          # NEW (Phase 1.2)
│   │   │   ├── handler_config.go            # NEW (Phase 1.2)
│   │   │   └── process.go                   # REFACTORED (Phase 1.4)
│   │   └── cpisi/
│   │       ├── handler.go                   # TRIMMED (Phase 1.3)
│   │       ├── handler_state.go             # NEW (Phase 1.3)
│   │       ├── handler_journal.go           # NEW (Phase 1.3)
│   │       ├── bridge.go                    # Phase 2 comments
│   │       └── journal.go                   # Phase 2 comments
│   └── ws/
│       ├── hub.go                           # Phase 2 comments
│       └── client.go                        # Phase 2 comments
├── docs/
│   ├── ARCHITECTURE.adoc                    # NEW (Phase 3)
│   ├── API.adoc                             # NEW (Phase 3)
│   └── DEPLOYMENT.adoc                      # NEW (Phase 3)
├── scripts/healthcheck.sh
└── website/
```

## Verification

- **After each Phase 1 step:** `make check` (go vet + build)
- **After Phase 2:** `make check` (comments don't break compilation, but verify no typos in code)
- **After Phase 3:** Review docs for accuracy against final code structure
- **Final:** `make deploy` to production
