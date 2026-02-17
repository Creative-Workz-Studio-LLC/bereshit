// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/formats/rust.ts
// key:     B-tov-cws-struct-lib-formats-rust
// title:   CWS Struct — Rust Format Handler
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-14
// authors: Nova Dawn (CPI-SI)
// purpose: Rust 4-block alignment linter + transformer. Validates Rust source
//          files for:
//          - //omni: directives (key, version, pragma, metadata)
//          - 4-block structure (METADATA → SETUP → BODY → CLOSING)
//          - END markers for each block
//          - Block order correctness
//          - Pragma and metadata statics (slice or BTreeMap)
//          - Doc comments (//! for crate/module level)
//          - Separator style (block = 76, subsection ─ 64)
//          - Template vs derived file classification
//          - SETUP subsection order (Imports → Error Types → Core Types)
//          - Identity registration function
//          - //omni:code directive format validation
//
//          Line-based parser — reads .rs files as text and checks structural
//          markers via regex. Does NOT parse Rust AST.
//
//          Adapted from go.ts — same architecture, Rust conventions.
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
// Constants — what we expect in a 4-block Rust file
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

/** SETUP subsection markers in expected order. */
const SETUP_SUBSECTIONS = [
  { tag: "Imports",     pattern: /Imports/i },
  { tag: "ErrorTypes",  pattern: /Error\s*Types/i },
  { tag: "CoreTypes",   pattern: /Core\s*Types/i },
] as const;

/** Known //omni:code directive patterns for Rust. */
const KNOWN_CODE_DIRECTIVES = [
  "--rust -library",
  "--rust -executable",
  "--rust -demo-test",
] as const;

