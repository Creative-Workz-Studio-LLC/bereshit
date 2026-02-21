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
} from "../foundation/mod.ts";
import {
  error, warn, info,
  computeContainerScore, computeBlockScore, computeHealthScore,
  loadCodeRules,
} from "../foundation/mod.ts";
import { registerFormat } from "../engine/mod.ts";

// Shared 4-block types, constants, and functions
import type { BlockPosition, DirectiveInfo, SubsectionRange, IdentityField, LanguageAdapter } from "./shared/mod.ts";
import type { FieldContentRule } from "./shared/mod.ts";
import {
  BLOCKS, REQUIRED_DIRECTIVES, RECOMMENDED_DIRECTIVES,
  PRAGMA_CONTENT_RULES, METADATA_CONTENT_RULES,
  BLOCK_SEPARATOR_WIDTH, SUBSECTION_SEPARATOR_WIDTH,
  BODY_SUBSECTION_PATTERN, BODY_SUBSECTION_LEGACY,
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
// Constants — Go-specific (schema-driven + legacy enrichment)
// ---------------------------------------------------------------------------
//
// LAZY INIT: Schema loads on first handler use (lint/transform/health), not
// at import time. This means Go schema failure only affects Go — Rust and
// TOML continue working. Module registration (registerFormat) is synchronous
// and schema-free.
//

import type { Code4BlockRules } from "../foundation/mod.ts";

/** Lazily-loaded Go rules. Populated by ensureGoRules(). */
let _goRules: Code4BlockRules | null = null;

// Schema-driven constants — populated lazily by ensureGoRules().
// Declared as `let` instead of `const` because they're filled on first use.
// deno-lint-ignore prefer-const
let PRAGMA_FIELD_REQUIREMENTS: Record<string, import("../foundation/code-schema.ts").SchemaFieldRequirement> = {};
// deno-lint-ignore prefer-const
let METADATA_FIELD_REQUIREMENTS: Record<string, import("../foundation/code-schema.ts").SchemaFieldRequirement> = {};
// deno-lint-ignore prefer-const
let SETUP_SUBSECTIONS: import("./shared/types.ts").SubsectionDef[] = [];
// deno-lint-ignore prefer-const
let BODY_CANONICAL_SUBSECTIONS: Record<string, readonly string[]> = {};

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

/** Known //omni:code directive patterns. */
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
// Types — Go-specific (shared types imported from ./shared/mod.ts)
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
export function validateICFields(
  file: string,
  fields: IdentityField[],
  requirements: Record<string, { required: string[]; defined: string[] }>,
  varName: string,
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
        results.push(warn(file, `identity/${varName}/${section}.${field}`,
          `Missing required field ${section}.${field} in ${varName} var`, {
            description: `Add ${section}.${field} to ${varName}`,
            toml: `\t{"${section}.${field}", ""},`,
            location: `in ${varName}`,
          }));
      }
    }

    for (const field of req.defined) {
      if (!sectionFields?.has(field)) {
        results.push(info(file, `identity/${varName}/${section}.${field}`,
          `Missing defined field ${section}.${field} in ${varName} var`, {
            description: `Add ${section}.${field} to ${varName}`,
            toml: `\t{"${section}.${field}", ""},`,
            location: `in ${varName}`,
          }));
      }
    }
  }

  return results;
}

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
  //   1. //omni:code --go -<subtype>
  //   2. #!omni template --go -<subtype>
  //   3. PRAGMA I2.subtype field
  let subtype: string | null = null;
  const KNOWN_SUBTYPES = Object.keys(_goRules!.subtypeDefinitions);

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

// checkSeparatorConsistency — imported from ./shared/mod.ts

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

/** Check 10a: SETUP required subsections for detected subtype. */
function checkRequiredSetupSubsections(ctx: GoFileContext): LintResult[] {
  const setupLines = getBlockLines(ctx.lines, ctx.blocks, "SETUP");
  return _sharedCheckRequiredSetup(
    setupLines, SETUP_SUBSECTIONS, _goRules!.setupData, ctx.subtype,
    ctx.blocks, ctx.filePath, ctx.isTemplate,
  );
}

