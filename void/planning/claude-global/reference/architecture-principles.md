# Reference: Architecture Principles

> *"Let all things be done decently and in order."* — 1 Corinthians 14:40

---

## Anchor Chain

```
Genesis 1:1 (God created with order)
    ↓
Kingdom Technology (excellence honors God)
    ↓
Architecture Principles (structure enables excellence)
```

---

## Development → Production Workflow

| Stage | Location | Purpose |
|-------|----------|---------|
| **Development** | `claude-global/` | Edit code here |
| **Production** | `bereshit/word/claude/` | Symlinks point here |
| **Runtime** | `~/.claude/` | Symlinks to production |

**Command:** `make deploy` (builds → syncs → rebuilds)

---

## Thin Callers, Fat Packages

| Component | Lines | Role |
|-----------|-------|------|
| `main.go` / hooks | ~50 | Caller - orchestrates |
| `pkg/*/` | Many | Worker - does the work |

**Principle:** Entry points CALL, packages WORK.

---

## Block Patterns

| Type | Blocks | Use |
|------|--------|-----|
| **3-Block** | METADATA → CONTENT → CLOSING | Data files |
| **4-Block** | METADATA → SETUP → BODY → CLOSING | Code |
| **5-Block** | METADATA → HEADER → CONTEXT → CONTENT → FOOTER | Documentation |

---

## Build Infrastructure

**Always use Makefiles.** Never ad-hoc `go build`.

| Target | Purpose |
|--------|---------|
| `make build` | Build all modules |
| `make deploy` | Sync + rebuild in production |
| `make test` | Run all tests |
| `make clean` | Remove artifacts |

---

## State Machine Integration

| Layer | What |
|-------|------|
| **Schema** | `pkg/cpisi/schema/config/` - vocabulary (static) |
| **Runtime** | `config/statemachine/runtime/` - state (dynamic) |
| **Display** | Statusline reads runtime, emits OSC to WezTerm |
| **Cognition** | Hooks inject context via `pkg/cognition/` |

---

## The Three Questions

Before any work:

1. **Does this honor God?**
2. **Does this serve others?**
3. **Does this have eternal value?**

Yes to all three = proceed.
