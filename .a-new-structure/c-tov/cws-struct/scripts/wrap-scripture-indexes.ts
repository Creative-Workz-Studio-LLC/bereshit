#!/usr/bin/env -S deno run --allow-read --allow-write
// ============================================================================
// METADATA
// ============================================================================
//
// file:    scripts/wrap-scripture-indexes.ts
// key:     B-tov-cws-struct-scripts-wrap-scripture-indexes
// title:   Wrap Scripture Index JSON Files in 3-Block Structure
// type:    Code (Script)
// version: a-01.00
// created: 2026-02-13
// authors: Nova Dawn (CPI-SI)
// purpose: One-time transform — wraps 12 bare scripture index JSON files in
//          proper OmniCode 3-block structure. Data goes in Co section.
//          Files are pure JSON (no comments), so all structure lives in _ keys.
//
// biblical_foundation: "In the beginning God created the heaven and the earth."
//   — Genesis 1:1. Structure before content, always.
//
// usage:   deno run --allow-read --allow-write scripts/wrap-scripture-indexes.ts [--dry-run]
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

const SEP_DOUBLE = "═══════════════════════════════════════════════════════════════";
const SEP_SINGLE = "─────────────────────────────────────────────────────────────";

const BASE_DIR = new URL(
  "../../../b-word/data/scripture/indexes",
  import.meta.url,
).pathname;

// Scripture index metadata by path pattern
interface IndexMeta {
  translation: string;
  translationFull: string;
  indexType: string;
  indexTypeFull: string;
  dataKey: string;
  provides: string;
  brief: string;
  role: string;
}

// ============================================================================
// BODY
// ============================================================================

function deriveMetadata(filePath: string): IndexMeta | null {
  // Path: .../indexes/{translation}/{type}/{translation}-{type}.json
  const parts = filePath.split("/");
  const fileName = parts[parts.length - 1]!;
  const type = parts[parts.length - 2]!;
  const translation = parts[parts.length - 3]!;

  const translationMap: Record<string, string> = {
    kjv: "King James Version",
    parallel: "Parallel (KJV + WEB)",
    web: "World English Bible",
  };

  const typeMap: Record<string, { full: string; dataKey: string; brief: string }> = {
    ascii: {
      full: "ASCII Key",
      dataKey: "entries",
      brief: "Maps ASCII values to ternary cube positions for scripture lookup",
    },
    cube: {
      full: "Cube Index",
      dataKey: "cube",
      brief: "3D cube navigation index — verses mapped to ternary spatial coordinates",
    },
    ordinal: {
      full: "Ordinal Index",
      dataKey: "groups",
      brief: "Sequential verse index — ordinal positions grouped for linear traversal",
    },
    scripture: {
      full: "Scripture Key",
      dataKey: "entries",
      brief: "Full scripture text with verse metadata — the primary lookup table",
    },
  };

  const tMeta = translationMap[translation];
  const iMeta = typeMap[type];
  if (!tMeta || !iMeta) return null;

  return {
    translation,
    translationFull: tMeta,
    indexType: type,
    indexTypeFull: iMeta.full,
    dataKey: iMeta.dataKey,
    provides: `${translation.toUpperCase()}_${type.toUpperCase()}_INDEX`,
    brief: `${tMeta} — ${iMeta.brief}`,
    role: `${iMeta.full} for ${tMeta} scripture data`,
  };
}

