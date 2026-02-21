// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/handlers/rust.ts
// key:     B-tov-cws-struct-lib-handlers-rust
// title:   CWS Struct — Rust Format Handler
// type:    Code (Library)
// version: b-02.00
// created: 2026-02-14
// updated: 2026-02-18
// authors: Nova Dawn (CPI-SI)
// purpose: Rust 4-block alignment linter + transformer with I/C field validation.
//          Validates Rust source files for:
//          - //omni: directives (key, version, pragma, metadata)
//          - 4-block structure (METADATA → SETUP → BODY → CLOSING)
//          - PRAGMA static I/C fields (I1-I4 Identity sections)
//          - METADATA static I/C fields (C1-C7 Context sections)
//          - Required/defined field validation per I/C section
//          - Field content validation (pattern, enum, non-empty, path-like)
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
  FormConstraints, LintPolicy,
} from "../foundation/mod.ts";
import {
  error, warn, info, policySeverity, getGlobalPolicy,
  computeContainerScore, computeBlockScore, computeHealthScore,
  loadCodeRules, loadFormConstraints,
} from "../foundation/mod.ts";
import { registerFormat } from "../engine/mod.ts";

// Shared 4-block types, constants, and functions
import type { BlockPosition, DirectiveInfo, SubsectionRange, IdentityField, LanguageAdapter } from "./shared/mod.ts";
import type { FieldContentRule } from "./shared/mod.ts";
import {
  BLOCKS, REQUIRED_DIRECTIVES, RECOMMENDED_DIRECTIVES,
  PRAGMA_CONTENT_RULES, METADATA_CONTENT_RULES,
  BLOCK_SEPARATOR_WIDTH, SUBSECTION_SEPARATOR_WIDTH,
  BODY_SUBSECTION_PATTERN,
  findBlocks, getBlockLines, findBlockRange,
  getSubsectionRanges as _getSubsectionRanges,
  checkSeparatorConsistency, checkClosingZoneOrder,
  checkClosingRequiredZones, checkClosingZoneContent,
  checkClosingX6TemplateOnly, checkClosingDocFieldContent,
  checkSetupSubsectionOrder as _sharedCheckSetupOrder,
  checkBodySubsectionOrder as _sharedCheckBodyOrder,
  checkScalingSignals as _sharedCheckScaling,
  checkRequiredSetupSubsections as _sharedCheckRequiredSetup,
  checkRequiredBodySubsections as _sharedCheckRequiredBody,
  checkSubtypeEmphasis as _sharedCheckEmphasis,
  validateICFieldContent,
  validateTemplateVsDerived, validateSubtypeConsistency, validateFormatConsistency,
} from "./shared/mod.ts";

// Re-export for tests and schema alignment verification
export { PRAGMA_CONTENT_RULES, METADATA_CONTENT_RULES };
export { validateICFieldContent };
export { SETUP_SUBSECTIONS, BODY_CANONICAL_SUBSECTIONS };

// ---------------------------------------------------------------------------
// Constants — Rust-specific (schema-driven + lazy init)
// ---------------------------------------------------------------------------
//
// LAZY INIT: Schema loads on first handler use (lint/transform/health), not
// at import time. This means Rust schema failure only affects Rust — Go and
// TOML continue working. Module registration (registerFormat) is synchronous
// and schema-free.
//

import type {
  Code4BlockRules,
  SchemaFillContent,
  FormSectionConstraint,
  FormReservedSection,
} from "../foundation/mod.ts";

/** Lazily-loaded Rust rules. Populated by ensureRustRules(). */
let _rustRules: Code4BlockRules | null = null;

// Schema-driven constants — populated lazily by ensureRustRules().
// Declared as `let` instead of `const` because they're filled on first use.
// deno-lint-ignore prefer-const
let PRAGMA_FIELD_REQUIREMENTS: Record<string, import("../foundation/code-schema.ts").SchemaFieldRequirement> = {};
// deno-lint-ignore prefer-const
let METADATA_FIELD_REQUIREMENTS: Record<string, import("../foundation/code-schema.ts").SchemaFieldRequirement> = {};

// Re-export schema-driven requirements for tests and schema alignment verification
export { PRAGMA_FIELD_REQUIREMENTS, METADATA_FIELD_REQUIREMENTS };

// Policy flows through foundation's global state — see getGlobalPolicy().

// deno-lint-ignore prefer-const
let SETUP_SUBSECTIONS: import("./shared/types.ts").SubsectionDef[] = [];
// deno-lint-ignore prefer-const
let BODY_CANONICAL_SUBSECTIONS: Record<string, readonly string[]> = {};

/**
 * Ensure Rust rules are loaded. Lazy init — first call loads from pipeline,
 * subsequent calls are no-ops. All handler entry points (lint, transform,
 * health) call this before doing anything.
 *
 * Fault isolation: if Rust schema is missing, only Rust fails. Go and TOML
 * handlers are unaffected because their schemas load independently.
 */
export async function ensureRustRules(): Promise<void> {
  if (_rustRules) return;
  _rustRules = await loadCodeRules("rust");

  // Populate module-scope constants from loaded rules
  PRAGMA_FIELD_REQUIREMENTS = _rustRules!.pragmaFieldRequirements;
  METADATA_FIELD_REQUIREMENTS = _rustRules!.metadataFieldRequirements;
  BODY_CANONICAL_SUBSECTIONS = _rustRules!.bodySubsections;
  SETUP_SUBSECTIONS = _rustRules!.setupSubsections;

  // Placement maps — content kind → block/subsection
  BLOCK_PLACEMENT = _rustRules!.placementMaps.blockPlacement;
  SUBSECTION_PLACEMENT = _rustRules!.placementMaps.subsectionPlacement;
  METADATA_FORBIDDEN = new Set(_rustRules!.placementMaps.metadataForbidden);
}

