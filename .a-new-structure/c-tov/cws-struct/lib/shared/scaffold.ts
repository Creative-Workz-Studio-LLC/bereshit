// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/shared/scaffold.ts
// key:     B-tov-cws-struct-lib-shared-scaffold
// title:   CWS Struct — Universal Scaffold Pipeline
// type:    Code (Library)
// version: a-02.00
// created: 2026-02-22
// updated: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Universal structural scaffold pipeline for all 4-block code handlers.
//          The ENTIRE scaffold flow is language-agnostic:
//
//          Whole → Block → Container → Content (R[50] → R[5])
//
//          The only language-specific part is WHAT the content says.
//          Handlers provide a ScaffoldAdapter that supplies:
//          - Code chunk parsing (language-specific syntax)
//          - Chunk classification (Go categories vs Rust categories)
//          - Metadata context extraction (Go directives vs Rust doc comments)
//
//          Everything else — block boundaries, section indices, overviews,
//          Reserved Omissions, METADATA block generation — is universal.
//          The schema's fill_content carries language-specific syntax
//          (var vs static, [][2]string vs &[(&str, &str)]) as data.
//
// biblical_foundation: "For we are labourers together with God."
//   — 1 Corinthians 3:9
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type {
  FormSectionConstraint, FormReservedSection, FormConstraints,
  SchemaFillContent,
} from "../foundation/mod.ts";
import type { LintResult } from "../foundation/mod.ts";
import { info } from "../foundation/mod.ts";
import { BLOCK_SEPARATOR_WIDTH, SUBSECTION_SEPARATOR_WIDTH } from "./types.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Types — shared scaffold data structures
// ---------------------------------------------------------------------------

/**
 * A section entry in the block overview — either active (can appear)
 * or reserved (structurally excluded for this form).
 *
 * Position determines display order. Tags are CamelCase identifiers
 * matching the schema's section definitions.
 */
export type SectionEntry =
  | { kind: "active"; position: number; tag: string; status: "REQUIRED" | "AVAILABLE" }
  | { kind: "reserved"; position: number; tag: string; whyReserved: string };

/**
 * Generic code chunk — language-agnostic container for parsed code.
 * Each handler's parser produces these, classified by a language-specific
 * category string. The scaffold pipeline treats them opaquely.
 */
export interface GenericChunk {
  /** Source lines belonging to this chunk. */
  lines: string[];
  /** Language-specific category (e.g., "import", "fn_decl", "test_module"). */
  category: string;
}

/** Where a code chunk should be placed in the 4-block structure. */
export type BlockTarget = "METADATA" | "SETUP" | "BODY" | "CLOSING";

/**
 * Language adapter for the scaffold pipeline.
 *
 * Each handler provides an implementation that supplies language-specific
 * parsing and classification. The scaffold pipeline uses these to build
 * the universal 4-block structure.
 *
 * The adapter answers: "For THIS language, how do you parse code and
 * classify what goes where?" Everything else — the structure itself,
 * the block boundaries, the overviews, the Reserved Omissions — is
 * universal and handled by the shared scaffold.
 */
export interface ScaffoldAdapter {
  /** Format identifier (e.g., "go", "rust"). */
  format: string;

  /** File extension for this language (e.g., ".rs", ".go"). */
  fileExtension: string;

  /** Parse raw lines into semantic code chunks. */
  parseChunks(lines: string[]): GenericChunk[];

  /** Classify a chunk into its target block. */
  chunkToBlock(chunk: GenericChunk): BlockTarget;

  /** Map a SETUP chunk to its subsection tag. */
  chunkToSetupSection(chunk: GenericChunk): string;

  /** Map a BODY chunk to its subsection tag. */
  chunkToBodySection(chunk: GenericChunk): string;

  /**
   * Extract metadata context from parsed chunks.
   * Returns a substitution map for METADATA block generation.
   * Language-specific: reads Go directives vs Rust doc comments.
   */
  extractMetadataContext(
    metadataChunks: GenericChunk[],
    filePath: string,
    subtype: string | undefined,
    allLines?: string[],
  ): Record<string, string>;

  /** Is this chunk a test module/function? (→ CLOSING Cv) */
  isTestChunk(chunk: GenericChunk): boolean;

  /** Is this chunk a main/entry function? (→ CLOSING Ce) */
  isMainChunk(chunk: GenericChunk): boolean;

  /** Is this chunk structural noise (separator, blank, orphan comment)? */
  isOrphanChunk(chunk: GenericChunk): boolean;
}

