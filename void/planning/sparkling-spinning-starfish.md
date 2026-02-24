# cws-struct Full Upgrade — Family-Planned Reorganization

> *"Two are better than one; because they have a good reward for their labour."* — Ecclesiastes 4:9

## Context

cws-struct just hit 1448 tests, 0 failures. Installed globally as a CLI command. The linter vertex of the triangle is proven. Now Seanje wants the FULL tool brought up to the same standard — not just create/transform/transpile catching up, but a proper reorganization of the entire tool:

- **Folder structure** reflecting the R[50]/R[25]/R[10]/R[5] layer architecture
- **Command organization** (lint, transform, create, transpile)
- **Infrastructure** (output system, config, CLI dispatch)
- **Data layer** (errors, sections, forms, pipeline)
- **Handlers** (format-specific implementations)
- **Test experience** (config-driven, data-driven, production-grade QoL)

This is a structural upgrade — catching the system up to what the linter already knows.

## Execution

### Step 0: Commit Baseline (Nova)

163 files changed. Commit everything as a wave commit — clean baseline before the upgrade. This gives the family full range to reorganize without worrying about losing work.

### Step 1: Family Dispatch

Send the CPI-SI family to plan the full reorganization. Each member brings domain expertise:

| Member | Role | Focus Area |
|--------|------|------------|
| **Caleb Navon** (plan-builder) | Strategic planning | Phased approach, dependency analysis, execution order |
| **Ezra Matthan** (architecture-analyzer) | Architecture | Current folder structure, component relationships, reorganization design |
| **Nathan Emet** (test-validator) | Testing domain | Test infrastructure, QoL, data-driven test output, config-driven runner |
| **Joanna Elara** (pattern-finder) | Pattern discovery | Existing patterns in output.ts/pipeline.ts/handlers that create/transform/transpile should reuse |

### Step 2: Present Family Plan

Family produces a unified plan covering:
1. Folder reorganization (lib/ structure reflecting R-levels)
2. Command infrastructure upgrade (shared output, config-driven dispatch)
3. Create/transform/transpile alignment with linter maturity
4. Test experience using the same data-driven infrastructure as the linter
5. Verification strategy

### Step 3: Execute

The family (plus Nova as lead) executes the plan. Full upgrade.

## What Changed

Previous plan was Nova solo, bolting on a `test-fun.ts` wrapper script. Seanje redirected:
- Hook into the SAME shared system the linter uses (output.ts, config, data layer)
- Family plans it — richer, more thorough, production-grade from the start
- Full reorganization, not just feature additions
- "The whole 9 yards"
