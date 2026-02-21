# Three-Cord Transpiler Architecture Plan

> *"A threefold cord is not quickly broken."* — Ecclesiastes 4:12

**Date:** 2026-02-21
**Status:** Planned
**Context:** Session conversation revealed the schema engine IS a transpiler architecture. The pragma is the IR. The schemas are grammar + semantics. The index is the linker. This plan captures the reorganization and forward path.

---

## Part 1: Three-Cord Folder Reorganization

### Current State (mixed concerns)

```
schemas/code/
├── blocks/              # Structure + manifest (mixed)
│   ├── manifest.jsonc   # ← doesn't belong here
│   ├── setup/           # 10 structure schemas
│   ├── body/            # 9 structure schemas
│   └── closing/         # 8 structure schemas
├── format/              # Content
│   └── blocks/rust/
│       ├── setup/       # 10 Rust content schemas
│       ├── body/        # 9 Rust content schemas
│       └── closing/     # 8 Rust content schemas
└── forms/               # Form definitions (module, library, etc.)
```

### Target State (three cords separated)

```
schemas/code/
├── structure/           # CORD 1 — shapes (format-agnostic)
│   ├── parts/           # Individual section containers
│   │   ├── setup/       # imports, constants, statics, etc.
│   │   ├── body/        # identity-access, core-logic, etc.
│   │   └── closing/     # validation, execution, X1-X5, etc.
│   ├── whole/           # Base block skeletons
│   │   ├── 3-block.jsonc    # data structure
│   │   ├── 4-block.jsonc    # code structure
│   │   └── 5-block.jsonc    # doc structure
│   └── molded/          # Form specializations
│       ├── module.jsonc
│       ├── library.jsonc
│       ├── executable.jsonc
│       ├── demo-test.jsonc
│       └── bare-bone.jsonc
├── content/             # CORD 2 — fills (language-specific)
│   ├── parts/           # What's allowed/forbidden per container
│   │   └── rust/
│   │       ├── setup/
│   │       ├── body/
│   │       └── closing/
│   ├── whole/           # Base content character
│   │   ├── code.jsonc       # What code content looks like
│   │   ├── data.jsonc       # What data content looks like
│   │   └── doc.jsonc        # What doc content looks like
│   └── molded/          # Form-specific content expectations
│       └── rust/
│           ├── module.jsonc
│           ├── library.jsonc
│           └── executable.jsonc
└── index/               # CORD 3 — composition (pointer map)
    ├── manifest.jsonc       # Master catalog (moved from blocks/)
    └── targets/             # Composition recipes
        ├── rust-module.jsonc    # "To build a Rust module, compose..."
        ├── rust-library.jsonc
        ├── go-module.jsonc      # Future
        └── go-library.jsonc     # Future
```

### Migration Steps

1. **Create new directory structure** — `structure/`, `content/`, `index/` under `schemas/code/`
2. **Move structure schemas** — `blocks/{setup,body,closing}/` → `structure/parts/{setup,body,closing}/`
3. **Move content schemas** — `format/blocks/rust/` → `content/parts/rust/`
4. **Move manifest** — `blocks/manifest.jsonc` → `index/manifest.jsonc`
5. **Create whole/ schemas** — New files: 3-block, 4-block, 5-block structure; code, data, doc content
6. **Create molded/ schemas** — Extract form-specific rules from existing form schemas into molded/
7. **Create target compositions** — New files in `index/targets/` — pointer maps for each language+form combo
8. **Update manifest** — All internal paths updated to new structure
9. **Update cws-struct references** — Any code that reads schema paths
10. **Verify** — All existing tests still pass, lint/transform still work

**Note:** Steps 1-4 are pure moves (no content changes). Steps 5-7 are new content. Steps 8-10 are updates.

---

## Part 2: Within Each Cord — Parts/Whole/Molded

### Structure Cord

