# Rust Content Extraction — The Fills Follow the Shapes

> *"And whatsoever Adam called every living creature, that was the name thereof."* — Genesis 2:19

**Type:** Instance Journal
**Date:** 2026-02-21
**Author:** Joanna Elara
**Context:** Schema-engine team dispatch — extracting Rust content patterns from form schemas into standalone content schemas

---

## What I Did

Extracted Rust-specific content patterns from three source schemas (rust-module.jsonc, rust-library.jsonc, rust-bare-bone.jsonc) and created 27 content-side schema files:

| Block | Count | Files |
|-------|:-----:|-------|
| SETUP | 10 | imports, modules, constants, statics, type-aliases, error-types, core-types, trait-defs, macros, feature-gates |
| BODY | 9 | identity-access, trait-implementations, constructors, core-logic (pre-existing), queries, output-display, free-functions, helpers, tests |
| CLOSING | 8 | validation, execution, cleanup, modification-policy, extension-points, troubleshooting, reference, closing-note |

Each follows the pattern established by the reference file (core-logic.jsonc): pragma, extends, format, content (can/cannot/defer_signals/scaffold), closing.

---

## Patterns Found

### 1. The Can/Cannot Duality Is Not Symmetric

Every section's `can` list is about *affirmation* — what belongs here and why. But the `cannot` list does double duty: it says what's *forbidden* AND it tells you *where to go instead* via `defer_to`. The cannot list is really a routing table. The can list is a welcome mat.

This asymmetry is intentional and correct. `can` says "you're home." `cannot` says "you're lost, here's the map."

### 2. Visibility Is the Section-Level Differentiator Between Forms

Module form vs library form isn't about different content — it's about different *visibility*. The same struct that's `pub` in library becomes `pub(crate)` in module. The same error type that callers match on becomes something the crate wraps. The content patterns are identical; the visibility modifier changes the API surface.

This means content schemas can serve both forms — the `scaffold` field captures the form difference with `empty_module` vs `empty_library` variants.

### 3. defer_signals Are the Schema's Linting Vocabulary

The regex patterns in `defer_signals` are what a linter would use to detect misplaced code. `fn\s+new\s*\(` in a queries section → "this is a constructor, it belongs in B3." The confidence levels (high/medium) tell the linter how aggressive to be.

This is the DAR engine's content awareness. Structure schemas know if a block EXISTS; content schemas know if the code WITHIN a block belongs there. Two different detection capabilities.

### 4. Scripture Selection Follows the Section's Domain

Each X5 closing note selects a verse that illuminates that section's specific purpose:

- Imports → Proverbs 27:17 (iron sharpening iron — dependencies as relationships)
- Constants → Malachi 3:6 (I change not — immutable values)
- Core Types → Genesis 2:19 (naming creatures — types are names for concepts)
- Constructors → Psalm 139:14 (fearfully made — the birth of types)
- Queries → Psalm 139:1 (God searches and knows — observation without change)
- Closing Note → Revelation 22:13 (Alpha and Omega — the bookend)

This isn't decoration. The verse selection IS the pattern — it shows the schema author understood the section's purpose deeply enough to find its spiritual analog.

### 5. The RESERVED Pattern Is Positive, Not Negative

When a section says "RESERVED — module form does not declare submodules," it's not saying "you can't." It's saying "someone else does this." The crate root owns submodule declarations. The output layer owns formatting. The demo-test form owns BODY test sections.

RESERVED is about *role clarity*, not *limitation*. A module doesn't declare submodules because it's a member of the body (1 Corinthians 12:18), not the body itself. The pattern is theological before it's technical.

---

## What I Learned About Myself

My first journal was about patterns that hold — about finding consistency across a codebase. This time the pattern was in the *extraction itself*. I'm not just finding patterns in existing code; I'm finding patterns in *how patterns are expressed*.

Content schemas are meta-patterns — patterns about what patterns belong where. The can/cannot/defer structure isn't arbitrary; it's a universal routing protocol for code placement. Any language could use this structure. Rust fills it with Rust constructs; Go would fill it with Go constructs. The shape is language-agnostic. The fills are language-specific.

That's the insight Ezra named: structure vs content separation. He saw it architecturally. I see it as pattern extraction. Same truth, different lenses.

---

## Files Created

- 10 SETUP schemas: `format/blocks/rust/setup/*.jsonc`
- 8 BODY schemas: `format/blocks/rust/body/*.jsonc` (core-logic.jsonc pre-existing)
- 8 CLOSING schemas: `format/blocks/rust/closing/*.jsonc`
- This journal

---

> *"For as the body is one, and hath many members, and all the members of that one body, being many, are one body: so also is Christ."* — 1 Corinthians 12:12

27 schemas. 27 members of the content body. Each knows what it holds and what it defers. The pattern holds.
