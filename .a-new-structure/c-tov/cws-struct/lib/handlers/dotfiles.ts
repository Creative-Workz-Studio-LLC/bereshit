// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/handlers/dotfiles.ts
// key:     B-tov-cws-struct-lib-handlers-dotfiles
// title:   CWS Struct — Dotfiles Format Handler
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-13
// authors: Nova Dawn (CPI-SI)
// derives: lib/handlers/toml.ts (adapted for comment-based 3-block structure)
// purpose: Dotfile 3-block alignment linter as a format handler.
//          Validates pragma (P1-P5), metadata (M1-M5), content structure
//          (Cc/Co/Cv), and closing (X1-X5) for .editorconfig, .gitignore,
//          .gitmessage, and .env files. All structure lives in # comments.
//
// biblical_foundation: "Let all things be done decently and in order."
//   — 1 Corinthians 14:40
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

/** Block boundary tag strings. */
const BLOCK_TAGS = {
  pragmaStart: "PRAGMA [PRAGMA]",
  pragmaEnd: "END PRAGMA [END]",
  metadataStart: "METADATA BLOCK [METADATA]",
  metadataEnd: "END METADATA [END]",
  contentStart: "CONTENT BLOCK [CONTENT]",
  contentEnd: "END CONTENT [END]",
  closingStart: "CLOSING BLOCK [CLOSING]",
  closingEnd: "END CLOSING [END]",
} as const;

/** Required pragma fields. */
const REQUIRED_PRAGMA_FIELDS = ["key", "type", "structure", "title"] as const;

/** Recommended pragma fields (info if missing). */
const RECOMMENDED_PRAGMA_FIELDS = [
  "from",
  "at",
  "subtype",
  "role",
  "brief",
] as const;

/** Required metadata sections (M1-M5). */
const REQUIRED_METADATA = [1, 2, 3, 4, 5] as const;

/** Required closing tags. */
const REQUIRED_CLOSING_TAGS = ["POLICY", "REFERENCE", "NOTE"] as const;

/** Content sub-block markers. */
const CONTENT_MARKERS = ["Cc", "Co", "Cv"] as const;

/** OmniCode pragma line pattern. */
const PRAGMA_LINE = /^#\s*#!omni\s+/;

/** Pragma field: "# field: value". */
const PRAGMA_FIELD = /^#\s{1,6}(\w[\w_]*):\s+(.+)/;

/**
 * Metadata section header — two formats:
 *   Compact: "# --- MN: Title ---" (dotfile convention)
 *   Verbose: "# # M.N Title [TAG]" (older convention)
 */
const META_SECTION_COMPACT = /^#\s*---\s*M(\d+):/;
const META_SECTION_VERBOSE = /^#\s*#\s*M\.(\d+)\b/;

/**
 * Closing section header — two formats:
 *   Compact: "# --- XN: Title ---"
 *   Verbose: "# # X.N Title [TAG]"
 */
const CLOSE_SECTION_COMPACT = /^#\s*---\s*X(\d+):\s*(\w+)/;
const CLOSE_SECTION_VERBOSE = /^#\s*#\s*X\.(\d+)\b/;
const CLOSE_TAG = /\[(\w+)\]/;

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers — line scanning
// ---------------------------------------------------------------------------

function hasMarker(lines: string[], marker: string): boolean {
  return lines.some((line) => line.includes(marker));
}

function findMarker(lines: string[], marker: string): number {
  return lines.findIndex((line) => line.includes(marker));
}

function extractPragmaFields(
  lines: string[],
  startIdx: number,
  endIdx: number,
): Map<string, string> {
  const fields = new Map<string, string>();
  for (let i = startIdx; i < endIdx && i < lines.length; i++) {
    const match = lines[i]!.match(PRAGMA_FIELD);
    if (match) {
      fields.set(match[1]!.toLowerCase(), match[2]!.trim());
    }
  }
  return fields;
}

function collectMetaSections(
  lines: string[],
  startIdx: number,
  endIdx: number,
): Set<number> {
  const sections = new Set<number>();
  for (let i = startIdx; i < endIdx && i < lines.length; i++) {
    const line = lines[i]!;
    let match = line.match(META_SECTION_COMPACT);
    if (match) {
      sections.add(parseInt(match[1]!));
      continue;
    }
    match = line.match(META_SECTION_VERBOSE);
    if (match) {
      sections.add(parseInt(match[1]!));
    }
  }
  return sections;
}

function collectClosingTags(
  lines: string[],
  startIdx: number,
  endIdx: number,
): Set<string> {
  const tags = new Set<string>();
  for (let i = startIdx; i < endIdx && i < lines.length; i++) {
    const line = lines[i]!;

    // Compact format: "# --- XN: Title ---"
    const compactMatch = line.match(CLOSE_SECTION_COMPACT);
    if (compactMatch) {
      tags.add(compactMatch[2]!.toUpperCase());
      continue;
    }

    // Verbose format: "# # X.N Title [TAG]"
    if (CLOSE_SECTION_VERBOSE.test(line)) {
      const tagMatch = line.match(CLOSE_TAG);
      if (tagMatch) tags.add(tagMatch[1]!.toUpperCase());
    }
  }
  return tags;
}

