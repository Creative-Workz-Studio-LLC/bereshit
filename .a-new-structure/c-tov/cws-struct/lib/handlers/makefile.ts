// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/handlers/makefile.ts
// key:     B-tov-cws-struct-lib-handlers-makefile
// title:   CWS Struct — Makefile Format Handler
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-13
// authors: Nova Dawn (CPI-SI)
// derives: lib/handlers/toml.ts (adapted for comment-based 4-block structure)
// purpose: Makefile 4-block alignment linter as a format handler.
//          Validates pragma (P1-P5), metadata (M1-M10), block boundaries
//          (METADATA → SETUP → BODY → CLOSING), and closing (X1-X10).
//          All structure lives in # comments — parsed line by line.
//
// biblical_foundation: "So built we the wall... for the people had a mind
//   to work." — Nehemiah 4:6
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { FormatHandler, LintResult } from "../foundation/mod.ts";
import { error, warn, info } from "../foundation/mod.ts";
import { registerFormat } from "../engine/mod.ts";

// ---------------------------------------------------------------------------
// Constants — block markers and section patterns
// ---------------------------------------------------------------------------

/** Block boundary tag strings (appear on comment lines between ═══ separators). */
const BLOCK_TAGS = {
  pragmaStart: "PRAGMA [PRAGMA]",
  pragmaEnd: "END PRAGMA [END]",
  metadataStart: "METADATA BLOCK [METADATA]",
  metadataEnd: "END METADATA [END]",
  setupStart: "SETUP BLOCK [SETUP]",
  closingStart: "CLOSING BLOCK [CLOSING]",
  closingEnd: "END CLOSING [END]",
} as const;

/** Required pragma fields (within # --- PN: sections). */
const REQUIRED_PRAGMA_FIELDS = ["key", "type", "structure", "title"] as const;

/** Recommended pragma fields (warn if missing). */
const RECOMMENDED_PRAGMA_FIELDS = [
  "from",
  "at",
  "subtype",
  "role",
  "brief",
] as const;

/**
 * Required metadata sections — M1-M5 in either format:
 *   Old: "# # M.N Title [TAG]"
 *   New: "# --- MN: Title ---"
 */
const REQUIRED_METADATA = [1, 2, 3] as const; // M1 Identity, M2 State, M3 Attribution
const RECOMMENDED_METADATA = [4, 5] as const; // M4 Grounding/Location, M5 Dependencies/Derivation
const EXTENSION_METADATA = [6, 7, 8, 9, 10] as const; // M6-M10 (optional)

/** Required closing sections. */
const REQUIRED_CLOSING_TAGS = ["POLICY", "REFERENCE", "NOTE"] as const;

/** OmniCode pragma line pattern. */
const PRAGMA_LINE = /^#\s*#!omni\s+/;

/** Pragma field: "# field: value" (within pragma block). */
const PRAGMA_FIELD = /^#\s{1,6}(\w[\w_]*):\s+(.+)/;

/**
 * Metadata section header — two formats:
 *   Old: "# # M.N Title [TAG]" (Makefile templates)
 *   New: "# --- MN: Title ---" (compact format)
 */
const META_SECTION_OLD = /^#\s*#\s*M\.(\d+)\b/;
const META_SECTION_NEW = /^#\s*---\s*M(\d+):/;

/**
 * Closing section header — two formats:
 *   Old: "# # X.N Title [TAG]" (Makefile templates)
 *   New: "# --- XN: Title ---" (compact format)
 */
const CLOSE_SECTION_OLD = /^#\s*#\s*X\.(\d+)\b/;
const CLOSE_SECTION_NEW = /^#\s*---\s*X(\d+):/;

/** Closing section tag extraction (from old format): "# # X.N Title [TAG]" */
const CLOSE_TAG = /\[(\w+)\]/;

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers — line scanning
// ---------------------------------------------------------------------------

/** Check if any line contains a marker string. */
function hasMarker(lines: string[], marker: string): boolean {
  return lines.some((line) => line.includes(marker));
}

/** Find the first line index containing a marker string. Returns -1 if not found. */
function findMarker(lines: string[], marker: string): number {
  return lines.findIndex((line) => line.includes(marker));
}

