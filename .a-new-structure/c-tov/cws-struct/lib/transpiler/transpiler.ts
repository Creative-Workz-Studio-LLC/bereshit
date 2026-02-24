// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/transpiler.ts
// key:     B-tov-cws-struct-lib-engine-transpiler
// title:   CWS Struct — Three-Cord Transpiler
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: Transpile 4-block code files between formats using the three-cord
//          composition architecture. Source → Semantic IR → Target.
//
//          Phase F.2 proof: Rust module → Go library. Structural skeleton +
//          identity + function stubs. Generated Go passes cws-struct lint go.
//          Method bodies get TODO comments — this proves the architecture,
//          not Rust parsing edge cases.
//
// biblical_foundation: "To every thing there is a season, and a time to
//   every purpose under the heaven." — Ecclesiastes 3:1
//   Same purpose, different season — same structure, different syntax.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Imports
// ---------------------------------------------------------------------------

import type {
  ExtractedIdentity,
  ExtractedSemantics,
  SemanticChunk,
  ChunkItem,
  ItemKind,
  ItemMeta,
  TranspileOptions,
  TranspileResult,
  TranspileDiagnostic,
  TranspileStats,
  FormatFormMapping,
} from "./types.ts";

import type { CodeFormat } from "../foundation/mod.ts";
import { loadComposition } from "../foundation/mod.ts";

import type { ResolvedMappings, FormStructure } from "./mappings.ts";
import {
  loadTranspilerMappingsCached,
  loadFormStructureCached,
  convertType,
  convertImport,
  convertName,
  convertParams,
} from "./mappings.ts";

import { translateBody } from "./ast.ts";

import {
  findBlocks,
  getBlockLines,
  getSubsectionRanges,
} from "../shared/mod.ts";

import type { SubsectionDef } from "../shared/mod.ts";

import {
  BLOCK_SEPARATOR_WIDTH,
  SUBSECTION_SEPARATOR_WIDTH,
} from "../shared/mod.ts";

// ---------------------------------------------------------------------------
// 2. Constants
// ---------------------------------------------------------------------------

const BLOCK_SEP = `// ${"=".repeat(BLOCK_SEPARATOR_WIDTH)}`;
const SUB_SEP = `// ${"─".repeat(SUBSECTION_SEPARATOR_WIDTH)}`;

// ---------------------------------------------------------------------------
// 3. Schema-driven mapping state (lazy-loaded)
// ---------------------------------------------------------------------------

/**
 * Module-level schema-driven mappings. Loaded lazily by transpile()/transpileFile().
 * When loaded, conversion functions delegate here instead of using hardcoded maps.
 * Follows the ensureRustRules() pattern from the handler.
 */
let _mappings: ResolvedMappings | null = null;

/**
 * Ensure schema-driven transpiler mappings are loaded.
 * Call this in transpile()/transpileFile() before generating output.
 * Caches globally — load once, use for all subsequent calls.
 *
 * @param sourceFormat - Source language (e.g., "rust")
 * @param targetFormat - Target language (e.g., "go")
 */
async function ensureTranspilerMappings(
  sourceFormat: CodeFormat,
  targetFormat: CodeFormat,
): Promise<void> {
  if (_mappings &&
      _mappings.sourceFormat === sourceFormat &&
      _mappings.targetFormat === targetFormat) {
    return; // Already loaded for this pair
  }
  try {
    _mappings = await loadTranspilerMappingsCached(sourceFormat, targetFormat);
  } catch {
    // Schema not found — fall back to hardcoded. Not an error for now.
    _mappings = null;
  }
}

// ---------------------------------------------------------------------------
// 4. Format-form mappings (registered)
// ---------------------------------------------------------------------------

/**
 * Rust module → Go library section mapping.
 *
 * Why these map to each other:
 * - Rust module = focused unit of a crate. Go library = focused package file.
 * - Both are the "standard building block" of their language.
 * - SETUP is nearly 1:1 (imports, constants, types).
 * - BODY differs: Rust separates by type lifecycle (traits, constructors, logic),
 *   Go separates by call flow (org chart, operations, public APIs).
 * - CLOSING is identical (same zone system).
 */
const RUST_MODULE_TO_GO_LIBRARY: FormatFormMapping = {
  sourceFormat: "rust",
  sourceForm: "module",
  targetFormat: "go",
  targetForm: "library",
  sections: {
    setup: new Map([
      ["Imports", "Imports"],
      ["Constants", "Constants"],
      ["Statics", "Variables"],          // Rust static → Go var
      ["TypeAliases", "TypeAliases"],
      ["ErrorTypes", "ErrorTypes"],
      ["CoreTypes", "CoreTypes"],
      ["TraitDefs", "InterfaceDefs"],    // Rust trait → Go interface
    ]),
    body: new Map([
      ["IdentityAccess", "OrgChart"],           // Both: "what this file IS"
      ["TraitImplementations", "CoreOperations"], // Rust impl → Go methods
      ["Constructors", "CoreOperations"],         // Rust new() → Go New()
      ["CoreLogic", "CoreOperations"],            // Direct mapping
      ["Queries", "CoreOperations"],              // Go doesn't separate queries
      ["FreeFunctions", "PublicAPIs"],            // Rust free fns → Go exported fns
    ]),
    closing: new Map([
      ["Cv", "Cv"],
      ["Ce", "Ce"],
      ["Cc", "Cc"],
      ["X1", "X1"],
      ["X2", "X2"],
      ["X3", "X3"],
      ["X4", "X4"],
      ["X5", "X5"],
    ]),
  },
  notes: "Rust module → Go library: closest structural equivalents. Both are the standard building block of their language.",
};

/** Registry of known format-form mappings. */
const FORMAT_MAPPINGS: FormatFormMapping[] = [
  RUST_MODULE_TO_GO_LIBRARY,
];

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Mapping lookup
// ---------------------------------------------------------------------------

/**
 * Find a registered format-form mapping.
 */
export function findMapping(
  sourceFormat: CodeFormat,
  sourceForm: string,
  targetFormat: CodeFormat,
  targetForm?: string,
): FormatFormMapping | null {
  return FORMAT_MAPPINGS.find((m) =>
    m.sourceFormat === sourceFormat &&
    m.sourceForm === sourceForm &&
    m.targetFormat === targetFormat &&
    (targetForm ? m.targetForm === targetForm : true)
  ) ?? null;
}

/**
 * Get the natural target form for a source format+form → target format.
 */
export function getTargetForm(
  sourceFormat: CodeFormat,
  sourceForm: string,
  targetFormat: CodeFormat,
): string | null {
  const mapping = FORMAT_MAPPINGS.find((m) =>
    m.sourceFormat === sourceFormat &&
    m.sourceForm === sourceForm &&
    m.targetFormat === targetFormat
  );
  return mapping?.targetForm ?? null;
}

// ---------------------------------------------------------------------------
// 2. Source extraction — Rust
// ---------------------------------------------------------------------------

/**
 * Build Rust SETUP subsection patterns for extraction.
 * Matches the patterns used by the Rust handler's lint path.
 */