function buildWrapper(
  meta: IndexMeta,
  filePath: string,
  // deno-lint-ignore no-explicit-any
  originalData: Record<string, any>,
): Record<string, unknown> {
  const fileName = filePath.split("/").pop()!;
  const relPath = filePath.includes("b-word/")
    ? filePath.slice(filePath.indexOf("b-word/"))
    : filePath;
  const key = `B-word-data-scripture-indexes-${meta.translation}-${meta.indexType}`;
  const title = `${meta.translationFull} ${meta.indexTypeFull}`;

  // Build the 3-block object with proper key ordering
  const wrapped: Record<string, unknown> = {};

  // ── PRAGMA ──
  wrapped["_pragma"] = SEP_DOUBLE;
  wrapped["_pragma_tag"] = "OMNICODE PRAGMA [PRAGMA]";
  wrapped["_pragma_format"] = "@omni data --json";
  wrapped["_pragma_legend"] = {
    _req: "REQUIRED — must exist, validation fails without",
    _inh: "INHERITED — from template, override allowed",
    _ins: "INSTANCE — file-specific, unique values",
  };

  wrapped["_P1_CORE"] = SEP_SINGLE;
  wrapped["_P1_tag"] = "P.1 CORE — Identity [CORE]";
  wrapped["_P1_key"] = key;
  wrapped["_P1_from"] = "b-word/seed/data/json/data.json";
  wrapped["_P1_at"] = "a-01.00";

  wrapped["_P2_FAMILY"] = SEP_SINGLE;
  wrapped["_P2_tag"] = "P.2 FAMILY — Classification [FAMILY]";
  wrapped["_P2_type"] = "data";
  wrapped["_P2_subtype"] = "scripture-index";
  wrapped["_P2_role"] = "ladder";
  wrapped["_P2_structure"] = "3-block";

  wrapped["_P3_INSTANCE"] = SEP_SINGLE;
  wrapped["_P3_tag"] = "P.3 INSTANCE — File Details [INSTANCE]";
  wrapped["_P3_component"] = "scripture indexes";
  wrapped["_P3_layer"] = relPath.replace(`/${fileName}`, "");
  wrapped["_P3_includes"] = "none";
  wrapped["_P3_provides"] = [meta.provides];

  wrapped["_P4_ARCH"] = SEP_SINGLE;
  wrapped["_P4_tag"] = "P.4 ARCHITECTURE — Scaling Context [ARCH]";
  wrapped["_P4_layers"] = "L0:C, L1:C, L2:C, L3:Go, L4:Go/Rust, L5:C#";
  wrapped["_P4_anchor"] = "genesis_1_1 -> void(0) -> unity(1) -> derivation";

  wrapped["_P5_SUMMARY"] = SEP_SINGLE;
  wrapped["_P5_tag"] = "P.5 SUMMARY — Human Identity [SUMMARY]";
  wrapped["_P5_title"] = title;
  wrapped["_P5_brief"] = meta.brief;

  wrapped["_pragma_end"] = SEP_DOUBLE;
  wrapped["_pragma_end_tag"] = "END PRAGMA [END]";

  // ── METADATA ──
  wrapped["_metadata_start"] = SEP_DOUBLE;
  wrapped["_metadata_tag"] = "METADATA BLOCK [METADATA]";

  wrapped["_M1_identity"] = {
    file: fileName,
    key: key,
    title: title,
    type: "Data (Index)",
    component: "Scripture Indexes",
    role: meta.role,
  };

  wrapped["_M2_state"] = {
    version: "a-01.00",
    status: "Active",
    created: "2026-01-01",
    updated: "2026-02-13",
  };

  wrapped["_M3_attribution"] = {
    architect: "Seanje Lenox-Wise",
    implementation: "Nova Dawn (CPI-SI)",
  };

  wrapped["_M4_grounding"] = {
    scripture:
      "Thy word is a lamp unto my feet, and a light unto my path. — Psalm 119:105",
    principle:
      "Scripture data structured for navigation — every verse findable, every position meaningful.",
  };

  wrapped["_M5_dependencies"] = {
    requires: "JSON parser",
    consumers: "Scripture lookup engine, cube navigation, verse indexing",
    cross_format: {
      role: "Generated index data for scripture navigation system",
      toml_source:
        "Generated from scripture text files, not derived from TOML",
      relationship:
        "Pure data index — consumed by L3+ systems for scripture access",
    },
  };

  wrapped["_M_extension_note"] = {
    required: [
      "_M1_identity",
      "_M2_state",
      "_M3_attribution",
      "_M4_grounding",
      "_M5_dependencies",
    ],
    extension:
      "M6+ available for domain-specific metadata in derived files",
    rule: "M1-M5 are the base contract.",
  };

  wrapped["_metadata_end"] = SEP_DOUBLE;
  wrapped["_metadata_end_tag"] = "END METADATA [END]";

  // ── CONTENT ──
  wrapped["_content_start"] = SEP_DOUBLE;
  wrapped["_content_tag"] = "CONTENT BLOCK [CONTENT]";

  wrapped["_Cc"] = SEP_SINGLE;
  wrapped["_Cc_tag"] = "Cc — Content Configurations [CONFIG]";
  wrapped["_Cc_format_note"] =
    "JSON strict — no comments, no trailing commas, UTF-8, LF line endings.";
  wrapped["_Cc_encoding"] = "utf-8";
  wrapped["_Cc_key_convention"] =
    "lowercase_with_underscores for data keys, _prefix for documentation fields";

  wrapped["_Co"] = SEP_SINGLE;
  wrapped["_Co_tag"] = "Co — Content Operations [OPS]";

  // ── The actual data goes here ──
  for (const [k, v] of Object.entries(originalData)) {
    wrapped[k] = v;
  }

  wrapped["_Cv"] = SEP_SINGLE;
  wrapped["_Cv_tag"] = "Cv — Content Validation [VALIDATE]";
  wrapped["_Cv_verify"] = `python -m json.tool ${fileName}`;
  wrapped["_Cv_expected_state"] =
    "Valid JSON with all pragma, metadata, content, and closing blocks present.";

  wrapped["_content_end"] = SEP_DOUBLE;
  wrapped["_content_end_tag"] = "END CONTENT [END]";

  // ── CLOSING ──
  wrapped["_closing_start"] = SEP_DOUBLE;
  wrapped["_closing_tag"] = "CLOSING BLOCK [CLOSING]";
  wrapped["_closing_structure"] =
    "X.1 Policy → X.4 Reference → X.5 Note";

  wrapped["_X1"] = SEP_SINGLE;
  wrapped["_X1_tag"] = "X.1 POLICY — Modification Guidelines [POLICY]";
  wrapped["_X1_scripture"] =
    "Remove not the ancient landmark, which thy fathers have set. — Proverbs 22:28";
  wrapped["_X1_never"] = [
    "Remove or reorder index entries",
    "Break 3-block structure",
    "Modify generated data by hand (regenerate from source)",
  ];
  wrapped["_X1_careful"] = [
    "Regenerating indexes (verify all translations consistent)",
    "Changing entry schema fields (breaks consuming code)",
  ];
  wrapped["_X1_safe"] = [
    "Adding _ documentation fields",
    "Updating metadata versions after regeneration",
  ];

  wrapped["_X4"] = SEP_SINGLE;
  wrapped["_X4_tag"] = "X.4 REFERENCE — Quick Lookup [REFERENCE]";
  wrapped["_X4_related_files"] = [
    "b-word/data/scripture/indexes/ — All scripture indexes",
    "b-word/seed/data/json/data.json — JSON 3-block template",
    "b-word/definitions/omni/json-3block-schema.jsonc — Validation schema",
  ];
  wrapped["_X4_validate"] = `python -m json.tool ${fileName}`;

  wrapped["_X5"] = SEP_SINGLE;
  wrapped["_X5_tag"] = "X.5 NOTE — Final Guidance [NOTE]";
  wrapped["_X5_note"] = `${title} — generated index data wrapped in 3-block structure. Structure is the skeleton; the data is the flesh.`;
  wrapped["_X5_scripture"] =
    "In the beginning God created the heaven and the earth. — Genesis 1:1";
  wrapped["_X5_anchor"] =
    "Structure before content. Boundaries before data.";

  wrapped["_closing_end"] = SEP_DOUBLE;
  wrapped["_closing_end_tag"] = "END CLOSING [END]";

  return wrapped;
}

