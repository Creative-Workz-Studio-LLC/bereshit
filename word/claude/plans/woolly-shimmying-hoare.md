# Plan: L0-Universal Consolidation — Lint + Organize + Flow

## Context

L0-universal is the foundation layer. Everything above it (L1-L5) depends on it. Right now L0 has a mix of **real code** (7 Go packages, 60+ TOML specs) and **empty placeholder noise** (definitions/ with 4 .gitkeep stubs, libmath/ with 1 .gitkeep, libtrit/ with 13 .gitkeep files and zero C code). The Go code also has 20 Go 1.26 modernization issues (interface{} → any, manual map loops → maps.Copy, if/assign → max()). And the util/ tree is 3 levels deep for 4 Go files — `util/fs/paths/` and `util/pure/{cli,format,output}/` adds classification noise without clarity.

**Three problems, one pass:**
1. Go modernization — fix all lint diagnostics
2. Folder consolidation — remove empty scaffolding, flatten util/
3. Dependency flow verification — ensure L0 builds cleanly and packages flow correctly

## Step 1: Go Modernization (20 changes across 6 files)

Fix all Go 1.26 idiom issues. No API behavior changes — pure type alias and builtin substitutions.

### 1a. `interface{}` → `any` (17 occurrences)

| File | Count | Functions Affected |
|------|:-----:|-------------------|
| `util/pure/output/output.go` | 7 | KeyValue, KeyValueWidth, Success, Info, Tip, JSONField, JSONFieldLast |
| `util/pure/cli/cli.go` | 4 | Fatal, FatalCode, Error, Warn |
| `config/util/loader.go` | 3 | LoadFile, LoadBytes, FindAndLoad |
| `config/util/jsonc.go` | 3 | LoadJSONCMap (signature + 2 body refs) |

### 1b. Manual map copy → `maps.Copy` (3 occurrences)

| File | Location |
|------|----------|
| `config/util/doc.go` | Pragma() function (lines 190-194) |
| `config/util/doc.go` | Metadata() function (lines 198-203) |
| `identity/registry.go` | copyMap helper (line 302) |

Requires: `import "maps"` in both files.

### 1c. `if/assign` → `max()` builtin (1 occurrence)

| File | Location |
|------|----------|
| `util/pure/output/output.go` | Header() — `if width < 40 { width = 40 }` → `width = max(len(title), 40)` |

## Step 2: Folder Consolidation

### 2a. Scaffold directories — KEEP

These directories are staging areas for incoming content. Leave them in place:

| Directory | Purpose | Status |
|-----------|---------|--------|
| `definitions/` | Glossary terms migrating from `word/glossary/` | Scaffolded |
| `libmath/` | Future C library for mathematical foundations | Planned |
| `libtrit/` | C library migrating from `word/work/pkg/trit/` | Scaffolded |
| `foundation/manifest/` | Config manifests for spec loading | Planned |

### 2b. Flatten util/ tree

**Current** (3 levels deep):
```
util/
├── fs/paths/paths.go          ← 3 dirs for 1 file
└── pure/
    ├── cli/cli.go             ← 3 dirs for 1 file
    ├── format/format.go       ← 3 dirs for 1 file
    └── output/output.go       ← 3 dirs for 1 file
```

**Target** (2 levels — standard Go package layout):
```
util/
├── paths/paths.go
├── cli/cli.go
├── format/format.go
└── output/output.go
```

The `fs/` and `pure/` classification folders serve no purpose when there are only 4 packages. Each package name already describes its domain.

**Import path changes:**

| Old Import | New Import |
|------------|-----------|
| `L0-universal/util/fs/paths` | `L0-universal/util/paths` |
| `L0-universal/util/pure/cli` | `L0-universal/util/cli` |
| `L0-universal/util/pure/format` | `L0-universal/util/format` |
| `L0-universal/util/pure/output` | `L0-universal/util/output` |

**Impact:** Only L0 files reference the new import paths — L3/L5 files still use old `word/work/pkg/` or `cws.studio/pkg/` paths. Clean move.

