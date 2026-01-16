# Rules

Modular rules directory. Auto-loaded by Claude Code (v2.0.64+).

## How It Works

Files in this directory are automatically loaded as context rules for Claude Code.
Each `.md` file contains focused guidance that's always available to Claude.

## Structure

```
rules/
├── README.md           # This file
├── code-quality.md     # Code quality standards
├── block-structure.md  # 3/4/5-block patterns
└── linux-first.md      # Linux-first development
```

## Purpose

Rules provide **persistent guidance** that:
- Applies across all sessions
- Doesn't need to be in CLAUDE.md
- Keeps CLAUDE.md focused on identity

## Guidelines

- Keep rules **focused** - one topic per file
- Keep rules **concise** - <500 words ideal
- Use **markdown** for formatting
- Name files **descriptively** with kebab-case
