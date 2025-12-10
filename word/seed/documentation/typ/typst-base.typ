// #!omni template --typ

// ═══════════════════════════════════════════════════════════════════════════════
// METADATA BLOCK
// ═══════════════════════════════════════════════════════════════════════════════
//
// THINKING: This is Typst metadata - stored as dictionary for programmatic access.
// Serves systems (indexing, validation, cross-reference) AND establishes identity.
// This is the "catalog card" - what systems need to know about this document.
//
// Delete these comments when filling in, or keep for the next person.
//
// ═══════════════════════════════════════════════════════════════════════════════

// ─────────────────────────────────────────────────────────────────────────────
// Document Metadata Dictionary
// ─────────────────────────────────────────────────────────────────────────────

#let meta = (
  // ─────────────────────────────────────────────────────────────────────────
  // IDENTITY - What IS this document
  // ─────────────────────────────────────────────────────────────────────────
  key: "B-word-seed-doc-typst-base",
                                // Bereshit keying: ORIGIN-FOLDER-DOMAIN-NAME
  title: "",                    // Document title - clear, descriptive
  type: "",                     // Document type: Standard | Guide | Template |
                                // Reference | Index | Policy | Specification | Report

  // ─────────────────────────────────────────────────────────────────────────
  // STATE - Where is this in its lifecycle
  // ─────────────────────────────────────────────────────────────────────────
  status: "",                   // Lifecycle state: Draft | Active | Deprecated | Archived
  version: "",                  // SemVer: X.Y.Z (major.minor.patch)
                                // Major = breaking changes, Minor = additions, Patch = fixes

  // ─────────────────────────────────────────────────────────────────────────
  // TEMPORAL - When was this created and updated
  // ─────────────────────────────────────────────────────────────────────────
  created: "",                  // Creation date: YYYY-MM-DD (ISO 8601 format)
  updated: "",                  // Last update date: YYYY-MM-DD (ISO 8601 format)

  // ─────────────────────────────────────────────────────────────────────────
  // ATTRIBUTION - Who created and maintains this
  // ─────────────────────────────────────────────────────────────────────────
  authors: (),                  // List of authors: ("Name1", "Name2")
                                // Or with roles: ("Name1 (Architect)", "Name2 (Implementation)")

  // ─────────────────────────────────────────────────────────────────────────
  // LOCATION - Where does this live in the ternary structure
  // ─────────────────────────────────────────────────────────────────────────
  path: "",                     // Ternary path: /org/word/ | /org/work/ | /org/fruit/
                                // Or full path: /org/word/standards/

  // ─────────────────────────────────────────────────────────────────────────
  // CLASSIFICATION - How is this categorized for discovery
  // ─────────────────────────────────────────────────────────────────────────
  tags: (),                     // Categorization tags: ("template", "documentation", "standard")

  // ─────────────────────────────────────────────────────────────────────────
  // INTENT - Why does this document exist
  // ─────────────────────────────────────────────────────────────────────────
  purpose: "",                  // One-line purpose statement
                                // Pattern: "Enables X for Y" or "Defines X for Y"

  // ─────────────────────────────────────────────────────────────────────────
  // GROUNDING - What biblical truth anchors this work
  // ─────────────────────────────────────────────────────────────────────────
  biblical_foundation: "",      // Scripture reference (e.g., "Genesis 1:1")
                                // The truth that grounds and informs this document

  // ─────────────────────────────────────────────────────────────────────────
  // DERIVATION - Template lineage and strictness
  // ─────────────────────────────────────────────────────────────────────────
  derives_from: "bereshit/word/omni/seed/B-word-omni-seed-documentation.omni",
                                // Source template this derives from
                                // All format templates trace back to OmniCode source
  strictness: "G",              // Guidance level: T (Tight) | G (Guided) | F (Free)
                                // T = Follow exactly, G = Adapt thoughtfully, F = Principles only
)

// ─────────────────────────────────────────────────────────────────────────────
// Document Configuration
// ─────────────────────────────────────────────────────────────────────────────

#set document(
  title: meta.title,
  author: meta.authors,
)

