# cws-struct Project Context

cws-struct is the CPI-SI DAR (Detect-Assess-Report) engine — a schema-driven linter, transformer, and creator for structured code and data files. TypeScript/Deno. 1709 tests. 8 format handlers.

## Architecture

```
cws-struct/
  mod.ts                 Entry point (CLI router)
  lib/
    engine/              DAR engine (output, scoring, witnesses)
    foundation/          Core types, errors, constants
    handlers/            Format handlers (one per format)
      go/ rust/ toml/ json/ makefile/ dotfiles/ omni/ ofd/
    data/                Error codes, schemas, recipes
    schemas/             TOML schemas (loaded at runtime)
  scripts/
    witness.ts           Dimensional error code coverage
    test-live.ts         Live test dashboard
    test-report.ts       Test report generator
  tests/
    existence/R50/       Layer 0 tests
    organization/R25/    Layer 1 tests
    content/R10/         Layer 2 tests
    pattern/R05/         Layer 3 tests
    pattern/R03/         Layer 3b tests (recursion)
    tool/T00/            Tool-level tests
    integration/         Transform tests
```

## Task Commands (USE THESE)

### Linting
```bash
deno task lint:rust <targets>       # Lint Rust files
deno task lint:go <targets>         # Lint Go files
deno task lint:toml <targets>       # Lint TOML files
deno task lint:json <targets>       # Lint JSON/JSONC files
deno task lint:makefile <targets>   # Lint Makefiles
deno task lint:dotfiles <targets>   # Lint dotfiles
deno task lint:omni <targets>       # Lint OmniCode files
deno task lint:verbose              # Verbose mode
deno task lint:errors               # Errors only
```

### Transform & Create
```bash
deno task transform:rust <targets>  # Add structure to Rust files
deno task transform:go <targets>    # Add structure to Go files
deno task transform:dry             # Preview without writing
deno task create:go <subtype> <dest>
deno task create:rust <subtype> <dest>
```

### Testing
```bash
deno task test                      # Full suite (1709 tests)
deno task dev                       # Type-check + tests
deno task test:rust:blocks          # Rust R50 tests
deno task test:go:ordering          # Go R25 tests
deno task test:transform            # Transform tests
deno task witness:grid              # Error code coverage
```

## Health Scoring

Files scored -100 to +100 across 4 layers:

| Layer | Weight | Checks |
|-------|--------|--------|
| R50 | 50% | Blocks, separators |
| R25 | 25% | Directives, identity, metadata |
| R10 | 10% | Content placement |
| R05 | 5% | Concept coverage, depth |

## Key Design Decisions

- Schema = Template = Linter (triangle collapsed)
- Pragma as covenant (file declares what it is, linter holds it accountable)
- Data-driven architecture (paths from recipe data, not code)
- Format handler registry (new format = new directory + register)
- Import maps: `@lib/` -> `./lib/`, `@test/` -> `./tests/`