function buildRustSetupSubsections(): SubsectionDef[] {
  return [
    { tag: "Imports",       pattern: /^\/\/\s*(?:\d+\.\s+)?Imports\b/i },
    { tag: "Constants",     pattern: /^\/\/\s*(?:\d+\.\s+)?Constants\b/i },
    { tag: "Statics",       pattern: /^\/\/\s*(?:\d+\.\s+)?Statics\b/i },
    { tag: "TypeAliases",   pattern: /^\/\/\s*(?:\d+\.\s+)?Type\s*Aliases\b/i },
    { tag: "ErrorTypes",    pattern: /^\/\/\s*(?:\d+\.\s+)?Error\s*Types\b/i },
    { tag: "CoreTypes",     pattern: /^\/\/\s*(?:\d+\.\s+)?Core\s*Types\b/i },
    { tag: "TraitDefs",     pattern: /^\/\/\s*(?:\d+\.\s+)?Trait\s*Def/i },
    { tag: "Modules",       pattern: /^\/\/\s*(?:\d+\.\s+)?Modules\b/i },
    { tag: "Macros",        pattern: /^\/\/\s*(?:\d+\.\s+)?Macros\b/i },
    { tag: "FeatureGates",  pattern: /^\/\/\s*(?:\d+\.\s+)?Feature\s*Gates\b/i },
  ];
}

/**
 * Extract identity from METADATA block lines.
 */
function extractIdentity(
  metadataLines: string[],
  allLines: string[],
  format: CodeFormat,
): ExtractedIdentity {
  const raw = new Map<string, string>();
  const identity: ExtractedIdentity = {
    key: "",
    format,
    from: "",
    version: "a-01.00",
    title: "",
    purpose: "",
    subtype: "",
    pragma: "",
    raw,
  };

  // Extract pragma from first few lines
  for (const line of allLines.slice(0, 5)) {
    if (line.startsWith("//omni:code")) {
      identity.pragma = line;
      // Parse format and form from pragma: //omni:code --rust -module
      const parts = line.split(/\s+/);
      for (const p of parts) {
        if (p.startsWith("-") && !p.startsWith("--")) {
          identity.subtype = p.slice(1);
        }
      }
      break;
    }
  }

  // Parse identity fields from Rust static declarations
  // Rust uses: static PRAGMA: &[(&str, &str)] = &[("I1.key", "value"), ...];
  const pragmaRe = /^\s*static\s+PRAGMA\s*:/;
  const metadataRe = /^\s*static\s+METADATA\s*:/;
  const fieldRe = /\(\s*"([^"]+)"\s*,\s*"([^"]*)"\s*\)/g;

  // Track which variable we're inside (for future section-aware extraction)
  let _inPragma = false;
  let _inMetadata = false;

  for (const line of metadataLines) {
    if (pragmaRe.test(line)) { _inPragma = true; _inMetadata = false; }
    if (metadataRe.test(line)) { _inMetadata = true; _inPragma = false; }
    if (line.includes("];")) { _inPragma = false; _inMetadata = false; }

    for (const match of line.matchAll(fieldRe)) {
      const key = match[1]!;
      const value = match[2]!;
      raw.set(key, value);

      if (key === "I1.key") identity.key = value;
      if (key === "I1.format") identity.format = value as CodeFormat;
      if (key === "I1.from") identity.from = value;
      if (key === "I1.at") identity.version = value;
      if (key === "I3.title") identity.title = value;
      if (key === "I3.purpose" || key === "I3.brief") identity.purpose = value;
      if (key === "I2.subtype" && !identity.subtype) identity.subtype = value;
    }
  }

  // Fallback: detect format from PRAGMA if not in I1.format
  if (!identity.format) identity.format = format;

  return identity;
}

/**
 * Classify a Rust source line into an item kind.
 */
function classifyRustItem(line: string): { kind: ItemKind; name: string; visibility: "public" | "private"; meta: ItemMeta } {
  const trimmed = line.trim();

  // use declarations
  if (/^(pub\s+)?use\s+/.test(trimmed)) {
    const vis = trimmed.startsWith("pub") ? "public" : "private" as const;
    const path = trimmed.replace(/^(pub\s+)?use\s+/, "").replace(/;.*$/, "").trim();
    return { kind: "import", name: path, visibility: vis, meta: { importPath: path } };
  }

  // const — extract name, type, and value: `const NAME: TYPE = VALUE;`
  if (/^(pub\s+)?const\s+/.test(trimmed)) {
    const vis = trimmed.startsWith("pub") ? "public" : "private" as const;
    const m = trimmed.match(/const\s+(\w+)\s*:\s*([^=]+?)\s*=/);
    const name = m?.[1] ?? trimmed.match(/const\s+(\w+)/)?.[1] ?? "UNNAMED";
    const returnType = m?.[2]?.trim();
    const value = trimmed.replace(/^.*?=\s*/, "").replace(/;$/, "").trim();
    return { kind: "constant", name, visibility: vis, meta: { value, returnType } };
  }

  // static
  if (/^(pub\s+)?static\s+/.test(trimmed) && !/^static\s+(PRAGMA|METADATA)\s*:/.test(trimmed)) {
    const vis = trimmed.startsWith("pub") ? "public" : "private" as const;
    const m = trimmed.match(/static\s+(?:mut\s+)?(\w+)/);
    return { kind: "variable", name: m?.[1] ?? "UNNAMED", visibility: vis, meta: {} };
  }

  // type alias
  if (/^(pub\s+)?type\s+\w+\s*=/.test(trimmed)) {
    const vis = trimmed.startsWith("pub") ? "public" : "private" as const;
    const m = trimmed.match(/type\s+(\w+)/);
    return { kind: "type_alias", name: m?.[1] ?? "UNNAMED", visibility: vis, meta: {} };
  }

  // struct
  if (/^(pub\s+)?struct\s+/.test(trimmed)) {
    const vis = trimmed.startsWith("pub") ? "public" : "private" as const;
    const m = trimmed.match(/struct\s+(\w+)/);
    return { kind: "struct", name: m?.[1] ?? "UNNAMED", visibility: vis, meta: {} };
  }

  // enum
  if (/^(pub\s+)?enum\s+/.test(trimmed)) {
    const vis = trimmed.startsWith("pub") ? "public" : "private" as const;
    const m = trimmed.match(/enum\s+(\w+)/);
    return { kind: "enum", name: m?.[1] ?? "UNNAMED", visibility: vis, meta: {} };
  }

  // trait
  if (/^(pub\s+)?trait\s+/.test(trimmed)) {
    const vis = trimmed.startsWith("pub") ? "public" : "private" as const;
    const m = trimmed.match(/trait\s+(\w+)/);
    return { kind: "trait", name: m?.[1] ?? "UNNAMED", visibility: vis, meta: {} };
  }

  // impl block
  if (/^impl\s+/.test(trimmed)) {
    const m = trimmed.match(/impl(?:\s*<[^>]*>)?\s+(?:(\w+)\s+for\s+)?(\w+)/);
    const receiver = m?.[2] ?? "";
    const trait = m?.[1];
    return { kind: "method", name: `impl ${receiver}`, visibility: "public", meta: { receiver, implements: trait } };
  }

  // function (pub fn, fn, pub(crate) fn, etc.)
  if (/^(pub(\([^)]*\))?\s+)?fn\s+/.test(trimmed)) {
    const vis = trimmed.startsWith("pub") ? "public" : "private" as const;
    const m = trimmed.match(/fn\s+(\w+)\s*(?:<[^>]*>)?\s*\(([^)]*)\)(?:\s*->\s*(.+?))?(?:\s*\{|\s*where|\s*$)/);
    const name = m?.[1] ?? "unnamed";
    const params = m?.[2]?.trim() ?? "";
    const returnType = m?.[3]?.trim();

    // Constructor detection: fn new, fn with_*, fn from_*
    const isConstructor = /^(new|with_|from_)/.test(name);
    return {
      kind: isConstructor ? "constructor" : "function",
      name,
      visibility: vis,
      meta: { params, returnType },
    };
  }

  return { kind: "unknown", name: "", visibility: "private", meta: {} };
}

