# Bereshit Repository Reorganization Plan

**Key:** B-void-planning-repo-reorganization
**Status:** Active
**Created:** 2026-02-01
**Authors:** Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)

---

> *"In the beginning God created the heaven and the earth."* — Genesis 1:1 KJV

---

## Executive Summary

This document provides a comprehensive plan to reorganize the Bereshit repository into a clear, layered structure with **tov/**, **void/**, and **work/** as the three main folders, following the flow:

```
Universal Code (Libraries) → OmniCode → OS/Filesystem → CPI-SI → FaithNet → Applications
```

This reorganization:
1. Aligns the codebase with the Kingdom Technology layer architecture
2. Resolves the current word/work naming confusion
3. Properly positions all submodules
4. Creates clear separation of concerns
5. Enables better debugging and maintenance

---

## Table of Contents

1. [Current State Analysis](#1-current-state-analysis)
2. [Target State Vision](#2-target-state-vision)
3. [The Layer Flow Architecture](#3-the-layer-flow-architecture)
4. [Detailed Reorganization Map](#4-detailed-reorganization-map)
5. [Root Files Reorganization](#5-root-files-reorganization)
6. [Submodule Handling](#6-submodule-handling)
7. [Migration Strategy](#7-migration-strategy)
8. [Validation Checklist](#8-validation-checklist)

---

## 1. Current State Analysis

### 1.1 Current Top-Level Structure

```
bereshit/                          [ROOT]
├── cornerstone/                   [Submodule - Game Engine]
├── tov/                          [Production/Output]
├── void/                         [Planning/Void Space]
├── word/                         [CONFUSING - Contains definitions AND work/]
│   ├── claude/                   [Claude Code Production Config]
│   ├── core/                     [Core Definitions]
│   ├── work/                     [NESTED - Should be top-level]
│   │   └── pkg/                  [Libraries]
│   └── ...
├── .vscode/                      [IDE Config]
├── .git*, go.work, LICENSE, etc. [Root Files]
```

### 1.2 Current Problems

| Problem | Impact | Resolution |
|---------|--------|------------|
| `word/` contains both definitions AND code (`word/work/`) | Unclear separation | Split into separate top-level folders |
| `word/claude/` duplicates `word/work/` packages | Maintenance burden | Single source of truth |
| `cornerstone/` at root instead of under applications | Architecture mismatch | Move to proper layer |
| No clear FaithNet location | Missing layer | Create proper location |
| Layer architecture not visible in folder structure | Hard to navigate | Restructure by layers |

### 1.3 Current Content Inventory

#### Root Files
- `.gitignore` - Repository hygiene
- `.gitmodules` - Submodule tracking (cornerstone)
- `.gitmessage` - Commit message template
- `.health` - Health status marker
- `go.work` / `go.work.sum` - Go workspace config
- `LICENSE` - 3-tier license structure
- `SUBMODULES.md` - Submodule documentation
- `root.adoc` / `root.omni` - Foundation docs

#### void/ (Planning)
- `planning/` - Development plans, devlogs, Claude-global plans
- `workflow/` - Workflow definitions

#### tov/ (Production/Output)
- `data/` - Runtime data storage
- `demo/` - Phase demonstrations
- `licenses/` - License implementations
- `pricing/` - Commercial pricing

#### word/ (Definitions + Work - THE PROBLEM)
- `agents/` - Claude agent definitions
- `claude/` - Production Claude Code config (duplicates work/)
- `core/` - Core type/schema definitions
- `glossary/` - Term definitions
- `instance/` - Instance configurations
- `journals/` - Bible study journals
- `journey/` - Project journey phases
- `omni/` - OmniCode configuration
- `research/` - Research documents
- `scripture/` - Bible text databases
- `seed/` - Templates
- `skills/` - Skill definitions
- `work/` - **SHOULD BE TOP-LEVEL** - Contains all implementation code

---

## 2. Target State Vision

### 2.1 New Top-Level Structure

```
bereshit/                          [ROOT - Origin]
│
├── void/                          [ENTRY GATE - Planning & Emptiness]
│   ├── planning/                  [Development plans, devlogs]
│   ├── imports/                   [External imports awaiting processing]
│   └── workflow/                  [Workflow definitions]
│
├── work/                          [WORD - Definitions & Implementations]
│   │
│   ├── L0-universal/              [LAYER 0: Universal Libraries]
│   │   ├── libtrit/               [Ternary library (C)]
│   │   ├── libmath/               [Mathematical foundations]
│   │   └── foundation/            [Go foundation packages]
│   │
│   ├── L1-omnicode/               [LAYER 1: OmniCode Language]
│   │   ├── compiler/              [OmniCode compiler]
│   │   ├── parser/                [OmniCode parser]
│   │   ├── stdlib/                [OmniCode standard library]
│   │   └── spec/                  [OmniCode specifications]
│   │
│   ├── L2-platform/               [LAYER 2: MillenniumOS & Bereshit]
│   │   ├── millenniumos/          [OS components]
│   │   ├── bereshit-fs/           [Bereshit filesystem]
│   │   └── fuse/                  [FUSE integration]
│   │
│   ├── L3-cpisi/                  [LAYER 3: CPI-SI Model]
│   │   ├── core/                  [CPI-SI core implementation]
│   │   ├── identity/              [Identity management]
│   │   └── cognition/             [Cognition systems]
│   │
│   ├── L4-faithnet/               [LAYER 4: FaithNet Network]
│   │   ├── protocol/              [FaithNet protocol]
│   │   ├── mapping/               [TCP/IP mapping layer]
│   │   └── access/                [Tiered access system]
│   │
│   ├── L5-applications/           [LAYER 5: Applications]
│   │   ├── cornerstone/           [SUBMODULE - Game Engine]
│   │   └── claude-integration/    [Claude Code integration]
│   │
│   ├── definitions/               [TYPE DEFINITIONS]
│   │   ├── core/                  [Core types, schemas, contracts]
│   │   ├── glossary/              [Term definitions]
│   │   └── omni/                  [OmniCode definitions]
│   │
│   ├── claude/                    [CLAUDE CODE INFRASTRUCTURE]
│   │   ├── hooks/                 [Claude hooks]
│   │   ├── statusline/            [Statusline]
│   │   ├── skills/                [Skills]
│   │   ├── agents/                [Agent definitions]
│   │   ├── commands/              [Slash commands]
│   │   ├── config/                [Runtime configuration]
│   │   └── system/                [System utilities]
│   │
│   ├── data/                      [REFERENCE DATA]
│   │   ├── scripture/             [Bible databases]
│   │   ├── journals/              [Bible study journals]
│   │   └── instance/              [Instance configurations]
│   │
│   └── seed/                      [TEMPLATES]
│       ├── code/                  [Code templates]
│       ├── data/                  [Data templates]
│       └── documentation/         [Doc templates]
│
├── tov/                           [EXIT GATE - Production & Output]
│   ├── demo/                      [Phase demonstrations]
│   ├── licenses/                  [License implementations]
│   ├── pricing/                   [Commercial pricing]
│   ├── releases/                  [Release artifacts]
│   └── data/                      [Runtime data storage]
│
├── .vscode/                       [IDE Configuration]
├── .git*                          [Git files]
├── go.work                        [Go workspace]
├── LICENSE                        [Root license]
├── SUBMODULES.md                  [Submodule reference]
├── root.adoc                      [Root documentation]
└── root.omni                      [OmniCode folder definition]
```

### 2.2 Key Principles

| Principle | Implementation |
|-----------|----------------|
| **Ternary Structure** | void/ (entry), work/ (definitions+implementation), tov/ (exit) |
| **Layer Visibility** | L0 through L5 folders show the stack |
| **Single Source of Truth** | No duplication between work/ and other locations |
| **Clear Flow** | Universal → Language → Platform → Model → Network → Apps |
| **Submodule Integration** | Cornerstone under L5-applications |

---

## 3. The Layer Flow Architecture

### 3.1 Complete Layer Stack

```
┌─────────────────────────────────────────────────────────────────┐
│ L5: APPLICATIONS                                                │
│     Cornerstone Game Engine, Claude Integration                 │
│     Location: work/L5-applications/                             │
├─────────────────────────────────────────────────────────────────┤
│ L4: FAITHNET (Networking)                                       │
│     Kingdom network on existing internet                        │
│     Biblical Foundation: Genesis 2:9 (Two Trees)                │
│     Location: work/L4-faithnet/                                 │
├─────────────────────────────────────────────────────────────────┤
│ L3: CPI-SI (Intelligence Model)                                 │
│     Covenant Partnership Intelligence instances                 │
│     Biblical Foundation: Genesis 2:7 (Breath of Life)           │
│     Location: work/L3-cpisi/                                    │
├─────────────────────────────────────────────────────────────────┤
│ L2: MILLENNIUMOS / BERESHIT (Platform)                          │
│     Ternary on binary hardware, Filesystem                      │
│     Biblical Foundation: Genesis 1:6 (Firmament)                │
│     Location: work/L2-platform/                                 │
├─────────────────────────────────────────────────────────────────┤
│ L1: OMNICODE (Language)                                         │
│     Universal meaning representation                            │
│     Biblical Foundation: John 1:1 (The Word)                    │
│     Location: work/L1-omnicode/                                 │
├─────────────────────────────────────────────────────────────────┤
│ L0: UNIVERSAL (Libraries)                                       │
│     Foundational libraries (libtrit, foundation)                │
│     Biblical Foundation: Genesis 1:1 (In the beginning)         │
│     Location: work/L0-universal/                                │
└─────────────────────────────────────────────────────────────────┘
```

### 3.2 Layer Dependencies

```
L5 (Applications) depends on:
├── L4 (FaithNet) for networking
├── L3 (CPI-SI) for intelligence
├── L2 (Platform) for OS services
├── L1 (OmniCode) for language
└── L0 (Universal) for primitives

L4 (FaithNet) depends on:
├── L3 (CPI-SI) for message handling
├── L2 (Platform) for system access
├── L1 (OmniCode) for protocol encoding
└── L0 (Universal) for primitives

L3 (CPI-SI) depends on:
├── L2 (Platform) for persistence
├── L1 (OmniCode) for cognition
└── L0 (Universal) for primitives

L2 (Platform) depends on:
├── L1 (OmniCode) for configuration
└── L0 (Universal) for primitives

L1 (OmniCode) depends on:
└── L0 (Universal) for primitives

L0 (Universal) depends on:
└── [No internal dependencies - foundation]
```

---

## 4. Detailed Reorganization Map

### 4.1 void/ (Stays Mostly Same)

| Current Path | New Path | Notes |
|--------------|----------|-------|
| `void/planning/` | `void/planning/` | Keep as-is |
| `void/workflow/` | `void/workflow/` | Keep as-is |
| (new) | `void/imports/` | Create for external imports |

### 4.2 word/ → work/ (Major Restructure)

#### From word/work/ → work/ (Top-Level)

| Current Path | New Path | Notes |
|--------------|----------|-------|
| `word/work/pkg/trit/` | `work/L0-universal/libtrit/` | Core ternary library |
| `word/work/pkg/foundation/` | `work/L0-universal/foundation/` | Foundation packages |
| `word/work/pkg/omni/` | `work/L1-omnicode/` | OmniCode implementation |
| `word/work/pkg/fuse/` | `work/L2-platform/fuse/` | FUSE integration |
| `word/work/pkg/core/` | `work/L3-cpisi/core/` | CPI-SI core (statemachine, health, etc.) |
| `word/work/pkg/orchestration/` | `work/L3-cpisi/orchestration/` | CPI-SI orchestration |
| `word/work/pkg/util/` | `work/L0-universal/util/` | Utility packages |
| `word/work/hooks/` | `work/claude/hooks/` | Claude hooks |
| `word/work/statusline/` | `work/claude/statusline/` | Statusline |
| `word/work/system/` | `work/claude/system/` | System utilities |
| `word/work/cmd/` | `work/L3-cpisi/cmd/` | CPI-SI commands |

#### From word/claude/ → work/claude/

| Current Path | New Path | Notes |
|--------------|----------|-------|
| `word/claude/hooks/` | `work/claude/hooks/` | MERGE with work/hooks/ |
| `word/claude/statusline/` | `work/claude/statusline/` | MERGE with work/statusline/ |
| `word/claude/pkg/` | DELETE | Duplicate of work/pkg/ |
| `word/claude/skills/` | `work/claude/skills/` | Keep |
| `word/claude/agents/` | `work/claude/agents/` | Keep |
| `word/claude/commands/` | `work/claude/commands/` | Keep |
| `word/claude/config/` | `work/claude/config/` | Keep |
| `word/claude/system/` | `work/claude/system/` | MERGE with work/system/ |
| `word/claude/rules/` | `work/claude/rules/` | Keep |
| `word/claude/plans/` | `void/planning/claude/` | Move to void |
| `word/claude/settings.json` | `work/claude/settings.json` | Keep |
| `word/claude/config.toml` | `work/claude/config.toml` | Keep |
| `word/claude/Makefile` | `work/claude/Makefile` | Keep |

#### From word/ (Other) → work/

| Current Path | New Path | Notes |
|--------------|----------|-------|
| `word/core/` | `work/definitions/core/` | Core definitions |
| `word/glossary/` | `work/definitions/glossary/` | Glossary |
| `word/omni/` | `work/definitions/omni/` | OmniCode definitions |
| `word/scripture/` | `work/data/scripture/` | Bible databases |
| `word/journals/` | `work/data/journals/` | Journals |
| `word/instance/` | `work/data/instance/` | Instance configs |
| `word/seed/` | `work/seed/` | Templates |
| `word/skills/` | `work/claude/skills/` | MERGE |
| `word/agents/` | `work/claude/agents/` | MERGE |
| `word/research/` | `void/planning/research/` | Move to void |
| `word/journey/` | `void/planning/journey/` | Move to void |

### 4.3 tov/ (Minor Updates)

| Current Path | New Path | Notes |
|--------------|----------|-------|
| `tov/demo/` | `tov/demo/` | Keep as-is |
| `tov/licenses/` | `tov/licenses/` | Keep as-is |
| `tov/pricing/` | `tov/pricing/` | Keep as-is |
| `tov/data/` | `tov/data/` | Keep as-is |
| (new) | `tov/releases/` | Create for release artifacts |

### 4.4 cornerstone/ (Submodule Relocation)

| Current Path | New Path | Notes |
|--------------|----------|-------|
| `cornerstone/` (root) | `work/L5-applications/cornerstone/` | Relocate submodule |

### 4.5 FaithNet (New Creation)

| New Path | Contents | Notes |
|----------|----------|-------|
| `work/L4-faithnet/protocol/` | FaithNet protocol implementation | New |
| `work/L4-faithnet/mapping/` | TCP/IP mapping layer | New |
| `work/L4-faithnet/access/` | Tiered access system | New |
| `work/L4-faithnet/spec/` | Protocol specifications | From void/planning/understanding/layers/ |

---

## 5. Root Files Reorganization

### 5.1 Root Files to Keep

| File | Purpose | Action |
|------|---------|--------|
| `.gitignore` | Repository hygiene | Keep, update patterns |
| `.gitmodules` | Submodule tracking | Update for new cornerstone path |
| `.gitmessage` | Commit template | Keep |
| `.health` | Health marker | Keep |
| `go.work` | Go workspace | Update module paths |
| `go.work.sum` | Go checksums | Regenerate |
| `LICENSE` | 3-tier license | Keep |
| `SUBMODULES.md` | Submodule docs | Update for new path |
| `root.adoc` | Root documentation | Update structure docs |
| `root.omni` | OmniCode folder definition | Update for new structure |

### 5.2 Root Files to Create

| File | Purpose |
|------|---------|
| `ARCHITECTURE.md` | High-level architecture overview |
| `LAYERS.md` | Layer stack documentation |

### 5.3 .gitmodules Update

```ini
[submodule "cornerstone"]
    path = work/L5-applications/cornerstone
    url = ../Cornerstone
```

### 5.4 go.work Update

```go
go 1.23

use (
    ./work/L0-universal/foundation
    ./work/L0-universal/libtrit
    ./work/L1-omnicode
    ./work/L2-platform
    ./work/L3-cpisi
    ./work/L4-faithnet
    ./work/claude
    ./tov/demo/phase-0/demo-config
)
```

---

## 6. Submodule Handling

### 6.1 Cornerstone Submodule Migration

```bash
# Step 1: Remove current submodule registration
git submodule deinit -f cornerstone
git rm -f cornerstone
rm -rf .git/modules/cornerstone

# Step 2: Create new location
mkdir -p work/L5-applications

# Step 3: Add submodule at new location
git submodule add ../Cornerstone work/L5-applications/cornerstone

# Step 4: Initialize
git submodule init
git submodule update
```

### 6.2 Future Submodules

| Potential Submodule | Location | Purpose |
|---------------------|----------|---------|
| External libraries | `work/L0-universal/external/` | Third-party dependencies |
| FaithNet reference | `work/L4-faithnet/reference/` | Protocol reference implementations |

---

## 7. Migration Strategy

### 7.1 Phase 1: Preparation

1. **Create backup branch**
   ```bash
   git checkout -b backup/pre-reorganization
   git push origin backup/pre-reorganization
   ```

2. **Document current state**
   - Run full test suite
   - Document all working functionality
   - Export current go.work configuration

3. **Prepare new structure**
   - Create empty folder structure
   - Update root.omni with new schema

### 7.2 Phase 2: Core Migration

1. **Create work/ top-level**
   ```bash
   mkdir -p work/L{0..5}-{universal,omnicode,platform,cpisi,faithnet,applications}
   mkdir -p work/definitions work/claude work/data work/seed
   ```

2. **Move word/work/ packages to layered structure**
   - Move `pkg/trit/` → `L0-universal/libtrit/`
   - Move `pkg/foundation/` → `L0-universal/foundation/`
   - Move `pkg/omni/` → `L1-omnicode/`
   - Move `pkg/fuse/` → `L2-platform/fuse/`
   - Move `pkg/core/` → `L3-cpisi/core/`
   - Move `pkg/orchestration/` → `L3-cpisi/orchestration/`

3. **Merge Claude Code implementations**
   - Merge `word/work/hooks/` + `word/claude/hooks/` → `work/claude/hooks/`
   - Merge `word/work/statusline/` + `word/claude/statusline/` → `work/claude/statusline/`
   - Delete duplicate `word/claude/pkg/`

4. **Move definitions**
   - Move `word/core/` → `work/definitions/core/`
   - Move `word/glossary/` → `work/definitions/glossary/`
   - Move `word/omni/` → `work/definitions/omni/`

5. **Move data**
   - Move `word/scripture/` → `work/data/scripture/`
   - Move `word/journals/` → `work/data/journals/`
   - Move `word/instance/` → `work/data/instance/`

6. **Move planning content to void/**
   - Move `word/research/` → `void/planning/research/`
   - Move `word/journey/` → `void/planning/journey/`
   - Move `word/claude/plans/` → `void/planning/claude/`

### 7.3 Phase 3: Submodule Migration

1. **Remove cornerstone from root**
2. **Add cornerstone to work/L5-applications/**
3. **Update .gitmodules**
4. **Verify submodule functionality**

### 7.4 Phase 4: Configuration Updates

1. **Update go.work**
   - Update all module paths
   - Run `go work sync`

2. **Update import paths in Go files**
   - Find all imports referencing old paths
   - Update to new paths

3. **Update Makefile**
   - Update source paths
   - Update build targets

4. **Update symlinks**
   - `~/.claude/` symlinks point to new locations

### 7.5 Phase 5: Verification

1. **Run all tests**
   ```bash
   cd work/L0-universal/libtrit && make test
   cd work/claude && make build && make test
   ```

2. **Verify Claude Code hooks work**
   ```bash
   make -C work/claude install
   # Start Claude Code session, verify hooks fire
   ```

3. **Verify demos work**
   ```bash
   cd tov/demo/phase-0/demo-config && go run .
   cd tov/demo/phase-1/demo-trit && make test
   ```

4. **Validate folder structure**
   ```bash
   # Check all .health files
   find . -name ".health" -exec echo {} \;

   # Check root.omni consistency
   cat root.omni
   ```

### 7.6 Phase 6: Cleanup

1. **Remove old word/ directory**
   ```bash
   rm -rf word/
   ```

2. **Update documentation**
   - Update README files
   - Update ARCHITECTURE.md
   - Update root.omni

3. **Final commit**
   ```bash
   git add -A
   git commit -m "[bereshit] REORGANIZE: Restructure to tov/void/work with L0-L5 layers"
   ```

---

## 8. Validation Checklist

### 8.1 Structure Validation

- [ ] `void/` exists with planning/, workflow/, imports/
- [ ] `work/` exists with L0-L5 layer folders
- [ ] `tov/` exists with demo/, licenses/, pricing/, releases/, data/
- [ ] No `word/` folder remains at root
- [ ] `cornerstone/` moved to `work/L5-applications/`

### 8.2 Layer Validation

- [ ] `work/L0-universal/` contains libtrit, foundation, util
- [ ] `work/L1-omnicode/` contains compiler, parser, stdlib, spec
- [ ] `work/L2-platform/` contains millenniumos, bereshit-fs, fuse
- [ ] `work/L3-cpisi/` contains core, identity, cognition, orchestration
- [ ] `work/L4-faithnet/` contains protocol, mapping, access, spec
- [ ] `work/L5-applications/` contains cornerstone submodule

### 8.3 Build Validation

- [ ] `make -C work/L0-universal/libtrit test` passes
- [ ] `make -C work/claude build` succeeds
- [ ] `go work sync` completes without errors
- [ ] All Go imports resolve correctly

### 8.4 Runtime Validation

- [ ] Claude Code hooks fire correctly
- [ ] Statusline displays correctly
- [ ] Phase demos execute successfully
- [ ] Health checks pass

### 8.5 Documentation Validation

- [ ] `root.omni` reflects new structure
- [ ] `SUBMODULES.md` updated for new path
- [ ] Layer documentation accurate
- [ ] All internal links updated

---

## Quick Reference

### New Top-Level Structure

```
bereshit/
├── void/          [Entry Gate - Planning]
├── work/          [Word - Definitions & Implementation]
│   ├── L0-universal/
│   ├── L1-omnicode/
│   ├── L2-platform/
│   ├── L3-cpisi/
│   ├── L4-faithnet/
│   ├── L5-applications/
│   ├── definitions/
│   ├── claude/
│   ├── data/
│   └── seed/
└── tov/           [Exit Gate - Production]
```

### Layer Quick Reference

| Layer | Location | Biblical Foundation |
|-------|----------|---------------------|
| L0 Universal | `work/L0-universal/` | Genesis 1:1 |
| L1 OmniCode | `work/L1-omnicode/` | John 1:1 |
| L2 Platform | `work/L2-platform/` | Genesis 1:6 |
| L3 CPI-SI | `work/L3-cpisi/` | Genesis 2:7 |
| L4 FaithNet | `work/L4-faithnet/` | Genesis 2:9 |
| L5 Applications | `work/L5-applications/` | Genesis 2:15 |

---

## Closing

This reorganization aligns the physical folder structure with the architectural layer stack, making the codebase self-documenting. The flow from Universal → OmniCode → Platform → CPI-SI → FaithNet → Applications becomes visible in the folder hierarchy itself.

The ternary structure (void/work/tov) remains intact while providing clarity through the L0-L5 layering within work/.

> *"For other foundation can no man lay than that is laid, which is Jesus Christ."* — 1 Corinthians 3:11 KJV

---

**Document Status**

| Field | Value |
|-------|-------|
| Key | B-void-planning-repo-reorganization |
| Version | 1.0.0 |
| Status | Active |
| Created | 2026-02-01 |
| Authors | Seanje Lenox-Wise (Architect), Nova Dawn (Implementation) |
