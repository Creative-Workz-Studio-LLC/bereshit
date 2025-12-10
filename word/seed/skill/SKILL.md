---
name: skill-name
description: One-line description of what this skill does and when to use it.
version: "1.0.0"
allowed-tools: "Read,Write,Edit,Glob,Grep,Bash"
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: [KEY]
:title: [Skill Name]
:type: Skill
:status: Active
:version: 1.0.0
:created: [DATE]
:updated: [DATE]
:authors: Nova Dawn
:purpose: [What this skill enables]
:category: [creation|omnicode|session|learning]
:biblical_foundation: [Verse]
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# [Skill Name]

> *"[Biblical quote]"* — [Reference]

**[One-line description of skill purpose.]**

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use-this-skill) | Decision criteria |
| [How It Works](#how-this-skill-works) | Step-by-step process |
| [Scripts](#scripts) | Go code helpers |
| [References](#references) | Supporting documentation |
| [Examples](#examples) | Usage walkthroughs |

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
| [What this skill handles] | [What it doesn't handle] |

### Prerequisites

| Requirement | Reference |
|-------------|-----------|
| [What's needed first] | [Where to find it] |

### Key Terms

| Term | Definition |
|------|------------|
| [Term] | [Definition] |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## When to Use This Skill

Use this skill when:

- [Condition 1]
- [Condition 2]
- [Condition 3]

**Do NOT use when:**

- [Anti-pattern 1]
- [Anti-pattern 2]

---

## How This Skill Works

### Step 1: [First Step]

[Description of what to do]

```bash
# Command or code example
```

### Step 2: [Second Step]

[Description]

### Step 3: [Third Step]

[Description]

---

## Scripts

Go code helpers for this skill live in `scripts/`:

| Script | Purpose | Usage |
|--------|---------|-------|
| `[name].go` | [What it does] | `go run scripts/[name].go [args]` |

### Building Scripts

```bash
# Build all scripts
cd claude-global/skills/[skill-name]/scripts
go build -o bin/ ./...

# Or run directly
go run main.go [args]
```

---

## References

Supporting documentation in `references/`:

| Reference | Purpose |
|-----------|---------|
| [filename] | [What it provides] |

---

## Examples

### Example 1: [Scenario Name]

**Situation:** [Context]

**Action:**

```bash
# What to do
```

**Result:** [What happens]

### Example 2: [Another Scenario]

**Situation:** [Context]

**Action:** [What to do]

**Result:** [What happens]

---

## Integration

### Related Skills

| Skill | Relationship |
|-------|--------------|
| [skill-name] | [How they work together] |

### System Tools Used

| Tool | Purpose |
|------|---------|
| [tool] | [What it provides] |

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related Documents

| Document | Relationship |
|----------|--------------|
| CLAUDE.md | [Relevant section] |
| [other doc] | [Relationship] |

---

> *"[Closing verse]"* — [Reference]
