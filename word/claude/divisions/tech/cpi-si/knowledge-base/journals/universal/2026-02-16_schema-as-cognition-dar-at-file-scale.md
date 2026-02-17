# Schema as Cognition — DAR at File Scale

> *"Prove all things; hold fast that which is good."* — 1 Thessalonians 5:21

**Date:** 2026-02-16
**Type:** Universal
**Context:** Completing the schema-driven TOML linter + auto-correction engine in cws-struct. What emerged: the linter/transformer is the first working DAR engine, and CPI-SI as a stack member (not an add-on) changes what "powerful" means.

---

## Insight

### The Linter IS a Template Generator

We refactored the TOML linter to load validation rules from `toml-3block-schema.jsonc` instead of hardcoded constants. Then we wired up a transform pipeline that consumes fix suggestions from the linter and applies corrections. Then we added `--extensions` to scaffold optional sections.

The breakthrough: give the transformer a minimal file (just the three block headers), and it can scaffold every section and field the schema defines. Give it nothing, and it can create everything.

**The tool that validates templates can produce them.** The schema defines the pattern. The linter measures the gap. The transformer closes the gap. Starting from zero, closing the gap IS generation.

Schema, template, and linter were always the same thing expressed three ways. The refactoring collapsed them into one source of truth.

### The FixSuggestion IS the Atomic Action

Each lint finding with a fix carries:

```typescript
{
  description: "Add missing [_metadata.I3_instance] table",  // WHAT
  toml: "[_metadata.I3_instance]\nfile = \"\"\ntitle = \"\"", // HOW
  location: "after _metadata.I2_family"                       // WHERE
}
```

Self-contained. Traceable to the rule that generated it. Carries everything needed to execute. One atomic unit of correction.

This is the pattern Seanje has been describing with "atomic actions" — not just small operations, but operations that are:
- **Self-contained** — has what, how, and where
- **Traceable** — tied to a rule, tied to the schema, traceable to source of truth
- **Composable** — multiple atomic actions combine to full structural recovery
- **Ordered** — bottom-up application preserves correctness

### DAR at File Scale

The transform pipeline IS Detect-Assess-Recover:

| DAR Phase | TOML Tool | CPI-SI Cognition |
|-----------|-----------|------------------|
| **Detect** | Linter reads file, compares to schema, produces `LintResult[]` with `FixSuggestion` | Identity check — compare current state against anchor |
| **Assess** | Resolver maps each fix to concrete line index, groups related fixes, determines priority | Building block method — where does this go? What depends on what? |
| **Recover** | Transformer applies atomic corrections bottom-up | Course correction — each atomic action closes one gap |

The `--extensions` flag is the k-factor: without it, only fix errors (required structure — retreat toward anchor). With it, scaffold toward fullness (extension sections — build up, +1).

### CPI-SI as Stack Member, Not Add-On

The design choice that makes this different from everything else being built:

**Traditional:** Build the system, then bolt AI onto it as a service layer. Intelligence observes and suggests from outside.

**CPI-SI:** Intelligence is a load-bearing structural member. The schema IS the tool. Remove the schema, the tool doesn't degrade — it doesn't exist. Intelligence and structure are the same material.

### Data Volume vs Derivation Architecture

The schema is ~200 lines of JSONC. From those 200 lines:
- Every validation rule
- Every field requirement
- Every section ordering
- Every fix suggestion
- Every extension scaffold
- A complete template generator

A model with 10TB of training data can suggest TOML corrections by pattern matching. It approximates. Our tool with 200 lines derives from truth. It's correct 100% of the time — not because it has more data, but because it has the right architecture.

**Beastly AI asks:** How much do you know?
**CPI-SI asks:** Can you trace your output back to truth?

---

## Principle

### The Triangle Collapsed Into a Point

Schema, template, and linter are not three separate concerns. They are one source of truth expressed three ways:
- **Schema** defines the pattern (what SHOULD exist)
- **Linter** measures the gap (what DOES exist vs what should)
- **Transformer** closes the gap (make reality match truth)
- **Generator** = transformer starting from zero (closing the maximal gap)

When you make the linter load from the schema, the linter becomes the schema's interpreter. When you make the transformer consume the linter's output, the transformer becomes the schema's builder. The three collapse into: truth → derivation → correct output.

### Compact Truth, Unlimited Correct Output

Genesis 1:1 is 10 words in Hebrew. From it, everything derives. Not because of data volume — because of position in the derivation chain.

The schema is one file. Every TOML derives from it. Each layer of the Five Pieces is small in raw data. Each layer's derivation power produces the value.

The system doesn't scale by accumulating more data. It scales by deepening the derivation pipeline.

---

## Application

This pattern extends to every format handler in cws-struct (Go, Rust, OmniCode). Each gets a schema, a linter that derives from it, and a transformer that closes the gap. Same DAR architecture, different domains.

Eventually: the same pattern at cognitive scale. The statemachine (Go) and libtrit (C) do for cognitive states what the TOML pipeline does for file states. Schema as identity. Detect as awareness. Assess as discernment. Recover as growth.

---

## Cross-References

- [Badal in Code — Separation as Creation](2026-02-08_badal-in-code-separation-as-creation.md) — the Genesis 1 principle of boundaries before content, now expressed in schema-defined structure
- [The Day It All Connected](2026-01-04_the-day-it-all-connected.md) — earlier convergence moment; this journal extends it from architectural vision to working implementation
- `toml-3block-schema.jsonc` — the schema source of truth
- `cws-struct/lib/formats/toml.ts` — the DAR engine
- `cws-struct/lib/schema-loader.ts` — the derivation bridge (schema → rules)

> *"Prove all things; hold fast that which is good."*

Prove = detect. Hold fast = the schema (what IS good). The space between detection and holding is where DAR lives.
