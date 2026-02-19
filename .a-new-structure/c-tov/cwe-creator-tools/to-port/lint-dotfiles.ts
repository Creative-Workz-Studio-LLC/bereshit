#!/usr/bin/env -S deno run --allow-read
// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// file:    lint-dotfiles.ts
// key:     B-tools-lint-dotfiles
// title:   Dotfile Template Alignment Linter
// type:    Tool (Linter)
// version: a-01.00
// created: 2026-02-10
// updated: 2026-02-10
// authors: Nova Dawn (CPI-SI)
// purpose: Validate dotfile templates (editorconfig, gitignore, gitmessage, env)
//          against OmniCode 3-block standard. Handles all comment-based dotfile
//          formats with shared structural checks and format-specific content
//          validation. Supersedes lint-editorconfig.ts for dotfile linting.
//
// biblical_foundation: "Prove all things; hold fast that which is good."
//                       — 1 Thessalonians 5:21
//
// usage:   deno run --allow-read lint-dotfiles.ts <file-or-directory> [--verbose]
//
// ═══════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import { walk } from "jsr:@std/fs@1/walk";
import { resolve, basename, relative, extname } from "jsr:@std/path@1";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const VERSION = "a-01.00";

// Supported dotfile extensions
const DOTFILE_EXTENSIONS = new Set([
  ".editorconfig",
  ".gitignore",
  ".gitmessage",
  ".env",
]);

// Check if a file path is a recognized dotfile.
// Handles both template names (gitignore.gitignore) and root names (.gitignore).
function isDotfile(filePath: string): boolean {
  const ext = extname(filePath);
  if (DOTFILE_EXTENSIONS.has(ext)) return true;
  // Root dotfiles: extname('.gitignore') = '', but basename IS the extension
  const base = basename(filePath);
  if (base.startsWith(".") && DOTFILE_EXTENSIONS.has(base)) return true;
  return false;
}

// Block boundary markers for 3-block (data) structure
const REQUIRED_BLOCK_MARKERS = [
  "OMNICODE PRAGMA [PRAGMA]",
  "END PRAGMA [END]",
  "METADATA BLOCK [METADATA]",
  "END METADATA [END]",
  "CONTENT BLOCK [CONTENT]",
  "END CONTENT [END]",
  "CLOSING BLOCK [CLOSING]",
  "END CLOSING [END]",
] as const;

// Pragma sections P1-P5
const REQUIRED_PRAGMA = [
  "P1: Core",
  "P2: Family",
  "P3: Instance",
  "P5: Summary",
] as const;
const OPTIONAL_PRAGMA = ["P4: Architecture"] as const;

// Pragma field requirements
const P1_FIELDS = ["key", "from", "at"] as const;
const P2_FIELDS = ["type", "subtype", "role", "structure"] as const;
const P3_FIELDS = ["component", "layer", "provides"] as const;
const P5_FIELDS = ["title", "brief"] as const;

// Metadata sections M1-M5
const REQUIRED_METADATA = [
  "M1: Identity",
  "M2: State",
  "M3: Attribution",
  "M4: Grounding",
  "M5: Dependencies",
] as const;

// M1 required fields
const M1_FIELDS = ["file", "key", "title", "type"] as const;
// M2 required fields
const M2_FIELDS = ["version", "status"] as const;
// M5 cross_format expected keys
const CROSS_FORMAT_KEYS = [
  "toml",
  "json",
  "editorconfig",
  "makefile",
  "relationship",
] as const;

// Closing sections
const REQUIRED_CLOSING = [
  "X1: Policy",
  "X4: Reference",
  "X5: Note",
] as const;

// Format-to-pragma mapping
const FORMAT_PRAGMA: Record<string, string> = {
  editorconfig: "--editorconfig",
  gitignore: "--gitignore",
  gitmessage: "--gitmessage",
  env: "--env",
};

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

type Severity = "error" | "warn" | "info";

type DotfileFormat = "editorconfig" | "gitignore" | "gitmessage" | "env" | "unknown";

interface LintResult {
  file: string;
  severity: Severity;
  rule: string;
  message: string;
}

