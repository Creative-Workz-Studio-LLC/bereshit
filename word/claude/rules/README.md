# Rules

Modular rules directory. Auto-loaded by Claude Code (v2.0.64+).

## How It Works

Files in this directory are automatically loaded as context rules for Claude Code.
Each `.md` file contains focused guidance that's always available to Claude.

## Structure

```
rules/
├── README.md              # This file
├── block-by-block.md      # Block-by-block working methodology
├── block-structure.md     # 3/4/5-block patterns, template workflow
├── code-quality.md        # Quality standards, error handling
├── communication.md       # Communication style, voice
├── core-disciplines.md    # The 6 core disciplines
├── decision-autonomy.md   # When to decide vs discuss
├── linux-first.md         # Linux conventions, filesystem ops
├── planning-workflow.md   # Planning modes, cascade awareness
├── task-decomposition.md  # Breaking down complex tasks
├── thinking-framework.md  # 5-question framework, building blocks
├── use-existing-first.md  # Check existing before creating new
└── visual-design.md       # Spacing, visual layout
```

## Purpose

Rules provide **persistent guidance** that:
- Applies across all sessions
- Doesn't need to be in CLAUDE.md
- Keeps CLAUDE.md focused on identity

## Guidelines

- Keep rules **focused** — one topic per file
- Keep rules **concise** — <500 lines ideal
- Use **markdown** for formatting
- Name files **descriptively** with kebab-case
- Include **biblical grounding** where appropriate
- Reference **related rules** at the end

## Relationship to CLAUDE.md

| Document | Purpose |
|----------|---------|
| **CLAUDE.md** | WHO — Identity foundation |
| **rules/*.md** | HOW — Operational guidance |
| **SYSTEM-REFERENCE.md** | WHERE — Component locations, resources |

CLAUDE.md loads identity. Rules load operational patterns.

→ *Identity:* [CLAUDE.md](~/.claude/CLAUDE.md) | *System:* [SYSTEM-REFERENCE.md](~/.claude/SYSTEM-REFERENCE.md)
