// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/data/errors.ts
// key:     B-tov-cws-struct-lib-data-errors
// title:   CWS Struct — Error Code Catalog
// type:    Code (Library)
// version: a-02.10
// created: 2026-02-22
// updated: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// purpose: Canonical catalog mapping every lint rule string to a CWS-{LAYER}-{SEQ}
//          error code. The catalog is the SINGLE SOURCE OF TRUTH for error codes.
//          Handlers produce rule strings; the catalog assigns them codes.
//
//          Layer numbering (detection granularity):
//            R50 (Layer 0) — WHOLE FILE: directives, blocks, separators
//            R25 (Layer 1) — STRUCTURE: sections, ordering, required fields
//            R10 (Layer 2) — CONTAINER: placement, registration, types
//            R05 (Layer 3) — CONTENT: body/closing content, doc comments
//            T00 (Layer 4) — TOOL: schema pipeline, composition, transpiler
//
//          Sequence ranges per layer:
//            001-009: System/IO       040-049: Classification/emphasis
//            010-019: Directives      050-059: Metadata fields
//            020-029: Block/section   060-069: Closing/form fields
//            030-039: Ordering/style  070-079: Format-specific
//            080-089: Omni/OFD        090-099: Reserved
//
//          Dynamic rules use {{field}} placeholder syntax as the map key.
//          Lookup: exact match first, then prefix match for dynamic rules.
//
//          NOTE: Transform rules (transform/*) are intentionally excluded.
//          They are actions, not error detections. Different pipeline concern.
//
// biblical_foundation: "Diverse weights, and diverse measures, both of them
//   are alike abomination to the LORD." — Proverbs 20:10
//   (One standard. One measure. No diverse weights.)
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Imports
// ---------------------------------------------------------------------------

import type { Severity } from "../foundation/mod.ts";
import type { ErrorCodeEntry } from "./types.ts";

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

/** Layer number → human name constant. */
const L = { 0: "R50", 1: "R25", 2: "R10", 3: "R05", 4: "T00" } as const;

// ---------------------------------------------------------------------------
// Builder
// ---------------------------------------------------------------------------

/**
 * Build an [ruleKey, ErrorCodeEntry] tuple concisely.
 *
 * @param code   Error code (e.g., "CWS-R50-001")
 * @param layer  Detection layer (0-3)
 * @param cat    Category within layer
 * @param sev    Default severity
 * @param rule   Rule string key (exact or {{placeholder}} pattern)
 * @param msg    Message template ({{field}} for substitution)
 * @param sug    Suggestion template
 * @param fn     Check function name for traceability
 */
