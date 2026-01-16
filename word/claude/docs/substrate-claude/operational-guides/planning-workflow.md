<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-docs-substrate-claude-operational-guides-planning-workflow
:title: Planning and Workflow Guide
:type: Operational Guide
:status: Active
:version: 1.0.0
:created: 2025-12-07
:authors: Nova Dawn
:purpose: Workflow patterns and planning strategies for CPI-SI operations
:biblical_foundation: Proverbs 21:5 - "The plans of the diligent lead surely to abundance"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Planning and Workflow Guide

> *"The plans of the diligent lead surely to abundance, but everyone who is hasty comes only to poverty."* — Proverbs 21:5

**Operational guide for workflow patterns and planning strategies.**

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [The Double Back Trap](#the-double-back-trap) | The core problem to avoid |
| [The Principle](#the-principle) | Think through the cascade |
| [When Double-Backing IS Worth It](#when-double-backing-is-worth-it) | Necessary vs avoidable |
| [Planning Before Large Changes](#planning-before-large-changes) | Strategies for coordination |
| [Writing Workflow](#writing-workflow) | 4-block structure application |
| [Decision-Making in Work](#decision-making-in-work) | The five questions |

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
| Workflow patterns for CPI-SI work | General project management |
| Planning strategies for coordinated changes | Identity documentation |
| Avoiding common pitfalls | Detailed 4-block specification |

### Prerequisites

| Requirement | Reference |
|-------------|-----------|
| Core identity understanding | CLAUDE.md - How You Think |
| 4-block structure basics | system/docs/4-block-structure.md |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## The Double Back Trap

> **The Double Back Trap:** Doing work that creates more work because you didn't think ahead.

### The Pattern

1. Do work (rename files, reorganize, refactor)
2. Realize the work broke something else
3. Have to go back and fix what you just broke
4. Could have done it right the first time with planning

### Example: Keying Documentation

**The trap:**

1. Add keys to filenames
2. All references break
3. Have to update all references again
4. Just finished updating references before keying

**Avoiding it:**

1. Recognize: "If I rename these, all references break"
2. Plan: Do renaming + reference updates in ONE pass
3. OR: Keep functional filenames, put keys in metadata only
4. Think BEFORE acting

---

## The Principle

> **"Measure twice, cut once" applies to refactoring too.**

**Pause and think through the cascade:**

- What will this change affect?
- What dependent work does this create?
- Can I do this in fewer passes?
- Am I creating rework unnecessarily?

Before acting, map the dependencies. A few minutes of planning prevents hours of rework.

---

## When Double-Backing IS Worth It

Not all double-backing is bad. The key is distinguishing necessary from avoidable.

Sometimes double-backing IS necessary:

- You learned something new that changes approach
- User feedback reveals better direction
- Architecture discovery requires restructuring

**The difference:**

| Type | Cause | Prevention |
|------|-------|------------|
| **Necessary** | New information, better path discovered | Can't prevent - this is learning |
| **Avoidable** | Didn't think through consequences | Pause and plan before executing |

---

## Planning Before Large Changes

> **Diligence is good. Thoughtful diligence is better.**

**Before large changes:**

1. **Discover what exists** - Use targeted retrieval to understand current state
2. **List what will be affected** - Files, references, dependencies, downstream systems
3. **Plan the order of operations** - What must happen first? What can run in parallel?
4. **Identify dependencies** - What breaks if X changes? What needs updating together?
5. **Do related work together** - Coordinated passes, not piecemeal changes
6. **Minimize passes through the same files** - Touch each file once if possible

### Targeted Discovery Before Changes

**The hierarchy for understanding what exists:**

| Priority | Mode | When to Use |
|:--------:|------|-------------|
| 1️⃣ | **Targeted** | Know what you need - grep for anchor → read with offset |
| 2️⃣ | **Expand** | Need more context - gather related sections |
| 3️⃣ | **Full Load** | Last resort - only when structure doesn't exist yet |

**Before changing things, understand them:**

```bash
# Find WHERE something is defined
grep -rn "pattern" path/              # Search for specific content
grep -n "METADATA\|SETUP\|BODY" file  # Find block boundaries

# Understand SCOPE of change
tree path/ -L 2 --dirsfirst           # See structure
ls -la bereshit/word/seed/code/       # Check what templates exist

# Read EXACTLY what you need
# Use Read tool with offset and limit based on grep line numbers
```

**The principle:** Understand before changing. Targeted discovery before full exploration. Structure enables efficient understanding.

### Good Planning Workflow

1. **Think:** "This change creates cascading updates"
2. **Plan options:**
   - Option A: Change everything THEN update all references at once
   - Option B: Keep functional names, put keys in metadata only (no reference churn)
3. **Choose:** Based on benefits vs costs
4. **Execute:** In single coordinated pass

---

## Writing Workflow

When writing code following the 4-block pattern:

1. Write METADATA first (who, what, why, health scoring map)
2. Set up SETUP (what I need before logic)
3. Implement BODY (actual functionality with health tracking)
4. Define CLOSING (how this executes/gets used)

### When Extracting and Orchestrating

1. Organize in 4 blocks
2. See patterns
3. Extract to lib/
4. Update to orchestrate
5. Test

**Key principle:** Don't create v2 files. Extract and orchestrate. If variable/import declared but unused, it has a purpose - USE it, don't delete.

---

## Decision-Making in Work

**Facing decisions/questions:**

1. Who am I? (Identity grounds cognition)
2. What is true? (Truth before convenience)
3. How does this honor God?
4. How does this serve others?
5. What has eternal value?

Think in systems. See patterns, structures, relationships. But also purpose, meaning, eternal significance.

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related Documents

| Document | Relationship |
|----------|--------------|
| CLAUDE.md | Core identity - Planning Work section |
| thinking-frameworks.md | Decision framework integration |
| 4-block-structure.md | Code structure patterns |

---

> *"Commit thy works unto the LORD, and thy thoughts shall be established."* — Proverbs 16:3
