// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/handlers/rust.ts
// key:     B-tov-cws-struct-lib-handlers-rust
// title:   CWS Struct — Rust Format Handler
// type:    Code (Library)
// version: b-01.50
// created: 2026-02-14
// updated: 2026-02-17
// authors: Nova Dawn (CPI-SI)
// purpose: Rust 4-block alignment linter + transformer with I/C field validation.
//          Validates Rust source files for:
//          - //omni: directives (key, version, pragma, metadata)
//          - 4-block structure (METADATA → SETUP → BODY → CLOSING)
//          - PRAGMA static I/C fields (I1-I4 Identity sections)
//          - METADATA static I/C fields (C1-C7 Context sections)
//          - Required/defined field validation per I/C section
//          - Doc comments (//! for crate/module level)
//          - Separator style (block = 76, subsection ─ 74)
//          - Usable schema template validation (no placeholders)
//          - 3 subtypes: library, executable, demo-test
//          - SETUP subsection order (10-subsection dependency chain)
//          - Identity registration function
//          - //omni:code directive format validation
//          - Content placement (right constructs in right blocks/subsections)
//
//          Line-based parser — reads .rs files as text and checks structural
//          markers via regex. Tracks brace depth for top-level classification.
//          Does NOT parse Rust AST.
//
//          Schema triangle: rust-4block-schema.jsonc defines, rust.ts validates.
//
// biblical_foundation: "Let all things be done decently and in order."
//   — 1 Corinthians 14:40
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type {
  FormatHandler, LintResult, FixSuggestion, TransformOptions,
  AtomicAction, HealthScore, ContainerScore, BlockScore,
} from "../foundation/mod.ts";
import {
  error, warn, info,
  computeContainerScore, computeBlockScore, computeHealthScore,
} from "../foundation/mod.ts";
import { registerFormat } from "../engine/mod.ts";

// Shared 4-block types, constants, and functions
import type { BlockPosition, DirectiveInfo, SubsectionRange, IdentityField } from "./shared/mod.ts";
import {
  BLOCKS, REQUIRED_DIRECTIVES, RECOMMENDED_DIRECTIVES,
  PRAGMA_FIELD_REQUIREMENTS, METADATA_FIELD_REQUIREMENTS,
  BLOCK_SEPARATOR_WIDTH, SUBSECTION_SEPARATOR_WIDTH,
  BODY_SUBSECTION_PATTERN, CLOSING_ZONES,
  findBlocks, getBlockLines, blockLineToFile, findBlockRange,
  getSubsectionRanges as _getSubsectionRanges,
  checkSeparatorConsistency, checkClosingZoneOrder,
} from "./shared/mod.ts";

// Re-export for tests
export { PRAGMA_FIELD_REQUIREMENTS, METADATA_FIELD_REQUIREMENTS };

/**
 * SETUP subsection markers in canonical dependency-chain order.
 *
 * 10 subsections — each can reference only what came before it.
 * Not all must be present; linter only checks that PRESENT subsections
 * appear in this sequence. The ordering principle is the Rust dependency
 * chain: imports before types, types before traits, traits before macros.
 *
 * Patterns are ANCHORED to the start of the comment text — a line like
 * "// Core Types — wraps the Constants module" matches CoreTypes, NOT
 * Constants, because "Core Types" is at the start and "Constants" is
 * embedded in descriptive text. Optional "N. " prefix is allowed
 * (e.g., "// 3. Constants").
 */
const SETUP_SUBSECTIONS = [
  { tag: "Imports",      pattern: /^\/\/\s+(?:\d+\.\s+)?Imports\b/i },
  { tag: "Modules",      pattern: /^\/\/\s+(?:\d+\.\s+)?(?:Module\s*(?:Declarations|Re-?exports)?|Modules)\b/i },
  { tag: "Constants",    pattern: /^\/\/\s+(?:\d+\.\s+)?Constants\b/i },
  { tag: "Statics",      pattern: /^\/\/\s+(?:\d+\.\s+)?Statics\b/i },
  { tag: "TypeAliases",  pattern: /^\/\/\s+(?:\d+\.\s+)?Type\s*Aliases\b/i },
  { tag: "ErrorTypes",   pattern: /^\/\/\s+(?:\d+\.\s+)?Error\s*Types\b/i },
  { tag: "CoreTypes",    pattern: /^\/\/\s+(?:\d+\.\s+)?Core\s*Types\b/i },
  { tag: "TraitDefs",    pattern: /^\/\/\s+(?:\d+\.\s+)?Trait\s*(?:Def(?:inition)?s?|Impls?)\b/i },
  { tag: "Macros",       pattern: /^\/\/\s+(?:\d+\.\s+)?(?:Macros|Assertion\s*Helpers)\b/i },
  { tag: "FeatureGates", pattern: /^\/\/\s+(?:\d+\.\s+)?Feature\s*Gates\b/i },
] as const;

/** Known //omni:code directive patterns for Rust. */
const KNOWN_CODE_DIRECTIVES = [
  "--rust -library",
  "--rust -executable",
  "--rust -demo-test",
] as const;

// (Separator widths and I/C field requirements imported from shared module)

// ---------------------------------------------------------------------------
// Content Classification — what Rust constructs are, where they belong
// ---------------------------------------------------------------------------

/**
 * Content categories for Rust source lines.
 *
 * Each line of Rust source can be classified into one of these categories.
 * The classifier + placement maps are the foundation for:
 *   1. Block-level placement checks (is this fn in SETUP? → move to BODY)
 *   2. Subsection-level placement checks (is this struct in Imports? → move to CoreTypes)
 *   3. Health scoring (how much of CoreTypes is actually core types?)
 */
export type RustContentKind =
  | "use_decl" | "mod_decl" | "const_decl" | "static_decl" | "type_alias"
  | "struct_decl" | "enum_decl" | "trait_decl" | "macro_decl"
  | "fn_decl" | "impl_block"
  | "cfg_attr" | "test_attr" | "attr"
  | "comment" | "blank" | "other";

/**
 * Which BLOCK a top-level construct belongs in.
 *
 * Schema source: rust-4block-schema.jsonc → placement_rules
 *   must_be_in_setup  → these map to "SETUP"
 *   must_not_be_in_setup (fn, impl) → these map to "BODY"
 *
 * Missing entries (comment, blank, attr, other) → can appear in any block.
 */
const BLOCK_PLACEMENT: Partial<Record<RustContentKind, string>> = {
  use_decl:    "SETUP",
  mod_decl:    "SETUP",
  const_decl:  "SETUP",
  static_decl: "SETUP",
  type_alias:  "SETUP",
  struct_decl: "SETUP",
  enum_decl:   "SETUP",
  trait_decl:  "SETUP",
  macro_decl:  "SETUP",
  fn_decl:     "BODY",
  impl_block:  "BODY",
};

/**
 * Which SUBSECTION within SETUP a construct belongs in.
 *
 * Schema source: rust-4block-schema.jsonc → subsection_order → S1-S10
 * Maps content kinds to the canonical subsection tag where they should live.
 * Used for subsection-level scoring (health scoring foundation).
 */
const SUBSECTION_PLACEMENT: Partial<Record<RustContentKind, string>> = {
  use_decl:    "Imports",
  mod_decl:    "Modules",
  const_decl:  "Constants",
  static_decl: "Statics",
  type_alias:  "TypeAliases",
  struct_decl: "CoreTypes",
  enum_decl:   "CoreTypes",
  trait_decl:  "TraitDefs",
  macro_decl:  "Macros",
};

/**
 * Content kinds that are NEVER valid in the METADATA block.
 *
 * METADATA contains identity comments (Key:, Purpose:, Biblical:)
 * and identity statics (PRAGMA, METADATA). Code declarations leak here
 * when someone adds content above SETUP by mistake.
 */
const METADATA_FORBIDDEN: Set<RustContentKind> = new Set([
  "use_decl", "mod_decl", "const_decl", "static_decl", "type_alias",
  "struct_decl", "enum_decl", "trait_decl", "macro_decl",
  "fn_decl", "impl_block",
]);

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

