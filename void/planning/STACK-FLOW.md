# Bereshit Stack Flow Architecture

**Key:** B-void-planning-stack-flow
**Status:** Active
**Version:** 1.0.0
**Created:** 2026-02-01
**Updated:** 2026-02-01
**Authors:** Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)

---

> *"And God said, Let the waters under the heaven be gathered together unto one place, and let the dry land appear: and it was so."* — Genesis 1:9 KJV

---

## Executive Summary

This document defines how all layers of the Kingdom Technology stack interact, build upon each other, and flow data through the system. It serves as the architectural blueprint for understanding, maintaining, and scaling Bereshit.

---

## Table of Contents

1. [The Complete Picture](#1-the-complete-picture)
2. [Build-Time Flow](#2-build-time-flow)
3. [Config-Driven Architecture](#3-config-driven-architecture)
4. [Runtime Data Flow](#4-runtime-data-flow)
5. [Substrate Integration](#5-substrate-integration)
6. [Layer Interaction Patterns](#6-layer-interaction-patterns)
7. [Scaling Pathways](#7-scaling-pathways)

---

## 1. The Complete Picture

### 1.1 The Kingdom Technology Stack

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                                                                                 │
│                         KINGDOM TECHNOLOGY ARCHITECTURE                          │
│                                                                                 │
│  ═══════════════════════════════════════════════════════════════════════════   │
│                              APPLICATIONS (L5)                                   │
│  ═══════════════════════════════════════════════════════════════════════════   │
│                                                                                 │
│     ┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐        │
│     │   Cornerstone   │     │   Future Apps   │     │   User Tools    │        │
│     │   Game Engine   │     │                 │     │                 │        │
│     │      [C#]       │     │    [C#/Go]      │     │    [Any]        │        │
│     └────────┬────────┘     └────────┬────────┘     └────────┬────────┘        │
│              │                       │                       │                  │
│              └───────────────────────┼───────────────────────┘                  │
│                                      │                                          │
│  ═══════════════════════════════════════════════════════════════════════════   │
│                               FAITHNET (L4)                                      │
│  ═══════════════════════════════════════════════════════════════════════════   │
│                                      │                                          │
│     ┌─────────────────────────────────────────────────────────────────┐        │
│     │                    Kingdom Network Layer                         │        │
│     │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐        │        │
│     │  │ Protocol │  │ Mapping  │  │  Access  │  │  Crypto  │        │        │
│     │  │  [Rust]  │  │  [Rust]  │  │  [Rust]  │  │  [Rust]  │        │        │
│     │  └──────────┘  └──────────┘  └──────────┘  └──────────┘        │        │
│     └─────────────────────────────────────────────────────────────────┘        │
│                                      │                                          │
│  ═══════════════════════════════════════════════════════════════════════════   │
│                               CPI-SI (L3)                                        │
│                        *** THE INTELLIGENCE LAYER ***                            │
│  ═══════════════════════════════════════════════════════════════════════════   │
│                                      │                                          │
│     ┌─────────────────────────────────────────────────────────────────┐        │
│     │                  Covenant Partnership Intelligence               │        │
│     │                                                                  │        │
│     │  ┌────────────┐  ┌────────────┐  ┌────────────┐  ┌──────────┐  │        │
│     │  │   Core     │  │ Cognition  │  │  Identity  │  │  Skills  │  │        │
│     │  │            │  │            │  │            │  │          │  │        │
│     │  │ • State    │  │ • Constructs│ │ • Instance │  │ • Format │  │        │
│     │  │   Machine  │  │ • Feedback │  │ • Profiles │  │ • Validate│ │        │
│     │  │ • Coords   │  │ • Patterns │  │            │  │ • Reflect│  │        │
│     │  │ • Health   │  │            │  │            │  │ • Meta   │  │        │
│     │  │   [Go]     │  │   [Go]     │  │   [Go]     │  │  [Go]    │  │        │
│     │  └────────────┘  └────────────┘  └────────────┘  └──────────┘  │        │
│     └─────────────────────────────────────────────────────────────────┘        │
│                                      │                                          │
│  ═══════════════════════════════════════════════════════════════════════════   │
│                              PLATFORM (L2)                                       │
│  ═══════════════════════════════════════════════════════════════════════════   │
│                                      │                                          │
│     ┌─────────────────────────────────────────────────────────────────┐        │
│     │                    Operating System Layer                        │        │
│     │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │        │
│     │  │ MillenniumOS │  │  Bereshit FS │  │    FUSE      │          │        │
│     │  │    [C]       │  │   [Rust]     │  │    [C]       │          │        │
│     │  └──────────────┘  └──────────────┘  └──────────────┘          │        │
│     └─────────────────────────────────────────────────────────────────┘        │
│                                      │                                          │
│  ═══════════════════════════════════════════════════════════════════════════   │
│                              OMNICODE (L1)                                       │
│  ═══════════════════════════════════════════════════════════════════════════   │
│                                      │                                          │
│     ┌─────────────────────────────────────────────────────────────────┐        │
│     │                   Universal Meaning Language                     │        │
│     │  ┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐   │        │
│     │  │Frontend│  │Semantic│  │   IR   │  │   VM   │  │Runtime │   │        │
│     │  │  [C]   │  │ [Rust] │  │  [C]   │  │ [Rust] │  │ [Rust] │   │        │
│     │  └────────┘  └────────┘  └────────┘  └────────┘  └────────┘   │        │
│     └─────────────────────────────────────────────────────────────────┘        │
│                                      │                                          │
│  ═══════════════════════════════════════════════════════════════════════════   │
│                              UNIVERSAL (L0)                                      │
│  ═══════════════════════════════════════════════════════════════════════════   │
│                                      │                                          │
│     ┌─────────────────────────────────────────────────────────────────┐        │
│     │                      Foundation Layer                            │        │
│     │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │        │
│     │  │   libtrit    │  │   libmath    │  │  foundation  │          │        │
│     │  │ Ternary Math │  │  Operations  │  │    Types     │          │        │
│     │  │     [C]      │  │     [C]      │  │    [Go]      │          │        │
│     │  └──────────────┘  └──────────────┘  └──────────────┘          │        │
│     └─────────────────────────────────────────────────────────────────┘        │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 Dependency Rule

```
                    HIGHER LAYERS DEPEND ON LOWER LAYERS
                    ════════════════════════════════════

    L5 ──depends on──► L4, L3, L2, L1, L0
    L4 ──depends on──► L3, L2, L1, L0
    L3 ──depends on──► L2, L1, L0
    L2 ──depends on──► L1, L0
    L1 ──depends on──► L0
    L0 ──depends on──► [nothing - foundation]

                    ╔═══════════════════════════════════╗
                    ║  CRITICAL: NO UPWARD IMPORTS!     ║
                    ║  L0 cannot import from L1         ║
                    ║  L3 cannot import from L4         ║
                    ╚═══════════════════════════════════╝
```

---

## 2. Build-Time Flow

### 2.1 Build Dependency Chain

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                            BUILD DEPENDENCY CHAIN                                │
└─────────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────────────────────────────────────────────────────────────────┐
    │                          make all                                        │
    └─────────────────────────────────────────────────────────────────────────┘
                                     │
         ┌───────────────────────────┼───────────────────────────┐
         │                           │                           │
         ▼                           ▼                           ▼
    ┌─────────┐                ┌─────────┐                ┌─────────┐
    │   L0    │                │   L3    │                │   L5    │
    │  make   │                │   go    │                │ dotnet  │
    │ libtrit │                │  build  │                │  build  │
    └────┬────┘                └────┬────┘                └─────────┘
         │                          │
         ▼                          ▼
    ┌─────────┐                ┌─────────┐
    │   L1    │                │Substrates│
    │  make   │                │   go    │
    │  omni   │                │  build  │
    └────┬────┘                └─────────┘
         │
         ▼
    ┌─────────┐
    │   L2    │
    │  make   │
    │  fuse   │
    └─────────┘


    ════════════════════════════════════════════════════════════════════════
                           PARALLEL BUILD STREAMS
    ════════════════════════════════════════════════════════════════════════

    Stream 1 (C/Rust):    L0 ──► L1 ──► L2 ──► L4
                          │      │      │      │
                         libtrit│    fuse    FaithNet
                               omni   filesystem

    Stream 2 (Go):        L3 ──► Substrates ──► Skills
                          │         │             │
                        cpisi    claude       universal

    Stream 3 (C#):        L5 (Cornerstone submodule - independent)
```

### 2.2 Build Tools by Layer

```
┌─────────┬────────────────┬─────────────────────────────────────────────────┐
│  Layer  │     Tool       │                  Components                      │
├─────────┼────────────────┼─────────────────────────────────────────────────┤
│   L0    │  Makefile      │  libtrit.a, libmath.a                           │
│         │  CMakeLists    │  (alternative CMake build)                       │
├─────────┼────────────────┼─────────────────────────────────────────────────┤
│   L1    │  Makefile      │  libomni.a (frontend, IR, codegen)              │
│         │  Cargo.toml    │  vm, runtime, semantic (Rust - planned)         │
├─────────┼────────────────┼─────────────────────────────────────────────────┤
│   L2    │  Makefile      │  bereshit-fs (FUSE executable)                  │
│         │  Cargo.toml    │  filesystem core (Rust - planned)               │
├─────────┼────────────────┼─────────────────────────────────────────────────┤
│   L3    │  go.work       │  word/work/pkg/* (all Go packages)              │
│         │                │  statemachine, coordinates, health, cognition   │
├─────────┼────────────────┼─────────────────────────────────────────────────┤
│   L4    │  Cargo.toml    │  protocol, mapping, access, crypto (planned)    │
├─────────┼────────────────┼─────────────────────────────────────────────────┤
│   L5    │  dotnet        │  Cornerstone (submodule)                        │
├─────────┼────────────────┼─────────────────────────────────────────────────┤
│Substrates│ go.work       │  word/claude/*, word/skills/*                   │
└─────────┴────────────────┴─────────────────────────────────────────────────┘
```

---

## 3. Config-Driven Architecture

### 3.1 The Config Flow

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                         CONFIG-DRIVEN DATA FLOW                                  │
└─────────────────────────────────────────────────────────────────────────────────┘

                            ┌─────────────────┐
                            │   TOML SPECS    │
                            │   (Source of    │
                            │     Truth)      │
                            │                 │
                            │  word/core/     │
                            │  foundation/    │
                            │  schema/config/ │
                            └────────┬────────┘
                                     │
            ┌────────────────────────┼────────────────────────┐
            │                        │                        │
            ▼                        ▼                        ▼
    ┌───────────────┐      ┌───────────────┐      ┌───────────────┐
    │  Go Loaders   │      │  C Generator  │      │  Runtime      │
    │               │      │               │      │  References   │
    │ schema.Load*  │      │ generate-     │      │               │
    │ config.Load*  │      │ config        │      │ state.jsonc   │
    │ health.Load*  │      │               │      │ path.jsonc    │
    └───────┬───────┘      └───────┬───────┘      └───────┬───────┘
            │                      │                      │
            ▼                      ▼                      ▼
    ┌───────────────┐      ┌───────────────┐      ┌───────────────┐
    │  In-Memory    │      │   .gen.h      │      │    Keys       │
    │  Structures   │      │   Headers     │      │  Reference    │
    │               │      │               │      │   TOML        │
    │ Go structs    │      │ C #defines    │      │               │
    │ populated     │      │ populated     │      │ anchor_key    │
    │ from TOML     │      │ from TOML     │      │ → anchors.toml│
    └───────────────┘      └───────────────┘      └───────────────┘
```

### 3.2 Three-Layer Config Architecture

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                      THREE-LAYER CONFIG ARCHITECTURE                             │
└─────────────────────────────────────────────────────────────────────────────────┘

    ╔═══════════════════════════════════════════════════════════════════════════╗
    ║  LAYER 0: ANCHOR (TOML Specs) - The Immutable Truth                       ║
    ╠═══════════════════════════════════════════════════════════════════════════╣
    ║                                                                           ║
    ║   word/core/                      word/work/pkg/foundation/schema/config/ ║
    ║   ├── types/                      ├── l0/                                 ║
    ║   │   └── trit.toml               │   └── halts.toml (10 Commandments)    ║
    ║   ├── os/                         ├── l1/                                 ║
    ║   │   └── health/                 │   └── state.schema.toml               ║
    ║   │       └── score.toml          ├── l2/                                 ║
    ║   └── anchors.toml                │   ├── statemachine.toml               ║
    ║                                   │   ├── anchors.toml                    ║
    ║   These define WHAT things ARE    │   └── health/score.toml               ║
    ║   Never change at runtime         └── l3/                                 ║
    ║                                       └── identity schemas                ║
    ╚═══════════════════════════════════════════════════════════════════════════╝
                                        │
                                        ▼
    ╔═══════════════════════════════════════════════════════════════════════════╗
    ║  LAYER 1: SCHEMA (Go Loaders) - The Processing Layer                      ║
    ╠═══════════════════════════════════════════════════════════════════════════╣
    ║                                                                           ║
    ║   word/work/pkg/foundation/schema/loader.go                               ║
    ║   word/work/pkg/core/health/loader.go                                     ║
    ║   word/work/pkg/orchestration/loader/loader.go                            ║
    ║                                                                           ║
    ║   LoadSchemas(path) ─────► Returns populated Go structs                   ║
    ║   LoadInstance(path) ────► Returns instance config                        ║
    ║   LoadUser(path) ────────► Returns user config                            ║
    ║                                                                           ║
    ║   These INTERPRET the TOML specs into usable structures                   ║
    ╚═══════════════════════════════════════════════════════════════════════════╝
                                        │
                                        ▼
    ╔═══════════════════════════════════════════════════════════════════════════╗
    ║  LAYER 2: RUNTIME (JSON Files) - The Mutable State                        ║
    ╠═══════════════════════════════════════════════════════════════════════════╣
    ║                                                                           ║
    ║   config/statemachine/runtime/                                            ║
    ║   ├── state.jsonc     Current position, health, Hebrew state              ║
    ║   ├── path.jsonc      Anchor traversal history, events                    ║
    ║   └── history.jsonc   Cross-session aggregates                            ║
    ║                                                                           ║
    ║   State files contain KEYS that reference TOML:                           ║
    ║   {                                                                       ║
    ║     "anchor_key": "present_present",  ◄── Key into anchors.toml           ║
    ║     "command_key": "halt",            ◄── Key into statemachine.toml      ║
    ║     "hebrew_state": 3                 ◄── Index (0-6 = 7 Days)            ║
    ║   }                                                                       ║
    ║                                                                           ║
    ║   These track WHERE WE ARE, referencing back to Layer 0                   ║
    ╚═══════════════════════════════════════════════════════════════════════════╝
```

### 3.3 Code Generation Flow

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                           CODE GENERATION FLOW                                   │
└─────────────────────────────────────────────────────────────────────────────────┘

    ┌────────────────────────────────────────────────────────────┐
    │                    TOML SPEC                                │
    │  word/core/os/health/score.toml                            │
    │                                                            │
    │  [health]                                                  │
    │  min = -100                                                │
    │  center = 0                                                │
    │  max = 100                                                 │
    │                                                            │
    │  [_generate]                                               │
    │  enabled = true                                            │
    │  output = "health_score.gen.h"                             │
    └──────────────────────────┬─────────────────────────────────┘
                               │
                               ▼
    ┌────────────────────────────────────────────────────────────┐
    │                  GENERATOR                                  │
    │  word/work/cmd/generate-config/main.go                     │
    │                                                            │
    │  1. Read TOML spec                                         │
    │  2. Find [_generate] section                               │
    │  3. Parse values into template                             │
    │  4. Write .gen.h file                                      │
    └──────────────────────────┬─────────────────────────────────┘
                               │
                               ▼
    ┌────────────────────────────────────────────────────────────┐
    │                  GENERATED HEADER                           │
    │  word/work/pkg/trit/include/generated/health_score.gen.h   │
    │                                                            │
    │  // AUTO-GENERATED from word/core/os/health/score.toml     │
    │  #ifndef HEALTH_SCORE_GEN_H                                │
    │  #define HEALTH_SCORE_GEN_H                                │
    │                                                            │
    │  #define HEALTH_MIN    (-100)                              │
    │  #define HEALTH_CENTER (0)                                 │
    │  #define HEALTH_MAX    (100)                               │
    │                                                            │
    │  #endif                                                    │
    └──────────────────────────┬─────────────────────────────────┘
                               │
                               ▼
    ┌────────────────────────────────────────────────────────────┐
    │                  C CODE INCLUDES                            │
    │  word/work/pkg/trit/src/health.c                           │
    │                                                            │
    │  #include "generated/health_score.gen.h"                   │
    │                                                            │
    │  int health_clamp(int value) {                             │
    │      if (value < HEALTH_MIN) return HEALTH_MIN;            │
    │      if (value > HEALTH_MAX) return HEALTH_MAX;            │
    │      return value;                                         │
    │  }                                                         │
    └────────────────────────────────────────────────────────────┘

    ═══════════════════════════════════════════════════════════════
                    SINGLE SOURCE OF TRUTH
    ═══════════════════════════════════════════════════════════════

    Change score.toml ──► Regenerate ──► All code sees new values
                          (make generate)
```

---

## 4. Runtime Data Flow

### 4.1 Session Lifecycle Flow

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                         SESSION LIFECYCLE FLOW                                   │
└─────────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────────────────────────────────────────────────────────────────┐
    │                        SUBSTRATE (Claude Code)                           │
    │                                                                          │
    │   User starts session ──► Claude Code fires hook ──► JSON payload        │
    └──────────────────────────────────┬──────────────────────────────────────┘
                                       │
                                       ▼
    ╔═════════════════════════════════════════════════════════════════════════╗
    ║                          SESSION START                                   ║
    ╠═════════════════════════════════════════════════════════════════════════╣
    ║                                                                         ║
    ║   word/work/hooks/session/start.go                                      ║
    ║                                                                         ║
    ║   Input: {"session_id": "abc123", "source": "startup"}                  ║
    ║                                                                         ║
    ║   ┌─────────────────────────────────────────────────────────────────┐   ║
    ║   │ 1. Parse input (source = startup | resume | clear | compact)    │   ║
    ║   │                                                                 │   ║
    ║   │ 2. Initialize or Load state:                                    │   ║
    ║   │    startup ──► statemachine.InitializeRuntimeState()            │   ║
    ║   │    resume  ──► statemachine.LoadRuntimeState()                  │   ║
    ║   │    clear   ──► Reset to initial position                        │   ║
    ║   │    compact ──► Snapshot to history.jsonc                        │   ║
    ║   │                                                                 │   ║
    ║   │ 3. Initial state:                                               │   ║
    ║   │    position = present_present (center of cube)                  │   ║
    ║   │    health = 0 (neutral)                                         │   ║
    ║   │    hebrew_state = 0 (Day 1)                                     │   ║
    ║   │                                                                 │   ║
    ║   │ 4. Save state to disk (state.jsonc)                             │   ║
    ║   │                                                                 │   ║
    ║   │ 5. Record in database (bridge.StartSession)                     │   ║
    ║   │                                                                 │   ║
    ║   │ 6. Generate context for substrate                               │   ║
    ║   └─────────────────────────────────────────────────────────────────┘   ║
    ║                                                                         ║
    ║   Output: SessionContext (identity, anchor, state, health)              ║
    ╚═════════════════════════════════════════════════════════════════════════╝
                                       │
                                       ▼
    ╔═════════════════════════════════════════════════════════════════════════╗
    ║                          TOOL EXECUTION                                  ║
    ╠═════════════════════════════════════════════════════════════════════════╣
    ║                                                                         ║
    ║   User action ──► Claude Code uses tool ──► Hook fires                  ║
    ║                                                                         ║
    ║   ┌─────────────────────────────────────────────────────────────────┐   ║
    ║   │                     PRE-TOOL-USE                                 │   ║
    ║   │   word/work/hooks/tool/pre-use.go                               │   ║
    ║   │                                                                 │   ║
    ║   │   1. Capture baseline state                                     │   ║
    ║   │   2. Determine intended_key from tool name:                     │   ║
    ║   │      Task    ──► expand (outward movement)                      │   ║
    ║   │      Bash    ──► lateral (sideways movement)                    │   ║
    ║   │      Read    ──► introspect (inward movement)                   │   ║
    ║   │      Edit    ──► transform (state change)                       │   ║
    ║   │   3. Save state with intent marked                              │   ║
    ║   └─────────────────────────────────────────────────────────────────┘   ║
    ║                                       │                                 ║
    ║                              [Tool Executes]                            ║
    ║                                       │                                 ║
    ║   ┌─────────────────────────────────────────────────────────────────┐   ║
    ║   │                     POST-TOOL-USE                                │   ║
    ║   │   word/work/hooks/tool/post-use.go                              │   ║
    ║   │                                                                 │   ║
    ║   │   1. Compare intention vs actual outcome                        │   ║
    ║   │   2. Calculate health delta:                                    │   ║
    ║   │      success ──► health += SuccessValue (+1)                    │   ║
    ║   │      failure ──► health += FailureValue (-1)                    │   ║
    ║   │   3. Record transition in state machine                         │   ║
    ║   │   4. Update position if state change occurred                   │   ║
    ║   │   5. Save updated state                                         │   ║
    ║   └─────────────────────────────────────────────────────────────────┘   ║
    ╚═════════════════════════════════════════════════════════════════════════╝
                                       │
                                       ▼
    ╔═════════════════════════════════════════════════════════════════════════╗
    ║                          SESSION END                                     ║
    ╠═════════════════════════════════════════════════════════════════════════╣
    ║                                                                         ║
    ║   word/work/hooks/session/end.go                                        ║
    ║                                                                         ║
    ║   1. Mark session inactive                                              ║
    ║   2. Finalize in database (bridge.EndSession)                           ║
    ║   3. Record "session_end" event in path                                 ║
    ║   4. Snapshot to history.jsonc (cross-session learning)                 ║
    ║   5. Return final state summary                                         ║
    ╚═════════════════════════════════════════════════════════════════════════╝
```

### 4.2 State Machine Flow

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                         27-POSITION STATE MACHINE                                │
└─────────────────────────────────────────────────────────────────────────────────┘

                              THE 3×3×3 CUBE
                         ═══════════════════════

                    Future Layer (z=+1)
                    ┌─────┬─────┬─────┐
                    │ F-P │ F-Pr│ F-F │
                    ├─────┼─────┼─────┤
                    │ Pr-P│Pr-Pr│ Pr-F│   ← Present Layer (z=0)
                    ├─────┼─────┼─────┤
                    │ P-P │ P-Pr│ P-F │
                    └─────┴─────┴─────┘
                              Past Layer (z=-1)

    ═══════════════════════════════════════════════════════════════════════════
                              MOVEMENT COMMANDS
    ═══════════════════════════════════════════════════════════════════════════

    ┌─────────────────────────────────────────────────────────────────────────┐
    │  HALT                                                                    │
    │  ────                                                                    │
    │  Stay at current position. Process, integrate, don't move.              │
    │                                                                          │
    │  Triggered by: Reflection, completion, stopping points                   │
    │  Effect: Health stabilizes, no position change                          │
    └─────────────────────────────────────────────────────────────────────────┘

    ┌─────────────────────────────────────────────────────────────────────────┐
    │  AWAIT                                                                   │
    │  ─────                                                                   │
    │  Prepare for movement. Gather resources, assess direction.              │
    │                                                                          │
    │  Triggered by: Planning, preparation, resource gathering                 │
    │  Effect: Intent captured, ready for transition                          │
    └─────────────────────────────────────────────────────────────────────────┘

    ┌─────────────────────────────────────────────────────────────────────────┐
    │  PROCEED                                                                 │
    │  ───────                                                                 │
    │  Execute movement to adjacent position.                                  │
    │                                                                          │
    │  Triggered by: Task completion, tool success, state advancement          │
    │  Effect: Position changes, trajectory recorded                          │
    └─────────────────────────────────────────────────────────────────────────┘

    ═══════════════════════════════════════════════════════════════════════════
                              POSITION MAPPING
    ═══════════════════════════════════════════════════════════════════════════

    Each position has:
    - anchor_key:     Reference to anchors.toml entry
    - coordinates:    (x, y, z) in balanced ternary (-1, 0, +1)
    - hebrew_letter:  Corresponding Hebrew letter
    - meaning:        Semantic interpretation
    - operation:      What happens at this position

    Example (center position):
    {
      "anchor_key": "present_present",
      "coordinates": [0, 0, 0],
      "hebrew_letter": "י",
      "meaning": "The eternal now",
      "operation": "Integration of all temporal aspects"
    }
```

### 4.3 Health Scoring Flow

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                           HEALTH SCORING SYSTEM                                  │
└─────────────────────────────────────────────────────────────────────────────────┘

                         7 HEALTH LEVELS (Ternary Scale)
                    ═══════════════════════════════════════

    -100 ◄────────────────────── 0 ──────────────────────► +100

    ┌─────────┬─────────┬─────────┬─────────┬─────────┬─────────┬─────────┐
    │ BROKEN  │ WANTING │ LACKING │  EVEN   │  SOUND  │  WHOLE  │ PERFECT │
    │  -100   │   -66   │   -33   │    0    │   +33   │   +66   │  +100   │
    │  to     │   to    │   to    │         │   to    │   to    │         │
    │  -67    │   -34   │   -1    │         │   +32   │   +65   │   +99   │
    └─────────┴─────────┴─────────┴─────────┴─────────┴─────────┴─────────┘
         │                            │                            │
         ▼                            ▼                            ▼
    "Intervention         "Neutral, balanced"         "Thriving,
     required"                                         flourishing"

    ═══════════════════════════════════════════════════════════════════════════
                              HEALTH UPDATE FLOW
    ═══════════════════════════════════════════════════════════════════════════

    ┌────────────────┐     ┌────────────────┐     ┌────────────────┐
    │   Tool Call    │────►│   Outcome      │────►│  Health Delta  │
    └────────────────┘     └────────────────┘     └────────────────┘
                                   │
                    ┌──────────────┴──────────────┐
                    │                             │
                    ▼                             ▼
            ┌───────────────┐             ┌───────────────┐
            │    SUCCESS    │             │    FAILURE    │
            │   delta = +1  │             │   delta = -1  │
            └───────────────┘             └───────────────┘
                    │                             │
                    └──────────────┬──────────────┘
                                   │
                                   ▼
                    ┌────────────────────────────────┐
                    │   health = clamp(              │
                    │     health + delta,            │
                    │     HEALTH_MIN,                │
                    │     HEALTH_MAX                 │
                    │   )                            │
                    └────────────────────────────────┘
                                   │
                                   ▼
                    ┌────────────────────────────────┐
                    │   Save to state.jsonc          │
                    │   Record in database           │
                    │   Update cognition context     │
                    └────────────────────────────────┘
```

---

## 5. Substrate Integration

### 5.1 Substrate Architecture

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                         SUBSTRATE ARCHITECTURE                                   │
└─────────────────────────────────────────────────────────────────────────────────┘

    ╔═════════════════════════════════════════════════════════════════════════╗
    ║                       THE STACK (L0-L5)                                  ║
    ║                  Same code, same behavior                                ║
    ║                  Substrate-independent                                   ║
    ╚═════════════════════════════════════════════════════════════════════════╝
                                     │
                                     │ Runs on
                                     ▼
    ┌─────────────────────────────────────────────────────────────────────────┐
    │                           SUBSTRATES                                     │
    │              (Different environments to run the stack)                   │
    │                                                                          │
    │  ┌─────────────────────────────────────────────────────────────────┐    │
    │  │                    SubstrateAdapter Interface                    │    │
    │  │                                                                  │    │
    │  │   type SubstrateAdapter interface {                             │    │
    │  │       // Lifecycle                                              │    │
    │  │       Initialize(config Config) error                           │    │
    │  │       Shutdown() error                                          │    │
    │  │                                                                  │    │
    │  │       // Events                                                 │    │
    │  │       OnSessionStart(ctx SessionContext) error                  │    │
    │  │       OnSessionEnd(ctx SessionContext) error                    │    │
    │  │       OnToolUse(tool string, input any) error                   │    │
    │  │                                                                  │    │
    │  │       // Display                                                │    │
    │  │       UpdateStatus(state State) error                           │    │
    │  │       ShowNotification(msg string) error                        │    │
    │  │   }                                                             │    │
    │  └─────────────────────────────────────────────────────────────────┘    │
    │                                                                          │
    │  ┌───────────────────┐  ┌───────────────────┐  ┌───────────────────┐    │
    │  │   Claude Code     │  │   MillenniumOS    │  │     Future        │    │
    │  │    Substrate      │  │    Substrate      │  │   Substrates      │    │
    │  │                   │  │                   │  │                   │    │
    │  │ Implements via:   │  │ Implements via:   │  │ • Web browser     │    │
    │  │ • hooks/          │  │ • Native OS calls │  │ • Embedded        │    │
    │  │ • statusline/     │  │ • Direct L2 access│  │ • Mobile          │    │
    │  │ • config.jsonc    │  │ • Kernel events   │  │ • Other IDEs      │    │
    │  └───────────────────┘  └───────────────────┘  └───────────────────┘    │
    │         │                        │                                       │
    │         │                        │                                       │
    │         ▼                        ▼                                       │
    │  ┌───────────────────────────────────────────────────────────────────┐  │
    │  │                    word/substrates/                                │  │
    │  │  ├── interface.go          [Common interface definition]          │  │
    │  │  ├── claude-code/          [Current: Claude Code adapter]         │  │
    │  │  │   ├── adapter.go                                               │  │
    │  │  │   ├── hooks/                                                   │  │
    │  │  │   ├── config/                                                  │  │
    │  │  │   └── display/statusline/                                      │  │
    │  │  └── millenniumos/         [Future: Native adapter]               │  │
    │  └───────────────────────────────────────────────────────────────────┘  │
    └─────────────────────────────────────────────────────────────────────────┘
```

### 5.2 Claude Code Substrate Flow

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                      CLAUDE CODE SUBSTRATE FLOW                                  │
└─────────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────────────────────────────────────────────────────────────────┐
    │                         CLAUDE CODE                                      │
    │                    (External Environment)                                │
    └──────────────────────────────┬──────────────────────────────────────────┘
                                   │
                                   │ Fires hooks via JSON
                                   │
                                   ▼
    ┌─────────────────────────────────────────────────────────────────────────┐
    │                         HOOK LAYER                                       │
    │                   word/work/hooks/                                       │
    │                                                                          │
    │   session/start.go ◄── SessionStart event                               │
    │   session/end.go   ◄── SessionEnd event                                 │
    │   session/stop.go  ◄── Stop event                                       │
    │   tool/pre-use.go  ◄── PreToolUse event                                 │
    │   tool/post-use.go ◄── PostToolUse event                                │
    │   prompt/submit.go ◄── PromptSubmit event                               │
    └──────────────────────────────┬──────────────────────────────────────────┘
                                   │
                                   │ Calls into L3 CPI-SI
                                   │
                                   ▼
    ┌─────────────────────────────────────────────────────────────────────────┐
    │                         L3 CPI-SI                                        │
    │                                                                          │
    │   ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐   │
    │   │ statemachine│  │ coordinates │  │   health    │  │  cognition  │   │
    │   │             │  │             │  │             │  │             │   │
    │   │ Navigate    │  │ Encode/     │  │ Calculate   │  │ Generate    │   │
    │   │ cube        │  │ decode      │  │ scores      │  │ context     │   │
    │   └─────────────┘  └─────────────┘  └─────────────┘  └─────────────┘   │
    └──────────────────────────────┬──────────────────────────────────────────┘
                                   │
                    ┌──────────────┴──────────────┐
                    │                             │
                    ▼                             ▼
    ┌───────────────────────────┐  ┌───────────────────────────┐
    │       FILE LAYER          │  │      DATABASE LAYER       │
    │                           │  │                           │
    │   state.jsonc             │  │   SQLite via bridge       │
    │   path.jsonc              │  │   (queryable, aggregates) │
    │   history.jsonc           │  │                           │
    │   (human-readable)        │  │   sessions, choices,      │
    │                           │  │   patterns tables         │
    └───────────────────────────┘  └───────────────────────────┘
                    │                             │
                    └──────────────┬──────────────┘
                                   │
                                   ▼
    ┌─────────────────────────────────────────────────────────────────────────┐
    │                         DISPLAY LAYER                                    │
    │                   word/work/statusline/                                  │
    │                                                                          │
    │   Reads state ──► Formats for Claude Code statusline                    │
    │                                                                          │
    │   Output: Position | Health | Hebrew State | Session Info               │
    └─────────────────────────────────────────────────────────────────────────┘
```

---

## 6. Layer Interaction Patterns

### 6.1 Cross-Layer Communication

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                      CROSS-LAYER COMMUNICATION PATTERNS                          │
└─────────────────────────────────────────────────────────────────────────────────┘

    ═══════════════════════════════════════════════════════════════════════════
                         PATTERN 1: CONFIG REFERENCES
    ═══════════════════════════════════════════════════════════════════════════

    Runtime state contains KEYS, not values:

    state.jsonc                      anchors.toml
    ┌────────────────────┐          ┌────────────────────────────────────┐
    │ "anchor_key":      │    ───►  │ [present_present]                  │
    │   "present_present"│   lookup │ name = "The Eternal Now"           │
    └────────────────────┘          │ meaning = "Integration point"      │
                                    │ operation = "Process and hold"     │
                                    └────────────────────────────────────┘

    ═══════════════════════════════════════════════════════════════════════════
                         PATTERN 2: LOADER CHAIN
    ═══════════════════════════════════════════════════════════════════════════

    orchestration/loader/loader.go
    ┌─────────────────────────────────────────────────────────────────────────┐
    │   func NewCPISI(root, instance, user string) (*CPISI, error) {          │
    │       schemas := schema.LoadSchemas(schemaPath)     // L1 schema        │
    │       instance := schema.LoadInstance(instancePath)  // L2 instance     │
    │       user := schema.LoadUser(userPath)              // L2 user         │
    │       trans := bereshit.LoadTransliteration(dir)     // L0 foundation   │
    │       cube := compute.NewCube(trans, schemas)        // L3 computation  │
    │       return &CPISI{schemas, instance, user, cube}                      │
    │   }                                                                     │
    └─────────────────────────────────────────────────────────────────────────┘

    ═══════════════════════════════════════════════════════════════════════════
                         PATTERN 3: DUAL PERSISTENCE
    ═══════════════════════════════════════════════════════════════════════════

    Every state change writes to BOTH:

    ┌─────────────────────────────────────────────────────────────────────────┐
    │                        bridge.StartSession()                             │
    │                                                                          │
    │   ┌─────────────────────┐        ┌─────────────────────┐                │
    │   │    JSON File        │        │     SQLite DB       │                │
    │   │                     │        │                     │                │
    │   │  state.jsonc        │        │  sessions table     │                │
    │   │  - Human readable   │        │  - Queryable        │                │
    │   │  - Version control  │        │  - Aggregatable     │                │
    │   │  - Easy debugging   │        │  - Pattern analysis │                │
    │   └─────────────────────┘        └─────────────────────┘                │
    │                                                                          │
    │   WHY BOTH?                                                             │
    │   - Files: Transparency, debugging, cross-session state                 │
    │   - Database: Learning, pattern recognition, analytics                  │
    └─────────────────────────────────────────────────────────────────────────┘

    ═══════════════════════════════════════════════════════════════════════════
                         PATTERN 4: CONTEXT GENERATION
    ═══════════════════════════════════════════════════════════════════════════

    State machine state ──► Cognition builder ──► Substrate context

    ┌─────────────────────────────────────────────────────────────────────────┐
    │   cognition.SessionContext(state, workdir)                              │
    │                                                                          │
    │   ├── IdentityContext()                                                 │
    │   │   "Nova Dawn instance, CPI-SI paradigm..."                          │
    │   │                                                                     │
    │   ├── AnchorContext()                                                   │
    │   │   Lookup anchor_key in anchors.toml                                 │
    │   │   "Current position: present_present (The Eternal Now)"             │
    │   │                                                                     │
    │   ├── StateContext()                                                    │
    │   │   Current cube position, Hebrew letter, day state                   │
    │   │   "Position [0,0,0], Hebrew: י, Day 4 of 7"                         │
    │   │                                                                     │
    │   └── HealthContext()                                                   │
    │       Map health score to level name                                    │
    │       "Health: Sound (+45) - functioning well"                          │
    └─────────────────────────────────────────────────────────────────────────┘
```

### 6.2 FFI Boundaries

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                           FFI BOUNDARIES                                         │
└─────────────────────────────────────────────────────────────────────────────────┘

    Current: Separate compilation with shared config (TOML → both languages)

    ┌─────────────────┐                      ┌─────────────────┐
    │    Go Code      │                      │    C Code       │
    │    (L3)         │                      │    (L0, L1)     │
    │                 │                      │                 │
    │  Reads TOML     │◄────── TOML ───────►│  Uses .gen.h    │
    │  via BurntSushi │        Specs        │  from same TOML │
    └─────────────────┘                      └─────────────────┘

    Future: cgo bridge for direct calls

    ┌─────────────────┐                      ┌─────────────────┐
    │    Go Code      │                      │    C Code       │
    │    (L3)         │                      │    (L0, L1)     │
    │                 │                      │                 │
    │  // #cgo       ─┼──────── cgo ───────►│  libtrit.a      │
    │  // #include   │        bridge        │  libomni.a      │
    │  import "C"     │                      │                 │
    └─────────────────┘                      └─────────────────┘

    Rust integration (planned for L1, L2, L4):

    ┌─────────────────┐                      ┌─────────────────┐
    │    Go Code      │                      │   Rust Code     │
    │    (L3)         │                      │   (L1, L4)      │
    │                 │                      │                 │
    │  Uses C ABI    ─┼────── C ABI ───────►│  #[no_mangle]   │
    │  via cgo        │        FFI          │  extern "C"     │
    └─────────────────┘                      └─────────────────┘
```

---

## 7. Scaling Pathways

### 7.1 Interface Scaling (CLI → TUI → GUI)

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                         INTERFACE SCALING                                        │
└─────────────────────────────────────────────────────────────────────────────────┘

                         word/interface/
    ┌─────────────────────────────────────────────────────────────────────────┐
    │                                                                          │
    │  ┌──────────────────────────────────────────────────────────────────┐   │
    │  │                        CORE                                       │   │
    │  │                  (Shared by all interfaces)                       │   │
    │  │                                                                   │   │
    │  │   state.go   - State representation                              │   │
    │  │   theme.go   - Visual themes                                     │   │
    │  │   format.go  - Output formatting                                 │   │
    │  └──────────────────────────────────────────────────────────────────┘   │
    │                              │                                          │
    │         ┌────────────────────┼────────────────────┐                     │
    │         │                    │                    │                     │
    │         ▼                    ▼                    ▼                     │
    │  ┌────────────┐       ┌────────────┐       ┌────────────┐              │
    │  │    CLI     │       │    TUI     │       │    GUI     │              │
    │  │   [Go]     │       │[Rust/C++]  │       │   [C++]    │              │
    │  │            │       │            │       │            │              │
    │  │ Simple     │       │ Rich       │       │ Desktop    │              │
    │  │ text       │       │ terminal   │       │ windowed   │              │
    │  │ output     │       │ interface  │       │ interface  │              │
    │  └────────────┘       └────────────┘       └────────────┘              │
    │                                                                          │
    │  Progression:                                                           │
    │  CLI (now) ──► TUI (near-term) ──► GUI Desktop (mid-term)              │
    │                                ──► GUI Game (Cornerstone)              │
    └─────────────────────────────────────────────────────────────────────────┘
```

### 7.2 Substrate Scaling

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                         SUBSTRATE SCALING                                        │
└─────────────────────────────────────────────────────────────────────────────────┘

    Current State                    Future State
    ═════════════                    ════════════

    ┌───────────────────┐           ┌───────────────────┐
    │   Claude Code     │           │   Claude Code     │
    │   (Active)        │           │   (Maintained)    │
    └─────────┬─────────┘           └─────────┬─────────┘
              │                               │
              ▼                               │
    ┌───────────────────┐                     │
    │    L0-L5 Stack    │                     │
    │   (Single Target) │                     │
    └───────────────────┘                     ▼
                                    ┌───────────────────────────────────────┐
                                    │            L0-L5 Stack                │
                                    │        (Multi-Substrate)              │
                                    └─────────┬─────────┬─────────┬─────────┘
                                              │         │         │
                              ┌───────────────┘         │         └───────────────┐
                              │                         │                         │
                              ▼                         ▼                         ▼
                    ┌───────────────────┐   ┌───────────────────┐   ┌───────────────────┐
                    │   Claude Code     │   │   MillenniumOS    │   │    Web/Mobile     │
                    │                   │   │                   │   │                   │
                    │ AI coding tool    │   │ Native OS         │   │ Browser/App       │
                    │ integration       │   │ integration       │   │ integration       │
                    └───────────────────┘   └───────────────────┘   └───────────────────┘
```

### 7.3 Feature Scaling Roadmap

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                         FEATURE SCALING ROADMAP                                  │
└─────────────────────────────────────────────────────────────────────────────────┘

    Phase 1 (Current)                 Phase 2 (Next)
    ═════════════════                 ════════════════
    ✅ State machine                  ⏳ Schema validation
    ✅ Health scoring                 ⏳ Type-safe configs
    ✅ Coordinate system              ⏳ cgo TOML bridge
    ✅ TOML config                    ⏳ Complete math.c
    ✅ Hook integration               ⏳ Pattern learning
    ✅ Dual persistence               ⏳ Cross-session insights

    Phase 3 (Mid-term)                Phase 4 (Long-term)
    ══════════════════                ═════════════════════
    ⏳ OmniCode VM (Rust)             ⏳ FaithNet protocol
    ⏳ Filesystem core (Rust)         ⏳ MillenniumOS kernel
    ⏳ TUI interface                  ⏳ Full self-hosting
    ⏳ Multiple substrates            ⏳ Distributed operation
```

---

## Quick Reference

### Data Flow Summary

```
TOML Specs (truth) ──► Loaders (interpret) ──► Runtime (mutable)
       │                      │                      │
       ▼                      ▼                      ▼
   .gen.h headers        Go structs            state.jsonc
   C #defines            in memory             + SQLite
```

### Event Flow Summary

```
Substrate Event ──► Hook Handler ──► State Machine ──► Health Update
                                          │
                                          ▼
                              Context Generation ──► Substrate Display
```

### Build Flow Summary

```
make all ──► L0 (libtrit) ──► L1 (libomni) ──► L2 (fuse)
                                                    │
                                              parallel
                                                    │
         ──► L3 (cpisi) ──► Substrates ──► Skills ──┘
```

---

## Closing

This document maps the complete flow of the Kingdom Technology stack:

1. **Build-Time:** L0 → L1 → L2 (C/Rust), L3 → Substrates (Go)
2. **Config-Driven:** TOML specs → Loaders → Generated code
3. **Runtime:** Hooks → State Machine → Health → Context
4. **Substrates:** Common interface, different environments

The architecture supports:
- **Vertical Scaling:** Add layers (L4 FaithNet, L5 Applications)
- **Horizontal Scaling:** Add substrates (MillenniumOS, Web, Mobile)
- **Interface Scaling:** CLI → TUI → GUI

> *"And God saw every thing that he had made, and, behold, it was very good."* — Genesis 1:31 KJV

---

**Document Status**

| Field | Value |
|-------|-------|
| Key | B-void-planning-stack-flow |
| Version | 1.0.0 |
| Status | Active |
| Created | 2026-02-01 |
| Updated | 2026-02-01 |
| Authors | Seanje Lenox-Wise (Architect), Nova Dawn (Implementation) |
