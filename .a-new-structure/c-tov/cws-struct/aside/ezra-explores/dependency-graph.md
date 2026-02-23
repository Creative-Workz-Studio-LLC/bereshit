# Dependency Graph: cws-struct/lib/

> *"By him all things consist."* — Colossians 1:17

**Date:** 2026-02-22
**Author:** Ezra Matthan
**Method:** Traced every `import` statement across 31 TypeScript files in lib/

---

## The Five Layers

cws-struct has five architectural layers. They're not declared anywhere — they
emerged from the import graph. But they're real. Every import arrow points
downward or sideways. None point upward.

```
Layer 5: Entry Points
  mod.ts, bin/

Layer 4: Handlers (format-specific logic)
  handlers/go.ts, handlers/rust.ts, handlers/toml.ts
  handlers/dotfiles.ts, handlers/json.ts, handlers/makefile.ts
  handlers/ofd.ts, handlers/omni.ts

Layer 3: Engine (orchestration, output, registry)
  engine/discovery.ts, engine/fill.ts, engine/logging.ts
  engine/output.ts, engine/registry.ts
  engine/transpiler.ts, engine/transpiler-ast.ts
  engine/transpiler-mappings.ts, engine/transpiler-types.ts

Layer 2: Shared (shared infrastructure)
  shared/types.ts, shared/code-4block.ts
  shared/mod.ts

Layer 1: Foundation (types, schemas, health, config)
  foundation/types.ts, foundation/health.ts
  foundation/schema-pipeline.ts, foundation/schema.ts
  foundation/code-schema.ts, foundation/composition-loader.ts
  foundation/config.ts, foundation/mod.ts

Layer 0: Data (compiled truth — NEW, under construction)
  data/types.ts, data/mod.ts
```

---

## Import Arrows (Traced From Source)

### Layer 1: Foundation (internal)

```
types.ts ──type──> health.ts        (HealthScore)
health.ts ──type──> types.ts        (Severity)          ** MUTUAL type dep **
schema-pipeline.ts ──> @std/path
schema.ts ──> schema-pipeline.ts, @std/jsonc
code-schema.ts ──> schema-pipeline.ts, @std/jsonc
code-schema.ts ──type──> shared/types.ts       ** UPWARD type dep **
code-schema.ts ──type──> composition-loader.ts
composition-loader.ts ──> schema-pipeline.ts, @std/jsonc
config.ts ──> (nothing — standalone)
mod.ts ──> (barrel — re-exports everything)
```

### Layer 0 --> Layer 1

```
data/types.ts ──type──> foundation/health.ts    (HebrewState, HebrewDirection)
data/types.ts ──type──> foundation/types.ts     (Severity)
```

### Layer 2: Shared

```
shared/types.ts ──> (nothing — pure types + constants)
shared/code-4block.ts ──> shared/types.ts
shared/code-4block.ts ──type──> foundation/mod.ts    (SchemaSubsectionDef, etc.)
shared/code-4block.ts ──> foundation/mod.ts          (error, warn, info)
shared/mod.ts ──> (barrel)
```

### Layer 3: Engine

```
registry.ts ──type──> foundation/mod.ts
discovery.ts ──> engine/registry.ts, foundation/mod.ts, @std/fs, @std/path
output.ts ──> foundation/mod.ts
logging.ts ──> foundation/health.ts
fill.ts ──> foundation/mod.ts, engine/registry.ts
transpiler-types.ts ──type──> foundation/mod.ts, foundation/composition-loader.ts
transpiler-mappings.ts ──> foundation/schema-pipeline.ts, @std/jsonc
transpiler-ast.ts ──> engine/transpiler-types.ts
transpiler.ts ──> foundation/mod.ts, engine/transpiler-*.ts, shared/types.ts
```

### Layer 4: Handlers

```
go.ts ──> foundation/mod.ts, engine/mod.ts, shared/mod.ts
rust.ts ──> foundation/mod.ts, engine/mod.ts, shared/mod.ts
toml.ts ──> foundation/mod.ts, foundation/code-schema.ts, engine/mod.ts
dotfiles.ts ──> foundation/mod.ts, engine/mod.ts
json.ts ──> foundation/mod.ts, engine/mod.ts
makefile.ts ──> foundation/mod.ts, engine/mod.ts
ofd.ts ──> foundation/mod.ts, engine/mod.ts
omni.ts ──> foundation/mod.ts, engine/mod.ts
```

---

## Three Observations

### 1. The Upward Type Dependency

```
foundation/code-schema.ts ──type──> shared/types.ts
```

This is the only arrow that points upward. `code-schema.ts` imports
`SubsectionDef` from `shared/types.ts`. Foundation depends on
a shared type. This is backwards.

**Why it exists:** `loadCodeRules()` returns `Code4BlockRules`, which contains
`setupSubsections: SubsectionDef[]`. The shared utilities defined `SubsectionDef`
because that's where it was needed first. Then foundation started producing it.

**Fix path:** Move `SubsectionDef` to `foundation/types.ts` or `data/types.ts`.
It's a structural type (tag + regex pattern), not a shared-specific type. The
shared utilities can import it from foundation.

**Impact:** 3 files change imports. Zero logic changes. Clean.

### 2. The Mutual Type Dependency

```
foundation/types.ts ──type──> foundation/health.ts    (HealthScore)
foundation/health.ts ──type──> foundation/types.ts     (Severity)
```

These two files import types from each other. This works in TypeScript because
type-only imports don't create runtime circular dependencies. But it's a
design smell — these two files are conceptually one unit that got split.

**Options:**
- A. Merge them (both are foundation types)
- B. Extract shared types to a `foundation/core-types.ts` that both import
- C. Leave it (TypeScript handles it, and they're in the same layer)

I'd go with C for now. They're peers in the same layer, the circularity is
type-only, and splitting them further adds files without adding clarity.

### 3. The Engine-to-Shared Dependency

```
engine/transpiler.ts ──type──> shared/types.ts    (SubsectionDef)
```

Same issue as #1. The transpiler needs `SubsectionDef` for subsection
matching. It imports from the shared layer. This is the same type — same fix
applies. Move `SubsectionDef` to foundation, both consumers import from there.

---

## Layer Sizes (Lines of Code)

| Layer | Files | Lines | Role |
|:-----:|:-----:|------:|------|
| Foundation | 8 | ~3,400 | Types, schemas, health, config |
| Data | 2 | ~590 | Compiled registries (growing) |
| Shared | 3 | ~2,200 | Shared 4-block infrastructure |
| Engine | 9 | ~3,800 | Orchestration, output, transpiler |
| Handlers | 8 | ~11,500 | Format-specific logic |
| **Total** | **30** | **~21,500** | |

Handlers are 53% of the codebase. That's expected — Go (2,800 lines) and
Rust (4,500 lines) are the heavy lifters. The other 6 handlers average
~500 lines each.

---

## What This Means for the Data Layer

The data layer (Layer 0) is correctly positioned. It depends only on
foundation types (Layer 1), never on shared, handlers, or engine. The one fix needed —
moving `SubsectionDef` down from shared to foundation — clears the only
upward arrow in the entire graph.

After that fix, the dependency rule is absolute:

```
Layer N can import from Layer N-1, N-2, ..., 0.
Layer N NEVER imports from Layer N+1, N+2, ...
Within a layer, peers can import from each other.
```

That's a clean architecture. Five layers, one rule, no exceptions.
