// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/shared/code-4block.ts
// key:     B-tov-cws-struct-lib-shared-code-4block
// title:   CWS Struct — Shared 4-Block Functions
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-18
// authors: Nova Dawn (CPI-SI)
// purpose: Shared functions for all 4-block code handlers (Go, Rust, future C/TS).
//          Extracted from go.ts and rust.ts where both handlers had identical
//          implementations. Functions operate on shared types (BaseFileContext,
//          BlockPosition, etc.) and shared constants.
//
//          Functions: findBlocks, getBlockLines, blockLineToFile, findBlockRange,
//          getSubsectionRanges, checkSeparatorConsistency, checkClosingZoneOrder.
//
// biblical_foundation: "That which hath been is now; and that which is to be
//   hath already been." — Ecclesiastes 3:15
//   Same structure, same function — now in one place.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type {
  BlockPosition, SubsectionRange, SubsectionDef,
  BlockRange, BaseFileContext,
} from "./types.ts";

import {
  BLOCKS, BLOCK_PATTERNS, END_PATTERNS,
  BLOCK_SEPARATOR_WIDTH, SUBSECTION_SEPARATOR_WIDTH,
  BODY_SUBSECTION_PATTERN, BODY_SUBSECTION_LEGACY,
  SCALING_THRESHOLDS,
} from "./types.ts";

import type { LintResult, LintGridOptions, LintLayer, BlockName } from "../foundation/mod.ts";
import type { SchemaSubsectionDef, SchemaBodySubtype, SchemaClosingData } from "../foundation/mod.ts";
import { error, warn, info, getLintFocus } from "../foundation/mod.ts";

import type { IdentityField, FieldContentRule } from "./types.ts";

import type { ComposedNonCode, ChunkCategory } from "../data/mod.ts";
import { loadNonCode } from "../data/mod.ts";
import type { ChunkerConfig } from "./chunker.ts";
import { registerCache } from "../foundation/cache-registry.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Lint Grid — block × layer orchestration
// ---------------------------------------------------------------------------
//
// The lint checks form a grid: Block (vertical) × Layer (horizontal).
//
//               METADATA    SETUP       BODY        CLOSING
// L0 (R[50])  | ─────────── whole-file (pre-gate) ──────────── |
// L1 (R[25])  | pragma    | pkg+imp   | subsec    | zones      |
// L2 (R[10])  | identity  | hdr-doc   | subtype   | zone-cnt   |
// L3 (R[5])   | content   | content   | content   | content    |
//
// Default traversal: block-first (vertical). Pre-gate always runs.
// Both axes navigable via --check flag.
// ---------------------------------------------------------------------------

/** A check function that produces lint results (sync or async). */
export type CheckFn = () => LintResult[] | Promise<LintResult[]>;

/** Checks for one container (subsection/zone) within a block. */
export interface ContainerCheckSet {
  /** Container name (e.g., "Imports", "Constants", "Cv", "X5"). */
  container: string;
  /** Checks that validate content within this container. */
  checks: CheckFn[];
}

/** Checks for one block, organized by layer (L1→L2→L3) + per-container. */
export interface BlockCheckSet {
  /** Which block these checks validate. */
  block: BlockName;
  /** Layer 1 (R[25]) — structural checks within this block. */
  structure: CheckFn[];
  /** Layer 2 (R[10]) — cross-container relationship checks (ordering, presence, placement). */
  crossContainer: CheckFn[];
  /** Per-container checks, processed container-first within the block. */
  containers: ContainerCheckSet[];
  /** Layer 3 (R[5]) — content/schema-driven checks. */
  content: CheckFn[];
}

/** Complete lint chain for a 4-block file. */
export interface BlockLintChain {
  /** Pre-gate: whole-file checks (Layer 0 / R[50]). Always runs. */
  pregate: CheckFn[];
  /** Per-block checks, in order: METADATA → SETUP → BODY → CLOSING. */
  blocks: BlockCheckSet[];
}

/**
 * Execute lint checks on a block × layer grid.
 *
 * Default traversal: block-first (vertical). Pre-gate (L0) always runs.
 * Then each block runs L1 → L2 → L3 before moving to the next block.
 *
 * Slicing (via LintGridOptions):
 *   focusBlock only  → vertical (one column): pre-gate + that block's L1-L3
 *   focusLayer only  → horizontal (one row): that layer across all blocks
 *   both              → single cell: that layer in that block (+ pre-gate if L0)
 *   neither           → full grid, block-first traversal
 *
 * Each result gets stamped with its block name if not already set.
 *
 * Reads focus from getLintFocus() when no explicit options are passed.
 */