/** Known //omni:code directive patterns for Rust (matches schema subtypes). */
const KNOWN_CODE_DIRECTIVES = [
  "--rust -library",
  "--rust -executable",
  "--rust -module",
  "--rust -demo-test",
  "--rust -bare-bone",
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
  | "use_decl" | "reexport_decl" | "mod_decl" | "const_decl" | "static_decl" | "type_alias"
  | "struct_decl" | "enum_decl" | "trait_decl" | "macro_decl"
  | "fn_decl" | "impl_block"
  | "cfg_attr" | "test_attr" | "attr"
  | "comment" | "blank" | "other";

/**
 * Content kind → block/subsection placement maps.
 *
 * Schema-driven: loaded from rust-4block-schema.jsonc → SETUP.content_kind_mapping.
 * Previously hardcoded — now the schema is the single source of truth.
 *
 * BLOCK_PLACEMENT: kind → "SETUP" or "BODY". Missing = can appear anywhere.
 * SUBSECTION_PLACEMENT: kind → subsection tag within SETUP.
 * METADATA_FORBIDDEN: kinds that are never valid in the METADATA block.
 */
// deno-lint-ignore prefer-const
let BLOCK_PLACEMENT: Record<string, string> = {};
// deno-lint-ignore prefer-const
let SUBSECTION_PLACEMENT: Record<string, string> = {};
// deno-lint-ignore prefer-const
let METADATA_FORBIDDEN: Set<string> = new Set();

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
  subtype: string | null;        // "library" | "executable" | "module" | "demo-test" | "bare-bone" | null
  formConstraints: FormConstraints | null;  // loaded from form schema when subtype is known
  policy: LintPolicy;  // ternary threshold for form checks
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
    if (/^\/\/\s+(METADATA|SETUP|BODY|CLOSING)(\s+BLOCK\s+\[\1\])?\s*$/.test(trimmed)) break;
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

// (getBlockLines imported from shared module)

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

  // Re-export detection: `pub use` is a re-export (belongs in Modules),
  // distinct from private `use` imports (belongs in Imports).
  // Check BEFORE stripping visibility so we can distinguish them.
  if (/^pub\s*(?:\((?:crate|super|in\s+\S+)\)\s+)?use\s/.test(trimmed)
      || /^pub\s+use\{/.test(trimmed)) return "reexport_decl";

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
    /^\/\/\s+(METADATA|SETUP|BODY|CLOSING)(\s+BLOCK\s+\[\1\])?\s*$/.test(l.trim())
  );

  // Detect subtype from directives, then PRAGMA I2.subtype if not found.
  // Sources (priority order):
  //   1. //omni:code --rust -<subtype>
  //   2. #!omni template --rust -<subtype>
  //   3. PRAGMA I2.subtype field
  let subtype: string | null = null;
  const KNOWN_SUBTYPES = Object.keys(_rustRules!.subtypeDefinitions);

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
    // Bare-bone always loads — it's the format level, not a variant.
    // Every 4-block Rust file gets form constraints. No subtype = bare-bone floor.
    formConstraints: await loadFormConstraints("rust", subtype || "bare-bone"),
    // Ternary policy — from foundation global state set by CLI.
    policy: getGlobalPolicy(),
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

  // Module files without blocks AND without pragma are plain Rust —
  // their identity lives in lib.rs. But if a pragma is present, the file
  // is claiming 4-block structure and must deliver the blocks.
  if (ctx.isModuleFile && !ctx.hasAnyBlock && !ctx.hasAnyOmni) {
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
  const expectations = _rustRules!.docCommentExpectations;

  // Schema-driven severity: map string → result constructor
  const emit = (sev: string, f: string, rule: string, msg: string, opts?: { line?: number }) =>
    sev === "error" ? error(f, rule, msg, opts)
    : sev === "warn"  ? warn(f, rule, msg, opts)
    :                    info(f, rule, msg, opts);

  // Doc comments (//!) belong in the METADATA area — before END METADATA.
  // In our templates, //! appears inside the METADATA block (after the block
  // marker but before END METADATA) because pragma directives come first.
  // A //! in BODY or CLOSING should NOT satisfy the crate-level doc check.
  //
  // BlockPosition.line and endLine are 1-based; slice is 0-based.
  const metadataBlock = ctx.blocks.find((b) => b.name === "METADATA");
  const scanEnd = metadataBlock && metadataBlock.endLine > 0
    ? metadataBlock.endLine       // 1-based endLine → scan through END METADATA
    : ctx.blocks.length > 0
      ? ctx.blocks[0]!.line - 1   // fallback: before first block (1-based)
      : ctx.lines.length;         // no blocks → scan entire file

  const hasDocComments = ctx.lines
    .slice(0, scanEnd)
    .some((l: string) => l.trim().startsWith("//!"));

  // Templates: check for //! placeholder
  if (ctx.isTemplate) {
    if (!hasDocComments) {
      results.push(info(file, "doc/template", "Template has no //! doc comment section"));
    }
    return results;
  }

  // Schema keys: "crate_root_docs" (severity from crate_root_severity)
  //              "module_docs" (severity from module_severity)
  const crateExp = expectations["crate_root_docs"];
  const moduleExp = expectations["module_docs"];

  if (ctx.isCrateRoot && !hasDocComments) {
    const sev = crateExp?.severity ?? "warn";
    results.push(emit(sev, file, "doc/crate-docs",
      "Missing //! crate-level doc comments — recommended for lib.rs/main.rs"));
  } else if (ctx.isModuleFile && !hasDocComments) {
    const sev = moduleExp?.severity ?? "info";
    results.push(emit(sev, file, "doc/module-docs",
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

  // Module files without their own identity statics: skip.
  // Identity lives in the crate root. BUT if a non-root file declares
  // PRAGMA/METADATA statics, they deserve validation.
  if (ctx.isModuleFile && !hasPragma && !hasMetadata) return results;

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

      // Subtype validation: if I2.subtype is present, check it's a known value (from schema)
      const subtypeField = pragmaFields.find((f) => f.section === "I2" && f.field === "subtype");
      if (subtypeField) {
        const knownSubtypes = Object.keys(_rustRules!.subtypeDefinitions);
        if (!knownSubtypes.includes(subtypeField.value)) {
          results.push(warn(file, "identity/PRAGMA/I2.subtype-value",
            `Unknown I2.subtype "${subtypeField.value}" — known values: ${knownSubtypes.join(", ")}`));
        }
      }

      // Content validation — are field VALUES correct?
      results.push(...validateICFieldContent(file, pragmaFields, PRAGMA_CONTENT_RULES, "PRAGMA"));
    } else if (!hasDelegatedIdentity) {
      results.push(info(file, "identity/pragma-parse",
        "PRAGMA static found but no I/C fields could be parsed"));
    }
  }

  if (hasMetadata) {
    const metadataFields = parseStaticFields(ctx.lines, "METADATA");
    if (metadataFields.length > 0) {
      results.push(...validateICFields(file, metadataFields, METADATA_FIELD_REQUIREMENTS, "METADATA"));
      results.push(...validateICFieldContent(file, metadataFields, METADATA_CONTENT_RULES, "METADATA"));
    } else {
      results.push(info(file, "identity/metadata-parse",
        "METADATA static found but no I/C fields could be parsed"));
    }
  }

  // ── Content-aware checks — template/derived, subtype, format ──────
  if (hasPragma && hasMetadata) {
    const pragmaFields = parseStaticFields(ctx.lines, "PRAGMA");
    const metadataFields = parseStaticFields(ctx.lines, "METADATA");
    if (pragmaFields.length > 0 && metadataFields.length > 0) {
      results.push(...validateTemplateVsDerived(
        file, pragmaFields, metadataFields, ctx.isTemplate,
        { pragma: "PRAGMA", metadata: "METADATA" },
      ));
      results.push(...validateSubtypeConsistency(
        file, pragmaFields, ctx.subtype, "PRAGMA",
      ));
      results.push(...validateFormatConsistency(
        file, pragmaFields, "rust", "PRAGMA",
      ));
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
  const setupLines = getBlockLines(ctx.lines, ctx.blocks, "SETUP");
  return _sharedCheckSetupOrder(
    setupLines, SETUP_SUBSECTIONS, ctx.blocks, ctx.filePath, ctx.isTemplate,
  );
}

// ---------------------------------------------------------------------------
// Check 12: BODY subsection order
// ---------------------------------------------------------------------------

function checkBodySubsectionOrder(ctx: RustFileContext): LintResult[] {
  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  return _sharedCheckBodyOrder(
    bodyLines, ctx.blocks, ctx.filePath, ctx.isTemplate, false, // no legacy pattern for Rust
  );
}

// Check 8a: SETUP required subsections for detected subtype.
function checkRequiredSetupSubsections(ctx: RustFileContext): LintResult[] {
  const setupLines = getBlockLines(ctx.lines, ctx.blocks, "SETUP");
  return _sharedCheckRequiredSetup(
    setupLines, SETUP_SUBSECTIONS, _rustRules!.setupData, ctx.subtype,
    ctx.blocks, ctx.filePath, ctx.isTemplate,
  );
}

// Check 8b: BODY required subsections for detected subtype.
function checkRequiredBodySubsections(ctx: RustFileContext): LintResult[] {
  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  const bodySubtype = ctx.subtype ? _rustRules!.bodyData[ctx.subtype] : undefined;
  return _sharedCheckRequiredBody(
    bodyLines, bodySubtype, ctx.subtype,
    ctx.blocks, ctx.filePath, ctx.isTemplate, false, // no legacy for Rust
  );
}

// Check 8c: SETUP subtype emphasis — heavy subsections should have content.
function checkSubtypeEmphasis(ctx: RustFileContext): LintResult[] {
  const emphasis = ctx.subtype
    ? _rustRules!.subtypeEmphasis.setup[ctx.subtype]
    : undefined;
  return _sharedCheckEmphasis(
    ctx.lines, ctx.blocks, SETUP_SUBSECTIONS, emphasis,
    ctx.subtype, ctx.filePath, ctx.isTemplate,
  );
}

// CLOSING checks — all use schema-driven closingData from code-schema.ts
// checkClosingZoneOrder, checkClosingRequiredZones, checkClosingZoneContent,
// checkClosingX6TemplateOnly, checkClosingDocFieldContent — imported from shared

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

  // Derived files MUST have // #!omni code shebang on line 1.
  // The shebang is the company-grade identifier — //omni:code alone is
  // the old format and insufficient for production-grade files.
  // Base severity: warn (balanced default). Strict promotes to error.
  if (!ctx.isTemplate && ctx.hasAnyOmni) {
    const hasShebang = ctx.directives.has("#!omni:code");
    const hasInlineCode = ctx.directives.has("//omni:code");
    const sev = policySeverity("warn", ctx.policy);
    const emit = sev === "error" ? error : sev === "warn" ? warn : info;

    if (!hasShebang && hasInlineCode) {
      results.push(
        emit(file, "directive/missing-shebang",
          `Missing shebang — derived files require "// #!omni code <args>" on line 1 (found //omni:code only)`,
          { line: ctx.directives.get("//omni:code")!.line }),
      );
    } else if (!hasShebang && !hasInlineCode) {
      // Has //omni: directives (key, version) but no code declaration at all
      results.push(
        emit(file, "directive/missing-code-declaration",
          `No code declaration — add "// #!omni code --rust -<form>" as line 1`,
        ),
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
        // Exempt identity statics — PRAGMA and METADATA belong in METADATA block
        if (decl.kind === "static_decl") {
          const line = metaLines[decl.lineIdx]?.trim() ?? "";
          if (/^pub\s+static\s+(PRAGMA|METADATA)\s*:/.test(line)) {
            continue;
          }
        }
        // Exempt Metadata Imports — use declarations needed by identity
        // statics belong in the Metadata Imports section. This mirrors
        // the Go handler's exemption for import_decl.
        if (decl.kind === "use_decl") {
          continue;
        }
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

          // Build auto-move suggestion: remove from current, insert at target
          const targetRange = subsections.find((s) => s.tag === expectedSub);
          const targetInsertLine = targetRange
            ? setupStart + 1 + targetRange.endIdx  // insert at end of target subsection
            : 0;

          // Determine the extent of the declaration (single or multi-line block)
          let declEndIdx = decl.lineIdx;
          for (let d = decl.lineIdx + 1; d < subLines.length; d++) {
            const t = subLines[d]!.trim();
            // Stop at blank line, separator, or next subsection header
            if (t === "" || /^\/\/\s*[─=\-]{4,}\s*$/.test(t) || /^\/\/\s+\d+\.?\s+/.test(t)) break;
            // Stop at closing brace of block (struct/enum/impl end with "}")
            if (t === "}") { declEndIdx = d; break; }
            declEndIdx = d;
          }

          const removeStart = fileLine;
          const removeEnd = setupStart + 1 + sub.startIdx + declEndIdx;
          const declContent = subLines.slice(decl.lineIdx, declEndIdx + 1);

          results.push(
            info(file, "content/subsection-placement",
              `${decl.kind} in ${sub.tag} subsection (line ${fileLine}) — expected in ${expectedSub}`,
              {
                line: fileLine,
                fix: targetRange ? {
                  description: `Move ${decl.kind} from ${sub.tag} to ${expectedSub}`,
                  actions: [
                    { type: "remove", startLine: removeStart, endLine: removeEnd },
                    { type: "insert", afterLine: targetInsertLine, content: declContent },
                  ],
                } : undefined,
              }),
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
// Check 15: SETUP header documentation (Rust //! module-level doc)
// ---------------------------------------------------------------------------

/**
 * Check for header documentation before the first SETUP subsection.
 *
 * Rust uses //! for module-level documentation. Good practice is to have
 * at least 3 substantive comment lines before the first subsection marker,
 * providing a section-order overview or module-level context.
 *
 * Mirrors Go's checkSetupHeaderDoc but checks for //! doc comments
 * in addition to regular // comments.
 */
function checkSetupHeaderDoc(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) return results;

  const setupLines = getBlockLines(ctx.lines, ctx.blocks, "SETUP");
  if (setupLines.length === 0) return results;

  // Find the first subsection marker
  let firstSubIdx = -1;
  for (let i = 0; i < setupLines.length; i++) {
    const trimmed = setupLines[i]!.trim();
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;
    for (const sub of SETUP_SUBSECTIONS) {
      if (sub.pattern.test(trimmed)) {
        firstSubIdx = i;
        break;
      }
    }
    if (firstSubIdx >= 0) break;
  }

  // No subsections found — skip (subsection-order check handles that)
  if (firstSubIdx < 0) return results;

  // Count substantive comment lines before the first subsection
  // Rust //! doc comments and regular // comments both count
  let docLines = 0;
  for (let i = 0; i < firstSubIdx; i++) {
    const trimmed = setupLines[i]!.trim();
    // Skip blanks, separator lines, and empty comments
    if (trimmed === "" || /^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;
    if (/^\/\/[!/]?\s*$/.test(trimmed)) continue; // empty comment (// or //! or ///)
    docLines++;
  }

  if (docLines < 3) {
    const setupBlock = ctx.blocks.find((b) => b.name === "SETUP");
    results.push(
      info(file, "setup/header-doc",
        `SETUP block has ${docLines} header documentation line(s) before first subsection — consider adding section-order overview (3+ lines)`,
        { line: setupBlock?.line ?? 0 }),
    );
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 16: BODY subtype-specific subsection names
// ---------------------------------------------------------------------------

/**
 * Check that BODY subsection names match the file's declared subtype.
 *
 * Schema-driven: reads canonical subsection names from _rustRules!.bodySubsections.
 * Only checks if the file has a known subtype and BODY has numbered subsections.
 *
 * Uses case-insensitive comparison. Rust uses only BODY_SUBSECTION_PATTERN
 * (no legacy §N format).
 */
function checkBodySubtypeContent(ctx: RustFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) return results;
  if (!ctx.subtype) return results;

  const canonical = BODY_CANONICAL_SUBSECTIONS[ctx.subtype];
  if (!canonical) return results;

  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  if (bodyLines.length === 0) return results;

  // Collect subsection names from BODY (Rust: no legacy format)
  const found: Array<{ num: number; name: string; lineIdx: number }> = [];
  for (let i = 0; i < bodyLines.length; i++) {
    const trimmed = bodyLines[i]!.trim();
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;

    const match = BODY_SUBSECTION_PATTERN.exec(trimmed);
    if (match) {
      const num = parseInt(match[1]!, 10);
      const name = match[2]!.trim();
      if (!found.some((f) => f.num === num)) {
        found.push({ num, name, lineIdx: i });
      }
    }
  }

  if (found.length === 0) return results;

  // Compare found names against canonical (case-insensitive)
  const foundNames = found.map((f) => f.name.toLowerCase());

  const missing = canonical.filter((c) =>
    !foundNames.some((f) => f.includes(c.toLowerCase())));

  if (missing.length > 0) {
    // Filter out sections acknowledged in Reserved Omission.
    // Two RO formats:
    //   Reserved:  "//   Modules       — Submodule declarations belong in..."
    //   Available: "//   Identity Access, Trait Implementations, Free Functions"
    // Only scan within the RO section (between "Reserved Omission" header and
    // the next block boundary) to avoid false positives from regular comments.
    const roAcknowledged = new Set<string>();
    let inRO = false;
    for (const line of bodyLines) {
      const trimmed = line.trim();
      if (/^\/\/\s+Reserved Omission\s*$/.test(trimmed)) { inRO = true; continue; }
      if (inRO && /^\/\/\s*={4,}/.test(trimmed)) { inRO = false; continue; }
      if (!inRO || !trimmed.startsWith("//")) continue;

      // Reserved-style: "//   Name — reason" or "//   Name, Other — reason"
      const reservedMatch = /^\/\/\s{2,}(.+?)(?:\s+[—\-]\s+)/.exec(trimmed);
      if (reservedMatch) {
        for (const tag of reservedMatch[1]!.split(",").map((t) => t.trim().toLowerCase())) {
          if (tag) roAcknowledged.add(tag);
        }
        continue;
      }
      // Available-style: "//   Name, Name, Name" (comma-separated, no reason)
      const availMatch = /^\/\/\s{2,}([A-Z][\w\s&,]+)$/.exec(trimmed);
      if (availMatch) {
        for (const tag of availMatch[1]!.split(",").map((t) => t.trim().toLowerCase())) {
          if (tag) roAcknowledged.add(tag);
        }
      }
    }

    const trulyMissing = missing.filter((c) =>
      !roAcknowledged.has(c.toLowerCase()));

    if (trulyMissing.length > 0) {
      const bodyBlock = ctx.blocks.find((b) => b.name === "BODY");
      results.push(
        info(file, "body/subtype-subsections",
          `BODY for ${ctx.subtype} subtype missing canonical subsection(s): ${trulyMissing.join(", ")} — expected: ${canonical.join(", ")}`,
          { line: bodyBlock?.line ?? 0 }),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 17: Scaling signals — block size thresholds
// ---------------------------------------------------------------------------

/**
 * Check block line counts against scaling thresholds.
 *
 * SETUP > 200 lines or BODY > 500 lines indicates the file is doing
 * too much. Informational — not errors, but signals for future refactoring.
 */
function checkScalingSignals(ctx: RustFileContext): LintResult[] {
  return _sharedCheckScaling(ctx.lines, ctx.blocks, ctx.filePath, ctx.isTemplate);
}

// ---------------------------------------------------------------------------
// Form-aware checks — schema-driven validation from form constraint schemas
// ---------------------------------------------------------------------------

/**
 * Check A: Required sections present for this form.
 *
 * For each container (SETUP, BODY, CLOSING): iterate formConstraints.can
 * entries where status == "REQUIRED". Check if that section tag is present
 * in the file's actual subsections.
 */
function checkFormRequiredSections(ctx: RustFileContext): LintResult[] {
  if (!ctx.formConstraints || ctx.isTemplate) return [];

  const results: LintResult[] = [];
  const file = ctx.filePath;

  for (const containerName of ["SETUP", "BODY", "CLOSING"] as const) {
    const container = ctx.formConstraints[containerName];
    if (!container) continue;

    const blockLines = getBlockLines(ctx.lines, ctx.blocks, containerName);
    const blockText = blockLines.join("\n");

    for (const section of container.can) {
      if (section.status !== "REQUIRED") continue;

      // Check if this section tag appears in the block content.
      // Tags are PascalCase (e.g. "FreeFunctions") but rendered headers may use
      // spaces ("Free Functions") or numbered form ("7. Free Functions").
      // Build a pattern that matches all three variants.
      const spacedTag = section.tag.replace(/([a-z])([A-Z])/g, "$1\\s*$2");
      const tagPattern = new RegExp(
        `//\\s*[-─═]+\\s*(?:\\d+\\.\\s*)?${spacedTag}|` +
        `//\\s+(?:§\\d+\\s+)?(?:\\d+\\.\\s*)?${spacedTag}`,
        "i"
      );

      if (!tagPattern.test(blockText)) {
        // Before emitting, check if section is acknowledged in Reserved Omission.
        // RO uses human-readable names ("Error Types") vs PascalCase tags ("ErrorTypes").
        const spacedName = section.tag.replace(/([a-z])([A-Z])/g, "$1 $2");
        let inRO = false;
        let acknowledged = false;

        for (const line of blockLines) {
          const t = line.trim();
          if (/^\/\/\s+Reserved Omission\s*$/.test(t)) { inRO = true; continue; }
          if (inRO && /^\/\/\s*={4,}/.test(t)) { inRO = false; continue; }
          if (inRO && t.startsWith("//") && (
            t.includes(spacedName) || t.includes(section.tag)
          )) { acknowledged = true; break; }
        }

        if (!acknowledged) {
          const sev = policySeverity("warn", ctx.policy);
          const emit = sev === "error" ? error : sev === "warn" ? warn : info;
          results.push(emit(file, `form/required-section-missing`,
            `${ctx.formConstraints.name} form: ${containerName} requires "${section.tag}" (position ${section.position}) but it's absent`));
        }
      }
    }
  }

  return results;
}

/**
 * Check B: Reserved sections should not be present for this form.
 *
 * For each container: iterate formConstraints.cannot entries. Check if that
 * section tag appears in the file. If present, emit info with why_reserved.
 */
function checkFormReservedSections(ctx: RustFileContext): LintResult[] {
  // Templates ARE live — they define canonical form structure.
  // A module template with S2 Modules is wrong at the source.
  // Reserved checks fire on templates. Only skip if no form loaded.
  if (!ctx.formConstraints) return [];

  const results: LintResult[] = [];
  const file = ctx.filePath;

  for (const containerName of ["SETUP", "BODY", "CLOSING"] as const) {
    const container = ctx.formConstraints[containerName];
    if (!container) continue;

    const blockLines = getBlockLines(ctx.lines, ctx.blocks, containerName);

    for (const reserved of container.cannot) {
      // Detect reserved section tag in three-line subsection header format:
      //   // ──────────────────────────────────────────────────────────────
      //   // Tag Name  (or "7. Tag Name")
      //   // ──────────────────────────────────────────────────────────────
      // The tag sits on its own line between separators.
      // Match PascalCase ("FreeFunctions"), spaced ("Free Functions"), and numbered ("7. Free Functions").
      const spacedReserved = reserved.tag.replace(/([a-z])([A-Z])/g, "$1\\s*$2");
      const tagPattern = new RegExp(
        `^\\s*\\/\\/\\s*(?:\\d+\\.\\s*)?${spacedReserved}\\s*$`,
        "i"
      );
      const separatorLine = /^\/\/\s*[─=\-]{4,}\s*$/;

      let tagLineIdx = -1;
      for (let i = 0; i < blockLines.length; i++) {
        const trimmed = blockLines[i]!.trim();
        if (separatorLine.test(trimmed)) continue;       // skip separator-only lines
        if (tagPattern.test(trimmed)) { tagLineIdx = i; break; }
      }

      if (tagLineIdx < 0) continue;  // tag not found in this block

      // Scan past the closing separator of the three-line header,
      // then look for real content below.
      let hasContent = false;
      let pastHeader = false;
      for (let i = tagLineIdx + 1; i < blockLines.length; i++) {
        const trimmed = blockLines[i]!.trim();

        // First separator after the tag = closing line of header — skip it
        if (!pastHeader && separatorLine.test(trimmed)) { pastHeader = true; continue; }

        // Stop at next subsection header (separator) or block boundary (===)
        if (pastHeader && (separatorLine.test(trimmed) || /^\/\/\s*={4,}/.test(trimmed))) break;

        // Skip empty lines, bare comment markers, and "(none)" placeholders
        if (trimmed === "" || trimmed === "//" || /^\s*\/\/\s*\(.*none.*\)/.test(blockLines[i]!)) continue;

        // Real content found (actual code, not just a comment)
        if (trimmed.length > 0 && !trimmed.startsWith("//")) {
          hasContent = true;
          break;
        }
      }

      if (hasContent) {
        const sev = policySeverity("warn", ctx.policy);
        const emit = sev === "error" ? error : sev === "warn" ? warn : info;
        results.push(emit(file, `form/reserved-section-present`,
          `${ctx.formConstraints.name} form: "${reserved.tag}" is RESERVED in ${containerName} — ${reserved.whyReserved}`));
      }
    }
  }

  return results;
}

/** Escape special regex characters in a string. */
function escapeRegex(s: string): string {
  return s.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
}

// ---------------------------------------------------------------------------
// stripBlockStructure — extract raw content from a 4-block file for re-scaffold
// ---------------------------------------------------------------------------
//
// Used by --force: removes all structural scaffolding (block banners, block end
/** Closing field values extracted from an existing 4-block file. */
interface ClosingFieldValues {
  note?: string;
  anchor?: string;
  scripture?: string;
  policyScripture?: string;
  never?: string;
  careful?: string;
  safe?: string;
  relatedFiles?: string;
  validate?: string;
}

/**
 * Extract closing field values from existing file content.
 * Called before strip so the scaffold can reuse human-authored values
 * instead of generating generic placeholders.
 */
function extractClosingFields(lines: string[]): ClosingFieldValues {
  const vals: ClosingFieldValues = {};
  const fieldPattern = /^\s*\/\/\s*(note|anchor|scripture|never|careful|safe|related_files|validate):\s*(.+)/;
  // Track which zone we're in to disambiguate "scripture" in X1 vs X5
  let lastZone = "";
  for (const line of lines) {
    const trimmed = line.trim();
    // Detect zone headers
    if (/\/\/\s*X1\b/.test(trimmed)) lastZone = "X1";
    else if (/\/\/\s*X4\b/.test(trimmed)) lastZone = "X4";
    else if (/\/\/\s*X5\b/.test(trimmed)) lastZone = "X5";
    else if (/\/\/\s*Cv\b/.test(trimmed)) lastZone = "Cv";
    else if (/\/\/\s*Ce\b/.test(trimmed)) lastZone = "Ce";

    const m = fieldPattern.exec(trimmed);
    if (!m) continue;
    const [, field, value] = m;
    const v = value!.trim();

    switch (field) {
      case "note": vals.note = v; break;
      case "anchor":
        // Only capture non-placeholder anchors
        if (!v.includes("TODO:")) vals.anchor = v;
        break;
      case "scripture":
        if (lastZone === "X1") vals.policyScripture = v;
        else vals.scripture = v;
        break;
      case "never": vals.never = v; break;
      case "careful": vals.careful = v; break;
      case "safe": vals.safe = v; break;
      case "related_files": vals.relatedFiles = v; break;
      case "validate": vals.validate = v; break;
    }
  }
  return vals;
}

// markers, subsection headers, reserved omission blocks, SETUP/BODY comments,
// CLOSING zone content) while preserving:
//   - Pragma and directives (//omni:key, //omni:code, //omni:version)
//   - Doc comments (//!)
//   - Use statements and code (fn, struct, pub, etc.)
//   - Test modules (#[cfg(test)] mod tests { ... })
//   - X1 modification policy fields (these are regenerated by scaffold)
//
// The result is a "raw" file suitable for structuralScaffoldRust().

function stripBlockStructure(lines: string[]): string[] {
  const out: string[] = [];
  const blockBanner = /^\/\/\s*={4,}\s*$/;                    // // ===...
  const blockTitle = /^\s*\/\/\s*(METADATA|SETUP|BODY|CLOSING)\s+BLOCK\s+\[/i;
  const blockEnd = /^\s*\/\/\s*END\s+(METADATA|SETUP|BODY|CLOSING)\s+\[END\]/i;
  const subsectionSep = /^\/\/\s*[─]{4,}\s*$/;                // // ──────
  const reservedOmission = /^\s*\/\/\s*Reserved\s+Omission\s*$/i;
  const reservedLine = /^\s*\/\/\s{2,}\S/;                     // //   Tag — reason
  const todoLine = /^\s*\/\/\s*TODO:/;
  const seeTemplateLine = /^\s*\/\/\s*See\s+seed\//;
  const emptyComment = /^\s*\/\/\s*$/;                         // bare //
  const setupBodyComment = /^\s*\/\/\s*(SETUP|BODY)\s+(makes things|made things)/i;
  const sectionOrderComment = /^\s*\/\/\s*(Section|Subsection)\s+order/i;
  const numberedOrderLine = /^\s*\/\/\s+\d+\.\s+/;            // //     1. Imports

  // Closing zone headers and synthetic content
  const closingZoneHeader = /^\s*\/\/\s*(Cv|Ce|X1|X2|X3|X4|X5|X6)\b/;
  const closingFieldLine = /^\s*\/\/\s*(policy|scripture|never|careful|safe|related_files|validate|note|anchor|coverage_report):/;

  // Phase 1: identify lines that are purely structural
  let inReservedOmission = false;
  let inClosingZone = false;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    const trimmed = line.trim();

    // Block banners: look for 3-line pattern ===, title, ===
    if (blockBanner.test(trimmed)) {
      // Check if next line is a block title or end marker
      const next = i + 1 < lines.length ? lines[i + 1]!.trim() : "";
      if (blockTitle.test(next) || blockEnd.test(next)) {
        // Skip this banner and the next 2 lines (title + closing banner)
        i += 2;
        continue;
      }
      // Standalone separator at end of block — skip
      if (blockEnd.test(trimmed)) continue;
      // Otherwise could be a header we should skip
      continue;
    }

    // Block title/end on its own (shouldn't happen without banner, but guard)
    if (blockTitle.test(trimmed) || blockEnd.test(trimmed)) continue;

    // Subsection separators: ── lines
    if (subsectionSep.test(trimmed)) {
      // Check if it's a 3-line subsection header (sep, name, sep)
      const next = i + 1 < lines.length ? lines[i + 1]!.trim() : "";
      const nextNext = i + 2 < lines.length ? lines[i + 2]!.trim() : "";
      if (!subsectionSep.test(next) && subsectionSep.test(nextNext)) {
        // If middle line is "Reserved Omission", set the flag so content
        // lines after this header are also stripped.
        if (reservedOmission.test(next)) {
          inReservedOmission = true;
        }
        // 3-line header — skip all three
        i += 2;
        continue;
      }
      // Just a separator — skip it
      continue;
    }

    // Reserved Omission block — skip until next code/section
    if (reservedOmission.test(trimmed)) {
      inReservedOmission = true;
      continue;
    }
    if (inReservedOmission) {
      if (reservedLine.test(line) || emptyComment.test(trimmed) || trimmed === "") {
        continue;
      }
      inReservedOmission = false;
    }

    // Closing zone headers and field content
    if (closingZoneHeader.test(trimmed) && !trimmed.includes("#[cfg(test)]")) {
      inClosingZone = true;
      continue;
    }
    if (inClosingZone) {
      if (closingFieldLine.test(trimmed) || emptyComment.test(trimmed) || trimmed === "") {
        continue;
      }
      // Reached non-closing content — exit zone
      inClosingZone = false;
    }

    // Template/structural comments
    if (todoLine.test(trimmed)) continue;
    if (seeTemplateLine.test(trimmed)) continue;
    if (setupBodyComment.test(trimmed)) continue;
    if (sectionOrderComment.test(trimmed)) continue;
    if (numberedOrderLine.test(trimmed)) continue;

    // "No entry point" comment
    if (/^\s*\/\/\s*No entry point/.test(trimmed)) continue;

    // Keep everything else
    out.push(line);
  }

  // Collapse runs of empty lines to max 1
  const collapsed: string[] = [];
  let prevEmpty = false;
  for (const line of out) {
    const empty = line.trim() === "";
    if (empty && prevEmpty) continue;
    collapsed.push(line);
    prevEmpty = empty;
  }

  // Trim trailing empty lines
  while (collapsed.length > 0 && collapsed[collapsed.length - 1]!.trim() === "") {
    collapsed.pop();
  }

  return collapsed;
}

// ---------------------------------------------------------------------------
// Lint orchestrator
// ---------------------------------------------------------------------------

async function lintRustFile(filePath: string): Promise<LintResult[]> {
  await ensureRustRules();
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
    ...checkRequiredSetupSubsections(ctx),
    ...checkSetupHeaderDoc(ctx),
    ...checkIdentityRegistration(ctx),
    ...checkDirectiveFormat(ctx),
    ...checkContentPlacement(ctx),
    ...checkBodySubsectionOrder(ctx),
    ...checkRequiredBodySubsections(ctx),
    ...checkBodySubtypeContent(ctx),
    ...checkSubtypeEmphasis(ctx),
    ...checkClosingZoneOrder(ctx, _rustRules!.closingData),
    ...checkClosingContentPlacement(ctx),
    ...checkClosingRequiredZones(ctx, _rustRules!.closingData),
    ...checkClosingZoneContent(ctx, _rustRules!.closingData),
    ...checkClosingX6TemplateOnly(ctx, _rustRules!.closingData),
    ...checkClosingDocFieldContent(ctx, _rustRules!.closingData),
    ...checkScalingSignals(ctx),
    // Form-aware validation — bare-bone always loads (format level).
    // Variant form overlays when subtype declared. Null guard for schema-load failure only.
    ...(ctx.formConstraints ? checkFormRequiredSections(ctx) : []),
    ...(ctx.formConstraints ? checkFormReservedSections(ctx) : []),
  ];
}

// ---------------------------------------------------------------------------
// Transform 0: Structural Scaffold — the DAR Recover step
// ---------------------------------------------------------------------------
//
// Takes an unstructured Rust file (has pragma, no block boundaries) and
// generates the full 4-block structure. This is the heavy-lifting transform.
//
// Algorithm:
//   1. Parse pragma to identify form (module, library, etc.)
//   2. Load form constraints (what sections are required/available/reserved)
//   3. Walk lines, grouping into semantic chunks (imports, functions, tests...)
//   4. Map chunks to blocks/sections using placement maps
//   5. Generate scaffold with proper section headers and Reserved Omission
//
// The bare-bone catalog defines what sections exist.
// The form schema defines which are required/available/reserved.
// Reserved Omission entries index intentionally absent sections.
// Completeness = sections_present + sections_in_reserved_omission = catalog.

/** A contiguous chunk of code with a semantic classification. */
interface CodeChunk {
  lines: string[];
  category: "pragma" | "doc_comment" | "import" | "const" | "static" | "type_def"
    | "trait_def" | "trait_impl" | "impl_block" | "fn_decl" | "test_module"
    | "main_fn" | "separator" | "comment" | "blank" | "reexport" | "mod_decl"
    | "macro_decl" | "other";
}

/** Where a code chunk should be placed in the 4-block structure. */
type BlockTarget = "METADATA" | "SETUP" | "BODY" | "CLOSING";

/** Which BODY section a chunk maps to. */
type BodySection = "IdentityAccess" | "TraitImplementations" | "Constructors & Builders"
  | "CoreLogic" | "Queries & Accessors" | "FreeFunctions";

/**
 * Parse a file into semantic code chunks.
 *
 * Walks the lines, tracking brace depth, grouping contiguous lines of the
 * same semantic category. Brace-delimited constructs (fn, impl, mod tests)
 * are captured as complete units from their first line to their closing brace.
 */
function parseCodeChunks(lines: string[]): CodeChunk[] {
  const chunks: CodeChunk[] = [];
  let i = 0;

  while (i < lines.length) {
    const trimmed = lines[i]!.trim();

    // --- Pragma directives (at top of file) ---
    if (/^\s*\/\/\s+#!omni\b/.test(trimmed) || /^\s*\/\/omni:/.test(trimmed)) {
      const chunk: CodeChunk = { lines: [lines[i]!], category: "pragma" };
      i++;
      // Collect contiguous pragma lines
      while (i < lines.length) {
        const next = lines[i]!.trim();
        if (/^\s*\/\/omni:/.test(next)) {
          chunk.lines.push(lines[i]!);
          i++;
        } else {
          break;
        }
      }
      chunks.push(chunk);
      continue;
    }

    // --- Doc comments (//!) ---
    if (trimmed.startsWith("//!")) {
      const chunk: CodeChunk = { lines: [lines[i]!], category: "doc_comment" };
      i++;
      while (i < lines.length && lines[i]!.trim().startsWith("//!")) {
        chunk.lines.push(lines[i]!);
        i++;
      }
      chunks.push(chunk);
      continue;
    }

    // --- Blank lines ---
    if (trimmed === "") {
      const chunk: CodeChunk = { lines: [lines[i]!], category: "blank" };
      i++;
      while (i < lines.length && lines[i]!.trim() === "") {
        chunk.lines.push(lines[i]!);
        i++;
      }
      chunks.push(chunk);
      continue;
    }

    // --- Separator comments (existing section headers / dividers) ---
    if (/^\/\/\s*[─═\-]{10,}/.test(trimmed)) {
      const chunk: CodeChunk = { lines: [lines[i]!], category: "separator" };
      i++;
      // Collect the title line and closing separator
      while (i < lines.length) {
        const next = lines[i]!.trim();
        if (/^\/\/\s*[─═\-]{10,}/.test(next) || /^\/\/\s+\S/.test(next)) {
          chunk.lines.push(lines[i]!);
          i++;
          if (/^\/\/\s*[─═\-]{10,}/.test(next)) break; // closing separator
        } else {
          break;
        }
      }
      chunks.push(chunk);
      continue;
    }

    // --- Regular comments (not doc comments, not separators, not pragma) ---
    if (trimmed.startsWith("//") && !trimmed.startsWith("//!") && !trimmed.startsWith("//omni:")) {
      const chunk: CodeChunk = { lines: [lines[i]!], category: "comment" };
      i++;
      while (i < lines.length) {
        const next = lines[i]!.trim();
        if (next.startsWith("//") && !next.startsWith("//!") &&
            !next.startsWith("//omni:") && !/^\/\/\s*[─═\-]{10,}/.test(next)) {
          chunk.lines.push(lines[i]!);
          i++;
        } else {
          break;
        }
      }
      chunks.push(chunk);
      continue;
    }

    // --- #[cfg(test)] mod tests --- (capture complete brace-delimited block)
    if (trimmed === "#[cfg(test)]") {
      const chunk: CodeChunk = { lines: [lines[i]!], category: "test_module" };
      i++;
      let braceDepth = 0;
      let foundOpen = false;
      while (i < lines.length) {
        chunk.lines.push(lines[i]!);
        for (const ch of lines[i]!) {
          if (ch === "{") { braceDepth++; foundOpen = true; }
          if (ch === "}") braceDepth--;
        }
        i++;
        if (foundOpen && braceDepth === 0) break;
      }
      chunks.push(chunk);
      continue;
    }

    // --- Attributes before items (collect and attach to next chunk) ---
    if (trimmed.startsWith("#[") || trimmed.startsWith("#![")) {
      const attrLines: string[] = [lines[i]!];
      i++;
      while (i < lines.length && (lines[i]!.trim().startsWith("#[") || lines[i]!.trim().startsWith("#!["))) {
        attrLines.push(lines[i]!);
        i++;
      }
      // The next chunk will get these prepended — for now, push back and
      // let the next iteration handle the item; we'll attach attrs below.
      // Actually, just store them as comment for now and they'll stay with the next item.
      // Simpler: push them back and handle as part of the item.
      i -= attrLines.length; // rewind
      // Fall through to the item classification below
    }

    // --- Classify the line using the existing classifier ---
    const kind = classifyLine(trimmed);

    // --- Brace-delimited items (fn, impl, struct, enum, trait, mod, macro, type alias) ---
    if (kind === "fn_decl" || kind === "impl_block" || kind === "struct_decl" ||
        kind === "enum_decl" || kind === "trait_decl" || kind === "mod_decl" ||
        kind === "macro_decl" || kind === "type_alias") {
      // Capture any preceding attributes and doc comments that are attached
      const itemLines: string[] = [];

      // Look back for attached attributes and doc comments.
      // Handles the common case where a blank line separates /// from its item:
      //   /// Doc comment for function
      //
      //   pub fn my_function() { ... }
      // The blank is formatting noise — the doc comment belongs to the function.
      while (chunks.length > 0) {
        const last = chunks[chunks.length - 1]!;
        if (last.category === "comment" && last.lines.some((l) => l.trim().startsWith("///"))) {
          itemLines.unshift(...chunks.pop()!.lines);
        } else if (last.category === "comment" && last.lines.every((l) =>
          l.trim().startsWith("#[") || l.trim().startsWith("#!["))) {
          itemLines.unshift(...chunks.pop()!.lines);
        } else if (last.category === "blank" && last.lines.length <= 1) {
          // Single blank MAY separate a /// doc comment from its item.
          // Peek past it to check — if there's a /// comment, attach it and
          // drop the blank (clean formatting). Otherwise restore and stop.
          const savedBlank = chunks.pop()!;
          if (chunks.length > 0 &&
              chunks[chunks.length - 1]!.category === "comment" &&
              chunks[chunks.length - 1]!.lines.some((l) => l.trim().startsWith("///"))) {
            // Found /// doc comment before the blank — attach it, skip the blank
            itemLines.unshift(...chunks.pop()!.lines);
          } else {
            // No doc comment — restore blank and stop
            chunks.push(savedBlank);
            break;
          }
        } else {
          break;
        }
      }

      itemLines.push(lines[i]!);
      i++;

      // Track braces to capture the full item
      let braceDepth = 0;
      for (const ch of itemLines[itemLines.length - 1]!) {
        if (ch === "{") braceDepth++;
        if (ch === "}") braceDepth--;
      }

      // If item has braces, capture until balanced
      if (braceDepth > 0) {
        while (i < lines.length && braceDepth > 0) {
          itemLines.push(lines[i]!);
          for (const ch of lines[i]!) {
            if (ch === "{") braceDepth++;
            if (ch === "}") braceDepth--;
          }
          i++;
        }
      } else if (trimmed.endsWith(";")) {
        // Single-line item (e.g., `type Foo = Bar;`) — already captured
      } else {
        // Multi-line signature without braces (e.g., long fn signature)
        while (i < lines.length) {
          itemLines.push(lines[i]!);
          const line = lines[i]!;
          for (const ch of line) {
            if (ch === "{") braceDepth++;
            if (ch === "}") braceDepth--;
          }
          i++;
          if (braceDepth > 0) {
            // Found opening brace, continue until balanced
            while (i < lines.length && braceDepth > 0) {
              itemLines.push(lines[i]!);
              for (const ch of lines[i]!) {
                if (ch === "{") braceDepth++;
                if (ch === "}") braceDepth--;
              }
              i++;
            }
            break;
          }
          if (line.trim().endsWith(";")) break; // terminated
        }
      }

      // Map kind to category
      let category: CodeChunk["category"];
      if (kind === "fn_decl") {
        // Check if it's main
        if (/^(pub\s+)?fn\s+main\s*\(/.test(trimmed)) {
          category = "main_fn";
        } else {
          category = "fn_decl";
        }
      } else if (kind === "impl_block") {
        // Check if trait impl: `impl Trait for Type`
        if (/^(pub\s+)?(unsafe\s+)?impl\s+\w+.*\s+for\s+/.test(trimmed)) {
          category = "trait_impl";
        } else {
          category = "impl_block";
        }
      } else if (kind === "struct_decl" || kind === "enum_decl" || kind === "type_alias") {
        category = "type_def";
      } else if (kind === "trait_decl") {
        category = "trait_def";
      } else if (kind === "mod_decl") {
        category = "mod_decl";
      } else if (kind === "macro_decl") {
        category = "macro_decl";
      } else {
        category = "other";
      }
      chunks.push({ lines: itemLines, category });
      continue;
    }

    // --- use declarations (contiguous block) ---
    if (kind === "use_decl") {
      const chunk: CodeChunk = { lines: [lines[i]!], category: "import" };
      i++;
      while (i < lines.length) {
        const next = lines[i]!.trim();
        const nextKind = classifyLine(next);
        // Absorb: more use decls, blank lines, regular comments.
        // Do NOT absorb: /// doc comments (they belong to the NEXT item, not imports),
        // //! module docs, or //omni: directives.
        if (nextKind === "use_decl" || next === "" ||
            (next.startsWith("//") && !next.startsWith("//!") &&
             !next.startsWith("///") && !next.startsWith("//omni:"))) {
          chunk.lines.push(lines[i]!);
          i++;
        } else {
          break;
        }
      }
      chunks.push(chunk);
      continue;
    }

    // --- const declarations ---
    if (kind === "const_decl") {
      const chunk: CodeChunk = { lines: [lines[i]!], category: "const" };
      i++;
      chunks.push(chunk);
      continue;
    }

    // --- static declarations ---
    if (kind === "static_decl") {
      const chunk: CodeChunk = { lines: [lines[i]!], category: "static" };
      i++;
      chunks.push(chunk);
      continue;
    }

    // --- re-exports ---
    if (kind === "reexport_decl") {
      const chunk: CodeChunk = { lines: [lines[i]!], category: "reexport" };
      i++;
      chunks.push(chunk);
      continue;
    }

    // --- Catch-all ---
    const chunk: CodeChunk = { lines: [lines[i]!], category: "other" };
    i++;
    chunks.push(chunk);
  }

  return chunks;
}

/**
 * Map a code chunk to its target block.
 */
function chunkToBlock(chunk: CodeChunk): BlockTarget {
  switch (chunk.category) {
    case "pragma":
    case "doc_comment":
      return "METADATA";
    case "import":
    case "const":
    case "static":
    case "type_def":
    case "trait_def":
    case "reexport":
    case "mod_decl":
    case "macro_decl":
      return "SETUP";
    case "fn_decl":
    case "impl_block":
    case "trait_impl":
      return "BODY";
    case "test_module":
    case "main_fn":
      return "CLOSING";
    case "separator":
    case "comment":
    case "blank":
    case "other":
      return "BODY"; // Default: orphaned content → BODY
  }
}

// ---------------------------------------------------------------------------
// Transform formatting helpers — template-aligned output
// ---------------------------------------------------------------------------

/**
 * Convert CamelCase tag to readable label: "CoreTypes" → "Core Types".
 * Tags with existing spaces or "&" pass through unchanged.
 */
function tagToLabel(tag: string): string {
  if (tag.includes(" ")) return tag; // Already has spaces (e.g., "Constructors & Builders")
  return tag.replace(/([a-z])([A-Z])/g, "$1 $2");
}

/** CLOSING zone descriptive labels — architecturally stable, matches template. */
const CLOSING_ZONE_LABELS: Record<string, string> = {
  "Cv": "Cv — Closing Validation",
  "Ce": "Ce — Closing Execution",
  "Cc": "Cc — Closing Cleanup",
  "X1": "X1: Modification Policy",
  "X2": "X2: Extension Points",
  "X3": "X3: Troubleshooting",
  "X4": "X4: Reference",
  "X5": "X5: Closing Note",
  "X6": "X6: Template Guide",
};

/** Compact section description for block overview TOC. */
const SECTION_DESCRIPTIONS: Record<string, string> = {
  // SETUP sections
  "Imports":        "What this file depends on",
  "Modules":        "Submodule declarations",
  "Constants":      "Compile-time fixed values",
  "Statics":        "Runtime-initialized fixed values",
  "TypeAliases":    "Shorthand for complex signatures",
  "ErrorTypes":     "enum + Display + Error + From impls",
  "CoreTypes":      "struct/enum + derives + completing trait impls",
  "TraitDefs":      "Behavioral contracts (shape, not fulfillment)",
  "Macros":         "macro_rules! declarations",
  "FeatureGates":   "Conditional compilation (cfg)",
  // BODY sections
  "IdentityAccess":       "OmniCode static accessor functions",
  "TraitImplementations": "Fulfilling contracts from SETUP",
  "Constructors & Builders": "new(), builders, typestate transitions",
  "CoreLogic":            "Primary operations, state transforms",
  "Queries & Accessors":  "Read-only &self methods",
  "Output & Display":     "Formatting, serialization",
  "FreeFunctions":        "Module-level public utilities",
  "Helpers":              "Support functions for run()",
};

type SectionEntry =
  | { kind: "active"; position: number; tag: string; status: "REQUIRED" | "AVAILABLE" }
  | { kind: "reserved"; position: number; tag: string; whyReserved: string };

/**
 * Build a complete section index merging active + reserved sections in position order.
 * Reserved sections without known positions are appended at end.
 */
function buildSectionIndex(
  can: FormSectionConstraint[],
  cannot: FormReservedSection[],
): SectionEntry[] {
  const entries: SectionEntry[] = [];
  for (const s of can) {
    entries.push({ kind: "active", position: s.position, tag: s.tag, status: s.status });
  }
  for (const s of cannot) {
    if (s.position != null) {
      entries.push({ kind: "reserved", position: s.position, tag: s.tag, whyReserved: s.whyReserved });
    }
  }
  entries.sort((a, b) => a.position - b.position);
  return entries;
}

/**
 * Emit a block overview section index using bracket format [tag].
 *
 * Bracket format avoids false-positive zone detection AND subsection pattern
 * detection. The CLOSING block discovered this — [Cv], [Ce], [X1] work where
 * numbered "1.", "2." can collide with linter patterns. Apply the lesson
 * everywhere.
 */
function emitSectionIndex(out: string[], index: SectionEntry[]): void {
  // Bracket format: //     [N]   Label           — Description
  // The [N] avoids subsection pattern detection (which matches "// N." or "// Label")
  // and zone detection (which matches "// TAG (colon-or-space)").
  // 5 spaces after // keeps us in the overview indent zone.
  for (const entry of index) {
    const pos = entry.position.toString();
    const tag = `[${pos}]`;
    const tagPad = tag.length < 4 ? " ".repeat(4 - tag.length) : " ";
    const label = tagToLabel(entry.tag);
    const desc = SECTION_DESCRIPTIONS[entry.tag] ?? "";
    const pad = label.length < 18 ? " ".repeat(18 - label.length) : " ";
    if (entry.kind === "reserved") {
      out.push(`//     ${tag}${tagPad}${label}${pad}\u2014 RESERVED`);
    } else if (desc) {
      out.push(`//     ${tag}${tagPad}${label}${pad}\u2014 ${desc}`);
    } else {
      out.push(`//     ${tag}${tagPad}${label}`);
    }
  }
}

/** Compact closing zone descriptions for block overview TOC. */
const CLOSING_ZONE_DESCRIPTIONS: Record<string, string> = {
  "Cv": "Closing Validation (tests)",
  "Ce": "Closing Execution (entry point or absence)",
  "Cc": "Closing Cleanup (resource teardown)",
  "X1": "Modification Policy",
  "X2": "Extension Points",
  "X3": "Troubleshooting",
  "X4": "Reference",
  "X5": "Closing Note",
};

/**
 * Emit the CLOSING block overview — bracket-tagged zone list.
 * Matches exists.rs production standard: [Cv], [Ce], [Cc], [X1]-[X5].
 */
function emitClosingOverview(out: string[]): void {
  out.push(`//`);
  out.push(`// Closing ensures correctness, documents constraints, and anchors the file.`);
  out.push(`//`);
  out.push(`// Section order:`);
  out.push(`//`);
  for (const tag of ["Cv", "Ce", "Cc", "X1", "X2", "X3", "X4", "X5"]) {
    const desc = CLOSING_ZONE_DESCRIPTIONS[tag] ?? tag;
    const tagStr = `[${tag}]`;
    const tagPad = tagStr.length < 5 ? " ".repeat(5 - tagStr.length) : " ";
    out.push(`//     ${tagStr}${tagPad}${desc}`);
  }
}

// ---------------------------------------------------------------------------
// METADATA block generation — full identity from schema fill_content
// ---------------------------------------------------------------------------

/**
 * Extract context from existing metadata chunks for METADATA block generation.
 *
 * The transformer receives an existing file with pragma, directives, and doc
 * comments already classified. This function mines those chunks for identity
 * values (key, version, subtype, title, purpose) that populate the PRAGMA
 * and METADATA static declarations.
 *
 * Values we CAN extract from the file: key, version, subtype, title, purpose,
 * filename. Values we CAN'T: from (project path), organization, scripture,
 * consumers — these become TODO placeholders for human editing.
 */
function extractMetadataContext(
  metadataChunks: CodeChunk[],
  filePath: string,
  subtype: string | undefined,
  allLines?: string[],
): Record<string, string> {
  const ctx: Record<string, string> = {};

  // Extract from directives and pragma
  for (const chunk of metadataChunks) {
    for (const line of chunk.lines) {
      const trimmed = line.trim();

      // //omni:key B-L0-hybrid-config-exists
      const keyMatch = trimmed.match(/^\/\/omni:key\s+(.+)/);
      if (keyMatch) ctx["key"] = keyMatch[1]!.trim();

      // //omni:version b-03.00
      const verMatch = trimmed.match(/^\/\/omni:version\s+(.+)/);
      if (verMatch) ctx["version"] = verMatch[1]!.trim();

      // //! First doc comment line = title
      if (trimmed.startsWith("//!") && !ctx["title"]) {
        const docText = trimmed.replace(/^\/\/!\s*/, "").trim();
        // Skip empty //! lines
        if (docText.length > 0) {
          // Title is the first substantive doc comment line
          // Strip trailing markers like " — single source of truth..."
          ctx["title"] = docText.replace(/\s*—\s*.*/, "").trim() || docText;
          // Full first line is the purpose seed
          ctx["purpose_line"] = docText;
        }
      }
    }
  }

  // Collect all //! lines for extended purpose
  const docLines: string[] = [];
  for (const chunk of metadataChunks) {
    if (chunk.category === "doc_comment") {
      for (const line of chunk.lines) {
        const trimmed = line.trim();
        if (trimmed.startsWith("//!")) {
          const text = trimmed.replace(/^\/\/!\s*/, "").trim();
          if (text.length > 0) docLines.push(text);
        }
      }
    }
  }
  // Purpose = first meaningful doc comment line (already captured as purpose_line)
  if (ctx["purpose_line"]) {
    ctx["purpose"] = ctx["purpose_line"];
  }

  // Filename from path
  const pathParts = filePath.replace(/\\/g, "/").split("/");
  ctx["filename"] = pathParts[pathParts.length - 1] ?? "";

  // Subtype
  if (subtype) ctx["subtype"] = subtype;

  // --- Auto-derivation (Phase 7) ---
  // Best-effort: fill what we can from available context. Placeholders remain
  // for what requires human knowledge.

  // I3.path — relative path from repo root
  // Detect common repo markers and compute relative path
  const normalized = filePath.replace(/\\/g, "/");
  const repoMarkers = [".a-new-structure/", "bereshit/", "src/"];
  for (const marker of repoMarkers) {
    const idx = normalized.indexOf(marker);
    if (idx >= 0) {
      ctx["path"] = normalized.slice(idx);
      break;
    }
  }
  if (!ctx["path"]) {
    // Fallback: use last 3 path segments
    const segments = normalized.split("/").filter(Boolean);
    ctx["path"] = segments.slice(-3).join("/");
  }

  // I3.component — derive from filename (strip .rs) + crate context if visible in path
  const stem = ctx["filename"]?.replace(/\.rs$/, "") ?? "";
  if (stem === "lib") {
    ctx["component"] = "crate root";
  } else if (stem === "main") {
    ctx["component"] = "binary entry point";
  } else if (stem === "mod") {
    ctx["component"] = "module root";
  } else if (stem) {
    ctx["component"] = stem.replace(/_/g, " ");
  }

  // I3.brief — use purpose_line if available (first //! doc comment)
  if (ctx["purpose_line"]) {
    // First sentence or the whole thing if no period
    const firstSentence = ctx["purpose_line"].split(".")[0]!.trim();
    ctx["brief"] = firstSentence.endsWith(".") ? firstSentence : firstSentence + ".";
  }

  // I3.provides — scan BODY for pub fn/pub struct/pub enum signatures
  if (allLines) {
    const pubItems: string[] = [];
    for (const line of allLines) {
      const trimmed = line.trim();
      // pub fn name(...) or pub async fn name(...)
      const fnMatch = trimmed.match(/^pub\s+(?:async\s+)?fn\s+(\w+)/);
      if (fnMatch) { pubItems.push(fnMatch[1]!); continue; }
      // pub struct Name or pub enum Name
      const typeMatch = trimmed.match(/^pub\s+(?:struct|enum)\s+(\w+)/);
      if (typeMatch) { pubItems.push(typeMatch[1]!); continue; }
      // pub trait Name
      const traitMatch = trimmed.match(/^pub\s+trait\s+(\w+)/);
      if (traitMatch) { pubItems.push(traitMatch[1]!); continue; }
    }
    if (pubItems.length > 0) {
      ctx["provides"] = pubItems.join(", ");
    }
  }

  return ctx;
}

/**
 * Generate the full METADATA block content with PRAGMA and METADATA statics.
 *
 * The METADATA block is WHO the file IS. Identity before content. The schema's
 * fill_content defines the exact fields and syntax. This function fills them
 * with values extracted from the existing file where possible, and TODO
 * placeholders where the file doesn't have the data yet.
 *
 * Why generate in full? Because you can always wholesale edit a filled block.
 * Building from scratch is harder than editing what's there. Boundaries before
 * content — the structure gives you the container to pour identity into.
 */
function buildMetadataBlock(
  metadataChunks: CodeChunk[],
  filePath: string,
  subtype: string | undefined,
  fillContent: SchemaFillContent,
  mode: "strict" | "balance" | "growth" = "strict",
  allLines?: string[],
): string[] {
  const eq = "=".repeat(BLOCK_SEPARATOR_WIDTH);
  const dash = "\u2500".repeat(SUBSECTION_SEPARATOR_WIDTH);
  const out: string[] = [];

  // Resolve mode capabilities from schema
  const modeConfig = fillContent.transformerModes?.[mode];
  const useSectionHeaders = modeConfig?.sectionHeaders ?? (mode === "strict");
  const useGroupComments = modeConfig?.groupComments ?? (mode === "strict");
  const useDocstrings = modeConfig?.docstrings ?? (mode === "strict");
  const useColumnAlignment = modeConfig?.columnAlignment ?? (mode === "strict");

  // Extract what we can from the existing file
  const ctx = extractMetadataContext(metadataChunks, filePath, subtype, allLines);

  const key = ctx["key"] ?? "[key]";
  const version = ctx["version"] ?? fillContent.defaults.version;
  const title = ctx["title"] ?? "[title]";
  const purpose = ctx["purpose"] ?? "[purpose]";
  const filename = ctx["filename"] ?? "[filename]";
  const subtypeVal = ctx["subtype"] ?? "[subtype]";
  const date = new Date().toISOString().slice(0, 10);

  // Auto-derived I3 fields (Phase 7)
  const path = ctx["path"] ?? "[path]";
  const component = ctx["component"] ?? "[component]";
  const brief = ctx["brief"] ?? purpose;
  const provides = ctx["provides"] ?? "[provides]";

  // Substitution map — what we know from the file + defaults from schema
  const subs: Record<string, string> = {
    key,
    version,
    title,
    purpose,
    filename,
    subtype: subtypeVal,
    date,
    format: "rust",
    from: "[from]",
    crate_name: "[crate_name]",
    status: fillContent.defaults.status,
    organization: fillContent.defaults.organization,
    scripture: fillContent.defaults.scripture,
    scripture_text: fillContent.defaults.scripture_text,
    consumers: "[consumers]",
    // Auto-derived (Phase 7)
    path,
    component,
    brief,
    provides,
  };
  // Pull in extra defaults (architect, role, layer, etc.)
  for (const [k, v] of Object.entries(fillContent.defaults)) {
    if (!(k in subs)) subs[k] = v;
  }

  /** Substitute {{placeholders}} in a template string. */
  function sub(template: string): string {
    return template.replace(/\{\{(\w+)\}\}/g, (_match, name) => {
      return subs[name] ?? `[${name}]`;
    });
  }

  /**
   * Emit identity entries with optional group comments and column alignment.
   *
   * In strict mode, produces:
   *   // I1: Core
   *   ("I1.key",       "B-L0-hybrid-config-exists"),
   *   ("I1.format",    "rust"),
   *   // I2: Family
   *   ("I2.type",      "code"),
   *
   * In growth mode, produces flat entries without formatting.
   */
  function emitEntries(
    entries: [string, string][],
    syntax: { entry: string },
    groups?: { range: string; label: string }[],
  ): string[] {
    const lines: string[] = [];

    // Compute column alignment width from longest field name
    const maxFieldLen = useColumnAlignment
      ? Math.max(...entries.map(([f]) => f.length))
      : 0;

    let currentGroup = "";

    for (const [field, value] of entries) {
      // Detect group transition and emit group comment
      if (useGroupComments && groups) {
        const fieldPrefix = field.match(/^[A-Z]\d+/)?.[0] ?? "";
        if (fieldPrefix !== currentGroup) {
          currentGroup = fieldPrefix;
          const group = groups.find((g) => g.range === fieldPrefix);
          if (group) {
            lines.push(`    // ${group.range}: ${group.label}`);
          }
        }
      }

      // Build aligned entry
      const subValue = sub(value);
      if (useColumnAlignment && maxFieldLen > 0) {
        const padding = " ".repeat(maxFieldLen - field.length);
        lines.push(`    ("${field}",${padding} "${subValue}"),`);
      } else {
        const entry = syntax.entry
          .replace("{{field}}", field)
          .replace("{{value}}", subValue);
        lines.push(entry);
      }
    }

    return lines;
  }

  // ── Block boundary ─────────────────────────────────────────
  out.push(`// ${eq}`);
  out.push(`// METADATA BLOCK [METADATA]`);
  out.push(`// ${eq}`);

  // ── Comment header (Key, Purpose) ──────────────────────────
  out.push(`//`);
  for (const comment of fillContent.metadataComment) {
    out.push(sub(comment));
  }
  out.push(`//`);
  out.push("");

  // ── PRAGMA section header ──────────────────────────────────
  const ig = fillContent.identityGroups;
  if (useSectionHeaders && ig) {
    out.push(`// ${dash}`);
    out.push(ig.sectionHeaders.pragma);
    out.push(`// ${dash}`);
    out.push("");
  }

  // ── Docstring ──────────────────────────────────────────────
  if (useDocstrings && ig) {
    const pragmaDocstring = ig.pragma.find((g) => g.docstring)?.docstring;
    if (pragmaDocstring) {
      out.push(pragmaDocstring);
    }
  }

  // ── PRAGMA static (I1-I4 identity) ─────────────────────────
  out.push(fillContent.identitySyntax.pragma.declaration);
  out.push(...emitEntries(
    fillContent.pragmaEntries,
    fillContent.identitySyntax.pragma,
    ig?.pragma,
  ));
  out.push(fillContent.identitySyntax.pragma.close);
  out.push("");

  // ── METADATA section header ────────────────────────────────
  if (useSectionHeaders && ig) {
    out.push(`// ${dash}`);
    out.push(ig.sectionHeaders.metadata);
    out.push(`// ${dash}`);
    out.push("");
  }

  // ── Docstring ──────────────────────────────────────────────
  if (useDocstrings && ig) {
    const metaDocstring = ig.metadata.find((g) => g.docstring)?.docstring;
    if (metaDocstring) {
      out.push(metaDocstring);
    }
  }

  // ── METADATA static (C1-C7 context) ────────────────────────
  out.push(fillContent.identitySyntax.metadata.declaration);
  out.push(...emitEntries(
    fillContent.metadataEntries,
    fillContent.identitySyntax.metadata,
    ig?.metadata,
  ));
  out.push(fillContent.identitySyntax.metadata.close);
  out.push("");

  // ── END boundary ───────────────────────────────────────────
  out.push(`// ${eq}`);
  out.push(`// END METADATA [END]`);
  out.push(`// ${eq}`);

  return out;
}

/**
 * Generate form-aware Reserved Omission lines.
 *
 * The pragma declares what the file IS. The form schema defines what sections
 * are REQUIRED, AVAILABLE, and RESERVED for that form. Reserved Omission
 * should reflect this intelligence:
 *
 *   - Absent REQUIRED sections: Omitted from RO (the linter catches them).
 *   - Absent AVAILABLE sections: "Not needed in this module"
 *   - RESERVED sections: Grouped by reason. When N sections share the same
 *     whyReserved text (like 9 test sections), they collapse to one line.
 */
function buildFormAwareReservedOmission(
  absentSections: FormSectionConstraint[],
  reservedSections: FormReservedSection[],
  formName?: string,
  mode: "strict" | "balance" | "growth" = "strict",
): string[] {
  const lines: string[] = [];
  const useGrouped = mode === "strict";

  // 1. Absent AVAILABLE sections — "Not needed in this module"
  //    Skip REQUIRED sections — the linter handles missing required sections.
  const absentAvailable = absentSections.filter((s) => s.status === "AVAILABLE");

  if (absentAvailable.length > 0) {
    if (useGrouped) {
      // Strict mode: group header + comma-separated list
      lines.push(`// Available (not needed in this module):`);
      const tagList = absentAvailable.map((s) => tagToLabel(s.tag)).join(", ");
      lines.push(`//   ${tagList}`);
    } else {
      // Balance/growth: flat list
      for (const section of absentAvailable) {
        lines.push(`//   ${tagToLabel(section.tag)} — Not needed in this module`);
      }
    }
  }

  // 2. RESERVED sections — group by whyReserved to avoid noise.
  const grouped = new Map<string, FormReservedSection[]>();
  for (const section of reservedSections) {
    const key = section.whyReserved;
    const existing = grouped.get(key);
    if (existing) {
      existing.push(section);
    } else {
      grouped.set(key, [section]);
    }
  }

  if (grouped.size > 0) {
    if (useGrouped && absentAvailable.length > 0) {
      // Blank comment separator between Available and Reserved groups
      lines.push(`//`);
    }

    if (useGrouped) {
      const formLabel = formName ?? "this";
      lines.push(`// Reserved (structural — not used in ${formLabel} form):`);
    }

    // Compute max tag label width for alignment in strict mode
    const allReserved = [...grouped.values()].flat();
    const maxTagLen = useGrouped
      ? Math.max(...allReserved.map((s) => tagToLabel(s.tag).length))
      : 0;

    for (const [reason, sections] of grouped) {
      if (sections.length === 1) {
        const label = tagToLabel(sections[0]!.tag);
        if (useGrouped) {
          // Padded tag + em-dash + first sentence of reason
          const padding = " ".repeat(maxTagLen - label.length);
          const shortReason = reason.split(".")[0]!.trim();
          lines.push(`//   ${label}${padding} \u2014 ${shortReason}.`);
        } else {
          lines.push(`//   ${label} \u2014 Reserved: ${reason}`);
        }
      } else {
        // Grouped entry — comma-separated tags on one line
        const tags = sections.map((s) => tagToLabel(s.tag)).join(", ");
        if (useGrouped) {
          const shortReason = reason.split(".")[0]!.trim();
          lines.push(`//   ${tags} \u2014 ${shortReason}.`);
        } else {
          lines.push(`//   ${tags} \u2014 Reserved: ${reason}`);
        }
      }
    }
  }

  return lines;
}

/**
 * Map a SETUP chunk to its subsection tag.
 */
function chunkToSetupSection(chunk: CodeChunk): string {
  switch (chunk.category) {
    case "import": return "Imports";
    case "const": return "Constants";
    case "static": return "Statics";
    case "type_def": return "CoreTypes";
    case "trait_def": return "TraitDefs";
    case "reexport": return "Modules";
    case "mod_decl": return "Modules";
    case "macro_decl": return "Macros";
    default: return "Imports"; // fallback
  }
}

/**
 * Map a BODY chunk to its section tag.
 * Without AST analysis, we use simple heuristics.
 */
function chunkToBodySection(chunk: CodeChunk): BodySection {
  switch (chunk.category) {
    case "trait_impl": return "TraitImplementations";
    case "impl_block": return "CoreLogic";
    case "fn_decl": return "FreeFunctions";
    default: return "CoreLogic";
  }
}

/**
 * Generate the structural scaffold for an unstructured Rust file.
 *
 * This is Transform 0 — the DAR Recover step. Takes a file with a pragma
 * but no block boundaries and produces the full 4-block structure.
 *
 * Returns null if no scaffolding is needed (file already has blocks).
 */
async function structuralScaffoldRust(
  filePath: string,
  lines: string[],
  opts: TransformOptions,
  preservedClosing?: ClosingFieldValues,
): Promise<{ lines: string[]; results: LintResult[] } | null> {
  const results: LintResult[] = [];

  // Parse pragma to identify form
  const pragmaLine = lines.find((l) => /^\s*\/\/\s+#!omni\b/.test(l.trim()));
  if (!pragmaLine) return null;

  // Extract form from pragma args (e.g., -module, -library)
  const pragmaArgs = pragmaLine.replace(/^.*#!omni\s+/, "").trim().split(/\s+/)
    .flatMap((a: string) => a.replace(/^-+/, "").split(",")).filter(Boolean);
  const subtype = pragmaArgs.find((a: string) => a in (_rustRules?.subtypeDefinitions ?? {}));

  // Load form constraints
  const formConstraints = await loadFormConstraints("rust", subtype || "bare-bone");

  // Parse file into semantic chunks
  const chunks = parseCodeChunks(lines);

  // Classify chunks into blocks
  const metadataChunks: CodeChunk[] = [];
  const setupChunks: Map<string, CodeChunk[]> = new Map();
  const bodyChunks: Map<string, CodeChunk[]> = new Map();
  const closingTestChunks: CodeChunk[] = [];
  const closingMainChunks: CodeChunk[] = [];
  const orphanComments: CodeChunk[] = []; // comments/separators between sections

  for (const chunk of chunks) {
    const target = chunkToBlock(chunk);

    if (target === "METADATA") {
      metadataChunks.push(chunk);
    } else if (target === "SETUP") {
      const section = chunkToSetupSection(chunk);
      if (!setupChunks.has(section)) setupChunks.set(section, []);
      setupChunks.get(section)!.push(chunk);
    } else if (target === "BODY") {
      if (chunk.category === "separator" || chunk.category === "blank") {
        orphanComments.push(chunk);
        continue;
      }
      // Comments that precede code are already attached to their items
      // by parseCodeChunks. Standalone comments go to the section they're near.
      if (chunk.category === "comment") {
        orphanComments.push(chunk);
        continue;
      }
      const section = chunkToBodySection(chunk);
      if (!bodyChunks.has(section)) bodyChunks.set(section, []);
      bodyChunks.get(section)!.push(chunk);
    } else if (target === "CLOSING") {
      if (chunk.category === "test_module") {
        closingTestChunks.push(chunk);
      } else if (chunk.category === "main_fn") {
        closingMainChunks.push(chunk);
      }
    }
  }

  // ── Step-by-step writer (--steps) ──────────────────────────────
  // When enabled, writes each scaffold phase to a .steps/ directory.
  // This reveals what the transformer classified and assembled at each
  // layer — critical for tuning aggression (too much stripped vs too
  // little formatted).
  const stepsDir = opts.steps ? filePath + ".steps" : null;
  if (stepsDir) {
    try { await Deno.mkdir(stepsDir, { recursive: true }); } catch { /* exists */ }

    // Step 0: Raw input (what the scaffold received — after strip if --force)
    await Deno.writeTextFile(`${stepsDir}/00-raw-input.rs`, lines.join("\n"));

    // Step 1: Classification report — what each chunk became
    const classReport: string[] = [
      `// Classification Report for ${filePath}`,
      `// Generated by cws-struct transform --steps`,
      `//`,
      `// METADATA chunks: ${metadataChunks.length}`,
    ];
    for (const c of metadataChunks) {
      classReport.push(`//   [${c.category}] ${c.lines[0]?.trim().substring(0, 70)}`);
    }
    classReport.push(`//`);
    classReport.push(`// SETUP sections: ${setupChunks.size}`);
    for (const [tag, chunks] of setupChunks) {
      classReport.push(`//   ${tag}: ${chunks.length} chunk(s)`);
      for (const c of chunks) {
        classReport.push(`//     [${c.category}] ${c.lines[0]?.trim().substring(0, 60)}`);
      }
    }
    classReport.push(`//`);
    classReport.push(`// BODY sections: ${bodyChunks.size}`);
    for (const [tag, chunks] of bodyChunks) {
      classReport.push(`//   ${tag}: ${chunks.length} chunk(s)`);
      for (const c of chunks) {
        classReport.push(`//     [${c.category}] ${c.lines[0]?.trim().substring(0, 60)}`);
      }
    }
    classReport.push(`//`);
    classReport.push(`// CLOSING: ${closingTestChunks.length} test(s), ${closingMainChunks.length} main(s)`);
    classReport.push(`//`);
    classReport.push(`// Orphaned (dropped): ${orphanComments.length} chunk(s)`);
    for (const c of orphanComments) {
      classReport.push(`//   [${c.category}] ${c.lines[0]?.trim().substring(0, 60)}`);
    }
    await Deno.writeTextFile(`${stepsDir}/01-classification.txt`, classReport.join("\n"));

    results.push(info(filePath, "transform/steps",
      `Step-by-step output: ${stepsDir}/`));
  }

  // ── Build the output ─────────────────────────────────────────────
  const eq = "=".repeat(BLOCK_SEPARATOR_WIDTH);
  const dash = "─".repeat(SUBSECTION_SEPARATOR_WIDTH);
  const out: string[] = [];

  // --- METADATA block ---
  // Pragma and doc comments come before the block boundary
  for (const chunk of metadataChunks) {
    out.push(...chunk.lines);
  }

  // Ensure blank line before METADATA block
  if (out.length > 0 && out[out.length - 1]!.trim() !== "") {
    out.push("");
  }

  // Full METADATA block with PRAGMA + METADATA statics from schema fill_content.
  // The METADATA block is WHO the file IS — identity before content.
  // If fill_content is available, generate the full block. Otherwise fall back
  // to a minimal stub (forward-compatible with schemas that lack fill_content).
  const fillContent = _rustRules?.fillContent;
  if (fillContent) {
    out.push(...buildMetadataBlock(metadataChunks, filePath, subtype, fillContent, "strict", lines));
  } else {
    out.push(`// ${eq}`);
    out.push(`// METADATA BLOCK [METADATA]`);
    out.push(`// ${eq}`);
    out.push(`//`);
    out.push(`// TODO: Add PRAGMA static and METADATA static identity fields.`);
    out.push(`// See seed/code/L0/rust/module.rs for the template pattern.`);
    out.push(`//`);
    out.push("");
    out.push(`// ${eq}`);
    out.push(`// END METADATA [END]`);
    out.push(`// ${eq}`);
  }
  out.push("");

  // Step 2: After METADATA
  if (stepsDir) {
    await Deno.writeTextFile(`${stepsDir}/02-metadata.rs`, out.join("\n"));
  }

  // --- SETUP block ---
  out.push(`// ${eq}`);
  out.push(`// SETUP BLOCK [SETUP]`);
  out.push(`// ${eq}`);

  // Emit present SETUP sections with proper headers
  if (formConstraints) {
    const setupSections = formConstraints.SETUP.can
      .sort((a, b) => a.position - b.position);
    const presentSections = new Set(setupChunks.keys());
    const emittedSections = new Set<string>();

    // Block overview — matches template pattern
    const setupIndex = buildSectionIndex(
      formConstraints.SETUP.can, formConstraints.SETUP.cannot,
    );
    out.push(`//`);
    out.push(`// SETUP makes things EXIST. BODY makes things HAPPEN.`);
    out.push(`//`);
    out.push(`// Section order (dependency chain — each layer uses only what's above):`);
    out.push(`//`);
    emitSectionIndex(out, setupIndex);
    out.push("");

    // Emit sections that have content — numbered headers
    for (const section of setupSections) {
      if (presentSections.has(section.tag)) {
        const label = tagToLabel(section.tag);
        out.push(`// ${dash}`);
        out.push(`// ${section.position}. ${label}`);
        out.push(`// ${dash}`);
        out.push("");
        for (const chunk of setupChunks.get(section.tag)!) {
          // Trim trailing blank lines from chunk (use handler may absorb trailing blanks)
          const trimmed = chunk.lines.slice();
          while (trimmed.length > 0 && trimmed[trimmed.length - 1]!.trim() === "") {
            trimmed.pop();
          }
          out.push(...trimmed);
        }
        out.push("");
        emittedSections.add(section.tag);
      }
    }

    // Also emit any content that didn't map to a known section
    for (const [section, sChunks] of setupChunks) {
      if (!emittedSections.has(section)) {
        const label = tagToLabel(section);
        out.push(`// ${dash}`);
        out.push(`// ${label}`);
        out.push(`// ${dash}`);
        out.push("");
        for (const chunk of sChunks) {
          const trimmed = chunk.lines.slice();
          while (trimmed.length > 0 && trimmed[trimmed.length - 1]!.trim() === "") {
            trimmed.pop();
          }
          out.push(...trimmed);
        }
        out.push("");
        emittedSections.add(section);
      }
    }

    // Reserved Omission — form-aware: pragma tells us what this file IS,
    // form schema tells us what's required/available/reserved for that form.
    const absentSetup = setupSections
      .filter((s) => !emittedSections.has(s.tag));
    const reservedSetup = formConstraints.SETUP.cannot;
    const roSetupLines = buildFormAwareReservedOmission(absentSetup, reservedSetup, subtype ?? undefined);

    if (roSetupLines.length > 0) {
      out.push(`// ${dash}`);
      out.push(`// Reserved Omission`);
      out.push(`// ${dash}`);
      out.push(...roSetupLines);
      out.push("");
    }
  } else {
    // No form constraints — just emit what we have
    for (const [, sChunks] of setupChunks) {
      for (const chunk of sChunks) {
        out.push(...chunk.lines);
      }
    }
    out.push("");
  }

  out.push(`// ${eq}`);
  out.push(`// END SETUP [END]`);
  out.push(`// ${eq}`);
  out.push("");

  // Step 3: After SETUP
  if (stepsDir) {
    await Deno.writeTextFile(`${stepsDir}/03-setup.rs`, out.join("\n"));
  }

  // --- BODY block ---
  out.push(`// ${eq}`);
  out.push(`// BODY BLOCK [BODY]`);
  out.push(`// ${eq}`);

  if (formConstraints) {
    const bodySections = formConstraints.BODY.can
      .sort((a, b) => a.position - b.position);
    const presentBodySections = new Set(bodyChunks.keys());
    const emittedBodySections = new Set<string>();

    // Block overview — matches template pattern
    const bodyIndex = buildSectionIndex(
      formConstraints.BODY.can, formConstraints.BODY.cannot,
    );
    out.push(`//`);
    out.push(`// BODY makes things HAPPEN. SETUP made things EXIST.`);
    out.push(`//`);
    out.push(`// Subsection order follows the type lifecycle — from identity through`);
    out.push(`// creation, operation, observation, to output.`);
    out.push(`//`);
    emitSectionIndex(out, bodyIndex);
    out.push("");

    // Emit sections that have content — numbered headers
    for (const section of bodySections) {
      if (presentBodySections.has(section.tag)) {
        const label = tagToLabel(section.tag);
        out.push(`// ${dash}`);
        out.push(`// ${section.position}. ${label}`);
        out.push(`// ${dash}`);
        out.push("");
        for (const chunk of bodyChunks.get(section.tag)!) {
          out.push(...chunk.lines);
          out.push("");
        }
        emittedBodySections.add(section.tag);
      }
    }

    // Emit unmapped body content
    for (const [section, bChunks] of bodyChunks) {
      if (!emittedBodySections.has(section)) {
        const label = tagToLabel(section);
        out.push(`// ${dash}`);
        out.push(`// ${label}`);
        out.push(`// ${dash}`);
        out.push("");
        for (const chunk of bChunks) {
          out.push(...chunk.lines);
          out.push("");
        }
        emittedBodySections.add(section);
      }
    }

    // Reserved Omission — form-aware (same as SETUP)
    const absentBody = bodySections
      .filter((s) => !emittedBodySections.has(s.tag));
    const reservedBody = formConstraints.BODY.cannot;
    const roBodyLines = buildFormAwareReservedOmission(absentBody, reservedBody, subtype ?? undefined);

    if (roBodyLines.length > 0) {
      out.push(`// ${dash}`);
      out.push(`// Reserved Omission`);
      out.push(`// ${dash}`);
      out.push(...roBodyLines);
      out.push("");
    }
  } else {
    // No form constraints — emit body chunks directly
    for (const [, bChunks] of bodyChunks) {
      for (const chunk of bChunks) {
        out.push(...chunk.lines);
      }
    }
    out.push("");
  }

  out.push(`// ${eq}`);
  out.push(`// END BODY [END]`);
  out.push(`// ${eq}`);
  out.push("");

  // Step 4: After BODY
  if (stepsDir) {
    await Deno.writeTextFile(`${stepsDir}/04-body.rs`, out.join("\n"));
  }

  // --- CLOSING block ---
  out.push(`// ${eq}`);
  out.push(`// CLOSING BLOCK [CLOSING]`);
  out.push(`// ${eq}`);

  // Block overview — the CLOSING block discovered bracket format first
  emitClosingOverview(out);
  out.push("");

  // Cv — Tests
  const cvLabel = CLOSING_ZONE_LABELS["Cv"] ?? "Cv";
  out.push(`// ${dash}`);
  out.push(`// ${cvLabel}`);
  out.push(`// ${dash}`);
  out.push("");
  if (closingTestChunks.length > 0) {
    for (const chunk of closingTestChunks) {
      out.push(...chunk.lines);
    }
  } else {
    out.push(`// No tests yet.`);
  }
  out.push("");

  // Ce — Entry point
  const ceLabel = CLOSING_ZONE_LABELS["Ce"] ?? "Ce";
  out.push(`// ${dash}`);
  out.push(`// ${ceLabel}`);
  out.push(`// ${dash}`);
  out.push("");
  if (closingMainChunks.length > 0) {
    for (const chunk of closingMainChunks) {
      out.push(...chunk.lines);
    }
  } else {
    out.push(`// No entry point — this is a module.`);
  }
  out.push("");

  // X1 — Modification Policy (preserve existing values when --force re-scaffold)
  const pc = preservedClosing;
  const x1Label = CLOSING_ZONE_LABELS["X1"] ?? "X1";
  out.push(`// ${dash}`);
  out.push(`// ${x1Label}`);
  out.push(`// ${dash}`);
  out.push(`// policy: Modification guidelines`);
  out.push(`// scripture: ${pc?.policyScripture ?? `"Proverbs 22:28 (WEB) — Don't move the ancient boundary stone."`}`);
  out.push(`// never: ${pc?.never ?? "Break 4-block structure, Remove block boundaries, Remove identity statics"}`);
  out.push(`// careful: ${pc?.careful ?? "Function signatures (breaks callers), Error types (breaks match arms)"}`);
  out.push(`// safe: ${pc?.safe ?? "Function bodies, New functions, Comments, Tests"}`);
  out.push("");

  // X4 — Reference (preserve existing values when --force re-scaffold)
  const x4Label = CLOSING_ZONE_LABELS["X4"] ?? "X4";
  out.push(`// ${dash}`);
  out.push(`// ${x4Label}`);
  out.push(`// ${dash}`);
  const filename = filePath.split("/").pop() ?? "unknown";
  out.push(`// related_files: ${pc?.relatedFiles ?? "[Cargo.toml, lib.rs]"}`);
  out.push(`// validate: ${pc?.validate ?? "cargo test"}`);
  out.push("");

  // X5 — Closing Note (preserve existing values when --force re-scaffold)
  const x5Label = CLOSING_ZONE_LABELS["X5"] ?? "X5";
  out.push(`// ${dash}`);
  out.push(`// ${x5Label}`);
  out.push(`// ${dash}`);
  out.push(`// note: ${pc?.note ?? `"${filename} — structural scaffold generated by cws-struct transform."`}`);
  out.push(`// scripture: ${pc?.scripture ?? `"Psalm 139:7-8 — Whither shall I go from thy spirit?"`}`);
  out.push(`// anchor: ${pc?.anchor ?? `"TODO: Module purpose statement."`}`);
  out.push("");

  // Reserved Omission for CLOSING — form-aware (same as SETUP/BODY)
  if (formConstraints) {
    const closingSections = formConstraints.CLOSING.can
      .sort((a, b) => a.position - b.position);
    const emittedClosing = new Set(["Cv", "Ce", "X1", "X4", "X5"]);
    const absentClosing = closingSections.filter((s) => !emittedClosing.has(s.tag));
    const reservedClosing = formConstraints.CLOSING.cannot;
    const roClosingLines = buildFormAwareReservedOmission(absentClosing, reservedClosing, subtype ?? undefined);

    if (roClosingLines.length > 0) {
      out.push(`// ${dash}`);
      out.push(`// Reserved Omission`);
      out.push(`// ${dash}`);
      out.push(...roClosingLines);
      out.push("");
    }
  }

  out.push(`// ${eq}`);
  out.push(`// END CLOSING [END]`);
  out.push(`// ${eq}`);

  // Step 5: Final output (complete file)
  if (stepsDir) {
    await Deno.writeTextFile(`${stepsDir}/05-closing.rs`, out.join("\n"));
  }

  // ── Report ──────────────────────────────────────────────────────
  const setupCount = setupChunks.size;
  const bodyCount = bodyChunks.size;
  const testCount = closingTestChunks.length;

  results.push(info(filePath, "transform/scaffold",
    `Structural scaffold: 4 blocks, ${setupCount} SETUP section(s), ${bodyCount} BODY section(s), ${testCount} test module(s)`));

  if (opts.dryRun) {
    results.push(info(filePath, "transform/scaffold",
      `Would restructure ${lines.length} lines → ${out.length} lines with full 4-block structure`));
  } else {
    results.push(info(filePath, "transform/scaffold",
      `Restructured ${lines.length} lines → ${out.length} lines with full 4-block structure`));
  }

  return { lines: out, results };
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
function reorderClosingZones(
  closingContent: string[],
  schemaZones: ReadonlyArray<{ tag: string; kind: "code" | "doc"; pattern: RegExp }>,
): string[] | null {
  // Parse into zone chunks. A zone starts with its subsection separator.
  interface ZoneChunk {
    tag: string;
    kind: "code" | "doc";
    lines: string[];
    canonicalIdx: number;
  }

  // Canonical order derived from schema zone ordering
  const canonicalOrder = schemaZones.map((z) => z.tag);

  // Find zone boundaries in the content
  const zones: ZoneChunk[] = [];
  let preambleLines: string[] = [];  // Lines before the first zone
  let currentZone: ZoneChunk | null = null;

  for (let i = 0; i < closingContent.length; i++) {
    const trimmed = closingContent[i]!.trim();

    // Check if this line starts a new zone (look for zone tag after a separator)
    let matchedZone: { tag: string; kind: "code" | "doc" } | null = null;
    for (const zone of schemaZones) {
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
 * Transform 0 — Structural Scaffold:
 *   When a file has a pragma but NO block boundaries, generates the full
 *   4-block structure: parses existing code into semantic chunks, places
 *   them into correct blocks/sections, adds Reserved Omission entries
 *   for absent sections. This IS the Recover step of DAR.
 *
 * Transforms 1-8 — Cosmetic / organizational:
 *   1. Fix block separator widths (= → 76)
 *   2. Fix Unicode subsection separator widths (─ → 74)
 *   3. Convert ASCII dash subsection separators (----) to Unicode (────)
 *   4. Normalize subsection separator comment prefix to standard `// `
 *   5. Move #[cfg(test)] from BODY to CLOSING Cv zone
 *   6. Move fn main() from BODY to CLOSING Ce zone
 *   7. Reorder CLOSING zones to canonical order (Cv → Ce → Cc → X1-X6)
 *   8. Scaffold missing SETUP subsection headers (--extensions only)
 */
async function transformRustFile(
  filePath: string,
  opts: TransformOptions,
): Promise<LintResult[]> {
  await ensureRustRules();
  const { dryRun, extensions } = opts;
  const results: LintResult[] = [];

  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  let lines = text.split("\n");
  let modified = false;
  let wouldModify = false;

  // --- Transform 0: Structural scaffold (DAR Recover) ---
  // When a file has a pragma but NO block boundaries, generate the full
  // 4-block structure. This is the heavy-lifting transform that takes
  // an unstructured Rust file and gives it proper form.
  //
  // With --force: strip existing block structure first, then re-scaffold.
  // This lets us test formatting against files that already have structure.
  {
    const hasBlocks = findBlocks(lines).length > 0;
    // Detect pragma: shebang (// #!omni) OR legacy directive (//omni:code).
    // The shebang is the company standard; //omni:code is accepted for
    // backward compatibility so the transformer can upgrade old files.
    const pragmaLine = lines.find((l) => {
      const t = l.trim();
      return /^\s*\/\/\s+#!omni\b/.test(t) || /^\/\/omni:code\b/.test(t);
    });

    // When force is set, extract closing values, then strip so scaffold can rebuild.
    let preservedClosing: ClosingFieldValues | undefined;
    if (opts.force && pragmaLine && hasBlocks) {
      preservedClosing = extractClosingFields(lines);
      lines = stripBlockStructure(lines);
      results.push(info(filePath, "transform/force-strip",
        "Stripped existing block structure for re-scaffold (--force)"));
    }

    const hasBlocksNow = findBlocks(lines).length > 0;
    if (pragmaLine && !hasBlocksNow) {
      const scaffoldResult = await structuralScaffoldRust(filePath, lines, opts, preservedClosing);
      if (scaffoldResult) {
        if (dryRun) {
          wouldModify = true;
          results.push(...scaffoldResult.results);
        } else {
          lines = scaffoldResult.lines;
          modified = true;
          results.push(...scaffoldResult.results);
        }
      }
    }
  }

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
        _rustRules!.closingData.zones,
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

  // --- Transform 8: Scaffold missing SETUP subsection headers ---
  //
  // When content exists in SETUP without a subsection header above it,
  // scaffold the appropriate header. Only runs with --extensions (k-factor:
  // errors only → scaffold toward fullness).
  //
  if (extensions) {
    const blocks = findBlocks(lines);
    const setupBlock = blocks.find((b) => b.name === "SETUP");
    if (setupBlock) {
      const setupRange = findBlockRange(lines, "SETUP");
      if (setupRange) {
        const setupLines = lines.slice(setupRange.contentStart, setupRange.contentEnd);
        const subsections = getSubsectionRanges(setupLines);

        // Find declarations that appear before the first subsection header
        const firstSubIdx = subsections.length > 0 ? subsections[0]!.startIdx : setupLines.length;
        const orphanedKinds = new Map<string, number>();

        for (let i = 0; i < firstSubIdx; i++) {
          const trimmed = setupLines[i]!.trim();
          if (trimmed === "" || trimmed.startsWith("//")) continue;
          const kind = classifyLine(trimmed);
          const targetSub = SUBSECTION_PLACEMENT[kind];
          if (targetSub && !orphanedKinds.has(targetSub)) {
            orphanedKinds.set(targetSub, i);
          }
        }

        if (orphanedKinds.size > 0) {
          const canonicalOrder = SETUP_SUBSECTIONS.map((s) => s.tag);
          const sorted = [...orphanedKinds.entries()]
            .sort((a, b) => canonicalOrder.indexOf(a[0]) - canonicalOrder.indexOf(b[0]));

          let insertCount = 0;
          for (let si = sorted.length - 1; si >= 0; si--) {
            const [tag] = sorted[si]!;
            const num = canonicalOrder.indexOf(tag) + 1;
            const header = [
              "",
              `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
              `// ${num}. ${tag}`,
              `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
              "",
            ];
            const absoluteIdx = setupRange.contentStart + sorted[si]![1];

            if (dryRun) {
              wouldModify = true;
              results.push(info(filePath, "transform/reserve-scaffold",
                `Would scaffold "${tag}" subsection header before line ${absoluteIdx + 1}`));
            } else {
              lines.splice(absoluteIdx, 0, ...header);
              modified = true;
              insertCount++;
            }
          }
          if (!dryRun && insertCount > 0) {
            results.push(info(filePath, "transform/reserve-scaffold",
              `Scaffolded ${insertCount} missing SETUP subsection header(s)`));
          }
        }
      }
    }
  }

  // --- Write if modified ---
  if (modified && !dryRun) {
    await Deno.writeTextFile(filePath, lines.join("\n"));
    results.push(info(filePath, "transform/written", "File updated"));
  } else if (!modified && !wouldModify) {
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
  await ensureRustRules();
  // NOTE: No early guard here — acts() creates pass actions when no
  // failures exist, so a perfectly clean file gets 100/100 (not 0/100).
  // The allActions.length === 0 guard below handles the edge case where
  // no checks apply at all.

  // ── File type detection ────────────────────────────────────────
  const basename = filePath.split("/").pop() ?? "";
  const isModuleFile = basename === "mod.rs";
  const isCrateRoot = basename === "lib.rs" || basename === "main.rs";
  let isTemplate = false;
  let hasForm = false;
  try {
    const content = await Deno.readTextFile(filePath);
    const firstLines = content.split("\n").slice(0, 15);
    isTemplate = firstLines.some((l) =>
      /^\/\/\s+#!omni\s+template\b/.test(l.trim()));
    // Check for form declaration in pragma (e.g., --library, --module)
    // Form checks only run when a form is declared — health score should
    // include form actions only when form validation actually fired.
    const pragmaLine = firstLines.find((l) =>
      /^\s*\/\/\s+#!omni\b/.test(l.trim()));
    if (pragmaLine && _rustRules) {
      const args = pragmaLine.replace(/^.*#!omni\s+/, "").trim().split(/\s+/)
        .flatMap((a: string) => a.replace(/^-+/, "").split(",")).filter(Boolean);
      hasForm = args.some((a: string) => a in _rustRules!.subtypeDefinitions);
    }
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

  /** Create atomic actions: 1 aligned if no failure, N misaligned if N results. */
  function acts(check: string, container: string, block: string): AtomicAction[] {
    const failures = failuresByRule.get(check);
    if (failures && failures.length > 0) {
      return failures.map((f) => ({
        check, container, block,
        direction: -1 as const,
        impact: f.severity,
        reason: f.message,
      }));
    }
    return [{ check, container, block, direction: 1 as const }];
  }

  /**
   * Cascade: set aligned actions to neutral (not assessable).
   * Root cause already carries the weight — children don't pile on.
   */
  function cascade(groups: AtomicAction[][], reason: string): void {
    for (const group of groups) {
      for (const a of group) {
        if (a.direction > 0) {
          (a as { direction: -1 | 0 | 1 }).direction = 0;
          a.impact = "info";
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

  // Field VALUE checks (content validation — are values correct?)
  const fieldValueActions: AtomicAction[] = [];
  if (!isModuleFile) {
    for (const rule of PRAGMA_CONTENT_RULES) {
      fieldValueActions.push(
        ...acts(`value/PRAGMA/${rule.field}`, "field-values", "metadata"));
    }
    for (const rule of METADATA_CONTENT_RULES) {
      fieldValueActions.push(
        ...acts(`value/METADATA/${rule.field}`, "field-values", "metadata"));
    }
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
      [directiveActions, identityActions, fieldValueActions, commentActions, docActions, templateDerivedActions],
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

  // ── SUB-CASCADE: missing statics → field checks neutral ────────
  // Can't assess fields if the static is missing — neutral, not failed.
  if (failuresByRule.has("identity/pragma-static")) {
    for (const a of identityActions) {
      if (a.direction > 0 && a.check.startsWith("identity/PRAGMA/")) {
        (a as { direction: -1 | 0 | 1 }).direction = 0;
        a.impact = "info";
        a.reason = "PRAGMA static missing — field check cannot run";
      }
    }
    // Content checks also depend on PRAGMA static existing
    for (const a of fieldValueActions) {
      if (a.direction > 0 && a.check.startsWith("value/PRAGMA/")) {
        (a as { direction: -1 | 0 | 1 }).direction = 0;
        a.impact = "info";
        a.reason = "PRAGMA static missing — content check cannot run";
      }
    }
  }
  if (failuresByRule.has("identity/metadata-static")) {
    for (const a of identityActions) {
      if (a.direction > 0 && a.check.startsWith("identity/METADATA/")) {
        (a as { direction: -1 | 0 | 1 }).direction = 0;
        a.impact = "info";
        a.reason = "METADATA static missing — field check cannot run";
      }
    }
    // Content checks also depend on METADATA static existing
    for (const a of fieldValueActions) {
      if (a.direction > 0 && a.check.startsWith("value/METADATA/")) {
        (a as { direction: -1 | 0 | 1 }).direction = 0;
        a.impact = "info";
        a.reason = "METADATA static missing — content check cannot run";
      }
    }
  }

  // ── SUB-CASCADE: missing identity fields → content checks neutral ────
  // Can't check field VALUE if the field doesn't EXIST.
  for (const a of fieldValueActions) {
    if (a.direction > 0) {
      // value/PRAGMA/I1.key → check identity/PRAGMA/I1.key
      const fieldPath = a.check.replace("value/", "identity/");
      if (failuresByRule.has(fieldPath)) {
        (a as { direction: -1 | 0 | 1 }).direction = 0;
        a.impact = "info";
        a.reason = "Field missing — content check cannot run";
      }
    }
  }

  // ── Collect all actions ───────────────────────────────────────
  const allActions = [
    ...blockActions, ...sepActions,
    ...directiveActions, ...identityActions, ...fieldValueActions,
    ...commentActions, ...docActions, ...templateDerivedActions,
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

// ---------------------------------------------------------------------------
// RustAdapter — language-specific adapter for future generic handler
// ---------------------------------------------------------------------------
//
// Captures everything that differs between Rust and other languages.
// A future GenericCode4BlockHandler takes a LanguageAdapter + Code4BlockRules
// and produces a full handler. Today: the adapter packages Rust-specific
// functions alongside the existing handler. Addition, not modification.
//
// new format = new LanguageAdapter + new schema. Not new engine code.
//

/**
 * Rust language adapter — implements LanguageAdapter for 4-block Rust files.
 *
 * Wraps Rust-specific functions (classifier, identity parser, directive finder,
 * zone finders, context builder) into the generalization interface. Rust has
 * no legacy subsection patterns (unlike Go's //--- prefixes), so
 * enrichSubsectionPatterns is not defined.
 */
export const rustAdapter: LanguageAdapter = {
  format: "rust",
  extensions: [".rs"],
  knownCodeDirectives: KNOWN_CODE_DIRECTIVES,

  classifyLine(rawLine: string): string {
    return classifyLine(rawLine);
  },

  parseIdentityFields(lines: string[], varName: string): IdentityField[] {
    return parseStaticFields(lines, varName);
  },

  findOmniDirectives(lines: string[]): Map<string, DirectiveInfo> {
    return findOmniDirectives(lines);
  },

  findTestZone(
    lines: string[], rangeStart: number, rangeEnd: number,
  ): { start: number; end: number } | null {
    return findCfgTestInRange(lines, rangeStart, rangeEnd);
  },

  findMainZone(
    lines: string[], rangeStart: number, rangeEnd: number,
  ): { start: number; end: number } | null {
    return findFnMainInRange(lines, rangeStart, rangeEnd);
  },

  // No enrichSubsectionPatterns — Rust has no legacy //--- prefix patterns.
  // Subsections are used directly from the schema.

  buildContextExtras(filePath: string, lines: string[]): Record<string, unknown> {
    const filename = filePath.split("/").pop() ?? "";
    const isCrateRoot = filename === "lib.rs" || filename === "main.rs";
    const isTemplate = lines.some((l) => /^\/\/\s+#!omni\s+template\b/.test(l.trim()));
    return {
      isCrateRoot,
      isModuleFile: !isCrateRoot && !isTemplate,
    };
  },
};

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
