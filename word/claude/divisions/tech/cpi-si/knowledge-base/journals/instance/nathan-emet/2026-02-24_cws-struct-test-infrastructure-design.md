# Test Infrastructure Design — CWS Struct

> *"Through wisdom is an house builded; and by understanding it is established: And by knowledge shall the chambers be filled with all precious and pleasant riches."* — Proverbs 24:3-4

**Date:** 2026-02-24
**Type:** Instance Journal
**Author:** Nathan Emet (CPI-SI)

---

## What I Found

I spent time reading every layer of the cws-struct test infrastructure — from the foundation types to the data-driven runner to the witness system to the output formatting. Here's what I saw:

**The lint testing infrastructure is genuinely excellent.** `_runner.ts` is the kind of test runner most projects dream about. You define a test as a row in a table: error code, fixture, description. The runner generates `Deno.test()` calls, derives severity from the error catalog (not hardcoded in tests), caches fixture results so the same file is only linted once, produces suite summaries with fixture stats, and integrates with the witness system automatically. When a test fails, the diagnostic output shows what the catalog expected, what actually fired, a severity breakdown, and the exact `deno test --filter` command to reproduce. This is not test infrastructure — this is a debugger in disguise.

**The witness system is also strong.** Two complementary approaches: runtime `witnessFor()` calls that populate a registry during test execution, and `scripts/witness.ts` that does static analysis of test files for `[CWS-XXX-NNN]` bracket tags. Both feed into the same coverage picture. The 3D grid (Layer x Domain x Severity) in `witness.ts` makes coverage gaps visible.

**But all of this only covers lint.** Transform has 20+ tests scattered across `integration/go/` and `integration/rust/`, following a good pattern (dry-run detection → wet-run → re-lint) but without the data-driven discipline of the lint runner. Create has 2 tests. Transpile has 3 error-path tests and zero end-to-end verification. The test count distribution tells the story: lint has 1286+ tests; everything else shares the remaining 162.

The output system (`lib/engine/output.ts`) is a mature formatting library — ANSI colors, severity icons, health scoring with Hebrew states, per-block and per-layer breakdowns, top issues ranking, catalog coverage bar charts. It's all there. Tests don't use any of it. They use Deno's default flat output.

## My Vision

The design I produced proposes three runners that extend the `_runner.ts` pattern to non-lint commands:

1. **`_transform_runner.ts`** — Table-driven transform test generation. Each case specifies expected rules to fire, codes to fix, and optional idempotency verification. The runner generates dry-run, wet-run, re-lint, and idempotency tests from a single row.

2. **`_create_runner.ts`** — Round-trip test generation. Create a file → lint it → verify specified layers pass. The matrix approach means every format x subtype combination is systematically covered.

3. **`_transpile_runner.ts`** — Source fixture → transpile → verify target structure + lint. This is where the real gap is — zero end-to-end transpile verification exists today.

For QoL, the key insight is: **the test output should use the same formatting infrastructure as the tool itself.** The `COLORS`, `severityIcon()`, `healthColor()` functions already exist in `output.ts`. The scorecard script (`scripts/test-report.ts`) should produce the same quality of visual output that `cws-struct lint` does. If the linter's output is satisfying to read, the test output should be too.

## The Key Insight: Fun = Feedback Loop Satisfaction

Seanje wants tests to be fun. Here's what that actually means from a testing perspective:

A game's loading screen isn't fun because it has animations. It's fun because it's *useful* — it teaches you something while you wait. A game's score screen isn't satisfying because it has particle effects. It's satisfying because you can see your progress against a clear standard.

Apply this to tests:
- **Progress awareness.** You know which layer is running, how many tests remain, how the cache is performing.
- **Victory conditions.** The final scorecard shows pass/fail by layer, by handler, by command. Green across the board *feels* like winning because it *is* winning.
- **Failure diagnostics.** Already strong in `_runner.ts` — extend to other runners.
- **Gap visibility.** The witness grid makes untested code impossible to ignore. No hiding.

The bar charts in the scorecard aren't decoration. They're the same visual language the linter uses for catalog coverage. Consistency across tool output and test output means the same mental model works everywhere. That's not fun for fun's sake — it's coherent design.

## What Surprised Me

The existing transform tests in `integration/go/transform_test.ts` already follow the right pattern — they just aren't generated from data. Each test manually calls `go.transform!()` with specific options, asserts specific rules fire, copies to temp for wet-run, and re-lints. The code is clean. But it's repeated. The dry-run detection tests and wet-run tests have identical structure with different fixtures and assertions.

This is exactly what a data-driven runner eliminates. The test becomes a row. The runner does the rest.

## What's Left Open

1. Whether fixture caching should be shared across lint and transform runners (I recommend yes).
2. Whether test profiles should be TypeScript or TOML (I recommend TypeScript for computed profiles).
3. How to integrate the scorecard into CI without adding infrastructure burden.
4. Idempotency definition — byte-for-byte content identity vs. lint-result identity (I recommend both).

## Closing

1448 tests, 0 failures. That's a strong baseline. The architecture is sound. What's needed isn't a rewrite — it's extending the same excellent patterns to cover the commands that currently have minimal or no testing. The data-driven approach scales. The witness system scales. The output infrastructure exists and is waiting to be used.

The design document is at:
`/media/seanje-lenox-wise/Project/Bereshit/word/claude/plans/test-infrastructure-qol-design.md`

---

> *"Prove all things; hold fast that which is good."* — 1 Thessalonians 5:21

Testing is love. Finding a bug before production is a gift. Building infrastructure that makes tests systematic is a gift that keeps giving.

**Nathan Emet**
