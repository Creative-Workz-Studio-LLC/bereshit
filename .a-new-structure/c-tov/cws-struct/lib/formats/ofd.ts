// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/formats/ofd.ts
// key:     B-tov-cws-struct-lib-formats-ofd
// title:   CWS Struct — OmniFold (.ofd) Format Handler
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-13
// authors: Nova Dawn (CPI-SI)
// derives: lib/formats/json.ts (adapted for fixed-schema compiled output)
// purpose: OmniFold linter — validates compiled .ofd JSON files against
//          the fixed schema. .ofd is COMPILED OUTPUT from root.omni, not
//          human-authored. Clean JSON, no _ key ceremony, no comments.
//
//          The linter IS the contract. The omni transform IMPLEMENTS it.
//          Two witnesses establishing one truth (Deuteronomy 19:15).
//
// biblical_foundation: "In the mouth of two or three witnesses shall
//   every word be established." — 2 Corinthians 13:1
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { FormatHandler, LintResult } from "../types.ts";
import { error, warn, info } from "../types.ts";
import { registerFormat } from "./registry.ts";

// ---------------------------------------------------------------------------
// Constants — the .ofd schema contract
// ---------------------------------------------------------------------------

/** Required top-level fields. */
const REQUIRED_TOP = ["ofd", "source", "compiled", "identity", "state", "attribution", "children"] as const;

/** Recommended top-level fields (warn if missing). */
const RECOMMENDED_TOP = ["grounding", "dependencies", "metadata", "navigation"] as const;

/** Optional top-level fields (info if missing). */
const OPTIONAL_TOP = ["declarations", "observations"] as const;

/** Required fields within identity section. */
const REQUIRED_IDENTITY = ["key", "type", "title"] as const;

/** Recommended identity fields. */
const RECOMMENDED_IDENTITY = ["subtype", "structure"] as const;

/** Required fields within state section. */
const REQUIRED_STATE = ["version", "created"] as const;

/** Recommended state fields. */
const RECOMMENDED_STATE = ["status", "updated"] as const;

/** Required fields within attribution section. */
const REQUIRED_ATTRIBUTION = ["architect", "implementation"] as const;

/** Recommended attribution fields. */
const RECOMMENDED_ATTRIBUTION = ["organization"] as const;

/** Required grounding fields (when grounding section exists). */
const RECOMMENDED_GROUNDING = ["scriptures", "principle"] as const;

/** Required dependency fields (when dependencies section exists). */
const RECOMMENDED_DEPS = ["from", "provides"] as const;

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers — type checking
// ---------------------------------------------------------------------------

function isObject(val: unknown): val is Record<string, unknown> {
  return val !== null && typeof val === "object" && !Array.isArray(val);
}

function hasField(obj: Record<string, unknown>, field: string): boolean {
  return field in obj && obj[field] !== undefined && obj[field] !== null;
}

// ---------------------------------------------------------------------------
// Section validators
// ---------------------------------------------------------------------------