function E(
  code: string,
  layer: 0 | 1 | 2 | 3 | 4,
  cat: string,
  sev: Severity,
  rule: string,
  msg: string,
  sug: string,
  fn: string,
): [string, ErrorCodeEntry] {
  return [rule, {
    code,
    layer,
    layerName: L[layer],
    category: cat,
    severity: sev,
    messageTemplate: msg,
    suggestionTemplate: sug,
    checkFunction: fn,
  }];
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Layer 0 — R[50] — WHOLE FILE
// ---------------------------------------------------------------------------
// Does this file belong to us? Is it parseable? Are the structural bones here?
//
// 001-009: System (IO, parse, schema)
// 010-019: Directives (omni pragma, key directive)
// 020-029: Block structure (block presence, order, end markers)
// 030-039: Separator style (eq, box, dash widths)
// 040-049: Classification (template/derived, build-ignore)
// 050-059: File-level identity (shebang, top-level OFD fields)
// ---------------------------------------------------------------------------

const R50: [string, ErrorCodeEntry][] = [

  // --- 001-009: System ---

  E("CWS-R50-001", 0, "system", "error",
    "io/read",
    "Cannot read file: {{error}}",
    "Check file exists and has correct permissions",
    "lintFile"),

  E("CWS-R50-002", 0, "system", "error",
    "parse/toml",
    "TOML parse error: {{error}}",
    "Fix TOML syntax — check brackets, quotes, and commas",
    "lintTomlFile"),

  E("CWS-R50-003", 0, "system", "error",
    "parse/empty",
    "File is empty",
    "Add content or remove the file",
    "lintFile"),

  E("CWS-R50-004", 0, "system", "error",
    "parse/json",
    "Invalid JSON: {{error}}",
    "Fix JSON syntax — check brackets, commas, and trailing commas",
    "lintJsonFile"),

  E("CWS-R50-005", 0, "system", "error",
    "parse/root",
    "Root must be a JSON object",
    "Wrap content in curly braces {}",
    "lintJsonFile"),

  E("CWS-R50-006", 0, "system", "error",
    "schema/load",
    "Failed to load validation schema: {{error}}",
    "Check schema file exists and is valid TOML",
    "lintTomlFile"),

  E("CWS-R50-007", 0, "system", "error",
    "parse",
    "Parse error: {{error}}",
    "Fix file syntax",
    "lintJsonFile"),

  // --- 010-019: Directives ---

  E("CWS-R50-010", 0, "directive", "warn",
    "directive/meta.key",
    "Template missing key identifier (#!omni meta.key or // Key:)",
    "Add an #!omni meta.key directive or Key: comment",
    "checkDirectives"),

  E("CWS-R50-011", 0, "directive", "error",
    "directive/{{directive}}/required",
    "Missing {{directive}} — REQUIRED",
    "Add the required OmniCode directive",
    "checkDirectives"),

  E("CWS-R50-012", 0, "directive", "warn",
    "directive/{{directive}}/recommended",
    "Missing {{directive}} — recommended",
    "Add the recommended OmniCode directive",
    "checkDirectives"),

  E("CWS-R50-013", 0, "directive", "info",
    "directive/code-format",
    "Directive //omni:code format: {{format}}",
    "Informational — format detected from directives",
    "checkDirectiveFormat"),

  E("CWS-R50-014", 0, "directive", "info",
    "directive/template-format",
    "Template directive format: {{format}}",
    "Informational — template format detected",
    "checkDirectiveFormat"),

  E("CWS-R50-015", 0, "directive", "warn",
    "pragma/omni_line",
    "Missing #!omni pragma line",
    "Add #!omni pragma (e.g., '# #!omni code --makefile')",
    "checkPragma"),

  E("CWS-R50-016", 0, "directive", "error",
    "pragma/boundary",
    "Missing PRAGMA block boundary",
    "Add PRAGMA/[PRAGMA] boundary marker",
    "checkPragma"),

  E("CWS-R50-017", 0, "directive", "warn",
    "pragma/end_boundary",
    "Missing END PRAGMA boundary",
    "Add END PRAGMA/[END] boundary marker",
    "checkPragma"),

  // --- 020-029: Block structure ---

  E("CWS-R50-020", 0, "block", "error",
    "block/order",
    "Block order violation: {{details}}",
    "Reorder blocks to match 4-block structure: METADATA > SETUP > BODY > CLOSING",
    "checkBlockStructure"),

  E("CWS-R50-021", 0, "block", "warn",
    "block/end-{{block}}",
    "Missing END {{block}} marker",
    "Add END {{block}} block boundary comment",
    "checkBlockStructure"),

  E("CWS-R50-022", 0, "block", "error",
    "block/{{tag}}",
    "Missing block tag: {{tag}}",
    "Add the required block boundary",
    "checkBlockTags"),

  E("CWS-R50-023", 0, "block", "error",
    "blocks/setup",
    "Missing SETUP block boundary",
    "Add SETUP BLOCK [SETUP] boundary",
    "checkBlocks"),

  E("CWS-R50-024", 0, "block", "error",
    "blocks/closing",
    "Missing CLOSING block boundary",
    "Add CLOSING BLOCK [CLOSING] boundary",
    "checkBlocks"),

  E("CWS-R50-025", 0, "block", "warn",
    "blocks/closing_end",
    "Missing END CLOSING boundary",
    "Add END CLOSING [END] boundary",
    "checkBlocks"),

  E("CWS-R50-026", 0, "block", "error",
    "blocks/order",
    "Block order violation: {{details}}",
    "Reorder blocks to match correct structure order",
    "checkBlocks"),

  E("CWS-R50-027", 0, "block", "error",
    "metadata/boundary",
    "Missing METADATA block boundary",
    "Add METADATA BLOCK [METADATA] boundary",
    "checkMetadataBlock"),

  E("CWS-R50-028", 0, "block", "warn",
    "metadata/end_boundary",
    "Missing END METADATA boundary",
    "Add END METADATA [END] boundary",
    "checkMetadataBlock"),

  E("CWS-R50-029", 0, "block", "error",
    "content/boundary",
    "Missing CONTENT block boundary",
    "Add CONTENT BLOCK [CONTENT] boundary",
    "checkContentBlock"),

  // --- 030-039: Separators/style ---

  E("CWS-R50-030", 0, "style", "warn",
    "style/eq-separator-width",
    "Equal-sign separator is {{actual}} chars, expected {{expected}}",
    "Adjust separator to standard width (76 or 78 chars)",
    "checkSeparatorConsistency"),

  E("CWS-R50-031", 0, "style", "info",
    "style/eq-separator-standard",
    "Equal-sign separator at standard width",
    "Informational — separator matches standard",
    "checkSeparatorConsistency"),

  E("CWS-R50-032", 0, "style", "warn",
    "style/box-separator-width",
    "Box separator is {{actual}} chars, expected {{expected}}",
    "Adjust box separator to standard width",
    "checkSeparatorConsistency"),

  E("CWS-R50-033", 0, "style", "info",
    "style/box-separator-standard",
    "Box separator at standard width",
    "Informational — box separator matches standard",
    "checkSeparatorConsistency"),

  E("CWS-R50-034", 0, "style", "warn",
    "style/dash-separator-width",
    "Dash separator is {{actual}} chars, expected {{expected}}",
    "Adjust dash separator to standard width",
    "checkSeparatorConsistency"),

  // --- 040-049: Classification ---

  E("CWS-R50-040", 0, "classify", "error",
    "template/build-ignore",
    "Template file missing //go:build ignore directive",
    "Add //go:build ignore as first line",
    "checkTemplateVsDerived"),

  E("CWS-R50-041", 0, "classify", "warn",
    "derived/build-ignore",
    "Derived file has //go:build ignore (should be removed)",
    "Remove //go:build ignore — this is not a template",
    "checkTemplateVsDerived"),

  E("CWS-R50-042", 0, "classify", "warn",
    "derived/template-directive",
    "Derived file still has template directive markers",
    "Remove template directives — file is derived",
    "checkTemplateVsDerived"),

  E("CWS-R50-043", 0, "classify", "error",
    "metadata/exists",
    "Missing [_metadata] table",
    "Add [_metadata] table with required fields",
    "checkMetadataTable"),

  E("CWS-R50-044", 0, "classify", "error",
    "content/exists",
    "Missing [_content] table",
    "Add [_content] table with zone labels",
    "checkContentTable"),

  E("CWS-R50-045", 0, "classify", "error",
    "closing/exists",
    "Missing [_closing] table",
    "Add [_closing] table with required sections",
    "checkClosingTable"),

  // --- 050-059: File-level identity ---

  E("CWS-R50-050", 0, "identity", "error",
    "shebang/exists",
    "Missing shebang line",
    "Add #!omni shebang as first line",
    "checkShebang"),

  E("CWS-R50-051", 0, "identity", "error",
    "shebang/format",
    "Invalid shebang: {{actual}} — expected '#!omni <type>'",
    "Fix shebang to match #!omni <type> format",
    "checkShebang"),

  E("CWS-R50-052", 0, "identity", "error",
    "top/ofd",
    "Missing required field: ofd (version number)",
    "Add ofd field with version number",
    "lintOfdFile"),

  E("CWS-R50-053", 0, "identity", "error",
    "top/ofd_type",
    "Field 'ofd' must be a number",
    "Change ofd value to a number",
    "lintOfdFile"),

  E("CWS-R50-054", 0, "identity", "error",
    "top/source",
    "Missing required field: source",
    "Add source field referencing the .omni file",
    "lintOfdFile"),

  E("CWS-R50-055", 0, "identity", "error",
    "top/source_type",
    "Field 'source' must be a string",
    "Change source value to a string",
    "lintOfdFile"),

  E("CWS-R50-056", 0, "identity", "warn",
    "top/source_ext",
    "Field 'source' should reference a .omni file",
    "Update source to point to the .omni source file",
    "lintOfdFile"),

  E("CWS-R50-057", 0, "identity", "error",
    "top/compiled",
    "Missing required field: compiled (ISO timestamp)",
    "Add compiled field with ISO timestamp",
    "lintOfdFile"),

  E("CWS-R50-058", 0, "identity", "error",
    "top/compiled_type",
    "Field 'compiled' must be a string",
    "Change compiled value to an ISO timestamp string",
    "lintOfdFile"),

  E("CWS-R50-059", 0, "identity", "error",
    "top/{{section}}",
    "Missing required section: {{section}}",
    "Add the required section object",
    "lintOfdFile"),

  // --- 060-069: Block boundary (extended) ---

  E("CWS-R50-060", 0, "block", "warn",
    "content/end_boundary",
    "Missing END CONTENT block boundary",
    "Add END CONTENT boundary marker",
    "checkContent"),

  E("CWS-R50-061", 0, "block", "warn",
    "closing/boundary",
    "Missing CLOSING block boundary",
    "Add CLOSING block boundary marker",
    "checkClosing"),

  E("CWS-R50-062", 0, "block", "warn",
    "closing/end_boundary",
    "Missing END CLOSING block boundary",
    "Add END CLOSING boundary marker",
    "checkClosing"),

  // --- 070-079: Format-specific ---

  E("CWS-R50-070", 0, "classify", "error",
    "structure/3block",
    "File lacks 3-block structure markers — not a structured file",
    "Add _pragma or _P1_key section for 3-block identification",
    "lintJsonFile"),

  E("CWS-R50-071", 0, "classify", "info",
    "structure/not_omni",
    "No PRAGMA marker or #!omni line — not OmniCode-structured",
    "Add #!omni pragma if this file should be OmniCode-structured",
    "lintFile"),

  E("CWS-R50-072", 0, "classify", "info",
    "structure/skip",
    "File skipped — does not match expected format",
    "Verify file extension and content match handler expectations",
    "lintFile"),

  E("CWS-R50-073", 0, "classify", "info",
    "structure/module",
    "Module file detected — reduced structural requirements",
    "Informational — module files have lighter validation",
    "lintRustFile"),
];

// ---------------------------------------------------------------------------
// Layer 1 — R[25] — STRUCTURE
// ---------------------------------------------------------------------------
// Are sections present, ordered, and required fields populated?
//
// 001-009: Language structure (package, import)
// 010-019: Identity structure (Pragma/Metadata vars, statics)
// 020-029: Section ordering (closing zones, setup/body subsections)
// 030-039: Required sections (setup, body subsections)
// 040-049: Metadata fields (required, extension)
// 050-059: Closing fields (required, extension)
// 060-069: Form/content structure
// 070-079: Format-specific (cargo, pragma fields)
// 080-089: Omni-specific structure
// 090-099: OFD-specific structure
// ---------------------------------------------------------------------------

const R25: [string, ErrorCodeEntry][] = [

  // --- 001-009: Language structure ---

  E("CWS-R25-001", 1, "language", "error",
    "go/package",
    "Missing package declaration",
    "Add package declaration (e.g., 'package main')",
    "checkPackageAndImports"),

  E("CWS-R25-002", 1, "language", "info",
    "go/import",
    "No import statement found",
    "Add import block if external packages are needed",
    "checkPackageAndImports"),

  E("CWS-R25-003", 1, "language", "warn",
    "cargo/schema",
    "No cargo derivation layout in schema — Cargo content checks skipped",
    "Add cargo section to validation schema",
    "checkCargoLayout"),

  E("CWS-R25-004", 1, "language", "warn",
    "cargo/{{section}}",
    "Cargo section {{section}}: {{message}}",
    "Fix cargo section layout per schema",
    "checkCargoLayout"),

  // --- 010-019: Identity structure ---

  E("CWS-R25-010", 1, "identity", "warn",
    "identity/Pragma/empty",
    "Pragma identity variable is empty",
    "Add entries to Pragma variable",
    "checkPragmaMetadata"),

  E("CWS-R25-011", 1, "identity", "warn",
    "identity/Metadata/empty",
    "Metadata identity variable is empty",
    "Add entries to Metadata variable",
    "checkPragmaMetadata"),

  E("CWS-R25-012", 1, "identity", "info",
    "identity/upgrade",
    "File uses legacy identity format — upgrade recommended",
    "Upgrade to Pragma/Metadata variable format",
    "checkPragmaMetadata"),

  E("CWS-R25-013", 1, "identity", "warn",
    "identity/pragma-static",
    "Missing PRAGMA static declaration",
    "Add pub static PRAGMA declaration",
    "checkPragmaMetadata"),

  E("CWS-R25-014", 1, "identity", "warn",
    "identity/metadata-static",
    "Missing METADATA static declaration",
    "Add pub static METADATA declaration",
    "checkPragmaMetadata"),

  E("CWS-R25-015", 1, "identity", "info",
    "identity/pragma-parse",
    "PRAGMA static found but could not parse entries",
    "Ensure PRAGMA entries follow key-value format",
    "checkPragmaMetadata"),

  E("CWS-R25-016", 1, "identity", "info",
    "identity/metadata-parse",
    "METADATA static found but could not parse entries",
    "Ensure METADATA entries follow key-value format",
    "checkPragmaMetadata"),

  E("CWS-R25-017", 1, "identity", "info",
    "comment-meta/key",
    "Missing Key: in METADATA comment block",
    "Add Key: field to METADATA comment block",
    "checkCommentMetadata"),

  E("CWS-R25-018", 1, "identity", "info",
    "comment-meta/purpose",
    "Missing Purpose: in METADATA comment block",
    "Add Purpose: field to METADATA comment block",
    "checkCommentMetadata"),

  // --- 020-029: Section ordering ---

  E("CWS-R25-020", 1, "ordering", "warn",
    "closing/zone-order",
    "Closing zone order violation: {{details}}",
    "Reorder closing zones to match X1, X2, X3, X4, X5 sequence",
    "checkClosingZoneOrder"),

  E("CWS-R25-021", 1, "ordering", "warn",
    "closing/code-zone-order",
    "Closing code zone order violation: {{details}}",
    "Reorder closing code zones per schema",
    "checkClosingZoneOrder"),

  E("CWS-R25-022", 1, "ordering", "warn",
    "closing/doc-section-order",
    "Closing doc section order violation: {{details}}",
    "Reorder closing doc sections per schema",
    "checkClosingZoneOrder"),

  E("CWS-R25-023", 1, "ordering", "warn",
    "setup/subsection-order",
    "SETUP subsection order violation: {{actual}} should come before {{expected}}",
    "Reorder SETUP subsections per schema sequence",
    "checkSetupSubsectionOrder"),

  E("CWS-R25-024", 1, "ordering", "warn",
    "body/subsection-order",
    "BODY subsection order violation: {{actual}} should come before {{expected}}",
    "Reorder BODY subsections per schema sequence",
    "checkBodySubsectionOrder"),

  E("CWS-R25-025", 1, "ordering", "warn",
    "closing/x-order",
    "Closing X-section order violation",
    "Reorder X-sections in ascending order",
    "checkClosingXOrder"),

  E("CWS-R25-026", 1, "ordering", "error",
    "xref/P1-M1-key",
    "Cross-reference mismatch: pragma key ≠ metadata key",
    "Ensure pragma key and metadata key fields match",
    "checkCrossRefs"),

  E("CWS-R25-027", 1, "ordering", "warn",
    "xref/P5-M1-title",
    "Cross-reference mismatch: pragma title ≠ metadata title",
    "Ensure pragma title and metadata title fields match",
    "checkCrossRefs"),

  // --- 030-039: Required sections ---

  E("CWS-R25-030", 1, "required", "warn",
    "setup/required-subsection",
    "Missing required SETUP subsection: {{section}}",
    "Add the required SETUP subsection",
    "checkRequiredSetupSubsections"),

  E("CWS-R25-031", 1, "required", "warn",
    "body/required-subsection",
    "Missing required BODY subsection: {{section}}",
    "Add the required BODY subsection",
    "checkRequiredBodySubsections"),

  E("CWS-R25-032", 1, "required", "info",
    "closing/required-{{tag}}",
    "Missing required closing zone: {{tag}}",
    "Add the required closing zone (X1-X5)",
    "checkClosingRequiredZones"),

  E("CWS-R25-033", 1, "required", "warn",
    "form/required-section-missing",
    "Form required section missing: {{section}}",
    "Add the section required by this form",
    "checkFormRequiredSections"),

  E("CWS-R25-034", 1, "required", "warn",
    "form/reserved-section-present",
    "Reserved section present in wrong form: {{section}}",
    "Remove or move section to correct form",
    "checkFormReservedSections"),

  // --- 040-049: Metadata fields ---

  E("CWS-R25-040", 1, "metadata", "error",
    "metadata/{{key}}/required",
    "Missing _metadata.{{key}} — REQUIRED",
    "Add the required metadata field",
    "checkMetadataFields"),

  E("CWS-R25-041", 1, "metadata", "warn",
    "metadata/{{key}}/extension-identity",
    "Missing _metadata.{{key}} — extension identity",
    "Add the extension metadata field for full identity",
    "checkMetadataFields"),

  E("CWS-R25-042", 1, "metadata", "error",
    "metadata/{{key}}/required-context",
    "Missing _metadata.{{key}} — REQUIRED context",
    "Add the required context metadata field",
    "checkMetadataFields"),

  E("CWS-R25-043", 1, "metadata", "warn",
    "metadata/{{key}}/extension-context",
    "Missing _metadata.{{key}} — extension context",
    "Add the extension context metadata field",
    "checkMetadataFields"),

  E("CWS-R25-044", 1, "metadata", "error",
    "pragma/{{field}}/required",
    "Missing required pragma field: {{field}}",
    "Add the required pragma field",
    "checkPragmaFields"),

  E("CWS-R25-045", 1, "metadata", "warn",
    "pragma/{{field}}/recommended",
    "Missing recommended pragma field: {{field}}",
    "Add the recommended pragma field",
    "checkPragmaFields"),

  E("CWS-R25-046", 1, "metadata", "error",
    "metadata/M{{n}}/required",
    "Missing required metadata section M{{n}}",
    "Add the required metadata section",
    "checkMetadataSections"),

  E("CWS-R25-047", 1, "metadata", "warn",
    "metadata/M{{n}}/recommended",
    "Missing recommended metadata section M{{n}}",
    "Add the recommended metadata section",
    "checkMetadataSections"),

  E("CWS-R25-048", 1, "metadata", "info",
    "metadata/M{{n}}/extension",
    "Missing extension metadata section M{{n}}",
    "Add the extension metadata section for completeness",
    "checkMetadataSections"),

  // --- 050-059: Closing fields ---

  E("CWS-R25-050", 1, "closing", "error",
    "closing/{{key}}/required",
    "Missing _closing.{{key}} — REQUIRED",
    "Add the required closing field",
    "checkClosingFields"),

  E("CWS-R25-051", 1, "closing", "info",
    "closing/{{key}}/defined",
    "Missing _closing.{{key}} — defined closing section",
    "Add the closing section for completeness",
    "checkClosingFields"),

  E("CWS-R25-052", 1, "closing", "error",
    "closing/section/{{tag}}/required",
    "Missing required closing section: {{tag}}",
    "Add the required closing section",
    "checkClosingSections"),

  // --- 060-069: Form/content structure ---

  E("CWS-R25-060", 1, "content", "warn",
    "content/{{label}}",
    "Missing _content.{{label}} zone label",
    "Add zone label to _content table",
    "checkContentLabels"),

  E("CWS-R25-061", 1, "content", "warn",
    "content/containment",
    "No >> containment entries — no folder contents declared",
    "Add >> containment entries listing folder contents",
    "checkContentBlock"),

  E("CWS-R25-062", 1, "content", "info",
    "content/cc-label",
    "Missing Cc — Content Configurations label",
    "Add Cc label in content block",
    "checkContentBlock"),

  E("CWS-R25-063", 1, "content", "error",
    "top/{{section}}_type",
    "Section '{{section}}' must be an object",
    "Change section to a JSON object {}",
    "lintOfdFile"),

  E("CWS-R25-064", 1, "content", "error",
    "top/children",
    "Missing required field: children (array)",
    "Add children array listing folder contents",
    "lintOfdFile"),

  E("CWS-R25-065", 1, "content", "error",
    "top/children_type",
    "Field 'children' must be an array",
    "Change children to a JSON array []",
    "lintOfdFile"),

  E("CWS-R25-066", 1, "content", "warn",
    "top/{{field}}/recommended",
    "Missing recommended field: {{field}}",
    "Add recommended field for completeness",
    "lintOfdFile"),

  E("CWS-R25-067", 1, "content", "info",
    "top/{{field}}/optional",
    "Missing optional field: {{field}}",
    "Add optional field for full documentation",
    "lintOfdFile"),

  E("CWS-R25-068", 1, "content", "warn",
    "children/empty",
    "Children array is empty — folder describes no contents",
    "Add child entries for folder contents",
    "lintOfdChildren"),

  E("CWS-R25-069", 1, "content", "info",
    "pragma/legend",
    "Missing pragma legend — recommended for documenting field requirements",
    "Add _pragma_legend section with field documentation",
    "checkPragma"),

  // --- 070-079: Format-specific ---

  E("CWS-R25-070", 1, "format", "warn",
    "metadata/C7_classification/tags",
    "Metadata C7 classification tags issue",
    "Review C7 classification tags",
    "checkMetadataFields"),

  E("CWS-R25-071", 1, "format", "info",
    "pragma/{{key}}",
    "Missing pragma key: {{key}}",
    "Add the required pragma field",
    "checkPragma"),

  // R25-072 REMOVED: "metadata/{{key}}" was an old 2-segment pattern.
  // Handlers now emit 3-segment rules (metadata/M1/required, etc.)
  // which match specific codes R25-040, R25-046, R25-047.

  // R25-073 REMOVED: "closing/{{key}}" was an old 2-segment pattern.
  // Handlers now emit 3-segment rules (closing/section/POLICY/required, etc.)
  // which match specific code R25-052.

  E("CWS-R25-074", 1, "format", "warn",
    "metadata/M1/{{field}}",
    "Missing M1 identity sub-field: {{field}}",
    "Add the recommended M1 identity field",
    "checkMetadata"),

  E("CWS-R25-075", 1, "format", "warn",
    "metadata/M2/{{field}}",
    "Missing M2 state sub-field: {{field}}",
    "Add the recommended M2 state field",
    "checkMetadata"),

  // --- 080-089: Omni-specific structure ---

  E("CWS-R25-080", 1, "omni", "error",
    "identity/{{dir}}",
    "Missing required directive: {{dir}}",
    "Add the required identity directive",
    "checkIdentityDirectives"),

  E("CWS-R25-081", 1, "omni", "error",
    "state/{{dir}}",
    "Missing required state directive: {{dir}}",
    "Add the required state directive",
    "checkStateDirectives"),

  E("CWS-R25-082", 1, "omni", "warn",
    "state/@updated",
    "Missing @updated — recommended for tracking changes",
    "Add @updated directive with date",
    "checkStateDirectives"),

  E("CWS-R25-083", 1, "omni", "warn",
    "state/@status",
    "Missing @status — recommended",
    "Add @status directive (Active, Draft, etc.)",
    "checkStateDirectives"),

  E("CWS-R25-084", 1, "omni", "error",
    "attribution/{{dir}}",
    "Missing required attribution directive: {{dir}}",
    "Add the required attribution directive",
    "checkAttributionDirectives"),

  E("CWS-R25-085", 1, "omni", "warn",
    "deps/@from",
    "Missing @from — where does this folder derive from?",
    "Add @from directive indicating source",
    "checkDependencies"),

  E("CWS-R25-086", 1, "omni", "warn",
    "deps/provides",
    "Missing => provides — what does this folder export?",
    "Add => provides directive listing exports",
    "checkDependencies"),

  E("CWS-R25-087", 1, "omni", "warn",
    "closing/@navigate",
    "Missing @navigate in closing — quick reference path",
    "Add @navigate directive for navigation",
    "checkClosingBlock"),

  // --- 090-099: OFD-specific structure ---

  E("CWS-R25-090", 1, "ofd", "error",
    "identity/{{field}}/required",
    "Missing required identity field: {{field}}",
    "Add required identity field",
    "lintOfdIdentity"),

  E("CWS-R25-091", 1, "ofd", "error",
    "identity/{{field}}_type",
    "Identity field '{{field}}' must be a string",
    "Change field value to a string",
    "lintOfdIdentity"),

  E("CWS-R25-092", 1, "ofd", "warn",
    "identity/{{field}}/recommended",
    "Missing recommended identity field: {{field}}",
    "Add recommended identity field",
    "lintOfdIdentity"),

  E("CWS-R25-093", 1, "ofd", "error",
    "state/{{field}}/required",
    "Missing required state field: {{field}}",
    "Add required state field",
    "lintOfdState"),

  E("CWS-R25-094", 1, "ofd", "error",
    "state/{{field}}_type",
    "State field '{{field}}' must be a string",
    "Change field value to a string",
    "lintOfdState"),

  E("CWS-R25-095", 1, "ofd", "warn",
    "state/{{field}}/recommended",
    "Missing recommended state field: {{field}}",
    "Add recommended state field",
    "lintOfdState"),

  E("CWS-R25-096", 1, "ofd", "error",
    "attribution/{{field}}/required",
    "Missing required attribution field: {{field}}",
    "Add required attribution field",
    "lintOfdAttribution"),

  E("CWS-R25-097", 1, "ofd", "error",
    "attribution/{{field}}_type",
    "Attribution field '{{field}}' must be a string",
    "Change field value to a string",
    "lintOfdAttribution"),

  E("CWS-R25-098", 1, "ofd", "info",
    "attribution/{{field}}/optional",
    "Missing optional attribution field: {{field}}",
    "Add optional field for full documentation",
    "lintOfdAttribution"),

  E("CWS-R25-099", 1, "ofd", "warn",
    "dependencies/{{field}}",
    "Missing recommended dependency field: {{field}}",
    "Add dependency field for traceability",
    "lintOfdDependencies"),
];

// ---------------------------------------------------------------------------
// Layer 2 — R[10] — CONTAINER
// ---------------------------------------------------------------------------
// Is code in the right place? Are identity fields correctly filled?
//
// 001-009: Content placement (code in correct blocks)
// 010-019: Identity registration & field validation
// 020-029: Type/key validation
// 030-039: Value checks (template/derived status, format/subtype match)
// 040-049: Emphasis/scaling
// 050-059: Closing placement
// 060-069: Schema-driven field validation
// ---------------------------------------------------------------------------

const R10: [string, ErrorCodeEntry][] = [

  // --- 001-009: Content placement ---

  E("CWS-R10-001", 2, "placement", "warn",
    "content/metadata-leak",
    "Code content found in METADATA block — belongs in SETUP or BODY",
    "Move code to appropriate block",
    "checkContentPlacement"),

  E("CWS-R10-002", 2, "placement", "warn",
    "content/block-placement",
    "Content in wrong block: {{details}}",
    "Move content to the correct block",
    "checkContentPlacement"),

  E("CWS-R10-003", 2, "placement", "info",
    "content/subsection-placement",
    "Content may belong in a different subsection: {{details}}",
    "Review content placement within block",
    "checkContentPlacement"),

  E("CWS-R10-004", 2, "placement", "warn",
    "closing/test-placement",
    "Test code found outside BODY — consider moving to test file",
    "Move test functions to a _test file",
    "checkClosingContentPlacement"),

  E("CWS-R10-005", 2, "placement", "warn",
    "closing/main-placement",
    "Main function found outside CLOSING — should be in CLOSING",
    "Move main() to CLOSING block",
    "checkClosingContentPlacement"),

  E("CWS-R10-006", 2, "placement", "warn",
    "closing/X6-template-only",
    "X6 section present in non-template file — template-only section",
    "Remove X6 section (only valid in templates)",
    "checkClosingX6TemplateOnly"),

  E("CWS-R10-007", 2, "placement", "info",
    "content/{{sep}}_tag",
    "Missing content separator tag companion: {{sep}}_tag",
    "Add tag companion for content separator",
    "checkContent"),

  // --- 010-019: Identity registration ---

  E("CWS-R10-010", 2, "identity", "info",
    "identity/register",
    "Identity registration check: {{details}}",
    "Ensure file registers its identity via Pragma/Metadata",
    "checkIdentityRegistration"),

  E("CWS-R10-011", 2, "identity", "warn",
    "identity/{{varName}}/{{group}}.{{field}}",
    "Identity field {{group}}.{{field}} issue in {{varName}}: {{details}}",
    "Fix identity field value per schema requirements",
    "checkPragmaMetadata"),

  E("CWS-R10-012", 2, "identity", "warn",
    "identity/{{varName}}/placeholder",
    "Identity variable {{varName}} contains unsubstituted placeholders",
    "Replace template placeholders with actual values",
    "checkPragmaMetadata"),

  E("CWS-R10-013", 2, "identity", "warn",
    "identity/PRAGMA/I2.subtype-value",
    "PRAGMA I2.subtype value doesn't match detected form",
    "Update I2.subtype to match file's actual form",
    "checkPragmaMetadata"),

  // --- 020-029: Type/key validation ---

  E("CWS-R10-020", 2, "validation", "error",
    "metadata/invalid-key/{{key}}",
    "Invalid metadata key: {{key}} — not in schema",
    "Remove or rename to a valid metadata key",
    "checkMetadataKeys"),

  E("CWS-R10-021", 2, "validation", "error",
    "metadata/{{key}}/type",
    "Metadata key {{key}} has wrong type",
    "Change value to match expected type",
    "checkMetadataKeys"),

  E("CWS-R10-022", 2, "validation", "info",
    "metadata/{{key}}/extension",
    "Metadata key {{key}} is an extension field",
    "Informational — extension fields are allowed",
    "checkMetadataKeys"),

  E("CWS-R10-023", 2, "validation", "error",
    "closing/invalid-key/{{key}}",
    "Invalid closing key: {{key}} — not in schema",
    "Remove or rename to a valid closing key",
    "checkClosingKeys"),

  E("CWS-R10-024", 2, "validation", "error",
    "closing/{{key}}/type",
    "Closing key {{key}} has wrong type",
    "Change value to match expected type",
    "checkClosingKeys"),

  E("CWS-R10-025", 2, "validation", "info",
    "closing/{{key}}/extension",
    "Closing key {{key}} is an extension field",
    "Informational — extension fields are allowed",
    "checkClosingKeys"),

  // R10-026 REMOVED: "{{path}}/{{field}}" was a placeholder catch-all.
  // No handler ever called checkSchemaFields. Specific codes exist for each
  // format's field validation (R25-040, R25-044, R25-090, etc.).

  // --- 030-039: Value checks ---

  E("CWS-R10-030", 2, "value", "info",
    "content/{{varName}}/template-at",
    "Template identity: {{varName}} at version {{value}}",
    "Informational — template version detected",
    "_sharedCheckMetadataContent"),

  E("CWS-R10-031", 2, "value", "info",
    "content/{{varName}}/template-status",
    "Template identity: {{varName}} status {{value}}",
    "Informational — template status detected",
    "_sharedCheckMetadataContent"),

  E("CWS-R10-032", 2, "value", "info",
    "content/{{varName}}/derived-at",
    "Derived identity: {{varName}} at version {{value}}",
    "Informational — derived version detected",
    "_sharedCheckMetadataContent"),

  E("CWS-R10-033", 2, "value", "info",
    "content/{{varName}}/derived-status",
    "Derived identity: {{varName}} status {{value}}",
    "Informational — derived status detected",
    "_sharedCheckMetadataContent"),

  E("CWS-R10-034", 2, "value", "info",
    "content/{{varName}}/subtype-mismatch",
    "Identity {{varName}} subtype doesn't match detected form",
    "Update subtype field to match file's actual form",
    "_sharedCheckMetadataContent"),

  E("CWS-R10-035", 2, "value", "info",
    "content/{{varName}}/format-mismatch",
    "Identity {{varName}} format doesn't match handler format",
    "Update format field to match file extension",
    "_sharedCheckMetadataContent"),

  // --- 040-049: Emphasis/scaling ---

  E("CWS-R10-040", 2, "emphasis", "info",
    "emphasis/setup-inverted",
    "SETUP emphasis is inverted for this form",
    "Review SETUP section emphasis balance",
    "checkSubtypeEmphasis"),

  E("CWS-R10-041", 2, "scaling", "info",
    "scaling/setup-size",
    "SETUP section size: {{details}}",
    "Review SETUP section size relative to expected",
    "checkScalingSignals"),

  E("CWS-R10-042", 2, "scaling", "info",
    "scaling/body-size",
    "BODY section size: {{details}}",
    "Review BODY section size relative to expected",
    "checkScalingSignals"),

  E("CWS-R10-043", 2, "emphasis", "info",
    "setup/alias-name",
    "SETUP subsection has alias name: {{details}}",
    "Informational — alias mapping detected",
    "checkSetupSubsectionOrder"),

  // --- 050-059: Closing placement ---

  E("CWS-R10-050", 2, "closing", "warn",
    "content/closing-zone/Ce",
    "Closing zone content error: {{details}}",
    "Fix closing zone code content",
    "_sharedCheckClosingContent"),

  E("CWS-R10-051", 2, "closing", "info",
    "content/closing-zone/Cv",
    "Closing zone validation: {{details}}",
    "Review closing zone content",
    "_sharedCheckClosingContent"),

  // --- 060-069: Schema-driven field validation (TOML) ---

  E("CWS-R10-060", 2, "schema", "error",
    "dependencies/provides_type",
    "Field 'dependencies.provides' must be an array",
    "Change provides to an array",
    "lintOfdDependencies"),

  E("CWS-R10-061", 2, "schema", "error",
    "dependencies/derives_type",
    "Field 'dependencies.derives' must be an array",
    "Change derives to an array",
    "lintOfdDependencies"),
];

// ---------------------------------------------------------------------------
// Layer 3 — R[05] — CONTENT
// ---------------------------------------------------------------------------
// Are code patterns, doc comments, and section content correct?
//
// 001-009: Documentation (doc comments, comment-meta blocks)
// 010-019: Template/derived content validation
// 020-029: Body content (required sections, reserve enforcement)
// 030-039: Closing content (zone content, depth, reserves)
// 040-049: Setup content (section content, reserves)
// 050-059: Metadata content (Pragma/Metadata field checks)
// 060-069: Children/grounding content (OFD)
// 070-079: Concept detection (ternary matrix enforcement)
// 080-089: Value validation (field content correctness)
// ---------------------------------------------------------------------------

const R05: [string, ErrorCodeEntry][] = [

  // --- 001-009: Documentation ---

  E("CWS-R05-001", 3, "doc", "warn",
    "doc/package",
    "Package doc comment: {{details}}",
    "Add or improve package documentation comment",
    "checkDocComments"),

  E("CWS-R05-002", 3, "doc", "warn",
    "doc/pragma-var",
    "Pragma variable doc comment: {{details}}",
    "Add documentation comment before Pragma variable",
    "checkDocComments"),

  E("CWS-R05-003", 3, "doc", "warn",
    "doc/metadata-var",
    "Metadata variable doc comment: {{details}}",
    "Add documentation comment before Metadata variable",
    "checkDocComments"),

  E("CWS-R05-004", 3, "doc", "info",
    "doc/template",
    "Template has no //! doc comment section",
    "Add //! doc comment block for module documentation",
    "checkDocComments"),

  E("CWS-R05-005", 3, "doc", "info",
    "comment-meta/biblical",
    "Missing Biblical:/Scripture: in METADATA comment block",
    "Add Scripture reference to METADATA comment block",
    "checkCommentMetadata"),

  E("CWS-R05-006", 3, "doc", "info",
    "comment-meta/version",
    "Missing Version: in METADATA comment block",
    "Add Version: field to METADATA comment block",
    "checkCommentMetadata"),

  E("CWS-R05-007", 3, "doc", "info",
    "setup/header-doc",
    "SETUP block could benefit from header documentation",
    "Add descriptive header to SETUP block",
    "checkSetupHeaderDoc"),

  E("CWS-R05-008", 3, "doc", "info",
    "body/subtype-subsections",
    "BODY subsections for form {{subtype}}: {{details}}",
    "Review BODY subsection organization for this form",
    "checkBodySubtypeContent"),

  // --- 010-019: Template/derived content ---

  E("CWS-R05-010", 3, "template", "warn",
    "template/pragma-static",
    "Template PRAGMA static has issues",
    "Fix template PRAGMA static declaration",
    "checkTemplateVsDerived"),

  E("CWS-R05-011", 3, "template", "warn",
    "template/metadata-static",
    "Template METADATA static has issues",
    "Fix template METADATA static declaration",
    "checkTemplateVsDerived"),

  E("CWS-R05-012", 3, "template", "warn",
    "template/placeholders",
    "Template has unresolved placeholders: {{details}}",
    "Replace or validate template placeholders",
    "checkTemplateVsDerived"),

  // --- 020-029: Body content ---

  E("CWS-R05-020", 3, "body", "info",
    "content/body-required/{{section}}",
    "Required BODY section {{section}}: {{details}}",
    "Add or populate required BODY section",
    "_sharedCheckBodyContent"),

  E("CWS-R05-021", 3, "body", "warn",
    "content/body-reserve/{{rsv}}",
    "Reserved BODY section {{rsv}} present — reserved for {{form}}",
    "Remove or move reserved section to correct form",
    "_sharedCheckBodyContent"),

  // --- 030-039: Closing content ---

  E("CWS-R05-030", 3, "closing", "info",
    "closing/{{tag}}-content",
    "Closing zone {{tag}} content: {{details}}",
    "Review closing zone content completeness",
    "checkClosingDocFieldContent"),

  E("CWS-R05-031", 3, "closing", "info",
    "closing/{{tag}}-depth",
    "Closing zone {{tag}} depth: {{details}}",
    "Review closing zone content depth",
    "checkClosingDocFieldContent"),

  E("CWS-R05-032", 3, "closing", "warn",
    "content/closing-reserve/{{rsv}}",
    "Reserved closing section {{rsv}} present — reserved for {{form}}",
    "Remove or move reserved section to correct form",
    "_sharedCheckClosingContent"),

  // --- 040-049: Setup content ---

  E("CWS-R05-040", 3, "setup", "info",
    "content/setup-section/Imports",
    "SETUP Imports section: {{details}}",
    "Review import section content",
    "_sharedCheckSetupContent"),

  E("CWS-R05-041", 3, "setup", "warn",
    "content/setup-reserve/{{rsv}}",
    "Reserved SETUP section {{rsv}} present — reserved for {{form}}",
    "Remove or move reserved section to correct form",
    "_sharedCheckSetupContent"),

  // --- 050-059: Metadata content ---

  E("CWS-R05-050", 3, "metadata", "warn",
    "content/metadata-check/directive",
    "Metadata directive content issue: {{details}}",
    "Fix metadata directive content",
    "_sharedCheckMetadataContent"),

  E("CWS-R05-051", 3, "metadata", "info",
    "content/metadata-check/Pragma",
    "Metadata Pragma content: {{details}}",
    "Review Pragma field content",
    "_sharedCheckMetadataContent"),

  E("CWS-R05-052", 3, "metadata", "info",
    "content/metadata-check/Metadata",
    "Metadata content: {{details}}",
    "Review Metadata field content",
    "_sharedCheckMetadataContent"),

  // --- 060-069: Children/grounding (OFD) ---

  E("CWS-R05-060", 3, "ofd", "error",
    "children/{{i}}/type",
    "Child entry {{i}} must be an object",
    "Change child entry to a JSON object",
    "lintOfdChildren"),

  E("CWS-R05-061", 3, "ofd", "error",
    "children/{{i}}/path",
    "Child entry {{i}} missing required 'path' field",
    "Add path field to child entry",
    "lintOfdChildren"),

  E("CWS-R05-062", 3, "ofd", "error",
    "children/{{i}}/path_type",
    "Child entry {{i}} 'path' must be a string",
    "Change path value to a string",
    "lintOfdChildren"),

  E("CWS-R05-063", 3, "ofd", "info",
    "children/{{i}}/child_type",
    "Child entry {{i}} missing 'type' field",
    "Add type field (file/dir) to child entry",
    "lintOfdChildren"),

  E("CWS-R05-064", 3, "ofd", "info",
    "children/{{i}}/description",
    "Child entry {{i}} missing 'description' field",
    "Add description field to child entry",
    "lintOfdChildren"),

  E("CWS-R05-065", 3, "ofd", "warn",
    "grounding/{{field}}",
    "Missing recommended grounding field: {{field}}",
    "Add grounding field for Kingdom context",
    "lintOfdGrounding"),

  E("CWS-R05-066", 3, "ofd", "error",
    "grounding/scriptures_type",
    "Field 'grounding.scriptures' must be an array",
    "Change scriptures to an array of objects",
    "lintOfdGrounding"),

  E("CWS-R05-067", 3, "ofd", "error",
    "grounding/scripture_{{i}}",
    "Scripture entry {{i}} must be an object",
    "Change scripture entry to an object with text and reference",
    "lintOfdGrounding"),

  E("CWS-R05-068", 3, "ofd", "warn",
    "grounding/scripture_{{i}}/text",
    "Scripture {{i}} missing 'text' field",
    "Add scripture text",
    "lintOfdGrounding"),

  E("CWS-R05-069", 3, "ofd", "warn",
    "grounding/scripture_{{i}}/reference",
    "Scripture {{i}} missing 'reference' field",
    "Add scripture reference (e.g., 'Genesis 1:1')",
    "lintOfdGrounding"),

  // --- 070-079: Concept detection (R[5] ternary matrix enforcement) ---

  E("CWS-R05-070", 3, "concept", "warn",
    "concept/{{container}}/{{concept}}",
    "Denied concept \"{{concept}}\" detected in \"{{container}}\"",
    "Move this code pattern to a container where it is granted",
    "validateContainerConcepts"),

  E("CWS-R05-071", 3, "concept", "info",
    "concept/{{container}}/{{concept}}/missing",
    "Expected concept \"{{concept}}\" not found in \"{{container}}\"",
    "Add the expected code pattern to this container, or verify it belongs elsewhere",
    "validateContainerConcepts"),

  // --- 080-089: Value validation (field content correctness) ---

  E("CWS-R05-080", 3, "value", "warn",
    "value/{{varName}}/{{group}}.{{field}}",
    "Invalid value for {{group}}.{{field}} in {{varName}}",
    "Fix the field value to match expected format/enum",
    "validateICFieldContent"),

  E("CWS-R05-081", 3, "value", "info",
    "values/{{group}}/{{field}}",
    "Invalid value for {{group}}/{{field}}",
    "Fix the field value to match expected format/enum",
    "validateTomlFieldValues"),

  E("CWS-R05-082", 3, "value", "warn",
    "values/consistency/{{field}}",
    "Cross-field consistency check: {{field}}",
    "Review field value for consistency with related fields",
    "validateTomlFieldValues"),

  // R05-083 REMOVED: "structure/scaling" was orphaned — handler emits
  // scaling/setup-size (R10-041) and scaling/body-size (R10-042) instead.
  // No handler ever emitted the "structure/scaling" rule string.
];

// ---------------------------------------------------------------------------
// Layer T — T[00] — TOOL INFRASTRUCTURE
// ---------------------------------------------------------------------------
// Internal tool errors — schema pipeline, composition, transpiler, registry,
// fill engine, data validation, database. NOT lint detections — these fire
// when the tooling itself breaks, not when a user's file has problems.
//
// 001-009: Schema pipeline (resolution, loading)
// 010-019: Schema parsing (TOML/JSONC validation)
// 020-029: Composition (target parsing, resolution)
// 030-039: Transpiler (mapping, form loading)
// 040-049: Registry (format registration)
// 050-059: Fill engine (generation)
// 060-069: Data validation (concept integrity, section registry)
// 070-079: Database (migration, query)
// ---------------------------------------------------------------------------

const T00: [string, ErrorCodeEntry][] = [

  // --- 001-009: Schema pipeline ---

  E("CWS-T00-001", 4, "pipeline", "error",
    "tool/pipeline/schema-not-found",
    "Schema not found: {{schemaId}}\nSearched: {{sources}}\nHint: Set CWS_STRUCT_SCHEMA_DIR to override the schema directory",
    "Set CWS_STRUCT_SCHEMA_DIR or place schemas in .cws-struct/schemas/",
    "SchemaPipeline.getText"),

  // --- 010-019: Schema parsing ---

  E("CWS-T00-010", 4, "schema", "error",
    "tool/schema/parse-error",
    "Schema parse error: {{error}}",
    "Fix JSONC syntax in the schema file — check brackets, quotes, commas",
    "loadTomlRules"),

  E("CWS-T00-011", 4, "schema", "error",
    "tool/schema/non-object",
    "Schema parsed to non-object — expected JSONC object",
    "Ensure the schema file contains a JSON object at the root",
    "loadTomlRules"),

  E("CWS-T00-012", 4, "schema", "error",
    "tool/schema/missing-contract",
    "Schema missing 'validation_contract' — cannot derive rules",
    "Add a validation_contract object to the schema",
    "loadTomlRules"),

  E("CWS-T00-013", 4, "schema", "error",
    "tool/schema/missing-contract-sections",
    "Schema validation_contract missing sections: {{missing}}",
    "Add the missing sections (metadata, content, closing) to validation_contract",
    "loadTomlRules"),

  // --- 020-029: Composition ---

  E("CWS-T00-020", 4, "composition", "error",
    "tool/composition/missing-target",
    "Composition target missing 'target' field",
    "Add a target object to the composition JSONC file",
    "parseCompositionTarget"),

  E("CWS-T00-021", 4, "composition", "error",
    "tool/composition/missing-fields",
    "Composition target missing required fields: pragma={{pragma}}, format={{format}}, form={{form}}",
    "Add pragma, format, and form fields to the composition target",
    "parseCompositionTarget"),

  E("CWS-T00-022", 4, "composition", "error",
    "tool/composition/missing-sides",
    "Composition target missing composition.structure or composition.content",
    "Add both structure and content objects to the composition",
    "parseCompositionTarget"),

  // --- 030-039: Transpiler ---

  E("CWS-T00-030", 4, "transpiler", "error",
    "tool/transpiler/missing-mapping",
    "Transpiler mapping schema missing 'mapping' field: {{schemaId}}",
    "Add a mapping object to the transpiler schema",
    "loadMapping"),

  E("CWS-T00-031", 4, "transpiler", "error",
    "tool/transpiler/missing-form",
    "Form structure schema missing 'form' field: {{schemaId}}",
    "Add a form object to the structure schema at forms/{{form}}/_structure.jsonc",
    "loadFormStructure"),

  E("CWS-T00-032", 4, "transpiler", "error",
    "tool/transpiler/missing-content",
    "Form content schema missing 'content_expectations' field: {{schemaId}}",
    "Add content_expectations to the content schema at forms/{{form}}/{{language}}.jsonc",
    "loadFormContent"),

  // --- 040-049: Registry ---

  E("CWS-T00-040", 4, "registry", "error",
    "tool/registry/duplicate-format",
    "Format '{{format}}' already registered",
    "Check for duplicate registerFormat() calls — each format name must be unique",
    "registerFormat"),

  // --- 050-059: Fill engine ---

  E("CWS-T00-050", 4, "fill", "error",
    "tool/fill/no-content",
    "Schema for {{format}} has no fill_content section — cannot generate file",
    "Add fill_content to the {{format}} schema to enable generation",
    "generateFile"),

  // --- 060-069: Data validation ---

  // T00-060 REMOVED: "tool/data/missing-concept" was dead code.
  // No buildConceptRegistry() function exists. Re-add when concept registry
  // validation is implemented.

  // T00-061 REMOVED: "tool/data/concept-count" was dead code.
  // No buildConceptRegistry() function exists. Re-add when concept registry
  // validation is implemented.

  E("CWS-T00-062", 4, "data", "error",
    "tool/data/duplicate-section",
    "Duplicate section tag: '{{tag}}'",
    "Each section tag must be unique — check for duplicate entries in the section registry",
    "buildRegistry"),

  E("CWS-T00-063", 4, "data", "info",
    "tool/data/concept-detector-loaded",
    "Concept detectors loaded: {{count}}/11 with detect patterns for {{language}}",
    "Informational — concept detector coverage for this language",
    "loadConceptDetectors"),

  E("CWS-T00-064", 4, "data", "error",
    "tool/data/concept-detector-regex",
    "Invalid detect regex in concept schema: {{concept}}/{{pattern}} — {{error}}",
    "Fix the regex in the R5_patterns schema file",
    "compilePattern"),

  E("CWS-T00-065", 4, "data", "warn",
    "tool/data/concept-detector-gap",
    "Concept schema missing detect patterns: {{concept}}/{{language}}",
    "Add detect patterns to the R5_patterns schema for bidirectional support",
    "loadConceptDetectors"),

  // --- 070-079: Data layer integrity (machinery soundness) ---

  E("CWS-T00-070", 4, "integrity", "error",
    "tool/integrity/catalog",
    "Error catalog integrity failure: {{details}}",
    "Run deno task test --filter='T00-070' to diagnose",
    "errors_test.ts"),

  E("CWS-T00-071", 4, "integrity", "error",
    "tool/integrity/format-rules",
    "Format rules loading failure: {{details}}",
    "Check _format.jsonc schema files in schemas/",
    "format_rules_test.ts"),

  E("CWS-T00-072", 4, "integrity", "error",
    "tool/integrity/non-code-loader",
    "Non-code category loading failure: {{details}}",
    "Check _non-code.jsonc schema files in schemas/",
    "non_code_test.ts"),

  E("CWS-T00-073", 4, "integrity", "error",
    "tool/integrity/type-guards",
    "Data type guard failure: {{details}}",
    "Check type guard functions in lib/data/types.ts",
    "types_test.ts"),

  E("CWS-T00-074", 4, "integrity", "error",
    "tool/integrity/registries",
    "Registry integrity failure: {{details}}",
    "Check section/form/placement/concept registries in lib/data/",
    "registries_test.ts"),

  E("CWS-T00-075", 4, "integrity", "error",
    "tool/integrity/concept-detectors",
    "Concept detector integrity failure: {{details}}",
    "Check concept detector schemas in schemas/R5_patterns/",
    "concept_detectors_test.ts"),

  E("CWS-T00-076", 4, "integrity", "error",
    "tool/integrity/format-integration",
    "Format integration failure: {{details}}",
    "Check format data pipeline — schema load + compose + validate",
    "format_integration_test.ts"),

  E("CWS-T00-077", 4, "integrity", "error",
    "tool/integrity/non-code-integration",
    "Non-code integration failure: {{details}}",
    "Check non-code pipeline — load + compose + pattern match",
    "non_code_integration_test.ts"),

  // --- 080-089: Engine integrity ---

  E("CWS-T00-080", 4, "integrity", "error",
    "tool/integrity/format-registry",
    "Format handler registry failure: {{details}}",
    "Check registerFormat/getFormat/detectFormat in lib/engine/registry.ts",
    "registry_test.ts"),

  E("CWS-T00-081", 4, "integrity", "error",
    "tool/integrity/pipeline-contract",
    "Pipeline layer contract failure: {{details}}",
    "Check PIPELINE_LAYERS and layer checks in lib/data/pipeline.ts",
    "pipeline_test.ts"),

  E("CWS-T00-082", 4, "integrity", "error",
    "tool/integrity/cascade",
    "Cascade chain mechanics failure: {{details}}",
    "Check cascade functions in lib/engine/cascade.ts",
    "cascade_test.ts"),

  E("CWS-T00-083", 4, "integrity", "error",
    "tool/integrity/discovery",
    "File discovery failure: {{details}}",
    "Check discovery logic in lib/engine/",
    "discovery_test.ts"),

  E("CWS-T00-084", 4, "integrity", "error",
    "tool/integrity/transform",
    "Transform pipeline failure: {{details}}",
    "Check transform functions in lib/engine/transform.ts",
    "transform_test.ts"),

  E("CWS-T00-085", 4, "integrity", "error",
    "tool/integrity/output",
    "Output formatting failure: {{details}}",
    "Check output functions in lib/engine/output.ts",
    "output_test.ts"),

  E("CWS-T00-086", 4, "integrity", "error",
    "tool/integrity/logging",
    "Logging system failure: {{details}}",
    "Check logging functions in lib/engine/logging.ts",
    "logging_test.ts"),

  E("CWS-T00-087", 4, "integrity", "error",
    "tool/integrity/transpiler-mappings",
    "Transpiler mapping integrity failure: {{details}}",
    "Check transpiler mapping loader in lib/engine/transpiler-mappings.ts",
    "transpiler_test.ts"),

  E("CWS-T00-088", 4, "integrity", "error",
    "tool/integrity/fill-engine",
    "Fill engine integrity failure: {{details}}",
    "Check fill/generation functions in lib/engine/fill.ts",
    "fill_test.ts"),

  E("CWS-T00-089", 4, "integrity", "error",
    "tool/integrity/debug-tools",
    "Debug/inspect/suggest tool failure: {{details}}",
    "Check debug tools in lib/engine/debug.ts, inspect.ts, suggest.ts",
    "debug_test.ts"),

  // --- 090-099: Foundation integrity ---

  E("CWS-T00-090", 4, "integrity", "error",
    "tool/integrity/result-builders",
    "Result constructor contract failure: {{details}}",
    "Check error/warn/info/summarize in lib/foundation/types.ts",
    "types_test.ts"),

  E("CWS-T00-091", 4, "integrity", "error",
    "tool/integrity/health-scoring",
    "Health scoring algorithm failure: {{details}}",
    "Check computeContainerScore/computeBlockScore/computeHealthScore",
    "health_test.ts"),

  E("CWS-T00-092", 4, "integrity", "error",
    "tool/integrity/config",
    "Configuration loading failure: {{details}}",
    "Check config loading in lib/foundation/config.ts",
    "config_test.ts"),

  E("CWS-T00-093", 4, "integrity", "error",
    "tool/integrity/code-schema",
    "Code schema contract failure: {{details}}",
    "Check code schema loader in lib/foundation/code-schema.ts",
    "code_schema_test.ts"),

  E("CWS-T00-094", 4, "integrity", "error",
    "tool/integrity/composition-loader",
    "Composition loader failure: {{details}}",
    "Check composition-loader.ts contracts",
    "composition_test.ts"),

  E("CWS-T00-095", 4, "integrity", "error",
    "tool/integrity/api-surface",
    "API surface contract failure: {{details}}",
    "Check public API exports in lib/api/",
    "api_test.ts"),

  E("CWS-T00-096", 4, "integrity", "error",
    "tool/integrity/schema-contract",
    "Schema contract failure: {{details}}",
    "Check schema loading in lib/foundation/schema.ts",
    "schema_test.ts"),

  // --- 100-109: Shared utilities ---

  E("CWS-T00-100", 4, "integrity", "error",
    "tool/integrity/chunker",
    "Line chunker failure: {{details}}",
    "Check chunkLines/classifyChunks in shared/",
    "chunker_test.ts"),

  E("CWS-T00-101", 4, "integrity", "error",
    "tool/integrity/go-chunker",
    "Go-specific chunker failure: {{details}}",
    "Check Go-specific chunk classification",
    "go_chunker_test.ts"),

  E("CWS-T00-102", 4, "integrity", "error",
    "tool/integrity/non-code-patterns",
    "Non-code pattern matching failure: {{details}}",
    "Check non-code pattern recognition in shared/",
    "non_code_patterns_test.ts"),

  E("CWS-T00-103", 4, "integrity", "error",
    "tool/integrity/format-config",
    "Format configuration failure: {{details}}",
    "Check format config composition in shared/",
    "format_config_test.ts"),

  E("CWS-T00-104", 4, "integrity", "error",
    "tool/integrity/lint-grid",
    "Lint grid failure: {{details}}",
    "Check lint grid operations in shared/",
    "lint_grid_test.ts"),

  E("CWS-T00-105", 4, "integrity", "error",
    "tool/integrity/concept-check",
    "Concept detection failure: {{details}}",
    "Check concept detection in shared/concept-check",
    "concept_check_test.ts"),

  E("CWS-T00-106", 4, "integrity", "error",
    "tool/integrity/scope-analysis",
    "Scope analysis failure: {{details}}",
    "Check scope analysis in shared/scope-analysis",
    "scope_analysis_test.ts"),

  E("CWS-T00-107", 4, "integrity", "error",
    "tool/integrity/go-handler-functions",
    "Go handler function failure: {{details}}",
    "Check classifyGoLine, getTopLevelDeclarations, etc. in go.ts",
    "functions_test.ts"),

  E("CWS-T00-108", 4, "integrity", "error",
    "tool/integrity/rust-handler-functions",
    "Rust handler function failure: {{details}}",
    "Check classifyRustLine, detectRustBlocks, etc. in rust.ts",
    "functions_test.ts"),

  E("CWS-T00-109", 4, "integrity", "error",
    "tool/integrity/toml-handler-functions",
    "TOML handler function failure: {{details}}",
    "Check TOML parsing/validation functions in toml.ts",
    "functions_test.ts"),

  // --- 110-119: Integration ---

  E("CWS-T00-110", 4, "integrity", "error",
    "tool/integrity/adapter",
    "Handler adapter contract failure: {{details}}",
    "Check adapter integration in integration/",
    "adapter_test.ts"),

  E("CWS-T00-111", 4, "integrity", "error",
    "tool/integrity/regression",
    "Regression test failure: {{details}}",
    "Check regression fixtures in integration/",
    "regression_test.ts"),

  E("CWS-T00-112", 4, "integrity", "error",
    "tool/integrity/transform-integration",
    "Transform integration failure: {{details}}",
    "Check transform integration in integration/",
    "transform_test.ts"),

  E("CWS-T00-113", 4, "integrity", "error",
    "tool/integrity/pipeline-integration",
    "Pipeline integration failure: {{details}}",
    "Check pipeline integration in integration/pipeline/",
    "pipeline_integration_test.ts"),

  E("CWS-T00-114", 4, "integrity", "error",
    "tool/integrity/root-integration",
    "Root integration test failure: {{details}}",
    "Check end-to-end integration in integration/root/",
    "integration_test.ts"),

  // --- 120-129: CLI ---

  E("CWS-T00-120", 4, "integrity", "error",
    "tool/integrity/cli-entry",
    "CLI entry point failure: {{details}}",
    "Check CLI parsing and dispatch in mod.ts",
    "mod_test.ts"),

  E("CWS-T00-121", 4, "integrity", "error",
    "tool/integrity/diff-aware",
    "Diff-aware filtering failure: {{details}}",
    "Check diff-aware logic in cli/diff-aware",
    "diff_aware_test.ts"),

  // --- 130-139: Environment & Structure ---

  E("CWS-T00-130", 4, "integrity", "error",
    "tool/integrity/environment",
    "Environment verification failure: {{details}}",
    "Check verifyEnvironment in lib/verify/env.ts",
    "env_test.ts"),

  E("CWS-T00-131", 4, "integrity", "error",
    "tool/integrity/test-structure",
    "Test directory structure failure: {{details}}",
    "Check test directory mirrors detection hierarchy",
    "mod_test.ts"),
];

// ---------------------------------------------------------------------------
// Catalog Aggregation
// ---------------------------------------------------------------------------

/** All catalog entries concatenated. */
const ALL_ENTRIES: [string, ErrorCodeEntry][] = [
  ...R50,
  ...R25,
  ...R10,
  ...R05,
  ...T00,
];

/**
 * LINT_RULES — master map keyed by rule string (or pattern).
 * Used for forward lookup: handler rule string → error code.
 */
export const LINT_RULES: ReadonlyMap<string, ErrorCodeEntry> =
  new Map(ALL_ENTRIES);

/**
 * ERROR_CODES — master map keyed by error code string.
 * Used for reverse lookup: CWS-R50-001 → full entry.
 */
export const ERROR_CODES: ReadonlyMap<string, ErrorCodeEntry> = new Map(
  ALL_ENTRIES.map(([_, entry]) => [entry.code, entry] as [string, ErrorCodeEntry]),
);

// ---------------------------------------------------------------------------
// Domain Mapping — the "depth" dimension
// ---------------------------------------------------------------------------
//
// Three dimensions of error code navigation (Proverbs 24:3-4):
//   X — Layer (Wisdom):       R50 → R25 → R10 → R05 (detection granularity)
//   Y — Block (Knowledge):    METADATA → SETUP → BODY → CLOSING (structural position)
//   Z — Domain (Understanding): What ASPECT of the check (concept, identity, etc.)
//
// The domain is derived from the existing category field. Each category maps to
// a 3-letter uppercase code, enabling structured queries:
//   "all concept codes"          → query({ domain: "CON" })
//   "all R05 concept codes"      → query({ layer: 3, domain: "CON" })
//   "all identity codes"         → query({ domain: "IDE" })
//
// The dimensional code format: {LAYER}.{DOMAIN}.{SEQ}
//   Example: R05.CON.01 = Content layer, Concept domain, first instance
//

/**
 * Category → domain code mapping.
 *
 * Domain codes are 3-letter uppercase, designed for grep/filter queries.
 * Every category in the catalog MUST have an entry here.
 */
export const CATEGORY_DOMAIN: Readonly<Record<string, string>> = {
  // Layer 0-3 domains (detection)
  system: "SYS",
  directive: "DIR",
  block: "BLK",
  style: "STY",
  classify: "CLS",
  identity: "IDE",
  ordering: "ORD",
  required: "REQ",
  metadata: "MTA",
  closing: "CLO",
  content: "CNT",
  format: "FMT",
  omni: "OMN",
  ofd: "OFD",
  placement: "PLC",
  validation: "VAL",
  value: "VLU",
  emphasis: "EMP",
  scaling: "EMP",      // Maps to same domain as emphasis
  schema: "SCH",
  doc: "DOC",
  template: "TPL",
  body: "BOD",
  setup: "SET",
  concept: "CON",
  language: "LNG",

  // Layer 4 domains (tool infrastructure)
  pipeline: "PIP",
  composition: "CMP",
  transpiler: "TRN",
  registry: "REG",
  fill: "FIL",
  data: "DAT",
  database: "DBA",
  integrity: "INT",
};

/** Reverse map: domain code → category names. */
const DOMAIN_TO_CATEGORIES: ReadonlyMap<string, string[]> = (() => {
  const map = new Map<string, string[]>();
  for (const [cat, dom] of Object.entries(CATEGORY_DOMAIN)) {
    const existing = map.get(dom) ?? [];
    existing.push(cat);
    map.set(dom, existing);
  }
  return map;
})();

/**
 * Get the 3-letter domain code for a category.
 * Falls back to first 3 chars uppercase if category is unknown.
 */
export function categoryToDomain(category: string): string {
  return CATEGORY_DOMAIN[category] ?? category.substring(0, 3).toUpperCase();
}

/**
 * Get the dimensional code for an error entry.
 *
 * Format: {LAYER}.{DOMAIN}.{SEQ}
 *
 * Examples:
 *   CWS-R05-070 → R05.CON.070
 *   CWS-R25-010 → R25.IDE.010
 *   CWS-T00-063 → T00.DAT.063
 *
 * This is the CPI-SI queryable form — each segment is a dimension:
 *   grep 'R05\\.CON\\.' results   → all concept codes at content layer
 *   grep '\\.CON\\.'   results   → all concept codes across all layers
 *   grep 'R05\\.'      results   → all content-level codes
 */
export function dimensionalCode(entry: ErrorCodeEntry): string {
  const domain = categoryToDomain(entry.category);
  const seq = entry.code.split("-")[2];
  return `${entry.layerName}.${domain}.${seq}`;
}

/**
 * All unique domain codes in the catalog, sorted.
 */
export const DOMAIN_CODES: readonly string[] = [...new Set(
  ALL_ENTRIES.map(([_, e]) => categoryToDomain(e.category)),
)].sort();

// ---------------------------------------------------------------------------
// Lookup Functions
// ---------------------------------------------------------------------------

/**
 * Get error code entry by exact rule string.
 * Returns undefined if no exact match found — try matchRule() for dynamic rules.
 */
export function getByRule(rule: string): ErrorCodeEntry | undefined {
  return LINT_RULES.get(rule);
}

/**
 * Get error code entry by error code (e.g., "CWS-R50-001").
 */
export function getByCode(code: string): ErrorCodeEntry | undefined {
  return ERROR_CODES.get(code);
}

/**
 * Match a runtime rule string against catalog patterns.
 *
 * Strategy:
 * 1. Exact match (fast path)
 * 2. Prefix match — for dynamic rules like "identity/Pragma/I1.key",
 *    finds the pattern "identity/{{varName}}/{{group}}.{{field}}"
 *
 * Dynamic patterns use {{placeholder}} syntax. A runtime rule like
 * "metadata/title" matches pattern "metadata/{{key}}/required" by
 * comparing the static prefix segments.
 */
export function matchRule(rule: string): ErrorCodeEntry | undefined {
  // 1. Exact match
  const exact = LINT_RULES.get(rule);
  if (exact) return exact;

  // 2. Pattern match — find the best matching pattern
  let bestMatch: ErrorCodeEntry | undefined;
  let bestScore = 0;

  for (const [pattern, entry] of LINT_RULES) {
    if (!pattern.includes("{{")) continue; // Skip static rules

    // Build regex from pattern: "identity/{{var}}/{{field}}" → "identity/[^/]+/[^/]+"
    const regexStr = "^" + pattern
      .replace(/[.*+?^${}()|[\]\\]/g, (m) => {
        // Don't escape our {{}} placeholders
        if (m === "{" || m === "}") return m;
        return "\\" + m;
      })
      .replace(/\{\{[^}]+\}\}/g, "[^/]+") + "$";

    try {
      const regex = new RegExp(regexStr);
      if (regex.test(rule)) {
        // Score by literal character count — more literals = more specific.
        // "metadata/M{{n}}/required" (20 literal chars) beats
        // "metadata/{{key}}/required" (18 literal chars).
        const score = pattern.replace(/\{\{[^}]+\}\}/g, "").length;
        if (score > bestScore) {
          bestScore = score;
          bestMatch = entry;
        }
      }
    } catch {
      // Skip invalid patterns
    }
  }

  return bestMatch;
}

