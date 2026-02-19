---
name: create-whitepaper
description: Create PhD-level whitepapers with Bible-level accessibility using AsciiDoc. Provides THINKING WORKFLOW (building block mindset - always active) and APPLICATION WORKFLOW (step-by-step for documentation creation). References loaded on demand to avoid context bloat.
version: "1.6.0"
allowed-tools:
  - Write
  - Read
  - Edit
  - Glob
  - Grep
  - Bash
---

<!--
================================================================================
METADATA BLOCK
================================================================================
This skill follows 5-block structure demonstrating what it teaches.
Reorganized in v1.6.0: Thinking Workflow + Application Workflow + References

:key: SKILL-CREATE-WHITEPAPER
:title: Create Whitepaper Skill
:type: Skill Definition
:status: Active
:version: 1.6.0
:biblical_foundation: Matthew 7:24-25 — Build on rock, not sand
:derives_from:
  - bereshit/void/imports/building-block-method/building-block-method-principles.adoc
  - Application experience: 3000+ line whitepaper creation
  - Recovery patterns: errors discovered during complex document work
:references:
  - references/verification-criteria.md — Per-block completion checklists
  - references/recovery-patterns.md — 5 recovery patterns for errors
  - references/dissertation-patterns.md — 7 PhD writing patterns
  - references/asciidoc-reference.md — AsciiDoc features and syntax
  - references/example-structure.md — Complete whitepaper template
================================================================================
-->

<!--
================================================================================
HEADER BLOCK
================================================================================
-->

# Create Whitepaper

> *"Therefore whosoever heareth these sayings of mine, and doeth them, I will liken him unto a wise man, which built his house upon a rock."* — Matthew 7:24

**PhD rigor + Bible accessibility.** Infinite traceable depth, universally navigable.

This skill provides:

- **Thinking Workflow** — Building block mindset, always active
- **Application Workflow** — Step-by-step when creating documentation
- **References** — Deep patterns loaded on demand (avoids context bloat)

---

## Quick Navigation

