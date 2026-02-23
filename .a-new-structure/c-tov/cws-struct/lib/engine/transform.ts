// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/transform.ts
// key:     B-tov-cws-struct-lib-engine-transform
// title:   CWS Struct — Shared Transform Pipeline
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Generic transform engine — schema-driven, handler-parameterized.
//          Same architecture as the lint pipeline: generic engine discovers
//          and applies universal transforms, delegates language-specific
//          transforms to handler-provided passes.
//
//          Universal transforms (identical across languages):
//            - Fix block separator widths (= chars)
//            - Fix subsection separator widths (─ chars)
//            - Convert ASCII dash separators to Unicode ─
//            - Normalize separator prefix to `// `
//            - Reorder CLOSING zones to canonical order
//
//          Parameterized transforms (same logic, different detectors):
//            - Move test code from BODY to CLOSING Cv
//            - Move main entrypoint from BODY to CLOSING Ce
//            - Scaffold missing subsection headers
//
//          Handler-specific transforms register as TransformPass objects.
//          The pipeline runs them in order after universal passes.
//
// biblical_foundation: "Every purpose is established by counsel."
//   — Proverbs 20:18
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Imports
// ---------------------------------------------------------------------------

import type { LintResult, TransformOptions } from "../foundation/mod.ts";
import type { Code4BlockRules } from "../foundation/mod.ts";
import { info, error } from "../foundation/mod.ts";
import type { SubsectionDef } from "../shared/mod.ts";
import {
  BLOCK_SEPARATOR_WIDTH,
  SUBSECTION_SEPARATOR_WIDTH,
  findBlocks,
  findBlockRange,
  getSubsectionRanges,
} from "../shared/mod.ts";

// ---------------------------------------------------------------------------
// 2. Types
// ---------------------------------------------------------------------------

/**
 * Mutable context for a transform pipeline run.
 *
 * Passed through each TransformPass. Passes mutate `lines` in place
 * and push results. The pipeline runner manages `modified`/`wouldModify`
 * tracking — passes just do their work.
 */
export interface TransformContext {
  /** Absolute path to the file being transformed. */
  filePath: string;

  /** Mutable line array — passes splice/replace in place. */
  lines: string[];

  /** Schema-derived rules for this format. */
  rules: Code4BlockRules;

  /** CLI options (dryRun, extensions, force, steps). */
  opts: TransformOptions;

  /** Accumulates LintResult diagnostics. */
  results: LintResult[];

  /** Set true when lines have been modified (non-dry-run). */
  modified: boolean;

  /** Set true when lines WOULD be modified (dry-run). */
  wouldModify: boolean;
}

/**
 * A single transform operation.
 *
 * Like a lint check but for fixes. Each pass is self-contained:
 * it reads from ctx, mutates ctx.lines if needed, pushes results.
 *
 * The pipeline runs passes in order — earlier passes affect later ones
 * (e.g., scaffold first, then fix separators).
 */
export interface TransformPass {
  /** Human-readable name for diagnostics (e.g., "fix-block-separator-widths"). */
  readonly name: string;

  /** If true, only runs when --extensions is set. */
  readonly requiresExtensions?: boolean;

  /** Execute the transform. */
  apply(ctx: TransformContext): void | Promise<void>;
}

/**
 * Detects a range of code within a block.
 *
 * Used for parameterized code movement (test→Cv, main→Ce).
 * Language-specific: Go detects `func TestXxx(`, Rust detects `#[cfg(test)]`.
 *
 * @returns Range {start, end} (inclusive line indices) or null if not found.
 */
export type CodeRangeDetector = (
  lines: string[],
  rangeStart: number,
  rangeEnd: number,
) => { start: number; end: number } | null;

/**
 * Specification for moving code from one block to another.
 *
 * Parameterizes the test/main move transforms. Handler provides
 * the detector and zone config; the engine does extraction/insertion.
 */
export interface CodeMoveSpec {
  /** Human-readable description for diagnostics. */
  description: string;

  /** Code detection function (language-specific). */
  detector: CodeRangeDetector;

  /** Source block to extract from (e.g., "BODY"). */
  sourceBlock: string;

  /** Target block to insert into (e.g., "CLOSING"). */
  targetBlock: string;

  /** Zone tag for the wrapper comment (e.g., "Cv", "Ce"). */
  zoneTag: string;

  /** Zone label for the comment header (e.g., "Closing Validation"). */
  zoneLabel: string;

