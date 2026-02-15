# L1-OmniCode Production-Grade Treatment

## Context

L0 has 6 Rust crates (202 tests), full Go packages with identity registration, comprehensive Makefile, and cws-struct lint coverage. L1 was just reorganized into three-domain structure (ladder/spiral/hybrid) but has **zero** Rust code, **zero** tests, **no** identity registration, and **no** Makefile.

The goal isn't translation — it's **architectural verification through the compiler**. L0 was designed with explicit L1 connection points that have never been exercised:
- `formats.rs:72` — "L1 calls this to register OmniCode-specific extensions"
- `format_loader.rs:163` — `load_toml_file<T: DeserializeOwned>` ready for L1 dispatch
- `identity/lib.rs:78` — `register(key, pragma, metadata)` waiting for L1 packages
- `lib.rs:199` — `load_language()` already wraps `load_system("language")`

Building L1 in Rust using L0's API makes the compiler witness whether these connections are real.

## Gap Already Discovered

L0 has `load_toml_file<T>(path)` but **no** `load_jsonc_file<T>(path)`. Go has both (`LoadTOMLFile[T]` and `LoadJSONCFile[T]`). L1's dispatch needs both. This gap needs fixing in L0 before L1 can fully connect.

## Plan

### Step 1: L0 Enhancement — Add `load_jsonc_file<T>` to format_loader.rs

**File:** `b-word/L0-universal/hybrid/config/src/format_loader.rs`

Add path-based JSONC loader that mirrors `load_toml_file<T>`:

```rust
pub fn load_jsonc_file<T: DeserializeOwned>(path: &Path) -> Result<T, ConfigError> {
    // Read file → strip_jsonc_comments → serde_json::from_str
}
```

**File:** `b-word/L0-universal/hybrid/config/src/lib.rs`
- Re-export `load_jsonc_file` from format_loader

**File:** `b-word/L0-universal/hybrid/config/src/lib.rs` (tests)
- Add `test_load_jsonc_file_typed` test matching `test_load_toml_file_typed` pattern

### Step 2: L1 Rust Pragma Crate

**New crate:** `b-word/L1-omnicode/hybrid/pragma/`

```
pragma/
├── Cargo.toml
└── src/
    ├── lib.rs          # Module root + identity (PRAGMA/METADATA/register_identity)
    ├── reader.rs       # #!omni pragma parser (strip prefix, parse lines)
    ├── types.rs        # Type, Format, Modifier, Pragma struct, Extension, Category, StateMapping
    ├── extensions.rs   # 16-extension registry + lookup functions
    └── dispatch.rs     # Format dispatch — pragma → L0 parser
```

#### Cargo.toml

```toml
[package]
name = "bereshit-l1-pragma"
version = "0.1.0"
description = "OmniCode pragma reader — identity before content"
edition.workspace = true
rust-version.workspace = true
authors.workspace = true
repository.workspace = true

[dependencies]
serde = { workspace = true }
bereshit-l0-config = { workspace = true }
bereshit-l0-identity = { workspace = true }
```

#### Key L0 Connections (compiler-verified)

| L1 Function | Calls L0 | Import |
|-------------|----------|--------|
| `dispatch::load_config<T>` | `bereshit_l0_config::load_toml_file::<T>(path)` | format_loader |
| `dispatch::load_config<T>` | `bereshit_l0_config::load_jsonc_file::<T>(path)` | format_loader (Step 1) |
| `extensions::register_all()` | `bereshit_l0_config::register_format(".omni", "omni")` × 16 | formats |
| `lib::register_identity()` | `bereshit_l0_identity::register(key, pragma, metadata)` | identity |
| `dispatch::load_config_map` | `bereshit_l0_config::load_config_file(path)` | format_loader |

#### types.rs — Shared Types

Ported from pragma.go with Rust idioms:

```rust
// Type represents what a file IS
pub enum FileType { Universal, Template, Code, Documentation, Interface, Data, Folder, Spec }

// Format represents the target language/format
pub enum Format { Omni, Go, C, Rs, Adoc, Md, Json, Yaml, Toml, Txt }

// Modifier refines the subtype
pub enum Modifier { None, Library, Executable, Test, Demo }

// Pragma holds the parsed identity of an OmniCode file
pub struct Pragma {
    pub file_type: FileType,
    pub format: Format,
    pub modifier: Modifier,
    pub key: String,
    pub from: String,
    pub at: String,
    pub raw_lines: Vec<String>,
}

// Extension holds the full identity of an OmniCode file extension
// Ported from extensions.go — TOML spec is truth, Rust is derived
pub struct Extension { ... } // 12 fields matching Go struct

// Category classifies extensions into families
pub enum Category { OmniSuite, Biblical, Cognitive }

// StateMapping connects extensions to state machine commands
pub enum StateMapping { Await, Proceed, Clear, Pivot, Halt }
```

#### reader.rs — Pragma Parser

Ported from pragma.go. **No L0 dependencies** — parsing is L1's domain.

