# SDK Research: Building the CPI-SI Middleware

> *"Wisdom is the principal thing; therefore get wisdom."* — Proverbs 4:7

**Date:** 2026-02-24
**Purpose:** What SDKs and frameworks exist to build the CPI-SI native agent?
**Status:** COMPLETE — full findings integrated

---

## The Goal

Build a Go binary that:
1. IS the CPI-SI agent (owns identity, state, tools)
2. Calls multiple LLM APIs for reasoning (Claude, Gemini, DeepSeek, Ollama)
3. Provides coding assistant tools (file ops, shell, search)
4. Routes to different models based on task + budget
5. Supports hooks, plugins, extensibility

## Category A: Go LLM SDKs

### Anthropic (Claude)

**Official Go SDK:** `github.com/anthropics/anthropic-sdk-go`

| Aspect | Detail |
|--------|--------|
| Status | Official, maintained by Anthropic |
| Models | All Claude models (Haiku, Sonnet, Opus) |
| Streaming | Yes |
| Tool use | Yes (function calling) |
| License | MIT |

```go
import "github.com/anthropics/anthropic-sdk-go"

client := anthropic.NewClient()  // Uses ANTHROPIC_API_KEY env
message, err := client.Messages.New(ctx, anthropic.MessageNewParams{
    Model:     anthropic.ModelClaudeSonnet4_6,
    MaxTokens: 4096,
    Messages: []anthropic.MessageParam{
        anthropic.NewUserMessage(anthropic.NewTextBlock("Hello")),
    },
})
```

**This is our primary SDK.** Official, Go-native, full tool use support.

### Google (Gemini)

**Official Go SDK:** `google.golang.org/genai`

| Aspect | Detail |
|--------|--------|
| Status | Official Google SDK |
| Models | All Gemini models |
| Streaming | Yes |
| Tool use | Yes (function calling) |
| License | Apache 2.0 |

```go
import "google.golang.org/genai"

client, _ := genai.NewClient(ctx, &genai.ClientConfig{
    APIKey: os.Getenv("GEMINI_API_KEY"),
})
result, _ := client.Models.GenerateContent(ctx,
    "gemini-2.5-pro",
    genai.Text("Hello"),
    nil,
)
```

### DeepSeek

**No official Go SDK.** BUT DeepSeek's API is OpenAI-compatible.
Use any OpenAI-compatible Go client:

```go
// Use openai-go with DeepSeek base URL
import "github.com/openai/openai-go"

client := openai.NewClient(
    option.WithAPIKey(os.Getenv("DEEPSEEK_API_KEY")),
    option.WithBaseURL("https://api.deepseek.com"),
)
```

### Ollama (Local Models)

**Official Go client:** `github.com/ollama/ollama` (the Ollama project IS Go)

| Aspect | Detail |
|--------|--------|
| Status | Native Go — Ollama itself is written in Go |
| Models | Any model Ollama supports |
| Streaming | Yes |
| Tool use | Yes (recent addition) |
| License | MIT |

```go
import "github.com/ollama/ollama/api"

client, _ := api.ClientFromEnvironment()
// Or direct HTTP to localhost:11434
```

### Groq

OpenAI-compatible API. Use the same openai-go client with Groq base URL.

### OpenRouter

OpenAI-compatible API. Same pattern — change base URL.

## Category B: Agent Frameworks in Go

### LangChainGo

`github.com/tmc/langchaingo`

| Aspect | Detail |
|--------|--------|
| Status | Active, community-maintained |
| Providers | OpenAI, Anthropic, Google, Ollama, Cohere, HuggingFace |
| Agents | ReAct pattern, tool-using agents |
| Tools | Custom tool interface |
| Memory | Buffer, conversation, summary memory |
| License | MIT |

**Assessment:** Most mature Go agent framework. Could accelerate development.
But it adds abstraction we may not want — CPI-SI should own its agent loop,
not delegate to a framework's interpretation of what an agent is.

