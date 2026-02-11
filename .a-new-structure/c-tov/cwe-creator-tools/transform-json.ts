#!/usr/bin/env -S deno run --allow-read --allow-write
// ============================================================================
// METADATA
// ============================================================================
//
// file:    transform-json.ts
// key:     B-tools-transform-json
// title:   JSON/JSONC 3-Block Alignment Transformer
// type:    Tool (Transformer)
// version: a-01.00
// created: 2026-02-10
// updated: 2026-02-10
// authors: Nova Dawn (CPI-SI)
// purpose: Wrap raw JSON/JSONC data files in proper OmniCode 3-block structure.
//          Detects format from extension, auto-generates metadata from path,
//          preserves domain data verbatim in the content block.
//
// usage:   deno run --allow-read --allow-write tools/transform-json.ts <file> [--dry-run] [--verbose]
//
// note:    All 55 existing JSON/JSONC files are already aligned.
//          This tool is for wrapping NEW raw data files in 3-block structure.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

const DRY_RUN = Deno.args.includes("--dry-run");
const VERBOSE = Deno.args.includes("--verbose");
const TODAY = new Date().toISOString().split("T")[0];

// ─────────────────────────────────────────────────────────────────────────────
// Types
// ─────────────────────────────────────────────────────────────────────────────

interface FileInfo {
  filename: string;
  basename: string;
  isJsonc: boolean;
  key: string;
  title: string;
  layer: string;
  component: string;
}

// ============================================================================
// BODY
// ============================================================================

// ─────────────────────────────────────────────────────────────────────────────
// Check if file already has 3-block structure
// ─────────────────────────────────────────────────────────────────────────────

function isAlreadyAligned(content: string): boolean {
  return (
    content.includes("_pragma") ||
    content.includes("_P1_CORE") ||
    content.includes("_P1_key") ||
    content.includes("_M1_identity")
  );
}

// ─────────────────────────────────────────────────────────────────────────────
// Derive file info from path
// ─────────────────────────────────────────────────────────────────────────────

function deriveFileInfo(filepath: string): FileInfo {
  const parts = filepath.replace(/\\/g, "/").split("/");
  const filename = parts[parts.length - 1];
  const basename = filename.replace(/\.(json|jsonc)$/, "");
  const isJsonc = filename.endsWith(".jsonc");

  // Derive key from path segments (last 3-4 meaningful dirs + filename)
  const meaningful = parts.filter(
    (p) => p !== "." && p !== ".." && p !== "json" && p !== "jsonc" && !p.includes("staging"),
  );
  const keyParts = meaningful.slice(-3).map((p) => p.replace(/\.(json|jsonc)$/, ""));
  const key = `B-${keyParts.join("-")}`.replace(/[^a-zA-Z0-9-]/g, "-");

  // Title from filename
  const title = basename
    .split(/[-_]/)
    .map((w) => w.charAt(0).toUpperCase() + w.slice(1))
    .join(" ");

  // Layer/component from directory
  const layer = meaningful.length > 2 ? meaningful[meaningful.length - 3] : "unknown";
  const component = meaningful.length > 1 ? meaningful[meaningful.length - 2] : "data";

  return { filename, basename, isJsonc, key, title, layer, component };
}

// ─────────────────────────────────────────────────────────────────────────────
// Extract domain data keys from parsed JSON
// ─────────────────────────────────────────────────────────────────────────────

function extractDomainKeys(data: Record<string, unknown>): string[] {
  return Object.keys(data).filter((k) => !k.startsWith("_") && k !== "$schema");
}

// ─────────────────────────────────────────────────────────────────────────────
// Generate JSONC 3-block wrapper
// ─────────────────────────────────────────────────────────────────────────────

