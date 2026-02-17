// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/formats/omni.ts
// key:     B-tov-cws-struct-lib-formats-omni
// title:   CWS Struct — OmniCode (.omni) Format Handler
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-13
// authors: Nova Dawn (CPI-SI)
// purpose: OmniCode folder description linter. Validates .omni files against
//          the operator syntax defined in omnicode-operators.toml — verifying
//          that the files actually USE the syntax we defined.
//
//          Line-based parser — .omni is not a structured data format like TOML,
//          it's a natural-language-meets-operator format parsed by line.
//
// biblical_foundation: "Try the spirits whether they are of God."
//   — 1 John 4:1
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { dirname, join } from "@std/path";
import type { FormatHandler, LintResult, TransformOptions } from "../types.ts";
import { error, warn, info } from "../types.ts";
import { registerFormat } from "./registry.ts";

// ---------------------------------------------------------------------------
// Constants — what we expect in each block
// ---------------------------------------------------------------------------

/** Required block tags in order. */
const BLOCK_TAGS = [
  "METADATA [METADATA]",
  "CONTENT [CONTENT]",
  "CLOSING [CLOSING]",
  "END [END]",
] as const;

/** Required @ directives in METADATA block. */
const REQUIRED_DIRECTIVES = [
  "@key",
  "@type",
  "@title",
  "@struct",
] as const;

/** Required state directives in METADATA block. */
const REQUIRED_STATE = [
  "@version",
  "@created",
] as const;

/** Required attribution directives. */
const REQUIRED_ATTRIBUTION = [
  "@architect",
  "@implementation",
] as const;

/** Recommended but not required directives (used in checkState/checkDependencies). */
const _RECOMMENDED_DIRECTIVES = [
  "@status",
  "@updated",
  "@from",
  "@role",
] as const;

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

interface ParsedOmni {
  lines: string[];
  shebang: string | null;
  blockPositions: Map<string, number>;
  directives: Map<string, string[]>;
  hasContainment: boolean;
  hasCcLabel: boolean;
  hasProvides: boolean;
  hasDerives: boolean;
  hasNavigate: boolean;
  hasTypeBinding: boolean;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers — line parsing
// ---------------------------------------------------------------------------

function isBlockTag(line: string): string | null {
  const trimmed = line.trim();
  // Match: // METADATA [METADATA], // CONTENT [CONTENT], etc.
  const match = trimmed.match(/^\/\/\s+\S+.*\[(\w+)\]\s*$/);
  if (match) return match[1]!;
  return null;
}

function getDirective(line: string): string | null {
  const trimmed = line.trim();
  const match = trimmed.match(/^(@\w+)\s/);
  if (match) return match[1]!;
  return null;
}

// ---------------------------------------------------------------------------
// Core — Parse .omni file into structure
// ---------------------------------------------------------------------------

function parseOmni(text: string): ParsedOmni {
  const lines = text.split("\n");
  const blockPositions = new Map<string, number>();
  const directives = new Map<string, string[]>();

  let hasContainment = false;
  let hasCcLabel = false;
  let hasProvides = false;
  let hasDerives = false;
  let hasNavigate = false;
  let hasTypeBinding = false;

  // First line = shebang
  const shebang = lines.length > 0 ? lines[0]!.trim() : null;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]!;
    const trimmed = line.trim();

    // Block tags
    const tag = isBlockTag(trimmed);
    if (tag && !blockPositions.has(tag)) {
      blockPositions.set(tag, i);
    }

    // @ directives
    const directive = getDirective(trimmed);
    if (directive) {
      const existing = directives.get(directive) ?? [];
      existing.push(trimmed);
      directives.set(directive, existing);
    }

    // >> containment
    if (trimmed.startsWith(">>")) {
      hasContainment = true;
    }

    // Cc label
    if (trimmed.includes("Cc") && trimmed.includes("Content Configuration")) {
      hasCcLabel = true;
    }

    // => provides
    if (trimmed.startsWith("=>")) {
      hasProvides = true;
    }

    // <- derives
    if (trimmed.startsWith("<-")) {
      hasDerives = true;
    }

    // @navigate
    if (directive === "@navigate") {
      hasNavigate = true;
    }

    // :: type binding
    if (trimmed.includes("::")) {
      hasTypeBinding = true;
    }
  }

  return {
    lines,
    shebang,
    blockPositions,
    directives,
    hasContainment,
    hasCcLabel,
    hasProvides,
    hasDerives,
    hasNavigate,
    hasTypeBinding,
  };
}