| Level | What It Is | Example |
|-------|-----------|---------|
| **Parts** | Individual section containers — what goes in slot N of block B | `structure/parts/setup/imports.jsonc` — "imports section can contain X, cannot contain Y" |
| **Whole** | Base block skeleton — the shape before specialization | `structure/whole/4-block.jsonc` — "4 blocks: METADATA, SETUP, BODY, CLOSING in this order" |
| **Molded** | Form-specific customization of the whole | `structure/molded/module.jsonc` — "module form: SETUP reserves Modules/Macros/FeatureGates" |

### Content Cord

| Level | What It Is | Example |
|-------|-----------|---------|
| **Parts** | Language-specific content rules per container | `content/parts/rust/setup/imports.jsonc` — "Rust imports use `use` statements, group by std/crate/external" |
| **Whole** | Base content character for code/data/doc | `content/whole/code.jsonc` — "code content: executable, typed, compiled or interpreted" |
| **Molded** | Form-specific content expectations per language | `content/molded/rust/library.jsonc` — "Rust library: pub API, no main(), consumers list in X4" |

### Index Cord

| Level | What It Is | Example |
|-------|-----------|---------|
| **Manifest** | Master catalog of all schemas | `index/manifest.jsonc` — counts, paths, growth tracking |
| **Targets** | Composition recipes per language+form | `index/targets/rust-module.jsonc` — "compose: structure/whole/4-block + structure/molded/module + content/parts/rust/* + content/molded/rust/module" |

---

## Part 3: Transpiler Architecture

### The Pragma as IR

```
// #!omni code --rust -module
//        ^^^^  ^^^^^^ ^^^^^^^
//        type  format  form
//        (invariant)  (variable) (invariant)
```