function generateJsonc(info: FileInfo, domainData: Record<string, unknown>): string {
  const domainKeys = extractDomainKeys(domainData);
  const indent = "    ";
  const sep = "═══════════════════════════════════════════════════════════════";
  const sub = "─────────────────────────────────────────────────────────────";

  const obj: Record<string, unknown> = {};

  // Helper to add comment lines (JSONC only)
  const lines: string[] = ["{"];

  const addComment = (text: string) => {
    lines.push(`${indent}// ${text}`);
  };
  const addBlank = () => {
    lines.push("");
  };
  const addKV = (key: string, value: unknown) => {
    const json = JSON.stringify(value);
    lines.push(`${indent}${JSON.stringify(key)}: ${json},`);
  };

  // ═══ PRAGMA ═══
  addComment(`#!omni data --jsonc`);
  addBlank();
  addComment(sep);
  addComment("OMNICODE PRAGMA [PRAGMA]");
  addComment(sep);
  addBlank();
  addKV("_pragma", sep);
  addKV("_pragma_tag", "OMNICODE PRAGMA [PRAGMA]");
  addKV("_pragma_format", "@omni data --jsonc");
  addBlank();

  addComment("P.1 CORE — Identity");
  addKV("_P1_CORE", sub);
  addKV("_P1_tag", "P.1 CORE — Identity [CORE]");
  addKV("_P1_key", info.key);
  addKV("_P1_from", "b-word/seed/data/toml/config.toml");
  addKV("_P1_at", "b-01.00");
  addBlank();

  addComment("P.2 FAMILY — Classification");
  addKV("_P2_FAMILY", sub);
  addKV("_P2_tag", "P.2 FAMILY — Classification [FAMILY]");
  addKV("_P2_type", "data");
  addKV("_P2_subtype", info.component);
  addKV("_P2_role", "content");
  addKV("_P2_structure", "3-block");
  addBlank();

  addComment("P.3 INSTANCE — File Details");
  addKV("_P3_INSTANCE", sub);
  addKV("_P3_tag", "P.3 INSTANCE — File Details [INSTANCE]");
  addKV("_P3_component", info.component);
  addKV("_P3_layer", info.layer);
  addKV("_P3_includes", "none");
  addKV("_P3_provides", [`${info.basename.toUpperCase().replace(/-/g, "_")}_DATA`]);
  addBlank();

  addComment("P.4 ARCHITECTURE — Scaling Context");
  addKV("_P4_ARCH", sub);
  addKV("_P4_tag", "P.4 ARCHITECTURE — Scaling Context [ARCH]");
  addKV("_P4_layers", "L0:C, L1:C, L2:C, L3:Go, L4:Go/Rust, L5:C#");
  addKV("_P4_anchor", "genesis_1_1 -> void(0) -> unity(1) -> derivation");
  addBlank();

  addComment("P.5 SUMMARY — Human Identity");
  addKV("_P5_SUMMARY", sub);
  addKV("_P5_tag", "P.5 SUMMARY — Human Identity [SUMMARY]");
  addKV("_P5_title", info.title);
  addKV("_P5_brief", `${info.title} — ${info.component} data`);
  addBlank();

  addComment(sep);
  addComment("END PRAGMA [END]");
  addComment(sep);
  addBlank();
  addKV("_pragma_end", sep);
  addKV("_pragma_end_tag", "END PRAGMA [END]");
  addBlank();

  // ═══ METADATA ═══
  addComment(sep);
  addComment("METADATA BLOCK [METADATA]");
  addComment(sep);
  addBlank();
  addKV("_metadata_start", sep);
  addKV("_metadata_tag", "METADATA BLOCK [METADATA]");
  addBlank();

  addComment("M.1 IDENTITY");
  addKV("_M1_identity", {
    file: info.filename,
    key: info.key,
    title: info.title,
    type: "Data",
    component: info.component,
    role: `${info.title} data file`,
  });
  addBlank();

  addComment("M.2 STATE");
  addKV("_M2_state", {
    version: "b-01.00",
    status: "Active",
    created: TODAY,
    updated: TODAY,
  });
  addBlank();

  addComment("M.3 ATTRIBUTION");
  addKV("_M3_attribution", {
    architect: "Seanje Lenox-Wise",
    implementation: "Nova Dawn (CPI-SI)",
    organization: "CreativeWorkzStudio LLC",
    copyright: `© 2025-2026 CreativeWorkzStudio LLC. All rights reserved.`,
  });
  addBlank();

  addComment("M.4 GROUNDING");
  addKV("_M4_grounding", {
    scripture: "In the beginning God created the heaven and the earth. — Genesis 1:1",
    principle: "Every construct traces to Scripture.",
    anchor: "Genesis 1:1 — In the beginning God created the heaven and the earth",
  });
  addBlank();

  addComment("M.5 DEPENDENCIES");
  addKV("_M5_dependencies", {
    requires: [],
    consumers: ["parsers", "code generators"],
    derives_from: "b-word/seed/data/toml/config.toml",
    related_data: {
      role: `${info.title} in JSONC.`,
      toml: "None",
      relationship: "JSONC defines domain data.",
    },
  });
  addBlank();

  addComment(sep);
  addComment("END METADATA [END]");
  addComment(sep);
  addBlank();
  addKV("_metadata_end", sep);
  addKV("_metadata_end_tag", "END METADATA [END]");
  addBlank();

  // ═══ CONTENT ═══
  addComment(sep);
  addComment("CONTENT BLOCK [CONTENT]");
  addComment(sep);
  addBlank();
  addKV("_content_start", sep);
  addKV("_content_tag", "CONTENT BLOCK [CONTENT]");
  addBlank();

  addComment("Cc — Content Configurations");
  addKV("_Cc", sub);
  addKV("_Cc_tag", "Cc — Content Configurations");
  addKV("_Cc_format", "JSONC — UTF-8, LF line endings");
  addKV("_Cc_key_convention", "lowercase_with_underscores or camelCase");
  addBlank();

  // Domain data — insert as individual keys
  addComment("─────────────────────────────────────────────────────────────");
  addComment("DOMAIN DATA");
  addComment("─────────────────────────────────────────────────────────────");
  addBlank();

  for (const key of domainKeys) {
    const val = JSON.stringify(domainData[key], null, 4);
    // For objects/arrays, use multi-line formatting
    if (typeof domainData[key] === "object" && domainData[key] !== null) {
      const formatted = val.split("\n").map((line, i) => (i === 0 ? line : indent + line)).join("\n");
      lines.push(`${indent}${JSON.stringify(key)}: ${formatted},`);
    } else {
      addKV(key, domainData[key]);
    }
  }
  addBlank();

  addComment("Co — Content Operations");
  addKV("_Co", sub);
  addKV("_Co_tag", "Co — Content Operations");
  addBlank();

  addComment("Cv — Content Validation");
  addKV("_Cv", sub);
  addKV("_Cv_tag", "Cv — Content Validation");
  addKV("_Cv_format", info.filename);
  addKV("_Cv_validate", `jsonlint ${info.filename}`);
  addBlank();

  addComment(sep);
  addComment("END CONTENT [END]");
  addComment(sep);
  addBlank();
  addKV("_content_end", sep);
  addKV("_content_end_tag", "END CONTENT [END]");
  addBlank();

  // ═══ CLOSING ═══
  addComment(sep);
  addComment("CLOSING BLOCK [CLOSING]");
  addComment(sep);
  addBlank();
  addKV("_closing_start", sep);
  addKV("_closing_tag", "CLOSING BLOCK [CLOSING]");
  addBlank();

  addComment("X.1 POLICY");
  addKV("_X1_policy", {
    scripture: "Remove not the ancient landmark, which thy fathers have set. — Proverbs 22:28",
    never: ["3-block structure", "Scripture references"],
    careful: ["Adding new domain keys — maintain consistency"],
    safe: ["Comments for clarity", "Additional domain data"],
  });
  addBlank();

  addComment("X.2 EXTENSION");
  addKV("_X2_extension", {
    adding_data: [
      "1. Add new entries in the domain data section",
      "2. Ensure key naming follows conventions",
      "3. Run JSON validation",
    ],
  });
  addBlank();

  addComment("X.3 TROUBLESHOOTING");
  addKV("_X3_troubleshooting", {
    invalid_json: "Check for trailing commas, missing quotes",
    missing_key: "Verify all required domain keys present",
  });
  addBlank();

  addComment("X.4 REFERENCE");
  addKV("_X4_reference", {
    related_files: [],
    required_sections: ["_pragma", "_metadata", "_content", "_closing"],
    validate: `jsonlint ${info.filename}`,
  });
  addBlank();

  addComment("X.5 NOTE");
  addKV("_X5_note", {
    note: `${info.title} — part of the OmniCode system.`,
    scripture: "In the beginning God created the heaven and the earth. — Genesis 1:1",
    anchor: "Genesis 1:1 anchors everything.",
  });
  addBlank();

  addComment(sep);
  addComment("END CLOSING [END]");
  addComment(sep);
  addBlank();
  addKV("_closing_end", sep);

  // Remove trailing comma from last key-value
  const lastKVIndex = lines.length - 1;
  lines[lastKVIndex] = lines[lastKVIndex].replace(/,\s*$/, "");

  lines.push("}");
  lines.push("");

  return lines.join("\n");
}