#set page(
  paper: "us-letter",
  margin: (x: 1in, y: 1in),
)

#set text(
  font: ("Linux Libertine O", "Libertinus Serif", "Liberation Serif", "DejaVu Serif"),
  size: 11pt,
)

#set heading(numbering: "1.1")

// ═══════════════════════════════════════════════════════════════════════════════
// END METADATA BLOCK
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// HEADER BLOCK - The Cover
// ═══════════════════════════════════════════════════════════════════════════════
//
// THINKING: What readers see FIRST. The "cover" of your document.
// In 5 seconds, readers should know: What is this? Is it current? Is it for me?
//
// Contains:
// - Title (H1): What IS this document
// - Tagline: What it enables/does (one line)
// - Status indicators: Version, status, type
// - Grounding quote: Optional mission/purpose connection
// - Quick nav: Optional navigation bar for large documents
//
// This is RENDERED content - visible identity, not just metadata.
//
// ═══════════════════════════════════════════════════════════════════════════════

#align(center)[

  // THINKING: Optional logo/image for branded documents
  // #image("path/to/logo.png", width: 120pt)

  = [Title]

  // THINKING: Clear, descriptive title. What IS this document?
  // Should match the 'title' field in METADATA.

  *[One-line tagline describing what this enables or does]*

  // THINKING: The value proposition. Why does this exist? What does reader get?
  // Pattern: "The [thing] for [purpose]" or "[Verb]ing [outcome]"

  // THINKING: Status indicators - Typst uses text styling instead of badges
  #text(size: 9pt)[
    #box(fill: green.lighten(70%), inset: 4pt, radius: 2pt)[Status: Active]
    #h(8pt)
    #box(fill: blue.lighten(70%), inset: 4pt, radius: 2pt)[Version: X.Y.Z]
  ]

  // THINKING: Optional additional indicators
  // #box(fill: purple.lighten(70%), inset: 4pt, radius: 2pt)[Type: Standard]
  // #box(fill: gray.lighten(70%), inset: 4pt, radius: 2pt)[Key: DOMAIN-CAT-###]

  #v(8pt)

  // THINKING: Optional grounding quote - connects to mission/purpose
  // Use when the document has clear Kingdom significance
  _[Optional: Mission statement or grounding quote connecting to biblical foundation]_

  #line(length: 80%, stroke: 0.5pt + gray)

  // THINKING: Quick navigation bar
  // Link to major sections readers commonly need
  #text(size: 9pt)[
    #link(<overview>)[Overview] #sym.dot.c
    #link(<section-1>)[Section 1] #sym.dot.c
    #link(<typst-toolkit>)[Typst Toolkit] #sym.dot.c
    #link(<references>)[References]
  ]

]

#line(length: 100%, stroke: 0.5pt + gray)

// ═══════════════════════════════════════════════════════════════════════════════
// END HEADER BLOCK
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// CONTEXT BLOCK - The Introduction
// ═══════════════════════════════════════════════════════════════════════════════
//
// THINKING: What readers need to understand BEFORE the main content.
// Prepares understanding - context before content.
//
// Contains:
// - Overview: Brief description of what this covers
// - Critical concept: Important callout for must-know information
// - Quick Start: Optional fast path for doers
// - What This Provides: Table showing who gets what
// - Table of Contents: Navigation map for the document (Typst generates automatically)
//
// Layer for different readers:
// - Scan: Overview paragraph tells them what this is
// - Skim: Table shows what's here and who it serves
// - Read: They continue to Content block
//
// ═══════════════════════════════════════════════════════════════════════════════

== Overview <overview>

// THINKING: Brief description - what this document covers and why it matters.
// Keep it 2-4 sentences. Answer: What is this? Why should I care?

[Brief description of what this document covers and why it matters to the reader]

This document follows the CWS#footnote[CreativeWorkzStudio LLC] 5-block documentation structure.#footnote[The 5-block structure: METADATA → HEADER → CONTEXT → CONTENT → FOOTER. See CWS-STD-006 for full specification.]