/** Check 10b: BODY required subsections for detected subtype. */
function checkRequiredBodySubsections(ctx: GoFileContext): LintResult[] {
  const bodyLines = getBlockLines(ctx.lines, ctx.blocks, "BODY");
  const bodySubtype = ctx.subtype ? _goRules!.bodyData[ctx.subtype] : undefined;
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

  // Check //omni:code value for derived files
  const codeInfo = ctx.directives.get("//omni:code");
  if (codeInfo !== undefined && codeInfo.value !== "") {
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
  if (templateInfo !== undefined && templateInfo.value !== "") {
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
// Check 12: CLOSING zone order — imported from ./shared/mod.ts

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

  const canonical = BODY_CANONICAL_SUBSECTIONS[ctx.subtype];
  if (!canonical) return results;

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
    const bodyBlock = ctx.blocks.find((b) => b.name === "BODY");
    results.push(
      info(file, "body/subtype-subsections",
        `BODY for ${ctx.subtype} subtype missing canonical subsection(s): ${missing.join(", ")} — expected: ${canonical.join(", ")}`,
        { line: bodyBlock?.line ?? 0 }),
    );
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
  await ensureGoRules();
  let ctx: GoFileContext;
  try {
    ctx = await buildContext(filePath);
  } catch (e: unknown) {
    const msg = e instanceof Error ? e.message : String(e);
    return [error(filePath, "io/read", `Cannot read file: ${msg}`)];
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
    ...checkPragmaMetadata(ctx),
    ...checkDocComments(ctx),
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
    ...checkClosingZoneOrder(ctx, _goRules!.closingData),
    ...checkClosingContentPlacement(ctx),
    ...checkClosingRequiredZones(ctx, _goRules!.closingData),
    ...checkClosingZoneContent(ctx, _goRules!.closingData),
    ...checkClosingX6TemplateOnly(ctx, _goRules!.closingData),
    ...checkClosingDocFieldContent(ctx, _goRules!.closingData),
    ...checkScalingSignals(ctx),
  ];
}

// ---------------------------------------------------------------------------
// Transformer helpers — block range detection, content moves
// ---------------------------------------------------------------------------

// BlockRange + findBlockRange — imported from ./shared/mod.ts

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
  interface ZoneChunk {
    tag: string;
    kind: "code" | "doc";
    lines: string[];
    canonicalIdx: number;
  }

  // Canonical order derived from schema zone ordering
  const canonicalOrder = schemaZones.map((z) => z.tag);

  const zones: ZoneChunk[] = [];
  let preambleLines: string[] = [];
  let currentZone: ZoneChunk | null = null;

  for (let i = 0; i < closingContent.length; i++) {
    const trimmed = closingContent[i]!.trim();

    let matchedZone: { tag: string; kind: "code" | "doc" } | null = null;
    for (const zone of schemaZones) {
      if (zone.pattern.test(trimmed)) {
        matchedZone = { tag: zone.tag, kind: zone.kind };
        break;
      }
    }

    if (matchedZone) {
      if (currentZone) {
        zones.push(currentZone);
      }

      const newZoneLines: string[] = [];

      // Grab the separator line that precedes this zone header
      if (i > 0 && /^\/\/\s*[─=\-]{10,}\s*$/.test(closingContent[i - 1]!.trim())) {
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

  if (currentZone) {
    zones.push(currentZone);
  }

  if (zones.length < 2) return null;

  let inOrder = true;
  for (let i = 1; i < zones.length; i++) {
    if (zones[i]!.canonicalIdx < zones[i - 1]!.canonicalIdx) {
      inOrder = false;
      break;
    }
  }

  if (inOrder) return null;

  zones.sort((a, b) => a.canonicalIdx - b.canonicalIdx);

  const result: string[] = [...preambleLines];
  for (const zone of zones) {
    result.push(...zone.lines);
  }

  return result;
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
 * Capabilities:
 *   1. Fix block separator widths (= → 76)
 *   2. Fix Unicode subsection separator widths (─ → 74)
 *   3. Convert ASCII dash subsection separators (----) to Unicode (────)
 *   4. Normalize subsection separator comment prefix to standard `// `
 *   5. Add //go:build ignore to template files
 *   6. Move test functions from BODY to CLOSING Cv zone
 *   7. Move func main() from BODY to CLOSING Ce zone
 *   8. Reorder CLOSING zones to canonical order (Cv → Ce → Cc → X1-X6)
 *   9. Scaffold missing identity fields in Pragma/Metadata vars
 *
 * Does NOT inject missing block boundaries (too risky for arbitrary positions).
 * Lint first, fix structure manually, then transform for cleanup.
 */
async function transformGoFile(
  filePath: string,
  opts: TransformOptions,
): Promise<LintResult[]> {
  await ensureGoRules();
  const { dryRun, extensions } = opts;
  const results: LintResult[] = [];

  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  const lines = text.split("\n");
  let modified = false;
  let wouldModify = false;

  // --- Transform 1: Fix block separator widths (= chars) ---
  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();
    const eqMatch = trimmed.match(/^(\/\/\s+)(={4,})(\s*)$/);
    if (eqMatch && eqMatch[2]!.length !== BLOCK_SEPARATOR_WIDTH) {
      const newLine = `${eqMatch[1]}${"=".repeat(BLOCK_SEPARATOR_WIDTH)}`;
      if (dryRun) {
        wouldModify = true;
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
        wouldModify = true;
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
  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();
    const dashMatch = trimmed.match(/^(\/\/\s*)(-{4,})(\s*)$/);
    if (dashMatch) {
      const newLine = `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`;
      if (dryRun) {
        wouldModify = true;
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
  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();
    const prefixMatch = trimmed.match(/^\/\/(\s{0}|\s{2,})(─{4,}|={4,})(\s*)$/);
    if (prefixMatch) {
      const newLine = `// ${prefixMatch[2]}`;
      if (dryRun) {
        wouldModify = true;
        results.push(info(filePath, "transform/prefix-normalize",
          `Line ${i + 1}: would normalize separator prefix to standard '// '`));
      } else if (lines[i]!.trim() !== newLine) {
        lines[i] = newLine;
        modified = true;
        const sepChar = prefixMatch[2]!.charAt(0);
        results.push(info(filePath, "transform/prefix-normalize",
          `Line ${i + 1}: normalized separator prefix to standard '// ' (${sepChar} separator)`));
      }
    }
  }

  // --- Transform 5: Add //go:build ignore to template files ---
  const isTemplate = lines.some((l: string) => /^\/\/\s+#!omni\s+template\b/.test(l.trim()));
  const hasBuildIgnore = lines.some((l: string) => l.trim() === "//go:build ignore");

  if (isTemplate && !hasBuildIgnore) {
    if (dryRun) {
      wouldModify = true;
      results.push(info(filePath, "transform/build-ignore",
        "Would add //go:build ignore at line 1"));
    } else {
      lines.unshift("//go:build ignore", "");
      modified = true;
      results.push(info(filePath, "transform/build-ignore",
        "Added //go:build ignore at line 1"));
    }
  }

  // --- Transform 6: Move test functions from BODY to CLOSING Cv ---
  // Loop: Go test functions are individual (unlike Rust's #[cfg(test)] module),
  // so we must find-and-move until BODY has no test functions left.
  {
    let moveCount = 0;
    const allExtracted: string[][] = [];

    // eslint-disable-next-line no-constant-condition
    while (true) {
      const bodyBlock = findBlockRange(lines, "BODY");
      const closingBlock = findBlockRange(lines, "CLOSING");
      if (!bodyBlock || !closingBlock) break;

      const testRange = findTestFuncInRange(lines, bodyBlock.contentStart, bodyBlock.contentEnd);
      if (!testRange) break;

      let extractStart = testRange.start;
      while (extractStart > bodyBlock.contentStart &&
             (lines[extractStart - 1]!.trim() === "" ||
              lines[extractStart - 1]!.trim().startsWith("// WRONG"))) {
        extractStart--;
      }

      const extractedLines = lines.slice(extractStart, testRange.end + 1);

      if (dryRun) {
        wouldModify = true;
        results.push(info(filePath, "transform/move-tests",
          `Lines ${extractStart + 1}–${testRange.end + 1}: would move test function from BODY to CLOSING Cv zone`));
        break; // Dry-run: report first, don't loop (no mutation)
      }

      // Remove from BODY (including trailing blank lines)
      let removeEnd = testRange.end + 1;
      while (removeEnd < bodyBlock.contentEnd && lines[removeEnd]!.trim() === "") {
        removeEnd++;
      }
      lines.splice(extractStart, removeEnd - extractStart);
      allExtracted.push(extractedLines);
      moveCount++;
    }

    // Insert all extracted tests into CLOSING Cv zone at once
    if (!dryRun && allExtracted.length > 0) {
      const closingBlock = findBlockRange(lines, "CLOSING");
      if (closingBlock) {
        const cvZone = [
          "",
          `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
          "// Cv — Closing Validation",
          `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
          "",
        ];
        for (const extracted of allExtracted) {
          cvZone.push(...extracted, "");
        }

        lines.splice(closingBlock.contentStart, 0, ...cvZone);
        modified = true;
        results.push(info(filePath, "transform/move-tests",
          `Moved ${moveCount} test function(s) from BODY to CLOSING Cv zone`));
      }
    }
  }

  // --- Transform 7: Move func main() from BODY to CLOSING Ce ---
  {
    const bodyBlock = findBlockRange(lines, "BODY");
    const closingBlock = findBlockRange(lines, "CLOSING");

    if (bodyBlock && closingBlock) {
      const mainRange = findMainFuncInRange(lines, bodyBlock.contentStart, bodyBlock.contentEnd);
      if (mainRange) {
        let extractStart = mainRange.start;
        while (extractStart > bodyBlock.contentStart &&
               (lines[extractStart - 1]!.trim() === "" ||
                lines[extractStart - 1]!.trim().startsWith("// WRONG"))) {
          extractStart--;
        }

        const extractedLines = lines.slice(extractStart, mainRange.end + 1);

        const ceZone = [
          "",
          `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
          "// Ce — Closing Execution",
          `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`,
          "",
          ...extractedLines,
        ];

        const insertIdx = closingBlock.contentStart;

        if (dryRun) {
          wouldModify = true;
          results.push(info(filePath, "transform/move-main",
            `Lines ${extractStart + 1}–${mainRange.end + 1}: would move func main() from BODY to CLOSING Ce zone`));
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
            `Moved func main() from BODY to CLOSING Ce zone (${extractedLines.length} lines)`));
        }
      }
    }
  }

  // --- Transform 8: Reorder CLOSING zones ---
  {
    const closingBlock = findBlockRange(lines, "CLOSING");
    if (closingBlock) {
      const reordered = reorderClosingZones(
        lines.slice(closingBlock.contentStart, closingBlock.contentEnd),
        _goRules!.closingData.zones,
      );
      if (reordered) {
        if (dryRun) {
          wouldModify = true;
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

  // --- Transform 9: Scaffold missing identity fields ---
  //
  // Pipeline: parse existing fields → compare against requirements → insert missing.
  // Required fields (warn-level) always scaffold. Defined fields (info-level)
  // only scaffold with --extensions. Same k-factor pattern as TOML.
  //
  // Process Metadata BEFORE Pragma (bottom-up) so line indices stay valid.
  if (!isTemplate) {
    for (const varConfig of [
      { varName: "Metadata", reqs: METADATA_FIELD_REQUIREMENTS },
      { varName: "Pragma", reqs: PRAGMA_FIELD_REQUIREMENTS },
    ] as const) {
      const fields = parseSliceFields(lines, varConfig.varName);
      if (fields.length === 0) continue;

      const presentFields = new Map<string, Set<string>>();
      for (const f of fields) {
        if (!presentFields.has(f.section)) presentFields.set(f.section, new Set());
        presentFields.get(f.section)!.add(f.field.split(".")[0]!);
      }

      const snippets: string[] = [];
      for (const [section, req] of Object.entries(varConfig.reqs)) {
        const sectionSet = presentFields.get(section);
        for (const field of req.required) {
          if (!sectionSet?.has(field)) {
            snippets.push(`\t{"${section}.${field}", ""},`);
          }
        }
        if (extensions) {
          for (const field of req.defined) {
            if (!sectionSet?.has(field)) {
              snippets.push(`\t{"${section}.${field}", ""},`);
            }
          }
        }
      }

      if (snippets.length === 0) continue;

      const closingIdx = findVarClosingLine(lines, varConfig.varName);
      if (closingIdx < 0) continue;

      if (dryRun) {
        wouldModify = true;
        for (const s of snippets) {
          results.push(info(filePath, "transform/identity-scaffold",
            `Would add to ${varConfig.varName}: ${s.trim()}`));
        }
      } else {
        lines.splice(closingIdx, 0, ...snippets);
        modified = true;
        results.push(info(filePath, "transform/identity-scaffold",
          `Added ${snippets.length} field(s) to ${varConfig.varName}`));
      }
    }
  }

  // --- Transform 10: Scaffold missing SETUP subsection headers ---
  //
  // When content exists in SETUP without a subsection header above it,
  // scaffold the appropriate header. Only runs with --extensions (k-factor:
  // errors only → scaffold toward fullness).
  //
  // Strategy: Work bottom-up within SETUP to keep line indices stable.
  // Find declarations before any subsection header, classify them, and
  // insert the expected subsection header.
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
        // (orphaned content with no subsection container).
        const firstSubIdx = subsections.length > 0 ? subsections[0]!.startIdx : setupLines.length;
        const orphanedKinds = new Map<string, number>(); // subsection tag → first line index

        for (let i = 0; i < firstSubIdx; i++) {
          const trimmed = setupLines[i]!.trim();
          if (trimmed === "" || trimmed.startsWith("//")) continue;
          const kind = classifyGoLine(trimmed);
          const targetSub = SUBSECTION_PLACEMENT[kind];
          if (targetSub && !orphanedKinds.has(targetSub)) {
            orphanedKinds.set(targetSub, i);
          }
        }

        if (orphanedKinds.size > 0) {
          // Sort by expected canonical order so we insert in sequence
          const canonicalOrder = SETUP_SUBSECTIONS.map((s) => s.tag);
          const sorted = [...orphanedKinds.entries()]
            .sort((a, b) => canonicalOrder.indexOf(a[0]) - canonicalOrder.indexOf(b[0]));

          // Insert headers bottom-up to keep indices stable
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
  try {
    const content = await Deno.readTextFile(filePath);
    const firstLines = content.split("\n").slice(0, 15);
    isTemplate = firstLines.some((l) =>
      /^\/\/\s+#!omni\s+template\b/.test(l.trim()));
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

  // ── Scaling signals (spans setup + body) ───────────────────────
  const scalingActions: AtomicAction[] = [];
  if (!isTemplate) {
    scalingActions.push(...acts("scaling/setup-size", "scaling", "setup"));
    scalingActions.push(...acts("scaling/body-size", "scaling", "body"));
  }

  // ── CASCADE: missing blocks → all children fail ───────────────
  const blockMissing = (name: string) => failuresByRule.has(`block/${name}`);

  if (blockMissing("METADATA")) {
    cascade(
      [directiveActions, identityActions, fieldValueActions, commentActions, templateDerivedActions],
      "METADATA block missing — all metadata checks fail",
    );
  }
  if (blockMissing("SETUP")) {
    cascade(
      [setupActions, setupContentActions, contentActions,
       scalingActions.filter((a) => a.check === "scaling/setup-size")],
      "SETUP block missing — all setup checks fail",
    );
  }
  if (blockMissing("BODY")) {
    cascade(
      [bodyOrderActions, bodyContentActions,
       scalingActions.filter((a) => a.check === "scaling/body-size")],
      "BODY block missing — all body checks fail",
    );
  }
  if (blockMissing("CLOSING")) {
    cascade([closingActions, closingContentActions], "CLOSING block missing — all closing checks fail");
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
