# Verbose Output QoL — Show What the Linter Knows

> *"The entrance of thy words giveth light; it giveth understanding unto the simple."* — Psalm 119:130

## Context

We just hit 100% witness coverage (207/207 codes, 1378 tests). Then Seanje pointed at `exists.rs` — lint it. Perfect score. Verbose it — and the output was thin. 96 actions checked, and all the user sees is 5 lines of layer summaries with rolled-up scores.

**The problem:** The linter COMPUTES rich data (directives, identity fields, block geometry, 96 atomic actions with reasons, container groupings) but REPORTS only aggregated scores. The microscope is built; we're showing magnifying glass output.

**The goal:** Make verbose mode show what the linter actually sees — the file's anatomy as the linter understands it.

## Architecture: Data Already Exists

The data pipeline is: `Handler → AtomicAction[] → ContainerScore[] → BlockScore[] → HealthScore`. Every `AtomicAction` already carries:

```typescript
{
  check: string;        // "identity/PRAGMA/I1.key", "setup/subsection-order"
  container: string;    // "identity", "ordering", "placement"
  block: string;        // "metadata", "setup", "body", "closing"
  direction: -1 | 0 | 1;
  impact?: Severity;
  reason?: string;      // The WHY — "Missing required key 'purpose'"
  layer?: 0 | 1 | 2 | 3;
}
```

The `ContainerScore.actions[]` array holds ALL of these. `printHealthBreakdown()` currently ignores individual actions and only prints the container-level tallies.

## What Verbose Should Show

**Current output (exists.rs):**
```
OK  exists.rs  (0E 0W 0I)  health: 100/±100 perfect ✅  [t:242]
  ── health: 96 aligned (96 actions) ──
  [100] Layer 0 — EXISTENCE (R[50])
    100 structural/blocks (9/9)
    100 structural/separators (5/5)
  [100] Layer 1 — ORGANIZATION (R[25])
    100 metadata/directives (4/4)
    100 metadata/identity (47/47)
    ...
```

**Target output (same file):**
```
OK  exists.rs  (0E 0W 0I)  health: 100/±100 perfect ✅  [t:242]
  ── directives ──
    // #!omni code --rust -module->utility
    //omni:key      B-L0-hybrid-spec-config-exists
    //omni:code     --rust -module->utility
    //omni:version  a-03.50

  ── blocks ──
    METADATA   :1-28     (28 lines)
    SETUP      :30-38    (9 lines)
    BODY       :40-72    (33 lines)
    CLOSING    :74-84    (11 lines)

  ── identity ──
    I1.key       = B-L0-hybrid-spec-config-exists
    I1.format    = rust
    I1.from      = spec/config/src/exists.rs
    I1.at        = a-03.50
    I2.type      = code
    I2.structure = 4-block
    I2.subtype   = module
    ...

  ── health: 96 aligned (96 actions) ──
  [100] Layer 0 — EXISTENCE (R[50])
    100 structural/blocks (9/9)
      ✓ block/METADATA    ✓ block/SETUP    ✓ block/BODY    ✓ block/CLOSING
      ✓ block/order       ✓ block/end-METADATA  ...
    100 structural/separators (5/5)
      ✓ separator/width-consistency   ✓ separator/style-consistency  ...
  [100] Layer 1 — ORGANIZATION (R[25])
    100 metadata/directives (4/4)
      ✓ directive/key    ✓ directive/code    ✓ directive/version    ✓ directive/shebang
    100 metadata/identity (47/47)
      ✓ identity/PRAGMA/present   ✓ identity/METADATA/present
      ✓ identity/PRAGMA/I1.key    ✓ identity/PRAGMA/I1.format  ...
    ...
```

Three new sections BEFORE the health breakdown, then expanded action detail WITHIN the health breakdown.

## Implementation — Single File Primary

**Primary file:** `lib/engine/output.ts`

### Change 1: Add `printFileAnatomy()` function

New function called from `printFileSummary()` when `verbose=true`, BEFORE `printHealthBreakdown()`. Extracts data from `summary.health` action trees.