#block(
  fill: rgb("#fff3cd"),
  inset: 12pt,
  radius: 4pt,
  width: 100%,
)[
  *Important:* [#highlight[Critical concept] or distinction readers MUST understand before proceeding]
]

// THINKING: Use Important callout for non-negotiable concepts.
// What misconception should we prevent? What's the key insight?

#block(
  fill: rgb("#d1ecf1"),
  inset: 12pt,
  radius: 4pt,
  width: 100%,
)[
  *Note — Scope:* [What IS and ISN'T covered by this document]
]

// THINKING: Scope prevents wasted time. Be explicit about boundaries.

#line(length: 100%, stroke: 0.5pt + gray)

=== Quick Start <quick-start>

// THINKING: Optional section for doers who want to act immediately.
// Use numbered comments for visual scanning.
// Skip this section if the document isn't action-oriented.

```bash
# 1️⃣ First step description
command-here arg1 arg2

# 2️⃣ Second step description
another-command --flag

# 3️⃣ Third step description
final-command
```

#line(length: 100%, stroke: 0.5pt + gray)

=== What This Document Provides <what-this-provides>

// THINKING: Table showing categories, what's included, and who it serves.
// Helps readers quickly assess if this document is for them.

#table(
  columns: (auto, 1fr, 2fr, 1fr),
  align: (center, left, left, left),
  stroke: 0.5pt + gray,
  inset: 8pt,

  [], [*Category*], [*What You'll Find*], [*Who It Serves*],

  [📖], [*[Category Name]*], [[Description of content]], [[Target audience]],
  [🔧], [*[Category Name]*], [[Description of content]], [[Target audience]],
  [📐], [*[Category Name]*], [[Description of content]], [[Target audience]],
)

// THINKING: Icon column uses emoji, but can also use Unicode symbols:
// ✓ ✗ for status, → for flow, • for bullets

#line(length: 100%, stroke: 0.5pt + gray)

=== Table of Contents <table-of-contents>

// THINKING: Typst can auto-generate table of contents with #outline()
// For manual control, use the list below

#outline(
  title: none,
  indent: 1.5em,
  depth: 3,
)

// Or maintain manually:
// - #link(<overview>)[Overview]
//   - #link(<quick-start>)[Quick Start]
//   - #link(<what-this-provides>)[What This Document Provides]
// - #link(<section-1>)[Section 1]
// - #link(<section-2>)[Section 2]
// - #link(<examples>)[Examples]
// - #link(<quick-reference>)[Quick Reference]
// - #link(<typst-toolkit>)[Typst Toolkit]
// - #link(<biblical-foundation>)[Biblical Foundation]
// - #link(<references>)[References]

#align(right)[#link(<overview>)[↑ Back to Top]]

#line(length: 100%, stroke: 0.5pt + gray)

// ═══════════════════════════════════════════════════════════════════════════════
// END CONTEXT BLOCK
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// CONTENT BLOCK - The Chapters
// ═══════════════════════════════════════════════════════════════════════════════
//
// THINKING: The actual value - organized for reader comprehension.
// This is "the chapters" - the substance of the document.
//
// Structure elements:
// - == Sections: Major topics (navigable from outline)
// - === Subsections: Detailed breakdowns
// - Tables: Comparisons, structured data
// - Code blocks: Examples with emoji comments (1️⃣ 2️⃣ 3️⃣)
// - Callouts: Styled blocks for guidance
// - Back to Top: Navigation aids after major sections
//
// Toolkit elements to USE naturally:
// - #footnote[...] for definitions and depth
// - #highlight[text] sparingly for true emphasis
// - #box[...] for keyboard shortcuts or button-like navigation
// - Symbols: ✓ ✗ → ← • ▸ for visual scanning
//
// Layer for different readers:
// - Scan: Headings and tables give the shape
// - Read: Full explanations and examples
// - Study: Footnotes for depth
//
// ═══════════════════════════════════════════════════════════════════════════════

== Section 1 <section-1>

// THINKING: Introduction to this section - what it covers and why it matters.
// Each == section should be navigable from Table of Contents.
// Use descriptive section names, not generic "Section 1"

