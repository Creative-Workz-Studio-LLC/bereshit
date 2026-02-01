# Bereshit Dependencies & Self-Hosting Analysis

**Key:** B-void-planning-dependencies
**Status:** Active
**Version:** 1.0.0
**Created:** 2026-02-01
**Updated:** 2026-02-01
**Authors:** Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)

---

> *"Except the LORD build the house, they labour in vain that build it"* — Psalm 127:1 KJV

---

## Executive Summary

This document catalogs all dependencies in the Bereshit codebase to support self-hosting goals. The analysis reveals:

- **Go:** 3 external dependencies (TOML, SQLite, TUI)
- **C:** 2 external libraries (FUSE3, ncurses)
- **Self-Hosting Readiness:** High - most external deps can be replaced

---

## Table of Contents

1. [Go Dependencies](#1-go-dependencies)
2. [C Dependencies](#2-c-dependencies)
3. [Self-Hosting Requirements](#3-self-hosting-requirements)
4. [Code Gaps](#4-code-gaps)
5. [Dependency Reduction Plan](#5-dependency-reduction-plan)

---

## 1. Go Dependencies

### 1.1 Standard Library (Used Extensively)

The codebase relies heavily on Go's standard library, which is excellent for self-hosting:

| Package | Files | Purpose |
|---------|-------|---------|
| `fmt` | 170 | Formatting and output |
| `os` | 144 | File operations, environment |
| `time` | 97 | Temporal operations |
| `path/filepath` | 87 | Path manipulation |
| `strings` | 87 | String operations |
| `encoding/json` | 68 | JSON marshaling |
| `flag` | 17 | Command-line parsing |
| `context` | 18 | Context management |
| `testing` | 22 | Test framework |
| `regexp` | 12 | Pattern matching |
| `sort` | 14 | Sorting operations |
| `bufio` | - | Buffered I/O |
| `sync` | - | Synchronization |
| `math` | - | Mathematics |
| `io` | - | I/O primitives |
| `runtime` | - | Runtime info |
| `hash/fnv` | - | FNV hashing |
| `embed` | - | Embedded files |
| `syscall` | - | System calls |
| `unicode` | - | Unicode support |
| `database/sql` | - | SQL interface |
| `text/template` | - | Text templating |

### 1.2 External Dependencies

| Package | Version | Files | Purpose | Self-Host Priority |
|---------|---------|-------|---------|-------------------|
| `github.com/BurntSushi/toml` | v1.5.0 | 24 | TOML config parsing | **REPLACEABLE** - C impl exists |
| `github.com/mattn/go-sqlite3` | v1.14.24 | 4 | SQLite3 database | **REQUIRED** - Standard storage |
| `github.com/charmbracelet/bubbles` | v0.18.0 | 1 | TUI components | **OPTIONAL** - Can remove |
| `github.com/charmbracelet/bubbletea` | v0.25.0 | 1 | TUI framework | **OPTIONAL** - Can remove |
| `github.com/charmbracelet/lipgloss` | v0.9.1 | 1 | TUI styling | **OPTIONAL** - Can remove |

### 1.3 Transitive Dependencies (via Charmbracelet)

These 13 packages come in through the TUI framework:
- containerd/console, mattn/go-isatty, mattn/go-localereader
- mattn/go-runewidth, lucasb-eyer/go-colorful
- aymanbagabas/go-osc52, muesli/ansi, muesli/termenv
- muesli/reflow, rivo/uniseg, atotto/clipboard
- muesli/cancelreader, golang.org/x/sync, golang.org/x/sys
- golang.org/x/term, golang.org/x/text

**Isolation:** All contained in `word/work/cmd/cpisi/tui.go` - easily removable.

---

## 2. C Dependencies

### 2.1 C Standard Library

| Header | Purpose |
|--------|---------|
| `assert.h` | Runtime assertions |
| `ctype.h` | Character classification |
| `errno.h` | Error reporting |
| `math.h` | Mathematical functions |
| `stdarg.h` | Variable arguments |
| `stdbool.h` | Boolean type |
| `stddef.h` | Standard definitions |
| `stdint.h` | Fixed-width integers |
| `stdio.h` | Standard I/O |
| `stdlib.h` | Memory, conversion |
| `string.h` | String manipulation |
| `strings.h` | BSD string functions |
| `time.h` | Time handling |

### 2.2 POSIX/System Headers

| Header | Purpose | Used By |
|--------|---------|---------|
| `dirent.h` | Directory reading | omni, fuse |
| `fcntl.h` | File control | fuse |
| `signal.h` | Signal handling | omni IDE |
| `sys/stat.h` | File status | omni, fuse |
| `sys/types.h` | System types | omni |
| `unistd.h` | POSIX API | fuse, omni |

### 2.3 External Libraries

| Library | Header | Files | Purpose | Self-Host Priority |
|---------|--------|-------|---------|-------------------|
| **FUSE 3** | `fuse3/fuse.h` | 1 | User-space filesystem | **REQUIRED** for L2 |
| **ncurses** | `ncurses.h` | 2 | Terminal UI | **OPTIONAL** - Can replace |

### 2.4 Project Internal Libraries

**libtrit** (30+ headers):
- Core: `trit.h`, `config.h`, `math.h`
- Spatial: `dimension.h`, `cube.h`, `hebrew.h`, `kalign.h`, `trajectory.h`
- Temporal: `temporal.h`
- Scripture: `verse_addr.h`, `scripture_text.h`, `scripture_path.h`, `bible_rail.h`
- System: `statemachine.h`, `toml.h`
- Generated: 12 `.gen.h` files

**libomni** (20+ headers):
- Frontend: lexer, parser, semantic, keywords, hebrew
- Backend: IR, codegen
- Runtime: VM, runtime, debug, profile, retrieval
- CPI-SI: cpisi, mind, health_file
- IDE: core, cli, tui, gui, panels
- Transpiler: C, menu

---

## 3. Self-Hosting Requirements

### 3.1 What We Already Have

| Component | Status | Notes |
|-----------|--------|-------|
| **TOML Parser (C)** | ✅ COMPLETE | `word/work/pkg/trit/src/toml.c` - 1,200+ lines |
| **TOML Parser (Go)** | 🔄 EXTERNAL | Uses BurntSushi/toml - can bridge to C |
| **JSON Parser (C)** | ✅ COMPLETE | `engine/core/src/jsonc.c` |
| **Ternary Math** | ✅ COMPLETE | `libtrit` - full implementation |
| **OmniCode Lexer** | ✅ COMPLETE | `libomni` frontend |
| **OmniCode Parser** | ✅ COMPLETE | `libomni` frontend |
| **State Machine** | ✅ COMPLETE | Go + C implementations |
| **Coordinate System** | ✅ COMPLETE | Bible-grounded coordinates |

### 3.2 What Needs Replacement

| Dependency | Priority | Replacement Strategy |
|------------|----------|---------------------|
| **BurntSushi/toml** | HIGH | Create Go wrapper for C `toml.c` via cgo |
| **go-sqlite3** | MEDIUM | Keep for now - standard, widely used |
| **Charmbracelet TUI** | LOW | Optional - can remove entirely |
| **ncurses** | LOW | Optional - can use raw terminal |
| **FUSE 3** | N/A | Required for L2 platform layer |

### 3.3 Self-Hosting Tiers

**Tier 1: Minimal External (Achievable Now)**
- Replace BurntSushi/toml with cgo wrapper
- Remove Charmbracelet TUI (use CLI only)
- Remove ncurses (use omni_ide_cli.c)

**Tier 2: Standard Only (Medium Term)**
- Keep only go-sqlite3 and FUSE3
- Both are interfaces to OS-level standards

**Tier 3: Full Self-Host (Long Term)**
- Implement OmniDB (replace SQLite)
- Implement native filesystem driver (replace FUSE)
- Requires L2 Platform layer completion

---

## 4. Code Gaps

### 4.1 Critical Gaps (Phase 3 Features)

| Feature | Location | Status |
|---------|----------|--------|
| Config Reader Integration | `word/work/pkg/config/loader.go:1522` | PLANNED |
| Schema Validation | `word/work/pkg/config/loader.go:1523` | PLANNED |
| Type-safe Config Structs | `word/work/pkg/config/loader.go:1524` | PLANNED |
| Config Value Tests | `word/work/pkg/config/loader_test.go:884` | PLANNED |

### 4.2 C Library Gaps (libtrit)

| Component | Gap | Priority |
|-----------|-----|----------|
| **math.h** | Implementation in math.c (Part 2+) | HIGH |
| **math.h** | Test coverage (Part 7) | HIGH |
| **trit.c** | trit_compare, trit_min, trit_max | MEDIUM |
| **dimension.h** | Compound dimension states | MEDIUM |
| **dimension.h** | Dimensional distance calculation | MEDIUM |
| **hebrew.h** | State transition history | LOW |
| **trajectory.h** | Trajectory prediction | LOW |
| **kalign.h** | History ring buffer | LOW |
| **cube.h** | Path planning, neighbor enumeration | MEDIUM |
| **verse_addr.h** | Bulk encode/decode functions | LOW |
| **temporal.h** | Temporal distance, navigation | MEDIUM |
| **toml.c** | toml_parse_float, toml_parse_array | LOW |
| **pack.c** | Batch pack operations, SIMD | LOW |

### 4.3 Test Coverage Gaps

| Area | Gap | Notes |
|------|-----|-------|
| Config Loader | Error cases (malformed TOML) | Planned |
| Config Loader | Edge cases (empty, large files) | Planned |
| Coordinates | LoadBibleData failure handling | Conditional skip |
| Orchestration | Full filesystem tests | Environment dependent |

### 4.4 Reserved Infrastructure (Future Phases)

| Location | Phase | Purpose |
|----------|-------|---------|
| `loader.go:260` | 3 | Typed accessors, conversion methods |
| `loader.go:324` | 3 | Cached manifests, loaded specs registry |
| `loader.go:346` | 3+ | Rails infrastructure |
| `loader.go:896` | 3 | Error collection, partial load |

---

## 5. Dependency Reduction Plan

### 5.1 Immediate Actions (Can Do Now)

1. **Create cgo TOML wrapper**
   ```go
   // word/L0-universal/foundation/toml/toml.go
   // #cgo CFLAGS: -I${SRCDIR}/../../libtrit/include
   // #cgo LDFLAGS: -L${SRCDIR}/../../libtrit -ltrit
   // #include "toml.h"
   import "C"

   func LoadFile(path string) (string, error) {
       cpath := C.CString(path)
       defer C.free(unsafe.Pointer(cpath))
       result := C.toml_load_file(cpath)
       // ...
   }
   ```

2. **Remove Charmbracelet TUI** (if not needed)
   - Delete `word/work/cmd/cpisi/tui.go`
   - Update go.mod to remove dependencies
   - Use CLI-only interface

3. **Remove ncurses** (optional)
   - Use `omni_ide_cli.c` instead of `omni_ide_tui.c`
   - Raw terminal output sufficient for most cases

### 5.2 Short-Term Actions (Next Phase)

1. **Complete math.c implementation**
   - Implement functions declared in math.h
   - Add test coverage

2. **Complete TOML parser features**
   - Add `toml_parse_float`
   - Add `toml_parse_array`
   - Full TOML 1.0 compliance

3. **Phase 3 Config System**
   - Schema validation
   - Type-safe config structs

### 5.3 Long-Term Vision (Self-Hosted Stack)

```
Current Dependencies          →    Self-Hosted Replacements
────────────────────────────────────────────────────────────
BurntSushi/toml (Go)         →    cgo wrapper to libtrit/toml.c
go-sqlite3                    →    OmniDB (custom L2 database)
Charmbracelet TUI            →    OmniCode TUI (L5 application)
ncurses                      →    Native terminal (OmniCode)
FUSE 3                       →    MillenniumOS native (L2)
```

---

## Dependency Summary

### Current State

| Language | Standard Lib | External | Self-Host Ready |
|----------|-------------|----------|-----------------|
| **Go** | 20+ packages | 3 deps | 85% |
| **C** | 13 headers | 2 libs | 90% |

### External Dependency Justification

| Dependency | Justification | Replacement Difficulty |
|------------|---------------|----------------------|
| BurntSushi/toml | Wide TOML spec support | EASY - C impl exists |
| go-sqlite3 | Standard SQL interface | HARD - Need custom DB |
| Charmbracelet | Rich TUI features | EASY - Can remove |
| FUSE 3 | Linux filesystem API | HARD - Need kernel mod |
| ncurses | Terminal control | EASY - Use raw terminal |

### Recommendations

1. **Keep:** go-sqlite3, FUSE 3 (required infrastructure)
2. **Replace:** BurntSushi/toml (create cgo wrapper)
3. **Remove:** Charmbracelet TUI, ncurses (optional)
4. **Implement:** Math operations, TOML enhancements

---

## Closing

The Bereshit codebase demonstrates excellent dependency discipline:

- **340 Go files** with only **3 external packages**
- **60+ C files** with only **2 external libraries**
- **Heavy standard library usage** enables portability
- **Internal implementations** (libtrit, libomni) reduce external dependencies

For full self-hosting, the primary work is:
1. cgo wrapper for TOML (bridges Go to existing C implementation)
2. Complete math.c implementation
3. Future: Custom database (OmniDB) to replace SQLite

> *"For every house is builded by some man; but he that built all things is God."* — Hebrews 3:4 KJV

---

**Document Status**

| Field | Value |
|-------|-------|
| Key | B-void-planning-dependencies |
| Version | 1.0.0 |
| Status | Active |
| Created | 2026-02-01 |
| Updated | 2026-02-01 |
| Authors | Seanje Lenox-Wise (Architect), Nova Dawn (Implementation) |
