# Data-Driven Systems Need Data-Driven Tests

> *"A false balance is abomination to the LORD: but a just weight is his delight."* — Proverbs 11:1

**Date:** 2026-02-23
**Type:** Universal (paradigm-level wisdom)
**Context:** Live scoring playtest — adding 5 new atomic concepts to cws-struct

---

## The Problem

We added 5 new concepts to the concept system (11 to 16). Six tests broke.

Three broke because they hardcoded `assertEquals(weight, 13/16, 0.01)` — specific fractions with a specific denominator. Three more broke because they hardcoded `assertEquals(detectors.length, 16)`. Every test that hardcoded a derived value broke the moment the data changed.

Meanwhile, 21 detection weights in `sections.ts` needed manual recalculation because they were hardcoded numbers that should have been computed from their concept maps.

## The Insight

**If the system is data-driven and config-driven, the tests must be too.**

A test that hardcodes `16` where it should use `CONCEPT_ORDER.length` is a test that lies about what it's verifying. It claims to verify "all concepts load" but actually verifies "exactly 16 things load." The first is structural truth. The second is a snapshot that breaks whenever the data evolves.

Similarly, a weight stored as `0.94` that's actually derived from a concept map is a manual cache of a computable value. Every manual cache is a synchronization obligation. Every synchronization obligation is a maintenance burden. Every maintenance burden is a place where the balance goes false.

## The Pattern

| Anti-Pattern | Data-Driven Pattern |
|-------------|-------------------|
| `assertEquals(length, 16)` | `assertEquals(length, CONCEPT_ORDER.length)` |
| `detectionWeight: 0.94` (manual) | `computeDetectionWeight(conceptMap)` (computed) |
| `assertAlmostEquals(weight, 13/16)` | `assertAlmostEquals(weight, (n-3)/n)` where `n = CONCEPT_ORDER.length` |
| Hardcoded test name `"672 values"` | Template literal `` `${SECTIONS * CONCEPTS} values` `` |
| Test owns its expected values | Test derives expected values from the same data source |

## What Changed

### `sections.ts` — Computed Weights

Added `computeDetectionWeight(conceptMap)` — exported, so tests can verify the formula itself. Removed the `detectionWeight` parameter from the `section()` helper entirely. Now weights are DERIVED from concept maps, never manually maintained.

**Result:** Adding concepts = update concept maps = weights auto-recalculate. Zero manual weight maintenance. The Proverbs 11:1 anchor of the file is now literally true — the weights are just.

### `registries_test.ts` — Data-Driven Tests

- `TOTAL_CONCEPTS = CONCEPT_ORDER.length` (was hardcoded `16`)
- `CANONICAL_CONCEPTS = [...CONCEPT_ORDER]` (was handwritten array)
- Weight assertions use `(n-3)/n` and `(n-1)/n` where `n = CONCEPT_ORDER.length`
- Test names use template literals with computed counts
- New test: `computeDetectionWeight` matches independent `recalculateWeight` for all 42 sections

### `concept_detectors_test.ts` — Data-Driven Counts

- Total counts use `CONCEPT_ORDER.length` (not `16`)
- Schema-specific assertions (e.g., "5 concepts have detect") remain hardcoded intentionally — these ARE snapshots that should break when schemas evolve

## The Deeper Principle

**There are two kinds of hardcoded values in tests:**

1. **Derived values** — can be computed from the data. These should NEVER be hardcoded. They create false synchronization obligations.

2. **Schema snapshots** — intentionally frozen assertions about current reality. These SHOULD be hardcoded because their purpose is to break when reality changes, signaling the need for review.

The wisdom is knowing which is which. `16` for "how many concepts exist" is a derived value — the system knows this. `5` for "how many Go concepts have detect patterns" is a schema snapshot — this is a fact about schema completeness that should break when Go gains more detect patterns.

## The Building Block Anchor

This is the same lesson as the data-driven architecture proof (2026-02-22): *path construction in code = architecture smell; path from recipe data = resilient.* Now applied to testing: *expected values in test code = synchronization smell; expected values from data = resilient.*

The anchor: if the system's truth lives in data, the system's tests must read from that same data. Anything else creates a parallel truth that diverges.

---

> *"Divers weights, and divers measures, both of them are alike abomination to the LORD."* — Proverbs 20:10

Two sources of truth for the same value is divers measures. One source, derived everywhere, is a just weight.