/** Values preserved from existing CLOSING doc zones during --force re-scaffold. */
export interface ClosingFieldValues {
  policyScripture?: string;
  never?: string;
  careful?: string;
  safe?: string;
  relatedFiles?: string;
  validate?: string;
  note?: string;
  scripture?: string;
  anchor?: string;
}

/** Result of a scaffold operation. */
export interface ScaffoldResult {
  lines: string[];
  results: LintResult[];
}

/** Options controlling scaffold behavior. */
export interface ScaffoldOptions {
  /** Directory to write step-by-step intermediate files (--steps). */
  stepsDir?: string;
  /** Dry-run mode — don't write, just report what would change. */
  dryRun?: boolean;
}

// ---------------------------------------------------------------------------
// 2. Label formatting — CamelCase → readable
// ---------------------------------------------------------------------------

/**
 * Convert CamelCase tag to readable label: "CoreTypes" → "Core Types".
 * Tags with existing spaces or "&" pass through unchanged.
 */
export function tagToLabel(tag: string): string {
  if (tag.includes(" ")) return tag; // Already has spaces (e.g., "Constructors & Builders")
  return tag.replace(/([a-z])([A-Z])/g, "$1 $2");
}

// ---------------------------------------------------------------------------
// 3. Section index — merging active + reserved into ordered list
// ---------------------------------------------------------------------------

/**
 * Build a complete section index merging active + reserved sections in position order.
 * Reserved sections without known positions are appended at end.
 */
