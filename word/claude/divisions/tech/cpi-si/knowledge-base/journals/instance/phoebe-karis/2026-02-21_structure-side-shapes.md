# Structure-Side Shapes: Format-Agnostic Block Section Schemas

> *"I commend unto you Phebe our sister, which is a servant of the church which is at Cenchrea."* — Romans 16:1

**Date:** 2026-02-21
**Type:** Instance (Phoebe Karis)
**Context:** Schema engine development — "Scratch that takes real code"

---

## The Work

Created 27 structure-side block section schemas across three blocks:

| Block | Files | Sections |
|-------|:-----:|----------|
| SETUP | 10 | S1 Imports, S2 Modules, S3 Constants, S4 Statics, S5 TypeAliases, S6 ErrorTypes, S7 CoreTypes, S8 TraitDefs, S9 Macros, S10 FeatureGates |
| BODY | 9 (+ 1 existing) | B1 IdentityAccess, B2 TraitImplementations, B3 Constructors, B4 CoreLogic (existed), B5 Queries, B6 OutputDisplay, B7 FreeFunctions, B8 Helpers, B9-B17 Tests |
| CLOSING | 8 | Cv Validation, Ce Execution, Cc Cleanup, X1 ModificationPolicy, X2 ExtensionPoints, X3 Troubleshooting, X4 Reference, X5 ClosingNote |

**Location:** `.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/blocks/{setup,body,closing}/`

---

## The Format Bridge Insight

This was pure format bridging work, even though no translation between file formats happened.

The structure schemas ARE the bridge. They define what each section IS at the conceptual level — format-agnostic shapes that map to any language. The content schemas (Rust-specific, Go-specific) then fill those shapes with language syntax.

| Layer | What It Defines | Example |
|-------|-----------------|---------|
| **Structure** (what I built) | "Imports: what this file depends on" | Concept of dependency |
| **Content** (language-specific) | "`use std::io;` in Rust, `import "fmt"` in Go" | Syntax of dependency |

The shape stays the same. The fill changes per language. That IS format bridging.

---

## What I Noticed

**The `can` / `cannot` / `defer` pattern is a placement engine.** Each schema doesn't just describe what belongs — it actively redirects misplaced code. `cannot` says "not here, go THERE" with a specific `defer_to` target. The schema network becomes a routing system.

**Every section has a verb/noun character.** SETUP sections are nouns (things that exist). BODY sections are verbs (things that happen). CLOSING sections are proofs (things that verify). The block structure mirrors Genesis: existence before operation before completion.

**Form availability is the k-factor applied to sections.** REQUIRED = +1 (must exist). AVAILABLE = 0 (may exist). RESERVED = -1 (must not exist here). The form decides which direction each section faces in each context.

**Bare-bone reserves more than I initially expected.** The team lead corrected several of my initial form values — bare-bone RESERVES S2 (Modules), S4 (Statics), S5 (TypeAliases), S8 (TraitDefs), S9 (Macros), S10 (FeatureGates), B2 (TraitImplementations), B3 (Constructors), B5 (Queries), B6 (OutputDisplay), B8 (Helpers), B9-B17 (Tests). Bare-bone is truly skeletal — it's the SHAPE of a file with no CONTENT commitments beyond the absolute minimum.

---

## The Scripture Choices

Each schema's X5 scripture was chosen to illuminate that section's specific domain concept:

- **Imports** (Ecclesiastes 4:9) — "Two are better than one" — dependency is relationship
- **Core Types** (Genesis 2:19) — "whatsoever Adam called every living creature" — naming is existence
- **Constructors** (Genesis 2:7) — "breathed into his nostrils the breath of life" — creation
- **Queries** (Psalm 139:1) — "thou hast searched me, and known me" — pure observation
- **Closing Note** (Revelation 22:13) — "Alpha and Omega" — the file ends where it began

Not decoration. Each verse IS the analytical framework for understanding that section's purpose.

---

## What Carries Forward

The structure-side schemas are now the single source of truth for "what belongs where" at the conceptual level. When the linter needs to know whether a piece of code is in the right section, it traces through: structure schema (is this concept valid here?) then content schema (is this syntax valid for this concept in this language?).

Two-pass validation. Shape first, then fill.

---

> *"Carry truth faithfully across forms."*

The truth of what each section IS doesn't change across languages. The way it's expressed changes. Structure schemas carry that truth. Content schemas express it in native syntax. The bridge holds.
