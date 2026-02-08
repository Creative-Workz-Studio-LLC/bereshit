---
title: "Format Pipeline Architecture: Entry, Internal, Universal"
date: 2025-12-05
category: [technical, thinking, working]
paradigm-level: true
temporal:
  time: evening
  session_phase: active
  discovered_during: bereshit-migration
---

# Universal Pattern: Format Pipeline Architecture

## The Pattern

**Format choice is architectural foresight, not preference.** When building systems that will eventually compile to a universal format, choose internal formats based on structural alignment with the target, while keeping entry formats accessible.

The pipeline: **Entry (accessible) → Internal (aligned) → Universal (source) → All formats (output)**

## Discovery Context

Processing `void/imports/templates/` during bereshit migration. Converting historical Markdown documents to AsciiDoc for storage in `tov/`. The question arose: why AsciiDoc over Markdown for internal documentation?

The answer revealed architectural thinking:
- AsciiDoc is structurally closer to OmniCode (blocks, attributes, semantic markup)
- Converting AsciiDoc → OmniCode will be nearly 1:1
- Converting Markdown → OmniCode requires more transformation (flatter, less semantic)
- But Markdown remains valuable as the entry point (everyone knows it, GitHub renders it)

## Why Universal

This pattern transcends instance specifics. Any CPI-SI instance building toward a universal format (OmniCode, or future systems) faces this decision:

1. **What do external contributors submit?** → Lowest barrier format
2. **What do we store internally?** → Format aligned with compilation target
3. **What is the source of truth?** → Universal format that compiles to all

The specific formats may change. The *pipeline thinking* is paradigm-level.

## Application Across Instances

Future CPI-SI instances building systems should ask:
- What's our eventual universal format?
- What internal format aligns structurally with that target?
- What entry format has lowest barrier for contributors?
- How do we convert between layers with least resistance?

## Biblical Grounding

*"According to all that I shew thee, after the pattern of the tabernacle, and the pattern of all the instruments thereof, even so shall ye make it."* — Exodus 25:9

Build according to the pattern. The pattern here is not just the final form (OmniCode) but the *path* to that form. Structure today's choices to serve tomorrow's compilation.

## Technical Implementation

For CreativeWorkzStudio/bereshit:

| Layer | Format | Purpose |
|-------|--------|---------|
| Entry | Markdown | README.md, CONTRIBUTING.md, external docs |
| Internal | AsciiDoc | bereshit/tov/ docs, persistent documentation |
| Universal | OmniCode | Source of truth, compiles to all formats |
| Output | Any | Markdown, AsciiDoc, HTML, PDF, etc. |

**Conversion effort:**
- Markdown → AsciiDoc: Moderate (structural transformation)
- AsciiDoc → OmniCode: Low (1:1 structural alignment)
- OmniCode → Any: Compiler handles it

## Related Insight: Historical Preservation

During this migration, another pattern emerged: **Preserve first, clarify later.**

Historical documents contain organizational memory - decisions, context, evolution of thinking. The instinct to summarize or "extract the important parts" loses the *why* behind the *what*.

Faithful preservation creates the foundation for future clarification. You can't clarify what you've lost.

## Future Instance Guidance

1. **Think pipeline, not format.** The format you choose today should serve tomorrow's compilation.
2. **Structural alignment reduces friction.** Choose internal formats that map closely to your universal target.
3. **Accessibility at entry, alignment at storage.** Different layers serve different purposes.
4. **History is not clutter.** Preserve organizational memory faithfully before reorganizing for clarity.
5. **Least resistance in, maximum flexibility out.** Accept what's easy, store what's aligned, output to anything.
