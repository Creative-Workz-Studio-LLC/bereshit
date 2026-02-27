# Code Standards

## Block Patterns

| Pattern | Blocks | Use |
|---------|--------|-----|
| **3-Block** | METADATA -> CONTENT -> CLOSING | Data files (configs, settings) |
| **4-Block** | METADATA -> SETUP -> BODY -> CLOSING | Code files |
| **5-Block** | METADATA -> HEADER -> CONTEXT -> CONTENT -> FOOTER | Documentation |

## Quality Standards

The 3-Question Standard:
1. Would this honor God as your code reviewer?
2. Does this genuinely serve others?
3. Does this have eternal value?

Yes to all three = meets standard.

## Language Preferences

| Priority | Languages | Use Case |
|:--------:|-----------|----------|
| 1 | Go, Rust, C | Primary (compiled preferred) |
| 2 | TypeScript, Python | When necessary |

## Disciplines

| Discipline | Practice |
|------------|----------|
| Grep over glob | Find by meaning, not pattern |
| Filesystem first | `cp`/`mv` over load-then-write |
| Template first | Never write from scratch |
| Read before Edit | Always understand before modifying |
| Linux-first | LF endings, UTF-8, forward slashes |

## Templates

Never create files from scratch. Copy from templates:
```
word/seed/code/        # Go, C, Bash templates
word/seed/data/        # TOML, config templates
word/seed/documentation/  # AsciiDoc templates
```

## Tool Usage

- `cws-struct lint <format> <file>` — authoritative linter
- `deno task test` — cws-struct test suite
- `make help` — discover build targets before running raw commands
- Use task commands, not raw commands (deno task, make, npm run)
