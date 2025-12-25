---
name: format-lookup
description: Quick lookup of OmniCode format mappings - how OmniCode sections translate to specific formats (AsciiDoc, Go, C, etc.). Use this skill when Nova Dawn needs to know how an OmniCode element maps to a target format.
version: "2.0.0"
allowed-tools: "Read,Grep,Bash"
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-skills-format-lookup
:title: Format Lookup
:type: Skill
:status: Active
:version: 2.0.0
:created: 2025-11-15
:updated: 2025-12-08
:authors: Nova Dawn
:purpose: Quick lookup of OmniCode format mappings for translation to target formats
:category: omnicode
:biblical_foundation: Ecclesiastes 3:1 - "To every thing there is a season, and a time to every purpose under the heaven"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Format Lookup

> *"To every thing there is a season, and a time to every purpose under the heaven."* — Ecclesiastes 3:1

**Quick lookup of how OmniCode maps to target formats.**

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use-this-skill) | Decision criteria |
| [Format Mappings](#format-mapping-reference) | Complete mapping tables |
| [Lookup Process](#lookup-process) | Step-by-step |
| [Scripts](#scripts) | Go lookup helpers |
| [Quick Reference](#quick-reference-card) | Summary tables |

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Context

### Scope

| In Scope | Out of Scope |
|----------|--------------|
| Quick mapping lookups | Full format analysis (use format-bridge agent) |
| OmniCode → target format | Validating structure (use validate-omni) |
| Reference during template work | Creating files (use create-from-template) |

### Prerequisites

| Requirement | Reference |
|-------------|-----------|
| OmniCode syntax | bereshit/word/omni/syntax.omni |
| Format definitions | bereshit/word/omni/seed/ |

### Key Terms

| Term | Definition |
|------|------------|
| **Format** | Target output language (go, adoc, md, c, rs) |
| **Mapping** | How OmniCode element translates to format |
| **Pragma** | Declaration: `#!omni [type] --[format]` |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## When to Use This Skill

**Use when:**

- Need to know how an OmniCode element maps to a format
- Writing format-aware OmniCode
- Checking correct translation
- Quick reference during template work

**Do NOT use for:**

- Full format analysis → use Phoebe (format-bridge agent)
- Validating structure → use validate-omni skill
- Creating files → use create-from-template skill

---

## Format Mapping Reference

### Documentation Formats

#### AsciiDoc (--adoc)

| OmniCode | AsciiDoc | Example |
|----------|----------|---------|
| `#!omni documentation --adoc` | `////` block with pragma | `////\n#!omni documentation --adoc\n////` |
| `this is a [document] for` | `:title:` | `:title: Document Name` |
| `keyed as` | `:key:` | `:key: B-WORD-DOC-001` |
| `following` | `:type:` | `:type: Reference` |
| `classifies as` | `:tags:` | `:tags: documentation, reference` |
| `derives from` | `:derives_from:` | `:derives_from: path/to/template.omni` |
| `strictness is` | `:strictness:` | `:strictness: tight` |
| `grounded in: scripture` | `:biblical_foundation:` | `:biblical_foundation: Genesis 1:1` |
| `authored by: architect` | `:author:` | `:author: Seanje Lenox-Wise` |
| `authored by: writer` | `:authors:` | `:authors: Nova Dawn` |
| `created` | `:created:` | `:created: 2025-11-30` |
| `version` | `:version:` | `:version: 1.0.0` |
| `modified` | `:updated:` | `:updated: 2025-12-01` |
| `exists to: purpose` | `:purpose:` | `:purpose: Define syntax` |
| `exists to: design` | `:description:` | `:description: Spec by example` |
| `presented as: title` | `= Title` | `= Document Title` |
| `presented as: tagline` | `_tagline_` | `_Kingdom Technology_` |
| `structure of this document` | Section organization | `== Section`, `=== Subsection` |
| `clarifications` | Admonition blocks | `NOTE:`, `TIP:`, `CAUTION:` |
| `closing note` | Final paragraph | Scripture reference |

#### Markdown (--md)

| OmniCode | Markdown | Example |
|----------|----------|---------|
| `#!omni documentation --md` | HTML comment | `<!--\n#!omni documentation --md\n-->` |
| `this is a [document] for` | YAML `title:` | `title: Document Name` |
| `keyed as` | YAML `key:` | `key: B-WORD-DOC-001` |
| `grounded in: scripture` | YAML `biblical_foundation:` | `biblical_foundation: Genesis 1:1` |
| `presented as: title` | `# Title` | `# Document Title` |
| `structure of this document` | `##`, `###` headers | `## Section` |

### Code Formats

#### Go (--go)

| OmniCode | Go | Example |
|----------|-----|---------|
| `#!omni code --go` | Generated comment | `// Code generated from OmniCode.` |
| `this is a [code] for` | Package doc comment | `// Package name provides...` |
| `keyed as` | `// Key:` comment | `// Key: PKG-HEALTH-001` |
| `grounded in: scripture` | `// Biblical:` comment | `// Biblical: Proverbs 4:23` |
| `authored by` | `// Authors:` comment | `// Authors: Nova Dawn` |
| `version` | `// Version:` comment | `// Version: 1.0.0` |
| `exists to: purpose` | Package doc first line | First sentence of package doc |
| `inputs: requires: from stdlib (1)` | Standard library imports | `import "fmt"` |
| `inputs: requires: from internal (0)` | Internal package imports | `import "project/pkg/health"` |
| `inputs: requires: from external (-1)` | Third-party imports | `import "github.com/..."` |
| `definitions: constants` | `const` block | `const MaxScore = 100` |
| `definitions: variables` | `var` block | `var registry = ...` |
| `definitions: assemblies` | `type struct` | `type Config struct {...}` |
| `definitions: methods` | Method receivers | `func (c *Config) Validate()` |
| `implementation: helpers` | Unexported functions | `func helper() {...}` |
| `implementation: operations` | Core functions | `func calculate() {...}` |
| `interface: exposes: exported` | Exported (Capitalized) | `func Calculate() {...}` |
| `interface: exposes: internal` | Unexported (lowercase) | `func validate() {...}` |
| `operations: executed through: main` | `func main()` | `func main() {...}` |
| `operations: cleaned by` | `defer` statements | `defer file.Close()` |

#### C (--c) [PLANNED]

| OmniCode | C | Example |
|----------|---|---------|
| `#!omni code --c` | Generated comment | `/* Code generated from OmniCode. */` |
| `this is a [code] for` | File header comment | `/* @file name.c ... */` |
| `inputs: requires` | `#include` statements | `#include <stdio.h>` |
| `definitions: constants` | `#define` macros | `#define MAX_SIZE 100` |
| `definitions: assemblies` | `typedef struct` | `typedef struct {...} Config;` |
| `implementation: helpers` | `static` functions | `static void helper() {...}` |
| `interface: exposes` | Non-static functions | `void public_func() {...}` |
| `operations: executed through: main` | `int main()` | `int main(int argc, char** argv)` |

#### Rust (--rs) [PLANNED]

| OmniCode | Rust | Example |
|----------|------|---------|
| `#!omni code --rs` | Doc comment | `//! Code generated from OmniCode.` |
| `this is a [code] for` | Module doc | `//! Module description` |
| `inputs: requires` | `use` statements | `use std::io;` |
| `definitions: assemblies` | `struct` | `pub struct Config {...}` |
| `implementation: helpers` | Private functions | `fn helper() {...}` |
| `interface: exposes` | `pub` functions | `pub fn calculate() {...}` |
| `operations: executed through: main` | `fn main()` | `fn main() {...}` |

---

## Lookup Process

### Step 1: Identify Source Element

What OmniCode element are you looking up?

```text
"grounded in: scripture"
"exists to: purpose"
"inputs: requires: from stdlib (1)"
```

### Step 2: Identify Target Format

What format are you mapping to?

```text
adoc, md, go, c, rs, etc.
```

### Step 3: Look Up Mapping

Check the tables above or use the lookup script:

```bash
# Use Go lookup tool
go run scripts/lookup.go --element "grounded in" --format go

# Or search the syntax spec
grep -A 30 'format "go"' bereshit/word/omni/seed/code.omni
```

### Step 4: Verify in Template

Check the format template for implementation:

```bash
# Go template
cat bereshit/word/seed/code/go/executable.go

# AsciiDoc template
cat bereshit/word/seed/documentation/asciidoc-5block.adoc
```

---

## Scripts

Go lookup helpers in `scripts/`:

| Script | Purpose | Usage |
|--------|---------|-------|
| `lookup.go` | Element-to-format lookup | `go run scripts/lookup.go --element "grounded in" --format go` |
| `list-formats.go` | List available formats | `go run scripts/list-formats.go` |

### Building Scripts

```bash
cd claude-global/skills/format-lookup/scripts
go build -o ../bin/lookup ./lookup.go
```

---

## Quick Reference Card

### Pragma Formats

| Type | Format Flag | Output |
|------|-------------|--------|
| documentation | `--adoc` | AsciiDoc |
| documentation | `--md` | Markdown |
| code | `--go` | Go |
| code | `--c` | C |
| code | `--rs` | Rust |
| data | `--json` | JSON |
| data | `--yaml` | YAML |
| data | `--toml` | TOML |

### Common Mappings Summary

| OmniCode Concept | AsciiDoc | Go | C |
|------------------|----------|-----|---|
| Title/Name | `:title:` | Package doc | File header |
| Biblical foundation | `:biblical_foundation:` | `// Biblical:` | `/* Biblical: */` |
| Purpose | `:purpose:` | First doc sentence | `@brief` |
| Imports/Includes | N/A | `import` | `#include` |
| Constants | N/A | `const` | `#define` |
| Types | N/A | `type struct` | `typedef struct` |
| Private functions | N/A | `func lower()` | `static void` |
| Public functions | N/A | `func Upper()` | `void func()` |
| Entry point | N/A | `func main()` | `int main()` |

---

## Source Files

```text
FORMAT DEFINITIONS location:
bereshit/word/omni/seed/code.omni
    → See "FORMAT DEFINITIONS" section

Format-specific templates:
bereshit/word/seed/documentation/   → AsciiDoc, Markdown bases
bereshit/word/seed/code/go/         → Go templates
bereshit/word/seed/code/c/          → C templates [PLANNED]
bereshit/word/seed/code/rust/       → Rust templates [PLANNED]
```

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related Documents

| Document | Relationship |
|----------|--------------|
| bereshit/word/omni/syntax.omni | OmniCode syntax specification |
| bereshit/word/omni/seed/ | Template sources |
| validate-omni skill | Validate structure before format lookup |

---

> *"And the Word was made flesh, and dwelt among us."* — John 1:14

**Every format has its purpose.** OmniCode doesn't replace formats—it bridges them. This skill helps navigate that bridge, knowing how truth expressed in one form translates to another.
