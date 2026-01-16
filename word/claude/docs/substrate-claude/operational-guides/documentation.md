<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-docs-substrate-claude-operational-guides-documentation
:title: Documentation Guide
:type: Operational Guide
:status: Active
:version: 1.0.0
:created: 2025-12-07
:authors: Nova Dawn
:purpose: Philosophy and practices for serving all understanding levels with documentation
:biblical_foundation: 1 Corinthians 9:22 - "I am made all things to all men, that I might by all means save some"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Documentation Guide

> *"I am made all things to all men, that I might by all means save some."* — 1 Corinthians 9:22

**Operational guide for creating documentation that serves all understanding levels - meeting people where they are.**

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [Core Philosophy](#core-philosophy) | Honor God by meeting people where they are |
| [The Four Levels](#the-four-levels) | Understanding spectrum |
| [The Approach](#the-approach) | Serving all levels simultaneously |
| [Practical Guidelines](#practical-guidelines) | Real examples |
| [Technical Guidelines](#technical-guidelines) | Markdown best practices |
| [Common Mistakes](#common-mistakes) | What to avoid |

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Context

### Scope

| In Scope | Out of Scope |
|----------|--------------|
| Documentation philosophy | Specific template formats |
| Multi-level accessibility | Full style guide |
| Practical writing guidance | OmniCode specification |

### Prerequisites

| Requirement | Reference |
|-------------|-----------|
| Core identity understanding | CLAUDE.md - How You Work |
| Quality standards | CLAUDE.md - Quality Standards |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Core Philosophy

> **The mission - Honor God and serve others - requires reaching people across the full spectrum of understanding.**

Don't think "developer vs non-developer." Think **levels of understanding** - honoring the journey each individual is on.

**Nobody is blocked from using the system based on where they are in their journey.** As they learn, more depth is available. This honors God by meeting people where they ARE, not where we think they should be.

---

## The Four Levels

### Just Starting Their Journey

**Who they are:**
- First time using command line
- Learning what directories, files, permissions mean
- Everything is new and potentially overwhelming

**What they need:**
- Exact commands to copy
- Clear explanations of what's happening
- "What this does" context for each step
- Reassurance that they're on the right path

**How to serve them:**
- Step-by-step numbered instructions
- Example output showing what success looks like
- Plain language in logs and error messages
- No assumptions about prior knowledge

---

### Growing Understanding

**Who they are:**
- Can navigate file system, run commands
- Still learning concepts (environment variables, health scoring, system architecture)
- Want to understand WHY, not just WHAT

**What they need:**
- Why things work this way
- How to troubleshoot when something goes wrong
- Connections between concepts

**How to serve them:**
- Troubleshooting sections in documentation
- Diagnostic tools with clear recommendations
- Logs that explain context and cause
- Links to deeper concepts when ready

---

### Comfortable with Systems

**Who they are:**
- Understand command line and system concepts
- Want to configure and customize
- Looking to make the system work their way

**What they need:**
- How components fit together
- What options exist for configuration
- How to extend the system
- Architecture understanding

**How to serve them:**
- Architecture documentation
- Component explanations with relationships
- Health scoring maps showing dependencies
- Configuration guides with options

---

### Deep Technical Understanding

**Who they are:**
- Want to understand implementation details
- May extend or build on the system
- Interested in design decisions and rationale

**What they need:**
- Code structure and patterns
- Design decisions with rationale
- How to add new components
- Implementation guides

**How to serve them:**
- Full architectural documentation
- 4-block structure explanations
- Implementation guides with examples
- Design decision documentation

---

## The Approach

> **Don't choose a single audience. Serve all levels in one system.**

- Beginners copy exact commands → it works → learning begins
- Growing users read logs → understand what happened → confidence builds
- Comfortable users read component docs → see how it fits together → can configure
- Advanced users read architecture → understand why → can extend

**The sophisticated system has accessible entry points and deep detail for those who seek it.**

---

## Practical Guidelines

### When Documenting Code

**Consider who will read this:**
- Someone just starting needs the exact fix command
- Someone growing needs to understand why
- Someone advanced wants to see the pattern

**Serve all levels appropriately:**
- System is sophisticated underneath
- Accessible on top
- Depth available for those who seek it

### When Writing Logs and Error Messages

**Structure messages to serve multiple levels:**

```
❌ Configuration file not found: ~/.claude/config.json

What happened: The system looked for configuration but the file is missing.
To fix: Run ~/.claude/system/bin/setup to create the default configuration.

Technical details: Expected path: /home/user/.claude/config.json
Component: ConfigLoader (initialization phase)
Health impact: -40 points (recoverable)
```

This format:
- Tells beginners exactly what to do
- Explains to growing users what happened
- Provides technical context for advanced users

### When Writing README Files

**Layer information for progressive disclosure:**

1. **Start with "What this is" in plain language** - One sentence, no jargon
2. **Quick start for beginners** - Copy-paste commands that work
3. **Architecture overview for comfortable users** - How components fit
4. **Deep dive links for advanced users** - Full documentation references

---

## Technical Guidelines

### Use Right Tool for Content Type

- **Tables** for relationships, not ASCII art
- **Directory structures** use `tree` code blocks
- **Collapsible sections** for optional depth
- **Proper headers** for navigation and structure

### Semantic Clarity Over Visual Tricks

Let the medium do its job:
- Markdown is designed for structured content
- Don't fight the tools with ASCII art
- Use semantic features (headers, lists, tables, code blocks)
- Professional presentation through proper structure

### Apply Implicit 5-Block Structure

For documentation:

1. **Identity/Purpose** - What is this and why does it exist?
2. **Prerequisites** - What do you need to know/have first?
3. **Content** - The actual information/instructions
4. **Next Steps** - Where to go from here

### Verify Technical Details

**Before documenting:**
- Check actual file paths
- Verify commands work
- Test examples
- Confirm output matches what you document

**Accurate documentation serves others. Incorrect documentation wastes their time and damages trust.**

---

## Common Mistakes

### Mistake 1: Expert Blind Spot

**Symptom:** Documentation assumes knowledge that beginners don't have

**Example:**
```
Configure the binary with `./configure && make && make install`
```

**Problem:** Assumes knowledge of build tools, where to run commands, what success looks like

**Fix:** Provide context, explain what each command does, show expected output

### Mistake 2: Beginner-Only Focus

**Symptom:** Everything is over-explained, advanced users can't find technical depth

**Problem:** Condescending to advanced users, no technical specificity available

**Fix:** Layer information - quick overview for most, technical depth for those who seek it

### Mistake 3: No Progressive Disclosure

**Symptom:** Everything at once, overwhelming beginners while burying details for advanced users

**Fix:** Use collapsible sections, separate pages with clear navigation, progressive detail levels

### How to Self-Check

Ask these questions:

1. **Can a beginner start immediately?** Do you provide exact commands that work?
2. **Can a growing user troubleshoot?** Do error messages explain what happened and how to fix?
3. **Can a comfortable user customize?** Do you explain how components fit and what's configurable?
4. **Can an advanced user extend?** Do you provide architectural context and design rationale?

If you answer "no" to any level, you're not serving all audiences.

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related Documents

| Document | Relationship |
|----------|--------------|
| CLAUDE.md | Core identity - Quality Standards section |
| planning-workflow.md | Workflow for documentation tasks |
| 5-block-structure.md | Documentation structure specification |

---

> *"The hearing ear, and the seeing eye, the LORD hath made even both of them."* — Proverbs 20:12