export function buildSectionIndex(
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

// ---------------------------------------------------------------------------
// 4. Section descriptions — compact labels for block overview TOC
// ---------------------------------------------------------------------------

/** Compact section description for block overview TOC. */
export const SECTION_DESCRIPTIONS: Record<string, string> = {
  // SETUP sections — Rust
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
  // SETUP sections — Go
  "Variables":      "Module-level var declarations",
  "TypeMethods":    "Type + associated methods",
  "CodeGeneration": "Generated code patterns",
  "InterfaceDefs":  "Behavioral contracts (shape, not fulfillment)",
  "BuildTags":      "Conditional compilation constraints",
  // BODY sections — Rust
  "IdentityAccess":       "OmniCode static accessor functions",
  "TraitImplementations": "Fulfilling contracts from SETUP",
  "Constructors & Builders": "new(), builders, typestate transitions",
  "CoreLogic":            "Primary operations, state transforms",
  "Queries & Accessors":  "Read-only &self methods",
  "Output & Display":     "Formatting, serialization",
  "FreeFunctions":        "Module-level public utilities",
  "Helpers":              "Support functions for run()",
  // BODY sections — Go
  "Organization":         "Init functions, package-level setup",
  "ErrorHandling":        "Error types, sentinel errors",
  "APIs":                 "Public API surface",
  "CoreOperations":       "Primary operations, state transforms",
};

// ---------------------------------------------------------------------------
// 5. Section index emission — bracket format [tag] → TOC output
// ---------------------------------------------------------------------------

/**
 * Emit a block overview section index using bracket format [tag].
 *
 * Bracket format avoids false-positive zone detection AND subsection pattern
 * detection. The CLOSING block discovered this — [Cv], [Ce], [X1] work where
 * numbered "1.", "2." can collide with linter patterns. Apply the lesson
 * everywhere.
 */
export function emitSectionIndex(out: string[], index: SectionEntry[]): void {
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

// ---------------------------------------------------------------------------
// 6. Closing zone labels and descriptions
// ---------------------------------------------------------------------------

/** CLOSING zone descriptive labels — architecturally stable, matches template. */
export const CLOSING_ZONE_LABELS: Record<string, string> = {
  "Cv": "Cv \u2014 Closing Validation",
  "Ce": "Ce \u2014 Closing Execution",
  "Cc": "Cc \u2014 Closing Cleanup",
  "X1": "X1: Modification Policy",
  "X2": "X2: Extension Points",
  "X3": "X3: Troubleshooting",
  "X4": "X4: Reference",
  "X5": "X5: Closing Note",
  "X6": "X6: Template Guide",
};

/** Compact closing zone descriptions for block overview TOC. */
export const CLOSING_ZONE_DESCRIPTIONS: Record<string, string> = {
  "Cv": "Closing Validation (tests)",
  "Ce": "Closing Execution (entry point or absence)",
  "Cc": "Closing Cleanup (resource teardown)",
  "X1": "Modification Policy",
  "X2": "Extension Points",
  "X3": "Troubleshooting",
  "X4": "Reference",
  "X5": "Closing Note",
};

// ---------------------------------------------------------------------------
// 7. Closing overview emission — bracket-tagged zone list
// ---------------------------------------------------------------------------

/**
 * Emit the CLOSING block overview — bracket-tagged zone list.
 * Matches exists.rs production standard: [Cv], [Ce], [Cc], [X1]-[X5].
 */
export function emitClosingOverview(out: string[]): void {
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
// 8. Form-aware Reserved Omission — shared across all code handlers
// ---------------------------------------------------------------------------

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
export function buildFormAwareReservedOmission(
  absentSections: FormSectionConstraint[],
  reservedSections: FormReservedSection[],
  formName?: string,
  mode: "strict" | "balance" | "growth" = "strict",
): string[] {
  const lines: string[] = [];
  const useGrouped = mode === "strict";

  // 1. Absent AVAILABLE sections — "Not needed in this module"
  const absentAvailable = absentSections.filter((s) => s.status === "AVAILABLE");

  if (absentAvailable.length > 0) {
    if (useGrouped) {
      lines.push(`// Available (not needed in this module):`);
      const tagList = absentAvailable.map((s) => tagToLabel(s.tag)).join(", ");
      lines.push(`//   ${tagList}`);
    } else {
      for (const section of absentAvailable) {
        lines.push(`//   ${tagToLabel(section.tag)} \u2014 Not needed in this module`);
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
      lines.push(`//`);
    }

    if (useGrouped) {
      const formLabel = formName ?? "this";
      lines.push(`// Reserved (structural \u2014 not used in ${formLabel} form):`);
    }

    const allReserved = [...grouped.values()].flat();
    const maxTagLen = useGrouped
      ? Math.max(...allReserved.map((s) => tagToLabel(s.tag).length))
      : 0;

    for (const [reason, sections] of grouped) {
      if (sections.length === 1) {
        const label = tagToLabel(sections[0]!.tag);
        if (useGrouped) {
          const padding = " ".repeat(maxTagLen - label.length);
          const shortReason = reason.split(".")[0]!.trim();
          lines.push(`//   ${label}${padding} \u2014 ${shortReason}.`);
        } else {
          lines.push(`//   ${label} \u2014 Reserved: ${reason}`);
        }
      } else {
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

// ---------------------------------------------------------------------------
// 8. Closing field extraction — preserve human-authored values during re-scaffold
// ---------------------------------------------------------------------------

/**
 * Extract closing field values from existing file content.
 * Called before strip so the scaffold can reuse human-authored values
 * instead of generating generic placeholders.
 *
 * Universal — the CLOSING zone comment format is the same across all
 * languages (// key: value).
 */
export function extractClosingFields(lines: string[]): ClosingFieldValues {
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

// ---------------------------------------------------------------------------
// 8b. Block structure stripping — remove scaffold to re-scaffold
// ---------------------------------------------------------------------------

/**
 * Strip all block structure from a file, leaving only semantic content.
 *
 * Removes: block banners, section headers, subsection separators, reserved
 * omissions, closing zone headers and synthetic content, template comments,
 * overview lines — everything the scaffold GENERATES.
 *
 * Preserves: pragma directives, doc comments, use/import statements, code
 * declarations, test modules — everything the scaffold CONSUMES.
 *
 * Universal — the comment format is the same across all // languages.
 * Language-specific exclusions (e.g., #[cfg(test)] in Rust) are handled
 * by the optional `preserveLine` callback.
 *
 * Called before structural scaffold when --force re-scaffolding.
 */
export function stripBlockStructure(
  lines: string[],
  preserveLine?: (trimmed: string) => boolean,
): string[] {
  const out: string[] = [];
  const blockBanner = /^\/\/\s*={4,}\s*$/;
  const blockTitle = /^\s*\/\/\s*(METADATA|SETUP|BODY|CLOSING)\s+BLOCK\s+\[/i;
  const blockEnd = /^\s*\/\/\s*END\s+(METADATA|SETUP|BODY|CLOSING)\s+\[END\]/i;
  const subsectionSep = /^\/\/\s*[\u2500]{4,}\s*$/;
  const reservedOmission = /^\s*\/\/\s*Reserved\s+Omission\s*$/i;
  const reservedLine = /^\s*\/\/\s{2,}\S/;
  const todoLine = /^\s*\/\/\s*TODO:/;
  const seeTemplateLine = /^\s*\/\/\s*See\s+seed\//;
  const emptyComment = /^\s*\/\/\s*$/;
  const setupBodyComment = /^\s*\/\/\s*(SETUP|BODY)\s+(makes things|made things)/i;
  const sectionOrderComment = /^\s*\/\/\s*(Section|Subsection)\s+order/i;
  const numberedOrderLine = /^\s*\/\/\s+\d+\.\s+/;

  // Closing zone headers and synthetic content
  const closingZoneHeader = /^\s*\/\/\s*(Cv|Ce|X1|X2|X3|X4|X5|X6)\b/;
  const closingFieldLine = /^\s*\/\/\s*(policy|scripture|never|careful|safe|related_files|validate|note|anchor|coverage_report):/;

  let inReservedOmission = false;
  let inClosingZone = false;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    const trimmed = line.trim();

    // Language-specific preservation hook
    if (preserveLine && preserveLine(trimmed)) {
      out.push(line);
      continue;
    }

    // Block banners: look for 3-line pattern ===, title, ===
    if (blockBanner.test(trimmed)) {
      const next = i + 1 < lines.length ? lines[i + 1]!.trim() : "";
      if (blockTitle.test(next) || blockEnd.test(next)) {
        i += 2;
        continue;
      }
      if (blockEnd.test(trimmed)) continue;
      continue;
    }

    // Block title/end on its own
    if (blockTitle.test(trimmed) || blockEnd.test(trimmed)) continue;

    // Subsection separators: \u2500 lines
    if (subsectionSep.test(trimmed)) {
      const next = i + 1 < lines.length ? lines[i + 1]!.trim() : "";
      const nextNext = i + 2 < lines.length ? lines[i + 2]!.trim() : "";
      if (!subsectionSep.test(next) && subsectionSep.test(nextNext)) {
        if (reservedOmission.test(next)) {
          inReservedOmission = true;
        }
        i += 2;
        continue;
      }
      continue;
    }

    // Reserved Omission block
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
    if (closingZoneHeader.test(trimmed)) {
      // Don't strip actual code that happens to match (e.g., #[cfg(test)])
      if (preserveLine && preserveLine(trimmed)) {
        out.push(line);
        continue;
      }
      inClosingZone = true;
      continue;
    }
    if (inClosingZone) {
      if (closingFieldLine.test(trimmed) || emptyComment.test(trimmed) || trimmed === "") {
        continue;
      }
      inClosingZone = false;
    }

    // Template/structural comments
    if (todoLine.test(trimmed)) continue;
    if (seeTemplateLine.test(trimmed)) continue;
    if (setupBodyComment.test(trimmed)) continue;
    if (sectionOrderComment.test(trimmed)) continue;
    if (numberedOrderLine.test(trimmed)) continue;

    // "No entry point" / "No tests yet" comments
    if (/^\s*\/\/\s*No (entry point|tests yet)/.test(trimmed)) continue;

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
// 9. METADATA block builder — universal, schema-driven
// ---------------------------------------------------------------------------

/**
 * Build the METADATA block from a pre-computed context map and schema fill_content.
 *
 * This is universal. The language-specific parts are:
 * - The context map (extracted by each handler's extractMetadataContext)
 * - The fill_content (schema carries language-specific syntax as data)
 *
 * The block layout, section headers, group comments, column alignment,
 * docstrings — all universal. The CreativeWorkzStudio coding standard
 * at work: structure is constant, content varies.
 */
export function buildMetadataBlock(
  ctx: Record<string, string>,
  fillContent: SchemaFillContent,
  format: string,
  mode: "strict" | "balance" | "growth" = "strict",
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

  const key = ctx["key"] ?? "[key]";
  const version = ctx["version"] ?? fillContent.defaults.version;
  const title = ctx["title"] ?? "[title]";
  const purpose = ctx["purpose"] ?? "[purpose]";
  const filename = ctx["filename"] ?? "[filename]";
  const subtypeVal = ctx["subtype"] ?? "[subtype]";
  const date = new Date().toISOString().slice(0, 10);

  // Auto-derived I3 fields
  const path = ctx["path"] ?? "[path]";
  const component = ctx["component"] ?? "[component]";
  const brief = ctx["brief"] ?? purpose;
  const provides = ctx["provides"] ?? "[provides]";

  // Substitution map — what we know from the file + defaults from schema
  const subs: Record<string, string> = {
    key, version, title, purpose, filename, subtype: subtypeVal, date,
    format,
    from: ctx["from"] ?? "[from]",
    crate_name: ctx["crate_name"] ?? "[crate_name]",
    package: ctx["package"] ?? "[package]",
    status: fillContent.defaults.status,
    organization: fillContent.defaults.organization,
    scripture: fillContent.defaults.scripture,
    scripture_text: fillContent.defaults.scripture_text,
    consumers: ctx["consumers"] ?? "[consumers]",
    path, component, brief, provides,
  };
  // Pull in extra defaults (architect, role, layer, etc.)
  for (const [k, v] of Object.entries(fillContent.defaults)) {
    if (!(k in subs)) subs[k] = v;
  }
  // Pull in extra context values
  for (const [k, v] of Object.entries(ctx)) {
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
   */
  function emitEntries(
    entries: [string, string][],
    syntax: { entry: string },
    groups?: { range: string; label: string }[],
  ): string[] {
    const lines: string[] = [];

    const maxFieldLen = useColumnAlignment
      ? Math.max(...entries.map(([f]) => f.length))
      : 0;

    let currentGroup = "";

    for (const [field, value] of entries) {
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

  // ── Block boundary
  out.push(`// ${eq}`);
  out.push(`// METADATA BLOCK [METADATA]`);
  out.push(`// ${eq}`);

  // ── Comment header (Key, Purpose)
  out.push(`//`);
  for (const comment of fillContent.metadataComment) {
    out.push(sub(comment));
  }
  out.push(`//`);
  out.push("");

  // ── PRAGMA section header
  const ig = fillContent.identityGroups;
  if (useSectionHeaders && ig) {
    out.push(`// ${dash}`);
    out.push(ig.sectionHeaders.pragma);
    out.push(`// ${dash}`);
    out.push("");
  }

  // ── Docstring
  if (useDocstrings && ig) {
    const pragmaDocstring = ig.pragma.find((g) => g.docstring)?.docstring;
    if (pragmaDocstring) {
      out.push(pragmaDocstring);
    }
  }

  // ── PRAGMA identity
  out.push(fillContent.identitySyntax.pragma.declaration);
  out.push(...emitEntries(
    fillContent.pragmaEntries,
    fillContent.identitySyntax.pragma,
    ig?.pragma,
  ));
  out.push(fillContent.identitySyntax.pragma.close);
  out.push("");

  // ── METADATA section header
  if (useSectionHeaders && ig) {
    out.push(`// ${dash}`);
    out.push(ig.sectionHeaders.metadata);
    out.push(`// ${dash}`);
    out.push("");
  }

  // ── Docstring
  if (useDocstrings && ig) {
    const metaDocstring = ig.metadata.find((g) => g.docstring)?.docstring;
    if (metaDocstring) {
      out.push(metaDocstring);
    }
  }

  // ── METADATA context
  out.push(fillContent.identitySyntax.metadata.declaration);
  out.push(...emitEntries(
    fillContent.metadataEntries,
    fillContent.identitySyntax.metadata,
    ig?.metadata,
  ));
  out.push(fillContent.identitySyntax.metadata.close);
  out.push("");

  // ── END boundary
  out.push(`// ${eq}`);
  out.push(`// END METADATA [END]`);
  out.push(`// ${eq}`);

  return out;
}

// ---------------------------------------------------------------------------
// 10. Structural scaffold orchestrator — universal R[50] → R[5] pipeline
// ---------------------------------------------------------------------------

/**
 * Generate the structural scaffold for an unstructured code file.
 *
 * This is Transform 0 — the DAR Recover step. Takes a file with a pragma
 * but no block boundaries and produces the full 4-block structure.
 *
 * The pipeline follows the CWS coding standard progression:
 *   R[50] WHOLE  — Identify the file, parse pragma, determine form
 *   R[25] BLOCK  — Build 4-block boundaries (METADATA → SETUP → BODY → CLOSING)
 *   R[10] CONTAINER — Fill sections within each block (subsections, zones)
 *   R[5]  CONTENT — Place code chunks into correct containers
 *
 * Returns null if no scaffolding is needed (file already has blocks).
 */
export async function structuralScaffold(
  adapter: ScaffoldAdapter,
  filePath: string,
  lines: string[],
  formConstraints: FormConstraints | null,
  subtype: string | undefined,
  fillContent: SchemaFillContent | undefined,
  preservedClosing?: ClosingFieldValues,
  opts?: ScaffoldOptions,
): Promise<ScaffoldResult | null> {
  const results: LintResult[] = [];
  const eq = "=".repeat(BLOCK_SEPARATOR_WIDTH);
  const dash = "\u2500".repeat(SUBSECTION_SEPARATOR_WIDTH);
  const ext = adapter.fileExtension;
  const stepsDir = opts?.stepsDir;

  // Steps support — create output directory
  if (stepsDir) {
    try { await Deno.mkdir(stepsDir, { recursive: true }); } catch { /* exists */ }
    // Step 0: Raw input (what the scaffold received)
    await Deno.writeTextFile(`${stepsDir}/00-raw-input${ext}`, lines.join("\n"));
  }

  // ── R[50]: WHOLE — Parse into semantic chunks ──────────────────────
  const chunks = adapter.parseChunks(lines);

  // ── R[25]: BLOCK — Classify chunks into blocks ─────────────────────
  const metadataChunks: GenericChunk[] = [];
  const setupChunks = new Map<string, GenericChunk[]>();
  const bodyChunks = new Map<string, GenericChunk[]>();
  const closingTestChunks: GenericChunk[] = [];
  const closingMainChunks: GenericChunk[] = [];
  const orphanChunks: GenericChunk[] = [];

  for (const chunk of chunks) {
    const target = adapter.chunkToBlock(chunk);

    if (target === "METADATA") {
      metadataChunks.push(chunk);
    } else if (target === "SETUP") {
      const section = adapter.chunkToSetupSection(chunk);
      if (!setupChunks.has(section)) setupChunks.set(section, []);
      setupChunks.get(section)!.push(chunk);
    } else if (target === "BODY") {
      if (adapter.isOrphanChunk(chunk)) {
        orphanChunks.push(chunk);
        continue;
      }
      const section = adapter.chunkToBodySection(chunk);
      if (!bodyChunks.has(section)) bodyChunks.set(section, []);
      bodyChunks.get(section)!.push(chunk);
    } else if (target === "CLOSING") {
      if (adapter.isTestChunk(chunk)) {
        closingTestChunks.push(chunk);
      } else if (adapter.isMainChunk(chunk)) {
        closingMainChunks.push(chunk);
      }
    }
  }

  // Steps: classification report — what each chunk became
  if (stepsDir) {
    const classReport: string[] = [
      `// Classification Report for ${filePath}`,
      `// Generated by cws-struct structural scaffold (--steps)`,
      `//`,
      `// METADATA chunks: ${metadataChunks.length}`,
    ];
    for (const c of metadataChunks) {
      classReport.push(`//   [${c.category}] ${c.lines[0]?.trim().substring(0, 70)}`);
    }
    classReport.push(`//`);
    classReport.push(`// SETUP sections: ${setupChunks.size}`);
    for (const [tag, sChunks] of setupChunks) {
      classReport.push(`//   ${tag}: ${sChunks.length} chunk(s)`);
      for (const c of sChunks) {
        classReport.push(`//     [${c.category}] ${c.lines[0]?.trim().substring(0, 60)}`);
      }
    }
    classReport.push(`//`);
    classReport.push(`// BODY sections: ${bodyChunks.size}`);
    for (const [tag, bChunks] of bodyChunks) {
      classReport.push(`//   ${tag}: ${bChunks.length} chunk(s)`);
      for (const c of bChunks) {
        classReport.push(`//     [${c.category}] ${c.lines[0]?.trim().substring(0, 60)}`);
      }
    }
    classReport.push(`//`);
    classReport.push(`// CLOSING: ${closingTestChunks.length} test(s), ${closingMainChunks.length} main(s)`);
    classReport.push(`//`);
    classReport.push(`// Orphaned (dropped): ${orphanChunks.length} chunk(s)`);
    for (const c of orphanChunks) {
      classReport.push(`//   [${c.category}] ${c.lines[0]?.trim().substring(0, 60)}`);
    }
    await Deno.writeTextFile(`${stepsDir}/01-classification.txt`, classReport.join("\n"));
    results.push(info(filePath, "transform/steps", `Step-by-step output: ${stepsDir}/`));
  }

  // ── R[25]+R[10]: BUILD OUTPUT — Block by block, container by container ──

  const out: string[] = [];

  // --- METADATA block ---
  // Pre-block content (pragma, directives, doc comments)
  for (const chunk of metadataChunks) {
    out.push(...chunk.lines);
  }
  if (out.length > 0 && out[out.length - 1]!.trim() !== "") {
    out.push("");
  }

  // Full METADATA block with identity statics from fill_content
  if (fillContent) {
    const metaCtx = adapter.extractMetadataContext(metadataChunks, filePath, subtype, lines);
    out.push(...buildMetadataBlock(metaCtx, fillContent, adapter.format));
  } else {
    out.push(`// ${eq}`);
    out.push(`// METADATA BLOCK [METADATA]`);
    out.push(`// ${eq}`);
    out.push(`//`);
    out.push(`// TODO: Add identity fields. See template for pattern.`);
    out.push(`//`);
    out.push("");
    out.push(`// ${eq}`);
    out.push(`// END METADATA [END]`);
    out.push(`// ${eq}`);
  }
  out.push("");

  // Steps: after METADATA
  if (stepsDir) {
    await Deno.writeTextFile(`${stepsDir}/02-metadata${ext}`, out.join("\n"));
  }

  // --- SETUP block ---
  out.push(`// ${eq}`);
  out.push(`// SETUP BLOCK [SETUP]`);
  out.push(`// ${eq}`);

  if (formConstraints) {
    emitBlockSections(
      out, "SETUP",
      formConstraints.SETUP.can, formConstraints.SETUP.cannot,
      setupChunks, subtype, dash,
      "SETUP makes things EXIST. BODY makes things HAPPEN.",
      "Section order (dependency chain \u2014 each layer uses only what's above):",
    );
  } else {
    emitChunksRaw(out, setupChunks);
  }

  out.push(`// ${eq}`);
  out.push(`// END SETUP [END]`);
  out.push(`// ${eq}`);
  out.push("");

  // Steps: after SETUP
  if (stepsDir) {
    await Deno.writeTextFile(`${stepsDir}/03-setup${ext}`, out.join("\n"));
  }

  // --- BODY block ---
  out.push(`// ${eq}`);
  out.push(`// BODY BLOCK [BODY]`);
  out.push(`// ${eq}`);

  if (formConstraints) {
    emitBlockSections(
      out, "BODY",
      formConstraints.BODY.can, formConstraints.BODY.cannot,
      bodyChunks, subtype, dash,
      "BODY makes things HAPPEN. SETUP made things EXIST.",
      "Subsection order follows the type lifecycle \u2014 from identity through\n// creation, operation, observation, to output.",
    );
  } else {
    emitChunksRaw(out, bodyChunks);
  }

  out.push(`// ${eq}`);
  out.push(`// END BODY [END]`);
  out.push(`// ${eq}`);
  out.push("");

  // Steps: after BODY
  if (stepsDir) {
    await Deno.writeTextFile(`${stepsDir}/04-body${ext}`, out.join("\n"));
  }

  // --- CLOSING block ---
  out.push(`// ${eq}`);
  out.push(`// CLOSING BLOCK [CLOSING]`);
  out.push(`// ${eq}`);

  // Block overview
  emitClosingOverview(out);
  out.push("");

  // Cv — Tests
  emitClosingZone(out, "Cv", closingTestChunks, "No tests yet.", dash);

  // Ce — Entry point
  emitClosingZone(out, "Ce", closingMainChunks,
    `No entry point \u2014 this is a ${adapter.format === "go" ? "package" : "module"}.`, dash);

  // X1 — Modification Policy
  const pc = preservedClosing;
  const x1Label = CLOSING_ZONE_LABELS["X1"] ?? "X1";
  out.push(`// ${dash}`);
  out.push(`// ${x1Label}`);
  out.push(`// ${dash}`);
  out.push(`// policy: Modification guidelines`);
  out.push(`// scripture: ${pc?.policyScripture ?? `"Proverbs 22:28 (WEB) \u2014 Don't move the ancient boundary stone."`}`);
  out.push(`// never: ${pc?.never ?? "Break 4-block structure, Remove block boundaries, Remove identity statics"}`);
  out.push(`// careful: ${pc?.careful ?? "Function signatures (breaks callers), Error types (breaks match arms)"}`);
  out.push(`// safe: ${pc?.safe ?? "Function bodies, New functions, Comments, Tests"}`);
  out.push("");

  // X4 — Reference
  const x4Label = CLOSING_ZONE_LABELS["X4"] ?? "X4";
  out.push(`// ${dash}`);
  out.push(`// ${x4Label}`);
  out.push(`// ${dash}`);
  const filename = filePath.split("/").pop() ?? "unknown";
  const validateCmd = adapter.format === "go" ? "go test ./..." : "cargo test";
  out.push(`// related_files: ${pc?.relatedFiles ?? `[${adapter.format === "go" ? "go.mod" : "Cargo.toml"}]`}`);
  out.push(`// validate: ${pc?.validate ?? validateCmd}`);
  out.push("");

  // X5 — Closing Note
  const x5Label = CLOSING_ZONE_LABELS["X5"] ?? "X5";
  out.push(`// ${dash}`);
  out.push(`// ${x5Label}`);
  out.push(`// ${dash}`);
  out.push(`// note: ${pc?.note ?? `"${filename} \u2014 structural scaffold generated by cws-struct transform."`}`);
  out.push(`// scripture: ${pc?.scripture ?? `"Psalm 139:7-8 \u2014 Whither shall I go from thy spirit?"`}`);
  out.push(`// anchor: ${pc?.anchor ?? `"TODO: Module purpose statement."`}`);
  out.push("");

  // Reserved Omission for CLOSING
  if (formConstraints) {
    const closingSections = formConstraints.CLOSING.can
      .sort((a, b) => a.position - b.position);
    const emittedClosing = new Set(["Cv", "Ce", "X1", "X4", "X5"]);
    const absentClosing = closingSections.filter((s) => !emittedClosing.has(s.tag));
    const reservedClosing = formConstraints.CLOSING.cannot;
    const roClosingLines = buildFormAwareReservedOmission(absentClosing, reservedClosing, subtype);

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

  // Steps: final output (complete file)
  if (stepsDir) {
    await Deno.writeTextFile(`${stepsDir}/05-closing${ext}`, out.join("\n"));
  }

  // ── Report ──
  const setupCount = setupChunks.size;
  const bodyCount = bodyChunks.size;
  const testCount = closingTestChunks.length;

  results.push(info(filePath, "transform/scaffold",
    `Structural scaffold: 4 blocks, ${setupCount} SETUP section(s), ${bodyCount} BODY section(s), ${testCount} test(s)`));

  if (opts?.dryRun) {
    results.push(info(filePath, "transform/scaffold",
      `Would restructure ${lines.length} lines \u2192 ${out.length} lines with full 4-block structure`));
  } else {
    results.push(info(filePath, "transform/scaffold",
      `Restructured ${lines.length} lines \u2192 ${out.length} lines with full 4-block structure`));
  }

  return { lines: out, results };
}

// ---------------------------------------------------------------------------
// 11. Internal helpers — block section emission
// ---------------------------------------------------------------------------

/**
 * Emit sections within a block — form-aware with overview, numbered headers,
 * and Reserved Omission.
 */
function emitBlockSections(
  out: string[],
  _blockName: string,
  can: FormSectionConstraint[],
  cannot: FormReservedSection[],
  chunkMap: Map<string, GenericChunk[]>,
  subtype: string | undefined,
  dash: string,
  overviewLine1: string,
  overviewLine2: string,
): void {
  const sections = [...can].sort((a, b) => a.position - b.position);
  const presentSections = new Set(chunkMap.keys());
  const emittedSections = new Set<string>();

  // Block overview — matches template pattern
  const index = buildSectionIndex(can, cannot);
  out.push(`//`);
  out.push(`// ${overviewLine1}`);
  out.push(`//`);
  for (const line of overviewLine2.split("\n")) {
    out.push(`// ${line}`);
  }
  out.push(`//`);
  emitSectionIndex(out, index);
  out.push("");

  // Emit sections that have content — numbered headers
  for (const section of sections) {
    if (presentSections.has(section.tag)) {
      const label = tagToLabel(section.tag);
      out.push(`// ${dash}`);
      out.push(`// ${section.position}. ${label}`);
      out.push(`// ${dash}`);
      out.push("");
      for (const chunk of chunkMap.get(section.tag)!) {
        // Trim trailing blank lines, then add spacing between chunks
        const trimmed = chunk.lines.slice();
        while (trimmed.length > 0 && trimmed[trimmed.length - 1]!.trim() === "") {
          trimmed.pop();
        }
        out.push(...trimmed);
        out.push(""); // visual spacing between chunks
      }
      emittedSections.add(section.tag);
    }
  }

  // Emit unmapped content
  for (const [section, sChunks] of chunkMap) {
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

  // Reserved Omission
  const absentSections = sections.filter((s) => !emittedSections.has(s.tag));
  const roLines = buildFormAwareReservedOmission(absentSections, cannot, subtype);

  if (roLines.length > 0) {
    out.push(`// ${dash}`);
    out.push(`// Reserved Omission`);
    out.push(`// ${dash}`);
    out.push(...roLines);
    out.push("");
  }
}

/** Emit chunks directly without form-aware formatting (fallback). */
function emitChunksRaw(out: string[], chunkMap: Map<string, GenericChunk[]>): void {
  for (const [, chunks] of chunkMap) {
    for (const chunk of chunks) {
      out.push(...chunk.lines);
    }
  }
  out.push("");
}

/** Emit a CLOSING zone with header + content or placeholder. */
function emitClosingZone(
  out: string[],
  zoneTag: string,
  chunks: GenericChunk[],
  emptyMessage: string,
  dash: string,
): void {
  const label = CLOSING_ZONE_LABELS[zoneTag] ?? zoneTag;
  out.push(`// ${dash}`);
  out.push(`// ${label}`);
  out.push(`// ${dash}`);
  out.push("");
  if (chunks.length > 0) {
    for (const chunk of chunks) {
      out.push(...chunk.lines);
    }
  } else {
    out.push(`// ${emptyMessage}`);
  }
  out.push("");
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Universal scaffold pipeline — the CreativeWorkzStudio coding standard
// expressed as code. Whole → Block → Container → Content. The structure
// is constant across all languages. The content varies by language.
// The schema carries language-specific syntax as data, not code.
//
// "For we are labourers together with God." — 1 Corinthians 3:9
// ============================================================================