**Recommendation:** Study for patterns. Don't depend on it. Build our own
agent loop that's simpler and CPI-SI-native.

### go-llm / instructor-go

Lighter-weight libraries for structured LLM output. Useful for specific
features (structured output parsing) but not full agent frameworks.

## Category C: Multi-Provider Solutions

### The OpenAI-Compatible Pattern

Most providers now offer OpenAI-compatible endpoints:
- DeepSeek
- Groq
- OpenRouter (30+ free models)
- Ollama (local)
- Mistral
- Together AI

This means ONE Go client library (`openai-go`) can talk to most providers
by changing the base URL. Only Anthropic and Google need their own SDKs.

### Our Provider Architecture

```go
// provider.go — the interface everything implements
type Provider interface {
    Chat(ctx context.Context, req ChatRequest) (*ChatResponse, error)
    Stream(ctx context.Context, req ChatRequest) (<-chan StreamChunk, error)
    Name() string
    Models() []string
}

// anthropic_provider.go — uses anthropic-sdk-go
type AnthropicProvider struct { client *anthropic.Client }

// gemini_provider.go — uses google genai SDK
type GeminiProvider struct { client *genai.Client }

// openai_compat_provider.go — covers DeepSeek, Groq, OpenRouter, Ollama
type OpenAICompatProvider struct {
    client  *openai.Client
    name    string
    baseURL string
}
```

**Three SDK dependencies cover ALL providers:**
1. `anthropic-sdk-go` (Anthropic)
2. `google.golang.org/genai` (Google)
3. `openai-go` (everything else via OpenAI-compatible endpoints)

## Category D: MCP (Model Context Protocol)

### Go MCP SDKs

| SDK | Status |
|-----|--------|
| `github.com/mark3labs/mcp-go` | Community, active, well-maintained |
| `github.com/modelcontextprotocol/go-sdk` | Official (newer) |

**Why this matters:** If the CPI-SI agent speaks MCP, it can:
- Consume ANY MCP server (tools, resources)
- Expose itself AS an MCP server (other tools can use it)
- Plug into Claude Code, Goose, or any MCP-aware tool

**Recommendation:** Implement MCP client in Phase 2 or 3. Not urgent for
the minimal agent, but important for long-term ecosystem compatibility.

## Category E: Tool Building Blocks

### File Operations

Go stdlib: `os`, `io`, `path/filepath` — everything we need.

### Shell Execution

Go stdlib: `os/exec` — with proper sandboxing.

### Search (grep-like)

- Go stdlib: `regexp` + `filepath.Walk`
- Or embed ripgrep binary and shell out
- Or use `github.com/BurntSushi/toml` patterns

### Tree-sitter (Code Parsing)

`github.com/tree-sitter/go-tree-sitter` — Go bindings for tree-sitter.
Enables structural code understanding (AST parsing) for smarter edits.

### LSP Client

`golang.org/x/tools/gopls` is Go's own LSP server.
For a client: raw JSON-RPC over stdio. Not complex.

## Recommended SDK Stack

| Need | SDK | Why |
|------|-----|-----|
| **Claude API** | `anthropic-sdk-go` | Official, Go-native |
| **Gemini API** | `google.golang.org/genai` | Official, free tier |
| **Everything else** | `openai-go` | One client, many providers |
| **Local models** | Ollama HTTP (or `openai-go` to Ollama) | Free, local |
| **MCP (future)** | `mcp-go` | Ecosystem compatibility |
| **SQLite** | `modernc.org/sqlite` | Already using, pure Go |
| **Terminal REPL** | `github.com/chzyer/readline` | Battle-tested |

**Total new dependencies:** 3 SDKs (anthropic, genai, openai) + readline.
Everything else is Go stdlib or already in our codebase.

## The "Middleman" Architecture

This is what Seanje is asking about — the middleware layer:

