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
  InspectResult, InspectBlock, InspectSection, InspectContent,
} from "../foundation/mod.ts";
import {
  error, warn, info, policySeverity, getGlobalPolicy,
  computeContainerScore, computeBlockScore, computeHealthScore,
  loadCodeRules, loadFormConstraints, loadCompositionFormConstraints,
  buildFormConstraintsFromRegistry,
} from "../foundation/mod.ts";
import { registerFormat, loadFormContentCached, loadFormStructureCached } from "../engine/mod.ts";
import { cascadeActionGroups, tagLayer } from "../engine/mod.ts";
import {
  isTrace,
  traceEnter, traceExit,
  debugLayerTransition,
  traceResult,
} from "../engine/mod.ts";
import type { TransformPass, TransformContext } from "../engine/mod.ts";
import {
  UNIVERSAL_PASSES,
  reorderClosingZonesPass,
  createCodeMovePass,
  createSubsectionScaffoldPass,
  createSubsectionReorderPass,
  runTransformPipeline,
} from "../engine/mod.ts";

// Shared 4-block types, constants, and functions
import type { BlockPosition, DirectiveInfo, SubsectionRange, IdentityField, LanguageAdapter } from "../shared/mod.ts";
import type { ContentExpectations, BlockCheckSet, BlockLintChain } from "../shared/mod.ts";
import {
  runLintGrid,
  BLOCKS, REQUIRED_DIRECTIVES, RECOMMENDED_DIRECTIVES,
  PRAGMA_CONTENT_RULES, METADATA_CONTENT_RULES,
  BODY_SUBSECTION_PATTERN,
  findBlocks, getBlockLines,
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
  validateICFields,
  validateICFieldContent,
  validateTemplateVsDerived, validateSubtypeConsistency, validateFormatConsistency,
  // Content linting — schema-driven content expectations
  checkBodyContentExpectations as _sharedCheckBodyContent,
  checkClosingContentExpectations as _sharedCheckClosingContent,
  checkSetupContentExpectations as _sharedCheckSetupContent,
  checkMetadataContentExpectations as _sharedCheckMetadataContent,
  // Scaffold — handler uses adapter pattern, shared pipeline handles internals
  extractClosingFields, stripBlockStructure, structuralScaffold,
} from "../shared/mod.ts";
import type {
  BlockTarget, ScaffoldAdapter, ClosingFieldValues, ScaffoldOptions,
} from "../shared/mod.ts";
import { buildConceptContainers } from "../shared/concept-check.ts";
import { loadConceptDetectors } from "../data/concept-detectors.ts";

// Data layer — form registry for form-aware transforms
import { FORM_REGISTRY } from "../data/mod.ts";

// Re-export for tests and schema alignment verification
export { PRAGMA_CONTENT_RULES, METADATA_CONTENT_RULES };
export { validateICFields, validateICFieldContent };
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

import type { Code4BlockRules } from "../foundation/mod.ts";
import { registerCache } from "../foundation/cache-registry.ts";

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
let SETUP_SUBSECTIONS: import("../foundation/types.ts").SubsectionDef[] = [];
// deno-lint-ignore prefer-const
let BODY_CANONICAL_SUBSECTIONS: Record<string, readonly string[]> = {};

// Content expectations — loaded from forms/{form}/rust.jsonc per subtype.
// Populated lazily on demand. Graceful degradation if schema not found.
const _rustContentExpectations = new Map<string, ContentExpectations>();

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

/**
 * Load Rust content expectations for a specific form/subtype.
 *
 * Lazy + cached: first call loads from schema pipeline, subsequent calls
 * return cached result. Returns null if schema not found (graceful degradation).
 */
async function loadRustContentExpectations(
  subtype: string,
): Promise<ContentExpectations | null> {
  const cached = _rustContentExpectations.get(subtype);
  if (cached) return cached;

  try {
    const content = await loadFormContentCached("rust", subtype);
    const exp = content.contentExpectations as ContentExpectations;
    _rustContentExpectations.set(subtype, exp);
    return exp;
  } catch {
    // Tripwire: content schema not found — degrade gracefully
    return null;
  }
}

/** Cached structure schema sections per subtype — ALL blocks. */
interface BlockSectionList { required: string[]; reserved: string[] }
interface FullStructureSections {
  SETUP: BlockSectionList;
  BODY: BlockSectionList;
  CLOSING: BlockSectionList;
}
const _rustStructureSections = new Map<string, FullStructureSections>();

/**
 * Load ALL block section lists from structure schema.
 * Returns required + reserved arrays per block for content linter and health scorer.
 * Tripwire: if structure schema missing, returns undefined (degrade gracefully).
 */
async function loadRustStructureSections(
  subtype: string,
): Promise<FullStructureSections | undefined> {
  const cached = _rustStructureSections.get(subtype);
  if (cached) return cached;

  try {
    const structure = await loadFormStructureCached(subtype);
    const sections: FullStructureSections = {
      SETUP: { required: structure.SETUP.required, reserved: structure.SETUP.reserved },
      BODY: { required: structure.BODY.required, reserved: structure.BODY.reserved },
      CLOSING: { required: structure.CLOSING.required, reserved: structure.CLOSING.reserved },
    };
    _rustStructureSections.set(subtype, sections);
    return sections;
  } catch {
    return undefined; // Tripwire: structure schema missing — degrade
  }
}

