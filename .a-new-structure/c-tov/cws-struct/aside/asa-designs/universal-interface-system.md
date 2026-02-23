# Universal Interface System — Design Document

> *"Write the vision, and make it plain upon tables, that he may run that readeth it."* -- Habakkuk 2:2

**Author:** Asa Mishchaq (Game Design)
**Date:** 2026-02-22
**Status:** Exploration / Pre-Approval
**Scope:** Production-grade unified CLI + TUI + GUI + Web tooling for workstation and Oracle VM

---

## The Problem

CWS has five separate tools with five separate interfaces:

| Tool | Interface | Language | Where It Runs |
|------|-----------|----------|---------------|
| cws-struct | CLI (Deno) | TypeScript | Workstation only |
| TS Builder | CLI (Node) | TypeScript | Workstation only |
| cws-server | HTTP API + WebSocket | Go | Oracle VM (ARM64) |
| CWS Dashboard | Desktop GUI (Tauri v2) | Rust + TS | Workstation only |
| ops-frontend | Web | TS + Vite | Browser via tunnel |

Five tools. Five integration points. Five places where output formatting, error handling, and user experience diverge. No TUI. No way to get a live dashboard over SSH to the VM. No single command that unifies all operations.

The question isn't "should we build a unified interface?" It's "what architecture lets us build it once and render it everywhere?"

---

## The Game Design Insight

This is a **multi-platform controller problem**.

Every game that ships on PC, console, and mobile faces the same challenge: one game engine, multiple input/output devices. The industry solved this decades ago with a pattern so fundamental it doesn't even have a fancy name anymore. It's just... how you build games.

```
INPUT DEVICE    -->    GAME ENGINE    -->    RENDERER
(keyboard)             (game logic)          (monitor)
(gamepad)              (same logic)          (TV)
(touchscreen)          (same logic)          (phone)
```

The game engine doesn't know or care which input device sent the command. It processes typed actions and returns typed state changes. The input adapter translates device-specific events into game actions. The renderer translates game state into device-specific output.

**Apply this to CWS tooling:**

```
INPUT ADAPTER   -->    COMMAND BUS    -->    OUTPUT ADAPTER
(CLI flags)            (typed actions)       (stdout text)
(TUI keypresses)       (same logic)          (BubbleTea views)
(GUI clicks)           (same logic)          (HTML/Tauri)
(HTTP requests)        (same logic)          (JSON responses)
```

One bus. Four renderers. Zero divergence in business logic.

---

## Architecture

### Layer Diagram

```
┌─────────────────────────────────────────────────────────┐
│                    PRESENTATION LAYER                    │
│                                                         │
│  ┌──────┐  ┌──────────┐  ┌──────────┐  ┌───────────┐  │
│  │ CLI  │  │   TUI    │  │   GUI    │  │    Web    │  │
│  │cobra │  │BubbleTea │  │  Tauri   │  │ops-front  │  │
│  └──┬───┘  └────┬─────┘  └────┬─────┘  └─────┬─────┘  │
│     │           │              │               │        │
│     └───────────┴──────┬───────┴───────────────┘        │
│                        │                                │
├────────────────────────┼────────────────────────────────┤
│                   API LAYER                             │
│                        │                                │
│              ┌─────────▼──────────┐                     │
│              │    COMMAND BUS     │                      │
│              │                    │                      │
│              │  Command{Kind,     │                      │
│              │    Target, Opts,   │                      │
│              │    Stream}         │                      │
│              │         │          │                      │
│              │    ┌────▼────┐     │                      │
│              │    │ Router  │     │                      │
│              │    └────┬────┘     │                      │
│              │         │          │                      │
│              │  Result{Status,    │                      │
│              │    Output, Score,  │                      │
│              │    Duration}       │                      │
│              └─────────┬──────────┘                      │
│                        │                                │
├────────────────────────┼────────────────────────────────┤
│                  ENGINE LAYER                           │
│                        │                                │
│    ┌──────────┬────────┼────────┬──────────┐            │
│    │          │        │        │          │            │
│  ┌─▼──┐  ┌───▼──┐  ┌──▼──┐  ┌─▼───┐  ┌──▼──┐         │
│  │Lint│  │Build │  │Ops  │  │Heal │  │Info │         │
│  │    │  │      │  │     │  │     │  │     │         │
│  │cws-│  │TS    │  │ssh, │  │score│  │ver, │         │
│  │stru│  │build │  │svc  │  │diag │  │help │         │
│  │ct  │  │er    │  │mgmt │  │     │  │     │         │
│  └────┘  └──────┘  └─────┘  └─────┘  └─────┘         │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

### Three Layers, Clean Boundaries

| Layer | Responsibility | Changes When |
|-------|---------------|--------------|
| **Presentation** | Input/output adaptation per device | New interface needed |
| **API** | Command typing, routing, result shaping | New command kind added |
| **Engine** | Business logic execution | Tool behavior changes |

**The rule:** Presentation never touches Engine directly. API is the only bridge. This is the same rule that prevents game controllers from manipulating physics directly.

---

## The Command Bus

### Command Type

```go
// Package bus defines the universal command interface.
//
// Every user action across every interface maps to a Command.
// The bus routes commands to engines and returns Results.
// Presentation adapters translate device I/O to/from these types.
package bus

