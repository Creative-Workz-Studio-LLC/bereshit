#!/usr/bin/env -S deno run --allow-read
// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// file:    lint-makefile.ts
// key:     B-tools-lint-makefile
// title:   Makefile Template Alignment Linter
// type:    Tool (Linter)
// version: a-01.00
// created: 2026-02-10
// updated: 2026-02-10
// authors: Nova Dawn (CPI-SI)
// purpose: Validate Makefile files against the OmniCode 4-block template.
//          Checks comment-based pragma (P1-P5), metadata (M1-M5 or M.1-M.10),
//          4-block structure (METADATA → SETUP → BODY → CLOSING), closing
//          policy (X1/X4/X5 or X.1-X.10), and cross-format references.
//          Makefiles are CODE files — 4-block: METADATA → SETUP → BODY → CLOSING.
//
// biblical_foundation: "Prove all things; hold fast that which is good."
//                       — 1 Thessalonians 5:21
//
// usage:   deno run --allow-read lint-makefile.ts <file-or-directory> [--verbose]
//
// ═══════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import { walk } from "jsr:@std/fs@1/walk";
import { resolve, basename, relative } from "jsr:@std/path@1";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const VERSION = "a-01.00";

// Block boundary markers for 4-block (code) structure
const REQUIRED_BLOCK_MARKERS = [
  "OMNICODE PRAGMA [PRAGMA]",
  "END PRAGMA [END]",
  "METADATA BLOCK [METADATA]",
  "END METADATA [END]",
  "SETUP BLOCK [SETUP]",
  "END SETUP [END]",
  "BODY BLOCK [BODY]",
  "END BODY [END]",
  "CLOSING BLOCK [CLOSING]",
  "END CLOSING [END]",
] as const;

// Pragma sections P1-P5
const REQUIRED_PRAGMA = ["P1: Core", "P2: Family", "P5: Summary"] as const;
const OPTIONAL_PRAGMA = ["P3: Instance", "P4: Architecture"] as const;

// Pragma field requirements
const P1_FIELDS = ["key", "from", "at"] as const;
const P2_FIELDS = ["type", "subtype", "role", "structure"] as const;
const P5_FIELDS = ["title", "brief"] as const;

// Compact metadata (P-style, used in newer aligned files)
const COMPACT_METADATA = ["M1: Identity", "M2: State", "M3: Attribution", "M4: Grounding", "M5: Dependencies"] as const;

// Detailed metadata (M.N style, used in template.mk and game-project.mk)
const DETAILED_METADATA_MARKERS = ["M.1", "M.2", "M.3", "M.4", "M.5", "M.8", "M.9"] as const;

// Cross-format keys
const CROSS_FORMAT_KEYS = ["toml", "json", "editorconfig", "makefile", "relationship"] as const;

// Makefile-specific patterns
const MAKEFILE_PATTERNS = [".mk", "Makefile", "GNUmakefile", "makefile"] as const;

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

type Severity = "error" | "warn" | "info";

interface LintResult {
  file: string;
  severity: Severity;
  rule: string;
  message: string;
}

interface LintSummary {
  file: string;
  errors: number;
  warnings: number;
  infos: number;
  results: LintResult[];
}

interface CommentSection {
  header: string;
  fields: Map<string, string>;
  startLine: number;
}

type MetadataStyle = "compact" | "detailed" | "none";

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ---------------------------------------------------------------------------
// Helpers — Result building
// ---------------------------------------------------------------------------

function error(file: string, rule: string, message: string): LintResult {
  return { file, severity: "error", rule, message };
}

function warn(file: string, rule: string, message: string): LintResult {
  return { file, severity: "warn", rule, message };
}

function info(file: string, rule: string, message: string): LintResult {
  return { file, severity: "info", rule, message };
}

// ---------------------------------------------------------------------------
// Helpers — Comment parsing
// ---------------------------------------------------------------------------