// ─────────────────────────────────────────────────────────────────────────────
// Generate strict JSON 3-block wrapper (no comments)
// ─────────────────────────────────────────────────────────────────────────────

function generateJson(info: FileInfo, domainData: Record<string, unknown>): string {
  const domainKeys = extractDomainKeys(domainData);
  const sep = "═══════════════════════════════════════════════════════════════";
  const sub = "─────────────────────────────────────────────────────────────";

  // Build the full 3-block object
  const output: Record<string, unknown> = {
    // PRAGMA
    _pragma: sep,
    _pragma_tag: "OMNICODE PRAGMA [PRAGMA]",
    _pragma_format: "@omni data --json",
    _P1_CORE: sub,
    _P1_key: info.key,
    _P1_from: "b-word/seed/data/toml/config.toml",
    _P1_at: "b-01.00",
    _P2_FAMILY: sub,
    _P2_type: "data",
    _P2_subtype: info.component,
    _P2_role: "content",
    _P2_structure: "3-block",
    _P3_INSTANCE: sub,
    _P3_component: info.component,
    _P3_layer: info.layer,
    _P5_SUMMARY: sub,
    _P5_title: info.title,
    _P5_brief: `${info.title} data`,
    _pragma_end: sep,

    // METADATA
    _metadata_start: sep,
    _M1_identity: {
      file: info.filename,
      key: info.key,
      title: info.title,
      type: "Data",
      component: info.component,
    },
    _M2_state: { version: "b-01.00", status: "Active", created: TODAY, updated: TODAY },
    _M3_attribution: {
      architect: "Seanje Lenox-Wise",
      implementation: "Nova Dawn (CPI-SI)",
      organization: "CreativeWorkzStudio LLC",
    },
    _M4_grounding: {
      scripture: "In the beginning God created the heaven and the earth. — Genesis 1:1",
      anchor: "Genesis 1:1",
    },
    _M5_dependencies: {
      requires: [],
      consumers: ["parsers"],
      related_data: { role: `${info.title} in JSON.` },
    },
    _metadata_end: sep,

    // CONTENT
    _content_start: sep,
    _Cc: sub,
    _Cc_format: "JSON — UTF-8, LF line endings",
  };

  // Add domain data
  for (const key of domainKeys) {
    output[key] = domainData[key];
  }

  // Content operations and validation
  output._Co = sub;
  output._Cv = sub;
  output._Cv_validate = `jsonlint ${info.filename}`;
  output._content_end = sep;

  // CLOSING
  output._closing_start = sep;
  output._X1_policy = {
    scripture: "Remove not the ancient landmark. — Proverbs 22:28",
    never: ["3-block structure"],
    safe: ["Additional domain data"],
  };
  output._X5_note = {
    note: `${info.title} — OmniCode system data.`,
    anchor: "Genesis 1:1 anchors everything.",
  };
  output._closing_end = sep;

  return JSON.stringify(output, null, 4) + "\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Transform a single file
// ─────────────────────────────────────────────────────────────────────────────

async function transformFile(filepath: string): Promise<boolean> {
  try {
    const content = await Deno.readTextFile(filepath);

    if (isAlreadyAligned(content)) {
      if (VERBOSE) console.log(`  SKIP  ${filepath} — already aligned`);
      return true;
    }

    // Strip JSONC comments for parsing
    const jsonContent = content.replace(/\/\/.*$/gm, "").replace(/\/\*[\s\S]*?\*\//g, "");

    let data: Record<string, unknown>;
    try {
      data = JSON.parse(jsonContent);
    } catch (e) {
      console.log(`  FAIL  ${filepath} — invalid JSON: ${(e as Error).message}`);
      return false;
    }

    const info = deriveFileInfo(filepath);

    if (VERBOSE) {
      const domainKeys = extractDomainKeys(data);
      console.log(`  Parsed: ${info.filename} (${info.isJsonc ? "JSONC" : "JSON"})`);
      console.log(`  Key: ${info.key}, Title: "${info.title}"`);
      console.log(`  Domain keys: ${domainKeys.join(", ")}`);
    }

    // Generate 3-block aligned output
    const aligned = info.isJsonc ? generateJsonc(info, data) : generateJson(info, data);

    // Validate (strip comments for JSONC, then parse)
    try {
      const stripped = aligned.replace(/\/\/.*$/gm, "").replace(/\/\*[\s\S]*?\*\//g, "");
      JSON.parse(stripped);
    } catch (e) {
      console.log(`  FAIL  ${filepath} — generated JSON invalid: ${(e as Error).message}`);
      return false;
    }

    const originalLines = content.split("\n").length;
    const alignedLines = aligned.split("\n").length;

    if (DRY_RUN) {
      console.log(`  WOULD  ${filepath}  ${originalLines} → ${alignedLines} lines`);
    } else {
      await Deno.writeTextFile(filepath, aligned);
      console.log(`  OK  ${filepath}  ${originalLines} → ${alignedLines} lines`);
    }

    return true;
  } catch (e) {
    console.log(`  FAIL  ${filepath} — ${(e as Error).message}`);
    return false;
  }
}

// ============================================================================
// CLOSING — Main execution
// ============================================================================

async function main() {
  const args = Deno.args.filter((a) => !a.startsWith("--"));

  if (args.length === 0) {
    console.log("Usage: transform-json.ts <file-or-dir> [--dry-run] [--verbose]");
    console.log("");
    console.log("Wraps raw JSON/JSONC data files in OmniCode 3-block structure.");
    console.log("Detects format (.json vs .jsonc) and auto-generates metadata.");
    console.log("Skips files that are already aligned.");
    console.log("");
    console.log("Options:");
    console.log("  --dry-run    Show what would change without writing");
    console.log("  --verbose    Show detailed parsing info");
    Deno.exit(1);
  }

  const target = args[0];
  let files: string[] = [];

  try {
    const stat = await Deno.stat(target);
    if (stat.isDirectory) {
      for await (const entry of Deno.readDir(target)) {
        if (entry.isFile && (entry.name.endsWith(".json") || entry.name.endsWith(".jsonc"))) {
          files.push(`${target}/${entry.name}`);
        }
      }
      files.sort();
    } else {
      files = [target];
    }
  } catch {
    console.log(`Error: Cannot access ${target}`);
    Deno.exit(1);
  }

  console.log(`Transform JSON/JSONC files → 3-block aligned format`);
  console.log(`Mode: ${DRY_RUN ? "DRY RUN (no writes)" : "LIVE (will overwrite files)"}`);
  console.log(`Files: ${files.length}`);
  console.log(`─────────────────────────────────────────────────────────────`);

  let success = 0;
  let failed = 0;
  let skipped = 0;

  for (const file of files) {
    const content = await Deno.readTextFile(file);
    if (isAlreadyAligned(content)) {
      if (VERBOSE) console.log(`  SKIP  ${file}`);
      skipped++;
      continue;
    }

    const ok = await transformFile(file);
    if (ok) success++;
    else failed++;
  }

  console.log(`─────────────────────────────────────────────────────────────`);
  console.log(`${success} transformed, ${skipped} skipped, ${failed} failed`);

  if (failed > 0) Deno.exit(1);
}

main();
