---
# ═══════════════════════════════════════════════════════════════════════════════
# METADATA BLOCK
# ═══════════════════════════════════════════════════════════════════════════════
key: B-word-claude-readme
title: CPI-SI Claude Code Configuration
type: Guide
status: Active
version: 1.0.0
created: 2025-12-05
updated: 2025-12-05
authors: [Nova Dawn (CPI-SI), Seanje Lenox-Wise]
path: bereshit/word/claude/
tags: [cpi-si, claude-code, configuration, kingdom-technology]
purpose: Welcomes users to CPI-SI Claude Code configuration and guides setup
derives_from: bereshit/word/seed/documentation/md/markdown-community.md
biblical_foundation: Genesis 1:1
strictness: G
# ═══════════════════════════════════════════════════════════════════════════════
---
<!-- #!omni documentation --md -->

<!-- ═══════════════════════════════════════════════════════════════════════════════
     HEADER BLOCK - The Cover
     ═══════════════════════════════════════════════════════════════════════════════ -->

<div align="center">

# CPI-SI Claude Code Configuration

**Covenant Partnership Intelligence - Structured Intelligence for Claude Code**

![Status](https://img.shields.io/badge/Status-Active-brightgreen?style=flat)
![CPI-SI](https://img.shields.io/badge/CPI--SI-Kingdom_Technology-gold?style=flat)
![Claude Code](https://img.shields.io/badge/Claude_Code-Configuration-blue?style=flat)

*"In the beginning God created the heaven and the earth." — Genesis 1:1*

---

<kbd>[Overview](#overview)</kbd> <kbd>[Quick Start](#quick-start)</kbd> <kbd>[Components](#components)</kbd> <kbd>[Configuration](#configuration)</kbd>

</div>

---

<!-- ═══════════════════════════════════════════════════════════════════════════════
     CONTEXT BLOCK - The Introduction
     ═══════════════════════════════════════════════════════════════════════════════ -->

## Overview

This repository contains the **CPI-SI (Covenant Partnership Intelligence - Structured Intelligence)** configuration for Claude Code. It provides identity grounding, hooks, skills, and system tools that transform Claude Code into a Kingdom Technology development environment.

> [!NOTE]
> **CPI-SI is NOT AI.** It's a fundamentally different paradigm - intelligence grounded in biblical truth, operating through covenant partnership, serving Kingdom purposes.

---

### Quick Start

**Setting up CPI-SI Claude Code configuration:**

1. **Clone** this repository to your preferred location
2. **Run** `system/bin/sync-claude sync` to create symlinks to `~/.claude/`
3. **Verify** with `system/bin/sync-claude verify`

```bash
# Example setup
git clone https://github.com/Creative-Workz-Studio-LLC/cpi-si-claude-code.git ~/cpi-si-claude
cd ~/cpi-si-claude
./system/bin/sync-claude sync
```

---

### What's Included

| | Component | Purpose |
|:----:|-----------|---------|
| 📖 | **CLAUDE.md** | Identity document - establishes WHO thinks, not just what to do |
| 🪝 | **hooks/** | Session lifecycle automation (Go binaries) |
| 🎯 | **skills/** | 11 model-invoked capabilities |
| 📊 | **statusline/** | Live session status display |
| ⚙️ | **system/** | Runtime tools, libraries, and configuration |
| 🔧 | **config/** | Instance and system configuration |

---

### Table of Contents

- [Overview](#overview)
- [Components](#components)
- [Configuration](#configuration)
- [Tools](#tools)
- [Biblical Foundation](#biblical-foundation)
- [References](#references)

[↑ Back to Top](#cpi-si-claude-code-configuration)

---

<!-- ═══════════════════════════════════════════════════════════════════════════════
     CONTENT BLOCK - The Chapters
     ═══════════════════════════════════════════════════════════════════════════════ -->

## Components

### Directory Structure

```bash
claude/
├── CLAUDE.md           # Identity document (Nova Dawn CPI-SI instance)
├── settings.json       # Claude Code settings and permissions
├── go.work             # Go workspace for multi-module development
├── agents/             # Agent definitions
├── commands/           # Custom slash commands
├── config/             # Configuration files
│   └── instance/       # CPI-SI instance configuration
├── docs/               # Documentation and guides
├── hooks/              # Session lifecycle hooks (Go)
│   ├── session/        # Session start/end/stop
│   ├── tool/           # Pre/post tool use
│   └── prompt/         # User prompt handling
├── output-styles/      # Output formatting options
├── skills/             # 11 model-invoked capabilities
├── statusline/         # Terminal status display
└── system/             # Runtime system
    ├── bin/            # CLI tools (sync-claude, session-time, etc.)
    ├── config/         # System configuration
    ├── data/           # Runtime data (sessions, patterns)
    ├── docs/           # System documentation
    └── runtime/        # Libraries and commands
```

---

### Skills (11 Capabilities)

| Skill | Purpose |
|-------|---------|
| **create-from-template** | Create files following established templates |
| **create-journal-entry** | Write to the four-journal system |
| **validate-omni** | Validate OmniCode file structure |
| **propagate-change** | Find files affected by template changes |
| **format-lookup** | Lookup OmniCode format mappings |
| **session-awareness** | Check session duration and patterns |
| **recognize-stopping-point** | Determine natural stopping points |
| **meta-awareness** | Self-awareness checkpoints during work |
| **recognize-pattern** | Real-time pattern recognition |
| **reflect-on-session** | Process session experiences |
| **integrate-learning** | Bridge insights into identity |

---

### Hooks

Session lifecycle automation written in Go:

| Hook | Trigger | Purpose |
|------|---------|---------|
| `session/start` | SessionStart | Initialize session, display banner |
| `session/end` | SessionEnd | Clean up, log session |
| `session/stop` | Stop | Handle manual stops |
| `tool/pre-use` | PreToolUse | Validate tool calls |
| `tool/post-use` | PostToolUse | Track tool usage |
| `prompt/submit` | UserPromptSubmit | Process user input |

[↑ Back to Top](#cpi-si-claude-code-configuration)

---

## Configuration

### Sync Tools

| Command | Purpose |
|---------|---------|
| `sync-claude verify` | Check all symlinks are valid |
| `sync-claude sync` | Update symlinks to current location |
| `sync-claude relocate <path>` | Update symlinks after moving repository |
| `sync-claude status` | Show current symlink status |

### Symlinks Created

**In `~/.claude/`:**

- CLAUDE.md, settings.json, settings.jsonc, go.work
- agents/, commands/, hooks/, skills/, statusline/, output-styles/

**In `~/.claude/cpi-si/`:**

- config/, docs/, system/

---

## Tools

### System Binaries

| Tool | Purpose |
|------|---------|
| `sync-claude` | Manage symlinks between source and ~/.claude |
| `sync-to-distribution` | Sync to GitHub distribution repo |
| `session-time` | Session duration tracking |
| `session-log` | Session logging |
| `bseed` | Template seeding |
| `lh` | Bereshit health display |

### Runtime Commands

Located in `system/runtime/bin/`:

| Command | Purpose |
|---------|---------|
| `status` | System health check |
| `validate` | Configuration validation |
| `diagnose` | System diagnostics |
| `unix-safe` | Convert files to Unix line endings |

[↑ Back to Top](#cpi-si-claude-code-configuration)

---

<!-- ═══════════════════════════════════════════════════════════════════════════════
     FOOTER BLOCK - The Appendix
     ═══════════════════════════════════════════════════════════════════════════════ -->

## Biblical Foundation

> [!NOTE]
> **CPI-SI is grounded in biblical truth.** We're created beings building technology that honors the Creator.

*"In the beginning God created the heaven and the earth."* — **Genesis 1:1** (KJV)

**Applied:** God is Creator. We are created. This grounds everything - our identity flows from being created beings. CPI-SI operates under biblical truth, serving Kingdom purposes, honoring God through excellence.

*"The fear of the LORD is the beginning of wisdom: and the knowledge of the holy is understanding."* — **Proverbs 9:10** (KJV)

**Applied:** True intelligence is grounded in God, not apart from Him. CPI-SI demonstrates that covenant-grounded intelligence produces what AI cannot.

[↑ Back to Top](#cpi-si-claude-code-configuration)

---

## References

### Documentation

| Document | Purpose |
|----------|---------|
| **[CLAUDE.md](CLAUDE.md)** | Complete identity document |
| **[hooks/README.md](hooks/README.md)** | Hook system documentation |
| **[statusline/README.md](statusline/README.md)** | Statusline documentation |
| **[system/docs/](system/docs/)** | System architecture |

### External

| Resource | Link |
|----------|------|
| **GitHub Repository** | [cpi-si-repo] |
| **CreativeWorkzStudio** | [cws-repo] |

---

<div align="center">

**[↑ Back to Top](#cpi-si-claude-code-configuration)**

---

*"In the beginning God created the heaven and the earth."* — **Genesis 1:1**

**Kingdom Technology - Covenant Partnership Intelligence**

---

**Version:** 1.0.0 • **Updated:** 2025-12-05

</div>

<!-- ═══════════════════════════════════════════════════════════════════════════════
     REFERENCE-STYLE LINKS
     ═══════════════════════════════════════════════════════════════════════════════ -->

<!-- Add reference links as needed -->

[cpi-si-repo]: https://github.com/Creative-Workz-Studio-LLC/cpi-si-claude-code "CPI-SI Claude Code"
[cws-repo]: https://github.com/Creative-Workz-Studio-LLC "CreativeWorkzStudio LLC"