- `strip_comment_prefix(line: &str) -> Option<&str>` — strips `//`, `#`, `<!--` prefixes
- `parse_line1(line: &str) -> (FileType, Format, Modifier)` — type/format/modifier from first pragma
- `parse_meta_line(line: &str) -> Option<(&str, &str)>` — field/value from meta lines
- `pub fn parse(path: &Path) -> Result<Pragma, PragmaError>` — reads file, extracts pragma
- `pub fn parse_reader(reader: &mut BufReader<impl Read>) -> Result<Pragma, PragmaError>`
- `pub fn parse_str(content: &str) -> Result<Pragma, PragmaError>` — for testing

#### extensions.rs — Extension Registry

Ported from extensions.go. Registers with L0 format registry.

- `static REGISTRY: LazyLock<BTreeMap<&str, Extension>>` — 16 entries
- `pub fn lookup(ext: &str) -> Option<&Extension>`
- `pub fn is_registered(ext: &str) -> bool`
- `pub fn by_category(cat: Category) -> Vec<&Extension>`
- `pub fn by_phase(phase: &str) -> Vec<&Extension>`
- `pub fn by_state(state: StateMapping) -> Vec<&Extension>`
- `pub fn all() -> Vec<&Extension>`
- `pub fn register_all_with_l0()` — calls `bereshit_l0_config::register_format()` for each

#### dispatch.rs — Format Dispatch

Ported from dispatch.go. **This is the L0-L1 bridge.**

```rust
use bereshit_l0_config::{self as config, ConfigError};

pub fn load_config<T: DeserializeOwned>(path: &Path) -> Result<(Pragma, T), DispatchError> {
    let pragma = crate::parse(path)?;
    let result: T = match pragma.format {
        Format::Toml => config::load_toml_file(path)?,
        Format::Json => config::load_jsonc_file(path)?,
        Format::Omni => return Err(DispatchError::NativeNotImplemented),
        _ => return Err(DispatchError::UnsupportedFormat(pragma.format)),
    };
    Ok((pragma, result))
}
```

#### Tests (in each module)

| Module | Tests | Count |
|--------|-------|-------|
| reader.rs | parse Go/TOML/HTML comments, parse line1 variants, parse meta lines, contiguous pragma, max line limit | ~12 |
| types.rs | Display impls, Default for Format (Omni), string conversions | ~5 |
| extensions.rs | lookup known/unknown, by_category/phase/state, all 16 registered, register_all_with_l0 | ~10 |
| dispatch.rs | load TOML config, load JSONC config, unsupported format error, missing pragma error | ~6 |
| lib.rs | identity completeness (P1-P5, M1-M10), register_identity | ~3 |
| **Total** | | **~36** |

### Step 3: Workspace Updates

**File:** `b-word/Cargo.toml`

```toml
[workspace]
members = [
    "L0-universal/hybrid/cli",
    "L0-universal/hybrid/config",
    "L0-universal/hybrid/format",
    "L0-universal/hybrid/output",
    "L0-universal/hybrid/paths",
    "L0-universal/spiral/identity",
    "L1-omnicode/hybrid/pragma",     # NEW
]

[workspace.dependencies]
# Existing...
bereshit-l1-pragma = { path = "L1-omnicode/hybrid/pragma" }  # NEW
```

### Step 4: Go Tests for L1

**New file:** `b-word/L1-omnicode/hybrid/pragma/pragma_test.go`

Tests for the existing Go code that has zero test coverage:

| Area | Tests |
|------|-------|
| `stripCommentPrefix` | Go (`//`), TOML (`#`), HTML (`<!-- -->`), bare, non-pragma lines |
| `parseLine1` | type+format+modifier, format only, defaults to omni |
| `parseMetaLine` | key/from/at fields, non-meta lines |
| `Parse` / `ParseString` | Full pragma from Go/TOML/HTML files, no-pragma error, MaxPragmaLines |
| `IsConfig` / `IsNativeOmni` / `IsFolder` | Method behaviors |
| `BlockPattern` | Returns correct pattern per type |
| `String` | Round-trip representation |
| `LookupExt` / `IsRegisteredExt` | Known and unknown extensions |
| `ExtsByCategory` / `ExtsByPhase` / `ExtsByState` | Filtering |
| `AllExts` | Count matches registry |
| `InternalFormatForExt` / `BlockPatternForExt` / `DefaultTypeForExt` | Format bridge |
| `IsOmniCodeFile` | .omni/.ofd detection |

### Step 5: Go Identity Registration

**File:** `b-word/L1-omnicode/hybrid/pragma/pragma.go`

Add at bottom of SETUP block (before END SETUP):

