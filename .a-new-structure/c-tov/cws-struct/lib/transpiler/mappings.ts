// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/transpiler-mappings.ts
// key:     B-tov-cws-struct-lib-engine-transpiler-mappings
// title:   CWS Struct — Transpiler Mapping Loader
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: Schema-driven transpiler mappings. Loads type, import, and convention
//          data from JSONC schemas instead of hardcoding them. Follows the
//          composition-loader pattern: lazy init, caching, pipeline abstraction.
//
//          First principles: the linter loads its rules from schemas. The
//          transpiler loads its mappings from schemas. Same infrastructure,
//          same pattern. Add a new language pair by adding a schema file.
//
// biblical_foundation: "There is one body, and one Spirit." — Ephesians 4:4
//   One mapping infrastructure, many language pairs.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Imports
// ---------------------------------------------------------------------------

import { parse as parseJsonc } from "@std/jsonc";
import { getDefaultPipeline } from "../foundation/mod.ts";
import { ToolError } from "../foundation/mod.ts";
import type { CodeFormat } from "../foundation/mod.ts";
import { registerCache } from "../foundation/cache-registry.ts";

// ---------------------------------------------------------------------------
// 2. Types — Mapping schema shape (matches R5_patterns/{src}-to-{tgt}.jsonc)
// ---------------------------------------------------------------------------

/** Import mapping entry from the schema. */
export interface ImportMappingEntry {
  go_path: string;
  category: "standard_library" | "external_dependency" | "internal_module" | "builtin";
}

/** Generic type pattern entry from the schema. */
export interface GenericTypePattern {
  concept: string;
  rust_pattern: string;
  go_pattern: string;
  notes: string;
}

/** Naming convention transform rule. */
export interface NamingRule {
  rust: string;
  go: string;
  transform: string;
}

/** Receiver convention from schema. */
export interface ReceiverConvention {
  rust_self: string;
  rust_mut_self: string;
  rust_owned_self: string;
  go_pattern: string;
  strip_self: boolean;
  short_name: string;
}

/** Section mapping from schema. */
export interface SchemaSectionMapping {
  source_form: string;
  target_form: string;
  setup: Record<string, string>;
  body: Record<string, string>;
  closing: Record<string, string>;
}

// ---------------------------------------------------------------------------
// 2b. Types — Form structure schema shape (matches forms/{form}/_structure.jsonc)
// ---------------------------------------------------------------------------

/** Block-level section inventory (required, available, reserved). */
export interface BlockSections {
  /** Required sections — must be present. */
  required: string[];
  /** Available sections — may be used. */
  available: string[];
  /** Reserved sections — not for this form. */
  reserved: string[];
}

/** BODY block sections — includes ordering_sequence. */
export interface BodyBlockSections {
  /** Required sections. */
  required: string[];
  /** Reserved sections (not for this form). */
  reserved: string[];
  /** Canonical ordering — the sequence emitters should follow. */
  ordering_sequence: string[];
}

/**
 * Form structure schema — what blocks/sections exist and their ordering.
 *
 * Loaded from: forms/{form}/_structure.jsonc
 * Drives the output SKELETON — what rooms the house has.
 */
export interface FormStructure {
  /** Form name (e.g., "library", "module"). */
  name: string;
  /** SETUP block section inventory. */
  SETUP: BlockSections;
  /** BODY block sections with ordering. */
  BODY: BodyBlockSections;
  /** CLOSING block section inventory. */
  CLOSING: BlockSections;
}

// ---------------------------------------------------------------------------
// 2c. Types — Form content schema shape (matches forms/{form}/{lang}.jsonc)
// ---------------------------------------------------------------------------

/**
 * Form content schema — what content goes in each section, language-specific.
 *
 * Loaded from: forms/{form}/{lang}.jsonc
 * Drives WHAT goes WHERE — the blueprint for each room.
 */
export interface FormContent {
  /** Format (e.g., "go", "rust"). */
  format: string;
  /** Form (e.g., "library", "module"). */
  form: string;
  /** Content expectations — the detailed blueprint. */
  contentExpectations: FormContentExpectations;
}

