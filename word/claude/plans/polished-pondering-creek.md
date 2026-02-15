# Plan: Organize Shell Profile & Environment

## Context

The `.bashrc` has grown organically — PATH is duplicated 3x, tools are added at the bottom without structure, and a referenced CPI-SI env file doesn't exist. The shell profile should reflect the same intentional design we apply to our code: organized, no redundancy, clear sections.

**Shell decision:** Stay with Bash. It's POSIX-compatible, all CPI-SI hooks and Makefiles are Bash-tested, and it's the most reliable choice. We'll add **Starship** (Rust-compiled cross-shell prompt) for modern UX without changing the shell itself.

## Changes

### 1. Install Starship prompt

```bash
curl -sS https://starship.rs/install.sh | sh
```

- Creates `~/.config/starship.toml` with CPI-SI-aligned theme (dark, minimal, shows git/Go/Rust/Node info)
- Single `eval "$(starship init bash)"` line at end of `.bashrc`

### 2. Rewrite `~/.bashrc` — organized into clear sections

Structure (order matters for PATH):

```
# ── Core Shell Settings ──────────────────────────────────────
#   Interactive check, history, window, completion

# ── PATH Construction ────────────────────────────────────────
#   Single, deduplicated PATH built in priority order:
#     1. ~/.local/bin          (Claude Code, user binaries)
#     2. ~/.deno/bin           (Deno)
#     3. ~/.opencode/bin       (OpenCode)
#     4. ~/.cargo/bin          (Rust — sourced from cargo env)
#     5. ~/.pyenv/shims + bin  (Python)
#     6. ~/.nvm node bin       (Node — via nvm)
#     7. ~/go/bin              (Go)
#     8. System paths          (already in PATH from /etc/environment)
#     9. ~/.dotnet/tools       (.NET)
#    10. CUDA toolkit          (nvidia)

# ── Tool Initialization ─────────────────────────────────────
#   pyenv init, nvm load, cargo env (idempotent)

# ── Environment Variables ────────────────────────────────────
#   GOPATH, DOTNET_ROOT, CUDA_HOME, DENO_INSTALL, etc.
#   SUDO_ASKPASS, ccache, telemetry opt-outs

# ── CPI-SI Environment ──────────────────────────────────────
#   Source non-interactive.conf (project paths, build vars)

# ── Aliases & Functions ──────────────────────────────────────
#   ls/grep colors, git shortcuts, mkcd, bat alias

# ── Prompt ───────────────────────────────────────────────────
#   Starship init (must be last)
```

**Key fixes:**
- `~/.local/bin` added ONCE (currently 3x)
- `.dotnet/tools` added ONCE (currently 2x)
- `~/.cargo/bin` loaded via `cargo env` (already has dedup guard) — not manually prepended
- Remove duplicate `ll/la/l` alias definitions (lines 91-93 and 134-136)
- HISTSIZE bumped to 10000/20000 (1000 is too small for development work)
- Enable `globstar` (currently commented out — useful for `**/*.go` patterns)

### 3. Clean up `~/.profile`

- Remove the duplicate `SUDO_ASKPASS` export (already in `.bashrc`)
- Keep the `~/.local/bin` PATH add (login shells need it)
- Keep cargo env source
- This file is minimal and correct otherwise

### 4. Create `~/.claude/system/env/non-interactive.conf`

Source location: `Bereshit/word/claude/system/env/non-interactive.conf`
Installed to: `~/.claude/system/env/non-interactive.conf` (via symlink from Bereshit source)

Contents:
```bash
# CPI-SI Environment Configuration
# Sourced by ~/.bashrc for all interactive shells

# Project paths
export CPISI_SOURCE="/media/seanje-lenox-wise/Project/Bereshit/word/claude"
export BERESHIT_ROOT="/media/seanje-lenox-wise/Project/Bereshit"
export CWS_ROOT="/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC"
export PROJECT_DRIVE="/media/seanje-lenox-wise/Project"

# CPI-SI binary locations
export CPISI_BIN="$HOME/.local/share/claude/bin"
export CPISI_SYSTEM_BIN="$HOME/.local/share/claude/system/bin"

# Build acceleration
export CPISI_CACHE="$HOME/.cache/cpisi"
export CPISI_STAMPS="$CPISI_CACHE/stamps"
```

### 5. Create `~/.config/starship.toml`

Minimal, dark-themed config showing:
- Current directory (truncated)
- Git branch + status
- Go, Rust, Node, Python versions (only when relevant files detected)
- Command duration (>2s)
- Exit code on error

## Files Modified

| File | Action |
|------|--------|
| `~/.bashrc` | Rewrite — organized, deduplicated |
| `~/.profile` | Minor cleanup — remove duplicate SUDO_ASKPASS |
| `Bereshit/word/claude/system/env/non-interactive.conf` | Create — CPI-SI env vars |
| `~/.claude/system/env/` | Create dir + symlink to Bereshit source |
| `~/.config/starship.toml` | Create — prompt configuration |

## Verification

1. `source ~/.bashrc` — no errors
2. `echo "$PATH" | tr ':' '\n' | sort | uniq -d` — no duplicates
3. `which claude` — resolves to `~/.local/bin/claude`
4. `which starship` — resolves correctly
5. `printenv CPISI_SOURCE` — shows Bereshit path
6. Open new terminal — prompt renders correctly with Starship
7. `cd` to a Go project — shows Go version in prompt
8. `cd` to a git repo — shows branch + status