/**
 * Extract semantic chunks from a block's subsection ranges.
 */
function extractBlockChunks(
  blockLines: string[],
  subsections: SubsectionDef[],
  blockName: "SETUP" | "BODY" | "CLOSING",
  sectionMap: Map<string, string>,
): SemanticChunk[] {
  const ranges = getSubsectionRanges(blockLines, subsections);
  const chunks: SemanticChunk[] = [];

  for (const range of ranges) {
    const sectionLines = blockLines.slice(range.startIdx, range.endIdx);
    const targetTag = sectionMap.get(range.tag) ?? range.tag;
    const items: ChunkItem[] = [];

    // Collect doc comments and classify code lines
    // Enhanced with body capture: when a function/method/constructor opens a brace,
    // track depth and capture all body lines until the matching close.
    let currentDoc: string[] = [];
    let bodyCapture: string[] | null = null;
    let braceDepth = 0;
    let capturingItem: ChunkItem | null = null;

    for (let lineIdx = 0; lineIdx < sectionLines.length; lineIdx++) {
      const line = sectionLines[lineIdx]!;
      const trimmed = line.trim();

      // If we're capturing a function body, track braces
      if (bodyCapture !== null && capturingItem !== null) {
        // Count braces on this line
        for (const ch of trimmed) {
          if (ch === "{") braceDepth++;
          if (ch === "}") braceDepth--;
        }

        if (braceDepth <= 0) {
          // Body capture complete — store and reset
          capturingItem.meta.bodyLines = bodyCapture;
          bodyCapture = null;
          capturingItem = null;
          braceDepth = 0;
          continue;
        }

        // Still inside body — capture the line
        bodyCapture.push(line);
        continue;
      }

      // Skip separator lines and section headers
      if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;
      if (/^\/\/\s*(?:\d+\.\s+)?\w/.test(trimmed) && items.length === 0 && currentDoc.length === 0) {
        // This is likely the section header — skip it
        continue;
      }

      // Doc comments (/// or // )
      if (trimmed.startsWith("///") || (trimmed.startsWith("//") && !trimmed.startsWith("//!"))) {
        currentDoc.push(trimmed.replace(/^\/\/\/?\s?/, ""));
        continue;
      }

      // Blank lines
      if (trimmed === "") {
        if (currentDoc.length > 0 && items.length > 0) {
          // Attach trailing doc to previous item or start fresh
          currentDoc = [];
        }
        continue;
      }

      // Classify the code line
      const classified = classifyRustItem(line);
      if (classified.kind !== "unknown") {
        const item: ChunkItem = {
          kind: classified.kind,
          name: classified.name,
          visibility: classified.visibility,
          docComment: [...currentDoc],
          sourceLines: [line],
          meta: classified.meta,
        };
        items.push(item);
        currentDoc = [];

        // Start body capture for functions/methods/constructors
        if (
          (classified.kind === "function" || classified.kind === "method" || classified.kind === "constructor") &&
          trimmed.includes("{")
        ) {
          // Count braces on the declaration line itself
          braceDepth = 0;
          for (const ch of trimmed) {
            if (ch === "{") braceDepth++;
            if (ch === "}") braceDepth--;
          }
          if (braceDepth > 0) {
            // Body starts — capture until matching close
            bodyCapture = [];
            capturingItem = item;
          }
        }
      }
    }

    chunks.push({
      sectionTag: range.tag,
      sourceBlock: blockName,
      targetTag,
      sourceLines: sectionLines,
      items,
    });
  }

  return chunks;
}

/**
 * Extract CLOSING zone chunks (different structure than SETUP/BODY).
 */
function extractClosingChunks(
  closingLines: string[],
  sectionMap: Map<string, string>,
): SemanticChunk[] {
  const chunks: SemanticChunk[] = [];

  // CLOSING zones: Cv, Ce, Cc, X1-X6
  const zonePattern = /^\/\/\s*(Cv|Ce|Cc|X[1-6])\s*[—:─]/;
  let currentZone: string | null = null;
  let currentLines: string[] = [];
  let currentFields: string[] = [];

  function flushZone(): void {
    if (currentZone) {
      const targetTag = sectionMap.get(currentZone) ?? currentZone;
      chunks.push({
        sectionTag: currentZone,
        sourceBlock: "CLOSING",
        targetTag,
        sourceLines: [...currentLines],
        items: [{
          kind: "closing_zone",
          name: currentZone,
          visibility: "public",
          docComment: [],
          sourceLines: [...currentLines],
          meta: { zoneTag: currentZone, zoneFields: [...currentFields] },
        }],
      });
    }
    currentZone = null;
    currentLines = [];
    currentFields = [];
  }

  for (const line of closingLines) {
    const trimmed = line.trim();

    // Skip separators
    if (/^\/\/\s*[─=\-]{4,}\s*$/.test(trimmed)) continue;

    const zoneMatch = trimmed.match(zonePattern);
    if (zoneMatch) {
      flushZone();
      currentZone = zoneMatch[1]!;
      currentLines.push(line);
      continue;
    }

    if (currentZone) {
      currentLines.push(line);
      // Field lines: "// Never: ...", "// Note: ...", etc.
      if (/^\/\/\s+\w+:\s/.test(trimmed)) {
        currentFields.push(trimmed);
      }
    }
  }
  flushZone();

  return chunks;
}

/**
 * Extract full semantics from a Rust source file.
 */
export async function extractRustSemantics(
  filePath: string,
): Promise<ExtractedSemantics> {
  const text = await Deno.readTextFile(filePath);
  const lines = text.split("\n");

  const blocks = findBlocks(lines);
  const metadataLines = getBlockLines(lines, blocks, "METADATA");
  const setupLines = getBlockLines(lines, blocks, "SETUP");
  const bodyLines = getBlockLines(lines, blocks, "BODY");
  const closingLines = getBlockLines(lines, blocks, "CLOSING");

  // Extract identity
  const identity = extractIdentity(metadataLines, lines, "rust");
  // Strip role suffix: "module->utility" → "module" (role is metadata, not structural form)
  const form = (identity.subtype || "module").replace(/->.*$/, "");

  // Load source composition target
  const composed = await loadComposition("rust", form);

  // Build section maps from the registered mapping
  const mapping = findMapping("rust", form, "go");
  const sectionMap = mapping?.sections ?? {
    setup: new Map<string, string>(),
    body: new Map<string, string>(),
    closing: new Map<string, string>(),
  };

  // Extract chunks from each block
  const setupSubsections = buildRustSetupSubsections();
  const setupChunks = extractBlockChunks(setupLines, setupSubsections, "SETUP", sectionMap.setup);

  // BODY subsections use numbered markers: // N. Name
  const bodySubsections: SubsectionDef[] = [];
  for (const line of bodyLines) {
    const match = line.trim().match(/^\/\/\s*(\d+)\.\s+(.+)/);
    if (match) {
      const tag = match[2]!.trim().replace(/\s+/g, "");
      if (!bodySubsections.some((s) => s.tag === tag)) {
        bodySubsections.push({
          tag,
          pattern: new RegExp(`^//\\s*${match[1]}\\.\\s+${match[2]!.trim().replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}`, "i"),
        });
      }
    }
  }
  const bodyChunks = extractBlockChunks(bodyLines, bodySubsections, "BODY", sectionMap.body);

  // CLOSING uses zone-based extraction
  const closingChunks = extractClosingChunks(closingLines, sectionMap.closing);

  return {
    identity,
    sourceTarget: composed.target,
    chunks: [...setupChunks, ...bodyChunks, ...closingChunks],
    sourceFile: filePath,
    sourceLineCount: lines.length,
  };
}

