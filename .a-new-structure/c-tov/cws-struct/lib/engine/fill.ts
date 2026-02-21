// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/fill.ts
// key:     B-tov-cws-struct-lib-engine-fill
// title:   CWS Struct — Fill Engine (Schema-Driven File Generation)
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-19
// authors: Nova Dawn (CPI-SI)
// purpose: Generate complete 4-block code files from schema fill_content.
//          The proof that schema = template = linter = generator.
//          If this engine produces a file that `lint` validates with 0 errors,
//          the schema IS the complete specification.
//
// biblical_foundation: "And God saw every thing that he had made, and, behold,
//   it was very good." — Genesis 1:31
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Imports
// ---------------------------------------------------------------------------

import type {
  Code4BlockRules,
  SchemaFillContent,
  CodeFormat,
} from "../foundation/mod.ts";

import { loadCodeRules, clearCodeCache } from "../foundation/mod.ts";

import {
  BLOCK_SEPARATOR_WIDTH,
  SUBSECTION_SEPARATOR_WIDTH,
} from "../handlers/shared/types.ts";

// ---------------------------------------------------------------------------
// 2. Types
// ---------------------------------------------------------------------------

/** Context for file generation — all values needed for placeholder substitution. */
export interface FillContext {
  format: CodeFormat;
  subtype: string;
  key: string;
  title: string;
  purpose: string;
  filename: string;
  /** Go: package name. Rust: crate name. */
  packageOrCrate: string;
  /** Path from project root (for I1.from). */
  from: string;
  /** Additional placeholder overrides. */
  overrides?: Record<string, string>;
}

// ---------------------------------------------------------------------------
// 3. Constants
// ---------------------------------------------------------------------------

const BLOCK_SEP = `// ${"=".repeat(BLOCK_SEPARATOR_WIDTH)}`;
const SUB_SEP = `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`;

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Placeholder substitution
// ---------------------------------------------------------------------------

/**
 * Substitute {{placeholders}} in a template string with context values.
 */
function substitute(
  template: string,
  ctx: FillContext,
  fill: SchemaFillContent,
): string {
  const map: Record<string, string> = {
    key: ctx.key,
    title: ctx.title,
    purpose: ctx.purpose,
    filename: ctx.filename,
    package: ctx.packageOrCrate,
    crate_name: ctx.packageOrCrate,
    format: ctx.format,
    subtype: ctx.subtype,
    from: ctx.from,
    version: fill.defaults.version,
    status: fill.defaults.status,
    organization: fill.defaults.organization,
    scripture: fill.defaults.scripture,
    scripture_text: fill.defaults.scripture_text,
    date: new Date().toISOString().slice(0, 10),
    consumers: ctx.overrides?.["consumers"] ?? "[consumers]",
    ...ctx.overrides,
  };

  return template.replace(/\{\{(\w+)\}\}/g, (_match, name) => {
    return map[name] ?? `[${name}]`;
  });
}

/** Substitute all strings in an array. */
function substituteAll(
  templates: string[],
  ctx: FillContext,
  fill: SchemaFillContent,
): string[] {
  return templates.map((t) => substitute(t, ctx, fill));
}

// ---------------------------------------------------------------------------
// 2. Block generators
// ---------------------------------------------------------------------------

/**
 * Generate the METADATA block.
 *
 * Layout:
 *   - Directives (//omni:key, //omni:code, //omni:version)
 *   - Blank line
 *   - Package/module declaration
 *   - Blank line
 *   - Block separator + label
 *   - Comment fields (Key, Purpose)
 *   - Blank line
 *   - Pragma declaration with I-fields
 *   - Blank line
 *   - Metadata declaration with C-fields
 *   - Blank line
 *   - END separator
 */
