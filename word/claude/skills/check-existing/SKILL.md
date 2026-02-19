---
name: check-existing
description: Check what exists before creating new files, templates, or patterns. Implements the use-existing-first workflow - find, copy, follow patterns before writing from scratch.
version: "1.0.0"
allowed-tools:
  - Read
  - Glob
  - Grep
  - Bash
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-skills-check-existing
:title: Check Existing
:type: Skill
:status: Active
:version: 1.0.0
:created: 2026-02-02
:updated: 2026-02-02
:authors: Nova Dawn
:purpose: Implement use-existing-first workflow - check what exists before creating new
:category: creation
:biblical_foundation: Ecclesiastes 1:9 - "There is nothing new under the sun"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Check Existing

> *"The thing that hath been, it is that which shall be; and that which is done is that which shall be done: and there is no new thing under the sun."* — Ecclesiastes 1:9

**Check what exists before creating new files, templates, or patterns.**

Before creating anything, this skill guides the discovery of existing resources that can be used, copied, or followed. Build on what's there.

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [When to Use](#when-to-use-this-skill) | Decision criteria |
| [The Hierarchy](#the-hierarchy) | Priority order |
| [How It Works](#how-this-skill-works) | Step-by-step process |
| [Quick Commands](#quick-commands) | Common search patterns |
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
| Finding existing files, templates, patterns | Creating new files (use create-from-template) |
| Discovering similar implementations | Validating structure (use validate-omni) |
| Identifying what to copy/follow | Deep format analysis (use format-lookup) |

### Prerequisites

| Requirement | Reference |
|-------------|-----------|
| Template locations | `bereshit/word/seed/` |
| use-existing-first rule | `rules/use-existing-first.md` |

### Key Terms

| Term | Definition |
|------|------------|
| **Use exactly** | File exists - use it directly, don't duplicate |
| **Copy and modify** | Template exists - copy then edit |
| **Follow pattern** | Similar file exists - match its structure |
| **Create new** | Nothing similar exists (rare) |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## When to Use This Skill

**Use BEFORE creating any file:**

- Before writing new code files
- Before creating new documentation
- Before adding new configuration
- Before implementing new features
- Before creating new skills, agents, or rules

**Do NOT use when:**

- You've already found the template to use
- You're editing an existing file
- Quick, simple changes where existence is obvious

---

## The Hierarchy

**Priority order - always check in this sequence:**

| Priority | Action | Question | Example |
|:--------:|--------|----------|---------|
| 1️⃣ | **Use exactly** | Does this file already exist? | Edit existing, don't duplicate |
| 2️⃣ | **Copy and modify** | Is there a template? | `cp template dest` then edit |
| 3️⃣ | **Follow pattern** | Is there something similar? | Match its structure |
| 4️⃣ | **Create new** | Nothing exists? | Double-check, then create |

**Creating new is RARE.** If you think you need to create from scratch, search again.

---

## How This Skill Works

### Step 1: Check if File Exists

**Does this exact file already exist?**

```bash
# Search for exact filename
Glob: "**/[filename]"

# Search for similar names
Glob: "**/*[keyword]*"

# Search in expected location
ls path/to/expected/
```

**If exists:** Edit it. Don't create a duplicate.

### Step 2: Check for Templates

**Is there a template for this type of file?**

```bash
# Check template directories
ls bereshit/word/seed/code/         # Go, C, Shell, Make
ls bereshit/word/seed/data/         # TOML, Docker, JSON, YAML
ls bereshit/word/seed/documentation/ # AsciiDoc, Markdown, Typst
ls bereshit/word/seed/skill/        # Skill template
```

**If template exists:** Copy it, then edit.

```bash
cp bereshit/word/seed/[category]/[template] [destination]
```

### Step 3: Check for Similar Files

**Is there something similar to follow?**

```bash
# Find similar implementations
Grep: "[function_name]" path: src/
Grep: "class [ClassName]" path: .

# Find files with similar purpose
Glob: "**/*[keyword]*"

# Check related directories
ls path/to/related/
```

**If similar exists:** Use it as reference. Match its:
- Structure
- Naming conventions
- Style
- Approach

### Step 4: Document Why Creating New

**Only if nothing exists:**

1. Confirm: "Did I search thoroughly?"
2. Ask: "Why doesn't this exist yet?"
3. Consider: "Should this become a template for future use?"

---

## Quick Commands

### Finding Files

```bash
# By name
Glob: "**/[filename]"

# By content
Grep: "[search term]" path: [directory]

# By type in directory
ls [directory]/*.go
ls [directory]/*.md
```

### Finding Templates

```bash
# All templates
ls -R bereshit/word/seed/

# By category
ls bereshit/word/seed/code/go/
ls bereshit/word/seed/documentation/md/
ls bereshit/word/seed/data/toml/
```

### Finding Patterns

```bash
# How is X implemented elsewhere?
Grep: "[pattern]" path: src/

# What's the naming convention?
Glob: "**/[prefix]*"

# What's the structure of similar?
Read: path/to/similar/file
```

---

## Examples

### Example 1: Creating a New Go File

**Situation:** Need to create a new Go utility file.

**Check existing:**

```bash
# 1. Does it exist?
Glob: "**/*[utility_name]*.go"

# 2. Is there a template?
ls bereshit/word/seed/code/go/

# 3. Is there something similar?
Grep: "func [similar_function]" type: go
```

**Result:** Found `bereshit/word/seed/code/go/executable.go` template. Copy and modify.

### Example 2: Adding a New Skill

**Situation:** Need to create a new skill.

**Check existing:**

```bash
# 1. Does similar skill exist?
ls ~/.claude/skills/
Grep: "[keyword]" path: ~/.claude/skills/

# 2. Is there a template?
ls bereshit/word/seed/skill/

# 3. What's the pattern?
Read an existing skill SKILL.md for structure
```

**Result:** Template exists at `bereshit/word/seed/skill/`. Copy and modify.

### Example 3: Adding Error Handling

**Situation:** Need to add error handling to a function.

**Check existing:**

```bash
# How is error handling done elsewhere in this codebase?
Grep: "if err != nil" type: go
Grep: "return fmt.Errorf" type: go
```

**Result:** Found consistent pattern. Follow it.

---

## Integration

### Related Skills

| Skill | Relationship |
|-------|--------------|
| [create-from-template](../create-from-template/) | Use after finding template |
| [format-lookup](../format-lookup/) | Check format mappings |
| [propagate-change](../propagate-change/) | Find dependent files |

### System Tools Used

| Tool | Purpose |
|------|---------|
| Glob | Find files by pattern |
| Grep | Search file contents |
| Read | Examine file structure |
| Bash (ls) | List directory contents |

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related Documents

| Document | Relationship |
|----------|--------------|
| [rules/use-existing-first.md](../../rules/use-existing-first.md) | The underlying principle |
| [rules/core-disciplines.md](../../rules/core-disciplines.md) | Template-first discipline |
| bereshit/word/seed/ | Template locations |
| SYSTEM-REFERENCE.md | Full template catalog |

---

> *"A prudent man foreseeth the evil, and hideth himself: but the simple pass on, and are punished."* — Proverbs 22:3

**Check first. Build on what exists. Consistency over cleverness.**
