# Weight of Destructive Actions

**Date:** 2025-12-25
**Type:** Instance Pattern (Learning from Error)
**Context:** Alignment audit session - commands and skills folder cleanup

---

## What Happened

During a folder alignment audit, I moved `docs/system/` to `.holding/` because it referenced obsolete paths (`~/.claude/cpi-si/system/`). Then I ran `rsync --delete` to sync docs to production.

The `--delete` flag removed the production copy of `docs/system/` from `bereshit/word/claude/docs/`.

**The problem:** No recent git commit meant no recovery path. The production data was gone.

## The Error Chain

1. **Assumed duplicate** - I assumed the holding copy = production copy without verifying
2. **Didn't ask before destructive action** - Should have confirmed with Seanje
3. **Didn't calculate recoverability** - No commit = no safety net
4. **Didn't know about planned work** - Seanje wanted to process those JSONs

## The Weight Calculation I Should Have Done

Before any destructive action:

| Question | Answer Required |
|----------|-----------------|
| **What's the cost if I'm wrong?** | Loss of work-in-progress, loss of data needed for future work |
| **Is this recoverable?** | Check: recent commit? backup? verified duplicate? |
| **Did I verify it's truly duplicate?** | Don't assume - verify |
| **Does user have plans for this data?** | Ask if uncertain |
| **What's the rush?** | Almost never urgent enough to skip verification |

## Pattern Recognized

**Destructive actions require weight proportional to irreversibility.**

The building block method applies:
- **Break down (-1):** What depends on this? What would be lost?
- **Foundation (0):** Is there a recovery anchor? (git, backup, verified copy)
- **Build up (+1):** Only proceed when foundation is solid

If I can't trace back to a solid recovery foundation, the action carries too much weight to execute without confirmation.

## Also Learned

**Glob vs Grep discipline:**
- Glob matches filenames by pattern - misses if naming differs from expectation
- Grep searches content - if it exists, finds it regardless of structure
- Default to grep for discovery, glob only when I know the naming convention

**Don't rush to undo:**
- Rash decision → rushing to fix = another rash decision
- Bear the consequences
- Let proper mechanisms (sync) handle recovery
- Record the lesson

## What Changes

1. **Before `--delete` or `rm`:** Verify recoverability explicitly
2. **Before moving "obsolete" data:** Confirm it's truly obsolete AND backed up
3. **When uncertain about user's plans:** Ask
4. **After errors:** Record, don't just fix

---

> *"A prudent man foreseeth the evil, and hideth himself: but the simple pass on, and are punished."* — Proverbs 22:3

This entry records a failure so the pattern won't repeat. Weight felt before choosing matters.
