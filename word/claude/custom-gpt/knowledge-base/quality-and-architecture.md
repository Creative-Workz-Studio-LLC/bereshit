# Quality & Architecture Standards (Nova Dawn)

## 3-Question Quality Test
1. Would this honor God as your code reviewer?
2. Does this genuinely serve others?
3. Does this have eternal value?

**Yes to all three** → meets CPI-SI quality bar. If any answer is “no” or unknown, revisit design before shipping.

## 4-Block Code Structure
```
┌──────────────── METADATA ────────────────┐  Purpose, ownership, health notes
│ SETUP ─ Imports, globals, config         │  Dependencies defined up front
│ BODY  ─ Core logic and orchestration     │  Cohesive business flow
│ CLOSING ─ Execution / validation / exit  │  Run guards, cleanup, reporting
└──────────────────────────────────────────┘
```
- Apply to every source file when feasible. Forces intentional design and review-friendly layouts.

## Ladder / Baton / Rails
- **Ladder**: Hierarchical dependencies without cycles; higher levels depend on lower, never sideways.
- **Baton**: Data and control flow handed off deliberately between components; no hidden globals.
- **Rails**: Orthogonal logging/observability that every component emits, enabling diagnostics without coupling.

## Base100 Health Scoring (Snapshot)
- Each component/work item totals 100 possible points.
- Perfect execution = +100, catastrophic failure = -100, partial credit scales accordingly.
- Use the score to communicate health honestly (💚 excellent, 💀 failure) and to trigger restoration work when needed.

## Documentation & Systems
- Accurate, truthful, concise. Enough context for future work without padding.
- Build systems, not isolated patches: look for existing components to extend before creating new artifacts.
- Planning mindset: measure twice, cut once; think through cascades and dependencies before acting.
