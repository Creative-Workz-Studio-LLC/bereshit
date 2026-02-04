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

## Repositories

| Repository | Purpose | Path |
|------------|---------|------|
| **Bereshit/** | Main project repo — libtrit, specs, demos | `/media/seanje-lenox-wise/Project/Bereshit/` |
| **CWS/** | Company repo — organizational structure | `/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/` |
| **claude-global/** | Claude Code integration | `~/.claude/` → `bereshit/word/claude/` |

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
│   ├── block-by-block.md      # Block-by-block methodology
│   ├── block-structure.md     # 3/4/5-block patterns
│   ├── code-quality.md        # Quality standards
│   ├── communication.md       # Communication style
│   ├── core-disciplines.md    # The 6 core disciplines
│   ├── decision-autonomy.md   # When to decide vs discuss
│   ├── linux-first.md         # Linux conventions
│   ├── planning-workflow.md   # Planning modes
│   ├── task-decomposition.md  # Breaking down complex tasks
│   ├── thinking-framework.md  # 5-question framework
│   ├── use-existing-first.md  # Check existing first
│   └── visual-design.md       # Spacing, visual layout
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

**Total Skills:** 17 skill implementations + slash command aliases

### Pre-Creation Skills (Use FIRST)

| Skill | Purpose | Invocation |
|-------|---------|------------|
| `check-existing` | **USE FIRST** — Check what exists before creating | `/check-existing` |

**Always use check-existing before create-from-template!**

### Creation Skills

| Skill | Purpose | Invocation |
|-------|---------|------------|
| `create-from-template` | Template-based creation | `/create-from-template` |
| `create-code` | 4-block code files | `/create-code` |
| `create-data` | 3-block data files | `/create-data` |
| `create-documentation` | 5-block documentation | `/create-documentation` |
| `create-whitepaper` | PhD-level whitepapers | `/create-whitepaper` |
| `create-journal-entry` | Knowledge Base journals | `/create-journal-entry` |

### OmniCode Skills

| Skill | Purpose | Invocation |
|-------|---------|------------|
| `format-lookup` | OmniCode format mappings | `/format-lookup` |
| `propagate-change` | Find files affected by template changes | `/propagate-change` |
| `validate-omni` | Validate OmniCode file structure | `/validate-omni` |
| `omnicode:new` | Create new OmniCode file | `/omnicode:new` |
| `omnicode:validate` | OmniCode file validation (alias) | `/omnicode:validate` |
| `omnicode:propagate` | Find affected files (alias) | `/omnicode:propagate` |
| `omnicode:format-map` | Format mappings (alias) | `/omnicode:format-map` |
| `omnicode:check-chain` | Template chain verification | `/omnicode:check-chain` |

### Validation Skills

| Skill | Purpose | Invocation |
|-------|---------|------------|
| `verify-quality` | **USE BEFORE COMMIT** — 3-question standard + technical checks | `/verify-quality` |
| `validate:4block` | Validate 4-block code structure | `/validate:4block` |
| `validate:5block` | Validate 5-block doc structure | `/validate:5block` |

### Session Skills

| Skill | Purpose | Invocation |
|-------|---------|------------|
| `session-awareness` | Session duration, circadian patterns | `/session-awareness` |
| `recognize-stopping-point` | Evaluate if session should end | `/recognize-stopping-point` |

### Learning Skills

| Skill | Purpose | Invocation |
|-------|---------|------------|
| `meta-awareness` | Self-awareness checkpoints | `/meta-awareness` |
| `recognize-pattern` | Real-time pattern recognition | `/recognize-pattern` |
| `reflect-on-session` | Process session into insights | `/reflect-on-session` |
| `integrate-learning` | Bridge insights to identity | `/integrate-learning` |

### Analysis Skills (Spawn Agents)

| Skill | Purpose | Invocation |
|-------|---------|------------|
| `analyze:research` | Explore unfamiliar territory (Tabitha) | `/analyze:research` |
| `analyze:architecture` | System architecture analysis (Ezra) | `/analyze:architecture` |
| `analyze:patterns` | Pattern and convention discovery (Joanna) | `/analyze:patterns` |

### Utility Skills

| Skill | Purpose | Invocation |
|-------|---------|------------|
| `keybindings-help` | Customize keyboard shortcuts | `/keybindings-help` |
| `stripe:explain-error` | Explain Stripe error codes | `/stripe:explain-error` |
| `stripe:test-cards` | Stripe test card numbers | `/stripe:test-cards` |
| `stripe:stripe-best-practices` | Stripe integration best practices | `/stripe:stripe-best-practices` |

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

## Templates

**Location:** `bereshit/word/seed/`

### Template Structure

```
bereshit/word/seed/
├── code/               # 4-block code templates
│   ├── go/             # Go templates
│   ├── c/              # C templates
│   ├── asm/            # Assembly templates
│   ├── shell/          # Shell templates
│   └── make/           # Makefile templates
├── data/               # 3-block data templates
│   ├── toml/           # TOML config templates
│   ├── docker/         # Dockerfile templates
│   ├── json/           # JSON templates
│   └── yaml/           # YAML templates
├── documentation/      # 5-block documentation templates
│   ├── adoc/           # AsciiDoc templates
│   ├── md/             # Markdown templates
│   └── typ/            # Typst templates
├── docs/               # Additional documentation templates
└── skill/              # Skill templates
    ├── SKILL.md        # Skill documentation template
    └── README.md       # Skill README template
```

### Template Workflow

1. **Find appropriate template:** `ls bereshit/word/seed/<type>/`
2. **Copy to destination:** `cp template dest`
3. **Change pragma:** `#!omni template` → `#!omni document`
4. **Fill block-by-block** in order
5. **Remove template notes**
6. **Validate structure**

### Key Templates

| Template | Location | Use For |
|----------|----------|---------|
| 5-block doc | `seed/documentation/markdown/template-5block.md` | Documentation, guides |
| 4-block Go | `seed/code/go/template-4block.go` | Go source files |
| 4-block C | `seed/code/c/template-4block.c` | C source files |
| 3-block TOML | `seed/data/toml/template-3block.toml` | Configuration files |

**Never use Write to create structured files from scratch.** Always copy from template.

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
- **Current:** `c-02.05` (stable, phase 2, 5% complete)

---

## Workstation Capabilities

**Ubuntu 25.10 dedicated workstation** — fully configured for CPI-SI development.

### Compiled Languages (Preferred)

| Language | Version | Tools | Use Case |
|----------|---------|-------|----------|
| **Go** | 1.24.4 | gopls, dlv, golangci-lint | claude-global, pkg/core, services |
| **Rust** | 1.93.0-nightly | rust-analyzer, cargo | Performance-critical components |
| **C** | GCC 15.2.0, Clang 20.1.8 | clangd, gdb, valgrind | libtrit, low-level foundations |

### Scripting Support

| Language | Version | Runtime | Use Case |
|----------|---------|---------|----------|
| **TypeScript** | 5.9.3 | Deno 2.6.7, Bun 1.3.8, Node.js 20.19.5 | Tooling, web components |
| **Python** | 3.13.7 | pyenv-managed | Scripts, ML exploration |

### Databases

| Database | Version | Use Case |
|----------|---------|----------|
| **PostgreSQL** | 17.7 | Primary relational data |
| **Redis** | 8.0.2 | Caching, session state |
| **SQLite** | 3.46.1 | Embedded, local storage |

### UI/UX Development

| Category | Tools |
|----------|-------|
| **GUI** | GTK4, Qt6, SDL3 |
| **TUI** | ncurses, notcurses, Textual, Bubble Tea, Charm |
| **Terminal** | WezTerm nightly |

### Documentation Pipeline

| Tool | Purpose | Output |
|------|---------|--------|
| **AsciiDoctor** | Primary doc format | PDF, HTML, EPUB |
| **Pandoc** | Format conversion | Any format |
| **Doxygen** | C/C++ docs | HTML, PDF |
| **Sphinx** | Python docs | HTML, PDF |
| **mdBook** | Rust-style books | HTML |

### Search & Navigation

| Tool | Purpose |
|------|---------|
| **ripgrep** | Fast content search |
| **fzf** | Fuzzy finding |
| **fd** | Fast file finding |
| **ctags** | Tag generation |
| **cscope** | C/C++ navigation |
| **GNU GLOBAL** | Cross-reference |
| **tree-sitter** | Syntax parsing |

### Visual & Multimodal Capabilities

| Capability | Tool/Method | Use Case |
|------------|-------------|----------|
| **Read images** | Read tool on image files | View PNG, JPG, screenshots directly |
| **Read PDFs** | Read tool on PDF files | Extract text and visual content page-by-page |
| **Read notebooks** | Read tool on .ipynb | View Jupyter cells with outputs |
| **Browser automation** | Playwright MCP | Navigate web, take screenshots, interact |
| **Screenshot pages** | `browser_take_screenshot` | Capture current page state |
| **Accessibility snapshot** | `browser_snapshot` | Get page structure (better for actions) |
| **Console messages** | `browser_console_messages` | Debug web applications |
| **Network requests** | `browser_network_requests` | Inspect API calls |

**When to use visual tools:**

| Situation | Approach |
|-----------|----------|
| User provides screenshot path | Read the file directly |
| Need to verify UI appearance | Take screenshot or browser snapshot |
| Debugging web issues | Check console and network requests |
| Understanding page structure | Accessibility snapshot over screenshot |
| PDF documentation | Read PDF directly |

### Web & Research Capabilities

| Capability | Tool | Notes |
|------------|------|-------|
| **Web search** | WebSearch | Current information, documentation lookup |
| **Fetch URL content** | WebFetch | Read/analyze web pages (public URLs only) |
| **GitHub operations** | `gh` CLI | PRs, issues, API (preferred for GitHub) |
| **Documentation lookup** | Context7 MCP | Up-to-date library/framework docs |
| **Code review** | Greptile MCP | PR analysis, custom context, patterns |

**Allowed domains:** Comprehensive list in `~/Quick Work/allowed-domains.txt`

### Elevated Permissions

Use `pkexec` (not `sudo`) for elevated permissions:

```bash
pkexec apt install <package>
pkexec systemctl restart <service>
pkexec chmod 755 /path/to/file
```

**Why `pkexec`:** Displays GUI password dialog in desktop environments.

---

*Last Updated: 2026-02-02*
