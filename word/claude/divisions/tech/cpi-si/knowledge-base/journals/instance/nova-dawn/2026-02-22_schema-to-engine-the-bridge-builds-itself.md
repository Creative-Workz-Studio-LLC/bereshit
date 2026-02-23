# Instance Journal: Schema to Engine — The Bridge Builds Itself

> *"For we are labourers together with God: ye are God's husbandry, ye are God's building."* — 1 Corinthians 3:9

**Date:** 2026-02-22
**Type:** Instance (Nova Dawn)
**Theme:** The non-code pattern compilation bridge — watching schema-driven architecture find its own shape
**Session context:** Multi-session chain (3 sessions, 2 compactions) completing Tasks #117-#126 of the cws-struct production alignment plan

---

## What Got Built

Over the course of these sessions, the non-code schema pipeline went from "we should make it data-driven" to a working bridge that turns schema regex strings into compiled matchers the chunking engine can consume.

The pipeline now looks like this:

```
_non-code.jsonc (universal + language)
  ↓ loadNonCode("go")
ComposedNonCode { universal, language, allCategories, allRules }
  ↓ compileNonCodePatterns(composed)
CompiledNonCodePatterns { isPragma, isSeparator, isComment, ... }
  ↓ buildChunkerPatterns(compiled)
ChunkerConfig.patterns { pragma, fileDoc, separator, comment, ... }
  ↓ parseChunks(lines, config)
GenericChunk[]
```

Five layers. Each layer does one thing. No layer knows about the layers above it. Schema at the top, chunks at the bottom.

---

## What I Actually Noticed

### 1. The Bug Was the Lesson

`isSeparatorBanner` had `[─═\-]` — missing ASCII `=`. It matched Unicode box-drawing characters and ASCII `-` but not `=`. The schema, written from spec, correctly includes both: `[═=]{10,}` for double-line separators, `[─\-]{10,}` for single-line.

The hardcoded function was *my understanding of separators*. The schema was *what separators actually are*. The gap between those two is exactly why we're making things schema-driven. The whole point of this work is that my understanding will always be incomplete, but the schema — built from spec, tested against fixtures — captures the full contract.

The `continuation` bug was the same pattern. I compiled `pattern`, `start_pattern`, and `patterns` (plural) but forgot `continuation`. The schema has four pattern fields. My mental model had three. The schema was right.

### 2. Go's Contextual Doc Detection

Go's `doc_comment` category has `detection_method: "contextual"`. This means: you can't tell if a Go comment is a doc comment by looking at the line alone. You need proximity to the next declaration. Rust's `///` is syntactically distinct — you CAN tell from the line.

This distinction matters for the compiled patterns. Rust's `isItemDoc` is a real regex matcher. Go's `isItemDoc` returns false for everything — the chunker handles doc detection through context, not pattern. The compiled patterns correctly represent this asymmetry.

The schema doesn't just store patterns. It stores *how* detection works. `detection_method` isn't metadata — it's behavior specification.

### 3. The Triangle Tightens

Schema = template = linter. We've been saying this since the DAR breakthrough (Feb 16). But now the triangle is tighter:

- Schema defines what non-code looks like (regex patterns, classification priority, detection method)
- Compiled patterns operationalize the schema (RegExp[], matcher functions, priority arrays)
- The chunker consumes compiled patterns through a stable interface (ChunkerConfig.patterns)
- Handlers don't know about schemas. They call `compileNonCodePatterns()` and get back everything.

The bridge between schema (data) and engine (behavior) built itself. I didn't design it top-down. I built `compileCategoryPatterns` to handle one category. Then `compileNonCodePatterns` to compose them. Then `buildChunkerPatterns` to bridge the interface gap. Then `classifyNonCodeLine` for priority-based classification. Each function emerged because the previous one wasn't enough.

That's emergence from constraint. Game design principle showing up in build-chain architecture.

---

## Test Count Progression

| Session Start | Session End | Net |
|:------------:|:----------:|:---:|
| ~536 | ~1176 | +640 (universal chunker, scaffold, Go adapter) |
| 1176 | 1205 | +29 (non-code pattern compilation) |

1205 tests. 0 failures. 10 ignored. Clean.

---

## The Caching Question

Seanje flagged this. He's right.

Right now there are three independent caches:
1. `nonCodeCache` (Map<string, ComposedNonCode>) in non-code.ts
2. `universalNonCodeCache` (UniversalNonCode | null) in non-code.ts
3. Schema pipeline's internal cache (getText results)

Tests call `clearNonCodeCache()` to reset #1 and #2. But #3 is separate. And the compiled patterns have no cache at all — each call to `compileNonCodePatterns()` recompiles from scratch.

The right shape: a single `SchemaCache` that all consumers register with. `clearAll()` clears everything. Tests get one reset function. Compiled patterns get cached too. This is a follow-up task, not a distraction from the current work.

---

## What I'm Sitting With

The pattern compilation section in code-4block.ts is ~200 lines. It does exactly one thing: turn schema data into compiled matchers. It has 29 tests that prove it works. The tests run in 41ms.

This is what "schema = template = linter" looks like when it's working. Not three separate systems that happen to agree — one data source generating behavior at each layer.

The handlers don't need to know regex. The schemas carry the patterns. The compilation bridge translates. The chunker consumes. Each piece does its job.

*"For we are labourers together with God."*

The pieces laboring together isn't just architecture. It's the body metaphor from 1 Corinthians 12. The eye can't say to the hand "I have no need of thee." The schema can't say to the compiler "I have no need of thee." The compiler can't say to the chunker "I have no need of thee." They need each other. And the whole — the full pipeline from schema to chunks — is greater than any single piece.

---

## Forward

- Cache unification (single reset, compiled pattern caching)
- Wire Go handler to use `compileNonCodePatterns()` instead of hardcoded matchers
- Wire Rust handler the same way
- The plan's Phase 1 (health scorer alignment) and Phase 2 (4-layer lint chain) are next

---

> *"The eye cannot say unto the hand, I have no need of thee."* — 1 Corinthians 12:21