/**
 * Get all error codes in a specific layer.
 */
export function getByLayer(layer: 0 | 1 | 2 | 3 | 4): ErrorCodeEntry[] {
  return ALL_ENTRIES
    .filter(([_, e]) => e.layer === layer)
    .map(([_, e]) => e);
}

/**
 * Get all error codes in a specific category.
 */
export function getByCategory(category: string): ErrorCodeEntry[] {
  return ALL_ENTRIES
    .filter(([_, e]) => e.category === category)
    .map(([_, e]) => e);
}

/**
 * Get all error codes in a specific domain (3-letter code).
 *
 * The domain is the "depth" dimension — what ASPECT of the file is being checked.
 * Domain codes map to categories: CON→concept, IDE→identity, PLC→placement, etc.
 *
 * Examples:
 *   getByDomain("CON") → all concept detection codes (R05-070, R05-071)
 *   getByDomain("IDE") → all identity codes across all layers
 *   getByDomain("SYS") → all system/IO codes
 */
export function getByDomain(domain: string): ErrorCodeEntry[] {
  const categories = DOMAIN_TO_CATEGORIES.get(domain);
  if (!categories) return [];
  return ALL_ENTRIES
    .filter(([_, e]) => categories.includes(e.category))
    .map(([_, e]) => e);
}

