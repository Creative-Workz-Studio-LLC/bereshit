# Documentation Guide

> *"It is the glory of God to conceal a thing: but the honour of kings is to search out a matter."* — Proverbs 25:2

## The Quality Standard

**PhD Grade by Bible Accessible.**

| Dimension | What It Means |
|-----------|---------------|
| **PhD Rigor** | Complete structure, clear citations, defensible, layered, machine-parseable |
| **Bible Accessibility** | 5-second scan, clear navigation, footnotes for terms, collapsible depth, timeless |

**The insight:** The Bible is the most read book in history — it's rigorous AND accessible. Dissertations are rigorous but often inaccessible. We want both.

---

## Decision Tree

Walk through to find the right document type:

```
What is the PRIMARY PURPOSE?
│
├─ Navigate/organize content
│  └─ → INDEX
│
├─ Welcome/onboard readers
│  └─ → COMMUNITY
│
├─ Present research findings
│  └─ → RESEARCH
│
└─ Document/explain something
   │
   └─ What DEPTH of analysis?
      │
      ├─ Deep investigation
      │  └─ → RESEARCH
      │
      └─ Standard documentation
         └─ → BASE
```

---

## Document Types

### Base Document

**When:** Standard documentation, guides, specifications, reports

**Template:** `bereshit/word/seed/documentation/adoc/asciidoc-base.adoc`

**Strictness:** Guided (G) — adapt thoughtfully

### Research Document

**When:** Original research, deep analysis, investigations

**Template:** `bereshit/word/seed/documentation/adoc/asciidoc-research.adoc`

**Strictness:** Tight (T) — follow structure exactly

### Index Document

**When:** Directory navigation (root.adoc), collection overviews

**Template:** `bereshit/word/seed/documentation/adoc/asciidoc-index.adoc`

**Strictness:** Tight (T) — consistent indexing structure

### Community Document

**When:** README, CONTRIBUTING, public-facing docs

**Template:** `bereshit/word/seed/documentation/adoc/asciidoc-community.adoc`

**Strictness:** Guided (G) — adapt for audience

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

### Block Contents

| Block | Purpose | Contains |
|-------|---------|----------|
| **METADATA** | Catalog card | Key, title, type, status, version, dates, authors, derivation, biblical foundation |
| **HEADER** | The cover | Title heading, tagline, badges, quick nav |
| **CONTEXT** | Introduction | Overview, important concept, scope, quick start, what this provides |
| **CONTENT** | Chapters | Major sections, tables, collapsibles, examples, quick reference |
| **FOOTER** | Appendix | Biblical foundation, references, related docs, see also, closing |

---

## Layered Reading

Write for multiple reading depths:

| Layer | Time | Focus | What They Get |
|-------|------|-------|---------------|
| **Scan** | 5 sec | Title, badges, tagline | Is this what I need? |
| **Skim** | 1 min | Overview, tables, headings | What's the shape? |
| **Read** | Full | All content | Understand and apply |
| **Study** | Deep | Collapsibles, footnotes | Master it |
| **Reference** | Quick | Quick ref, anchors | Find what I need |

---

## AsciiDoc Toolkit

### Use Throughout (Not Just Demonstrate)

| Element | Syntax | When to Use |
|---------|--------|-------------|
| **Footnotes** | `termfootnote:[definition]` | First mention of technical terms |
| **Highlight** | `#marked text#` | True emphasis (rare!) |
| **Collapsible open** | `[%collapsible%open]` | Major content most need |
| **Collapsible closed** | `[%collapsible]` | Optional deep-dives |
| **Cross-refs** | `<<anchor,text>>` | Internal navigation |
| **Tables** | `[cols='...'] |===` | Comparison, decisions, lookup |

### Admonitions

| Type | When to Use |
|------|-------------|
| **NOTE** | Additional context, scope clarification |
| **TIP** | Recommendations, best practices |
| **IMPORTANT** | Critical concepts, must-know |
| **WARNING** | Potential issues, cautions |
| **CAUTION** | Serious warnings |

### Symbols for Scanning

| Symbol | Meaning |
|--------|---------|
| ✓ | Yes, has, included |
| ✗ | No, missing, excluded |
| ○ | Optional, partial |
| → | Flow, leads to |
| • | Separator, bullet |

---

## The Creation Process

### 1. Determine Type

Walk the decision tree. Know your document type before starting.

### 2. Copy Template

```bash
cp bereshit/word/seed/documentation/adoc/asciidoc-[type].adoc path/to/new-doc.adoc
```

### 3. Fill METADATA First

- Unique key (bereshit keying format)
- Clear title and type
- Biblical foundation that genuinely grounds this work

### 4. Write HEADER

- Tagline: What does reader get in one line?
- Badges: Current status
- Quick nav: Where will readers want to go?

### 5. Build CONTEXT

- Overview: 2-4 sentences, what and why
- Important: The one thing they MUST understand
- Scope: What's in, what's out

### 6. Develop CONTENT

- Use toolkit elements THROUGHOUT
- Tables for comparisons and decisions
- Collapsibles for depth without clutter
- Real examples with actual file links

### 7. Complete FOOTER

- Biblical foundation: How does Scripture inform this?
- References: Connect to ecosystem
- See also: Where next?

### 8. Verify

- All blocks present?
- Layered reading works?
- Cross-references valid?
- Footnotes on first technical terms?

---

## Common Patterns

### For Guides/Tutorials

- Quick Start in CONTEXT
- Step-by-step in CONTENT
- Troubleshooting collapsible
- Examples with real code

### For Specifications/Standards

- Strict structure
- Requirement tables
- Compliance checklists
- Reference-heavy FOOTER

### For Research

- Methodology section
- Findings with evidence
- Analysis tables
- Extensive citations

### For Indices

- Minimal CONTEXT
- Navigation-focused CONTENT
- Links to all children
- Consistent format across directory

---

## Related

| Resource | Relationship |
|----------|--------------|
| [bereshit/word/seed/documentation/](bereshit/word/seed/documentation/) | Templates live here |
| [OmniCode syntax](bereshit/word/omni/) | 5-block derives from this |
| [create-from-template](../create-from-template/) | For code templates |

---

> *"Study to shew thyself approved unto God, a workman that needeth not to be ashamed."* — 2 Timothy 2:15