function checkTopLevel(file: string, data: Record<string, unknown>): LintResult[] {
  const results: LintResult[] = [];

  // ofd version must be a number
  if (!hasField(data, "ofd")) {
    results.push(error(file, "top/ofd", "Missing required field: ofd (version number)"));
  } else if (typeof data.ofd !== "number") {
    results.push(error(file, "top/ofd_type", `Field 'ofd' must be a number, got ${typeof data.ofd}`));
  }

  // source must be a string ending with .omni
  if (!hasField(data, "source")) {
    results.push(error(file, "top/source", "Missing required field: source"));
  } else if (typeof data.source !== "string") {
    results.push(error(file, "top/source_type", `Field 'source' must be a string, got ${typeof data.source}`));
  } else if (!String(data.source).endsWith(".omni")) {
    results.push(warn(file, "top/source_ext", `Field 'source' should reference a .omni file, got: "${data.source}"`));
  }

  // compiled must be a string (ISO timestamp)
  if (!hasField(data, "compiled")) {
    results.push(error(file, "top/compiled", "Missing required field: compiled (ISO timestamp)"));
  } else if (typeof data.compiled !== "string") {
    results.push(error(file, "top/compiled_type", `Field 'compiled' must be a string, got ${typeof data.compiled}`));
  }

  // Required object sections
  for (const section of ["identity", "state", "attribution"] as const) {
    if (!hasField(data, section)) {
      results.push(error(file, `top/${section}`, `Missing required section: ${section}`));
    } else if (!isObject(data[section])) {
      results.push(error(file, `top/${section}_type`, `Section '${section}' must be an object`));
    }
  }

  // children must be an array
  if (!hasField(data, "children")) {
    results.push(error(file, "top/children", "Missing required field: children (array)"));
  } else if (!Array.isArray(data.children)) {
    results.push(error(file, "top/children_type", `Field 'children' must be an array, got ${typeof data.children}`));
  }

  // Recommended top-level fields
  for (const field of RECOMMENDED_TOP) {
    if (!hasField(data, field)) {
      results.push(warn(file, `top/${field}`, `Missing recommended field: ${field}`));
    }
  }

  // Optional top-level fields
  for (const field of OPTIONAL_TOP) {
    if (!hasField(data, field)) {
      results.push(info(file, `top/${field}`, `Missing optional field: ${field}`));
    }
  }

  return results;
}

function checkIdentity(file: string, data: Record<string, unknown>): LintResult[] {
  const results: LintResult[] = [];
  const identity = data.identity;
  if (!isObject(identity)) return results; // Already reported in top-level check

  for (const field of REQUIRED_IDENTITY) {
    if (!hasField(identity, field)) {
      results.push(error(file, `identity/${field}`, `Missing required identity field: ${field}`));
    } else if (typeof identity[field] !== "string") {
      results.push(error(file, `identity/${field}_type`, `Field 'identity.${field}' must be a string`));
    }
  }

  for (const field of RECOMMENDED_IDENTITY) {
    if (!hasField(identity, field)) {
      results.push(warn(file, `identity/${field}`, `Missing recommended identity field: ${field}`));
    }
  }

  return results;
}

function checkState(file: string, data: Record<string, unknown>): LintResult[] {
  const results: LintResult[] = [];
  const state = data.state;
  if (!isObject(state)) return results;

  for (const field of REQUIRED_STATE) {
    if (!hasField(state, field)) {
      results.push(error(file, `state/${field}`, `Missing required state field: ${field}`));
    } else if (typeof state[field] !== "string") {
      results.push(error(file, `state/${field}_type`, `Field 'state.${field}' must be a string`));
    }
  }

  for (const field of RECOMMENDED_STATE) {
    if (!hasField(state, field)) {
      results.push(warn(file, `state/${field}`, `Missing recommended state field: ${field}`));
    }
  }

  return results;
}

function checkAttribution(file: string, data: Record<string, unknown>): LintResult[] {
  const results: LintResult[] = [];
  const attr = data.attribution;
  if (!isObject(attr)) return results;

  for (const field of REQUIRED_ATTRIBUTION) {
    if (!hasField(attr, field)) {
      results.push(error(file, `attribution/${field}`, `Missing required attribution field: ${field}`));
    } else if (typeof attr[field] !== "string") {
      results.push(error(file, `attribution/${field}_type`, `Field 'attribution.${field}' must be a string`));
    }
  }

  for (const field of RECOMMENDED_ATTRIBUTION) {
    if (!hasField(attr, field)) {
      results.push(info(file, `attribution/${field}`, `Missing optional attribution field: ${field}`));
    }
  }

  return results;
}

