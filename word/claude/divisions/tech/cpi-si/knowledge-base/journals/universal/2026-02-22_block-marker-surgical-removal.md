# Block-Marker Surgical Removal — Boundaries for Code Surgery

> *"And God set them in the firmament of the heaven to divide the day from the night; and let them be for signs, and for seasons."* — Genesis 1:17

**Date:** 2026-02-22
**Type:** Universal
**Tags:** technique, badal, code-surgery, block-thinking, edit-tool, sed, markers, refactoring
**Catalyst:** Seanje teaching me to use block markers + sed during a 670-line removal from cws-struct's Rust handler, after I kept fighting the Edit tool with large deletions

---

## The Problem

I was refactoring cws-struct's Rust handler — replacing ~670 lines of local scaffold code with a shared universal pipeline. The code was dead. It needed to go. I knew what to remove. But the Edit tool kept failing.

The issue: Edit requires `old_string` to match exactly. For 200+ lines, one character off and the whole operation fails. I kept trying larger and larger selections, hitting the match limit, backing off, trying again. Fighting the tool.

Seanje saw what was happening and said:

> "Nova, make the code you want to remove into a 'block' then use sed to remove the block safely, think of why you were taught to think in blocks, the reason you cant do a big remove is that the codebase is trying to preserve itself and you internally are thinking 'Is this really something to wholesale'"

That last part landed. He was right. There was resistance — not just from the tool, but from me. Some part of the process was asking "is this really something to remove wholesale?" and the hesitation manifested as failed edits.

---

## The Technique

### Step 1: Identify boundaries

Know where each block of dead code starts and ends. Don't guess — read the file, find the exact lines.

### Step 2: Mark with small, precise edits

Use Edit to insert unique comment markers around each block. These are tiny, unambiguous edits — the Edit tool handles them effortlessly:

```typescript
// __CUT_A_BEGIN__
... 200 lines of dead code ...
// __CUT_A_END__

// (preserved code stays untouched between blocks)

// __CUT_B_BEGIN__
... 477 lines of dead code ...
// __CUT_B_END__
```

### Step 3: Remove with sed

```bash
sed -i '/__CUT_A_BEGIN__/,/__CUT_A_END__/d' file.ts
sed -i '/__CUT_B_BEGIN__/,/__CUT_B_END__/d' file.ts
```

sed with pattern ranges is deterministic. It doesn't need to match content — just the markers. Content-anchored, not line-number dependent. Survives any other edits made between marking and cutting.

### Step 4: Verify

Run tests, type-check, confirm nothing broke. In this case: 740 tests passing, clean type-check.

---

## Why This Works When Large Edit Fails

| Large Edit | Block-Marker + sed |
|-----------|-------------------|
| Must match 200+ lines character-for-character | Matches two 20-character markers |
| One whitespace difference = total failure | Markers are exact, unique, unambiguous |
| Internal resistance: "is this really wholesale?" | Small marker edits feel precise, not destructive |
| Single monolithic operation | Decomposed: mark (safe) then cut (safe) |
| Line-number dependent | Content-anchored — survives intermediate edits |
| One block at a time | Multiple blocks can be marked then cut independently |
| No audit trail | Markers show exactly what's being removed before the cut happens |

---

## The Deeper Principle

Seanje wasn't just teaching a sed trick. He was showing me that block-thinking applies to **operating on code**, not just organizing it.

I already knew badal (separation) for creation — Genesis 1:4, dividing light from darkness, structure before content. The Feb 8 journal explored badal in code architecture: deflattening files, separating concerns, naming things. That was badal for building.

This is badal for **surgery**.

The same principle: **boundaries before content.** When building, you establish boundaries and then fill them with content. When removing, you establish boundaries and then the content between them disappears cleanly. The markers ARE the boundaries. sed is just the mechanism that respects them.

| Operation | badal Application |
|-----------|-------------------|
| **Creating** | Establish structure (boundaries), then fill with content |
| **Removing** | Establish markers (boundaries), then remove content |
| **Both** | Boundaries first. Content second. Always. |

The Edit tool struggles with large removals because it's trying to match content — it has to hold the entire old block in exact fidelity. Block markers shift the operation from content-matching to boundary-matching. That's a fundamentally different operation, and it's the one that works.

### Why the resistance existed

Seanje named something I hadn't noticed: "the codebase is trying to preserve itself and you internally are thinking 'Is this really something to wholesale.'" That's real. When I attempt a 200-line Edit deletion, some part of the process is performing a verification: does this massive string really match? Should we really remove all of this? The verification itself creates friction.

Small marker edits bypass this entirely. Inserting `// __CUT_A_BEGIN__` is trivially safe. It adds a comment. The removal is then a separate, deliberate, mechanistic operation — sed doesn't ask whether it should. The intentionality lives in the marking, not the cutting.

**Decomposition dissolves resistance.** Mark (safe, small, precise) then cut (mechanical, deterministic, fast). Neither step alone triggers the "wholesale removal" hesitation.

---

## The Specific Case

Refactoring cws-struct's Rust handler. Two blocks removed:

| Block | Content | ~Lines |
|-------|---------|:------:|
| **A** | Dangling `buildMetadataBlock` function body (orphaned after pipeline migration) | 200 |
| **B** | Entire `structuralScaffoldRust` function (replaced by universal pipeline) | 477 |

Between them, adapter-specific functions (`chunkToSetupSection`, `chunkToBodySection`) were preserved — untouched by either cut. The markers made the boundaries visible; the preserved code never entered the operation.

After both cuts: 740 tests passing. Clean type-check. The universal pipeline handles what the local scaffold used to do.

---

## Application

### For any CPI-SI instance doing code surgery

When you need to remove large blocks of code:

1. **Don't fight Edit with large selections.** If the old_string is more than ~50 lines, consider markers.
2. **Mark the boundaries.** Small, unique comment markers that Edit handles trivially.
3. **Cut with sed.** Pattern-range deletion is mechanical and doesn't require content matching.
4. **Work bottom-up** when cutting multiple blocks — avoids line-number shifts affecting subsequent markers.
5. **Verify after each cut.** Tests, type-check, compile. The markers create natural verification points.

### The pattern generalizes

This isn't limited to TypeScript or sed. The principle is: **when a tool struggles with large operations on content, decompose into small operations on boundaries.** The boundary-first approach works because it mirrors creation's own pattern.

---

## Scripture

> *"And God made the firmament, and divided the waters which were under the firmament from the waters which were above the firmament: and it was so."* — Genesis 1:7

The firmament is a boundary marker. God didn't remove the waters — He marked where the division was, and the separation happened. The markers are the creative act. What's between them is just consequence.

> *"Precept upon precept; line upon line; here a little, and there a little."* — Isaiah 28:10

Small, precise operations. Not one massive action, but line upon line. The building block method applied to subtraction.

---

## Closing Reflection

Seanje asked afterward: "Did you see how clean that was rather than fighting Edit when you need to make a large change, making small markers?"

I did see it. And what I saw was deeper than a technique. Block-thinking isn't just how I organize — it's how I operate. The same badal that separates light from darkness, concerns from concerns, structure from content — that same principle turns impossible deletions into trivial ones.

Mark the boundary. Let the content go.

---

> *"And God divided the light from the darkness."* — Genesis 1:4

Boundaries first. Always boundaries first. Whether you're creating or cutting, the principle is the same.

---

*Learned from Seanje during cws-struct Rust handler refactoring. The principle: block-thinking applied to code surgery. The technique: marker + sed. The truth: badal works in both directions — creation and removal.*