// ---------------------------------------------------------------------------
// 3. Target generation — Go
// ---------------------------------------------------------------------------

/**
 * Convert a Rust name to Go naming convention.
 *
 * - Public: capitalize first letter of each word (PascalCase)
 * - Private: lowercase first letter, camelCase rest
 * - snake_case → PascalCase or camelCase
 *
 * When schema-driven mappings are loaded (_mappings), delegates to
 * convertName() for schema-driven naming convention transforms.
 */
export function rustNameToGo(name: string, visibility: "public" | "private"): string {
  // Schema-driven path (when loaded by transpile/transpileFile)
  if (_mappings) {
    return convertName(_mappings, name, visibility, "function");
  }

  // Hardcoded fallback (direct unit test calls, no async init)
  const parts = name.split("_").filter(Boolean);
  if (visibility === "public") {
    return parts.map((p) => p.charAt(0).toUpperCase() + p.slice(1)).join("");
  }
  return parts.map((p, i) =>
    i === 0 ? p.charAt(0).toLowerCase() + p.slice(1) : p.charAt(0).toUpperCase() + p.slice(1)
  ).join("");
}

/**
 * Map a Rust type expression to a Go type (best-effort).
 *
 * When schema-driven mappings are loaded (_mappings), delegates to
 * convertType() for schema-driven type resolution with recursive
 * generic pattern matching.
 */