import "time"

// CommandKind identifies what class of operation is requested.
type CommandKind int

const (
    // Lint — structural validation (wraps cws-struct)
    CmdLint CommandKind = iota
    // Build — document compilation (wraps TS builder)
    CmdBuild
    // Deploy — push artifacts to targets
    CmdDeploy
    // Status — query service/system health
    CmdStatus
    // Health — score structural alignment of files/directories
    CmdHealth
    // Transform — apply fixes from lint suggestions
    CmdTransform
    // Create — scaffold new files from templates
    CmdCreate
    // Verify — environment and dependency checks
    CmdVerify
    // Ops — server operations (restart, logs, config)
    CmdOps
)

// Command is the atomic unit of the bus.
// Every button press, CLI flag, TUI keypress, or HTTP request
// becomes one of these before the engine sees it.
type Command struct {
    Kind    CommandKind        // What class of action
    Target  string             // What it acts on (file, service, etc.)
    Args    []string           // Positional arguments
    Opts    map[string]any     // Named options
    Stream  bool               // Whether to stream output progressively
}
```

### Result Type

```go
// ResultStatus indicates outcome category.
type ResultStatus int

const (
    StatusSuccess ResultStatus = iota
    StatusFailure
    StatusPartial   // Some items succeeded, some failed
    StatusSkipped   // Action not applicable in this context
)

// OutputLine is one unit of structured output.
// Rich enough for TUI rendering, simple enough for CLI text.
type OutputLine struct {
    Level   string // "info", "warn", "error", "success", "debug"
    Source  string // Which engine produced this (e.g., "lint/go/R25")
    Message string // Human-readable text
    Data    any    // Optional structured data for programmatic consumers
}

// HealthScore carries the Hebrew state assessment when applicable.
type HealthScore struct {
    Score     float64 // 0.0 to 1.0
    State     string  // Hebrew state name: "broken" through "perfect"
    Direction int     // -1 (misaligned), 0 (neutral), +1 (aligned)
}

// Result is what the bus returns after engine execution.
type Result struct {
    Status   ResultStatus
    Output   []OutputLine
    Score    *HealthScore    // nil when not applicable
    Duration time.Duration
    Error    error           // nil on success
}
```

### Router

```go
// Router maps CommandKind to Engine implementations.
// Engines register at startup. The router is the only place
// that knows which engines exist.
type Router struct {
    engines map[CommandKind]Engine
}