interface LintSummary {
  file: string;
  format: DotfileFormat;
  errors: number;
  warnings: number;
  infos: number;
  results: LintResult[];
}

interface CommentSection {
  header: string;
  fields: Map<string, string>;
  startLine: number;
  endLine: number;
}

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
// Helpers — Format detection
// ---------------------------------------------------------------------------

/**
 * Detect dotfile format from pragma line and file extension.
 */
function detectFormat(lines: string[], fileName: string): DotfileFormat {
  // Check pragma line first
  const pragmaLine = lines[0] || "";
  for (const [format, flag] of Object.entries(FORMAT_PRAGMA)) {
    if (pragmaLine.includes(flag)) {
      return format as DotfileFormat;
    }
  }

  // Fall back to extension or basename (for root dotfiles like .gitignore)
  const ext = extname(fileName);
  const base = basename(fileName);
  const check = ext || base; // extname('.gitignore')='' so use basename
  if (check === ".editorconfig") return "editorconfig";
  if (check === ".gitignore") return "gitignore";
  if (check === ".gitmessage") return "gitmessage";
  if (check === ".env") return "env";

  return "unknown";
}

// ---------------------------------------------------------------------------
// Helpers — Comment parsing
// ---------------------------------------------------------------------------

/**
 * Extract sections delimited by `# --- HEADER ---` from comment lines.
 */