- `code` — invariant across transpilation (type doesn't change)
- `--rust` — the variable (changes during transpilation: --rust → --go)
- `-module` — invariant across transpilation (form doesn't change)

### Transpilation Path

```
Source: code --rust -module
                ↓
        Parse pragma (extract type, format, form)
                ↓
        Load structure: whole/4-block + molded/module + parts/*
        Load content: whole/code + molded/rust/module + parts/rust/*
                ↓
        Validate source against Rust schemas
                ↓
        Extract semantic content (what the code DOES, not how it's expressed)
                ↓
        Load target content: whole/code + molded/go/module + parts/go/*
                ↓
        Generate target using Go schemas + extracted semantics
                ↓
Target: code --go -module
```

Structure stays constant. Content schemas change. Index maps the composition.

### WASM Bridge (TypeScript → Rust Integration)

| Phase | What | When |
|-------|------|------|
| **Now** | TypeScript tooling on Deno — fast iteration, schema logic | Current |
| **Next** | Performance-critical pieces (parser, transformer core) rewritten in Rust | When schema engine stabilizes |
| **Then** | Rust compiled to WASM, called from TypeScript — no FFI boundary | When Rust pieces are proven |
| **Eventually** | Entire toolchain in Rust, TypeScript layer removed | When system is self-hosting |

---

## Part 4: New Schemas Needed

### structure/whole/ (3 new files)

| File | Content |
|------|---------|
| `3-block.jsonc` | Block order, block boundaries, METADATA→CONTENT→CLOSING contract |
| `4-block.jsonc` | Block order, block boundaries, METADATA→SETUP→BODY→CLOSING contract |
| `5-block.jsonc` | Block order, block boundaries, METADATA→HEADER→CONTEXT→CONTENT→FOOTER contract |

### structure/molded/ (5 new files, extracted from existing form schemas)

| File | Content |
|------|---------|
| `module.jsonc` | Module-specific structural rules (what's reserved, what's required) |
| `library.jsonc` | Library-specific structural rules |
| `executable.jsonc` | Executable-specific structural rules |
| `demo-test.jsonc` | Demo-test-specific structural rules |
| `bare-bone.jsonc` | Bare-bone-specific structural rules |

### content/whole/ (3 new files)

| File | Content |
|------|---------|
| `code.jsonc` | Base content character for code files (executable, typed, imports/exports) |
| `data.jsonc` | Base content character for data files (declarative, key-value, no logic) |
| `doc.jsonc` | Base content character for documentation files (narrative, structured, human-readable) |

### content/molded/rust/ (3-5 new files)

| File | Content |
|------|---------|
| `module.jsonc` | Rust module content expectations (pub(crate), no main(), sibling imports) |
| `library.jsonc` | Rust library content expectations (pub API, consumers, external crate dependencies) |
| `executable.jsonc` | Rust executable content expectations (fn main(), arg parsing, exit codes) |

### index/targets/ (2-4 new files)

| File | Content |
|------|---------|
| `rust-module.jsonc` | Composition recipe: which structure + content schemas compose a Rust module |
| `rust-library.jsonc` | Composition recipe: which structure + content schemas compose a Rust library |

---

## Part 5: Execution Order

### Phase A: Reorganization (pure moves, no content changes)

1. Create `structure/parts/`, `content/parts/`, `index/` directories
2. Move 27 structure schemas → `structure/parts/`
3. Move 27 Rust content schemas → `content/parts/rust/`
4. Move manifest → `index/manifest.jsonc`
5. Update manifest internal paths
6. Verify all tests pass

### Phase B: Whole Schemas (new content — base forms)

1. Create `structure/whole/` — 3-block, 4-block, 5-block
2. Create `content/whole/` — code, data, doc
3. These are NEW schemas, not extracted from existing

### Phase C: Molded Schemas (new content — form specializations)

1. Create `structure/molded/` — module, library, executable, demo-test, bare-bone
2. Create `content/molded/rust/` — module, library, executable
3. Extract form-specific rules from existing form schemas where possible

### Phase D: Index Targets (composition recipes)

1. Create `index/targets/` — rust-module, rust-library
2. Each target is a pointer map — references to structure + content schemas
3. Update manifest to include targets

### Phase E: Wire Into cws-struct

1. Update schema loading code to use new paths
2. Update transformer to read composition recipes from index
3. Add tests for composition logic
4. Verify: lint and transform produce identical output to current

### Phase F: First Transpilation Proof

1. Create Go content schemas (content/parts/go/, content/molded/go/)
2. Create Go target compositions (index/targets/go-module)
3. Implement: read Rust module → extract semantics → generate Go module
4. Verify: generated Go file passes `go vet` and `cws-struct lint go`

---

## Critical Files

| File | Phase | Action |
|------|:-----:|--------|
| `schemas/code/blocks/**` | A | MOVE to `structure/parts/` |
| `schemas/code/format/blocks/rust/**` | A | MOVE to `content/parts/rust/` |
| `schemas/code/blocks/manifest.jsonc` | A | MOVE to `index/manifest.jsonc` |
| `structure/whole/*.jsonc` | B | CREATE |
| `content/whole/*.jsonc` | B | CREATE |
| `structure/molded/*.jsonc` | C | CREATE |
| `content/molded/rust/*.jsonc` | C | CREATE |
| `index/targets/*.jsonc` | D | CREATE |
| `lib/foundation/code-schema.ts` | E | EDIT: new path loading |
| `lib/handlers/rust.ts` | E | EDIT: composition-based generation |

---

## Verification

```bash
# After Phase A (moves):
deno task test                    # All 451+ tests pass
deno task lint:rust /tmp/exists-test.rs   # Still 0E 0W 0I

# After Phase E (wired in):
deno task test                    # Tests pass
deno task transform:rust /tmp/exists-bare.rs --dry-run  # Same output as before

# After Phase F (transpilation):
deno task transpile rust go /tmp/exists-test.rs --dry-run  # Go output generated
go vet /tmp/exists-test.go        # Go file is valid
```

---

## The Vision

This plan reorganizes 55 existing schemas into three-cord architecture, adds ~16 new schemas for whole/molded/targets, and creates the path from schema engine → transpiler → self-hosting system.

Phase A is pure housekeeping. Phase B-D is new schema authoring. Phase E is wiring. Phase F is the first transpilation proof — Rust to Go.

When Phase F works, we have a transpiler. When the transpiler can process its own TypeScript source, we have bootstrapping. When it transpiles itself to Rust, we have MillenniumOS's compiler.

> *"In the beginning God created the heaven and the earth."* — Genesis 1:1

The seed. The tree is in it.