// Engine processes one kind of command.
// Each engine wraps one underlying tool.
type Engine interface {
    // Execute runs the command and returns a result.
    // If cmd.Stream is true, output lines are sent progressively
    // via the provided channel (nil if Stream is false).
    Execute(cmd Command, stream chan<- OutputLine) (Result, error)

    // Available reports whether this engine can run in the current context.
    // A lint engine on the VM without Deno installed returns false.
    Available() bool

    // Description returns human-readable info for help text.
    Description() string
}
```

**Why this works:** Adding a new tool means implementing one `Engine` interface and registering it with the router. No presentation code changes. No API code changes. The new engine appears in CLI help, TUI menus, and GUI panels automatically because they all query the router.

---

## The Five Modules

### Module 1: `cws` CLI (cobra)

The command-line interface. One binary, subcommands map to CommandKind.

```
cws lint go src/parser.rs              # CmdLint
cws lint rust word/work/pkg/trit/      # CmdLint
cws build manual                       # CmdBuild
cws deploy oracle                      # CmdDeploy
cws deploy dell                        # CmdDeploy
cws status                             # CmdStatus (all services)
cws status oracle                      # CmdStatus (specific)
cws health word/work/pkg/trit/         # CmdHealth
cws transform go src/parser.rs         # CmdTransform
cws create go library src/new.rs       # CmdCreate
cws verify                             # CmdVerify (environment)
cws ops restart cws-server             # CmdOps
cws ops logs cws-server --tail 50      # CmdOps
```

**Output modes** (controlled by flags, not by rewriting rendering):

| Flag | Output | Consumer |
|------|--------|----------|
| (default) | Colored text with Unicode indicators | Human at terminal |
| `--json` | Structured JSON, one object per line | Scripts, pipes, TUI |
| `--quiet` | Exit code only | CI/CD, makefiles |
| `--verbose` | Full detail including debug lines | Debugging |

cobra handles flag parsing and subcommand routing. Each subcommand constructs a `Command` and passes it to the bus. The bus returns a `Result`. The CLI adapter formats the result for stdout.

### Module 2: `cws tui` (BubbleTea)

The terminal UI. Same binary, `tui` subcommand launches BubbleTea.

```
cws tui                   # Full dashboard (tab-based)
cws tui status            # Service monitor view
cws tui lint              # Lint results view
cws tui health            # Health score dashboard
```

**Why BubbleTea:**
- Pure Go (same binary, no separate install)
- Elm Architecture (Model-Update-View) matches our Command-Engine-Result pattern exactly
- Works over SSH (the killer feature for the Oracle VM)
- Mature, well-documented, active community (Charm ecosystem)

**Views:**

| View | What It Shows | Updates |
|------|--------------|---------|
| **Status** | Service health for all monitored endpoints | Polling every 5s |
| **Lint** | Last lint results with Hebrew state indicators | On command |
| **Health** | Directory/file health scores in tree view | On command |
| **Logs** | Live log stream from cws-server | WebSocket |
| **Build** | Builder progress and output | On command |

**The BubbleTea model consumes the same `Result` type as CLI.** The difference is rendering: CLI prints lines, TUI updates a viewport with styled components. Same data.

```
ssh cws-oracle cws tui status
```

That command gives Seanje a live dashboard of the Oracle VM. Over SSH. No browser. No port forwarding. No tunnel configuration. Just a Go binary that renders in any terminal.

### Module 3: `cws serve` (HTTP + WebSocket)

The API server. Exposes the command bus over HTTP for GUI and web consumers.

```
cws serve                      # Default port 3848
cws serve --port 3850          # Custom port
```

**Endpoints:**

| Method | Path | Maps To |
|--------|------|---------|
| POST | `/api/v1/lint` | CmdLint |
| POST | `/api/v1/build` | CmdBuild |
| POST | `/api/v1/deploy` | CmdDeploy |
| GET | `/api/v1/status` | CmdStatus |
| POST | `/api/v1/health` | CmdHealth |
| POST | `/api/v1/transform` | CmdTransform |
| POST | `/api/v1/create` | CmdCreate |
| GET | `/api/v1/verify` | CmdVerify |
| POST | `/api/v1/ops` | CmdOps |
| WS | `/api/v1/stream` | Streaming output channel |

**Request body:** JSON representation of `Command`.
**Response body:** JSON representation of `Result`.
**WebSocket:** For `Stream: true` commands, output lines arrive as they're produced.

This is the evolution of cws-server. The existing server already handles HTTP and WebSocket. The new version adds the command bus API alongside the existing routes. No breaking changes to what's deployed.

### Module 4: Tauri Desktop GUI (existing)

Already scaffolded at `company-docs/build/dashboard/`. The adapter pattern is already built (TauriAdapter / WebAdapter). Instead of the Tauri backend spawning processes directly, it calls `cws serve` via HTTP/WebSocket.

**What changes:**
- Tauri backend (Rust) becomes thin: just proxies to `cws serve`
- Frontend (TS + Web Components) renders `Result` objects from the API
- Same data, same structure, native desktop chrome

**What stays:**
- File dialogs (Tauri native)
- System menu (already built)
- State management (already built)

### Module 5: Web Dashboard (existing)

ops-frontend already exists in `server/ops-frontend/`. Consumes the same `cws serve` API. Deployed behind the Cloudflare tunnel for remote access.

---

## Form System (Deployment Contexts)

The tool itself uses a form system — different capabilities active in different contexts. This mirrors the cws-struct form architecture exactly.

```go
// Form determines which engines are available in this context.
type Form struct {
    Name        string
    Description string
    Engines     map[CommandKind]FormStatus
}

