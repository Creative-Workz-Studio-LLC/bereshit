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
  BLOCKS, BLOCK_PATTERNS, END_PATTERNS,
  BLOCK_SEPARATOR_WIDTH, SUBSECTION_SEPARATOR_WIDTH,
  BODY_SUBSECTION_PATTERN, BODY_SUBSECTION_LEGACY,
  SCALING_THRESHOLDS,
} from "./types.ts";

import type { LintResult } from "../../foundation/mod.ts";
import type { SchemaSubsectionDef, SchemaBodySubtype, SchemaClosingData } from "../../foundation/mod.ts";
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

  const results: LintResult[] = [];
  const setupBlock = blocks.find((b) => b.name === "SETUP");

  for (const sub of setupData) {
    if (sub.requiredIn.includes(subtype) && !presentTags.has(sub.tag)) {
      results.push(
        warn(file, "setup/required-subsection",
          `SETUP missing subsection "${sub.tag}" — required for ${subtype} subtype`,
          { line: setupBlock?.line ?? 0 }),
      );
    }
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

  const results: LintResult[] = [];
  const bodyBlock = blocks.find((b) => b.name === "BODY");

  for (const sub of bodySubtype.subsections) {
    if (!sub.requiredIn.includes(subtype)) continue;

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
