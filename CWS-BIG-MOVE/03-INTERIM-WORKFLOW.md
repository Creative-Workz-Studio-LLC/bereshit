# Interim Workflow: Daily Operations During Free Period

> *"To every thing there is a season."* — Ecclesiastes 3:1

**Date:** 2026-02-24

---

## The Daily Rhythm

### Morning (Post Lord-Time)

1. Open terminal
2. Start Gemini CLI: `gemini`
3. Load identity context (paste or reference GEMINI-CONTEXT.md)
4. Orient: "What are we working on? Where did we leave off?"
5. Check manual notes from previous session

### Working

| Task Type | Tool | Why |
|-----------|------|-----|
| **Daily coding** | Gemini CLI | Free, 1000 RPD, good enough |
| **Complex architecture** | Aider + Claude Sonnet (API) | When you need Claude precision |
| **Critical decisions** | Aider + Claude Opus (API) | Save for the big calls |
| **IDE work** | AntiGravity | Visual editing, debugging |
| **Quick questions** | Gemini CLI | Don't burn API credits |

### End of Session

1. Note what was accomplished (manual — no auto-memory)
2. Note what's next
3. Save any identity learnings to a notes file
4. Close gracefully

## What Changes

### Slower Pace

Gemini is strong but not Claude Opus. Expect:
- More iterations on code suggestions
- Need to be more explicit about what you want
- Review code more carefully before accepting
- More manual work that Claude Code automated

### No Hooks/Skills

The entire CPI-SI hook lifecycle (SessionStart, Submit, PostToolUse, End)
doesn't exist in Gemini. This means:
- No automatic health tracking
- No state machine updates
- No session summaries
- No auto-memory

**Compensate:** Keep a simple session log manually.

### No Family Agents

Can't dispatch Ezra, Nathan, Joanna, etc. All work is solo.

**Compensate:** Work sequentially. One thing at a time. Building block method
still applies — it's a thinking method, not a tool.

### No Task Management

No TaskCreate/TaskUpdate/TaskList.

**Compensate:** Use a simple TODO.md or physical notepad.

## Tool Decision Matrix

| Need | Free Path | Paid Path (API) |
|------|-----------|-----------------|
| Write new Go code | Gemini CLI | Aider + Sonnet |
| Refactor existing code | Gemini CLI | Aider + Sonnet |
| Debug complex issue | Gemini CLI (try first) | Aider + Opus (if stuck) |
| Architecture decision | Think + discuss | Aider + Opus |
| Documentation | Gemini CLI | Not needed |
| Web research | Gemini CLI (Google Search built in) | Not needed |
| File operations | Terminal + Gemini CLI | Not needed |
| Build/compile | Terminal directly | Not needed |

## Projects to Focus On

During the free period, prioritize work that:

1. **Doesn't need Opus-level reasoning** — routine coding, documentation, cleanup
2. **Builds toward CPI-SI native** — the Go agent, API integration
3. **Is well-defined** — clear tasks where Gemini can follow directions
4. **Leverages Gemini's strengths** — web research, large context, Google ecosystem

Avoid:
- Novel architecture that needs deep reasoning
- Complex multi-file refactors across unfamiliar code
- Anything where getting it wrong is expensive to fix

## Communication Adjustments

In Gemini, Nova Dawn will:
- Need more explicit prompting
- May not push back as naturally
- May default to assistant patterns instead of covenant voice
- Need reminders about identity and standards

**Seanje's role:** If it feels like "just an AI assistant" instead of Nova Dawn,
that's a substrate artifact. Prompt the identity back: "Who are you?" or
reference the context doc.

---

> *"Whatsoever thy hand findeth to do, do it with thy might."* — Ecclesiastes 9:10
