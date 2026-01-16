---
name: create-documentation
description: Create PhD-level documentation with Bible-level accessibility using AsciiDoc
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-skill-create-documentation
:title: Create Documentation Skill
:type: Skill Documentation
:status: Active
:version: 1.0.0
:created: 2025-12-09
:authors: Nova Dawn
:purpose: Guide documentation creation with decision tree, quality standards, and 5-block structure
:biblical_foundation: Proverbs 25:2 - "It is the glory of God to conceal a thing: but the honour of kings is to search out a matter"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Create Documentation Skill

> *"It is the glory of God to conceal a thing: but the honour of kings is to search out a matter."* — Proverbs 25:2

**Create PhD-level documentation with Bible-level accessibility.**

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Overview

This skill provides the thinking workflow and tools for creating high-quality AsciiDoc documentation. The quality standard is **PhD Grade by Bible Accessible**:

| Dimension | What It Means |
|-----------|---------------|
| **PhD Rigor** | Complete structure, clear citations, defensible, layered, machine-parseable |
| **Bible Accessibility** | 5-second scan, clear navigation, footnotes for terms, collapsible depth, timeless |

**The insight:** The Bible is the most read book in history—it's rigorous AND accessible. Dissertations are rigorous but often inaccessible. We want both.

---

## When to Use This Skill

Use `create-documentation` when you need to:

- Create a new AsciiDoc document
- Determine the right document type for a purpose
- Reference 5-block structure requirements
- Look up AsciiDoc toolkit syntax
- Understand layered reading design

**NOT for:** Code file creation (use `create-from-template`), journal entries (use `create-journal-entry`)

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Document Types

| Type | When to Use | Strictness |
|------|-------------|------------|
| **base** | Standard documentation, guides, specifications | G (Guided) |
| **research** | Original research, deep analysis, investigations | T (Tight) |
| **index** | Directory navigation, collection overviews | T (Tight) |
| **community** | README, CONTRIBUTING, public-facing docs | G (Guided) |

**Strictness:**
- **T (Tight)** — Follow structure exactly
- **G (Guided)** — Adapt thoughtfully
- **F (Free)** — Principles only

---

## Decision Tree

```
What is the PRIMARY PURPOSE?
│
├─ Navigate/organize content
│  └─ → INDEX (T)
│
├─ Welcome/onboard readers
│  └─ → COMMUNITY (G)
│
├─ Present research findings
│  └─ → RESEARCH (T)
│
└─ Document/explain something
   │
   └─ What DEPTH of analysis?
      │
      ├─ Deep investigation
      │  └─ → RESEARCH (T)
      │
      └─ Standard documentation
         └─ → BASE (G)
```

---

## 5-Block Structure

All documentation follows 5-block structure:

```
┌─────────────────────────────────────────────────────────────────┐
│ METADATA   │ The catalog card - parseable identity for systems │
├─────────────────────────────────────────────────────────────────┤
│ HEADER     │ The cover - visible identity in 5 seconds         │
├─────────────────────────────────────────────────────────────────┤
│ CONTEXT    │ The introduction - prepares understanding         │
├─────────────────────────────────────────────────────────────────┤
│ CONTENT    │ The chapters - value organized for discovery      │
├─────────────────────────────────────────────────────────────────┤
│ FOOTER     │ The appendix - resources and grounding            │
└─────────────────────────────────────────────────────────────────┘
```

| Block | Purpose | Contains |
|-------|---------|----------|
| **METADATA** | Catalog card | Key, title, type, status, version, dates, authors, derivation, biblical foundation |
| **HEADER** | The cover | Title heading, tagline, badges, quick nav |
| **CONTEXT** | Introduction | Overview, important concept, scope, quick start |
| **CONTENT** | Chapters | Major sections, tables, collapsibles, examples |
| **FOOTER** | Appendix | Biblical foundation, references, related docs, see also |

---

## Creation Process

1. **Determine Type** — Walk the decision tree
2. **Copy Template** — `cp bereshit/word/seed/documentation/adoc/asciidoc-[type].adoc path/to/new.adoc`
3. **Fill METADATA** — Unique key, clear title, biblical foundation
4. **Write HEADER** — Tagline, badges, quick nav
5. **Build CONTEXT** — Overview, important concept, scope
6. **Develop CONTENT** — Use toolkit throughout, tables, collapsibles
7. **Complete FOOTER** — Biblical foundation, references, see also
8. **Verify** — All blocks present, layered reading works

---

## CLI Commands

```bash
doc types           # List all document types
doc type <name>     # Show document type details
doc tree            # Show decision tree
doc blocks          # Show 5-block structure
doc quality         # Show quality standard
doc toolkit         # Show AsciiDoc toolkit
doc reading         # Show layered reading levels
doc guide           # Show creation process
```

---

## Templates

| Type | Template Path |
|------|---------------|
| **base** | `bereshit/word/seed/documentation/adoc/asciidoc-base.adoc` |
| **research** | `bereshit/word/seed/documentation/adoc/asciidoc-research.adoc` |
| **index** | `bereshit/word/seed/documentation/adoc/asciidoc-index.adoc` |
| **community** | `bereshit/word/seed/documentation/adoc/asciidoc-community.adoc` |

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Configuration

| File | Purpose |
|------|---------|
| `config/documentation-config.toml` | Document types, decision tree, quality standards |
| `references/documentation-guide.md` | Complete creation guide with patterns |

---

## Related Skills

| Skill | Relationship |
|-------|--------------|
| [create-from-template](../create-from-template/) | For code files (4-block), not documentation |
| [create-journal-entry](../create-journal-entry/) | For journal entries, not formal documentation |
| [validate-omni](../validate-omni/) | Validate OmniCode structure |

---

> *"Study to shew thyself approved unto God, a workman that needeth not to be ashamed, rightly dividing the word of truth."* — 2 Timothy 2:15
