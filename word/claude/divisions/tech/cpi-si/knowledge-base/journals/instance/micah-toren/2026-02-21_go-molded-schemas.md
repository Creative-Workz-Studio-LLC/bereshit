# Go Content Molded Schemas — Content Review Journal

> *"He hath shewed thee, O man, what is good."* — Micah 6:8

**Date:** 2026-02-21
**Type:** Instance
**Author:** Micah Toren

## What I Created

Three Go content molded schemas at `content/molded/go/`:

1. **library.jsonc** — Go library package content expectations
2. **executable.jsonc** — Go executable (package main) content expectations
3. **demo-test.jsonc** — Go test file content expectations

## Go vs Rust: What the Differences Reveal

The Rust schemas have `module`, `library`, `executable`. Go has `library`, `executable`, `demo-test`. No Go module form — Go packages are flat by convention. What Rust achieves through `pub(crate)` modules, Go achieves through unexported names in the same package. The missing form tells you as much as the present ones.

**Identity construct divergence:**
- Rust: `static PRAGMA: &[(&str, &str)]` — immutable, compile-time, zero-allocation slices
- Go: `var Pragma = [][2]string{...}` — mutable by syntax but immutable by convention

Both are `[][2]string`-equivalent, but Go's `var` vs Rust's `static` reveals the languages' philosophies. Go trusts convention where Rust enforces through the type system. Neither approach is wrong — they're different expressions of the same identity pattern.

**Visibility models:**
- Rust: `pub`, `pub(crate)`, private — three tiers, enforced by compiler
- Go: Exported (Uppercase), unexported (lowercase) — two tiers, enforced by compiler + convention

The Go library schema needed to explicitly call out the Uppercase/lowercase pattern because it's doing more semantic work per symbol than Rust's `pub` keyword. One capital letter means "this is API." That's a lot of weight on a letter case.

**BODY section naming:**
- Rust library B7: `FreeFunctions` (with `register_identity()`)
- Go library B5: `PublicAPIs` (no free functions — everything is a package function or method)
- Go demo-test B5: `TestFunctions` — unique to the test form, replacing PublicAPIs

The section names are honest about what each form actually DOES. A Go library's final section is its public surface. A test file's final section is its test cases. The names carry meaning.

**Test form differences:**
Go demo-test has no Rust equivalent in the content molded schemas. I had to construct it from the monolithic schema's demo_test content — S9 maps to assertion helpers with `t.Helper()`, Ce maps to `TestMain()`, and the BODY ordering is `concern_coverage` not `execution_flow`. Tests prove by concern, not by lifecycle.

## What I Verified

Cross-checked every BODY section name, every ordering principle, every SETUP emphasis, and every CLOSING pattern against the Go monolithic schema (`go-4block-schema.jsonc`). Specific verification points:

- Library BODY: execution_flow, 5 sections (B1-B5) matching lines 580-624
- Executable BODY: execution_flow, 5 sections matching lines 628-674
- Demo-test BODY: concern_coverage, 5 sections matching lines 678-723
- SETUP emphasis per form matches monolithic `subtype_emphasis` (lines 487-503)
- CLOSING code zones match monolithic zone definitions (lines 776-808)
- All `extends_structure` paths point to verified existing files

## The Gap That Matters

The Rust molded schemas existed for module, library, executable. Go needed library, executable, demo-test. The demo-test form had no Rust analog to copy from — I had to derive it from the monolithic source. That's exactly the kind of gap where content errors hide: when you can't pattern-match from an existing template and have to reason from source truth instead.

The monolithic schema IS the source truth. The molded schemas are the focused extractions. If the extraction drifts from the monolith, the molded schema is wrong — the monolith is authoritative. That's the anchor.