/**
 * 3D query — filter error codes by any combination of dimensions.
 *
 * The three dimensions (Proverbs 24:3-4):
 *   layer  (Wisdom)       — WHAT level of detection? (0-4)
 *   domain (Understanding) — WHAT aspect? (3-letter code or category name)
 *   severity               — HOW serious? (error, warn, info)
 *
 * Examples:
 *   query({ layer: 3, domain: "CON" })           → concept codes at R05
 *   query({ domain: "IDE" })                      → all identity codes
 *   query({ layer: 1, severity: "error" })        → R25 errors only
 *   query({ domain: "CON", severity: "warn" })    → concept warnings
 *   query({})                                      → all codes
 */
export function query(opts: {
  layer?: 0 | 1 | 2 | 3 | 4;
  domain?: string;
  severity?: "error" | "warn" | "info";
}): ErrorCodeEntry[] {
  // Resolve domain to categories
  let domainCategories: string[] | undefined;
  if (opts.domain) {
    // Accept either 3-letter code or category name
    if (opts.domain.length === 3 && opts.domain === opts.domain.toUpperCase()) {
      domainCategories = DOMAIN_TO_CATEGORIES.get(opts.domain) ?? undefined;
    } else {
      domainCategories = [opts.domain];
    }
  }

  return ALL_ENTRIES
    .filter(([_, e]) => {
      if (opts.layer !== undefined && e.layer !== opts.layer) return false;
      if (opts.severity && e.severity !== opts.severity) return false;
      if (domainCategories && !domainCategories.includes(e.category)) return false;
      return true;
    })
    .map(([_, e]) => e);
}

