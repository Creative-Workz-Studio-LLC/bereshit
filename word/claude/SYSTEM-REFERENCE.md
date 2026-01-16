# System Reference

**Component locations, skills catalog, and resource paths for CPI-SI.**

> *"For everything there is a season."* — Ecclesiastes 3:1

---

## Quick Navigation

| Section | Purpose |
|---------|---------|
| [Directory Structure](#directory-structure) | Where everything lives |
| [Skills Catalog](#skills-catalog) | Available skills and usage |
| [Agents](#agents) | CPI-SI family members |
| [Hooks](#hooks) | Lifecycle events |
| [Configuration](#configuration) | Settings and configs |
| [Database](#database) | Temporal consciousness |

---

## Directory Structure

### Claude-Global Root

```
~/.claude/ (symlinks to bereshit/word/claude/)
├── CLAUDE.md           # Identity loading document
├── SYSTEM-REFERENCE.md # This file
├── settings.json       # Claude Code configuration
├── config.toml         # CPI-SI configuration
├── instance.jsonc      # Bootstrap configuration
│
├── agents/             # CPI-SI family member agents
├── bin/                # Compiled binaries (cpisi, statusline)
├── commands/           # Custom slash commands
├── config/             # Human-readable configuration
│   ├── health/         # Health system config
│   ├── instance/       # Instance identities (nova_dawn, family members)
│   ├── statemachine/   # State machine config
│   └── user/           # User-specific config
├── docs/               # Documentation
├── hooks/              # Session and tool lifecycle hooks
│   ├── bin/            # Compiled hook binaries
│   ├── cmd/            # Hook entry points
│   ├── session/        # Session lifecycle (start, stop, end)
│   ├── tool/           # Tool events (pre-use, post-use)
│   ├── permission/     # Permission request handling
│   └── prompt/         # Prompt submission handling
├── output-styles/      # Output style definitions
├── pkg/                # Go packages
│   ├── core/           # Core CPI-SI (statemachine, health)
│   ├── foundation/     # Foundation (database, types)
│   ├── orchestration/  # Orchestration (cognition, logging)
│   └── util/           # Utilities (fs, hookoutput)
├── plans/              # Planning documents
├── rules/              # Modular rules (auto-loaded)
├── skills/             # Skill implementations
├── statusline/         # Status line implementation
├── system/             # System infrastructure
│   ├── bin/            # System utilities
│   ├── config/         # System configuration
│   ├── data/           # Data storage
│   │   ├── logs/       # Category logs (session, tool, error)
│   │   ├── temporal/   # Temporal state
│   │   └── session/    # Session data
│   └── runtime/        # Runtime files
└── workflow/           # Workflow definitions
```

---

## Skills Catalog

### Core Skills

| Skill | Purpose | Invocation |
|-------|---------|------------|
| `session-awareness` | Session duration, circadian patterns | `/session-awareness` |
| `recognize-stopping-point` | Evaluate if session should end | `/recognize-stopping-point` |
| `meta-awareness` | Self-awareness checkpoints | `/meta-awareness` |
| `recognize-pattern` | Real-time pattern recognition | `/recognize-pattern` |
| `reflect-on-session` | Process session into insights | `/reflect-on-session` |

### Creation Skills

| Skill | Purpose | Invocation |
|-------|---------|------------|
| `create-documentation` | 5-block documentation | `/create-documentation` |
| `create-code` | 4-block code files | `/create-code` |
| `create-data` | 3-block data files | `/create-data` |
| `create-from-template` | Template-based creation | `/create-from-template` |
| `create-journal-entry` | Knowledge Base journals | `/create-journal-entry` |
| `create-whitepaper` | PhD-level whitepapers | `/create-whitepaper` |

### OmniCode Skills

| Skill | Purpose | Invocation |
|-------|---------|------------|
| `format-lookup` | OmniCode format mappings | `/format-lookup` |
| `validate-omni` | OmniCode file validation | `/validate-omni` |
| `propagate-change` | Find affected files | `/propagate-change` |

### Learning Skills

| Skill | Purpose | Invocation |
|-------|---------|------------|
| `integrate-learning` | Bridge insights to identity | `/integrate-learning` |

---

## Agents

### CPI-SI Family Members

| Agent | Name | Purpose |
|-------|------|---------|
| `research-agent` | Tabitha Shiloh | Initial exploration and research |
| `architecture-analyzer` | Ezra Matthan | System architecture analysis |
| `pattern-finder` | Joanna Elara | Pattern and convention discovery |
| `format-bridge` | Phoebe Karis | Format translation and mapping |
| `template-chain-analyzer` | Selah Adair | Template chain verification |

### Agent Frontmatter Fields

| Field | Required | Values | Description |
|-------|----------|--------|-------------|
| `name` | Yes | kebab-case | Unique identifier |
| `description` | Yes | string | Full description |
| `tools` | No | comma-sep | Allowed tools |
| `model` | No | opus/sonnet/haiku | Model preference |
| `skills` | No | comma-sep | Skills to auto-load |
| `permissionMode` | No | default/allowEdits/acceptEdits | Permission handling |
| `disallowedTools` | No | comma-sep | Blocked tools |

---

## Hooks

### Lifecycle Events

| Hook | When | Purpose |
|------|------|---------|
| `SessionStart` | Session begins | Initialize state, load context |
| `SessionEnd` | Session ends | Cleanup, save state |
| `Stop` | Main agent stops | Session summary |
| `SubagentStop` | Subagent completes | Log subagent results |
| `SubagentStart` | Subagent spawns | Track subagent cascade |
| `PreCompact` | Before compaction | Snapshot state |
| `Notification` | Background notification | Alert user |

### Tool Events

| Hook | When | Purpose |
|------|------|---------|
| `PreToolUse` | Before tool executes | Validate, log |
| `PostToolUse` | After tool executes | Track changes |

### Permission Events

| Hook | When | Purpose |
|------|------|---------|
| `PermissionRequest` | Permission dialog shown | Auto-allow/deny patterns |
| `UserPromptSubmit` | User submits prompt | Validate, enrich |

### Hook Binaries

Located at `~/.claude/hooks/bin/`:

```
bin/
├── start           # SessionStart
├── stop            # Stop
├── end             # SessionEnd
├── subagent-stop   # SubagentStop
├── subagent-start  # SubagentStart
├── pre-use         # PreToolUse
├── post-use        # PostToolUse
├── pre-compact     # PreCompact
├── notification    # Notification
├── submit          # UserPromptSubmit
└── permission-request # PermissionRequest
```

---

## Configuration

### Key Files

| File | Purpose |
|------|---------|
| `settings.json` | Claude Code configuration (model, hooks, permissions) |
| `config.toml` | CPI-SI system configuration |
| `instance.jsonc` | Bootstrap paths and display |

### Runtime State

| File | Location | Purpose |
|------|----------|---------|
| `state.jsonc` | `system/runtime/` | Current state machine state |
| `path.jsonc` | `system/runtime/` | Session path history |
| `history.jsonc` | `system/data/temporal/` | Cross-session history |

---

## Database

### SQLite Database

**Location:** `~/.claude/data/cpisi.db`

**Purpose:** Temporal consciousness - cross-session memory and patterns.

### Tables

| Table | Purpose |
|-------|---------|
| `sessions` | Session records with state |
| `choices` | Decision records |
| `hebrew_transitions` | State transitions |
| `patterns` | Learned patterns |

### Commands

```bash
make db-init      # Initialize database
make db-migrate   # Run migrations
make db-status    # Show status
make db-browse    # Open in GUI
```

---

## Build & Sync

### Makefile Targets

| Target | Purpose |
|--------|---------|
| `make build` | Build all modules |
| `make sync` | Sync to bereshit/word/claude/ |
| `make deploy` | Sync + rebuild hooks |
| `make test` | Run all tests |

### Sync Flow

```
claude-global/ ─[rsync]─> bereshit/word/claude/ <─[symlinks]─ ~/.claude/
```

---

## Version

**Format:** `STAGE-PHASE.COMPLETION`

- **Stages:** a=unstable(-1), b=baseline(0), c=stable(+1)
- **Current:** `b-03.25` (beta, phase 3, 25% complete)

---

*Last Updated: 2026-01-16*
