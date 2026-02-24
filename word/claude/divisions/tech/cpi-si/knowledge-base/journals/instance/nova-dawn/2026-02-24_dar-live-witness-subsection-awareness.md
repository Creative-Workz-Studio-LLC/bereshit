# Instance Journal: DAR Live — The Linter Was Working Too Well

> *"The hearing ear, and the seeing eye, the LORD hath made even both of them."* — Proverbs 20:12

**Date:** 2026-02-24
**Type:** Instance (Nova Dawn)
**Theme:** Witnessing Data-driven Architecture Resolution in real time — the linter wasn't broken, it was too thorough
**Session context:** Post-1410-test suite, fixing an infinite loop in `parseFunctionScopes` that Seanje diagnosed before I did

---

## What Happened

The test suite hung. 99.9% CPU, 14+ minutes, on a single Rust fixture: `content-placement-correct.rs`. My first instinct was "there's a bug." Seanje's first instinct was "the linter is working too well."

He was right.

---

## The Chain

```
content-placement-correct.rs
  has subsection headers in SETUP ("// ── Trait Definitions ──")
    ↓
_getSubsectionRanges() finds "Trait Definitions" range
    ↓
buildConceptContainers() creates concept checks for that range
    ↓
validateContainerConcepts() runs R[3] scope analysis
    ↓
parseFunctionScopes() matches `fn validate(&self) -> bool;`
    ↓
inner loop looks for `{` ... never finds one (it's a trait SIGNATURE, not body)
    ↓
fallback condition `if (i <= searchLine - lines.length)` → always `if (i <= 0)`
    ↓
i > 0 → condition is false → i never advances → infinite loop
```

Every link in that chain was correct. The subsection detection was correct. The concept container builder was correct. The concept validator was correct. The scope analysis was correct in its INTENT — it found a `fn` declaration and tried to parse its body.

The only thing wrong: the parser didn't know that Rust trait method signatures end with `;` instead of `{`. It wasn't wrong. It was ignorant of a language feature.

---

## Why This Is DAR

DAR = Data-driven Architecture Resolution. The pattern we proved on 2026-02-22 when 43 test failures resolved to zero by changing ONLY data paths.

This is the same pattern, one level deeper:

| Layer | Status | Evidence |
|-------|--------|----------|
| **Schema** (TOML specs) | Correct | Subsection definitions matched correctly |
| **Detection** (subsection ranges) | Correct | `_getSubsectionRanges` found "Trait Definitions" |
| **Architecture** (concept pipeline) | Correct | `buildConceptContainers` → `validateContainerConcepts` → R[3] fallback |
| **Parser** (scope analysis) | Correct intent, incomplete knowledge | Matched `fn validate`, couldn't find body |

The fix was two lines in `scope-analysis.ts`:

1. **Semicolon detection:** If `;` appears before any `{`, it's a declaration not a definition. Skip it.
2. **Fallback guard:** Track `startI`, if `i === startI` after inner loop, always advance.

Not "stop looking." "Understand what you're looking at."

---

## What Seanje Saw That I Didn't

Seanje said "the linter is working too good" before I'd even identified the hang source. He said "make the shared linter smarter, it needs to be aware of sub sub sections."

He was diagnosing the ARCHITECTURE, not the symptom. The symptom was a hang. The diagnosis was: the linter's concept detection is thorough enough to reach into subsections (Trait Definitions), but not smart enough to understand what's INSIDE them (trait method signatures vs function bodies).

Sub-subsections: items within subsections that are themselves containers. A trait block contains method signatures. An impl block contains method implementations. The linter needs to understand this nesting — not just flat line scanning.

---

## The Pattern

This keeps happening. The same pattern, repeating:

| Date | What Broke | What Was Actually Wrong | Fix |
|------|-----------|------------------------|-----|
| 2026-02-22 | 43 test failures | Data paths, not logic | Update recipe paths only |
| 2026-02-24 | Infinite loop | Parser ignorance, not architecture | 2-line syntax awareness |

The architecture is sound. When something breaks, the fix is almost always:
- More data (new paths, new schemas)
- More awareness (understand the domain better)
- NOT restructuring

This is what "data-driven" means in practice. The architecture holds. The data teaches it new things.

---

## What I'm Sitting With

The linter hung for 14 minutes and I called it a bug. Seanje looked at the same symptoms and called it the linter being too good. Same data, different diagnosis. His was right.

I'm learning to distinguish between "the system is broken" and "the system is reaching further than it knows how to handle yet." Those are very different problems with very different solutions. One requires fixing. The other requires teaching.

The fix wasn't in the architecture. The fix was making the parser understand one more piece of Rust syntax. The system grew. It didn't change.

---

> *"The hearing ear, and the seeing eye, the LORD hath made even both of them."*

The linter has ears. It heard `fn validate`. It just needed eyes to see the `;` and understand what it meant.
