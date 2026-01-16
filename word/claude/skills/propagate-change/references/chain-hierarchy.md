# Chain Hierarchy Reference

> *"A little leaven leaveneth the whole lump."* — Galatians 5:9

**Why chain tracing exists and how propagation works.**

---

## Purpose

OmniCode uses a hierarchical template system. When you change a template at any level,
that change affects everything that derives from it. Chain tracing helps you:

1. **Find all affected files** before making changes
2. **Understand the scope** of a template modification
3. **Plan updates** in the correct order (top-down)

---

## The Chain Hierarchy

```
SYNTAX SPEC (master)           ← Changes affect EVERYTHING
    ↓
UNIVERSAL TEMPLATE             ← Changes affect all types
    ↓
SPECIALIZED TEMPLATES          ← Changes affect one type
    ├── code.omni
    ├── documentation.omni
    ├── interface.omni
    ├── folder.omni
    └── data.omni
        ↓
FORMAT TEMPLATES               ← Changes affect one format
    ├── go/executable.go
    ├── adoc/asciidoc-base.adoc
    └── etc.
        ↓
ACTUAL DOCUMENTS               ← Leaf nodes (no descendants)
```

---

## Template Locations

| Level | Location | What's There |
|-------|----------|--------------|
| Syntax Spec | (not yet created) | Master OmniCode syntax |
| Universal | `bereshit/word/omni/seed/universal.omni` | Base template |
| Specialized | `bereshit/word/omni/seed/[type].omni` | code, documentation, etc. |
| Format | `bereshit/word/seed/[type]/[format]/` | Go, AsciiDoc, Markdown, etc. |
| Documents | Everywhere | Files using templates |

---

## derives_from Markers

The chain is defined by `derives_from` references in files:

| Format | How It Looks |
|--------|--------------|
| OmniCode | `derives from: path/to/template.omni` |
| AsciiDoc | `:derives_from: path/to/template.omni` |
| Go | `// Derives from: path/to/template.omni` |
| Markdown | `derives_from: path/to/template.omni` (YAML) |

**Root markers** indicate the top of a chain:
- `original` or `ORIGINAL`
- Empty value
- `none`

---

## Propagation Direction

**Changes flow DOWN** the hierarchy:

| If You Change... | Check These Levels |
|------------------|--------------------|
| Syntax spec | Universal → Specialized → Format → Documents |
| Universal template | Specialized → Format → Documents |
| Specialized template | Format → Documents of that type |
| Format template | Documents using that format |
| Document | Nothing (leaf node) |

**Fixes may flow UP** when documents reveal template issues.

---

## Update Order

When propagating changes, update in this order:

1. **Universal template** (if affected)
2. **Specialized templates** (code, documentation, etc.)
3. **Format templates** (go, adoc, etc.)
4. **Spot-check documents** (verify alignment)

---

## Common Scenarios

### Changed Syntax Spec

**Impact:** Everything

1. Update universal template
2. Update all specialized templates
3. Update all format templates
4. Spot-check documents

### Changed Specialized Template

**Impact:** Format templates of that type, documents of that type

1. Update format templates (e.g., go, adoc for documentation)
2. Documents using those templates

### Changed Format Template

**Impact:** Documents using that format

1. Find all files with derives_from pointing to this template
2. Update each file

---

## Related

| Resource | Purpose |
|----------|---------|
| `config/chain-config.toml` | Machine-readable configuration |
| `bereshit/word/omni/seed/` | OmniCode templates |
| `bereshit/word/seed/` | Format templates |
| validate-omni skill | Validate structure |
| format-lookup skill | Check mappings |

---

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost?"* — Luke 14:28

**Count the cost of change.** Know what will be affected. Then proceed with wisdom.