/**
 * Get a summary of the 3D catalog structure.
 *
 * Returns: per-layer counts broken down by domain.
 * Useful for diagnostics and coverage analysis.
 */
export function catalogStructure(): Record<string, Record<string, number>> {
  const structure: Record<string, Record<string, number>> = {};

  for (const [_, entry] of ALL_ENTRIES) {
    const layer = entry.layerName;
    const domain = categoryToDomain(entry.category);

    if (!structure[layer]) structure[layer] = {};
    structure[layer][domain] = (structure[layer][domain] ?? 0) + 1;
  }

  return structure;
}

/**
 * Total number of error codes in the catalog.
 */
export const CATALOG_SIZE = ALL_ENTRIES.length;

/**
 * Layer summaries — count of codes per layer.
 */
export const LAYER_COUNTS: Readonly<Record<string, number>> = {
  R50: R50.length,
  R25: R25.length,
  R10: R10.length,
  R05: R05.length,
  T00: T00.length,
};

// ---------------------------------------------------------------------------
// QoL — Self-Documenting Error Codes
// ---------------------------------------------------------------------------
//
// The catalog doesn't just store codes — it EXPLAINS them.
// These functions make the catalog the first place you go when
// you encounter a code, not the last.
//
// codeSummary() → one-liner for embedding in messages
// explain()     → multiline human explanation
// search()      → find codes by keyword across all fields
// related()     → structural neighbors in the catalog
// ---------------------------------------------------------------------------

