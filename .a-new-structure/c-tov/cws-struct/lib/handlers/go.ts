// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/handlers/go.ts
// key:     B-tov-cws-struct-lib-handlers-go
// title:   CWS Struct — Go Format Handler
// type:    Code (Library)
// version: a-06.00
// created: 2026-02-13
// updated: 2026-02-18
// authors: Nova Dawn (CPI-SI)
// purpose: Go 4-block alignment linter + transformer with I/C field validation.
//          Validates Go source files for:
//          - //omni: directives (key, version, pragma, metadata)
//          - 4-block structure (METADATA → SETUP → BODY → CLOSING)
//          - END markers for each block
//          - Block order correctness
//          - Pragma and Metadata identity vars ([][2]string I/C fields)
//          - I/C field requirements (I1-I4 PRAGMA, C1-C7 METADATA)
//          - Required/defined field validation per I/C section
//          - Legacy _pragma/_metadata map detection + upgrade notice
//          - Package declaration, import presence
//          - Separator style (consistency + standard widths)
//          - Template vs derived file classification
//          - SETUP section order (10-section dependency chain)
//          - BODY subsection order (Org → Helpers → Core → Error → APIs)
//          - //omni:code directive format validation
//          - Content placement (right constructs in right blocks/subsections)
//
//          Line-based parser — reads .go files as text and checks structural
//          markers via regex. Does NOT parse Go AST.
//
//          Schema triangle: go-4block-schema.jsonc defines, go.ts validates.
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
  InspectResult, InspectBlock, InspectSection, InspectContent,
} from "../foundation/mod.ts";
import {
  error, warn, info,
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
import type { FieldContentRule, ContentExpectations } from "../shared/mod.ts";
import type { GenericChunk, BlockTarget, ScaffoldAdapter, ClosingFieldValues, ScaffoldOptions } from "../shared/mod.ts";
import type { ChunkerConfig, BlockLintChain } from "../shared/mod.ts";
import {
  BLOCKS, REQUIRED_DIRECTIVES, RECOMMENDED_DIRECTIVES,
  PRAGMA_CONTENT_RULES, METADATA_CONTENT_RULES,
  BODY_SUBSECTION_PATTERN, BODY_SUBSECTION_LEGACY,
  findBlocks, getBlockLines,
  getSubsectionRanges as _getSubsectionRanges,
  checkSeparatorConsistency, checkClosingZoneOrder,
  checkClosingRequiredZones, checkClosingZoneContent,
  checkClosingX6TemplateOnly, checkClosingDocFieldContent,
  extractClosingFields, stripBlockStructure,
  structuralScaffold,
  parseChunks, isOmniPragma, isSeparatorBanner, isRegularComment,
  checkSetupSubsectionOrder as _sharedCheckSetupOrder,
  checkBodySubsectionOrder as _sharedCheckBodyOrder,
  checkScalingSignals as _sharedCheckScaling,
  checkRequiredSetupSubsections as _sharedCheckRequiredSetup,
  checkRequiredBodySubsections as _sharedCheckRequiredBody,
  checkSubtypeEmphasis as _sharedCheckEmphasis,
  validateICFields,
  validateICFieldContent,
  parseReservedOmissions,
  validateTemplateVsDerived, validateSubtypeConsistency, validateFormatConsistency,
  // Content linting — schema-driven content expectations
  checkBodyContentExpectations as _sharedCheckBodyContent,
  checkClosingContentExpectations as _sharedCheckClosingContent,
  checkSetupContentExpectations as _sharedCheckSetupContent,
  checkMetadataContentExpectations as _sharedCheckMetadataContent,
  runLintGrid,
} from "../shared/mod.ts";

// Data layer — form registry for form-aware transforms
import { FORM_REGISTRY } from "../data/mod.ts";

// Concept detection — R[5] per-container checks (shared across all code handlers)
import { buildConceptContainers } from "../shared/concept-check.ts";
import { loadConceptDetectors } from "../data/concept-detectors.ts";

// Re-export for tests and schema alignment verification
export { PRAGMA_CONTENT_RULES, METADATA_CONTENT_RULES };
export { validateICFields, validateICFieldContent };
export { SETUP_SUBSECTIONS, BODY_CANONICAL_SUBSECTIONS };

// Re-export chunking/scaffold functions for Go-specific chunker tests
export {
  classifyGoChunkLine,
  parseGoCodeChunks,
  goChunkToBlock,
  goChunkToSetupSection,
  goChunkToBodySection,
  extractGoMetadataContext,
  buildGoAdapter,
};

// ---------------------------------------------------------------------------
// Constants — Go-specific (schema-driven + legacy enrichment)
// ---------------------------------------------------------------------------
//
// LAZY INIT: Schema loads on first handler use (lint/transform/health), not
// at import time. This means Go schema failure only affects Go — Rust and
// TOML continue working. Module registration (registerFormat) is synchronous
// and schema-free.
//

import type { Code4BlockRules } from "../foundation/mod.ts";
import { registerCache } from "../foundation/cache-registry.ts";

/** Lazily-loaded Go rules. Populated by ensureGoRules(). */
let _goRules: Code4BlockRules | null = null;

// Schema-driven constants — populated lazily by ensureGoRules().
// Declared as `let` instead of `const` because they're filled on first use.
// deno-lint-ignore prefer-const
let PRAGMA_FIELD_REQUIREMENTS: Record<string, import("../foundation/code-schema.ts").SchemaFieldRequirement> = {};
// deno-lint-ignore prefer-const
let METADATA_FIELD_REQUIREMENTS: Record<string, import("../foundation/code-schema.ts").SchemaFieldRequirement> = {};
// deno-lint-ignore prefer-const
let SETUP_SUBSECTIONS: import("../foundation/types.ts").SubsectionDef[] = [];
// deno-lint-ignore prefer-const
let BODY_CANONICAL_SUBSECTIONS: Record<string, readonly string[]> = {};

// Content expectations — loaded from forms/{form}/go.jsonc per subtype.
// Populated lazily in ensureGoRules() for the first subtype, then on demand.
const _goContentExpectations = new Map<string, ContentExpectations>();

// Re-export schema-driven requirements for tests and schema alignment verification
export { PRAGMA_FIELD_REQUIREMENTS, METADATA_FIELD_REQUIREMENTS };

/**
 * Go-specific legacy //--- pattern prefixes for backward compatibility.
 *
 * Early Go files used //--- X.N markers (e.g., "//--- I.1" for Imports).
 * The schema carries canonical names + aliases; these legacy patterns
 * are appended so the linter recognizes both old and new formats.
 */
const GO_LEGACY_PREFIXES: Record<string, string> = {
  Imports:        "I",
  Constants:      "K",
  Variables:      "V",
  CoreTypes:      "T",
  TypeMethods:    "TM",
  CodeGeneration: "PS",
};

/**
 * Ensure Go rules are loaded. Lazy init — first call loads from pipeline,
 * subsequent calls are no-ops. All handler entry points (lint, transform,
 * health) call this before doing anything.
 *
 * Fault isolation: if Go schema is missing, only Go fails. Rust and TOML
 * handlers are unaffected because their schemas load independently.
 */
export async function ensureGoRules(): Promise<void> {
  if (_goRules) return;
  _goRules = await loadCodeRules("go");

  // Populate module-scope constants from loaded rules
  PRAGMA_FIELD_REQUIREMENTS = _goRules!.pragmaFieldRequirements;
  METADATA_FIELD_REQUIREMENTS = _goRules!.metadataFieldRequirements;
  BODY_CANONICAL_SUBSECTIONS = _goRules!.bodySubsections;

  // Placement maps — content kind → block/subsection
  BLOCK_PLACEMENT = _goRules!.placementMaps.blockPlacement;
  SUBSECTION_PLACEMENT = _goRules!.placementMaps.subsectionPlacement;
  METADATA_FORBIDDEN = new Set(_goRules!.placementMaps.metadataForbidden);

  // SETUP subsections: schema-derived + Go legacy pattern enrichment
  SETUP_SUBSECTIONS = _goRules!.setupSubsections.map((sub) => {
    const prefix = GO_LEGACY_PREFIXES[sub.tag];
    if (!prefix) return sub;
    // Build legacy alternative: //--- X.N  or  //--- TM (space, not dot)
    const legacySrc = prefix === "TM"
      ? `^\\/\\/---\\s+TM\\s`
      : `^\\/\\/---\\s+${prefix}\\.\\d`;
    return {
      ...sub,
      pattern: new RegExp(`${sub.pattern.source}|${legacySrc}`, sub.pattern.flags),
    };
  });
}

/**
 * Load Go content expectations for a specific form/subtype.
 *
 * Lazy + cached: first call loads from schema pipeline, subsequent calls
 * return cached result. Returns null if schema not found (graceful degradation).
 */
async function loadGoContentExpectations(
  subtype: string,
): Promise<ContentExpectations | null> {
  const cached = _goContentExpectations.get(subtype);
  if (cached) return cached;

  try {
    const content = await loadFormContentCached("go", subtype);
    const exp = content.contentExpectations as ContentExpectations;
    _goContentExpectations.set(subtype, exp);
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
const _goStructureSections = new Map<string, FullStructureSections>();

/**
 * Load ALL block section lists from structure schema.
 * Returns required + reserved arrays per block for content linter and health scorer.
 * Tripwire: if structure schema missing, returns undefined (degrade gracefully).
 */
async function loadGoStructureSections(
  subtype: string,
): Promise<FullStructureSections | undefined> {
  const cached = _goStructureSections.get(subtype);
  if (cached) return cached;

  try {
    const structure = await loadFormStructureCached(subtype);
    const sections: FullStructureSections = {
      SETUP: { required: structure.SETUP.required, reserved: structure.SETUP.reserved },
      BODY: { required: structure.BODY.required, reserved: structure.BODY.reserved },
      CLOSING: { required: structure.CLOSING.required, reserved: structure.CLOSING.reserved },
    };
    _goStructureSections.set(subtype, sections);
    return sections;
  } catch {
    return undefined; // Tripwire: structure schema missing — degrade
  }
}

/**
 * Clear ALL Go handler caches. Exported for test isolation.
 * Resets: rules singleton, content expectations, structure sections, sibling cache.
 */
export function clearGoHandlerCaches(): void {
  _goRules = null;
  _goContentExpectations.clear();
  _goStructureSections.clear();
  _goSiblingCache.clear();
}

/** Known //omni:code directive patterns. */
registerCache("handlers/go", clearGoHandlerCaches);
const KNOWN_CODE_DIRECTIVES = [
  "--go -library",
  "--go -executable",
  "--go -demo-test",
] as const;

// ---------------------------------------------------------------------------
// Content Classification — what Go constructs are, where they belong
// ---------------------------------------------------------------------------

/**
 * Content categories for Go source lines.
 *
 * Each top-level line of Go source can be classified into one of these
 * categories. The classifier + placement maps are the foundation for:
 *   1. Block-level placement checks (is this func in SETUP? → move to BODY)
 *   2. Subsection-level placement checks (is this const in Types? → move to Constants)
 *   3. Health scoring (how much of Types is actually type declarations?)
 */
export type GoContentKind =
  | "package_decl" | "import_decl" | "const_decl" | "var_decl"
  | "type_decl" | "func_decl" | "method_decl" | "init_func"
  | "comment" | "blank" | "other";

/**
 * Content kind → block/subsection placement maps.
 *
 * Schema-driven: loaded from go-4block-schema.jsonc → SETUP.content_kind_mapping.
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
// Types — Go-specific (shared types imported from ../shared/mod.ts)
// ---------------------------------------------------------------------------

/** File-level context gathered once, passed to all check functions. */
interface GoFileContext {
  filePath: string;
  lines: string[];
  isTemplate: boolean;         // has #!omni template
  isDocGo: boolean;            // filename is doc.go
  hasAnyOmni: boolean;         // any //omni: directives present
  hasAnyBlock: boolean;        // any block markers present
  blocks: BlockPosition[];
  directives: Map<string, DirectiveInfo>;
  pkgHasIdentity: boolean;     // sibling file has Pragma/Metadata identity vars
  subtype: string | null;      // "library" | "executable" | "demo-test" | null
  typing: string | null;       // arrow refinement: -module->utility → "utility"
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/**
 * Find all //omni: directives at the top of the file.
 * Also checks for #!omni directives in comments (templates use // #!omni).
 * Returns map of directive name → { value, line }.
 */
function findOmniDirectives(lines: string[]): Map<string, DirectiveInfo> {
  const directives = new Map<string, DirectiveInfo>();

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();
    const lineNum = i + 1;

    // //omni: directives are at the top, before or near `package`
    if (trimmed.startsWith("package ")) break;
    // Also stop at block markers
    if (/^\/\/\s+(METADATA|SETUP|BODY|CLOSING)(\s+BLOCK\s+\[\1\])?\s*$/.test(trimmed)) break;

    // Standard //omni: directive
    const omniMatch = trimmed.match(/^\/\/omni:(\S+)\s*(.*)?$/);
    if (omniMatch) {
      directives.set(`//omni:${omniMatch[1]}`, { value: omniMatch[2]?.trim() ?? "", line: lineNum });
    }

    // Template-style // #!omni directive (used in template files)
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

// ---------------------------------------------------------------------------
// Content classifier — what's on this line?
// ---------------------------------------------------------------------------

/**
 * Classify a single Go source line into a content category.
 *
 * Works on trimmed lines. Handles comment prefixes, blank lines,
 * and all top-level Go declaration forms. Returns the semantic
 * category, not the syntactic form.
 *
 * Foundation for content placement checks and health scoring.
 */
export function classifyGoLine(rawLine: string): GoContentKind {
  const trimmed = rawLine.trim();

  if (trimmed === "") return "blank";
  if (trimmed.startsWith("//")) return "comment";

  // Package declaration
  if (trimmed.startsWith("package ")) return "package_decl";

  // Import — single-line `import "path"` or block `import (`
  if (trimmed.startsWith("import ") || trimmed === "import (") return "import_decl";

  // func init() — special: execution logic, belongs in BODY
  if (/^func\s+init\s*\(/.test(trimmed)) return "init_func";

  // Method declaration: func (r Receiver) Method(...)
  if (/^func\s+\([^)]+\)\s+\w+/.test(trimmed)) return "method_decl";

  // Function declaration: func Name(...)
  if (/^func\s+\w+/.test(trimmed)) return "func_decl";

  // Type declaration: type Name ...
  if (/^type\s+\w/.test(trimmed) || trimmed === "type (") return "type_decl";

  // Const declaration: const Name = ... or const (
  if (/^const\s+\w/.test(trimmed) || trimmed === "const (") return "const_decl";

  // Var declaration: var Name ... or var (
  if (/^var\s+\w/.test(trimmed) || trimmed === "var (") return "var_decl";

  return "other";
}

/**
 * Go-specific wrapper: binds SETUP_SUBSECTIONS to the shared function.
 * Tests import this — signature stays the same.
 */
export function getSubsectionRanges(blockLines: string[]): SubsectionRange[] {
  return _getSubsectionRanges(blockLines, SETUP_SUBSECTIONS);
}

/**
 * Classify top-level declarations in a line array.
 *
 * Tracks brace depth to skip nested content — a `const` inside a
 * function body is NOT a top-level const_decl. Only declarations at
 * brace depth 0 are returned.
 */
export function getTopLevelDeclarations(
  lines: string[],
): Array<{ lineIdx: number; kind: GoContentKind }> {
  const results: Array<{ lineIdx: number; kind: GoContentKind }> = [];
  let braceDepth = 0;

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();

    // At top level, classify the line
    if (braceDepth === 0) {
      const kind = classifyGoLine(trimmed);
      // Only track actual declarations, not noise
      if (kind !== "blank" && kind !== "comment" && kind !== "other") {
        results.push({ lineIdx: i, kind });
      }
    }

    // Update brace depth AFTER classification
    // Simple brace counting — handles 95%+ of Go code.
    // Edge cases (braces in strings/raw strings) are rare at top-level.
    for (const ch of trimmed) {
      if (ch === '{') braceDepth++;
      if (ch === '}') braceDepth = Math.max(0, braceDepth - 1);
    }
  }

  return results;
}

/**
 * Parse a Go identity var to extract I/C field key-value pairs.
 *
 * Input format:
 *   var Pragma = [][2]string{
 *       // comments are skipped
 *       {"I1.key", "value"},
 *   }
 *
 * Returns fields with section (e.g., "I1"), field (e.g., "key"), and value.
 */
export function parseSliceFields(lines: string[], varName: string): IdentityField[] {
  const fields: IdentityField[] = [];
  let inSlice = false;
  const startPattern = new RegExp(
    `^var\\s+${varName}\\s*=\\s*\\[\\]\\[2\\]string\\s*\\{`,
  );

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();
    const lineNum = i + 1;

    if (!inSlice) {
      if (startPattern.test(trimmed)) {
        inSlice = true;
        // Single-line: ends with } on same line after the opening {
        if (/\}\s*$/.test(trimmed) && trimmed.indexOf("{") < trimmed.lastIndexOf("}")) {
          for (const pair of trimmed.matchAll(/\{"([^"]+)",\s*"([^"]*)"\}/g)) {
            const fullKey = pair[1]!;
            const value = pair[2] ?? "";
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

    // End of slice literal
    if (trimmed === "}" || trimmed.startsWith("}")) break;

    // Skip comment-only lines
    if (trimmed.startsWith("//")) continue;

    // Extract {"key", "value"} pair — supports double-quoted and backtick strings.
    // Double:   {"I1.key", "value"}
    // Backtick: {"I1.key", `value with "quotes"`}
    const match = trimmed.match(/\{"([^"]+)",\s*"([^"]*)"\}/)
      ?? trimmed.match(/\{"([^"]+)",\s*`([^`]*)`\}/);
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
// validateICFields — now in shared/code-4block.ts, imported above

/**
 * Check if any sibling .go file in the same package has identity vars.
 *
 * Detects both new (var Pragma = [][2]string{}) and old (var _pragma =) patterns.
 * L0 uses doc.go for package identity. L1 uses the primary file.
 * Either pattern is valid — the package has identity if ANY sibling declares it.
 *
 * Results are cached per directory — when linting N files in the same directory,
 * the directory scan only happens once.
 */
const _goSiblingCache = new Map<string, boolean>();

/** Clear the sibling cache. Exported for test isolation. */
export function clearGoSiblingCache(): void {
  _goSiblingCache.clear();
}

async function packageHasIdentityVars(filePath: string): Promise<boolean> {
  const filename = filePath.split("/").pop() ?? "";
  const dir = filePath.substring(0, filePath.lastIndexOf("/"));
  if (!dir) return false;

  // Cache hit — avoid redundant directory scans
  const cacheKey = dir;
  const cached = _goSiblingCache.get(cacheKey);
  if (cached !== undefined) return cached;

  try {
    for await (const entry of Deno.readDir(dir)) {
      if (!entry.isFile || !entry.name.endsWith(".go")) continue;
      if (entry.name === filename) continue; // skip self
      if (entry.name.endsWith("_test.go")) continue; // skip test files

      const siblingText = await Deno.readTextFile(`${dir}/${entry.name}`);
      // New pattern: var Pragma = [][2]string{
      const hasNewPragma = /^var\s+Pragma\s*=\s*\[\]\[2\]string\s*\{/m.test(siblingText);
      const hasNewMetadata = /^var\s+Metadata\s*=\s*\[\]\[2\]string\s*\{/m.test(siblingText);
      if (hasNewPragma && hasNewMetadata) {
        _goSiblingCache.set(cacheKey, true);
        return true;
      }
      // Old pattern: var _pragma = map[string]string{
      const hasOldPragma = /^var\s+_pragma\s*=/m.test(siblingText);
      const hasOldMetadata = /^var\s+_metadata\s*=/m.test(siblingText);
      if (hasOldPragma && hasOldMetadata) {
        _goSiblingCache.set(cacheKey, true);
        return true;
      }
    }
  } catch {
    _goSiblingCache.set(cacheKey, false);
    return false;
  }

  _goSiblingCache.set(cacheKey, false);
  return false;
}

/**
 * Build file-level context — gathered once, passed to all checks.
 */
async function buildContext(filePath: string): Promise<GoFileContext> {
  // Let read errors propagate — caller handles with full context
  const text = await Deno.readTextFile(filePath);

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
    /^\/\/\s+(METADATA|SETUP|BODY|CLOSING)(\s+BLOCK\s+\[\1\])?\s*$/.test(l.trim())
  );

  // Detect subtype from directives, then PRAGMA I2.subtype if not found.
  // Sources (priority order):
  //   1. //omni:code --go -<subtype>             (e.g., -library)
  //   2. //omni:code --go -<subtype>-><typing>   (e.g., -module->utility)
  //   3. #!omni template --go -<subtype>[-><typing>]
  //   4. PRAGMA I2.subtype field
  let subtype: string | null = null;
  let typing: string | null = null;
  const KNOWN_SUBTYPES = Object.keys(_goRules!.subtypeDefinitions);

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

  // Fallback: parse I2.subtype from Pragma var
  if (!subtype) {
    const pragmaFields = parseSliceFields(lines, "Pragma");
    const subtypeField = pragmaFields.find((f) => f.section === "I2" && f.field === "subtype");
    if (subtypeField && KNOWN_SUBTYPES.includes(subtypeField.value)) {
      subtype = subtypeField.value;
    }
  }

  return {
    filePath,
    lines,
    isTemplate,
    isDocGo: filename === "doc.go",
    hasAnyOmni,
    hasAnyBlock,
    blocks: findBlocks(lines),
    directives,
    pkgHasIdentity: await packageHasIdentityVars(filePath),
    subtype,
    typing,
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
  // Note: strip leading // from directive names in rule strings to avoid
  // embedded slashes breaking matchRule() pattern matching.
  for (const directive of REQUIRED_DIRECTIVES) {
    if (!ctx.directives.has(directive)) {
      const tag = directive.replace(/^\/\//, "");
      results.push(error(file, `directive/${tag}/required`, `Missing ${directive} — REQUIRED`));
    }
  }

  for (const directive of RECOMMENDED_DIRECTIVES) {
    if (!ctx.directives.has(directive)) {
      const tag = directive.replace(/^\/\//, "");
      results.push(warn(file, `directive/${tag}/recommended`, `Missing ${directive} — recommended`));
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
        warn(file, `block/end-${block.name}`, `Missing END ${block.name} marker`,
          { line: block.line }),
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

/**
 * Detect bracketed placeholder values in I/C fields (e.g. "[YOUR-KEY-HERE]", "[template-path]").
 * These indicate unfilled template values that need real content.
 */
function detectPlaceholders(file: string, fields: IdentityField[], varName: string): LintResult[] {
  const results: LintResult[] = [];
  const PLACEHOLDER_RE = /^\[.+\]$/;

  for (const f of fields) {
    if (PLACEHOLDER_RE.test(f.value)) {
      results.push(
        warn(file, `identity/${varName}/placeholder`,
          `${varName}.${f.field} has placeholder value "${f.value}" — replace with actual content`,
          { line: f.line }),
      );
    }
  }

  return results;
}

function checkPragmaMetadata(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // --- Detect new pattern: var Pragma = [][2]string{ ---
  let hasNewPragma = false;
  let hasNewMetadata = false;
  // --- Detect old pattern: var _pragma = map[string]string{ ---
  let hasOldPragma = false;
  let hasOldMetadata = false;

  for (const line of ctx.lines) {
    const trimmed = line.trim();
    if (/^var\s+Pragma\s*=\s*\[\]\[2\]string\s*\{/.test(trimmed)) hasNewPragma = true;
    if (/^var\s+Metadata\s*=\s*\[\]\[2\]string\s*\{/.test(trimmed)) hasNewMetadata = true;
    if (/^var\s+_pragma\s*=/.test(trimmed)) hasOldPragma = true;
    if (/^var\s+_metadata\s*=/.test(trimmed)) hasOldMetadata = true;
  }

  // ── Content-aware checks — template/derived, subtype, format ──────
  // These run for ALL files (including templates) — the whole point is
  // validating that template files have template-appropriate I/C values.
  if (hasNewPragma && hasNewMetadata) {
    const pragmaFields = parseSliceFields(ctx.lines, "Pragma");
    const metadataFields = parseSliceFields(ctx.lines, "Metadata");
    if (pragmaFields.length > 0 && metadataFields.length > 0) {
      results.push(...validateTemplateVsDerived(
        file, pragmaFields, metadataFields, ctx.isTemplate,
        { pragma: "Pragma", metadata: "Metadata" },
      ));
      results.push(...validateSubtypeConsistency(
        file, pragmaFields, ctx.subtype, "Pragma",
      ));
      results.push(...validateFormatConsistency(
        file, pragmaFields, "go", "Pragma",
      ));
    }
  }

  // Templates don't need field-existence or value-pattern checks
  // (they use placeholders). Content-aware checks above already ran.
  if (ctx.isTemplate) return results;

  // Old pattern → upgrade notice
  if (hasOldPragma || hasOldMetadata) {
    results.push(info(file, "identity/upgrade",
      "Legacy _pragma/_metadata maps detected — migrate to var Pragma/Metadata = [][2]string{} with I/C fields"));
  }

  // New pattern → parse and validate I/C fields
  if (hasNewPragma) {
    const pragmaFields = parseSliceFields(ctx.lines, "Pragma");
    if (pragmaFields.length === 0) {
      results.push(warn(file, "identity/Pragma/empty",
        "var Pragma declared but no I/C fields parsed"));
    } else {
      results.push(...validateICFields(file, pragmaFields, PRAGMA_FIELD_REQUIREMENTS, "Pragma"));
      results.push(...detectPlaceholders(file, pragmaFields, "Pragma"));
      results.push(...validateICFieldContent(file, pragmaFields, PRAGMA_CONTENT_RULES, "Pragma"));
    }
  }

  if (hasNewMetadata) {
    const metadataFields = parseSliceFields(ctx.lines, "Metadata");
    if (metadataFields.length === 0) {
      results.push(warn(file, "identity/Metadata/empty",
        "var Metadata declared but no I/C fields parsed"));
    } else {
      results.push(...validateICFields(file, metadataFields, METADATA_FIELD_REQUIREMENTS, "Metadata"));
      results.push(...detectPlaceholders(file, metadataFields, "Metadata"));
      results.push(...validateICFieldContent(file, metadataFields, METADATA_CONTENT_RULES, "Metadata"));
    }
  }

  // Neither old nor new → warn/info based on package context
  if (!hasNewPragma && !hasOldPragma) {
    const level = ctx.pkgHasIdentity ? info : warn;
    const msg = ctx.pkgHasIdentity
      ? "No Pragma var (package identity in sibling file)"
      : "Missing var Pragma = [][2]string{} — recommended for runtime identity";
    results.push(level(file, "identity/pragma-var", msg));
  }

  if (!hasNewMetadata && !hasOldMetadata) {
    const level = ctx.pkgHasIdentity ? info : warn;
    const msg = ctx.pkgHasIdentity
      ? "No Metadata var (package identity in sibling file)"
      : "Missing var Metadata = [][2]string{} — recommended for runtime identity";
    results.push(level(file, "identity/metadata-var", msg));
  }

  return results;
}

function checkCommentMetadata(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  const hasPragmaVar = ctx.lines.some((l) =>
    /^var\s+(Pragma\s*=\s*\[\]\[2\]string|_pragma\s*=)/.test(l.trim()));
  const hasMetadataVar = ctx.lines.some((l) =>
    /^var\s+(Metadata\s*=\s*\[\]\[2\]string|_metadata\s*=)/.test(l.trim()));
  const hasVars = hasPragmaVar && hasMetadataVar;

  let inMetadata = false;
  let hasKey = false;
  let hasPurpose = false;
  let hasBiblical = false;
  let hasVersion = false;

  for (const line of ctx.lines) {
    const trimmed = line.trim();

    if (/^\/\/\s+METADATA(\s+BLOCK\s+\[METADATA\])?\s*$/.test(trimmed)) {
      inMetadata = true;
      continue;
    }
    if (inMetadata && /^\/\/\s+(SETUP(\s+BLOCK\s+\[SETUP\])?|END METADATA(\s+\[END\])?)\s*$/.test(trimmed)) {
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
    const level = hasVars ? info : warn;

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

/**
 * Check 6b: Go doc comment quality — schema-driven.
 *
 * Go convention: `//` comment block immediately before a declaration
 * IS that declaration's doc comment (visible in `go doc` / godoc).
 * The schema defines which doc comments matter:
 *   - package_doc: // block before `package` declaration
 *   - pragma_doc:  // block before `var Pragma`
 *   - metadata_doc: // block before `var Metadata`
 *
 * A blank line between comment and declaration BREAKS the doc link.
 */
function checkDocComments(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;
  const expectations = _goRules!.docCommentExpectations;

  // Schema-driven severity: map string → result constructor
  const emit = (sev: string, f: string, rule: string, msg: string, opts?: { line?: number }) =>
    sev === "error" ? error(f, rule, msg, opts)
    : sev === "warn"  ? warn(f, rule, msg, opts)
    :                    info(f, rule, msg, opts);

  // Templates get info-level only — they may have placeholder docs
  if (ctx.isTemplate) return results;

  // doc.go files are PACKAGE doc — check differently
  if (ctx.isDocGo) return results;

  // --- package_doc: comment block immediately before `package` ---
  const pkgExp = expectations["package_doc"];
  if (pkgExp) {
    let packageLine = -1;
    for (let i = 0; i < ctx.lines.length; i++) {
      if (/^\s*package\s+\w+/.test(ctx.lines[i]!)) {
        packageLine = i;
        break;
      }
    }
    if (packageLine > 0) {
      const prevLine = ctx.lines[packageLine - 1]!.trim();
      if (!prevLine.startsWith("//")) {
        results.push(emit(pkgExp.severity, file, "doc/package",
          "No doc comment before package declaration — recommended for go doc visibility",
          { line: packageLine + 1 }));
      }
    }
  }

  // --- pragma_doc: comment block immediately before `var Pragma` ---
  const pragmaExp = expectations["pragma_doc"];
  if (pragmaExp) {
    for (let i = 0; i < ctx.lines.length; i++) {
      if (/^var\s+Pragma\s*=/.test(ctx.lines[i]!.trim())) {
        if (i > 0 && !ctx.lines[i - 1]!.trim().startsWith("//")) {
          results.push(emit(pragmaExp.severity, file, "doc/pragma-var",
            "No doc comment before var Pragma — recommended for go doc visibility",
            { line: i + 1 }));
        }
        break;
      }
    }
  }

  // --- metadata_doc: comment block immediately before `var Metadata` ---
  const metaExp = expectations["metadata_doc"];
  if (metaExp) {
    for (let i = 0; i < ctx.lines.length; i++) {
      if (/^var\s+Metadata\s*=/.test(ctx.lines[i]!.trim())) {
        if (i > 0 && !ctx.lines[i - 1]!.trim().startsWith("//")) {
          results.push(emit(metaExp.severity, file, "doc/metadata-var",
            "No doc comment before var Metadata — recommended for go doc visibility",
            { line: i + 1 }));
        }
        break;
      }
    }
  }

  return results;
}

// checkSeparatorConsistency — imported from ../shared/mod.ts

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

/** Check 8: SETUP section order (10-section dependency chain). */
function checkSetupSubsectionOrder(ctx: GoFileContext): LintResult[] {
  const setupLines = getBlockLines(ctx.lines, ctx.blocks, "SETUP");
  return _sharedCheckSetupOrder(
    setupLines, SETUP_SUBSECTIONS, ctx.blocks, ctx.filePath, ctx.isTemplate,
  );
}

/** Check 9: BODY subsection order (numeric ascending, subtype-agnostic). */
function checkBodySubsectionOrder(ctx: GoFileContext): LintResult[] {
  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  return _sharedCheckBodyOrder(
    bodyLines, ctx.blocks, ctx.filePath, ctx.isTemplate, true, // includeLegacy for Go
  );
}

/** Check 10a: SETUP required subsections for detected subtype.
 *  When typing arrow is present (e.g., -module->utility), pre-filter setup data
 *  to exclude sections irrelevant for that typing. */
function checkRequiredSetupSubsections(ctx: GoFileContext): LintResult[] {
  const setupLines = getBlockLines(ctx.lines, ctx.blocks, "SETUP");

  // Typing arrow narrows REQUIRED checks to only typing-required sections.
  // "Available" sections are optional — they should NOT produce required-subsection warnings.
  let setupData = _goRules!.setupData;
  if (ctx.typing && ctx.subtype) {
    const typingProfile = _goRules!.typingMaps[ctx.subtype]?.[ctx.typing];
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

/** Check 10b: BODY required subsections for detected subtype.
 *  When typing arrow is present, narrow what counts as "required" for this variant. */
function checkRequiredBodySubsections(ctx: GoFileContext): LintResult[] {
  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  let bodySubtype = ctx.subtype ? _goRules!.bodyData[ctx.subtype] : undefined;

  // Typing arrow narrows REQUIRED checks: only typing-required sections flagged.
  if (ctx.typing && ctx.subtype && bodySubtype) {
    const typingProfile = _goRules!.typingMaps[ctx.subtype]?.[ctx.typing];
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
    ctx.blocks, ctx.filePath, ctx.isTemplate, true, // includeLegacy for Go
  );
}

/** Check 10c: SETUP subtype emphasis — heavy subsections should have content. */
function checkSubtypeEmphasis(ctx: GoFileContext): LintResult[] {
  const emphasis = ctx.subtype
    ? _goRules!.subtypeEmphasis.setup[ctx.subtype]
    : undefined;
  return _sharedCheckEmphasis(
    ctx.lines, ctx.blocks, SETUP_SUBSECTIONS, emphasis,
    ctx.subtype, ctx.filePath, ctx.isTemplate,
  );
}

/** Check 11: //omni:code directive format. */
function checkDirectiveFormat(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Check //omni:code value for derived files.
  // Arrow syntax: -subtype->role is valid if the base -subtype is known.
  // e.g., "--go -library->api" is known because "--go -library" is known.
  const codeInfo = ctx.directives.get("//omni:code");
  if (codeInfo !== undefined && codeInfo.value !== "") {
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
  if (templateInfo !== undefined && templateInfo.value !== "") {
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

  return results;
}

// CLOSING checks — all use schema-driven closingData from code-schema.ts
// checkClosingZoneOrder, checkClosingRequiredZones, checkClosingZoneContent,
// checkClosingX6TemplateOnly, checkClosingDocFieldContent — imported from shared

// ---------------------------------------------------------------------------
// Check 11: Content placement — right constructs in right blocks
// ---------------------------------------------------------------------------

/**
 * Validate that Go constructs appear in the correct block and subsection.
 *
 * Three levels of checking:
 *   1. METADATA block — no code declarations allowed (func, type, var, etc.)
 *   2. SETUP vs BODY — declarations in SETUP, logic in BODY
 *   3. Subsection placement — import in Imports, type in Types, etc.
 *
 * Only checks TOP-LEVEL declarations (brace depth 0). Nested content
 * (e.g., a var inside a func body) is not flagged — it belongs to
 * the enclosing construct.
 *
 * Foundation for health scoring: once we know WHAT is WHERE, scoring is
 * just counting + weighting.
 */
function checkContentPlacement(ctx: GoFileContext): LintResult[] {
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
        // Exempt identity vars — Pragma and Metadata [][2]string belong in METADATA.
        // Default names are "Pragma" and "Metadata", but //omni:pragma and
        // //omni:metadata directives can override to alternate names (e.g.
        // "TestFilePragma" in same-package test files that can't redeclare).
        if (decl.kind === "var_decl") {
          const line = metaLines[decl.lineIdx]?.trim() ?? "";
          const pragmaName = ctx.directives.get("//omni:pragma")?.value || "Pragma";
          const metadataName = ctx.directives.get("//omni:metadata")?.value || "Metadata";
          const identityPattern = new RegExp(
            `^var\\s+(${pragmaName}|${metadataName})\\s*=\\s*\\[\\]\\[2\\]string`
          );
          if (identityPattern.test(line)) {
            continue;
          }
        }
        // Exempt Metadata Imports — Go requires ALL imports before any
        // declarations. Since Pragma/Metadata vars live in METADATA,
        // the import block must precede them. This is a Go language
        // constraint, not a structural error.
        if (decl.kind === "import_decl") {
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
      // Skip kinds that can appear anywhere (comments, blanks, package_decl)
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
          // Walk forward to find end of declaration block (next blank or next decl)
          for (let d = decl.lineIdx + 1; d < subLines.length; d++) {
            const t = subLines[d]!.trim();
            // Stop at blank line, separator, or next subsection header
            if (t === "" || /^\/\/\s*[─=\-]{4,}\s*$/.test(t) || /^\/\/\s+\d+\.?\s+/.test(t)) break;
            // Stop at closing paren of block (const/var/type groups end with ")")
            if (t === ")") { declEndIdx = d; break; }
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
// Check 12: CLOSING zone order — imported from ../shared/mod.ts

// ---------------------------------------------------------------------------
// Check 13: Identity registration
// ---------------------------------------------------------------------------

/**
 * Check if a Go package with identity vars registers them at init() or startup.
 *
 * Go equivalent of Rust's checkIdentityRegistration. In Go, the pattern is:
 *   - File has Pragma/Metadata identity vars
 *   - File should have some form of registration (init() or explicit call)
 *
 * This is informational — not all packages need runtime identity registration,
 * but it's a good practice for packages that declare identity.
 */
function checkIdentityRegistration(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  // Skip templates and doc.go
  if (ctx.isTemplate || ctx.isDocGo) return results;

  // Only check files that have identity vars
  const hasPragma = ctx.lines.some((l) =>
    /^var\s+Pragma\s*=\s*\[\]\[2\]string\s*\{/.test(l.trim()));

  if (!hasPragma) return results;

  // Check for init() function — common Go pattern for self-registration
  const hasInit = ctx.lines.some((l) =>
    /^func\s+init\s*\(\s*\)/.test(l.trim()));

  // Check for explicit registration call
  const hasRegisterCall = ctx.lines.some((l) =>
    /identity\.Register\(|RegisterIdentity\(/.test(l));

  if (!hasInit && !hasRegisterCall) {
    results.push(info(file, "identity/register",
      "File has Pragma identity var but no init() or registration call — consider adding for runtime identity"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 14: CLOSING content placement — tests and main() belong in CLOSING
// ---------------------------------------------------------------------------

/**
 * Detect content that belongs in CLOSING zones but was placed in BODY.
 *
 * Two specific patterns for Go:
 *   1. func TestXxx(...) in BODY → belongs in CLOSING Cv zone
 *   2. func main() in BODY → belongs in CLOSING Ce zone (executables only)
 *
 * These are more specific than checkContentPlacement's generic check.
 * The generic check catches func_decl in BODY (which is correct for most),
 * but these specific patterns deserve CLOSING-specific guidance.
 */
function checkClosingContentPlacement(ctx: GoFileContext): LintResult[] {
  const results: LintResult[] = [];
  const file = ctx.filePath;

  if (ctx.isTemplate) return results;

  // Demo-test files ARE test files — Test*/Benchmark*/Example* in BODY
  // section 5 is correct by design. Only flag test functions in library
  // or executable source files where they don't belong.
  if (ctx.subtype === "demo-test") return results;

  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  if (bodyLines.length === 0) return results;

  const bodyBlock = ctx.blocks.find((b) => b.name === "BODY");
  const bodyStart = bodyBlock?.line ?? 0;

  // --- Check 1: Test functions in BODY ---
  for (let i = 0; i < bodyLines.length; i++) {
    const trimmed = bodyLines[i]!.trim();

    if (/^func\s+Test\w+\s*\(\s*\w+\s+\*testing\.T\s*\)/.test(trimmed)) {
      const fileLine = bodyStart + 1 + i;
      results.push(
        warn(file, "closing/test-placement",
          `Test function in BODY block (line ${fileLine}) — tests belong in CLOSING Cv zone`,
          { line: fileLine }),
      );
    }
  }

  // --- Check 2: func main() in BODY (executable subtype) ---
  for (let i = 0; i < bodyLines.length; i++) {
    const trimmed = bodyLines[i]!.trim();

    if (/^func\s+main\s*\(\s*\)/.test(trimmed)) {
      const fileLine = bodyStart + 1 + i;
      results.push(
        warn(file, "closing/main-placement",
          `func main() in BODY block (line ${fileLine}) — entry point belongs in CLOSING Ce zone`,
          { line: fileLine }),
      );
      break; // Only one main per file
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Check 15: SETUP header documentation
// ---------------------------------------------------------------------------

/**
 * Check that the SETUP block has header documentation before the first
 * subsection marker. Production SETUP blocks should explain the section
 * ordering principle and provide a TOC.
 *
 * Looks for at least 3 non-blank, non-separator comment lines before the
 * first subsection header. Why 3: even a minimal header has a purpose line,
 * a blank separator, and an ordering reference.
 */
function checkSetupHeaderDoc(ctx: GoFileContext): LintResult[] {
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
  let docLines = 0;
  for (let i = 0; i < firstSubIdx; i++) {
    const trimmed = setupLines[i]!.trim();
    // Skip blanks, separator lines, and the block header itself
    if (trimmed === "" || /^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;
    if (/^\/\/\s*$/.test(trimmed)) continue; // empty comment
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
 * Libraries should have: Org Chart, Helpers, Core Operations, Error Handling, Public APIs
 * Executables should have: same (different content, same structure)
 * Demo-tests should have: ...Error Handling, Test Functions (not Public APIs)
 *
 * Only checks if:
 * - File has a known subtype (from //omni:code directive)
 * - BODY has numbered subsections
 *
 * Uses case-insensitive comparison with leading/trailing whitespace ignored.
 */
function checkBodySubtypeContent(ctx: GoFileContext): LintResult[] {
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
    const typingProfile = _goRules!.typingMaps[ctx.subtype]?.[ctx.typing];
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

  // Collect subsection names from BODY
  const found: Array<{ num: number; name: string; lineIdx: number }> = [];
  for (let i = 0; i < bodyLines.length; i++) {
    const trimmed = bodyLines[i]!.trim();
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;

    const match = BODY_SUBSECTION_PATTERN.exec(trimmed) ??
                  BODY_SUBSECTION_LEGACY.exec(trimmed);
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
    // Filter out sections acknowledged in Reserved Omission
    const omitted = parseReservedOmissions(bodyLines);
    const trulyMissing = missing.filter((c) => {
      const cNorm = c.replace(/[^a-zA-Z0-9]/g, "").toLowerCase();
      return !Array.from(omitted).some((o) =>
        o.includes(cNorm) || cNorm.includes(o));
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
 * too much. These are informational — not errors, but signals for
 * future refactoring.
 *
 * Counts only content lines (excluding block markers and separators).
 */
function checkScalingSignals(ctx: GoFileContext): LintResult[] {
  return _sharedCheckScaling(ctx.lines, ctx.blocks, ctx.filePath, ctx.isTemplate);
}

// ---------------------------------------------------------------------------
// Lint orchestrator
// ---------------------------------------------------------------------------

async function lintGoFile(filePath: string): Promise<LintResult[]> {
  traceEnter("lintGoFile", filePath);
  await ensureGoRules();
  let ctx: GoFileContext;
  try {
    ctx = await buildContext(filePath);
  } catch (e: unknown) {
    const msg = e instanceof Error ? e.message : String(e);
    traceExit("lintGoFile", 1);
    return [error(filePath, "io/read", `Cannot read file: ${msg}`)];
  }

  // Quick check: is this a Go file with any structural markers?
  if (!ctx.hasAnyOmni && !ctx.hasAnyBlock) {
    traceExit("lintGoFile", 1);
    return [
      info(filePath, "structure/skip",
        "No //omni: directives or block markers — not a 4-block file"),
    ];
  }

  // ── Block × Layer Lint Grid ─────────────────────────────────────────────
  //
  //               METADATA    SETUP       BODY        CLOSING
  // L0 (R[50])  | ─────────── whole-file (pre-gate) ──────────── |
  // L1 (R[25])  | pragma    | pkg+imp   | subsec    | zones      |
  // L2 (R[10])  | identity  | hdr-doc   | subtype   | zone-cnt   |
  // L3 (R[5])   | content   | content   | content   | content    |
  //
  // Traversal: block-first (vertical). Pre-gate always runs.
  // Cascade is handled in computeGoHealth(), not here — all executed
  // checks run, and the health scorer neutralizes children when root
  // causes fail. "Root cause already carries the weight."
  //
  // The --check flag slices the grid: block, layer, or both.
  // ─────────────────────────────────────────────────────────────────────

  // Pre-load content schemas (async) so closures can reference them
  let contentExp: ContentExpectations | null = null;
  let struct: FullStructureSections | null = null;
  if (ctx.subtype) {
    contentExp = await loadGoContentExpectations(ctx.subtype);
    if (contentExp) {
      struct = await loadGoStructureSections(ctx.subtype) ?? null;
    }
  }

  // ── Concept detection — R[5] per-container checks ────────────────────────
  // Same universal pipeline as Rust handler: detect patterns from R5_patterns
  // schemas, compute containers for ALL 4 blocks uniformly.
  // When typing arrow is present, concept maps are overlaid per-section:
  //   required → keep (+ concept_overrides), available → soften, irrelevant → skip.
  // Go currently has emit-only schemas (no detect) — the loader gracefully
  // returns empty detectors, so concept checks produce no results until
  // Go detect patterns are added (Phase 0).
  const conceptDetectors = await loadConceptDetectors("go");

  // Resolve typing profile for concept map overlays.
  const typingProfile = (ctx.typing && ctx.subtype)
    ? _goRules!.typingMaps[ctx.subtype]?.[ctx.typing]
    : undefined;

  // METADATA — all 6 sections are ALL_DENIED. No typing overlay needed.
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
  const bodyRanges = _getSubsectionRanges(bodyLines, _goRules!.bodySubsectionDefs);
  const bodyConceptContainers = buildConceptContainers(
    "body", ctx.filePath, bodyLines, bodyRanges, conceptDetectors,
    { typingBlock: typingProfile?.BODY },
  );

  // CLOSING — 8 sections (Cv/Ce/Cc code zones + X1-X5 doc zones)
  // When typing is active, CLOSING gets softened (tests verify BODY, not primary code).
  const closingLines = getBlockLines(ctx.lines, ctx.blocks, "CLOSING");
  const closingRanges = _getSubsectionRanges(closingLines, _goRules!.closingSubsectionDefs);
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
      // ── METADATA block ──────────────────────────────────────────
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
      // ── SETUP block ─────────────────────────────────────────────
      {
        block: "setup",
        structure: [
          () => checkPackageAndImports(ctx),
          () => checkSetupSubsectionOrder(ctx),
          () => checkRequiredSetupSubsections(ctx),
        ],
        crossContainer: [
          () => checkSetupHeaderDoc(ctx),
          () => checkContentPlacement(ctx),
          () => checkScalingSignals(ctx),
        ],
        containers: setupConceptContainers,
        content: [
          () => contentExp && struct
            ? _sharedCheckSetupContent(ctx, contentExp, struct.SETUP.required, struct.SETUP.reserved)
            : [],
        ],
      },
      // ── BODY block ──────────────────────────────────────────────
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
          () => contentExp && struct
            ? _sharedCheckBodyContent(ctx, contentExp, struct.BODY.required, struct.BODY.reserved)
            : [],
        ],
      },
      // ── CLOSING block ───────────────────────────────────────────
      {
        block: "closing",
        structure: [
          () => checkClosingZoneOrder(ctx, _goRules!.closingData),
          () => checkClosingRequiredZones(ctx, _goRules!.closingData),
        ],
        crossContainer: [
          () => checkClosingContentPlacement(ctx),
          () => checkClosingX6TemplateOnly(ctx, _goRules!.closingData),
          () => checkClosingDocFieldContent(ctx, _goRules!.closingData),
          () => checkClosingZoneContent(ctx, _goRules!.closingData),
        ],
        containers: closingConceptContainers,
        content: [
          () => contentExp && struct
            ? _sharedCheckClosingContent(ctx, contentExp, struct.CLOSING.required, struct.CLOSING.reserved)
            : [],
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

  traceExit("lintGoFile", results.length);
  return results;
}

// ---------------------------------------------------------------------------
// Transformer helpers — block range detection, content moves
// ---------------------------------------------------------------------------

// BlockRange + findBlockRange — imported from ../shared/mod.ts

/**
 * Find Test functions (func TestXxx(*testing.T)) in a line range.
 * Returns the start/end indices (inclusive) of the first test function found,
 * or null if none. Uses brace tracking to find the complete function body.
 */
function findTestFuncInRange(
  lines: string[], rangeStart: number, rangeEnd: number,
): { start: number; end: number } | null {
  for (let i = rangeStart; i < rangeEnd; i++) {
    const trimmed = lines[i]!.trim();
    if (!/^func\s+Test\w+\s*\(/.test(trimmed)) continue;

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

    return null; // Incomplete block — don't move
  }
  return null;
}

/**
 * Find func main() in a line range.
 * Returns the start/end indices (inclusive) or null.
 */
function findMainFuncInRange(
  lines: string[], rangeStart: number, rangeEnd: number,
): { start: number; end: number } | null {
  for (let i = rangeStart; i < rangeEnd; i++) {
    const trimmed = lines[i]!.trim();
    if (!/^func\s+main\s*\(\s*\)/.test(trimmed)) continue;

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
// Transformer helpers — identity field scaffolding
// ---------------------------------------------------------------------------

/**
 * Find the closing line index of a [][2]string var declaration.
 *
 *   var Pragma = [][2]string{
 *       ...
 *   }   // ← returns this line index (0-based)
 *
 * Returns -1 if the var is not found or has no closing brace.
 */
function findVarClosingLine(lines: string[], varName: string): number {
  const startPattern = new RegExp(
    `^var\\s+${varName}\\s*=\\s*\\[\\]\\[2\\]string\\s*\\{`,
  );
  let inVar = false;

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();

    if (!inVar) {
      if (startPattern.test(trimmed)) {
        inVar = true;
        // Single-line var: var Pragma = [][2]string{{"I1.key","v"}}
        if (/\}\s*$/.test(trimmed) && trimmed.indexOf("{") < trimmed.lastIndexOf("}")) {
          return i;
        }
      }
      continue;
    }

    if (trimmed === "}" || trimmed.startsWith("}")) {
      return i;
    }
  }

  return -1;
}

// ---------------------------------------------------------------------------
// Transformer
// ---------------------------------------------------------------------------

/**
 * Transform a Go file to fix structural issues.
 *
 * Schema-driven pipeline — shared engine + Go-specific passes.
 *
 * Universal passes (from shared transform engine):
 *   1. Fix block separator widths (= → 76)
 *   2. Fix Unicode subsection separator widths (─ → 74)
 *   3. Convert ASCII dash subsection separators (----) to Unicode (────)
 *   4. Normalize subsection separator comment prefix to standard `// `
 *
 * Go-specific passes:
 *   5. Add //go:build ignore to template files
 *   6. Move test functions from BODY to CLOSING Cv zone
 *   7. Move func main() from BODY to CLOSING Ce zone
 *   8. Reorder CLOSING zones to canonical order (Cv → Ce → Cc → X1-X6)
 *   9. Scaffold missing identity fields in Pragma/Metadata vars
 *  10. Scaffold missing SETUP subsection headers (--extensions)
 *
 * Does NOT inject missing block boundaries (too risky for arbitrary positions).
 * Lint first, fix structure manually, then transform for cleanup.
 */
async function transformGoFile(
  filePath: string,
  opts: TransformOptions,
): Promise<LintResult[]> {
  await ensureGoRules();

  // Detect subtype from pragma for form-aware transforms
  const fileText = await Deno.readTextFile(filePath);
  const pragmaLine = fileText.split("\n").find((l: string) =>
    /^\/\/\s+#!omni\b/.test(l.trim()) || /^\/\/omni:code\b/.test(l.trim()));
  const pragmaArgs = pragmaLine
    ? pragmaLine.replace(/^.*#!omni\s+/, "").trim().split(/\s+/)
        .flatMap((a: string) => a.replace(/^-+/, "").split(",")).filter(Boolean)
    : [];
  const detectedSubtype = pragmaArgs.find(
    (a: string) => a in (_goRules?.subtypeDefinitions ?? {}),
  ) ?? "bare-bone";
  const form = FORM_REGISTRY[detectedSubtype];
  const setupFormStatus = form?.sections?.["setup"] ?? {};

  // --- Go-specific passes ---

  // 0. Structural scaffold — DAR Recover step
  // When a file has a pragma but NO block boundaries, generate full 4-block.
  // With --force: strip existing blocks first, then re-scaffold.
  // Runs BEFORE universal passes so separator fixes apply to scaffolded output.
  const goStructuralScaffoldPass: TransformPass = {
    name: "go-structural-scaffold",
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
        ctx.lines = stripBlockStructure(ctx.lines, goPreserveLine);
        ctx.results.push(info(ctx.filePath, "transform/force-strip",
          "Stripped existing block structure for re-scaffold (--force)"));
      }

      const hasBlocksNow = findBlocks(ctx.lines).length > 0;
      if (pragmaLine && !hasBlocksNow) {
        // Parse pragma args → subtype
        const pragmaArgsLocal = pragmaLine.replace(/^.*#!omni\s+/, "").trim().split(/\s+/)
          .flatMap((a: string) => a.replace(/^-+/, "").split(",")).filter(Boolean);
        const subtypeLocal = pragmaArgsLocal.find(
          (a: string) => a in (_goRules?.subtypeDefinitions ?? {}),
        );

        // Load form constraints — composition > form schema > registry fallback
        const formName = subtypeLocal || "bare-bone";
        const formConstraints =
          await loadCompositionFormConstraints("go", formName)
          ?? await loadFormConstraints("go", formName)
          ?? buildFormConstraintsFromRegistry(formName);

        // Build adapter and delegate to universal scaffold pipeline
        const adapter = buildGoAdapter();
        const fillContent = _goRules?.fillContent;
        const scaffoldOpts: ScaffoldOptions = {
          stepsDir: ctx.opts.steps ? ctx.filePath + ".steps" : undefined,
          dryRun: ctx.opts.dryRun,
        };

        const scaffoldResult = await structuralScaffold(
          adapter, ctx.filePath, ctx.lines, formConstraints,
          subtypeLocal, fillContent, preservedClosing, scaffoldOpts,
        );

        if (scaffoldResult) {
          if (ctx.opts.dryRun) {
            ctx.wouldModify = true;
          } else {
            ctx.lines = scaffoldResult.lines;
            ctx.modified = true;
          }
          ctx.results.push(...scaffoldResult.results);
        }
      }
    },
  };

  // 5. Add //go:build ignore to template files
  const goBuildIgnorePass: TransformPass = {
    name: "go-build-ignore",
    apply(ctx: TransformContext): void {
      const isTemplate = ctx.lines.some((l: string) =>
        /^\/\/\s+#!omni\s+template\b/.test(l.trim()));
      const hasBuildIgnore = ctx.lines.some((l: string) =>
        l.trim() === "//go:build ignore");

      if (isTemplate && !hasBuildIgnore) {
        if (ctx.opts.dryRun) {
          ctx.wouldModify = true;
          ctx.results.push(info(ctx.filePath, "transform/build-ignore",
            "Would add //go:build ignore at line 1"));
        } else {
          ctx.lines.unshift("//go:build ignore", "");
          ctx.modified = true;
          ctx.results.push(info(ctx.filePath, "transform/build-ignore",
            "Added //go:build ignore at line 1"));
        }
      }
    },
  };

  // 6. Move test functions from BODY to CLOSING Cv (loop — multiple test funcs)
  const goMoveTestsPass = createCodeMovePass({
    description: "test function",
    detector: findTestFuncInRange,
    sourceBlock: "BODY",
    targetBlock: "CLOSING",
    zoneTag: "Cv",
    zoneLabel: "Closing Validation",
    loop: true,
    resultTag: "transform/move-tests",
  });

  // 7. Move func main() from BODY to CLOSING Ce
  const goMoveMainPass = createCodeMovePass({
    description: "func main()",
    detector: findMainFuncInRange,
    sourceBlock: "BODY",
    targetBlock: "CLOSING",
    zoneTag: "Ce",
    zoneLabel: "Closing Execution",
    loop: false,
    resultTag: "transform/move-main",
  });

  // 9. Scaffold missing identity fields in Pragma/Metadata vars
  const goIdentityScaffoldPass: TransformPass = {
    name: "go-identity-scaffold",
    apply(ctx: TransformContext): void {
      // Skip templates — they already have placeholder fields
      const isTemplate = ctx.lines.some((l: string) =>
        /^\/\/\s+#!omni\s+template\b/.test(l.trim()));
      if (isTemplate) return;

      // Process Metadata BEFORE Pragma (bottom-up) so line indices stay valid
      for (const varConfig of [
        { varName: "Metadata", reqs: METADATA_FIELD_REQUIREMENTS },
        { varName: "Pragma", reqs: PRAGMA_FIELD_REQUIREMENTS },
      ] as const) {
        const fields = parseSliceFields(ctx.lines, varConfig.varName);
        if (fields.length === 0) continue;

        const presentFields = new Map<string, Set<string>>();
        for (const f of fields) {
          if (!presentFields.has(f.section)) presentFields.set(f.section, new Set());
          presentFields.get(f.section)!.add(f.field.split(".")[0]!);
        }

        const scaffoldItems: Array<{ snippet: string; field: string; level: "required" | "defined" }> = [];
        for (const [section, req] of Object.entries(varConfig.reqs)) {
          const sectionSet = presentFields.get(section);
          for (const field of req.required) {
            if (!sectionSet?.has(field)) {
              scaffoldItems.push({
                snippet: `\t{"${section}.${field}", ""},`,
                field: `${section}.${field}`,
                level: "required",
              });
            }
          }
          if (ctx.opts.extensions) {
            for (const field of req.defined) {
              if (!sectionSet?.has(field)) {
                scaffoldItems.push({
                  snippet: `\t{"${section}.${field}", ""},`,
                  field: `${section}.${field}`,
                  level: "defined",
                });
              }
            }
          }
        }

        if (scaffoldItems.length === 0) continue;

        const closingIdx = findVarClosingLine(ctx.lines, varConfig.varName);
        if (closingIdx < 0) continue;

        if (ctx.opts.dryRun) {
          ctx.wouldModify = true;
          for (const item of scaffoldItems) {
            ctx.results.push(info(ctx.filePath, "transform/identity-scaffold",
              `Would add ${item.field} to ${varConfig.varName} (${item.level}, per field registry)`));
          }
        } else {
          ctx.lines.splice(closingIdx, 0, ...scaffoldItems.map((i) => i.snippet));
          ctx.modified = true;
          const reqCount = scaffoldItems.filter((i) => i.level === "required").length;
          const defCount = scaffoldItems.filter((i) => i.level === "defined").length;
          const parts: string[] = [];
          if (reqCount > 0) parts.push(`${reqCount} required`);
          if (defCount > 0) parts.push(`${defCount} defined`);
          ctx.results.push(info(ctx.filePath, "transform/identity-scaffold",
            `Added ${scaffoldItems.length} field(s) to ${varConfig.varName} (${parts.join(", ")})`));
        }
      }
    },
  };

  // 10. Scaffold missing SETUP subsection headers (--extensions only)
  // Schema-driven: reads subsection defs and placement maps from Code4BlockRules
  const goSubsectionScaffoldPass = createSubsectionScaffoldPass({
    block: "SETUP",
    canonicalOrder: SETUP_SUBSECTIONS.map((s) => s.tag),
    subsectionDefs: SETUP_SUBSECTIONS,
    classifyLine: classifyGoLine,
    kindToSubsection: SUBSECTION_PLACEMENT,
    formStatus: setupFormStatus,
  });

  // 11. Reorder SETUP subsections to canonical order (--extensions only)
  const goSetupReorderPass = createSubsectionReorderPass({
    block: "SETUP",
    canonicalOrder: SETUP_SUBSECTIONS.map((s) => s.tag),
    subsectionDefs: SETUP_SUBSECTIONS,
  });

  // --- Compose pipeline: structural scaffold + universal + Go-specific ---
  const allPasses: TransformPass[] = [
    goStructuralScaffoldPass,   // FIRST — may replace entire file content
    ...UNIVERSAL_PASSES,
    goBuildIgnorePass,
    goMoveTestsPass,
    goMoveMainPass,
    reorderClosingZonesPass,    // shared, reads from Code4BlockRules.closingData
    goIdentityScaffoldPass,
    goSubsectionScaffoldPass,
    goSetupReorderPass,         // reorder after scaffold (needs headers to exist)
  ];

  return runTransformPipeline(filePath, _goRules!, opts, allPasses, lintGoFile);
}

// ---------------------------------------------------------------------------
// Health scoring — atomic action mapping for Go files
// ---------------------------------------------------------------------------
//
// Every lint check becomes an atomic action: pass or fail.
// Actions are grouped into containers → blocks → file health.
// Mirrors the Rust handler's scoring structure — same algorithm, Go rules.

/**
 * Compute health score for a Go file from its lint results.
 *
 * Maps each lint rule to an AtomicAction within a container and block.
 * Uses cascading failure: if a parent check fails (e.g., missing METADATA
 * block), all child checks within that block also fail.
 *
 * The scoring formula is asymmetric — errors cost 2×, warnings 1×,
 * infos 0.25×. One error in 10 checks → 70%, not 90%.
 * Truth in measurement.
 */
async function computeGoHealth(
  filePath: string,
  results: LintResult[],
): Promise<HealthScore> {
  await ensureGoRules();
  // ── File type detection ────────────────────────────────────────
  const basename = filePath.split("/").pop() ?? "";
  const isDocGo = basename === "doc.go";
  let isTemplate = false;
  let detectedSubtype: string | undefined;
  try {
    const content = await Deno.readTextFile(filePath);
    const firstLines = content.split("\n").slice(0, 15);
    isTemplate = firstLines.some((l) =>
      /^\/\/\s+#!omni\s+template\b/.test(l.trim()));
    // Detect subtype from //omni:code directive for per-section content scoring
    for (const line of firstLines) {
      const m = line.match(/\/\/omni:code\s+--go\s+-(\S+)/);
      if (m) { detectedSubtype = m[1]; break; }
    }
  } catch { /* best-effort — defaults to non-template */ }

  // ── Build failure index ────────────────────────────────────────
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
   * The 0 balances things out: one root failure, not 30 cascaded failures.
   *
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

  const goActions: AtomicAction[] = [];
  if (!isTemplate) {
    goActions.push(...acts("go/package", "go-specific", "structural"));
    goActions.push(...acts("go/import", "go-specific", "structural"));
  }

  // ── METADATA block ────────────────────────────────────────────
  const directiveActions: AtomicAction[] = [];
  if (isTemplate) {
    directiveActions.push(...acts("directive/meta.key", "directives", "metadata"));
    directiveActions.push(...acts("directive/template-format", "directives", "metadata"));
  } else if (!isDocGo) {
    for (const d of REQUIRED_DIRECTIVES) {
      directiveActions.push(...acts(`directive/${d}`, "directives", "metadata"));
    }
    for (const d of RECOMMENDED_DIRECTIVES) {
      directiveActions.push(...acts(`directive/${d}`, "directives", "metadata"));
    }
    directiveActions.push(...acts("directive/code-format", "directives", "metadata"));
  }

  const identityActions: AtomicAction[] = [];
  if (!isDocGo) {
    identityActions.push(...acts("identity/Pragma/empty", "identity", "metadata"));
    identityActions.push(...acts("identity/Metadata/empty", "identity", "metadata"));
    identityActions.push(...acts("identity/upgrade", "identity", "metadata"));

    // PRAGMA field checks (I1-I4)
    for (const [section, req] of Object.entries(PRAGMA_FIELD_REQUIREMENTS)) {
      for (const field of [...req.required, ...req.defined]) {
        identityActions.push(
          ...acts(`identity/Pragma/${section}.${field}`, "identity", "metadata"));
      }
    }

    // METADATA field checks (C1-C7)
    for (const [section, req] of Object.entries(METADATA_FIELD_REQUIREMENTS)) {
      for (const field of [...req.required, ...req.defined]) {
        identityActions.push(
          ...acts(`identity/Metadata/${section}.${field}`, "identity", "metadata"));
      }
    }
  }

  // Field VALUE checks — content validation (is the value valid?)
  const fieldValueActions: AtomicAction[] = [];
  if (!isDocGo && !isTemplate) {
    for (const rule of PRAGMA_CONTENT_RULES) {
      fieldValueActions.push(
        ...acts(`value/Pragma/${rule.field}`, "field-values", "metadata"));
    }
    for (const rule of METADATA_CONTENT_RULES) {
      fieldValueActions.push(
        ...acts(`value/Metadata/${rule.field}`, "field-values", "metadata"));
    }
  }

  const commentActions: AtomicAction[] = [];
  if (!isTemplate && !isDocGo) {
    commentActions.push(...acts("comment-meta/biblical", "comment-meta", "metadata"));
    commentActions.push(...acts("comment-meta/version", "comment-meta", "metadata"));
  }

  const templateDerivedActions: AtomicAction[] = [];
  if (isTemplate) {
    templateDerivedActions.push(...acts("template/build-ignore", "template", "metadata"));
  } else if (!isDocGo) {
    templateDerivedActions.push(...acts("derived/build-ignore", "derived", "metadata"));
  }

  // ── SETUP block ───────────────────────────────────────────────
  const setupActions: AtomicAction[] = [];
  if (!isTemplate) {
    setupActions.push(...acts("setup/subsection-order", "ordering", "setup"));
  }

  const setupContentActions: AtomicAction[] = [];
  if (!isTemplate) {
    setupContentActions.push(...acts("setup/header-doc", "content", "setup"));
  }

  // ── Content placement (spans METADATA, SETUP, BODY) ─────────
  const contentActions: AtomicAction[] = [];
  if (!isTemplate) {
    contentActions.push(...acts("content/metadata-leak", "placement", "metadata"));
    contentActions.push(...acts("content/block-placement", "placement", "setup"));
    contentActions.push(...acts("content/subsection-placement", "placement", "setup"));
  }

  // ── BODY block ────────────────────────────────────────────────
  const bodyOrderActions: AtomicAction[] = [];
  if (!isTemplate) {
    bodyOrderActions.push(...acts("body/subsection-order", "ordering", "body"));
  }

  const bodyContentActions: AtomicAction[] = [];
  if (!isTemplate) {
    bodyContentActions.push(...acts("body/subtype-subsections", "content", "body"));
  }

  // ── CLOSING block ─────────────────────────────────────────────
  const closingActions: AtomicAction[] = [];
  if (!isTemplate) {
    closingActions.push(...acts("closing/zone-order", "zone-ordering", "closing"));
    closingActions.push(...acts("closing/code-zone-order", "zone-ordering", "closing"));
    closingActions.push(...acts("closing/doc-section-order", "zone-ordering", "closing"));
    closingActions.push(...acts("closing/test-placement", "content-placement", "closing"));
    closingActions.push(...acts("closing/main-placement", "content-placement", "closing"));
  }

  const closingContentActions: AtomicAction[] = [];
  if (!isTemplate) {
    closingContentActions.push(...acts("closing/required-X1", "required-zones", "closing"));
    closingContentActions.push(...acts("closing/required-X5", "required-zones", "closing"));
    closingContentActions.push(...acts("closing/X1-content", "zone-content", "closing"));
    closingContentActions.push(...acts("closing/X5-content", "zone-content", "closing"));
  }

  // ── CONTENT EXPECTATIONS (schema-driven content linting) ─────
  // Layer 2 scoring — the content layer sits on top of the structure layer.
  // Each block gets its own content-expectations container for proper
  // traceback: structure tells you the skeleton, content tells you the flesh.
  //
  // Per-section atomic scoring:
  //   Required section present → +1 aligned (no lint result = pass)
  //   Required section missing → -1 misaligned (lint result = fail)
  //   Reserved section absent → +1 aligned (reserve omission = correct)
  //   Reserved section with code → -1 misaligned (reserve violation)
  //
  // This gives true 1-point-per-section granularity on the -100/+100 scale.
  const contentExpActions: AtomicAction[] = [];
  if (!isTemplate && !isDocGo) {
    // ── METADATA: per-check atomic (directive + 2 identity sections) ──
    contentExpActions.push(...acts("content/metadata-check/directive", "content-expectations", "metadata"));
    contentExpActions.push(...acts("content/metadata-check/Pragma", "content-expectations", "metadata"));
    contentExpActions.push(...acts("content/metadata-check/Metadata", "content-expectations", "metadata"));

    // ── ALL BLOCKS: per-section atomic from structure schema ──
    if (detectedSubtype) {
      const struct = await loadGoStructureSections(detectedSubtype);
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

  // ── Scaling signals (spans setup + body) ───────────────────────
  const scalingActions: AtomicAction[] = [];
  if (!isTemplate) {
    scalingActions.push(...acts("scaling/setup-size", "scaling", "setup"));
    scalingActions.push(...acts("scaling/body-size", "scaling", "body"));
  }

  // ── TAG LAYERS: stamp pipeline layer on each action group ─────
  // Layer 0 (R[50]) — Whole file: blocks, separators, Go-specific
  tagLayer(blockActions, 0);
  tagLayer(sepActions, 0);
  tagLayer(goActions, 0);

  // Layer 1 (R[25]) — Structure: directives, identity, ordering
  tagLayer(directiveActions, 1);
  tagLayer(identityActions, 1);
  tagLayer(fieldValueActions, 1);
  tagLayer(commentActions, 1);
  tagLayer(templateDerivedActions, 1);
  tagLayer(setupActions, 1);
  tagLayer(bodyOrderActions, 1);
  tagLayer(closingActions, 1);

  // Layer 2 (R[10]) — Container: content placement, zone content
  tagLayer(setupContentActions, 2);
  tagLayer(contentActions, 2);
  tagLayer(bodyContentActions, 2);
  tagLayer(closingContentActions, 2);
  tagLayer(scalingActions, 2);

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
       templateDerivedActions, contentExpByBlock("metadata")],
      "METADATA block missing — all metadata checks fail",
    );
  }
  if (blockMissing("SETUP")) {
    cascade(
      [setupActions, setupContentActions, contentActions,
       contentExpByBlock("setup"),
       scalingActions.filter((a) => a.check === "scaling/setup-size")],
      "SETUP block missing — all setup checks fail",
    );
  }
  if (blockMissing("BODY")) {
    cascade(
      [bodyOrderActions, bodyContentActions, contentExpByBlock("body"),
       scalingActions.filter((a) => a.check === "scaling/body-size")],
      "BODY block missing — all body checks fail",
    );
  }
  if (blockMissing("CLOSING")) {
    cascade(
      [closingActions, closingContentActions, contentExpByBlock("closing")],
      "CLOSING block missing — all closing checks fail",
    );
  }

  // ── SUB-CASCADE: empty identity vars → field + value checks neutral ──
  // Can't assess fields if the var is empty — neutral, not failed.
  if (failuresByRule.has("identity/Pragma/empty")) {
    for (const a of identityActions) {
      if (a.direction > 0 && a.check.startsWith("identity/Pragma/I")) {
        (a as { direction: -1 | 0 | 1 }).direction = 0;
        a.impact = "info";
        a.reason = "Pragma var empty — field check cannot run";
      }
    }
    for (const a of fieldValueActions) {
      if (a.direction > 0 && a.check.startsWith("value/Pragma/")) {
        (a as { direction: -1 | 0 | 1 }).direction = 0;
        a.impact = "info";
        a.reason = "Pragma var empty — content check cannot run";
      }
    }
  }
  if (failuresByRule.has("identity/Metadata/empty")) {
    for (const a of identityActions) {
      if (a.direction > 0 && a.check.startsWith("identity/Metadata/C")) {
        (a as { direction: -1 | 0 | 1 }).direction = 0;
        a.impact = "info";
        a.reason = "Metadata var empty — field check cannot run";
      }
    }
    for (const a of fieldValueActions) {
      if (a.direction > 0 && a.check.startsWith("value/Metadata/")) {
        (a as { direction: -1 | 0 | 1 }).direction = 0;
        a.impact = "info";
        a.reason = "Metadata var empty — content check cannot run";
      }
    }
  }

  // ── SUB-CASCADE: missing identity fields → content value checks neutral ──
  // Can't assess content if the field doesn't exist.
  for (const a of fieldValueActions) {
    if (a.direction <= 0) continue; // already cascaded or failed
    const match = a.check.match(/^value\/(Pragma|Metadata)\/(.+)$/);
    if (match) {
      const [, vn, fieldPath] = match;
      if (failuresByRule.has(`identity/${vn}/${fieldPath}`)) {
        (a as { direction: -1 | 0 | 1 }).direction = 0;
        a.impact = "info";
        a.reason = `${fieldPath} missing — cannot assess content`;
      }
    }
  }

  // ── SUB-CASCADE: missing CLOSING zone → zone content checks neutral ──
  // Can't assess X1 content if X1 zone is missing (and same for X5).
  if (failuresByRule.has("closing/required-X1")) {
    for (const a of closingContentActions) {
      if (a.check === "closing/X1-content") {
        (a as { direction: -1 | 0 | 1 }).direction = 0;
        a.impact = "info";
        a.reason = "X1 zone missing — cannot assess X1 content";
      }
    }
  }
  if (failuresByRule.has("closing/required-X5")) {
    for (const a of closingContentActions) {
      if (a.check === "closing/X5-content") {
        (a as { direction: -1 | 0 | 1 }).direction = 0;
        a.impact = "info";
        a.reason = "X5 zone missing — cannot assess X5 content";
      }
    }
  }

  // ── Collect all actions ───────────────────────────────────────
  const allActions = [
    ...blockActions, ...sepActions, ...goActions,
    ...directiveActions, ...identityActions, ...fieldValueActions,
    ...commentActions, ...templateDerivedActions,
    ...setupActions, ...setupContentActions,
    ...contentActions,
    ...bodyOrderActions, ...bodyContentActions,
    ...closingActions, ...closingContentActions,
    ...contentExpActions,
    ...scalingActions,
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
// GoAdapter — language-specific adapter for future generic handler
// ---------------------------------------------------------------------------
//
// Captures everything that differs between Go and other languages.
// A future GenericCode4BlockHandler takes a LanguageAdapter + Code4BlockRules
// and produces a full handler. Today: the adapter packages Go-specific
// functions alongside the existing handler. Addition, not modification.
//
// new format = new LanguageAdapter + new schema. Not new engine code.
//

/**
 * Go language adapter — implements LanguageAdapter for 4-block Go files.
 *
 * Wraps Go-specific functions (classifier, identity parser, directive finder,
 * zone finders, legacy enrichment, context builder) into the generalization
 * interface. The existing handler still uses these functions directly — the
 * adapter packages them for a future that's open by design.
 */
export const goAdapter: LanguageAdapter = {
  format: "go",
  extensions: [".go"],
  knownCodeDirectives: KNOWN_CODE_DIRECTIVES,

  classifyLine(rawLine: string): string {
    return classifyGoLine(rawLine);
  },

  parseIdentityFields(lines: string[], varName: string): IdentityField[] {
    return parseSliceFields(lines, varName);
  },

  findOmniDirectives(lines: string[]): Map<string, DirectiveInfo> {
    return findOmniDirectives(lines);
  },

  findTestZone(
    lines: string[], rangeStart: number, rangeEnd: number,
  ): { start: number; end: number } | null {
    return findTestFuncInRange(lines, rangeStart, rangeEnd);
  },

  findMainZone(
    lines: string[], rangeStart: number, rangeEnd: number,
  ): { start: number; end: number } | null {
    return findMainFuncInRange(lines, rangeStart, rangeEnd);
  },

  enrichSubsectionPatterns(subsections) {
    return subsections.map((sub) => {
      const prefix = GO_LEGACY_PREFIXES[sub.tag];
      if (!prefix) return sub;
      // Build legacy alternative: //--- X.N  or  //--- TM (space, not dot)
      const legacySrc = prefix === "TM"
        ? `^\\/\\/---\\s+TM\\s`
        : `^\\/\\/---\\s+${prefix}\\.\\d`;
      return {
        ...sub,
        pattern: new RegExp(`${sub.pattern.source}|${legacySrc}`, sub.pattern.flags),
      };
    });
  },

  buildContextExtras(filePath: string, _lines: string[]): Record<string, unknown> {
    const filename = filePath.split("/").pop() ?? "";
    return {
      isDocGo: filename === "doc.go",
      isTestFile: filename.endsWith("_test.go"),
    };
  },
};

// ---------------------------------------------------------------------------
// Scaffold Adapter — universal chunker → structural scaffold pipeline
// ---------------------------------------------------------------------------

/**
 * Chunking classifier for Go — finer-grained than classifyGoLine.
 *
 * Distinguishes single-line declarations from block forms:
 *   - `import "path"` (import_decl) vs `import (` (import_block)
 *   - `const X = 1` (const_decl) vs `const (` (const_block)
 *   - `var X int` (var_decl) vs `var (` (var_block)
 *   - `type Foo int` (type_alias) vs `type Foo struct {` (type_block)
 *
 * Block forms go into balancedKinds; single-line forms are either
 * import-grouped or treated as individual chunks.
 */
function classifyGoChunkLine(trimmed: string): string {
  if (trimmed === "") return "blank";
  if (trimmed.startsWith("//")) return "comment";
  if (trimmed.startsWith("package ")) return "package_decl";

  // Import: single-line vs block
  if (trimmed === "import (" || /^import\s+\(/.test(trimmed)) return "import_block";
  if (trimmed.startsWith("import ")) return "import_decl";

  // Functions: always balanced (have { eventually)
  if (/^func\s+init\s*\(/.test(trimmed)) return "init_func";
  if (/^func\s+\([^)]+\)\s+\w+/.test(trimmed)) return "method_decl";
  if (/^func\s+\w+/.test(trimmed)) return "func_decl";

  // Types: struct/interface → balanced block; simple alias → single-line
  if (/^type\s+\w/.test(trimmed) || trimmed === "type (") {
    if (trimmed === "type (" || /\b(struct|interface)\b/.test(trimmed)) return "type_block";
    return "type_alias";
  }

  // Const/var: block vs single
  if (trimmed === "const (" || /^const\s+\(/.test(trimmed)) return "const_block";
  if (/^const\s+\w/.test(trimmed)) return "const_decl";
  if (trimmed === "var (" || /^var\s+\(/.test(trimmed)) return "var_block";
  if (/^var\s+\w/.test(trimmed)) return "var_decl";

  return "other";
}

/**
 * Go-specific chunker configuration.
 *
 * Key Go differences from Rust:
 *   - No `//!` file docs (fileDoc returns false)
 *   - No `///` item docs (itemDoc returns false — Go uses regular `//` comments)
 *   - No `#[attr]` attributes (attribute returns false)
 *   - No `#[cfg(test)]` test blocks (testBlock returns false)
 *   - singleLineIfNoDelimiter = true (Go has no semicolons)
 *   - Both `{}` and `()` balanced blocks
 */
const goChunkerConfig: ChunkerConfig = {
  classifyLine: classifyGoChunkLine,

  patterns: {
    pragma: isOmniPragma,
    fileDoc: () => false,        // Go has no file-level doc syntax
    separator: isSeparatorBanner,
    comment: isRegularComment,
    itemDoc: () => false,        // Go doc comments are just regular // comments
    attribute: () => false,      // Go has no attributes
    testBlock: () => false,      // Go tests are individual funcs, not blocks
  },

  balancedKinds: new Set([
    "func_decl", "method_decl", "init_func",  // always have {}
    "import_block", "const_block", "var_block", // have ()
    "type_block",                               // struct/interface with {}
  ]),

  importKinds: new Set(["import_decl"]),  // single-line `import "path"`

  kindToCategory: (kind: string, trimmed: string): string => {
    switch (kind) {
      case "func_decl":
        return /^func\s+main\s*\(/.test(trimmed) ? "main_fn" : "fn_decl";
      case "method_decl":    return "fn_decl";
      case "init_func":      return "fn_decl";
      case "import_block":   return "import";
      case "import_decl":    return "import";
      case "const_block":    return "const";
      case "const_decl":     return "const";
      case "var_block":      return "var";
      case "var_decl":       return "var";
      case "type_block":     return "type_def";
      case "type_alias":     return "type_def";
      case "package_decl":   return "package";
      default:               return "other";
    }
  },

  singleLineIfNoDelimiter: true,
};

/** Parse Go source into semantic chunks using the universal engine. */
function parseGoCodeChunks(lines: string[]): GenericChunk[] {
  return parseChunks(lines, goChunkerConfig);
}

/**
 * Map a Go chunk to its target block.
 * Uses the same logic as BLOCK_PLACEMENT from the schema.
 */
function goChunkToBlock(chunk: GenericChunk): BlockTarget {
  switch (chunk.category) {
    case "pragma":
    case "doc_comment":
    case "package":
      return "METADATA";
    case "import":
    case "const":
    case "var":
    case "type_def":
      return "SETUP";
    case "fn_decl":
      return "BODY";
    case "main_fn":
      return "CLOSING";
    case "separator":
    case "comment":
    case "blank":
    case "other":
      return "BODY"; // Default: orphaned content → BODY
  }
  return "BODY";
}

/**
 * Map a SETUP chunk to its subsection tag.
 * Mirrors SUBSECTION_PLACEMENT from the Go schema.
 */
function goChunkToSetupSection(chunk: GenericChunk): string {
  switch (chunk.category) {
    case "import":   return "Imports";
    case "const":    return "Constants";
    case "var":      return "Variables";
    case "type_def": return "Types";
    default:         return "Imports"; // fallback
  }
}

/**
 * Map a BODY chunk to its section tag.
 * Without AST, simple heuristic — functions → helpers or core logic.
 */
function goChunkToBodySection(chunk: GenericChunk): string {
  // Check if it looks like a helper (unexported, no receiver)
  const firstLine = chunk.lines[0]?.trim() ?? "";
  if (/^func\s+[a-z]/.test(firstLine)) return "Helpers";
  return "CoreLogic";
}

/**
 * Extract context from existing Go metadata chunks for METADATA block generation.
 *
 * Extracts identity values from pragma directives, doc comments, and package
 * declarations for populating scaffolded METADATA block content.
 */
function extractGoMetadataContext(
  metadataChunks: GenericChunk[],
  filePath: string,
  subtype: string | undefined,
  allLines?: string[],
): Record<string, string> {
  const ctx: Record<string, string> = {};

  // Extract from directives and pragma
  for (const chunk of metadataChunks) {
    for (const line of chunk.lines) {
      const trimmed = line.trim();

      // //omni:key B-...
      const keyMatch = trimmed.match(/^\/\/omni:key\s+(.+)/);
      if (keyMatch) ctx["key"] = keyMatch[1]!.trim();

      // //omni:version a-01.00
      const verMatch = trimmed.match(/^\/\/omni:version\s+(.+)/);
      if (verMatch) ctx["version"] = verMatch[1]!.trim();

      // package name
      const pkgMatch = trimmed.match(/^package\s+(\w+)/);
      if (pkgMatch && !ctx["package"]) ctx["package"] = pkgMatch[1]!;
    }
  }

  // Filename from path
  const pathParts = filePath.replace(/\\/g, "/").split("/");
  ctx["filename"] = pathParts[pathParts.length - 1] ?? "";

  // Subtype
  if (subtype) ctx["subtype"] = subtype;

  // I3.path — relative path from repo root
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
    const segments = normalized.split("/").filter(Boolean);
    ctx["path"] = segments.slice(-3).join("/");
  }

  // I3.component — derive from filename (strip .go)
  const stem = ctx["filename"]?.replace(/\.go$/, "") ?? "";
  if (stem === "main") {
    ctx["component"] = "binary entry point";
  } else if (stem === "doc") {
    ctx["component"] = "package documentation";
  } else if (stem) {
    ctx["component"] = stem.replace(/_/g, " ");
  }

  // I3.provides — scan for exported symbols
  if (allLines) {
    const pubItems: string[] = [];
    for (const line of allLines) {
      const trimmed = line.trim();
      // Exported function: func Name(
      const fnMatch = trimmed.match(/^func\s+([A-Z]\w*)\s*\(/);
      if (fnMatch) { pubItems.push(fnMatch[1]!); continue; }
      // Exported type: type Name
      const typeMatch = trimmed.match(/^type\s+([A-Z]\w*)\s/);
      if (typeMatch) { pubItems.push(typeMatch[1]!); continue; }
    }
    if (pubItems.length > 0) {
      ctx["provides"] = pubItems.join(", ");
    }
  }

  return ctx;
}

/**
 * Line preservation predicate for Go scaffold --force mode.
 *
 * Lines we want to keep when stripping block structure:
 *   - //go:build directives (compiler directives)
 *   - //go:generate directives
 */
function goPreserveLine(line: string): boolean {
  const trimmed = line.trim();
  return trimmed.startsWith("//go:");
}

/**
 * Build a ScaffoldAdapter for Go files.
 *
 * Maps Go-specific parsing/classification functions into the universal
 * ScaffoldAdapter interface. The shared structuralScaffold pipeline calls
 * these through the adapter — only content words change per language.
 */
function buildGoAdapter(): ScaffoldAdapter {
  return {
    format: "go",
    fileExtension: ".go",

    parseChunks: (lines: string[]) => parseGoCodeChunks(lines),

    chunkToBlock: (chunk) => goChunkToBlock(chunk),

    chunkToSetupSection: (chunk) => goChunkToSetupSection(chunk),

    chunkToBodySection: (chunk) => goChunkToBodySection(chunk),

    extractMetadataContext: (metadataChunks, filePath, subtype, allLines) =>
      extractGoMetadataContext(metadataChunks, filePath, subtype, allLines),

    isTestChunk: (chunk) => {
      // Go test functions: func TestXxx(t *testing.T)
      const firstLine = chunk.lines[0]?.trim() ?? "";
      return /^func\s+Test\w+\s*\(\s*\w+\s+\*testing\.T/.test(firstLine);
    },

    isMainChunk: (chunk) => chunk.category === "main_fn",

    isOrphanChunk: (chunk) =>
      chunk.category === "separator" ||
      chunk.category === "blank" ||
      chunk.category === "comment",
  };
}

// ---------------------------------------------------------------------------
// Inspect — show parsed structure without checks
// ---------------------------------------------------------------------------

async function inspectGoFile(filePath: string): Promise<InspectResult> {
  await ensureGoRules();
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
    // getBlockLines expects 0-based start/end, but BlockPosition is 1-based
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
    // Body subsections use numeric pattern
    let bodyPos = 0;
    for (let i = 0; i < bodyLines.length; i++) {
      const line = bodyLines[i]!.trim();
      const numMatch = line.match(BODY_SUBSECTION_PATTERN);
      const legMatch = !numMatch ? line.match(BODY_SUBSECTION_LEGACY) : null;
      if (numMatch || legMatch) {
        bodyPos++;
        const name = numMatch
          ? line.replace(/^\/\/\s*/, "").trim()
          : (legMatch ? legMatch[0]!.replace(/^\/\/\s*/, "").trim() : "unknown");
        sections.push({
          name,
          block: "BODY",
          line: bodyStart + i + 1,
          position: bodyPos,
        });
      }
    }
  }

  // Content classification — count Go constructs per block
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
      if (/^func\s/.test(line)) addContent("functions", block.name);
      else if (/^import\s/.test(line) || line === "import (") addContent("imports", block.name);
      else if (/^type\s/.test(line)) addContent("type declarations", block.name);
      else if (/^const\s/.test(line) || line === "const (") addContent("constants", block.name);
      else if (/^var\s/.test(line) || line === "var (") addContent("variables", block.name);
      else if (/^\/\/omni:/.test(line)) addContent("omni directives", block.name);
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

  return {
    filePath,
    format: "go",
    subtype: ctx.subtype ?? undefined,
    isTemplate: ctx.isTemplate,
    lineCount: ctx.lines.length,
    blocks,
    sections,
    content,
    directives,
    pragma,
  };
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// Registration — plug into the registry
// ---------------------------------------------------------------------------

const goHandler: FormatHandler = {
  name: "go",
  description: "Go 4-block alignment (//omni: directives, METADATA → SETUP → BODY → CLOSING, identity vars, subsection order)",
  extensions: [".go"],
  maxDepth: 10,
  lint: lintGoFile,
  computeHealth: computeGoHealth,
  transform: transformGoFile,
  inspect: inspectGoFile,
};

registerFormat(goHandler);

export default goHandler;

//
// Go files express structure through comments — the //omni: pragma directives
// and block boundary markers — and through identity vars (Pragma/Metadata as
// [][2]string with I/C fields). This handler reads markers, parses identity,
// and verifies the skeleton is sound before anyone fills in the flesh.
//
// a-05.00: BODY subsection numeric alignment (matches Rust handler pattern).
//   - BODY_SUBSECTIONS label array → BODY_SUBSECTION_PATTERN numeric regex
//   - checkBodySubsectionOrder: ascending numeric check (subtype-agnostic)
//   - Legacy §N — Name format matched via BODY_SUBSECTION_LEGACY regex
//
// a-04.00: 10-section SETUP alignment + separator upgrade + transformer expansion.
//   Linter:
//   - SETUP subsections upgraded from 6 (I/T/TM/K/V/PS) to 10-section
//     dependency chain: Imports → Constants → Variables → TypeAliases →
//     ErrorTypes → CoreTypes → InterfaceDefs → TypeMethods → CodeGeneration →
//     BuildTags (aligned with Rust handler and go-4block-schema.jsonc)
//   - Legacy //--- TAG.N patterns still matched for backward compatibility
//   - Separator check: 3-tier (76= blocks, 74─ SETUP, ASCII - legacy)
//   - Unicode box-drawing ─ (U+2500) separators validated at 74 chars
//   Transformer:
//   - Transform 1: Fix block separator widths (= → 76)
//   - Transform 2: Fix Unicode ─ separator widths (─ → 74)
//   - Transform 3: Convert ASCII dash separators to Unicode ─
//   - Transform 4: Normalize separator prefix spacing to `// `
//   - Transform 5: Add //go:build ignore to template files
//   - Transform 6: Move test functions from BODY to CLOSING Cv zone
//   - Transform 7: Move func main() from BODY to CLOSING Ce zone
//   - Transform 8: Reorder CLOSING zones to canonical order
//   - Transform 9: Scaffold missing identity fields
//
// a-03.00: I/C identity parsing + field validation + transformer scaffolding.
//
// a-02.00: 11 checks (6 original + 5 new) + transformer.
//
// "Let all things be done decently and in order." — 1 Corinthians 14:40
// ============================================================================