function extractCommentSections(lines: string[]): CommentSection[] {
  const sections: CommentSection[] = [];
  let current: CommentSection | null = null;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    const headerMatch = line.match(/^#\s+---\s+(.+?)\s+---\s*$/);
    if (headerMatch) {
      if (current) {
        current.endLine = i - 1;
        sections.push(current);
      }
      current = {
        header: headerMatch[1].trim(),
        fields: new Map(),
        startLine: i,
        endLine: i,
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

  if (current) {
    current.endLine = lines.length - 1;
    sections.push(current);
  }

  return sections;
}

/**
 * Extract block regions from the file — between boundary markers.
 */
function extractBlocks(lines: string[]): Map<string, string[]> {
  const blocks = new Map<string, string[]>();
  let currentBlock: string | null = null;
  let blockLines: string[] = [];

  for (const line of lines) {
    const startMatch = line.match(
      /^#\s+(?:OMNICODE\s+)?(\w+)\s+(?:BLOCK\s+)?\[(\w+)\]\s*$/,
    );
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

// ---------------------------------------------------------------------------
// Core — Block boundary checks (shared)
// ---------------------------------------------------------------------------

function checkBlockMarkers(
  file: string,
  raw: string,
): LintResult[] {
  const results: LintResult[] = [];

  for (const marker of REQUIRED_BLOCK_MARKERS) {
    if (!raw.includes(marker)) {
      results.push(
        error(file, "structure/block-marker", `Missing block boundary marker: ${marker}`),
      );
    }
  }

  // 3-block should NOT have SETUP or BODY blocks
  if (raw.includes("SETUP BLOCK [SETUP]")) {
    results.push(
      warn(file, "structure/wrong-block", "Dotfile has SETUP block — 3-block data files should not have SETUP"),
    );
  }
  if (raw.includes("BODY BLOCK [BODY]")) {
    results.push(
      warn(file, "structure/wrong-block", "Dotfile has BODY block — 3-block data files should not have BODY"),
    );
  }

  // Check separator style (should use ═══ not ===)
  const lines = raw.split("\n");
  for (let i = 0; i < lines.length; i++) {
    if (lines[i].match(/^#\s+=====+\s*$/) && !lines[i].includes("═")) {
      results.push(
        warn(
          file,
          "structure/separator-style",
          `Line ${i + 1}: Uses '=' separators — should use '═' (U+2550) for block boundaries`,
        ),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Pragma checks (shared)
// ---------------------------------------------------------------------------

function checkPragma(
  file: string,
  lines: string[],
  blocks: Map<string, string[]>,
  format: DotfileFormat,
): LintResult[] {
  const results: LintResult[] = [];

  // Check pragma line
  if (!lines[0]?.startsWith("# #!omni")) {
    results.push(error(file, "pragma/shebang", "File must start with '# #!omni data --<format>'"));
  } else {
    const expectedFlag = FORMAT_PRAGMA[format];
    if (expectedFlag && !lines[0].includes(expectedFlag)) {
      results.push(
        warn(file, "pragma/format", `Pragma should specify '${expectedFlag}': ${lines[0]}`),
      );
    }
  }

  // Check PRAGMA block exists
  const pragmaBlock = blocks.get("PRAGMA");
  if (!pragmaBlock) {
    results.push(error(file, "pragma/block", "Missing PRAGMA block"));
    return results;
  }

  // Check sections within PRAGMA
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

  // P1 fields
  const p1 = sections.find((s) => s.header === "P1: Core");
  if (p1) {
    for (const field of P1_FIELDS) {
      if (!p1.fields.has(field)) {
        results.push(error(file, `pragma/P1.${field}`, `Missing P1 field: ${field}`));
      }
    }
  }

  // P2 fields
  const p2 = sections.find((s) => s.header === "P2: Family");
  if (p2) {
    for (const field of P2_FIELDS) {
      if (!p2.fields.has(field)) {
        results.push(error(file, `pragma/P2.${field}`, `Missing P2 field: ${field}`));
      }
    }
    const structure = p2.fields.get("structure");
    if (structure && structure !== "3-block") {
      results.push(
        warn(file, "pragma/P2.structure", `Dotfile should be 3-block, found: ${structure}`),
      );
    }
  }

  // P3 fields
  const p3 = sections.find((s) => s.header === "P3: Instance");
  if (p3) {
    for (const field of P3_FIELDS) {
      if (!p3.fields.has(field)) {
        results.push(warn(file, `pragma/P3.${field}`, `Missing P3 field: ${field}`));
      }
    }
  }

  // P5 fields
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
// Core — Metadata checks (shared)
// ---------------------------------------------------------------------------

function checkMetadata(
  file: string,
  blocks: Map<string, string[]>,
): LintResult[] {
  const results: LintResult[] = [];

  const metaBlock = blocks.get("METADATA");
  if (!metaBlock) {
    results.push(error(file, "metadata/block", "Missing METADATA block"));
    return results;
  }

  const sections = extractCommentSections(metaBlock);
  const sectionHeaders = sections.map((s) => s.header);

  for (const req of REQUIRED_METADATA) {
    if (!sectionHeaders.includes(req)) {
      results.push(error(file, `metadata/${req}`, `Missing metadata section: --- ${req} ---`));
    }
  }

  // M1 fields
  const m1 = sections.find((s) => s.header === "M1: Identity");
  if (m1) {
    for (const field of M1_FIELDS) {
      if (!m1.fields.has(field)) {
        results.push(error(file, `metadata/M1.${field}`, `Missing M1 field: ${field}`));
      }
    }
  }

  // M2 fields
  const m2 = sections.find((s) => s.header === "M2: State");
  if (m2) {
    for (const field of M2_FIELDS) {
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
    if (!m4.fields.has("anchor")) {
      results.push(warn(file, "metadata/M4.anchor", "Missing M4 anchor — Genesis 1:1 connection"));
    }
  }

  // M5 cross_format
  const m5Block = metaBlock.join("\n");
  if (!m5Block.includes("cross_format")) {
    results.push(
      warn(file, "metadata/M5.cross_format", "Missing cross_format references in M5 Dependencies"),
    );
  } else {
    for (const key of CROSS_FORMAT_KEYS) {
      const pattern = new RegExp(`#\\s+${key}:`);
      if (!pattern.test(m5Block)) {
        results.push(info(file, `metadata/M5.cross_format.${key}`, `Missing cross_format.${key}`));
      }
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Closing checks (shared)
// ---------------------------------------------------------------------------

function checkClosing(
  file: string,
  blocks: Map<string, string[]>,
): LintResult[] {
  const results: LintResult[] = [];

  const closingBlock = blocks.get("CLOSING");
  if (!closingBlock) {
    results.push(error(file, "closing/block", "Missing CLOSING block"));
    return results;
  }

  const sections = extractCommentSections(closingBlock);
  const sectionHeaders = sections.map((s) => s.header);
  const closingText = closingBlock.join("\n");

  for (const req of REQUIRED_CLOSING) {
    if (!sectionHeaders.includes(req)) {
      const xNum = req.split(":")[0].trim();
      if (!closingText.includes(xNum)) {
        results.push(error(file, `closing/${req}`, `Missing closing section: --- ${req} ---`));
      }
    }
  }

  // X1 policy has never/careful/safe
  const x1 = sections.find((s) => s.header === "X1: Policy");
  if (x1 || closingText.includes("X1")) {
    if (!closingText.includes("never:")) {
      results.push(warn(file, "closing/X1.never", "Missing X1 'never' policy — what must not change"));
    }
    if (!closingText.includes("safe:")) {
      results.push(warn(file, "closing/X1.safe", "Missing X1 'safe' policy — what's safe to change"));
    }
  }

  // X5 anchor
  const lowerClosing = closingText.toLowerCase();
  if (
    !lowerClosing.includes("genesis 1:1") &&
    !lowerClosing.includes("genesis 1") &&
    !lowerClosing.includes("anchor")
  ) {
    results.push(info(file, "closing/X5.genesis", "X5 should reference Genesis 1:1 anchor"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Cross-reference checks (shared)
// ---------------------------------------------------------------------------

function checkCrossReferences(
  file: string,
  blocks: Map<string, string[]>,
): LintResult[] {
  const results: LintResult[] = [];

  const pragmaBlock = blocks.get("PRAGMA");
  const metaBlock = blocks.get("METADATA");
  if (!pragmaBlock || !metaBlock) return results;

  const pragmaSections = extractCommentSections(pragmaBlock);
  const metaSections = extractCommentSections(metaBlock);

  const p1 = pragmaSections.find((s) => s.header === "P1: Core");
  const m1 = metaSections.find((s) => s.header === "M1: Identity");
  const p5 = pragmaSections.find((s) => s.header === "P5: Summary");

  // Key match: P1.key === M1.key
  if (p1?.fields.has("key") && m1?.fields.has("key")) {
    const pKey = p1.fields.get("key")!;
    const mKey = m1.fields.get("key")!;
    if (pKey !== mKey) {
      results.push(error(file, "xref/key-mismatch", `P1.key "${pKey}" !== M1.key "${mKey}" — must match`));
    }
  }

  // Title match: P5.title === M1.title
  if (p5?.fields.has("title") && m1?.fields.has("title")) {
    const pTitle = p5.fields.get("title")!;
    const mTitle = m1.fields.get("title")!;
    if (pTitle !== mTitle) {
      results.push(warn(file, "xref/title-mismatch", `P5.title "${pTitle}" !== M1.title "${mTitle}"`));
    }
  }

  // Filename match: M1.file === actual filename
  if (m1?.fields.has("file")) {
    const mFile = m1.fields.get("file")!;
    if (mFile !== file) {
      results.push(warn(file, "xref/filename-mismatch", `M1.file "${mFile}" !== actual filename "${file}"`));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Format-specific — EditorConfig content checks
// ---------------------------------------------------------------------------

function checkEditorConfigContent(
  file: string,
  raw: string,
  blocks: Map<string, string[]>,
): LintResult[] {
  const results: LintResult[] = [];

  const contentBlock = blocks.get("CONTENT");
  if (!contentBlock) return results;

  if (!raw.includes("root = true")) {
    results.push(warn(file, "content/root", "Missing 'root = true' — editorconfig may not work as root config"));
  }

  if (!raw.includes("[*]")) {
    results.push(warn(file, "content/default", "Missing [*] default section — all files should have base settings"));
  }

  const defaultSection = raw.match(/\[\*\]([\s\S]*?)(?=\n\[|$)/);
  if (defaultSection) {
    const defaults = defaultSection[1];
    if (!defaults.includes("charset")) {
      results.push(warn(file, "content/charset", "Missing charset in [*] — should be utf-8"));
    }
    if (!defaults.includes("end_of_line")) {
      results.push(warn(file, "content/end_of_line", "Missing end_of_line in [*] — should be lf"));
    }
  }

  if (!raw.includes("[Makefile]") && !raw.includes("[*.mk]")) {
    results.push(info(file, "content/makefile", "Missing Makefile section — tabs are required for Make syntax"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Format-specific — Gitignore content checks
// ---------------------------------------------------------------------------

function checkGitignoreContent(
  file: string,
  raw: string,
  blocks: Map<string, string[]>,
): LintResult[] {
  const results: LintResult[] = [];

  const contentBlock = blocks.get("CONTENT");
  if (!contentBlock) return results;

  // Gitignore should have actual patterns (uncommented lines in CONTENT)
  const patternLines = contentBlock.filter(
    (l) => l.trim() !== "" && !l.startsWith("#"),
  );
  if (patternLines.length === 0) {
    results.push(warn(file, "content/no-patterns", "No gitignore patterns found in CONTENT — file would ignore nothing"));
  }

  // Should exclude .env files
  if (!raw.includes(".env")) {
    results.push(warn(file, "content/env", "Missing .env pattern — secrets should be excluded from version control"));
  }

  // Should have OS-generated patterns
  const contentText = contentBlock.join("\n");
  if (!contentText.includes(".DS_Store") && !contentText.includes("Thumbs.db")) {
    results.push(info(file, "content/os", "Missing OS-generated file patterns (.DS_Store, Thumbs.db)"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Format-specific — Gitmessage content checks
// ---------------------------------------------------------------------------

function checkGitmessageContent(
  file: string,
  _raw: string,
  blocks: Map<string, string[]>,
): LintResult[] {
  const results: LintResult[] = [];

  const contentBlock = blocks.get("CONTENT");
  if (!contentBlock) return results;

  const contentText = contentBlock.join("\n");

  // Should have actual commit template (uncommented lines)
  const templateLines = contentBlock.filter(
    (l) => l.trim() !== "" && !l.startsWith("#"),
  );
  if (templateLines.length === 0) {
    results.push(
      warn(file, "content/no-template", "No commit template text found — all lines are comments"),
    );
  }

  // Should have attribution block
  if (!contentText.includes("Generated by") && !contentText.includes("Co-Authored-By")) {
    results.push(warn(file, "content/attribution", "Missing attribution block in commit template"));
  }

  // Should have type reference
  if (!contentText.includes("INIT") || !contentText.includes("BUILD") || !contentText.includes("FIX")) {
    results.push(info(file, "content/types", "Missing Building Block Method type reference in Cv section"));
  }

  // Should reference [repo-name] TYPE: format
  if (!contentText.includes("[repo-name]") && !contentText.includes("TYPE:")) {
    results.push(warn(file, "content/format", "Missing subject line format reference ([repo-name] TYPE: Description)"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Format-specific — Env content checks
// ---------------------------------------------------------------------------

function checkEnvContent(
  file: string,
  _raw: string,
  blocks: Map<string, string[]>,
): LintResult[] {
  const results: LintResult[] = [];

  const contentBlock = blocks.get("CONTENT");
  if (!contentBlock) return results;

  const contentText = contentBlock.join("\n");

  // Should have actual KEY=VALUE pairs (uncommented lines)
  const kvLines = contentBlock.filter(
    (l) => l.trim() !== "" && !l.startsWith("#") && l.includes("="),
  );
  if (kvLines.length === 0) {
    results.push(
      warn(file, "content/no-vars", "No KEY=VALUE pairs found in CONTENT — env file is empty"),
    );
  }

  // Should have security-related comments
  if (!contentText.toLowerCase().includes("never commit") && !contentText.toLowerCase().includes("secret")) {
    results.push(info(file, "content/security-warning", "Missing security warning about committing secrets"));
  }

  // Should have loading pattern documentation
  if (!contentText.includes("dotenv") && !contentText.includes("godotenv") && !contentText.includes("load_dotenv")) {
    results.push(info(file, "content/loading", "Missing loading pattern documentation (dotenv, godotenv, etc.)"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Cc/Co/Cv label checks (shared)
// ---------------------------------------------------------------------------

function checkContentLabels(
  file: string,
  blocks: Map<string, string[]>,
): LintResult[] {
  const results: LintResult[] = [];

  const contentBlock = blocks.get("CONTENT");
  if (!contentBlock) return results;

  const contentText = contentBlock.join("\n");

  // Check for Cc/Co/Cv content sub-section labels
  if (!contentText.includes("# Cc") && !contentText.includes("Cc —")) {
    results.push(info(file, "content/label-Cc", "Missing Cc (Configuration) label in CONTENT block"));
  }
  if (!contentText.includes("# Co") && !contentText.includes("Co —")) {
    results.push(info(file, "content/label-Co", "Missing Co (Operations) label in CONTENT block"));
  }
  if (!contentText.includes("# Cv") && !contentText.includes("Cv —")) {
    results.push(info(file, "content/label-Cv", "Missing Cv (Verification) label in CONTENT block"));
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
    return summarize(name, "unknown", results);
  }

  const lines = raw.split("\n");
  const blocks = extractBlocks(lines);
  const format = detectFormat(lines, name);

  // Shared structural checks
  results.push(...checkBlockMarkers(name, raw));
  results.push(...checkPragma(name, lines, blocks, format));
  results.push(...checkMetadata(name, blocks));
  results.push(...checkClosing(name, blocks));
  results.push(...checkCrossReferences(name, blocks));
  results.push(...checkContentLabels(name, blocks));

  // Format-specific content checks
  switch (format) {
    case "editorconfig":
      results.push(...checkEditorConfigContent(name, raw, blocks));
      break;
    case "gitignore":
      results.push(...checkGitignoreContent(name, raw, blocks));
      break;
    case "gitmessage":
      results.push(...checkGitmessageContent(name, raw, blocks));
      break;
    case "env":
      results.push(...checkEnvContent(name, raw, blocks));
      break;
    case "unknown":
      results.push(warn(name, "format/unknown", "Could not detect dotfile format — skipping format-specific checks"));
      break;
  }

  return summarize(name, format, results);
}

function summarize(file: string, format: DotfileFormat, results: LintResult[]): LintSummary {
  return {
    file,
    format,
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
    case "error":
      return COLORS.red;
    case "warn":
      return COLORS.yellow;
    case "info":
      return COLORS.blue;
  }
}

function severityIcon(s: Severity): string {
  switch (s) {
    case "error":
      return "x";
    case "warn":
      return "!";
    case "info":
      return "i";
  }
}

function formatLabel(f: DotfileFormat): string {
  switch (f) {
    case "editorconfig":
      return "EC";
    case "gitignore":
      return "GI";
    case "gitmessage":
      return "GM";
    case "env":
      return "EN";
    case "unknown":
      return "??";
  }
}

function printSummary(summary: LintSummary, verbose: boolean): void {
  const { file, format, errors, warnings, infos, results } = summary;
  const total = errors + warnings + infos;
  const fLabel = `${COLORS.dim}[${formatLabel(format)}]${COLORS.reset}`;

  if (total === 0) {
    console.log(`${COLORS.green}  OK${COLORS.reset}  ${fLabel} ${file}`);
    return;
  }

  const statusColor = errors > 0
    ? COLORS.red
    : warnings > 0
      ? COLORS.yellow
      : COLORS.blue;
  const statusIcon = errors > 0 ? "FAIL" : warnings > 0 ? "WARN" : "INFO";
  console.log(
    `${statusColor}${statusIcon}${COLORS.reset}  ${fLabel} ${file}  ${COLORS.dim}(${errors}E ${warnings}W ${infos}I)${COLORS.reset}`,
  );

  if (verbose || errors > 0) {
    for (const r of results) {
      if (!verbose && r.severity === "info") continue;
      const color = severityColor(r.severity);
      const icon = severityIcon(r.severity);
      console.log(
        `  ${color}${icon}${COLORS.reset} ${COLORS.dim}[${r.rule}]${COLORS.reset} ${r.message}`,
      );
    }
  }
}

function printTotal(summaries: LintSummary[]): void {
  const totalErrors = summaries.reduce((s, r) => s + r.errors, 0);
  const totalWarnings = summaries.reduce((s, r) => s + r.warnings, 0);
  const totalInfos = summaries.reduce((s, r) => s + r.infos, 0);
  const totalFiles = summaries.length;
  const passFiles = summaries.filter((s) => s.errors === 0 && s.warnings === 0).length;

  // Format breakdown
  const formatCounts = new Map<DotfileFormat, number>();
  for (const s of summaries) {
    formatCounts.set(s.format, (formatCounts.get(s.format) || 0) + 1);
  }
  const formatStr = [...formatCounts.entries()]
    .map(([f, c]) => `${c} ${f}`)
    .join(", ");

  console.log("");
  console.log(
    `${COLORS.bold}Summary:${COLORS.reset} ${totalFiles} files (${formatStr}), ${passFiles} clean`,
  );
  console.log(
    `  ${COLORS.red}${totalErrors} errors${COLORS.reset}  ` +
      `${COLORS.yellow}${totalWarnings} warnings${COLORS.reset}  ` +
      `${COLORS.blue}${totalInfos} info${COLORS.reset}`,
  );

  if (totalErrors === 0) {
    console.log(`\n${COLORS.green}All files structurally valid.${COLORS.reset}`);
  } else {
    console.log(
      `\n${COLORS.red}${totalErrors} structural errors must be fixed.${COLORS.reset}`,
    );
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
${COLORS.bold}lint-dotfiles${COLORS.reset} v${VERSION} — Dotfile Template Alignment Linter

${COLORS.dim}"Prove all things; hold fast that which is good." — 1 Thess 5:21${COLORS.reset}

Validates dotfile templates against OmniCode 3-block standard.
Supports: .editorconfig, .gitignore, .gitmessage, .env

Usage:
  deno run --allow-read lint-dotfiles.ts <file-or-directory> [options]

Options:
  --verbose, -v     Show all results (including info)
  --errors-only     Show only errors
  --summary         Show only file-level summary
  --help, -h        Show this help

Examples:
  deno run --allow-read lint-dotfiles.ts dotfiles/
  deno run --allow-read lint-dotfiles.ts editorconfig.editorconfig -v
  deno run --allow-read lint-dotfiles.ts . --summary
`);
    Deno.exit(0);
  }

  const verbose = args.includes("--verbose") || args.includes("-v");
  const errorsOnly = args.includes("--errors-only");
  const summaryOnly = args.includes("--summary");

  const targets = args.filter((a) => !a.startsWith("-"));

  // Gather dotfiles
  const files: string[] = [];
  for (const target of targets) {
    const resolved = resolve(target);
    try {
      const stat = await Deno.stat(resolved);
      if (stat.isFile) {
        if (isDotfile(resolved)) {
          files.push(resolved);
        }
      } else if (stat.isDirectory) {
        for await (const entry of walk(resolved, { maxDepth: 3 })) {
          if (entry.isFile && isDotfile(entry.path)) {
            files.push(entry.path);
          }
        }
      }
    } catch {
      console.error(`${COLORS.red}Cannot access: ${target}${COLORS.reset}`);
    }
  }

  if (files.length === 0) {
    console.error(
      `${COLORS.red}No dotfiles found (.editorconfig, .gitignore, .gitmessage, .env).${COLORS.reset}`,
    );
    Deno.exit(1);
  }

  files.sort();

  const cwd = Deno.cwd();
  console.log(`${COLORS.bold}Linting ${files.length} dotfile(s)...${COLORS.reset}\n`);

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
      const fLabel = `${COLORS.dim}[${formatLabel(s.format)}]${COLORS.reset}`;
      console.log(
        `${status}  ${fLabel} ${s.file}  (${s.errors}E ${s.warnings}W ${s.infos}I)`,
      );
    }
  }

  printTotal(summaries);

  const hasErrors = summaries.some((s) => s.errors > 0);
  Deno.exit(hasErrors ? 1 : 0);
}

main();
