# Plan: Prometheus + Grafana + CWS Ops Tool

## Context

Seanje wants monitoring that grows naturally rather than being retrofitted later, plus a management tool (CLI + GUI) so he can do server-side infrastructure work himself — not just Nova Dawn. Two independent tracks executed in sequence.

## Track 1: Prometheus + Grafana (~4 hours)

### 1A. Instrument Go Server with Prometheus

**Add `github.com/prometheus/client_golang` to `server/go.mod`.**

Create `server/internal/server/prometheus.go` (4-block):
- Bridge pattern: reads existing `sync/atomic` counters at scrape time (zero hot-path overhead for existing metrics)
- One hot-path addition: `HistogramVec` for request duration distribution (label: method, route, status_code)
- Registers: `cws_http_requests_total`, `cws_http_errors_total`, `cws_http_active_requests`, `cws_http_request_duration_seconds`, `cws_server_uptime_seconds`, `cws_server_goroutines`, `cws_ws_clients`
- Also registers Go runtime + process collectors

Modify `server/internal/server/metrics.go`:
- Add `SetPrometheusBridge(*PrometheusMetrics)` method
- Add `StartedAt() time.Time` getter
- In `Middleware()`: call `promBridge.ObserveRequest()` after response (one nil check + histogram observe)

Modify `server/cmd/cws-server/main.go`:
- Init `PrometheusMetrics`, attach to `Metrics`, register `GET /metrics` endpoint
- Existing `GET /api/metrics` (JSON) stays for backward compat

**Files:** `prometheus.go` (new), `metrics.go` (modify), `main.go` (modify), `go.mod` (modify)

### 1B. Install Monitoring Stack on Oracle

All services bind to `127.0.0.1` only. Grafana exposed via tunnel.

| Service | Port | Memory | Install Method |
|---------|------|--------|----------------|
| node_exporter | 9100 | ~15MB | Binary download (ARM64) |
| Prometheus | 9090 | ~200MB | Binary download (ARM64) |
| Grafana | 3000 | ~120MB | APT repository |

**Total new memory: ~335MB** (Oracle has 11.7GB headroom)

Prometheus config (`prometheus.yml`): scrape cws-server:3847/metrics (10s), node:9100 (15s), self:9090 (15s). Retention: 30 days / 5GB cap.

Grafana config: `root_url = https://grafana.creativeworkzstudio.com`, anonymous auth disabled, provisioned Prometheus datasource, admin password set on deploy.

systemd units for node_exporter and Prometheus (Grafana ships its own). All with hardening: NoNewPrivileges, ProtectSystem=strict, dedicated system users.

**Files to create:**
- `word/claude/infrastructure/monitoring/prometheus.yml`
- `word/claude/infrastructure/monitoring/prometheus.service`
- `word/claude/infrastructure/monitoring/node-exporter.service`
- `word/claude/infrastructure/monitoring/grafana/grafana.ini`
- `word/claude/infrastructure/monitoring/grafana/provisioning/datasources/prometheus.yml`
- `word/claude/infrastructure/monitoring/grafana/provisioning/dashboards/dashboard.yml`

### 1C. Tunnel + DNS

Update `word/claude/infrastructure/cloudflared/config.yml`: add `grafana.creativeworkzstudio.com` -> `http://localhost:3000`.

Create CNAME: `grafana.creativeworkzstudio.com` -> tunnel.

### 1D. Ansible + Makefile

Add Phase 12 to `word/claude/infrastructure/ansible/cws-production.yaml` (tag: `monitoring`): system users, binary downloads, configs, systemd units, Grafana APT install + provisioning.

Add to `server/Makefile`: `deploy-monitoring`, `monitoring-status`, `monitoring-logs`.

### 1E. Deploy + Verify

1. Deploy Go binary with `/metrics` endpoint
2. Run Ansible `--tags monitoring` on Oracle
3. Create Grafana dashboards (Server Health: request rate, error rate, P50/P95/P99 latency, goroutines, WS clients, heap. System: CPU, RAM, disk, network, load)
4. Verify `grafana.creativeworkzstudio.com` accessible

---

## Track 2: CWS Ops Tool (multi-session)

### Architecture

Standalone Go CLI binary (`cws-ops`) in the same Go module as `cws-server`. Two interfaces: CLI for terminal, embedded web GUI via `cws-ops serve`. Lives at `server/cmd/cws-ops/`.

**Key decisions:**
- **Exec-based SSH** (not `x/crypto/ssh`) — reuses existing `~/.ssh/config`, keys, agent
- **Makefile wrapping** for deploys — battle-tested targets, no reimplementation
- **ConfirmDialog** on every destructive operation — safety for CEO-level usage
- **Port 4847** (3847 + 1000) — runs alongside cws-server
- **go:embed** for single-binary web GUI distribution

