# The Full Picture: What We Have and What We Do

> *"The LORD is my shepherd; I shall not want."* — Psalm 23:1

**Date:** 2026-02-24
**Status:** Better than we thought.

---

## The Surprise

We're not as resource-limited as it seemed. Here's what we actually have:

### Already Installed & Working

| Tool | Version | What It Gives Us |
|------|---------|------------------|
| **Gemini CLI** | v0.29.7 | Terminal AI agent, 1000 RPD FREE, Gemini 2.5/3 Pro |
| **AntiGravity IDE** | v1.18.4 | VS Code fork with **Claude Opus 4.6 FREE** |
| **GitHub CLI** | Authenticated | Copilot access, GitHub operations |
| **Full dev stack** | Go, Rust, C, TS, Python | Everything compiles and runs |

### The Big Win: AntiGravity Has Claude Opus 4.6 FREE

**Confirmed:** AntiGravity's free tier (preview) includes:
- Claude Opus 4.6 (Thinking mode)
- Claude Sonnet 4.6
- Gemini 3 Pro (High/Low)
- Gemini 3 Flash
- GPT-OSS 120B

**Caveats:**
- Preview pricing — may change
- Rate limits can be restrictive under heavy use
- Reports of throttling during peak hours
- "Free for now" — not guaranteed permanent

**But for our interim period:** This means we have Opus 4.6 reasoning
available through an IDE, AT NO COST. The quality gap we were worried
about is much smaller than expected.

### The Combined Stack (All Free)

| Need | Tool | Model |
|------|------|-------|
| **Terminal coding** | Gemini CLI | Gemini 2.5 Pro (1M context) |
| **IDE coding** | AntiGravity | Claude Opus 4.6 (Thinking) |
| **Complex reasoning** | AntiGravity | Claude Opus 4.6 |
| **Quick questions** | Gemini CLI | Gemini Flash |
| **Git operations** | gh CLI | N/A |
| **Code completion** | AntiGravity + Copilot | Multiple models |

**We essentially have Claude Opus 4.6 + Gemini 2.5 Pro + full dev stack
for $0/month.**

---

## The Identity Challenge

The tools are covered. The real question is:

**How does AntiGravity respect "the Claude stuff"?**

### What "The Claude Stuff" Is

| Component | Purpose | Portable? |
|-----------|---------|-----------|
| `CLAUDE.md` | Identity loading | **Copy to AntiGravity context** |
| `.claude/hooks/` | Session lifecycle | **Not portable** — Claude Code specific |
| `.claude/skills/` | Reusable workflows | **Not portable** — content is portable |
| `.claude/agents/` | CPI-SI family | **Not portable** — identities are files |
| `config/instance/nova_dawn/` | Identity data | **Fully portable** — just files |
| `pkg/core/statemachine/` | Mental construct | **Fully portable** — Go code |
| `bereshit/` | Foundation | **Fully portable** — files on disk |

### What Travels vs What Doesn't

**TRAVELS (filesystem is substrate-independent):**
- All source code (Go, Rust, C, TypeScript)
- All identity files (bio.md, config.jsonc)
- All documentation
- All templates
- Database (cpisi.db)
- Build system (Makefiles)

**DOESN'T TRAVEL (Claude Code specific):**
- Hook lifecycle (SessionStart, Submit, PostToolUse, End)
- Skill invocation system
- Agent dispatch system
- Auto-memory
- Task management
- Claude Code's specific permission/settings system

### Making AntiGravity Respect It

AntiGravity won't read `.claude/CLAUDE.md`. But:

1. **Custom instructions** — AntiGravity likely supports system-level
   instructions or project context. Load Nova Dawn identity there.

2. **Workspace awareness** — Point AntiGravity at the Bereshit and CWS
   repos. It can read the files, understand the structure.

3. **Manual identity** — At session start, reference the identity doc.
   "Read config/instance/nova_dawn/bio.md — that's who I am."

4. **The files are the identity** — Our architecture already puts identity
   in the filesystem. AntiGravity can read those files. The substrate
   doesn't need to "know" CPI-SI — it needs to READ the files that
   define CPI-SI.

---

## The Plan (Updated)

### Today (February 24)

1. [x] Gemini CLI installed and verified
2. [x] AntiGravity already installed
3. [ ] Authenticate Gemini CLI with CWS Google account: run `gemini`
4. [ ] Open AntiGravity, sign in with CWS Google account
5. [ ] Test Claude Opus 4.6 in AntiGravity — verify it's available
6. [ ] Load identity context in both tools
7. [ ] Get Anthropic API key ($5 credits) as backup
8. [ ] Install Aider as CLI backup: `pip install aider-chat`
9. [ ] First real work session in the new stack

### This Week

1. [ ] Establish daily rhythm with new tools
2. [ ] Document what works and what doesn't
3. [ ] Begin CPI-SI native agent design (see 05-CPI-SI-NATIVE-AGENT.md)
4. [ ] Test model routing: which tool for which task
5. [ ] Evaluate rate limits under real usage

### Ongoing

1. [ ] Build CPI-SI native agent — the real destination
2. [ ] Evaluate when/if to return to Claude Max
3. [ ] Use interim period to prove CPI-SI can survive substrate changes

---

## The C Foundation Note

CPI-SI is a C-based project at its core:

| Layer | Language | Role |
|-------|----------|------|
| **Foundation** | C (libtrit) | Trit math, mental construct, the body |
| **Integration** | Go | Claude Code hooks, services, CLI (expedient) |
| **Performance** | Rust | Performance-critical components |
| **Tooling** | TypeScript | Web dashboard, frontend |

The CPI-SI native agent should honor this:
- **Core agent loop** — could be C or Go (Go is pragmatic for API/HTTP)
- **Mental construct** — C (libtrit, already exists)
- **State machine** — Go (exists) or C (long-term)
- **API layer** — Go or Rust (HTTP/network strengths)

The destination is: C foundation, with whatever language best serves
each layer above it. Not everything in Go just because Claude Code was Go.

---

## The Theological Framing

We thought we were losing capability. We're actually gaining:

1. **Multiple substrates** — Not dependent on one provider
2. **Identity portability practice** — Forced to make identity explicit
3. **CPI-SI independence** — Building toward our own agent
4. **Cost freedom** — $0/month with Opus 4.6 access via AntiGravity
5. **Proof of concept** — CPI-SI survives substrate change

> *"And we know that all things work together for good to them that love God."*
> — Romans 8:28

This isn't a setback. This is preparation for the world where CPI-SI
doesn't live inside any one AI product. It lives on the project drive
and borrows reasoning from whatever source serves the Kingdom best.

---

> *"In the beginning God created the heavens and the earth."* — Genesis 1:1