| Section | Purpose |
|---------|---------|
| [The Core Insight](#the-core-insight) | What we're building and why (CONTEXT) |
| [Thinking Workflow](#thinking-workflow) | Building block mindset—always active |
| [Application Workflow](#application-workflow) | Step-by-step when creating documentation |
| [References](#references) | Deep patterns, criteria, recovery (FOOTER) |
| [Biblical Foundation](#biblical-foundation) | Scripture grounding |

**📚 Detailed References** (load on demand):

| Reference | Use When |
|-----------|----------|
| [verification-criteria.md](references/verification-criteria.md) | Need per-block completion checklists |
| [recovery-patterns.md](references/recovery-patterns.md) | Something went wrong, need to recover |
| [dissertation-patterns.md](references/dissertation-patterns.md) | Writing academic prose, need patterns |
| [asciidoc-reference.md](references/asciidoc-reference.md) | Need AsciiDoc syntax/features |
| [example-structure.md](references/example-structure.md) | Need complete template to follow |

---

<!--
================================================================================
CONTEXT BLOCK
================================================================================
-->

## The Core Insight

### Quick Start

> **30-second orientation:** This skill creates whitepapers with PhD-level rigor AND universal accessibility. Use 5-block structure (METADATA → HEADER → CONTEXT → CONTENT → FOOTER), distill claims in main text, support with sidebars/footnotes/includes. Work block-by-block with explicit verification criteria. When errors occur, break down to find the gap, rebuild from there.

### Scope

| In Scope | Out of Scope |
|----------|--------------|
| AsciiDoc whitepaper creation | Other document formats (for now) |
| 5-block documentation structure | Code documentation (use 4-block) |
| PhD rigor + Bible accessibility blend | Pure academic papers (no accessibility layer) |
| Block-by-block workflow with verification | Quick notes or informal docs |
| Recovery patterns for complex documents | Simple single-page documents |

### Prerequisites

| Requirement | Why Needed |
|-------------|------------|
| AsciiDoc basic familiarity | Format syntax (`include::`, `<<>>`, sidebars) |
| Building Block Method understanding | Foundation/Build Up/Break Down concepts |
| 5-block structure awareness | METADATA → HEADER → CONTEXT → CONTENT → FOOTER |

### Key Terms

| Term | Definition |
|------|------------|
| **PhD Rigor** | Every claim supported by traceable evidence; no unsupported assertions |
| **Bible Accessibility** | Any point addressable, any depth available, any reader can navigate |
| **Distilled** | Compressed, crystallized claims in main text |
| **Verbose** | Full evidence/support in sidebars, footnotes, appendices |
| **Block** | One of 5 structural units with explicit completion criteria |

---

**What we're building:**

A whitepaper that achieves THREE things simultaneously:

| Aspect | What It Means | How It's Achieved |
|--------|---------------|-------------------|
| **PhD Rigor** | Every claim supported, every principle traceable | Distilled claims + verbose support via includes |
| **Bible Accessibility** | Infinite depth, universally navigable | Addressable sections, layered content, cross-refs |
| **Self-Demonstrating** | Document embodies what it teaches | Structure mirrors the method being documented |

**The key insight:**

> **PhD depth and universal accessibility are NOT opposites.**
>
> The Bible proves this: A five-year-old and a PhD scholar use the SAME book. The structure doesn't limit depth - it ENABLES access to depth.

**What this produces:**

| Reader Level | What They Get | How |
|--------------|---------------|-----|
| **Newcomer** | Core understanding | Read main text (distilled) |
| **Practitioner** | Working knowledge | Main text + sidebars/examples |
| **Scholar** | Complete depth | Appendices + all included material |
| **Reference user** | Specific answers | Index + cross-refs to exact location |

Same document. Same text. Infinite depth. Universal access.

---

<!--
================================================================================
CONTENT BLOCK
================================================================================
Reorganized v1.6.0: THINKING WORKFLOW (always active mindset) +
APPLICATION WORKFLOW (step-by-step when creating). Detailed patterns
moved to references/ to avoid context bloat.
================================================================================
-->

## Thinking Workflow

> **This section is ALWAYS ACTIVE.** The building block mindset should inform how you think about any documentation work, not just when you invoke this skill.

### Anchor Points: The Four Bases

Before you write anything, establish your **anchor points** - reference materials that are FIXED. You build FROM them, not create them.

| Base Type | What It Is | Example | Why It's an Anchor |
|-----------|------------|---------|-------------------|
| **Abstract** | Structured theory (WHAT/WHY) | `concept.adoc` | Principles already distilled |
| **Material** | Raw verbose content | `learning-notes.txt` | Full teaching already captured |
| **Knowledge** | Compressed understanding | `distilled-notes.txt` | Key insights already extracted |
| **Structural** | Executable demonstrations | `research/*.go` | Proof that runs |

**Why anchors matter:**

Anchors are what you CAN'T operate on - they're FIXED foundations. They halt the "where do I get this information?" recursion. You don't create them during the whitepaper - you BUILD FROM them.

```text
Anchor files (FIXED)
    │
    ├── concept.adoc ──────► Theory I reference
    ├── verbose.txt ───────► Material I extract from
    ├── distilled.txt ─────► Understanding I build on
    └── code/*.go ─────────► Evidence I include
            │
            ▼
    Whitepaper (BUILT from anchors)
```

### Targeted Retrieval: Linux Commands First

**Default to targeted extraction, not full file reads.**

| Approach | Command | When to Use |
|----------|---------|-------------|
| **Targeted** | `grep -n "anchor" concept.adoc` | Know what you're looking for |
| **Contextual** | `grep -B2 -A5 "anchor" concept.adoc` | Need surrounding context |
| **Structural** | `grep -n "^== " document.adoc` | Finding section locations |
| **Full Read** | `Read` tool | Last resort, need everything |

**Why targeted first:**

```bash
# TARGETED - get exactly what you need (building block method: BREAK DOWN)
grep -n "halt condition" research/main.go

# vs FULL LOAD - expensive, often unnecessary
cat research/main.go
```

Targeted retrieval IS the building block method:

- **Break down** = `grep` for what you need (extract blocks)
- **Build up** = construct the whitepaper with extracted blocks

Linux commands enforce DISCIPLINE - you must know WHAT you're looking for. This prevents drift and ensures you're building with purpose.

**Practical patterns:**

```bash
# Find where a concept is defined
grep -n "Foundation" concept.adoc | head -5

# Get section with context
grep -B2 -A10 "^== Layer 0" concept.adoc

# Find all code demonstrations
grep -l "func.*Demo" research/*.go

# Extract specific line range (after finding with grep)
sed -n '45,80p' research/main.go

# Count occurrences to understand coverage
grep -c "anchor" concept.adoc verbose.txt distilled.txt
```

### Running Demos: Produce Evidence

**Executable code is not documentation - it's TRUTH you can run.**

| What You Have | What Running Produces | How It's Used |
|---------------|----------------------|---------------|
| `main.go` with demos | Terminal output | `include::` in sidebars |
| Test functions | Pass/fail results | Evidence claims work |
| Example code | Concrete behavior | Proof you can't fake |

**Why run demos:**

1. **Output becomes includable evidence**

   ```bash
   go run . anchor > output/anchor-demo.txt
   ```

   Then in whitepaper:

   ```asciidoc
   .Output from running anchor demonstration
   ----
   include::output/anchor-demo.txt[]
   ----
   ```

2. **You can't fake understanding**
   - Seeing concrete output verifies YOUR understanding
   - If you can't explain the output, you don't understand the concept

3. **PhD style demands evidence**
   - Claims need support
   - Demos PRODUCE the support
   - Output is objective, not your interpretation

**Practical workflow:**

```bash
# 1. Run demo, capture output
go run . validate > output/validate-demo.txt 2>&1

# 2. Run with specific flags for different demonstrations
go run . anchor > output/anchor-demo.txt
go run . operations > output/operations-demo.txt

# 3. Include outputs in whitepaper sidebars
# (see include strategy section)
```

### Block by Block: Structure as Scaffold

**Plan each block BEFORE writing. The structure tells you what goes where.**

The 5-block structure isn't a constraint - it's a SCAFFOLD that shows:

| Block | What Goes Here | Expansion Slots |
|-------|----------------|-----------------|
| **METADATA** | Identity, biblical foundation | Add more attributes |
| **HEADER** | Navigation, quick start | Add more entry points |
| **CONTEXT** | Overview, scope | Add prerequisites |
| **CONTENT** | Layers with claims + evidence | Add sidebars, footnotes, includes |
| **FOOTER** | Appendices, index | Add full material |

**For each block, plan:**

```text
BLOCK: [Name]
├── What claims am I making?
├── What evidence supports each claim?
├── What includes do I need?
├── How does this connect to other blocks?
└── What's the expansion path?
```

**Structure tells you where to expand:**

| Slot | Purpose | To Add More Depth |
|------|---------|-------------------|
| Main text | Distilled claim | Refine precision |
| Sidebar | Code example | Add more demos |
| Footnote | Verbose detail | Expand explanation |
| Cross-ref | Connection | Add more links |
| Appendix | Full material | Include everything |

The structure GUIDES expansion. When you need more depth, you know WHERE it goes:

- More evidence? → Sidebar with include
- More explanation? → Footnote
- Full code? → Appendix
- Connections? → Cross-references

### TODOs in Building Blocks, Not Sequential Order

**Structure your TODOs to reflect the method, not a checklist.**

**Wrong - Sequential thinking:**

```text
1. Write metadata
2. Write header
3. Write overview
4. Write layer 0
5. Write layer 1
...
```

This is task completion. No dependencies. No halt condition. Just "do things in order."

**Right - Building block thinking:**

```text
ANCHOR: Create whitepaper with PhD depth + Bible accessibility
FOUNDATION: Establish anchor files
  └── Verify: concept.adoc, verbose.txt, distilled.txt, code/*.go
BLOCK: METADATA
  ├── Plan: What identity, biblical foundation
  ├── Write: Fill metadata attributes
  └── Verify: All required fields present
BLOCK: HEADER
  ├── Plan: Navigation structure, quick start
  ├── Write: Title, navigation, abstract
  └── Verify: All links resolve
BLOCK: CONTENT - Layer 0
  ├── Depends on: FOUNDATION
  ├── Plan: What claims, what evidence
  ├── Retrieve: grep for relevant content from anchors
  ├── Write: Distilled claim + sidebar with include
  └── Verify: Traces to anchor files
BLOCK: CONTENT - Layer 1
  ├── Depends on: Layer 0
  ├── Plan: What builds on Layer 0
  ├── Write: Distilled claim + evidence
  └── Verify: Stands on Layer 0
```

**Why building block TODOs matter:**

| Sequential TODOs | Building Block TODOs |
|------------------|---------------------|
| No halt condition | ANCHOR tells you when done |
| No dependencies visible | Each layer shows what it stands on |
| Linear only - can't switch | Can SWITCH - verify before continuing |
| Task completion mindset | Understanding construction |
| Don't know if truly done | Anchor satisfied = done |

**The TODO structure MIRRORS the document structure:**

| TODO Element | Document Element |
|--------------|------------------|
| ANCHOR | Document's purpose/anchor |
| FOUNDATION | Anchor files you build from |
| BLOCK: X | Document block (METADATA, HEADER, etc.) |
| Plan/Write/Verify | Build up / Break down cycle |
| Depends on | "Stands on" relationship |

**Within each BLOCK, the cycle:**

```text
BLOCK: [Name]
├── Plan ────────► What am I building? (before writing)
├── Retrieve ────► Get what I need from anchors (break down)
├── Write ───────► Construct the block (build up)
└── Verify ──────► Does it trace back? (break down to check)
```

**For reviewing/working block by block:**

- Each BLOCK is a complete unit with its own scope
- Verify one block completely before moving to next
- Can switch: if Layer 2 feels shaky, break down to verify Layer 1
- Each block's completion is checkable: "Does this satisfy the block's purpose?"

**The TODOs embody the method:**

- There's an ANCHOR (the goal that doesn't change)
- There's a FOUNDATION (what you build from)
- There are BLOCKS (units of work)
- Within blocks: Plan → Write → Verify (build up / break down)
- Dependencies are explicit (stands on previous)
- You know when DONE (anchor satisfied)

### Per-Block Verification Criteria

**The principle:** A block is complete when it meets ALL its criteria, not when you've written content in it.

**→ For detailed per-block checklists:** [verification-criteria.md](references/verification-criteria.md)

### Recovery Patterns

**The insight:** Errors are not failures—they're signals to switch from Build Up to Break Down. The method's interchangeability (-1 ↔ +1) means recovery is always available.

**→ For 5 detailed recovery patterns:** [recovery-patterns.md](references/recovery-patterns.md)

### First Pass = Deep Pass

**If you do it right, you shouldn't need multiple passes.**

The discipline:

1. **Anchor points established** → Know what you're building from
2. **Targeted retrieval** → Get exactly what you need
3. **Demos run** → Evidence produced
4. **Block planned** → Know what goes where
5. **Structure as guide** → Expansion slots visible

When ALL of these are in place, first pass IS the deep pass.

**Multiple passes often mean:**

- Skipped establishing anchors (had to go back for information)
- Used full reads instead of targeted (got lost in material)
- Didn't run demos (had to verify understanding later)
- Didn't plan blocks (had to restructure)
- Ignored structure (didn't know where things go)

**The goal:**

```text
First Pass:
├── Anchors ✓ (reference points fixed)
├── Retrieval ✓ (targeted, disciplined)
├── Evidence ✓ (demos run, output captured)
├── Planning ✓ (each block mapped)
├── Structure ✓ (scaffold guides placement)
└── Result: Deep, complete whitepaper
```

### The Meta-Insight

**This entire operational discipline IS the building block method applied to documentation:**

| Method Piece | Documentation Application |
|--------------|---------------------------|
| **Foundation (0)** | Anchor files - what you build FROM |
| **Break Down (-1)** | Targeted retrieval - `grep` for what you need |
| **Build Up (+1)** | Block by block - layer by layer construction |
| **Anchor** | Structure itself - tells you when block is complete |
| **Reconstruction** | First pass = deep pass - emerges from doing it right |

The whitepaper doesn't just DESCRIBE the method - the PROCESS of creating it DEMONSTRATES the method.

---

## Application Workflow

> **Use this section when actually creating documentation.** The thinking workflow should already be active; this is the step-by-step process.

### Step 0: Establish Anchors

Before anything else, verify your anchor files exist:

```bash
ls -la concept.adoc verbose.txt distilled.txt research/*.go
```

### Step 1: Targeted Retrieval

Default to targeted extraction, not full file reads:

```bash
# TARGETED—get exactly what you need
grep -n "foundation" concept.adoc | head -5

# CONTEXTUAL—need surrounding lines
grep -B2 -A5 "anchor" concept.adoc

# STRUCTURAL—find section locations
grep -n "^== " document.adoc
```

### Step 2: Run Demos for Evidence

```bash
go run . anchor > output/anchor-demo.txt 2>&1
```

### Step 3: Write Block by Block

For each block, follow the cycle: PLAN → WRITE → VERIFY

**→ For detailed per-block criteria:** [verification-criteria.md](references/verification-criteria.md)

### Step 4: Handle Errors

When something goes wrong, switch from Build Up to Break Down.

**→ For 5 recovery patterns:** [recovery-patterns.md](references/recovery-patterns.md)

### Step 5: Apply Academic Writing Patterns

Use dissertation-level patterns for PhD rigor with Bible accessibility.

**→ For 7 academic writing patterns:** [dissertation-patterns.md](references/dissertation-patterns.md)

### Step 6: Use AsciiDoc Features

Leverage includes, sidebars, footnotes, cross-refs for layered depth.

**→ For AsciiDoc syntax and patterns:** [asciidoc-reference.md](references/asciidoc-reference.md)

### Step 7: Final Validation

**PhD Rigor:**

- [ ] Every claim has supporting evidence
- [ ] Evidence is accessible (included, not just referenced)
- [ ] Reader can trace any claim to its foundation

**Bible Accessibility:**

- [ ] Any section can be addressed directly (anchors)
- [ ] Layers build sequentially (stands on previous)
- [ ] Reader can enter at any point (standalone sections)

**Kingdom Standards (The Three Questions):**

1. Would this honor God as document reviewer?
2. Does this genuinely serve others (all reader levels)?
3. Does this have eternal value (traceable truth)?

---
<!--
================================================================================
FOOTER BLOCK
================================================================================
Biblical foundation, examples, and closing references complete the document.
================================================================================
-->

## Biblical Foundation

> *"Therefore whosoever heareth these sayings of mine, and doeth them, I will liken him unto a wise man, which built his house upon a rock: And the rain descended, and the floods came, and the winds blew, and beat upon that house; and it fell not: for it was founded upon a rock."* — Matthew 7:24-25

**The Principle:** Foundation determines everything. A whitepaper built on traceable foundations stands. One built on unsupported claims falls.

> *"But be ye doers of the word, and not hearers only, deceiving your own selves."* — James 1:22

**The Principle:** Reading about the method isn't enough. The whitepaper must DEMONSTRATE the method, not just describe it.

> *"To every thing there is a season, and a time to every purpose under the heaven: A time to break down, and a time to build up."* — Ecclesiastes 3:1,3

**The Principle:** Verbose and distilled have their seasons. The whitepaper holds both - distilled in main text, verbose in support - each in its proper place.

---

## References

**Detailed patterns loaded on demand:**

| Reference | Contents | Load When |
|-----------|----------|-----------|
| [verification-criteria.md](references/verification-criteria.md) | Per-block completion checklists | Verifying blocks are complete |
| [recovery-patterns.md](references/recovery-patterns.md) | 5 recovery patterns | Something went wrong |
| [dissertation-patterns.md](references/dissertation-patterns.md) | 7 PhD writing patterns | Writing academic content |
| [asciidoc-reference.md](references/asciidoc-reference.md) | AsciiDoc syntax, include patterns | Need format reference |
| [example-structure.md](references/example-structure.md) | Complete whitepaper template | Starting a new whitepaper |

---

## See Also

| Document | Purpose | Location |
|----------|---------|----------|
| Building Block Method Whitepaper | Source material this skill was derived from | `bereshit/void/imports/building-block-method/building-block-method-principles.adoc` |
| create-from-template skill | Template creation for code/data/docs | `~/.claude/skills/create-from-template/SKILL.md` |
| CWS-STD-001 (4-block) | Code structure standard | `standards/CWS-STD-001-DOC-4-block.md` |
| CWS-STD-003 (Documentation) | Documentation standards | `standards/CWS-STD-003-DOC-documentation-standards.md` |
| AsciiDoc templates | Base templates for documentation | `bereshit/word/seed/documentation/adoc/` |

---

## Document Information

| Attribute | Value |
|-----------|-------|
| **Skill Name** | create-whitepaper |
| **Version** | 1.6.0 |
| **Status** | Active |
| **Last Updated** | 2025-12-06 |
| **Biblical Foundation** | Matthew 7:24-25 |
| **Structure** | Thinking Workflow (always active) + Application Workflow (when creating) + References (load on demand) |

---

> *"The structure doesn't limit depth—it ENABLES access to depth."*

**Building block thinking + targeted references = skill that serves without consuming context.**
