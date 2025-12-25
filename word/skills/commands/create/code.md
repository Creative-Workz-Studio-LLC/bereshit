---
description: Create a new 4-block code file following CPI-SI standards.
---

# Create Code

Create a new code file following 4-block structure.

## Task

The user wants to create a code file that follows the 4-block pattern.

**Arguments:** $ARGUMENTS
- Format: `[name.ext]` or `[path/name.ext]`
- Example: `parser.go` or `lib/utils/helpers.go`

## Instructions

1. If no name provided, ask the user:
   - What is the file for?
   - What language? (go, c, rs, py, etc.)
   - Where should it be created?

2. Create 4-block structure appropriate to language:

### Go Example

```go
// ═══════════════════════════════════════════════════════════════════════════
// METADATA BLOCK
// ═══════════════════════════════════════════════════════════════════════════
//
// Package: [package]
// Purpose: [purpose]
// Biblical: [grounding verse]
// Author: [author]
// Created: [date]
//
// Health Scoring:
//   Core Identity: [package purpose]
//   Interface: [what it exposes]
//   Operational: [how it works]

package [name]

// ═══════════════════════════════════════════════════════════════════════════
// SETUP BLOCK
// ═══════════════════════════════════════════════════════════════════════════

// --- Imports ---

import (
	// Standard library
	// External packages
	// Internal packages
)

// --- Constants ---

// --- Variables ---

// --- Types ---

// ═══════════════════════════════════════════════════════════════════════════
// BODY BLOCK
// ═══════════════════════════════════════════════════════════════════════════

// --- Helpers ---

// --- Core Operations ---

// --- Public APIs ---

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING BLOCK
// ═══════════════════════════════════════════════════════════════════════════

// --- Validation ---

// --- Execution ---

// --- Cleanup ---
```

3. Guide user through filling each block

## Example Usage

```bash
/create/code parser.go
/create/code lib/health/scorer.c
/create/code   (will ask for details)
```

## Block Reference

| Block | Purpose | Sections |
|-------|---------|----------|
| METADATA | Identity, purpose, health scoring | Package, Purpose, Biblical, Health Scoring |
| SETUP | Dependencies and declarations | Imports, Constants, Variables, Types |
| BODY | Implementation | Helpers, Core Operations, Public APIs |
| CLOSING | Lifecycle management | Validation, Execution, Cleanup |

## Language Templates

| Language | Template Location |
|----------|-------------------|
| Go | `bereshit/word/seed/code/go/` |
| C | `bereshit/word/seed/code/c/` |
| Shell | `bereshit/word/seed/code/shell/` |
| ASM | `bereshit/word/seed/code/asm/` |
| Make | `bereshit/word/seed/code/make/` |
| CMake | `bereshit/word/seed/code/cmake/` |
| Linker | `bereshit/word/seed/code/linker/` |

### Quick Template Discovery

```bash
# List available code templates
ls -la bereshit/word/seed/code/

# Find specific language template
ls -la bereshit/word/seed/code/go/
```