/** Extract a comment field value: "# field: value" → value (trimmed). */
function extractField(line: string): { field: string; value: string } | null {
  const match = line.match(PRAGMA_FIELD);
  if (!match) return null;
  return { field: match[1]!.toLowerCase(), value: match[2]!.trim() };
}

/** Collect all metadata section numbers found in lines (both old and new format). */
function collectMetaSections(
  lines: string[],
  startIdx: number,
  endIdx: number,
): Set<number> {
  const sections = new Set<number>();
  for (let i = startIdx; i < endIdx && i < lines.length; i++) {
    let match = lines[i]!.match(META_SECTION_OLD);
    if (match) {
      sections.add(parseInt(match[1]!));
      continue;
    }
    match = lines[i]!.match(META_SECTION_NEW);
    if (match) {
      sections.add(parseInt(match[1]!));
    }
  }
  return sections;
}

/** Collect all closing section tags found in lines (both old and new format). */
function collectClosingTags(
  lines: string[],
  startIdx: number,
  endIdx: number,
): Set<string> {
  const tags = new Set<string>();
  for (let i = startIdx; i < endIdx && i < lines.length; i++) {
    const line = lines[i]!;

    // Old format: "# # X.N Title [TAG]"
    if (CLOSE_SECTION_OLD.test(line)) {
      const tagMatch = line.match(CLOSE_TAG);
      if (tagMatch) tags.add(tagMatch[1]!.toUpperCase());
    }

    // New format: "# --- XN: Title ---"
    const newMatch = line.match(CLOSE_SECTION_NEW);
    if (newMatch) {
      // Extract tag from context — check if the title includes [TAG]
      const tagMatch = line.match(CLOSE_TAG);
      if (tagMatch) {
        tags.add(tagMatch[1]!.toUpperCase());
      } else {
        // Infer from section name (e.g., "# --- X1: Policy ---" → POLICY)
        const titleMatch = line.match(/---\s*X\d+:\s*(\w+)/);
        if (titleMatch) tags.add(titleMatch[1]!.toUpperCase());
      }
    }
  }
  return tags;
}

/** Extract pragma fields between pragma start and end markers. */
function extractPragmaFields(
  lines: string[],
  startIdx: number,
  endIdx: number,
): Map<string, string> {
  const fields = new Map<string, string>();
  for (let i = startIdx; i < endIdx && i < lines.length; i++) {
    const extracted = extractField(lines[i]!);
    if (extracted) {
      fields.set(extracted.field, extracted.value);
    }
  }
  return fields;
}

/** Extract a specific "# field: value" from metadata section lines. */
function extractMetaField(
  lines: string[],
  startIdx: number,
  endIdx: number,
  fieldName: string,
): string | undefined {
  for (let i = startIdx; i < endIdx && i < lines.length; i++) {
    const match = lines[i]!.match(
      new RegExp(`^#\\s+${fieldName}:\\s+(.+)`, "i"),
    );
    if (match) return match[1]!.trim();
  }
  return undefined;
}

// ---------------------------------------------------------------------------
// Core — Block structure checks
// ---------------------------------------------------------------------------

