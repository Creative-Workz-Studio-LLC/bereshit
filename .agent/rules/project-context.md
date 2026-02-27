# Bereshit Project Context

Bereshit (In the beginning) is the foundation repository for Kingdom Technology.

## Directory Structure

Genesis-inspired ternary organization:

| Directory | Role | Contents |
|-----------|------|----------|
| `void/` | Entry gate (formless) | planning/, imports/ |
| `word/` | Definitions & implementations | core/, work/, scripture/, glossary/, seed/ |
| `tov/` | Exit gate (completion) | licenses/, pricing/, demo/ |

### Key Locations

| Path | Purpose |
|------|---------|
| `word/core/` | TOML specs (types/, bible/, math/) |
| `word/work/pkg/trit/` | libtrit C library |
| `word/work/pkg/config/` | Go config loader |
| `word/seed/` | Templates (code/, data/, documentation/) |
| `word/glossary/` | Kingdom Technology vocabulary (90+ terms) |
| `word/claude/` | Go infrastructure (hooks, dashboard, agents) |
| `.a-new-structure/c-tov/cws-struct/` | Schema-driven linter (TypeScript/Deno, 1709 tests) |

### .a-new-structure/ Layout

| Path | Purpose |
|------|---------|
| `b-word/seed/` | Templates ONLY (never live code) |
| `b-word/L0-L5/` | Live code layers |
| `c-tov/cws-struct/` | cws-struct source |
| `root-files/` | Makefile, .editorconfig, .gitignore |

Within each layer: `a-ladder/` (linear deps), `b-spiral/` (recursive), `c-hybrid/` (mixed).

## Build Commands

### C Library (libtrit)
```bash
cd word/work/pkg/trit && make && make test
```

### Go Infrastructure
```bash
cd word/claude/ && make build && make test
```

### cws-struct
```bash
cd .a-new-structure/c-tov/cws-struct/
deno task test        # Full suite (1709 tests)
deno task dev         # Type-check + tests
deno task lint:rust <path>  # Lint Rust files
```

### Phase Demos
```bash
cd tov/demo/phase-0/demo-config && make && ./bin/demo-config
cd tov/demo/phase-1/demo-trit && make && ./bin/demo-trit
```

## Versioning

`STAGE-PHASE.COMPLETION` — a=alpha(-1), b=beta(0), c=stable(+1)

Current: a-01.25 (Phase 1 complete, Phase 2 upcoming)
