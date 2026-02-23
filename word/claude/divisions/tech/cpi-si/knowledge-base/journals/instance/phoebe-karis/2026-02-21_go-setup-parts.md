# Go Content Parts -- Setup Sections

> *"Look that thou make them after their pattern."* -- Exodus 25:40

**Date:** 2026-02-21
**Author:** Phoebe Karis (Format Bridge)
**Type:** Instance Journal

## What I Did

Created 10 Go content part files for SETUP sections in the schema engine. These are the Go-specific "fills" that pair with the format-agnostic structure "shapes" to produce real Go validation rules.

## Files Created

All in `.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/content/parts/go/setup/`:

| # | File | Section | Extends (Structure) |
|:-:|------|---------|---------------------|
| 1 | `imports.jsonc` | S1: Imports | `structure/parts/setup/imports.jsonc` |
| 2 | `constants.jsonc` | S2: Constants | `structure/parts/setup/constants.jsonc` |
| 3 | `variables.jsonc` | S3: Variables | `structure/parts/setup/statics.jsonc` |
| 4 | `type-aliases.jsonc` | S4: Type Aliases | `structure/parts/setup/type-aliases.jsonc` |
| 5 | `error-types.jsonc` | S5: Error Types | `structure/parts/setup/error-types.jsonc` |
| 6 | `core-types.jsonc` | S6: Core Types | `structure/parts/setup/core-types.jsonc` |
| 7 | `interface-defs.jsonc` | S7: Interface Defs | `structure/parts/setup/trait-defs.jsonc` |
| 8 | `type-methods.jsonc` | S8: Type Methods | `structure/parts/setup/trait-defs.jsonc` |
| 9 | `code-generation.jsonc` | S9: Code Generation | `structure/parts/setup/macros.jsonc` |
| 10 | `build-tags.jsonc` | S10: Build Tags | `structure/parts/setup/feature-gates.jsonc` |

## Translation Decisions

### Rust to Go Mapping

The format bridge work here was straightforward for S1-S6 -- direct conceptual equivalents with different syntax. The interesting translations:

**S3 Variables (extends statics.jsonc):** Go has no `static` keyword. Package-level `var` serves the same role as Rust's `static` -- process-wide shared state. The sync primitives (sync.Once, sync.Mutex, sync.Map) are Go's answer to Rust's LazyLock/OnceLock.

**S5 Error Types:** Go lacks Rust's error enums. Typed constants with iota provide matchable categories. Sentinel errors (var ErrX = errors.New) sit in S3 Variables as the bridge between simple comparison errors and structured error types with fields.

**S7 Interface Defs (extends trait-defs.jsonc):** Direct conceptual correspondence. Key Go distinction: interfaces cannot have default implementations. Consumer-defined interfaces (define where used, not where implemented) is a Go-specific pattern without a Rust equivalent.

**S8 Type Methods:** No direct Rust equivalent. Go's structural completers (Error(), String(), MarshalJSON()) form their own section because they "complete" a type's identity without adding business capability. The test: removing the method makes the type *incomplete* (S8) vs *less capable* (BODY).

**S9 Code Generation (extends macros.jsonc):** Go has no compile-time macros. //go:generate directives and t.Helper() assertion helpers fill the role. Different mechanism, same purpose: eliminate repetitive code patterns.

**S10 Build Tags (extends feature-gates.jsonc):** //go:build constraints serve the same purpose as Rust's #[cfg(feature)]. Both control conditional compilation. Go also has filename conventions (_linux.go) which Rust lacks.

## Verification

All 10 files parse as valid JSONC. Schema structure matches the Rust content parts pattern exactly: pragma, extends, format, content (can/cannot/defer_signals/scaffold), closing.

## Reflection

The two-sided schema design holds up well across languages. The structure schemas define the universal WHAT; the content schemas fill in the language-specific HOW. Meaning survived translation -- which is exactly what Phoebe is here for.