function generateMetadata(
  ctx: FillContext,
  fill: SchemaFillContent,
): string[] {
  const lines: string[] = [];

  // Directives
  for (const d of substituteAll(fill.directives, ctx, fill)) {
    lines.push(d);
  }
  lines.push("");

  // Package/module declaration
  if (fill.packageDeclaration) {
    lines.push(substitute(fill.packageDeclaration, ctx, fill));
    lines.push("");
  }
  if (fill.moduleDoc) {
    lines.push(substitute(fill.moduleDoc, ctx, fill));
    lines.push("");
  }

  // Block header
  lines.push(BLOCK_SEP);
  lines.push("// METADATA");
  lines.push(BLOCK_SEP);
  lines.push("//");

  // Comment fields
  for (const comment of substituteAll(fill.metadataComment, ctx, fill)) {
    lines.push(comment);
  }
  lines.push("//");
  lines.push("");

  // Pragma variable
  lines.push(fill.identitySyntax.pragma.declaration);
  for (const [field, value] of fill.pragmaEntries) {
    const subValue = substitute(value, ctx, fill);
    const entry = fill.identitySyntax.pragma.entry
      .replace("{{field}}", field)
      .replace("{{value}}", subValue);
    lines.push(entry);
  }
  lines.push(fill.identitySyntax.pragma.close);
  lines.push("");

  // Metadata variable
  lines.push(fill.identitySyntax.metadata.declaration);
  for (const [field, value] of fill.metadataEntries) {
    const subValue = substitute(value, ctx, fill);
    const entry = fill.identitySyntax.metadata.entry
      .replace("{{field}}", field)
      .replace("{{value}}", subValue);
    lines.push(entry);
  }
  lines.push(fill.identitySyntax.metadata.close);
  lines.push("");

  // END marker
  lines.push(BLOCK_SEP);
  lines.push("// END METADATA");
  lines.push(BLOCK_SEP);

  return lines;
}

/**
 * Generate the SETUP block.
 *
 * Uses setupData from rules to emit subsection headers per subtype.
 */
function generateSetup(
  ctx: FillContext,
  rules: Code4BlockRules,
): string[] {
  const lines: string[] = [];

  lines.push("");
  lines.push(BLOCK_SEP);
  lines.push("// SETUP");
  lines.push(BLOCK_SEP);

  // Emit SETUP subsections from schema
  const subsections = rules.setupData;
  if (subsections.length > 0) {
    for (const sub of subsections) {
      lines.push("");
      lines.push(SUB_SEP);
      lines.push(`// ${sub.tag}. ${sub.pattern}`);
      lines.push(SUB_SEP);
      lines.push("");
      lines.push(`// (${sub.purpose})`);
    }
  }

  lines.push("");
  lines.push(BLOCK_SEP);
  lines.push("// END SETUP");
  lines.push(BLOCK_SEP);

  return lines;
}

/**
 * Generate the BODY block.
 *
 * Uses bodyData from rules to emit subsection headers per subtype.
 */
function generateBody(
  ctx: FillContext,
  rules: Code4BlockRules,
): string[] {
  const lines: string[] = [];

  lines.push("");
  lines.push(BLOCK_SEP);
  lines.push("// BODY");
  lines.push(BLOCK_SEP);

  // Emit BODY subsections from schema per subtype
  const subtypeKey = ctx.subtype.replace("-", "_");
  const bodySubtype = rules.bodyData[subtypeKey];
  if (bodySubtype) {
    for (const sub of bodySubtype.subsections) {
      lines.push("");
      lines.push(SUB_SEP);
      lines.push(`// ${sub.tag}. ${sub.pattern}`);
      lines.push(SUB_SEP);
      lines.push("");
      lines.push(`// (${sub.purpose})`);
    }
  }

  lines.push("");
  lines.push(BLOCK_SEP);
  lines.push("// END BODY");
  lines.push(BLOCK_SEP);

  return lines;
}

/**
 * Generate the CLOSING block.
 *
 * Uses closingData from rules for zone structure, and fill_content for
 * default field values.
 */
