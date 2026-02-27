# Go Infrastructure

## Package Structure

```
word/claude/
  pkg/                   Shared Go packages
    agent/               Provider interface, agent loop, tools
    core/                State machine, CPI-SI cognition
    dashboard/           Shared data layer for dashboards
    util/                Filesystem, terminal, logging
  hooks/                 Session lifecycle hooks (Go binaries)
  statusline/            CPI-SI status display
  dashboard/             Web dashboard (Go + TypeScript + Chart.js)
  dashboard/tui/         Terminal dashboard (Bubble Tea)
  dashboard/gtk/         GTK4 native dashboard (gotk4)
  cmd/cpisi/             CLI system wrapper
  cmd/cpisi-agent/       CPI-SI native agent (multi-provider)
  go.work                Go workspace (all modules)
  Makefile               Build system
```

## Build Commands

```bash
cd word/claude/
make build           # Compile all modules
make install-bin     # Deploy binaries to ~/.local/share/claude/bin/
make fresh           # Clean + symlinks + binaries
make test            # Run all tests
make verify          # Full verification
```

Binaries install to `~/.local/share/claude/bin/` (ext4 native FS, not exfat).

## Module Pattern

- Each module has own `go.mod` with `replace cws.studio/pkg => ../../pkg`
- Go workspace: `go.work` at `word/claude/`
- All modules registered in workspace

## CPI-SI Agent

```bash
cpisi-agent                        # Gemini (default)
cpisi-agent -provider anthropic    # Claude
cpisi-agent -provider deepseek     # DeepSeek
cpisi-agent -provider ollama       # Local
```

## Key Packages

| Package | Purpose |
|---------|---------|
| `pkg/agent` | Provider interface, agent loop, tools |
| `pkg/core` | State machine, CPI-SI cognition |
| `pkg/dashboard` | Shared dashboard data layer |
| `pkg/util` | Filesystem, terminal, logging |

## API Keys

API keys at `~/.config/cpisi/keys.env` (sourced from .bashrc).