function checkPragma(file: string, lines: string[]): LintResult[] {
  const results: LintResult[] = [];

  // Check #!omni pragma line
  const hasPragmaLine = lines.some((line) => PRAGMA_LINE.test(line));
  if (!hasPragmaLine) {
    results.push(
      warn(file, "pragma/omni_line", 'Missing #!omni pragma line (e.g., "# #!omni code --makefile")'),
    );
  }

  // Check pragma block boundary
  if (!hasMarker(lines, BLOCK_TAGS.pragmaStart)) {
    results.push(
      error(file, "pragma/boundary", "Missing PRAGMA [PRAGMA] block boundary"),
    );
    return results;
  }

  if (!hasMarker(lines, BLOCK_TAGS.pragmaEnd)) {
    results.push(
      warn(file, "pragma/end_boundary", "Missing END PRAGMA [END] boundary"),
    );
  }

  // Extract pragma fields
  const pragmaStart = findMarker(lines, BLOCK_TAGS.pragmaStart);
  const pragmaEnd = findMarker(lines, BLOCK_TAGS.pragmaEnd);
  const end = pragmaEnd >= 0 ? pragmaEnd : lines.length;

  const fields = extractPragmaFields(lines, pragmaStart, end);

  // Check required fields
  for (const field of REQUIRED_PRAGMA_FIELDS) {
    if (!fields.has(field)) {
      results.push(
        error(file, `pragma/${field}`, `Missing required pragma field: ${field}`),
      );
    }
  }

  // Check recommended fields
  for (const field of RECOMMENDED_PRAGMA_FIELDS) {
    if (!fields.has(field)) {
      results.push(
        info(file, `pragma/${field}`, `Missing recommended pragma field: ${field}`),
      );
    }
  }

  // Verify structure is 4-block (accept "4-block" or "METADATA → SETUP → BODY → CLOSING")
  const structure = fields.get("structure");
  if (
    structure &&
    !structure.includes("4-block") &&
    !structure.includes("SETUP") &&
    !structure.includes("BODY")
  ) {
    results.push(
      warn(
        file,
        "pragma/structure",
        `Expected 4-block structure, got: ${structure}`,
      ),
    );
  }

  return results;
}

function checkMetadata(file: string, lines: string[]): LintResult[] {
  const results: LintResult[] = [];

  const metaStart = findMarker(lines, BLOCK_TAGS.metadataStart);
  if (metaStart < 0) {
    results.push(
      error(file, "metadata/boundary", "Missing METADATA BLOCK [METADATA] boundary"),
    );
    return results;
  }

  const metaEnd = findMarker(lines, BLOCK_TAGS.metadataEnd);
  if (metaEnd < 0) {
    results.push(
      warn(file, "metadata/end_boundary", "Missing END METADATA [END] boundary"),
    );
  }

  const end = metaEnd >= 0 ? metaEnd : lines.length;
  const sections = collectMetaSections(lines, metaStart, end);

  // Check required metadata sections
  for (const n of REQUIRED_METADATA) {
    if (!sections.has(n)) {
      results.push(
        error(file, `metadata/M${n}`, `Missing required metadata section M${n}`),
      );
    }
  }

  // Check recommended metadata sections
  for (const n of RECOMMENDED_METADATA) {
    if (!sections.has(n)) {
      results.push(
        warn(file, `metadata/M${n}`, `Missing recommended metadata section M${n}`),
      );
    }
  }

  // Check extension metadata (info only)
  for (const n of EXTENSION_METADATA) {
    if (!sections.has(n)) {
      results.push(
        info(file, `metadata/M${n}`, `Missing extension metadata section M${n}`),
      );
    }
  }

  return results;
}

function checkBlockOrder(file: string, lines: string[]): LintResult[] {
  const results: LintResult[] = [];

  const metaStart = findMarker(lines, BLOCK_TAGS.metadataStart);
  const setupStart = findMarker(lines, BLOCK_TAGS.setupStart);
  const closingStart = findMarker(lines, BLOCK_TAGS.closingStart);

  // Check setup block exists
  if (setupStart < 0) {
    results.push(
      error(file, "blocks/setup", "Missing SETUP BLOCK [SETUP] boundary"),
    );
  }

  // Check closing block exists
  if (closingStart < 0) {
    results.push(
      error(file, "blocks/closing", "Missing CLOSING BLOCK [CLOSING] boundary"),
    );
  }

  if (!hasMarker(lines, BLOCK_TAGS.closingEnd)) {
    results.push(
      warn(file, "blocks/closing_end", "Missing END CLOSING [END] boundary"),
    );
  }

  // Verify order: METADATA < SETUP < CLOSING
  if (metaStart >= 0 && setupStart >= 0 && setupStart <= metaStart) {
    results.push(
      error(file, "blocks/order", "SETUP block appears before METADATA block"),
    );
  }

  if (setupStart >= 0 && closingStart >= 0 && closingStart <= setupStart) {
    results.push(
      error(file, "blocks/order", "CLOSING block appears before SETUP block"),
    );
  }

  return results;
}