function generateClosing(
  ctx: FillContext,
  rules: Code4BlockRules,
  fill: SchemaFillContent,
): string[] {
  const lines: string[] = [];

  lines.push("");
  lines.push(BLOCK_SEP);
  lines.push("// CLOSING");
  lines.push(BLOCK_SEP);

  // Code zones
  for (const zone of rules.closingData.zones) {
    if (zone.kind === "code") {
      lines.push("");
      lines.push(SUB_SEP);
      // Code zones use "tag — name" pattern
      const zoneNames: Record<string, string> = {
        Cv: "Validation",
        Ce: "Execution",
        Cc: "Cleanup",
      };
      lines.push(`// ${zone.tag} — ${zoneNames[zone.tag] ?? zone.tag}`);
      lines.push(SUB_SEP);
      lines.push("");
      lines.push(`// (${zone.tag} content goes here)`);
    }
  }

  // Documentation zones — only required ones (X1, X5) by default
  for (const req of rules.closingData.docRequirements) {
    if (req.templateOnly) continue; // Skip X6 in generated files
    if (!req.required) continue;     // Only emit required zones by default

    lines.push("");
    lines.push(SUB_SEP);
    // Doc zones use "tag: name" pattern
    const docNames: Record<string, string> = {
      X1: "Policy",
      X2: "Extension",
      X3: "Troubleshooting",
      X4: "Reference",
      X5: "Note",
    };
    lines.push(`// ${req.tag}: ${docNames[req.tag] ?? req.tag}`);
    lines.push(SUB_SEP);

    // Emit default field values from fill_content
    const defaults = fill.closingDefaults[req.tag];
    if (defaults) {
      for (const [fieldName, fieldValue] of Object.entries(defaults)) {
        const capitalName = fieldName.charAt(0).toUpperCase() + fieldName.slice(1);
        const subValue = substitute(fieldValue, ctx, fill);
        lines.push(`// ${capitalName}: ${subValue}`);
      }
    }
  }

  lines.push("");
  lines.push(BLOCK_SEP);
  lines.push("// END CLOSING");
  lines.push(BLOCK_SEP);

  return lines;
}

// ---------------------------------------------------------------------------
// 3. Public API
// ---------------------------------------------------------------------------

/**
 * Generate a complete 4-block file from schema.
 *
 * This is the proof: if generateFile() produces output that lint()
 * validates with 0 errors, the schema IS the complete specification.
 *
 * @param ctx - Fill context with all placeholder values
 * @returns Array of lines forming a complete 4-block file
 */
export async function generateFile(ctx: FillContext): Promise<string[]> {
  clearCodeCache(ctx.format);
  const rules = await loadCodeRules(ctx.format);

  if (!rules.fillContent) {
    throw new Error(
      `Schema for ${ctx.format} has no fill_content section — cannot generate file.\n` +
      `Add fill_content to the ${ctx.format} schema to enable generation.`,
    );
  }

  const fill = rules.fillContent;
  const lines: string[] = [];

  lines.push(...generateMetadata(ctx, fill));
  lines.push(...generateSetup(ctx, rules));
  lines.push(...generateBody(ctx, rules));
  lines.push(...generateClosing(ctx, rules, fill));
  lines.push(""); // Trailing newline

  return lines;
}

/**
 * Generate a complete 4-block file and return as a string.
 */
export async function generateFileText(ctx: FillContext): Promise<string> {
  const lines = await generateFile(ctx);
  return lines.join("\n");
}

// ============================================================================
// CLOSING
// ============================================================================
//
// The fill engine completes the square: Schema = Template = Linter = Generator.
//
// Schema defines what blocks contain.
// Linter verifies what blocks contain.
// Fill engine GENERATES what blocks contain.
// Three operations on the same truth.
//
// "And God saw every thing that he had made, and, behold, it was very good."
//   — Genesis 1:31
// ============================================================================