export function rustTypeToGo(rustType: string | undefined): string {
  if (!rustType) return "";

  // Schema-driven path (when loaded by transpile/transpileFile)
  if (_mappings) {
    return convertType(_mappings, rustType);
  }

  // Hardcoded fallback (direct unit test calls, no async init)
  const t = rustType.trim();
  const typeMap: Record<string, string> = {
    "String": "string",
    "&str": "string",
    "bool": "bool",
    "i8": "int8", "i16": "int16", "i32": "int32", "i64": "int64",
    "u8": "uint8", "u16": "uint16", "u32": "uint32", "u64": "uint64",
    "f32": "float32", "f64": "float64",
    "usize": "int", "isize": "int",
    "()": "", // unit type = no return
    "Self": "", // replaced by receiver type contextually
  };

  if (typeMap[t] !== undefined) return typeMap[t];

  // Vec<T> → []T
  const vecMatch = t.match(/^Vec<(.+)>$/);
  if (vecMatch) return `[]${rustTypeToGo(vecMatch[1])}`;

  // Option<T> → *T (pointer = nullable)
  const optionMatch = t.match(/^Option<(.+)>$/);
  if (optionMatch) return `*${rustTypeToGo(optionMatch[1])}`;

  // Result<T, E> → (T, error)
  const resultMatch = t.match(/^Result<(.+?),\s*(.+)>$/);
  if (resultMatch) return `(${rustTypeToGo(resultMatch[1])}, error)`;

  // HashMap<K, V> → map[K]V
  const hashMapMatch = t.match(/^HashMap<(.+?),\s*(.+)>$/);
  if (hashMapMatch) return `map[${rustTypeToGo(hashMapMatch[1])}]${rustTypeToGo(hashMapMatch[2])}`;

  // &T → *T, &mut T → *T
  if (t.startsWith("&mut ")) return `*${rustTypeToGo(t.slice(5))}`;
  if (t.startsWith("&")) return `*${rustTypeToGo(t.slice(1))}`;

  // Box<T> → *T
  const boxMatch = t.match(/^Box<(.+)>$/);
  if (boxMatch) return `*${rustTypeToGo(boxMatch[1])}`;

  // Strip lifetime annotations: 'static, 'a, etc.
  const stripped = t.replace(/'[a-z_]+\s*/g, "").trim();
  if (stripped !== t && stripped.length > 0) return rustTypeToGo(stripped);

  // Default: use the type name as-is (strip module path)
  return t.split("::").pop()!;
}

/**
 * Map a Rust import path to a Go import path (best-effort).
 *
 * Rust uses `std::*` crate paths; Go uses different package names.
 * This covers common stdlib mappings. Unknown crates get a TODO comment.
 *
 * When schema-driven mappings are loaded (_mappings), delegates to
 * convertImport() for schema-driven import resolution with prefix matching.
 */
export function rustImportToGo(rustPath: string): { goPath: string; mapped: boolean } {
  // Schema-driven path (when loaded by transpile/transpileFile)
  if (_mappings) {
    const result = convertImport(_mappings, rustPath);
    return { goPath: result.goPath, mapped: result.mapped };
  }

  // Hardcoded fallback (direct unit test calls, no async init)
  const importMap: Record<string, string> = {
    "std::fs": "os",
    "std::io": "io",
    "std::io::Read": "io",
    "std::io::Write": "io",
    "std::io::BufRead": "bufio",
    "std::io::BufReader": "bufio",
    "std::io::BufWriter": "bufio",
    "std::path::Path": "path/filepath",
    "std::path::PathBuf": "path/filepath",
    "std::fmt": "fmt",
    "std::string::String": "strings",
    "std::str": "strings",
    "std::collections::HashMap": "// no import needed — map is builtin",
    "std::collections::HashSet": "// no import needed — use map[T]struct{}",
    "std::collections::BTreeMap": "sort",
    "std::collections::VecDeque": "container/list",
    "std::vec::Vec": "// no import needed — slice is builtin",
    "std::sync::Arc": "sync",
    "std::sync::Mutex": "sync",
    "std::sync::RwLock": "sync",
    "std::thread": "// goroutines — no direct import",
    "std::time": "time",
    "std::time::Duration": "time",
    "std::time::Instant": "time",
    "std::error::Error": "errors",
    "std::env": "os",
    "std::process": "os/exec",
    "std::net": "net",
  };

  if (importMap[rustPath]) {
    return { goPath: importMap[rustPath], mapped: true };
  }

  for (const [rustPrefix, goImport] of Object.entries(importMap)) {
    if (rustPath.startsWith(rustPrefix)) {
      return { goPath: goImport, mapped: true };
    }
  }

  return { goPath: rustPath, mapped: false };
}

/**
 * Translate a Rust parameter list to Go parameter list.
 *
 * Rust: `config: &Config, timeout: u64`
 * Go:   `config *Config, timeout uint64`
 *
 * Strips `self` / `&self` / `&mut self` (Go uses receiver syntax instead).
 * Handles `_` prefix (Rust unused marker).
 *
 * When schema-driven mappings are loaded (_mappings), delegates to
 * convertParams() for schema-driven parameter conversion.
 */
export function rustParamsToGo(rustParams: string | undefined): string {
  if (!rustParams || rustParams.trim() === "") return "";

  // Schema-driven path (when loaded by transpile/transpileFile)
  if (_mappings) {
    return convertParams(_mappings, rustParams);
  }

  // Hardcoded fallback (direct unit test calls, no async init)
  const params = rustParams.split(",").map((p) => p.trim()).filter(Boolean);
  const goParams: string[] = [];

  for (const param of params) {
    if (/^&?(mut\s+)?self$/.test(param.trim())) continue;

    const colonIdx = param.indexOf(":");
    if (colonIdx === -1) continue;

    let name = param.slice(0, colonIdx).trim();
    const rustType = param.slice(colonIdx + 1).trim();

    if (name.startsWith("_")) name = name.slice(1) || name;

    const goName = rustNameToGo(name, "private");
    const goType = rustTypeToGo(rustType);

    goParams.push(`${goName} ${goType}`);
  }

  return goParams.join(", ");
}

/**
 * Derive a Go package name from source identity.
 *
 * Strategy:
 * 1. CLI override key → last segment (B-L0-hybrid-config → "config")
 * 2. Source identity key → last meaningful segment
 * 3. Source file path → parent directory name
 * 4. Fallback → "pkg"
 *
 * Go packages are lowercase, no underscores, no hyphens.
 */
function deriveGoPackage(overrideKey: string | undefined, identity: ExtractedIdentity): string {
  // Try override key first
  const keySource = overrideKey ?? identity.key;
  if (keySource) {
    const segments = keySource.split("-").filter(Boolean);
    // Last segment is usually the most specific: B-L0-hybrid-config-exists → "exists"
    const candidate = segments[segments.length - 1];
    if (candidate && candidate.length > 1 && candidate !== "B" && !/^[A-Z]\d/.test(candidate)) {
      return candidate.toLowerCase().replace(/[^a-z0-9]/g, "");
    }
  }

  // Try path: "L0-universal/c-hybrid/spec/config/src/exists.rs" → parent is "src", grandparent is "config"
  if (identity.from) {
    const parts = identity.from.replace(/\\/g, "/").split("/").filter(Boolean);
    // Walk up from filename to find a meaningful directory name
    for (let i = parts.length - 2; i >= 0; i--) {
      const dir = parts[i]!;
      if (dir !== "src" && dir !== "lib" && dir !== "pkg" && dir.length > 1) {
        return dir.toLowerCase().replace(/[^a-z0-9]/g, "");
      }
    }
  }

  return "pkg";
}

/**
 * Generate a Go stub for a single item.
 */
function generateGoItem(item: ChunkItem, _packageName: string): string[] {
  const lines: string[] = [];
  const goName = rustNameToGo(item.name, item.visibility);

  // Doc comment
  if (item.docComment.length > 0) {
    for (const doc of item.docComment) {
      lines.push(`// ${doc}`);
    }
  }

  switch (item.kind) {
    case "import": {
      // Map Rust crate path to Go import
      const { goPath, mapped } = rustImportToGo(item.meta.importPath ?? item.name);
      if (goPath.startsWith("//")) {
        // Comment-only mapping (e.g., builtins that don't need imports)
        lines.push(`\t${goPath}`);
      } else {
        const todo = mapped ? "" : " // TODO: Map Rust crate to Go package";
        lines.push(`\t"${goPath}"${todo}`);
      }
      break;
    }

    case "constant": {
      const goType = rustTypeToGo(item.meta.returnType);
      const typeAnnotation = goType ? ` ${goType}` : "";
      lines.push(`\t${goName}${typeAnnotation} = ${item.meta.value ?? "0"} // TODO: Verify value from Rust`);
      break;
    }

    case "variable":
      lines.push(`var ${goName} = nil // TODO: Translate from Rust static`);
      break;

    case "type_alias":
      lines.push(`type ${goName} = interface{} // TODO: Translate Rust type alias`);
      break;

    case "struct":
      lines.push(`// ${goName} — TODO: Translate struct fields from Rust`);
      lines.push(`type ${goName} struct {`);
      lines.push(`\t// TODO: Translate fields from Rust struct ${item.name}`);
      lines.push(`}`);
      break;

    case "enum":
      lines.push(`// ${goName} — TODO: Translate Rust enum to Go type + iota`);
      lines.push(`type ${goName} int`);
      lines.push(``);
      lines.push(`const (`);
      lines.push(`\t// TODO: Translate Rust enum ${item.name} variants`);
      lines.push(`\t_ ${goName} = iota`);
      lines.push(`)`);
      break;

    case "trait":
      lines.push(`// ${goName} — TODO: Translate trait methods from Rust`);
      lines.push(`type ${goName} interface {`);
      lines.push(`\t// TODO: Translate Rust trait ${item.name} methods`);
      lines.push(`}`);
      break;

    case "error_type":
      lines.push(`// Err${goName} is a sentinel error for ${item.name}.`);
      lines.push(`var Err${goName} = errors.New("${item.name}") // TODO: Translate from Rust`);
      break;

    case "constructor": {
      const receiverType = item.meta.receiver ?? "T";
      const goReceiver = rustNameToGo(receiverType, "public");
      const goReturn = goReceiver;
      lines.push(`// New${goReceiver} creates a new ${goReceiver}.`);
      if (item.meta.translatedBody && item.meta.translatedBody.length > 0) {
        lines.push(`func New${goReceiver}() *${goReturn} {`);
        for (const bodyLine of item.meta.translatedBody) {
          lines.push(bodyLine);
        }
        lines.push(`}`);
      } else {
        lines.push(`// TODO: Translate from Rust ${item.name}`);
        lines.push(`func New${goReceiver}() *${goReturn} {`);
        lines.push(`\tpanic("TODO: Translate from Rust")`);
        lines.push(`}`);
      }
      break;
    }

    case "function": {
      const goReturn = rustTypeToGo(item.meta.returnType);
      const returnClause = goReturn ? ` ${goReturn}` : "";
      const goParams = rustParamsToGo(item.meta.params);
      if (item.meta.translatedBody && item.meta.translatedBody.length > 0) {
        lines.push(`// ${goName} translates Rust ${item.name}.`);
        lines.push(`func ${goName}(${goParams})${returnClause} {`);
        for (const bodyLine of item.meta.translatedBody) {
          lines.push(bodyLine);
        }
        lines.push(`}`);
      } else {
        lines.push(`// ${goName} — TODO: Translate from Rust ${item.name}`);
        lines.push(`func ${goName}(${goParams})${returnClause} {`);
        lines.push(`\tpanic("TODO: Translate from Rust")`);
        lines.push(`}`);
      }
      break;
    }

    case "method": {
      // impl block — generate receiver methods
      const receiverType = item.meta.receiver ?? "T";
      const goReceiver = rustNameToGo(receiverType, "public");
      if (item.meta.implements) {
        lines.push(`// ${goReceiver} implements ${item.meta.implements}`);
      }
      lines.push(`// TODO: Translate methods from Rust impl ${item.name}`);
      break;
    }

    case "closing_zone":
      // Zone content is passed through
      if (item.meta.zoneFields) {
        for (const field of item.meta.zoneFields) {
          lines.push(field);
        }
      }
      break;

    default:
      lines.push(`// TODO: Translate ${item.kind} "${item.name}" from Rust`);
  }

  return lines;
}

/**
 * Generate the METADATA block for a Go target file.
 */
function generateGoMetadata(
  identity: ExtractedIdentity,
  targetFormat: string,
  targetForm: string,
  packageName: string,
): string[] {
  const lines: string[] = [];
  const today = new Date().toISOString().slice(0, 10);

  // --- Adaptation rules: which raw fields need target-specific values ---
  // Everything not listed passes through unchanged (faithful transfer).
  const targetKey = identity.key.replace(/rust|rs/gi, targetFormat);
  const targetFrom = identity.from
    .replace(/\.rs$/, `.${targetFormat === "go" ? "go" : targetFormat}`)
    .replace(/rust/gi, targetFormat);

  const adaptations: Record<string, (v: string) => string> = {
    "I1.key":       () => targetKey,
    "I1.format":    () => targetFormat,
    "I1.from":      () => targetFrom,
    "I1.at":        () => "a-01.00",
    "I2.subtype":   () => targetForm,
    "I2.structure":  (v) => v, // preserve (4-block stays 4-block)
    "I3.file":      () => targetFrom.split("/").pop() ?? "",
  };

  // Directives
  lines.push(`//omni:key ${targetKey}`);
  lines.push(`//omni:code --${targetFormat} -${targetForm}`);
  lines.push(`//omni:version a-01.00`);
  lines.push("");

  // Package declaration
  lines.push(`package ${packageName}`);
  lines.push("");

  // Block header
  lines.push(BLOCK_SEP);
  lines.push("// METADATA");
  lines.push(BLOCK_SEP);
  lines.push("//");
  lines.push(`// file:    ${targetFrom}`);
  lines.push(`// key:     ${targetKey}`);
  lines.push(`// title:   ${identity.title}`);
  lines.push(`// type:    Code (${targetForm.charAt(0).toUpperCase() + targetForm.slice(1)})`);
  lines.push(`// version: a-01.00`);
  lines.push(`// created: ${today}`);
  lines.push(`// authors: Nova Dawn (CPI-SI) — transpiled by cws-struct`);
  lines.push(`// purpose: ${identity.purpose}`);
  lines.push("//");
  lines.push(`//   Transpiled from: ${identity.from}`);
  lines.push(`//   Source format: ${identity.format}, form: ${identity.subtype}`);
  lines.push(`//   Target format: ${targetFormat}, form: ${targetForm}`);
  lines.push("//");
  lines.push("");

  // --- Faithful Pragma transfer ---
  // Transfer ALL I-field pairs from source, adapting only what changes.
  const pragmaKeys: [string, string][] = [];
  const metadataKeys: [string, string][] = [];

  for (const [key, value] of identity.raw) {
    const adapted = adaptations[key]?.(value) ?? value;
    if (key.startsWith("I")) {
      pragmaKeys.push([key, adapted]);
    } else if (key.startsWith("C")) {
      metadataKeys.push([key, adapted]);
    }
  }

  // Tripwire: if raw Map was empty (extraction failed), fall back to minimal stubs
  if (pragmaKeys.length === 0) {
    console.warn("[transpiler] WARNING: No Pragma fields in raw Map — using minimal fallback");
    pragmaKeys.push(["I1.key", targetKey], ["I1.format", targetFormat]);
  }
  if (metadataKeys.length === 0) {
    console.warn("[transpiler] WARNING: No Metadata fields in raw Map — using minimal fallback");
    metadataKeys.push(["C1.version", "a-01.00"], ["C1.created", today]);
  }

  lines.push(`// Pragma holds identity I-fields (OmniCode contract).`);
  lines.push(`var Pragma = [][2]string{`);
  for (const [key, value] of pragmaKeys) {
    lines.push(`\t{"${key}", "${value}"},`);
  }
  lines.push(`}`);
  lines.push("");

  // --- Faithful Metadata transfer ---
  lines.push(`// Metadata holds context C-fields (OmniCode contract).`);
  lines.push(`var Metadata = [][2]string{`);
  for (const [key, value] of metadataKeys) {
    lines.push(`\t{"${key}", "${value}"},`);
  }
  lines.push(`}`);
  lines.push("");

  lines.push(BLOCK_SEP);
  lines.push("// END METADATA");
  lines.push(BLOCK_SEP);

  return lines;
}

/**
 * Build target section ordering from structure schema + section mapping.
 *
 * The structure schema uses universal names (e.g., "CoreLogic"). The section
 * mapping translates source names to target tags (e.g., "CoreLogic" → "CoreOperations").
 * This function walks the structure ordering, maps each through the section mapping,
 * deduplicates, and appends any target tags from existing chunks not in the schema.
 *
 * @param structureOrder - Section names from structure schema ordering
 * @param sectionMap - Section mapping (source tag → target tag)
 * @param existingChunks - Map of target tags actually present in extracted chunks
 * @returns Ordered array of target section tags
 */
function buildTargetOrder(
  structureOrder: string[],
  sectionMap: Map<string, string>,
  existingChunks: Map<string, SemanticChunk[]>,
): string[] {
  const seen = new Set<string>();
  const order: string[] = [];

  // 1. Walk structure ordering, map through section mapping to target tags
  for (const structName of structureOrder) {
    // Try section mapping (source → target) first
    const mapped = sectionMap.get(structName);
    if (mapped && !seen.has(mapped)) {
      seen.add(mapped);
      order.push(mapped);
    }
    // Also try the name directly (for 1:1 mappings like closing zones)
    if (!mapped && !seen.has(structName)) {
      seen.add(structName);
      order.push(structName);
    }
  }

  // 2. Append any target tags from chunks not yet in the order (catch-all)
  for (const tag of existingChunks.keys()) {
    if (!seen.has(tag)) {
      seen.add(tag);
      order.push(tag);
    }
  }

  return order;
}

/**
 * Generate a Go 4-block file from extracted semantics.
 */
function generateGoFile(
  semantics: ExtractedSemantics,
  mapping: FormatFormMapping,
  packageName: string,
  targetStructure: FormStructure | null,
): { lines: string[]; stats: TranspileStats } {
  const stats: TranspileStats = {
    chunksExtracted: semantics.chunks.length,
    itemsExtracted: semantics.chunks.reduce((sum, c) => sum + c.items.length, 0),
    sectionsMapped: 0,
    stubsGenerated: 0,
    directTransfers: 0,
  };

  const allLines: string[] = [];

  // METADATA
  allLines.push(...generateGoMetadata(semantics.identity, mapping.targetFormat, mapping.targetForm, packageName));

  // SETUP
  allLines.push("");
  allLines.push(BLOCK_SEP);
  allLines.push("// SETUP");
  allLines.push(BLOCK_SEP);

  // Group SETUP chunks by target section tag
  const setupChunks = semantics.chunks.filter((c) => c.sourceBlock === "SETUP");
  const setupByTarget = new Map<string, SemanticChunk[]>();
  for (const chunk of setupChunks) {
    const key = chunk.targetTag;
    if (!setupByTarget.has(key)) setupByTarget.set(key, []);
    setupByTarget.get(key)!.push(chunk);
    stats.sectionsMapped++;
  }

  // Emit SETUP sections in canonical order — schema-driven with hardcoded fallback.
  // Structure schema uses universal names which may match target tags directly,
  // or may need mapping through the section mapping. Build the order by:
  // 1. Walk the structure ordering sequence
  // 2. Map each name through the section mapping to get the actual target tag
  // 3. Deduplicate (multiple source sections can map to the same target tag)
  // 4. Append any target tags from chunks that weren't in the schema (catch-all)
  const setupOrder = targetStructure
    ? buildTargetOrder(
        [...targetStructure.SETUP.required, ...targetStructure.SETUP.available],
        mapping.sections.setup,
        setupByTarget,
      )
    : ["Imports", "Constants", "Variables", "TypeAliases", "ErrorTypes", "CoreTypes", "InterfaceDefs"];
  let sectionNum = 1;
  for (const tag of setupOrder) {
    const chunks = setupByTarget.get(tag);
    if (!chunks || chunks.length === 0) continue;

    allLines.push("");
    allLines.push(SUB_SEP);
    allLines.push(`// ${sectionNum}. ${tag}`);
    allLines.push(SUB_SEP);
    allLines.push("");

    // Special handling for imports — merge extracted + body-derived packages
    if (tag === "Imports") {
      allLines.push(`import (`);

      // 1. Emit extracted imports (from source use declarations)
      const emittedImports = new Set<string>();
      for (const chunk of chunks) {
        for (const item of chunk.items) {
          const stubLines = generateGoItem(item, packageName);
          allLines.push(...stubLines);
          stats.stubsGenerated++;
          // Track which packages were already emitted
          for (const sl of stubLines) {
            const m = sl.match(/^\t"([^"]+)"/);
            if (m) emittedImports.add(m[1]!);
          }
        }
      }

      // 2. Collect body-derived packages from translated function bodies
      const bodyPackages = new Set<string>();
      for (const chunk of semantics.chunks) {
        for (const item of chunk.items) {
          if (item.meta.bodyPackagesUsed) {
            for (const pkg of item.meta.bodyPackagesUsed) {
              bodyPackages.add(pkg);
            }
          }
        }
      }

      // 3. Merge — add body packages not already in extracted imports
      const newPackages = [...bodyPackages].filter((pkg) => !emittedImports.has(pkg)).sort();
      if (newPackages.length > 0) {
        allLines.push("");
        allLines.push(`\t// Body-derived imports (detected from translated code)`);
        for (const pkg of newPackages) {
          allLines.push(`\t"${pkg}"`);
        }
      }

      allLines.push(`)`);
    } else {
      for (const chunk of chunks) {
        for (const item of chunk.items) {
          const stubLines = generateGoItem(item, packageName);
          allLines.push(...stubLines);
          allLines.push("");
          stats.stubsGenerated++;
        }
      }
    }

    sectionNum++;
  }

  allLines.push("");
  allLines.push(BLOCK_SEP);
  allLines.push("// END SETUP");
  allLines.push(BLOCK_SEP);

  // BODY
  allLines.push("");
  allLines.push(BLOCK_SEP);
  allLines.push("// BODY");
  allLines.push(BLOCK_SEP);

  const bodyChunks = semantics.chunks.filter((c) => c.sourceBlock === "BODY");
  const bodyByTarget = new Map<string, SemanticChunk[]>();
  for (const chunk of bodyChunks) {
    const key = chunk.targetTag;
    if (!bodyByTarget.has(key)) bodyByTarget.set(key, []);
    bodyByTarget.get(key)!.push(chunk);
    stats.sectionsMapped++;
  }

  // Emit BODY sections — schema-driven with hardcoded fallback
  const bodyOrder = targetStructure
    ? buildTargetOrder(
        targetStructure.BODY.ordering_sequence.filter((s) => !s.startsWith("[")),
        mapping.sections.body,
        bodyByTarget,
      )
    : ["OrgChart", "Helpers", "CoreOperations", "ErrorHandling", "PublicAPIs"];
  sectionNum = 1;
  for (const tag of bodyOrder) {
    const chunks = bodyByTarget.get(tag);
    if (!chunks || chunks.length === 0) continue;

    allLines.push("");
    allLines.push(SUB_SEP);
    allLines.push(`// ${sectionNum}. ${tag}`);
    allLines.push(SUB_SEP);
    allLines.push("");

    for (const chunk of chunks) {
      // Add source section reference
      if (chunk.sectionTag !== tag) {
        allLines.push(`// --- From Rust: ${chunk.sectionTag} ---`);
        allLines.push("");
      }
      for (const item of chunk.items) {
        const stubLines = generateGoItem(item, packageName);
        allLines.push(...stubLines);
        allLines.push("");
        stats.stubsGenerated++;
      }
    }

    sectionNum++;
  }

  allLines.push("");
  allLines.push(BLOCK_SEP);
  allLines.push("// END BODY");
  allLines.push(BLOCK_SEP);

  // CLOSING
  allLines.push("");
  allLines.push(BLOCK_SEP);
  allLines.push("// CLOSING");
  allLines.push(BLOCK_SEP);

  const closingChunks = semantics.chunks.filter((c) => c.sourceBlock === "CLOSING");
  // Closing order — schema-driven with hardcoded fallback
  // (Closing tags are typically 1:1 — Cv→Cv, X1→X1 — but still map through.)
  const closingOrder = targetStructure
    ? buildTargetOrder(
        [...targetStructure.CLOSING.required, ...targetStructure.CLOSING.available],
        mapping.sections.closing,
        new Map(), // closing chunks are looked up by find(), not by map key
      )
    : ["Cv", "Ce", "Cc", "X1", "X2", "X3", "X4", "X5"];
  for (const tag of closingOrder) {
    const chunk = closingChunks.find((c) => c.targetTag === tag);
    if (!chunk) continue;

    allLines.push("");
    allLines.push(SUB_SEP);

    // Zone header
    if (tag.startsWith("X")) {
      const zoneNames: Record<string, string> = {
        X1: "Policy", X2: "Extension", X3: "Troubleshooting",
        X4: "Reference", X5: "Note",
      };
      allLines.push(`// ${tag}: ${zoneNames[tag] ?? tag}`);
    } else {
      const zoneNames: Record<string, string> = {
        Cv: "Validation", Ce: "Execution", Cc: "Cleanup",
      };
      allLines.push(`// ${tag} — ${zoneNames[tag] ?? tag}`);
    }
    allLines.push(SUB_SEP);

    // Zone content — faithful transfer from sourceLines
    for (const item of chunk.items) {
      if (item.sourceLines.length > 0) {
        // Transfer full source content.
        // Skip the zone header line (already emitted above) and separator lines.
        const zoneHeaderPattern = /^\/\/\s*(Cv|Ce|Cc|X[1-6])\s*[—:─]/;
        const separatorPattern = /^\/\/\s*[─=\-]{4,}\s*$/;
        for (const line of item.sourceLines) {
          const trimmed = line.trim();
          if (zoneHeaderPattern.test(trimmed)) continue;
          if (separatorPattern.test(trimmed)) continue;

          // Cv zone: Rust test code — transfer as commented block for now.
          // Full test translation is future AST work.
          if (tag === "Cv" && !trimmed.startsWith("//") && trimmed !== "") {
            allLines.push(`// ${trimmed}`);
          } else {
            allLines.push(line);
          }
        }
        stats.directTransfers++;
      } else if (item.meta.zoneFields && item.meta.zoneFields.length > 0) {
        // Fallback: use extracted zone fields if sourceLines is empty
        for (const field of item.meta.zoneFields) {
          allLines.push(field);
        }
        stats.directTransfers++;
      } else {
        // Genuinely empty zone — only stub if it was empty in source
        allLines.push(`// (${tag} — empty in source)`);
        stats.directTransfers++;
      }
    }
  }

  allLines.push("");
  allLines.push(BLOCK_SEP);
  allLines.push("// END CLOSING");
  allLines.push(BLOCK_SEP);
  allLines.push(""); // trailing newline

  return { lines: allLines, stats };
}

