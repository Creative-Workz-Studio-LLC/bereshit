# Format Mappings Reference

> *"To every thing there is a season, and a time to every purpose under the heaven."* — Ecclesiastes 3:1

**Why format mappings exist and how they serve OmniCode translation.**

---

## Purpose

OmniCode is format-agnostic. It expresses concepts in a universal way that can be
translated ("stepped down") to any target format. Format mappings define HOW each
OmniCode element translates to a specific format's syntax.

This reference explains the mappings in `config/format-mappings.toml`.

---

## Source Templates

The mappings derive from actual templates in `bereshit/word/seed/`:

| Format | Template Source | Structure |
|--------|-----------------|-----------|
| Go | `seed/code/go/executable.go` | 4-block code |
| AsciiDoc | `seed/documentation/adoc/asciidoc-base.adoc` | 5-block docs |
| Markdown | `seed/documentation/md/` | 5-block docs |
| C | `seed/code/c/` | 4-block code |
| Rust | `seed/code/rs/` (Planned) | 4-block code |

---

## Block Structures

### 4-Block Code (Go, C, Rust, Shell)

```
METADATA → SETUP → BODY → CLOSING
```

**METADATA** (Identity tiers from Core Identity → Interface → Operational):
| Section | Purpose |
|---------|---------|
| Biblical Foundation | Scripture grounding |
| CPI-SI Identity | Key, Purpose |
| Authorship & Lineage | Authors, Version, Derives from |
| Purpose & Function | Package/module documentation |

**SETUP** (Declares dependencies and definitions):
| Section | Order |
|---------|-------|
| Imports | stdlib (1) → internal (0) → external (-1) |
| Constants | `const` declarations |
| Variables | `var` declarations |
| Types | `struct`, `interface` definitions |
| Type Methods | Methods with receivers |
| Package-Level State | `init()`, singletons |

**BODY** (Implementation flows from private to public):
| Section | Purpose |
|---------|---------|
| Org Chart | Comment describing function organization |
| Helpers | Private utility functions |
| Core Ops | Primary business logic |
| Error Handling | Error types and handling |
| Public APIs | Exported functions - the interface |

**CLOSING** (Runtime and cleanup operations):
| Section | Purpose |
|---------|---------|
| Validation | Pre-execution checks |
| Execution | `main()` entry point |
| Cleanup | `defer` statements, resource cleanup |
| Documentation | End of file comments |

### 5-Block Documentation (AsciiDoc, Markdown)

```
METADATA → HEADER → CONTEXT → CONTENT → FOOTER
```

**METADATA** (Machine-readable document identity):
- key, title, type, status, version, created, updated
- authors, purpose, derives_from, biblical_foundation
- strictness (T/G/F), tags

**HEADER** (Human-readable identity for quick orientation):
- title (level 0/1 heading), tagline
- navigation table, status badges

**CONTEXT** (What reader needs to know before content):
- scope (In Scope / Out of Scope table)
- prerequisites (Requirement / Reference table)
- key_terms (Term / Definition table)

**CONTENT** (Main body - flexible based on document purpose):
- section headings, subsection headings
- admonitions (NOTE, TIP, WARNING, IMPORTANT)
- code blocks

**FOOTER** (References and grounding):
- related_documents table
- biblical_closing (Scripture quote)
- closing_note (summary paragraph)

### 3-Block Data (TOML, YAML, JSON)

```
METADATA → CONTENT → CLOSING
```

- **METADATA**: key, purpose, version
- **CONTENT**: Schema and values (structure varies)
- **CLOSING**: validation, policy (optional)

---

## METADATA Section Mappings

The METADATA block has required sections that map differently per format:

| OmniCode Element | Go | AsciiDoc | Markdown |
|------------------|-----|----------|----------|
| `grounded in: scripture` | `// Biblical:` comment | `:biblical_foundation:` | YAML `biblical_foundation:` |
| `serves as` | Package doc | `:title:` | YAML `title:` |
| `authored by` | `// Authors:` comment | `:authors:` | YAML `authors:` |
| `exists to: purpose` | Package doc first line | `:purpose:` | YAML `purpose:` |
| `keyed as` | `// Key:` comment | `:key:` | YAML `key:` |
| `version` | `// Version:` comment | `:version:` | YAML `version:` |

---

## Dependency Classification (Go)

Go templates distinguish dependency types in imports:

| OmniCode | Go Import Group | Rationale |
|----------|-----------------|-----------|
| `from stdlib (1)` | Standard library | Foundation - if Go works, this works |
| `from internal (0)` | Internal packages | Shared project functionality |
| `from external (-1)` | Third-party | Use sparingly - each adds risk |

---

## Code Structure Mappings (Go)

| OmniCode | Go Equivalent |
|----------|---------------|
| `definitions: constants` | `const` block |
| `definitions: variables` | `var` block |
| `definitions: assemblies` | `type struct` |
| `definitions: methods` | Method receivers |
| `implementation: helpers` | Unexported functions (lowercase) |
| `implementation: operations` | Core functions |
| `interface: exposes: exported` | Exported functions (Capitalized) |
| `interface: exposes: internal` | Unexported functions (lowercase) |
| `operations: executed through: main` | `func main()` |
| `operations: cleaned by` | `defer` statements |

---

## Documentation Mappings (AsciiDoc)

| OmniCode | AsciiDoc Equivalent |
|----------|---------------------|
| `this is a [document] for` | `:title:` attribute |
| `following` | `:type:` attribute |
| `classifies as` | `:tags:` attribute |
| `derives from` | `:derives_from:` attribute |
| `strictness is` | `:strictness:` attribute (T/G/F) |
| `presented as: title` | `= Title` heading |
| `structure of this document` | `== Section` headings |
| `clarifications` | Admonition blocks (NOTE, TIP, etc.) |
| `closing note` | Final paragraph in FOOTER |

---

## Strictness Levels

Templates define strictness for how closely files should follow the template:

| Level | Code | Meaning |
|-------|------|---------|
| Tight | T | Follow exactly - structure is required |
| Guided | G | Adapt thoughtfully - structure is recommended |
| Free | F | Principles only - structure is flexible |

---

## Adding New Formats

To add a new format mapping:

1. **Create template** in `bereshit/word/seed/[category]/[format]/`
2. **Follow block structure** (4-block for code, 5-block for docs, 3-block for data)
3. **Add mappings** to `config/format-mappings.toml`
4. **Document** the WHY in this reference

---

## Related

- **Templates**: `bereshit/word/seed/` - Source templates
- **Validation**: `validate-omni` skill - Validates structure
- **Propagation**: `propagate-change` skill - Traces template chains

---

> *"And the Word was made flesh, and dwelt among us."* — John 1:14

**OmniCode doesn't replace formats—it bridges them.** Each format has its purpose,
its season. The mappings honor both the universal truth (OmniCode) and the
specific expression (target format).
