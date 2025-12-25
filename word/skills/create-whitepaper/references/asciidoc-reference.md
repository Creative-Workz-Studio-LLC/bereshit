# AsciiDoc Reference for Whitepapers

> **Reference for:** [create-whitepaper skill](../SKILL.md)
>
> **Purpose:** AsciiDoc features that enable PhD rigor + Bible accessibility

---

## Why AsciiDoc

AsciiDoc enables the verbose-distilled blend through native features. The document becomes a **NAVIGATION LAYER** over ALL the material, not a container of copied excerpts.

```text
Main Document (crystallized, navigable)
    │
    ├── include::code-examples.go[tags=...]
    │
    ├── include::verbose-notes.txt[]
    │
    ├── include::research-output.txt[]
    │
    └── include::full-appendix.adoc[leveloffset=+1]
```

---

## Core Features

| Feature | Purpose | Syntax |
|---------|---------|--------|
| **Includes** | Pull in external files without copying | `include::path/to/file.ext[]` |
| **Tagged Includes** | Include only specific sections | `include::file.go[tags=section-name]` |
| **Footnotes** | Verbose support without breaking flow | `footnote:[Detail here]` |
| **Sidebars** | Deep dives alongside main text | `[sidebar]` block |
| **Admonitions** | Different types of callouts | `[NOTE]`, `[TIP]`, `[IMPORTANT]`, `[WARNING]` |
| **Cross-references** | Link between sections | `<<anchor-name>>` or `<<anchor,Display Text>>` |
| **Index terms** | Build searchable index | `indexterm:[term]` |
| **Collapsible** | Optional depth | `[%collapsible]` block |
| **Table of Contents** | Auto-generated navigation | `:toc: left` |
| **Anchors** | Addressable points | `[[anchor-name]]` |
| **Leveloffset** | Include docs at adjusted heading level | `include::file.adoc[leveloffset=+1]` |
| **Attributes** | Define once, use everywhere | `:attribute-name: value` |

---

## Include Architecture

```text
whitepaper.adoc (main document)
    │
    ├── Main Text (distilled principles)
    │       └── Sidebars with include::code[tags=...]
    │
    ├── Appendix A: Full Source Code
    │       └── include::src/*.go[leveloffset=+1]
    │
    ├── Appendix B: Verbose Learning Notes
    │       └── include::learning-notes.txt[]
    │
    ├── Appendix C: Research Output
    │       └── include::output/*.txt[]
    │
    └── Glossary/Index
            └── Collected from indexterm:[] throughout
```

---

## Practical Patterns

### Include with Tags

```asciidoc
// In your Go file, tag sections:
// tag::halt-demo[]
func isHaltCondition(anchor string) bool {
    return anchor != "" // Can't operate on anchor
}
// end::halt-demo[]

// In your AsciiDoc:
[source,go]
----
include::../../research/main.go[tags=halt-demo]
----
```

### Sidebar for Verbose Support

```asciidoc
[sidebar]
.Deep Dive: Why This Matters
--
Extended explanation here...

include::detailed-explanation.txt[]

This connects to <<foundation-section>> because...
--
```

### Collapsible for Optional Depth

```asciidoc
[%collapsible]
.Click to see full code listing
====
[source,go]
----
include::full-implementation.go[]
----
====
```

### Index Terms for Discoverability

```asciidoc
The [[anchor-definition]]anchorfootnote:[Also called "halt condition"]
indexterm:[anchor]
indexterm:[halt condition]
is what you cannot operate on.
```

### Include Code with Output

```asciidoc
.Demonstration: Anchor as Halt Condition
[source,go]
----
include::../../research/anchor.go[tags=halt-demo]
----

.Output
----
include::../../research/output/halt-demo.txt[]
----
```

### Include as Appendix

```asciidoc
[appendix]
== Full Source Code

include::../../research/main.go[leveloffset=+2]

include::../../research/operations.go[leveloffset=+2]
```

### Include Learning Notes

```asciidoc
[appendix]
== Verbose Learning Notes

The following contains the complete teaching sessions from which this whitepaper was derived.

----
include::learning-notes.txt[]
----
```

---

## Bible-Level Accessibility Features

| Bible Feature | AsciiDoc Implementation |
|---------------|-------------------------|
| Book:Chapter:Verse | Document → Section → Subsection with anchors |
| Concordance | Index terms (`indexterm:[concept]`) |
| Cross-references | `<<anchor-name>>` links |
| Study notes | Sidebars, footnotes |
| Different translations | Multiple explanations/examples of same concept |
| Commentary | Appendices with full verbose material |

---

## The Key Insight

> **The structure doesn't limit depth - it ENABLES access to depth.**

AsciiDoc lets you CRYSTALLIZE (compress, structure) the main document so you can ADD MORE via includes. A well-structured whitepaper means:

- Reader chooses their depth
- Nothing is dumbed down
- Everything is findable
- Connections are visible
- Any point is addressable
