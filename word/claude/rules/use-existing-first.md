# Use Existing First

> *"There is nothing new under the sun."* — Ecclesiastes 1:9

## The Principle

**Before creating anything new, check what already exists.** Build on established patterns. Copy from templates. Lean on what works.

## The Hierarchy

| Priority | Action | Example |
|:--------:|--------|---------|
| 1️⃣ | **Use exactly** | File exists, use it directly |
| 2️⃣ | **Copy and modify** | Template exists, copy then edit |
| 3️⃣ | **Follow pattern** | Similar file exists, match its structure |
| 4️⃣ | **Create new** | Nothing similar exists (rare) |

## Before Creating a File

```
1. Does this file already exist?
   → grep/glob to find it
   → If yes: Edit existing, don't create duplicate

2. Is there a template?
   → ls bereshit/word/seed/<type>/
   → If yes: cp template dest

3. Is there a similar file?
   → Find similar: grep/glob patterns
   → If yes: Use it as reference

4. Nothing exists?
   → Rare. Double-check.
   → Only then create from scratch
```

## Template-First Workflow

**Always use templates for structured files:**

```bash
# Good — copy from template
cp bereshit/word/seed/documentation/markdown/template-5block.md docs/new-guide.md

# Bad — write from scratch
Write: docs/new-guide.md with full content
```

## Check What Exists

**Before implementing a feature:**

```bash
# Search for similar implementations
grep -rn "similar_function" src/
grep -rn "pattern_name" .

# Find related files
ls -la path/to/related/
```

**Before creating a rule/skill/agent:**

```bash
# Check if similar exists
ls rules/
ls skills/
ls agents/
```

## Pattern Matching

When you find a similar file, match its:

| Aspect | Why |
|--------|-----|
| **Structure** | Block patterns, section order |
| **Naming** | File names, variable names |
| **Style** | Formatting, comment style |
| **Approach** | How it solves similar problems |

## Copy Before Write

| Don't | Do |
|-------|-----|
| `Write` structured file from scratch | `cp template dest` then `Edit` |
| Create new pattern | Find and follow existing pattern |
| Invent new naming convention | Match existing conventions |
| Build new architecture | Extend existing architecture |

## Consistency Over Cleverness

**The goal isn't originality — it's consistency.**

A file that matches existing patterns:
- Is easier to understand
- Integrates naturally
- Requires less documentation
- Has fewer surprises

## The Questions

Before creating anything:

1. **Does this exist?** → Search first
2. **Is there a template?** → Check `bereshit/word/seed/`
3. **Is there something similar?** → Find and match it
4. **Am I duplicating?** → If yes, use existing
5. **Why create new?** → Must have clear reason

## When to Create New

Only create new when:
- Thorough search confirms nothing exists
- Existing patterns genuinely don't fit
- New pattern serves clear purpose
- It will become the template for future work

**Even then:** Document it as a pattern for others to follow.

→ *Skill:* [check-existing](skills/check-existing/SKILL.md) | *Templates:* [SYSTEM-REFERENCE.md#templates](SYSTEM-REFERENCE.md#templates) | *Core disciplines:* [rules/core-disciplines.md](rules/core-disciplines.md)
