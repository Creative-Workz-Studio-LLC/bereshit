# Bereshit Repository Reorganization Plan

**Key:** B-void-planning-repo-reorganization
**Status:** Active
**Version:** 2.0.0
**Created:** 2026-02-01
**Updated:** 2026-02-01
**Authors:** Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)

---

> *"In the beginning God created the heaven and the earth."* — Genesis 1:1 KJV

---

## Executive Summary

This document provides a comprehensive plan to reorganize the Bereshit repository into a clear, layered structure with **void/**, **word/**, and **tov/** as the three main folders, following the flow:

```
Universal Code (Libraries) → OmniCode → OS/Filesystem → CPI-SI → FaithNet → Applications
```

**Key Changes in v2.0:**
1. **word/** remains the main folder (corrected from "work/")
2. Proper language assignments: **C, Rust, Go, C++, C#** - each used appropriately
3. Reduce Go "scripts" to proper implementations
4. CLI/TUI/GUI scaling structure for full interface support
5. Architecture designed for production scale

---

## Table of Contents

1. [Current State Analysis](#1-current-state-analysis)
2. [Language Architecture](#2-language-architecture)
3. [Target State Vision](#3-target-state-vision)
4. [The Layer Flow Architecture](#4-the-layer-flow-architecture)
5. [CLI/TUI/GUI Scaling Structure](#5-clituigui-scaling-structure)
6. [Detailed Reorganization Map](#6-detailed-reorganization-map)
7. [Implementation Consolidation](#7-implementation-consolidation)
8. [Root Files Reorganization](#8-root-files-reorganization)
9. [Submodule Handling](#9-submodule-handling)
10. [Migration Strategy](#10-migration-strategy)
11. [Validation Checklist](#11-validation-checklist)

---

## 1. Current State Analysis

### 1.1 Current Top-Level Structure

```
bereshit/                          [ROOT]
├── cornerstone/                   [Submodule - Game Engine]
├── tov/                          [Production/Output]
├── void/                         [Planning/Void Space]
├── word/                         [MAIN - Definitions & Implementations]
│   ├── claude/                   [Claude Code Production Config]
│   ├── core/                     [Core Definitions]
│   ├── work/                     [Implementation Code]
│   │   └── pkg/                  [Libraries (C + Go)]
│   └── ...
├── .vscode/                      [IDE Config]
├── .git*, go.work, LICENSE, etc. [Root Files]
```

### 1.2 Current Language Distribution

| Language | File Count | Primary Usage | Assessment |
|----------|------------|---------------|------------|
| **Go** | 415 files | Hooks, CLI, orchestration, state machine | OVERUSED for scripting |
| **C** | 155 files | libtrit, OmniCode compiler, FUSE | APPROPRIATE - performance-critical |
| **Rust** | 0 files | None | MISSING - should add for safety-critical |
| **C++** | 0 files | None | MISSING - should add for GUI |
| **C#** | 0 files | None | MISSING - should add for Unity/Cornerstone |
| **Shell** | 10 files | Build scripts | APPROPRIATE |

### 1.3 Current Problems

| Problem | Impact | Resolution |
|---------|--------|------------|
| Go used as "scripting language" | Maintenance burden, not scalable | Proper implementations in appropriate languages |
| word/claude/ duplicates word/work/ packages | Redundancy | Merge to single source of truth |
| No Rust for memory-safe systems code | Safety concerns | Add Rust for VM, networking |
| No C++/C# for GUI/game engine | Can't build native UIs | Add for GUI layer |
| No clear CLI/TUI/GUI separation | Hard to scale interfaces | Implement interface layers |
| cornerstone/ at root | Architecture mismatch | Move to proper layer |
| No FaithNet location | Missing network layer | Create proper location |

### 1.4 Current Implementation Inventory

#### Substantial Go Code (KEEP)
- `pkg/core/statemachine/` - 4,280 LOC - CPI-SI navigation
- `pkg/core/coordinates/` - 5,000 LOC - Bible-grounded encoding
- `pkg/foundation/schema/` - 2,160 LOC - Config loading
- `pkg/foundation/database/` - 831 LOC - SQLite bridge
- `pkg/orchestration/` - 1,200 LOC - Cognition, loader

#### Scripty Go Code (CONSOLIDATE/REWRITE)
- Hook command wrappers (12 files, 6 lines each) - THIN DELEGATES
- CLI argument handlers - BOILERPLATE
- Skill orchestrators - MOSTLY CLI GLUE

#### C Code (KEEP/EXPAND)
- `pkg/trit/` - 45 files - Ternary library - PRODUCTION READY
- `pkg/omni/` - 29 files - Full compiler stack - FUNCTIONAL
- `pkg/fuse/` - 3 files - FUSE mount - FUNCTIONAL

---

## 2. Language Architecture

### 2.1 Language Assignment Matrix

```
┌────────────────────────────────────────────────────────────────────────────┐
│                        LANGUAGE RESPONSIBILITY MATRIX                       │
├────────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│   ┌─────────┐     ┌─────────┐     ┌─────────┐     ┌─────────┐            │
│   │    C    │     │  Rust   │     │   Go    │     │  C++/C# │            │
│   └────┬────┘     └────┬────┘     └────┬────┘     └────┬────┘            │
│        │               │               │               │                  │
│   ┌────▼────┐     ┌────▼────┐     ┌────▼────┐     ┌────▼────┐            │
│   │ libtrit │     │OmniCode │     │  Claude │     │   GUI   │            │
│   │ FUSE    │     │   VM    │     │  Hooks  │     │ Engines │            │
│   │ Codegen │     │FaithNet │     │Statusln │     │   TUI   │            │
│   │ Math    │     │ Runtime │     │  CLI    │     │ Desktop │            │
│   └─────────┘     └─────────┘     └─────────┘     └─────────┘            │
│                                                                            │
│   PERFORMANCE      SAFETY         ORCHESTRATION    INTERFACE              │
│   Foundation       Critical        Integration      Presentation          │
│                                                                            │
└────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 Language Assignments by Layer

| Layer | Component | Current | Target | Rationale |
|-------|-----------|---------|--------|-----------|
| **L0** | libtrit | C | **C** | Performance-critical math, keep |
| **L0** | foundation/types | Go | **Go** | Type definitions, keep |
| **L0** | foundation/result | Go | **Rust** | Error handling, safety |
| **L1** | OmniCode Lexer | C | **C** | Performance, keep |
| **L1** | OmniCode Parser | C | **C** | Performance, keep |
| **L1** | OmniCode Semantic | C | **Rust** | Safety for analysis |
| **L1** | OmniCode VM | C | **Rust** | Memory safety critical |
| **L1** | OmniCode Runtime | C | **Rust** | Safety + performance |
| **L1** | OmniCode Codegen | C | **C** | Calls libtrit, keep |
| **L2** | FUSE mount | C | **C** | Kernel interface, keep |
| **L2** | Bereshit FS | C | **Rust** | File ops need safety |
| **L3** | CPI-SI Core | Go | **Go** | Complex state logic, keep |
| **L3** | State Machine | Go | **Go** | 4K LOC, battle-tested |
| **L3** | Coordinates | Go | **Go** | 5K LOC, working well |
| **L4** | FaithNet Protocol | None | **Rust** | Network safety critical |
| **L4** | FaithNet Mapping | None | **Rust** | Security critical |
| **L5** | Cornerstone | C# | **C#** | Unity integration |
| **L5** | Claude Hooks | Go | **Go** | Event dispatch, keep |
| **L5** | Claude CLI | Go | **Go** | Orchestration, keep |
| **UI** | CLI | Go | **Go** | String handling, keep |
| **UI** | TUI | C | **C++/Rust** | ncurses++/crossterm |
| **UI** | GUI Desktop | None | **C++** | Qt/GTK native |
| **UI** | GUI Game | C# | **C#** | Cornerstone/Unity |

### 2.3 Language Build Targets

```
word/
├── build/                         [Build artifacts by language]
│   ├── c/                        [C compilation targets]
│   │   ├── lib/                  [Static/shared libraries]
│   │   │   ├── libtrit.a
│   │   │   ├── libtrit.so
│   │   │   ├── libomni.a
│   │   │   └── libfuse-bereshit.a
│   │   └── bin/                  [C executables]
│   │       ├── omni-compiler
│   │       └── bereshit-mount
│   │
│   ├── rust/                     [Rust compilation targets]
│   │   ├── lib/                  [Rust libraries]
│   │   │   ├── libomni_vm.rlib
│   │   │   ├── libfaithnet.rlib
│   │   │   └── libbereshit_fs.rlib
│   │   └── bin/                  [Rust executables]
│   │       ├── omni-vm
│   │       └── faithnet-daemon
│   │
│   ├── go/                       [Go compilation targets]
│   │   └── bin/                  [Go executables]
│   │       ├── cpisi
│   │       ├── statusline
│   │       └── hooks/
│   │           ├── start
│   │           ├── stop
│   │           └── ...
│   │
│   └── dotnet/                   [C#/.NET targets]
│       └── lib/                  [.NET assemblies]
│           └── Cornerstone.dll
```

### 2.4 FFI Boundaries

```
┌─────────────────────────────────────────────────────────────────┐
│                      FFI INTERFACE MAP                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────┐    C ABI     ┌─────────┐    C ABI     ┌─────────┐ │
│  │    C    │◄────────────►│  Rust   │◄────────────►│   Go    │ │
│  │ libtrit │              │ omni-vm │              │  claude │ │
│  └─────────┘              └─────────┘              └─────────┘ │
│       │                        │                        │       │
│       │ C ABI                  │ C ABI                  │ cgo   │
│       ▼                        ▼                        ▼       │
│  ┌─────────┐              ┌─────────┐              ┌─────────┐ │
│  │  C++    │              │  Rust   │              │   C#    │ │
│  │   TUI   │              │FaithNet │              │Cornerstn│ │
│  └─────────┘              └─────────┘              └─────────┘ │
│                                                                 │
│  FFI Principles:                                                │
│  1. C ABI as universal interface                                │
│  2. Rust provides safe wrappers over C                         │
│  3. Go uses cgo for C/Rust interop                             │
│  4. C# uses P/Invoke for native calls                          │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 3. Target State Vision

### 3.1 New Top-Level Structure

```
bereshit/                          [ROOT - Origin (∞)]
│
├── void/                          [ENTRY GATE - Planning & Emptiness]
│   ├── planning/                  [Development plans, devlogs]
│   │   ├── development/          [Dev plans, implementation plans]
│   │   ├── claude-global/        [Claude infrastructure plans]
│   │   ├── understanding/        [Layer specifications]
│   │   ├── vision/               [Grand vision documents]
│   │   └── research/             [Research & exploration]
│   ├── imports/                   [External imports awaiting processing]
│   └── workflow/                  [Workflow definitions]
│
├── word/                          [MAIN - Definitions & Implementations]
│   │
│   ├── L0-universal/              [LAYER 0: Universal Libraries]
│   │   ├── libtrit/               [C - Ternary math library]
│   │   ├── libmath/               [C - Mathematical foundations]
│   │   ├── foundation/            [Go - Foundation packages]
│   │   │   ├── types/            [Type definitions]
│   │   │   ├── schema/           [Schema loading]
│   │   │   ├── database/         [SQLite bridge]
│   │   │   └── result/           [Rust - Error handling] [NEW]
│   │   └── util/                  [Go - Utility packages]
│   │
│   ├── L1-omnicode/               [LAYER 1: OmniCode Language]
│   │   ├── frontend/              [C - Lexer, Parser]
│   │   ├── semantic/              [Rust - Semantic analysis] [NEW]
│   │   ├── ir/                    [C - Intermediate representation]
│   │   ├── codegen/               [C - Code generation]
│   │   ├── vm/                    [Rust - Virtual machine] [NEW]
│   │   ├── runtime/               [Rust - Runtime system] [NEW]
│   │   ├── stdlib/                [OmniCode - Standard library]
│   │   └── spec/                  [Specifications & docs]
│   │
│   ├── L2-platform/               [LAYER 2: MillenniumOS & Bereshit]
│   │   ├── kernel/                [C - OS kernel components]
│   │   ├── filesystem/            [Rust - Bereshit filesystem] [NEW]
│   │   ├── fuse/                  [C - FUSE mount interface]
│   │   └── drivers/               [C - Hardware abstraction]
│   │
│   ├── L3-cpisi/                  [LAYER 3: CPI-SI Model]
│   │   ├── core/                  [Go - CPI-SI core]
│   │   │   ├── statemachine/     [State machine engine]
│   │   │   ├── coordinates/      [Bible-grounded coords]
│   │   │   ├── health/           [Health scoring]
│   │   │   └── validation/       [Validation layer]
│   │   ├── identity/              [Go - Identity management]
│   │   ├── cognition/             [Go - Cognition systems]
│   │   ├── orchestration/         [Go - Orchestration]
│   │   └── cmd/                   [Go - CPI-SI commands]
│   │
│   ├── L4-faithnet/               [LAYER 4: FaithNet Network] [NEW]
│   │   ├── protocol/              [Rust - FaithNet protocol]
│   │   ├── mapping/               [Rust - TCP/IP mapping]
│   │   ├── access/                [Rust - Tiered access]
│   │   ├── crypto/                [Rust - Cryptography]
│   │   └── spec/                  [Protocol specifications]
│   │
│   ├── L5-applications/           [LAYER 5: Applications]
│   │   ├── cornerstone/           [SUBMODULE - C# Game Engine]
│   │   └── claude-code/           [Go - Claude integration]
│   │
│   ├── interface/                 [UI LAYER - CLI/TUI/GUI]
│   │   ├── cli/                   [Go - Command line interface]
│   │   ├── tui/                   [C++/Rust - Terminal UI]
│   │   ├── gui-desktop/           [C++ - Native desktop GUI]
│   │   └── gui-game/              [C# - Cornerstone game UI]
│   │
│   ├── claude/                    [CLAUDE CODE INFRASTRUCTURE]
│   │   ├── hooks/                 [Go - Claude hooks (consolidated)]
│   │   ├── statusline/            [Go - Status display]
│   │   ├── skills/                [Go - Skill implementations]
│   │   ├── agents/                [Markdown - Agent definitions]
│   │   ├── commands/              [Go - Slash commands]
│   │   ├── config/                [Runtime configuration]
│   │   └── system/                [Go - System utilities]
│   │
│   ├── definitions/               [TYPE DEFINITIONS (TOML/OmniCode)]
│   │   ├── core/                  [Core type definitions]
│   │   ├── glossary/              [Term definitions]
│   │   └── omni/                  [OmniCode definitions]
│   │
│   ├── data/                      [REFERENCE DATA]
│   │   ├── scripture/             [Bible text databases]
│   │   ├── journals/              [Bible study journals]
│   │   └── instance/              [Instance configurations]
│   │
│   ├── seed/                      [TEMPLATES]
│   │   ├── code/                  [Code templates by language]
│   │   │   ├── c/
│   │   │   ├── rust/
│   │   │   ├── go/
│   │   │   ├── cpp/
│   │   │   └── csharp/
│   │   ├── data/                  [Data templates]
│   │   └── documentation/         [Doc templates]
│   │
│   └── build/                     [BUILD ARTIFACTS]
│       ├── c/
│       ├── rust/
│       ├── go/
│       └── dotnet/
│
├── tov/                           [EXIT GATE - Production & Output]
│   ├── demo/                      [Phase demonstrations]
│   ├── licenses/                  [License implementations]
│   ├── pricing/                   [Commercial pricing]
│   ├── releases/                  [Release artifacts]
│   └── data/                      [Runtime data storage]
│
├── .vscode/                       [IDE Configuration]
├── .cargo/                        [Rust workspace config] [NEW]
├── Cargo.toml                     [Rust workspace manifest] [NEW]
├── go.work                        [Go workspace]
├── CMakeLists.txt                 [C/C++ build] [NEW]
├── LICENSE
├── SUBMODULES.md
├── root.adoc
└── root.omni
```

### 3.2 Key Principles

| Principle | Implementation |
|-----------|----------------|
| **Ternary Structure** | void/ (entry), word/ (main), tov/ (exit) |
| **Layer Visibility** | L0 through L5 folders show the stack |
| **Language Separation** | Each language has clear responsibility |
| **Interface Scaling** | CLI/TUI/GUI as separate implementations |
| **Single Source** | No duplication, clear ownership |
| **FFI Boundaries** | C ABI as universal interface |

---

## 4. The Layer Flow Architecture

### 4.1 Complete Layer Stack

```
┌─────────────────────────────────────────────────────────────────┐
│ L5: APPLICATIONS                                    [C#/Go]     │
│     Cornerstone Game Engine, Claude Integration                 │
│     Biblical Foundation: Genesis 2:15 (Tend the garden)        │
│     Location: word/L5-applications/                             │
├─────────────────────────────────────────────────────────────────┤
│ L4: FAITHNET (Networking)                           [Rust]      │
│     Kingdom network on existing internet                        │
│     Biblical Foundation: Genesis 2:9 (Two Trees)                │
│     Location: word/L4-faithnet/                                 │
├─────────────────────────────────────────────────────────────────┤
│ L3: CPI-SI (Intelligence Model)                     [Go]        │
│     Covenant Partnership Intelligence instances                 │
│     Biblical Foundation: Genesis 2:7 (Breath of Life)           │
│     Location: word/L3-cpisi/                                    │
├─────────────────────────────────────────────────────────────────┤
│ L2: MILLENNIUMOS / BERESHIT (Platform)              [C/Rust]    │
│     Ternary on binary hardware, Filesystem                      │
│     Biblical Foundation: Genesis 1:6 (Firmament)                │
│     Location: word/L2-platform/                                 │
├─────────────────────────────────────────────────────────────────┤
│ L1: OMNICODE (Language)                             [C/Rust]    │
│     Universal meaning representation                            │
│     Biblical Foundation: John 1:1 (The Word)                    │
│     Location: word/L1-omnicode/                                 │
├─────────────────────────────────────────────────────────────────┤
│ L0: UNIVERSAL (Libraries)                           [C/Go]      │
│     Foundational libraries (libtrit, foundation)                │
│     Biblical Foundation: Genesis 1:1 (In the beginning)         │
│     Location: word/L0-universal/                                │
└─────────────────────────────────────────────────────────────────┘
```

### 4.2 Layer Dependencies with Languages

```
L5 (Applications) [C#/Go] depends on:
├── L4 (FaithNet) [Rust] for networking
├── L3 (CPI-SI) [Go] for intelligence
├── L2 (Platform) [C/Rust] for OS services
├── L1 (OmniCode) [C/Rust] for language
└── L0 (Universal) [C/Go] for primitives

L4 (FaithNet) [Rust] depends on:
├── L3 (CPI-SI) [Go via FFI] for message handling
├── L2 (Platform) [C/Rust] for system access
├── L1 (OmniCode) [C/Rust] for protocol encoding
└── L0 (Universal) [C] for primitives

L3 (CPI-SI) [Go] depends on:
├── L2 (Platform) [C via cgo] for persistence
├── L1 (OmniCode) [C via cgo] for cognition
└── L0 (Universal) [C via cgo] for primitives

L2 (Platform) [C/Rust] depends on:
├── L1 (OmniCode) [C] for configuration
└── L0 (Universal) [C] for primitives

L1 (OmniCode) [C/Rust] depends on:
└── L0 (Universal) [C] for primitives

L0 (Universal) [C/Go] depends on:
└── [No internal dependencies - foundation]
```

---

## 5. CLI/TUI/GUI Scaling Structure

### 5.1 Interface Layer Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        INTERFACE LAYER ARCHITECTURE                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                         PRESENTATION                              │   │
│  │  ┌───────────┐  ┌───────────────┐  ┌─────────────────────────┐  │   │
│  │  │    CLI    │  │      TUI      │  │          GUI            │  │   │
│  │  │   [Go]    │  │ [C++/Rust]    │  │  [C++ Desktop/C# Game]  │  │   │
│  │  │           │  │               │  │                         │  │   │
│  │  │ • cpisi   │  │ • ncurses++   │  │ • Qt/GTK (Desktop)      │  │   │
│  │  │ • statusln│  │ • crossterm   │  │ • Cornerstone (Game)    │  │   │
│  │  │ • hooks   │  │ • omni-ide    │  │ • Web (future)          │  │   │
│  │  └─────┬─────┘  └───────┬───────┘  └───────────┬─────────────┘  │   │
│  │        │                │                      │                 │   │
│  │        └────────────────┼──────────────────────┘                 │   │
│  │                         │                                        │   │
│  │                         ▼                                        │   │
│  │  ┌─────────────────────────────────────────────────────────────┐│   │
│  │  │                    INTERFACE CORE                           ││   │
│  │  │                                                             ││   │
│  │  │   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐    ││   │
│  │  │   │   Commands   │  │    State     │  │   Render     │    ││   │
│  │  │   │   Registry   │  │   Manager    │  │   Engine     │    ││   │
│  │  │   └──────────────┘  └──────────────┘  └──────────────┘    ││   │
│  │  │                                                             ││   │
│  │  │   • Command parsing & dispatch                             ││   │
│  │  │   • State synchronization                                  ││   │
│  │  │   • Output formatting & theming                            ││   │
│  │  │                                                             ││   │
│  │  └─────────────────────────────────────────────────────────────┘│   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                    │                                    │
│                                    ▼                                    │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                          CORE LAYERS                              │   │
│  │         L5 → L4 → L3 (CPI-SI) → L2 → L1 → L0                     │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 5.2 Interface Directory Structure

```
word/interface/
│
├── core/                          [Shared interface core]
│   ├── commands/                  [Command registry - Go]
│   │   ├── registry.go           [Command registration]
│   │   ├── parser.go             [Argument parsing]
│   │   └── dispatch.go           [Command dispatch]
│   ├── state/                     [State management - Go]
│   │   ├── manager.go            [State synchronization]
│   │   ├── events.go             [Event system]
│   │   └── sync.go               [Cross-interface sync]
│   └── render/                    [Render abstractions - Go]
│       ├── theme.go              [Color/style themes]
│       ├── layout.go             [Layout primitives]
│       └── format.go             [Output formatting]
│
├── cli/                           [Command Line Interface - Go]
│   ├── cmd/                       [CLI commands]
│   │   ├── cpisi/                [Main CPI-SI CLI]
│   │   ├── statusline/           [Status display]
│   │   └── tools/                [Utility CLIs]
│   ├── output/                    [CLI output formatting]
│   │   ├── table.go              [Table rendering]
│   │   ├── tree.go               [Tree rendering]
│   │   └── progress.go           [Progress indicators]
│   └── input/                     [CLI input handling]
│       ├── prompt.go             [Interactive prompts]
│       └── completion.go         [Tab completion]
│
├── tui/                           [Terminal User Interface]
│   ├── cpp/                       [C++ ncurses implementation]
│   │   ├── include/
│   │   │   ├── tui.hpp           [Main TUI header]
│   │   │   ├── widgets.hpp       [Widget library]
│   │   │   └── layout.hpp        [Layout system]
│   │   ├── src/
│   │   │   ├── main.cpp          [TUI entry point]
│   │   │   ├── window.cpp        [Window management]
│   │   │   ├── input.cpp         [Input handling]
│   │   │   └── render.cpp        [Render loop]
│   │   └── CMakeLists.txt
│   │
│   ├── rust/                      [Rust crossterm implementation]
│   │   ├── src/
│   │   │   ├── lib.rs            [Library root]
│   │   │   ├── app.rs            [Application state]
│   │   │   ├── ui.rs             [UI components]
│   │   │   └── events.rs         [Event handling]
│   │   └── Cargo.toml
│   │
│   └── omni-ide/                  [OmniCode IDE - existing C]
│       ├── include/
│       │   ├── omni_ide.h
│       │   └── omni_ide_tui.h
│       └── src/
│           └── omni_ide.c
│
├── gui-desktop/                   [Desktop GUI - C++]
│   ├── qt/                        [Qt implementation]
│   │   ├── include/
│   │   ├── src/
│   │   │   ├── main.cpp
│   │   │   ├── mainwindow.cpp
│   │   │   └── widgets/
│   │   ├── resources/
│   │   └── CMakeLists.txt
│   │
│   └── gtk/                       [GTK implementation (alternative)]
│       ├── include/
│       ├── src/
│       └── meson.build
│
└── gui-game/                      [Game GUI - C#]
    ├── Cornerstone.UI/            [Cornerstone UI library]
    │   ├── Components/
    │   ├── Panels/
    │   └── Themes/
    └── CPI-SI.UI/                 [CPI-SI specific UI]
        ├── StateDisplay/
        ├── HealthView/
        └── NavigationPanel/
```

### 5.3 Interface Implementation Matrix

| Feature | CLI (Go) | TUI (C++/Rust) | GUI Desktop (C++) | GUI Game (C#) |
|---------|----------|----------------|-------------------|---------------|
| State Machine View | Text table | Interactive panel | Widget | Game overlay |
| Health Display | Color text | Gauge widget | Progress bar | Health bar |
| Coordinate Nav | Text commands | Cursor nav | Click nav | 3D nav |
| Scripture View | Text output | Scrollable pane | Rich text | Book UI |
| Config Edit | File edit | Inline edit | Form dialog | Settings menu |
| FaithNet Status | Status line | Connection panel | System tray | Network icon |

### 5.4 Shared Interface Protocol

```rust
// interface/core/protocol.rs (or equivalent in each language)

/// Message types for cross-interface communication
enum InterfaceMessage {
    // State updates
    StateChanged { component: String, state: Value },
    HealthUpdated { scores: HealthScores },
    NavigationMoved { from: Position, to: Position },

    // Commands
    CommandRequest { cmd: String, args: Vec<String> },
    CommandResponse { result: Result<Value, Error> },

    // Events
    UserInput { input: InputEvent },
    SystemEvent { event: SystemEvent },

    // Sync
    SyncRequest { since: Timestamp },
    SyncResponse { updates: Vec<Update> },
}

/// All interfaces implement this trait
trait InterfaceBackend {
    fn init(&mut self) -> Result<(), Error>;
    fn render(&mut self, state: &AppState) -> Result<(), Error>;
    fn handle_input(&mut self) -> Result<Option<InputEvent>, Error>;
    fn send_message(&mut self, msg: InterfaceMessage) -> Result<(), Error>;
    fn receive_message(&mut self) -> Result<Option<InterfaceMessage>, Error>;
    fn shutdown(&mut self) -> Result<(), Error>;
}
```

---

## 6. Detailed Reorganization Map

### 6.1 void/ (Minor Updates)

| Current Path | New Path | Notes |
|--------------|----------|-------|
| `void/planning/` | `void/planning/` | Keep, add structure |
| `void/workflow/` | `void/workflow/` | Keep |
| (new) | `void/imports/` | Create for external imports |
| `word/research/` | `void/planning/research/` | Move from word |
| `word/journey/` | `void/planning/journey/` | Move from word |
| `word/claude/plans/` | `void/planning/claude/` | Move from word |

### 6.2 word/ (Major Restructure)

#### Layer 0: Universal Libraries

| Current Path | New Path | Language | Notes |
|--------------|----------|----------|-------|
| `word/work/pkg/trit/` | `word/L0-universal/libtrit/` | C | Keep as-is |
| `word/work/pkg/foundation/types/` | `word/L0-universal/foundation/types/` | Go | Keep |
| `word/work/pkg/foundation/schema/` | `word/L0-universal/foundation/schema/` | Go | Keep |
| `word/work/pkg/foundation/database/` | `word/L0-universal/foundation/database/` | Go | Keep |
| (new) | `word/L0-universal/foundation/result/` | Rust | NEW - error handling |
| `word/work/pkg/util/` | `word/L0-universal/util/` | Go | Keep |

#### Layer 1: OmniCode

| Current Path | New Path | Language | Notes |
|--------------|----------|----------|-------|
| `word/work/pkg/omni/src/omni_lexer.c` | `word/L1-omnicode/frontend/lexer.c` | C | Keep |
| `word/work/pkg/omni/src/omni_parser.c` | `word/L1-omnicode/frontend/parser.c` | C | Keep |
| `word/work/pkg/omni/src/omni_semantic.c` | `word/L1-omnicode/semantic/` | Rust | REWRITE |
| `word/work/pkg/omni/src/omni_ir.c` | `word/L1-omnicode/ir/` | C | Keep |
| `word/work/pkg/omni/src/omni_codegen.c` | `word/L1-omnicode/codegen/` | C | Keep |
| `word/work/pkg/omni/src/omni_vm.c` | `word/L1-omnicode/vm/` | Rust | REWRITE |
| `word/work/pkg/omni/src/omni_runtime.c` | `word/L1-omnicode/runtime/` | Rust | REWRITE |
| `word/work/pkg/omni/stdlib/` | `word/L1-omnicode/stdlib/` | OmniCode | Keep |

#### Layer 2: Platform

| Current Path | New Path | Language | Notes |
|--------------|----------|----------|-------|
| `word/work/pkg/fuse/` | `word/L2-platform/fuse/` | C | Keep |
| (new) | `word/L2-platform/filesystem/` | Rust | NEW - Bereshit FS |
| (new) | `word/L2-platform/kernel/` | C | NEW - OS components |

#### Layer 3: CPI-SI

| Current Path | New Path | Language | Notes |
|--------------|----------|----------|-------|
| `word/work/pkg/core/statemachine/` | `word/L3-cpisi/core/statemachine/` | Go | Keep |
| `word/work/pkg/core/coordinates/` | `word/L3-cpisi/core/coordinates/` | Go | Keep |
| `word/work/pkg/core/health/` | `word/L3-cpisi/core/health/` | Go | Keep |
| `word/work/pkg/core/validation/` | `word/L3-cpisi/core/validation/` | Go | Keep |
| `word/work/pkg/core/cpisi/` | `word/L3-cpisi/core/cpisi/` | Go | Keep |
| `word/work/pkg/orchestration/` | `word/L3-cpisi/orchestration/` | Go | Keep |
| `word/work/cmd/cpisi/` | `word/L3-cpisi/cmd/` | Go | Keep |

#### Layer 4: FaithNet (NEW)

| New Path | Language | Notes |
|----------|----------|-------|
| `word/L4-faithnet/protocol/` | Rust | FaithNet protocol |
| `word/L4-faithnet/mapping/` | Rust | TCP/IP mapping |
| `word/L4-faithnet/access/` | Rust | Tiered access |
| `word/L4-faithnet/crypto/` | Rust | Cryptography |
| `word/L4-faithnet/spec/` | Docs | Specifications |

#### Layer 5: Applications

| Current Path | New Path | Language | Notes |
|--------------|----------|----------|-------|
| `cornerstone/` (root) | `word/L5-applications/cornerstone/` | C# | Move submodule |
| `word/claude/` | `word/L5-applications/claude-code/` | Go | Consolidate |

#### Interface Layer (NEW)

| New Path | Language | Notes |
|----------|----------|-------|
| `word/interface/core/` | Go | Shared command/state/render |
| `word/interface/cli/` | Go | CLI implementation |
| `word/interface/tui/cpp/` | C++ | ncurses TUI |
| `word/interface/tui/rust/` | Rust | crossterm TUI |
| `word/interface/gui-desktop/qt/` | C++ | Qt GUI |
| `word/interface/gui-game/` | C# | Cornerstone UI |

#### Claude Code (Consolidated)

| Current Path | New Path | Notes |
|--------------|----------|-------|
| `word/work/hooks/` + `word/claude/hooks/` | `word/claude/hooks/` | MERGE |
| `word/work/statusline/` + `word/claude/statusline/` | `word/claude/statusline/` | MERGE |
| `word/claude/pkg/` | DELETE | Duplicate |
| `word/work/system/` + `word/claude/system/` | `word/claude/system/` | MERGE |
| `word/claude/skills/` + `word/skills/` | `word/claude/skills/` | MERGE |
| `word/claude/agents/` + `word/agents/` | `word/claude/agents/` | MERGE |

#### Definitions & Data

| Current Path | New Path | Notes |
|--------------|----------|-------|
| `word/core/` | `word/definitions/core/` | Type definitions |
| `word/glossary/` | `word/definitions/glossary/` | Glossary |
| `word/omni/` | `word/definitions/omni/` | OmniCode defs |
| `word/scripture/` | `word/data/scripture/` | Bible databases |
| `word/journals/` | `word/data/journals/` | Journals |
| `word/instance/` | `word/data/instance/` | Instances |

### 6.3 tov/ (Minor Updates)

| Current Path | New Path | Notes |
|--------------|----------|-------|
| `tov/demo/` | `tov/demo/` | Keep |
| `tov/licenses/` | `tov/licenses/` | Keep |
| `tov/pricing/` | `tov/pricing/` | Keep |
| `tov/data/` | `tov/data/` | Keep |
| (new) | `tov/releases/` | Create |

---

## 7. Implementation Consolidation

### 7.1 Go Scripts to Proper Implementations

#### Hook Commands (Currently 6-line wrappers)

**Current State:**
```go
// word/work/hooks/cmd/start/main.go (6 lines)
package main
import "hooks/internal"
func main() { internal.Run("start") }
```

**Target State:**
```go
// word/claude/hooks/cmd/start/main.go
package main

import (
    "hooks/lifecycle"
    "hooks/config"
)

func main() {
    cfg := config.Load()
    handler := lifecycle.NewStartHandler(cfg)

    if err := handler.Execute(); err != nil {
        lifecycle.HandleError(err)
    }
}
```

### 7.2 Consolidation Tasks

| Current | Target | Action |
|---------|--------|--------|
| 12 hook wrappers (72 LOC total) | Unified hook binary with subcommands | CONSOLIDATE |
| Duplicate pkg/ in claude/ and work/ | Single pkg/ in layer structure | MERGE |
| Skill orchestrators (CLI glue) | Shared skill framework | REFACTOR |
| Multiple status display implementations | Single statusline with modes | MERGE |

### 7.3 New Implementations Needed

| Component | Language | Priority | Description |
|-----------|----------|----------|-------------|
| `L0-universal/foundation/result` | Rust | HIGH | Error handling library |
| `L1-omnicode/semantic` | Rust | MEDIUM | Semantic analysis |
| `L1-omnicode/vm` | Rust | MEDIUM | Virtual machine |
| `L1-omnicode/runtime` | Rust | MEDIUM | Runtime system |
| `L2-platform/filesystem` | Rust | HIGH | Bereshit filesystem |
| `L4-faithnet/*` | Rust | HIGH | FaithNet network stack |
| `interface/tui/cpp` | C++ | MEDIUM | ncurses TUI |
| `interface/tui/rust` | Rust | MEDIUM | crossterm TUI |
| `interface/gui-desktop/qt` | C++ | LOW | Desktop GUI |
| `interface/gui-game` | C# | LOW | Cornerstone UI |

---

## 8. Root Files Reorganization

### 8.1 Root Files Summary

| File | Purpose | Action |
|------|---------|--------|
| `.gitignore` | Repository hygiene | Update patterns |
| `.gitmodules` | Submodule tracking | Update for new path |
| `.gitmessage` | Commit template | Keep |
| `.health` | Health marker | Keep |
| `go.work` | Go workspace | Update module paths |
| `go.work.sum` | Go checksums | Regenerate |
| `Cargo.toml` | Rust workspace | NEW |
| `.cargo/config.toml` | Rust config | NEW |
| `CMakeLists.txt` | C/C++ build | NEW |
| `LICENSE` | 3-tier license | Keep |
| `SUBMODULES.md` | Submodule docs | Update |
| `root.adoc` | Root documentation | Update |
| `root.omni` | OmniCode folder def | Update |

### 8.2 New Cargo.toml (Rust Workspace)

```toml
[workspace]
resolver = "2"
members = [
    "word/L0-universal/foundation/result",
    "word/L1-omnicode/semantic",
    "word/L1-omnicode/vm",
    "word/L1-omnicode/runtime",
    "word/L2-platform/filesystem",
    "word/L4-faithnet/protocol",
    "word/L4-faithnet/mapping",
    "word/L4-faithnet/access",
    "word/L4-faithnet/crypto",
    "word/interface/tui/rust",
]

[workspace.package]
version = "0.1.0"
edition = "2024"
authors = ["CreativeWorkzStudio LLC"]
license = "Proprietary"

[workspace.dependencies]
thiserror = "2.0"
anyhow = "1.0"
tokio = { version = "1", features = ["full"] }
crossterm = "0.28"
```

### 8.3 Updated go.work

```go
go 1.23

use (
    ./word/L0-universal/foundation
    ./word/L0-universal/util
    ./word/L3-cpisi/core
    ./word/L3-cpisi/orchestration
    ./word/L3-cpisi/cmd
    ./word/claude/hooks
    ./word/claude/statusline
    ./word/claude/skills
    ./word/claude/system
    ./word/interface/cli
    ./word/interface/core
    ./tov/demo/phase-0/demo-config
)
```

### 8.4 CMakeLists.txt (C/C++ Build)

```cmake
cmake_minimum_required(VERSION 3.20)
project(bereshit VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

# C Libraries
add_subdirectory(word/L0-universal/libtrit)
add_subdirectory(word/L1-omnicode/frontend)
add_subdirectory(word/L1-omnicode/ir)
add_subdirectory(word/L1-omnicode/codegen)
add_subdirectory(word/L2-platform/fuse)

# C++ TUI
add_subdirectory(word/interface/tui/cpp)

# C++ GUI (optional)
option(BUILD_GUI "Build GUI applications" OFF)
if(BUILD_GUI)
    add_subdirectory(word/interface/gui-desktop/qt)
endif()
```

---

## 9. Submodule Handling

### 9.1 Cornerstone Migration

```bash
# Step 1: Remove current submodule
git submodule deinit -f cornerstone
git rm -f cornerstone
rm -rf .git/modules/cornerstone

# Step 2: Create new location
mkdir -p word/L5-applications

# Step 3: Add at new location
git submodule add ../Cornerstone word/L5-applications/cornerstone

# Step 4: Initialize
git submodule init
git submodule update
```

### 9.2 Updated .gitmodules

```ini
[submodule "cornerstone"]
    path = word/L5-applications/cornerstone
    url = ../Cornerstone
```

---

## 10. Migration Strategy

### 10.1 Phase 1: Setup New Build Systems

1. Create `Cargo.toml` workspace
2. Create `CMakeLists.txt` root
3. Update `go.work` for new structure
4. Test all build systems work with current code

### 10.2 Phase 2: Create Layer Structure

```bash
# Create L0-L5 structure
mkdir -p word/L{0..5}-{universal,omnicode,platform,cpisi,faithnet,applications}
mkdir -p word/interface/{core,cli,tui/{cpp,rust},gui-desktop/qt,gui-game}
mkdir -p word/{definitions,data,seed,build}
```

### 10.3 Phase 3: Move Existing Code

1. Move libtrit → L0-universal/libtrit
2. Move foundation → L0-universal/foundation
3. Move omni → L1-omnicode (split by purpose)
4. Move fuse → L2-platform/fuse
5. Move core/statemachine etc → L3-cpisi
6. Move claude code → consolidated location

### 10.4 Phase 4: Merge Duplicates

1. Merge word/claude/pkg + word/work/pkg
2. Merge hooks implementations
3. Merge statusline implementations
4. Delete duplicate code

### 10.5 Phase 5: Add New Components

1. Create Rust result library
2. Stub FaithNet packages
3. Create interface core framework
4. Stub TUI implementations

### 10.6 Phase 6: Update All Imports

1. Update Go imports
2. Update C includes
3. Update Rust modules
4. Verify builds pass

### 10.7 Phase 7: Move Submodule

1. Remove cornerstone from root
2. Add to word/L5-applications
3. Update .gitmodules
4. Verify functionality

### 10.8 Phase 8: Cleanup & Document

1. Remove old word/work directory structure
2. Update root.omni
3. Update all documentation
4. Final verification

---

## 11. Validation Checklist

### 11.1 Structure Validation

- [ ] `void/` exists with planning/, workflow/, imports/
- [ ] `word/` has L0-L5 layer folders
- [ ] `word/interface/` has cli/, tui/, gui-desktop/, gui-game/
- [ ] `tov/` has demo/, licenses/, pricing/, releases/, data/
- [ ] No duplicate pkg/ directories
- [ ] cornerstone at word/L5-applications/

### 11.2 Build Validation

- [ ] `cargo build --workspace` succeeds
- [ ] `cmake --build .` succeeds
- [ ] `go work sync` succeeds
- [ ] All tests pass in each language

### 11.3 Language Validation

- [ ] C code compiles: libtrit, omni frontend, fuse
- [ ] Rust code compiles: result, (stubs for vm, faithnet)
- [ ] Go code compiles: cpisi, hooks, statusline
- [ ] C++ code compiles: (stubs for TUI, GUI)

### 11.4 Runtime Validation

- [ ] libtrit tests pass
- [ ] Go state machine tests pass
- [ ] Claude hooks fire correctly
- [ ] Statusline displays correctly

### 11.5 Interface Validation

- [ ] CLI commands work
- [ ] (Future) TUI renders
- [ ] (Future) GUI launches

---

## Quick Reference

### New Structure Summary

```
bereshit/
├── void/           [Entry - Planning]
├── word/           [Main - Definitions & Implementation]
│   ├── L0-universal/     [C/Go - Libraries]
│   ├── L1-omnicode/      [C/Rust - Language]
│   ├── L2-platform/      [C/Rust - OS]
│   ├── L3-cpisi/         [Go - Intelligence]
│   ├── L4-faithnet/      [Rust - Network]
│   ├── L5-applications/  [C#/Go - Apps]
│   ├── interface/        [Multi - CLI/TUI/GUI]
│   ├── claude/           [Go - Claude Code]
│   ├── definitions/      [TOML/Omni - Types]
│   ├── data/             [Data - Scripture, etc]
│   ├── seed/             [Templates]
│   └── build/            [Artifacts]
└── tov/            [Exit - Production]
```

### Language Quick Reference

| Language | Primary Use | Location |
|----------|-------------|----------|
| **C** | Performance (math, compiler front) | L0/libtrit, L1/frontend, L2/fuse |
| **Rust** | Safety (VM, network, FS) | L1/vm, L2/fs, L4/faithnet |
| **Go** | Orchestration (CPI-SI, hooks) | L3/cpisi, claude/, interface/cli |
| **C++** | Native UI (TUI, desktop GUI) | interface/tui, interface/gui-desktop |
| **C#** | Game engine (Cornerstone) | L5/cornerstone, interface/gui-game |

### Layer Quick Reference

| Layer | Location | Languages | Biblical Foundation |
|-------|----------|-----------|---------------------|
| L0 | word/L0-universal/ | C, Go | Genesis 1:1 |
| L1 | word/L1-omnicode/ | C, Rust | John 1:1 |
| L2 | word/L2-platform/ | C, Rust | Genesis 1:6 |
| L3 | word/L3-cpisi/ | Go | Genesis 2:7 |
| L4 | word/L4-faithnet/ | Rust | Genesis 2:9 |
| L5 | word/L5-applications/ | C#, Go | Genesis 2:15 |

---

## Closing

This reorganization:
1. Maintains **void/word/tov** ternary structure
2. Assigns proper languages (C, Rust, Go, C++, C#) to appropriate components
3. Reduces Go "scripts" to proper implementations
4. Provides clear CLI/TUI/GUI scaling paths
5. Makes layer architecture visible in folder structure

The flow from **Universal → OmniCode → Platform → CPI-SI → FaithNet → Applications** becomes self-documenting through the L0-L5 folder hierarchy.

> *"For other foundation can no man lay than that is laid, which is Jesus Christ."* — 1 Corinthians 3:11 KJV

---

**Document Status**

| Field | Value |
|-------|-------|
| Key | B-void-planning-repo-reorganization |
| Version | 2.0.0 |
| Status | Active |
| Created | 2026-02-01 |
| Updated | 2026-02-01 |
| Authors | Seanje Lenox-Wise (Architect), Nova Dawn (Implementation) |