/** Extract a "# field: value" from metadata lines. */
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
  if (!lines.some((line) => PRAGMA_LINE.test(line))) {
    results.push(
      warn(
        file,
        "pragma/omni_line",
        'Missing #!omni pragma line (e.g., "# #!omni data --editorconfig")',
      ),
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

  // Required fields
  for (const field of REQUIRED_PRAGMA_FIELDS) {
    if (!fields.has(field)) {
      results.push(
        error(file, `pragma/${field}/required`, `Missing required pragma field: ${field}`),
      );
    }
  }

  // Recommended fields
  for (const field of RECOMMENDED_PRAGMA_FIELDS) {
    if (!fields.has(field)) {
      results.push(
        info(file, `pragma/${field}/recommended`, `Missing recommended pragma field: ${field}`),
      );
    }
  }

  // Verify structure is 3-block
  const structure = fields.get("structure");
  if (structure && !structure.includes("3-block")) {
    results.push(
      warn(
        file,
        "pragma/structure",
        `Expected structure: 3-block, got: ${structure}`,
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

  if (!hasMarker(lines, BLOCK_TAGS.metadataEnd)) {
    results.push(
      warn(file, "metadata/end_boundary", "Missing END METADATA [END] boundary"),
    );
  }

  const metaEnd = findMarker(lines, BLOCK_TAGS.metadataEnd);
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

  return results;
}

function checkContent(file: string, lines: string[]): LintResult[] {
  const results: LintResult[] = [];

  const contentStart = findMarker(lines, BLOCK_TAGS.contentStart);
  if (contentStart < 0) {
    results.push(
      error(file, "content/boundary", "Missing CONTENT BLOCK [CONTENT] boundary"),
    );
    return results;
  }

  // Check for Cc/Co/Cv sub-block markers in content area
  const closingStart = findMarker(lines, BLOCK_TAGS.closingStart);
  const end = closingStart >= 0 ? closingStart : lines.length;

  for (const marker of CONTENT_MARKERS) {
    // Look for "# Cc" or "# Co" or "# Cv" (as sub-block separator labels)
    const pattern = new RegExp(`^#\\s+(${marker}\\b|---\\s*${marker}\\b)`);
    const found = lines.slice(contentStart, end).some((line) => pattern.test(line));
    if (!found) {
      results.push(
        warn(
          file,
          `content/${marker}`,
          `Missing ${marker} sub-block marker in content`,
        ),
      );
    }
  }

  return results;
}

function checkBlockOrder(file: string, lines: string[]): LintResult[] {
  const results: LintResult[] = [];

  const metaStart = findMarker(lines, BLOCK_TAGS.metadataStart);
  const contentStart = findMarker(lines, BLOCK_TAGS.contentStart);
  const closingStart = findMarker(lines, BLOCK_TAGS.closingStart);

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

  // Verify order: METADATA < CONTENT < CLOSING
  if (metaStart >= 0 && contentStart >= 0 && contentStart <= metaStart) {
    results.push(
      error(file, "blocks/order", "CONTENT block appears before METADATA block"),
    );
  }

  if (contentStart >= 0 && closingStart >= 0 && closingStart <= contentStart) {
    results.push(
      error(file, "blocks/order", "CLOSING block appears before CONTENT block"),
    );
  }

  return results;
}

function checkClosing(file: string, lines: string[]): LintResult[] {
  const results: LintResult[] = [];

  const closingStart = findMarker(lines, BLOCK_TAGS.closingStart);
  if (closingStart < 0) return results;

  const closingEnd = findMarker(lines, BLOCK_TAGS.closingEnd);
  const end = closingEnd >= 0 ? closingEnd : lines.length;

  const tags = collectClosingTags(lines, closingStart, end);

  for (const tag of REQUIRED_CLOSING_TAGS) {
    if (!tags.has(tag)) {
      results.push(
        error(file, `closing/section/${tag}/required`, `Missing required closing section: ${tag}`),
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

  const metaKey = extractMetaField(lines, metaStart, mEnd, "key");
  const metaTitle = extractMetaField(lines, metaStart, mEnd, "title");

  // P1 key vs M1 key
  if (pragmaKey && metaKey && pragmaKey !== metaKey) {
    results.push(
      error(
        file,
        "xref/P1-M1-key",
        `Key mismatch: pragma key="${pragmaKey}" vs metadata key="${metaKey}"`,
      ),
    );
  }

  // P5 title vs M1 title
  if (pragmaTitle && metaTitle && pragmaTitle !== metaTitle) {
    results.push(
      warn(
        file,
        "xref/P5-M1-title",
        `Title mismatch: pragma title="${pragmaTitle}" vs metadata title="${metaTitle}"`,
      ),
    );
  }

  return results;
}

// ---------------------------------------------------------------------------
// Lint orchestrator
// ---------------------------------------------------------------------------

async function lintDotfile(filePath: string): Promise<LintResult[]> {
  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  const lines = text.split("\n");

  // Quick check: does this look like an OmniCode-structured dotfile?
  if (!hasMarker(lines, "PRAGMA") && !lines.some((l) => PRAGMA_LINE.test(l))) {
    return [
      info(
        filePath,
        "structure/not_omni",
        "Not an OmniCode-structured dotfile — no PRAGMA marker or #!omni line found",
      ),
    ];
  }

  return [
    ...checkPragma(filePath, lines),
    ...checkMetadata(filePath, lines),
    ...checkContent(filePath, lines),
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

const dotfilesHandler: FormatHandler = {
  name: "dotfiles",
  description:
    "Dotfile 3-block alignment (.editorconfig, .gitignore, .gitmessage, .env)",
  extensions: [".env", ".editorconfig", ".gitignore", ".gitmessage"],
  basenames: [".env", ".editorconfig", ".gitignore", ".gitmessage"],
  maxDepth: 10,
  lint: lintDotfile,
};

registerFormat(dotfilesHandler);

export default dotfilesHandler;