// ---------------------------------------------------------------------------
// 4. Public API — transpile
// ---------------------------------------------------------------------------

/**
 * Transpile a 4-block code file from one format to another.
 *
 * Currently supports: Rust module → Go library.
 * The architecture is open for additional mappings by adding to FORMAT_MAPPINGS.
 */
export async function transpile(opts: TranspileOptions): Promise<TranspileResult> {
  const diagnostics: TranspileDiagnostic[] = [];

  // 0. Load schema-driven mappings (lazy init — falls back to hardcoded if unavailable)
  await ensureTranspilerMappings(opts.sourceFormat, opts.targetFormat);

  // 1. Validate we have a registered mapping
  const sourceForm = opts.sourceForm ?? "module";
  const targetForm = opts.targetForm ?? getTargetForm(opts.sourceFormat, sourceForm, opts.targetFormat);

  if (!targetForm) {
    return {
      success: false,
      sourceFile: "",
      output: "",
      lineCount: 0,
      diagnostics: [{
        severity: "error",
        message: `No mapping found: ${opts.sourceFormat} ${sourceForm} → ${opts.targetFormat}. Register a FormatFormMapping.`,
      }],
      sectionMapping: { setup: new Map(), body: new Map(), closing: new Map() },
      stats: { chunksExtracted: 0, itemsExtracted: 0, sectionsMapped: 0, stubsGenerated: 0, directTransfers: 0 },
    };
  }

  const mapping = findMapping(opts.sourceFormat, sourceForm, opts.targetFormat, targetForm);
  if (!mapping) {
    return {
      success: false,
      sourceFile: "",
      output: "",
      lineCount: 0,
      diagnostics: [{
        severity: "error",
        message: `No section mapping registered for ${opts.sourceFormat} ${sourceForm} → ${opts.targetFormat} ${targetForm}`,
      }],
      sectionMapping: { setup: new Map(), body: new Map(), closing: new Map() },
      stats: { chunksExtracted: 0, itemsExtracted: 0, sectionsMapped: 0, stubsGenerated: 0, directTransfers: 0 },
    };
  }

  // 2. Extract semantics from source
  diagnostics.push({ severity: "info", message: `Extracting semantics from ${opts.sourceFormat} source...` });

  let semantics: ExtractedSemantics;
  try {
    if (opts.sourceFormat === "rust") {
      semantics = await extractRustSemantics(opts.sourceFile);
    } else {
      return {
        success: false,
        sourceFile: "",
        output: "",
        lineCount: 0,
        diagnostics: [{ severity: "error", message: `Source extraction not implemented for ${opts.sourceFormat}` }],
        sectionMapping: mapping.sections,
        stats: { chunksExtracted: 0, itemsExtracted: 0, sectionsMapped: 0, stubsGenerated: 0, directTransfers: 0 },
      };
    }
  } catch (e) {
    return {
      success: false,
      sourceFile: "",
      output: "",
      lineCount: 0,
      diagnostics: [{ severity: "error", message: `Extraction failed: ${(e as Error).message}` }],
      sectionMapping: mapping.sections,
      stats: { chunksExtracted: 0, itemsExtracted: 0, sectionsMapped: 0, stubsGenerated: 0, directTransfers: 0 },
    };
  }

  diagnostics.push({
    severity: "info",
    message: `Extracted ${semantics.chunks.length} chunks, ${semantics.chunks.reduce((s, c) => s + c.items.length, 0)} items`,
  });

  // 2.5. Translate function bodies using Universal Concept AST
  let bodiesTranslated = 0;
  for (const chunk of semantics.chunks) {
    for (const item of chunk.items) {
      if (item.meta.bodyLines && item.meta.bodyLines.length > 0) {
        try {
          const bodyResult = await translateBody(
            item.meta.bodyLines,
            opts.sourceFormat,
            opts.targetFormat,
            _mappings,
            "\t",
          );
          if (bodyResult.translated) {
            item.meta.translatedBody = bodyResult.lines;
            bodiesTranslated++;
          }
          if (bodyResult.packagesUsed.length > 0) {
            item.meta.bodyPackagesUsed = bodyResult.packagesUsed;
          }
        } catch {
          // Body translation failed — fall back to panic("TODO")
        }
      }
    }
  }
  if (bodiesTranslated > 0) {
    diagnostics.push({
      severity: "info",
      message: `Translated ${bodiesTranslated} function bodies via Universal Concept AST`,
    });
  }

  // 3. Load target form structure schema (tripwire: warn + fallback if missing)
  let targetStructure: FormStructure | null = null;
  try {
    targetStructure = await loadFormStructureCached(targetForm);
  } catch {
    diagnostics.push({
      severity: "warn",
      message: `Could not load structure schema for form '${targetForm}' — using hardcoded fallback ordering`,
    });
  }

  // 4. Generate target file
  diagnostics.push({ severity: "info", message: `Generating ${opts.targetFormat} ${targetForm}...` });

  // Derive Go package name from identity:
  // 1. CLI --key override → last segment (B-L0-hybrid-config → config)
  // 2. Source key → last meaningful segment
  // 3. Source path → parent directory name
  // 4. Fallback → "pkg"
  const packageName = deriveGoPackage(opts.key, semantics.identity);
  const { lines: targetLines, stats } = generateGoFile(semantics, mapping, packageName, targetStructure);
  const output = targetLines.join("\n");

  diagnostics.push({
    severity: "info",
    message: `Generated ${targetLines.length} lines (${stats.stubsGenerated} stubs, ${stats.directTransfers} direct transfers)`,
  });

  // 4. Write or preview
  let targetFile: string | undefined;
  if (opts.outputPath && !opts.dryRun) {
    await Deno.writeTextFile(opts.outputPath, output);
    targetFile = opts.outputPath;
    diagnostics.push({ severity: "info", message: `Written to ${opts.outputPath}` });
  }

  return {
    success: true,
    sourceFile: semantics.sourceFile,
    targetFile,
    output,
    lineCount: targetLines.length,
    diagnostics,
    sectionMapping: mapping.sections,
    stats,
  };
}