// ---------------------------------------------------------------------------
// Checks — each returns LintResult[]
// ---------------------------------------------------------------------------

function checkShebang(file: string, parsed: ParsedOmni): LintResult[] {
  const results: LintResult[] = [];

  if (!parsed.shebang) {
    results.push(error(file, "shebang/exists", "Missing shebang line"));
    return results;
  }

  if (!parsed.shebang.startsWith("#!omni")) {
    results.push(
      error(file, "shebang/format", `Invalid shebang: "${parsed.shebang}" — expected "#!omni <type>"`),
    );
  }

  return results;
}

function checkBlocks(file: string, parsed: ParsedOmni): LintResult[] {
  const results: LintResult[] = [];

  // Check each required block tag exists
  for (const tag of BLOCK_TAGS) {
    const tagName = tag.match(/\[(\w+)\]/)![1]!;
    if (!parsed.blockPositions.has(tagName)) {
      results.push(
        error(file, `block/${tagName}`, `Missing block tag: ${tag}`),
      );
    }
  }

  // Check block order
  const positions = BLOCK_TAGS
    .map((tag) => {
      const tagName = tag.match(/\[(\w+)\]/)![1]!;
      return { tag: tagName, pos: parsed.blockPositions.get(tagName) ?? -1 };
    })
    .filter((p) => p.pos >= 0);

  for (let i = 1; i < positions.length; i++) {
    if (positions[i]!.pos < positions[i - 1]!.pos) {
      results.push(
        error(
          file,
          "block/order",
          `Block order violation: [${positions[i]!.tag}] appears before [${positions[i - 1]!.tag}]`,
        ),
      );
    }
  }

  return results;
}

function checkIdentity(file: string, parsed: ParsedOmni): LintResult[] {
  const results: LintResult[] = [];

  // Required identity directives
  for (const dir of REQUIRED_DIRECTIVES) {
    if (!parsed.directives.has(dir)) {
      results.push(
        error(file, `identity/${dir}`, `Missing required directive: ${dir}`),
      );
    }
  }

  // Check @type uses :: binding
  const typeLines = parsed.directives.get("@type") ?? [];
  if (typeLines.length > 0) {
    const hasBinding = typeLines.some((l) => l.includes("::"));
    if (!hasBinding) {
      results.push(
        warn(
          file,
          "identity/@type-binding",
          `@type should use :: type binding (e.g., @type seed :: folder)`,
        ),
      );
    }
  }

  return results;
}

function checkState(file: string, parsed: ParsedOmni): LintResult[] {
  const results: LintResult[] = [];

  for (const dir of REQUIRED_STATE) {
    if (!parsed.directives.has(dir)) {
      results.push(
        error(file, `state/${dir}`, `Missing required directive: ${dir}`),
      );
    }
  }

  // Recommended state directives
  if (!parsed.directives.has("@updated")) {
    results.push(
      warn(file, "state/@updated", "Missing @updated — recommended for tracking changes"),
    );
  }

  if (!parsed.directives.has("@status")) {
    results.push(
      warn(file, "state/@status", "Missing @status — recommended (Active, Draft, etc.)"),
    );
  }

  return results;
}

function checkAttribution(file: string, parsed: ParsedOmni): LintResult[] {
  const results: LintResult[] = [];

  for (const dir of REQUIRED_ATTRIBUTION) {
    if (!parsed.directives.has(dir)) {
      results.push(
        error(file, `attribution/${dir}`, `Missing required directive: ${dir}`),
      );
    }
  }

  return results;
}

function checkDependencies(file: string, parsed: ParsedOmni): LintResult[] {
  const results: LintResult[] = [];

  if (!parsed.directives.has("@from")) {
    results.push(
      warn(file, "deps/@from", "Missing @from — where does this folder derive from?"),
    );
  }

  if (!parsed.hasProvides) {
    results.push(
      warn(file, "deps/provides", "Missing => provides — what does this folder export?"),
    );
  }

  return results;
}

function checkContent(file: string, parsed: ParsedOmni): LintResult[] {
  const results: LintResult[] = [];

  if (!parsed.hasContainment) {
    results.push(
      warn(file, "content/containment", "No >> containment entries — content block has no folder contents declared"),
    );
  }

  if (!parsed.hasCcLabel) {
    results.push(
      info(file, "content/cc-label", "Missing Cc — Content Configurations label in content block"),
    );
  }

  return results;
}