function checkClosing(file: string, lines: string[]): LintResult[] {
  const results: LintResult[] = [];

  const closingStart = findMarker(lines, BLOCK_TAGS.closingStart);
  if (closingStart < 0) return results; // Already reported in checkBlockOrder

  const closingEnd = findMarker(lines, BLOCK_TAGS.closingEnd);
  const end = closingEnd >= 0 ? closingEnd : lines.length;

  const tags = collectClosingTags(lines, closingStart, end);

  // Check required closing sections by tag
  for (const tag of REQUIRED_CLOSING_TAGS) {
    if (!tags.has(tag)) {
      results.push(
        error(file, `closing/${tag}`, `Missing required closing section: ${tag}`),
      );
    }
  }

  return results;
}

function checkCrossRefs(file: string, lines: string[]): LintResult[] {
  const results: LintResult[] = [];

  // Extract key and title from pragma
  const pragmaStart = findMarker(lines, BLOCK_TAGS.pragmaStart);
  const pragmaEnd = findMarker(lines, BLOCK_TAGS.pragmaEnd);
  if (pragmaStart < 0) return results;
  const pEnd = pragmaEnd >= 0 ? pragmaEnd : lines.length;

  const pragmaFields = extractPragmaFields(lines, pragmaStart, pEnd);
  const pragmaKey = pragmaFields.get("key");
  const pragmaTitle = pragmaFields.get("title");

  // Extract key and title from metadata
  const metaStart = findMarker(lines, BLOCK_TAGS.metadataStart);
  const metaEnd = findMarker(lines, BLOCK_TAGS.metadataEnd);
  if (metaStart < 0) return results;
  const mEnd = metaEnd >= 0 ? metaEnd : lines.length;

  const metaKey = extractMetaField(lines, metaStart, mEnd, "Key");
  const metaTitle = extractMetaField(lines, metaStart, mEnd, "Title");

  // Compare P1 key with M1 key (skip if either looks like a template placeholder)
  if (
    pragmaKey &&
    metaKey &&
    pragmaKey !== metaKey &&
    !metaKey.startsWith("[") &&
    !pragmaKey.startsWith("[")
  ) {
    results.push(
      error(
        file,
        "xref/P1-M1-key",
        `Key mismatch: pragma key="${pragmaKey}" vs metadata Key="${metaKey}"`,
      ),
    );
  }

  // Compare P5 title with M1 title (skip if either looks like a template placeholder)
  if (
    pragmaTitle &&
    metaTitle &&
    pragmaTitle !== metaTitle &&
    !metaTitle.startsWith("[") &&
    !pragmaTitle.startsWith("[")
  ) {
    results.push(
      warn(
        file,
        "xref/P5-M1-title",
        `Title mismatch: pragma title="${pragmaTitle}" vs metadata Title="${metaTitle}"`,
      ),
    );
  }

  return results;
}

// ---------------------------------------------------------------------------
// Lint orchestrator
// ---------------------------------------------------------------------------

async function lintMakefileFile(filePath: string): Promise<LintResult[]> {
  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  const lines = text.split("\n");

  // Quick check: does this look like an OmniCode-structured Makefile?
  if (!hasMarker(lines, "PRAGMA") && !lines.some((l) => PRAGMA_LINE.test(l))) {
    return [
      info(
        filePath,
        "structure/not_omni",
        "Not an OmniCode-structured Makefile — no PRAGMA marker or #!omni line found",
      ),
    ];
  }

  return [
    ...checkPragma(filePath, lines),
    ...checkMetadata(filePath, lines),
    ...checkBlockOrder(filePath, lines),
    ...checkClosing(filePath, lines),
    ...checkCrossRefs(filePath, lines),
  ];
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// Registration — plug into the registry
// ---------------------------------------------------------------------------

const makefileHandler: FormatHandler = {
  name: "makefile",
  description:
    "Makefile 4-block alignment (pragma P1-P5, metadata M1-M10, SETUP/BODY/CLOSING)",
  extensions: [".mk"],
  basenames: ["Makefile", "GNUmakefile"],
  maxDepth: 10,
  lint: lintMakefileFile,
};

registerFormat(makefileHandler);

export default makefileHandler;
