---
description: Create a new 5-block documentation file following CPI-SI standards.
---

# Create Documentation

Create a new documentation file following 5-block structure.

## Task

The user wants to create documentation that follows the 5-block pattern.

**Arguments:** $ARGUMENTS
- Format: `[name]` or `[path/name]`
- Example: `api-reference` or `docs/guides/getting-started`

## Instructions

1. If no name provided, ask the user:
   - What is the document about?
   - Where should it be created?
   - What format? (md, adoc)

2. Determine appropriate location:
   - `method/` - Building block method docs
   - `standards/` - Standards documentation
   - `research/` - Research findings
   - `wisdom/` - CPI-SI insights
   - `claude-global/docs/` - System documentation

3. Create 5-block structure:

```markdown
<!--
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: [KEY]
:title: [Title]
:type: Documentation
:status: Draft
:version: 0.1.0
:created: [date]
:authors: [author]
:purpose: [purpose]
:biblical_foundation: [verse]
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# [Title]

> *"[Biblical quote]"* — [Reference]

**[Brief description]**

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Prerequisites

[What reader needs to know first]

## Key Terms

| Term | Definition |
|------|------------|
| [term] | [definition] |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## [Main Content]

[Body of the document]

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## References

- [Reference 1]
- [Reference 2]

## Related Documents

- [Related doc 1]
```

4. Guide user through filling each block

## Example Usage

```bash
/create/doc getting-started
/create/doc standards/new-standard
/create/doc   (will ask for details)
```

## Block Reference

| Block | Purpose | Required Sections |
|-------|---------|-------------------|
| METADATA | Machine-readable identity | key, title, type, status, purpose |
| HEADER | Human-readable identity | Title, quote, description |
| CONTEXT | Prerequisites | Prerequisites, Key Terms |
| CONTENT | Main body | [varies by document] |
| FOOTER | References | References, Related Documents |

## Documentation Templates

| Format | Template Location |
|--------|-------------------|
| AsciiDoc | `bereshit/word/seed/documentation/adoc/` |
| Markdown | `bereshit/word/seed/documentation/md/` |
| Typst | `bereshit/word/seed/documentation/typ/` |
| OmniCode | `bereshit/word/omni/seed/documentation.omni` |

### Quick Template Discovery

```bash
# List available documentation templates
ls -la bereshit/word/seed/documentation/

# Find specific format template
ls -la bereshit/word/seed/documentation/adoc/
```