```go
import "creativeworkzstudio.com/bereshit/L0-universal/spiral/identity"

var _pragma = map[string]string{
    "P1.key": "B-L1-omnicode-hybrid-pragma",
    "P1.type": "code", "P1.format": "go", "P1.style": "library",
    "P2.blocks": "4-block",
    "P3.from": "bereshit/word/omni/syntax.omni",
    "P4.at": "a-01.00",
    "P5.title": "OmniCode Pragma Reader",
    "P5.summary": "Parse #!omni pragma lines from any file format",
}

var _metadata = map[string]string{
    "M1.key": "B-L1-omnicode-hybrid-pragma",
    "M6.layer": "L1-omnicode", "M6.domain": "pragma",
    "M6.tags": "pragma, identity, parser, omnicode",
    "M7.purpose": "The first thing read in any OmniCode file — identity before content",
    "M8.scripture": "Genesis 1:3 — And God said, Let there be light",
    "M8.anchor": "Genesis 1:1",
    // ... remaining M sections
}

func init() {
    identity.Register(_pragma["P1.key"], _pragma, _metadata)
}
```

### Step 6: L1 Makefile

**New file:** `b-word/L1-omnicode/Makefile`

Mirrors L0's pattern with cached quality gates:

```makefile
.PHONY: build test lint clean help

build:           # Go build + Rust build
test:            # Go test + Rust test
lint:            # cws-struct lint (Go + Rust + TOML)
vet:             # Go vet
clean:           # Remove build artifacts
help:            # Show targets
```

### Step 7: STRUCT.adoc Update

Update the L1 section in STRUCT.adoc to mark completed items:
- [x] hybrid/pragma/ Rust crate
- [x] Go tests for pragma package
- [x] Go identity registration
- [x] L1 Makefile

## Execution Order

| # | Step | Why This Order |
|:-:|------|----------------|
| 1 | L0 `load_jsonc_file<T>` | Foundation first — L1 needs this to connect |
| 2 | L1 Rust crate (types → reader → extensions → dispatch → lib) | Block by block, compiler verifies each |
| 3 | Workspace Cargo.toml | Wire L1 into the workspace |
| 4 | `cargo build` + `cargo test` | **The compiler witnesses** |
| 5 | Go tests | Second witness — same contracts, different language |
| 6 | Go identity registration | L1 becomes a known citizen |
| 7 | L1 Makefile | Quality gates |
| 8 | cws-struct lint | Third witness — structural alignment |

## Critical Files

| File | Action |
|------|--------|
| `L0-universal/hybrid/config/src/format_loader.rs` | Add `load_jsonc_file<T>` |
| `L0-universal/hybrid/config/src/lib.rs` | Re-export + test |
| `L1-omnicode/hybrid/pragma/Cargo.toml` | **NEW** |
| `L1-omnicode/hybrid/pragma/src/lib.rs` | **NEW** — module root + identity |
| `L1-omnicode/hybrid/pragma/src/types.rs` | **NEW** — FileType, Format, Modifier, Pragma, Extension |
| `L1-omnicode/hybrid/pragma/src/reader.rs` | **NEW** — #!omni parser |
| `L1-omnicode/hybrid/pragma/src/extensions.rs` | **NEW** — 16-extension registry |
| `L1-omnicode/hybrid/pragma/src/dispatch.rs` | **NEW** — L0-L1 format bridge |
| `L1-omnicode/hybrid/pragma/pragma_test.go` | **NEW** — Go test coverage |
| `L1-omnicode/hybrid/pragma/pragma.go` | Add _pragma/_metadata + init() |
| `L1-omnicode/Makefile` | **NEW** — quality gates |
| `b-word/Cargo.toml` | Add L1 workspace member |
| `b-word/STRUCT.adoc` | Update L1 migration map |

## Reuse

| From | What | Used By |
|------|------|---------|
| L0 config `load_toml_file<T>` | Generic TOML loader | L1 dispatch |
| L0 config `load_jsonc_file<T>` | Generic JSONC loader (Step 1) | L1 dispatch |
| L0 config `register_format()` | Extension registration | L1 extensions |
| L0 config `load_config_file()` | Format-agnostic map loader | L1 dispatch (map variant) |
| L0 identity `register()` | Package identity | L1 lib.rs |
| L0 identity `by_layer("L1")` | Layer query verification | L1 tests |
| L0 Makefile pattern | Cached quality gates | L1 Makefile |
| L0 Go identity pattern | `_pragma` + `_metadata` + `init()` | L1 Go identity |

## Verification

```bash
# Step 1: L0 enhancement
cd .a-new-structure/b-word && cargo test -p bereshit-l0-config

# Step 4: Compiler witnesses L0-L1 connectivity
cargo build -p bereshit-l1-pragma
cargo test -p bereshit-l1-pragma

# Step 5: Go tests
go test ./L1-omnicode/hybrid/pragma/...

# Step 6: Go identity registration works
go build ./L1-omnicode/...

# Step 7: Makefile quality gates
cd L1-omnicode && make test && make lint

# Step 8: Full workspace
cd .a-new-structure/b-word && cargo test --workspace
go test ./...

# Step 9: cws-struct lint
cd .a-new-structure && deno run --allow-read c-tov/cws-struct/mod.ts lint rust b-word/L1-omnicode/
deno run --allow-read c-tov/cws-struct/mod.ts lint go b-word/L1-omnicode/

# Step 10: Identity system verification
# L1 packages appear in identity registry:
#   by_layer("L1-omnicode") returns bereshit-l1-pragma
#   16 OmniCode extensions registered in L0 format registry
```