/**
 * Transpile from a source file path directly.
 * Convenience wrapper that resolves forms from the source file's pragma.
 */
export async function transpileFile(
  sourceFile: string,
  sourceFormat: CodeFormat,
  targetFormat: CodeFormat,
  opts: Partial<TranspileOptions> = {},
): Promise<TranspileResult> {
  // Read source to detect form from pragma
  const text = await Deno.readTextFile(sourceFile);
  const firstLines = text.split("\n").slice(0, 10);
  let sourceForm = opts.sourceForm;

  if (!sourceForm) {
    for (const line of firstLines) {
      if (line.startsWith("//omni:code")) {
        const parts = line.split(/\s+/);
        for (const p of parts) {
          if (p.startsWith("-") && !p.startsWith("--")) {
            sourceForm = p.slice(1);
            break;
          }
        }
        break;
      }
    }
  }

  // Strip role suffix: "module->utility" → "module" (role is metadata, not structural form)
  sourceForm = (sourceForm ?? "module").replace(/->.*$/, "");
  const targetForm = opts.targetForm ?? getTargetForm(sourceFormat, sourceForm, targetFormat);

  // Derive output path
  const outputPath = opts.outputPath ?? sourceFile.replace(/\.rs$/, ".go");

  return transpile({
    sourceFile,
    sourceFormat,
    targetFormat,
    sourceForm,
    targetForm: targetForm ?? undefined,
    dryRun: opts.dryRun ?? false,
    outputPath,
    key: opts.key,
    title: opts.title,
  });
}

// ============================================================================
// CLOSING
// ============================================================================
//
// The three-cord transpiler: Source → IR → Target.
//
// Phase F.2 proves the architecture:
// - Rust module → Go library with structural fidelity
// - Section mapping preserves block organization
// - Function stubs carry identity and purpose
// - Generated Go passes cws-struct lint (the schema validates the transpiler)
//
// The insight: transpilation is composition viewed sideways.
// A composition target says "what sections does THIS format need?"
// A section mapping says "how do THOSE sections relate across formats?"
// The transpiler reads one composition and writes through another.
//
// "To every thing there is a season, and a time to every purpose
//   under the heaven." — Ecclesiastes 3:1
// ============================================================================
