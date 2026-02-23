// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/inspect.ts
// key:     B-tov-cws-struct-lib-engine-inspect
// title:   CWS Struct — Inspect Output Formatter
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Format InspectResult into human-readable or JSON output.
//          --inspect shows what the parser sees BEFORE any checks run.
//          "Here's what I see" — pure observation, no judgment.
//
// biblical_foundation: "The hearing ear, and the seeing eye, the LORD hath
//   made even both of them." — Proverbs 20:12
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { InspectResult } from "../foundation/mod.ts";
import { COLORS } from "./output.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Text output — human-readable inspect format
// ---------------------------------------------------------------------------

/**
 * Format an InspectResult as human-readable text.
 *
 * Output looks like:
 * ```
 * File: path/to/file.go
 * Format: go | Subtype: library | Template: false | Lines: 780
 *
 * Blocks detected (4/4):
 *   METADATA  lines 1-42    (separator at line 1)
 *   SETUP     lines 43-180  (separator at line 43)
 *   BODY      lines 181-650 (separator at line 181)
 *   CLOSING   lines 651-780 (separator at line 651)
 *
 * Sections detected:
 *   SETUP.Imports     line 44   (position 1, expected 1) ✓
 *   SETUP.Constants   line 78   (position 3, expected 3) ✓
 *   ...
 *
 * Content classification:
 *   Function declarations: 12 (in BODY, CLOSING)
 *   Import statements: 5 (in SETUP)
 *   ...
 *
 * Directives:
 *   key: B-example-file
 *   version: a-01.00
 *   pragma: //omni:code --go -library
 * ```
 */
export function formatInspectText(result: InspectResult): string {
  const lines: string[] = [];

  // Header
  lines.push(`${COLORS.bold}File:${COLORS.reset} ${result.filePath}`);
  const subtypeStr = result.subtype ? ` | Subtype: ${result.subtype}` : "";
  lines.push(
    `Format: ${COLORS.blue}${result.format}${COLORS.reset}${subtypeStr}` +
    ` | Template: ${result.isTemplate}` +
    ` | Lines: ${result.lineCount}`,
  );
  if (result.pragma) {
    lines.push(`Pragma: ${COLORS.blue}${result.pragma}${COLORS.reset}`);
  }
  lines.push("");

  // Blocks
  const expectedBlocks = 4; // 4-block is the standard for code
  lines.push(
    `${COLORS.bold}Blocks detected (${result.blocks.length}/${expectedBlocks}):${COLORS.reset}`,
  );
  if (result.blocks.length === 0) {
    lines.push(`  ${COLORS.red}(none)${COLORS.reset}`);
  } else {
    for (const b of result.blocks) {
      const sepStr = b.separatorLine
        ? ` (separator at line ${b.separatorLine})`
        : "";
      const name = b.name.padEnd(10);
      lines.push(
        `  ${COLORS.blue}${name}${COLORS.reset} lines ${b.startLine}-${b.endLine}${sepStr}`,
      );
    }
  }
  lines.push("");

  // Sections
  if (result.sections.length > 0) {
    lines.push(
      `${COLORS.bold}Sections detected (${result.sections.length}):${COLORS.reset}`,
    );
    for (const s of result.sections) {
      const label = `${s.block}.${s.name}`.padEnd(28);
      const pos = `position ${s.position}`;
      let expected = "";
      let icon = "";
      if (s.expectedPosition !== undefined) {
        expected = `, expected ${s.expectedPosition}`;
        icon = s.aligned ? ` ${COLORS.green}✓${COLORS.reset}` : ` ${COLORS.red}✗${COLORS.reset}`;
      }
      lines.push(
        `  ${COLORS.blue}${label}${COLORS.reset} line ${s.line}  (${pos}${expected})${icon}`,
      );
    }
    lines.push("");
  }

  // Content classification
  if (result.content.length > 0) {
    lines.push(
      `${COLORS.bold}Content classification:${COLORS.reset}`,
    );
    for (const c of result.content) {
      const blocksStr = c.blocks.length > 0
        ? ` (in ${c.blocks.join(", ")})`
        : "";
      lines.push(`  ${c.category}: ${c.count}${blocksStr}`);
    }
    lines.push("");
  }

  // Directives
  const dirKeys = Object.keys(result.directives);
  if (dirKeys.length > 0) {
    lines.push(`${COLORS.bold}Directives:${COLORS.reset}`);
    for (const k of dirKeys) {
      lines.push(`  ${k}: ${result.directives[k]}`);
    }
    lines.push("");
  }

  return lines.join("\n");
}

// ---------------------------------------------------------------------------
// JSON output — machine-readable inspect format
// ---------------------------------------------------------------------------

/**
 * Format an InspectResult as JSON string (for --json --inspect).
 */
export function formatInspectJson(result: InspectResult): string {
  return JSON.stringify(result, null, 2);
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Inspect formatter — presents parsed structure without judgment.
// The eyes that see before the mind that judges.
//
// "The hearing ear, and the seeing eye, the LORD hath made even both of them."
// — Proverbs 20:12
// ============================================================================
