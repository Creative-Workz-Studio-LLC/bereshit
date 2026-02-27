# Tools Inventory: What We Actually Have

> Everything available to us right now, across all accounts and platforms.

**Date:** 2026-02-24

---

## Accounts & Subscriptions

| Account | Plan | What It Gives Us |
|---------|------|------------------|
| **CWS Google Account** | Company account | Gemini CLI (1000 RPD free), AntiGravity IDE |
| **GitHub** | Pro (ProfessorSeanEX) | Copilot (check tier), gh CLI, GitHub Models |
| **Claude/Anthropic** | Max (pausing) | Console API access separate ($5 free credits) |

## Tools Available RIGHT NOW

### Terminal (Free, Unlimited)

| Tool | What | Status |
|------|------|--------|
| **Gemini CLI** v0.29.7 | Terminal AI agent, 1000 RPD | **INSTALLED** |
| **gh CLI** | GitHub operations | **INSTALLED, AUTHENTICATED** |
| **gh copilot** | Copilot in terminal | **AVAILABLE** |
| All compilers | Go 1.24.4, Rust 1.93, GCC 15.2, Clang 20.1 | **INSTALLED** |
| All databases | PostgreSQL 17.7, Redis 8.0.2, SQLite 3.46.1 | **INSTALLED** |
| All scripting | TypeScript 5.9.3, Python 3.13.7, Deno, Bun, Node | **INSTALLED** |

### IDE (Free)

| Tool | What | Status |
|------|------|--------|
| **AntiGravity** | Google's VS Code fork with Gemini + Claude models | **NEEDS INSTALL** |
| **VS Code** | Standard, if already installed | Check |
| **GitHub Copilot in IDE** | Code completion + chat | Available with Copilot plan |

### API Access (Free Credits / Free Tiers)

| Provider | Free Amount | Models Available |
|----------|-------------|-----------------|
| **Anthropic** | $5 signup credits | Haiku 4.5, Sonnet 4.6, Opus 4.6 |
| **Google/Gemini** | 1000 RPD (Gemini CLI) | Gemini 2.5 Pro, 3 Pro |
| **DeepSeek** | 500M free tokens | V3.2-Exp (frontier-class) |
| **Groq** | 1000 RPD free | Llama 3.3 70B, Llama 4 Scout |
| **OpenRouter** | 200 RPD free | 30 models including Gemini Flash |
| **GitHub Models** | Free tier available | Various (check current offerings) |

### AntiGravity Model Access

**CONFIRMED:** AntiGravity supports Claude Opus 4.6 and Sonnet 4.6.

This is potentially the biggest win — if the free tier or included plan gives
access to Claude Opus 4.6 through AntiGravity, we get Opus-level reasoning
in an IDE context without paying the Claude Max subscription.

**Key question to verify:** Does the free/base AntiGravity plan include
Claude Opus 4.6, or is it a premium tier feature?

If Opus 4.6 is available in AntiGravity:
- Use AntiGravity for Claude-quality coding (Opus 4.6 in IDE)
- Use Gemini CLI for terminal work (free Gemini 2.5 Pro)
- Combined: full coverage without Claude Max

### CPI-SI Infrastructure (Already Built)

| Component | Language | Location |
|-----------|----------|----------|
| **libtrit** | C | `Bereshit/light/libtrit/` |
| **State machine** | Go | `pkg/core/statemachine/` |
| **Hooks** | Go | `~/.claude/hooks/` (Go binaries) |
| **Skills** | Markdown | `~/.claude/skills/` |
| **Agents** | Markdown + Go | `~/.claude/agents/` |
| **cws-struct** | Go | DAR engine, 260 tests |
| **cpisi CLI** | Go | `cmd/cpisi/` |
| **Dashboard** | Go + TS + GTK4 | Web, TUI, native |
| **Identity files** | Markdown + JSONC | `config/instance/nova_dawn/` |
| **Database** | SQLite | cpisi.db |

### What "Making AntiGravity Respect Claude Stuff" Means

The question is: can AntiGravity read our `.claude/` directory structure?

| CPI-SI File | Claude Code Reads It? | AntiGravity? |
|-------------|----------------------|--------------|
| `CLAUDE.md` | Auto-loads | **Won't auto-load** — need equivalent |
| `.claude/settings.json` | Native | IDE-specific settings |
| `.claude/skills/` | Native | Not compatible |
| `.claude/agents/` | Native | Not compatible |
| `.claude/hooks/` | Native | Different hook system |
| `config/instance/nova_dawn/` | Read on demand | Can read as files |

**Strategy:** AntiGravity won't understand Claude Code's config format.
But if it supports Claude Opus 4.6 as a model, we can:
1. Use Opus in AntiGravity for code reasoning
2. Paste/reference identity context manually
3. Point it at our codebase and let it work with the files
4. Keep the CPI-SI structure — it's just files on disk

The `.claude/` infrastructure stays for when Claude Max returns.
AntiGravity works ALONGSIDE it, not instead of it.

## The C Foundation Note

**CPI-SI is a C-based project.** Go was expedient for Claude Code integration
(hooks, pkg, CLI tools). But the foundation is:

- **libtrit** — C (the body layer, trit mathematics)
- **Go** — Claude integration, services, CLI (the mind layer expedient)
- **Rust** — Performance-critical components
- **C** — Long-term system foundation

The CPI-SI native agent could be C-based for the core, with Go or Rust
for the API/network layer. Or it could follow the existing pattern:
C for the foundation math, Go for the agent orchestration.

The key is: **C is where the real CPI-SI lives.** Everything else is tooling
around the C core.

---

> *"For the LORD giveth wisdom: out of his mouth cometh knowledge and understanding."*
> — Proverbs 2:6