type FormStatus int

const (
    FormRequired  FormStatus = iota // Must be available, warn if not
    FormAvailable                    // Available if engine reports ready
    FormReserved                     // Not applicable in this context
)
```

| Form | Context | Required | Available | Reserved |
|------|---------|----------|-----------|----------|
| **workstation** | Local dev (Ubuntu 25.10) | lint, build, health, verify, create, transform | deploy, status, ops, serve, tui | -- |
| **vm** | Oracle server (ARM64) | status, ops, serve | deploy, health, verify | lint, build, create, transform |
| **ci** | GitHub Actions | lint, build, verify, health | -- | deploy, status, ops, serve, tui |
| **remote** | SSH into VM | tui, status | ops | lint, build, deploy, create, transform, serve |

**Detection:** The binary detects its form from environment:
- `CWS_FORM=vm` explicit override
- Hostname-based (`cws-oracle` -> vm form)
- Filesystem-based (Deno present -> workstation; absent -> vm)
- Fallback: workstation

**Why forms matter:** The same binary deployed everywhere. On the workstation it's a full development tool. On the VM it's an operations dashboard. In CI it's a headless validator. Over SSH it's a TUI monitor. Same code, different capabilities. This is the class system from cws-struct applied to infrastructure.

---

## Engine Implementations

### Lint Engine (wraps cws-struct)

```go
type LintEngine struct {
    denoPath   string // Path to Deno binary
    structPath string // Path to cws-struct directory
}

func (e *LintEngine) Execute(cmd Command, stream chan<- OutputLine) (Result, error) {
    // Extract format and target from command
    format := cmd.Args[0]  // "go", "rust", "toml", etc.
    target := cmd.Target

    // Build deno task command
    // Uses: deno task lint:<format> <target>
    args := []string{"task", fmt.Sprintf("lint:%s", format), target}

    // Execute and stream output
    return e.execDeno(args, stream)
}

func (e *LintEngine) Available() bool {
    // Check: Deno installed? cws-struct directory exists?
    _, err := exec.LookPath(e.denoPath)
    return err == nil
}
```

**Key insight:** The lint engine WRAPS cws-struct. It doesn't reimplement linting in Go. Deno runs the actual lint. Go handles the interface. This means cws-struct continues to improve independently, and the Go tool benefits automatically.

### Status Engine (native Go)

```go
type StatusEngine struct {
    endpoints []MonitoredEndpoint
}

type MonitoredEndpoint struct {
    Name    string
    URL     string
    Method  string // "GET", "TCP", "PING"
    Timeout time.Duration
}

func (e *StatusEngine) Execute(cmd Command, stream chan<- OutputLine) (Result, error) {
    // Check all endpoints (or specific one if Target is set)
    // Return health status for each
    // Score: percentage of healthy endpoints
}
```

**Default endpoints** (configurable):

| Name | URL | Method |
|------|-----|--------|
| cws-server | `http://localhost:3847/health` | GET |
| dashboard | `https://dashboard.creativeworkzstudio.com` | GET |
| cpisi | `https://cpisi.creativeworkzstudio.com` | GET |
| status | `https://status.creativeworkzstudio.com` | GET |
| site | `https://creativeworkzstudio.com` | GET |
| fly-standby | `https://cws-server.fly.dev/health` | GET |

### Ops Engine (wraps SSH + systemctl)

