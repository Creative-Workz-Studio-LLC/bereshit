// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/formats/go.ts
// key:     B-tov-cws-struct-lib-formats-go
// title:   CWS Struct — Go Format Handler
// type:    Code (Library)
// version: a-02.00
// created: 2026-02-13
// updated: 2026-02-13
// authors: Nova Dawn (CPI-SI)
// purpose: Go 4-block alignment linter + transformer. Validates Go source files for:
//          - //omni: directives (key, version, pragma, metadata)
//          - 4-block structure (METADATA → SETUP → BODY → CLOSING)
//          - END markers for each block
//          - Block order correctness
//          - Pragma and metadata maps (_pragma, _metadata)
//          - Package declaration, import presence
//          - Separator style (consistency + standard widths)
//          - Template vs derived file classification
//          - SETUP subsection order (I → T → TM → K → V → PS)
//          - BODY subsection order (Org → Helpers → Core → Error → APIs)
//          - //omni:code directive format validation
//
//          Line-based parser — reads .go files as text and checks structural
//          markers via regex. Does NOT parse Go AST.
//
//          a-02.00: Added 5 new checks, transformer, file context refactor.
//          a-01.00: Initial 6-check linter.
//
// biblical_foundation: "Let all things be done decently and in order."
//   — 1 Corinthians 14:40
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { FormatHandler, LintResult, TransformOptions } from "../types.ts";
import { error, warn, info } from "../types.ts";
import { registerFormat } from "./registry.ts";

// ---------------------------------------------------------------------------
// Constants — what we expect in a 4-block Go file
// ---------------------------------------------------------------------------

/** Required //omni: directives at top of file. */
const REQUIRED_DIRECTIVES = [
  "//omni:key",
] as const;

/** Recommended //omni: directives. */
const RECOMMENDED_DIRECTIVES = [
  "//omni:code",
  "//omni:version",
] as const;

/** The 4 blocks in required order. */
const BLOCKS = ["METADATA", "SETUP", "BODY", "CLOSING"] as const;

/** Patterns that identify a block boundary. */
const BLOCK_PATTERNS: Record<string, RegExp> = {
  METADATA: /^\/\/\s*={4,}\s*$|^\/\/\s+METADATA\s*$/,
  SETUP:    /^\/\/\s+SETUP\s*$/,
  BODY:     /^\/\/\s+BODY\s*$/,
  CLOSING:  /^\/\/\s+CLOSING\s*$/,
};

/** END marker patterns. */
const END_PATTERNS: Record<string, RegExp> = {
  METADATA: /^\/\/\s+END METADATA\s*$/,
  SETUP:    /^\/\/\s+END SETUP\s*$/,
  BODY:     /^\/\/\s+END BODY\s*$/,
  CLOSING:  /^\/\/\s+END CLOSING\s*$/,
};

/** SETUP subsection markers in required order. */
const SETUP_SUBSECTIONS = [
  { tag: "I",  label: "Imports",             pattern: /^\/\/---\s+I\.\d/ },
  { tag: "T",  label: "Types",              pattern: /^\/\/---\s+T\.\d/ },
  { tag: "TM", label: "Type Methods",       pattern: /^\/\/---\s+TM\s/ },
  { tag: "K",  label: "Constants",           pattern: /^\/\/---\s+K\.\d/ },
  { tag: "V",  label: "Variables",           pattern: /^\/\/---\s+V\.\d/ },
  { tag: "PS", label: "Package-Level State", pattern: /^\/\/---\s+PS\.\d/ },
] as const;

/** BODY subsection labels in expected order (flexible — checks what's present). */
const BODY_SUBSECTIONS = [
  { label: "Org Chart",        pattern: /Org\s*Chart|APU\s*Inventory/i },
  { label: "Helpers",          pattern: /^\/\/\s+Helpers|^\/\/\s+────.*Helpers/i },
  { label: "Core Operations",  pattern: /Core\s*Operations/i },
  { label: "Error Handling",   pattern: /Error\s*(?:Handling|Helpers)/i },
  { label: "Public APIs",      pattern: /Public\s*APIs/i },
] as const;