function checkClosing(file: string, parsed: ParsedOmni): LintResult[] {
  const results: LintResult[] = [];

  if (!parsed.hasNavigate) {
    results.push(
      warn(file, "closing/@navigate", "Missing @navigate in closing — quick reference path"),
    );
  }

  return results;
}

function checkOperatorUsage(file: string, parsed: ParsedOmni): LintResult[] {
  const results: LintResult[] = [];

  // Check that :: type binding is used somewhere (sign of using defined syntax)
  if (!parsed.hasTypeBinding) {
    results.push(
      info(
        file,
        "syntax/type-binding",
        "No :: type binding found — consider using @type <subtype> :: <type>",
      ),
    );
  }

  return results;
}

// ---------------------------------------------------------------------------
// Lint orchestrator
// ---------------------------------------------------------------------------

async function lintOmniFile(filePath: string): Promise<LintResult[]> {
  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  if (text.trim().length === 0) {
    return [error(filePath, "parse/empty", "File is empty")];
  }

  const parsed = parseOmni(text);

  return [
    ...checkShebang(filePath, parsed),
    ...checkBlocks(filePath, parsed),
    ...checkIdentity(filePath, parsed),
    ...checkState(filePath, parsed),
    ...checkAttribution(filePath, parsed),
    ...checkDependencies(filePath, parsed),
    ...checkContent(filePath, parsed),
    ...checkClosing(filePath, parsed),
    ...checkOperatorUsage(filePath, parsed),
  ];
}

// ---------------------------------------------------------------------------
// Transform — omni → ofd compilation
// ---------------------------------------------------------------------------

// --- Value extraction helpers ---

/** Extract the value portion after `@directive ` from a full directive line. */
function extractDirectiveValue(line: string): string {
  const trimmed = line.trim();
  // Skip the @word part and return the rest
  const match = trimmed.match(/^@\w+\s+(.*)/);
  return match ? match[1]!.trim() : "";
}

/** Strip surrounding double quotes from a value. */
function unquote(val: string): string {
  if (val.startsWith('"') && val.endsWith('"')) {
    return val.slice(1, -1);
  }
  return val;
}

/** Get the first value for a directive (unquoted). */
function getFirstValue(parsed: ParsedOmni, directive: string): string | undefined {
  const lines = parsed.directives.get(directive);
  if (!lines || lines.length === 0) return undefined;
  const raw = extractDirectiveValue(lines[0]!);
  return unquote(raw);
}

/** Get all values for a directive (unquoted). */
function getAllValues(parsed: ParsedOmni, directive: string): string[] {
  const lines = parsed.directives.get(directive);
  if (!lines) return [];
  return lines.map((l) => unquote(extractDirectiveValue(l)));
}

/** Parse @type with :: binding: "@type seed :: folder" → { type, subtype }. */
function parseTypeBinding(parsed: ParsedOmni): { type: string; subtype?: string } {
  const lines = parsed.directives.get("@type");
  if (!lines || lines.length === 0) return { type: "unknown" };
  const raw = extractDirectiveValue(lines[0]!);
  const parts = raw.split("::");
  const type = parts[0]!.trim();
  const subtype = parts.length > 1 ? parts[1]!.trim() : undefined;
  return { type, subtype };
}

/**
 * Parse @scripture directives with continuation lines.
 * Pattern: @scripture "text" followed by `from Reference in Translation`
 * The `from...in` may be on the same line or a continuation line.
 */
function parseScriptures(text: string): Array<{ text: string; reference: string; translation: string }> {
  const lines = text.split("\n");
  const scriptures: Array<{ text: string; reference: string; translation: string }> = [];

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();
    if (!trimmed.startsWith("@scripture")) continue;

    const raw = extractDirectiveValue(trimmed);
    const scriptureText = unquote(raw.replace(/\s+from\s+.*$/, "").trim());

    // Look for `from ... in ...` on same line or continuation
    let reference = "";
    let translation = "";

    // Check same line
    const fromMatchSame = raw.match(/from\s+(.+?)\s+in\s+(\w+)/);
    if (fromMatchSame) {
      reference = fromMatchSame[1]!.trim();
      translation = fromMatchSame[2]!.trim();
    } else {
      // Check next line for continuation
      if (i + 1 < lines.length) {
        const nextTrimmed = lines[i + 1]!.trim();
        const fromMatchNext = nextTrimmed.match(/^from\s+(.+?)\s+in\s+(\w+)/);
        if (fromMatchNext) {
          reference = fromMatchNext[1]!.trim();
          translation = fromMatchNext[2]!.trim();
        }
      }
    }

    scriptures.push({ text: scriptureText, reference, translation });
  }

  return scriptures;
}