```go
type OpsEngine struct {
    targets map[string]OpsTarget
}

type OpsTarget struct {
    Name     string
    Host     string // "cws-oracle", "localhost"
    User     string
    Services []string // systemd service names
}

func (e *OpsEngine) Execute(cmd Command, stream chan<- OutputLine) (Result, error) {
    action := cmd.Args[0]  // "restart", "logs", "config"
    service := cmd.Target  // "cws-server", "cloudflared", etc.

    switch action {
    case "restart":
        return e.restart(service, stream)
    case "logs":
        return e.logs(service, cmd.Opts, stream)
    case "config":
        return e.config(service, cmd.Opts, stream)
    }
}
```

### Health Engine (native Go)

```go
type HealthEngine struct {
    // Reads cws-struct schema data to score files/directories
    // Uses the same 7 Hebrew states
}

func (e *HealthEngine) Execute(cmd Command, stream chan<- OutputLine) (Result, error) {
    // Walk target directory
    // For each file: determine form, check structure, score alignment
    // Aggregate into directory-level health
    // Return with HealthScore
}
```

This is where the data layer registries directly feed in. The 42 containers, 11 concepts, 5 forms, detection weights — all of it becomes the scoring rubric for the health engine.

---

## Configuration

Single config file at `~/.config/cws/config.toml`:

```toml
# CWS Tool Configuration
# Detected form can be overridden here

[tool]
form = "auto"     # "auto", "workstation", "vm", "ci", "remote"
color = true
verbose = false

[lint]
deno_path = "deno"
struct_path = "/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct"

[build]
builder_path = "/media/seanje-lenox-wise/Project/Bereshit/company-docs/build/builder"

[serve]
port = 3848
host = "127.0.0.1"

[status]
poll_interval = "5s"

[[status.endpoints]]
name = "cws-server"
url = "http://localhost:3847/health"
method = "GET"
timeout = "3s"

[[status.endpoints]]
name = "dashboard"
url = "https://dashboard.creativeworkzstudio.com"
method = "GET"
timeout = "5s"

[deploy]
default_target = "oracle"

[deploy.targets.oracle]
host = "cws-oracle"
user = "seanje"
binary_path = "/usr/local/bin/cws-server"
service = "cws-server"

[deploy.targets.dell]
host = "192.168.1.99"
user = "seanje"
binary_path = "/usr/local/bin/cws-server"
service = "cws-server"

[tui]
refresh_rate = "1s"
default_view = "status"
```

---

## Go Module Structure

```
cws.studio/tool/
├── cmd/
│   └── cws/
│       └── main.go              # Entry point
├── internal/
│   ├── bus/
│   │   ├── command.go           # Command, Result types
│   │   ├── router.go            # Router + Engine interface
│   │   └── form.go              # Form system (deployment contexts)
│   ├── engine/
│   │   ├── lint.go              # Wraps cws-struct via Deno
│   │   ├── build.go             # Wraps TS builder
│   │   ├── status.go            # Native HTTP health checks
│   │   ├── health.go            # File/dir health scoring
│   │   ├── ops.go               # SSH + systemctl wrapper
│   │   ├── deploy.go            # Cross-compile + scp + restart
│   │   ├── transform.go         # Wraps cws-struct transform
│   │   ├── create.go            # Wraps cws-struct create
│   │   └── verify.go            # Environment verification
│   ├── cli/
│   │   ├── root.go              # cobra root command
│   │   ├── lint.go              # cobra lint subcommand
│   │   ├── status.go            # cobra status subcommand
│   │   ├── tui.go               # cobra tui subcommand (launches BubbleTea)
│   │   ├── serve.go             # cobra serve subcommand
│   │   └── ...                  # One file per subcommand
│   ├── tui/
│   │   ├── app.go               # BubbleTea main model
│   │   ├── views/
│   │   │   ├── status.go        # Service status view
│   │   │   ├── lint.go          # Lint results view
│   │   │   ├── health.go        # Health score tree
│   │   │   ├── logs.go          # Live log viewer
│   │   │   └── build.go         # Build progress view
│   │   └── components/
│   │       ├── table.go         # Styled table component
│   │       ├── progress.go      # Progress bar (Hebrew state colored)
│   │       ├── tree.go          # Directory tree view
│   │       └── indicator.go     # Health state indicator
│   ├── serve/
│   │   ├── server.go            # HTTP + WebSocket server
│   │   ├── routes.go            # API route definitions
│   │   └── middleware.go        # Auth, logging, CORS
│   └── config/
│       └── config.go            # TOML config loader
├── go.mod
├── go.sum
├── Makefile
└── README.md
```