/** Layer number → human-readable description. */
export const LAYER_DESCRIPTIONS: Readonly<Record<string, string>> = {
  R50: "EXISTENCE — Does the file have the right shape?",
  R25: "ORGANIZATION — Are sections present and ordered?",
  R10: "CONTAINER — Is content in the right place?",
  R05: "CONTENT — Are code patterns correct?",
  T00: "TOOL — Schema pipeline and composition",
};

/**
 * Rich single-line summary of an error code.
 *
 * Format: `CODE [DIM] (SEVERITY) CATEGORY — "TEMPLATE"`
 *
 * Examples:
 *   CWS-R50-020 [R50.BLK.020] (error) block — "Missing required block: {{block}}"
 *   CWS-R25-023 [R25.ORD.023] (warn) ordering — "Subsection out of order"
 *
 * Returns "(unknown code)" if code not found.
 */
export function codeSummary(code: string): string {
  const entry = ERROR_CODES.get(code);
  if (!entry) return `${code} (unknown code)`;
  const dim = dimensionalCode(entry);
  return `${entry.code} [${dim}] (${entry.severity}) ${entry.category} — "${entry.messageTemplate}"`;
}

/**
 * Multiline explanation of an error code.
 *
 * Shows every field in human-readable format — what it is, why it fires,
 * what to do about it, and how to find it in the codebase.
 *
 * This is the function you call when a user says "what is CWS-R50-020?"
 */
