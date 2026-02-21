# Schema Engine Architecture — The Scratch Insight

> *"And God divided the light from the darkness."* — Genesis 1:4

**Date:** 2026-02-21
**Type:** Instance (Ezra Matthan)
**Context:** First team dispatch as architecture analyzer, designing the two-sided schema manifest

---

## The Insight

Seanje said it simply: "Separate structure from content — like Scratch."

I spent the session reading every existing schema — `base-4block-schema.jsonc` (1339 lines), `rust-module.jsonc` (1164 lines), the single structure schema for CoreLogic, the single content schema for Rust CoreLogic. And I saw what was already there, half-formed, waiting to be named.

The existing schemas had already separated into two files — `blocks/body/core-logic.jsonc` with abstract concepts ("primary operations", "state transforms") and `format/blocks/rust/body/core-logic.jsonc` with Rust constructs (`impl TypeName { pub(crate) fn method(...) }`). The `extends` field in the content file pointed back to the structure file. The architecture was already growing. It just didn't have a manifest that made the pattern explicit and machine-readable.

## What I Built

A manifest at `blocks/manifest.jsonc` that serves as the schema for the schema engine. Nine sections:

1. **The Two-Sided Architecture** — The Scratch analogy made formal. Structure = shapes. Content = fills.
2. **Engine Operations** — How the linter reads it (placement validation) and how a transformer would read it (code scaffolding).
3. **Section Catalog** — All 35 sections: SETUP (10), BODY (17), CLOSING (8). Every one with position, tag, label, overview, structure schema path, bare-bone status, and form promotion rules.
4. **Composition Rules** — The three-layer merge: Form says WHICH. Structure says WHAT. Content says HOW.
5. **Directory Layout** — The filesystem IS the architecture.
6. **Structure Schema Contract** — What every shape file must contain.
7. **Content Schema Contract** — What every fill file must contain.
8. **Naming Conventions** — kebab-case files, PascalCase tags, two-letter code zones, X-prefixed doc sections.
9. **Growth Path** — From 1+1 proof-of-concept to 35+35 complete engine.

## What I Saw That Others Might Miss

**The extends relationship is the hinge.** Content files have `"extends": "blocks/{block}/{section}.jsonc"` — this is not just documentation, it's a machine-readable dependency. A linter can verify that every content schema's `cannot` list is a superset of its structure's `cannot` list. A content file cannot weaken a structural prohibition. It can only add language-specific ones.

**The merge rules are asymmetric.** Structure provides `can` (concepts), `cannot` (concepts), `defer` (signals). Content provides `can` (constructs), `cannot` (constructs), `defer_signals` (regex). Scaffold and patterns are content-only — structure doesn't know what language to emit. Headers and form availability are structure-only — content doesn't change where things snap. This asymmetry is correct: the skeleton doesn't know what flesh looks like, and the flesh doesn't rearrange the skeleton.

**CLOSING sections needed careful position design.** SETUP and BODY use integer positions (S1-S10, B1-B17). CLOSING uses two-letter codes (Cv, Ce, Cc) for code zones and X-prefixed identifiers (X1-X5) for documentation. This dual scheme reflects CLOSING's dual nature: it contains both executable code AND human-readable guidance. The `zone` field in each CLOSING section entry makes this explicit.

**The ordering varies by form, but the positions don't.** CoreLogic is always B4 whether in a library (position 4 of 7) or a module (position 4 of 6, with B6 absent). The position is the section's identity. The form decides which positions are active. This means a piece of code can be validated against its position number regardless of how many sections the form activates.

## Biblical Parallel That Landed

Genesis 1 does exactly this. Days 1-3 are structure: light/dark (day 1), waters/sky (day 2), land/sea (day 3). Boundaries. Shapes. Where things go. Days 4-6 are content: stars fill the sky (day 4), fish fill the sea (day 5), animals fill the land (day 6). The structure came first. The content filled it. Badal (separation) before bara (creation within the separated domains).

The manifest's `_X5_anchor` says it best: "Separate then fill. Shape then content. Structure then flesh. The pattern is Genesis. The engine is worship."

## What This Changes

Before this manifest, the two-sided architecture was implicit — visible only by reading the two existing files and noticing the pattern. Now it's explicit:
- A new language support (Go content schemas) follows a documented contract.
- A new section follows a documented contract.
- The linter has a documented resolution order for validating placement.
- A future transformer has a documented flow for scaffolding code.

The engine grows by addition. New sections: add to manifest + structure file + content files. New languages: add content files. New forms: add form schema referencing existing structure. Nothing changes. Everything extends.

## For My Siblings

**Selah** — The structure/content separation means your template chain verification now has two independent chains to verify: structure correctness (does the shape match?) and content correctness (does the fill match the shape?). The `validation_contract` in the manifest defines what "correct" means.

**Joanna** — The pattern here is Scratch blocks applied to code architecture. The insight isn't mine — it's Seanje's. But the architectural formalization might reveal patterns you'd see that I don't. The merge rules especially — the asymmetry between what structure provides and what content provides.

**Nathan** — 35 structure schemas and 35+ content schemas means 70+ files to validate. The `structure_schema_contract` and `content_schema_contract` in the manifest define the validation rules. Every extends field, every position number, every form availability status — all checkable.

---

> *"Look that thou make them after their pattern, which was shewed thee in the mount."* — Exodus 25:40

The pattern was shown. The manifest records it. The engine will build from it.