function extractCommentSections(lines: string[]): CommentSection[] {
  const sections: CommentSection[] = [];
  let current: CommentSection | null = null;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    const headerMatch = line.match(/^#\s+---\s+(.+?)\s+---\s*$/);
    if (headerMatch) {
      if (current) sections.push(current);
      current = {
        header: headerMatch[1].trim(),
        fields: new Map(),
        startLine: i,
      };
      continue;
    }

    if (current) {
      const kvMatch = line.match(/^#\s+(\w[\w_]*)\s*:\s+(.+?)\s*$/);
      if (kvMatch) {
        current.fields.set(kvMatch[1].toLowerCase(), kvMatch[2].trim());
      }
    }
  }

  if (current) sections.push(current);
  return sections;
}

function extractBlocks(lines: string[]): Map<string, string[]> {
  const blocks = new Map<string, string[]>();
  let currentBlock: string | null = null;
  let blockLines: string[] = [];

  for (const line of lines) {
    const startMatch = line.match(/^#\s+(?:OMNICODE\s+)?(\w+)\s+(?:BLOCK\s+)?\[(\w+)\]\s*$/);
    if (startMatch) {
      currentBlock = startMatch[2];
      blockLines = [];
      continue;
    }

    const endMatch = line.match(/^#\s+END\s+\w+\s+\[END\]\s*$/);
    if (endMatch && currentBlock) {
      blocks.set(currentBlock, blockLines);
      currentBlock = null;
      blockLines = [];
      continue;
    }

    if (currentBlock) {
      blockLines.push(line);
    }
  }

  return blocks;
}

/**
 * Detect whether the metadata uses compact (M1-M5), detailed (M.1-M.10),
 * or subsection (CWS-STD ──── delimited) style.
 */
function detectMetadataStyle(raw: string): MetadataStyle {
  if (raw.includes("--- M1: Identity ---") || raw.includes("--- M1:")) return "compact";
  if (raw.includes("# M.1") || raw.includes("M.1 Core Identity")) return "detailed";
  // CWS-standards style: subsection headers with ──── separators
  if (raw.includes("Biblical Foundation") && raw.includes("────")) return "detailed";
  if (raw.includes("Package:") && raw.includes("Key:") && raw.includes("────")) return "detailed";
  return "none";
}

// ---------------------------------------------------------------------------
// Core — Pragma checks
// ---------------------------------------------------------------------------

function checkPragma(file: string, lines: string[], blocks: Map<string, string[]>): LintResult[] {
  const results: LintResult[] = [];

  // Check pragma line (may be line 1 or 2 — line 1 could be .PHONY template guard)
  const pragmaLine = lines.find((l) => l.startsWith("# #!omni"));
  if (!pragmaLine) {
    results.push(error(file, "pragma/shebang", "File must have '# #!omni code --makefile' or '# #!omni template --makefile'"));
  } else if (!pragmaLine.includes("--makefile")) {
    results.push(warn(file, "pragma/format", `Pragma line should specify '--makefile': ${pragmaLine}`));
  }

  const pragmaBlock = blocks.get("PRAGMA");
  if (!pragmaBlock) {
    results.push(error(file, "pragma/block", "Missing PRAGMA block"));
    return results;
  }

  const sections = extractCommentSections(pragmaBlock);
  const sectionHeaders = sections.map((s) => s.header);

  for (const req of REQUIRED_PRAGMA) {
    if (!sectionHeaders.includes(req)) {
      results.push(error(file, `pragma/${req}`, `Missing pragma section: --- ${req} ---`));
    }
  }

  for (const opt of OPTIONAL_PRAGMA) {
    if (!sectionHeaders.includes(opt)) {
      results.push(info(file, `pragma/${opt}`, `Missing optional pragma section: --- ${opt} ---`));
    }
  }

  // Check P1 fields
  const p1 = sections.find((s) => s.header === "P1: Core");
  if (p1) {
    for (const field of P1_FIELDS) {
      if (!p1.fields.has(field)) {
        results.push(error(file, `pragma/P1.${field}`, `Missing P1 field: ${field}`));
      }
    }
  }

  // Check P2 fields
  const p2 = sections.find((s) => s.header === "P2: Family");
  if (p2) {
    for (const field of P2_FIELDS) {
      if (!p2.fields.has(field)) {
        results.push(error(file, `pragma/P2.${field}`, `Missing P2 field: ${field}`));
      }
    }
    const structure = p2.fields.get("structure");
    if (structure && structure !== "4-block") {
      results.push(warn(file, "pragma/P2.structure", `Makefile should be 4-block, found: ${structure}`));
    }
  }

  // Check P5 fields
  const p5 = sections.find((s) => s.header === "P5: Summary");
  if (p5) {
    for (const field of P5_FIELDS) {
      if (!p5.fields.has(field)) {
        results.push(error(file, `pragma/P5.${field}`, `Missing P5 field: ${field}`));
      }
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Metadata checks
// ---------------------------------------------------------------------------

function checkMetadata(file: string, raw: string, blocks: Map<string, string[]>): LintResult[] {
  const results: LintResult[] = [];

  const metaBlock = blocks.get("METADATA");
  if (!metaBlock) {
    results.push(error(file, "metadata/block", "Missing METADATA block"));
    return results;
  }

  const style = detectMetadataStyle(raw);

  if (style === "compact") {
    // Compact style: M1-M5 sections with --- headers ---
    const sections = extractCommentSections(metaBlock);
    const headers = sections.map((s) => s.header);

    for (const req of COMPACT_METADATA) {
      if (!headers.includes(req)) {
        results.push(error(file, `metadata/${req}`, `Missing metadata section: --- ${req} ---`));
      }
    }

    // M1 field checks
    const m1 = sections.find((s) => s.header === "M1: Identity");
    if (m1) {
      for (const field of ["file", "key", "title", "type"]) {
        if (!m1.fields.has(field)) {
          results.push(error(file, `metadata/M1.${field}`, `Missing M1 field: ${field}`));
        }
      }
    }

    // M2 field checks
    const m2 = sections.find((s) => s.header === "M2: State");
    if (m2) {
      for (const field of ["version", "status"]) {
        if (!m2.fields.has(field)) {
          results.push(error(file, `metadata/M2.${field}`, `Missing M2 field: ${field}`));
        }
      }
    }

    // M4 grounding
    const m4 = sections.find((s) => s.header === "M4: Grounding");
    if (m4) {
      if (!m4.fields.has("scripture")) {
        results.push(warn(file, "metadata/M4.scripture", "Missing M4 scripture — biblical anchor"));
      }
    }
  } else if (style === "detailed") {
    // Detailed style: M.1 through M.10 with # M.N headers
    const metaText = metaBlock.join("\n");

    for (const marker of DETAILED_METADATA_MARKERS) {
      if (!metaText.includes(marker)) {
        results.push(warn(file, `metadata/${marker}`, `Missing detailed metadata section: ${marker}`));
      }
    }

    // Check for Key (identity)
    if (!metaText.includes("Key:") && !metaText.includes("key:")) {
      results.push(error(file, "metadata/key", "Missing Key field in metadata"));
    }
  } else {
    results.push(warn(file, "metadata/style", "No recognized metadata style (compact M1-M5 or detailed M.1-M.10)"));
  }

  // Cross-format references (any style)
  const fullMeta = metaBlock.join("\n");
  if (!fullMeta.toLowerCase().includes("cross-format") && !fullMeta.toLowerCase().includes("cross_format")) {
    results.push(warn(file, "metadata/cross_format", "Missing cross-format references in metadata"));
  } else {
    for (const key of CROSS_FORMAT_KEYS) {
      const pattern = new RegExp(`#\\s+${key}:`, "i");
      if (!pattern.test(fullMeta)) {
        results.push(info(file, `metadata/cross_format.${key}`, `Missing cross-format key: ${key}`));
      }
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Setup checks
// ---------------------------------------------------------------------------

function checkSetup(file: string, blocks: Map<string, string[]>): LintResult[] {
  const results: LintResult[] = [];

  const setupBlock = blocks.get("SETUP");
  if (!setupBlock) {
    results.push(error(file, "setup/block", "Missing SETUP block"));
    return results;
  }

  // Check for .PHONY declarations (Makefile convention)
  const setupText = setupBlock.join("\n");
  const hasPhony = setupText.includes(".PHONY");
  if (!hasPhony) {
    results.push(info(file, "setup/phony", "No .PHONY declarations in SETUP — targets may conflict with files"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Body checks
// ---------------------------------------------------------------------------

function checkBody(file: string, blocks: Map<string, string[]>): LintResult[] {
  const results: LintResult[] = [];

  const bodyBlock = blocks.get("BODY");
  if (!bodyBlock) {
    results.push(error(file, "body/block", "Missing BODY block"));
    return results;
  }

  // Check for actual Make targets (lines starting with word: or word:word)
  const bodyText = bodyBlock.join("\n");
  const hasTargets = /^[a-zA-Z_][\w-]*\s*:/m.test(bodyText);
  if (!hasTargets) {
    results.push(warn(file, "body/targets", "No Make targets found in BODY — is this a template?"));
  }

  // Check for help target
  if (!bodyText.includes("help:") && !bodyText.includes("help :")) {
    results.push(info(file, "body/help", "No help target in BODY — recommended for documentation"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Closing checks
// ---------------------------------------------------------------------------

function checkClosing(file: string, raw: string, blocks: Map<string, string[]>): LintResult[] {
  const results: LintResult[] = [];

  const closingBlock = blocks.get("CLOSING");
  if (!closingBlock) {
    results.push(error(file, "closing/block", "Missing CLOSING block"));
    return results;
  }

  const closingText = closingBlock.join("\n");

  // Detect closing style: compact (X1/X4/X5) or detailed (X.1-X.10) or subsection (CWS-STD)
  const hasCompact = closingText.includes("X1:") || closingText.includes("X4:") || closingText.includes("X5:");
  const hasDetailed = closingText.includes("X.1") || closingText.includes("X.2");
  // CWS-standards closing style uses named subsections like "Modification Policy", "Troubleshooting"
  const hasSubsection = closingText.includes("Modification Policy") || closingText.includes("Makefile Validation");

  if (hasCompact) {
    // Compact closing: X1 Policy, X4 Reference, X5 Note
    if (!closingText.includes("X1")) {
      results.push(error(file, "closing/X1", "Missing X1 Policy section"));
    }
    if (!closingText.includes("X4")) {
      results.push(error(file, "closing/X4", "Missing X4 Reference section"));
    }
    if (!closingText.includes("X5")) {
      results.push(error(file, "closing/X5", "Missing X5 Note section"));
    }

    // Check X1 has never/careful/safe
    if (closingText.includes("X1")) {
      if (!closingText.includes("never:")) {
        results.push(warn(file, "closing/X1.never", "Missing X1 'never' policy"));
      }
      if (!closingText.includes("careful:")) {
        results.push(warn(file, "closing/X1.careful", "Missing X1 'careful' policy"));
      }
      if (!closingText.includes("safe:")) {
        results.push(warn(file, "closing/X1.safe", "Missing X1 'safe' policy"));
      }
    }
  } else if (hasDetailed) {
    // Detailed closing: X.1 through X.10+
    const detailedSections = ["X.1", "X.5", "X.6", "X.9"];
    for (const s of detailedSections) {
      if (!closingText.includes(s)) {
        results.push(info(file, `closing/${s}`, `Missing detailed closing section: ${s}`));
      }
    }
  } else if (hasSubsection) {
    // CWS-standards subsection style
    if (!closingText.includes("Modification Policy")) {
      results.push(info(file, "closing/modification-policy", "Missing Modification Policy subsection"));
    }
  } else {
    // Some closing content but no recognized structure
    results.push(warn(file, "closing/style", "Closing block has no recognized structure (compact X1/X4/X5 or detailed X.1-X.10)"));
  }

  // Check for Genesis anchor
  const lowerClosing = closingText.toLowerCase();
  if (!lowerClosing.includes("genesis") && !lowerClosing.includes("psalm") && !lowerClosing.includes("proverbs")) {
    results.push(info(file, "closing/scripture", "Closing block has no scripture reference — recommended for grounding"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Block boundary checks
// ---------------------------------------------------------------------------

function checkBlockMarkers(file: string, raw: string): LintResult[] {
  const results: LintResult[] = [];

  for (const marker of REQUIRED_BLOCK_MARKERS) {
    if (!raw.includes(marker)) {
      results.push(error(file, "structure/block-marker", `Missing block boundary marker: ${marker}`));
    }
  }

  // 4-block should NOT have CONTENT block (that's 3-block data)
  if (raw.includes("CONTENT BLOCK [CONTENT]")) {
    results.push(warn(file, "structure/wrong-block", "Makefile has CONTENT block — 4-block code files use SETUP + BODY, not CONTENT"));
  }

  // Check separator style
  const lines = raw.split("\n");
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    // Only flag standalone comment separators, not @echo lines
    if (line.match(/^#\s+=====+\s*$/) && !line.includes("═") && !line.includes("@echo")) {
      results.push(warn(file, "structure/separator-style",
        `Line ${i + 1}: Uses '=' separators — should use '═' (U+2550) for block boundaries`));
    }
  }

  // Check block order (PRAGMA before METADATA before SETUP before BODY before CLOSING)
  const blockOrder = ["PRAGMA", "METADATA", "SETUP", "BODY", "CLOSING"];
  let lastIndex = -1;
  for (const block of blockOrder) {
    const marker = block === "PRAGMA" ? `OMNICODE PRAGMA [PRAGMA]` : `${block} BLOCK [${block}]`;
    const idx = raw.indexOf(marker);
    if (idx !== -1) {
      if (idx < lastIndex) {
        results.push(error(file, "structure/block-order", `Block ${block} appears before a preceding block — order should be PRAGMA → METADATA → SETUP → BODY → CLOSING`));
      }
      lastIndex = idx;
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Cross-reference checks
// ---------------------------------------------------------------------------

function checkCrossReferences(file: string, blocks: Map<string, string[]>): LintResult[] {
  const results: LintResult[] = [];

  const pragmaBlock = blocks.get("PRAGMA");
  const metaBlock = blocks.get("METADATA");
  if (!pragmaBlock || !metaBlock) return results;

  const pragmaSections = extractCommentSections(pragmaBlock);
  const p1 = pragmaSections.find((s) => s.header === "P1: Core");
  const p5 = pragmaSections.find((s) => s.header === "P5: Summary");

  // For compact metadata
  const metaSections = extractCommentSections(metaBlock);
  const m1 = metaSections.find((s) => s.header === "M1: Identity");

  // Key match
  if (p1?.fields.has("key") && m1?.fields.has("key")) {
    const pKey = p1.fields.get("key")!;
    const mKey = m1.fields.get("key")!;
    if (pKey !== mKey) {
      results.push(error(file, "xref/key-mismatch", `P1.key "${pKey}" !== M1.key "${mKey}" — must match`));
    }
  }

  // Title match
  if (p5?.fields.has("title") && m1?.fields.has("title")) {
    const pTitle = p5.fields.get("title")!;
    const mTitle = m1.fields.get("title")!;
    if (pTitle !== mTitle) {
      results.push(warn(file, "xref/title-mismatch", `P5.title "${pTitle}" !== M1.title "${mTitle}"`));
    }
  }

  // Filename match
  if (m1?.fields.has("file")) {
    const mFile = m1.fields.get("file")!;
    if (mFile !== file) {
      results.push(warn(file, "xref/filename-mismatch", `M1.file "${mFile}" !== actual filename "${file}"`));
    }
  }

  // For detailed metadata: check Key in raw text
  // Skip template placeholders (values containing [ brackets)
  if (!m1) {
    const metaText = metaBlock.join("\n");
    const keyMatch = metaText.match(/^#\s+Key:\s+(.+)/m);
    const pragmaKeyMatch = p1?.fields.get("key");
    if (keyMatch && pragmaKeyMatch) {
      const detailedKey = keyMatch[1].trim();
      // Skip comparison if detailed key is a template placeholder
      if (detailedKey.includes("[") || detailedKey.includes("]")) {
        results.push(info(file, "xref/key-template-placeholder",
          `Detailed metadata Key is a template placeholder: "${detailedKey}"`));
      } else if (detailedKey !== pragmaKeyMatch) {
        results.push(warn(file, "xref/key-mismatch-detailed",
          `Detailed metadata Key "${detailedKey}" !== P1.key "${pragmaKeyMatch}"`));
      }
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Makefile-specific quality checks
// ---------------------------------------------------------------------------

function checkMakefileQuality(file: string, raw: string): LintResult[] {
  const results: LintResult[] = [];
  const lines = raw.split("\n");

  // Check for tab vs space in recipe lines
  let inRecipe = false;
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];

    // Detect target lines (not comments, not blank, has colon)
    if (!line.startsWith("#") && !line.startsWith("\t") && /^[a-zA-Z_][\w.-]*\s*:/.test(line)) {
      inRecipe = true;
      continue;
    }

    // Recipe lines should start with tab
    if (inRecipe && line.length > 0) {
      if (line.startsWith("\t") || line.startsWith("#") || line === "") {
        // Valid recipe line or comment or blank
        if (line === "" || line.startsWith("#")) inRecipe = false;
      } else if (line.match(/^\s+@/) || line.match(/^\s+\$/) || line.match(/^\s+-/)) {
        // Space-indented recipe — this is an error in Makefiles
        results.push(warn(file, "quality/tab-recipe",
          `Line ${i + 1}: Recipe line uses spaces instead of tab — Make requires tabs`));
        if (results.filter((r) => r.rule === "quality/tab-recipe").length >= 3) break; // Cap warnings
      } else {
        inRecipe = false;
      }
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — File linting orchestrator
// ---------------------------------------------------------------------------

async function lintFile(filePath: string): Promise<LintSummary> {
  const name = basename(filePath);
  const results: LintResult[] = [];

  let raw: string;
  try {
    raw = await Deno.readTextFile(filePath);
  } catch (e) {
    results.push(error(name, "io/read", `Cannot read file: ${e}`));
    return summarize(name, results);
  }

  const lines = raw.split("\n");
  const blocks = extractBlocks(lines);

  // Block structure checks
  results.push(...checkBlockMarkers(name, raw));

  // Pragma checks
  results.push(...checkPragma(name, lines, blocks));

  // Metadata checks
  results.push(...checkMetadata(name, raw, blocks));

  // Setup checks
  results.push(...checkSetup(name, blocks));

  // Body checks
  results.push(...checkBody(name, blocks));

  // Closing checks
  results.push(...checkClosing(name, raw, blocks));

  // Cross-reference checks
  results.push(...checkCrossReferences(name, blocks));

  // Makefile quality checks
  results.push(...checkMakefileQuality(name, raw));

  return summarize(name, results);
}

function summarize(file: string, results: LintResult[]): LintSummary {
  return {
    file,
    errors: results.filter((r) => r.severity === "error").length,
    warnings: results.filter((r) => r.severity === "warn").length,
    infos: results.filter((r) => r.severity === "info").length,
    results,
  };
}

// ---------------------------------------------------------------------------
// Display
// ---------------------------------------------------------------------------

const COLORS = {
  reset: "\x1b[0m",
  red: "\x1b[31m",
  yellow: "\x1b[33m",
  blue: "\x1b[34m",
  green: "\x1b[32m",
  dim: "\x1b[2m",
  bold: "\x1b[1m",
};

function severityColor(s: Severity): string {
  switch (s) {
    case "error": return COLORS.red;
    case "warn": return COLORS.yellow;
    case "info": return COLORS.blue;
  }
}

function severityIcon(s: Severity): string {
  switch (s) {
    case "error": return "x";
    case "warn": return "!";
    case "info": return "i";
  }
}

function printSummary(summary: LintSummary, verbose: boolean): void {
  const { file, errors, warnings, infos, results } = summary;
  const total = errors + warnings + infos;

  if (total === 0) {
    console.log(`${COLORS.green}  OK${COLORS.reset}  ${file}`);
    return;
  }

  const statusColor = errors > 0 ? COLORS.red : warnings > 0 ? COLORS.yellow : COLORS.blue;
  const statusIcon = errors > 0 ? "FAIL" : warnings > 0 ? "WARN" : "INFO";
  console.log(`${statusColor}${statusIcon}${COLORS.reset}  ${file}  ${COLORS.dim}(${errors}E ${warnings}W ${infos}I)${COLORS.reset}`);

  if (verbose || errors > 0) {
    for (const r of results) {
      if (!verbose && r.severity === "info") continue;
      const color = severityColor(r.severity);
      const icon = severityIcon(r.severity);
      console.log(`  ${color}${icon}${COLORS.reset} ${COLORS.dim}[${r.rule}]${COLORS.reset} ${r.message}`);
    }
  }
}

function printTotal(summaries: LintSummary[]): void {
  const totalErrors = summaries.reduce((s, r) => s + r.errors, 0);
  const totalWarnings = summaries.reduce((s, r) => s + r.warnings, 0);
  const totalInfos = summaries.reduce((s, r) => s + r.infos, 0);
  const totalFiles = summaries.length;
  const passFiles = summaries.filter((s) => s.errors === 0 && s.warnings === 0).length;

  console.log("");
  console.log(`${COLORS.bold}Summary:${COLORS.reset} ${totalFiles} files, ${passFiles} clean`);
  console.log(
    `  ${COLORS.red}${totalErrors} errors${COLORS.reset}  ` +
    `${COLORS.yellow}${totalWarnings} warnings${COLORS.reset}  ` +
    `${COLORS.blue}${totalInfos} info${COLORS.reset}`
  );

  if (totalErrors === 0) {
    console.log(`\n${COLORS.green}All files structurally valid.${COLORS.reset}`);
  } else {
    console.log(`\n${COLORS.red}${totalErrors} structural errors must be fixed.${COLORS.reset}`);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════

// ---------------------------------------------------------------------------
// CLI entrypoint
// ---------------------------------------------------------------------------

async function main(): Promise<void> {
  const args = Deno.args;

  if (args.length === 0 || args.includes("--help") || args.includes("-h")) {
    console.log(`
${COLORS.bold}lint-makefile${COLORS.reset} v${VERSION} — Makefile Template Alignment Linter

${COLORS.dim}"Prove all things; hold fast that which is good." — 1 Thess 5:21${COLORS.reset}

Usage:
  deno run --allow-read lint-makefile.ts <file-or-directory> [options]

Options:
  --verbose, -v     Show all results (including info)
  --errors-only     Show only errors
  --summary         Show only file-level summary
  --help, -h        Show this help

Examples:
  deno run --allow-read lint-makefile.ts makefiles/
  deno run --allow-read lint-makefile.ts template.mk -v
  deno run --allow-read lint-makefile.ts . --summary
`);
    Deno.exit(0);
  }

  const verbose = args.includes("--verbose") || args.includes("-v");
  const errorsOnly = args.includes("--errors-only");
  const summaryOnly = args.includes("--summary");

  const targets = args.filter((a) => !a.startsWith("-"));

  // Gather Makefile files (.mk, Makefile, GNUmakefile)
  const files: string[] = [];
  for (const target of targets) {
    const resolved = resolve(target);
    try {
      const stat = await Deno.stat(resolved);
      if (stat.isFile && isMakefile(resolved)) {
        files.push(resolved);
      } else if (stat.isDirectory) {
        for await (const entry of walk(resolved, { maxDepth: 3 })) {
          if (entry.isFile && isMakefile(entry.path)) {
            files.push(entry.path);
          }
        }
      }
    } catch {
      console.error(`${COLORS.red}Cannot access: ${target}${COLORS.reset}`);
    }
  }

  if (files.length === 0) {
    console.error(`${COLORS.red}No Makefile files found.${COLORS.reset}`);
    Deno.exit(1);
  }

  files.sort();

  const cwd = Deno.cwd();
  console.log(`${COLORS.bold}Linting ${files.length} Makefile(s)...${COLORS.reset}\n`);

  const summaries: LintSummary[] = [];
  for (const file of files) {
    const summary = await lintFile(file);
    summary.file = relative(cwd, file);
    summaries.push(summary);

    if (!summaryOnly) {
      if (errorsOnly) {
        if (summary.errors > 0) printSummary(summary, verbose);
      } else {
        printSummary(summary, verbose);
      }
    }
  }

  if (summaryOnly) {
    for (const s of summaries) {
      const status = s.errors === 0
        ? `${COLORS.green}OK${COLORS.reset}`
        : `${COLORS.red}FAIL${COLORS.reset}`;
      console.log(`${status}  ${s.file}  (${s.errors}E ${s.warnings}W ${s.infos}I)`);
    }
  }

  printTotal(summaries);

  const hasErrors = summaries.some((s) => s.errors > 0);
  Deno.exit(hasErrors ? 1 : 0);
}

function isMakefile(path: string): boolean {
  const name = basename(path);
  return name.endsWith(".mk") || name === "Makefile" || name === "GNUmakefile" || name === "makefile";
}

main();