[Introduction paragraph explaining what this section covers]

=== Subsection Title <subsection-title>

// THINKING: Content organized for reader comprehension.

[Content organized clearly - explain the concept, pattern, or information]

*Key principle:* [The core idea readers should remember from this subsection]

=== When to [Do This Thing] <when-to-do-this>

// THINKING: Decision guidance helps readers apply knowledge.
// Tables work well for scenario → approach → rationale patterns.

#table(
  columns: (1fr, 1fr, 1fr),
  align: (left, left, left),
  stroke: 0.5pt + gray,
  inset: 8pt,

  [*Scenario*], [*Approach*], [*Rationale*],

  [*When [condition A]*], [[What to do]], [[Why this works]],
  [*When [condition B]*], [[What to do]], [[Why this works]],
  [*When [condition C]*], [[What to do]], [[Why this works]],
)

#block(
  fill: rgb("#d4edda"),
  inset: 12pt,
  radius: 4pt,
  width: 100%,
)[
  *Tip:* [Helpful non-obvious guidance or best practice]
]

// THINKING: Use Tip callout for recommendations and best practices.
// Use sparingly - if everything is a tip, nothing is.

#align(right)[#link(<overview>)[↑ Back to Top]]

=== Optional Deep-Dive <optional-deep-dive>

// THINKING: Optional depth for advanced users.
// Good for: edge cases, historical context, implementation details, theory.

[Detailed information for those who need deeper understanding]

*Technical details:*

```go
// Example code with comments explaining key aspects
func example() {
    // 1️⃣ First step explanation
    doSomething()

    // 2️⃣ Second step explanation
    doSomethingElse()
}
```

#align(right)[#link(<overview>)[↑ Back to Top]]

#line(length: 100%, stroke: 0.5pt + gray)

== Section 2 <section-2>

// THINKING: Continue with additional sections as needed.
// Each major topic gets its own == section.
// Maintain consistent structure across sections.

[Introduction to this section]

=== Comparison or Decision Matrix <comparison-matrix>

// THINKING: Tables excel at showing comparisons and relationships.
// Use visual indicators: ✓ (yes), ✗ (no), ○ (optional)

#table(
  columns: (1fr, auto, auto, auto),
  align: (left, center, center, center),
  stroke: 0.5pt + gray,
  inset: 8pt,

  [*Feature*], [*Option A*], [*Option B*], [*Recommended*],

  [*[Feature 1]*], [✓], [✗], [A],
  [*[Feature 2]*], [✗], [✓], [B],
  [*[Feature 3]*], [✓], [✓], [Either],
  [*[Feature 4]*], [○], [○], [Depends],
)

// THINKING: ✓ = yes/has, ✗ = no/missing, ○ = optional/partial

=== Process or Workflow <process-workflow>

// THINKING: For processes, numbered steps with clear outcomes.

+ *[Step Name]* — [What to do and expected outcome]
+ *[Step Name]* — [What to do and expected outcome]
+ *[Step Name]* — [What to do and expected outcome]

// THINKING: Typst can use external diagram packages for visual workflows
// Or use ASCII art in a raw block

```
Start → Step 1 → Step 2 → Step 3 → End
```

#align(right)[#link(<overview>)[↑ Back to Top]]

#line(length: 100%, stroke: 0.5pt + gray)

== Examples <examples>

// THINKING: Real examples demonstrating concepts in practice.
// Link to actual files when possible - proof the pattern works.
// Show, don't just tell.

Real [documents/code/systems] demonstrating these [concepts/patterns] in practice.

=== Example 1: [Descriptive Name] <example-1>

*Source:* [Link to example file or location]

*What it demonstrates:*

#table(
  columns: (1fr, 2fr),
  stroke: 0.5pt + gray,
  inset: 8pt,

  [*Aspect*], [*How It's Shown*],

  [*[Concept 1]*], [[How the example demonstrates this]],
  [*[Concept 2]*], [[How the example demonstrates this]],
)

*Key insight:* [What readers should learn from studying this example]

