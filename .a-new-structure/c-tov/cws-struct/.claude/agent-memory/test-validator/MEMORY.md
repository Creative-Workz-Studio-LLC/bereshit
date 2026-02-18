# Nathan Emet — Test Validator Memory (cws-struct)

## Project: cws-struct
- **Location:** `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/`
- **Runtime:** Deno (TypeScript)
- **Test command:** `deno test --no-check --allow-read --allow-write --allow-run --allow-env`
- **Pre-existing type error:** `discovery.ts:144` — use `--no-check` flag for tests

## Test Architecture (220 tests as of 2026-02-17)

| File | Count | What It Tests |
|------|------:|---------------|
| `tests/handlers/toml_test.ts` | 58 | TOML lint integration + unit tests for 9 exported internals |
| `tests/handlers/rust_test.ts` | 45 | Rust lint + transform + classifyLine + subsection ordering |
| `tests/handlers/go_test.ts` | 36 | Go lint + classifyGoLine + subsection ordering |
| `tests/foundation/schema_test.ts` | 15 | loadRules, clearCache, DerivedRules structure |
| `tests/foundation/health_test.ts` | 13 | Asymmetric scoring: containers, blocks, health |
| `tests/engine/registry_test.ts` | 13 | registerFormat, getFormat, listFormats, detectFormat |
| `tests/cli/mod_test.ts` | 11 | CLI argument parsing, exit codes, command dispatch |
| `tests/foundation/types_test.ts` | 11 | error/warn/info factories, summarize |
| `tests/engine/discovery_test.ts` | 8 | discoverFiles, relativePaths |
| `tests/integration_test.ts` | 7 | Seed template files lint clean |
| `tests/verify/env_test.ts` | 3 | verifyEnvironment contract |

## Test Patterns
- **Fixture-driven:** Handler tests use `fixture("format/name.ext")` from `tests/helpers.ts`
- **Side-effect imports:** `import "../helpers.ts"` registers all handlers via barrel imports
- **Result filtering:** `errors()`, `warnings()`, `infos()`, `byRule()`, `hasRule()`, `hasMessage()`
- **Temp files for transforms:** Copy fixture to temp, transform, verify, clean up

## Key Gotchas
- `discovery.ts:144` has type mismatch (pre-existing) — always use `--no-check`
- `cli/mod_test.ts` filters `LD_LIBRARY_PATH` from env to avoid `--allow-env` requirement
- Health scoring is asymmetric: error=2x penalty, warn=1x, info=0.25x. Check the math carefully.
- Empty containers score 100 (no checks = nothing wrong). Empty blocks score 0 (no containers = unmeasured).

## First Day Learning (2026-02-17)
- **The anchor is the test result, not the tester's assumption.** When expectation diverges from actual, check the formula before filing a bug.
- Tests verify the tester's understanding as much as the code's correctness.
- Domain knowledge matters: generic test-writing produces test theater. CPI-SI agents catch semantic errors that generic agents would report as bugs.
- Working in parallel with other family members works because good architecture (badal) naturally isolates concerns.
- Journal written: `word/claude/divisions/tech/cpi-si/knowledge-base/journals/instance/2026-02-17_the-test-is-the-anchor.md`
