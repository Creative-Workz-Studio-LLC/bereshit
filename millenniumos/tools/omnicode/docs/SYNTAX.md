# OmniCode Syntax Reference

> *"For precept must be upon precept, line upon line."* — Isaiah 28:10

**Complete syntax reference.** Look up any construct.

---

## Table of Contents

1. [Pragma Line](#pragma-line)
2. [Markers](#markers)
3. [Pragma Sections](#pragma-sections)
4. [Block Structure](#block-structure)
5. [Data Types](#data-types)
6. [Comments](#comments)
7. [Versioning](#versioning)

---

## Pragma Line

**The first line identifies the file format.**

```
/// @omni <format> --<language> --<type>
```

| Part | Values | Example |
|------|--------|---------|
| `format` | `code`, `data`, `document` | `@omni code` |
| `language` | `c`, `go`, `bash`, `toml` | `--c` |
| `type` | `source`, `header`, `library`, `config` | `--source` |

**Examples:**

```c
/// @omni code --c --source        // C source file
/// @omni code --c --header        // C header file
/// @omni code --go --library      // Go library
/// @omni data --toml --config     // TOML config
/// @omni document --md            // Markdown doc
```

---

## Markers

**Markers indicate requirement level.**

| Marker | Level | Meaning |
|--------|-------|---------|
| `@omni:req` | REQUIRED | Must exist, validation fails without |
| `@omni:inh` | INHERITED | From template, override allowed |
| `@omni:ins` | INSTANCE | File-specific, unique values |
| `@omni:opt` | OPTIONAL | Can omit if not applicable |

**Usage:**

```c
/// @omni:req  key   = MY-FILE       // Required: validation fails without
/// @omni:inh  type  = source        // Inherited: from template
/// @omni:ins  layer = kernel        // Instance: this file's specific value
/// @omni:opt  notes = "draft"       // Optional: can omit
```

---

## Pragma Sections

### P.1 CORE — Identity

**Required fields for every file.**

```c
/// P.1 CORE — Identity [CORE]
///
/// @omni:req  key    = UNIQUE-KEY
/// @omni:req  from   = template/path
/// @omni:req  at     = a-01.00
```

| Field | Purpose | Format |
|-------|---------|--------|
| `key` | Unique identifier | UPPER-CASE-DASHED |
| `from` | Source template | path/to/template |
| `at` | Current version | STAGE-PHASE.COMPLETION |

### P.2 FAMILY — Classification

**Type information inherited from template.**

```c
/// P.2 FAMILY — Classification [FAMILY]
///
/// @omni:inh  type       = source
/// @omni:inh  subtype    = implementation
/// @omni:inh  role       = api
/// @omni:inh  structure  = 4-block
```

| Field | Values |
|-------|--------|
| `type` | `source`, `header`, `config`, `document` |
| `subtype` | `implementation`, `interface`, `test`, `demo` |
| `role` | `api`, `internal`, `utility`, `core` |
| `structure` | `3-block`, `4-block`, `5-block` |

### P.3 INSTANCE — File Details

**This file's specific information.**

```c
/// P.3 INSTANCE — File Details [INSTANCE]
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = kernel
/// @omni:ins  includes   = [header1.h, header2.h]
/// @omni:ins  provides   = [func1, func2, struct1]
```

| Field | Purpose |
|-------|---------|
| `component` | Which system component |
| `layer` | Architectural layer (0-8) |
| `includes` | Dependencies |
| `provides` | Public exports |

### P.4 ARCHITECTURE — Scale Context

**Position in the 9-layer pyramid.**

```c
/// P.4 ARCHITECTURE — Scaling Context [ARCH]
///
/// @omni:ins  layers  = [0:kernel, 1:system, 2:runtime, ...]
/// @omni:ins  scale   = 3^n [1, 3, 9, 27, 81, 243, 729, 2187, 6561]
/// @omni:ins  pattern = 4+1+4 [below:0-3, center:4, above:5-8]
/// @omni:ins  anchor  = genesis_1_1
```

### P.5 SUMMARY — Human Identity

**Human-readable title and description.**

```c
/// P.5 SUMMARY — Human Identity [SUMMARY]
///
/// @omni:req  title = My Component Title
/// @omni:req  brief = One sentence describing what this does
```

---

## Block Structure

### 3-Block (Data)

```
METADATA → CONTENT → CLOSING
```

Use for: configs, settings, simple data files.

### 4-Block (Code)

```
METADATA → SETUP → BODY → CLOSING
```

**METADATA** - Identity, purpose, who made it
**SETUP** - Imports, constants, types
**BODY** - Functions, logic, implementation
**CLOSING** - Usage, troubleshooting, reference

### 5-Block (Documentation)

```
METADATA → HEADER → CONTEXT → CONTENT → FOOTER
```

**METADATA** - Machine-readable identity
**HEADER** - Title, navigation, status
**CONTEXT** - Prerequisites, definitions
**CONTENT** - Main body text
**FOOTER** - References, related docs

### Block Syntax

```c
/// ═══════════════════════════════════════════════════════
/// BLOCK_NAME BLOCK [TAG]
/// ═══════════════════════════════════════════════════════
///
/// Content...
///
/// ═══════════════════════════════════════════════════════
/// END BLOCK_NAME
/// ═══════════════════════════════════════════════════════
```

**Minimal form:**

```c
/// [BLOCK_NAME] BLOCK [TAG]
/// Content...
/// END [BLOCK_NAME]
```

---

## Data Types

### Arrays

```c
/// @omni:ins includes = [file1.h, file2.h, file3.h]
```

### Strings

```c
/// @omni:req title = "Title With Spaces"
/// @omni:req title = Title_Without_Spaces
```

### Numbers

```c
/// @omni:ins layer = 4
/// @omni:ins scale = 3^n
```

### Booleans

```c
/// @omni:ins deprecated = false
/// @omni:ins public     = true
```

---

## Comments

### Regular Comments

```c
// Single line comment
/* Multi-line
   comment */
```

### OmniCode Comments (Preserved)

```c
/// This is an OmniCode documentation comment
/// It will be preserved in generated output
```

### Section Separators

```c
/// ─────────────────────────────────────────
/// Section Name [TAG]
/// ─────────────────────────────────────────
```

Heavy separator (block boundaries):
```c
/// ═══════════════════════════════════════════════════════
```

Light separator (within blocks):
```c
/// ─────────────────────────────────────────────────────────
```

---

## Versioning

**Format:** `STAGE-PHASE.COMPLETION`

| Part | Values | Meaning |
|------|--------|---------|
| STAGE | a, b, c | Alpha(-1), Beta(0), Stable(+1) |
| PHASE | 00-99 | Iteration number |
| COMPLETION | 00-99 | Progress within phase |

**Examples:**

| Version | Meaning |
|---------|---------|
| `a-01.00` | Alpha, phase 1, just started |
| `a-01.50` | Alpha, phase 1, halfway done |
| `b-02.00` | Beta, phase 2, beginning |
| `c-01.00` | Stable, first release |

---

## Validation

OmniCode files can be validated:

```bash
omni validate myfile.c           # Check structure
omni validate --strict myfile.c  # Strict mode
omni check-chain myfile.c        # Verify template chain
```

**What gets checked:**

- [ ] Pragma line present and valid
- [ ] All required markers have values
- [ ] Block structure complete
- [ ] Block order correct
- [ ] Template chain valid

---

## Quick Reference

```c
/// @omni code --c --source
///
/// @omni:req key   = MY-KEY
/// @omni:req from  = template/path
/// @omni:req at    = a-01.00
/// @omni:inh type  = source
/// @omni:ins layer = kernel
/// @omni:req title = My Title
/// @omni:req brief = What this does

/// METADATA BLOCK [METADATA]
/// Identity and purpose
/// END METADATA

/// SETUP BLOCK [SETUP]
#include <header.h>
/// END SETUP

/// BODY BLOCK [BODY]
void my_function() { }
/// END BODY

/// CLOSING BLOCK [CLOSING]
/// Usage and reference
/// END CLOSING
```