/** Parse >> containment entries: ">> path :: \"description\"" */
function parseChildren(text: string): Array<{ path: string; type: string; description: string }> {
  const lines = text.split("\n");
  const children: Array<{ path: string; type: string; description: string }> = [];

  for (const line of lines) {
    const trimmed = line.trim();
    if (!trimmed.startsWith(">>")) continue;

    const rest = trimmed.slice(2).trim();

    // Split on :: to get path and description
    const bindingIdx = rest.indexOf("::");
    if (bindingIdx < 0) {
      // No description, just a path
      const path = rest.trim();
      children.push({
        path: path.replace(/\s+$/, ""),
        type: path.endsWith("/") ? "dir" : "file",
        description: "",
      });
      continue;
    }

    const path = rest.slice(0, bindingIdx).trim();
    const desc = unquote(rest.slice(bindingIdx + 2).trim());

    children.push({
      path,
      type: path.endsWith("/") ? "dir" : "file",
      description: desc,
    });
  }

  return children;
}

/** Parse let...be declarations: "let name be value" */
function parseDeclarations(text: string): Record<string, unknown> {
  const lines = text.split("\n");
  const decls: Record<string, unknown> = {};

  for (const line of lines) {
    const trimmed = line.trim();
    const match = trimmed.match(/^let\s+(\w+)\s+be\s+(.+)/);
    if (!match) continue;

    const name = match[1]!;
    const rawValue = match[2]!.trim();

    // Try to parse as number
    const num = Number(rawValue);
    if (!isNaN(num)) {
      decls[name] = num;
    } else {
      decls[name] = unquote(rawValue);
    }
  }

  return decls;
}

/** Parse => provides: "=> [TAG1, TAG2]" */
function parseProvides(text: string): string[] {
  const lines = text.split("\n");
  for (const line of lines) {
    const trimmed = line.trim();
    if (!trimmed.startsWith("=>")) continue;
    const bracketMatch = trimmed.match(/=>\s*\[(.+)\]/);
    if (bracketMatch) {
      return bracketMatch[1]!.split(",").map((s) => s.trim());
    }
  }
  return [];
}

/** Parse <- derives: "<- path :: \"description\"" */
function parseDerives(text: string): Array<{ path: string; description: string }> {
  const lines = text.split("\n");
  const derives: Array<{ path: string; description: string }> = [];

  for (const line of lines) {
    const trimmed = line.trim();
    if (!trimmed.startsWith("<-")) continue;

    const rest = trimmed.slice(2).trim();
    const bindingIdx = rest.indexOf("::");
    if (bindingIdx < 0) {
      derives.push({ path: rest.trim(), description: "" });
      continue;
    }

    const path = rest.slice(0, bindingIdx).trim();
    const desc = unquote(rest.slice(bindingIdx + 2).trim());
    derives.push({ path, description: desc });
  }

  return derives;
}

// --- OFD builder ---

interface OfdDocument {
  ofd: number;
  source: string;
  compiled: string;
  identity: {
    key: string;
    type: string;
    subtype?: string;
    title: string;
    structure?: string;
  };
  state: {
    version: string;
    status?: string;
    created: string;
    updated?: string;
  };
  attribution: {
    architect: string;
    implementation: string;
    organization?: string;
  };
  grounding?: {
    scriptures: Array<{ text: string; reference: string; translation: string }>;
    principle?: string;
  };
  dependencies?: {
    from?: string;
    provides: string[];
    derives: Array<{ path: string; description: string }>;
  };
  metadata?: {
    role?: string;
    design?: string;
  };
  children: Array<{ path: string; type: string; description: string }>;
  declarations?: Record<string, unknown>;
  observations?: string[];
  navigation?: string;
}

/**
 * Compile a parsed .omni file into an .ofd JSON document.
 * This is the first real OmniCode compiler.
 */