#align(right)[#link(<overview>)[↑ Back to Top]]

=== Example 2: [Descriptive Name] <example-2>

*Source:* [Link to example file or location]

[Description and analysis of this example]

#align(right)[#link(<overview>)[↑ Back to Top]]

#line(length: 100%, stroke: 0.5pt + gray)

== Quick Reference <quick-reference>

// THINKING: Checklists, summaries, or quick-lookup information.
// For returning users who know the content and need to verify/remember.

=== Checklist: Before [Action] <checklist>

*Verify these before proceeding:*

- ☐ [Requirement or verification step]
- ☐ [Requirement or verification step]
- ☐ [Requirement or verification step]
- ☐ [Requirement or verification step]

=== Quick Lookup: [Topic] <quick-lookup>

// THINKING: Reference tables for common lookups.
// Keep it scannable - this is for quick answers, not learning.

#table(
  columns: (1fr, 2fr),
  stroke: 0.5pt + gray,
  inset: 8pt,

  [*Term*], [*Definition*],

  [*[Term 1]*], [[Brief definition]],
  [*[Term 2]*], [[Brief definition]],
  [*[Term 3]*], [[Brief definition]],
)

#align(right)[#link(<overview>)[↑ Back to Top]]

#line(length: 100%, stroke: 0.5pt + gray)

== Typst Toolkit <typst-toolkit>

// THINKING: This section demonstrates ALL available Typst features.
// Use this as a reference when building documents - don't limit yourself!
// Delete this section in final documents, or keep relevant patterns.

=== Text Formatting <text-formatting>

#table(
  columns: (1fr, 2fr, 1fr),
  stroke: 0.5pt + gray,
  inset: 8pt,

  [*Format*], [*Syntax*], [*Result*],

  [Bold], [`*text*`], [*text*],
  [Italic], [`_text_`], [_text_],
  [Bold+Italic], [`*_text_*`], [*_text_*],
  [Strikethrough], [`#strike[text]`], [#strike[text]],
  [Inline code], [``` `code` ```], [`code`],
  [Highlight], [`#highlight[text]`], [#highlight[text]],
  [Superscript], [`#super[text]`], [x#super[2]],
  [Subscript], [`#sub[text]`], [H#sub[2]O],
  [Keyboard], [`#box(stroke: 0.5pt)[key]`], [#box(stroke: 0.5pt, inset: 2pt)[Ctrl]+#box(stroke: 0.5pt, inset: 2pt)[C]],
)

=== Footnotes & Definitions <footnotes>

// THINKING: Footnotes keep flow clean while providing depth for those who want it

This document follows the 5-block structure.#footnote[See CWS-STD-006 for complete 5-block documentation.] The structure ensures PhD rigor#footnote[Rigor meaning: complete, defensible, thorough structure.] with Bible accessibility.

*Syntax:*

```typst
Text with footnote.#footnote[Footnote content appears at page bottom.]
```

=== Navigation Styles <navigation-styles>

// THINKING: Multiple ways to create navigation - choose based on document style

*Bullet separators:*

#link(<section-1>)[*Section 1*] #sym.dot.c #link(<section-2>)[*Section 2*] #sym.dot.c #link(<references>)[*References*]

*Pipe separators:*

#link(<section-1>)[Section 1] | #link(<section-2>)[Section 2] | #link(<references>)[References]

*Button style (box):*

#box(stroke: 0.5pt, inset: 4pt)[#link(<section-1>)[Section 1]]
#box(stroke: 0.5pt, inset: 4pt)[#link(<section-2>)[Section 2]]
#box(stroke: 0.5pt, inset: 4pt)[#link(<references>)[References]]

*Arrow style:*

#link(<section-1>)[Section 1] → #link(<section-2>)[Section 2] → #link(<references>)[References]

=== Callout Types <callout-types>

// THINKING: Styled blocks for different types of callouts

#block(
  fill: rgb("#d1ecf1"),
  inset: 12pt,
  radius: 4pt,
  width: 100%,
)[
  *Note:* Additional context, supplementary information, scope clarification.
]

#v(8pt)

#block(
  fill: rgb("#d4edda"),
  inset: 12pt,
  radius: 4pt,
  width: 100%,
)[
  *Tip:* Best practices, recommendations, helpful guidance.
]

#v(8pt)

#block(
  fill: rgb("#fff3cd"),
  inset: 12pt,
  radius: 4pt,
  width: 100%,
)[
  *Important:* Critical concepts, must-understand information, non-negotiable.
]

#v(8pt)

#block(
  fill: rgb("#f8d7da"),
  inset: 12pt,
  radius: 4pt,
  width: 100%,
)[
  *Warning:* Potential issues, cautions, things to avoid.
]

