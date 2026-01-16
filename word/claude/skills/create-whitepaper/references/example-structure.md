# Whitepaper Structure Example

> **Reference for:** [create-whitepaper skill](../SKILL.md)
>
> **Purpose:** Complete 5-block whitepaper template

---

## Full Example

```asciidoc
////
METADATA BLOCK
////
:key: WP-METHOD-001
:title: The Method - A Complete Guide
:type: Whitepaper
:status: Active
:version: 1.0.0
:biblical_foundation: Matthew 7:24-25
:derives_from: source-material.txt, learning-notes.txt
// ... other metadata

////
HEADER BLOCK
////
= {title}

[.text-center]
****
*Navigation*
<<layer-0>> • <<layer-1>> • <<layer-2>> • <<conclusion>>
<<appendix-a>> • <<appendix-b>> • <<index>>
****

[abstract]
--
*Abstract:* Brief purpose statement here.

*Keywords:* term1, term2, term3

*Biblical Grounding:* Matthew 7:24-25 — _"Therefore whosoever heareth these sayings of mine, and doeth them, I will liken him unto a wise man, which built his house upon a rock."_
--

////
CONTEXT BLOCK
////
== Overview

[.lead]
Brief introduction to what this whitepaper covers.

=== Quick Start

> **30-second orientation:** Core summary here.

=== Scope

| In Scope | Out of Scope |
|----------|--------------|
| What's covered | What's not |

=== Prerequisites

| Requirement | Why Needed |
|-------------|------------|
| Prereq 1 | Reason |

=== Key Terms

| Term | Definition |
|------|------------|
| **Term 1** | Definition |

////
CONTENT BLOCK
////
[[layer-0]]
== Layer 0: The Foundation
indexterm:[foundation]

_(Foundation - everything stands on this)_

Main text with distilled claim.

[sidebar]
.Evidence
--
include::../../code/foundation.go[tags=demo]
--

[[layer-1]]
== Layer 1: First Principle
indexterm:[first principle]

_(Stands on: <<layer-0>>)_

Main text building on foundation.

footnote:[Detailed explanation that supports the claim]

[sidebar]
.Prior Understanding
****
Before this learning, what was known:

[quote, Source, Line reference]
____
Exact quote from source
____

What was missing: specific gap identified.
****

[[layer-2]]
== Layer 2: Second Principle
indexterm:[second principle]

_(Stands on: <<layer-1>>)_

Main text continuing the build.

[[conclusion]]
== Conclusion

_(Stands on: All layers)_

Synthesis of everything above.

////
FOOTER BLOCK
////
== Biblical Foundation

> *"Scripture quote here."* — Reference

**The Principle:** How this grounds the work.

[appendix]
[[appendix-a]]
== Full Source Code

include::../../code/main.go[]

[appendix]
[[appendix-b]]
== Verbose Learning Notes

----
include::learning-notes.txt[]
----

== See Also

| Document | Purpose | Location |
|----------|---------|----------|
| Related doc | Why relevant | Path |

== Document Information

| Attribute | Value |
|-----------|-------|
| **Version** | 1.0.0 |
| **Status** | Active |
| **Last Updated** | Date |
| **Biblical Foundation** | Reference |

[[index]]
== Index
// Auto-generated from indexterm:[]
```

---

## Block-by-Block Checklist

Use this when creating a new whitepaper:

```text
METADATA:
[ ] :key: unique identifier
[ ] :title: document title
[ ] :type: Whitepaper
[ ] :status: Active/Draft
[ ] :version: semantic version
[ ] :biblical_foundation: grounding Scripture
[ ] :derives_from: source files

HEADER:
[ ] Title renders correctly
[ ] Navigation links all resolve
[ ] Abstract in 30 seconds
[ ] Multiple entry points

CONTEXT:
[ ] Quick Start present
[ ] Scope (In/Out) defined
[ ] Prerequisites listed
[ ] Key Terms defined

CONTENT (each layer):
[ ] Anchor [[layer-N]]
[ ] "Stands on" previous layer
[ ] Distilled claim in main text
[ ] Evidence in sidebar/footnote
[ ] Cross-refs functional
[ ] Index terms present

FOOTER:
[ ] Biblical Foundation section
[ ] Appendices with full material
[ ] See Also with related docs
[ ] Document Information table
```
