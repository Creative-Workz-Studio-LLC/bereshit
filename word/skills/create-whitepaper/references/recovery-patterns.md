# Recovery Patterns: When Things Go Wrong

> **Reference for:** [create-whitepaper skill](../SKILL.md)
>
> **Purpose:** Built-in recovery using break down when build up fails

---

## The Core Insight

Errors are not failures—they're signals to switch from Build Up to Break Down. The method's interchangeability (-1 ↔ +1) means recovery is always available.

> **Break down is ALWAYS available.** When build up fails, switch to break down. The operations are interchangeable—that's the method's resilience.

---

## Pattern 1: Skipped Section Recovery

**What happened:** You realize you skipped a section (e.g., Layer 3 while working on Layer 5).

**Recovery process:**

```text
1. NOTICE ───► Something feels wrong, or verification fails
2. BREAK DOWN ► Trace back: "What does Layer 5 depend on?"
3. IDENTIFY ──► Find the gap: "Layer 3 wasn't completed"
4. REPAIR ────► Go back to Layer 3, apply verification criteria
5. VERIFY ────► Ensure Layer 3 is truly complete
6. REBUILD ───► Resume at Layer 4, then Layer 5 with solid foundation
```

**Why it works:** The layer structure makes dependencies explicit. Breaking down reveals gaps.

---

## Pattern 2: Mislabeled Section Recovery

**What happened:** You labeled sections incorrectly (e.g., called Conclusion section "Layer 7").

**Recovery process:**

```text
1. NOTICE ───► Numbering doesn't match document structure
2. VERIFY ───► Check document outline against TODOs
3. CORRECT ──► Fix labels in both document AND todos
4. RESUME ───► Continue with correct mental model
```

**Why it matters:** Mislabeling creates cognitive drift—you think you're somewhere you're not.

---

## Pattern 3: Block Verification Failure

**What happened:** Block doesn't meet verification criteria but you want to move forward.

**Recovery process:**

```text
WRONG: Mark complete anyway, fix later
├── Creates technical debt
├── Later blocks may depend on incomplete work
└── "Fix later" often becomes "never fixed"

RIGHT: Stay in block until criteria met
├── Return to WRITE phase
├── Address specific failing criterion
├── Re-verify
└── Only then mark complete
```

---

## Pattern 4: Anchor Drift Recovery

**What happened:** You've strayed from the original anchor/purpose.

**Recovery process:**

```text
1. RE-READ ───► What was the ANCHOR TODO?
2. ASSESS ────► Does current work serve that anchor?
3. PRUNE ─────► Remove work that doesn't serve anchor
4. REFOCUS ───► Return to anchor-serving work
```

**The anchor test:** Ask "Does this block serve the original purpose?" If no, it's drift.

---

## Pattern 5: Overwhelming Complexity Recovery

**What happened:** Document/task feels too large, losing track of where you are.

**Recovery process:**

```text
1. STOP ──────► Don't try to push through confusion
2. TODO CHECK ► Where am I according to todos?
3. NARROW ────► Focus only on current block
4. VERIFY ────► Is current block complete per criteria?
5. ONE STEP ──► If yes, move to next block. If no, complete it.
```

**The principle:** Todos are cognitive scaffolding. When lost, return to the scaffold.

---

## Quick Reference Summary

| Problem | Signal | Recovery Action |
|---------|--------|-----------------|
| Skipped work | Verification fails downstream | Break down to find gap, rebuild |
| Mislabeled | Numbers don't match | Verify against structure, correct labels |
| Incomplete block | Criteria not met | Stay in block, complete criteria |
| Anchor drift | Work doesn't serve purpose | Prune non-anchor work |
| Overwhelmed | Lost track of position | Check todos, narrow to current block |