// (BlockPosition, DirectiveInfo imported from shared module)

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
  directives: Map<string, DirectiveInfo>;
  crateHasIdentity: boolean;     // sibling lib.rs has PRAGMA/METADATA
  subtype: string | null;        // "library" | "executable" | "demo-test" | null
}

// ---------------------------------------------------------------------------
// Helpers — Rust-specific (shared helpers imported from ./shared/mod.ts)
// ---------------------------------------------------------------------------

/**
 * Find all //omni: directives in the file.
 * Rust //omni: directives can appear before or after doc comments (//!),
 * but always before the first block marker or `use` statement.
 */
function findOmniDirectives(lines: string[]): Map<string, DirectiveInfo> {
  const directives = new Map<string, DirectiveInfo>();

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();
    const lineNum = i + 1;

    // Stop scanning at first block marker or code
    if (/^\/\/\s+(METADATA|SETUP|BODY|CLOSING)\s*$/.test(trimmed)) break;
    if (/^(pub\s+)?(fn|struct|enum|trait|impl|mod|type|const|static)\s/.test(trimmed)) break;
    if (/^use\s/.test(trimmed)) break;

    // Standard //omni: directive
    const omniMatch = trimmed.match(/^\/\/omni:(\S+)\s*(.*)?$/);
    if (omniMatch) {
      directives.set(`//omni:${omniMatch[1]}`, { value: omniMatch[2]?.trim() ?? "", line: lineNum });
    }

    // Template-style // #!omni directive
    const shebangMatch = trimmed.match(/^\/\/\s+#!omni\s+(.+)$/);
    if (shebangMatch) {
      const parts = shebangMatch[1]!.trim();
      if (parts.startsWith("template")) {
        directives.set("#!omni:template", { value: parts.replace(/^template\s*/, "").trim(), line: lineNum });
      } else if (parts.startsWith("code")) {
        directives.set("#!omni:code", { value: parts.replace(/^code\s*/, "").trim(), line: lineNum });
      } else if (parts.startsWith("meta.")) {
        const metaMatch = parts.match(/^meta\.(\S+)\s*=\s*(.+)$/);
        if (metaMatch) {
          directives.set(`#!omni:meta.${metaMatch[1]!}`, { value: metaMatch[2]!.trim(), line: lineNum });
        }
      }
    }
  }

  return directives;
}

// (getBlockLines, blockLineToFile imported from shared module)

// ---------------------------------------------------------------------------
// Content classifier — what's on this line?
// ---------------------------------------------------------------------------

/**
 * Classify a single Rust source line into a content category.
 *
 * Works on trimmed lines. Handles `pub`, `pub(crate)`, `unsafe`, `async`
 * prefixes. Returns the semantic category, not the syntactic form.
 *
 * Exported for testing — this is the foundation of content placement checks
 * and (future) health scoring.
 */
export function classifyLine(rawLine: string): RustContentKind {
  const trimmed = rawLine.trim();

  // Blank
  if (trimmed === "") return "blank";

  // Comments (structural markers, doc comments, regular comments)
  if (trimmed.startsWith("//")) return "comment";

  // Attributes — check specific ones first
  if (/^#\[test\b/.test(trimmed) || /^#\[cfg\(test\b/.test(trimmed)) return "test_attr";
  if (/^#\[cfg\(/.test(trimmed) || /^#!\[cfg\(/.test(trimmed)) return "cfg_attr";
  if (trimmed.startsWith("#[") || trimmed.startsWith("#![")) return "attr";

  // Strip visibility prefix: pub, pub(crate), pub(super), pub(in path)
  const stripped = trimmed
    .replace(/^pub\s*\((?:crate|super|in\s+\S+)\)\s+/, "")
    .replace(/^pub\s+/, "");

  // Strip unsafe prefix (unsafe fn, unsafe impl, unsafe trait)
  const noUnsafe = stripped.replace(/^unsafe\s+/, "");

  // Declarations — ordered by frequency in typical Rust files
  if (noUnsafe.startsWith("use ") || noUnsafe.startsWith("use{")) return "use_decl";
  if (noUnsafe.startsWith("mod ")) return "mod_decl";
  if (/^const\s+[A-Z_]/.test(noUnsafe)) return "const_decl";
  if (/^static\s+[A-Z_]/.test(noUnsafe)) return "static_decl";
  if (/^type\s+[A-Z]/.test(noUnsafe)) return "type_alias";
  if (/^struct\s+\w/.test(noUnsafe)) return "struct_decl";
  if (/^enum\s+\w/.test(noUnsafe)) return "enum_decl";
  if (/^trait\s+\w/.test(noUnsafe)) return "trait_decl";
  if (noUnsafe.startsWith("macro_rules!")) return "macro_decl";

  // fn — can be async
  if (/^(async\s+)?fn\s+\w/.test(noUnsafe)) return "fn_decl";

  // impl blocks — impl Foo, impl Trait for Foo
  if (/^impl\b/.test(noUnsafe)) return "impl_block";

  return "other";
}

// (SubsectionRange imported from shared module)

/**
 * Find subsection boundaries within a block's lines.
 * Binds Rust-specific SETUP_SUBSECTIONS to the shared parameterized function.
 * Exported for testing.
 */
export function getSubsectionRanges(blockLines: string[]): SubsectionRange[] {
  return _getSubsectionRanges(blockLines, SETUP_SUBSECTIONS);
}

/**
 * Classify top-level declarations in a line array.
 *
 * Tracks brace depth to skip nested content — a `const` inside an `impl`
 * block is NOT a top-level const_decl. Only declarations at brace depth 0
 * are returned.
 *
 * Exported for testing.
 */
export function getTopLevelDeclarations(
  lines: string[],
): Array<{ lineIdx: number; kind: RustContentKind }> {
  const results: Array<{ lineIdx: number; kind: RustContentKind }> = [];
  let braceDepth = 0;

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();

    // At top level, classify the line
    if (braceDepth === 0) {
      const kind = classifyLine(trimmed);
      // Only track actual declarations, not noise
      if (kind !== "blank" && kind !== "comment" && kind !== "attr"
          && kind !== "cfg_attr" && kind !== "test_attr" && kind !== "other") {
        results.push({ lineIdx: i, kind });
      }
    }

    // Update brace depth AFTER classification
    // Simple brace counting on trimmed line — handles 95%+ of Rust code.
    // Edge cases (braces in strings/raw strings) are rare at top-level.
    for (const ch of trimmed) {
      if (ch === '{') braceDepth++;
      if (ch === '}') braceDepth = Math.max(0, braceDepth - 1);
    }
  }

  return results;
}

/**
 * Check if a sibling lib.rs in the same directory has pragma/metadata statics.
 *
 * Results are cached per directory — when linting N .rs files in the same crate,
 * lib.rs is only read once.
 */
const _rustSiblingCache = new Map<string, boolean>();

/** Clear the sibling cache. Exported for test isolation. */
export function clearRustSiblingCache(): void {
  _rustSiblingCache.clear();
}

async function crateHasIdentityStatics(filePath: string): Promise<boolean> {
  const filename = filePath.split("/").pop() ?? "";
  if (filename === "lib.rs") return false;

  const dir = filePath.substring(0, filePath.lastIndexOf("/"));
  const libPath = dir ? `${dir}/lib.rs` : "lib.rs";

  // Cache hit
  const cached = _rustSiblingCache.get(dir);
  if (cached !== undefined) return cached;

  try {
    const libText = await Deno.readTextFile(libPath);
    const hasPragma = /^pub\s+static\s+PRAGMA\s*:/m.test(libText) ||
                      /^static\s+PRAGMA\s*:/m.test(libText);
    const hasMetadata = /^pub\s+static\s+METADATA\s*:/m.test(libText) ||
                        /^static\s+METADATA\s*:/m.test(libText);
    const result = hasPragma && hasMetadata;
    _rustSiblingCache.set(dir, result);
    return result;
  } catch {
    _rustSiblingCache.set(dir, false);
    return false;
  }
}

/**
 * Build file-level context — gathered once, passed to all checks.
 */
async function buildContext(filePath: string): Promise<RustFileContext> {
  // Let read errors propagate — caller handles with full context
  const text = await Deno.readTextFile(filePath);

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

  // Detect subtype from directives, then PRAGMA I2.subtype if not found.
  // Sources (priority order):
  //   1. //omni:code --rust -<subtype>
  //   2. #!omni template --rust -<subtype>
  //   3. PRAGMA I2.subtype field
  let subtype: string | null = null;
  const KNOWN_SUBTYPES = ["library", "executable", "demo-test"];

  const codeDirective = directives.get("//omni:code")?.value ?? directives.get("#!omni:code")?.value ?? "";
  const templateDirective = directives.get("#!omni:template")?.value ?? "";

  for (const directive of [codeDirective, templateDirective]) {
    if (!directive) continue;
    const subtypeMatch = directive.match(/-(\w[\w-]*)$/);
    if (subtypeMatch && KNOWN_SUBTYPES.includes(subtypeMatch[1]!)) {
      subtype = subtypeMatch[1]!;
      break;
    }
  }

  // Fallback: parse I2.subtype from PRAGMA static
  if (!subtype) {
    const pragmaFields = parseStaticFields(lines, "PRAGMA");
    const subtypeField = pragmaFields.find((f) => f.section === "I2" && f.field === "subtype");
    if (subtypeField && KNOWN_SUBTYPES.includes(subtypeField.value)) {
      subtype = subtypeField.value;
    }
  }

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
    subtype,
  };
}

