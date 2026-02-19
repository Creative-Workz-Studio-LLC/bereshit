// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/handlers/shared/code-4block.ts
// key:     B-tov-cws-struct-lib-handlers-shared-code-4block
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
  BLOCKS, BLOCK_PATTERNS, END_PATTERNS, CLOSING_ZONES,
  BLOCK_SEPARATOR_WIDTH, SUBSECTION_SEPARATOR_WIDTH,
  CLOSING_DOC_REQUIREMENTS, X1_FIELD_PATTERNS, X5_FIELD_PATTERNS,
} from "./types.ts";

import type { LintResult } from "../../foundation/mod.ts";
import { error, warn, info } from "../../foundation/mod.ts";

import type { IdentityField, FieldContentRule } from "./types.ts";

// ============================================================================
// BODY
// ============================================================================

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

      // Look for block header: "// METADATA", "// SETUP", etc.
      if (blockName === "METADATA") {
        if (/^\/\/\s+METADATA\s*$/.test(trimmed) && headerLine === 0) {
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

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();

    // Block header: `// BLOCKNAME` between separators
    if (trimmed === `// ${blockName}` && headerLine < 0) {
      const above = i > 0 ? lines[i - 1]!.trim() : "";
      if (/^\/\/\s+={10,}$/.test(above)) {
        headerLine = i;
        continue;
      }
    }

    // END marker: `// END BLOCKNAME`
    if (trimmed === `// END ${blockName}` && headerLine >= 0 && endLine < 0) {
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
    if (dominant !== BLOCK_SEPARATOR_WIDTH) {
      results.push(
        info(file, "style/eq-separator-standard",
          `Block separators are ${dominant} chars wide (standard: ${BLOCK_SEPARATOR_WIDTH})`,
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
    if (dominant !== SUBSECTION_SEPARATOR_WIDTH) {
      results.push(
        info(file, "style/box-separator-standard",
          `${blockName} ─ separators are ${dominant} chars wide (standard: ${SUBSECTION_SEPARATOR_WIDTH})`,
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
export function checkClosingZoneOrder(ctx: BaseFileContext): LintResult[] {
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

    for (const zone of CLOSING_ZONES) {
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
  const codeOrder: string[] = CLOSING_ZONES.filter((z) => z.kind === "code").map((z) => z.tag);

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
  const docOrder: string[] = CLOSING_ZONES.filter((z) => z.kind === "doc").map((z) => z.tag);

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
// I/C field content validation — value checks (shared across all 4-block handlers)
// ---------------------------------------------------------------------------

/**
 * Validate the CONTENT of parsed I/C fields against content rules.
 *
 * Existence checks ask "is the field present?" (validateICFields in each handler).
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
export function checkClosingRequiredZones(ctx: BaseFileContext): LintResult[] {
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
    for (const zone of CLOSING_ZONES) {
      if (zone.kind === "doc" && zone.pattern.test(trimmed)) {
        presentTags.add(zone.tag);
        break;
      }
    }
  }

  // Check required zones
  for (const req of CLOSING_DOC_REQUIREMENTS) {
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
export function checkClosingZoneContent(ctx: BaseFileContext): LintResult[] {
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

    for (const zone of CLOSING_ZONES) {
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

  // Check X1 content
  const x1 = zoneRanges.find((z) => z.tag === "X1");
  if (x1) {
    const x1Lines = closingLines.slice(x1.startIdx, x1.endIdx);
    const foundFields = new Set<string>();

    for (const line of x1Lines) {
      const trimmed = line.trim();
      for (const [field, pattern] of Object.entries(X1_FIELD_PATTERNS)) {
        if (pattern.test(trimmed)) {
          foundFields.add(field);
        }
      }
    }

    const required = CLOSING_DOC_REQUIREMENTS.find((r) => r.tag === "X1");
    if (required?.fields) {
      const missing = required.fields.required.filter((f) => !foundFields.has(f));
      if (missing.length > 0) {
        results.push(
          info(file, "closing/X1-content",
            `X1 Policy zone missing required fields: ${missing.join(", ")} (expected: Never, Careful, Safe)`,
            { line: closingStart + 1 + x1.startIdx }),
        );
      }
    }
  }

  // Check X5 content
  const x5 = zoneRanges.find((z) => z.tag === "X5");
  if (x5) {
    const x5Lines = closingLines.slice(x5.startIdx, x5.endIdx);
    const foundFields = new Set<string>();

    for (const line of x5Lines) {
      const trimmed = line.trim();
      for (const [field, pattern] of Object.entries(X5_FIELD_PATTERNS)) {
        if (pattern.test(trimmed)) {
          foundFields.add(field);
        }
      }
    }

    const required = CLOSING_DOC_REQUIREMENTS.find((r) => r.tag === "X5");
    if (required?.fields) {
      const missing = required.fields.required.filter((f) => !foundFields.has(f));
      if (missing.length > 0) {
        results.push(
          info(file, "closing/X5-content",
            `X5 Note zone missing required fields: ${missing.join(", ")} (expected: note, scripture)`,
            { line: closingStart + 1 + x5.startIdx }),
        );
      }
    }
  }

  return results;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Shared 4-block functions — the identical logic that Go and Rust handlers
// both needed. Extracted here so changes propagate to both automatically.
//
// "That which hath been is now; and that which is to be hath already been."
// — Ecclesiastes 3:15
// ============================================================================