function compileToOfd(text: string, parsed: ParsedOmni, sourceFilename: string): OfdDocument {
  const typeInfo = parseTypeBinding(parsed);
  const scriptures = parseScriptures(text);
  const children = parseChildren(text);
  const declarations = parseDeclarations(text);
  const provides = parseProvides(text);
  const derives = parseDerives(text);

  const ofd: OfdDocument = {
    ofd: 1,
    source: sourceFilename,
    compiled: new Date().toISOString(),
    identity: {
      key: getFirstValue(parsed, "@key") ?? "",
      type: typeInfo.type,
      title: getFirstValue(parsed, "@title") ?? "",
    },
    state: {
      version: getFirstValue(parsed, "@version") ?? "",
      created: getFirstValue(parsed, "@created") ?? "",
    },
    attribution: {
      architect: getFirstValue(parsed, "@architect") ?? "",
      implementation: getFirstValue(parsed, "@implementation") ?? "",
    },
    children,
  };

  // Optional identity fields
  if (typeInfo.subtype) ofd.identity.subtype = typeInfo.subtype;
  const struct = getFirstValue(parsed, "@struct");
  if (struct) ofd.identity.structure = struct;

  // Optional state fields
  const status = getFirstValue(parsed, "@status");
  if (status) ofd.state.status = status;
  const updated = getFirstValue(parsed, "@updated");
  if (updated) ofd.state.updated = updated;

  // Optional attribution fields
  const org = getFirstValue(parsed, "@organization");
  if (org) ofd.attribution.organization = org;

  // Grounding (only if we have scriptures or principle)
  const principle = getFirstValue(parsed, "@principle");
  if (scriptures.length > 0 || principle) {
    ofd.grounding = { scriptures, principle };
  }

  // Dependencies (only if we have @from, provides, or derives)
  const from = getFirstValue(parsed, "@from");
  if (from || provides.length > 0 || derives.length > 0) {
    ofd.dependencies = { from, provides, derives };
  }

  // Metadata (only if we have role or design)
  const role = getFirstValue(parsed, "@role");
  const design = getFirstValue(parsed, "@design");
  if (role || design) {
    ofd.metadata = { role, design };
  }

  // Declarations (only if non-empty)
  if (Object.keys(declarations).length > 0) {
    ofd.declarations = declarations;
  }

  // Observations
  const observations = getAllValues(parsed, "@observe");
  if (observations.length > 0) {
    ofd.observations = observations;
  }

  // Navigation
  const navigate = getFirstValue(parsed, "@navigate");
  if (navigate) {
    ofd.navigation = navigate;
  }

  return ofd;
}

// --- Transform orchestrator ---

async function transformOmniFile(filePath: string, opts: TransformOptions): Promise<LintResult[]> {
  const { dryRun } = opts;
  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  if (text.trim().length === 0) {
    return [error(filePath, "parse/empty", "File is empty")];
  }

  const parsed = parseOmni(text);

  // Verify shebang is folder type
  if (!parsed.shebang || !parsed.shebang.startsWith("#!omni")) {
    return [error(filePath, "transform/shebang", "Cannot compile: not an #!omni file")];
  }

  // Extract source filename
  const sourceFilename = filePath.split("/").pop() ?? "root.omni";

  // Compile
  const ofd = compileToOfd(text, parsed, sourceFilename);

  // Determine output path (root.omni → root.ofd)
  const dir = dirname(filePath);
  const baseName = sourceFilename.replace(/\.omni$/, ".ofd");
  const outputPath = join(dir, baseName);

  if (dryRun) {
    return [
      info(filePath, "transform/dry-run", `Would compile → ${baseName}`),
      info(filePath, "transform/identity", `key=${ofd.identity.key}, type=${ofd.identity.type}`),
      info(filePath, "transform/children", `${ofd.children.length} children entries`),
      info(filePath, "transform/output", `Would write ${JSON.stringify(ofd, null, 4).length} bytes to ${outputPath}`),
    ];
  }

  // Write .ofd
  try {
    const json = JSON.stringify(ofd, null, 4) + "\n";
    await Deno.writeTextFile(outputPath, json);
  } catch (e) {
    return [error(filePath, "io/write", `Cannot write ${outputPath}: ${e}`)];
  }

  return [
    info(filePath, "transform/compiled", `Compiled → ${baseName} (${ofd.children.length} children, key=${ofd.identity.key})`),
  ];
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

const omniHandler: FormatHandler = {
  name: "omni",
  description: "OmniCode folder descriptions — @directives, >> containment, :: bindings, [TAG] blocks",
  extensions: [".omni"],
  maxDepth: 10,
  lint: lintOmniFile,
  transform: transformOmniFile,
};

registerFormat(omniHandler);

export default omniHandler;