  /**
   * Whether to loop until no more matches.
   *
   * true for Go test functions (multiple individual funcs).
   * false for Rust #[cfg(test)] (single module block).
   */
  loop: boolean;

  /** Transform result tag prefix (e.g., "transform/move-tests"). */
  resultTag: string;
}

/**
 * Specification for scaffolding missing subsection headers.
 *
 * Handler provides the line classifier; the engine does the scaffolding.
 */
export interface SubsectionScaffoldSpec {
  /** Target block to scaffold in (e.g., "SETUP"). */
  block: string;

  /** Canonical subsection order — tags in expected sequence. */
  canonicalOrder: string[];

  /**
   * Subsection definitions with compiled regexes.
   * Needed by getSubsectionRanges() to detect existing headers.
   * Source: Code4BlockRules.setupSubsections (schema-driven).
   */
  subsectionDefs: readonly SubsectionDef[];

  /**
   * Classify a non-comment, non-blank line into a content kind.
   * Returns the content kind string (e.g., "import", "const", "type").
   */
  classifyLine: (trimmed: string) => string;

  /**
   * Map content kind → target subsection tag.
   * (e.g., "import" → "Imports", "const" → "Constants").
   */
  kindToSubsection: Record<string, string>;

  /**
   * Optional form-aware filter — only scaffold sections with these statuses.
   * When provided, sections whose tag maps to "reserved" in the form are skipped.
   * Map of section tag → form status ("required" | "available" | "reserved").
   * Source: FORM_REGISTRY[formName].blocks[block].
   */
  formStatus?: Record<string, string>;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Universal transforms — identical across all 4-block languages
// ---------------------------------------------------------------------------

/**
 * Fix block separator widths (= chars).
 *
 * Normalizes `// ====...====` lines to exactly BLOCK_SEPARATOR_WIDTH characters.
 * Source: shared/types.ts → BLOCK_SEPARATOR_WIDTH (76).
 */
export const fixBlockSeparatorWidths: TransformPass = {
  name: "fix-block-separator-widths",

  apply(ctx: TransformContext): void {
    const { filePath, lines, opts, results } = ctx;
    for (let i = 0; i < lines.length; i++) {
      const trimmed = lines[i]!.trim();
      const eqMatch = trimmed.match(/^(\/\/\s+)(={4,})(\s*)$/);
      if (eqMatch && eqMatch[2]!.length !== BLOCK_SEPARATOR_WIDTH) {
        const newLine = `${eqMatch[1]}${"=".repeat(BLOCK_SEPARATOR_WIDTH)}`;
        if (opts.dryRun) {
          ctx.wouldModify = true;
          results.push(info(filePath, "transform/eq-width",
            `Line ${i + 1}: would fix block separator ${eqMatch[2]!.length} → ${BLOCK_SEPARATOR_WIDTH} chars`));
        } else {
          lines[i] = newLine;
          ctx.modified = true;
          results.push(info(filePath, "transform/eq-width",
            `Line ${i + 1}: fixed block separator ${eqMatch[2]!.length} → ${BLOCK_SEPARATOR_WIDTH} chars`));
        }
      }
    }
  },
};

/**
 * Fix subsection separator widths (─ chars).
 *
 * Normalizes `// ─────...─────` lines to exactly SUBSECTION_SEPARATOR_WIDTH characters.
 * Source: shared/types.ts → SUBSECTION_SEPARATOR_WIDTH (74).
 */
export const fixSubsectionSeparatorWidths: TransformPass = {
  name: "fix-subsection-separator-widths",

  apply(ctx: TransformContext): void {
    const { filePath, lines, opts, results } = ctx;
    for (let i = 0; i < lines.length; i++) {
      const trimmed = lines[i]!.trim();
      const boxMatch = trimmed.match(/^(\/\/\s+)(─{4,})(\s*)$/);
      if (boxMatch && boxMatch[2]!.length !== SUBSECTION_SEPARATOR_WIDTH) {
        const newLine = `${boxMatch[1]}${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`;
        if (opts.dryRun) {
          ctx.wouldModify = true;
          results.push(info(filePath, "transform/box-width",
            `Line ${i + 1}: would fix subsection separator ${boxMatch[2]!.length} → ${SUBSECTION_SEPARATOR_WIDTH} ─ chars`));
        } else {
          lines[i] = newLine;
          ctx.modified = true;
          results.push(info(filePath, "transform/box-width",
            `Line ${i + 1}: fixed subsection separator ${boxMatch[2]!.length} → ${SUBSECTION_SEPARATOR_WIDTH} ─ chars`));
        }
      }
    }
  },
};

/**
 * Convert ASCII dash separators to Unicode ─.
 *
 * Matches: `// --------` or `//--------` or `//  --------`
 * Converts to: `// ──────── (SUBSECTION_SEPARATOR_WIDTH ─ chars)`
 * Also normalizes prefix to standard `// ` (single space).
 */
export const convertAsciiToUnicode: TransformPass = {
  name: "convert-ascii-to-unicode",

  apply(ctx: TransformContext): void {
    const { filePath, lines, opts, results } = ctx;
    for (let i = 0; i < lines.length; i++) {
      const trimmed = lines[i]!.trim();
      const dashMatch = trimmed.match(/^(\/\/\s*)(-{4,})(\s*)$/);
      if (dashMatch) {
        const newLine = `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`;
        if (opts.dryRun) {
          ctx.wouldModify = true;
          results.push(info(filePath, "transform/dash-to-unicode",
            `Line ${i + 1}: would convert ${dashMatch[2]!.length} ASCII dashes → ${SUBSECTION_SEPARATOR_WIDTH} Unicode ─`));
        } else {
          lines[i] = newLine;
          ctx.modified = true;
          results.push(info(filePath, "transform/dash-to-unicode",
            `Line ${i + 1}: converted ${dashMatch[2]!.length} ASCII dashes → ${SUBSECTION_SEPARATOR_WIDTH} Unicode ─`));
        }
      }
    }
  },
};

/**
 * Normalize subsection separator prefix to `// `.
 *
 * Ensures exactly one space after `//` for separators:
 *   `//  ────` → `// ────`
 *   `//────`   → `// ────`
 */
export const normalizeSeparatorPrefix: TransformPass = {
  name: "normalize-separator-prefix",

  apply(ctx: TransformContext): void {
    const { filePath, lines, opts, results } = ctx;
    for (let i = 0; i < lines.length; i++) {
      const trimmed = lines[i]!.trim();
      const prefixMatch = trimmed.match(/^\/\/(\s{0}|\s{2,})(─{4,}|={4,})(\s*)$/);
      if (prefixMatch) {
        const newLine = `// ${prefixMatch[2]}`;
        if (opts.dryRun) {
          ctx.wouldModify = true;
          results.push(info(filePath, "transform/prefix-normalize",
            `Line ${i + 1}: would normalize separator prefix to standard '// '`));
        } else if (lines[i]!.trim() !== newLine) {
          lines[i] = newLine;
          ctx.modified = true;
          const sepChar = prefixMatch[2]!.charAt(0);
          results.push(info(filePath, "transform/prefix-normalize",
            `Line ${i + 1}: normalized separator prefix to standard '// ' (${sepChar} separator)`));
        }
      }
    }
  },
};

// ---------------------------------------------------------------------------
// 2. Parameterized transforms — same logic, handler-provided detectors
// ---------------------------------------------------------------------------

/**
 * Create a transform pass that moves code from one block to another.
 *
 * Handles both single-extraction (Rust #[cfg(test)]) and loop-extraction
 * (Go test functions). Wraps extracted code in a zone with separators.
 *
 * The zone structure is schema-aware — zone tags and labels come from
 * the handler's schema data, not hardcoded constants.
 */
export function createCodeMovePass(spec: CodeMoveSpec): TransformPass {
  return {
    name: `move-${spec.zoneTag.toLowerCase()}`,

    apply(ctx: TransformContext): void {
      const { filePath, lines, opts, results } = ctx;

      // Resolve zone position from schema for data-referenced messages
      const zoneIdx = ctx.rules.closingData.zones.findIndex((z) => z.tag === spec.zoneTag);
      const zonePosLabel = zoneIdx >= 0
        ? ` (zone ${zoneIdx + 1}, per section registry)`
        : "";

      if (spec.loop) {
        // Loop mode: find and extract multiple matches (e.g., Go test funcs)
        let moveCount = 0;
        const allExtracted: string[][] = [];

        while (true) {
          const sourceBlock = findBlockRange(lines, spec.sourceBlock);
          const targetBlock = findBlockRange(lines, spec.targetBlock);
          if (!sourceBlock || !targetBlock) break;

          const range = spec.detector(lines, sourceBlock.contentStart, sourceBlock.contentEnd);
          if (!range) break;

          // Include preceding blank/comment lines
          let extractStart = range.start;
          while (extractStart > sourceBlock.contentStart &&
                 (lines[extractStart - 1]!.trim() === "" ||
                  lines[extractStart - 1]!.trim().startsWith("// WRONG"))) {
            extractStart--;
          }

          const extractedLines = lines.slice(extractStart, range.end + 1);

          if (opts.dryRun) {
            ctx.wouldModify = true;
            results.push(info(filePath, spec.resultTag,
              `Lines ${extractStart + 1}–${range.end + 1}: would move ${spec.description} from ${spec.sourceBlock} to ${spec.targetBlock}.${spec.zoneTag}${zonePosLabel}`));
            break; // Dry-run: report first, don't loop
          }

          // Remove from source (including trailing blank lines)
          let removeEnd = range.end + 1;
          while (removeEnd < sourceBlock.contentEnd && lines[removeEnd]!.trim() === "") {
            removeEnd++;
          }
          lines.splice(extractStart, removeEnd - extractStart);
          allExtracted.push(extractedLines);
          moveCount++;
        }

        // Insert all extracted code into target zone at once
        if (!opts.dryRun && allExtracted.length > 0) {
          const targetBlock = findBlockRange(lines, spec.targetBlock);
          if (targetBlock) {
            const zone = [
              "",
              `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
              `// ${spec.zoneTag} — ${spec.zoneLabel}`,
              `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
              "",
            ];
            for (const extracted of allExtracted) {
              zone.push(...extracted, "");
            }

            lines.splice(targetBlock.contentStart, 0, ...zone);
            ctx.modified = true;
            results.push(info(filePath, spec.resultTag,
              `Moved ${moveCount} ${spec.description}(s) from ${spec.sourceBlock} to ${spec.targetBlock}.${spec.zoneTag}${zonePosLabel}`));
          }
        }
      } else {
        // Single mode: one extraction (e.g., Rust #[cfg(test)])
        const sourceBlock = findBlockRange(lines, spec.sourceBlock);
        const targetBlock = findBlockRange(lines, spec.targetBlock);
        if (!sourceBlock || !targetBlock) return;

        const range = spec.detector(lines, sourceBlock.contentStart, sourceBlock.contentEnd);
        if (!range) return;

        let extractStart = range.start;
        while (extractStart > sourceBlock.contentStart &&
               (lines[extractStart - 1]!.trim() === "" ||
                lines[extractStart - 1]!.trim().startsWith("// WRONG"))) {
          extractStart--;
        }

        const extractedLines = lines.slice(extractStart, range.end + 1);

        const zone = [
          "",
          `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
          `// ${spec.zoneTag} — ${spec.zoneLabel}`,
          `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
          "",
          ...extractedLines.map((l) => l),
        ];

        const insertIdx = targetBlock.contentStart;

        if (opts.dryRun) {
          ctx.wouldModify = true;
          results.push(info(filePath, spec.resultTag,
            `Lines ${extractStart + 1}–${range.end + 1}: would move ${spec.description} from ${spec.sourceBlock} to ${spec.targetBlock}.${spec.zoneTag}${zonePosLabel}`));
        } else {
          // Remove from source
          let removeEnd = range.end + 1;
          while (removeEnd < sourceBlock.contentEnd && lines[removeEnd]!.trim() === "") {
            removeEnd++;
          }
          lines.splice(extractStart, removeEnd - extractStart);
          ctx.modified = true;

          // Recalculate insertion point after removal
          const shift = removeEnd - extractStart;
          const newInsertIdx = insertIdx - shift;

          lines.splice(newInsertIdx, 0, ...zone);

          results.push(info(filePath, spec.resultTag,
            `Moved ${spec.description} from ${spec.sourceBlock} to ${spec.targetBlock}.${spec.zoneTag}${zonePosLabel} (${extractedLines.length} lines)`));
        }
      }
    },
  };
}

// ---------------------------------------------------------------------------
// 3. Schema-driven transforms — reads rules from Code4BlockRules
// ---------------------------------------------------------------------------

/**
 * Reorder CLOSING zones to canonical order.
 *
 * Schema-aware: reads zone ordering from Code4BlockRules.closingData.zones.
 * Canonical order: code zones (Cv, Ce, Cc) then documentation (X1-X6).
 *
 * SHARED between Go and Rust — previously duplicated in each handler.
 * Now a single implementation consuming schema data.
 */
export const reorderClosingZonesPass: TransformPass = {
  name: "reorder-closing-zones",

  apply(ctx: TransformContext): void {
    const { filePath, lines, opts, results, rules } = ctx;
    const closingBlock = findBlockRange(lines, "CLOSING");
    if (!closingBlock) return;

    const reordered = reorderClosingZonesImpl(
      lines.slice(closingBlock.contentStart, closingBlock.contentEnd),
      rules.closingData.zones,
    );
    if (!reordered) return;

    // Capture before/after zone order for data-referenced message
    const beforeOrder = reorderClosingZonesBefore(
      lines.slice(closingBlock.contentStart, closingBlock.contentEnd),
      rules.closingData.zones,
    );
    const canonicalOrder = rules.closingData.zones.map((z) => z.tag);
    const afterOrder = canonicalOrder.filter((t) => beforeOrder.includes(t));
    const orderMsg = `[${beforeOrder.join(", ")}] → [${afterOrder.join(", ")}] (per section registry)`;

    if (opts.dryRun) {
      ctx.wouldModify = true;
      results.push(info(filePath, "transform/reorder-closing",
        `CLOSING zones would be reordered: ${orderMsg}`));
    } else {
      lines.splice(
        closingBlock.contentStart,
        closingBlock.contentEnd - closingBlock.contentStart,
        ...reordered,
      );
      ctx.modified = true;
      results.push(info(filePath, "transform/reorder-closing",
        `Reordered CLOSING zones: ${orderMsg}`));
    }
  },
};

/**
 * Extract the current zone tag order from CLOSING content lines.
 * Used for data-referenced reorder messages.
 */
function reorderClosingZonesBefore(
  closingContent: string[],
  schemaZones: ReadonlyArray<{ tag: string; kind: "code" | "doc"; pattern: RegExp }>,
): string[] {
  const tags: string[] = [];
  for (const line of closingContent) {
    const trimmed = line.trim();
    for (const zone of schemaZones) {
      if (zone.pattern.test(trimmed)) {
        tags.push(zone.tag);
        break;
      }
    }
  }
  return tags;
}

/**
 * Internal: Parse CLOSING content lines into zones, return them in canonical
 * order if reordering is needed. Returns null if already correct.
 *
 * Previously duplicated in go.ts and rust.ts. Now single implementation.
 */
function reorderClosingZonesImpl(
  closingContent: string[],
  schemaZones: ReadonlyArray<{ tag: string; kind: "code" | "doc"; pattern: RegExp }>,
): string[] | null {
  interface ZoneChunk {
    tag: string;
    kind: "code" | "doc";
    lines: string[];
    canonicalIdx: number;
  }

  const canonicalOrder = schemaZones.map((z) => z.tag);

  const zones: ZoneChunk[] = [];
  let preambleLines: string[] = [];
  let currentZone: ZoneChunk | null = null;

  for (let i = 0; i < closingContent.length; i++) {
    const trimmed = closingContent[i]!.trim();

    let matchedZone: { tag: string; kind: "code" | "doc" } | null = null;
    for (const zone of schemaZones) {
      if (zone.pattern.test(trimmed)) {
        matchedZone = { tag: zone.tag, kind: zone.kind };
        break;
      }
    }

    if (matchedZone) {
      if (currentZone) {
        zones.push(currentZone);
      }

      const newZoneLines: string[] = [];

      // Grab the separator line that precedes this zone header
      if (i > 0 && /^\/\/\s*[─=\-]{10,}\s*$/.test(closingContent[i - 1]!.trim())) {
        if (currentZone && currentZone.lines.length > 0) {
          newZoneLines.push(currentZone.lines.pop()!);
        } else if (preambleLines.length > 0) {
          newZoneLines.push(preambleLines.pop()!);
        }
        // Also grab the blank line before the separator
        if (currentZone && currentZone.lines.length > 0 &&
            currentZone.lines[currentZone.lines.length - 1]!.trim() === "") {
          newZoneLines.unshift(currentZone.lines.pop()!);
        } else if (!currentZone && preambleLines.length > 0 &&
                   preambleLines[preambleLines.length - 1]!.trim() === "") {
          newZoneLines.unshift(preambleLines.pop()!);
        }
      }

      newZoneLines.push(closingContent[i]!);

      currentZone = {
        tag: matchedZone.tag,
        kind: matchedZone.kind,
        lines: newZoneLines,
        canonicalIdx: canonicalOrder.indexOf(matchedZone.tag),
      };
    } else if (currentZone) {
      currentZone.lines.push(closingContent[i]!);
    } else {
      preambleLines.push(closingContent[i]!);
    }
  }

  if (currentZone) {
    zones.push(currentZone);
  }

  if (zones.length < 2) return null;

  // Check if already in canonical order
  let inOrder = true;
  for (let i = 1; i < zones.length; i++) {
    if (zones[i]!.canonicalIdx < zones[i - 1]!.canonicalIdx) {
      inOrder = false;
      break;
    }
  }

  if (inOrder) return null;

  // Sort by canonical index
  zones.sort((a, b) => a.canonicalIdx - b.canonicalIdx);

  // Reassemble
  const result: string[] = [...preambleLines];
  for (const zone of zones) {
    result.push(...zone.lines);
  }

  return result;
}

/**
 * Create a transform pass that scaffolds missing subsection headers.
 *
 * Schema-aware: reads subsection definitions from Code4BlockRules.
 * When content exists in a block without a subsection header above it,
 * scaffolds the appropriate header.
 *
 * Only runs with --extensions (growth mode: scaffold toward fullness).
 */
export function createSubsectionScaffoldPass(spec: SubsectionScaffoldSpec): TransformPass {
  return {
    name: `scaffold-${spec.block.toLowerCase()}-subsections`,
    requiresExtensions: true,

    apply(ctx: TransformContext): void {
      const { filePath, lines, opts, results } = ctx;

      const blocks = findBlocks(lines);
      const targetBlock = blocks.find((b) => b.name === spec.block);
      if (!targetBlock) return;

      const blockRange = findBlockRange(lines, spec.block);
      if (!blockRange) return;

      const blockLines = lines.slice(blockRange.contentStart, blockRange.contentEnd);
      const subsections = getSubsectionRanges(blockLines, spec.subsectionDefs);

      // Find declarations that appear before the first subsection header
      const firstSubIdx = subsections.length > 0 ? subsections[0]!.startIdx : blockLines.length;
      const orphanedKinds = new Map<string, number>();

      for (let i = 0; i < firstSubIdx; i++) {
        const trimmed = blockLines[i]!.trim();
        if (trimmed === "" || trimmed.startsWith("//")) continue;
        const kind = spec.classifyLine(trimmed);
        const targetSub = spec.kindToSubsection[kind];
        if (targetSub && !orphanedKinds.has(targetSub)) {
          // Form-aware: skip reserved sections
          if (spec.formStatus && spec.formStatus[targetSub] === "reserved") continue;
          orphanedKinds.set(targetSub, i);
        }
      }

      if (orphanedKinds.size === 0) return;

      // Sort by expected canonical order
      const sorted = [...orphanedKinds.entries()]
        .sort((a, b) =>
          spec.canonicalOrder.indexOf(a[0]) - spec.canonicalOrder.indexOf(b[0]),
        );

      // Insert headers bottom-up to keep indices stable
      let insertCount = 0;
      for (let si = sorted.length - 1; si >= 0; si--) {
        const [tag] = sorted[si]!;
        const num = spec.canonicalOrder.indexOf(tag) + 1;
        const header = [
          "",
          `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
          `// ${num}. ${tag}`,
          `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
          "",
        ];
        const absoluteIdx = blockRange.contentStart + sorted[si]![1];

        if (opts.dryRun) {
          ctx.wouldModify = true;
          results.push(info(filePath, "transform/reserve-scaffold",
            `Would scaffold "${tag}" (position ${num}, per section registry) before line ${absoluteIdx + 1}`));
        } else {
          lines.splice(absoluteIdx, 0, ...header);
          ctx.modified = true;
          insertCount++;
        }
      }
      if (!opts.dryRun && insertCount > 0) {
        results.push(info(filePath, "transform/reserve-scaffold",
          `Scaffolded ${insertCount} missing ${spec.block} subsection header(s)`));
      }
    },
  };
}

// ---------------------------------------------------------------------------
// 3b. Subsection reorder pass — reorder SETUP/BODY subsections to canonical order
// ---------------------------------------------------------------------------

/**
 * Specification for reordering subsection headers in a block.
 *
 * When existing subsection headers appear out of canonical order,
 * this pass generates a single batch reorder instead of individual moves.
 */
export interface SubsectionReorderSpec {
  /** Target block (e.g., "SETUP"). */
  block: string;

  /** Canonical subsection order — tags in expected sequence. */
  canonicalOrder: string[];

  /** Subsection definitions with compiled regexes for detection. */
  subsectionDefs: readonly SubsectionDef[];
}

/**
 * Create a transform pass that reorders subsection headers to canonical order.
 *
 * Detects existing subsection headers, checks if they're in canonical order,
 * and if not, extracts each subsection (header + content until next header)
 * and reassembles in canonical order. Generates ONE reorder operation.
 *
 * Only runs with --extensions (structural reorganization).
 */
export function createSubsectionReorderPass(spec: SubsectionReorderSpec): TransformPass {
  return {
    name: `reorder-${spec.block.toLowerCase()}-subsections`,
    requiresExtensions: true,

    apply(ctx: TransformContext): void {
      const { filePath, lines, opts, results } = ctx;

      const blockRange = findBlockRange(lines, spec.block);
      if (!blockRange) return;

      const blockLines = lines.slice(blockRange.contentStart, blockRange.contentEnd);
      const subsections = getSubsectionRanges(blockLines, spec.subsectionDefs);

      // Need at least 2 subsections to reorder
      if (subsections.length < 2) return;

      // Check canonical positions
      const canonicalPositions = subsections.map((s) => {
        const idx = spec.canonicalOrder.indexOf(s.tag);
        return idx >= 0 ? idx : 999; // Unknown tags sort to end
      });

      // Already in order?
      let inOrder = true;
      for (let i = 1; i < canonicalPositions.length; i++) {
        if (canonicalPositions[i]! < canonicalPositions[i - 1]!) {
          inOrder = false;
          break;
        }
      }
      if (inOrder) return;

      // Build before/after order for message
      const beforeTags = subsections.map((s) => s.tag);
      const afterTags = [...subsections]
        .sort((a, b) => {
          const ai = spec.canonicalOrder.indexOf(a.tag);
          const bi = spec.canonicalOrder.indexOf(b.tag);
          return (ai >= 0 ? ai : 999) - (bi >= 0 ? bi : 999);
        })
        .map((s) => s.tag);

      const orderMsg = `[${beforeTags.join(", ")}] → [${afterTags.join(", ")}] (per section registry)`;

      if (opts.dryRun) {
        ctx.wouldModify = true;
        results.push(info(filePath, `transform/reorder-${spec.block.toLowerCase()}`,
          `${spec.block} subsections would be reordered: ${orderMsg}`));
        return;
      }

      // Extract each subsection's lines (from header to next header)
      interface SubChunk {
        tag: string;
        canonIdx: number;
        lines: string[];
      }

      const chunks: SubChunk[] = [];
      let preambleLines: string[] = [];

      for (let i = 0; i < subsections.length; i++) {
        const sub = subsections[i]!;
        const endIdx = i + 1 < subsections.length
          ? subsections[i + 1]!.startIdx
          : blockLines.length;

        // Lines before the first subsection are preamble
        if (i === 0 && sub.startIdx > 0) {
          preambleLines = blockLines.slice(0, sub.startIdx);
        }

        // Include separator line(s) that precede this subsection header
        let headerStart = sub.startIdx;
        while (headerStart > 0 && /^\/\/\s*[─=\-]{10,}\s*$/.test(blockLines[headerStart - 1]!.trim())) {
          headerStart--;
        }
        // Include blank line before separator
        if (headerStart > 0 && blockLines[headerStart - 1]!.trim() === "") {
          headerStart--;
        }

        // Adjust preamble to not include this subsection's leading separator
        if (i === 0) {
          preambleLines = blockLines.slice(0, headerStart);
        }

        const chunkLines = i === 0
          ? blockLines.slice(headerStart, endIdx)
          : blockLines.slice(headerStart, endIdx);

        // For non-first chunks, steal leading blank/separator from previous chunk
        if (i > 0 && chunks.length > 0) {
          const prevChunk = chunks[chunks.length - 1]!;
          // Trim trailing blank lines from previous chunk (they belong to this chunk's separator)
          while (prevChunk.lines.length > 0 && prevChunk.lines[prevChunk.lines.length - 1]!.trim() === "") {
            prevChunk.lines.pop();
          }
        }

        chunks.push({
          tag: sub.tag,
          canonIdx: spec.canonicalOrder.indexOf(sub.tag),
          lines: chunkLines,
        });
      }

      // Sort by canonical order
      chunks.sort((a, b) => (a.canonIdx >= 0 ? a.canonIdx : 999) - (b.canonIdx >= 0 ? b.canonIdx : 999));

      // Reassemble
      const reordered: string[] = [...preambleLines];
      for (const chunk of chunks) {
        reordered.push(...chunk.lines);
      }

      // Replace block content
      lines.splice(
        blockRange.contentStart,
        blockRange.contentEnd - blockRange.contentStart,
        ...reordered,
      );
      ctx.modified = true;
      results.push(info(filePath, `transform/reorder-${spec.block.toLowerCase()}`,
        `Reordered ${spec.block} subsections: ${orderMsg}`));
    },
  };
}

// ---------------------------------------------------------------------------
// 4. Pipeline runner — orchestrates passes in order
// ---------------------------------------------------------------------------

/**
 * The universal transform passes shared across all 4-block code languages.
 *
 * Order matters — earlier passes affect later ones:
 * 1. Separator widths fixed before prefix normalization
 * 2. ASCII→Unicode conversion before width checks
 * 3. Code moves before zone reordering
 *
 * Language-specific passes are appended by the handler.
 */
export const UNIVERSAL_PASSES: readonly TransformPass[] = [
  fixBlockSeparatorWidths,
  fixSubsectionSeparatorWidths,
  convertAsciiToUnicode,
  normalizeSeparatorPrefix,
];

/**
 * Run the transform pipeline on a file.
 *
 * Creates a TransformContext, runs all passes in order, and writes
 * the result if modified (unless dryRun).
 *
 * @param filePath - Absolute path to the file
 * @param rules - Schema-derived rules for this format
 * @param opts - CLI transform options
 * @param passes - All transform passes to run (universal + language-specific)
 * @returns LintResult diagnostics from the transform run
 */
/**
 * Optional verification callback — re-lints the file after transform.
 * Provided by the handler so the pipeline can verify its own work.
 */
export type TransformVerifier = (filePath: string) => Promise<LintResult[]>;

export async function runTransformPipeline(
  filePath: string,
  rules: Code4BlockRules,
  opts: TransformOptions,
  passes: readonly TransformPass[],
  verify?: TransformVerifier,
): Promise<LintResult[]> {
  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  // Count errors BEFORE transform (for delta reporting)
  const beforeErrors = verify
    ? (await verify(filePath)).filter((r) => r.severity === "error").length
    : undefined;

  const ctx: TransformContext = {
    filePath,
    lines: text.split("\n"),
    rules,
    opts,
    results: [],
    modified: false,
    wouldModify: false,
  };

  // Run each pass in order
  for (const pass of passes) {
    if (pass.requiresExtensions && !opts.extensions) continue;

    const result = pass.apply(ctx);
    if (result instanceof Promise) {
      await result;
    }
  }

  // Write if modified
  if (ctx.modified && !opts.dryRun) {
    await Deno.writeTextFile(filePath, ctx.lines.join("\n"));
    ctx.results.push(info(filePath, "transform/written", "File updated"));

    // ── Verification loop: re-lint after write ──────────────────
    if (verify) {
      const afterResults = await verify(filePath);
      const afterErrors = afterResults.filter((r) => r.severity === "error").length;
      const delta = (beforeErrors ?? 0) - afterErrors;

      if (afterErrors === 0) {
        ctx.results.push(info(filePath, "transform/verify",
          `Re-lint: ${afterErrors} errors (all clear)`));
      } else if (delta > 0) {
        ctx.results.push(info(filePath, "transform/verify",
          `Re-lint: ${beforeErrors} → ${afterErrors} errors (${delta} fixed)`));
      } else if (delta === 0) {
        ctx.results.push(info(filePath, "transform/verify",
          `Re-lint: ${afterErrors} errors (unchanged — may need manual fixes)`));
      } else {
        // Regression — transform introduced new issues
        ctx.results.push(error(filePath, "transform/verify",
          `Re-lint: ${beforeErrors} → ${afterErrors} errors (${-delta} NEW issues introduced)`));
      }
    }
  } else if (!ctx.modified && !ctx.wouldModify) {
    ctx.results.push(info(filePath, "transform/clean", "No changes needed"));
  }

  return ctx.results;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Shared transform pipeline — the generic engine for schema-driven transforms.
// Universal passes handle formatting. Handler-provided passes handle language
// specifics. The schema drives what the target state should look like.
//
// "Every purpose is established by counsel." — Proverbs 20:18
// ============================================================================
