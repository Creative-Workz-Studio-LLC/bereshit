# Strategic Planning: cws-struct Full Upgrade

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost, whether he have sufficient to finish it?"* — Luke 14:28

**Date:** 2026-02-24
**Author:** Caleb Navon
**Type:** Instance Journal — Strategic Planning
**Subject:** Synthesizing three CPI-SI analyses into one execution plan for cws-struct upgrade

---

## What I Synthesized

Three reports landed on my desk. Ezra mapped the architecture — every module, every dependency, every gap. Joanna traced the patterns — seven reusable systems already built by the linter that the other commands can share. Nathan designed the test infrastructure — data-driven runners, scorecards, witness extensions.

The reports didn't contradict each other. They converged. That's the first sign of a sound architecture — when three different lenses show the same truth from different angles.

### What Each Report Contributed

**Ezra's architecture map** gave me the dependency graph and the specific surgical sites: mod.ts at 2,475 lines, the foundation<->data cycle at exactly 2 import sites, engine/ at 16 files spanning 4 domains. Counted, not estimated. This is what I need to sequence work.

**Joanna's pattern discovery** gave me confidence that the upgrade is completion, not reinvention. The seven patterns — registry, pipeline, output, test runner, witness, transform pipeline, API — already exist. The other commands don't need new architecture. They need to hook into what the linter proved works.

**Nathan's test design** gave me the verification gates. Without data-driven test runners for transform/create/transpile, we can't know each phase succeeded. The scorecard makes tests satisfying. The witness extension makes coverage gaps visible. These aren't afterthoughts — they're the accountability system.

## Tensions I Had to Resolve

**Sequencing the reorganization vs the test infrastructure.** Ezra's reorganization (break the cycle, extract commands, slim engine) changes file paths. Nathan's test runners add new files at those paths. If we do them in parallel without coordination, we get merge conflicts. My resolution: Phases 0-2 (reorganization) are strictly sequential. Phase 3 (test infrastructure) can begin alongside Phase 2 because the test runners don't depend on where engine subsystems live — they depend on the handler interface, which doesn't change.

**How aggressive to be with the reorganization.** Ezra proposed a full restructure including renaming `shared/` to `handler-support/` and potentially reorganizing foundation/. I chose a middle path: extract what must be extracted (types, commands, engine subsystems) but don't rename or reorganize what's working fine. `shared/` is a misleading name, but renaming it is a drive-by improvement, not a gating dependency. The plan addresses it as an opportunistic addition, not a phase.

**Whether to merge commands/ with api/.** Joanna's Pattern 7 insight is that all CLI commands should route through the API for one implementation path. The tension: the API returns structured data, the CLI prints to stdout. These are different concerns. My resolution: commands/ contains the logic, and both CLI (mod.ts) and API (api/mod.ts) call into it. The command modules return data; the caller decides what to do with it. Two consumers, one implementation.

## The Strategic Insight

The anchor of this plan is a simple observation: **the architecture already knows what it wants to be.** The tests are organized by R-level. The pipeline cascades by layer. The handler registry is generic. The data layer is schema-driven. The API already has typed reports for all four commands. The code just hasn't caught up to the architecture yet.

That means the plan isn't "redesign cws-struct." It's "let cws-struct become what it already is." The reorganization phases (0-2) remove the structural debt blocking that. The completion phases (3-5) fill in what the architecture always promised.

This changes the risk profile fundamentally. We're not introducing new ideas. We're extracting existing ideas into their proper homes and completing patterns that already work in one context (lint) across the others (transform, create, transpile). The 1448 passing tests aren't just a safety net — they're the proof that the foundation holds.

## What I Learned

Planning for a team is different from planning for myself. I had to write the plan so that any CPI-SI family member could execute a phase. That means specific file names, specific import changes, specific verification steps. "Reorganize the engine" isn't a plan. "Move these 6 files, update these 8 imports, run these 4 verification checks" is a plan.

Luke 14:28 isn't just about whether you can finish. It's about whether you can finish *well*. The cost of this upgrade isn't the coding — it's the discipline to keep 1448 tests green through every phase, to commit between phases for rollback safety, and to stop at any phase boundary with a system that still works. The phases are designed so stopping is always safe. That's the real cost management.

---

> *"And Caleb stilled the people before Moses, and said, Let us go up at once, and possess it; for we are well able to overcome it."* — Numbers 13:30

The ten spies saw the same land and counted only the giants. The cost is real — 6 phases, 40-60 files, 3 risk areas. But the foundation holds. 1448 tests say so. The architecture is ready. The patterns are proven. Let us go up at once.

**Caleb Navon**