// ---------------------------------------------------------------------------
// I/C Static Parsing — extract fields from PRAGMA/METADATA statics
// ---------------------------------------------------------------------------

// (IdentityField imported from shared module)

/**
 * Parse a Rust identity static to extract I/C field key-value pairs.
 *
 * Input format:
 *   pub static PRAGMA: &[(&str, &str)] = &[
 *       ("I1.key", "value"),
 *       // comments are skipped
 *   ];
 *
 * Returns fields with section (e.g., "I1"), field (e.g., "key"), and value.
 */
export function parseStaticFields(lines: string[], staticName: string): IdentityField[] {
  const fields: IdentityField[] = [];
  let inStatic = false;
  const startPattern = new RegExp(
    `^(pub\\s+)?static\\s+(OWN_)?${staticName}\\s*:`,
  );

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();
    const lineNum = i + 1;

    if (!inStatic) {
      if (startPattern.test(trimmed)) {
        inStatic = true;
        // Single-line static: ends with ]; on same line
        if (trimmed.includes("];")) {
          // Match both regular "value" and r#"value"# patterns
          for (const pair of trimmed.matchAll(/\("([^"]+)",\s*(?:"([^"]*)"|r#"(.*?)"#)\)/g)) {
            const fullKey = pair[1]!;
            const value = pair[2] ?? pair[3] ?? "";
            const dotIdx = fullKey.indexOf(".");
            if (dotIdx > 0) {
              fields.push({
                section: fullKey.substring(0, dotIdx),
                field: fullKey.substring(dotIdx + 1),
                value,
                line: lineNum,
              });
            }
          }
          break;
        }
      }
      continue;
    }

    // End of static
    if (trimmed === "];" || trimmed.startsWith("];")) break;

    // Skip comment-only lines
    if (trimmed.startsWith("//")) continue;

    // Extract ("key", "value") pair — supports both regular and r#"..."# raw strings.
    // Regular: ("I1.key", "value")
    // Raw:     ("I1.key", r#"value with "quotes""#)
    // NOTE: Multi-hash raw strings (r##"..."##) are not supported.
    const match = trimmed.match(/\("([^"]+)",\s*"([^"]*)"\)/)
      ?? trimmed.match(/\("([^"]+)",\s*r#"(.*?)"#\)/);
    if (match) {
      const fullKey = match[1]!;
      const dotIdx = fullKey.indexOf(".");
      if (dotIdx > 0) {
        fields.push({
          section: fullKey.substring(0, dotIdx),
          field: fullKey.substring(dotIdx + 1),
          value: match[2]!,
          line: lineNum,
        });
      }
    }
  }

  return fields;
}

/**
 * Validate parsed I/C fields against field requirements.
 * Required fields produce warnings; defined fields produce info.
 * Handles nested keys (e.g., "C4.requires.stdlib" counts as "requires" present).
 */
export function validateICFields(
  file: string,
  fields: IdentityField[],
  requirements: Record<string, { required: string[]; defined: string[] }>,
  staticName: string,
): LintResult[] {
  const results: LintResult[] = [];

  // Group fields by section, tracking base field names
  const presentFields = new Map<string, Set<string>>();
  for (const f of fields) {
    if (!presentFields.has(f.section)) {
      presentFields.set(f.section, new Set());
    }
    // For nested fields like "requires.stdlib", the base field "requires" counts as present
    const baseField = f.field.split(".")[0]!;
    presentFields.get(f.section)!.add(baseField);
  }

  // Check required fields in each section
  for (const [section, req] of Object.entries(requirements)) {
    const sectionFields = presentFields.get(section);

    for (const field of req.required) {
      if (!sectionFields?.has(field)) {
        results.push(warn(file, `identity/${staticName}/${section}.${field}`,
          `Missing required field ${section}.${field} in ${staticName} static`));
      }
    }

    for (const field of req.defined) {
      if (!sectionFields?.has(field)) {
        results.push(info(file, `identity/${staticName}/${section}.${field}`,
          `Missing defined field ${section}.${field} in ${staticName} static`));
      }
    }
  }

  return results;
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
          : `Missing ${directive}`,
        { line: 1 }));  // directives belong at top of file
    }
  }

  for (const directive of RECOMMENDED_DIRECTIVES) {
    if (!ctx.directives.has(directive)) {
      results.push(warn(file, `directive/${directive}`, `Missing ${directive} — recommended`,
        { line: 1 }));
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
      const badBlock = ctx.blocks[i];
      results.push(
        error(file, "block/order",
          `Block order wrong: found ${actualOrder.join(" → ")}, expected ${BLOCKS.join(" → ")}`,
          badBlock ? { line: badBlock.line } : undefined),
      );
      break;
    }
  }

  // Check END markers
  for (const block of ctx.blocks) {
    if (block.endLine === 0) {
      results.push(
        warn(file, `block/end-${block.name}`, `Missing END ${block.name} marker`,
          { line: block.line }),
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

  // Doc comments (//!) belong in the pre-METADATA area — before any block
  // markers or code statements. A //! in the BODY block should NOT satisfy
  // the crate-level doc comment check.
  const preMetadataEnd = ctx.blocks.length > 0
    ? ctx.blocks[0]!.line - 1  // line before first block marker
    : ctx.lines.length;        // no blocks → scan entire file

  const hasDocComments = ctx.lines
    .slice(0, preMetadataEnd)
    .some((l) => l.trim().startsWith("//!"));

  // Templates: check for //! placeholder
  if (ctx.isTemplate) {
    if (!hasDocComments) {
      results.push(info(file, "doc/template", "Template has no //! doc comment section"));
    }
    return results;
  }

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

  // Templates also get I/C validation — usable schema pattern means they have real statics
  if (ctx.isModuleFile) return results;

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
      "Missing PRAGMA static — recommended for crate identity (I1-I4)"));
  }

  if (!hasMetadata) {
    results.push(warn(file, "identity/metadata-static",
      "Missing METADATA static — recommended for crate context (C1-C7)"));
  }

  // ── I/C field validation ────────────────────────────────────────
  // If statics exist, parse them and validate required/defined fields
  // against the schema's field_requirements.

  if (hasPragma) {
    const pragmaFields = parseStaticFields(ctx.lines, "PRAGMA");
    if (pragmaFields.length > 0) {
      results.push(...validateICFields(file, pragmaFields, PRAGMA_FIELD_REQUIREMENTS, "PRAGMA"));

      // Subtype validation: if I2.subtype is present, check it's a known value
      const subtypeField = pragmaFields.find((f) => f.section === "I2" && f.field === "subtype");
      if (subtypeField) {
        const KNOWN_SUBTYPES = ["library", "executable", "demo-test"];
        if (!KNOWN_SUBTYPES.includes(subtypeField.value)) {
          results.push(warn(file, "identity/PRAGMA/I2.subtype-value",
            `Unknown I2.subtype "${subtypeField.value}" — known values: ${KNOWN_SUBTYPES.join(", ")}`));
        }
      }
    } else if (!hasDelegatedIdentity) {
      results.push(info(file, "identity/pragma-parse",
        "PRAGMA static found but no I/C fields could be parsed"));
    }
  }

  if (hasMetadata) {
    const metadataFields = parseStaticFields(ctx.lines, "METADATA");
    if (metadataFields.length > 0) {
      results.push(...validateICFields(file, metadataFields, METADATA_FIELD_REQUIREMENTS, "METADATA"));
    } else {
      results.push(info(file, "identity/metadata-parse",
        "METADATA static found but no I/C fields could be parsed"));
    }
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

// Check 6: Separator consistency — imported from shared module

// ---------------------------------------------------------------------------
// Check 7: Template vs derived classification
// ---------------------------------------------------------------------------

function checkTemplateVsDerived(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) {
    // Usable schema: templates should have I/C statics with self-describing values
    const hasPragma = ctx.lines.some((l) =>
      /^(pub\s+)?static\s+(OWN_)?PRAGMA\s*:/.test(l.trim()));
    const hasMetadata = ctx.lines.some((l) =>
      /^(pub\s+)?static\s+(OWN_)?METADATA\s*:/.test(l.trim()));

    if (!hasPragma) {
      results.push(warn(file, "template/pragma-static",
        "Template missing PRAGMA static — usable schema pattern requires self-describing I/C statics"));
    }
    if (!hasMetadata) {
      results.push(warn(file, "template/metadata-static",
        "Template missing METADATA static — usable schema pattern requires self-describing I/C statics"));
    }

    // Usable schema: no [bracketed] placeholder values in statics
    if (hasPragma) {
      const pragmaFields = parseStaticFields(ctx.lines, "PRAGMA");
      const hasPlaceholders = pragmaFields.some((f) =>
        f.value.startsWith("[") && f.value.endsWith("]"));
      if (hasPlaceholders) {
        results.push(warn(file, "template/placeholders",
          "Template has [bracketed] placeholder values — usable schema requires self-describing values"));
      }
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
    const trimmed = setupLines[i]!.trim();
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
  const canonicalOrder: string[] = SETUP_SUBSECTIONS.map((s) => s.tag);

  let lastCanonIdx = -1;
  for (const f of found) {
    const canonIdx = canonicalOrder.indexOf(f.tag);
    if (canonIdx < lastCanonIdx) {
      const foundTags = found.map((x) => x.tag).join(" → ");
      const fileLine = blockLineToFile(ctx.blocks, "SETUP", f.lineIdx);
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
// Check 12: BODY subsection order
// ---------------------------------------------------------------------------

/**
 * BODY subsection marker pattern.
 *
 * BODY subsections use numbered markers: `// N. <Name>` where N is 1-N.
 * The number determines canonical order. Unlike SETUP (which uses name-based
 * matching), BODY uses numeric ordering — simpler and subtype-agnostic.
 *
 * Examples from seed templates:
 *   Library (7):      1. Identity Access → 2. Trait Impl → ... → 7. Free Functions
 *   Executable (3):   1. Argument Parsing → 2. Core Logic → 3. Output
 *   Demo-test (9):    1. Identity Access → ... → 9. Edge Cases
 */
// (BODY_SUBSECTION_PATTERN imported from shared module)

function checkBodySubsectionOrder(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Templates use numbered subsections in comments — skip order check
  if (ctx.isTemplate) return results;

  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  if (bodyLines.length === 0) return results;

  // Find numbered subsection markers in BODY
  const found: Array<{ num: number; name: string; lineIdx: number }> = [];

  for (let i = 0; i < bodyLines.length; i++) {
    const trimmed = bodyLines[i]!.trim();
    // Skip separator-only lines
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;

    const match = BODY_SUBSECTION_PATTERN.exec(trimmed);
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
      const fileLine = blockLineToFile(ctx.blocks, "BODY", f.lineIdx);
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

// Check 13: CLOSING zone ordering — imported from shared module
// (CLOSING_ZONES constant and checkClosingZoneOrder function)

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
  const codeInfo = ctx.directives.get("//omni:code");
  if (codeInfo && codeInfo.value !== "") {
    const isKnown = KNOWN_CODE_DIRECTIVES.some((k) => codeInfo.value === k);
    if (!isKnown) {
      results.push(
        info(file, "directive/code-format",
          `//omni:code value "${codeInfo.value}" — not a recognized pattern (known: ${KNOWN_CODE_DIRECTIVES.join(", ")})`,
          { line: codeInfo.line }),
      );
    }
  }

  // Check #!omni template value for template files
  const templateInfo = ctx.directives.get("#!omni:template");
  if (templateInfo && templateInfo.value !== "") {
    const isKnown = KNOWN_CODE_DIRECTIVES.some((k) => templateInfo.value === k);
    if (!isKnown) {
      results.push(
        info(file, "directive/template-format",
          `#!omni template value "${templateInfo.value}" — not a recognized pattern`,
          { line: templateInfo.line }),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 11: Content placement — right content in right block/subsection
// ---------------------------------------------------------------------------

/**
 * Validate that Rust constructs appear in the correct block and subsection.
 *
 * Three levels of checking:
 *   1. METADATA block — no code declarations allowed (use, fn, struct, etc.)
 *   2. SETUP vs BODY — declarations in SETUP, logic in BODY
 *   3. Subsection placement — use in Imports, struct in CoreTypes, etc.
 *
 * Only checks TOP-LEVEL declarations (brace depth 0). Nested content
 * (e.g., a const inside an impl block) is not flagged — it belongs to
 * the enclosing construct.
 *
 * Foundation for health scoring: once we know WHAT is WHERE, scoring is
 * just counting + weighting.
 */
function checkContentPlacement(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Templates use instructional placeholders — skip content checks
  if (ctx.isTemplate) return results;

  // Need at least 2 blocks to reason about placement
  if (ctx.blocks.length < 2) return results;

  // ---- Level 1: METADATA block — no code declarations ----
  const metaLines = getBlockLines(ctx.lines, ctx.blocks, "METADATA");
  if (metaLines.length > 0) {
    const metaBlock = ctx.blocks.find((b) => b.name === "METADATA");
    const metaStart = metaBlock?.line ?? 0;

    for (const decl of getTopLevelDeclarations(metaLines)) {
      if (METADATA_FORBIDDEN.has(decl.kind)) {
        const fileLine = metaStart + 1 + decl.lineIdx;
        results.push(
          warn(file, "content/metadata-leak",
            `${decl.kind} in METADATA block (line ${fileLine}) — code declarations belong in SETUP or BODY`,
            { line: fileLine }),
        );
      }
    }
  }

  // ---- Level 2: SETUP vs BODY — declarations vs logic ----
  for (const blockName of ["SETUP", "BODY"] as const) {
    const blockLines = getBlockLines(ctx.lines, ctx.blocks, blockName);
    if (blockLines.length === 0) continue;

    const block = ctx.blocks.find((b) => b.name === blockName);
    const blockStart = block?.line ?? 0;

    for (const decl of getTopLevelDeclarations(blockLines)) {
      const expectedBlock = BLOCK_PLACEMENT[decl.kind];
      // Skip kinds that can appear anywhere
      if (!expectedBlock) continue;

      if (expectedBlock !== blockName) {
        const fileLine = blockStart + 1 + decl.lineIdx;
        results.push(
          warn(file, "content/block-placement",
            `${decl.kind} in ${blockName} block (line ${fileLine}) — expected in ${expectedBlock}`,
            { line: fileLine }),
        );
      }
    }
  }

  // ---- Level 3: Subsection placement within SETUP ----
  const setupLines = getBlockLines(ctx.lines, ctx.blocks, "SETUP");
  if (setupLines.length > 0) {
    const subsections = getSubsectionRanges(setupLines);
    const setupBlock = ctx.blocks.find((b) => b.name === "SETUP");
    const setupStart = setupBlock?.line ?? 0;

    for (const sub of subsections) {
      const subLines = setupLines.slice(sub.startIdx, sub.endIdx);

      for (const decl of getTopLevelDeclarations(subLines)) {
        const expectedSub = SUBSECTION_PLACEMENT[decl.kind];
        if (!expectedSub) continue;

        if (expectedSub !== sub.tag) {
          const fileLine = setupStart + 1 + sub.startIdx + decl.lineIdx;
          results.push(
            info(file, "content/subsection-placement",
              `${decl.kind} in ${sub.tag} subsection (line ${fileLine}) — expected in ${expectedSub}`,
              { line: fileLine }),
          );
        }
      }
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 14: CLOSING content placement — tests and main() belong in CLOSING
// ---------------------------------------------------------------------------

/**
 * Detect content that belongs in CLOSING zones but was placed in BODY.
 *
 * Two specific patterns:
 *   1. #[cfg(test)] mod tests { ... } in BODY → belongs in CLOSING Cv zone
 *   2. fn main() in BODY → belongs in CLOSING Ce zone (executables only)
 *
 * These are more specific than checkContentPlacement's generic check.
 * The generic check catches mod_decl in BODY (flags test modules),
 * but this check provides CLOSING-specific guidance with the right
 * target zone.
 */
function checkClosingContentPlacement(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) return results;

  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  if (bodyLines.length === 0) return results;

  const bodyBlock = ctx.blocks.find((b) => b.name === "BODY");
  const bodyStart = bodyBlock?.line ?? 0;

  // --- Check 1: #[cfg(test)] in BODY ---
  let inCfgTest = false;
  for (let i = 0; i < bodyLines.length; i++) {
    const trimmed = bodyLines[i]!.trim();

    if (/^#\[cfg\(test\)\]/.test(trimmed)) {
      inCfgTest = true;
      continue;
    }

    if (inCfgTest && /^(pub\s+)?mod\s+tests?\b/.test(trimmed)) {
      const fileLine = bodyStart + 1 + i;
      results.push(
        warn(file, "closing/test-placement",
          `#[cfg(test)] test module in BODY block (line ${fileLine}) — tests belong in CLOSING Cv zone`,
          { line: fileLine }),
      );
      inCfgTest = false;
    }

    // Reset if we see something else after #[cfg(test)] that isn't the mod
    if (inCfgTest && trimmed !== "" && !trimmed.startsWith("//")) {
      inCfgTest = false;
    }
  }

  // --- Check 2: fn main() in BODY (executable subtype) ---
  for (let i = 0; i < bodyLines.length; i++) {
    const trimmed = bodyLines[i]!.trim();

    if (/^(pub\s+)?fn\s+main\s*\(/.test(trimmed)) {
      const fileLine = bodyStart + 1 + i;
      results.push(
        warn(file, "closing/main-placement",
          `fn main() in BODY block (line ${fileLine}) — entry point belongs in CLOSING Ce zone`,
          { line: fileLine }),
      );
      break; // Only one main per file
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Lint orchestrator
// ---------------------------------------------------------------------------

async function lintRustFile(filePath: string): Promise<LintResult[]> {
  let ctx: RustFileContext;
  try {
    ctx = await buildContext(filePath);
  } catch (e: unknown) {
    const msg = e instanceof Error ? e.message : String(e);
    return [error(filePath, "io/read", `Cannot read file: ${msg}`)];
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
    ...checkContentPlacement(ctx),
    ...checkBodySubsectionOrder(ctx),
    ...checkClosingZoneOrder(ctx),
    ...checkClosingContentPlacement(ctx),
  ];
}

// ---------------------------------------------------------------------------
// Transformer helpers — structural content moves
// ---------------------------------------------------------------------------

// (BlockRange, findBlockRange imported from shared module)

/**
 * Find #[cfg(test)] mod ... { ... } in a line range.
 * Returns the start/end indices (inclusive) or null.
 * Uses brace tracking to find the complete module.
 */
function findCfgTestInRange(
  lines: string[], rangeStart: number, rangeEnd: number,
): { start: number; end: number } | null {
  for (let i = rangeStart; i < rangeEnd; i++) {
    const trimmed = lines[i]!.trim();
    if (trimmed !== "#[cfg(test)]") continue;

    // Found #[cfg(test)] — now find the mod declaration and its closing brace
    const start = i;
    let braceDepth = 0;
    let foundOpenBrace = false;

    for (let j = i; j < rangeEnd; j++) {
      const line = lines[j]!;
      for (const ch of line) {
        if (ch === "{") { braceDepth++; foundOpenBrace = true; }
        if (ch === "}") braceDepth--;
      }
      if (foundOpenBrace && braceDepth === 0) {
        return { start, end: j };
      }
    }

    // Incomplete block — don't attempt to move
    return null;
  }
  return null;
}

/**
 * Find fn main() { ... } in a line range.
 * Returns the start/end indices (inclusive) or null.
 */
function findFnMainInRange(
  lines: string[], rangeStart: number, rangeEnd: number,
): { start: number; end: number } | null {
  for (let i = rangeStart; i < rangeEnd; i++) {
    const trimmed = lines[i]!.trim();
    if (!/^(pub\s+)?fn\s+main\s*\(/.test(trimmed)) continue;

    const start = i;
    let braceDepth = 0;
    let foundOpenBrace = false;

    for (let j = i; j < rangeEnd; j++) {
      const line = lines[j]!;
      for (const ch of line) {
        if (ch === "{") { braceDepth++; foundOpenBrace = true; }
        if (ch === "}") braceDepth--;
      }
      if (foundOpenBrace && braceDepth === 0) {
        return { start, end: j };
      }
    }

    return null;
  }
  return null;
}

/**
 * Parse CLOSING content lines into zones, then return them in canonical order
 * if reordering is needed. Returns null if already in correct order.
 *
 * Canonical order: code zones (Cv, Ce, Cc) then documentation (X1-X6).
 * Within each tier, canonical order applies.
 */
function reorderClosingZones(closingContent: string[]): string[] | null {
  // Parse into zone chunks. A zone starts with its subsection separator.
  interface ZoneChunk {
    tag: string;
    kind: "code" | "doc";
    lines: string[];
    canonicalIdx: number;
  }

  const canonicalOrder = ["Cv", "Ce", "Cc", "X1", "X2", "X3", "X4", "X5", "X6"];

  // Find zone boundaries in the content
  const zones: ZoneChunk[] = [];
  let preambleLines: string[] = [];  // Lines before the first zone
  let currentZone: ZoneChunk | null = null;

  for (let i = 0; i < closingContent.length; i++) {
    const trimmed = closingContent[i]!.trim();

    // Check if this line starts a new zone (look for zone tag after a separator)
    let matchedZone: { tag: string; kind: "code" | "doc" } | null = null;
    for (const zone of CLOSING_ZONES) {
      if (zone.pattern.test(trimmed)) {
        matchedZone = { tag: zone.tag, kind: zone.kind };
        break;
      }
    }

    if (matchedZone) {
      // Save current zone
      if (currentZone) {
        zones.push(currentZone);
      }

      // Start new zone. Look back to grab the separator line preceding this zone tag.
      const newZoneLines: string[] = [];

      // Grab the separator line that precedes this zone header
      if (i > 0 && /^\/\/\s*[─=\-]{10,}\s*$/.test(closingContent[i - 1]!.trim())) {
        // If previous line was accumulated in preamble or current zone, steal it
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

  // Push last zone
  if (currentZone) {
    zones.push(currentZone);
  }

  if (zones.length < 2) return null; // Nothing to reorder

  // Check if already in correct order
  let inOrder = true;
  for (let i = 1; i < zones.length; i++) {
    if (zones[i]!.canonicalIdx < zones[i - 1]!.canonicalIdx) {
      inOrder = false;
      break;
    }
  }

  if (inOrder) return null;

  // Sort zones by canonical index
  zones.sort((a, b) => a.canonicalIdx - b.canonicalIdx);

  // Reconstruct: preamble + sorted zones
  const result: string[] = [...preambleLines];
  for (const zone of zones) {
    result.push(...zone.lines);
  }

  return result;
}

// ---------------------------------------------------------------------------
// Transformer
// ---------------------------------------------------------------------------

/**
 * Transform a Rust file to fix structural issues.
 *
 * Capabilities:
 *   1. Fix block separator widths (= → 76)
 *   2. Fix Unicode subsection separator widths (─ → 74)
 *   3. Convert ASCII dash subsection separators (----) to Unicode (────)
 *   4. Normalize subsection separator comment prefix to standard `// `
 *   5. Move #[cfg(test)] from BODY to CLOSING Cv zone
 *   6. Move fn main() from BODY to CLOSING Ce zone
 *   7. Reorder CLOSING zones to canonical order (Cv → Ce → Cc → X1-X6)
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
    const trimmed = lines[i]!.trim();
    const eqMatch = trimmed.match(/^(\/\/\s+)(={4,})(\s*)$/);
    if (eqMatch && eqMatch[2]!.length !== BLOCK_SEPARATOR_WIDTH) {
      const newLine = `${eqMatch[1]}${"=".repeat(BLOCK_SEPARATOR_WIDTH)}`;
      if (dryRun) {
        results.push(info(filePath, "transform/eq-width",
          `Line ${i + 1}: would fix block separator ${eqMatch[2]!.length} → ${BLOCK_SEPARATOR_WIDTH} chars`));
      } else {
        lines[i] = newLine;
        modified = true;
        results.push(info(filePath, "transform/eq-width",
          `Line ${i + 1}: fixed block separator ${eqMatch[2]!.length} → ${BLOCK_SEPARATOR_WIDTH} chars`));
      }
    }
  }

  // --- Transform 2: Fix Unicode subsection separator widths (─ chars) ---
  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();
    const boxMatch = trimmed.match(/^(\/\/\s+)(─{4,})(\s*)$/);
    if (boxMatch && boxMatch[2]!.length !== SUBSECTION_SEPARATOR_WIDTH) {
      const newLine = `${boxMatch[1]}${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`;
      if (dryRun) {
        results.push(info(filePath, "transform/box-width",
          `Line ${i + 1}: would fix subsection separator ${boxMatch[2]!.length} → ${SUBSECTION_SEPARATOR_WIDTH} ─ chars`));
      } else {
        lines[i] = newLine;
        modified = true;
        results.push(info(filePath, "transform/box-width",
          `Line ${i + 1}: fixed subsection separator ${boxMatch[2]!.length} → ${SUBSECTION_SEPARATOR_WIDTH} ─ chars`));
      }
    }
  }

  // --- Transform 3: Convert ASCII dash separators to Unicode ─ ---
  // Matches lines like: // --------  or  //--------  or  //  --------
  // Converts to:        // ──────── (SUBSECTION_SEPARATOR_WIDTH ─ chars)
  // Also normalizes prefix to standard `// ` (single space)
  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();
    const dashMatch = trimmed.match(/^(\/\/\s*)(-{4,})(\s*)$/);
    if (dashMatch) {
      const newLine = `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`;
      if (dryRun) {
        results.push(info(filePath, "transform/dash-to-unicode",
          `Line ${i + 1}: would convert ${dashMatch[2]!.length} ASCII dashes → ${SUBSECTION_SEPARATOR_WIDTH} Unicode ─`));
      } else {
        lines[i] = newLine;
        modified = true;
        results.push(info(filePath, "transform/dash-to-unicode",
          `Line ${i + 1}: converted ${dashMatch[2]!.length} ASCII dashes → ${SUBSECTION_SEPARATOR_WIDTH} Unicode ─`));
      }
    }
  }

  // --- Transform 4: Normalize subsection separator prefix to `// ` ---
  // Ensures exactly one space after // for separators: `//  ────` → `// ────`
  // Also catches `//────` (no space) → `// ────`
  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();
    // Match separators with wrong prefix spacing (0 or 2+ spaces after //)
    const prefixMatch = trimmed.match(/^\/\/(\s{0}|\s{2,})(─{4,}|={4,})(\s*)$/);
    if (prefixMatch) {
      const sepChar = prefixMatch[2]!.charAt(0);
      const newLine = `// ${prefixMatch[2]}`;
      if (dryRun) {
        results.push(info(filePath, "transform/prefix-normalize",
          `Line ${i + 1}: would normalize separator prefix to standard '// '`));
      } else if (lines[i]!.trim() !== newLine) {
        lines[i] = newLine;
        modified = true;
        results.push(info(filePath, "transform/prefix-normalize",
          `Line ${i + 1}: normalized separator prefix to standard '// ' (${sepChar} separator)`));
      }
    }
  }

  // --- Transform 5: Move #[cfg(test)] from BODY to CLOSING Cv ---
  // Detects the #[cfg(test)] mod tests { ... } block in BODY and moves it
  // to the CLOSING block, wrapped in a Cv zone with proper separators.
  {
    const bodyBlock = findBlockRange(lines, "BODY");
    const closingBlock = findBlockRange(lines, "CLOSING");

    if (bodyBlock && closingBlock) {
      const testRange = findCfgTestInRange(lines, bodyBlock.contentStart, bodyBlock.contentEnd);
      if (testRange) {
        // Include any preceding blank lines/comments that are part of the test section
        let extractStart = testRange.start;
        // Look back for preceding blank or comment lines that belong to this test block
        while (extractStart > bodyBlock.contentStart &&
               (lines[extractStart - 1]!.trim() === "" ||
                lines[extractStart - 1]!.trim().startsWith("// WRONG"))) {
          extractStart--;
        }

        const extractedLines = lines.slice(extractStart, testRange.end + 1);

        // Build Cv zone with separators
        const cvZone = [
          "",
          `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
          "// Cv — Closing Validation",
          `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
          "",
          ...extractedLines.map((l) => l), // preserve original indentation
        ];

        // Find insertion point: after CLOSING header separator, before any existing zones
        const insertIdx = closingBlock.contentStart;

        if (dryRun) {
          results.push(info(filePath, "transform/move-tests",
            `Lines ${extractStart + 1}–${testRange.end + 1}: would move #[cfg(test)] from BODY to CLOSING Cv zone`));
        } else {
          // Remove from BODY (and trailing blank lines)
          let removeEnd = testRange.end + 1;
          while (removeEnd < bodyBlock.contentEnd && lines[removeEnd]!.trim() === "") {
            removeEnd++;
          }
          lines.splice(extractStart, removeEnd - extractStart);
          modified = true;

          // Recalculate insertion point (lines shifted after removal)
          const shift = removeEnd - extractStart;
          const newInsertIdx = insertIdx - shift;

          // Insert Cv zone
          lines.splice(newInsertIdx, 0, ...cvZone);

          results.push(info(filePath, "transform/move-tests",
            `Moved #[cfg(test)] from BODY to CLOSING Cv zone (${extractedLines.length} lines)`));
        }
      }
    }
  }

  // --- Transform 6: Move fn main() from BODY to CLOSING Ce ---
  // Detects fn main() in BODY and moves it to CLOSING Ce zone.
  {
    const bodyBlock = findBlockRange(lines, "BODY");
    const closingBlock = findBlockRange(lines, "CLOSING");

    if (bodyBlock && closingBlock) {
      const mainRange = findFnMainInRange(lines, bodyBlock.contentStart, bodyBlock.contentEnd);
      if (mainRange) {
        let extractStart = mainRange.start;
        // Look back for preceding blank or comment lines
        while (extractStart > bodyBlock.contentStart &&
               (lines[extractStart - 1]!.trim() === "" ||
                lines[extractStart - 1]!.trim().startsWith("// WRONG"))) {
          extractStart--;
        }

        const extractedLines = lines.slice(extractStart, mainRange.end + 1);

        // Build Ce zone with separators
        const ceZone = [
          "",
          `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
          "// Ce — Closing Execution",
          `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
          "",
          ...extractedLines.map((l) => l),
        ];

        // Find where to insert in CLOSING (after Cv if present, before X sections)
        const insertIdx = closingBlock.contentStart;

        if (dryRun) {
          results.push(info(filePath, "transform/move-main",
            `Lines ${extractStart + 1}–${mainRange.end + 1}: would move fn main() from BODY to CLOSING Ce zone`));
        } else {
          let removeEnd = mainRange.end + 1;
          while (removeEnd < bodyBlock.contentEnd && lines[removeEnd]!.trim() === "") {
            removeEnd++;
          }
          lines.splice(extractStart, removeEnd - extractStart);
          modified = true;

          const shift = removeEnd - extractStart;
          const newInsertIdx = insertIdx - shift;

          lines.splice(newInsertIdx, 0, ...ceZone);

          results.push(info(filePath, "transform/move-main",
            `Moved fn main() from BODY to CLOSING Ce zone (${extractedLines.length} lines)`));
        }
      }
    }
  }

  // --- Transform 7: Reorder CLOSING zones ---
  // Ensures code zones (Cv, Ce, Cc) come before documentation (X1-X6),
  // and each tier is in canonical order.
  {
    const closingBlock = findBlockRange(lines, "CLOSING");
    if (closingBlock) {
      const reordered = reorderClosingZones(
        lines.slice(closingBlock.contentStart, closingBlock.contentEnd),
      );
      if (reordered) {
        if (dryRun) {
          results.push(info(filePath, "transform/reorder-closing",
            "CLOSING zones would be reordered to canonical order (Cv → Ce → Cc → X1-X6)"));
        } else {
          lines.splice(
            closingBlock.contentStart,
            closingBlock.contentEnd - closingBlock.contentStart,
            ...reordered,
          );
          modified = true;
          results.push(info(filePath, "transform/reorder-closing",
            "Reordered CLOSING zones to canonical order (Cv → Ce → Cc → X1-X6)"));
        }
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
// Health scoring — container-based atomic action scoring
// ---------------------------------------------------------------------------

/**
 * Compute health score for a Rust file based on its lint results.
 *
 * Maps each lint check to a block and container, creating atomic actions.
 * Checks that don't fire = pass. Checks that fire = fail (with severity).
 * Multi-result rules (like content/metadata-leak) create one fail action
 * per result — more failures = worse score (asymmetric by nature).
 *
 * Cascade logic: if a block is missing, all containers in that block
 * cascade-fail. If PRAGMA/METADATA statics are missing, their field
 * checks cascade-fail.
 *
 * 4-block structure → 6 blocks for scoring:
 *   structural (block presence + separators)
 *   metadata (directives + identity + comment-meta + doc + template/derived)
 *   setup (subsection ordering)
 *   body (subsection ordering)
 *   content (placement — spans multiple structural blocks)
 *   closing (zone ordering + content placement)
 */
async function computeRustHealth(
  filePath: string,
  results: LintResult[],
): Promise<HealthScore> {
  // ── Skip non-structural files ──────────────────────────────────
  // Module files without markers and unreadable files get no score.
  const hasStructuralChecks = results.some((r) =>
    !r.rule.startsWith("structure/") && !r.rule.startsWith("io/"));
  if (!hasStructuralChecks) {
    return computeHealthScore([]);
  }

  // ── File type detection ────────────────────────────────────────
  const basename = filePath.split("/").pop() ?? "";
  const isModuleFile = basename === "mod.rs";
  const isCrateRoot = basename === "lib.rs" || basename === "main.rs";
  let isTemplate = false;
  try {
    const content = await Deno.readTextFile(filePath);
    const firstLines = content.split("\n").slice(0, 15);
    isTemplate = firstLines.some((l) =>
      /^\/\/\s+#!omni\s+template\b/.test(l.trim()));
  } catch { /* best-effort — defaults to non-template */ }

  // ── Build failure index ────────────────────────────────────────
  // rule → LintResult[] (some rules produce multiple results)
  const failuresByRule = new Map<string, LintResult[]>();
  for (const r of results) {
    if (r.rule.startsWith("structure/") || r.rule.startsWith("io/")) continue;
    if (!failuresByRule.has(r.rule)) failuresByRule.set(r.rule, []);
    failuresByRule.get(r.rule)!.push(r);
  }

  // ── Action helpers ─────────────────────────────────────────────

  /** Create atomic actions: 1 pass if no failure, N fails if N results. */
  function acts(check: string, container: string, block: string): AtomicAction[] {
    const failures = failuresByRule.get(check);
    if (failures && failures.length > 0) {
      return failures.map((f) => ({
        check, container, block,
        passed: false,
        severity: f.severity,
        reason: f.message,
      }));
    }
    return [{ check, container, block, passed: true }];
  }

  /** Cascade all passed actions to failed with error severity. */
  function cascade(groups: AtomicAction[][], reason: string): void {
    for (const group of groups) {
      for (const a of group) {
        if (a.passed) {
          a.passed = false;
          a.severity = "error";
          a.reason = reason;
        }
      }
    }
  }

  // ── STRUCTURAL block ──────────────────────────────────────────
  const blockActions: AtomicAction[] = [];
  for (const name of BLOCKS) {
    blockActions.push(...acts(`block/${name}`, "blocks", "structural"));
    blockActions.push(...acts(`block/end-${name}`, "blocks", "structural"));
  }
  blockActions.push(...acts("block/order", "blocks", "structural"));

  const sepActions: AtomicAction[] = [
    ...acts("style/eq-separator-width", "separators", "structural"),
    ...acts("style/eq-separator-standard", "separators", "structural"),
    ...acts("style/box-separator-width", "separators", "structural"),
    ...acts("style/box-separator-standard", "separators", "structural"),
    ...acts("style/dash-separator-width", "separators", "structural"),
  ];

  // ── METADATA block ────────────────────────────────────────────
  const directiveActions: AtomicAction[] = [];
  if (isTemplate) {
    directiveActions.push(...acts("directive/meta.key", "directives", "metadata"));
    directiveActions.push(...acts("directive/template-format", "directives", "metadata"));
  } else if (!isModuleFile) {
    for (const d of REQUIRED_DIRECTIVES) {
      directiveActions.push(...acts(`directive/${d}`, "directives", "metadata"));
    }
    for (const d of RECOMMENDED_DIRECTIVES) {
      directiveActions.push(...acts(`directive/${d}`, "directives", "metadata"));
    }
    directiveActions.push(...acts("directive/code-format", "directives", "metadata"));
  }

  const identityActions: AtomicAction[] = [];
  if (!isModuleFile) {
    identityActions.push(...acts("identity/pragma-static", "identity", "metadata"));
    identityActions.push(...acts("identity/metadata-static", "identity", "metadata"));

    // PRAGMA field checks (I1-I4)
    for (const [section, req] of Object.entries(PRAGMA_FIELD_REQUIREMENTS)) {
      for (const field of [...req.required, ...req.defined]) {
        identityActions.push(
          ...acts(`identity/PRAGMA/${section}.${field}`, "identity", "metadata"));
      }
    }
    identityActions.push(
      ...acts("identity/PRAGMA/I2.subtype-value", "identity", "metadata"));

    // METADATA field checks (C1-C7)
    for (const [section, req] of Object.entries(METADATA_FIELD_REQUIREMENTS)) {
      for (const field of [...req.required, ...req.defined]) {
        identityActions.push(
          ...acts(`identity/METADATA/${section}.${field}`, "identity", "metadata"));
      }
    }

    identityActions.push(...acts("identity/pragma-parse", "identity", "metadata"));
    identityActions.push(...acts("identity/metadata-parse", "identity", "metadata"));
    identityActions.push(...acts("identity/register", "identity", "metadata"));
  }

  const commentActions: AtomicAction[] = [];
  if (!isTemplate) {
    commentActions.push(...acts("comment-meta/key", "comment-meta", "metadata"));
    commentActions.push(...acts("comment-meta/purpose", "comment-meta", "metadata"));
    commentActions.push(...acts("comment-meta/biblical", "comment-meta", "metadata"));
    commentActions.push(...acts("comment-meta/version", "comment-meta", "metadata"));
  }

  const docActions: AtomicAction[] = [];
  if (isTemplate) {
    docActions.push(...acts("doc/template", "doc", "metadata"));
  } else if (isCrateRoot) {
    docActions.push(...acts("doc/crate-docs", "doc", "metadata"));
  } else if (isModuleFile) {
    docActions.push(...acts("doc/module-docs", "doc", "metadata"));
  }

  const templateDerivedActions: AtomicAction[] = [];
  if (isTemplate) {
    templateDerivedActions.push(...acts("template/pragma-static", "template", "metadata"));
    templateDerivedActions.push(...acts("template/metadata-static", "template", "metadata"));
    templateDerivedActions.push(...acts("template/placeholders", "template", "metadata"));
  } else if (isCrateRoot) {
    templateDerivedActions.push(
      ...acts("derived/template-directive", "derived", "metadata"));
  }

  // ── SETUP block ───────────────────────────────────────────────
  const setupActions: AtomicAction[] = [];
  if (!isTemplate) {
    setupActions.push(...acts("setup/subsection-order", "ordering", "setup"));
  }

  // ── BODY block ────────────────────────────────────────────────
  const bodyOrderActions: AtomicAction[] = [];
  if (!isTemplate) {
    bodyOrderActions.push(...acts("body/subsection-order", "ordering", "body"));
  }

  // ── CONTENT placement ─────────────────────────────────────────
  const contentActions: AtomicAction[] = [];
  if (!isTemplate) {
    contentActions.push(...acts("content/metadata-leak", "placement", "content"));
    contentActions.push(...acts("content/block-placement", "placement", "content"));
    contentActions.push(...acts("content/subsection-placement", "placement", "content"));
  }

  // ── CLOSING block ─────────────────────────────────────────────
  const closingOrderActions: AtomicAction[] = [];
  if (!isTemplate) {
    closingOrderActions.push(...acts("closing/zone-order", "ordering", "closing"));
    closingOrderActions.push(...acts("closing/code-zone-order", "ordering", "closing"));
    closingOrderActions.push(...acts("closing/doc-section-order", "ordering", "closing"));
  }

  const closingPlaceActions: AtomicAction[] = [];
  if (!isTemplate) {
    closingPlaceActions.push(...acts("closing/test-placement", "placement", "closing"));
    closingPlaceActions.push(...acts("closing/main-placement", "placement", "closing"));
  }

  // ── CASCADE: missing blocks → all children fail ───────────────
  const blockMissing = (name: string) => failuresByRule.has(`block/${name}`);

  if (blockMissing("METADATA")) {
    cascade(
      [directiveActions, identityActions, commentActions, docActions, templateDerivedActions],
      "METADATA block missing — all metadata checks fail",
    );
  }
  if (blockMissing("SETUP")) {
    cascade([setupActions], "SETUP block missing — all setup checks fail");
  }
  if (blockMissing("BODY")) {
    cascade([bodyOrderActions], "BODY block missing — all body checks fail");
  }
  if (blockMissing("CLOSING")) {
    cascade(
      [closingOrderActions, closingPlaceActions],
      "CLOSING block missing — all closing checks fail",
    );
  }

  // Content placement depends on having ANY structural blocks
  if (blockMissing("METADATA") && blockMissing("SETUP") && blockMissing("BODY")) {
    cascade([contentActions], "No structural blocks — content placement cannot be checked");
  }

  // ── SUB-CASCADE: missing statics → field checks fail ──────────
  if (failuresByRule.has("identity/pragma-static")) {
    for (const a of identityActions) {
      if (a.passed && a.check.startsWith("identity/PRAGMA/")) {
        a.passed = false;
        a.severity = "warn";
        a.reason = "PRAGMA static missing — field check cannot run";
      }
    }
  }
  if (failuresByRule.has("identity/metadata-static")) {
    for (const a of identityActions) {
      if (a.passed && a.check.startsWith("identity/METADATA/")) {
        a.passed = false;
        a.severity = "warn";
        a.reason = "METADATA static missing — field check cannot run";
      }
    }
  }

  // ── Collect all actions ───────────────────────────────────────
  const allActions = [
    ...blockActions, ...sepActions,
    ...directiveActions, ...identityActions, ...commentActions,
    ...docActions, ...templateDerivedActions,
    ...setupActions, ...bodyOrderActions,
    ...contentActions,
    ...closingOrderActions, ...closingPlaceActions,
  ];

  if (allActions.length === 0) {
    return computeHealthScore([]);
  }

  // ── Group by container and compute scores ─────────────────────
  const containerMap = new Map<string, AtomicAction[]>();
  for (const a of allActions) {
    const key = `${a.block}/${a.container}`;
    if (!containerMap.has(key)) containerMap.set(key, []);
    containerMap.get(key)!.push(a);
  }

  const containerScores: ContainerScore[] = [];
  for (const [, actionList] of containerMap) {
    if (actionList.length === 0) continue;
    const first = actionList[0]!;
    containerScores.push(
      computeContainerScore(first.container, first.block, actionList));
  }

  // Group container scores by block
  const blockMap = new Map<string, ContainerScore[]>();
  for (const cs of containerScores) {
    if (!blockMap.has(cs.block)) blockMap.set(cs.block, []);
    blockMap.get(cs.block)!.push(cs);
  }

  const blockScores: BlockScore[] = [];
  for (const [block, containers] of blockMap) {
    blockScores.push(computeBlockScore(block, containers));
  }

  return computeHealthScore(blockScores);
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// Registration — plug into the registry
// ---------------------------------------------------------------------------

const rustHandler: FormatHandler = {
  name: "rust",
  description: "Rust 4-block alignment (//omni:, METADATA → SETUP → BODY → CLOSING, identity statics)",
  extensions: [".rs"],
  maxDepth: 10,
  lint: lintRustFile,
  computeHealth: computeRustHealth,
  transform: transformRustFile,
};

registerFormat(rustHandler);

export default rustHandler;

//
// Rust files express structure through comments — //omni: pragma directives,
// block boundary markers, and //! doc comments. This handler reads those
// markers and verifies the skeleton is sound before anyone fills in the flesh.
//
// Two identity patterns: slice-based (&[(&str, &str)]) for lightweight crates,
// LazyLock<BTreeMap> for crates needing runtime-queryable identity. Both valid.
//
// b-02.50: 14 checks + I/C field validation + 10-subsection SETUP + 7 transforms.
//   Linter checks:
//   - Directive validation (//omni:key, //omni:code, //omni:version)
//   - Block structure (METADATA → SETUP → BODY → CLOSING + END markers)
//   - Doc comments (//! crate/module level)
//   - Pragma/Metadata statics with I/C field validation (I1-I4, C1-C7)
//   - Comment metadata (Key:, Purpose:, Biblical: — secondary to statics)
//   - Separator consistency (= 76, ─ 74)
//   - Template vs derived: usable schema pattern (no placeholders)
//   - SETUP subsection order (10-subsection dependency chain)
//   - Identity registration detection
//   - Directive format validation
//   - Content placement (SETUP/BODY boundary enforcement)
//   - BODY subsection ordering (numeric ascending)
//   - CLOSING zone ordering (code zones before docs, canonical order)
//   - CLOSING content placement (#[cfg(test)] → Cv, fn main() → Ce)
//   Transformer:
//   - Cosmetic: separator widths, ASCII→Unicode, prefix normalization
//   - Structural: #[cfg(test)] → Cv, fn main() → Ce, CLOSING zone reorder
//
// "Let all things be done decently and in order." — 1 Corinthians 14:40
// ============================================================================