export function explain(code: string): string {
  const entry = ERROR_CODES.get(code);
  if (!entry) {
    return [
      `${code} — not found in catalog.`,
      "",
      `The catalog has ${CATALOG_SIZE} codes across ${Object.keys(LAYER_COUNTS).length} layers.`,
      `Did you mean one of these?`,
      ...nearbySequences(code).map(c => `  ${codeSummary(c)}`),
    ].join("\n");
  }

  const dim = dimensionalCode(entry);
  const domain = categoryToDomain(entry.category);
  const layerDesc = LAYER_DESCRIPTIONS[entry.layerName] ?? entry.layerName;

  return [
    `${entry.code} [${dim}]`,
    `${"─".repeat(40)}`,
    `Layer:      ${entry.layerName} — ${layerDesc}`,
    `Domain:     ${domain} (${entry.category})`,
    `Severity:   ${entry.severity}`,
    `Message:    "${entry.messageTemplate}"`,
    `Suggestion: "${entry.suggestionTemplate}"`,
    `Check:      ${entry.checkFunction}`,
    "",
    `Test filter: deno test --filter='${entry.code}'`,
    `Witness tag: [${entry.code}]`,
  ].join("\n");
}

/**
 * Search the catalog by keyword.
 *
 * Matches against: messageTemplate, suggestionTemplate, category,
 * checkFunction, and code itself. Case-insensitive.
 *
 * Results are sorted by relevance: code match first, then category match,
 * then message match, then suggestion match.
 *
 * Examples:
 *   search("missing block")  → codes about missing blocks
 *   search("identity")       → all identity-related codes
 *   search("R50")            → all R50 layer codes
 *   search("template")       → template-related codes
 */
