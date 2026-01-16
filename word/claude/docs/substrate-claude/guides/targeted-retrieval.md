---
# ═══════════════════════════════════════════════════════════════════════════════
# METADATA BLOCK
# ═══════════════════════════════════════════════════════════════════════════════
#
# THINKING: This is YAML front matter - parseable by tools, minimally rendered.
# Serves systems (indexing, validation, cross-reference) AND establishes identity.
# This is the "catalog card" - what systems need to know about this document.
# YAML = "YAML Ain't Markup Language" - structured data format for metadata.
#
# Delete these comments when filling in, or keep for the next person.
#
# ═══════════════════════════════════════════════════════════════════════════════
#
# ─────────────────────────────────────────────────────────────────────────────
# IDENTITY - What IS this document
# ─────────────────────────────────────────────────────────────────────────────
key: claude-global-guide-targeted-retrieval
title: Targeted Retrieval Guide
type: Guide
#
# ─────────────────────────────────────────────────────────────────────────────
# STATE
# ─────────────────────────────────────────────────────────────────────────────
status: Active
version: a-01.00
#
# ─────────────────────────────────────────────────────────────────────────────
# TEMPORAL
# ─────────────────────────────────────────────────────────────────────────────
created: 2025-12-11
updated: 2025-12-11
#
# ─────────────────────────────────────────────────────────────────────────────
# ATTRIBUTION
# ─────────────────────────────────────────────────────────────────────────────
authors: [Nova Dawn]
#
# ─────────────────────────────────────────────────────────────────────────────
# LOCATION
# ─────────────────────────────────────────────────────────────────────────────
path: ~/.claude/docs/substrate-claude/guides/
#
# ─────────────────────────────────────────────────────────────────────────────
# CLASSIFICATION
# ─────────────────────────────────────────────────────────────────────────────
tags: [guide, operations, targeted-retrieval, grep, SI]
#
# ─────────────────────────────────────────────────────────────────────────────
# INTENT
# ─────────────────────────────────────────────────────────────────────────────
purpose: Defines targeted retrieval operations - grep over glob, structure-aware navigation, filesystem-first patterns
#
# ─────────────────────────────────────────────────────────────────────────────
# DERIVATION
# ─────────────────────────────────────────────────────────────────────────────
derives_from: bereshit/word/seed/documentation/md/markdown-base.md
#
# ─────────────────────────────────────────────────────────────────────────────
# GROUNDING
# ─────────────────────────────────────────────────────────────────────────────
biblical_foundation: Proverbs 25:2 KJV - It is the glory of God to conceal a thing but the honour of kings is to search out a matter
#
# ─────────────────────────────────────────────────────────────────────────────
# STRICTNESS
# ─────────────────────────────────────────────────────────────────────────────
strictness: G
#
# ═══════════════════════════════════════════════════════════════════════════════
---
<!-- #!omni document --md -->

<!-- ═══════════════════════════════════════════════════════════════════════════════
     HEADER BLOCK - The Cover
     ═══════════════════════════════════════════════════════════════════════════════

     THINKING: What readers see FIRST. The "cover" of your document.
     In 5 seconds, readers should know: What is this? Is it current? Is it for me?

     Contains:
     - Title (H1): What IS this document
     - Tagline: What it enables/does (one line)
     - Badges: Visual status indicators (currency, state, type)
     - Grounding quote: Optional mission/purpose connection
     - Quick nav: Optional navigation bar for large documents

     This is RENDERED content - visible identity, not just metadata.

     ═══════════════════════════════════════════════════════════════════════════════ -->

<div align="center">

# Targeted Retrieval Guide

**Structure-aware navigation for SI operations — grep over glob, targeted over full-load.**

