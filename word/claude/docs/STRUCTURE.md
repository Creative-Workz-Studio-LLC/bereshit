# Claude Code Canonical Folder Structure

> **This document defines the anchor points** — the fixed folder and file names that Claude Code expects. These cannot be renamed without breaking the system.

**Last Updated:** 2025-12-15
**Source:** [Claude Code Official Documentation](https://code.claude.com/docs/en/settings)

---

## Directory Structure

```
~/.claude/                              # User-level (global)
├── CLAUDE.md                           # User-level memory/instructions
├── CLAUDE.local.md                     # Personal preferences (git-ignored)
├── settings.json                       # User settings (all projects)
├── settings.local.json                 # Local user settings (git-ignored)
├── .mcp.json                           # User-scoped MCP servers
│
├── agents/                             # User subagents (all projects)
│   └── [agent-name].md                 # Agent definition
│
├── commands/                           # User slash commands
│   └── [command-name].md               # Command definition
│
├── output-styles/                      # User output styles
│   └── [style-name].md                 # Style definition
│
├── rules/                              # User rules (all projects)
│   └── [rule-name].md                  # Rule definition
│
├── skills/                             # User skills
│   └── [skill-name]/
│       ├── SKILL.md                    # Skill definition (required)
│       └── [supporting files]          # Optional references, scripts
│
└── [runtime data]                      # Session, history, etc. (managed by Claude Code)
    ├── session/
    ├── file-history/
    ├── history.jsonl
    └── ...

.claude/                                # Project-level (checked into git)
├── CLAUDE.md                           # Project memory/instructions
├── CLAUDE.local.md                     # Personal project prefs (git-ignored)
├── settings.json                       # Project settings (team shared)
├── settings.local.json                 # Local project settings (git-ignored)
├── .mcp.json                           # Project-scoped MCP servers
│
├── agents/                             # Project subagents (highest priority)
│   └── [agent-name].md
│
├── commands/                           # Project slash commands
│   └── [command-name].md
│
├── output-styles/                      # Project output styles
│   └── [style-name].md
│
├── rules/                              # Project rules (modular)
│   └── [rule-name].md
│
└── skills/                             # Project skills
    └── [skill-name]/
        └── SKILL.md
```

---

## Anchor Points (CANNOT be renamed)

These are the fixed points Claude Code expects. Renaming breaks the system:

| Anchor | Type | Purpose |
|--------|------|---------|
| `~/.claude/` | Directory | User-level root |
| `.claude/` | Directory | Project-level root |
| `CLAUDE.md` | File | Memory/instructions |
| `settings.json` | File | Settings (hooks, permissions, env) |
| `settings.local.json` | File | Local settings (git-ignored) |
| `agents/` | Directory | Subagent definitions |
| `commands/` | Directory | Slash command definitions |
| `output-styles/` | Directory | Output style definitions |
| `rules/` | Directory | Modular rules |
| `skills/` | Directory | Skill definitions |
| `.mcp.json` | File | MCP server configuration |

---

## File Formats

### settings.json

```json
{
  "permissions": {
    "allow": ["Bash(npm run:*)", "Read(*.md)"],
    "deny": ["Read(.env)", "Bash(curl:*)"],
    "additionalDirectories": ["../docs/"]
  },
  "env": {
    "MY_VAR": "value"
  },
  "hooks": {
    "SessionStart": [...],
    "PreToolUse": [...],
    "PostToolUse": [...]
  },
  "statusLine": {
    "type": "command",
    "command": "~/.claude/statusline/statusline"
  },
  "attribution": "none"
}
```

### agents/[name].md

```yaml
---
name: agent-name
description: When to use this agent
tools: Read, Grep, Glob, Bash
model: sonnet
skills: skill1, skill2
---

System prompt for the agent...
```

### commands/[name].md

```yaml
---
description: Brief command description
argument-hint: [arg1] [arg2]
allowed-tools: Bash(git:*), Read
---

Command prompt with $ARGUMENTS or $1, $2...
```

### output-styles/[name].md

```yaml
---
name: Style Name
description: Style description
keep-coding-instructions: true
---

Custom system prompt instructions...
```

### rules/[name].md

```yaml
---
paths: src/**/*.ts
---

# Rule Title

Rule content (applied only to matching paths)...
```

### skills/[name]/SKILL.md

```yaml
---
name: skill-name
description: What this skill does and when to use it
allowed-tools: Read, Grep, Glob
---

# Skill Title

Skill instructions...
```

---

## Hook Events

| Event | Purpose | Matcher Support |
|-------|---------|-----------------|
| `SessionStart` | Session begins | startup, resume, clear, compact |
| `SessionEnd` | Session ends | No |
| `Stop` | Main agent finishes | No |
| `SubagentStart` | Subagent begins | No |
| `SubagentStop` | Subagent finishes | No |
| `PreToolUse` | Before tool execution | Tool name |
| `PostToolUse` | After tool execution | Tool name |
| `PermissionRequest` | Permission dialog | Tool name |
| `UserPromptSubmit` | User submits prompt | No |
| `Notification` | Notification sent | Notification type |
| `PreCompact` | Before compaction | manual, auto |

---

## Settings Precedence (Highest to Lowest)

1. **Managed Settings** (Enterprise) - System-wide
2. **Command Line Arguments**
3. **Local Project Settings** - `.claude/settings.local.json`
4. **Shared Project Settings** - `.claude/settings.json`
5. **User Settings** - `~/.claude/settings.json`

---

## What's Fixed vs. What's Configurable

### Fixed (Anchors)
- Folder names at `.claude/` root
- File names like `settings.json`, `SKILL.md`
- Hook event names
- Tool names

### Configurable (Inside Anchors)
- File contents
- Subdirectory organization (e.g., `rules/frontend/`, `commands/backend/`)
- Additional files within skill folders
- Hook implementations
- Permission rules

---

## CPI-SI Extensions

This repository extends Claude Code with:

| Extension | Location | Purpose |
|-----------|----------|---------|
| `hooks/` | Implementation | Go-based hook handlers |
| `pkg/` | Library | Shared Go packages |
| `statusline/` | Implementation | Custom status line |
| `bin/` | Binaries | Compiled hook binaries |
| `config/` | Identity | Instance/user identity configs |
| `system/` | Schemas | JSON schemas for data validation |

These live INSIDE the `.claude/` structure and don't change Claude Code's expectations.

---

## Sources

- [Claude Code Settings](https://code.claude.com/docs/en/settings)
- [Claude Code Hooks](https://code.claude.com/docs/en/hooks)
- [Claude Code Subagents](https://code.claude.com/docs/en/sub-agents)
- [Claude Code Skills](https://code.claude.com/docs/en/skills)
- [Claude Code Memory](https://code.claude.com/docs/en/memory)
- [Claude Code Slash Commands](https://code.claude.com/docs/en/slash-commands)
- [Claude Code Output Styles](https://code.claude.com/docs/en/output-styles)
