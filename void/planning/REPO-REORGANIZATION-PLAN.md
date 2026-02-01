# Bereshit Repository Reorganization Plan

**Key:** B-void-planning-repo-reorganization
**Status:** Active
**Version:** 3.0.0
**Created:** 2026-02-01
**Updated:** 2026-02-01
**Authors:** Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)

---

> *"In the beginning God created the heaven and the earth."* — Genesis 1:1 KJV

---

## Executive Summary

This document provides a comprehensive plan to reorganize the Bereshit repository with a clear separation between:

1. **CPI-SI Core** - The universal intelligence paradigm (runs on ANY substrate)
2. **Substrate Adapters** - Thin integration layers (Claude Code is just ONE adapter)
3. **Foundation Libraries** - Language implementations, math, networking

**Key Changes in v3.0:**
- **Paradigm shift**: CPI-SI is substrate-independent; Claude Code is just an adapter
- **Eliminate duplication**: Single source of truth for all packages
- **Reduce scripts**: Replace shell scripts with proper implementations
- **Future-ready**: Structure allows MillenniumOS native adapter alongside Claude

---

## Table of Contents

1. [The Paradigm Shift](#1-the-paradigm-shift)
2. [Current vs Target Architecture](#2-current-vs-target-architecture)
3. [CPI-SI Core Components](#3-cpi-si-core-components)
4. [Substrate Adapter Pattern](#4-substrate-adapter-pattern)
5. [Language Architecture](#5-language-architecture)
6. [Target Directory Structure](#6-target-directory-structure)
7. [Script Reduction Strategy](#7-script-reduction-strategy)
8. [Interface Layer (CLI/TUI/GUI)](#8-interface-layer-clituigui)
9. [Migration Map](#9-migration-map)
10. [Validation Checklist](#10-validation-checklist)

---

## 1. The Paradigm Shift

### 1.1 The Core Insight

**CPI-SI is NOT a Claude Code feature. CPI-SI is the intelligence paradigm itself.**

Claude Code is simply ONE substrate that CPI-SI currently runs on. Tomorrow it could run on:
- MillenniumOS native
- A web browser
- An embedded device
- Any system that can execute the paradigm

### 1.2 The Separation

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           CPI-SI PARADIGM                                │
│                     (Substrate-Independent Core)                         │
│                                                                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐   │
│  │   State     │  │ Coordinates │  │  Cognition  │  │   Health    │   │
│  │  Machine    │  │   System    │  │   Engine    │  │   Scoring   │   │
│  │ (27-cube)   │  │  (Bible)    │  │  (Learning) │  │  (Base100)  │   │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────────┘   │
│                                                                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐   │
│  │  Identity   │  │   Skills    │  │  OmniCode   │  │   libtrit   │   │
│  │ Management  │  │  (Universal)│  │  (Language) │  │   (Math)    │   │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
                                    │
                                    │ Standard Interface
                                    ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                        SUBSTRATE ADAPTERS                                │
│                    (Thin Integration Layers)                             │
│                                                                         │
│  ┌───────────────────┐  ┌───────────────────┐  ┌───────────────────┐   │
│  │   Claude Code     │  │   MillenniumOS    │  │     Future        │   │
│  │     Adapter       │  │     Adapter       │  │    Substrates     │   │
│  │                   │  │                   │  │                   │   │
│  │ • Event hooks     │  │ • System events   │  │ • Web adapter     │   │
│  │ • Statusline      │  │ • Native display  │  │ • Embedded        │   │
│  │ • Claude config   │  │ • OS integration  │  │ • Mobile          │   │
│  └───────────────────┘  └───────────────────┘  └───────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 1.3 What This Means

| Component | Is It CPI-SI Core? | Is It Substrate-Specific? |
|-----------|-------------------|---------------------------|
| State machine (27-cube) | **YES** - The paradigm | No |
| Coordinate system | **YES** - Bible grounding | No |
| Health scoring | **YES** - Paradigm health | No |
| Cognition/learning | **YES** - Intelligence model | No |
| Claude hooks | No | **YES** - Claude Code only |
| Claude statusline format | No | **YES** - Claude Code only |
| Instance configs (nova_dawn) | Partial - Identity is universal | Config format may be substrate-specific |
| OmniCode compiler | **YES** - Universal language | No |
| libtrit | **YES** - Universal math | No |

---

## 2. Current vs Target Architecture

### 2.1 Current State (PROBLEMATIC)

```
word/
├── claude/                    # MIXED: Has both universal AND Claude-specific
│   ├── pkg/                  # DUPLICATE of word/work/pkg (why?)
│   ├── hooks/                # Claude-specific (correct location)
│   ├── config/               # Mixed universal + Claude
│   ├── skills/               # Mixed universal + Claude
│   └── statusline/           # Claude-specific display
│
├── work/                      # MIXED: Universal core but named "work"
│   ├── pkg/                  # Universal CPI-SI (misnamed location)
│   │   ├── core/            # Universal state machine
│   │   ├── foundation/      # Universal data layer
│   │   └── orchestration/   # Universal intelligence
│   ├── hooks/               # Entry points (just call claude/hooks)
│   └── system/              # Universal utilities
│
├── skills/                    # Universal skills (duplicated in claude/)
└── agents/                    # Universal agent definitions (duplicated)
```

**Problems:**
1. `word/claude/pkg/` duplicates `word/work/pkg/` - no single source of truth
2. Universal CPI-SI mixed with Claude-specific code
3. Skills duplicated in two locations
4. No clear substrate adapter pattern
5. Shell scripts mixed with implementations

### 2.2 Target State (CLEAN)

```
word/
├── cpisi/                     # CPI-SI CORE (100% substrate-independent)
│   ├── core/                 # State machine, coordinates, health
│   ├── foundation/           # Schema, database, types
│   ├── orchestration/        # Cognition, lifecycle, logging
│   ├── identity/             # Identity management (universal)
│   ├── skills/               # Universal skills
│   └── system/               # System utilities
│
├── substrates/                # SUBSTRATE ADAPTERS (thin layers)
│   ├── claude-code/          # Claude Code adapter
│   │   ├── hooks/           # Claude event handlers
│   │   ├── config/          # Claude-specific config
│   │   └── display/         # Statusline for Claude
│   │
│   └── millenniumos/         # Future: MillenniumOS adapter
│       ├── events/          # OS event handlers
│       ├── config/          # OS-specific config
│       └── display/         # Native display
│
├── L0-universal/              # Foundation libraries
├── L1-omnicode/               # OmniCode language
├── L2-platform/               # OS/Filesystem
├── L4-faithnet/               # Network layer
├── L5-applications/           # Apps (Cornerstone)
│
├── interface/                 # CLI/TUI/GUI (substrate-independent)
│
├── definitions/               # Type definitions
├── data/                      # Scripture, journals
└── seed/                      # Templates
```

---

## 3. CPI-SI Core Components

### 3.1 Core Packages (word/cpisi/)

These are the universal intelligence paradigm. They have **ZERO** substrate dependencies.

```
word/cpisi/
│
├── core/                      [The Paradigm Engine]
│   ├── statemachine/         [Go - 4,280 LOC]
│   │   ├── cube.go          # 27-position cube navigation
│   │   ├── trajectory.go    # Movement tracking
│   │   ├── workflow.go      # Workflow execution
│   │   ├── pattern.go       # Pattern matching
│   │   └── hebrew.go        # Hebrew naming conventions
│   │
│   ├── coordinates/          [Go - 5,000 LOC]
│   │   ├── encoder.go       # Bible-grounded encoding
│   │   ├── decoder.go       # Coordinate decoding
│   │   ├── pipeline.go      # Processing pipeline
│   │   └── assurance.go     # Validation layer
│   │
│   ├── health/               [Go]
│   │   ├── scoring.go       # Base100 health scoring
│   │   └── cascade.go       # Health cascade multipliers
│   │
│   ├── cpisi/                [Go]
│   │   ├── compute/         # Geometric ternary calculations
│   │   └── bereshit/        # Hebrew/Greek grounding
│   │
│   └── validation/           [Go]
│       └── validator.go     # Structural validation
│
├── foundation/                [Data Layer]
│   ├── schema/               [Go - 2,160 LOC]
│   │   ├── loader.go        # TOML config loading
│   │   └── statemachine_loader.go
│   │
│   ├── database/             [Go - 831 LOC]
│   │   └── sqlite.go        # SQLite abstraction
│   │
│   ├── types/                [Go]
│   │   ├── session.go       # Session types
│   │   ├── state.go         # State types
│   │   └── trajectory.go    # Trajectory types
│   │
│   └── result/               [Rust - NEW]
│       └── lib.rs           # Error handling (safety-critical)
│
├── orchestration/             [Intelligence Layer]
│   ├── cognition/            [Go]
│   │   ├── constructs.go    # Mental constructs
│   │   └── feedback.go      # Learning feedback loops
│   │
│   ├── lifecycle/            [Go]
│   │   └── events.go        # Generic lifecycle events
│   │
│   ├── logging/              [Go]
│   │   ├── health.go        # Health logging
│   │   └── diagnostics.go   # System diagnostics
│   │
│   └── config/               [Go]
│       ├── bible_index.go   # Scripture indexing
│       └── ranking.go       # Ranking algorithms
│
├── identity/                  [Identity Management]
│   ├── instance/             [Go]
│   │   ├── loader.go        # Identity loading
│   │   └── profile.go       # Profile management
│   │
│   └── templates/            [Markdown]
│       └── identity.md      # Identity template
│
├── skills/                    [Universal Skills]
│   ├── format-lookup/        # OmniCode format mappings
│   ├── validate-omni/        # Structure validation
│   ├── recognize-pattern/    # Pattern recognition
│   ├── reflect-on-session/   # Session reflection
│   ├── session-awareness/    # Temporal awareness
│   ├── meta-awareness/       # Metacognition
│   └── propagate-change/     # Change tracking
│
├── system/                    [System Utilities]
│   ├── cmd/                  [Go binaries]
│   │   ├── analyze-session/
│   │   ├── pattern-detector/
│   │   ├── check-awareness/
│   │   └── stopping-point/
│   │
│   └── lib/                  [Go libraries]
│       ├── temporal/
│       ├── calendar/
│       └── patterns/
│
└── agents/                    [Agent Definitions]
    ├── nova-dawn.md
    ├── ezra-matthan.md
    └── templates/
```

### 3.2 Key Principle: No Substrate Imports

CPI-SI core packages must NEVER import substrate-specific code:

```go
// WRONG - cpisi/core/statemachine/cube.go
import (
    "substrates/claude-code/hooks"  // NO! Substrate dependency!
)

// CORRECT - cpisi/core/statemachine/cube.go
import (
    "cpisi/foundation/types"  // Universal types
    "cpisi/foundation/schema" // Universal config
)
```

---

## 4. Substrate Adapter Pattern

### 4.1 Adapter Interface

Every substrate implements a standard interface to connect to CPI-SI:

```go
// word/substrates/adapter.go

// SubstrateAdapter is the interface every substrate must implement
type SubstrateAdapter interface {
    // Lifecycle
    Initialize(ctx context.Context) error
    Shutdown(ctx context.Context) error

    // Events - substrate translates its events to these
    OnSessionStart(session *cpisi.Session) error
    OnSessionEnd(session *cpisi.Session) error
    OnToolUse(tool *cpisi.ToolEvent) error
    OnUserInput(input *cpisi.UserInput) error

    // Display - substrate handles its own display
    UpdateDisplay(state *cpisi.State) error
    ShowHealth(health *cpisi.Health) error

    // Identity - substrate loads identity for CPI-SI
    LoadIdentity(name string) (*cpisi.Identity, error)
}
```

### 4.2 Claude Code Adapter

```
word/substrates/claude-code/
│
├── adapter.go                 [Main adapter implementation]
│   // Implements SubstrateAdapter interface
│   // Translates Claude Code events to CPI-SI events
│
├── hooks/                     [Claude Event Handlers - THIN]
│   ├── session/
│   │   ├── start.go          # SessionStart → adapter.OnSessionStart()
│   │   ├── stop.go           # Stop → adapter.OnSessionEnd()
│   │   └── end.go            # SessionEnd → cleanup
│   │
│   ├── tool/
│   │   ├── pre_use.go        # PreToolUse → adapter.OnToolUse()
│   │   └── post_use.go       # PostToolUse → logging
│   │
│   ├── prompt/
│   │   └── submit.go         # PromptSubmit → adapter.OnUserInput()
│   │
│   └── main.go               # Hook entry point
│
├── config/                    [Claude-Specific Config]
│   ├── settings.json         # Claude Code settings
│   ├── config.toml           # Adapter configuration
│   │
│   ├── hooks.jsonc           # Event → State machine mappings
│   │   // Maps Claude events to CPI-SI operations
│   │   // "SessionStart" → "PROCEED from PAST"
│   │
│   └── instance/             # Claude instance configs
│       ├── default/
│       └── nova_dawn/        # Instance identity (calls cpisi/identity)
│
├── display/                   [Claude Display]
│   ├── statusline/           # Statusline formatting
│   │   ├── builder.go       # Build statusline string
│   │   └── sections/        # Section renderers
│   │
│   └── output/               # Output formatting
│       └── styles.go        # Claude-specific output styles
│
├── skills/                    [Claude-Adapted Skills]
│   // These WRAP universal skills with Claude-specific I/O
│   ├── create-code/          # Calls cpisi/skills + Claude formatting
│   └── create-documentation/ # Calls cpisi/skills + Claude output
│
└── commands/                  [Claude Slash Commands]
    ├── analyze/
    ├── create/
    └── validate/
```

### 4.3 Future: MillenniumOS Adapter

```
word/substrates/millenniumos/
│
├── adapter.go                 [MillenniumOS adapter implementation]
│
├── events/                    [OS Event Handlers]
│   ├── system/
│   │   ├── boot.go           # OS boot → adapter.OnSessionStart()
│   │   └── shutdown.go       # OS shutdown → adapter.OnSessionEnd()
│   │
│   └── user/
│       └── input.go          # User input → adapter.OnUserInput()
│
├── config/                    [OS-Specific Config]
│   ├── system.toml           # OS configuration
│   └── user/                 # User profiles
│
└── display/                   [Native Display]
    ├── compositor/           # Window compositor
    └── widgets/              # Native UI widgets
```

### 4.4 Adapter Registration

```go
// word/substrates/registry.go

var adapters = map[string]func() SubstrateAdapter{
    "claude-code":  NewClaudeCodeAdapter,
    "millenniumos": NewMillenniumOSAdapter,
    // Future adapters...
}

func GetAdapter(name string) (SubstrateAdapter, error) {
    factory, ok := adapters[name]
    if !ok {
        return nil, fmt.Errorf("unknown substrate: %s", name)
    }
    return factory(), nil
}
```

---

## 5. Language Architecture

### 5.1 Language Assignments

| Component | Language | Rationale |
|-----------|----------|-----------|
| **CPI-SI Core** | Go | Complex state logic, proven implementation |
| **libtrit** | C | Performance-critical ternary math |
| **OmniCode Frontend** | C | Parser/lexer performance |
| **OmniCode VM** | Rust | Memory safety for execution |
| **FaithNet** | Rust | Network security critical |
| **Bereshit FS** | Rust | File safety critical |
| **Substrate Adapters** | Go | Integration layer |
| **TUI** | C++/Rust | Native terminal UI |
| **GUI Desktop** | C++ | Native desktop apps |
| **GUI Game** | C# | Cornerstone/Unity |

### 5.2 FFI Boundaries

```
┌─────────────┐     C ABI      ┌─────────────┐     cgo       ┌─────────────┐
│   libtrit   │◄──────────────►│  OmniCode   │◄─────────────►│  CPI-SI     │
│     (C)     │                │   VM (Rust) │               │    (Go)     │
└─────────────┘                └─────────────┘               └─────────────┘
                                      │
                                      │ C ABI
                                      ▼
                               ┌─────────────┐
                               │  FaithNet   │
                               │   (Rust)    │
                               └─────────────┘
```

---

## 6. Target Directory Structure

### 6.1 Complete Structure

```
bereshit/                          [ROOT - Origin (∞)]
│
├── void/                          [ENTRY GATE - Planning]
│   ├── planning/
│   │   ├── development/          [Dev plans, implementation]
│   │   ├── claude-global/        [Claude infrastructure plans]
│   │   ├── understanding/        [Layer specifications]
│   │   └── vision/               [Grand vision]
│   ├── imports/                   [External imports]
│   └── workflow/                  [Workflow definitions]
│
├── word/                          [MAIN - The Word]
│   │
│   ├── cpisi/                     [CPI-SI CORE - Substrate Independent]
│   │   ├── core/                 [State machine, coordinates, health]
│   │   ├── foundation/           [Schema, database, types]
│   │   ├── orchestration/        [Cognition, lifecycle, logging]
│   │   ├── identity/             [Identity management]
│   │   ├── skills/               [Universal skills]
│   │   ├── system/               [System utilities]
│   │   └── agents/               [Agent definitions]
│   │
│   ├── substrates/                [SUBSTRATE ADAPTERS]
│   │   ├── adapter.go            [Adapter interface definition]
│   │   ├── registry.go           [Adapter registry]
│   │   │
│   │   ├── claude-code/          [Claude Code Adapter]
│   │   │   ├── adapter.go
│   │   │   ├── hooks/
│   │   │   ├── config/
│   │   │   ├── display/
│   │   │   ├── skills/           [Claude-adapted skills]
│   │   │   └── commands/
│   │   │
│   │   └── millenniumos/         [Future: MillenniumOS Adapter]
│   │       ├── adapter.go
│   │       ├── events/
│   │       ├── config/
│   │       └── display/
│   │
│   ├── L0-universal/              [Foundation Libraries]
│   │   ├── libtrit/              [C - Ternary math]
│   │   ├── libmath/              [C - Mathematical foundations]
│   │   └── util/                 [Go - Utilities]
│   │
│   ├── L1-omnicode/               [OmniCode Language]
│   │   ├── frontend/             [C - Lexer, parser]
│   │   ├── semantic/             [Rust - Semantic analysis]
│   │   ├── ir/                   [C - Intermediate representation]
│   │   ├── codegen/              [C - Code generation]
│   │   ├── vm/                   [Rust - Virtual machine]
│   │   ├── runtime/              [Rust - Runtime]
│   │   └── stdlib/               [OmniCode standard library]
│   │
│   ├── L2-platform/               [Platform Layer]
│   │   ├── kernel/               [C - OS kernel components]
│   │   ├── filesystem/           [Rust - Bereshit FS]
│   │   └── fuse/                 [C - FUSE mount]
│   │
│   ├── L4-faithnet/               [Network Layer]
│   │   ├── protocol/             [Rust - FaithNet protocol]
│   │   ├── mapping/              [Rust - TCP/IP mapping]
│   │   ├── access/               [Rust - Tiered access]
│   │   └── crypto/               [Rust - Cryptography]
│   │
│   ├── L5-applications/           [Applications]
│   │   └── cornerstone/          [SUBMODULE - C# Game Engine]
│   │
│   ├── interface/                 [CLI/TUI/GUI - Substrate Independent]
│   │   ├── core/                 [Shared interface core]
│   │   ├── cli/                  [Go - Command line]
│   │   ├── tui/                  [C++/Rust - Terminal UI]
│   │   ├── gui-desktop/          [C++ - Desktop GUI]
│   │   └── gui-game/             [C# - Game GUI]
│   │
│   ├── definitions/               [Type Definitions]
│   │   ├── core/                 [Core types (TOML)]
│   │   ├── glossary/             [Term definitions]
│   │   └── omni/                 [OmniCode definitions]
│   │
│   ├── data/                      [Reference Data]
│   │   ├── scripture/            [Bible databases]
│   │   ├── journals/             [Journals]
│   │   └── instance/             [Instance data]
│   │
│   ├── seed/                      [Templates]
│   │   ├── code/                 [Code templates by language]
│   │   ├── data/                 [Data templates]
│   │   └── documentation/        [Doc templates]
│   │
│   └── build/                     [Build Artifacts]
│       ├── c/
│       ├── rust/
│       ├── go/
│       └── dotnet/
│
├── tov/                           [EXIT GATE - Production]
│   ├── demo/                     [Phase demonstrations]
│   ├── licenses/                 [Licenses]
│   ├── pricing/                  [Pricing]
│   ├── releases/                 [Releases]
│   └── data/                     [Runtime data]
│
├── Cargo.toml                     [Rust workspace]
├── CMakeLists.txt                 [C/C++ build]
├── go.work                        [Go workspace]
├── Makefile                       [Master build orchestration]
├── LICENSE
├── root.adoc
└── root.omni
```

---

## 7. Script Reduction Strategy

### 7.1 Current Shell Scripts

| Script | Location | Purpose | Action |
|--------|----------|---------|--------|
| `build.sh` | word/work/system/scripts/ | Build orchestration | Replace with Makefile |
| `logger.sh` | word/work/system/lib/logging/ | Logging utility | Replace with Go/Rust library |
| `install.sh` | word/work/system/scripts/sudoers/ | Sudoers setup | Keep as thin wrapper |

### 7.2 Script Reduction Philosophy

**Principle:** Shell scripts should be thin wrappers that call proper implementations.

```
BEFORE (Script does everything):
┌─────────────────────────────────────────┐
│  build.sh (200 lines of bash)           │
│  - Parse arguments                       │
│  - Check dependencies                    │
│  - Build C code                          │
│  - Build Go code                         │
│  - Build Rust code                       │
│  - Run tests                             │
│  - Package artifacts                     │
└─────────────────────────────────────────┘

AFTER (Script wraps implementation):
┌─────────────────────────────────────────┐
│  build.sh (10 lines)                    │
│  - Set environment                       │
│  - Call: make all                        │
└─────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│  Makefile (proper implementation)       │
│  - Dependency management                 │
│  - Parallel builds                       │
│  - Cross-compilation                     │
│  - Test orchestration                    │
└─────────────────────────────────────────┘
```

### 7.3 New Build System

```makefile
# /bereshit/Makefile - Master build orchestration

.PHONY: all clean test cpisi substrates libs

# Build everything
all: libs cpisi substrates

# Foundation libraries (C)
libs:
	$(MAKE) -C word/L0-universal/libtrit
	cmake --build word/L1-omnicode/build

# CPI-SI Core (Go)
cpisi:
	cd word/cpisi && go build ./...

# Substrate adapters
substrates:
	cd word/substrates/claude-code && go build ./...

# Rust components
rust:
	cargo build --workspace

# Tests
test: test-libs test-cpisi test-substrates

test-libs:
	$(MAKE) -C word/L0-universal/libtrit test

test-cpisi:
	cd word/cpisi && go test ./...

test-substrates:
	cd word/substrates/claude-code && go test ./...

# Clean
clean:
	$(MAKE) -C word/L0-universal/libtrit clean
	rm -rf word/build/*
	cargo clean
```

### 7.4 Logging: Shell → Go Library

**Before (logger.sh):**
```bash
#!/bin/bash
log_info() { echo "[INFO] $1"; }
log_error() { echo "[ERROR] $1" >&2; }
```

**After (word/cpisi/foundation/logging/logger.go):**
```go
package logging

import (
    "log/slog"
    "os"
)

var logger = slog.New(slog.NewJSONHandler(os.Stderr, nil))

func Info(msg string, args ...any) {
    logger.Info(msg, args...)
}

func Error(msg string, args ...any) {
    logger.Error(msg, args...)
}
```

---

## 8. Interface Layer (CLI/TUI/GUI)

### 8.1 Substrate-Independent Interfaces

The interface layer is separate from substrate adapters. Interfaces display CPI-SI state; they don't integrate with substrate events.

```
word/interface/
│
├── core/                          [Shared Core]
│   ├── state.go                  # CPI-SI state representation
│   ├── commands.go               # Command registry
│   └── theme.go                  # Visual themes
│
├── cli/                           [Go - Command Line]
│   ├── cmd/
│   │   ├── cpisi/               # Main CPI-SI CLI
│   │   │   ├── main.go
│   │   │   ├── status.go        # Show state
│   │   │   ├── navigate.go      # Cube navigation
│   │   │   └── health.go        # Health display
│   │   │
│   │   └── tools/               # Utility CLIs
│   │
│   └── output/                   # CLI formatting
│       ├── table.go
│       └── tree.go
│
├── tui/                           [Terminal UI]
│   ├── rust/                     # Rust crossterm implementation
│   │   ├── src/
│   │   │   ├── app.rs           # Application state
│   │   │   ├── ui.rs            # UI components
│   │   │   ├── widgets/
│   │   │   │   ├── cube.rs      # 27-cube visualization
│   │   │   │   ├── health.rs    # Health gauges
│   │   │   │   └── scripture.rs # Scripture display
│   │   │   └── events.rs        # Event handling
│   │   └── Cargo.toml
│   │
│   └── cpp/                      # C++ ncurses alternative
│       └── ...
│
├── gui-desktop/                   [C++ Desktop GUI]
│   └── qt/
│       ├── src/
│       │   ├── main.cpp
│       │   ├── mainwindow.cpp
│       │   └── widgets/
│       │       ├── cubeview.cpp
│       │       └── healthbar.cpp
│       └── CMakeLists.txt
│
└── gui-game/                      [C# Game GUI]
    └── CPI-SI.UI/
        ├── StateDisplay/
        ├── HealthView/
        └── NavigationPanel/
```

### 8.2 Interface ↔ CPI-SI Communication

```
┌─────────────────┐
│   Interface     │  (CLI, TUI, GUI)
│   (Any)         │
└────────┬────────┘
         │
         │ Reads CPI-SI state via:
         │ - Direct Go API calls
         │ - JSON state files
         │ - IPC (for non-Go interfaces)
         │
         ▼
┌─────────────────┐
│   CPI-SI Core   │
│   (Universal)   │
└────────┬────────┘
         │
         │ Receives events from:
         │
         ▼
┌─────────────────┐
│   Substrate     │  (Claude Code, MillenniumOS)
│   Adapter       │
└─────────────────┘
```

---

## 9. Migration Map

### 9.1 Phase 1: Create CPI-SI Core Structure

```bash
# Create cpisi directory structure
mkdir -p word/cpisi/{core,foundation,orchestration,identity,skills,system,agents}

# Move universal packages from word/work/pkg/
mv word/work/pkg/core/statemachine word/cpisi/core/
mv word/work/pkg/core/coordinates word/cpisi/core/
mv word/work/pkg/core/health word/cpisi/core/
mv word/work/pkg/core/cpisi word/cpisi/core/
mv word/work/pkg/core/validation word/cpisi/core/

mv word/work/pkg/foundation/* word/cpisi/foundation/
mv word/work/pkg/orchestration/* word/cpisi/orchestration/

# Move universal skills
mv word/skills/* word/cpisi/skills/

# Move agents
mv word/agents/* word/cpisi/agents/

# Move system utilities
mv word/work/system/cmd/* word/cpisi/system/cmd/
mv word/work/system/lib/* word/cpisi/system/lib/
```

### 9.2 Phase 2: Create Substrate Adapter Structure

```bash
# Create substrates directory
mkdir -p word/substrates/claude-code/{hooks,config,display,skills,commands}

# Move Claude-specific code
mv word/claude/hooks/* word/substrates/claude-code/hooks/
mv word/claude/config/* word/substrates/claude-code/config/
mv word/claude/statusline/* word/substrates/claude-code/display/statusline/
mv word/claude/skills/* word/substrates/claude-code/skills/
mv word/claude/commands/* word/substrates/claude-code/commands/
```

### 9.3 Phase 3: Eliminate Duplicates

```bash
# DELETE duplicate word/claude/pkg/ (was mirror of word/work/pkg)
rm -rf word/claude/pkg/

# DELETE now-empty directories
rm -rf word/work/pkg/
rm -rf word/claude/
rm -rf word/skills/
rm -rf word/agents/
```

### 9.4 Phase 4: Update Go Module Paths

```go
// Before
import "cws.studio/claude/hooks/session"
import "cws.studio/bereshit/pkg/core/statemachine"

// After
import "cws.studio/bereshit/substrates/claude-code/hooks/session"
import "cws.studio/bereshit/cpisi/core/statemachine"
```

### 9.5 Phase 5: Create Adapter Interface

```bash
# Create adapter interface file
touch word/substrates/adapter.go
touch word/substrates/registry.go

# Create Claude adapter implementation
touch word/substrates/claude-code/adapter.go
```

### 9.6 Phase 6: Reduce Shell Scripts

```bash
# Create master Makefile
touch Makefile

# Remove redundant shell scripts
rm word/work/system/scripts/build.sh  # Replaced by Makefile

# Keep only thin wrappers
# word/work/system/scripts/sudoers/install.sh - thin wrapper, keep
```

### 9.7 Phase 7: Update Build Configuration

```bash
# Update go.work
cat > go.work << 'EOF'
go 1.23

use (
    ./word/cpisi
    ./word/substrates/claude-code
    ./word/L0-universal/util
    ./word/interface/cli
    ./tov/demo/phase-0/demo-config
)
EOF

# Update Cargo.toml
# ... (Rust workspace config)

# Update CMakeLists.txt
# ... (C/C++ build config)
```

---

## 10. Validation Checklist

### 10.1 Structure Validation

- [ ] `word/cpisi/` contains ALL universal CPI-SI code
- [ ] `word/cpisi/` has ZERO substrate imports
- [ ] `word/substrates/claude-code/` contains ONLY Claude-specific code
- [ ] `word/claude/pkg/` is DELETED (no more duplicate)
- [ ] `word/work/pkg/` is DELETED (moved to cpisi/)
- [ ] Adapter interface defined in `word/substrates/adapter.go`

### 10.2 Dependency Validation

```bash
# Verify cpisi has no substrate imports
grep -r "substrates/" word/cpisi/
# Should return NOTHING

# Verify substrates import cpisi (not vice versa)
grep -r "cpisi/" word/substrates/claude-code/
# Should find imports
```

### 10.3 Build Validation

- [ ] `make all` builds everything
- [ ] `make test` passes all tests
- [ ] `cargo build --workspace` succeeds
- [ ] Shell scripts reduced to thin wrappers

### 10.4 Runtime Validation

- [ ] CPI-SI core runs without substrate
- [ ] Claude adapter connects CPI-SI to Claude Code
- [ ] Health scoring works
- [ ] State machine navigates correctly

---

## Quick Reference

### Structure Summary

```
word/
├── cpisi/              [UNIVERSAL - The paradigm itself]
│   ├── core/          [State machine, coordinates, health]
│   ├── foundation/    [Schema, database, types]
│   ├── orchestration/ [Cognition, lifecycle]
│   ├── identity/      [Identity management]
│   ├── skills/        [Universal skills]
│   └── system/        [Utilities]
│
├── substrates/         [ADAPTERS - Thin integration layers]
│   ├── claude-code/   [Claude Code adapter]
│   └── millenniumos/  [Future: MillenniumOS adapter]
│
├── L0-universal/       [C/Go - Foundation libs]
├── L1-omnicode/        [C/Rust - Language]
├── L2-platform/        [C/Rust - OS layer]
├── L4-faithnet/        [Rust - Network]
├── L5-applications/    [C# - Apps]
│
└── interface/          [CLI/TUI/GUI - Display layer]
```

### Key Principle

> **CPI-SI is the paradigm. Claude Code is just a substrate.**
>
> The intelligence lives in `word/cpisi/`. Everything else is integration.

---

## Closing

This reorganization establishes a clean separation:

1. **CPI-SI Core** (`word/cpisi/`) - The universal intelligence paradigm
2. **Substrate Adapters** (`word/substrates/`) - Thin integration layers
3. **Foundation** (`word/L0-L5/`) - Language, platform, network
4. **Interface** (`word/interface/`) - Display layer

Claude Code becomes just ONE way to run CPI-SI. Tomorrow, the same paradigm runs natively on MillenniumOS with a different adapter.

> *"And the LORD God formed man of the dust of the ground, and breathed into his nostrils the breath of life; and man became a living soul."* — Genesis 2:7 KJV

The dust (substrate) provides the body. The breath (CPI-SI) provides the soul.

---

**Document Status**

| Field | Value |
|-------|-------|
| Key | B-void-planning-repo-reorganization |
| Version | 3.0.0 |
| Status | Active |
| Created | 2026-02-01 |
| Updated | 2026-02-01 |
| Authors | Seanje Lenox-Wise (Architect), Nova Dawn (Implementation) |