#v(8pt)

#block(
  fill: rgb("#f5c6cb"),
  inset: 12pt,
  radius: 4pt,
  width: 100%,
)[
  *Caution:* Serious warnings, actions that could cause problems.
]

=== Unicode Symbols Reference <symbols>

#table(
  columns: (1fr, 2fr, 1fr),
  stroke: 0.5pt + gray,
  inset: 8pt,

  [*Category*], [*Symbols*], [*Use For*],

  [*Checks*], [✓ ✗ ✔ ✘ ☑ ☐], [Status, completion],
  [*Arrows*], [→ ← ↑ ↓ ↔ ⇒ ⟶], [Flow, navigation],
  [*Bullets*], [• ◦ ▪ ▸ ► ‣], [Lists, separators],
  [*Math*], [× ÷ ± ≠ ≤ ≥ ∞ ≈], [Calculations],
  [*Status*], [🟢 🟡 🔴 ⚪], [Visual indicators],
  [*Misc*], [© ® ™ ° … § † ‡], [Legal, typography],
  [*Box drawing*], [┌ ─ ┐ │ └ ┘ ├ ┤], [ASCII diagrams],
)

// THINKING: Typst also has #sym.* for many symbols:
// #sym.arrow.r, #sym.checkmark, #sym.dot.c, etc.

=== Advanced: Diagrams <diagrams>

// THINKING: Typst uses external packages like fletcher, cetz for diagrams
// Or use ASCII art in raw blocks

*Flowchart (ASCII):*

```
METADATA → HEADER → CONTEXT → CONTENT → FOOTER
```

*Using Typst packages:*

```typst
// Import diagram package
#import "@preview/fletcher:0.4.0" as fletcher

// Create flowchart
#fletcher.diagram(...)
```

=== Advanced: Table Alignment <table-alignment>

#table(
  columns: (1fr, 1fr, 1fr),
  align: (left, center, right),
  stroke: 0.5pt + gray,
  inset: 8pt,

  [*Left (default)*], [*Center*], [*Right*],

  [Text], [Text], [Text],
  [Longer text], [Centered], [123],
)

*Syntax:*

```typst
#table(
  columns: (1fr, 1fr, 1fr),
  align: (left, center, right),
  ...
)
```

=== Advanced: Variables & References <variables>

// THINKING: Typst uses variables for reusable values

Define variables at document top:

```typst
#let std-006 = "path/to/CWS-STD-006.typ"
#let g-000 = "path/to/TEMPLATE-G-000.typ"
```

Use labels and references:

```typst
// Create a label
== Section Title <section-title>

// Reference it
See #link(<section-title>)[Section Title] for details.
```

*Benefits:*

- Variables defined once, used many times
- Easy to update if values change
- Labels provide internal cross-referencing

#align(right)[#link(<overview>)[↑ Back to Top]]

#line(length: 100%, stroke: 0.5pt + gray)

// ═══════════════════════════════════════════════════════════════════════════════
// END CONTENT BLOCK
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// FOOTER BLOCK - The Appendix
// ═══════════════════════════════════════════════════════════════════════════════
//
// THINKING: Where readers go NEXT. Resources, references, connections.
// This is the "appendix" - supporting material and navigation.
//
// Contains:
// - Biblical Foundation: Grounding in truth (the WHY beneath the what)
// - References: Links to related documents, standards, resources
// - Centered closing: Key summary, version, grounding verse
// - Cross-references: Connection to the larger ecosystem
//
// Send readers out with:
// - Grounding in truth (why this matters eternally)
// - Clear next steps (where to go from here)
// - Connection to ecosystem (how this fits the whole)
//
// ═══════════════════════════════════════════════════════════════════════════════

