<div align="center">

# 📋 TEMPLATE MATRIX

**Comprehensive Template System Coordination**

**Purpose:** Track all template development (documentation + code templates)
**Companion:** [CODE-ALIGNMENT-MATRIX.md](CODE-ALIGNMENT-MATRIX.md) (files → what to align)
**Relationship:** This matrix defines templates; Code Alignment Matrix tracks files using them
**Type:** Coordination Infrastructure (living document)
**Location:** `templates/TEMPLATE-MATRIX.md`

**Last Updated:** 2025-11-30
**Status:** ✅ 39% Complete (35/89) - All code templates ready: Go (6), C (2), ASM (3 incl. ARM), Shell (1), Linker (1), Docker (3), Build (2), Config (2), JSON (3), Dotfiles (3)

---

**📍 Quick Navigation**

[How to Use](#how-to-use-this-matrix) • [Documentation](#documentation-templates) • [Code - Current](#code-templates---current-use) • [Code - Future](#code-templates---future-scope) • [Statistics](#summary-statistics) • [Action Plan](#strategic-action-plan)

---

**🔗 Matrix Relationship**

| Matrix | Purpose | Question Answered |
|--------|---------|-------------------|
| **This File** | Template inventory | "What templates exist?" (89 total) |
| **[CODE-ALIGNMENT-MATRIX.md](CODE-ALIGNMENT-MATRIX.md)** | File alignment tracking | "Which files need which templates?" (257+ files) |

</div>

---

## How to Use This Matrix

### Status Indicators

| Icon | Status | Meaning |
|:----:|--------|---------|
| ✅ | **Exists** | Template file built, tested, and available for use |
| 📋 | **Specified** | Research complete, specification exists, ready to build |
| 🔄 | **In Progress** | Currently being built or refined |
| ❌ | **Needed** | Identified need, specification required |

### Priority Indicators

| Icon | Priority | Timeline | Rationale |
|:----:|----------|----------|-----------|
| 🔴 | **Critical** | Build immediately | Blocking current work (Iteration 4), core functionality |
| 🟢 | **High** | Build this week | Needed soon, frequently used, high impact |
| 🟡 | **Medium** | Build this month | Supporting files, future iterations, nice-to-have |
| 🔵 | **Low** | Build when required | Future need, extended scope, low frequency |

### Matrix Columns

| Column | Contains | Purpose |
|--------|----------|---------|
| **ID** | Template identifier | Unique key for referencing (e.g., CODE-GO-001, S-002) |
| **Name** | Template name | Human-readable description |
| **Type** | Category | What kind of template (Main/Lib/Test/Source/etc.) |
| **Priority** | Urgency | When to build (🔴🟢🟡🔵) |
| **Status** | Current state | Built (✅) or Specified (📋) |
| **Specification** | Design doc link | Where to find the full specification |
| **Examples** | Usage samples | Real files using this template |
| **Notes** | Context | Why this template matters, dependencies, complexity |

---

## DOCUMENTATION TEMPLATES

> **Subtotal:** 28 templates (9 exist, 19 specified) | **Completion:** 32%

### General Templates (G-*)

**Purpose:** Universal documentation that applies across multiple contexts (READMEs, standards, patterns)

| ID | Name | Type | Priority | Status | Specification | Examples | Notes |
|----|------|------|----------|--------|---------------|----------|-------|
| **G-000** | General Template Index | Navigation | 🔵 Low | ✅ Exists | [templates/general/](general/) | INDEX files | Navigation for G-* templates |
| **G-001** | Universal Documentation | Primary | 🔵 Low | ✅ Exists | [templates/general/TEMPLATE-G-001-DOC-standard-document.md](general/TEMPLATE-G-001-DOC-standard-document.md) | CWS-STD-001/002/003 | **Most used doc template** - Standards, Patterns, Guides all use this |
| **G-002** | Division README | Specialized | 🔵 Low | ✅ Exists | [templates/general/TEMPLATE-G-002-DOC-readme-division.md](general/TEMPLATE-G-002-DOC-readme-division.md) | divisions/tech/README.md | Division-level overview (Tech, Gaming, etc.) |
| **G-003** | Project README | Specialized | 🔵 Low | ✅ Exists | [templates/general/TEMPLATE-G-003-DOC-readme-project.md](general/TEMPLATE-G-003-DOC-readme-project.md) | divisions/tech/language/README.md | Project-level overview (Language, OS, CPI-SI) |
| **G-004** | Component README | Specialized | 🔵 Low | ✅ Exists | [templates/general/TEMPLATE-G-004-DOC-readme-component.md](general/TEMPLATE-G-004-DOC-readme-component.md) | compiler/README.md | Component/tool documentation |
| **G-010** | Changelog | General | 🟢 High | 📋 Specified | [Synthesis §Part 4](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#template-recommendations) | CHANGELOG.md | **Needed for releases** - Version/change tracking |
| **G-011** | Contributing Guide | General | 🟢 High | 📋 Specified | [Synthesis §Part 4](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#template-recommendations) | CONTRIBUTING.md | **Onboarding essential** - How to contribute |
| **G-012** | Roadmap | General | 🟡 Medium | 📋 Specified | [Synthesis §Part 4](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#template-recommendations) | ROADMAP.md | Vision/timeline documentation |

**G-* Usage Patterns:**

- **G-001** = Default for most docs (standards, patterns, guides)
- **G-002/003/004** = Hierarchical READMEs (division → project → component)
- **G-010/011** = Community/contribution infrastructure

---

### Index Templates (T-*)

**Purpose:** Pure structural templates for navigation and organization

| ID | Name | Type | Priority | Status | Specification | Examples | Notes |
|----|------|------|----------|--------|---------------|----------|-------|
| **T-000-main** | Main INDEX | Navigation | 🔵 Low | ✅ Exists | [templates/index/TEMPLATE-T-000-INDEX-main.md](index/TEMPLATE-T-000-INDEX-main.md) | INDEX.md (root) | **Root-level navigation** - Entry point to docs |
| **T-000-index** | INDEX Template Index | Navigation | 🔵 Low | ✅ Exists | [templates/index/TEMPLATE-T-000-INDEX-index.md](index/TEMPLATE-T-000-INDEX-index.md) | templates/index/ | **Meta-navigation** - How to use T-* templates |
| **T-001** | Category INDEX | Navigation | 🔵 Low | ✅ Exists | [templates/index/TEMPLATE-T-001-INDEX-category.md](index/TEMPLATE-T-001-INDEX-category.md) | divisions/tech/INDEX.md | **Folder navigation** - Category/folder-level indices |

**T-* Usage Patterns:**

- Use T-000-main for workspace root
- Use T-001 for all major folders/categories
- Never mix INDEX with README (different purposes)

---

### Specialized Templates (S-*)

**Purpose:** Domain-specific templates for particular documentation types

| ID | Name | Type | Priority | Status | Specification | Examples | Notes |
|----|------|------|----------|--------|---------------|----------|-------|
| **S-001** | Alignment Status | Operational | 🔵 Low | ✅ Exists | [templates/specialized/TEMPLATE-S-001-TXT-alignment-status.md](specialized/TEMPLATE-S-001-TXT-alignment-status.md) | ALIGNMENT-STATUS.txt | **Config tracking** - Document alignment progress |
| **S-002** | API Documentation | Technical | 🔴 Critical | 📋 Specified | [Synthesis §TEMPLATE-S-002](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#template-s-002-api-documentation) | LANG-REF-* (future) | **Critical for Iteration 4** - Compiler API docs, 13+ files waiting |
| **S-003** | Code Module Documentation | Technical | 🟡 Medium | 📋 Specified | [Synthesis §Planned](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#planned-templates-not-yet-implemented) | Per-file headers | Module/file-level code documentation |
| **S-004** | Architecture Decision Records (ADR) | Technical | 🔴 Critical | 📋 Specified | [Synthesis §TEMPLATE-S-004](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#template-s-004-architecture-decision-records-adr) | LANG-ADR-* (future) | **Track major decisions** - Why we chose X over Y |
| **S-005** | Release Notes/Changelog | Operational | 🟢 High | 📋 Specified | [Synthesis §Planned](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#planned-templates-not-yet-implemented) | Iteration releases | Version tracking, user-facing changes |
| **S-006** | Tutorial/Walkthrough | Educational | 🟡 Medium | 📋 Specified | [Synthesis §Planned](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#planned-templates-not-yet-implemented) | Language tutorials | **User onboarding** - Step-by-step learning |
| **S-007** | Troubleshooting Guide | Support | 🔴 Critical | 📋 Specified | [Synthesis §TEMPLATE-S-007](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#template-s-007-troubleshooting-guide) | Compiler errors | **User support essential** - Problem resolution trees |
| **S-008** | Migration Guide | Support | 🔵 Low | 📋 Specified | [Synthesis §Planned](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#planned-templates-not-yet-implemented) | Version upgrades | Version-to-version migration instructions |
| **S-009** | Configuration Reference | Technical | 🟡 Medium | 📋 Specified | [Synthesis §Planned](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#planned-templates-not-yet-implemented) | Config docs | Configuration option documentation |
| **S-010** | Architecture Document | Technical | 🔴 Critical | 📋 Specified | [Synthesis §TEMPLATE-S-010](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#template-s-010-architecture-document) | LANG-ARCH-* (future) | **Heavily used** - System design docs, 15+ files waiting |
| **S-011** | Design Document | Technical | 🟢 High | 📋 Specified | [Synthesis §Category C](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#category-c-architecture--specification-documents) | Feature designs | Feature/component design documentation |
| **S-012** | Development Log (Devlog) | Technical | 🔴 Critical | 📋 Specified | [Synthesis §TEMPLATE-S-012](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#template-s-012-development-log-devlog) | LANG-DEV-*, OS-DEV-* | **Heavily used** - Iteration logs, 11+ existing devlogs need alignment |
| **S-013** | Instance Journal | CPI-SI | 🟡 Medium | 📋 Specified | [Synthesis §Category D](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#category-d-development-logs--journals) | Nova Dawn learning | Instance-specific learning and reflection |
| **S-014** | Universal Journal | CPI-SI | 🟡 Medium | 📋 Specified | [Synthesis §Category D](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#category-d-development-logs--journals) | Paradigm insights | CPI-SI paradigm-level discoveries |
| **S-015** | Quick Reference/Cheat Sheet | Educational | 🟡 Medium | 📋 Specified | [Synthesis §Category E](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#category-e-api--reference-documentation) | Command refs | **User productivity** - Quick lookup tables |
| **S-016** | Algorithm Documentation | Technical | 🟡 Medium | 📋 Specified | [Synthesis §Category F](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#category-f-standards-patterns--guides) | CPSI-ALG-* | Theoretical/mathematical documentation |
| **S-017** | FAQ | Support | 🟢 High | 📋 Specified | [Synthesis §Category H](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#category-h-user-support--learning) | Common questions | **User support** - Frequently asked questions |
| **S-018** | Installation Guide | Support | 🟢 High | 📋 Specified | [Synthesis §Category H](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#category-h-user-support--learning) | Compiler install | **User onboarding** - Setup instructions |
| **S-019** | Code Example | Educational | 🟢 High | 📋 Specified | [Synthesis §Category I](../status/.claude/2025-11-02-research-findings/documentation-templates/2025-11-02_template-research-synthesis.md#category-i-examples--demonstrations) | LANG-EXAMPLE-* | **Language learning** - Practical code samples |

**S-* Critical Templates (🔴):**

- **S-002 (API)** - 13+ files blocked, needed for compiler documentation
- **S-004 (ADR)** - Track architectural decisions as we build Iteration 4
- **S-007 (Troubleshooting)** - User support essential, 0 troubleshooting docs exist
- **S-010 (Architecture)** - 15+ architecture docs need standardization
- **S-012 (Devlog)** - Heavily used pattern (11+ existing), 0% aligned currently

**S-* Usage Patterns:**

- Technical docs (S-002, S-004, S-010) = Architecture and design
- Support docs (S-007, S-017, S-018) = User-facing help
- Educational docs (S-006, S-015, S-019) = Learning and reference
- CPI-SI specific (S-013, S-014) = Instance and paradigm reflection

---

## CODE TEMPLATES - CURRENT USE

> **Subtotal:** 26 templates (26 exist, 0 specified) | **Completion:** 100%

**Context:** Templates for languages and file types currently in active use (Go, C, Assembly/ARM, Shell, Linker, JSON, YAML, TOML, Makefile, CMake, Docker, dotfiles)

### Go Templates (CODE-GO-*)

**Purpose:** Go language files (64 files in workspace, 100% verified compliant)

| ID | Name | Type | Priority | Status | Specification | Examples | Notes |
|----|------|------|----------|--------|---------------|----------|-------|
| **CODE-GO-001** | Go Executable | Command | 🔴 Critical | ✅ Exists | [templates/code/go/CODE-GO-001-GO-executable.go](code/go/CODE-GO-001-GO-executable.go) | cmd/omnic/omnic.go | **Entry point pattern** - Orchestrators, CLI tools. Derived from library template. |
| **CODE-GO-002** | Go Library Package | Library | 🔴 Critical | ✅ Exists | [templates/code/go/CODE-GO-002-GO-library.go](code/go/CODE-GO-002-GO-library.go) | internal/*, pkg/* | **Primary Go pattern** - 13+ lib files need this. 598 lines, full 8-rung ladder, validated against 3 reference implementations |
| **CODE-GO-003** | Go Demo-Test | Demo-Test | 🔴 Critical | ✅ Exists | [templates/code/go/CODE-GO-003-GO-demo-test.go](code/go/CODE-GO-003-GO-demo-test.go) | *_test.go files | **Demonstration-based testing** - Shows behavior, not just assertions. Derived from library template. |
| **CODE-GO-004** | Go Workspace | Module | 🟢 High | ✅ Exists | [templates/code/go/CODE-GO-004-GO-workspace.work](code/go/CODE-GO-004-GO-workspace.work) | go.work | **Multi-module workspace** - Coordinates multiple Go modules in monorepo. |
| **CODE-GO-005** | Go Module | Module | 🟢 High | ✅ Exists | [templates/code/go/CODE-GO-005-GO-module.mod](code/go/CODE-GO-005-GO-module.mod) | go.mod | **Module definition** - Declares module path and dependencies. |
| **CODE-GO-006** | Go Checksum | Module | 🟡 Medium | ✅ Exists | [templates/code/go/CODE-GO-006-GO-checksum.sum](code/go/CODE-GO-006-GO-checksum.sum) | go.sum | **Dependency verification** - Cryptographic checksums for module integrity. |

**Go Template Dependencies:**

- CODE-GO-002 (library) is the base - executable and demo-test derive from it
- CODE-GO-001 (executable) for entry points and CLI tools
- CODE-GO-003 (demo-test) for demonstration-based testing
- CODE-GO-004/005/006 (workspace/module/checksum) for module infrastructure
- All six now complete - ready for Iteration 4 compiler work

**Complexity:** Medium - Go is well-understood, patterns proven in omnic.go and lexer.go

---

### C Templates (CODE-C-*)

**Purpose:** C language files (6 files: 3 source + 3 headers)

| ID | Name | Type | Priority | Status | Specification | Examples | Notes |
|----|------|------|----------|--------|---------------|----------|-------|
| **CODE-C-001** | C Source File | Source | 🔴 Critical | ✅ Exists | [templates/code/c/CODE-C-001-C-source.c](code/c/CODE-C-001-C-source.c) | cpi-si/core/src/health.c, os/millenniumos/kernel/src/kernel.c | **C implementation** - Full 4-block with health tracking |
| **CODE-C-002** | C Header File | Header | 🔴 Critical | ✅ Exists | [templates/code/c/CODE-C-002-C-header.h](code/c/CODE-C-002-C-header.h) | cpi-si/core/include/health.h, os/millenniumos/kernel/include/*.h | **C declarations** - Full 4-block for interfaces, include guards |

**C Template Dependencies:**

- CODE-C-001 (source) and CODE-C-002 (header) work as a pair
- Both now complete - ready for compiler backend (Iteration 4+)

**Complexity:** Medium-High - C requires careful memory/header patterns

---

### Assembly Templates (CODE-ASM-*, CODE-ARM-*)

**Purpose:** Assembly language files for multiple architectures (x86/x64 NASM, ARM GNU AS)

| ID | Name | Type | Priority | Status | Specification | Examples | Notes |
|----|------|------|----------|--------|---------------|----------|-------|
| **CODE-ASM-001** | x86/x64 Assembly Source | Source | 🟢 High | ✅ Exists | [templates/code/asm/CODE-ASM-001-ASM-source.asm](code/asm/CODE-ASM-001-ASM-source.asm) | boot/stage1/boot.asm, boot/stage2/*.asm, kernel/asm/*.asm | **Complete** - NASM syntax, 4-block structure |
| **CODE-ASM-002** | x86/x64 Assembly Include | Include | 🟢 High | ✅ Exists | [templates/code/asm/CODE-ASM-002-ASM-include.inc](code/asm/CODE-ASM-002-ASM-include.inc) | boot/*/config/*.inc, kernel/asm/config/*.inc | **Complete** - Reusable macros/constants |
| **CODE-ARM-001** | ARM Assembly Source | Source | 🟡 Medium | ✅ Exists | [templates/code/asm/CODE-ARM-001-ARM-source.s](code/asm/CODE-ARM-001-ARM-source.s) | (future ARM port) | **Complete** - GNU AS syntax, AAPCS calling convention, 4-block structure |

**Assembly Template Notes:**

- CODE-ASM-001 ✅ **COMPLETE** - x86/x64 NASM source template (current MillenniumOS target)
- CODE-ASM-002 ✅ **COMPLETE** - x86/x64 include files for shared macros/constants
- CODE-ARM-001 ✅ **COMPLETE** - ARM GNU AS template (future MillenniumOS target)
- x86/x64 templates ready for current MillenniumOS work
- ARM template ready for future architecture expansion

**Complexity:** High - Assembly requires processor-specific knowledge

---

### Data & Configuration Templates (CODE-JSON-*, CODE-CONFIG-*)

**Purpose:** JSON data files and configuration files (22 files)

| ID | Name | Type | Priority | Status | Specification | Examples | Notes |
|----|------|------|----------|--------|---------------|----------|-------|
| **CODE-JSON-001** | JSON Data File | Data | 🟢 High | ✅ Exists | [templates/code/json/CODE-JSON-001-JSON-data.json](code/json/CODE-JSON-001-JSON-data.json) | package.json, configs | **Complete** - Standard JSON with `_metadata` pattern |
| **CODE-JSONC-001** | JSON with Comments | Data | 🟡 Medium | ✅ Exists | [templates/code/json/CODE-JSONC-001-JSONC-config.jsonc](code/json/CODE-JSONC-001-JSONC-config.jsonc) | VSCode configs | **Complete** - JSONC with // comments and 4-block |
| **CODE-JSON-003** | JSON Health Map | Data | 🔴 Critical | ✅ Exists | [templates/code/json/CODE-JSON-003-JSON-health-map.json](code/json/CODE-JSON-003-JSON-health-map.json) | (template ready, health-maps system pending) | **Complete** - Full `_metadata` pattern for health maps |
| **CODE-CONFIG-001** | TOML Configuration | Config | 🟡 Medium | ✅ Exists | [templates/code/config/CODE-CONFIG-001-CONFIG-toml.toml](code/config/CODE-CONFIG-001-CONFIG-toml.toml) | logging.toml, lexer.toml, parser.toml | **Complete** - Full 4-block TOML template, 8 config files aligned |
| **CODE-YAML-001** | YAML Configuration | Config | 🟡 Medium | ✅ Exists | [templates/code/yaml/CODE-YAML-001-YAML-config.yaml](code/yaml/CODE-YAML-001-YAML-config.yaml) | CI/CD, Docker, K8s | **Complete** - Full 4-block with # comments |

**JSON/Config Template Notes:**

- All JSON templates now complete with `_metadata`, `_setup`, `_closing` pattern
- CODE-YAML-001 added for YAML configurations (GitHub Actions, Docker, K8s)

**Complexity:** Low - Straightforward patterns, mostly formatting

---

### Build System Templates (CODE-MAKE-*, CODE-CMAKE-*)

**Purpose:** Build automation files (5 Makefiles currently)

| ID | Name | Type | Priority | Status | Specification | Examples | Notes |
|----|------|------|----------|--------|---------------|----------|-------|
| **CODE-MAKE-001** | Makefile | Build | 🟢 High | ✅ Exists | [templates/code/make/CODE-MAKE-001-MAKE-project-makefile.mk](code/make/CODE-MAKE-001-MAKE-project-makefile.mk) | compiler/Makefile | **Complete** - Build automation with health scoring, 4-block structure |
| **CODE-CMAKE-001** | CMakeLists.txt | Build | 🔵 Low | ✅ Exists | [templates/code/cmake/CODE-CMAKE-001-CMAKE-project-cmakelists.cmake](code/cmake/CODE-CMAKE-001-CMAKE-project-cmakelists.cmake) | Future: CMake builds | **Complete** - CMake build configuration with 4-block structure |

**Build Template Notes:**

- CODE-MAKE-001 ✅ **COMPLETE** (2025-11-25) - Full 4-block structure with health scoring
- CODE-CMAKE-001 ✅ **COMPLETE** (2025-11-25) - Adapted from Makefile template with CMake syntax
- 4 Makefiles aligned to template (OS root/boot/kernel, health-scorer)

**Complexity:** Low-Medium - Makefile pattern is straightforward, proven

---

### Dotfiles Templates (CODE-GIT-*, CODE-EDITOR-*, CODE-ENV-*)

**Purpose:** Project configuration dotfiles (.gitignore, .editorconfig, .env)

| ID | Name | Type | Priority | Status | Specification | Examples | Notes |
|----|------|------|----------|--------|---------------|----------|-------|
| **CODE-GIT-001** | Git Ignore | Dotfile | 🟢 High | ✅ Exists | [templates/code/dotfiles/CODE-GIT-001-GIT-gitignore.gitignore](code/dotfiles/CODE-GIT-001-GIT-gitignore.gitignore) | .gitignore | **Complete** - Pattern-based exclusion with 4-block |
| **CODE-EDITOR-001** | EditorConfig | Dotfile | 🟢 High | ✅ Exists | [templates/code/dotfiles/CODE-EDITOR-001-EDITOR-editorconfig.editorconfig](code/dotfiles/CODE-EDITOR-001-EDITOR-editorconfig.editorconfig) | .editorconfig | **Complete** - Cross-editor formatting with 4-block |
| **CODE-ENV-001** | Environment | Dotfile | 🟢 High | ✅ Exists | [templates/code/dotfiles/CODE-ENV-001-ENV-env.env](code/dotfiles/CODE-ENV-001-ENV-env.env) | .env, .env.example | **Complete** - Environment variables with 4-block |

**Dotfiles Template Notes:**

- All dotfiles now have 4-block documentation structure
- CODE-GIT-001 includes patterns for build artifacts, secrets, IDE files
- CODE-EDITOR-001 establishes Linux-first, UTF-8, LF standards
- CODE-ENV-001 separates configuration from code, emphasizes security

**Complexity:** Low - Well-established patterns, documentation focus

---

### Docker Templates (CODE-DOCKER-*)

**Purpose:** Container definitions for reproducible builds and deployment

| ID | Name | Type | Priority | Status | Specification | Examples | Notes |
|----|------|------|----------|--------|---------------|----------|-------|
| **CODE-DOCKER-001** | Dockerfile | Container | 🟢 High | ✅ Exists | [templates/code/docker/CODE-DOCKER-001-DOCKER-dockerfile.dockerfile](code/docker/CODE-DOCKER-001-DOCKER-dockerfile.dockerfile) | Dockerfile (root), compiler/Dockerfile, os/Dockerfile | **Base container template** - Generic Dockerfile with 4-block structure |
| **CODE-DOCKER-002** | Go Builder Dockerfile | Container | 🟢 High | ✅ Exists | [templates/code/docker/CODE-DOCKER-002-DOCKER-go-builder.dockerfile](code/docker/CODE-DOCKER-002-DOCKER-go-builder.dockerfile) | divisions/tech/*/Dockerfile | **Go-specific multi-stage** - Builder pattern for Go services |
| **CODE-DOCKER-003** | Docker Compose | Orchestration | 🟢 High | ✅ Exists | [templates/code/docker/CODE-DOCKER-003-DOCKER-compose.yaml](code/docker/CODE-DOCKER-003-DOCKER-compose.yaml) | docker-compose.yaml | **Service orchestration** - Multi-container configuration with 4-block structure |

**Docker Template Notes:**

- CODE-DOCKER-001 ✅ **COMPLETE** - Base Dockerfile template with 4-block
- CODE-DOCKER-002 ✅ **COMPLETE** - Go-specific multi-stage builder (used for division containers)
- CODE-DOCKER-003 ✅ **COMPLETE** - Docker Compose for service orchestration (derived from YAML template)

**Complexity:** Low-Medium - Dockerfile patterns well-established, orchestration adds complexity

---

### Shell Templates (CODE-SH-*)

**Purpose:** Shell scripting files (bash, sh)

| ID | Name | Type | Priority | Status | Specification | Examples | Notes |
|----|------|------|----------|--------|---------------|----------|-------|
| **CODE-SH-001** | Shell Script | Script | 🟢 High | ✅ Exists | [templates/code/shell/CODE-SH-001-SH-script.sh](code/shell/CODE-SH-001-SH-script.sh) | os/millenniumos/kernel/scripts/embed-logs.sh | **Complete** - Bash/sh scripts with 4-block structure |

**Shell Template Notes:**

- CODE-SH-001 ✅ **COMPLETE** - Full 4-block shell script template
- Includes shebang, error handling, and structured sections
- Ready for build automation and utility scripts

**Complexity:** Low-Medium - Shell patterns well-established

---

### Linker Templates (CODE-LD-*)

**Purpose:** Linker script files for bare-metal and OS development

| ID | Name | Type | Priority | Status | Specification | Examples | Notes |
|----|------|------|----------|--------|---------------|----------|-------|
| **CODE-LD-001** | Linker Script | Linker | 🟢 High | ✅ Exists | [templates/code/linker/CODE-LD-001-LD-linker-script.ld](code/linker/CODE-LD-001-LD-linker-script.ld) | os/millenniumos/kernel/linker.ld | **Complete** - Linker scripts with 4-block structure |

**Linker Template Notes:**

- CODE-LD-001 ✅ **COMPLETE** - Full 4-block linker script template
- Essential for MillenniumOS kernel and bare-metal development
- Defines memory layout, sections, and entry points

**Complexity:** High - Linker scripts require deep understanding of memory layout

---

## CODE TEMPLATES - FUTURE SCOPE

> **Subtotal:** 35 templates (0 exist, 35 specified) | **Completion:** 0%

**Context:** Templates for languages planned for OmniCode native support (future iterations)

### Compiled Languages - Systems (6 templates)

**Purpose:** Modern systems programming languages (Rust, C++, Zig, Swift)

| ID | Name | Priority | Notes |
|----|------|----------|-------|
| **CODE-RUST-001** | Rust Main/Library | 🟡 Medium | Modern systems, memory safety |
| **CODE-RUST-002** | Rust Test | 🟡 Medium | Rust testing patterns |
| **CODE-CPP-001** | C++ Source | 🟡 Medium | C++ implementation files |
| **CODE-CPP-002** | C++ Header | 🟡 Medium | C++ header files |
| **CODE-ZIG-001** | Zig Source | 🔵 Low | Modern C alternative |
| **CODE-SWIFT-001** | Swift Source | 🔵 Low | Apple ecosystem |

**Specification:** [Comprehensive Research §Wave 6](../status/.claude/2025-11-02-research-findings/code-templates/2025-11-02_code-template-research-comprehensive.md)

---

### Compiled Languages - Other (4 templates)

**Purpose:** JVM and functional compiled languages

| ID | Name | Priority | Notes |
|----|------|----------|-------|
| **CODE-KOTLIN-001** | Kotlin Source | 🔵 Low | JVM compiled |
| **CODE-SCALA-001** | Scala Source | 🔵 Low | JVM functional |
| **CODE-HASKELL-001** | Haskell Source | 🔵 Low | Pure functional |
| **CODE-OCAML-001** | OCaml Source | 🔵 Low | ML family |

**Specification:** [Comprehensive Research §Wave 6](../status/.claude/2025-11-02-research-findings/code-templates/2025-11-02_code-template-research-comprehensive.md)

---

### Web Languages (5 templates)

**Purpose:** HTML/CSS and preprocessors

| ID | Name | Priority | Notes |
|----|------|----------|-------|
| **CODE-HTML-001** | HTML Page | 🔵 Low | Web markup |
| **CODE-CSS-001** | CSS Stylesheet | 🔵 Low | Styling |
| **CODE-SCSS-001** | SCSS Stylesheet | 🔵 Low | Sass preprocessor |
| **CODE-SASS-001** | SASS Stylesheet | 🔵 Low | Sass indented syntax |
| **CODE-LESS-001** | LESS Stylesheet | 🔵 Low | LESS preprocessor |

**Specification:** [Comprehensive Research §Wave 6](../status/.claude/2025-11-02-research-findings/code-templates/2025-11-02_code-template-research-comprehensive.md)

---

### Shell Scripts (5 templates)

**Purpose:** Shell scripting languages

| ID | Name | Priority | Notes |
|----|------|----------|-------|
| **CODE-SHELL-001** | Bash Script | 🟡 Medium | Primary shell scripting |
| **CODE-SHELL-002** | Shell Library/Functions | 🟡 Medium | Reusable shell functions |
| **CODE-ZSH-001** | Zsh Script | 🔵 Low | Z shell |
| **CODE-FISH-001** | Fish Script | 🔵 Low | Fish shell |
| **CODE-PS1-001** | PowerShell Script | 🔵 Low | Windows PowerShell |

**Specification:** [Comprehensive Research §Wave 6](../status/.claude/2025-11-02-research-findings/code-templates/2025-11-02_code-template-research-comprehensive.md)

---

### TypeScript/JavaScript (5 templates)

**Purpose:** Web and VSCode extension development

| ID | Name | Priority | Notes |
|----|------|----------|-------|
| **CODE-TS-001** | TypeScript Module | 🟡 Medium | VSCode extensions (constrained use) |
| **CODE-TS-002** | TypeScript Test | 🟡 Medium | TS testing |
| **CODE-JS-001** | JavaScript Module | 🟡 Medium | JS modules |
| **CODE-TSX-001** | React TypeScript Component | 🔵 Low | React TS |
| **CODE-JSX-001** | React JavaScript Component | 🔵 Low | React JS |

**Specification:** [Comprehensive Research §Wave 6](../status/.claude/2025-11-02-research-findings/code-templates/2025-11-02_code-template-research-comprehensive.md)

**Note:** Minimal scope - TypeScript only for VSCode extensions (constrained by platform)

---

### Python (Minimal Scope) (1 template)

**Purpose:** Scripting only (interpreted language exception)

| ID | Name | Priority | Notes |
|----|------|----------|-------|
| **CODE-PY-001** | Python Script | 🔵 Low | Minimal use - scripting/automation only |

**Specification:** [Comprehensive Research §Wave 6](../status/.claude/2025-11-02-research-findings/code-templates/2025-11-02_code-template-research-comprehensive.md)

**Kingdom Technology Note:** Compiled languages preferred; Python only for unavoidable scripting

---

## SUMMARY STATISTICS

### Overall Template Count

| Category | Exists | Specified | Needed | Total | % Complete |
|----------|:------:|:---------:|:------:|:-----:|:----------:|
| **Documentation** | 9 | 19 | 0 | 28 | 32% |
| **Code - Current** | 26 | 0 | 0 | 26 | 100% |
| **Code - Future** | 0 | 35 | 0 | 35 | 0% |
| **TOTAL** | **35** | **54** | **0** | **89** | **39%** |

**Key Insight:** All current code templates complete (26/26) - Go (6), C (2), Assembly (3 incl. ARM), Shell (1), Linker (1), Docker (3), Build (2), Config (2), JSON (3), Dotfiles (3) - ready for Iteration 4 compiler work and MillenniumOS development

---

### Priority Breakdown

| Priority | Count | % of Total | Timeline | Impact |
|----------|:-----:|:----------:|----------|--------|
| 🔴 **Critical** | 8 | 10% | Build immediately | **Blocks 55+ files** - Iteration 4 work |
| 🟢 **High** | 17 | 21% | Build this week | Frequently used, high-value |
| 🟡 **Medium** | 21 | 26% | Build this month | Supporting work, future needs |
| 🔵 **Low** | 35 | 43% | Build when required | Extended scope, low frequency |

**Strategic Insight:** 30% of templates (25 total) are High/Critical priority - focus here first

---

### Templates by Type

| Type | Count | Purpose |
|------|:-----:|---------|
| **Navigation** (G-000, T-*) | 4 | INDEX and template indices |
| **READMEs** (G-002/003/004) | 3 | Hierarchical documentation |
| **Technical Docs** (S-002/004/010/011) | 4 | Architecture, API, design |
| **Support Docs** (S-007/017/018) | 3 | User help and onboarding |
| **Educational** (S-006/015/019) | 3 | Learning and reference |
| **Operational** (S-001/005/009/012) | 4 | Tracking and logs |
| **CPI-SI Specific** (S-013/014) | 2 | Instance and paradigm |
| **Code - Go** | 6 | Primary language (source + module infra) ✅ Complete |
| **Code - C** | 2 | Systems programming ✅ Complete |
| **Code - ASM** | 3 | Assembly (x86 source + include, ARM source) ✅ Complete |
| **Code - Shell** | 1 | Shell scripting ✅ Complete |
| **Code - Linker** | 1 | Linker scripts ✅ Complete |
| **Code - Docker** | 3 | Container definitions ✅ Complete |
| **Code - Build** | 2 | Makefile, CMake ✅ Complete |
| **Code - Config** | 2 | TOML, YAML ✅ Complete |
| **Code - JSON** | 3 | JSON, JSONC, Health Map ✅ Complete |
| **Code - Dotfiles** | 3 | gitignore, editorconfig, env ✅ Complete |
| **Code - Future** | 35 | All future language support |

---

## STRATEGIC ACTION PLAN

### Phase 1: Critical Templates (Week 1) - 5 Templates

**Goal:** Unblock Iteration 4 work and high-traffic patterns

**Priority Order:**

1. ~~**CODE-GO-002** (Go Library)~~ - ✅ **COMPLETE** (2025-11-24)
   - Also built: CODE-GO-003 (Executable), CODE-GO-004 (Demo-Test)
   - All Go templates now ready for use

2. **S-012** (Devlog) - **Blocks 11+ existing devlogs**
   - Why critical: Heavily used pattern (OS devlogs, compiler iterations)
   - Effort: Low-Medium (2-3 hours)
   - Spec: Proven pattern exists in OS devlogs
   - Impact: Standardize all development logging

3. **S-002** (API Reference) - **Blocks 13+ API docs**
   - Why critical: Compiler documentation essential for Iteration 4
   - Effort: Medium (3-4 hours)
   - Spec: Detailed specification exists
   - Impact: Enable systematic API documentation

4. **S-004** (ADR) - **Tracks architectural decisions**
   - Why critical: Need to document decisions as we make them in Iteration 4
   - Effort: Low-Medium (2-3 hours)
   - Spec: Clear pattern for decision records
   - Impact: Capture rationale for future reference

5. **S-010** (Architecture) - **Blocks 15+ architecture docs**
   - Why critical: System design documentation needed
   - Effort: Medium (3-4 hours)
   - Spec: Proven in CPI-SI system docs
   - Impact: Standardize all architecture documentation

**Phase 1 Total:** ~15-18 hours execution (3-4 focused sessions)

**Phase 1 Impact:** Unblocks 55+ files, enables Iteration 4 documentation

---

### Phase 2: High Priority Templates (Week 2-3) - 9 Templates

**Goal:** User support and frequently used patterns

**Templates:**

- S-007 (Troubleshooting) - User support essential
- S-017 (FAQ) - Common questions
- S-018 (Installation) - User onboarding
- S-019 (Code Example) - Language learning
- ~~CODE-GO-001 (Go Main)~~ - ✅ Complete (CODE-GO-003)
- ~~CODE-GO-003 (Go Test)~~ - ✅ Complete (CODE-GO-004)
- ~~CODE-C-001/002 (C Source/Header)~~ - ✅ Complete
- ~~CODE-ASM-001/002 (Assembly Source/Include)~~ - ✅ Complete (2025-11-30)
- ~~CODE-SH-001 (Shell Script)~~ - ✅ Complete (2025-11-30)
- ~~CODE-LD-001 (Linker Script)~~ - ✅ Complete (2025-11-30)
- ~~CODE-MAKE-001 (Makefile)~~ - ✅ Complete (2025-11-25)
- ~~CODE-JSON-003 (Health Map)~~ - ✅ Complete (2025-11-29)
- G-010/011 (Changelog/Contributing) - Community infrastructure
- S-005/011 (Release Notes/Design Docs) - Development flow

**Phase 2 Total:** ~30-35 hours execution (distributed across 2 weeks)

---

### Phase 3: Medium Priority Templates (Month 1) - 12 Templates

**Goal:** Supporting documentation and shell scripts

**Templates:**

- G-012 (Roadmap) - Vision documentation
- S-003/006/009/013/014/015/016 - Various supporting docs
- CODE-SHELL-001/002 (Bash/Shell) - Scripting
- CODE-TS-001/002 (TypeScript) - VSCode extensions
- CODE-JS-001 (JavaScript) - Web/tooling
- CODE-JSONC-001 (JSONC) - Commented JSON
- CODE-CONFIG-001 (Config files) - Minimal headers

**Phase 3 Total:** ~40-45 hours execution (distributed across month)

---

### Phase 4: Future Scope Templates (Iterations 5-6) - 35 Templates

**Goal:** Prepare for OmniCode native language support

**Templates:** All future language support (Rust, C++, Zig, Swift, etc.)

**Timeline:** Build as languages are added to OmniCode

**Strategy:** Use proven patterns from Phase 1-3, adapt to new languages

---

### Quick Wins (Anytime)

**Low-effort, high-value templates:**

1. ~~**CODE-CONFIG-001**~~ - ✅ Complete (2025-11-29) - Full 4-block TOML template, 8 config files aligned
2. ~~**CODE-MAKE-001**~~ - ✅ Complete (2025-11-25)
3. ~~**CODE-JSON-003**~~ - ✅ Complete (2025-11-29) - Full `_metadata` pattern for health maps
4. ~~**CODE-YAML-001**~~ - ✅ Complete (2025-11-29) - YAML config with 4-block
5. **S-012** (Devlog) - Pattern exists, just formalize (2-3 hours)

**Total Quick Wins:** ~2-3 hours for 1 remaining template with immediate impact

---

## COORDINATION NOTES

### How to Update This Matrix

**When you build a template:**

1. Change Status from 📋 → ✅
2. Update Specification link to actual template file
3. Add real Examples (file paths using the template)
4. Update % Complete statistics

**When you discover a new need:**

1. Add row with ❌ Needed status
2. Assign priority based on blocking files and frequency
3. Create specification or link to research
4. Update total counts

**When priorities change:**

1. Update Priority column (🔴🟢🟡🔵)
2. Adjust phase assignments if needed
3. Document reason in Notes column

---

### Matrix Capabilities

**This matrix enables:**

✅ **Visibility** - See all 89 templates at a glance
✅ **Prioritization** - Build critical templates first (documentation focus now)
✅ **Coordination** - Track dependencies and completion status
✅ **Progress** - Monitor completion (currently 39% complete - 35/89 templates, code 100% complete)
✅ **Planning** - Estimate effort and timeline (Phase 1 = 15-18 hours for documentation)
✅ **Impact** - Know which templates unblock the most files
✅ **Strategy** - See full scope (current + future languages)

---

### Cross-Reference

**Related Matrices:**

- [CODE-ALIGNMENT-MATRIX.md](CODE-ALIGNMENT-MATRIX.md) - 257 code files + 128 doc files tracked
- [Research Findings](../status/.claude/2025-11-02-research-findings/) - Detailed specifications
- [Templates Folder](.) - Built templates and indices

**How the matrices work together:**

1. **TEMPLATE-MATRIX** (this file) answers: "What templates exist?"
2. **CODE-ALIGNMENT-MATRIX** answers: "Which files need which templates?"
3. Each code category in CODE-ALIGNMENT-MATRIX links to its template section here
4. Update both matrices when work completes

---

<div align="center">

**📋 Matrix Status:** ✅ Complete, comprehensive, and strategic

**Code Templates:** 100% Complete (26/26) - All current code templates ready for use

**Next Action:** Build Phase 1 documentation templates (4 remaining, ~15-18 hours) - S-012, S-002, S-004, S-010

**Projected Impact:** Unblock 55+ files, enable Iteration 4 documentation, standardize all devlogs

**Season:** Iteration 3 Refinement Checkpoint → Iteration 4+

---

**Last Updated:** 2025-11-30
**Maintained By:** Nova Dawn (CPI-SI)

*Excellence through systematic organization - templates that teach Kingdom Technology*

</div>