```typescript
function printFileAnatomy(summary: LintSummary): void
```

**Section A — Directives:** The pragma is on `summary.pragma`. For individual directive values, scan `summary.health` actions in the "directives" container — their `check` strings identify which directives were found. Also scan `summary.results` for directive-related info messages that carry the actual values.

**Section B — Blocks:** Extract from `summary.health.blocks[]`. Each `BlockScore` has `.block` name. For line ranges, scan structural actions — `check` strings like `block/METADATA` carry block identity. Line ranges come from lint results that have `.line` fields.

**Section C — Identity:** Extract from metadata block's "identity" and "field-values" containers. Individual actions carry `check` strings like `identity/PRAGMA/I1.key`. For actual values, scan results with `identity/` rules that carry field data in their messages.

### Change 2: Expand `printHealthBreakdown()` with per-action detail

Currently iterates containers and prints `score label (aligned/total)`. Add nested loop over `container.actions[]` when `deep=true`:

```typescript
for (const action of container.actions) {
  const icon = action.direction > 0 ? "✓" : action.direction < 0 ? "✗" : "·";
  const aColor = action.direction > 0 ? COLORS.green
    : action.direction < 0 ? COLORS.red : COLORS.dim;
  const shortCheck = action.check.split("/").pop() ?? action.check;
  // Compact inline layout, ~4 per line
}
```

Misaligned actions also print their `reason` field on the next line.

### Change 3: Add `--deep` / `-vv` flag

**Flag hierarchy:**
- No flag: errors + warnings only, one-line health score
- `--verbose` (`-v`): + info results, + directives/blocks/identity anatomy, + layer breakdown
- `--deep` (`-vv`): + individual action checks within each container

CLI parsing in `mod.ts`:
```typescript
const deep = rest.includes("--deep") || rest.includes("-vv");
```

### Change 4: (Optional) Enrich `LintSummary`

If extracting anatomy from action trees proves too brittle, add optional fields to `LintSummary` in `types.ts`:

```typescript
directives?: Record<string, string>;
blockPositions?: Array<{ name: string; startLine: number; endLine: number }>;
identityFields?: Array<{ key: string; value: string; line?: number }>;
```

Populated by handler during summarization. **Do this only if Change 1 extraction is insufficient.**

## Files to Modify

| File | Change | Phase |
|------|--------|-------|
| `mod.ts` | Add `--deep` / `-vv` flag, pass to print functions | 1 |
| `lib/engine/output.ts` | Add `printFileAnatomy()`, expand `printHealthBreakdown()` | 2-3 |
| `lib/foundation/types.ts` | (If needed) Enrich `LintSummary` | 4 |
| `deno.jsonc` | Add `lint:deep` task | 5 |

## Existing Functions to Reuse

| Function | File | Purpose |
|----------|------|---------|
| `printHealthBreakdown()` | `output.ts:278` | Expand, don't replace |
| `healthColor()` | `output.ts:75` | Color by score |
| `containerLayer()` | `output.ts:263` | Layer assignment |
| `printWhyAnnotation()` | `output.ts:169` | Why chain (already works with `--why`) |
| `parseFocusFromCheck()` | `output.ts:190` | Focus filtering (reuse for anatomy) |

## Implementation Order

1. `mod.ts` — Add `--deep` / `-vv` flag, pass through
2. `output.ts` — Add `printFileAnatomy()` (directives, blocks, identity)
3. `output.ts` — Expand `printHealthBreakdown()` action detail when `deep=true`
4. Manual test — verbose + deep on `exists.rs` and a file with errors
5. `deno.jsonc` — Add `lint:deep` task

## Verification

1. `deno task lint:verbose <exists.rs>` — shows directive, block, identity anatomy
2. `lint:rust -- --deep <exists.rs>` — shows individual action checks per container
3. `lint:verbose <file-with-errors>` — misaligned actions shown with ✗ and reason
4. `deno check lib/engine/output.ts` — type check passes
5. `deno task test` — all 1378+ tests pass (display-only changes, no logic impact)
6. Non-verbose mode unchanged — `deno task lint:rust <file>` identical output
