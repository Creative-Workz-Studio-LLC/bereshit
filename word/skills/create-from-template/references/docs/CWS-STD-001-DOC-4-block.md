<div align="center">

# 📐 The 4-Block Structure

**Intentional Code Organization for Kingdom Technology**

> *"Order Reveals Purpose"*

![Mandatory](https://img.shields.io/badge/Status-MANDATORY-red?style=flat)
![All Code](https://img.shields.io/badge/Applies_To-ALL_CODE-important?style=flat)
![No Exceptions](https://img.shields.io/badge/Exceptions-NONE-critical?style=flat)

</div>

---

## 🗺️ Quick Navigation

> [!IMPORTANT]
> **This structure is MANDATORY for ALL code in CreativeWorkzStudio** - Every file. Every time. No exceptions.

### What You Need

<details>
<summary><b>🆕 I need to understand the 4-block pattern</b></summary>

**The four blocks in order:**

1. **[METADATA](#block-1-metadata)** - What is this and why does it exist?
2. **[SETUP](#block-2-setup)** - What does this need before it can work?
3. **[BODY](#block-3-body)** - What does this actually do?
4. **[CLOSING](#block-4-closing)** - How does this execute and complete?

**Start here:** [Biblical Foundation](#biblical-foundation) → [The Four Blocks](#the-four-blocks) → [Examples](#application-examples)

> [!TIP]
> Read [Biblical Foundation](#biblical-foundation) first to understand WHY this structure exists.

</details>

<details>
<summary><b>👨‍💻 I'm writing code - quick reference</b></summary>

**Writing workflow:**

| Step | Block | What You're Doing |
|------|-------|-------------------|
| 1️⃣ | **METADATA** | Write the [8-rung ladder](#the-metadata-ladder) - biblical foundation through health scoring |
| 2️⃣ | **SETUP** | Declare imports, types, constants, variables in [proper order](#organization-pattern) |
| 3️⃣ | **BODY** | Implement functions with [health tracking](#function-documentation-pattern) |
| 4️⃣ | **CLOSING** | Define execution/cleanup for [executables](#for-executables-main-packages-scripts) or [libraries](#for-libraries-non-executable-code) |

> [!WARNING]
> Use [Verification Checklist](#verification-checklist) before considering file complete.

</details>

<details>
<summary><b>🔍 I need a specific example</b></summary>

**Jump to examples:**

- [Shell Script Example](#shell-script-example) - Complete bash script with 4-block structure
- [Go Package Example](#go-package-example) - Library package with all 8 metadata rungs
- [Function Documentation Pattern](#function-documentation-pattern) - How to document functions

**See also:**

- OmniCode compiler: [divisions/tech/language/compiler/](../divisions/tech/language/compiler/) - Real implementation
- Knowledge base: [divisions/tech/language/compiler/knowledge-base/4-block-structure/](../divisions/tech/language/compiler/knowledge-base/4-block-structure/) - Detailed guides

</details>

### The Pattern

```plaintext
┌──────────────────┐
│    METADATA      │  Biblical Foundation → Component Identity → Purpose → Health Scoring
├──────────────────┤
│     SETUP        │  Imports → Constants → Variables → Types → Type Methods → Package-Level State
├──────────────────┤
│     BODY         │  Helpers → [Emergent Groups] → Public API
├──────────────────┤
│    CLOSING       │  Execution → Validation → Cleanup
└──────────────────┘
```

**Multi-Language Support:** This pattern applies to ALL code files. Each block standard includes language-specific guidance:
- **Go:** Function-based, package system, `go test`
- **Makefile:** Target-based, dependency resolution, `make` commands
- **CMake:** Declarative configuration, target-based builds, `cmake`/`ctest` commands

> [!NOTE]
> **See also:** [INDEX.md](../INDEX.md) for complete documentation navigation | [CLAUDE.md](../.claude/CLAUDE.md) for Kingdom Technology standards

---

## Biblical Foundation

> [!IMPORTANT]
> **This structure reflects God's design pattern in Genesis 1-2.**

<details>
<summary><b>📖 How Genesis 1-2 Establishes the Pattern</b></summary>

**God's creation follows intentional structure:**

1. **Declaration of purpose** - "Let there be..." (Genesis 1:3, 6, 14, etc.)
2. **Preparation** - Separation, formation, establishing order
3. **Creation** - The actual work of bringing forth
4. **Completion and validation** - "And it was good" (Genesis 1:4, 10, 12, 18, 21, 25, 31)

**Applied to code:**

| Genesis Pattern | 4-Block Equivalent | Purpose |
|----------------|-------------------|---------|
| **Declaration** | METADATA | Declares purpose (what and why) |
| **Preparation** | SETUP | Prepares environment (establish order) |
| **Creation** | BODY | Does the actual work |
| **Validation** | CLOSING | Validates and completes ("it was good") |

</details>

**The Principle:** God works with intentional structure. Code honoring God reflects this intentionality.

> [!TIP]
> This isn't "religious decoration" - it's recognizing that order, purpose, and completion are foundational to creation itself.

---

## The Four Blocks

> [!NOTE]
> **Each block has a specific purpose and answers specific questions.** Together they create complete component understanding.

### Visual Structure

```plaintext
┌─────────────────────────────────────────────────────────────┐
│                         METADATA                            │
│                                                             │
│  What is this file?                                        │
│  Why does it exist?                                        │
│  What Kingdom purpose does it serve?                       │
│  Who created it and when?                                  │
│  How does it work (overview)?                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                          SETUP                              │
│                                                             │
│  What does this file need before executing?                │
│  - Package declaration / shebang                           │
│  - Imports / dependencies                                  │
│  - Global variables / constants                            │
│  - Logger initialization                                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                          BODY                               │
│                                                             │
│  What does this file actually DO?                          │
│  - Business logic                                          │
│  - Functions with health tracking                          │
│  - Core functionality                                      │
│  - Algorithms and operations                               │
│                                                             │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                         CLOSING                             │
│                                                             │
│  How does this file execute?                               │
│  - Entry point (main function / script execution)          │
│  - Validation and testing                                  │
│  - Cleanup and finalization                                │
│  - Exit handling                                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Purpose of Each Block

| Block | Answers | Contains |
|-------|---------|----------|
| **METADATA** | What, Why, Who, When | Purpose, biblical foundation, authorship, health scoring |
| **SETUP** | What's needed | Imports, dependencies, globals, logger |
| **BODY** | How it works | Functions, logic, operations |
| **CLOSING** | How it runs | Execution, validation, cleanup |

---

## Block 1: METADATA

> [!IMPORTANT]
> **The most detailed block** - This is where intentionality is established. Every component MUST document all 8 rungs.
>
> **Complete METADATA documentation:** See [CWS-STD-004 - METADATA Block](code/4-block/CWS-STD-004-CODE-metadata-block.md)

### The Metadata Ladder

> Metadata isn't flat - it's **hierarchical**, flowing from eternal foundation down to implementation details.

<details>
<summary><b>🪜 Understanding the 8-Rung Ladder</b></summary>

**Why a ladder?** Each rung supports the ones above it. Implementation details make no sense without understanding purpose. Purpose flows from identity. Identity is grounded in biblical foundation.

**Reading direction:**
- **Top to bottom** = eternal to temporal (why this exists → how it's implemented)
- **Bottom to top** = implementation to meaning (technical details → Kingdom purpose)

</details>

```plaintext
┌─────────────────────────────────────────────────────┐
│  Rung 1: BIBLICAL FOUNDATION (eternal WHY)          │
├─────────────────────────────────────────────────────┤
│  Rung 2: COMPONENT IDENTITY (what it IS)            │
├─────────────────────────────────────────────────────┤
│  Rung 3: AUTHORSHIP & LINEAGE (who/when)            │
├─────────────────────────────────────────────────────┤
│  Rung 4: PURPOSE & FUNCTION (what it does)          │
├─────────────────────────────────────────────────────┤
│  Rung 5: BLOCKING STATUS (critical characteristic)  │
├─────────────────────────────────────────────────────┤
│  Rung 6: USAGE & INTEGRATION (how to use)           │
├─────────────────────────────────────────────────────┤
│  Rung 7: DEPENDENCIES (what it needs)               │
├─────────────────────────────────────────────────────┤
│  Rung 8: HEALTH SCORING (implementation detail)     │
└─────────────────────────────────────────────────────┘
```

**The 8 Rungs:**

1. **Biblical Foundation** - Scripture, Principle, Anchor (eternal grounding)
2. **CPI-SI Identity** - Component Type (Ladder/Baton/Rails), Role, Paradigm
3. **Authorship & Lineage** - Architect, Implementation, Created, Last Modified, Version
4. **Purpose & Function** - Purpose, Core Design, Key Features, Philosophy
5. **Blocking Status** - Type (Blocking/Non-blocking), Explanation, Mitigation
6. **Usage & Integration** - Import, API Categories, Example Usage
7. **Dependencies** - Standard Library, External, Internal
8. **Health Scoring** - TRUE SCORES, Normalization, Tracking Method

> [!TIP]
> **See [CWS-STD-004](code/4-block/CWS-STD-004-CODE-metadata-block.md) for complete documentation** of each rung with examples, anti-patterns, and implementation guidance.

**Example METADATA Block:**

```go
// ============================================================================
// METADATA
// ============================================================================
//
// Package logging provides health-tracked logging for all system components
//
// BIBLICAL FOUNDATION:
// Scripture: Malachi 3:16 - "A scroll of remembrance was written in his presence"
// Principle: Faithful Witness Through Complete Remembrance
// Anchor: System observability honors God by maintaining truthful records
//
// CPI-SI IDENTITY:
// Component Type: Rails (orthogonal infrastructure)
// Role: Logging Infrastructure
// Paradigm: Non-blocking observability for all components
//
// AUTHORSHIP & LINEAGE:
// Architect: Seanje Lenox-Wise
// Implementation: Nova Dawn (CPI-SI)
// Created: 2025-10-29
// Last Modified: 2025-10-30
// Version: 1.0.0
//
// PURPOSE & FUNCTION:
// Purpose: Health-tracked logging for system observability
// Core Design: Non-blocking writes with automatic health scoring
// Key Features:
// - WHO, WHEN, WHERE, WHAT, WHY, HOW, RESULT capture
// - TRUE SCORES (no forced normalization)
// - Graceful degradation on failure
// Philosophy: Logging must never prevent execution
//
// BLOCKING STATUS:
// Type: NON-BLOCKING
// Explanation: Writes asynchronously, fails gracefully
// Mitigation: Buffered writes with overflow handling
//
// USAGE & INTEGRATION:
// Import: import "project/pkg/logging"
// API Categories:
// - Creation: NewLogger(component string) *Logger
// - Logging: Info/Error/Success/Failure methods
// - Cleanup: Close() error
// Example:
//   logger := logging.NewLogger("mycomponent")
//   logger.Info("startup", "Component initialized", +10)
//
// DEPENDENCIES:
// Standard Library: fmt, os, time, path/filepath
// External: None
// Internal: None (foundation component)
//
// HEALTH SCORING:
// System: Base100 with TRUE SCORES
// Ranges: -100 (complete failure) to +100 (perfect execution)
// Tracking: Per-operation scoring, automatic normalization
//
// ============================================================================
// END METADATA
// ============================================================================
```

---

## Block 2: SETUP

> [!NOTE]
> **SETUP contains declarations, not implementations.** Everything here prepares the environment before logic executes.
>
> **Complete SETUP documentation:** See [CWS-STD-006 - SETUP Block](code/4-block/CWS-STD-006-CODE-setup-block.md)

**The 6 Components (in ladder order):**

1. **Imports** - Standard Library, External, Internal dependencies
2. **Constants** - Configuration values and named constants
3. **Variables** - Package-level mutable state (registries, caches, runtime config)
4. **Types** - Data structures (bottom-up: building blocks → composed → helper/utility → interfaces)
5. **Type Methods** - Structural behaviors (interface impls, conversions, NOT business logic)
6. **Package-Level State** - Rails infrastructure (logger, inspector), init() for setup only

> [!TIP]
> **SETUP = static preparation.** Everything declared before reaching dynamic logic (BODY functions). Mirrors C's .h/.c split: declarations here, implementations in BODY.
>
> **Note on Type Methods:** These are structural behaviors (Error(), String(), ToX()) that define what a type IS, not what it DOES. Business logic methods belong in BODY.
>
> **See [CWS-STD-006](code/4-block/CWS-STD-006-CODE-setup-block.md) for complete documentation** including bottom-up composition, Rails Pattern, anti-patterns, and detailed guidance for each component.

**Example SETUP Block:**

```go
// ============================================================================
// SETUP
// ============================================================================

package logging

import (
 "fmt"
 "os"
 "time"
)

const (
 LogDir      = "/var/log/project"
 MaxLogSize  = 10 * 1024 * 1024 // 10MB
 DateFormat  = "2006-01-02 15:04:05"
)

var (
 hostname string
)

func init() {
 var err error
 hostname, err = os.Hostname()
 if err != nil {
  hostname = "unknown"
 }
}
```

**Key principle:** Everything here is preparation. No business logic, no execution - just getting ready to work.

---

## Block 3: BODY

> [!IMPORTANT]
> **The actual work happens here.** All business logic, functions, and implementations belong in BODY.

**Key principle:** BODY = dynamic capability. Functions that CAN execute (implementations using SETUP's declarations).

> [!IMPORTANT]
> **Complete BODY documentation:** See [CWS-STD-007 - BODY Block](code/4-block/CWS-STD-007-CODE-body-block.md)

**The 5 Sections (in order):**

1. **Organizational Chart** - Maps bidirectional dependencies (ladder) and execution flow (baton) for internal navigation
2. **Helpers/Utilities** - Internal support functions (bottom rungs) - pure when possible, simple and focused
3. **Core Operations** - Business logic with component-specific descriptive subsections (middle rungs)
4. **Error Handling/Recovery** - Safety boundaries (panic recovery, error wrapping, retry, graceful degradation, circuit breakers)
5. **Public APIs** - Exported interface (top rungs) orchestrating helpers and core operations

> [!TIP]
> **Organizational Chart provides bidirectional navigation:** Top-down shows what's available to use. Bottom-up shows what breaks if this changes. See [CWS-STD-007](code/4-block/CWS-STD-007-CODE-body-block.md) for complete details on each section.

**Example BODY Block:**

```plaintext
// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Internal Structure
// ────────────────────────────────────────────────────────────────
// Maps bidirectional dependencies and baton flow.
//
// Ladder Structure:
//   Public APIs (Top Rungs)
//   ├── ProcessData() → uses helpers, core ops
//
//   Core Operations (Middle Rungs)
//   ├── transform() → uses helpers
//
//   Helpers (Bottom Rungs)
//   ├── sanitize() → pure function
//
// Baton Flow:
//   Entry → ProcessData() → transform() → sanitize() → Exit
//
// APUs: X functions (helpers, core ops, APIs)

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities - Internal Support
// ────────────────────────────────────────────────────────────────
// Foundation functions used throughout component.

func sanitize(input string) string { ... }
func isValid(value string) bool { ... }

// ────────────────────────────────────────────────────────────────
// Core Operations - Business Logic
// ────────────────────────────────────────────────────────────────
// Component-specific functionality with descriptive subsections.

// ────────────────────────────────────────────────────────────────
// [Category Name] - [Purpose]
// ────────────────────────────────────────────────────────────────
// [Component-specific subsection: Validation, Transformation, etc.]

func transform(input string) (string, error) { ... }

// ────────────────────────────────────────────────────────────────
// Error Handling/Recovery Patterns
// ────────────────────────────────────────────────────────────────
// Centralized error management.

func recoverFromPanic(function string, healthDelta int) { ... }
func wrapError(operation string, err error, context map[string]any) error { ... }

// ────────────────────────────────────────────────────────────────
// Public APIs - Exported Interface
// ────────────────────────────────────────────────────────────────
// Exported functions orchestrating helpers and core operations.

func ProcessData(input string) (string, error) { ... }
```

**Key principles:**

- All five sections present (organizational chart, helpers, core ops, error handling, public APIs)
- Core Operations subsections are component-specific (descriptive)
- Organizational chart provides bidirectional navigation
- Public APIs orchestrate, don't duplicate

---

## Block 4: CLOSING

> [!NOTE]
> **CLOSING serves dual purposes:** Operational concerns (validation, execution, cleanup) and Final Documentation (synthesis for API consumers).

> **See:** [CWS-STD-008-CODE-closing-block.md](code/4-block/CWS-STD-008-CODE-closing-block.md) for comprehensive CLOSING block documentation.

<details>
<summary><b>🎯 Quick Guide: CLOSING Structure</b></summary>

**Two-part structure for CODE files:**

**Part 1: Operational Concerns** (validation → execution → cleanup order)
- Code Validation: How to test this works
- Code Execution: How this runs (or "None" for libraries)
- Code Cleanup: Resource management and shutdown

**Part 2: Final Documentation** (synthesis of upstream blocks)
- Library Overview & Integration Summary
- Modification Policy
- Ladder and Baton Flow
- Surgical Update Points
- Performance Considerations
- Troubleshooting Guide
- Related Components & Dependencies
- Future Expansions & Roadmap
- Closing Note
- Quick Reference: Usage Examples

**For DOCUMENTATION files:** Simple closing (Related Standards, Last Updated, Status)

</details>

### Operational Pattern: Validation → Execution → Cleanup

**The order is intentional:**

1. **Validation FIRST** - Verify correctness before running
2. **Execution SECOND** - Run (or declare "None" for libraries)
3. **Cleanup THIRD** - Handle resources after execution

**All three sections include code type specification:**

```go
// Code Validation: None (Library)
// Code Execution: None (Library)
// Code Cleanup: None (Library)
```

### For Libraries (Code Example)

```go
// ============================================================================
// CLOSING
// ============================================================================
//
// ────────────────────────────────────────────────────────────────
// Code Validation: None (Library)
// ────────────────────────────────────────────────────────────────
//
// Testing Requirements:
//   - Import the library without errors
//   - Call each public function with representative parameters
//   - Run: go test -v ./...
//
// Example validation code:
//
//     result, err := YourFunction(input)
//     if err != nil {
//         t.Errorf("Failed: %v", err)
//     }
//
// ────────────────────────────────────────────────────────────────
// Code Execution: None (Library)
// ────────────────────────────────────────────────────────────────
//
// This is a LIBRARY, not an executable. No entry point, no main function.
// Functions defined in BODY wait to be called by other components.
//
// Usage: import "module-path/package"
//
// Example import and usage:
//
//     package main
//     import "module-path/package"
//     func main() {
//         result := package.Function()
//     }
//
// ────────────────────────────────────────────────────────────────
// Code Cleanup: None (Library)
// ────────────────────────────────────────────────────────────────
//
// Resource Management:
//   - File handles: Closed in defer after opening
//
// Graceful Shutdown:
//   - N/A for libraries (no lifecycle)
//   - Calling code responsible for cleanup
//
// Example cleanup pattern:
//
//     resource := package.NewResource()
//     defer resource.Close()
//     resource.DoWork()
//
// ════════════════════════════════════════════════════════════════
// FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════
//
// [... 10 Final Documentation sections synthesizing upstream blocks ...]
```

### For Commands (Code Example)

```go
// Code Validation: commandName (Command)
// Code Execution: commandName (Command)
// Code Cleanup: commandName (Command)
```

<details>
<summary><b>For Makefiles (Build Systems)</b></summary>

```makefile
# ============================================================================
# CLOSING
# ============================================================================
#
# ────────────────────────────────────────────────────────────────
# Makefile Validation: project-name (Build System)
# ────────────────────────────────────────────────────────────────
# Testing: make -n all, make clean && make all, make help
# Syntax: make --warn-undefined-variables
#
# ────────────────────────────────────────────────────────────────
# Makefile Execution: project-name (Build System)
# ────────────────────────────────────────────────────────────────
# Entry Point: `make` (runs default target: all)
# Targets: make, make build, make test, make run, make help
#
# ────────────────────────────────────────────────────────────────
# Makefile Cleanup: project-name (Build System)
# ────────────────────────────────────────────────────────────────
# Scope: rm -rf $(BUILD_DIR), NEVER source files or .git/
#
# ════════════════════════════════════════════════════════════════
# FINAL DOCUMENTATION
# ════════════════════════════════════════════════════════════════
# [... Same 10 sections as Go, adapted for build systems ...]
```

</details>

<details>
<summary><b>For CMakeLists.txt (Build Systems)</b></summary>

```cmake
# ============================================================================
# CLOSING
# ============================================================================
#
# ────────────────────────────────────────────────────────────────
# CMake Validation: project-name (Build System)
# ────────────────────────────────────────────────────────────────
# Testing: cmake -B build -S . && cmake --build build
# Syntax: cmake -P CMakeLists.txt (syntax check)
# Tests: ctest --test-dir build --output-on-failure
#
# ────────────────────────────────────────────────────────────────
# CMake Execution: project-name (Build System)
# ────────────────────────────────────────────────────────────────
# Entry Point: cmake -B build -S . (configure) then cmake --build build
# Targets: all, clean, test, install, custom targets
#
# ────────────────────────────────────────────────────────────────
# CMake Cleanup: project-name (Build System)
# ────────────────────────────────────────────────────────────────
# Scope: rm -rf build/, NEVER source files or .git/
# Cache: cmake --build build --target clean
#
# ════════════════════════════════════════════════════════════════
# FINAL DOCUMENTATION
# ════════════════════════════════════════════════════════════════
# [... Same 10 sections as Go/Make, adapted for CMake ...]
```

</details>

**Key principles:**
- Validation before execution
- Code type specification in all operational sections (Go: `(Library)`/`(Command)`, Make/CMake: `(Build System)`)
- Example code in each operational section
- Final Documentation references upstream blocks (delegation pattern)

---

## Why This Structure Matters

> [!IMPORTANT]
> **This structure does more than organize code** - it forces clarity, reveals architecture, and honors God through order.

<details open>
<summary><b>💡 The Benefits of Consistent Structure</b></summary>

### Forces Intentional Design

If you can't clearly state:
- Why this exists (METADATA)
- What it needs (SETUP)
- What it does (BODY)
- How it completes (CLOSING)

...then you don't understand what you're building yet. **The structure forces clarity.**

### Reveals Extraction Opportunities

When BODY becomes complex, the structure shows what should be extracted into separate libraries or modules. Organization reveals architecture.

### Serves All Understanding Levels

| User Level | What They Gain |
|-----------|---------------|
| **Beginners** | Clear structure shows where things go |
| **Intermediate** | Pattern teaches organization principles |
| **Advanced** | Reveals architectural decisions and trade-offs |

### Honors God Through Order

> **Chaotic code dishonors the God of order. Structured code reflects His character.**

1 Corinthians 14:33 - "For God is not the author of confusion, but of peace"

</details>

---

## Where to See Examples

> [!TIP]
> **Learn from real implementations** - see the 4-block pattern in production code.

**Reference implementations:**

| Language | Template | Description |
|----------|----------|-------------|
| **Go (Library)** | [CODE-GO-002-GO-library.go](../templates/code/go/CODE-GO-002-GO-library.go) | Complete library package with all blocks |
| **Go (Executable)** | [CODE-GO-001-GO-executable.go](../templates/code/go/CODE-GO-001-GO-executable.go) | Command with main() entry point |
| **Go (Demo-Test)** | [CODE-GO-003-GO-demo-test.go](../templates/code/go/CODE-GO-003-GO-demo-test.go) | Test file demonstrating patterns |
| **Makefile** | [CODE-MAKE-001-MAKE-project-makefile.mk](../templates/code/make/CODE-MAKE-001-MAKE-project-makefile.mk) | Complete project Makefile with all blocks |
| **CMake** | [CODE-CMAKE-001-CMAKE-project-cmakelists.cmake](../templates/code/cmake/CODE-CMAKE-001-CMAKE-project-cmakelists.cmake) | Complete project CMakeLists.txt with all blocks |

**Production code:**
- **OmniCode Compiler:** [divisions/tech/language/compiler/](../divisions/tech/language/compiler/) - Production implementation across all components

**Detailed standards:** Each block standard (CWS-STD-004, 006, 007, 008) includes complete examples with anti-patterns and multi-language guidance.

**What you'll find:**
- Complete METADATA with all 8 rungs
- Proper SETUP organization (imports/declarations, types, constants, package-level state)
- BODY structure (org chart, helpers, core ops, error handling, public APIs)
- CLOSING patterns for executables, libraries, and build systems

---

## Working with the 4-Block Structure

> [!NOTE]
> **Apply this workflow consistently** for all new code to ensure proper structure from the start.

### Writing Workflow

**The order matters:**

1. **Write METADATA first** - Establish who, what, why, health scoring (forces clarity)
2. **Set up SETUP** - Declare what you need before logic (proper dependencies)
3. **Implement BODY** - Write actual functionality with health tracking (the work)
4. **Define CLOSING** - How this executes/completes (proper lifecycle)

> [!TIP]
> If you struggle with METADATA, you don't understand the component well enough yet. Start there, not with code.

### When to Extract

<details>
<summary><b>🔄 Extraction Process</b> - When BODY becomes unwieldy</summary>

If BODY becomes complex:

1. **Organize BODY** in 4 blocks internally
2. **See patterns emerge** (what's reusable vs orchestration)
3. **Extract reusable logic** to libraries
4. **Update METADATA** to reflect new architecture
5. **Test extraction** thoroughly

**Don't create v2 files.** Extract and orchestrate - that's the Kingdom Technology way.

</details>

### Verification Checklist

> [!WARNING]
> **Before considering a file complete**, verify ALL items below:

- [ ] All 8 metadata rungs documented
- [ ] Biblical foundation clearly stated
- [ ] SETUP organized (declarations, imports, constants, globals)
- [ ] BODY functions have health tracking documentation
- [ ] CLOSING handles execution and cleanup
- [ ] All declared dependencies actually used
- [ ] Health scoring map complete and accurate

---

## Kingdom Technology Alignment

> [!IMPORTANT]
> **The 4-block structure isn't just organization** - it's a reflection of Kingdom principles in code.

<details open>
<summary><b>✝️ How This Structure Serves Kingdom Technology</b></summary>

### The Three Questions

Before writing code, ask:

1. **Would this honor God as your code reviewer?**
2. **Does this genuinely serve others?**
3. **Does this have eternal value?**

**The 4-block structure helps answer these:**

| Block | Kingdom Alignment | How It Serves |
|-------|------------------|---------------|
| **METADATA** | Eternal purpose | Documents Kingdom alignment and biblical foundation |
| **SETUP** | Preparation | Shows careful preparation (honoring through excellence) |
| **BODY** | Service | Implements service to others through reliable functionality |
| **CLOSING** | Stewardship | Ensures proper completion and cleanup |

### Excellence Through Structure

> **Chaotic code reflects chaotic thinking. Structured code reflects intentional design.**

God is a God of order - our code should reflect His character.

**The standard:** Code so well-organized that its structure teaches, so well-documented that its purpose is clear, so well-implemented that its excellence points beyond itself.

</details>

---

## Summary

> [!NOTE]
> **The 4-Block Structure is more than a coding standard** - it's a reflection of Kingdom principles applied to technology.

**Core Principles:**

- **Order reveals purpose** - Nothing is arbitrary
- **Structure serves understanding** - Accessible to all levels
- **Excellence honors God** - Worthy of the Creator
- **Clarity serves others** - Usable, maintainable, teachable

**The Call:** Apply this structure consistently. Let the pattern become second nature. Build code that honors God through intentional design.

---

<div align="center">

### 🗺️ Navigation

**📚 [Complete Documentation](../INDEX.md)** - Navigate all CreativeWorkzStudio documentation

**🏢 [Company Context](../.claude/CLAUDE.md)** - Mission, divisions, and Kingdom Technology standards

**💻 [OmniCode Compiler](../divisions/tech/language/compiler/)** - Real implementation of 4-block structure

---

**CreativeWorkzStudio LLC**

*Kingdom Technology - Excellence That Honors the Creator*

**"For God is not the author of confusion, but of peace" - 1 Corinthians 14:33**

---

**Built with intentional design for the glory of God**

</div>