/** Known //omni:code directive patterns. */
const KNOWN_CODE_DIRECTIVES = [
  "--go -library",
  "--go -executable",
  "--go -demo-test",
] as const;

/** Standard separator widths. */
const BLOCK_SEPARATOR_WIDTH = 76;     // // ====...==== (76 = chars)
const SUBSECTION_SEPARATOR_WIDTH = 64; // // ----...---- (64 - chars)

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

interface BlockPosition {
  name: string;
  line: number;       // 1-based line number where the block header appears
  endLine: number;    // 1-based line of END marker, 0 if absent
}

/** File-level context gathered once, passed to all check functions. */
interface GoFileContext {
  filePath: string;
  lines: string[];
  isTemplate: boolean;         // has #!omni template
  isDocGo: boolean;            // filename is doc.go
  hasAnyOmni: boolean;         // any //omni: directives present
  hasAnyBlock: boolean;        // any block markers present
  blocks: BlockPosition[];
  directives: Map<string, string>;
  pkgHasMaps: boolean;         // sibling doc.go has _pragma/_metadata maps
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/**
 * Scan lines for block boundaries.
 * Returns positions of each block found, in order of appearance.
 */
function findBlocks(lines: string[]): BlockPosition[] {
  const positions: BlockPosition[] = [];

  for (const blockName of BLOCKS) {
    let headerLine = 0;
    let endLine = 0;

    for (let i = 0; i < lines.length; i++) {
      const trimmed = lines[i].trim();

      // Look for block header: "// METADATA", "// SETUP", etc.
      if (blockName === "METADATA") {
        if (/^\/\/\s+METADATA\s*$/.test(trimmed) && headerLine === 0) {
          headerLine = i + 1;
        }
      } else {
        if (BLOCK_PATTERNS[blockName].test(trimmed) && headerLine === 0) {
          headerLine = i + 1;
        }
      }

      // Look for END marker
      if (END_PATTERNS[blockName].test(trimmed) && endLine === 0) {
        endLine = i + 1;
      }
    }

    if (headerLine > 0) {
      positions.push({ name: blockName, line: headerLine, endLine });
    }
  }

  return positions;
}

/**
 * Find all //omni: directives at the top of the file.
 * Also checks for #!omni directives in comments (templates use // #!omni).
 * Returns map of directive name → value.
 */
function findOmniDirectives(lines: string[]): Map<string, string> {
  const directives = new Map<string, string>();

  for (const line of lines) {
    const trimmed = line.trim();

    // //omni: directives are at the top, before or near `package`
    if (trimmed.startsWith("package ")) break;

    // Standard //omni: directive
    const omniMatch = trimmed.match(/^\/\/omni:(\S+)\s*(.*)?$/);
    if (omniMatch) {
      directives.set(`//omni:${omniMatch[1]}`, omniMatch[2]?.trim() ?? "");
    }

    // Template-style // #!omni directive (used in template files)
    const shebangMatch = trimmed.match(/^\/\/\s+#!omni\s+(.+)$/);
    if (shebangMatch) {
      const parts = shebangMatch[1].trim();
      if (parts.startsWith("template")) {
        directives.set("#!omni:template", parts.replace(/^template\s*/, "").trim());
      } else if (parts.startsWith("code")) {
        directives.set("#!omni:code", parts.replace(/^code\s*/, "").trim());
      } else if (parts.startsWith("meta.")) {
        const metaMatch = parts.match(/^meta\.(\S+)\s*=\s*(.+)$/);
        if (metaMatch) {
          directives.set(`#!omni:meta.${metaMatch[1]}`, metaMatch[2].trim());
        }
      }
    }
  }

  return directives;
}

/**
 * Extract lines belonging to a specific block (between header and END/next block).
 */
function getBlockLines(
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
 * Check if any sibling .go file in the same package has _pragma/_metadata maps.
 *
 * L0 uses doc.go for package identity. L1 uses pragma.go (the primary file).
 * Either pattern is valid — the package has identity if ANY file declares it.
 * We skip checking the current file (it wouldn't need the warning if it had maps).
 */
async function packageHasIdentityMaps(filePath: string): Promise<boolean> {
  const filename = filePath.split("/").pop() ?? "";
  const dir = filePath.substring(0, filePath.lastIndexOf("/"));
  if (!dir) return false;

  try {
    for await (const entry of Deno.readDir(dir)) {
      if (!entry.isFile || !entry.name.endsWith(".go")) continue;
      if (entry.name === filename) continue; // skip self
      if (entry.name.endsWith("_test.go")) continue; // skip test files

      const siblingText = await Deno.readTextFile(`${dir}/${entry.name}`);
      const hasPragma = /^var\s+_pragma\s*=/m.test(siblingText);
      const hasMetadata = /^var\s+_metadata\s*=/m.test(siblingText);
      if (hasPragma && hasMetadata) return true;
    }
  } catch {
    return false;
  }

  return false;
}

/**
 * Build file-level context — gathered once, passed to all checks.
 */
async function buildContext(filePath: string): Promise<GoFileContext | null> {
  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch {
    return null;
  }

  const lines = text.split("\n");
  const filename = filePath.split("/").pop() ?? "";
  const directives = findOmniDirectives(lines);

  const isTemplate = directives.has("#!omni:template") ||
    lines.some((l) => /^\/\/\s+#!omni\s+template\b/.test(l.trim())) ||
    // Old-format templates: //go:build ignore + // TEMPLATE: header
    (lines.some((l) => l.trim() === "//go:build ignore") &&
     lines.some((l) => /^\/\/\s+TEMPLATE:\s/.test(l.trim())));

  const hasAnyOmni = lines.some((l) => l.trim().startsWith("//omni:")) ||
    lines.some((l) => /^\/\/\s+#!omni\s/.test(l.trim()));

  const hasAnyBlock = lines.some((l) =>
    /^\/\/\s+(METADATA|SETUP|BODY|CLOSING)\s*$/.test(l.trim())
  );

  return {
    filePath,
    lines,
    isTemplate,
    isDocGo: filename === "doc.go",
    hasAnyOmni,
    hasAnyBlock,
    blocks: findBlocks(lines),
    directives,
    pkgHasMaps: await packageHasIdentityMaps(filePath),
  };
}

// ---------------------------------------------------------------------------
// Checks (original 6)
// ---------------------------------------------------------------------------

function checkDirectives(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Templates use #!omni meta.key or old-format // Key: header
  if (ctx.isTemplate) {
    if (!ctx.directives.has("#!omni:meta.key")) {
      const hasKeyComment = ctx.lines.some((l) => /^\/\/\s+Key:\s/.test(l.trim()));
      if (!hasKeyComment) {
        results.push(warn(file, "directive/meta.key",
          "Template missing key identifier (#!omni meta.key or // Key:)"));
      }
    }
    return results;
  }

  // Derived files: check standard //omni: directives
  for (const directive of REQUIRED_DIRECTIVES) {
    if (!ctx.directives.has(directive)) {
      results.push(error(file, `directive/${directive}`, `Missing ${directive} — REQUIRED`));
    }
  }

  for (const directive of RECOMMENDED_DIRECTIVES) {
    if (!ctx.directives.has(directive)) {
      results.push(warn(file, `directive/${directive}`, `Missing ${directive} — recommended`));
    }
  }

  return results;
}

function checkBlockStructure(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;
  const foundNames = ctx.blocks.map((b) => b.name);

  // Check all 4 blocks present (warn for templates — old-format may omit markers)
  for (const blockName of BLOCKS) {
    if (!foundNames.includes(blockName)) {
      const level = ctx.isTemplate ? warn : error;
      results.push(level(file, `block/${blockName}`,
        ctx.isTemplate
          ? `Missing ${blockName} block marker (template may use alternative format)`
          : `Missing ${blockName} block`));
    }
  }

  // Check block order
  const blockOrder = BLOCKS.filter((b) => foundNames.includes(b));
  const actualOrder = ctx.blocks.map((b) => b.name);

  for (let i = 0; i < blockOrder.length; i++) {
    if (i < actualOrder.length && blockOrder[i] !== actualOrder[i]) {
      results.push(
        error(
          file,
          "block/order",
          `Block order wrong: found ${actualOrder.join(" → ")}, expected ${BLOCKS.join(" → ")}`,
        ),
      );
      break;
    }
  }

  // Check END markers
  for (const block of ctx.blocks) {
    if (block.endLine === 0) {
      results.push(
        warn(file, `block/end-${block.name}`, `Missing END ${block.name} marker`),
      );
    }
  }

  return results;
}

function checkPackageAndImports(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;
  let hasPackage = false;
  let hasImport = false;

  for (const line of ctx.lines) {
    const trimmed = line.trim();
    if (trimmed.startsWith("package ")) hasPackage = true;
    if (trimmed.startsWith("import ") || trimmed === "import (") hasImport = true;
  }

  // Templates have commented-out package declarations
  if (!hasPackage && !ctx.isTemplate) {
    results.push(error(file, "go/package", "Missing package declaration"));
  }

  if (!hasImport && !ctx.isTemplate) {
    results.push(info(file, "go/import", "No import statement found"));
  }

  return results;
}

function checkPragmaMetadataMaps(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Templates don't need runtime maps
  if (ctx.isTemplate) return results;

  let hasPragmaMap = false;
  let hasMetadataMap = false;

  for (const line of ctx.lines) {
    const trimmed = line.trim();
    if (/^var\s+_pragma\s*=/.test(trimmed)) hasPragmaMap = true;
    if (/^var\s+_metadata\s*=/.test(trimmed)) hasMetadataMap = true;
  }

  const level = ctx.pkgHasMaps ? info : warn;

  if (!hasPragmaMap) {
    const msg = ctx.pkgHasMaps
      ? "No var _pragma map (package identity in doc.go)"
      : "Missing var _pragma map — recommended for runtime identity";
    results.push(level(file, "identity/pragma-map", msg));
  }

  if (!hasMetadataMap) {
    const msg = ctx.pkgHasMaps
      ? "No var _metadata map (package identity in doc.go)"
      : "Missing var _metadata map — recommended for runtime identity";
    results.push(level(file, "identity/metadata-map", msg));
  }

  return results;
}

function checkCommentMetadata(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  const hasPragmaMap = ctx.lines.some((l) => /^var\s+_pragma\s*=/.test(l.trim()));
  const hasMetadataMap = ctx.lines.some((l) => /^var\s+_metadata\s*=/.test(l.trim()));
  const hasMaps = hasPragmaMap && hasMetadataMap;

  let inMetadata = false;
  let hasKey = false;
  let hasPurpose = false;
  let hasBiblical = false;
  let hasVersion = false;

  for (const line of ctx.lines) {
    const trimmed = line.trim();

    if (/^\/\/\s+METADATA\s*$/.test(trimmed)) {
      inMetadata = true;
      continue;
    }
    if (inMetadata && /^\/\/\s+(SETUP|END METADATA)\s*$/.test(trimmed)) {
      break;
    }

    if (inMetadata) {
      if (/^\/\/\s+Key:/.test(trimmed)) hasKey = true;
      if (/^\/\/\s+Purpose:/.test(trimmed)) hasPurpose = true;
      if (/^\/\/\s+(Biblical|Scripture):/.test(trimmed)) hasBiblical = true;
      if (/^\/\/\s+Version:/.test(trimmed)) hasVersion = true;
    }
  }

  if (inMetadata && !ctx.isTemplate) {
    const level = hasMaps ? info : warn;

    if (!hasKey) {
      results.push(level(file, "comment-meta/key", "Missing Key: in METADATA comment block"));
    }
    if (!hasPurpose) {
      results.push(level(file, "comment-meta/purpose", "Missing Purpose: in METADATA comment block"));
    }
    if (!hasBiblical) {
      results.push(info(file, "comment-meta/biblical", "Missing Biblical:/Scripture: in METADATA comment block — recommended"));
    }
    if (!hasVersion) {
      results.push(info(file, "comment-meta/version", "Missing Version: in METADATA comment block"));
    }
  }

  return results;
}

function checkSeparatorConsistency(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  const eqSeparators: Array<{ line: number; width: number }> = [];
  const dashSeparators: Array<{ line: number; width: number }> = [];

  for (let i = 0; i < ctx.lines.length; i++) {
    const trimmed = ctx.lines[i].trim();

    // Block separators (=)
    const eqMatch = trimmed.match(/^\/\/\s+(={4,})\s*$/);
    if (eqMatch) {
      eqSeparators.push({ line: i + 1, width: eqMatch[1].length });
    }

    // Subsection separators (-)
    const dashMatch = trimmed.match(/^\/\/\s+(-{4,})\s*$/);
    if (dashMatch) {
      dashSeparators.push({ line: i + 1, width: dashMatch[1].length });
    }
  }

  // Check block separator consistency
  if (eqSeparators.length >= 2) {
    const widths = new Set(eqSeparators.map((s) => s.width));
    if (widths.size > 1) {
      const widthList = [...widths].sort().join(", ");
      results.push(
        warn(file, "style/eq-separator-width",
          `Inconsistent block separator widths: ${widthList} chars — pick one`),
      );
    }
    // Standard-aware: check against expected width
    const dominant = eqSeparators[0].width;
    if (dominant !== BLOCK_SEPARATOR_WIDTH) {
      results.push(
        info(file, "style/eq-separator-standard",
          `Block separators are ${dominant} chars wide (standard: ${BLOCK_SEPARATOR_WIDTH})`),
      );
    }
  }

  // Check subsection separator consistency
  if (dashSeparators.length >= 2) {
    const widths = new Set(dashSeparators.map((s) => s.width));
    if (widths.size > 1) {
      const widthList = [...widths].sort().join(", ");
      results.push(
        warn(file, "style/dash-separator-width",
          `Inconsistent subsection separator widths: ${widthList} chars — pick one`),
      );
    }
    const dominant = dashSeparators[0].width;
    if (dominant !== SUBSECTION_SEPARATOR_WIDTH) {
      results.push(
        info(file, "style/dash-separator-standard",
          `Subsection separators are ${dominant} chars wide (standard: ${SUBSECTION_SEPARATOR_WIDTH})`),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Checks (new — a-02.00)
// ---------------------------------------------------------------------------

/** Check 7: Template vs derived file classification. */
function checkTemplateVsDerived(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  const hasBuildIgnore = ctx.lines.some((l) =>
    l.trim() === "//go:build ignore"
  );

  if (ctx.isTemplate) {
    if (!hasBuildIgnore) {
      results.push(
        error(file, "template/build-ignore",
          "Template file missing //go:build ignore — will fail compilation"),
      );
    }
  } else {
    if (hasBuildIgnore) {
      results.push(
        warn(file, "derived/build-ignore",
          "Derived file has //go:build ignore — won't compile"),
      );
    }
  }

  return results;
}

/** Check 8: SETUP subsection order (I → T → TM → K → V → PS). */
function checkSetupSubsectionOrder(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Templates mention subsection names in overview comments — skip order check
  if (ctx.isTemplate) return results;

  const setupLines = getBlockLines(ctx.lines, ctx.blocks, "SETUP");
  if (setupLines.length === 0) return results;

  // Find which subsections are present and their positions
  const found: Array<{ tag: string; label: string; lineIdx: number }> = [];

  for (let i = 0; i < setupLines.length; i++) {
    const trimmed = setupLines[i].trim();
    for (const sub of SETUP_SUBSECTIONS) {
      if (sub.pattern.test(trimmed)) {
        found.push({ tag: sub.tag, label: sub.label, lineIdx: i });
        break;
      }
    }
  }

  if (found.length < 2) return results; // Nothing to check ordering on

  // Check ordering against the canonical sequence
  const canonicalOrder: string[] = SETUP_SUBSECTIONS.map((s) => s.tag);
  const foundTags = found.map((f) => f.tag);

  let lastCanonIdx = -1;
  for (const f of found) {
    const canonIdx = canonicalOrder.indexOf(f.tag);
    if (canonIdx < lastCanonIdx) {
      results.push(
        warn(file, "setup/subsection-order",
          `SETUP subsection ${f.tag} (${f.label}) appears after a later subsection — expected: ${foundTags.join(" → ")}, canonical: I → T → TM → K → V → PS`),
      );
      break;
    }
    lastCanonIdx = canonIdx;
  }

  return results;
}

/** Check 9: BODY subsection order. */
function checkBodySubsectionOrder(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Templates mention subsection names in overview comments — skip order check
  if (ctx.isTemplate) return results;

  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  if (bodyLines.length === 0) return results;

  // Find which subsections are present
  const found: Array<{ label: string; lineIdx: number }> = [];

  for (let i = 0; i < bodyLines.length; i++) {
    const trimmed = bodyLines[i].trim();
    for (const sub of BODY_SUBSECTIONS) {
      if (sub.pattern.test(trimmed)) {
        // Only take first occurrence of each
        if (!found.some((f) => f.label === sub.label)) {
          found.push({ label: sub.label, lineIdx: i });
        }
        break;
      }
    }
  }

  if (found.length < 2) return results;

  // Check ordering
  const canonicalOrder: string[] = BODY_SUBSECTIONS.map((s) => s.label);

  let lastCanonIdx = -1;
  for (const f of found) {
    const canonIdx = canonicalOrder.indexOf(f.label);
    if (canonIdx < lastCanonIdx) {
      const foundLabels = found.map((x) => x.label).join(" → ");
      results.push(
        warn(file, "body/subsection-order",
          `BODY subsection "${f.label}" appears after a later subsection — found: ${foundLabels}`),
      );
      break;
    }
    lastCanonIdx = canonIdx;
  }

  return results;
}

/** Check 10: //omni:code directive format. */
function checkDirectiveFormat(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Check //omni:code value for derived files
  const codeValue = ctx.directives.get("//omni:code");
  if (codeValue !== undefined && codeValue !== "") {
    const isKnown = KNOWN_CODE_DIRECTIVES.some((k) => codeValue === k);
    if (!isKnown) {
      results.push(
        info(file, "directive/code-format",
          `//omni:code value "${codeValue}" — not a recognized pattern (known: ${KNOWN_CODE_DIRECTIVES.join(", ")})`),
      );
    }
  }

  // Check #!omni template value for template files
  const templateValue = ctx.directives.get("#!omni:template");
  if (templateValue !== undefined && templateValue !== "") {
    const isKnown = KNOWN_CODE_DIRECTIVES.some((k) => templateValue === k);
    if (!isKnown) {
      results.push(
        info(file, "directive/template-format",
          `#!omni template value "${templateValue}" — not a recognized pattern`),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Lint orchestrator
// ---------------------------------------------------------------------------

async function lintGoFile(filePath: string): Promise<LintResult[]> {
  const ctx = await buildContext(filePath);
  if (!ctx) {
    return [error(filePath, "io/read", "Cannot read file")];
  }

  // Quick check: is this a Go file with any structural markers?
  if (!ctx.hasAnyOmni && !ctx.hasAnyBlock) {
    return [
      info(filePath, "structure/skip",
        "No //omni: directives or block markers — not a 4-block file"),
    ];
  }

  return [
    ...checkDirectives(ctx),
    ...checkBlockStructure(ctx),
    ...checkPackageAndImports(ctx),
    ...checkPragmaMetadataMaps(ctx),
    ...checkCommentMetadata(ctx),
    ...checkSeparatorConsistency(ctx),
    ...checkTemplateVsDerived(ctx),
    ...checkSetupSubsectionOrder(ctx),
    ...checkBodySubsectionOrder(ctx),
    ...checkDirectiveFormat(ctx),
  ];
}

// ---------------------------------------------------------------------------
// Transformer
// ---------------------------------------------------------------------------

/**
 * Transform a Go file to fix structural issues.
 *
 * Capabilities:
 *   1. Fix separator widths (= → 76, - → 64)
 *   2. Add missing END markers
 *   3. Add //go:build ignore to template files
 *
 * Does NOT inject missing block boundaries (too risky for arbitrary positions).
 * Lint first, fix structure manually, then transform for cleanup.
 */
async function transformGoFile(
  filePath: string,
  opts: TransformOptions,
): Promise<LintResult[]> {
  const { dryRun } = opts;
  const results: LintResult[] = [];

  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  const lines = text.split("\n");
  let modified = false;

  // --- Transform 1: Fix block separator widths (= chars) ---
  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i].trim();
    const eqMatch = trimmed.match(/^(\/\/\s+)(={4,})(\s*)$/);
    if (eqMatch && eqMatch[2].length !== BLOCK_SEPARATOR_WIDTH) {
      const newLine = `${eqMatch[1]}${"=".repeat(BLOCK_SEPARATOR_WIDTH)}`;
      if (dryRun) {
        results.push(info(filePath, "transform/eq-width",
          `Line ${i + 1}: would fix block separator ${eqMatch[2].length} → ${BLOCK_SEPARATOR_WIDTH} chars`));
      } else {
        lines[i] = newLine;
        modified = true;
        results.push(info(filePath, "transform/eq-width",
          `Line ${i + 1}: fixed block separator ${eqMatch[2].length} → ${BLOCK_SEPARATOR_WIDTH} chars`));
      }
    }
  }

  // --- Transform 2: Fix subsection separator widths (- chars) ---
  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i].trim();
    const dashMatch = trimmed.match(/^(\/\/\s+)(-{4,})(\s*)$/);
    if (dashMatch && dashMatch[2].length !== SUBSECTION_SEPARATOR_WIDTH) {
      const newLine = `${dashMatch[1]}${"-".repeat(SUBSECTION_SEPARATOR_WIDTH)}`;
      if (dryRun) {
        results.push(info(filePath, "transform/dash-width",
          `Line ${i + 1}: would fix subsection separator ${dashMatch[2].length} → ${SUBSECTION_SEPARATOR_WIDTH} chars`));
      } else {
        lines[i] = newLine;
        modified = true;
        results.push(info(filePath, "transform/dash-width",
          `Line ${i + 1}: fixed subsection separator ${dashMatch[2].length} → ${SUBSECTION_SEPARATOR_WIDTH} chars`));
      }
    }
  }

  // --- Transform 3: Add //go:build ignore to template files ---
  const isTemplate = lines.some((l) => /^\/\/\s+#!omni\s+template\b/.test(l.trim()));
  const hasBuildIgnore = lines.some((l) => l.trim() === "//go:build ignore");

  if (isTemplate && !hasBuildIgnore) {
    if (dryRun) {
      results.push(info(filePath, "transform/build-ignore",
        "Would add //go:build ignore at line 1"));
    } else {
      lines.unshift("//go:build ignore", "");
      modified = true;
      results.push(info(filePath, "transform/build-ignore",
        "Added //go:build ignore at line 1"));
    }
  }

  // --- Write if modified ---
  if (modified && !dryRun) {
    await Deno.writeTextFile(filePath, lines.join("\n"));
    results.push(info(filePath, "transform/written", "File updated"));
  } else if (!modified) {
    results.push(info(filePath, "transform/clean", "No changes needed"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Handler registration
// ---------------------------------------------------------------------------

const goHandler: FormatHandler = {
  name: "go",
  description: "Go 4-block alignment (//omni: directives, METADATA → SETUP → BODY → CLOSING, subsection order)",
  extensions: [".go"],
  maxDepth: 10,
  lint: lintGoFile,
  transform: transformGoFile,
};

registerFormat(goHandler);

// ============================================================================
// CLOSING
// ============================================================================
//
// Go files express structure through comments — the //omni: pragma directives
// and block boundary markers. This handler reads those markers and verifies
// the skeleton is sound before anyone fills in the flesh.
//
// a-02.00: 11 checks (6 original + 5 new) + transformer.
//   - Template vs derived classification
//   - SETUP subsection order (I → T → TM → K → V → PS)
//   - BODY subsection order (Org → Helpers → Core → Error → APIs)
//   - Directive format validation
//   - Standard-aware separator widths (76= / 64-)
//   - Transformer: fix separators, add build tags
//
// "Let all things be done decently and in order." — 1 Corinthians 14:40
// ============================================================================