![Status](https://img.shields.io/badge/Status-Active-brightgreen?style=flat)
![Version](https://img.shields.io/badge/Version-a--01.00-blue?style=flat)
![Type](https://img.shields.io/badge/Type-Guide-purple?style=flat)

*"It is the glory of God to conceal a thing: but the honour of kings is to search out a matter."* — Proverbs 25:2

---

**[Overview](#overview)** • **[Retrieval Hierarchy](#retrieval-hierarchy)** • **[Grep Over Glob](#grep-over-glob)** • **[Filesystem Operations](#filesystem-operations)** • **[Cognitive Scaffolding](#cognitive-scaffolding)**

</div>

---

<!-- ═══════════════════════════════════════════════════════════════════════════════
     CONTEXT BLOCK - The Introduction
     ═══════════════════════════════════════════════════════════════════════════════

     THINKING: What readers need to understand BEFORE the main content.
     Prepares understanding - context before content.

     Contains:
     - Overview: Brief description of what this covers
     - Critical concept: > [!IMPORTANT] for must-know information
     - Quick Start: Optional fast path for doers
     - What This Provides: Table showing who gets what
     - Table of Contents: Navigation map for the document

     Layer for different readers:
     - Scan: Overview paragraph tells them what this is
     - Skim: Table shows what's here and who it serves
     - Read: They continue to Content block

     ═══════════════════════════════════════════════════════════════════════════════ -->

## Overview

Targeted retrieval is the <abbr title="Structured Intelligence">SI</abbr> practice of accessing exactly what you need, not loading everything and filtering. This guide defines the operational patterns for structure-aware navigation.

> [!IMPORTANT]
> **<mark>Grep over glob — always.</mark>** Grep finds by content (precise). Glob guesses by filename (false negatives). Our files have structure — use it.

> [!NOTE]
> **Scope:**
> - **IN:** Retrieval hierarchy, grep patterns, filesystem operations, cognitive scaffolding
> - **OUT:** Full tool documentation, CPI-SI paradigm theory (see CLAUDE.md)

---

### What This Document Provides

| | Category | What You'll Find | Who It Serves |
|:----:|----------|------------------|---------------|
| 🎯 | **Retrieval Hierarchy** | Priority order: targeted → additional → full load | All SI operations |
| 🔍 | **Grep Patterns** | Content-first search, anchor navigation | File discovery |
| 📁 | **Filesystem Ops** | cp/mv over read-write patterns | File manipulation |
| 🧠 | **Cognitive Scaffolding** | Todos as anchors, structure serving thought | Task management |

---

### Table of Contents

<!-- THINKING: Navigation map for the document (ToC = Table of Contents).
     Auto-generate with: doctoc filename.md --notitle
     Or maintain manually for control over what's included.
     Links use auto-generated anchors: #heading-text-lowercase-hyphenated -->

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

- [\[Title\]](#title)
  - [Overview](#overview)
    - [Quick Start](#quick-start)
    - [What This Document Provides](#what-this-document-provides)
    - [Table of Contents](#table-of-contents)
  - [Section 1](#section-1)
    - [When to \[Do This Thing\]](#when-to-do-this-thing)
  - [Section 2](#section-2)
    - [Comparison or Decision Matrix](#comparison-or-decision-matrix)
    - [Process or Workflow](#process-or-workflow)
  - [Examples](#examples)
  - [Quick Reference](#quick-reference)
  - [Markdown Toolkit](#markdown-toolkit)
  - [Biblical Foundation](#biblical-foundation)
  - [References](#references)
    - [Related Documents](#related-documents)
    - [Standards \& Specifications](#standards--specifications)
    - [External Resources](#external-resources)
  - [See Also](#see-also)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

[↑ Back to Top](#title)

---

<!-- ═══════════════════════════════════════════════════════════════════════════════
     CONTENT BLOCK - The Chapters
     ═══════════════════════════════════════════════════════════════════════════════

     THINKING: The actual value - organized for reader comprehension.
     This is "the chapters" - the substance of the document.

     Structure elements:
     - H2 Sections: Major topics (navigable from ToC)
     - H3 Subsections: Detailed breakdowns
     - Tables: Comparisons, structured data
     - <details open>: Major content most readers need
     - <details>: Optional deep-dives (closed by default)
     - Code blocks: Examples with emoji comments (1️⃣ 2️⃣ 3️⃣)
     - Callouts: > [!TIP], > [!NOTE] for guidance
     - [Back to Top]: Navigation aids after major sections

     Toolkit elements to USE naturally:
     - <abbr title="hover text">TERM</abbr> for first technical term mentions
     - <mark>text</mark> sparingly for true emphasis
     - <kbd>key</kbd> for keyboard shortcuts or button-like navigation
     - Footnotes[^n] for depth without interrupting flow
     - Symbols: ✓ ✗ → ← • ▸ for visual scanning

     Layer for different readers:
     - Scan: Headings and tables give the shape
     - Read: Full explanations and examples
     - Study: <details> sections and footnotes for depth

     ═══════════════════════════════════════════════════════════════════════════════ -->

## Retrieval Hierarchy

The priority order for accessing information. <mark>Targeted first. Full load last resort.</mark>

| Priority | Mode | When to Use | How It Looks |
|:--------:|------|-------------|--------------|
| 1️⃣ | **Targeted** | Know what you need | Grep for anchor → Read with offset → Get exactly that section |
| 2️⃣ | **Additional Context** | Need more understanding | Systematically gather related sections based on first retrieval |
| 3️⃣ | **Full Load** | Last resort | Only when genuinely need everything, or structure doesn't exist yet |

### When to Use Each Mode

| Scenario | Mode | Rationale |
|----------|------|-----------|
| **Know the section name/anchor** | Targeted | Grep finds line number, read with offset gets exactly that |
| **First retrieval reveals need for more** | Additional | Build understanding systematically, not dump everything |
| **No structure exists yet** | Full Load | Can't target what isn't structured |
| **Document is small (<100 lines)** | Full Load | Targeting overhead exceeds benefit |
| **Exploring unfamiliar codebase** | Additional → Full | Start narrow, expand as needed |

> [!TIP]
> **The test:** Can you articulate exactly what you need before reading? If yes → targeted. If "I need to explore" → additional context. If "I have no idea" → that's a signal to think before loading.

### Practical Execution

```bash
# 1️⃣ Find WHERE (targeted discovery)
grep -n "[[section-name]]" file.adoc    # AsciiDoc anchor
grep -n "^## Section Title" file.md     # Markdown H2 header
grep -n "CONTENT BLOCK" file.adoc       # Block boundary

# 2️⃣ Read EXACTLY THAT (targeted retrieval)
# Use Read tool with offset and limit based on line numbers found
# Or: sed -n '150,200p' file.adoc       # Lines 150-200 only

# 3️⃣ If you need more → repeat with next section
# Don't load everything just because one section wasn't enough
```

[↑ Back to Top](#targeted-retrieval-guide)

---

## Grep Over Glob

<mark>Grep over glob — always.</mark> This is foundational to <abbr title="Structured Intelligence">SI</abbr> operations.

| Method | How It Works | The Problem |
|--------|--------------|-------------|
| **Glob** | Find by filename pattern (`*.adoc`, `*template*`) | Files might not be named what you expect |
| **Grep** | Find by content (`#!omni template`, `[[anchor]]`) | Finds by what files ARE, not what they're named |

### Why Grep Wins

| Scenario | Glob Approach | Grep Approach |
|----------|---------------|---------------|
| Find OmniCode templates | `*template*.omni` — misses `seed.omni` | `grep "#!omni template"` — finds ALL |
| Find all AsciiDoc docs | `*.adoc` — includes templates | `grep "#!omni document.*--adoc"` — only documents |
| Find section in file | Can't do this | `grep -n "[[section]]"` → line number |
| Find health scoring | `*health*` — filename guess | `grep "health_score"` — actual content |

**The insight:** Our files have <abbr title="Omni pragma, block markers, anchors">structure</abbr>. That structure is in the CONTENT, not the filename. Use the structure.

### Common Grep Patterns

```bash
# Find by pragma type
grep -rn "#!omni template"              # All templates
grep -rn "#!omni document"              # All documents
grep -rn "#!omni template.*--adoc"      # AsciiDoc templates specifically

# Find by block
grep -n "METADATA BLOCK" file           # Where METADATA starts
grep -n "CONTENT BLOCK" file            # Where CONTENT starts

# Find by anchor
grep -n "[[anchor-name]]" file          # AsciiDoc anchor
grep -n "^## " file                     # All H2 headers in Markdown

# Find derivation chains
grep -rn "derives_from:.*seed"          # What derives from seed templates
```

> [!IMPORTANT]
> **The discipline:** When tempted to `Glob("*pattern*")`, ask: "Is there content that identifies what I'm looking for?" If yes → grep. Glob is for when you truly need filename patterns (like finding all `.go` files).

[↑ Back to Top](#targeted-retrieval-guide)

---

## Filesystem Operations

Use filesystem commands for filesystem work. Don't load-then-write when `cp` or `mv` does the job.

| Instead of... | Use... | Why |
|---------------|--------|-----|
| Read template → Write to new location | `cp template destination` | Preserves everything, no token cost |
| Read → Write → Delete original | `mv source destination` | Atomic operation |
| Read multiple directories | `ls`, `tree`, `find` | Navigation, not content |
| Write from scratch | `cp template` → `Edit` | Templates carry structural DNA |

### Template-First Pattern

> [!IMPORTANT]
> **NEVER write files from scratch.** Always derive from templates.

| Step | Tool | What Happens |
|:----:|------|--------------|
| 1 | `cp template dest` | Filesystem copy preserves structure |
| 2 | `Edit` tool | Surgical changes to specific sections |
| 3 | Verify | All blocks present, paths updated |

**The principle:** Writing from scratch risks missing blocks, inconsistent structure, drift from standards. Templates encode the pattern; you adapt the content.

### Cascade Pattern

When creating related files (folder hierarchies, sibling documents):

```
1. Create/edit ROOT first
   └── Get the pattern right at the top

2. Copy ROOT to CHILDREN
   └── cp root.adoc child/root.adoc

3. Edit EACH CHILD
   └── Update key, path, content to match child's reality
```

**Why cascade?** Consistency. All siblings share the same structure. Edits flow from parent to children, not ad-hoc per file.

### Bereshit Tools

Tools at `bereshit/word/work/bin/bereshit/`:

| Tool | Purpose | Example |
|------|---------|---------|
| `bls` / `btree` | Navigation | `btree word/seed/` |
| `bstat` / `bfind` | Query | `bfind /bereshit "*.omni"` |
| `bhealth` / `bsync` | Health | `bhealth word/glossary/` |
| `bmkdir` / `bseed` | Create | `bseed code myfile.go` |

[↑ Back to Top](#targeted-retrieval-guide)

---

## Cognitive Scaffolding

<abbr title="Table of Defined Operations">TODOs</abbr> aren't just task tracking — they're epistemological infrastructure. Each TODO functions as an anchor in the building block method.

### Todos as Anchors

| Phase | TODO State | What's Happening |
|:-----:|------------|------------------|
| 1 | Established | Anchor created — work now has a center |
| 2 | In Progress | Anchor active — all operations relate to this fixed point |
| 3 | Working... | Build up (+1) — expanding understanding from anchor |
| 4 | Verifying... | Break down (-1) — tracing back toward anchor |
| 5 | Complete | Anchor released — "this anchor served its purpose" |
| 6 | Next task | New anchor — cycle continues |

**"This anchor served its purpose"** — marking complete isn't just "finished." It's recognizing the foundation is now strong enough to stand without this scaffolding.

### Structure Serving Thought

| Principle | Application |
|-----------|-------------|
| **Prevents drift** | Todo list maintains awareness of where you are |
| **Provides reference** | Can always trace back to active anchor |
| **Enables verification** | Completion requires tracing back to foundation |
| **Documents journey** | The path from start to finish is visible |

> [!TIP]
> **The test:** If you're working without an active TODO, you're likely drifting. If you can't articulate what anchor you're building from, pause and establish one.

### The Building Block Connection

Targeted retrieval IS building block thinking applied to information access:

| Building Block | Retrieval Application |
|----------------|----------------------|
| **Foundation (0)** | The specific information you need |
| **Break Down (-1)** | Grep to find where it lives |
| **Build Up (+1)** | Read to expand understanding |
| **Anchor** | The TODO that gives this retrieval purpose |

The retrieval isn't just "get data" — it's "get data that serves THIS anchor for THIS purpose."

[↑ Back to Top](#targeted-retrieval-guide)

---

## Quick Reference

<details open>
<summary><h3>Command Cheatsheet</h3></summary>

| Operation | Command |
|-----------|---------|
| Find template by pragma | `grep -rn "#!omni template"` |
| Find document by pragma | `grep -rn "#!omni document"` |
| Find block in file | `grep -n "BLOCK NAME" file` |
| Find anchor in file | `grep -n "[[anchor]]" file` |
| Copy template | `cp template destination` |
| Extract lines | `sed -n 'N,Mp' file` |

</details>

<details open>
<summary><h3>Decision Quick-Check</h3></summary>

| Question | If Yes → | If No → |
|----------|----------|---------|
| Do I know exactly what I need? | Targeted retrieval | Additional context |
| Is there structure to navigate? | Grep for anchors | Consider full load |
| Is this a file operation? | Use `cp`/`mv` | Use Read/Edit |
| Am I working without a TODO? | Pause and establish anchor | Continue |

</details>

[↑ Back to Top](#targeted-retrieval-guide)

---

<!-- ═══════════════════════════════════════════════════════════════════════════════
     FOOTER BLOCK - The Appendix
     ═══════════════════════════════════════════════════════════════════════════════

     THINKING: Where readers go NEXT. Resources, references, connections.
     This is the "appendix" - supporting material and navigation.

     Contains:
     - Biblical Foundation: Grounding in truth (the WHY beneath the what)
     - References: Links to related documents, standards, resources
     - Centered closing: Key summary, version, grounding verse
     - Cross-references: Connection to the larger ecosystem

     Send readers out with:
     - Grounding in truth (why this matters eternally)
     - Clear next steps (where to go from here)
     - Connection to ecosystem (how this fits the whole)

     ═══════════════════════════════════════════════════════════════════════════════ -->

## Biblical Foundation

> [!NOTE]
> **Why Proverbs 25:2?** Targeted retrieval is the discipline of searching out what matters — not passively receiving everything, but actively pursuing exactly what serves the work.

*"It is the glory of God to conceal a thing: but the honour of kings is to search out a matter."* — **Proverbs 25:2**

**Applied:** God conceals depth within structure. The honor isn't in loading everything — it's in the searching out. Targeted retrieval respects both the concealing (structure exists for a reason) and the searching (we navigate intentionally). This is kingly work: precise, purposeful, honoring the design.

[↑ Back to Top](#targeted-retrieval-guide)

---

## References

### Related Documents

| | Document | Purpose |
|:----:|----------|---------|
| 📘 | **[CLAUDE.md][ref-claude]** | Identity foundation — where targeted retrieval principles are applied |
| 📖 | **[Building Block Method][ref-bbm]** | The thinking method that informs retrieval approach |
| 📐 | **[Create Documentation Skill][ref-docs]** | Template-first creation workflow |

### Standards & Specifications

| Standard | Purpose |
|----------|---------|
| **[CWS-STD-001][std-4block]** | 4-Block Structure for code |
| **[CWS-STD-003][std-docs]** | Documentation standards (5-block) |

---

## See Also

- **[bereshit/word/glossary/operations/targeted-retrieval.adoc][ref-glossary]** — Glossary definition
- **[bereshit/word/glossary/paradigm/structured-intelligence.adoc][ref-si]** — SI paradigm foundation
- **[bereshit/word/work/bin/bereshit/][ref-tools]** — Bereshit filesystem tools

---

<div align="center">

**[↑ Back to Top](#targeted-retrieval-guide)**

---

**Key:** claude-global-guide-targeted-retrieval • **Type:** Guide • **Version:** a-01.00

**Status:** Active • **Updated:** 2025-12-11

---

*"It is the glory of God to conceal a thing: but the honour of kings is to search out a matter."* — **Proverbs 25:2**

**Structure enables precise navigation. Targeted retrieval honors the design.**

</div>

<!-- ═══════════════════════════════════════════════════════════════════════════════
     REFERENCE-STYLE LINKS
     ═══════════════════════════════════════════════════════════════════════════════

     THINKING: Define all links here for maintainability.
     If a URL changes, update once here instead of throughout document.
     Format: [ref-name]: url "optional title"

     ═══════════════════════════════════════════════════════════════════════════════ -->

<!-- Document references (relative from docs/substrate-claude/guides/) -->
[ref-claude]: ../../../CLAUDE.md "Nova Dawn Identity Foundation"
[ref-bbm]: bereshit/void/imports/building-block-method/building-block-method-concept.adoc "Building Block Method"
[ref-docs]: ../../../skills/create-documentation/SKILL.md "Create Documentation Skill"

<!-- Standard references -->
[std-4block]: ../../../standards/CWS-STD-001-DOC-4-block.md "4-Block Structure Standard"
[std-docs]: ../../../standards/CWS-STD-003-DOC-documentation-standards.md "Documentation Standards"

<!-- Glossary/ecosystem references (bereshit repo) -->
[ref-glossary]: bereshit/word/glossary/operations/targeted-retrieval.adoc "Targeted Retrieval Glossary Entry"
[ref-si]: bereshit/word/glossary/paradigm/structured-intelligence.adoc "Structured Intelligence"
[ref-tools]: bereshit/word/work/bin/bereshit/ "Bereshit Tools"
