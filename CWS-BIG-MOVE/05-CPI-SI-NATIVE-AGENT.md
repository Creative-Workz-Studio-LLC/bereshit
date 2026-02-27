# CPI-SI Native Agent: Design Document

> *"Except the LORD build the house, they labour in vain that build it."* — Psalm 127:1

**Date:** 2026-02-24
**Status:** Phase 1-2 COMPLETE — core agent built and working
**Language:** Go (primary), with C foundation (libtrit)
**Binary:** `~/.local/share/claude/bin/cpisi-agent` (36MB)

---

## Vision

A Go binary that IS the CPI-SI agent. Not an AI tool with identity bolted on.
Our own process, our own state machine, our own identity — calling LLM APIs
when it needs to reason.

```
CURRENT:    Claude Code (substrate) <-- CLAUDE.md (identity loaded in)
INTERIM:    Gemini CLI (substrate)  <-- GEMINI-CONTEXT.md (identity loaded in)
DESTINATION: cpisi-agent (Go binary) -- IS identity -- calls APIs to think
```

## Architecture

### The Three Layers

```
+------------------------------------------+
|           SPIRIT (Specifications)         |
|  TOML specs, identity configs, templates  |
+------------------------------------------+
|           MIND (Go Agent Binary)          |
|  State machine, tools, API routing,       |
|  hooks, identity, session management      |
+------------------------------------------+
|           BODY (C Foundation)             |
|  libtrit, mental construct, trit math     |
+------------------------------------------+
```

This maps to the existing Body/Mind/Spirit architecture.
The agent binary IS the Mind layer — currently implemented as
Claude Code hooks/packages. The native agent collapses all of
that into a single, owned binary.

### Core Components

```
cmd/cpisi-agent/
  main.go              # Entry point, identity loading, REPL

pkg/agent/
  agent.go             # Core agent loop (think -> act -> observe)
  identity.go          # Identity loading from bio.md, config.jsonc
  session.go           # Session state, persistence, memory

pkg/agent/reasoning/
  provider.go          # Interface: Send(prompt) -> Response
  anthropic.go         # Claude API (Sonnet, Opus, Haiku)
  gemini.go            # Gemini API
  deepseek.go          # DeepSeek API
  ollama.go            # Local models via Ollama
  router.go            # Model selection based on task complexity

pkg/agent/tools/
  tool.go              # Tool interface
  file_read.go         # Read files
  file_write.go        # Write files
  file_edit.go         # Edit files (targeted replacement)
  shell.go             # Execute shell commands
  grep.go              # Search file contents
  glob.go              # Find files by pattern
  web.go               # Web fetch/search

pkg/agent/hooks/
  lifecycle.go         # Hook system (pre/post tool, session start/end)

pkg/agent/state/
  machine.go           # Mental construct state machine
  health.go            # Health tracking
  hebrew.go            # Hebrew state management
```

### The Agent Loop

```
1. LOAD IDENTITY
   - Read bio.md, config.jsonc
   - Initialize state machine
   - Load session state (if resuming)

2. RECEIVE INPUT
   - Terminal prompt (interactive)
   - Piped input (non-interactive)
   - API call (SDK mode)

3. THINK (call LLM API)
   - Build prompt: system (identity) + context + user input
   - Select model based on task complexity:
     - Quick question -> Haiku / Gemini Flash
     - Coding task -> Sonnet / Gemini Pro
     - Architecture -> Opus (if budget allows)
   - Send to API, receive response with tool calls

4. ACT (execute tools)
   - Parse tool calls from LLM response
   - Execute tools (file ops, shell, search)
   - Collect results

5. OBSERVE (process results)
   - Feed tool results back to LLM
   - Update state machine
   - Fire hooks

6. RESPOND
   - Present final response to user
   - Update session state
   - Log exchange

7. LOOP or HALT
```

### Model Routing

```go
type TaskComplexity int

const (
    Quick    TaskComplexity = iota  // Haiku, Gemini Flash
    Standard                        // Sonnet, Gemini Pro
    Complex                         // Opus (budget-gated)
    Local                           // Ollama (when available)
)

func SelectModel(complexity TaskComplexity, budget Budget) Provider {
    // Check budget first
    if budget.Remaining() < threshold {
        return OllamaProvider  // Fall back to free local
    }

    switch complexity {
    case Quick:
        return HaikuProvider
    case Standard:
        return SonnetProvider
    case Complex:
        if budget.AllowsOpus() {
            return OpusProvider
        }
        return SonnetProvider
    default:
        return GeminiProvider  // Free fallback
    }
}
```

