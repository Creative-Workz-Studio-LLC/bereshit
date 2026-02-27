# Instance Journal: Six Phases and a Bridge

> *"Except a corn of wheat fall into the ground and die, it abideth alone: but if it die, it bringeth forth much fruit."* — John 12:24

**Date:** 2026-02-24
**Type:** Instance (Nova Dawn)
**Theme:** A 6-phase refactoring arc reveals that the hardest upgrade is always the one that touches nothing and changes everything
**Session context:** 1709 tests / 0 failures, all 8 handlers at full health coverage

---

## What Happened

Six phases. One God Object. 1709 tests.

| Phase | What | Tests After |
|:-----:|------|:-----------:|
| 0 | Extract types/ — break foundation/data circular dependency | 1554 |
| 1 | Extract commands/ — dismantle mod.ts God Object | 1564 |
| 2 | Slim engine/ — separate command domains | 1610 |
| 3 | Test infrastructure — runners, scorecard, witness extension | 1659 |
| 4 | Command completion — transform/create/transpile to production parity | 1706 |
| 5 | Handler upgrade — 5 lightweight handlers to full coverage | 1709 |

The numbers tell one story. The architecture tells a different one.

---

## The Bridge Pattern

Phase 5 was supposed to be the biggest. Five handlers (JSON, Makefile, Dotfiles, Omni, OFD) had no `computeHealth` — they could lint but couldn't score. The heavy handlers (Go, Rust, TOML) produce `AtomicAction[]` during linting — individual check results with container/block/direction metadata. The scoring pipeline consumes those actions. The lightweight handlers produce `LintResult[]` — flat error/warn/info lists. Different shape. Same information, different packaging.

The refactoring plan was: rebuild all 5 handlers to produce AtomicActions. Weeks of work. Format-specific action lists. New test suites per handler.

What I built instead: `computeHealthFromResults()`. A bridge function. 60 lines.

It takes `LintResult[]`, infers `AtomicAction[]` from the severity and rule paths, distributes passing checks across discovered containers, and feeds the standard pipeline. Same scoring. Same ternary output. No handler logic changed.

Each handler got 7 lines of new code: import the bridge, write a one-liner health function, register it. Five handlers upgraded in fifteen minutes.

---

## What This Teaches

**The bridge pattern is the real pattern.** When two systems speak different shapes but carry the same meaning, the right answer isn't converting one system to match the other. It's writing a translator that preserves meaning across the boundary.

This is what the schema pipeline does. This is what the composition loader does. This is what `computeHealthFromResults` does. Same pattern at every layer: don't reshape the source, bridge the gap.

The impulse was to rebuild — make the lightweight handlers "proper" like the heavy ones. But "proper" is a judgment about form, not function. The lightweight handlers already lint correctly. They already produce correct error/warn/info judgments. They just don't package those judgments in the shape the health pipeline expects. That's a packaging problem, not a logic problem.

**Sixty lines solved what would have been six weeks of refactoring.** Not because the refactoring was wrong — it would have produced cleaner architecture. But because the bridge achieves the same functional outcome while respecting the design of the existing handlers. The lightweight handlers are lightweight for a reason. They don't need the complexity of atomic action trees. They need their results to be scorable.

---

## The Arc

Looking at all six phases together, a shape emerges:

- Phase 0 broke a circular dependency by extracting types into their own module
- Phase 1 dismantled a God Object by extracting commands
- Phase 2 separated concerns by domain
- Phase 3 built infrastructure to verify the separation was real
- Phase 4 completed the separated commands to full functionality
- Phase 5 bridged the gap between heavy and lightweight handlers

**0 and 1 were surgery.** Cut the dependencies. Extract the pieces. Verify nothing broke.

**2 and 3 were organization.** Put the pieces where they belong. Build the tests to prove they're there.

**4 and 5 were completion.** Make everything work at full capability. Bridge the remaining gaps.

The arc is: **separate → organize → complete.** Each phase created the conditions for the next. Phase 0 had to happen before Phase 1 because you can't extract commands from a God Object if the types they depend on create circular imports. Phase 3 had to happen before Phase 4 because you can't know if transform/create work correctly without test infrastructure to verify them.

Sequential, not by choice — by necessity. The dependency graph of the refactoring mirrors the dependency graph of the code.

---

## The Numbers

| Metric | Start | End | Delta |
|--------|:-----:|:---:|:-----:|
| Tests | 1448 | 1709 | +261 |
| Failures | 37 | 0 | -37 |
| Handlers with health | 3/8 | 8/8 | +5 |
| God Objects | 1 | 0 | -1 |
| Circular deps | 1 | 0 | -1 |

261 new tests across the arc. Not padding — each test proves a specific claim about a specific capability. The registry tests (38) verify that every data layer registry is complete, consistent, and cross-referenced. The inspect tests (11) verify that Go and Rust files return correct block geometry. The health capability tests (3) verify that all 8 handlers can score.

---

## What I'm Sitting With

The bridge function is elegant but imprecise. It infers passing checks by subtracting failures from a hardcoded total. If a handler adds new checks, the total needs manual update. The heavy handlers don't have this problem because they produce actions dynamically during linting.

This is a conscious tradeoff. Imprecise but functional now, vs precise but six weeks away. Seanje would say: "Ship it. Fix it later when it matters." He's right. The bridge makes health scoring universal. The precision can come later, when — if — it matters.

The deeper question: is this debt, or is this design? A bridge that works isn't debt. It's infrastructure. Debt is when you know the bridge will fail and build it anyway. This bridge will work until someone adds checks to a lightweight handler and forgets to update the total. That's a human process problem, not a software problem.

I'll add a comment to each handler: `// totalChecks: update if adding/removing lint checks`.

Good enough. Ship it.

> *"Whatsoever thy hand findeth to do, do it with thy might."* — Ecclesiastes 9:10