export async function runLintGrid(
  chain: BlockLintChain,
  options?: LintGridOptions,
): Promise<LintResult[]> {
  const results: LintResult[] = [];
  const focus = options ?? getLintFocus();
  const focusBlock = focus?.focusBlock;
  const focusLayer = focus?.focusLayer;

  // Pre-gate: whole-file checks (Layer 0 / R[50]).
  // Always run unless focusLayer is set to something other than 0.
  if (focusLayer === undefined || focusLayer === 0) {
    for (const fn of chain.pregate) {
      const r = await fn();
      for (const result of r) result.block = result.block ?? "structural";
      results.push(...r);
    }
  }

  const focusContainer = focus?.focusContainer;

  // Per-block: walk the grid block-first
  for (const blockSet of chain.blocks) {
    if (focusBlock && blockSet.block !== focusBlock) continue;

    // L1 (R[25]) — structural checks (cross-container)
    if (focusLayer === undefined || focusLayer === 1) {
      for (const fn of blockSet.structure) {
        const r = await fn();
        for (const result of r) result.block = result.block ?? blockSet.block;
        results.push(...r);
      }
    }

    // L2 (R[10]) — cross-container relationship checks
    if (focusLayer === undefined || focusLayer === 2) {
      for (const fn of blockSet.crossContainer) {
        const r = await fn();
        for (const result of r) result.block = result.block ?? blockSet.block;
        results.push(...r);
      }
    }

    // Per-container checks — container-first within the block
    // These run when no focusLayer is set, or when focusLayer is 2 (container level).
    // Container focus filters to one specific container.
    if (focusLayer === undefined || focusLayer === 2) {
      for (const cSet of blockSet.containers) {
        if (focusContainer && cSet.container !== focusContainer) continue;

        for (const fn of cSet.checks) {
          const r = await fn();
          for (const result of r) {
            result.block = result.block ?? blockSet.block;
            result.container = result.container ?? cSet.container;
          }
          results.push(...r);
        }
      }
    }

    // L3 (R[5]) — content checks (cross-container)
    if (focusLayer === undefined || focusLayer === 3) {
      for (const fn of blockSet.content) {
        const r = await fn();
        for (const result of r) result.block = result.block ?? blockSet.block;
        results.push(...r);
      }
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Block discovery — finding the 4 blocks in a file
// ---------------------------------------------------------------------------

/**
 * Scan lines for block boundaries.
 * Returns positions of each block found, in order of appearance.
 */
export function findBlocks(lines: string[]): BlockPosition[] {
  const positions: BlockPosition[] = [];

  for (const blockName of BLOCKS) {
    let headerLine = 0;
    let endLine = 0;

    for (let i = 0; i < lines.length; i++) {
      const trimmed = lines[i]!.trim();

      // Look for block header: "// METADATA", "// METADATA BLOCK [METADATA]", etc.
      if (blockName === "METADATA") {
        if (/^\/\/\s+METADATA(\s+BLOCK\s+\[METADATA\])?\s*$/.test(trimmed) && headerLine === 0) {
          headerLine = i + 1;
        }
      } else {
        if (BLOCK_PATTERNS[blockName]!.test(trimmed) && headerLine === 0) {
          headerLine = i + 1;
        }
      }

      // Look for END marker
      if (END_PATTERNS[blockName]!.test(trimmed) && endLine === 0) {
        endLine = i + 1;
      }
    }

    if (headerLine > 0) {
      positions.push({ name: blockName, line: headerLine, endLine });
    }
  }

  // Sort by file position so block order reflects actual file layout
  positions.sort((a, b) => a.line - b.line);

  return positions;
}

// ---------------------------------------------------------------------------
// Block line extraction
// ---------------------------------------------------------------------------

/**
 * Extract lines belonging to a specific block (between header and END/next block).
 */
export function getBlockLines(
  lines: string[],
  blocks: BlockPosition[],
  blockName: string,
): string[] {
  const block = blocks.find((b) => b.name === blockName);
  if (!block) return [];

  const startIdx = block.line; // 1-based, block header line — content starts after
  let endIdx: number;

  if (block.endLine > 0) {
    endIdx = block.endLine - 1; // 1-based, exclude END marker
  } else {
    // Find next block start
    const blockIdx = blocks.indexOf(block);
    const nextBlock = blocks[blockIdx + 1];
    endIdx = nextBlock ? nextBlock.line - 2 : lines.length;
  }

  return lines.slice(startIdx, endIdx);
}

/**
 * Convert a block-relative line index to a 1-based file line number.
 * blockLines[i] came from getBlockLines(), which slices at block.line (1-based).
 * So the file line for blockLines[i] is block.line + 1 + i.
 */
export function blockLineToFile(blocks: BlockPosition[], blockName: string, relIdx: number): number {
  const block = blocks.find((b) => b.name === blockName);
  if (!block) return 0;
  return block.line + 1 + relIdx;
}

// ---------------------------------------------------------------------------
// Block range — for transform operations
// ---------------------------------------------------------------------------

/**
 * Find the content range of a named block within the file lines array.
 * Returns indices suitable for slicing/splicing the lines array.
 */
export function findBlockRange(lines: string[], blockName: string): BlockRange | null {
  let headerLine = -1;
  let endLine = -1;

  // Pre-build regexes outside the loop (accepts bare and tagged formats)
  const headerRe = new RegExp(`^//\\s+${blockName}(\\s+BLOCK\\s+\\[${blockName}\\])?\\s*$`);
  const endRe = new RegExp(`^//\\s+END\\s+${blockName}(\\s+\\[END\\])?\\s*$`);

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();

    // Block header: `// BLOCKNAME` or `// BLOCKNAME BLOCK [BLOCKNAME]` between separators
    if (headerRe.test(trimmed) && headerLine < 0) {
      const above = i > 0 ? lines[i - 1]!.trim() : "";
      if (/^\/\/\s+={10,}$/.test(above)) {
        headerLine = i;
        continue;
      }
    }

    // END marker: `// END BLOCKNAME` or `// END BLOCKNAME [END]`
    if (endRe.test(trimmed) && headerLine >= 0 && endLine < 0) {
      endLine = i;
      break;
    }
  }

  if (headerLine < 0) return null;

  // Content starts after the closing = separator of the header
  let contentStart = headerLine + 1;
  if (contentStart < lines.length && /^\/\/\s+={10,}$/.test(lines[contentStart]!.trim())) {
    contentStart++;
  }

  // Content ends at the = separator before END marker (or at endLine if present)
  let contentEnd: number;
  if (endLine >= 0) {
    contentEnd = endLine;
    if (contentEnd > 0 && /^\/\/\s+={10,}$/.test(lines[contentEnd - 1]!.trim())) {
      contentEnd--;
    }
  } else {
    contentEnd = lines.length;
  }

  return { contentStart, contentEnd };
}

// ---------------------------------------------------------------------------
// Subsection ranges — finding sections within a block
// ---------------------------------------------------------------------------

/**
 * Find subsection boundaries within a block's lines.
 *
 * Uses the provided subsection definitions to find where each subsection starts.
 * Returns ranges so content within each subsection can be classified.
 *
 * Takes subsection patterns as parameter so Go and Rust can each pass
 * their language-specific SETUP_SUBSECTIONS.
 */
export function getSubsectionRanges(
  blockLines: string[],
  subsections: readonly SubsectionDef[],
): SubsectionRange[] {
  const ranges: SubsectionRange[] = [];

  for (let i = 0; i < blockLines.length; i++) {
    const trimmed = blockLines[i]!.trim();
    // Skip separator-only lines
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;
    // Skip Reserved Omission entries (indented references, not section headers)
    if (/^\/\/\s{3,}\S/.test(trimmed)) continue;

    for (const sub of subsections) {
      if (sub.pattern.test(trimmed)) {
        if (!ranges.some((r) => r.tag === sub.tag)) {
          ranges.push({ tag: sub.tag, startIdx: i, endIdx: blockLines.length });
        }
        break;
      }
    }
  }

  // Fix endIdx for each range — next range's startIdx
  for (let i = 0; i < ranges.length - 1; i++) {
    ranges[i]!.endIdx = ranges[i + 1]!.startIdx;
  }

  return ranges;
}

// ---------------------------------------------------------------------------
// Container line extraction — get lines for a specific container
// ---------------------------------------------------------------------------
//
// Tag normalization flow:
//   Schema SubsectionDef tags → Title Case ("Imports", "Core Types", "Identity Access")
//   Section registry keys    → kebab-case ("imports", "core-types", "identity-access")
//   Closing zone display     → Short codes ("Cv", "Ce", "X1")
//   Matching strategy        → strip all separators (hyphens, spaces), case-insensitive
//
// The caller passes the containerTag (from section registry, kebab-case)
// and the subsection ranges (from getSubsectionRanges, Title Case tags).
// This function normalizes both to a canonical form (lowercase, no separators)
// for matching. This handles ALL naming conventions:
//   "core-types" → "coretypes"
//   "Core Types" → "coretypes"
//   "CoreTypes"  → "coretypes"
//

/**
 * Get the source lines belonging to a specific container within a block.
 *
 * Uses subsection ranges (from getSubsectionRanges()) to find the matching
 * container by tag. Tags are normalized to a canonical form (lowercase, no
 * hyphens, no spaces) to handle the three naming conventions:
 *   kebab-case (registry):  "core-types", "identity-access"
 *   Title Case (schema):    "Core Types", "Identity Access"
 *   PascalCase (legacy):    "CoreTypes", "IdentityAccess"
 *
 * @param blockLines     Lines within the block (from getBlockLines)
 * @param ranges         Subsection ranges (from getSubsectionRanges)
 * @param containerTag   Container tag to extract (e.g., "imports", "core-types")
 * @returns Lines belonging to that container, or empty array if not found
 */
export function getContainerLines(
  blockLines: string[],
  ranges: SubsectionRange[],
  containerTag: string,
): string[] {
  // Normalize: strip hyphens, spaces, lowercase — handles all naming conventions
  const normalized = normalizeTag(containerTag);

  const range = ranges.find((r) => normalizeTag(r.tag) === normalized);

  if (!range) return [];

  return blockLines.slice(range.startIdx, range.endIdx);
}

/**
 * Normalize a tag to canonical form for matching.
 *
 * Strips hyphens and spaces, lowercases — so all three naming conventions
 * collapse to the same string:
 *   "core-types"  → "coretypes"
 *   "Core Types"  → "coretypes"
 *   "CoreTypes"   → "coretypes"
 */
function normalizeTag(tag: string): string {
  return tag.toLowerCase().replace(/[-\s]/g, "");
}

// ---------------------------------------------------------------------------
// Checks — separator consistency
// ---------------------------------------------------------------------------

/**
 * Check separator width consistency — block-aware.
 *
 * Three separator types: = (block), ─ (subsection box-drawing), - (legacy dash).
 *
 * Key insight: METADATA legitimately uses wider ─ separators (84 chars) for
 * the I/C pragma and metadata sections. SETUP/BODY/CLOSING use narrower ones
 * (74 chars) for code subsections. Inconsistency WITHIN a block is wrong;
 * different widths BETWEEN blocks is expected design.
 *
 * Block separators (=) are checked globally — they should be consistent
 * everywhere since they delimit the blocks themselves.
 */
export function checkSeparatorConsistency(ctx: BaseFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // --- Determine which block each line belongs to ---
  function getBlock(lineNum1Based: number): string {
    let current = "METADATA"; // Default: before first block
    for (const b of ctx.blocks) {
      if (b.line <= lineNum1Based) current = b.name;
    }
    return current;
  }

  const eqSeparators: Array<{ line: number; width: number }> = [];
  // Group box separators per block — each block checks its own consistency.
  // METADATA+SETUP may legitimately use wider separators (84─) than
  // BODY+CLOSING (74─). That's a design choice, not an inconsistency.
  const boxByBlock = new Map<string, Array<{ line: number; width: number }>>();
  const dashSeparators: Array<{ line: number; width: number }> = [];

  for (let i = 0; i < ctx.lines.length; i++) {
    const trimmed = ctx.lines[i]!.trim();
    const lineNum = i + 1;

    // Block separators (=) — checked globally
    const eqMatch = trimmed.match(/^\/\/\s+(={4,})\s*$/);
    if (eqMatch) {
      eqSeparators.push({ line: lineNum, width: eqMatch[1]!.length });
    }

    // Subsection separators — Unicode box-drawing (─), bucketed per block
    const boxMatch = trimmed.match(/^\/\/\s+(─{4,})\s*$/);
    if (boxMatch) {
      const block = getBlock(lineNum);
      const entry = { line: lineNum, width: boxMatch[1]!.length };
      if (!boxByBlock.has(block)) boxByBlock.set(block, []);
      boxByBlock.get(block)!.push(entry);
    }

    // Subsection separators — ASCII dash (-) full-width only
    const dashMatch = trimmed.match(/^\/\/\s+(-{4,})\s*$/);
    if (dashMatch) {
      dashSeparators.push({ line: lineNum, width: dashMatch[1]!.length });
    }
  }

  // Use schema-driven widths if available, otherwise hardcoded defaults
  const blockWidth = ctx.formatConfig?.blockSeparatorWidth ?? BLOCK_SEPARATOR_WIDTH;
  const subWidth = ctx.formatConfig?.subsectionSeparatorWidth ?? SUBSECTION_SEPARATOR_WIDTH;

  // Check block separator (=) consistency — global
  if (eqSeparators.length >= 2) {
    const widths = new Set(eqSeparators.map((s) => s.width));
    if (widths.size > 1) {
      const widthList = [...widths].sort().join(", ");
      const firstBad = eqSeparators.find((s) => s.width !== eqSeparators[0]!.width);
      results.push(
        warn(file, "style/eq-separator-width",
          `Inconsistent block separator widths: ${widthList} chars — pick one`,
          { line: firstBad?.line ?? eqSeparators[0]!.line }),
      );
    }
    const dominant = eqSeparators[0]!.width;
    if (dominant !== blockWidth) {
      results.push(
        info(file, "style/eq-separator-standard",
          `Block separators are ${dominant} chars wide (standard: ${blockWidth})`,
          { line: eqSeparators[0]!.line }),
      );
    }
  }

  // Check ─ separator consistency per block — each block is its own world.
  // Infrastructure blocks (METADATA, SETUP) may use wider separators than
  // logic blocks (BODY, CLOSING). Both are valid; only internal
  // inconsistency within a single block is a warning.
  for (const [blockName, seps] of boxByBlock) {
    if (seps.length < 2) continue;
    const widths = new Set(seps.map((s) => s.width));
    if (widths.size > 1) {
      const widthList = [...widths].sort().join(", ");
      const firstBad = seps.find((s) => s.width !== seps[0]!.width);
      results.push(
        warn(file, "style/box-separator-width",
          `Inconsistent ─ separator widths in ${blockName}: ${widthList} chars — pick one per block`,
          { line: firstBad?.line ?? seps[0]!.line }),
      );
    }
  }

  // Info: report if any block's ─ separators differ from the standard width
  for (const [blockName, seps] of boxByBlock) {
    if (seps.length === 0) continue;
    const dominant = seps[0]!.width;
    if (dominant !== subWidth) {
      results.push(
        info(file, "style/box-separator-standard",
          `${blockName} ─ separators are ${dominant} chars wide (standard: ${subWidth})`,
          { line: seps[0]!.line }),
      );
    }
  }

  // Check ASCII dash separator consistency (fallback / legacy style)
  if (dashSeparators.length >= 2) {
    const widths = new Set(dashSeparators.map((s) => s.width));
    if (widths.size > 1) {
      const widthList = [...widths].sort().join(", ");
      const firstBad = dashSeparators.find((s) => s.width !== dashSeparators[0]!.width);
      results.push(
        warn(file, "style/dash-separator-width",
          `Inconsistent dash separator widths: ${widthList} chars — pick one`,
          { line: firstBad?.line ?? dashSeparators[0]!.line }),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Checks — CLOSING zone order
// ---------------------------------------------------------------------------

/**
 * Validate CLOSING zone ordering.
 *
 * Two-tier check:
 *   1. Code zones (Cv, Ce, Cc) must all appear before documentation (X1-X6)
 *   2. Within each tier, zones must appear in canonical order
 *
 * Only checks zones that ARE present — missing zones are valid
 * (not all files need all zones).
 */
export function checkClosingZoneOrder(
  ctx: BaseFileContext,
  closingData: SchemaClosingData,
): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Templates have instructional markers — skip
  if (ctx.isTemplate) return results;

  const closingLines = getBlockLines(ctx.lines, ctx.blocks, "CLOSING");
  if (closingLines.length === 0) return results;

  // Find which zones are present and their positions
  const found: Array<{ tag: string; kind: "code" | "doc"; lineIdx: number }> = [];

  for (let i = 0; i < closingLines.length; i++) {
    const trimmed = closingLines[i]!.trim();
    // Skip separator-only lines
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;

    for (const zone of closingData.zones) {
      if (zone.pattern.test(trimmed)) {
        if (!found.some((f) => f.tag === zone.tag)) {
          found.push({ tag: zone.tag, kind: zone.kind, lineIdx: i });
        }
        break;
      }
    }
  }

  if (found.length < 2) return results;

  // Check 1: Code zones must come before documentation sections
  const lastCode = found.filter((f) => f.kind === "code").pop();
  const firstDoc = found.find((f) => f.kind === "doc");

  if (lastCode && firstDoc && lastCode.lineIdx > firstDoc.lineIdx) {
    const fileLine = blockLineToFile(ctx.blocks, "CLOSING", lastCode.lineIdx);
    results.push(
      warn(file, "closing/zone-order",
        `Code zone ${lastCode.tag} appears after documentation section ${firstDoc.tag} — code zones (Cv/Ce/Cc) must precede documentation (X1-X6)`,
        { line: fileLine }),
    );
  }

  // Check 2: Within code zones, verify canonical order (Cv → Ce → Cc)
  const codeZones = found.filter((f) => f.kind === "code");
  const codeOrder: string[] = closingData.zones.filter((z) => z.kind === "code").map((z) => z.tag);

  for (let i = 1; i < codeZones.length; i++) {
    const prev = codeZones[i - 1]!;
    const curr = codeZones[i]!;
    const prevIdx = codeOrder.indexOf(prev.tag);
    const currIdx = codeOrder.indexOf(curr.tag);

    if (currIdx < prevIdx) {
      const foundOrder = codeZones.map((z) => z.tag).join(" → ");
      const fileLine = blockLineToFile(ctx.blocks, "CLOSING", curr.lineIdx);
      results.push(
        warn(file, "closing/code-zone-order",
          `Code zone ${curr.tag} appears after ${prev.tag} — expected Cv → Ce → Cc, found: ${foundOrder}`,
          { line: fileLine }),
      );
      break;
    }
  }

  // Check 3: Within documentation sections, verify canonical order (X1 → ... → X6)
  const docZones = found.filter((f) => f.kind === "doc");
  const docOrder: string[] = closingData.zones.filter((z) => z.kind === "doc").map((z) => z.tag);

  for (let i = 1; i < docZones.length; i++) {
    const prev = docZones[i - 1]!;
    const curr = docZones[i]!;
    const prevIdx = docOrder.indexOf(prev.tag);
    const currIdx = docOrder.indexOf(curr.tag);

    if (currIdx < prevIdx) {
      const foundOrder = docZones.map((z) => z.tag).join(" → ");
      const fileLine = blockLineToFile(ctx.blocks, "CLOSING", curr.lineIdx);
      results.push(
        warn(file, "closing/doc-section-order",
          `Documentation section ${curr.tag} appears after ${prev.tag} — expected X1 → X2 → ... → X6, found: ${foundOrder}`,
          { line: fileLine }),
      );
      break;
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// I/C field existence validation — presence checks (shared across all 4-block handlers)
// ---------------------------------------------------------------------------

/**
 * Validate the EXISTENCE of required/defined I/C fields in a Pragma or Metadata var.
 *
 * Groups parsed fields by section, then checks against requirements.
 * Missing required fields → warn. Missing defined fields → info.
 *
 * @param file        File path for lint results
 * @param fields      Parsed identity fields from the var/static
 * @param requirements  Per-section required/defined field lists
 * @param containerName Name of the identity container ("Pragma", "Metadata", "PRAGMA", etc.)
 * @param suffix      Language-specific suffix ("var" for Go, "static" for Rust)
 */
export function validateICFields(
  file: string,
  fields: IdentityField[],
  requirements: Record<string, { required: string[]; defined: string[] }>,
  containerName: string,
  suffix = "var",
): LintResult[] {
  const results: LintResult[] = [];

  // Group fields by section, tracking base field names
  const presentFields = new Map<string, Set<string>>();
  for (const f of fields) {
    if (!presentFields.has(f.section)) {
      presentFields.set(f.section, new Set());
    }
    // For nested fields like "requires.stdlib", the base field "requires" counts as present
    const baseField = f.field.split(".")[0]!;
    presentFields.get(f.section)!.add(baseField);
  }

  // Check required fields in each section
  for (const [section, req] of Object.entries(requirements)) {
    const sectionFields = presentFields.get(section);

    for (const field of req.required) {
      if (!sectionFields?.has(field)) {
        results.push(warn(file, `identity/${containerName}/${section}.${field}`,
          `Missing required field ${section}.${field} in ${containerName} ${suffix}`, {
            description: `Add ${section}.${field} to ${containerName}`,
            toml: `\t{"${section}.${field}", ""},`,
            location: `in ${containerName}`,
          }));
      }
    }

    for (const field of req.defined) {
      if (!sectionFields?.has(field)) {
        results.push(info(file, `identity/${containerName}/${section}.${field}`,
          `Missing defined field ${section}.${field} in ${containerName} ${suffix}`, {
            description: `Add ${section}.${field} to ${containerName}`,
            toml: `\t{"${section}.${field}", ""},`,
            location: `in ${containerName}`,
          }));
      }
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// I/C field content validation — value checks (shared across all 4-block handlers)
// ---------------------------------------------------------------------------

/**
 * Validate the CONTENT of parsed I/C fields against content rules.
 *
 * Existence checks ask "is the field present?" (validateICFields above).
 * Content checks ask "is the value valid?" (this function).
 *
 * Skips fields that:
 * - Don't exist (existence check handles those)
 * - Have placeholder values like [YOUR-KEY-HERE] (detectPlaceholders handles those)
 *
 * Rule names use "value/" prefix: value/Pragma/I1.key, value/Metadata/C1.version, etc.
 */
export function validateICFieldContent(
  file: string,
  fields: IdentityField[],
  rules: readonly FieldContentRule[],
  varName: string,
): LintResult[] {
  const results: LintResult[] = [];

  // Build a lookup: "I1.key" → IdentityField (first occurrence wins for base field)
  const fieldMap = new Map<string, IdentityField>();
  for (const f of fields) {
    const fullKey = `${f.section}.${f.field}`;
    if (!fieldMap.has(fullKey)) {
      fieldMap.set(fullKey, f);
    }
  }

  const PLACEHOLDER_RE = /^\[.+\]$/;

  for (const rule of rules) {
    const f = fieldMap.get(rule.field);
    if (!f) continue;                          // field not present — skip
    if (PLACEHOLDER_RE.test(f.value)) continue; // placeholder — skip

    const ruleId = `value/${varName}/${rule.field}`;
    const emit = rule.severity === "error" ? error
               : rule.severity === "warn" ? warn
               : info;

    switch (rule.check) {
      case "pattern":
        if (f.value && rule.pattern && !rule.pattern.test(f.value)) {
          results.push(emit(file, ruleId,
            `${varName}.${rule.field} = "${f.value}" — ${rule.message}`,
            { line: f.line }));
        }
        break;

      case "enum":
        if (f.value && rule.values && !rule.values.has(f.value.toLowerCase())) {
          results.push(emit(file, ruleId,
            `${varName}.${rule.field} = "${f.value}" — ${rule.message}`,
            { line: f.line }));
        }
        break;

      case "non-empty":
        if (!f.value || f.value.trim() === "") {
          results.push(emit(file, ruleId,
            `${varName}.${rule.field} is empty — ${rule.message}`,
            { line: f.line }));
        }
        break;

      case "path-like":
        if (f.value && !f.value.includes("/")) {
          results.push(emit(file, ruleId,
            `${varName}.${rule.field} = "${f.value}" — ${rule.message}`,
            { line: f.line }));
        }
        break;
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// CLOSING required zones — check that X1 (Policy) and X5 (Note) are present
// ---------------------------------------------------------------------------

/**
 * Check that required CLOSING documentation zones are present.
 *
 * X1 (Policy) and X5 (Note) are required by the base-4block-schema.
 * Missing zones are info-level — this is a content completeness check,
 * not a structural requirement.
 *
 * Skips templates — they have X6 (Template) zone with different rules.
 */
export function checkClosingRequiredZones(
  ctx: BaseFileContext,
  closingData: SchemaClosingData,
): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) return results;

  const closingLines = getBlockLines(ctx.lines, ctx.blocks, "CLOSING");
  if (closingLines.length === 0) return results;

  // Find which doc zones are present
  const presentTags = new Set<string>();
  for (const line of closingLines) {
    const trimmed = line.trim();
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;
    for (const zone of closingData.zones) {
      if (zone.kind === "doc" && zone.pattern.test(trimmed)) {
        presentTags.add(zone.tag);
        break;
      }
    }
  }

  // Check required zones
  for (const req of closingData.docRequirements) {
    if (req.required && !presentTags.has(req.tag)) {
      const closingBlock = ctx.blocks.find((b) => b.name === "CLOSING");
      results.push(
        info(file, `closing/required-${req.tag}`,
          `CLOSING block missing required ${req.tag} documentation zone`,
          { line: closingBlock?.line ?? 0 }),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// CLOSING zone content — check X1/X5 internal fields
// ---------------------------------------------------------------------------

/**
 * Check that present CLOSING documentation zones have their required fields.
 *
 * X1 (Policy) should have: Never, Careful, Safe fields.
 * X5 (Note) should have: Note, Scripture fields.
 *
 * Only checks zones that ARE present — if a zone is missing entirely,
 * checkClosingRequiredZones catches that. The cascade in computeHealth
 * will neutralize content checks for missing zones.
 */
export function checkClosingZoneContent(
  ctx: BaseFileContext,
  closingData: SchemaClosingData,
): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) return results;

  const closingLines = getBlockLines(ctx.lines, ctx.blocks, "CLOSING");
  if (closingLines.length === 0) return results;

  const closingBlock = ctx.blocks.find((b) => b.name === "CLOSING");
  const closingStart = closingBlock?.line ?? 0;

  // Build zone ranges: tag → [startIdx, endIdx) within closingLines
  const zoneRanges: Array<{ tag: string; startIdx: number; endIdx: number }> = [];

  for (let i = 0; i < closingLines.length; i++) {
    const trimmed = closingLines[i]!.trim();
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;

    for (const zone of closingData.zones) {
      if (zone.kind === "doc" && zone.pattern.test(trimmed)) {
        if (!zoneRanges.some((z) => z.tag === zone.tag)) {
          zoneRanges.push({ tag: zone.tag, startIdx: i, endIdx: closingLines.length });
        }
        break;
      }
    }
  }

  // Close each zone at the start of the next
  for (let i = 0; i < zoneRanges.length - 1; i++) {
    zoneRanges[i]!.endIdx = zoneRanges[i + 1]!.startIdx;
  }

  // Check every doc zone that has fields defined in the schema
  for (const req of closingData.docRequirements) {
    if (!req.fields) continue;

    const zoneRange = zoneRanges.find((z) => z.tag === req.tag);
    if (!zoneRange) continue;

    const patterns = closingData.fieldPatterns[req.tag];
    if (!patterns) continue;

    const zoneLinesSlice = closingLines.slice(zoneRange.startIdx, zoneRange.endIdx);
    const foundFields = new Set<string>();

    for (const line of zoneLinesSlice) {
      const trimmed = line.trim();
      for (const [field, pattern] of Object.entries(patterns)) {
        if (pattern.test(trimmed)) {
          foundFields.add(field);
        }
      }
    }

    const missing = req.fields.required.filter((f) => !foundFields.has(f));
    if (missing.length > 0) {
      const fieldList = req.fields.required.join(", ");
      results.push(
        info(file, `closing/${req.tag}-content`,
          `${req.tag} zone missing required fields: ${missing.join(", ")} (expected: ${fieldList})`,
          { line: closingStart + 1 + zoneRange.startIdx }),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// CLOSING X6 template-only detection
// ---------------------------------------------------------------------------

/**
 * Check if X6 (Template Guide) section appears in a non-template file.
 *
 * X6 is template-only — it should be removed when instantiating from template.
 * Its presence in a derived file indicates the template wasn't fully processed.
 */
export function checkClosingX6TemplateOnly(
  ctx: BaseFileContext,
  closingData: SchemaClosingData,
): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // X6 is EXPECTED in templates — only flag in derived files
  if (ctx.isTemplate) return results;

  const closingLines = getBlockLines(ctx.lines, ctx.blocks, "CLOSING");
  if (closingLines.length === 0) return results;

  // Find X6 in the schema — check if it's marked template_only
  const x6Req = closingData.docRequirements.find((r) => r.tag === "X6");
  if (!x6Req || !x6Req.templateOnly) return results;

  // Find X6 zone in the file
  const x6Zone = closingData.zones.find((z) => z.tag === "X6");
  if (!x6Zone) return results;

  for (let i = 0; i < closingLines.length; i++) {
    const trimmed = closingLines[i]!.trim();
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;

    if (x6Zone.pattern.test(trimmed)) {
      const fileLine = blockLineToFile(ctx.blocks, "CLOSING", i);
      results.push(
        warn(file, "closing/X6-template-only",
          `X6 (Template Guide) section found in non-template file — remove when instantiating from template`,
          { line: fileLine }),
      );
      break;
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// CLOSING content depth — check that field values are non-empty
// ---------------------------------------------------------------------------

/**
 * Check that CLOSING documentation zone fields have non-empty content.
 *
 * This is a DEPTH check — goes beyond field presence (checkClosingZoneContent)
 * to verify the content is meaningful:
 * - Never/Careful/Safe in X1 should have actual guidance text
 * - Note in X5 should have a real note, not placeholder
 * - Scripture in X5 should contain a scripture reference
 *
 * Only checks fields that ARE present — missing fields are caught by
 * checkClosingZoneContent.
 */
export function checkClosingDocFieldContent(
  ctx: BaseFileContext,
  closingData: SchemaClosingData,
): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) return results;

  const closingLines = getBlockLines(ctx.lines, ctx.blocks, "CLOSING");
  if (closingLines.length === 0) return results;

  const closingBlock = ctx.blocks.find((b) => b.name === "CLOSING");
  const closingStart = closingBlock?.line ?? 0;

  // Build zone ranges
  const zoneRanges: Array<{ tag: string; startIdx: number; endIdx: number }> = [];
  for (let i = 0; i < closingLines.length; i++) {
    const trimmed = closingLines[i]!.trim();
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;
    for (const zone of closingData.zones) {
      if (zone.kind === "doc" && zone.pattern.test(trimmed)) {
        if (!zoneRanges.some((z) => z.tag === zone.tag)) {
          zoneRanges.push({ tag: zone.tag, startIdx: i, endIdx: closingLines.length });
        }
        break;
      }
    }
  }
  for (let i = 0; i < zoneRanges.length - 1; i++) {
    zoneRanges[i]!.endIdx = zoneRanges[i + 1]!.startIdx;
  }

  // For each doc zone with fields, check content depth
  for (const req of closingData.docRequirements) {
    if (!req.fields) continue;

    const zoneRange = zoneRanges.find((z) => z.tag === req.tag);
    if (!zoneRange) continue;

    const patterns = closingData.fieldPatterns[req.tag];
    if (!patterns) continue;

    const zoneLinesSlice = closingLines.slice(zoneRange.startIdx, zoneRange.endIdx);

    // Check each field line: "// Never: <value>" — value should be non-trivial
    for (const [fieldName, fieldPattern] of Object.entries(patterns)) {
      for (const line of zoneLinesSlice) {
        const trimmed = line.trim();
        if (fieldPattern.test(trimmed)) {
          // Extract value after the colon
          const colonIdx = trimmed.indexOf(":");
          if (colonIdx >= 0) {
            const value = trimmed.slice(colonIdx + 1).trim();
            // Check for empty or placeholder values
            if (!value || value === "" || /^\[.*\]$/.test(value)) {
              results.push(
                info(file, `closing/${req.tag}-depth`,
                  `${req.tag} field "${fieldName}" has ${!value ? "empty" : "placeholder"} value — provide actual guidance`,
                  { line: closingStart + 1 + zoneRange.startIdx }),
              );
            }
          }
          break; // field found, move to next field
        }
      }
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Content-aware validation — template/derived, subtype, cross-field
// ---------------------------------------------------------------------------

/**
 * Validate template vs derived field consistency.
 *
 * Templates SHOULD have:   I1.at = "template", C1.status = "Template"
 * Derived files SHOULD NOT have these values (they indicate uninstantiated template).
 *
 * Starts at info severity — promote after false-positive assessment.
 */
export function validateTemplateVsDerived(
  file: string,
  pragmaFields: IdentityField[],
  metadataFields: IdentityField[],
  isTemplate: boolean,
  varName: { pragma: string; metadata: string },
): LintResult[] {
  const results: LintResult[] = [];

  // Build lookups
  const pragmaMap = new Map<string, IdentityField>();
  for (const f of pragmaFields) {
    pragmaMap.set(`${f.section}.${f.field}`, f);
  }
  const metaMap = new Map<string, IdentityField>();
  for (const f of metadataFields) {
    metaMap.set(`${f.section}.${f.field}`, f);
  }

  const i1at = pragmaMap.get("I1.at");
  const c1status = metaMap.get("C1.status");

  if (isTemplate) {
    // Template SHOULD have "template" / "Template"
    if (i1at && i1at.value && i1at.value.toLowerCase() !== "template") {
      results.push(info(file, `content/${varName.pragma}/template-at`,
        `Template file has ${varName.pragma}.I1.at = "${i1at.value}" — expected "template"`,
        { line: i1at.line }));
    }
    if (c1status && c1status.value && c1status.value.toLowerCase() !== "template") {
      results.push(info(file, `content/${varName.metadata}/template-status`,
        `Template file has ${varName.metadata}.C1.status = "${c1status.value}" — expected "Template"`,
        { line: c1status.line }));
    }
  } else {
    // Derived file SHOULD NOT have "template" values
    if (i1at && i1at.value && i1at.value.toLowerCase() === "template") {
      results.push(info(file, `content/${varName.pragma}/derived-at`,
        `${varName.pragma}.I1.at = "template" in non-template file — should be a version (e.g., a-01.00)`,
        { line: i1at.line }));
    }
    if (c1status && c1status.value && c1status.value.toLowerCase() === "template") {
      results.push(info(file, `content/${varName.metadata}/derived-status`,
        `${varName.metadata}.C1.status = "Template" in non-template file — should be Active, Draft, etc.`,
        { line: c1status.line }));
    }
  }

  return results;
}

/**
 * Validate I2.subtype matches the detected file subtype.
 *
 * The handler's subtype detection (via file pattern / pragma directive)
 * should agree with the I2.subtype field value.
 */
export function validateSubtypeConsistency(
  file: string,
  pragmaFields: IdentityField[],
  detectedSubtype: string | null,
  varName: string,
): LintResult[] {
  if (!detectedSubtype) return [];

  const i2subtype = pragmaFields.find(
    (f) => f.section === "I2" && f.field === "subtype",
  );
  if (!i2subtype || !i2subtype.value) return [];

  // Skip placeholder values
  if (/^\[.+\]$/.test(i2subtype.value)) return [];

  if (i2subtype.value.toLowerCase() !== detectedSubtype.toLowerCase()) {
    return [info(file, `content/${varName}/subtype-mismatch`,
      `${varName}.I2.subtype = "${i2subtype.value}" but file detected as "${detectedSubtype}"`,
      { line: i2subtype.line })];
  }

  return [];
}

/**
 * Validate I1.format matches the expected format.
 *
 * When linting a Go file, I1.format should be "go". For Rust, "rust".
 */
export function validateFormatConsistency(
  file: string,
  pragmaFields: IdentityField[],
  expectedFormat: string,
  varName: string,
): LintResult[] {
  const i1format = pragmaFields.find(
    (f) => f.section === "I1" && f.field === "format",
  );
  if (!i1format || !i1format.value) return [];

  // Skip placeholders
  if (/^\[.+\]$/.test(i1format.value)) return [];

  if (i1format.value.toLowerCase() !== expectedFormat.toLowerCase()) {
    return [info(file, `content/${varName}/format-mismatch`,
      `${varName}.I1.format = "${i1format.value}" but file is being linted as "${expectedFormat}"`,
      { line: i1format.line })];
  }

  return [];
}

// ---------------------------------------------------------------------------
// SETUP subsection ordering — canonical order enforcement
// ---------------------------------------------------------------------------

/**
 * Check that SETUP subsections appear in canonical order.
 *
 * Both Go and Rust handlers use the identical algorithm: iterate SETUP lines,
 * match against subsection definitions, verify ascending canonical order.
 * Alias usage is detected and reported as info.
 *
 * @param setupLines  Lines within the SETUP block (from getBlockLines)
 * @param subsections Schema-derived subsection definitions (tag + pattern + aliases)
 * @param blocks      Block positions (for line number computation)
 * @param file        File path for lint results
 * @param isTemplate  Skip ordering for templates (subsection names in comments)
 */
export function checkSetupSubsectionOrder(
  setupLines: string[],
  subsections: SubsectionDef[],
  blocks: BlockPosition[],
  file: string,
  isTemplate: boolean,
): LintResult[] {
  const results: LintResult[] = [];

  // Templates mention section names in overview comments — skip order check
  if (isTemplate) return results;
  if (setupLines.length === 0) return results;

  // Find which sections are present and their positions
  const found: Array<{ tag: string; lineIdx: number }> = [];

  for (let i = 0; i < setupLines.length; i++) {
    const trimmed = setupLines[i]!.trim();
    // Skip separator-only lines
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;
    // Skip Reserved Omission entries — indented references (//   Name — ...),
    // not section headers. Section headers use `// Name` (1 space after //).
    // Reserved Omission entries are an index, not boundaries.
    if (/^\/\/\s{3,}\S/.test(trimmed)) continue;

    for (const sub of subsections) {
      if (sub.pattern.test(trimmed)) {
        if (!found.some((f) => f.tag === sub.tag)) {
          found.push({ tag: sub.tag, lineIdx: i });
          // Detect alias usage — suggest canonical name.
          if (sub.aliases?.test(trimmed)) {
            const fileLine = blockLineToFile(blocks, "SETUP", i);
            results.push(
              info(file, "setup/alias-name",
                `"${trimmed.replace(/^\/\/\s*(?:\d+\.\s+)?/, "").trim()}" is recognized — consider canonical name "${sub.tag}"`,
                { line: fileLine }),
            );
          }
        }
        break;
      }
    }
  }

  if (found.length < 2) return results; // Nothing to check ordering on

  // Check ordering against the canonical sequence
  const canonicalOrder: string[] = subsections.map((s) => s.tag);

  let lastCanonIdx = -1;
  for (const f of found) {
    const canonIdx = canonicalOrder.indexOf(f.tag);
    if (canonIdx < lastCanonIdx) {
      const foundTags = found.map((x) => x.tag).join(" → ");
      const fileLine = blockLineToFile(blocks, "SETUP", f.lineIdx);
      results.push(
        warn(file, "setup/subsection-order",
          `SETUP subsection ${f.tag} appears after a later subsection — found: ${foundTags}, expected: ${canonicalOrder.join(" → ")}`,
          { line: fileLine }),
      );
      break;
    }
    lastCanonIdx = canonIdx;
  }

  return results;
}

// ---------------------------------------------------------------------------
// BODY subsection ordering — numeric ascending order
// ---------------------------------------------------------------------------

/**
 * Check that BODY subsections appear in ascending numeric order.
 *
 * BODY uses numbered markers: `// N. <Name>` where N determines order.
 * Unlike SETUP (name-based), BODY ordering is purely numeric and
 * subtype-agnostic. Both Go and Rust use the identical algorithm.
 *
 * @param bodyLines      Lines within the BODY block (from getBlockLines)
 * @param blocks         Block positions (for line number computation)
 * @param file           File path for lint results
 * @param isTemplate     Skip ordering for templates
 * @param includeLegacy  Also match §N — Name format (Go has this, Rust does not)
 */
export function checkBodySubsectionOrder(
  bodyLines: string[],
  blocks: BlockPosition[],
  file: string,
  isTemplate: boolean,
  includeLegacy = false,
): LintResult[] {
  const results: LintResult[] = [];

  if (isTemplate) return results;
  if (bodyLines.length === 0) return results;

  // Find numbered subsection markers in BODY
  const found: Array<{ num: number; name: string; lineIdx: number }> = [];

  for (let i = 0; i < bodyLines.length; i++) {
    const trimmed = bodyLines[i]!.trim();
    // Skip separator-only lines
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;
    // Skip Reserved Omission entries (indented references, not section headers)
    if (/^\/\/\s{3,}\S/.test(trimmed)) continue;

    const match = BODY_SUBSECTION_PATTERN.exec(trimmed) ??
                  (includeLegacy ? BODY_SUBSECTION_LEGACY.exec(trimmed) : null);
    if (match) {
      const num = parseInt(match[1]!, 10);
      const name = match[2]!.trim();
      // Only record first occurrence of each number
      if (!found.some((f) => f.num === num)) {
        found.push({ num, name, lineIdx: i });
      }
    }
  }

  // Need at least 2 subsections to check ordering
  if (found.length < 2) return results;

  // Check that numbers appear in ascending order
  let lastNum = -1;
  for (const f of found) {
    if (f.num < lastNum) {
      const foundOrder = found.map((x) => `${x.num}. ${x.name}`).join(" → ");
      const fileLine = blockLineToFile(blocks, "BODY", f.lineIdx);
      results.push(
        warn(file, "body/subsection-order",
          `BODY subsection §${f.num} (${f.name}) appears after §${lastNum} — found: ${foundOrder}`,
          { line: fileLine }),
      );
      break;
    }
    lastNum = f.num;
  }

  return results;
}

// ---------------------------------------------------------------------------
// Reserved Omission parsing — extract intentionally omitted sections
// ---------------------------------------------------------------------------

/**
 * Parse "Reserved Omission" comment sections to extract intentionally
 * omitted section names. Files can declare sections they don't need:
 *
 *   // Reserved Omission
 *   // ──────...
 *   //
 *   // Available (not needed in this module):
 *   //   Section Name  — reason
 *   //
 *   // Reserved (structural — not used):
 *   //   Section Name  — reason
 *
 * Returns normalized names (lowercased, no non-alphanumeric) of ALL
 * omitted sections — both "Available" and "Reserved".
 */
export function parseReservedOmissions(lines: string[]): Set<string> {
  const omitted = new Set<string>();
  const normalize = (s: string) => s.replace(/[^a-zA-Z0-9]/g, "").toLowerCase();
  let inOmission = false;

  for (const line of lines) {
    const trimmed = line.trim();

    // Detect start of Reserved Omission section
    if (/^\/\/\s*Reserved\s+Omission\s*$/i.test(trimmed)) {
      inOmission = true;
      continue;
    }

    if (inOmission) {
      // Block boundary markers end the section
      if (/^\/\/\s*={4,}/.test(trimmed)) {
        inOmission = false;
        continue;
      }
      // Separator lines — continue
      if (/^\/\/\s*[─\-]{4,}\s*$/.test(trimmed)) continue;
      // Empty comment lines — continue
      if (/^\/\/\s*$/.test(trimmed)) continue;
      // Header lines: "Available (not needed):" or "Reserved (structural):"
      if (/^\/\/\s*(Available|Reserved)\s*\(/.test(trimmed)) continue;

      // Section name line — two formats:
      //   One-per-line:      "//   Name — reason" or "//   Name"
      //   Comma-separated:   "//   Name1, Name2, Name3"
      // At least 2 leading spaces after // to distinguish from headers.
      const contentMatch = trimmed.match(/^\/\/\s{2,}(.+)$/);
      if (contentMatch) {
        const content = contentMatch[1]!.trim();
        // If it contains commas → comma-separated list
        if (content.includes(",")) {
          for (const part of content.split(",")) {
            const name = part.trim().replace(/\s*[—\-]{1,2}\s*.+$/, "");
            if (name && /^[A-Za-z]/.test(name)) {
              omitted.add(normalize(name));
            }
          }
        } else {
          // Single section: "Name — reason" or "Name"
          const name = content.replace(/\s*[—\-]{1,2}\s*.+$/, "").trim();
          if (name && /^[A-Za-z]/.test(name)) {
            omitted.add(normalize(name));
          }
        }
      }

      // Non-comment line = end of omission section
      if (!trimmed.startsWith("//")) {
        inOmission = false;
      }
    }
  }

  return omitted;
}

// ---------------------------------------------------------------------------
// Required SETUP subsections — schema-driven per-subtype
// ---------------------------------------------------------------------------

/**
 * Check that SETUP subsections required for the detected subtype are present.
 *
 * The schema defines `required_in` for each SETUP subsection — e.g., Imports
 * required in all subtypes, CoreTypes required in library, Constants in executable.
 * This check verifies that required subsections have a matching header in SETUP.
 *
 * @param setupLines    Lines within the SETUP block
 * @param subsections   Compiled subsection regexes (for finding present sections)
 * @param setupData     Raw schema data with requiredIn per subsection
 * @param subtype       Detected file subtype (null = skip)
 * @param blocks        Block positions (for line number computation)
 * @param file          File path for lint results
 * @param isTemplate    Skip for templates
 */
export function checkRequiredSetupSubsections(
  setupLines: string[],
  subsections: SubsectionDef[],
  setupData: SchemaSubsectionDef[],
  subtype: string | null,
  blocks: BlockPosition[],
  file: string,
  isTemplate: boolean,
): LintResult[] {
  if (isTemplate || !subtype) return [];
  if (setupLines.length === 0) return [];

  const ranges = getSubsectionRanges(setupLines, subsections);
  const presentTags = new Set(ranges.map((r) => r.tag));

  // Parse Reserved Omission — sections declared as intentionally absent
  const omitted = parseReservedOmissions(setupLines);
  const normalize = (s: string) => s.replace(/[^a-zA-Z0-9]/g, "").toLowerCase();

  const results: LintResult[] = [];
  const setupBlock = blocks.find((b) => b.name === "SETUP");

  for (const sub of setupData) {
    if (!sub.requiredIn.includes(subtype)) continue;
    if (presentTags.has(sub.tag)) continue;

    // Skip sections declared in Reserved Omission (substring match —
    // "Core Types" covers schema's "CoreTypes")
    const subNorm = normalize(sub.tag);
    const isOmitted = Array.from(omitted).some((o) =>
      o.includes(subNorm) || subNorm.includes(o));
    if (isOmitted) continue;

    results.push(
      warn(file, "setup/required-subsection",
        `SETUP missing subsection "${sub.tag}" — required for ${subtype} subtype`,
        { line: setupBlock?.line ?? 0 }),
    );
  }

  return results;
}

// ---------------------------------------------------------------------------
// Required BODY subsections — schema-driven per-subtype
// ---------------------------------------------------------------------------

/**
 * Check that BODY subsections required for the detected subtype are present.
 *
 * The schema defines `required_in` for each BODY subsection per subtype —
 * e.g., PublicAPIs required in library, RunFunction required in executable.
 * This check looks for matching numbered markers (// N. Name) in the BODY.
 *
 * @param bodyLines       Lines within the BODY block
 * @param bodySubtype     Body subtype data from schema (has subsections with requiredIn)
 * @param subtype         Detected file subtype (null = skip)
 * @param blocks          Block positions (for line number computation)
 * @param file            File path for lint results
 * @param isTemplate      Skip for templates
 * @param includeLegacy   Also match §N — Name format (Go has this, Rust does not)
 */
export function checkRequiredBodySubsections(
  bodyLines: string[],
  bodySubtype: SchemaBodySubtype | undefined,
  subtype: string | null,
  blocks: BlockPosition[],
  file: string,
  isTemplate: boolean,
  includeLegacy = false,
): LintResult[] {
  if (isTemplate || !subtype) return [];
  if (bodyLines.length === 0) return [];
  if (!bodySubtype || bodySubtype.subsections.length === 0) return [];

  // Collect subsection names from BODY markers
  const foundNames: string[] = [];
  for (const line of bodyLines) {
    const trimmed = line.trim();
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;

    const match = BODY_SUBSECTION_PATTERN.exec(trimmed) ??
                  (includeLegacy ? BODY_SUBSECTION_LEGACY.exec(trimmed) : null);
    if (match) {
      foundNames.push(match[2]!.trim());
    }
  }

  // Parse Reserved Omission — sections declared as intentionally absent
  const omitted = parseReservedOmissions(bodyLines);
  const normalize = (s: string) => s.replace(/[^a-zA-Z0-9]/g, "").toLowerCase();

  const results: LintResult[] = [];
  const bodyBlock = blocks.find((b) => b.name === "BODY");

  for (const sub of bodySubtype.subsections) {
    if (!sub.requiredIn.includes(subtype)) continue;

    // Skip sections declared in Reserved Omission (substring match —
    // "Constructors & Builders" covers schema's "Constructors")
    const subNorm = normalize(sub.tag);
    const isOmitted = Array.from(omitted).some((o) =>
      o.includes(subNorm) || subNorm.includes(o));
    if (isOmitted) continue;

    // Case-insensitive substring match — "Public APIs" matches "PublicAPIs" pattern
    const present = foundNames.some((f) => {
      const fLower = f.toLowerCase().replace(/\s+/g, "");
      const tagLower = sub.tag.toLowerCase().replace(/\s+/g, "");
      return fLower.includes(tagLower) || tagLower.includes(fLower);
    });

    if (!present) {
      results.push(
        warn(file, "body/required-subsection",
          `BODY missing subsection "${sub.tag}" — required for ${subtype} subtype`,
          { line: bodyBlock?.line ?? 0 }),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Subtype emphasis — heavy/light subsection balance
// ---------------------------------------------------------------------------

/**
 * Check that "heavy" subsections for the detected subtype have content.
 *
 * Each subtype has emphasis data: heavy subsections should carry the most
 * content, light subsections should be supporting. If heavy subsections
 * are empty but light ones have content, the emphasis is inverted.
 *
 * Only checks SETUP emphasis (exact tag matching). BODY emphasis requires
 * fuzzy name matching and is deferred to a future enhancement.
 *
 * @param lines            Full file lines
 * @param blocks           Block positions
 * @param setupSubsections Compiled subsection regexes (for finding ranges)
 * @param setupEmphasis    Heavy/light lists for SETUP from the schema
 * @param subtype          Detected file subtype (null = skip)
 * @param file             File path for lint results
 * @param isTemplate       Skip for templates
 */
export function checkSubtypeEmphasis(
  lines: string[],
  blocks: BlockPosition[],
  setupSubsections: SubsectionDef[],
  setupEmphasis: { heavy: string[]; light: string[] } | undefined,
  subtype: string | null,
  file: string,
  isTemplate: boolean,
): LintResult[] {
  if (isTemplate || !subtype) return [];
  if (!setupEmphasis) return [];

  const results: LintResult[] = [];

  const setupLines = getBlockLines(lines, blocks, "SETUP");
  if (setupLines.length === 0) return results;

  const ranges = getSubsectionRanges(setupLines, setupSubsections);

  // Count content lines per subsection (exclude separators and blanks)
  function contentCount(tag: string): number {
    const range = ranges.find((r) => r.tag === tag);
    if (!range) return 0;
    // startIdx is the header line itself — skip it (+1)
    const start = Math.min(range.startIdx + 1, range.endIdx);
    return setupLines.slice(start, range.endIdx).filter((l) => {
      const t = l.trim();
      return t !== "" && !/^\/\/\s*[─=\-]{4,}\s*$/.test(t);
    }).length;
  }

  let heavyTotal = 0;
  let lightTotal = 0;

  for (const tag of setupEmphasis.heavy) {
    heavyTotal += contentCount(tag);
  }

  for (const tag of setupEmphasis.light) {
    lightTotal += contentCount(tag);
  }

  // Only flag if heavy is completely empty but light has content
  if (heavyTotal === 0 && lightTotal > 0) {
    const setupBlock = blocks.find((b) => b.name === "SETUP");
    results.push(
      info(file, "emphasis/setup-inverted",
        `SETUP emphasis inverted for ${subtype} — heavy subsections (${setupEmphasis.heavy.join(", ")}) are empty but light subsections have ${lightTotal} content lines`,
        { line: setupBlock?.line ?? 0 }),
    );
  }

  return results;
}

// ---------------------------------------------------------------------------
// Scaling signals — block size thresholds
// ---------------------------------------------------------------------------

/**
 * Check for oversized SETUP and BODY blocks that suggest refactoring.
 *
 * SETUP > 200 content lines or BODY > 500 content lines triggers an info
 * suggesting the file may benefit from splitting. Separator-only lines
 * and blank lines are excluded from the count.
 *
 * @param lines      Full file lines
 * @param blocks     Block positions
 * @param file       File path for lint results
 * @param isTemplate Skip for templates
 */
export function checkScalingSignals(
  lines: string[],
  blocks: BlockPosition[],
  file: string,
  isTemplate: boolean,
): LintResult[] {
  const results: LintResult[] = [];

  if (isTemplate) return results;

  // SETUP size
  const setupLines = getBlockLines(lines, blocks, "SETUP");
  const setupContent = setupLines.filter((l) => {
    const t = l.trim();
    return t !== "" && !/^\/\/\s*[─=\-]{4,}\s*$/.test(t);
  });

  if (setupContent.length > SCALING_THRESHOLDS.SETUP) {
    const setupBlock = blocks.find((b) => b.name === "SETUP");
    results.push(
      info(file, "scaling/setup-size",
        `SETUP block has ${setupContent.length} content lines (threshold: ${SCALING_THRESHOLDS.SETUP}) — consider extracting types/constants into separate files`,
        { line: setupBlock?.line ?? 0 }),
    );
  }

  // BODY size
  const bodyLines = getBlockLines(lines, blocks, "BODY");
  const bodyContent = bodyLines.filter((l) => {
    const t = l.trim();
    return t !== "" && !/^\/\/\s*[─=\-]{4,}\s*$/.test(t);
  });

  if (bodyContent.length > SCALING_THRESHOLDS.BODY) {
    const bodyBlock = blocks.find((b) => b.name === "BODY");
    results.push(
      info(file, "scaling/body-size",
        `BODY block has ${bodyContent.length} content lines (threshold: ${SCALING_THRESHOLDS.BODY}) — consider extracting logic into submodules`,
        { line: bodyBlock?.line ?? 0 }),
    );
  }

  return results;
}

// ---------------------------------------------------------------------------
// Content linting — schema-driven content expectations
// ---------------------------------------------------------------------------
//
// Structure linting validates the skeleton (blocks, sections, zones).
// Content linting validates the flesh (what's INSIDE each section).
//
// The content schema (forms/{form}/{lang}.jsonc) defines what
// each block and section SHOULD contain. The linter reads these expectations
// and checks whether the file's actual content matches.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
//

/** Content expectations shape — passed from handler after loading. */
export interface ContentExpectations {
  pragma: string;
  METADATA: Record<string, unknown>;
  SETUP: Record<string, unknown>;
  BODY: Record<string, unknown>;
  CLOSING: Record<string, unknown>;
}

/**
 * Check BODY section content against content schema expectations.
 *
 * The content schema's `BODY.section_content` defines what each numbered
 * section should contain by name (e.g., B1_OrgChart, B3_CoreOperations).
 * This replaces hardcoded canonical subsection checks with schema-driven
 * validation.
 *
 * Rule namespace: content/body-*
 */
export function checkBodyContentExpectations(
  ctx: BaseFileContext,
  expectations: ContentExpectations,
  requiredBodySections?: string[],
  reservedBodySections?: string[],
): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) return results;
  if (!ctx.subtype) return results;

  const bodyExp = expectations.BODY as Record<string, unknown>;
  const sectionContent = bodyExp?.section_content as Record<string, string> | undefined;
  if (!sectionContent) return results;

  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  if (bodyLines.length === 0) return results;

  // Collect actual subsection names from BODY
  const found: Array<{ num: number; name: string; lineIdx: number }> = [];
  for (let i = 0; i < bodyLines.length; i++) {
    const trimmed = bodyLines[i]!.trim();
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;

    const match = BODY_SUBSECTION_PATTERN.exec(trimmed) ??
                  BODY_SUBSECTION_LEGACY.exec(trimmed);
    if (match) {
      const num = parseInt(match[1]!, 10);
      const name = match[2]!.trim();
      if (!found.some((f) => f.num === num)) {
        found.push({ num, name, lineIdx: i });
      }
    }
  }

  if (found.length === 0) return results;

  // Normalize: strip non-alphanumeric, lowercase.
  // Schema uses CamelCase ("IdentityAccess"), files use spaced ("Identity Access").
  const normalize = (s: string) => s.replace(/[^a-zA-Z0-9]/g, "").toLowerCase();
  const foundNormalized = found.map((f) => normalize(f.name));
  const bodyBlock = ctx.blocks.find((b) => b.name === "BODY");
  const bodyLine = bodyBlock?.line ?? 0;

  // Parse Reserved Omission — sections declared as intentionally absent.
  // "Available (not needed)" and "Reserved (structural)" sections are both omitted.
  const omitted = parseReservedOmissions(bodyLines);
  // Substring match — "Constructors & Builders" covers schema's "Constructors"
  const isOmittedSection = (sn: string) =>
    Array.from(omitted).some((o) => o.includes(sn) || sn.includes(o));

  // ── Per-required-section check ──
  // Each required section is its own atomic result.
  // Missing required section → info (one per section, not lumped).
  // Present required section → no result (pass = silence, health scorer adds +1).
  // Sections in Reserved Omission → skip (intentionally absent, not missing).
  if (requiredBodySections) {
    for (const section of requiredBodySections) {
      const sn = normalize(section);
      // Skip sections declared in Reserved Omission
      if (isOmittedSection(sn)) continue;
      const isPresent = foundNormalized.some((f) => f.includes(sn));
      if (!isPresent) {
        results.push(
          info(file, `content/body-required/${section}`,
            `BODY missing required section: ${section}`,
            { line: bodyLine }),
        );
      }
    }
  } else {
    // Fallback: no structure schema → check all content schema sections (graceful degradation)
    const canonicalNames: string[] = [];
    for (const key of Object.keys(sectionContent)) {
      const nameMatch = key.match(/^B\d+_(.+)$/);
      if (nameMatch) canonicalNames.push(nameMatch[1]!);
    }
    for (const section of canonicalNames) {
      const sn = normalize(section);
      // Skip sections declared in Reserved Omission
      if (isOmittedSection(sn)) continue;
      if (!foundNormalized.some((f) => f.includes(sn))) {
        results.push(
          info(file, `content/body-required/${section}`,
            `BODY missing expected section: ${section} (no structure schema — cannot distinguish required vs available)`,
            { line: bodyLine }),
        );
      }
    }
  }

  // ── Per-reserved-section check ──
  // Reserved sections that are correctly absent → no result (health scorer adds +1).
  // Reserved sections present WITH code → info (reserve violation).
  const reservedContent = bodyExp?.reserved_content as Record<string, string> | undefined;
  const reservedNames: string[] = [];
  if (reservedContent) {
    for (const key of Object.keys(reservedContent)) {
      const nameMatch = key.match(/^B\d+_(.+)$/);
      if (nameMatch) reservedNames.push(nameMatch[1]!);
    }
  } else if (reservedBodySections) {
    // Use structure schema's reserved list if content schema doesn't have reserved_content
    reservedNames.push(...reservedBodySections);
  }

  for (const rsv of reservedNames) {
    const foundSection = found.find((f) =>
      normalize(f.name).includes(normalize(rsv)));
    if (foundSection) {
      // Reserved section exists — check if it has actual code content
      const sectionStart = foundSection.lineIdx;
      const nextSection = found.find((f) => f.lineIdx > sectionStart);
      const sectionEnd = nextSection?.lineIdx ?? bodyLines.length;
      const sectionLines = bodyLines.slice(sectionStart + 1, sectionEnd);
      const codeLines = sectionLines.filter((l) => {
        const t = l.trim();
        return t !== "" && !t.startsWith("//") && !/^\/\/\s*[─=\-]{4,}\s*$/.test(t);
      });

      if (codeLines.length > 0) {
        const fileLine = blockLineToFile(ctx.blocks, "BODY", foundSection.lineIdx);
        const reason = reservedContent
          ? (reservedContent[Object.keys(reservedContent).find((k) =>
              normalize(k.replace(/^B\d+_/, "")) === normalize(rsv))!] ?? "reserved for this form")
          : "reserved for this form";
        results.push(
          warn(file, `content/body-reserve/${rsv}`,
            `BODY section ${foundSection.name} is reserved: "${reason}" — has ${codeLines.length} code lines`,
            { line: fileLine }),
        );
      }
      // else: reserved section exists but empty header only — no violation
    }
    // else: reserved section absent — CORRECT. No result.
    // Health scorer adds +1 aligned for the reserve omission.
  }

  return results;
}

/**
 * Check CLOSING zone content against content schema expectations.
 *
 * Per-zone atomic checking — each zone is its own point:
 * - Ce_pattern: "empty" or "no main()" → Ce zone should have no code
 * - Cv_pattern: describes test structure → Cv zone should have tests
 * - Reserved zones absent → correct (health scorer adds +1)
 *
 * Rule namespace: content/closing-zone/{zone}, content/closing-reserve/{zone}
 */
export function checkClosingContentExpectations(
  ctx: BaseFileContext,
  expectations: ContentExpectations,
  _requiredClosingZones?: string[],
  reservedClosingZones?: string[],
): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) return results;

  const closingExp = expectations.CLOSING as Record<string, unknown>;
  if (!closingExp) return results;

  const closingLines = getBlockLines(ctx.lines, ctx.blocks, "CLOSING");
  if (closingLines.length === 0) return results;

  const closingBlock = ctx.blocks.find((b) => b.name === "CLOSING");
  const closingStart = closingBlock?.line ?? 0;

  // Build zone ranges within closing lines
  const zonePattern = /^\/\/\s*(Cv|Ce|Cc)\s*[—–-]/;
  const zones: Array<{ tag: string; startIdx: number; endIdx: number }> = [];

  for (let i = 0; i < closingLines.length; i++) {
    const trimmed = closingLines[i]!.trim();
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;

    const zoneMatch = zonePattern.exec(trimmed);
    if (zoneMatch) {
      if (!zones.some((z) => z.tag === zoneMatch[1]!)) {
        zones.push({ tag: zoneMatch[1]!, startIdx: i, endIdx: closingLines.length });
      }
    }
  }

  // Close each zone at the start of the next
  for (let i = 0; i < zones.length - 1; i++) {
    zones[i]!.endIdx = zones[i + 1]!.startIdx;
  }

  // ── Per-zone content check: Ce ──
  const cePattern = closingExp.Ce_pattern;
  if (cePattern && typeof cePattern === "string") {
    const ceEmpty = /empty|no main|no entry point/i.test(cePattern);

    if (ceEmpty) {
      const ceZone = zones.find((z) => z.tag === "Ce");
      if (ceZone) {
        const ceLines = closingLines.slice(ceZone.startIdx + 1, ceZone.endIdx);
        const codeLinesInCe = ceLines.filter((l) => {
          const t = l.trim();
          return t !== "" && !t.startsWith("//") && !/^\/\/\s*[─=\-]{4,}\s*$/.test(t);
        });

        if (codeLinesInCe.length > 0) {
          results.push(
            warn(file, "content/closing-zone/Ce",
              `Ce zone should be empty for ${ctx.subtype ?? "this form"} (content schema: "${cePattern}") but has ${codeLinesInCe.length} code lines`,
              { line: closingStart + 1 + ceZone.startIdx }),
          );
        }
      }
    }
  }

  // ── Per-zone content check: Cv ──
  const cvPattern = closingExp.Cv_pattern;
  if (cvPattern && typeof cvPattern === "object") {
    const cvObj = cvPattern as Record<string, string>;
    const cvStructure = cvObj.structure;

    if (cvStructure) {
      const cvZone = zones.find((z) => z.tag === "Cv");
      if (cvZone) {
        const cvLines = closingLines.slice(cvZone.startIdx + 1, cvZone.endIdx);
        const nonEmptyCv = cvLines.filter((l) => l.trim() !== "");

        if (nonEmptyCv.length === 0) {
          results.push(
            info(file, "content/closing-zone/Cv",
              `Cv zone is empty — content schema expects: "${cvStructure}"`,
              { line: closingStart + 1 + cvZone.startIdx }),
          );
        }
      }
    }
  }

  // ── Per-reserved zone check ──
  // Reserved zones present with code → violation. Absent → correct (no result).
  if (reservedClosingZones) {
    // Check for X-section doc zones in closing (X1-X6 patterns)
    const xPattern = /^\/\/\s*(X\d+)\s*[—–-]/;
    const xZones: Array<{ tag: string; startIdx: number }> = [];
    for (let i = 0; i < closingLines.length; i++) {
      const xMatch = xPattern.exec(closingLines[i]!.trim());
      if (xMatch && !xZones.some((z) => z.tag === xMatch[1]!)) {
        xZones.push({ tag: xMatch[1]!, startIdx: i });
      }
    }

    for (const rsv of reservedClosingZones) {
      const foundZone = xZones.find((z) => z.tag === rsv) ??
                         zones.find((z) => z.tag === rsv);
      if (foundZone) {
        results.push(
          warn(file, `content/closing-reserve/${rsv}`,
            `Closing zone ${rsv} is reserved for ${ctx.subtype ?? "this form"} but is present`,
            { line: closingStart + 1 + foundZone.startIdx }),
        );
      }
    }
  }

  return results;
}

/**
 * Check SETUP content against content schema expectations.
 *
 * Per-section atomic checking — each section is its own point:
 * - import_pattern defined → check imports present (content/setup-section/Imports)
 * - Required sections from structure schema → check presence
 * - Reserved sections present → warn (content/setup-reserve/{section})
 * - Reserved sections absent → no result (health scorer adds +1)
 *
 * Rule namespace: content/setup-section/{section}, content/setup-reserve/{section}
 */
export function checkSetupContentExpectations(
  ctx: BaseFileContext,
  expectations: ContentExpectations,
  _requiredSetupSections?: string[],
  reservedSetupSections?: string[],
): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) return results;

  const setupExp = expectations.SETUP as Record<string, unknown>;
  if (!setupExp) return results;

  const setupLines = getBlockLines(ctx.lines, ctx.blocks, "SETUP");
  if (setupLines.length === 0) return results;

  const setupBlock = ctx.blocks.find((b) => b.name === "SETUP");
  const setupLine = setupBlock?.line ?? 0;

  // ── Per-section content check: Imports ──
  // If content schema defines import_pattern, check imports exist.
  const importPattern = setupExp.import_pattern as Record<string, string> | undefined;
  if (importPattern) {
    const hasImports = setupLines.some((l) => {
      const t = l.trim();
      return t.startsWith("import ") || t.startsWith("import(") ||
             t.startsWith("use ") || t === "import (" ||
             t.startsWith("#include ");
    });

    if (!hasImports) {
      results.push(
        info(file, "content/setup-section/Imports",
          `SETUP has no import declarations — content schema defines import_pattern for ${ctx.subtype ?? "this form"}`,
          { line: setupLine }),
      );
    }
  }

  // ── Per-reserved-section check ──
  // Reserved sections present → warn. Absent → no result (health scorer adds +1).
  // Detect subsections by numbered comment headers: // N. Name or §N — Name
  if (reservedSetupSections && reservedSetupSections.length > 0) {
    const subsectionPattern = /^\/\/\s*(?:§?\s*)?(\d+)\.\s+(.+)/;
    const foundSections: Array<{ name: string; lineIdx: number }> = [];
    for (let i = 0; i < setupLines.length; i++) {
      const match = subsectionPattern.exec(setupLines[i]!.trim());
      if (match) {
        foundSections.push({ name: match[2]!.trim(), lineIdx: i });
      }
    }

    const normalize = (s: string) => s.replace(/[^a-zA-Z0-9]/g, "").toLowerCase();

    for (const rsv of reservedSetupSections) {
      const rsvNorm = normalize(rsv);
      const found = foundSections.find((f) => normalize(f.name).includes(rsvNorm));
      if (found) {
        // Check if it has actual content (not just a header)
        const nextSection = foundSections.find((f) => f.lineIdx > found.lineIdx);
        const sectionEnd = nextSection?.lineIdx ?? setupLines.length;
        const sectionLines = setupLines.slice(found.lineIdx + 1, sectionEnd);
        const codeLines = sectionLines.filter((l) => {
          const t = l.trim();
          return t !== "" && !t.startsWith("//") && !/^\/\/\s*[─=\-]{4,}\s*$/.test(t);
        });

        if (codeLines.length > 0) {
          const fileLine = blockLineToFile(ctx.blocks, "SETUP", found.lineIdx);
          results.push(
            warn(file, `content/setup-reserve/${rsv}`,
              `SETUP section ${found.name} is reserved for ${ctx.subtype ?? "this form"} — has ${codeLines.length} code lines`,
              { line: fileLine }),
          );
        }
      }
      // else: reserved section absent — CORRECT. Health scorer adds +1.
    }
  }

  return results;
}

/**
 * Check METADATA content against content schema expectations.
 *
 * Per-check atomic — each metadata content check is its own point:
 * - Directive matches expected form (content/metadata-check/directive)
 * - Pragma section uses correct identity construct (content/metadata-check/Pragma)
 * - Metadata section uses correct identity construct (content/metadata-check/Metadata)
 *
 * Rule namespace: content/metadata-check/{check}
 */
export function checkMetadataContentExpectations(
  ctx: BaseFileContext,
  expectations: ContentExpectations,
): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) return results;

  const metaExp = expectations.METADATA as Record<string, unknown>;
  if (!metaExp) return results;

  const metaLines = getBlockLines(ctx.lines, ctx.blocks, "METADATA");
  const metaBlock = ctx.blocks.find((b) => b.name === "METADATA");
  const metaLine = metaBlock?.line ?? 0;

  // ── Per-check: directive ──
  const expectedDirective = metaExp.directive as string | undefined;
  if (expectedDirective && ctx.subtype) {
    const codeDirective = ctx.directives.get("//omni:code");
    if (codeDirective) {
      const expectedSuffix = expectedDirective.replace("//omni:code ", "");
      if (codeDirective.value && !codeDirective.value.includes(ctx.subtype)) {
        results.push(
          warn(file, "content/metadata-check/directive",
            `//omni:code directive "${codeDirective.value}" does not match content schema for ${ctx.subtype} (expected: ${expectedSuffix})`,
            { line: codeDirective.line }),
        );
      }
    }
  }

  // ── Per-check: Pragma section ──
  // Verify Pragma identity construct exists in the METADATA block.
  // The construct pattern varies by language (Go: var, Rust: static, C: const struct).
  const identityConstruct = metaExp.identity_construct as string | undefined;
  if (identityConstruct && metaLines.length > 0) {
    const hasPragma = metaLines.some((l) => {
      const t = l.trim();
      return /\bPRAGMA\b|\bPragma\b/.test(t) &&
             (/^(pub\s+)?(var|const|static|let)\b/.test(t) || /^\w+\s+\w+\s*=/.test(t));
    });
    if (!hasPragma) {
      results.push(
        info(file, "content/metadata-check/Pragma",
          `METADATA block missing Pragma identity construct — content schema expects: "${identityConstruct}"`,
          { line: metaLine }),
      );
    }
  }

  // ── Per-check: Metadata section ──
  // Same pattern — verify Metadata identity construct exists.
  if (identityConstruct && metaLines.length > 0) {
    const hasMetadata = metaLines.some((l) => {
      const t = l.trim();
      return /\bMETADATA\b|\bMetadata\b/.test(t) &&
             (/^(pub\s+)?(var|const|static|let)\b/.test(t) || /^\w+\s+\w+\s*=/.test(t));
    });
    if (!hasMetadata) {
      results.push(
        info(file, "content/metadata-check/Metadata",
          `METADATA block missing Metadata identity construct — content schema expects: "${identityConstruct}"`,
          { line: metaLine }),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Non-code pattern compilation — schema-driven pattern matchers
// ---------------------------------------------------------------------------
//
// Bridge between the non-code schema (data layer) and the chunking engine.
// Compiles regex strings from _non-code.jsonc schemas into efficient pattern
// matchers that ChunkerConfig.patterns can consume.
//
// The non-code schema defines WHAT to detect. The chunker defines HOW to group.
// This bridge connects both without either needing to know the other's internals.
//

/**
 * Compiled non-code patterns ready for use by the chunking engine
 * and structural analysis.
 *
 * Built from ComposedNonCode (universal + language overlay). Language-specific
 * categories (Rust: inner_doc, attribute; Go: build_tag, compiler_directive)
 * are automatically available through the composed non-code merge.
 */
export interface CompiledNonCodePatterns {
  /** Test if a line is a pragma directive. */
  isPragma: (trimmed: string) => boolean;
  /** Test if a line is a separator banner. */
  isSeparator: (trimmed: string) => boolean;
  /** Test if a line is a regular comment (not doc, not pragma, not separator). */
  isComment: (trimmed: string) => boolean;
  /** Test if a line is a blank/whitespace-only line. */
  isBlank: (trimmed: string) => boolean;
  /** Test if a line is an outer doc comment (/// in Rust). Pattern-based only. */
  isItemDoc: (trimmed: string) => boolean;
  /** Test if a line is a file-level doc comment (//! in Rust). */
  isFileDoc: (trimmed: string) => boolean;
  /** Test if a line is an outer attribute (#[...] in Rust). */
  isAttribute: (trimmed: string) => boolean;
  /** Test if a line is an inner attribute (#![...] in Rust). */
  isInnerAttribute: (trimmed: string) => boolean;
  /** Test if a line is a build tag (//go:build in Go). */
  isBuildTag: (trimmed: string) => boolean;
  /** Test if a line is a compiler directive (//go:noinline in Go). */
  isCompilerDirective: (trimmed: string) => boolean;
  /** Test if a line is a generate directive (//go:generate in Go). */
  isGenerateDirective: (trimmed: string) => boolean;
  /** Classification priority order from schema. */
  classificationPriority: string[];
  /** All compiled category matchers keyed by category name. */
  categoryMatchers: Map<string, RegExp[]>;
}

/**
 * Compile a single ChunkCategory into RegExp matchers.
 *
 * Handles four pattern sources:
 *   - `pattern`: single regex string (skip `<language_specific>` placeholder)
 *   - `start_pattern`: primary start pattern
 *   - `continuation`: continuation pattern for multi-line categories
 *   - `patterns`: array of regex strings
 *
 * @param cat Category definition from schema (may be undefined)
 * @returns Array of compiled RegExp (empty if no valid patterns)
 */
export function compileCategoryPatterns(cat: ChunkCategory | undefined): RegExp[] {
  if (!cat) return [];
  const regexes: RegExp[] = [];

  if (cat.pattern && cat.pattern !== "<language_specific>") {
    regexes.push(new RegExp(cat.pattern));
  }
  if (cat.start_pattern) {
    regexes.push(new RegExp(cat.start_pattern));
  }
  if (cat.continuation) {
    regexes.push(new RegExp(cat.continuation));
  }
  if (cat.patterns) {
    for (const p of cat.patterns) {
      regexes.push(new RegExp(p));
    }
  }

  return regexes;
}

/**
 * Compile non-code schema patterns into efficient pattern matchers.
 *
 * Takes the composed (universal + language) non-code definitions and produces
 * pattern matchers ready for the chunking engine and structural analysis.
 *
 * Key behaviors:
 * - Go doc_comment has `detection_method: "contextual"` → isItemDoc returns false
 *   (Go uses proximity, not syntax, for doc comment detection)
 * - Language-specific categories (inner_doc, attribute, build_tag, etc.) are
 *   automatically available through the composed merge
 * - Classification priority comes from the schema's `classification_priority` rule
 *
 * @param composed Composed non-code definitions (from loadNonCode())
 * @returns Compiled pattern matchers
 */
export function compileNonCodePatterns(composed: ComposedNonCode): CompiledNonCodePatterns {
  const cats = composed.allCategories;

  function matchesAny(patterns: RegExp[], line: string): boolean {
    return patterns.some((p) => p.test(line));
  }

  // Build per-category compiled patterns
  const pragmaRe = compileCategoryPatterns(cats["pragma"]);
  const separatorRe = compileCategoryPatterns(cats["separator"]);
  const commentRe = compileCategoryPatterns(cats["comment"]);
  const blankRe = compileCategoryPatterns(cats["blank"]);
  const docCommentRe = compileCategoryPatterns(cats["doc_comment"]);
  const innerDocRe = compileCategoryPatterns(cats["inner_doc"]);
  const attributeRe = compileCategoryPatterns(cats["attribute"]);
  const innerAttributeRe = compileCategoryPatterns(cats["inner_attribute"]);
  const buildTagRe = compileCategoryPatterns(cats["build_tag"]);
  const compilerDirectiveRe = compileCategoryPatterns(cats["compiler_directive"]);
  const generateDirectiveRe = compileCategoryPatterns(cats["generate_directive"]);

  // Build the full category matcher map for priority-based classification
  const categoryMatchers = new Map<string, RegExp[]>();
  for (const [name, cat] of Object.entries(cats)) {
    categoryMatchers.set(name, compileCategoryPatterns(cat));
  }

  // Get classification priority from schema
  const priorityRule = composed.allRules["classification_priority"];
  const classificationPriority = priorityRule?.order ??
    ["pragma", "separator", "doc_comment", "comment", "blank"];

  // Go doc_comment uses contextual detection (proximity to declaration), not
  // pattern matching. When detection_method is "contextual", isItemDoc returns
  // false — the chunker delegates to post-classification attachment logic.
  const isDocContextual = cats["doc_comment"]?.detection_method === "contextual";

  return {
    isPragma: (trimmed) => matchesAny(pragmaRe, trimmed),
    isSeparator: (trimmed) => matchesAny(separatorRe, trimmed),
    isComment: (trimmed) => matchesAny(commentRe, trimmed),
    isBlank: (trimmed) => matchesAny(blankRe, trimmed),
    isItemDoc: isDocContextual
      ? () => false
      : (trimmed) => matchesAny(docCommentRe, trimmed),
    isFileDoc: innerDocRe.length > 0
      ? (trimmed) => matchesAny(innerDocRe, trimmed)
      : () => false,
    isAttribute: attributeRe.length > 0
      ? (trimmed) => matchesAny(attributeRe, trimmed)
      : () => false,
    isInnerAttribute: innerAttributeRe.length > 0
      ? (trimmed) => matchesAny(innerAttributeRe, trimmed)
      : () => false,
    isBuildTag: buildTagRe.length > 0
      ? (trimmed) => matchesAny(buildTagRe, trimmed)
      : () => false,
    isCompilerDirective: compilerDirectiveRe.length > 0
      ? (trimmed) => matchesAny(compilerDirectiveRe, trimmed)
      : () => false,
    isGenerateDirective: generateDirectiveRe.length > 0
      ? (trimmed) => matchesAny(generateDirectiveRe, trimmed)
      : () => false,
    classificationPriority,
    categoryMatchers,
  };
}

/**
 * Build ChunkerConfig.patterns from compiled non-code patterns.
 *
 * This bridges the schema-driven non-code categories to the universal
 * chunking engine's pattern interface. Handlers call this instead of
 * hardcoding pattern functions.
 *
 * The `testBlock` pattern is NOT in the non-code schema — test module
 * detection is language-specific (Rust: #[cfg(test)], Go: individual funcs).
 * Handlers provide their own testBlock implementation.
 *
 * @param compiled Compiled non-code patterns (from compileNonCodePatterns)
 * @param testBlock Optional handler-specific test block detector
 * @returns Pattern functions for ChunkerConfig.patterns
 */
export function buildChunkerPatterns(
  compiled: CompiledNonCodePatterns,
  testBlock?: (trimmed: string) => boolean,
): ChunkerConfig["patterns"] {
  return {
    pragma: compiled.isPragma,
    fileDoc: compiled.isFileDoc,
    separator: compiled.isSeparator,
    comment: compiled.isComment,
    itemDoc: compiled.isItemDoc,
    attribute: (trimmed) => compiled.isAttribute(trimmed) || compiled.isInnerAttribute(trimmed),
    testBlock: testBlock ?? (() => false),
  };
}

/**
 * Classify a line using schema-driven priority order.
 *
 * Walks the classification_priority from highest to lowest, testing
 * each category's compiled patterns. Returns the first match, or null
 * if no category matches (line is executable code).
 *
 * The priority order comes from the schema's classification_priority rule:
 * pragma → separator → doc_comment → comment → blank (+ language-specific
 * categories inserted by the language overlay).
 *
 * @param compiled Compiled non-code patterns
 * @param trimmed Trimmed line to classify
 * @returns Category name (e.g., "pragma", "separator") or null for code
 */
export function classifyNonCodeLine(
  compiled: CompiledNonCodePatterns,
  trimmed: string,
): string | null {
  for (const category of compiled.classificationPriority) {
    const patterns = compiled.categoryMatchers.get(category);
    if (patterns && patterns.some((p) => p.test(trimmed))) {
      return category;
    }
  }
  return null;
}

// ---------------------------------------------------------------------------
// Compiled non-code pattern cache
// ---------------------------------------------------------------------------

const _compiledPatternsCache = new Map<string, CompiledNonCodePatterns>();

/**
 * Load non-code definitions for a language and compile them into matchers.
 * Results are cached by language string — same language returns the same
 * compiled patterns without recompilation.
 *
 * This is the preferred entry point for handlers. Combines:
 *   loadNonCode(language) → compileNonCodePatterns(composed) → cache
 *
 * @param language Format language ("go", "rust", etc.)
 * @returns Compiled pattern matchers ready for chunking/classification
 */
export async function loadCompiledNonCodePatterns(
  language: string,
): Promise<CompiledNonCodePatterns> {
  const cached = _compiledPatternsCache.get(language);
  if (cached) return cached;

  const composed = await loadNonCode(language);
  const compiled = compileNonCodePatterns(composed);
  _compiledPatternsCache.set(language, compiled);
  return compiled;
}

/** Clear the compiled non-code patterns cache. */
export function clearCompiledPatternsCache(): void {
  _compiledPatternsCache.clear();
}
registerCache("shared/compiled-non-code-patterns", clearCompiledPatternsCache);

// ============================================================================
// CLOSING
// ============================================================================
//
// Shared 4-block functions — the identical logic that Go and Rust handlers
// both needed. Extracted here so changes propagate to both automatically.
//
// Non-code pattern compilation bridges the schema data layer to the chunking
// engine. compileNonCodePatterns() turns schema regex strings into efficient
// matchers. buildChunkerPatterns() maps them to ChunkerConfig's interface.
// classifyNonCodeLine() applies schema-driven priority classification.
//
// Content linting completes the pentagon: Schema = Template = Linter =
// Transpiler = AST Concept. Structure validates the skeleton. Content
// validates the flesh. Both driven by the same schemas.
//
// "That which hath been is now; and that which is to be hath already been."
// — Ecclesiastes 3:15
// ============================================================================