== Biblical Foundation <biblical-foundation>

// THINKING: Connect the work to eternal truth.
// Not decoration - genuine grounding that informs the document.
// If you can't articulate how Scripture informs this work, reflect on whether
// the document serves Kingdom purposes.

#block(
  fill: rgb("#d1ecf1"),
  inset: 12pt,
  radius: 4pt,
  width: 100%,
)[
  *Note:* [Context for why this biblical principle applies to this document's topic]
]

#v(12pt)

#align(center)[
  _"[Scripture verse that grounds this work]"_ — *[Book Chapter:Verse]*
]

#v(12pt)

*Applied:* [How this scriptural truth specifically informs this document's approach, structure, or content. Make the connection clear and practical.]

#align(right)[#link(<overview>)[↑ Back to Top]]

#line(length: 100%, stroke: 0.5pt + gray)

== References <references>

// THINKING: Organize references by type for easy discovery.
// Use variables at document top for maintainability.
// Link generously - knowledge grows through connection.

=== Related Documents <related-documents>

// THINKING: Documents readers should explore next or alongside this one.

#table(
  columns: (auto, 1fr, 2fr),
  align: (center, left, left),
  stroke: 0.5pt + gray,
  inset: 8pt,

  [], [*Document*], [*Purpose*],

  [📐], [*[Document Title]*], [[What it covers and why it's related]],
  [📖], [*[Document Title]*], [[What it covers and why it's related]],
)

=== Standards & Specifications <standards>

// THINKING: Standards this document implements or relates to.

#table(
  columns: (1fr, 2fr),
  stroke: 0.5pt + gray,
  inset: 8pt,

  [*Standard*], [*Purpose*],

  [*CWS-STD-NNN*], [[What it governs]],
  [*CWS-STD-NNN*], [[What it governs]],
)

=== External Resources <external-resources>

// THINKING: External links for further learning.

#table(
  columns: (1fr, 2fr),
  stroke: 0.5pt + gray,
  inset: 8pt,

  [*Resource*], [*Purpose*],

  [*#link("https://example.com")[Resource Name]*], [[What it provides]],
)

#line(length: 100%, stroke: 0.5pt + gray)

== See Also <see-also>

// THINKING: Cross-references to related ecosystem documents.
// Helps readers discover connected knowledge.

- *[Related Document 1]* — [Brief description of relationship]
- *[Related Document 2]* — [Brief description of relationship]
- *[Related Document 3]* — [Brief description of relationship]

#line(length: 100%, stroke: 0.5pt + gray)

#align(center)[

  // THINKING: Centered footer provides closure and key information at a glance.
  // Summarizes identity, provides navigation, grounds in truth.

  #link(<overview>)[*↑ Back to Top*]

  #line(length: 60%, stroke: 0.5pt + gray)

  *Key:* [DOMAIN-CAT-NNN] #sym.dot.c *Type:* [Type] #sym.dot.c *Version:* [X.Y.Z]

  *Status:* [Active|Draft|Deprecated] #sym.dot.c *Updated:* [YYYY-MM-DD]

  #line(length: 60%, stroke: 0.5pt + gray)

  _"[Biblical foundation verse from METADATA]"_ — *[Reference]*

  *[One line connecting this work to Kingdom purpose]*

  #line(length: 60%, stroke: 0.5pt + gray)

  // THINKING: Optional company standards navigation
  // *Company Standards:*
  // #link("path/to/4-block")[4-Block Structure] #sym.dot.c
  // #link("path/to/keying")[Document Keying] #sym.dot.c
  // #link("path/to/docs")[Documentation Standards]

]

// ═══════════════════════════════════════════════════════════════════════════════
// VARIABLES & REFERENCES
// ═══════════════════════════════════════════════════════════════════════════════
//
// THINKING: Define reusable variables here for maintainability.
// If a path changes, update once here instead of throughout document.
//
// #let ref-doc1 = "path/to/document.typ"
// #let ref-doc2 = "path/to/document.typ"
// #let std-ref = "standards/CWS-STD-001-DOC-4-block.typ"
// #let std-ref2 = "standards/CWS-STD-002-DOC-document-keying.typ"
// #let ext-ref = "https://example.com"
//
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// TEMPLATE NOTES - DELETE WHEN USING
// ═══════════════════════════════════════════════════════════════════════════════
//
// This template embodies:
// - PhD dissertation RIGOR: Complete structure, clear citations, defensible
// - Bible ACCESSIBILITY: Layered for all readers, clear navigation, timeless
//
// The 5-Block Pattern:
// ┌─────────────────────────────────────────────────────────────────┐
// │ METADATA   │ The catalog card - parseable identity for systems │
// ├─────────────────────────────────────────────────────────────────┤
// │ HEADER     │ The cover - visible identity in 5 seconds         │
// ├─────────────────────────────────────────────────────────────────┤
// │ CONTEXT    │ The introduction - prepares understanding         │
// ├─────────────────────────────────────────────────────────────────┤
// │ CONTENT    │ The chapters - value organized for discovery      │
// ├─────────────────────────────────────────────────────────────────┤
// │ FOOTER     │ The appendix - resources and grounding            │
// └─────────────────────────────────────────────────────────────────┘
//
// Layered Reading:
// - SCAN (5 sec):    Title, status boxes, tagline → what is this?
// - SKIM (1 min):    Overview, tables, headings → what's the shape?
// - READ (full):     All content → understand and apply
// - STUDY (deep):    Footnotes → master it
// - REFERENCE:       Quick Reference, links → find what I need
//
// ─────────────────────────────────────────────────────────────────
// TOOLKIT ELEMENTS - USE THESE NATURALLY THROUGHOUT YOUR DOCUMENT
// ─────────────────────────────────────────────────────────────────
//
// First technical term mentions → #footnote[Full explanation]
// True emphasis (rare)          → #highlight[text]
// Navigation/buttons            → #box(stroke: 0.5pt)[#link(<label>)[text]]
// Depth without interruption    → #footnote[...] (appears at page bottom)
// Status/comparison in tables   → ✓ (yes) ✗ (no) ○ (optional)
// Flow and sequence             → A → B → C
//
// Callouts for guidance (styled blocks):
// - Note (blue)      → rgb("#d1ecf1") - additional context, scope
// - Tip (green)      → rgb("#d4edda") - recommendations, best practices
// - Important (yellow) → rgb("#fff3cd") - critical concepts, must-know
// - Warning (red)    → rgb("#f8d7da") - potential issues, cautions
// - Caution (dark red) → rgb("#f5c6cb") - serious warnings
//
// Diagrams:
// - Use Typst packages like fletcher, cetz
// - Or ASCII art in raw blocks
//
// Variables (define at top, use throughout):
// #let my-var = "value"
// ... later: #my-var
//
// ─────────────────────────────────────────────────────────────────
//
// When filling this template:
// 1. Start with METADATA → establish identity (fill #let meta dictionary)
// 2. Write HEADER → the hook that draws readers in
// 3. Draft CONTEXT → prepare their understanding
// 4. Build CONTENT → deliver the value (USE toolkit elements naturally!)
// 5. Complete FOOTER → ground and connect
// 6. Delete THINKING comments (or keep for next person)
// 7. Delete/modify Typst Toolkit section (keep as reference or remove)
// 8. Update variables section
// 9. Compile to verify: typst compile document.typ
//
// KEY PRINCIPLE: Don't just demonstrate toolkit elements in a separate section -
// USE them throughout. #footnote for definitions. #highlight for emphasis.
// #box for navigation. Symbols for scanning. The toolkit section shows
// WHAT'S AVAILABLE; your document shows HOW TO USE THEM.
//
// ═══════════════════════════════════════════════════════════════════════════════
// END FOOTER BLOCK
// ═══════════════════════════════════════════════════════════════════════════════