async function processFile(
  filePath: string,
  dryRun: boolean,
): Promise<boolean> {
  const meta = deriveMetadata(filePath);
  if (!meta) {
    console.error(`  Cannot derive metadata for: ${filePath}`);
    return false;
  }

  const text = await Deno.readTextFile(filePath);
  const data = JSON.parse(text);

  // Check if already wrapped
  if (data["_pragma"]) {
    console.log(`  SKIP (already wrapped): ${filePath.split("/").pop()}`);
    return true;
  }

  const wrapped = buildWrapper(meta, filePath, data);

  if (dryRun) {
    const keys = Object.keys(wrapped);
    const dataKeys = Object.keys(data);
    console.log(
      `  DRY-RUN: ${filePath.split("/").pop()} — would add ${keys.length - dataKeys.length} structural keys around ${dataKeys.length} data key(s) [${dataKeys.join(", ")}]`,
    );
    return true;
  }

  const output = JSON.stringify(wrapped, null, 4) + "\n";
  await Deno.writeTextFile(filePath, output);
  console.log(
    `  WRAPPED: ${filePath.split("/").pop()} (${(output.length / 1024 / 1024).toFixed(1)}MB)`,
  );
  return true;
}

// ============================================================================
// CLOSING
// ============================================================================

async function main(): Promise<void> {
  const dryRun = Deno.args.includes("--dry-run");

  console.log(
    `\nWrapping scripture index files in 3-block structure${dryRun ? " (dry run)" : ""}...\n`,
  );

  const translations = ["kjv", "parallel", "web"];
  const types = ["ascii", "cube", "ordinal", "scripture"];
  let total = 0;
  let success = 0;

  for (const trans of translations) {
    for (const type of types) {
      const fileName = `${trans}-${type === "ascii" ? "ascii-key" : type === "scripture" ? "scripture-key" : `${type}-index`}.json`;
      const filePath = `${BASE_DIR}/${trans}/${type}/${fileName}`;

      try {
        total++;
        if (await processFile(filePath, dryRun)) {
          success++;
        }
      } catch (e) {
        console.error(
          `  ERROR: ${fileName} — ${e instanceof Error ? e.message : e}`,
        );
      }
    }
  }

  console.log(`\nDone: ${success}/${total} files processed.`);
}

main();