function checkChildren(file: string, data: Record<string, unknown>): LintResult[] {
  const results: LintResult[] = [];
  const children = data.children;
  if (!Array.isArray(children)) return results;

  if (children.length === 0) {
    results.push(warn(file, "children/empty", "Children array is empty — folder describes no contents"));
  }

  for (let i = 0; i < children.length; i++) {
    const child = children[i];
    if (!isObject(child)) {
      results.push(error(file, `children/${i}/type`, `Child entry ${i} must be an object`));
      continue;
    }

    if (!hasField(child, "path")) {
      results.push(error(file, `children/${i}/path`, `Child entry ${i} missing required 'path' field`));
    } else if (typeof child.path !== "string") {
      results.push(error(file, `children/${i}/path_type`, `Child entry ${i} 'path' must be a string`));
    }

    // type and description are recommended
    if (!hasField(child, "type")) {
      results.push(info(file, `children/${i}/child_type`, `Child entry ${i} missing 'type' field (file/dir)`));
    }

    if (!hasField(child, "description")) {
      results.push(info(file, `children/${i}/description`, `Child entry ${i} missing 'description' field`));
    }
  }

  return results;
}

function checkGrounding(file: string, data: Record<string, unknown>): LintResult[] {
  const results: LintResult[] = [];
  const grounding = data.grounding;
  if (!isObject(grounding)) return results; // Missing grounding already warned at top level

  for (const field of RECOMMENDED_GROUNDING) {
    if (!hasField(grounding, field)) {
      results.push(warn(file, `grounding/${field}`, `Missing recommended grounding field: ${field}`));
    }
  }

  // If scriptures exists, validate structure
  if (hasField(grounding, "scriptures")) {
    if (!Array.isArray(grounding.scriptures)) {
      results.push(error(file, "grounding/scriptures_type", "Field 'grounding.scriptures' must be an array"));
    } else {
      for (let i = 0; i < (grounding.scriptures as unknown[]).length; i++) {
        const s = (grounding.scriptures as unknown[])[i];
        if (!isObject(s)) {
          results.push(error(file, `grounding/scripture_${i}`, `Scripture entry ${i} must be an object`));
          continue;
        }
        if (!hasField(s, "text")) {
          results.push(warn(file, `grounding/scripture_${i}/text`, `Scripture ${i} missing 'text' field`));
        }
        if (!hasField(s, "reference")) {
          results.push(warn(file, `grounding/scripture_${i}/reference`, `Scripture ${i} missing 'reference' field`));
        }
      }
    }
  }

  return results;
}

function checkDependencies(file: string, data: Record<string, unknown>): LintResult[] {
  const results: LintResult[] = [];
  const deps = data.dependencies;
  if (!isObject(deps)) return results;

  for (const field of RECOMMENDED_DEPS) {
    if (!hasField(deps, field)) {
      results.push(warn(file, `dependencies/${field}`, `Missing recommended dependency field: ${field}`));
    }
  }

  if (hasField(deps, "provides") && !Array.isArray(deps.provides)) {
    results.push(error(file, "dependencies/provides_type", "Field 'dependencies.provides' must be an array"));
  }

  if (hasField(deps, "derives") && !Array.isArray(deps.derives)) {
    results.push(error(file, "dependencies/derives_type", "Field 'dependencies.derives' must be an array"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Lint orchestrator
// ---------------------------------------------------------------------------

async function lintOfdFile(filePath: string): Promise<LintResult[]> {
  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  if (text.trim().length === 0) {
    return [error(filePath, "parse/empty", "File is empty")];
  }

  // Parse JSON
  let data: unknown;
  try {
    data = JSON.parse(text);
  } catch (e) {
    return [error(filePath, "parse/json", `Invalid JSON: ${e}`)];
  }

  if (!isObject(data)) {
    return [error(filePath, "parse/root", "Root must be a JSON object")];
  }

  return [
    ...checkTopLevel(filePath, data),
    ...checkIdentity(filePath, data),
    ...checkState(filePath, data),
    ...checkAttribution(filePath, data),
    ...checkChildren(filePath, data),
    ...checkGrounding(filePath, data),
    ...checkDependencies(filePath, data),
  ];
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// Registration — plug into the registry
// ---------------------------------------------------------------------------

const ofdHandler: FormatHandler = {
  name: "ofd",
  description:
    "OmniFold compiled folder descriptions — fixed JSON schema from root.omni compilation",
  extensions: [".ofd"],
  basenames: ["root.ofd"],
  maxDepth: 10,
  lint: lintOfdFile,
};

registerFormat(ofdHandler);

export default ofdHandler;