/**
 * Clear ALL Rust handler caches. Exported for test isolation.
 * Resets: rules singleton, content expectations, structure sections, sibling cache.
 */
export function clearRustHandlerCaches(): void {
  _rustRules = null;
  _rustContentExpectations.clear();
  _rustStructureSections.clear();
  _rustSiblingCache.clear();
}

/** Known //omni:code directive patterns for Rust (matches schema subtypes). */
registerCache("handlers/rust", clearRustHandlerCaches);
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
  typing: string | null;         // arrow refinement: -module->utility → "utility"
  formConstraints: FormConstraints | null;  // loaded from form schema when subtype is known
  policy: LintPolicy;  // ternary threshold for form checks
}

// ---------------------------------------------------------------------------
// Helpers — Rust-specific (shared helpers imported from ../shared/mod.ts)
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

  // Template vs Derived detection:
  //   // #!omni template  →  key "#!omni:template"  →  TEMPLATE shebang
  //   // #!omni code      →  key "#!omni:code"      →  DERIVED shebang
  //   //omni:code         →  key "//omni:code"       →  format directive (both)
  // A file with the code SHEBANG (// #!omni code) is definitively derived.
  // A file with just the //omni:code DIRECTIVE could be either template or derived.
  // R50-042 fires when a derived file has a leftover template shebang.
  const hasCodeShebang = directives.has("#!omni:code");
  const hasTemplateMarker = directives.has("#!omni:template") ||
    lines.some((l) => /^\/\/\s+#!omni\s+template\b/.test(l.trim()));
  const isTemplate = hasTemplateMarker && !hasCodeShebang;

  const isCrateRoot = filename === "lib.rs" || filename === "main.rs";

  const hasAnyOmni = lines.some((l) => l.trim().startsWith("//omni:")) ||
    lines.some((l) => /^\/\/\s+#!omni\s/.test(l.trim()));

  const hasAnyBlock = lines.some((l) =>
    /^\/\/\s+(METADATA|SETUP|BODY|CLOSING)(\s+BLOCK\s+\[\1\])?\s*$/.test(l.trim())
  );

  // Detect subtype + typing arrow from directives, then PRAGMA I2.subtype if not found.
  // Sources (priority order):
  //   1. //omni:code --rust -<subtype>             (e.g., -module)
  //   2. //omni:code --rust -<subtype>-><typing>   (e.g., -module->utility)
  //   3. #!omni template --rust -<subtype>[-><typing>]
  //   4. PRAGMA I2.subtype field
  let subtype: string | null = null;
  let typing: string | null = null;
  const KNOWN_SUBTYPES = Object.keys(_rustRules!.subtypeDefinitions);

  const codeDirective = directives.get("//omni:code")?.value ?? directives.get("#!omni:code")?.value ?? "";
  const templateDirective = directives.get("#!omni:template")?.value ?? "";

  for (const directive of [codeDirective, templateDirective]) {
    if (!directive) continue;
    // Match: -<subtype> or -<subtype>-><typing>
    const subtypeMatch = directive.match(/-(\w[\w-]*)(?:->(\w[\w-]*))?$/);
    if (subtypeMatch && KNOWN_SUBTYPES.includes(subtypeMatch[1]!)) {
      subtype = subtypeMatch[1]!;
      typing = subtypeMatch[2] ?? null;
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
    typing,
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

// validateICFields — now in shared/code-4block.ts, imported above

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
  // Note: strip leading // from directive names in rule strings to avoid
  // embedded slashes breaking matchRule() pattern matching.
  for (const directive of REQUIRED_DIRECTIVES) {
    if (!ctx.directives.has(directive)) {
      const level = ctx.isCrateRoot ? error : warn;
      const tag = directive.replace(/^\/\//, "");
      results.push(level(file, `directive/${tag}/required`,
        ctx.isCrateRoot
          ? `Missing ${directive} — REQUIRED for crate root`
          : `Missing ${directive}`,
        { line: 1 }));  // directives belong at top of file
    }
  }

  for (const directive of RECOMMENDED_DIRECTIVES) {
    if (!ctx.directives.has(directive)) {
      const tag = directive.replace(/^\/\//, "");
      results.push(warn(file, `directive/${tag}/recommended`, `Missing ${directive} — recommended`,
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

  // Schema keys match severity section: "crate_root" and "module"
  const crateExp = expectations["crate_root"];
  const moduleExp = expectations["module"];

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
      results.push(...validateICFields(file, pragmaFields, PRAGMA_FIELD_REQUIREMENTS, "PRAGMA", "static"));

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
      results.push(...validateICFields(file, metadataFields, METADATA_FIELD_REQUIREMENTS, "METADATA", "static"));
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
// When typing arrow is present (e.g., -module->utility), pre-filter setup data
// to exclude sections irrelevant for that typing.
function checkRequiredSetupSubsections(ctx: RustFileContext): LintResult[] {
  const setupLines = getBlockLines(ctx.lines, ctx.blocks, "SETUP");

  // Typing arrow narrows REQUIRED checks to only typing-required sections.
  // "Available" sections are optional — they should NOT produce required-subsection warnings.
  let setupData = _rustRules!.setupData;
  if (ctx.typing && ctx.subtype) {
    const typingProfile = _rustRules!.typingMaps[ctx.subtype]?.[ctx.typing];
    if (typingProfile) {
      const norm = (s: string) => s.toLowerCase().replace(/\s+/g, "");
      const required = new Set(typingProfile.SETUP.required.map(norm));
      setupData = setupData.filter((s) => required.has(norm(s.tag)));
    }
  }

  return _sharedCheckRequiredSetup(
    setupLines, SETUP_SUBSECTIONS, setupData, ctx.subtype,
    ctx.blocks, ctx.filePath, ctx.isTemplate,
  );
}

// Check 8b: BODY required subsections for detected subtype.
// When typing arrow is present, narrow what counts as "required" for this variant.
function checkRequiredBodySubsections(ctx: RustFileContext): LintResult[] {
  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  let bodySubtype = ctx.subtype ? _rustRules!.bodyData[ctx.subtype] : undefined;

  // Typing arrow narrows REQUIRED checks: only typing-required sections flagged.
  // module->utility requires only FreeFunctions — other module-required sections
  // become available or irrelevant and should NOT produce required warnings.
  if (ctx.typing && ctx.subtype && bodySubtype) {
    const typingProfile = _rustRules!.typingMaps[ctx.subtype]?.[ctx.typing];
    if (typingProfile) {
      const norm = (s: string) => s.toLowerCase().replace(/\s+/g, "");
      const required = new Set(typingProfile.BODY.required.map(norm));
      bodySubtype = {
        ...bodySubtype,
        subsections: bodySubtype.subsections.filter((s) =>
          required.has(norm(s.tag))
        ),
      };
    }
  }

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

  // Check //omni:code value for derived files.
  // Arrow syntax: -subtype->role is valid if the base -subtype is known.
  // e.g., "--rust -module->utility" is known because "--rust -module" is known.
  const codeInfo = ctx.directives.get("//omni:code");
  if (codeInfo && codeInfo.value !== "") {
    const baseValue = codeInfo.value.replace(/->[\w-]+$/, "");
    const isKnown = KNOWN_CODE_DIRECTIVES.some((k) => baseValue === k);
    if (!isKnown) {
      results.push(
        info(file, "directive/code-format",
          `//omni:code value "${codeInfo.value}" — not a recognized pattern (known: ${KNOWN_CODE_DIRECTIVES.join(", ")})`,
          { line: codeInfo.line }),
      );
    }
  }

  // Check #!omni template value for template files.
  // Same arrow syntax applies to templates.
  const templateInfo = ctx.directives.get("#!omni:template");
  if (templateInfo && templateInfo.value !== "") {
    const baseValue = templateInfo.value.replace(/->[\w-]+$/, "");
    const isKnown = KNOWN_CODE_DIRECTIVES.some((k) => baseValue === k);
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

  let canonical = BODY_CANONICAL_SUBSECTIONS[ctx.subtype];
  if (!canonical) return results;

  // Typing arrow narrows canonical list: -module->utility keeps only relevant sections.
  // The typing map is INCLUSIVE — only required + available sections matter.
  // Anything not mentioned is implicitly irrelevant for this typing.
  if (ctx.typing && ctx.subtype) {
    const typingProfile = _rustRules!.typingMaps[ctx.subtype]?.[ctx.typing];
    if (typingProfile) {
      const norm = (s: string) => s.toLowerCase().replace(/\s+/g, "");
      const relevant = new Set([
        ...typingProfile.BODY.required.map(norm),
        ...typingProfile.BODY.available.map(norm),
      ]);
      canonical = canonical.filter((c) => relevant.has(norm(c)));
    }
  }

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

    const trulyMissing = missing.filter((c) => {
      const cLow = c.toLowerCase();
      // Substring match — "Constructors & Builders" covers canonical "Constructors"
      return !Array.from(roAcknowledged).some((ro) =>
        ro.includes(cLow) || cLow.includes(ro));
    });

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

// NOTE: ClosingFieldValues, extractClosingFields, stripBlockStructure —
// all imported from shared/scaffold.ts (universal across all languages).
//
// Scaffold pipeline: shared/scaffold.ts handles structure, Rust adapter provides
// language-specific parsing/classification via buildRustAdapter().

// Rust-specific stripBlockStructure preserveLine callback:
// Don't strip #[cfg(test)] lines — they're test module markers, not closing zones.
function rustPreserveLine(trimmed: string): boolean {
  return trimmed.includes("#[cfg(test)]");
}

// ---------------------------------------------------------------------------
// Lint orchestrator
// ---------------------------------------------------------------------------

async function lintRustFile(filePath: string): Promise<LintResult[]> {
  traceEnter("lintRustFile", filePath);
  await ensureRustRules();
  let ctx: RustFileContext;
  try {
    ctx = await buildContext(filePath);
  } catch (e: unknown) {
    const msg = e instanceof Error ? e.message : String(e);
    traceExit("lintRustFile", 1);
    return [error(filePath, "io/read", `Cannot read file: ${msg}`)];
  }

  // Quick check: is this a Rust file with any structural markers?
  if (!ctx.hasAnyOmni && !ctx.hasAnyBlock) {
    // Module files without markers are fine — just note it
    if (ctx.isModuleFile) {
      traceExit("lintRustFile", 1);
      return [
        info(filePath, "structure/module",
          "Module file — no //omni: directives or block markers (identity in lib.rs)"),
      ];
    }
    traceExit("lintRustFile", 1);
    return [
      info(filePath, "structure/skip",
        "No //omni: directives or block markers — not a 4-block file"),
    ];
  }

  // ── Block × Layer lint grid ─────────────────────────────────────────────
  // The lint grid has two dimensions:
  //   Block (vertical): METADATA → SETUP → BODY → CLOSING
  //   Layer (horizontal): L0/R[50] → L1/R[25] → L2/R[10] → L3/R[5]
  //
  // Default traversal: block-first (vertical). Pre-gate (L0) always runs.
  // Each block runs L1 → L2 → L3 before moving to the next block.
  //
  // Cascade is handled in computeRustHealth(), not here — all layers run
  // regardless, and the health scorer neutralizes children when root
  // causes fail. "Root cause already carries the weight."
  // ─────────────────────────────────────────────────────────────────────

  // Pre-load content schemas (async) so closures can reference them
  let contentExp: ContentExpectations | null = null;
  let struct: FullStructureSections | null = null;
  if (ctx.subtype) {
    contentExp = await loadRustContentExpectations(ctx.subtype);
    if (contentExp) {
      struct = await loadRustStructureSections(ctx.subtype) ?? null;
    }
  }

  // ── Concept detection — R[5] per-container checks ────────────────────────
  // Load detect patterns from R5_patterns schemas. Compute containers for ALL
  // 4 blocks uniformly: schema → SubsectionDef[] → getSubsectionRanges() →
  // buildConceptContainers(). Detection is universal; form filtering is separate.
  //
  // When a typing arrow is present (-module->utility), the concept map is
  // overlaid per-section: required sections keep their map (with possible
  // concept_overrides), available sections soften (granted→defer), irrelevant
  // sections skip entirely (all→defer). This is bidirectional verification.
  //
  const conceptDetectors = await loadConceptDetectors("rust");

  // Resolve typing profile for concept map overlays.
  // The typing maps narrow concept expectations from the generic form
  // to the specific variant (e.g., utility only expects pure functions).
  const typingProfile = (ctx.typing && ctx.subtype)
    ? _rustRules!.typingMaps[ctx.subtype]?.[ctx.typing]
    : undefined;

  // METADATA — all 6 sections are ALL_DENIED (no code expected).
  // METADATA doesn't have subsection headers like SETUP/BODY — it's structured
  // by content type (directives, doc-comments, comment-block, pragma, context, subtypes).
  // No typing overlay for METADATA (all denied regardless of role).
  const metadataLines = getBlockLines(ctx.lines, ctx.blocks, "METADATA");
  const metadataConceptContainers = buildConceptContainers(
    "metadata", ctx.filePath, metadataLines, [], conceptDetectors,
  );

  // SETUP — 15 sections with compiled SubsectionDef[]
  const setupLines = getBlockLines(ctx.lines, ctx.blocks, "SETUP");
  const setupRanges = _getSubsectionRanges(setupLines, SETUP_SUBSECTIONS);
  const setupConceptContainers = buildConceptContainers(
    "setup", ctx.filePath, setupLines, setupRanges, conceptDetectors,
    { typingBlock: typingProfile?.SETUP },
  );

  // BODY — 13 sections with compiled SubsectionDef[] (universal detection)
  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  const bodyRanges = _getSubsectionRanges(bodyLines, _rustRules!.bodySubsectionDefs);
  const bodyConceptContainers = buildConceptContainers(
    "body", ctx.filePath, bodyLines, bodyRanges, conceptDetectors,
    { typingBlock: typingProfile?.BODY },
  );

  // CLOSING — 8 sections (Cv/Ce/Cc code zones + X1-X5 doc zones)
  // Typing maps don't cover CLOSING directly, but when typing IS active,
  // the overlay function softens CLOSING (tests verify BODY, not primary code).
  const closingLines = getBlockLines(ctx.lines, ctx.blocks, "CLOSING");
  const closingRanges = _getSubsectionRanges(closingLines, _rustRules!.closingSubsectionDefs);
  const closingConceptContainers = buildConceptContainers(
    "closing", ctx.filePath, closingLines, closingRanges, conceptDetectors,
    typingProfile ? { typingBlock: { required: [], available: [], irrelevant: [] } } : undefined,
  );

  const chain: BlockLintChain = {
    // ── Pre-gate: WHOLE FILE (Layer 0 / R[50]) ──────────────────────
    // Does this file have OmniCode markers? Are all 4 blocks present
    // and ordered? Do block separators follow spec? Template vs derived?
    pregate: [
      () => checkDirectives(ctx),
      () => checkBlockStructure(ctx),
      () => checkSeparatorConsistency(ctx),
      () => checkTemplateVsDerived(ctx),
    ],
    blocks: [
      // ── METADATA ──────────────────────────────────────────────────
      {
        block: "metadata",
        structure: [
          () => checkPragmaMetadata(ctx),
          () => checkDocComments(ctx),
          () => checkCommentMetadata(ctx),
        ],
        crossContainer: [
          () => checkIdentityRegistration(ctx),
          () => checkDirectiveFormat(ctx),
        ],
        containers: metadataConceptContainers,
        content: [
          () => contentExp ? _sharedCheckMetadataContent(ctx, contentExp) : [],
        ],
      },
      // ── SETUP ─────────────────────────────────────────────────────
      {
        block: "setup",
        structure: [
          () => checkSetupSubsectionOrder(ctx),
          () => checkRequiredSetupSubsections(ctx),
        ],
        crossContainer: [
          () => checkSetupHeaderDoc(ctx),
          () => checkContentPlacement(ctx),
          () => checkScalingSignals(ctx),
          // Form-aware validation — bare-bone always loads (format level).
          // Variant form overlays when subtype declared.
          () => ctx.formConstraints ? checkFormRequiredSections(ctx) : [],
          () => ctx.formConstraints ? checkFormReservedSections(ctx) : [],
        ],
        containers: setupConceptContainers,
        content: [
          () => contentExp ? _sharedCheckSetupContent(ctx, contentExp, struct?.SETUP.required, struct?.SETUP.reserved) : [],
        ],
      },
      // ── BODY ──────────────────────────────────────────────────────
      {
        block: "body",
        structure: [
          () => checkBodySubsectionOrder(ctx),
          () => checkRequiredBodySubsections(ctx),
        ],
        crossContainer: [
          () => checkBodySubtypeContent(ctx),
          () => checkSubtypeEmphasis(ctx),
        ],
        containers: bodyConceptContainers,
        content: [
          () => contentExp ? _sharedCheckBodyContent(ctx, contentExp, struct?.BODY.required, struct?.BODY.reserved) : [],
        ],
      },
      // ── CLOSING ───────────────────────────────────────────────────
      {
        block: "closing",
        structure: [
          () => checkClosingZoneOrder(ctx, _rustRules!.closingData),
          () => checkClosingRequiredZones(ctx, _rustRules!.closingData),
        ],
        crossContainer: [
          () => checkClosingContentPlacement(ctx),
          () => checkClosingX6TemplateOnly(ctx, _rustRules!.closingData),
          () => checkClosingDocFieldContent(ctx, _rustRules!.closingData),
          () => checkClosingZoneContent(ctx, _rustRules!.closingData),
        ],
        containers: closingConceptContainers,
        content: [
          () => contentExp ? _sharedCheckClosingContent(ctx, contentExp, struct?.CLOSING.required, struct?.CLOSING.reserved) : [],
        ],
      },
    ],
  };

  const results = await runLintGrid(chain);

  // Trace: emit each result if --trace is on
  if (isTrace()) {
    for (const r of results) {
      traceResult(r);
    }
  }

  traceExit("lintRustFile", results.length);
  return results;
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
// METADATA extraction — Rust-specific context extraction for scaffold adapter
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
 * Build a ScaffoldAdapter for Rust files.
 *
 * Maps local Rust-specific parsing/classification functions into the
 * universal ScaffoldAdapter interface. The shared structuralScaffold
 * pipeline calls these through the adapter — only content words change
 * per language.
 */
function buildRustAdapter(): ScaffoldAdapter {
  return {
    format: "rust",
    fileExtension: ".rs",

    parseChunks: (lines: string[]) => parseCodeChunks(lines),

    chunkToBlock: (chunk) => chunkToBlock(chunk as CodeChunk),

    chunkToSetupSection: (chunk) => chunkToSetupSection(chunk as CodeChunk),

    chunkToBodySection: (chunk) => chunkToBodySection(chunk as CodeChunk),

    extractMetadataContext: (metadataChunks, filePath, subtype, allLines) =>
      extractMetadataContext(metadataChunks as CodeChunk[], filePath, subtype, allLines),

    isTestChunk: (chunk) => chunk.category === "test_module",

    isMainChunk: (chunk) => chunk.category === "main_fn",

    isOrphanChunk: (chunk) =>
      chunk.category === "separator" ||
      chunk.category === "blank" ||
      chunk.category === "comment",
  };
}

// ---------------------------------------------------------------------------
// Transformer helpers — structural content moves
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Transformer
// ---------------------------------------------------------------------------

/**
 * Transform a Rust file to fix structural issues.
 *
 * Pipeline architecture — shared engine runs universal passes, then
 * Rust-specific passes defined here. Same pattern as Go handler.
 *
 * Universal passes (from shared transform engine):
 *   1. Fix block separator widths (= → 76)
 *   2. Fix Unicode subsection separator widths (─ → 74)
 *   3. Convert ASCII dash subsection separators (----) to Unicode (────)
 *   4. Normalize subsection separator comment prefix to standard `// `
 *
 * Rust-specific passes:
 *   0. Structural scaffold (DAR Recover) — generates full 4-block from unstructured
 *   5. Move #[cfg(test)] from BODY to CLOSING Cv zone
 *   6. Move fn main() from BODY to CLOSING Ce zone
 *   7. Reorder CLOSING zones to canonical order (Cv → Ce → Cc → X1-X6)
 *   8. Scaffold missing SETUP subsection headers (--extensions)
 *
 * Transform 0 runs FIRST (before universal passes) because it may
 * completely replace file content — subsequent passes work on the
 * scaffolded result.
 */
async function transformRustFile(
  filePath: string,
  opts: TransformOptions,
): Promise<LintResult[]> {
  await ensureRustRules();

  // Detect subtype from pragma for form-aware transforms
  const fileText = await Deno.readTextFile(filePath);
  const pragmaLineTop = fileText.split("\n").find((l: string) =>
    /^\/\/\s+#!omni\b/.test(l.trim()) || /^\/\/omni:code\b/.test(l.trim()));
  const pragmaArgsTop = pragmaLineTop
    ? pragmaLineTop.replace(/^.*#!omni\s+/, "").trim().split(/\s+/)
        .flatMap((a: string) => a.replace(/^-+/, "").split(",")).filter(Boolean)
    : [];
  const detectedSubtype = pragmaArgsTop.find(
    (a: string) => a in (_rustRules?.subtypeDefinitions ?? {}),
  ) ?? "bare-bone";
  const form = FORM_REGISTRY[detectedSubtype];
  const setupFormStatus = form?.sections?.["setup"] ?? {};

  // --- Rust-specific passes ---

  // 0. Structural scaffold — DAR Recover step
  // When a file has a pragma but NO block boundaries, generate full 4-block.
  // With --force: strip existing blocks first, then re-scaffold.
  // Runs BEFORE universal passes so separator fixes apply to scaffolded output.
  const rustStructuralScaffoldPass: TransformPass = {
    name: "rust-structural-scaffold",
    async apply(ctx: TransformContext): Promise<void> {
      const hasBlocks = findBlocks(ctx.lines).length > 0;
      const pragmaLine = ctx.lines.find((l: string) => {
        const t = l.trim();
        return /^\s*\/\/\s+#!omni\b/.test(t) || /^\/\/omni:code\b/.test(t);
      });

      // --force: extract closing values, strip blocks, re-scaffold
      let preservedClosing: ClosingFieldValues | undefined;
      if (ctx.opts.force && pragmaLine && hasBlocks) {
        preservedClosing = extractClosingFields(ctx.lines);
        ctx.lines = stripBlockStructure(ctx.lines, rustPreserveLine);
        ctx.results.push(info(ctx.filePath, "transform/force-strip",
          "Stripped existing block structure for re-scaffold (--force)"));
      }

      const hasBlocksNow = findBlocks(ctx.lines).length > 0;
      if (pragmaLine && !hasBlocksNow) {
        // Parse pragma args → subtype
        const pragmaArgs = pragmaLine.replace(/^.*#!omni\s+/, "").trim().split(/\s+/)
          .flatMap((a: string) => a.replace(/^-+/, "").split(",")).filter(Boolean);
        const subtype = pragmaArgs.find(
          (a: string) => a in (_rustRules?.subtypeDefinitions ?? {}),
        );

        // Load form constraints — composition > form schema > registry fallback
        const formName = subtype || "bare-bone";
        const formConstraints =
          await loadCompositionFormConstraints("rust", formName)
          ?? await loadFormConstraints("rust", formName)
          ?? buildFormConstraintsFromRegistry(formName);

        // Build adapter and delegate to universal scaffold pipeline
        const adapter = buildRustAdapter();
        const fillContent = _rustRules?.fillContent;
        const scaffoldOpts: ScaffoldOptions = {
          stepsDir: ctx.opts.steps ? ctx.filePath + ".steps" : undefined,
          dryRun: ctx.opts.dryRun,
        };

        const scaffoldResult = await structuralScaffold(
          adapter, ctx.filePath, ctx.lines, formConstraints,
          subtype, fillContent, preservedClosing, scaffoldOpts,
        );

        if (scaffoldResult) {
          if (ctx.opts.dryRun) {
            ctx.wouldModify = true;
            ctx.results.push(...scaffoldResult.results);
          } else {
            ctx.lines = scaffoldResult.lines;
            ctx.modified = true;
            ctx.results.push(...scaffoldResult.results);
          }
        }
      }
    },
  };

  // 5. Move #[cfg(test)] from BODY to CLOSING Cv (single — one test module per file)
  const rustMoveTestsPass = createCodeMovePass({
    description: "#[cfg(test)]",
    detector: findCfgTestInRange,
    sourceBlock: "BODY",
    targetBlock: "CLOSING",
    zoneTag: "Cv",
    zoneLabel: "Closing Validation",
    loop: false,
    resultTag: "transform/move-tests",
  });

  // 6. Move fn main() from BODY to CLOSING Ce
  const rustMoveMainPass = createCodeMovePass({
    description: "fn main()",
    detector: findFnMainInRange,
    sourceBlock: "BODY",
    targetBlock: "CLOSING",
    zoneTag: "Ce",
    zoneLabel: "Closing Execution",
    loop: false,
    resultTag: "transform/move-main",
  });

  // 8. Scaffold missing SETUP subsection headers (--extensions only)
  // Schema-driven: reads subsection defs and placement maps from Code4BlockRules
  const rustSubsectionScaffoldPass = createSubsectionScaffoldPass({
    block: "SETUP",
    canonicalOrder: SETUP_SUBSECTIONS.map((s) => s.tag),
    subsectionDefs: SETUP_SUBSECTIONS,
    classifyLine: classifyLine,
    kindToSubsection: SUBSECTION_PLACEMENT,
    formStatus: setupFormStatus,
  });

  // 9. Reorder SETUP subsections to canonical order (--extensions only)
  const rustSetupReorderPass = createSubsectionReorderPass({
    block: "SETUP",
    canonicalOrder: SETUP_SUBSECTIONS.map((s) => s.tag),
    subsectionDefs: SETUP_SUBSECTIONS,
  });

  // --- Compose pipeline: scaffold first, then universal, then Rust-specific ---
  // Scaffold runs before universals because it may generate the entire file.
  // Universal separator fixes then clean up the scaffolded output.
  const allPasses: TransformPass[] = [
    rustStructuralScaffoldPass,    // 0: DAR Recover — may rewrite entire file
    ...UNIVERSAL_PASSES,           // 1-4: separator fixes (shared)
    rustMoveTestsPass,             // 5: #[cfg(test)] → CLOSING Cv
    rustMoveMainPass,              // 6: fn main() → CLOSING Ce
    reorderClosingZonesPass,       // 7: canonical CLOSING zone order (shared)
    rustSubsectionScaffoldPass,    // 8: SETUP subsection headers
    rustSetupReorderPass,          // 9: reorder after scaffold
  ];

  return runTransformPipeline(filePath, _rustRules!, opts, allPasses, lintRustFile);
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
  let detectedSubtype: string | undefined;
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
    // Extract the actual subtype for per-section content scoring.
    // Checks //omni:code directive (separate from // #!omni shebang).
    for (const line of firstLines) {
      const m = line.match(/\/\/omni:code\s+--rust\s+-(\S+)/);
      if (m) { detectedSubtype = m[1]; break; }
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
   * Uses shared cascade logic from engine/cascade.ts.
   */
  function cascade(groups: AtomicAction[][], reason: string): void {
    cascadeActionGroups(groups, reason, 0);
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

  // ── CONTENT EXPECTATIONS (schema-driven content linting) ─────
  // Layer 2 scoring — the content layer sits on top of the structure layer.
  // Each block gets its own content-expectations container for proper
  // traceback: structure tells you the skeleton, content tells you the flesh.
  const contentExpActions: AtomicAction[] = [];
  if (!isTemplate) {
    // ── METADATA: per-check atomic (directive + 2 identity sections) ──
    contentExpActions.push(...acts("content/metadata-check/directive", "content-expectations", "metadata"));
    contentExpActions.push(...acts("content/metadata-check/Pragma", "content-expectations", "metadata"));
    contentExpActions.push(...acts("content/metadata-check/Metadata", "content-expectations", "metadata"));

    // ── ALL BLOCKS: per-section atomic from structure schema ──
    if (detectedSubtype) {
      const struct = await loadRustStructureSections(detectedSubtype);
      if (struct) {
        // SETUP: imports check + per-reserved-section
        contentExpActions.push(...acts("content/setup-section/Imports", "content-expectations", "setup"));
        for (const section of struct.SETUP.reserved) {
          contentExpActions.push(
            ...acts(`content/setup-reserve/${section}`, "content-expectations", "setup"));
        }

        // BODY: per-required + per-reserved section
        for (const section of struct.BODY.required) {
          contentExpActions.push(
            ...acts(`content/body-required/${section}`, "content-expectations", "body"));
        }
        for (const section of struct.BODY.reserved) {
          contentExpActions.push(
            ...acts(`content/body-reserve/${section}`, "content-expectations", "body"));
        }

        // CLOSING: per-zone (Ce, Cv) + per-reserved zone
        contentExpActions.push(...acts("content/closing-zone/Ce", "content-expectations", "closing"));
        contentExpActions.push(...acts("content/closing-zone/Cv", "content-expectations", "closing"));
        for (const zone of struct.CLOSING.reserved) {
          contentExpActions.push(
            ...acts(`content/closing-reserve/${zone}`, "content-expectations", "closing"));
        }
      }
    }
  }

  // ── TAG LAYERS: stamp pipeline layer on each action group ─────
  // Layer 0 (R[50]) — Whole file: blocks, separators
  tagLayer(blockActions, 0);
  tagLayer(sepActions, 0);

  // Layer 1 (R[25]) — Structure: directives, identity, ordering
  tagLayer(directiveActions, 1);
  tagLayer(identityActions, 1);
  tagLayer(fieldValueActions, 1);
  tagLayer(commentActions, 1);
  tagLayer(docActions, 1);
  tagLayer(templateDerivedActions, 1);
  tagLayer(setupActions, 1);
  tagLayer(bodyOrderActions, 1);
  tagLayer(closingOrderActions, 1);

  // Layer 2 (R[10]) — Container: content placement, closing zones
  tagLayer(contentActions, 2);
  tagLayer(closingPlaceActions, 2);

  // Layer 3 (R[5]) — Content: schema-driven expectations
  tagLayer(contentExpActions, 3);

  // ── CASCADE: missing blocks → all children fail ───────────────
  const blockMissing = (name: string) => failuresByRule.has(`block/${name}`);

  // Filter content-expectations actions by block for per-block cascade
  const contentExpByBlock = (block: string) =>
    contentExpActions.filter((a) => a.block === block);

  if (blockMissing("METADATA")) {
    cascade(
      [directiveActions, identityActions, fieldValueActions, commentActions,
       docActions, templateDerivedActions, contentExpByBlock("metadata")],
      "METADATA block missing — all metadata checks fail",
    );
  }
  if (blockMissing("SETUP")) {
    cascade(
      [setupActions, contentExpByBlock("setup")],
      "SETUP block missing — all setup checks fail",
    );
  }
  if (blockMissing("BODY")) {
    cascade(
      [bodyOrderActions, contentExpByBlock("body")],
      "BODY block missing — all body checks fail",
    );
  }
  if (blockMissing("CLOSING")) {
    cascade(
      [closingOrderActions, closingPlaceActions, contentExpByBlock("closing")],
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
    ...contentExpActions,
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

// ---------------------------------------------------------------------------
// Inspect — show parsed structure without judgment
// ---------------------------------------------------------------------------

/**
 * Build an InspectResult from a Rust file.
 * Uses the same buildContext() as lint, but only reports what the parser sees.
 */
async function inspectRustFile(filePath: string): Promise<InspectResult> {
  await ensureRustRules();
  const ctx = await buildContext(filePath);

  // Blocks — BlockPosition has { name, line, endLine } (all 1-based)
  const blocks: InspectBlock[] = ctx.blocks.map((b) => ({
    name: b.name,
    startLine: b.line,
    endLine: b.endLine || b.line,
    separatorLine: b.line,
  }));

  // Sections — from SETUP and BODY subsection detection
  const sections: InspectSection[] = [];
  const setupBlock = ctx.blocks.find((b) => b.name === "SETUP");
  if (setupBlock) {
    const setupLines = ctx.lines.slice(setupBlock.line - 1, (setupBlock.endLine || ctx.lines.length));
    const setupSubs = getSubsectionRanges(setupLines);
    for (let i = 0; i < setupSubs.length; i++) {
      const s = setupSubs[i]!;
      sections.push({
        name: s.tag,
        block: "SETUP",
        line: (setupBlock.line - 1) + s.startIdx + 1,
        position: i + 1,
      });
    }
  }

  const bodyBlock = ctx.blocks.find((b) => b.name === "BODY");
  if (bodyBlock) {
    const bodyStart = bodyBlock.line - 1;
    const bodyEnd = bodyBlock.endLine || ctx.lines.length;
    const bodyLines = ctx.lines.slice(bodyStart, bodyEnd);
    let bodyPos = 0;
    for (let i = 0; i < bodyLines.length; i++) {
      const line = bodyLines[i]!.trim();
      const match = BODY_SUBSECTION_PATTERN.exec(line);
      if (match) {
        bodyPos++;
        const name = line.replace(/^\/\/\s*/, "").trim();
        sections.push({
          name,
          block: "BODY",
          line: bodyStart + i + 1,
          position: bodyPos,
        });
      }
    }
  }

  // Content classification — count Rust constructs per block
  const contentMap = new Map<string, { count: number; blocks: Set<string> }>();
  const addContent = (category: string, blockName: string) => {
    const existing = contentMap.get(category);
    if (existing) {
      existing.count++;
      existing.blocks.add(blockName);
    } else {
      contentMap.set(category, { count: 1, blocks: new Set([blockName]) });
    }
  };

  for (const block of ctx.blocks) {
    const bStart = block.line - 1;
    const bEnd = block.endLine || ctx.lines.length;
    for (let i = bStart; i < bEnd; i++) {
      const line = ctx.lines[i]?.trim() ?? "";
      const kind = classifyLine(line);
      if (kind === "blank" || kind === "comment" || kind === "other") continue;
      // Map content kind to a human-readable category
      const category = kind.replace(/_decl$/, "").replace(/_/, " ") + "s";
      addContent(category, block.name);
    }
  }

  const content: InspectContent[] = [];
  for (const [category, data] of contentMap) {
    content.push({ category, count: data.count, blocks: [...data.blocks] });
  }

  // Directives
  const directives: Record<string, string> = {};
  for (const [key, info] of ctx.directives) {
    directives[key] = info.value;
  }

  // Pragma
  const pragmaDir = ctx.directives.get("//omni:code") || ctx.directives.get("#!omni:code");
  const pragma = pragmaDir ? `//omni:code ${pragmaDir.value}` : undefined;

  // Identity fields — parse PRAGMA and METADATA statics
  const identity: Record<string, Array<{ key: string; value: string }>> = {};
  const pragmaFields = parseStaticFields(ctx.lines, "PRAGMA");
  if (pragmaFields.length > 0) {
    identity["PRAGMA"] = pragmaFields.map((f) => ({
      key: `${f.section}.${f.field}`,
      value: f.value,
    }));
  }
  const metadataFields = parseStaticFields(ctx.lines, "METADATA");
  if (metadataFields.length > 0) {
    identity["METADATA"] = metadataFields.map((f) => ({
      key: `${f.section}.${f.field}`,
      value: f.value,
    }));
  }

  return {
    filePath,
    format: "rust",
    subtype: ctx.subtype ?? undefined,
    isTemplate: ctx.isTemplate,
    lineCount: ctx.lines.length,
    blocks,
    sections,
    content,
    directives,
    pragma,
    identity: Object.keys(identity).length > 0 ? identity : undefined,
  };
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
  inspect: inspectRustFile,
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