/** Standard separator widths. */
const BLOCK_SEPARATOR_WIDTH = 76;         // // ====...==== (76 = chars)
const SUBSECTION_SEPARATOR_WIDTH = 64;    // // ────...──── (64 ─ chars)

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
interface RustFileContext {
  filePath: string;
  lines: string[];
  isTemplate: boolean;           // has #!omni template
  isCrateRoot: boolean;          // filename is lib.rs or main.rs
  isModuleFile: boolean;         // not root, not template
  hasAnyOmni: boolean;           // any //omni: directives present
  hasAnyBlock: boolean;          // any block markers present
  blocks: BlockPosition[];
  directives: Map<string, string>;
  crateHasIdentity: boolean;     // sibling lib.rs has PRAGMA/METADATA
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
 * Find all //omni: directives in the file.
 * Rust //omni: directives can appear before or after doc comments (//!),
 * but always before the first block marker or `use` statement.
 */
function findOmniDirectives(lines: string[]): Map<string, string> {
  const directives = new Map<string, string>();

  for (const line of lines) {
    const trimmed = line.trim();

    // Stop scanning at first block marker or code
    if (/^\/\/\s+(METADATA|SETUP|BODY|CLOSING)\s*$/.test(trimmed)) break;
    if (/^(pub\s+)?(fn|struct|enum|trait|impl|mod|type|const|static)\s/.test(trimmed)) break;
    if (/^use\s/.test(trimmed)) break;

    // Standard //omni: directive
    const omniMatch = trimmed.match(/^\/\/omni:(\S+)\s*(.*)?$/);
    if (omniMatch) {
      directives.set(`//omni:${omniMatch[1]}`, omniMatch[2]?.trim() ?? "");
    }

    // Template-style // #!omni directive
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
 * Check if a sibling lib.rs in the same directory has pragma/metadata statics.
 */
async function crateHasIdentityStatics(filePath: string): Promise<boolean> {
  const filename = filePath.split("/").pop() ?? "";
  if (filename === "lib.rs") return false;

  const dir = filePath.substring(0, filePath.lastIndexOf("/"));
  const libPath = dir ? `${dir}/lib.rs` : "lib.rs";

  try {
    const libText = await Deno.readTextFile(libPath);
    const hasPragma = /^pub\s+static\s+PRAGMA\s*:/m.test(libText) ||
                      /^static\s+PRAGMA\s*:/m.test(libText);
    const hasMetadata = /^pub\s+static\s+METADATA\s*:/m.test(libText) ||
                        /^static\s+METADATA\s*:/m.test(libText);
    return hasPragma && hasMetadata;
  } catch {
    return false;
  }
}

/**
 * Build file-level context — gathered once, passed to all checks.
 */
async function buildContext(filePath: string): Promise<RustFileContext | null> {
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
    lines.some((l) => /^\/\/\s+#!omni\s+template\b/.test(l.trim()));

  const isCrateRoot = filename === "lib.rs" || filename === "main.rs";

  const hasAnyOmni = lines.some((l) => l.trim().startsWith("//omni:")) ||
    lines.some((l) => /^\/\/\s+#!omni\s/.test(l.trim()));

  const hasAnyBlock = lines.some((l) =>
    /^\/\/\s+(METADATA|SETUP|BODY|CLOSING)\s*$/.test(l.trim())
  );

  return {
    filePath,
    lines,
    isTemplate,
    isCrateRoot,
    isModuleFile: !isCrateRoot && !isTemplate,
    hasAnyOmni,
    hasAnyBlock,
    blocks: findBlocks(lines),
    directives,
    crateHasIdentity: await crateHasIdentityStatics(filePath),
  };
}

// ---------------------------------------------------------------------------
// Check 1: //omni: directives
// ---------------------------------------------------------------------------

function checkDirectives(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Templates use #!omni meta.key
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

  // Module files: directives are optional (identity lives in lib.rs)
  if (ctx.isModuleFile && !ctx.hasAnyOmni) {
    return results;
  }

  // Crate root or files with //omni: directives: check them
  for (const directive of REQUIRED_DIRECTIVES) {
    if (!ctx.directives.has(directive)) {
      const level = ctx.isCrateRoot ? error : warn;
      results.push(level(file, `directive/${directive}`,
        ctx.isCrateRoot
          ? `Missing ${directive} — REQUIRED for crate root`
          : `Missing ${directive}`));
    }
  }

  for (const directive of RECOMMENDED_DIRECTIVES) {
    if (!ctx.directives.has(directive)) {
      results.push(warn(file, `directive/${directive}`, `Missing ${directive} — recommended`));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 2: Block structure
// ---------------------------------------------------------------------------

function checkBlockStructure(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;
  const foundNames = ctx.blocks.map((b) => b.name);

  // Module files without blocks are fine — blocks live in lib.rs
  if (ctx.isModuleFile && !ctx.hasAnyBlock) {
    return results;
  }

  // Check all 4 blocks present
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
        error(file, "block/order",
          `Block order wrong: found ${actualOrder.join(" → ")}, expected ${BLOCKS.join(" → ")}`),
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

// ---------------------------------------------------------------------------
// Check 3: Doc comments
// ---------------------------------------------------------------------------

function checkDocComments(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Templates: check for //! placeholder
  if (ctx.isTemplate) {
    const hasDocTemplate = ctx.lines.some((l) => l.trim().startsWith("//!"));
    if (!hasDocTemplate) {
      results.push(info(file, "doc/template", "Template has no //! doc comment section"));
    }
    return results;
  }

  const hasDocComments = ctx.lines.some((l) => l.trim().startsWith("//!"));

  if (ctx.isCrateRoot && !hasDocComments) {
    results.push(warn(file, "doc/crate-docs",
      "Missing //! crate-level doc comments — recommended for lib.rs/main.rs"));
  } else if (ctx.isModuleFile && !hasDocComments) {
    results.push(info(file, "doc/module-docs",
      "No //! module-level doc comments"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 4: Pragma/Metadata statics
// ---------------------------------------------------------------------------

function checkPragmaMetadata(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Only check crate roots and files with existing identity
  if (ctx.isTemplate || ctx.isModuleFile) return results;

  let hasPragma = false;
  let hasMetadata = false;
  let hasDelegatedIdentity = false;

  for (const line of ctx.lines) {
    const trimmed = line.trim();
    // Slice-based: pub static PRAGMA: &[(&str, &str)]
    if (/^pub\s+static\s+PRAGMA\s*:/.test(trimmed)) hasPragma = true;
    // LazyLock-based: static PRAGMA: LazyLock<BTreeMap<...>>
    if (/^static\s+PRAGMA\s*:/.test(trimmed)) hasPragma = true;
    // OWN_PRAGMA (identity crate pattern)
    if (/^static\s+OWN_PRAGMA\s*:/.test(trimmed)) hasPragma = true;

    // Same for METADATA
    if (/^pub\s+static\s+METADATA\s*:/.test(trimmed)) hasMetadata = true;
    if (/^static\s+METADATA\s*:/.test(trimmed)) hasMetadata = true;
    if (/^static\s+OWN_METADATA\s*:/.test(trimmed)) hasMetadata = true;

    // Delegated identity: pub mod identity (config crate pattern)
    if (/^pub\s+mod\s+identity\s*;/.test(trimmed)) hasDelegatedIdentity = true;
  }

  // If identity is delegated to a module, it's in the module, not lib.rs
  if (hasDelegatedIdentity) {
    if (!hasPragma) {
      results.push(info(file, "identity/pragma-static",
        "PRAGMA in identity module (pub mod identity) — OK"));
    }
    return results;
  }

  if (!hasPragma) {
    results.push(warn(file, "identity/pragma-static",
      "Missing PRAGMA static — recommended for crate identity"));
  }

  if (!hasMetadata) {
    results.push(warn(file, "identity/metadata-static",
      "Missing METADATA static — recommended for crate identity"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 5: Comment metadata in METADATA block
// ---------------------------------------------------------------------------

function checkCommentMetadata(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Check for pragma/metadata statics (determines severity)
  const hasStaticIdentity = ctx.lines.some((l) => {
    const t = l.trim();
    return /^(pub\s+)?static\s+(OWN_)?PRAGMA\s*:/.test(t);
  });

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
    const level = hasStaticIdentity ? info : warn;

    if (!hasKey) {
      results.push(level(file, "comment-meta/key", "Missing Key: in METADATA comment block"));
    }
    if (!hasPurpose) {
      results.push(level(file, "comment-meta/purpose", "Missing Purpose: in METADATA comment block"));
    }
    if (!hasBiblical) {
      results.push(info(file, "comment-meta/biblical",
        "Missing Biblical:/Scripture: in METADATA comment block — recommended"));
    }
    if (!hasVersion) {
      results.push(info(file, "comment-meta/version", "Missing Version: in METADATA comment block"));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 6: Separator consistency
// ---------------------------------------------------------------------------

function checkSeparatorConsistency(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  const eqSeparators: Array<{ line: number; width: number }> = [];
  const boxSeparators: Array<{ line: number; width: number }> = [];
  const dashSeparators: Array<{ line: number; width: number }> = [];

  for (let i = 0; i < ctx.lines.length; i++) {
    const trimmed = ctx.lines[i].trim();

    // Block separators (=)
    const eqMatch = trimmed.match(/^\/\/\s+(={4,})\s*$/);
    if (eqMatch) {
      eqSeparators.push({ line: i + 1, width: eqMatch[1].length });
    }

    // Subsection separators — Unicode box-drawing (─) full-width only
    const boxMatch = trimmed.match(/^\/\/\s+(─{4,})\s*$/);
    if (boxMatch) {
      boxSeparators.push({ line: i + 1, width: boxMatch[1].length });
    }

    // Subsection separators — ASCII dash (-) full-width only
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
    const dominant = eqSeparators[0].width;
    if (dominant !== BLOCK_SEPARATOR_WIDTH) {
      results.push(
        info(file, "style/eq-separator-standard",
          `Block separators are ${dominant} chars wide (standard: ${BLOCK_SEPARATOR_WIDTH})`),
      );
    }
  }

  // Check Unicode box-drawing separator consistency
  if (boxSeparators.length >= 2) {
    const widths = new Set(boxSeparators.map((s) => s.width));
    if (widths.size > 1) {
      const widthList = [...widths].sort().join(", ");
      results.push(
        warn(file, "style/box-separator-width",
          `Inconsistent subsection separator widths: ${widthList} ─ chars — pick one`),
      );
    }
    const dominant = boxSeparators[0].width;
    if (dominant !== SUBSECTION_SEPARATOR_WIDTH) {
      results.push(
        info(file, "style/box-separator-standard",
          `Subsection separators are ${dominant} ─ chars wide (standard: ${SUBSECTION_SEPARATOR_WIDTH})`),
      );
    }
  }

  // Check ASCII dash separator consistency (fallback / template style)
  if (dashSeparators.length >= 2) {
    const widths = new Set(dashSeparators.map((s) => s.width));
    if (widths.size > 1) {
      const widthList = [...widths].sort().join(", ");
      results.push(
        warn(file, "style/dash-separator-width",
          `Inconsistent dash separator widths: ${widthList} chars — pick one`),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 7: Template vs derived classification
// ---------------------------------------------------------------------------

function checkTemplateVsDerived(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) {
    // Templates should have placeholder markers
    const hasPlaceholders = ctx.lines.some((l) => /\[.*\]/.test(l) && !l.trim().startsWith("//!"));
    if (!hasPlaceholders) {
      results.push(info(file, "template/placeholders",
        "Template has no [bracketed] placeholders — may be a derived file with template directive"));
    }
  } else if (ctx.isCrateRoot) {
    // Derived files should not have template directives
    const hasTemplateDirective = ctx.lines.some((l) =>
      /^\/\/\s+#!omni\s+template\b/.test(l.trim()));
    if (hasTemplateDirective) {
      results.push(warn(file, "derived/template-directive",
        "Derived file has #!omni template directive — should be //omni:code"));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 8: SETUP subsection order
// ---------------------------------------------------------------------------

function checkSetupSubsectionOrder(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Templates mention subsection names in comments — skip order check
  if (ctx.isTemplate) return results;

  const setupLines = getBlockLines(ctx.lines, ctx.blocks, "SETUP");
  if (setupLines.length === 0) return results;

  // Find which subsections are present and their positions
  const found: Array<{ tag: string; lineIdx: number }> = [];

  for (let i = 0; i < setupLines.length; i++) {
    const trimmed = setupLines[i].trim();
    // Skip separator-only lines
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;

    for (const sub of SETUP_SUBSECTIONS) {
      if (sub.pattern.test(trimmed)) {
        if (!found.some((f) => f.tag === sub.tag)) {
          found.push({ tag: sub.tag, lineIdx: i });
        }
        break;
      }
    }
  }

  if (found.length < 2) return results;

  // Check ordering against canonical sequence
  const canonicalOrder = SETUP_SUBSECTIONS.map((s) => s.tag);

  let lastCanonIdx = -1;
  for (const f of found) {
    const canonIdx = canonicalOrder.indexOf(f.tag);
    if (canonIdx < lastCanonIdx) {
      const foundTags = found.map((x) => x.tag).join(" → ");
      results.push(
        warn(file, "setup/subsection-order",
          `SETUP subsection ${f.tag} appears after a later subsection — found: ${foundTags}, expected: Imports → ErrorTypes → CoreTypes`),
      );
      break;
    }
    lastCanonIdx = canonIdx;
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 9: Identity registration
// ---------------------------------------------------------------------------

function checkIdentityRegistration(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Only check crate roots that have pragma/metadata
  if (!ctx.isCrateRoot || ctx.isTemplate) return results;

  const hasPragma = ctx.lines.some((l) => {
    const t = l.trim();
    return /^(pub\s+)?static\s+(OWN_)?PRAGMA\s*:/.test(t);
  });

  if (!hasPragma) return results;

  // Check for register_identity function
  const hasRegister = ctx.lines.some((l) =>
    /^pub\s+fn\s+register_identity\s*\(/.test(l.trim()));

  // Also check for self-registration pattern (identity crate)
  const hasSelfRegister = ctx.lines.some((l) =>
    /bereshit_l0_identity::register\(/.test(l));

  if (!hasRegister && !hasSelfRegister) {
    results.push(info(file, "identity/register",
      "Crate has PRAGMA but no register_identity() — consider adding for runtime identity"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 10: //omni:code directive format
// ---------------------------------------------------------------------------

function checkDirectiveFormat(ctx: RustFileContext): LintResult[] {
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

async function lintRustFile(filePath: string): Promise<LintResult[]> {
  const ctx = await buildContext(filePath);
  if (!ctx) {
    return [error(filePath, "io/read", "Cannot read file")];
  }

  // Quick check: is this a Rust file with any structural markers?
  if (!ctx.hasAnyOmni && !ctx.hasAnyBlock) {
    // Module files without markers are fine — just note it
    if (ctx.isModuleFile) {
      return [
        info(filePath, "structure/module",
          "Module file — no //omni: directives or block markers (identity in lib.rs)"),
      ];
    }
    return [
      info(filePath, "structure/skip",
        "No //omni: directives or block markers — not a 4-block file"),
    ];
  }

  return [
    ...checkDirectives(ctx),
    ...checkBlockStructure(ctx),
    ...checkDocComments(ctx),
    ...checkPragmaMetadata(ctx),
    ...checkCommentMetadata(ctx),
    ...checkSeparatorConsistency(ctx),
    ...checkTemplateVsDerived(ctx),
    ...checkSetupSubsectionOrder(ctx),
    ...checkIdentityRegistration(ctx),
    ...checkDirectiveFormat(ctx),
  ];
}

// ---------------------------------------------------------------------------
// Transformer
// ---------------------------------------------------------------------------

/**
 * Transform a Rust file to fix structural issues.
 *
 * Capabilities:
 *   1. Fix block separator widths (= → 76)
 *   2. Fix Unicode subsection separator widths (─ → 64)
 *
 * Does NOT inject missing block boundaries (too risky for arbitrary positions).
 * Lint first, fix structure manually, then transform for cleanup.
 */
async function transformRustFile(
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

  // --- Transform 2: Fix Unicode subsection separator widths (─ chars) ---
  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i].trim();
    const boxMatch = trimmed.match(/^(\/\/\s+)(─{4,})(\s*)$/);
    if (boxMatch && boxMatch[2].length !== SUBSECTION_SEPARATOR_WIDTH) {
      const newLine = `${boxMatch[1]}${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`;
      if (dryRun) {
        results.push(info(filePath, "transform/box-width",
          `Line ${i + 1}: would fix subsection separator ${boxMatch[2].length} → ${SUBSECTION_SEPARATOR_WIDTH} ─ chars`));
      } else {
        lines[i] = newLine;
        modified = true;
        results.push(info(filePath, "transform/box-width",
          `Line ${i + 1}: fixed subsection separator ${boxMatch[2].length} → ${SUBSECTION_SEPARATOR_WIDTH} ─ chars`));
      }
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

const rustHandler: FormatHandler = {
  name: "rust",
  description: "Rust 4-block alignment (//omni:, METADATA → SETUP → BODY → CLOSING, identity statics)",
  extensions: [".rs"],
  maxDepth: 10,
  lint: lintRustFile,
  transform: transformRustFile,
};

registerFormat(rustHandler);

// ============================================================================
// CLOSING
// ============================================================================
//
// Rust files express structure through comments — //omni: pragma directives,
// block boundary markers, and //! doc comments. This handler reads those
// markers and verifies the skeleton is sound before anyone fills in the flesh.
//
// Two identity patterns: slice-based (&[(&str, &str)]) for lightweight crates,
// LazyLock<BTreeMap> for crates needing runtime-queryable identity. Both valid.
//
// a-01.00: 10 checks + transformer. Adapted from Go handler (go.ts).
//   - Directive validation (//omni:key, //omni:code, //omni:version)
//   - Block structure (METADATA → SETUP → BODY → CLOSING + END markers)
//   - Doc comments (//! crate/module level)
//   - Pragma/Metadata statics (slice or BTreeMap)
//   - Comment metadata (Key:, Purpose:, Biblical:)
//   - Separator consistency (= 76, ─ 64)
//   - Template vs derived classification
//   - SETUP subsection order (Imports → Error Types → Core Types)
//   - Identity registration detection
//   - Directive format validation
//   - Transformer: fix separator widths
//
// "Let all things be done decently and in order." — 1 Corinthians 14:40
// ============================================================================
