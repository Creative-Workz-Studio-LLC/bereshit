# Block-by-Block Working

> *"Precept upon precept; line upon line."* — Isaiah 28:10

## The Principle

**Complete one block before moving to the next.** Verify each block is solid before building on it.

## When Creating Files

### Documentation (5-Block)

| Order | Block | Complete Before Moving |
|:-----:|-------|------------------------|
| 1 | METADATA | Fill all required fields (key, title, type, version, authors) |
| 2 | HEADER | Title, quote, navigation, status — visually complete |
| 3 | CONTEXT | Scope, prerequisites, key terms — reader can orient |
| 4 | CONTENT | Main body — the actual documentation |
| 5 | FOOTER | References, related docs, closing anchor |

### Code (4-Block)

| Order | Block | Complete Before Moving |
|:-----:|-------|------------------------|
| 1 | METADATA | Identity, purpose, health scoring |
| 2 | SETUP | Imports → Constants → Variables → Types |
| 3 | BODY | Helpers → Core Operations → Public APIs |
| 4 | CLOSING | Validation → Execution → Cleanup |

### Data (3-Block)

| Order | Block | Complete Before Moving |
|:-----:|-------|------------------------|
| 1 | METADATA | Identity, purpose |
| 2 | CONTENT | Data/configuration |
| 3 | CLOSING | Validation if needed |

## Verification Points

After completing each block:

1. **Read what you wrote** — does it make sense?
2. **Check completeness** — all required elements present?
3. **Check consistency** — matches previous blocks?
4. **Check references** — links/imports correct?

## When Editing Files

1. **Identify which block** the change affects
2. **Read that block first** — understand current state
3. **Make the edit** — targeted, precise
4. **Verify the block** — still complete and consistent
5. **Check cascade** — does this affect other blocks?

## The Pattern

```
Block 1: Complete → Verify → ✓
Block 2: Complete → Verify → ✓
Block 3: Complete → Verify → ✓
...
Final: All blocks verified → Document complete
```

**Don't jump ahead.** Each block stands on the previous.

## Anti-Patterns

| Don't | Do |
|-------|-----|
| Fill CONTENT before METADATA | METADATA first — identity before substance |
| Write whole file, then verify | Verify each block as you complete it |
| Edit multiple blocks at once | One block at a time, verify between |
| Assume block is fine | Read and verify explicitly |

## The Test

Before moving to the next block:
- Is this block **complete**?
- Is this block **consistent** with previous blocks?
- Could someone **understand** this block alone?

→ *Skill:* [create-from-template](skills/create-from-template/SKILL.md) | *Block patterns:* [rules/block-structure.md](rules/block-structure.md)