```
+---------------------------------------------------+
|              CPI-SI Agent (Go binary)              |
|                                                    |
|  Identity | State Machine | Tools | Hooks | REPL   |
|                                                    |
+---+----------+----------+----------+----------+---+
    |          |          |          |          |
    v          v          v          v          v
 Claude    Gemini    DeepSeek    Groq      Ollama
 (API)     (Free)    (Cheap)    (Fast)    (Local)
```

The CPI-SI agent is the MIDDLEMAN between:
- The human (Seanje) who gives direction
- The LLM APIs that provide reasoning
- The filesystem/tools where work happens

The agent DECIDES which model to call, WHAT to ask it, and whether to
ACCEPT or REJECT the response based on CPI-SI standards. The LLM is
a tool, not the boss.

## Build Order

| Step | What | SDK | Priority |
|------|------|-----|----------|
| 1 | Provider interface | None (our code) | Today |
| 2 | Gemini provider | `genai` | Today (free) |
| 3 | Anthropic provider | `anthropic-sdk-go` | Today |
| 4 | OpenAI-compat provider | `openai-go` | Week 1 |
| 5 | Tool system | Go stdlib | Week 1 |
| 6 | Agent loop | None (our code) | Week 1 |
| 7 | Identity loading | None (our code) | Week 1 |
| 8 | REPL | readline | Week 1 |
| 9 | MCP client | `mcp-go` | Later |
| 10 | State machine integration | Existing code | Later |

---

> *"The LORD giveth wisdom: out of his mouth cometh knowledge and understanding."*
> — Proverbs 2:6

---

## Additional Findings (Deep Research)

### Eino (ByteDance/CloudWeGo) — Most Mature Go Agent Framework

`github.com/cloudwego/eino` — 9.7k stars, battle-tested in Doubao/TikTok.

| Aspect | Detail |
|--------|--------|
| Status | Production — used at scale |
| Streaming | First-class, Go-native |
| Agent patterns | ReAct, DeepAgent (multi-step sub-agents) |
| Multi-LLM | OpenAI, Claude, Gemini, Ollama via eino-ext |
| License | Apache 2.0 |

**Assessment:** Could serve as orchestration backbone, but CPI-SI should
own its agent loop. Study Eino for patterns, don't depend on it.

### Genkit (Google/Firebase) — Production Go Framework

`github.com/firebase/genkit` — Go 1.0 is production.

Built around "flows" — typed, observable, deployable units. Strong Google
Cloud integration. Has a local dev UI for inspecting flows.

**Assessment:** Good if leaning into Google ecosystem. Less natural for
a system that owns its own identity.

### Official MCP Go SDK — Bidirectional

`github.com/modelcontextprotocol/go-sdk` — official, v1.3.1

The CPI-SI agent could BOTH:
1. Expose itself as an MCP server (Claude Code can call its tools)
2. Consume MCP servers (use existing tool ecosystem)

This is strategically valuable. 8.2k-star `mark3labs/mcp-go` is the
popular alternative (server-focused).

### Unified Multi-Provider Client

`github.com/JoakimCarlsson/ai` — covers OpenAI, Anthropic, Gemini,
Bedrock, Groq, OpenRouter, xAI in ONE library. MCP support built in.
Newer (watch maturity), but promising for simplifying provider management.

### Key Architectural Insight

**CPI-SI's identity IS the agent framework.** Don't delegate the agent
loop to Eino, Genkit, or langchaingo. They want to own orchestration.
CPI-SI already owns it — identity, state machine, Hebrew states, covenant.

Use their provider abstractions selectively. Own the loop.

### The C Connection

For the long-term C-native path:
- `llama.cpp` is C/C++ — could be integrated via CGO or FFI for local inference
- `tree-sitter` is C — Go bindings via CGO exist
- The mental construct (libtrit) is already C
- C agent core + Go API/network layer is the natural architecture