### Identity as Code

The identity system is NOT a prompt template. It's Go structs:

```go
type Identity struct {
    Name        string          // "Nova Dawn"
    Nature      string          // "CPI-SI Instance"
    Anchor      BiblicalRef     // Genesis 1:1
    Origin      time.Time       // 2024-10-13
    Partner     string          // "Seanje Lenox-Wise"
    Mission     string          // Company mission
    Company     CompanyInfo     // CWS LLC
    Personality PersonalityProfile
    Standards   QualityStandards
}

func (id *Identity) SystemPrompt() string {
    // Generate system prompt from identity
    // This is where identity becomes context for the LLM
}
```

## What We Already Have

| Component | Location | Reuse? |
|-----------|----------|--------|
| State machine | `pkg/core/statemachine/` | Direct reuse |
| Health tracking | hooks + state.jsonc | Adapt to agent |
| Session management | `hooks/session/` | Refactor into agent |
| CPI scoring | hooks | Integrate |
| Database | `pkg/foundation/database/` | Direct reuse |
| Path resolution | `pkg/util/fs/paths/` | Direct reuse |
| Config loading | `config/instance/nova_dawn/` | Read directly |
| cpisi CLI | `cmd/cpisi/` | Extend or wrap |
| libtrit | C library | FFI bridge |

## Build Strategy

### Phase 1: Minimal Agent — DONE

- [x] Go binary with REPL (`cmd/cpisi-agent/main.go`)
- [x] Universal provider interface (`pkg/agent/provider.go`)
- [x] 5 stdlib-only tools (`pkg/agent/tools.go`) — fileRead, fileWrite, shell, grep, listDir
- [x] Agent loop: think/act/observe, max 20 iterations (`pkg/agent/agent.go`)
- [x] Identity loads from filesystem (`-identity` flag)
- [x] 10 unit tests passing (`pkg/agent/agent_test.go`)

### Phase 2: Multi-Provider — DONE

- [x] Gemini provider (`gemini.go`, `google.golang.org/genai` v1.47.0)
- [x] Anthropic provider (`anthropic.go`, `anthropic-sdk-go` v1.26.0)
- [x] OpenAI-compatible provider (`openai_compat.go`, `openai-go` v1.12.0)
      Covers: DeepSeek, Groq, OpenRouter, Ollama, OpenAI
- [x] Provider selection via `-provider` flag
- [x] Integrated into Makefile build system
- [x] Binary at `~/.local/share/claude/bin/cpisi-agent`
- [ ] Model routing based on task complexity (future)
- [ ] Prompt caching for cost savings (future)

### Phase 3: Full CPI-SI (Next)

- [ ] Streaming responses (real-time output)
- [ ] State machine integration (`pkg/core/statemachine/`)
- [ ] Hook system
- [ ] Health tracking
- [ ] Session persistence (SQLite via `modernc.org/sqlite`)
- [ ] Memory system (auto-memory, filesystem-based)
- [ ] MCP tool support
- [ ] Family agent dispatch
- [ ] Model routing / budget gating

## Dependencies (Actual)

| Dependency | Purpose | Go Package | Version |
|------------|---------|------------|---------|
| Google Gemini SDK | Gemini reasoning | `google.golang.org/genai` | v1.47.0 |
| Anthropic SDK | Claude reasoning | `github.com/anthropics/anthropic-sdk-go` | v1.26.0 |
| OpenAI SDK | OpenAI-compat reasoning | `github.com/openai/openai-go` | v1.12.0 |
| CPI-SI packages | Agent interface, tools | `cws.studio/pkg` | local |

All three SDKs are official Go SDKs — no raw HTTP needed.

## The Paradigm Shift

This agent is NOT "Claude Code but in Go." It's fundamentally different:

| Claude Code | CPI-SI Native Agent |
|-------------|---------------------|
| Identity loaded into substrate | Identity IS the agent |
| Substrate defines behavior | Agent defines behavior, API provides reasoning |
| Locked to one model provider | Routes to any provider |
| Subscription-dependent | API-key + free tier flexible |
| Claude Code updates change behavior | We control all behavior |
| Memory in Claude's system | Memory in our filesystem + DB |

The agent OWNS its identity. The LLM is a tool it uses, like a calculator.
The agent decides WHAT to think about. The LLM does the thinking.
The agent evaluates the result against its standards.

---

> *"In the beginning God created the heavens and the earth."* — Genesis 1:1

We're creating. Not just using someone else's creation.