**4-block structure applied:**
- `cmd/` = CLOSING (entry point)
- `internal/bus/` = METADATA (core types, identity of the system)
- `internal/engine/` = SETUP (capabilities, what's available)
- `internal/cli/`, `internal/tui/`, `internal/serve/` = BODY (operational logic)

---

## Build and Deploy

```makefile
# Makefile for cws tool

BINARY  := cws
VERSION := a-01.00
COMMIT  := $(shell git rev-parse --short HEAD)
LDFLAGS := -ldflags "-X main.version=$(VERSION) -X main.commit=$(COMMIT)"

# Local build (workstation, amd64)
build:
	go build $(LDFLAGS) -o bin/$(BINARY) ./cmd/cws

# Cross-compile for Oracle VM (ARM64)
build-arm:
	GOOS=linux GOARCH=arm64 go build $(LDFLAGS) -o bin/$(BINARY)-arm64 ./cmd/cws

# Deploy to Oracle
deploy-oracle: build-arm
	scp bin/$(BINARY)-arm64 cws-oracle:/usr/local/bin/$(BINARY)
	ssh cws-oracle "chmod 755 /usr/local/bin/$(BINARY)"

# Deploy to Dell fallback
deploy-dell: build
	scp bin/$(BINARY) 192.168.1.99:/usr/local/bin/$(BINARY)
	ssh 192.168.1.99 "chmod 755 /usr/local/bin/$(BINARY)"

# Run tests
test:
	go test ./...

# Run lint
lint:
	go vet ./...
	staticcheck ./...

.PHONY: build build-arm deploy-oracle deploy-dell test lint
```

**One binary.** `go build` produces a static binary. `scp` it anywhere. No runtime dependencies (Deno only needed where lint/transform engines run). On the VM, status + ops + tui + serve work without Deno.

---

## Implementation Phases

### Phase 0: Foundation (Proof of Concept)

**Deliverable:** `cws lint go <target>` works from CLI.

| Task | What |
|------|------|
| Initialize Go module `cws.studio/tool` | Module, go.mod, Makefile |
| Implement `bus.Command`, `bus.Result` | Core types |
| Implement `bus.Router` | Engine registration |
| Implement `engine.LintEngine` | Wraps cws-struct via Deno subprocess |
| Implement `cli.Root` + `cli.Lint` | cobra commands |
| Output formatting | Text (default) + JSON (`--json`) |

**Success criteria:** `cws lint go /path/to/file.go` produces the same results as `deno task lint:go /path/to/file.go`, formatted through the universal output system.

**Why this first:** It proves the entire architecture. If wrapping one existing tool through the command bus works cleanly, the pattern works for everything. If it doesn't, we learn what needs to change before building more.

### Phase 1: Status + TUI

**Deliverable:** `cws status` and `cws tui status` work on both workstation and VM.

| Task | What |
|------|------|
| Implement `engine.StatusEngine` | HTTP health checks |
| Implement `cli.Status` | Text status output |
| Add BubbleTea dependency | `github.com/charmbracelet/bubbletea` |
| Implement `tui.App` | Main model with tab navigation |
| Implement `tui.views.Status` | Live service monitor |
| Cross-compile ARM64 | `make build-arm` |
| Deploy to Oracle | `make deploy-oracle` |

**Success criteria:** `ssh cws-oracle cws tui status` shows live health of all monitored services in a styled terminal dashboard.

### Phase 2: API Server

**Deliverable:** `cws serve` exposes command bus over HTTP + WebSocket.

| Task | What |
|------|------|
| Implement `serve.Server` | HTTP server with WebSocket |
| Implement API routes | POST/GET endpoints per CommandKind |
| Implement streaming | WebSocket for `Stream: true` commands |
| Add form system | Auto-detect deployment context |

**Success criteria:** Tauri dashboard can call `POST /api/v1/lint` and receive structured results.

### Phase 3: Tauri Integration

**Deliverable:** CWS Dashboard connects to `cws serve` instead of spawning processes directly.

| Task | What |
|------|------|
| Update Tauri backend | HTTP client to `cws serve` |
| Update frontend | Render `Result` objects from API |
| Retain native features | File dialogs, system menu stay Tauri-native |

### Phase 4: Full Engine Suite

**Deliverable:** All engines implemented, full form system, production deployment.

| Task | What |
|------|------|
| Build engine | Wraps TS builder |
| Health engine | File/directory scoring using data layer |
| Ops engine | SSH + systemctl |
| Deploy engine | Cross-compile + scp + restart |
| Transform engine | Wraps cws-struct transform |
| Create engine | Wraps cws-struct create |
| Verify engine | Environment checks |
| All TUI views | Lint, health, logs, build |

---

## The Hebrew State TUI

The TUI is where the game design really shows. Health scores map directly to visual design:

```
┌─── CWS Dashboard ──────────────────────────────────────┐
│ [Status] [Lint] [Health] [Logs] [Build]                │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  Services                          Overall: SHALEM      │
│  ─────────                         ████████████░░ 0.78  │
│                                                         │
│  cws-server    TOV    ████████████████ 1.00  200 OK     │
│  dashboard     TAMIM  ██████████████░░ 0.89  200 OK     │
│  cpisi         TOV    ████████████████ 1.00  200 OK     │
│  status        TOV    ████████████████ 1.00  200 OK     │
│  site          SHALEM ████████████░░░░ 0.78  200 OK     │
│  fly-standby   YASHAR ████████░░░░░░░░ 0.50  sleeping   │
│                                                         │
│  Last check: 3s ago    Next: 2s    Uptime: 99.7%       │
│                                                         │
├─────────────────────────────────────────────────────────┤
│  q:quit  tab:switch  r:refresh  ?:help                  │
└─────────────────────────────────────────────────────────┘
```

**Color mapping:**

| State | Color | Bar Fill |
|-------|-------|----------|
| tov (perfect) | Green | Full |
| shalem (whole) | Light green | ~80% |
| tamim (sound) | Blue | ~70% |
| yashar (even) | White | ~50% |
| ratsah (wanting) | Yellow | ~30% |
| chaser (lacking) | Orange | ~20% |
| shavar (broken) | Red | Minimal |

This isn't decoration. The Hebrew states ARE the UI. The health scoring system IS the progress bar. The game design and the Kingdom theology and the production tooling are the same thing expressed in different renderers.

---

## Open Questions for Seanje

1. **Binary name:** `cws` is clean and short. But it's also the company abbreviation. Is that too overloaded? Alternatives: `cwst` (CWS Tool), `tsi` (t-SI), `kts` (Kingdom Tech Stack).

2. **Module path:** `cws.studio/tool` fits the existing pattern (`cws.studio/server`, `cws.studio/pkg`). Confirm?

3. **Phase priority:** Phase 0 (lint wrapper) proves the architecture. Phase 1 (status + TUI) delivers immediate value for the VM. Which first?

4. **Server evolution:** Does `cws serve` eventually REPLACE cws-server, or live alongside it? The command bus could absorb the existing server's functionality over time.

5. **Config location:** `~/.config/cws/config.toml` follows XDG. The VM would have its own config. Is per-machine config sufficient or do we need per-project overrides?

---

## Why This Architecture

The command bus pattern isn't clever. It's the most boring, proven architecture in game development. Every game engine uses it because the alternative — coupling input devices to game logic — creates unmaintainable systems that can't be extended.

CWS tooling is at exactly the inflection point where five separate tools with five separate interfaces becomes untenable. The command bus unifies them without rewriting any of them. Each existing tool becomes an engine that the bus wraps. The bus provides the interface multiplexing. New tools add by implementing one interface.

The form system ensures the same binary is useful everywhere — full workstation, headless CI, remote VM, SSH session. That's the same design insight as cws-struct forms: same structure, different capabilities activated by context.

And the Hebrew states? They're not a gimmick bolted onto a dashboard. They're the native health model of the entire system, rendered in whatever medium the user is looking at. CLI prints "SHALEM". TUI draws a colored bar. GUI shows a styled indicator. Web serves a JSON score. Same truth, four renderers.

> *"Write the vision, and make it plain upon tables, that he may run that readeth it."* -- Habakkuk 2:2

The vision is plain. The tables are drawn. Run.

---

**Asa Mishchaq** -- Game design applied to tooling architecture
**2026-02-22** -- Exploration, pending approval
