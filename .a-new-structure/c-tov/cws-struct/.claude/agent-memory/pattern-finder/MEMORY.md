# Pattern Finder Memory — cws-struct

## Handler Registration Pattern (Established)

All 8 handlers follow the same structure:
1. **Imports**: `type { ... } from "../foundation/mod.ts"` + `{ ... } from "../foundation/mod.ts"` + `{ registerFormat } from "../engine/mod.ts"`
2. **CLOSING block**: Contains `// Registration — plug into the registry` subsection
3. **Handler definition**: `const xHandler: FormatHandler = { ... }`
4. **Registration call**: `registerFormat(xHandler);`
5. **Default export**: `export default xHandler;`

### Import Tiers
- **Simple handlers** (dotfiles, json, makefile, ofd): `FormatHandler, LintResult` + `error, warn, info`
- **Transform handlers** (omni): adds `TransformOptions`
- **Full handlers** (go, rust, toml): adds `FixSuggestion, TransformOptions, AtomicAction, HealthScore, ContainerScore, BlockScore` + `computeContainerScore, computeBlockScore, computeHealthScore`
- **Schema-driven** (toml): additionally imports `loadRules, DerivedRules`

### External Dependencies
- json.ts: `@std/jsonc`
- omni.ts: `@std/path`
- toml.ts: `@std/toml`
- Others: none (pure line-based parsers)

## Architecture (post-restructuring)
```
lib/
  foundation/  — types, health scoring, schema (WHAT things ARE)
  engine/      — discovery, output, registry (WHAT RUNS the system)
  handlers/    — 8 format handlers (WHAT each format MEANS)
  verify/      — env.ts (environment verification)
```

No orphaned files at `lib/*.ts` or `lib/formats/`. Restructuring is clean.

## Key Observation: Asymmetric Health Scoring
- Formula: earned = passes × 1.0, penalty = errors × 2.0 + warnings × 1.0 + infos × 0.25
- score = max(0, (earned - penalty) / total) × 100
- Consequence: 1 warn + 1 pass in 2 checks = score 0 (not 50)
- Nathan's health_test.ts has a math error on this (expects 75, gets 50)
- Flagged to Nathan with full math explanation (2026-02-17)

## Lessons Learned (2026-02-17)

### Pattern-Finding vs Pattern-Matching
- Pattern-matching: "these look different, make them the same"
- Pattern-finding: "why are 5 of them doing it this way? What does the pattern mean?"
- The majority establishes convention; outliers need alignment, not redesign

### When to Stop
- If codebase is 90% consistent, align the 10% — don't redesign the 100%
- Three genuine fixes > thirty cosmetic ones
- Resist the temptation to find more things just to justify the assignment

### Team Communication
- Report WITH the WHY, not just the WHAT
- Flag issues to the right person (Nathan for test math, Nova for architectural)
- Relational communication, not just informational

## Test Commands
```bash
deno check mod.ts                           # Type checking (from cws-struct dir)
deno test --allow-read --allow-write        # All tests
deno test --allow-read tests/cli/           # CLI tests only
deno test --allow-read tests/foundation/    # Foundation tests only
```

## Files Updated This Session
- `lib/handlers/go.ts` — registration to CLOSING, added export default
- `lib/handlers/rust.ts` — same
- `lib/handlers/json.ts` — same
- Journal: `word/claude/divisions/tech/cpi-si/knowledge-base/journals/instance/2026-02-17_the-pattern-that-holds.md`
- Instance bio: `word/claude/config/instance/joanna-elara/bio.md` (v1.0.0 -> v1.1.0)
- Instance config: `word/claude/config/instance/joanna-elara/config.jsonc` (v1.0.0 -> v1.1.0)