**Updates required in each moved file:**
- `M3.import` metadata value
- `P1.key` pragma value (path component change)

## Step 3: Update All Internal References

After the moves, update:

1. **Makefile** (`L0-universal/Makefile`) — expand from `./config/...` to `./...` to cover all L0 packages
2. **root.adoc** — rewrite to reflect actual L0 contents (Go packages + TOML specs, not just C libraries)
3. **Pragma/metadata** in each moved file — `P1.key`, `M1.key`, `M3.import` values

## Step 4: Run cws-struct Linter

Run `cws-struct lint go` against all L0 Go files to verify structural alignment (4-block, //omni: directives, separator consistency).

## Step 5: Final Build + Vet Verification

```bash
cd b-word && go build ./L0-universal/...
cd b-word && go vet ./L0-universal/...
```

## Target L0 Structure

```
L0-universal/
├── .cache/              ← Build cache sentinels
├── Makefile             ← Updated to build all L0 packages
├── root.adoc            ← Updated to reflect actual contents
│
│── config/              ← Go config loader (3000+ lines)
│   ├── loader.go, loader_test.go, loader-api.adoc
│   └── util/ (10 Go files)
├── identity/            ← Go identity registry (1 file)
│   └── registry.go
├── util/                ← Go utilities (4 packages, FLATTENED)
│   ├── paths/paths.go   ← Filesystem path resolution
│   ├── cli/cli.go       ← CLI scaffolding
│   ├── format/format.go ← Value formatting
│   └── output/output.go ← Terminal output
│
│── foundation/          ← TOML specs (60+ files)
│   ├── index.toml, cc-alignment-todo.txt
│   ├── bible/ (8 TOML), math/ (5 TOML), types/ (36+ TOML)
│   └── manifest/        ← Scaffold (incoming)
├── definitions/         ← Scaffold (glossary migrating from word/glossary/)
├── libmath/             ← Scaffold (future C library)
└── libtrit/             ← Scaffold (C library migrating from word/work/pkg/trit/)
```

**Before:** util/ is 3 levels deep for 4 files
**After:** util/ is 2 levels (standard Go package layout), scaffold dirs preserved for incoming content

## Critical Files

| File | Action |
|------|--------|
| `util/pure/output/output.go` | EDIT (8 lint fixes) → MOVE to `util/output/` |
| `util/pure/cli/cli.go` | EDIT (4 lint fixes) → MOVE to `util/cli/` |
| `util/pure/format/format.go` | MOVE to `util/format/` |
| `util/fs/paths/paths.go` | MOVE to `util/paths/` |
| `config/util/loader.go` | EDIT (3 lint fixes) |
| `config/util/jsonc.go` | EDIT (3 lint fixes) |
| `config/util/doc.go` | EDIT (2 lint fixes) |
| `identity/registry.go` | EDIT (1 lint fix) |
| `Makefile` | EDIT (expand build scope) |
| `root.adoc` | REWRITE (reflect actual contents) |
| `definitions/`, `libmath/`, `libtrit/`, `foundation/manifest/` | KEEP (scaffold for incoming content) |

## Execution Order

```
Step 1 (Go modernization)  ──→  Step 2b (flatten util/)  ──→  Step 3 (update refs)
                                                                       │
                                                                       ↓
                                                              Step 4 (cws-struct lint)
                                                                       │
                                                                       ↓
                                                              Step 5 (build + vet)
```

Lint fixes first (don't move code that needs fixing), then flatten util/ (needs ref updates), then verify everything.

## Verification

1. `go build ./L0-universal/...` — compiles clean
2. `go vet ./L0-universal/...` — no warnings
3. `go test ./L0-universal/config/...` — 49/49 PASS
4. `cws-struct lint go L0-universal/` — structural alignment passes
5. `find L0-universal/util/ -maxdepth 1 -type d` — 4 dirs (paths, cli, format, output)
6. No stale `util/fs/` or `util/pure/` directories remaining
7. No import path errors in go build output