### Directory Structure

```
server/
  cmd/
    cws-ops/
      main.go           # Entry point, subcommand dispatch
      serve.go          # 'serve' subcommand: HTTP server + embedded frontend
      embed.go          # go:embed for static assets
  internal/
    config/
      hosts.go          # NEW: Host definitions (oracle, dell), shared types
    ops/                # NEW: All operations logic
      ops.go            # Package doc, shared types (ExecResult, ServiceStatus)
      ssh.go            # SSH execution (RunSSH, RunSSHCapture, StreamSSH)
      status.go         # FullStatus gathering (services, resources, tunnel)
      deploy.go         # Deploy operations (wraps Makefile targets)
      logs.go           # Log streaming via SSH journalctl
      services.go       # systemd service management
      dns.go            # Cloudflare DNS API v4 (net/http, no SDK)
      backups.go        # Litestream + Restic status and restore
    ops/handler/        # NEW: HTTP handlers for web GUI
      handler.go        # Route registration
      status.go         # GET /api/ops/status
      deploy.go         # POST /api/ops/deploy (streams via WS)
      logs.go           # GET /api/ops/logs/stream (WS upgrade)
      services.go       # Service management endpoints
      dns.go            # DNS CRUD endpoints
      backups.go        # Backup status + restore endpoints
  ops-frontend/         # NEW: Web GUI (Vite + vanilla TS + Web Components)
    src/
      main.ts           # Entry: store, WS, router
      state/store.ts    # EventTarget-based reactive store (dashboard pattern)
      router/router.ts  # Hash-based routing
      components/
        pages/          # Overview, Deploy, Logs, Services, DNS, Backups
        shared/         # StatusBadge, TerminalOutput, ConfirmDialog, Card, Table
    styles/
      main.css          # Design tokens (matches dashboard variables)
```

### CLI Command Tree

```
cws-ops [--target oracle|dell] [--verbose] [--dry-run] [--json] <command>

  status [--full|--services|--resources]   Check all infrastructure
  deploy <all|binary|dash|site|builder|fly> [--no-confirm]
  logs [--follow] [--lines N] [--service NAME] [--since DURATION]
  services <list|restart|stop|start> [NAME] [--no-confirm]
  dns <list|update NAME VALUE> [--no-confirm]
  backups <status|list|verify|restore DB> [--to PATH] [--no-confirm]
  ssh                                      Quick SSH connect
  health [--lines N]                       View health watchdog log
  serve [--port 4847] [--open]             Start web GUI
  version / help
```

### Web GUI Pages

| Page | What It Shows |
|------|---------------|
| **Overview** | Status cards (server, resources, tunnel), service grid, quick actions |
| **Deploy** | Target cards with streaming output terminal, cancel button |
| **Logs** | Full-width terminal, service filter, time range, live streaming |
| **Services** | Service cards with status badge, restart/stop/start buttons |
| **DNS** | Records table, inline editing, confirmation dialogs |
| **Backups** | Litestream status table, Restic snapshots, restore buttons |

### Build Phases

| Phase | Scope | Sessions |
|-------|-------|----------|
| **0: Foundation** | CLI skeleton, `hosts.go`, `ssh.go`, `cws-ops status` + `ssh` | 1 |
| **1: Core CLI** | All CLI commands, confirmation prompts, `--json`/`--dry-run` | 1-2 |
| **2: Web Foundation** | Vite scaffold, state store, router, App shell, Overview page, `cws-ops serve` | 1-2 |
| **3: Web Pages** | Services, Deploy, Logs, Backups, DNS pages (one per focused session) | 2-3 |
| **4: Polish** | Auto-refresh, notifications, dark/light theme, keyboard shortcuts | 1 |

### Makefile

```
make ops              # Build cws-ops binary (to ~/.local/bin/)
make ops-frontend     # Compile frontend (Vite build -> cmd/cws-ops/static/)
make ops-dev          # Dev mode (Vite HMR + Go server)
```

---

## Execution Order

1. **Track 1** first (Prometheus + Grafana) — self-contained, ~4 hours
2. **Track 2 Phase 0+1** (CLI foundation) — useful immediately
3. **Track 2 Phase 2+3** (Web GUI) — built on working CLI
4. **Track 2 Phase 4** (Polish)

## Verification

**Track 1:**
- `curl localhost:3847/metrics` returns Prometheus text format
- `grafana.creativeworkzstudio.com` shows dashboards
- `make monitoring-status` shows all green
- Dashboard panels show real request data

**Track 2:**
- `cws-ops status` returns infrastructure health from Oracle
- `cws-ops deploy binary --dry-run` shows what would happen
- `cws-ops serve --open` launches GUI in browser
- All destructive operations show confirmation dialog
- Seanje can deploy, restart, check status without SSH knowledge