/** Content expectations for a form. */
export interface FormContentExpectations {
  /** Pragma directive (e.g., "//omni:code --go -library"). */
  pragma: string;
  /** METADATA block expectations. */
  METADATA: Record<string, unknown>;
  /** SETUP block expectations. */
  SETUP: Record<string, unknown>;
  /** BODY block expectations. */
  BODY: Record<string, unknown>;
  /** CLOSING block expectations. */
  CLOSING: Record<string, unknown>;
}

/**
 * Resolved transpiler mappings — all data loaded and typed.
 *
 * This is what the transpiler functions consume. The hardcoded Maps
 * in transpiler.ts are replaced by data loaded from schemas.
 */
export interface ResolvedMappings {
  /** Source format (e.g., "rust"). */
  sourceFormat: CodeFormat;
  /** Target format (e.g., "go"). */
  targetFormat: CodeFormat;
  /** Primitive type map: Rust type → Go type. */
  typesPrimitive: Map<string, string>;
  /** Generic type patterns (ordered — first match wins). */
  typesGeneric: GenericTypePattern[];
  /** Import path map: Rust path → Go import info. */
  imports: Map<string, ImportMappingEntry>;
  /** Naming conventions. */
  naming: Record<string, NamingRule>;
  /** Receiver conventions. */
  receiver: ReceiverConvention;
  /** Section mapping (form-level). */
  sectionMapping: SchemaSectionMapping;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Schema loading
// ---------------------------------------------------------------------------

/**
 * Build the schema ID for a transpiler mapping file.
 *
 * @param sourceFormat - Source language (e.g., "rust")
 * @param targetFormat - Target language (e.g., "go")
 * @returns Schema ID for the pipeline
 */
function mappingSchemaId(sourceFormat: string, targetFormat: string): string {
  return `code/R5_patterns/${sourceFormat}-to-${targetFormat}.jsonc`;
}

/**
 * Load and parse a transpiler mapping schema from JSONC.
 *
 * Follows the composition-loader pattern: use the default pipeline,
 * resolve the schema ID, parse JSONC, extract the typed data.
 *
 * @param sourceFormat - Source language (e.g., "rust")
 * @param targetFormat - Target language (e.g., "go")
 * @returns Fully resolved mappings
 */
export async function loadTranspilerMappings(
  sourceFormat: CodeFormat,
  targetFormat: CodeFormat,
): Promise<ResolvedMappings> {
  const pipeline = getDefaultPipeline();
  const schemaId = mappingSchemaId(sourceFormat, targetFormat);
  const text = await pipeline.getText(schemaId);
  const raw = parseJsonc(text) as Record<string, unknown>;
  const mapping = raw.mapping as Record<string, unknown>;

  if (!mapping) {
    throw new ToolError("CWS-T00-030", { schemaId });
  }

  // Extract typed data from the raw schema

  // 1. Primitive types
  const typesPrimitive = new Map<string, string>();
  const rawPrimitives = mapping.types_primitive as Record<string, string>;
  if (rawPrimitives) {
    for (const [rustType, goType] of Object.entries(rawPrimitives)) {
      typesPrimitive.set(rustType, goType);
    }
  }

  // 2. Generic type patterns
  const typesGeneric = (mapping.types_generic ?? []) as GenericTypePattern[];

  // 3. Import mappings
  const imports = new Map<string, ImportMappingEntry>();
  const rawImports = mapping.imports as Record<string, ImportMappingEntry>;
  if (rawImports) {
    for (const [rustPath, entry] of Object.entries(rawImports)) {
      imports.set(rustPath, entry);
    }
  }

  // 4. Conventions
  const conventions = mapping.conventions as Record<string, unknown>;
  const naming = (conventions?.naming ?? {}) as Record<string, NamingRule>;
  const receiver = (conventions?.receiver ?? {}) as ReceiverConvention;

  // 5. Section mapping
  const sectionMapping = (mapping.section_mapping ?? {}) as SchemaSectionMapping;

  return {
    sourceFormat,
    targetFormat,
    typesPrimitive,
    typesGeneric,
    imports,
    naming,
    receiver,
    sectionMapping,
  };
}

// ---------------------------------------------------------------------------
// 2. Cache management
// ---------------------------------------------------------------------------

const mappingCache = new Map<string, ResolvedMappings>();

/**
 * Load transpiler mappings with caching.
 *
 * Same as loadTranspilerMappings but caches by format pair key.
 * Use clearMappingCache() to reset.
 *
 * @param sourceFormat - Source language
 * @param targetFormat - Target language
 * @returns Cached or freshly loaded mappings
 */
export async function loadTranspilerMappingsCached(
  sourceFormat: CodeFormat,
  targetFormat: CodeFormat,
): Promise<ResolvedMappings> {
  const key = `${sourceFormat}:${targetFormat}`;
  const cached = mappingCache.get(key);
  if (cached) return cached;

  const mappings = await loadTranspilerMappings(sourceFormat, targetFormat);
  mappingCache.set(key, mappings);
  return mappings;
}

/** Clear the mapping cache. */
export function clearMappingCache(): void {
  mappingCache.clear();
}

registerCache("engine/transpiler-mappings", clearMappingCache);
// ---------------------------------------------------------------------------
// 3. Schema-driven conversion functions
// ---------------------------------------------------------------------------

/**
 * Convert a Rust type expression to Go using loaded schema mappings.
 *
 * Resolution order:
 * 1. Check primitive type map (direct 1:1)
 * 2. Check generic type patterns (pattern matching with recursive inner type resolution)
 * 3. Strip lifetime annotations (e.g., &'static → &)
 * 4. Return as-is if no mapping found (unknown custom type)
 *
 * @param mappings - Loaded schema mappings
 * @param rustType - Rust type expression (e.g., "Vec<String>", "&str", "Option<i32>")
 * @returns Go type expression (e.g., "[]string", "string", "*int32")
 */
export function convertType(
  mappings: ResolvedMappings,
  rustType: string | undefined,
): string {
  if (!rustType || rustType.trim() === "") return "";

  const trimmed = rustType.trim();

  // 1. Strip lifetime annotations: &'static str → &str, &'a T → &T
  const stripped = trimmed.replace(/'[a-z_]+\s*/g, "").trim();

  // 2. Check primitive map (direct 1:1) — try both original and stripped
  const primitive = mappings.typesPrimitive.get(stripped);
  if (primitive !== undefined) return primitive;
  if (stripped !== trimmed) {
    const origPrim = mappings.typesPrimitive.get(trimmed);
    if (origPrim !== undefined) return origPrim;
  }

  // 3. Check generic patterns
  for (const pattern of mappings.typesGeneric) {
    const result = matchGenericPattern(mappings, stripped, pattern);
    if (result !== null) return result;
  }

  // 4. Strip module prefix and retry (io::Result<T> → Result<T>)
  const colonIdx = stripped.lastIndexOf("::");
  if (colonIdx >= 0) {
    const withoutModule = stripped.slice(colonIdx + 2);
    // Check primitives again with stripped prefix
    const modPrim = mappings.typesPrimitive.get(withoutModule);
    if (modPrim !== undefined) return modPrim;
    // Check generics again
    for (const pattern of mappings.typesGeneric) {
      const result = matchGenericPattern(mappings, withoutModule, pattern);
      if (result !== null) return result;
    }
  }

  // 5. Unknown type — strip module path, return last segment
  if (stripped.includes("::")) {
    return stripped.split("::").pop()!;
  }
  return stripped;
}

/**
 * Try to match a Rust type against a generic pattern.
 *
 * Pattern syntax: "Option<{T}>" matches "Option<String>" with T="String".
 * Recursively resolves inner types.
 *
 * @returns Go type if matched, null if no match
 */
function matchGenericPattern(
  mappings: ResolvedMappings,
  rustType: string,
  pattern: GenericTypePattern,
): string | null {
  // Extract the prefix and determine if this pattern matches
  // Pattern: "Vec<{T}>" → prefix="Vec<", suffix=">"
  // Pattern: "HashMap<{K}, {V}>" → prefix="HashMap<", needs multi-param

  const rustPattern = pattern.rust_pattern;
  const goPattern = pattern.go_pattern;

  // Simple single-parameter patterns: Prefix<{T}>
  // Split is guaranteed to produce 2+ parts because we checked includes("{T}").
  if (rustPattern.includes("{T}") && !rustPattern.includes("{K}")) {
    const tParts = rustPattern.split("{T}");
    const prefix = tParts[0]!;
    const suffix = tParts[1]!;

    if (rustType.startsWith(prefix) && rustType.endsWith(suffix)) {
      const innerType = rustType.slice(prefix.length, rustType.length - suffix.length);
      const convertedInner = convertType(mappings, innerType);
      return goPattern.replace("{T}", convertedInner);
    }
  }

  // Two-parameter patterns: Prefix<{K}, {V}>
  if (rustPattern.includes("{K}") && rustPattern.includes("{V}")) {
    const kParts = rustPattern.split("{K}");
    const prefix = kParts[0]!;
    const kTail = kParts[1]!;
    const vParts = kTail.split("{V}");
    const middle = vParts[0]!;
    const suffix = rustPattern.split("{V}")[1]!;

    if (rustType.startsWith(prefix) && rustType.endsWith(suffix)) {
      const inner = rustType.slice(prefix.length, rustType.length - suffix.length);
      const middleTrimmed = middle.trim();
      const sepIdx = inner.indexOf(middleTrimmed);
      if (sepIdx >= 0) {
        const keyType = inner.slice(0, sepIdx).trim();
        const valType = inner.slice(sepIdx + middleTrimmed.length).trim();
        const convertedKey = convertType(mappings, keyType);
        const convertedVal = convertType(mappings, valType);
        return goPattern.replace("{K}", convertedKey).replace("{V}", convertedVal);
      }
    }
  }

  // Two-parameter patterns: Result<{T}, {E}>
  if (rustPattern.includes("{T}") && rustPattern.includes("{E}")) {
    const teParts = rustPattern.split("{T}");
    const prefix = teParts[0]!;
    const teTail = teParts[1]!;
    const eParts = teTail.split("{E}");
    const middle = eParts[0]!;
    const suffix = rustPattern.split("{E}")[1]!;

    if (rustType.startsWith(prefix) && rustType.endsWith(suffix)) {
      const inner = rustType.slice(prefix.length, rustType.length - suffix.length);
      const middleTrimmed = middle.trim();
      const sepIdx = inner.indexOf(middleTrimmed);
      if (sepIdx >= 0) {
        const tType = inner.slice(0, sepIdx).trim();
        const convertedT = convertType(mappings, tType);
        return goPattern.replace("{T}", convertedT).replace("{E}", "error");
      }
    }
  }

  return null;
}

/**
 * Convert a Rust import path to Go using loaded schema mappings.
 *
 * @param mappings - Loaded schema mappings
 * @param rustPath - Rust import path (e.g., "std::fs", "std::collections::HashMap")
 * @returns { goPath, mapped, category }
 */
export function convertImport(
  mappings: ResolvedMappings,
  rustPath: string,
): { goPath: string; mapped: boolean; category: string } {
  // Try exact match first
  const exact = mappings.imports.get(rustPath);
  if (exact) {
    return { goPath: exact.go_path, mapped: true, category: exact.category };
  }

  // Try prefix matching (e.g., "std::sync::mpsc" → try "std::sync" → "sync")
  const parts = rustPath.split("::");
  for (let i = parts.length - 1; i >= 1; i--) {
    const prefix = parts.slice(0, i).join("::");
    const prefixMatch = mappings.imports.get(prefix);
    if (prefixMatch) {
      return { goPath: prefixMatch.go_path, mapped: true, category: prefixMatch.category };
    }
  }

  // No mapping found
  return { goPath: rustPath, mapped: false, category: "unknown" };
}

/**
 * Convert a Rust name to Go naming convention using loaded mappings.
 *
 * @param mappings - Loaded schema mappings
 * @param name - Rust identifier
 * @param visibility - "public" or "private"
 * @param kind - "function", "type", "constant", "parameter"
 * @returns Go-convention name
 */
export function convertName(
  mappings: ResolvedMappings,
  name: string,
  visibility: "public" | "private",
  kind: "function" | "type" | "constant" | "parameter" = "function",
): string {
  // Determine the convention key
  let conventionKey: string;
  if (kind === "parameter") {
    conventionKey = "parameter";
  } else if (kind === "constant") {
    conventionKey = visibility === "public" ? "constant_public" : "constant_private";
  } else if (kind === "type") {
    conventionKey = visibility === "public" ? "public_type" : "private_type";
  } else {
    conventionKey = visibility === "public" ? "public_function" : "private_function";
  }

  const rule = mappings.naming[conventionKey];
  if (!rule) return name; // No rule → preserve

  switch (rule.transform) {
    case "snake_to_pascal":
      return snakeToPascal(name);
    case "snake_to_camel":
      return snakeToCamel(name);
    case "preserve":
      return name;
    case "pascal_to_camel":
      return name.charAt(0).toLowerCase() + name.slice(1);
    case "upper_snake_to_pascal":
      return snakeToPascal(name.toLowerCase());
    case "upper_snake_to_camel":
      return snakeToCamel(name.toLowerCase());
    case "to_lowercase_stripped":
      return name.replace(/_/g, "").toLowerCase();
    default:
      return name;
  }
}

// ---------------------------------------------------------------------------
// 4. Helpers — naming transforms
// ---------------------------------------------------------------------------

/** Convert snake_case to PascalCase. */
function snakeToPascal(name: string): string {
  return name
    .split("_")
    .map((part) => part.charAt(0).toUpperCase() + part.slice(1))
    .join("");
}

/** Convert snake_case to camelCase. */
function snakeToCamel(name: string): string {
  const pascal = snakeToPascal(name);
  return pascal.charAt(0).toLowerCase() + pascal.slice(1);
}

/**
 * Convert Rust parameters to Go using loaded schema mappings.
 *
 * @param mappings - Loaded schema mappings
 * @param params - Rust parameter string (e.g., "&self, key: &str, value: String")
 * @returns Go parameter string (e.g., "key string, value string")
 */
export function convertParams(
  mappings: ResolvedMappings,
  params: string | undefined,
): string {
  if (!params || params.trim() === "") return "";

  // Split on commas, filter out self/&self/&mut self
  const parts = params.split(",").map((p) => p.trim());
  const filtered = parts.filter(
    (p) => p !== "&self" && p !== "&mut self" && p !== "self" && p !== "mut self"
  );

  if (filtered.length === 0) return "";

  // Convert each param: "name: Type" → "name GoType"
  return filtered
    .map((param) => {
      const colonIdx = param.indexOf(":");
      if (colonIdx < 0) return param;

      const name = param.slice(0, colonIdx).trim();
      const type = param.slice(colonIdx + 1).trim();
      const goName = convertName(mappings, name, "private", "parameter");
      const goType = convertType(mappings, type);
      return `${goName} ${goType}`;
    })
    .join(", ");
}

// ---------------------------------------------------------------------------
// 5. Form structure and content loading
// ---------------------------------------------------------------------------

/**
 * Build the schema ID for a form structure schema.
 *
 * @param form - Form name (e.g., "library", "module")
 * @returns Schema ID for the pipeline
 */
function structureSchemaId(form: string): string {
  return `code/forms/${form}/_structure.jsonc`;
}

/**
 * Build the schema ID for a form content schema.
 *
 * @param language - Language (e.g., "go", "rust")
 * @param form - Form name (e.g., "library", "module")
 * @returns Schema ID for the pipeline
 */
function contentSchemaId(language: string, form: string): string {
  return `code/forms/${form}/${language}.jsonc`;
}

/**
 * Load a form's structure schema (section ordering, required/available).
 *
 * Drives the output skeleton — what blocks/sections exist and their order.
 *
 * @param form - Form name (e.g., "library", "module")
 * @returns Parsed FormStructure
 */
export async function loadFormStructure(form: string): Promise<FormStructure> {
  const pipeline = getDefaultPipeline();
  const schemaId = structureSchemaId(form);
  const text = await pipeline.getText(schemaId);
  const raw = parseJsonc(text) as Record<string, unknown>;
  const formData = raw.form as Record<string, unknown>;

  if (!formData) {
    throw new ToolError("CWS-T00-031", { schemaId, form });
  }

  const setup = formData.SETUP as Record<string, unknown> ?? {};
  const body = formData.BODY as Record<string, unknown> ?? {};
  const closing = formData.CLOSING as Record<string, unknown> ?? {};

  return {
    name: (formData.name as string) ?? form,
    SETUP: {
      required: (setup.required as string[]) ?? [],
      available: (setup.available as string[]) ?? [],
      reserved: (setup.reserved as string[]) ?? [],
    },
    BODY: {
      required: (body.required as string[]) ?? [],
      reserved: (body.reserved as string[]) ?? [],
      ordering_sequence: (body.ordering_sequence as string[]) ?? [],
    },
    CLOSING: {
      required: (closing.required as string[]) ?? [],
      available: (closing.available as string[]) ?? [],
      reserved: (closing.reserved as string[]) ?? [],
    },
  };
}

/**
 * Load a form's content expectations for a specific language.
 *
 * Drives what content goes in each section — the blueprint for each room.
 *
 * @param language - Language identifier (e.g., "go", "rust")
 * @param form - Form name (e.g., "library", "module")
 * @returns Parsed FormContent
 */
export async function loadFormContent(
  language: string,
  form: string,
): Promise<FormContent> {
  const pipeline = getDefaultPipeline();
  const schemaId = contentSchemaId(language, form);
  const text = await pipeline.getText(schemaId);
  const raw = parseJsonc(text) as Record<string, unknown>;
  const expectations = raw.content_expectations as Record<string, unknown>;

  if (!expectations) {
    throw new ToolError("CWS-T00-032", { schemaId, form, language });
  }

  return {
    format: (raw.format as string) ?? language,
    form: (raw.form as string) ?? form,
    contentExpectations: {
      pragma: (expectations.pragma as string) ?? "",
      METADATA: (expectations.METADATA as Record<string, unknown>) ?? {},
      SETUP: (expectations.SETUP as Record<string, unknown>) ?? {},
      BODY: (expectations.BODY as Record<string, unknown>) ?? {},
      CLOSING: (expectations.CLOSING as Record<string, unknown>) ?? {},
    },
  };
}

// ---------------------------------------------------------------------------
// 5b. Form schema cache
// ---------------------------------------------------------------------------

const structureCache = new Map<string, FormStructure>();
const contentCache = new Map<string, FormContent>();

/**
 * Load form structure with caching.
 *
 * @param form - Form name
 * @returns Cached or freshly loaded form structure
 */
export async function loadFormStructureCached(
  form: string,
): Promise<FormStructure> {
  const cached = structureCache.get(form);
  if (cached) return cached;

  const structure = await loadFormStructure(form);
  structureCache.set(form, structure);
  return structure;
}

/**
 * Load form content with caching.
 *
 * @param language - Language identifier
 * @param form - Form name
 * @returns Cached or freshly loaded form content
 */
export async function loadFormContentCached(
  language: string,
  form: string,
): Promise<FormContent> {
  const key = `${language}:${form}`;
  const cached = contentCache.get(key);
  if (cached) return cached;

  const content = await loadFormContent(language, form);
  contentCache.set(key, content);
  return content;
}

/** Clear the form schema caches. */
export function clearFormSchemaCache(): void {
  structureCache.clear();
  contentCache.clear();
}
registerCache("engine/transpiler-form-schema", clearFormSchemaCache);

// ---------------------------------------------------------------------------
// 6. Universal concept loading — schema-driven AST concepts
// ---------------------------------------------------------------------------

/** A pattern within a language concept expression file. */
export interface ConceptPattern {
  /** Unique pattern identifier within this concept+language. */
  id: string;
  /** Human-readable syntax template. */
  syntax?: string;
  /** Regex for detecting this pattern in source code (source languages). */
  detect?: string;
  /** Named capture group mapping: name → group index. */
  captures?: Record<string, number>;
  /** Template for emitting this pattern in target code (target languages). */
  emit?: string;
  /** When this pattern should be chosen over alternatives. */
  when?: string;
  /** Additional notes about this pattern. */
  notes?: string;
  /** Pattern match priority (lower = higher priority). */
  priority?: number;
  /** For variable_binding: whether the binding is mutable. */
  mutable?: boolean;
  /** For return_value: whether the return is explicit. */
  explicit?: boolean;
  /** For ok_wrap: whether this is a void/unit success. */
  void?: boolean;
}

/** A language-specific expression of a universal concept. */
export interface LanguageConcept {
  /** The universal concept this expresses (e.g., "variable_binding"). */
  concept: string;
  /** The language (e.g., "rust", "go"). */
  language: string;
  /** Patterns for detecting (source) or emitting (target) this concept. */
  patterns: ConceptPattern[];
}

/** A concept mapping pair: source language + target language for one concept. */
export interface ConceptMappingPair {
  /** Source language patterns (with detect regexes). */
  source: LanguageConcept;
  /** Target language patterns (with emit templates). */
  target: LanguageConcept;
}

/**
 * Load a single language concept from its schema file.
 *
 * Schema path: code/R5_patterns/{concept}/{language}.jsonc
 *
 * @param language - Language identifier (e.g., "rust", "go")
 * @param concept - Concept name in kebab-case (e.g., "variable-binding")
 * @returns Parsed LanguageConcept
 */
export async function loadLanguageConcept(
  language: string,
  concept: string,
): Promise<LanguageConcept> {
  const pipeline = getDefaultPipeline();
  const schemaId = `code/R5_patterns/${concept}/${language}.jsonc`;
  const text = await pipeline.getText(schemaId);
  const raw = parseJsonc(text) as Record<string, unknown>;

  return {
    concept: raw.concept as string,
    language: raw.language as string,
    patterns: (raw.patterns ?? []) as ConceptPattern[],
  };
}

/**
 * Discover all concept names available in the concepts directory.
 *
 * Lists the concept schema files and extracts names.
 *
 * @returns Array of concept names in kebab-case
 */
export async function discoverConcepts(): Promise<string[]> {
  const pipeline = getDefaultPipeline();
  // Try to list concept files by reading the concepts directory
  // The pipeline resolves against the schemas root, so we try known concepts
  const knownConcepts = [
    "variable-binding",
    "return-value",
    "conditional-if",
    "conditional-match",
    "error-propagation",
    "function-call",
    "method-call",
    "ok-wrap",
    "err-wrap",
    "string-format",
    "self-access",
  ];

  const available: string[] = [];
  for (const concept of knownConcepts) {
    try {
      await pipeline.getText(`code/R5_patterns/${concept}/_structure.jsonc`);
      available.push(concept);
    } catch {
      // Concept file doesn't exist — skip
    }
  }
  return available;
}

/**
 * Load all concept mappings for a source→target language pair.
 *
 * For each discovered concept, loads both the source and target
 * language expression files. If either side is missing, that concept
 * is skipped (not all languages support all concepts).
 *
 * @param sourceLanguage - Source language (e.g., "rust")
 * @param targetLanguage - Target language (e.g., "go")
 * @returns Map of concept name → { source, target } pairs
 */
export async function loadConceptMappings(
  sourceLanguage: string,
  targetLanguage: string,
): Promise<Map<string, ConceptMappingPair>> {
  const concepts = await discoverConcepts();
  const mappings = new Map<string, ConceptMappingPair>();

  for (const concept of concepts) {
    try {
      const source = await loadLanguageConcept(sourceLanguage, concept);
      const target = await loadLanguageConcept(targetLanguage, concept);
      mappings.set(concept, { source, target });
    } catch {
      // One side missing — skip this concept for this language pair
    }
  }

  return mappings;
}

// ---------------------------------------------------------------------------
// 6. Concept mapping cache
// ---------------------------------------------------------------------------

const conceptCache = new Map<string, Map<string, ConceptMappingPair>>();

/**
 * Load concept mappings with caching.
 *
 * @param sourceLanguage - Source language
 * @param targetLanguage - Target language
 * @returns Cached or freshly loaded concept mappings
 */
export async function loadConceptMappingsCached(
  sourceLanguage: string,
  targetLanguage: string,
): Promise<Map<string, ConceptMappingPair>> {
  const key = `${sourceLanguage}:${targetLanguage}`;
  const cached = conceptCache.get(key);
  if (cached) return cached;

  const mappings = await loadConceptMappings(sourceLanguage, targetLanguage);
  conceptCache.set(key, mappings);
  return mappings;
}

/** Clear the concept mapping cache. */
export function clearConceptCache(): void {
  conceptCache.clear();
}

registerCache("engine/transpiler-concepts", clearConceptCache);
// ============================================================================
// CLOSING
// ============================================================================
//
// The transpiler mapping loader replaces hardcoded conversion functions with
// schema-driven data. Same pattern as the linter: load schemas, apply rules.
// Add a new language pair by adding a schema file.
//
// The pentagon extends: schema = template = linter = transpiler mapping = AST concept.
// One source of truth, five operations.
//
// "There is one body, and one Spirit." — Ephesians 4:4
// ============================================================================