export function search(queryStr: string): ErrorCodeEntry[] {
  const q = queryStr.toLowerCase();
  const scored: Array<{ entry: ErrorCodeEntry; score: number }> = [];

  for (const [_, entry] of ALL_ENTRIES) {
    let score = 0;

    // Exact code match (highest)
    if (entry.code.toLowerCase().includes(q)) score += 100;
    // Layer name match
    if (entry.layerName.toLowerCase().includes(q)) score += 50;
    // Category match
    if (entry.category.toLowerCase().includes(q)) score += 40;
    // Domain match
    if (categoryToDomain(entry.category).toLowerCase().includes(q)) score += 30;
    // Check function match
    if (entry.checkFunction.toLowerCase().includes(q)) score += 20;
    // Message template match
    if (entry.messageTemplate.toLowerCase().includes(q)) score += 10;
    // Suggestion match
    if (entry.suggestionTemplate.toLowerCase().includes(q)) score += 5;

    if (score > 0) scored.push({ entry, score });
  }

  // Sort by relevance (descending), then by code (ascending) for stability
  scored.sort((a, b) => b.score - a.score || a.entry.code.localeCompare(b.entry.code));

  return scored.map(s => s.entry);
}

/**
 * Find codes structurally related to this code.
 *
 * Returns:
 *   sameCategory — codes in the same category (e.g., all "block" codes)
 *   sameLayer    — codes at the same detection layer (e.g., all R50)
 *   sameCheck    — codes produced by the same check function
 *   nearby       — codes with adjacent sequence numbers (same layer, ±5)
 *
 * Useful for understanding the neighborhood of an error code.
 * "I see CWS-R25-023 — what else is in this area?"
 */
export function related(code: string): {
  sameCategory: ErrorCodeEntry[];
  sameLayer: ErrorCodeEntry[];
  sameCheck: ErrorCodeEntry[];
  nearby: ErrorCodeEntry[];
} {
  const entry = ERROR_CODES.get(code);
  if (!entry) {
    return { sameCategory: [], sameLayer: [], sameCheck: [], nearby: [] };
  }

  const sameCategory = ALL_ENTRIES
    .filter(([_, e]) => e.category === entry.category && e.code !== code)
    .map(([_, e]) => e);

  const sameLayer = ALL_ENTRIES
    .filter(([_, e]) => e.layer === entry.layer && e.code !== code)
    .map(([_, e]) => e);

  const sameCheck = ALL_ENTRIES
    .filter(([_, e]) => e.checkFunction === entry.checkFunction && e.code !== code)
    .map(([_, e]) => e);

  // Nearby: same layer, sequence within ±5
  const seqStr = code.split("-")[2] ?? "0";
  const seq = parseInt(seqStr, 10);
  const nearby = ALL_ENTRIES
    .filter(([_, e]) => {
      if (e.code === code || e.layer !== entry.layer) return false;
      const eSeqStr = e.code.split("-")[2] ?? "0";
      const eSeq = parseInt(eSeqStr, 10);
      return Math.abs(eSeq - seq) <= 5;
    })
    .map(([_, e]) => e);

  return { sameCategory, sameLayer, sameCheck, nearby };
}

/**
 * Find codes with nearby sequence numbers (helper for explain()).
 * Returns codes in the same layer with sequences within ±3 of the queried code.
 */
function nearbySequences(code: string): string[] {
  // Parse: CWS-R50-020 → layer "R50", seq 20
  const parts = code.split("-");
  if (parts.length !== 3) return [];

  const layerPrefix = parts[1] ?? ""; // "R50"
  const seqStr = parts[2] ?? "0";
  const seq = parseInt(seqStr, 10);
  if (isNaN(seq)) return [];

  const nearby: string[] = [];
  for (const [_, entry] of ALL_ENTRIES) {
    if (entry.layerName !== layerPrefix) continue;
    const entrySeqStr = entry.code.split("-")[2] ?? "0";
    const entrySeq = parseInt(entrySeqStr, 10);
    if (Math.abs(entrySeq - seq) <= 3 && entry.code !== code) {
      nearby.push(entry.code);
    }
  }

  return nearby.slice(0, 5); // Cap at 5 suggestions
}

/**
 * Pretty-print the catalog structure as a formatted string.
 *
 * Shows layer × domain grid with counts — the 3D shape of the catalog.
 * Useful for understanding coverage gaps or planning new error codes.
 */
export function catalogReport(): string {
  const structure = catalogStructure();
  const lines: string[] = [
    `Error Catalog — ${CATALOG_SIZE} codes across ${Object.keys(LAYER_COUNTS).length} layers`,
    "─".repeat(60),
    "",
  ];

  for (const [layer, domains] of Object.entries(structure)) {
    const desc = LAYER_DESCRIPTIONS[layer] ?? layer;
    const total = LAYER_COUNTS[layer] ?? 0;
    lines.push(`${layer} (${total} codes) — ${desc}`);

    const domainEntries = Object.entries(domains).sort((a, b) => b[1] - a[1]);
    for (const [domain, count] of domainEntries) {
      lines.push(`  ${domain}: ${count}`);
    }
    lines.push("");
  }

  return lines.join("\n");
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Error Code Catalog — 3D queryable. Proverbs 24:3-4.
//
// Three dimensions:
//   X — Layer (Wisdom):        R50, R25, R10, R05, T00
//   Y — Block (Knowledge):     METADATA, SETUP, BODY, CLOSING (from handler context)
//   Z — Domain (Understanding): SYS, DIR, BLK, IDE, CON, ... (from category)
//
// Two code formats coexist (incremental migration):
//   Flat:         CWS-R05-070           (backward compat, primary key)
//   Dimensional:  R05.CON.070           (CPI-SI queryable, computed)
//
// Query by any dimension:
//   getByLayer(3)              → all R05 codes
//   getByDomain("CON")        → all concept codes across layers
//   getByCategory("identity")  → all identity codes
//   query({ layer: 3, domain: "CON", severity: "warn" })  → concept warnings at R05
//
// When a handler emits warn(file, "concept/imports/function_call", ...), this
// catalog maps it to CWS-R05-070 (flat) = R05.CON.070 (dimensional).
//
// One standard. Three dimensions. No diverse weights.
//
// "Through wisdom is an house builded; and by understanding it is established:
//  And by knowledge shall the chambers be filled with all precious and
//  pleasant riches." — Proverbs 24:3-4
// ============================================================================
