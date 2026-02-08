# Builder Source Reorganization Plan

## Context

The builder (`company-docs/build/builder/src/`) has grown to 60 files / ~15K lines. Several directories are too flat for their scope — especially `validate/` (11 files, 7,444 lines all flat) and `display/` (12 files all flat). The request is to add subdirectory organization.

## Scope

**In scope:** Directory restructuring of `validate/` and `display/` with all import path updates.

**Not in scope (deferred):**
- Splitting large files internally (editorial 3K, theme 1.6K, pages 1K) — the new directories *enable* future splitting without further import changes
- Splitting `config/types.ts` (1,727 lines) — pure type definitions, large but cohesive
- Splitting `errors.ts` (369 lines) — small enough to stay as-is

## Target Structure

### validate/ (11 flat files -> 5 subdirectories)

```
validate/
  index.ts              # Updated barrel
  runner.ts             # Updated imports
  types.ts              # No change
  structural/           # Structure validators
    index.ts            # New barrel
    base.ts             # git mv
    manual.ts           # git mv
    manual-l.ts         # git mv
    operational.ts      # git mv
  editorial/            # 57-check editorial lint
    index.ts            # git mv from editorial.ts
  theme/                # Theme YAML validation
    index.ts            # git mv from theme.ts
  bible/                # Scripture validation
    index.ts            # git mv from bible.ts
  toc/                  # Page/TOC validation
    index.ts            # git mv from pages.ts (renamed)
```

### display/ (12 flat files -> 2 subdirectories)

```
display/
  index.ts              # Updated barrel
  types.ts              # Stays at root
  shared/               # Shared utilities
    banner.ts           # git mv
    format.ts           # git mv
    progress.ts         # git mv
  formatters/           # Domain-specific formatters
    validation.ts       # git mv
    editorial.ts        # git mv
    bible.ts            # git mv
    toc.ts              # git mv
    spine.ts            # git mv
    assets.ts           # git mv from assets-report.ts (renamed)
    witness.ts          # git mv
```

## Execution Order

### Phase 1: Baseline + Directory Creation
- Run `tsc --noEmit` to confirm clean compilation
- Create all subdirectories (7 new dirs)

### Phase 2: validate/ Moves + Import Updates
- `git mv` 8 files to subdirectories
- Create `validate/structural/index.ts` barrel
- Update imports in moved files (relative paths go one level deeper)
- Update `validate/runner.ts` (5 import path changes)
- Rewrite `validate/index.ts` barrel
- Update external consumers: `witness/types.ts`, `witness/collector.ts`, `witness/runner.ts`, `cli/commands/lint.ts`, `cli/commands/toc.ts`
- **Checkpoint:** `tsc --noEmit` must pass

### Phase 3: display/ Moves + Import Updates
- `git mv` 10 files to subdirectories
- Create `display/shared/index.ts` and `display/formatters/index.ts` barrels
- Update imports in moved files
- Rewrite `display/index.ts` barrel
- Update external consumers: all 7 `cli/commands/*.ts` files
- **Checkpoint:** `tsc --noEmit` must pass

### Phase 4: Final Verification
- Full `tsc` build
- `node dist/cli.js --help` smoke test
- `node dist/cli.js spine` / `node dist/cli.js validate` functional tests

## Change Summary

| Metric | Count |
|--------|-------|
| Files moved (git mv) | 18 |
| New barrel files created | 3 |
| Files modified (import updates) | ~24 |
| Total import path changes | ~70 |
| Files unchanged | ~37 |

## Key Risk: Node16 Module Resolution

When `editorial.ts` becomes `editorial/index.ts`, imports must change from `./editorial.js` to `./editorial/index.js`. Node16 ESM does NOT auto-resolve directory indexes. Dynamic imports (`await import(...)`) in `lint.ts`, `toc.ts`, and `witness/runner.ts` must also be updated — the compiler won't catch broken dynamic import paths.
