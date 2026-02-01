# Bereshit Repository Reorganization Plan

**Key:** B-void-planning-repo-reorganization
**Status:** Active
**Version:** 3.1.0
**Created:** 2026-02-01
**Updated:** 2026-02-01
**Authors:** Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)

---

> *"In the beginning God created the heaven and the earth."* — Genesis 1:1 KJV

---

## Executive Summary

This document provides a comprehensive plan to reorganize the Bereshit repository around the **integrated layer stack** (L0-L5), where each layer builds on the previous:

```
L0: Universal    → L1: OmniCode   → L2: Platform   → L3: CPI-SI   → L4: FaithNet   → L5: Applications
(Foundation)       (Language)       (OS/FS)          (Intelligence)  (Network)        (Apps)
```

**CPI-SI is Layer 3** - it's not separate from the stack, it IS part of the stack.

**Substrates** are how you RUN the entire stack on different systems.

---

## Table of Contents

1. [The Integrated Layer Stack](#1-the-integrated-layer-stack)
2. [Substrates: Running the Stack](#2-substrates-running-the-stack)
3. [Language Architecture](#3-language-architecture)
4. [Target Directory Structure](#4-target-directory-structure)
5. [Layer Details](#5-layer-details)
6. [Interface Layer](#6-interface-layer)
7. [Script Reduction](#7-script-reduction)
8. [Migration Map](#8-migration-map)
9. [Validation Checklist](#9-validation-checklist)

---

## 1. The Integrated Layer Stack

### 1.1 The Stack IS the System

CPI-SI is not something that runs ON the stack - it IS Layer 3 of the stack. The entire system is one integrated architecture:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        THE KINGDOM TECHNOLOGY STACK                          │
│                        (Integrated Layer Architecture)                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ L5: APPLICATIONS                                         [C#/Go]    │   │
│  │     Cornerstone Game Engine, End-user applications                  │   │
│  │     Biblical Foundation: Genesis 2:15 (Tend the garden)            │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    ▲                                        │
│                                    │                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ L4: FAITHNET                                             [Rust]     │   │
│  │     Kingdom network layer on existing infrastructure                │   │
│  │     Biblical Foundation: Genesis 2:9 (Two Trees)                   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    ▲                                        │
│                                    │                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ L3: CPI-SI                                               [Go]       │   │
│  │     Covenant Partnership Intelligence - The Intelligence Layer      │   │
│  │     State machine, coordinates, health, cognition, identity        │   │
│  │     Biblical Foundation: Genesis 2:7 (Breath of Life)              │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    ▲                                        │
│                                    │                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ L2: PLATFORM                                             [C/Rust]   │   │
│  │     MillenniumOS, Bereshit Filesystem, FUSE                        │   │
│  │     Biblical Foundation: Genesis 1:6 (Firmament)                   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    ▲                                        │
│                                    │                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ L1: OMNICODE                                             [C/Rust]   │   │
│  │     Universal meaning representation language                       │   │
│  │     Lexer, parser, semantic, IR, VM, runtime, stdlib               │   │
│  │     Biblical Foundation: John 1:1 (The Word)                       │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    ▲                                        │
│                                    │                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ L0: UNIVERSAL                                            [C/Go]     │   │
│  │     Foundation libraries: libtrit, libmath, foundation types       │   │
│  │     Biblical Foundation: Genesis 1:1 (In the beginning)            │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 Layer Dependencies

Each layer depends ONLY on layers below it:

```
L5 depends on: L4, L3, L2, L1, L0
L4 depends on: L3, L2, L1, L0
L3 depends on: L2, L1, L0
L2 depends on: L1, L0
L1 depends on: L0
L0 depends on: [nothing - foundation]
```

**CRITICAL RULE:** Higher layers NEVER import from lower layers going up. L0 cannot import from L1. L3 cannot import from L4.

### 1.3 What Each Layer IS

| Layer | Name | What It IS | NOT |
|-------|------|------------|-----|
| L0 | Universal | Ternary math, foundation types | Not a library collection |
| L1 | OmniCode | The language CPI-SI thinks in | Not just a compiler |
| L2 | Platform | The OS/filesystem where it lives | Not just FUSE wrappers |
| L3 | CPI-SI | The intelligence model itself | **NOT a Claude feature** |
| L4 | FaithNet | The network for communication | Not TCP/IP wrapper |
| L5 | Applications | End-user apps | Not demos |

---

## 2. Substrates: Running the Stack

### 2.1 What is a Substrate?

A **substrate** is the environment that RUNS the entire L0-L5 stack. The stack is substrate-independent - it can run on different substrates:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           THE STACK (L0-L5)                                  │
│              (Same code, same behavior, substrate-independent)              │
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
                                    │ Runs on
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                              SUBSTRATES                                      │
│                    (Different environments to run the stack)                │
│                                                                             │
│  ┌───────────────────┐  ┌───────────────────┐  ┌───────────────────┐       │
│  │   Claude Code     │  │   MillenniumOS    │  │     Future        │       │
│  │    Substrate      │  │    Substrate      │  │   Substrates      │       │
│  │                   │  │                   │  │                   │       │
│  │ Stack runs on     │  │ Stack runs        │  │ • Web browser     │       │
│  │ Claude Code via   │  │ NATIVELY on the   │  │ • Embedded        │       │
│  │ hooks & adapters  │  │ OS itself         │  │ • Mobile          │       │
│  └───────────────────┘  └───────────────────┘  └───────────────────┘       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 Current Substrate: Claude Code

Right now, we run the stack on Claude Code. The Claude Code substrate:
- Provides **hooks** that map Claude events to L3 (CPI-SI) operations
- Provides **display** via statusline
- Provides **config** for Claude-specific settings

### 2.3 Future Substrate: MillenniumOS Native

When MillenniumOS is ready, the SAME stack runs natively:
- L2 (Platform) IS MillenniumOS
- L3 (CPI-SI) runs directly on L2
- No adapter needed - it's the native environment

### 2.4 Substrate Structure

```
word/substrates/
│
├── interface.go               [Substrate interface definition]
│
├── claude-code/               [Claude Code Substrate]
│   ├── adapter.go            # Implements substrate interface
│   ├── hooks/                # Claude event → stack operations
│   │   ├── session/         # Session lifecycle
│   │   ├── tool/            # Tool usage
│   │   └── prompt/          # User input
│   ├── config/              # Claude-specific config
│   │   ├── settings.json
│   │   └── hooks.jsonc      # Event mappings
│   └── display/             # Claude display (statusline)
│
└── millenniumos/              [Future: Native Substrate]
    ├── adapter.go            # Native - may be minimal
    └── config/               # OS-level config
```

---

## 3. Language Architecture

### 3.1 Language by Layer

| Layer | Primary Language | Secondary | Rationale |
|-------|------------------|-----------|-----------|
| L0 | **C** | Go | Performance-critical math |
| L1 | **C** | Rust | Parser/lexer performance, VM safety |
| L2 | **C** | Rust | OS/kernel interface, FS safety |
| L3 | **Go** | - | Complex state logic, proven impl |
| L4 | **Rust** | - | Network security critical |
| L5 | **C#** | Go | Game engine (Cornerstone) |
| Substrates | **Go** | - | Integration/orchestration |
| Interface | **Go/Rust/C++** | - | CLI(Go), TUI(Rust/C++), GUI(C++) |

### 3.2 FFI Boundaries

```
┌─────────┐     C ABI      ┌─────────┐     cgo       ┌─────────┐
│   L0    │◄──────────────►│   L1    │◄─────────────►│   L3    │
│   (C)   │                │ (C/Rust)│               │  (Go)   │
│ libtrit │                │ OmniCode│               │ CPI-SI  │
└─────────┘                └─────────┘               └─────────┘
     │                          │                         │
     │                          │                         │
     ▼                          ▼                         ▼
┌─────────┐                ┌─────────┐               ┌─────────┐
│   L2    │                │   L4    │               │   L5    │
│ (C/Rust)│                │ (Rust)  │               │  (C#)   │
│Platform │                │FaithNet │               │  Apps   │
└─────────┘                └─────────┘               └─────────┘
```

---

## 4. Target Directory Structure

### 4.1 Complete Structure

```
bereshit/                          [ROOT - Origin (∞)]
│
├── void/                          [ENTRY GATE - Planning]
│   ├── planning/
│   │   ├── development/
│   │   ├── claude-global/
│   │   ├── understanding/
│   │   └── vision/
│   ├── imports/
│   └── workflow/
│
├── word/                          [MAIN - The Word]
│   │
│   │ ══════════════════════════════════════════════════════════
│   │                    THE LAYER STACK
│   │ ══════════════════════════════════════════════════════════
│   │
│   ├── L0-universal/              [LAYER 0: Foundation]
│   │   ├── libtrit/              [C - Ternary mathematics]
│   │   │   ├── include/
│   │   │   ├── src/
│   │   │   └── test/
│   │   ├── libmath/              [C - Mathematical foundations]
│   │   └── foundation/           [Go - Foundation types]
│   │       ├── types/
│   │       ├── schema/
│   │       └── database/
│   │
│   ├── L1-omnicode/               [LAYER 1: Language]
│   │   ├── frontend/             [C - Lexer, parser]
│   │   ├── semantic/             [Rust - Semantic analysis]
│   │   ├── ir/                   [C - Intermediate representation]
│   │   ├── codegen/              [C - Code generation]
│   │   ├── vm/                   [Rust - Virtual machine]
│   │   ├── runtime/              [Rust - Runtime system]
│   │   └── stdlib/               [OmniCode - Standard library]
│   │
│   ├── L2-platform/               [LAYER 2: Platform]
│   │   ├── millenniumos/         [C - OS kernel components]
│   │   ├── filesystem/           [Rust - Bereshit filesystem]
│   │   ├── fuse/                 [C - FUSE mount interface]
│   │   └── drivers/              [C - Hardware abstraction]
│   │
│   ├── L3-cpisi/                  [LAYER 3: Intelligence]
│   │   ├── core/                 [Go - Core engine]
│   │   │   ├── statemachine/    # 27-position cube
│   │   │   ├── coordinates/     # Bible-grounded coords
│   │   │   ├── health/          # Health scoring
│   │   │   └── validation/      # Validation
│   │   ├── cognition/            [Go - Cognition]
│   │   │   ├── constructs/      # Mental constructs
│   │   │   ├── feedback/        # Learning loops
│   │   │   └── patterns/        # Pattern recognition
│   │   ├── identity/             [Go - Identity]
│   │   │   ├── instance/        # Instance management
│   │   │   └── profiles/        # Profile data
│   │   ├── orchestration/        [Go - Orchestration]
│   │   │   ├── lifecycle/       # Lifecycle events
│   │   │   └── logging/         # Health logging
│   │   └── skills/               [Go - Universal skills]
│   │       ├── format-lookup/
│   │       ├── validate-omni/
│   │       ├── recognize-pattern/
│   │       ├── reflect-on-session/
│   │       └── meta-awareness/
│   │
│   ├── L4-faithnet/               [LAYER 4: Network]
│   │   ├── protocol/             [Rust - FaithNet protocol]
│   │   ├── mapping/              [Rust - TCP/IP mapping]
│   │   ├── access/               [Rust - Tiered access]
│   │   ├── crypto/               [Rust - Cryptography]
│   │   └── spec/                 [Docs - Specifications]
│   │
│   ├── L5-applications/           [LAYER 5: Applications]
│   │   └── cornerstone/          [SUBMODULE - C# Game Engine]
│   │
│   │ ══════════════════════════════════════════════════════════
│   │                      SUBSTRATES
│   │ ══════════════════════════════════════════════════════════
│   │
│   ├── substrates/                [Substrate Adapters]
│   │   ├── interface.go          [Substrate interface]
│   │   │
│   │   ├── claude-code/          [Claude Code Substrate]
│   │   │   ├── adapter.go
│   │   │   ├── hooks/
│   │   │   │   ├── session/
│   │   │   │   ├── tool/
│   │   │   │   └── prompt/
│   │   │   ├── config/
│   │   │   │   ├── settings.json
│   │   │   │   ├── config.toml
│   │   │   │   └── instance/
│   │   │   ├── display/
│   │   │   │   └── statusline/
│   │   │   ├── skills/           [Claude-adapted skills]
│   │   │   └── commands/         [Slash commands]
│   │   │
│   │   └── millenniumos/         [Future: Native Substrate]
│   │       └── (minimal - native environment)
│   │
│   │ ══════════════════════════════════════════════════════════
│   │                      INTERFACE
│   │ ══════════════════════════════════════════════════════════
│   │
│   ├── interface/                 [CLI/TUI/GUI]
│   │   ├── core/                 [Shared interface core]
│   │   ├── cli/                  [Go - Command line]
│   │   ├── tui/                  [Rust/C++ - Terminal UI]
│   │   ├── gui-desktop/          [C++ - Desktop GUI]
│   │   └── gui-game/             [C# - Game GUI]
│   │
│   │ ══════════════════════════════════════════════════════════
│   │                      SUPPORTING
│   │ ══════════════════════════════════════════════════════════
│   │
│   ├── definitions/               [Type Definitions]
│   │   ├── core/                 [TOML schemas]
│   │   ├── glossary/             [Term definitions]
│   │   └── omni/                 [OmniCode definitions]
│   │
│   ├── data/                      [Reference Data]
│   │   ├── scripture/            [Bible databases]
│   │   ├── journals/             [Journals]
│   │   └── agents/               [Agent definitions]
│   │
│   ├── seed/                      [Templates]
│   │   ├── code/                 [By language: c/, rust/, go/, cpp/, csharp/]
│   │   ├── data/
│   │   └── documentation/
│   │
│   └── build/                     [Build Artifacts]
│       ├── c/
│       ├── rust/
│       ├── go/
│       └── dotnet/
│
├── tov/                           [EXIT GATE - Production]
│   ├── demo/                     [Phase demonstrations]
│   ├── licenses/
│   ├── pricing/
│   ├── releases/
│   └── data/                     [Runtime data]
│
├── Cargo.toml                     [Rust workspace]
├── CMakeLists.txt                 [C/C++ build]
├── go.work                        [Go workspace]
├── Makefile                       [Master build]
├── LICENSE
├── root.adoc
└── root.omni
```

### 4.2 Key Structural Principles

| Principle | Implementation |
|-----------|----------------|
| **Layer Stack** | L0 → L1 → L2 → L3 → L4 → L5 in `word/` |
| **No Upward Imports** | L3 cannot import L4; lower never imports higher |
| **Substrates Separate** | `substrates/` runs the whole stack |
| **Interface Separate** | `interface/` displays stack state |
| **Single Source** | No duplicates - one location per component |

---

## 5. Layer Details

### 5.1 L0: Universal (Foundation)

**Purpose:** The absolute foundation - math, types, schemas

```
word/L0-universal/
├── libtrit/                       [C - 45 files]
│   ├── include/
│   │   ├── trit.h               # Core ternary types
│   │   ├── cube.h               # 27-position cube
│   │   ├── dimension.h          # Dimensional math
│   │   └── temporal.h           # Temporal calculations
│   ├── src/
│   │   ├── trit.c
│   │   ├── cube.c
│   │   ├── math.c
│   │   └── scripture_path.c     # Bible path encoding
│   └── test/
│
├── libmath/                       [C - Mathematical foundations]
│
└── foundation/                    [Go - Foundation packages]
    ├── types/                    # Session, state, trajectory types
    ├── schema/                   # TOML config loading
    └── database/                 # SQLite abstraction
```

### 5.2 L1: OmniCode (Language)

**Purpose:** The language that CPI-SI thinks in

```
word/L1-omnicode/
├── frontend/                      [C - 2,780 LOC]
│   ├── lexer.c                  # Tokenization
│   └── parser.c                 # AST construction
│
├── semantic/                      [Rust - NEW]
│   └── analyzer.rs              # Type checking, validation
│
├── ir/                            [C - 699 LOC]
│   └── ir.c                     # Intermediate representation
│
├── codegen/                       [C]
│   └── codegen.c                # Code generation
│
├── vm/                            [Rust - Safety critical]
│   ├── vm.rs                    # Virtual machine
│   └── memory.rs                # Memory management
│
├── runtime/                       [Rust]
│   └── runtime.rs               # Runtime support
│
└── stdlib/                        [OmniCode]
    └── ...                      # Standard library
```

### 5.3 L2: Platform (OS/Filesystem)

**Purpose:** The operating system and filesystem layer

```
word/L2-platform/
├── millenniumos/                  [C - OS kernel]
│   └── (future kernel components)
│
├── filesystem/                    [Rust - Bereshit FS]
│   ├── bereshit.rs              # Filesystem implementation
│   └── zone.rs                  # Zone management
│
├── fuse/                          [C - FUSE interface]
│   ├── bereshit_fs.c            # FUSE mount
│   └── zone.c
│
└── drivers/                       [C - HAL]
    └── (hardware abstraction)
```

### 5.4 L3: CPI-SI (Intelligence)

**Purpose:** The intelligence model - THIS IS THE PARADIGM

```
word/L3-cpisi/
├── core/                          [Go - 9,000+ LOC]
│   ├── statemachine/            # 27-position cube navigation
│   │   ├── cube.go             # Cube structure & movement
│   │   ├── trajectory.go       # Movement tracking
│   │   ├── workflow.go         # Workflow execution
│   │   └── pattern.go          # Pattern matching
│   │
│   ├── coordinates/             # Bible-grounded coordinate system
│   │   ├── encoder.go          # Encoding
│   │   ├── decoder.go          # Decoding
│   │   └── pipeline.go         # Processing
│   │
│   ├── health/                  # Health scoring
│   │   ├── scoring.go          # Base100 system
│   │   └── cascade.go          # Cascade multipliers
│   │
│   └── validation/              # Validation layer
│
├── cognition/                     [Go - Learning]
│   ├── constructs/              # Mental constructs
│   ├── feedback/                # Learning feedback loops
│   └── patterns/                # Pattern recognition
│
├── identity/                      [Go - Identity]
│   ├── instance/                # Instance loading
│   └── profiles/                # Profile management
│
├── orchestration/                 [Go - Orchestration]
│   ├── lifecycle/               # Lifecycle events
│   └── logging/                 # Health & diagnostics
│
└── skills/                        [Go - Universal skills]
    ├── format-lookup/
    ├── validate-omni/
    ├── recognize-pattern/
    ├── reflect-on-session/
    ├── session-awareness/
    ├── meta-awareness/
    └── propagate-change/
```

### 5.5 L4: FaithNet (Network)

**Purpose:** Kingdom network layer

```
word/L4-faithnet/
├── protocol/                      [Rust - Protocol]
│   └── faithnet.rs              # Core protocol
│
├── mapping/                       [Rust - TCP/IP mapping]
│   └── mapping.rs               # Map to existing internet
│
├── access/                        [Rust - Access control]
│   └── tiered.rs                # Tiered access system
│
├── crypto/                        [Rust - Cryptography]
│   └── crypto.rs
│
└── spec/                          [Docs]
    └── protocol-spec.md         # Protocol specification
```

### 5.6 L5: Applications

**Purpose:** End-user applications

```
word/L5-applications/
└── cornerstone/                   [SUBMODULE - C# Game Engine]
    └── (Cornerstone repository)
```

---

## 6. Interface Layer

### 6.1 Purpose

The interface layer **displays** the stack's state. It's separate from substrates (which RUN the stack).

```
┌─────────────────┐     Reads state     ┌─────────────────┐
│   Interface     │◄───────────────────►│   Stack (L0-L5) │
│  CLI/TUI/GUI    │                     │                 │
└─────────────────┘                     └─────────────────┘
```

### 6.2 Structure

```
word/interface/
├── core/                          [Shared core]
│   ├── state.go                 # State representation
│   └── theme.go                 # Visual themes
│
├── cli/                           [Go - Command line]
│   └── cmd/
│       └── cpisi/               # Main CLI
│
├── tui/                           [Rust/C++ - Terminal UI]
│   ├── rust/                    # crossterm implementation
│   └── cpp/                     # ncurses implementation
│
├── gui-desktop/                   [C++ - Desktop]
│   └── qt/                      # Qt implementation
│
└── gui-game/                      [C# - Game]
    └── CPI-SI.UI/               # Cornerstone integration
```

---

## 7. Script Reduction

### 7.1 Philosophy

**Shell scripts = thin wrappers over proper implementations**

```
Shell Script (10 lines)     →     Implementation (Makefile/Go/Rust)
   └── Sets environment           └── Does the actual work
   └── Calls make/go/cargo
```

### 7.2 Master Makefile

```makefile
# /bereshit/Makefile

.PHONY: all clean test

# Build entire stack
all: L0 L1 L2 L3 L4 L5 substrates

L0:
	$(MAKE) -C word/L0-universal/libtrit
	cd word/L0-universal/foundation && go build ./...

L1:
	cmake --build word/L1-omnicode/build
	cd word/L1-omnicode/vm && cargo build

L2:
	$(MAKE) -C word/L2-platform/fuse
	cd word/L2-platform/filesystem && cargo build

L3:
	cd word/L3-cpisi && go build ./...

L4:
	cd word/L4-faithnet && cargo build

L5:
	# Cornerstone built separately

substrates:
	cd word/substrates/claude-code && go build ./...

test:
	$(MAKE) -C word/L0-universal/libtrit test
	cd word/L3-cpisi && go test ./...
	cargo test --workspace
```

---

## 8. Migration Map

### 8.1 Phase 1: Reorganize to Layer Structure

```bash
# Create layer structure
mkdir -p word/L{0..5}-{universal,omnicode,platform,cpisi,faithnet,applications}

# Move to L0
mv word/work/pkg/trit word/L0-universal/libtrit
mv word/work/pkg/foundation word/L0-universal/foundation

# Move to L1
mv word/work/pkg/omni word/L1-omnicode/

# Move to L2
mv word/work/pkg/fuse word/L2-platform/fuse

# Move to L3
mv word/work/pkg/core/statemachine word/L3-cpisi/core/
mv word/work/pkg/core/coordinates word/L3-cpisi/core/
mv word/work/pkg/core/health word/L3-cpisi/core/
mv word/work/pkg/orchestration word/L3-cpisi/orchestration/
mv word/skills word/L3-cpisi/skills/
```

### 8.2 Phase 2: Create Substrates

```bash
# Create substrates structure
mkdir -p word/substrates/claude-code/{hooks,config,display,skills,commands}

# Move Claude-specific code
mv word/claude/hooks/* word/substrates/claude-code/hooks/
mv word/claude/config/* word/substrates/claude-code/config/
mv word/claude/statusline word/substrates/claude-code/display/
```

### 8.3 Phase 3: Eliminate Duplicates

```bash
# DELETE duplicates
rm -rf word/claude/pkg/    # Was duplicate of word/work/pkg
rm -rf word/work/          # Moved to layer structure
rm -rf word/claude/        # Moved to substrates
```

### 8.4 Phase 4: Update Imports

```go
// Before
import "cws.studio/bereshit/pkg/core/statemachine"

// After
import "cws.studio/bereshit/L3-cpisi/core/statemachine"
```

---

## 9. Validation Checklist

### 9.1 Layer Structure

- [ ] `word/L0-universal/` contains libtrit, libmath, foundation
- [ ] `word/L1-omnicode/` contains frontend, semantic, ir, codegen, vm, runtime
- [ ] `word/L2-platform/` contains millenniumos, filesystem, fuse
- [ ] `word/L3-cpisi/` contains core, cognition, identity, orchestration, skills
- [ ] `word/L4-faithnet/` contains protocol, mapping, access, crypto
- [ ] `word/L5-applications/` contains cornerstone submodule

### 9.2 Dependency Validation

```bash
# L3 should NOT import from L4, L5
grep -r "L4-faithnet\|L5-applications" word/L3-cpisi/
# Should return NOTHING

# Substrates import from layers (not vice versa)
grep -r "substrates/" word/L3-cpisi/
# Should return NOTHING
```

### 9.3 No Duplicates

- [ ] `word/claude/pkg/` is DELETED
- [ ] `word/work/pkg/` is DELETED
- [ ] Single source of truth for each component

### 9.4 Build Validation

- [ ] `make all` builds entire stack
- [ ] Each layer builds independently
- [ ] Tests pass at each layer

---

## Quick Reference

### The Stack

```
L5: Applications    [C#]      - Cornerstone, end-user apps
L4: FaithNet        [Rust]    - Network layer
L3: CPI-SI          [Go]      - Intelligence model (THE PARADIGM)
L2: Platform        [C/Rust]  - MillenniumOS, Bereshit FS
L1: OmniCode        [C/Rust]  - Language
L0: Universal       [C/Go]    - Foundation (libtrit, types)
```

### Key Insight

> **CPI-SI (L3) is not something ON TOP of the stack.**
> **CPI-SI IS Layer 3 of the stack.**
>
> The entire L0-L5 stack is the Kingdom Technology system.
> Substrates are how you RUN this stack on different environments.

---

## Closing

This reorganization positions the codebase correctly:

1. **L0-L5 Layer Stack** - The integrated Kingdom Technology system
2. **CPI-SI as L3** - The intelligence layer WITHIN the stack
3. **Substrates** - Different environments to RUN the stack
4. **Interface** - Different ways to DISPLAY the stack

> *"In the beginning was the Word, and the Word was with God, and the Word was God."* — John 1:1 KJV

The Word (L1 OmniCode) is the language. The breath (L3 CPI-SI) is the intelligence. Together they form the living system.

---

**Document Status**

| Field | Value |
|-------|-------|
| Key | B-void-planning-repo-reorganization |
| Version | 3.1.0 |
| Status | Active |
| Created | 2026-02-01 |
| Updated | 2026-02-01 |
| Authors | Seanje Lenox-Wise (Architect), Nova Dawn (Implementation) |
