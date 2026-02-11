#!/usr/bin/env -S deno run --allow-read --allow-write
// ============================================================================
// METADATA
// ============================================================================
//
// file:    transform-bible.ts
// key:     B-tools-transform-bible
// title:   Bible TOML 3-Block Alignment Transformer
// type:    Tool (Transformer)
// version: a-01.00
// created: 2026-02-10
// updated: 2026-02-10
// authors: Nova Dawn (CPI-SI)
// purpose: Transform old comment-based bible TOML files into proper OmniCode
//          3-block aligned format. Preserves domain content verbatim while
//          wrapping in proper [_pragma], [_metadata], [_content], [_validation],
//          [_closing] infrastructure.
//
// usage:   deno run --allow-read --allow-write tools/transform-bible.ts [--dry-run]
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { parse as parseToml } from "jsr:@std/toml";

const BIBLE_DIR = "toml/.staging/L0/bible";
const DRY_RUN = Deno.args.includes("--dry-run");

// ─────────────────────────────────────────────────────────────────────────────
// Bible file metadata — extracted from comment-based metadata in each file
// ─────────────────────────────────────────────────────────────────────────────

interface BibleFileMeta {
  file: string;
  key: string;
  title: string;
  type: string;
  role: string;
  component: string;
  scripture: string;
  principle: string;
  anchor: string;
  created: string;
  requires: string[];
  consumers: string[];
  cross_format_role: string;
  domain_tables: string[];   // required domain tables for schema validation
  never: string[];
  safe: string[];
  careful: string[];
  closing_scripture: string;
  closing_anchor: string;
}

const FILES: BibleFileMeta[] = [
  {
    file: "encoding.toml",
    key: "B-L0-bible-encoding",
    title: "Bible Encoding Map",
    type: "Data (Bible Encoding)",
    role: "Encoding map — Scripture concepts to OmniCode constructs (Hebrew roots, Greek terms, creation/covenant/genealogy patterns, number symbolism)",
    component: "Foundation",
    scripture: "All scripture is given by inspiration of God, and is profitable for doctrine, for reproof, for correction, for instruction in righteousness. — 2 Timothy 3:16",
    principle: "The Bible is the codex. This file is the Rosetta Stone between Scripture and code.",
    anchor: "Genesis 1:1 — In the beginning God created the heaven and the earth",
    created: "2025-12-11",
    requires: ["b-word/scripture/KJV/", "b-word/scripture/WEB/", "b-word/glossary/biblical/"],
    consumers: ["b-word/L0-universal/bible/decoding.toml", "code generators", "documentation"],
    cross_format_role: "Encoding RULES in TOML. No JSONC/JSON counterpart.",
    domain_tables: ["hebrew", "greek", "creation", "covenant", "genealogy", "numbers"],
    never: [
      "Hebrew roots — linguistically fixed",
      "Greek terms — linguistically fixed",
      "Scripture references — canonical",
      "Fundamental mappings (bara → declaration, etc.)",
    ],
    safe: [
      "New Hebrew roots (with scholarly verification)",
      "New Greek terms (with scholarly verification)",
      "Additional biblical patterns",
      "Additional number symbolism",
      "Notes and clarifications",
    ],
    careful: [
      "Changing omnicode_mapping — affects keywords.toml, syntax.toml",
      "Adding maps_to values — affects what keywords derive from",
      "Modifying fundamental patterns — affects all syntax",
    ],
    closing_scripture: "All scripture is given by inspiration of God. — 2 Timothy 3:16",
    closing_anchor: "The Bible is the source code. Every OmniCode construct traces to Scripture. Genesis 1:1 anchors the entire encoding.",
  },
  {
    file: "decoding.toml",
    key: "B-L0-bible-decoding",
    title: "Bible Decoding Map",
    type: "Data (Bible Decoding)",
    role: "Reverse lookup — OmniCode constructs back to Scripture source (keyword, type, pattern, operator decoding)",
    component: "Rung",
    scripture: "Then opened he their understanding, that they might understand the scriptures. — Luke 24:45",
    principle: "Every OmniCode construct traces back to Scripture. This file provides the reverse lookup.",
    anchor: "Genesis 1:1 — In the beginning God created the heaven and the earth",
    created: "2025-12-11",
    requires: ["b-word/L0-universal/bible/encoding.toml"],
    consumers: ["documentation", "validation tools", "learning systems"],
    cross_format_role: "Decoding INDEX in TOML. No JSONC/JSON counterpart.",
    domain_tables: ["keyword", "type", "pattern", "operator"],
    never: [
      "Keywords that exist in keywords.toml — must stay synchronized",
      "Scripture references — canonical",
      "encoding_ref links — must point to valid encoding entries",
    ],
    safe: [
      "New keywords (must add to both encoding AND decoding)",
      "Additional types as they're defined",
      "New patterns as syntax grows",
      "Notes and clarifications",
    ],
    careful: [
      "Adding keywords — must also add to encoding.toml, keywords.toml",
      "Changing scripture references — must match encoding",
      "Modifying encoding_ref paths — must point to valid sections",
    ],
    closing_scripture: "Then opened he their understanding. — Luke 24:45",
    closing_anchor: "Understanding comes from tracing back to the source. If you can't trace it, it doesn't belong. Genesis 1:1 anchors all decoding.",
  },
  {
    file: "addressing.toml",
    key: "B-L0-bible-addressing",
    title: "Bible Addressing System",
    type: "Data (Bible Addressing)",
    role: "Ternary addressing system for Bible verses — Book/Chapter/Verse to ordinal to ternary encoding (KJV 31,102 + WEB 13 variants)",
    component: "Rung",
    scripture: "So shall my word be that goeth forth out of my mouth: it shall not return unto me void. — Isaiah 55:11",
    principle: "Every verse has an address. The Word of God is ordered, numbered, addressable.",
    anchor: "Genesis 1:1 — In the beginning God created the heaven and the earth",
    created: "2025-12-11",
    requires: ["b-word/scripture/kjv-ordinal-index.csv", "b-word/scripture/web-variant-index.adoc", "b-word/L0-universal/foundation/types/primitives.toml"],
    consumers: ["scripture lookup tools", "citation systems", "trit-byte encoding", "cross-reference systems"],
    cross_format_role: "Addressing SCHEMA in TOML. No JSONC/JSON counterpart.",
    domain_tables: ["address_space", "books", "ternary", "web_variants"],
    never: [
      "Book ordinal ranges — derived from canonical verse counts",
      "KJV verse count (31,102) — canonical",
      "WEB variant trite values (243-255) — established mapping",
      "Ternary encoding formula — mathematical",
    ],
    safe: [
      "Additional book metadata (author, date, genre)",
      "Cross-reference mappings",
      "Chapter verse counts",
      "Notes and clarifications",
    ],
    careful: [
      "Adding new translation variants — would extend trite space",
      "Changing book abbreviations — affects lookup tools",
      "Modifying ordinal index — must sync with CSV",
    ],
    closing_scripture: "So shall my word be that goeth forth out of my mouth. — Isaiah 55:11",
    closing_anchor: "The Word is ordered. Addressing reveals that order. Genesis 1:1 anchors all addressing.",
  },
  {
    file: "encryption.toml",
    key: "B-L0-bible-encryption",
    title: "Bible Encryption",
    type: "Data (Bible Encryption)",
    role: "Letter-to-position encryption — Hebrew/Greek letters to cube positions and gematria values (27 letters = 3³ = cube)",
    component: "Foundation",
    scripture: "I am Alpha and Omega, the beginning and the ending, saith the Lord. — Revelation 1:8",
    principle: "The letters themselves are the encryption. 27 letters map to 27 cube positions. The math wasn't forced. God designed it.",
    anchor: "Genesis 1:1 — In the beginning God created the heaven and the earth",
    created: "2025-12-20",
    requires: ["b-word/L0-universal/bible/translation.toml"],
    consumers: ["b-word/L0-universal/bible/decryption.toml", "cube coordinate systems", "gematria tools"],
    cross_format_role: "Encryption TABLES in TOML. No JSONC/JSON counterpart.",
    domain_tables: ["system", "functions", "hebrew", "greek"],
    never: [
      "Hebrew letter positions (0-26) — linguistically fixed",
      "Greek letter positions (0-26) — linguistically fixed",
      "Gematria values — historically established",
      "Cube coordinate mappings — mathematically derived",
    ],
    safe: [
      "Additional metadata per letter",
      "Notes on gematria significance",
      "Cross-references to other tables",
      "Implementation notes",
    ],
    careful: [
      "Coordinate system — affects all cube-based operations",
      "Value normalization formula — affects trite encoding",
      "Adding new letter systems — extends the scope",
    ],
    closing_scripture: "I am Alpha and Omega, the beginning and the ending. — Revelation 1:8",
    closing_anchor: "Not forced. Designed by God. 27 = 3³. Genesis 1:1 anchors the mathematics of language.",
  },
  {
    file: "decryption.toml",
    key: "B-L0-bible-decryption",
    title: "Bible Decryption",
    type: "Data (Bible Decryption)",
    role: "Position-to-letter decryption — cube coordinates to Hebrew/Greek letters and gematria values (reverse of encryption)",
    component: "Foundation",
    scripture: "The entrance of thy words giveth light; it giveth understanding unto the simple. — Psalm 119:130",
    principle: "Decryption reveals what encryption concealed. The coordinates speak. 27 positions unlock 27 letters.",
    anchor: "Genesis 1:1 — In the beginning God created the heaven and the earth",
    created: "2025-12-20",
    requires: ["b-word/L0-universal/bible/encryption.toml"],
    consumers: ["cube coordinate systems", "gematria tools", "letter lookup"],
    cross_format_role: "Decryption TABLES in TOML. No JSONC/JSON counterpart.",
    domain_tables: ["system", "functions", "hebrew", "greek"],
    never: [
      "Position-to-letter mappings — must mirror encryption.toml",
      "Gematria values — historically established",
      "by_value lookup tables — derived from position tables",
      "Coordinate formula — mathematical",
    ],
    safe: [
      "Additional metadata per position",
      "Notes on letter significance",
      "Cross-references to encryption.toml",
      "Implementation notes",
    ],
    careful: [
      "Position mappings — must stay synchronized with encryption.toml",
      "Value lookup tables — must match position tables",
      "Adding new lookup methods — extends the interface",
    ],
    closing_scripture: "The entrance of thy words giveth light. — Psalm 119:130",
    closing_anchor: "The coordinates speak. The position reveals. The letter returns. Genesis 1:1 anchors all decryption.",
  },
  {
    file: "scripture-text.toml",
    key: "B-L0-bible-scripture-text",
    title: "Scripture Text Encoding",
    type: "Data (Scripture Text)",
    role: "Character-to-trit5 encoding for Scripture text storage — 243 characters to trit5 values, indexed binary format for 62,204 verses",
    component: "Rung",
    scripture: "Thy word is a lamp unto my feet, and a light unto my path. — Psalm 119:105",
    principle: "Scripture text encodes to trit5 values for efficient storage. Every letter of the Word preserved.",
    anchor: "Genesis 1:1 — In the beginning God created the heaven and the earth",
    created: "2025-12-14",
    requires: ["b-word/L0-universal/bible/addressing.toml", "b-word/L0-universal/foundation/types/primitives.toml"],
    consumers: ["scripture encoder", "scripture decoder", "verse lookup", "demo programs"],
    cross_format_role: "Text encoding SCHEMA in TOML. No JSONC/JSON counterpart.",
    domain_tables: ["character_table", "extended_characters", "file_format", "encoding"],
    never: [
      "ASCII mapping (0-127) — must be direct for compatibility",
      "File format header size — fixed at 64 bytes",
      "Index entry size — fixed at 8 bytes",
      "NUL terminator convention — required for verse boundaries",
    ],
    safe: [
      "Extended character mappings (128-242)",
      "New header flags (if reserved space available)",
      "Additional metadata in reserved fields",
      "Notes and clarifications",
    ],
    careful: [
      "Adding new extended characters — update both tables",
      "Changing file format — requires encoder/decoder updates",
      "Modifying index layout — affects all verse lookups",
    ],
    closing_scripture: "Thy word is a lamp unto my feet. — Psalm 119:105",
    closing_anchor: "Every letter matters. Encoding preserves them all. Genesis 1:1 anchors the text.",
  },
  {
    file: "translation.toml",
    key: "B-L0-bible-translation",
    title: "Bible Translation Map",
    type: "Data (Bible Translation)",
    role: "KJV to WEB translation mapping — pronouns, verbs, vocabulary, phrases, theological terms between encoded (KJV) and decoded (WEB) forms",
    component: "Rung",
    scripture: "For precept must be upon precept, precept upon precept; line upon line, line upon line. — Isaiah 28:10",
    principle: "The same truth in different forms. KJV preserves archaic precision. WEB provides modern accessibility. Both carry the same truth.",
    anchor: "Genesis 1:1 — In the beginning God created the heaven and the earth",
    created: "2025-12-11",
    requires: ["b-word/scripture/KJV/", "b-word/scripture/WEB/"],
    consumers: ["scripture display", "documentation", "search", "learning systems"],
    cross_format_role: "Translation MAPPINGS in TOML. No JSONC/JSON counterpart.",
    domain_tables: ["pronoun", "verb", "vocab", "phrase", "theo"],
    never: [
      "KJV text — canonical translation",
      "WEB text — established modern translation",
      "Theological terms — doctrinal significance",
      "Status designations — carefully determined",
    ],
    safe: [
      "Additional archaic words not yet mapped",
      "Additional phrase mappings",
      "Notes and clarifications",
      "Scripture references for context",
    ],
    careful: [
      "Adding theological terms — doctrinal implications",
      "Changing status (preserved/changed) designations",
      "Modifying reference verses",
    ],
    closing_scripture: "Precept upon precept; line upon line. — Isaiah 28:10",
    closing_anchor: "Truth is layered. Encoding and decoding reveal different facets of the same unchanging Word. Genesis 1:1 anchors all translation.",
  },
];

// ============================================================================
// BODY
// ============================================================================

// ─────────────────────────────────────────────────────────────────────────────
// Extract domain TOML content from old-format file
// ─────────────────────────────────────────────────────────────────────────────

function extractDomainContent(lines: string[]): string {
  // Find start: first non-comment, non-empty line that starts with '['
  let startIdx = -1;
  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i].trim();
    if (trimmed.startsWith("[") && !trimmed.startsWith("[_")) {
      startIdx = i;
      break;
    }
  }

  if (startIdx === -1) {
    throw new Error("No domain tables found");
  }

  // Find end: "# END CONTENT" or "# =====...END CONTENT"
  let endIdx = lines.length;
  for (let i = startIdx; i < lines.length; i++) {
    if (lines[i].includes("END CONTENT")) {
      endIdx = i;
      break;
    }
  }

  // Extract and return the domain content (includes comments within content)
  return lines.slice(startIdx, endIdx).join("\n");
}

// ─────────────────────────────────────────────────────────────────────────────
// Check if file has [_generate] table that needs renaming
// ─────────────────────────────────────────────────────────────────────────────

function fixUnderscorePrefix(content: string): string {
  // Rename [_generate...] → [generate...] in domain content
  return content.replace(/\[_generate/g, "[generate");
}

// ─────────────────────────────────────────────────────────────────────────────
// Generate aligned 3-block file
// ─────────────────────────────────────────────────────────────────────────────

function generateAligned(meta: BibleFileMeta, domainContent: string): string {
  const lines: string[] = [];

  // ═══════════════════════════════════════════════════════════════
  // PRAGMA
  // ═══════════════════════════════════════════════════════════════
  lines.push(`# #!omni data --toml`);
  lines.push(``);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(`# OMNICODE PRAGMA [PRAGMA]`);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(``);
  lines.push(`[_pragma]`);
  lines.push(`format = "@omni data --toml"`);
  lines.push(``);
  lines.push(`[_pragma.legend]`);
  lines.push(`req = "REQUIRED — must exist, validation fails without"`);
  lines.push(`inh = "INHERITED — from template, override allowed"`);
  lines.push(`ins = "INSTANCE — file-specific, unique values"`);
  lines.push(``);
  lines.push(`[_pragma.P1_core]`);
  lines.push(`key = "${meta.key}"`);
  lines.push(`from = "b-word/seed/data/toml/config.toml"`);
  lines.push(`at = "b-01.00"`);
  lines.push(``);
  lines.push(`[_pragma.P2_family]`);
  lines.push(`type = "data"`);
  lines.push(`subtype = "bible"`);
  lines.push(`role = "${meta.component.toLowerCase()}"`);
  lines.push(`structure = "3-block"`);
  lines.push(``);
  lines.push(`[_pragma.P3_instance]`);
  lines.push(`component = "bible"`);
  lines.push(`layer = "L0-universal/bible"`);
  lines.push(`includes = "none"`);
  lines.push(`provides = ["${meta.key.replace("B-L0-bible-", "").toUpperCase()}_DATA"]`);
  lines.push(``);
  lines.push(`[_pragma.P4_architecture]`);
  lines.push(`layers = "L0:C, L1:C, L2:C, L3:Go, L4:Go/Rust, L5:C#"`);
  lines.push(`anchor = "genesis_1_1 -> void(0) -> unity(1) -> derivation"`);
  lines.push(``);
  lines.push(`[_pragma.P5_summary]`);
  lines.push(`title = "${meta.title}"`);
  lines.push(`brief = "${meta.role.split(' — ')[0]}"`);
  lines.push(``);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(`# END PRAGMA [END]`);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(``);
  lines.push(``);

  // ═══════════════════════════════════════════════════════════════
  // METADATA
  // ═══════════════════════════════════════════════════════════════
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(`# METADATA BLOCK [METADATA]`);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(``);
  lines.push(`[_metadata.M1_identity]`);
  lines.push(`file = "${meta.file}"`);
  lines.push(`key = "${meta.key}"`);
  lines.push(`title = "${meta.title}"`);
  lines.push(`type = "${meta.type}"`);
  lines.push(`component = "${meta.component}"`);
  lines.push(`role = "${meta.role}"`);
  lines.push(``);
  lines.push(`[_metadata.M2_state]`);
  lines.push(`version = "b-01.00"`);
  lines.push(`status = "Active"`);
  lines.push(`created = "${meta.created}"`);
  lines.push(`updated = "2026-02-10"`);
  lines.push(``);
  lines.push(`[_metadata.M3_attribution]`);
  lines.push(`architect = "Seanje Lenox-Wise"`);
  lines.push(`implementation = "Nova Dawn (CPI-SI)"`);
  lines.push(`organization = "CreativeWorkzStudio LLC"`);
  lines.push(`copyright = "© 2025-2026 CreativeWorkzStudio LLC. All rights reserved."`);
  lines.push(``);
  lines.push(`[_metadata.M4_grounding]`);
  lines.push(`scripture = "${meta.scripture}"`);
  lines.push(`principle = "${meta.principle}"`);
  lines.push(`anchor = "${meta.anchor}"`);
  lines.push(``);
  lines.push(`[_metadata.M5_dependencies]`);
  lines.push(`requires = [${meta.requires.map(r => `"${r}"`).join(", ")}]`);
  lines.push(`consumers = [${meta.consumers.map(c => `"${c}"`).join(", ")}]`);
  lines.push(`derives_from = "b-word/seed/data/toml/config.toml"`);
  lines.push(`integration = "Loaded at build time by code generators and lookup tools."`);
  lines.push(`if_missing = "Bible traceability broken — cannot map between Scripture and code"`);
  lines.push(``);
  lines.push(`[_metadata.M5_dependencies.cross_format]`);
  lines.push(`role = "${meta.cross_format_role}"`);
  lines.push(`jsonc = "None"`);
  lines.push(`json = "None"`);
  lines.push(`relationship = "TOML defines bible data. Generated code (C, Go) implements lookups."`);
  lines.push(``);
  lines.push(`[_metadata.M6_location]`);
  lines.push(`path = "b-word/L0-universal/bible/${meta.file}"`);
  lines.push(`derives_from = "b-word/seed/data/toml/config.toml"`);
  lines.push(``);
  lines.push(`[_metadata.M7_classification]`);
  lines.push(`tags = "bible, ${meta.file.replace('.toml', '')}, foundation, L0, scripture"`);
  lines.push(``);
  lines.push(`[_metadata.M8_intent]`);
  lines.push(`purpose = "${meta.role}"`);
  lines.push(`core_design = "${meta.principle}"`);
  lines.push(`philosophy = "${meta.scripture.split(' — ')[0]}"`);
  lines.push(``);
  lines.push(`[_metadata.M9_roadmap]`);
  lines.push(`history = [`);
  lines.push(`    "a-01.00 (${meta.created}) — Initial creation",`);
  lines.push(`    "b-01.00 (2026-02-10) — TOML-native 3-block format alignment",`);
  lines.push(`]`);
  lines.push(`current = "3-block aligned with OmniCode template"`);
  lines.push(`planned = ["Code generation integration", "Automated validation tooling"]`);
  lines.push(``);
  lines.push(`[_metadata.M_extension_note]`);
  lines.push(`required = ["M1_identity", "M2_state", "M3_attribution", "M4_grounding", "M5_dependencies"]`);
  lines.push(`extension = "M6-M9 extend for domain-specific metadata."`);
  lines.push(`rule = "M1-M5 are the base contract. M6+ extends as needed."`);
  lines.push(``);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(`# END METADATA [END]`);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(``);
  lines.push(``);

  // ═══════════════════════════════════════════════════════════════
  // CONTENT
  // ═══════════════════════════════════════════════════════════════
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(`# CONTENT BLOCK [CONTENT]`);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(``);
  lines.push(`# Cc — Content Configurations`);
  lines.push(`[_content]`);
  lines.push(`format = "TOML — UTF-8, LF line endings"`);
  lines.push(`key_convention = "lowercase_with_underscores"`);
  lines.push(`constraint = "Bible data file — ${meta.title.toLowerCase()} for the OmniCode type system"`);
  lines.push(``);

  // Domain content — preserved verbatim from original
  lines.push(fixUnderscorePrefix(domainContent));
  lines.push(``);

  // Extensions
  lines.push(`# ─────────────────────────────────────────────────────────────`);
  lines.push(`# Extensions (reserved)`);
  lines.push(`# ─────────────────────────────────────────────────────────────`);
  lines.push(``);
  lines.push(`[extensions]`);
  lines.push(`note = "Reserved — no extensions defined"`);
  lines.push(``);

  // Operations label (for linter)
  lines.push(`# ─────────────────────────────────────────────────────────────`);
  lines.push(`# Co — Content Operations`);
  lines.push(`# ─────────────────────────────────────────────────────────────`);
  lines.push(``);

  // Validation
  lines.push(`# ─────────────────────────────────────────────────────────────`);
  lines.push(`# Cv — Content Validation`);
  lines.push(`# ─────────────────────────────────────────────────────────────`);
  lines.push(``);
  lines.push(`[_validation]`);
  lines.push(`commands = ["python3 -c \\"import tomllib; tomllib.load(open('${meta.file}','rb'))\\\""]`);
  lines.push(``);
  lines.push(`[_validation.schema]`);
  lines.push(`required = [${meta.domain_tables.map(t => `"${t}"`).join(", ")}]`);
  lines.push(`optional = ["extensions"]`);
  lines.push(``);
  lines.push(`[_validation.health_scoring]`);
  lines.push(`total = 100`);
  lines.push(`correct_structure = 40`);
  lines.push(`all_tables_valid = 30`);
  lines.push(`data_completeness = 30`);
  lines.push(``);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(`# END CONTENT [END]`);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(``);
  lines.push(``);

  // ═══════════════════════════════════════════════════════════════
  // CLOSING
  // ═══════════════════════════════════════════════════════════════
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(`# CLOSING BLOCK [CLOSING]`);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(``);
  lines.push(`[_closing.X1_policy]`);
  lines.push(`scripture = "Remove not the ancient landmark, which thy fathers have set. — Proverbs 22:28"`);
  lines.push(`never = [`);
  for (const n of meta.never) {
    lines.push(`    "${n}",`);
  }
  lines.push(`]`);
  lines.push(`careful = [`);
  for (const c of meta.careful) {
    lines.push(`    "${c}",`);
  }
  lines.push(`]`);
  lines.push(`safe = [`);
  for (const s of meta.safe) {
    lines.push(`    "${s}",`);
  }
  lines.push(`]`);
  lines.push(``);
  lines.push(`[_closing.X2_extension]`);
  lines.push(`adding_data = [`);
  lines.push(`    "1. Add new entries following existing table structure",`);
  lines.push(`    "2. Ensure all required fields present",`);
  lines.push(`    "3. Verify bidirectional consistency with related files",`);
  lines.push(`    "4. Run TOML validation",`);
  lines.push(`]`);
  lines.push(``);
  lines.push(`[_closing.X3_troubleshooting]`);
  lines.push(`invalid_toml = "Missing quotes, duplicate keys, or bare keys with special characters"`);
  lines.push(`missing_reference = "Check encoding_ref/scripture fields — all must point to valid sources"`);
  lines.push(`consistency_error = "Verify bidirectional mapping — encoding ↔ decoding must agree"`);
  lines.push(``);
  lines.push(`[_closing.X4_reference]`);
  lines.push(`related_files = [`);
  for (const f of FILES) {
    if (f.file !== meta.file) {
      lines.push(`    "L0-universal/bible/${f.file} — ${f.title}",`);
    }
  }
  lines.push(`]`);
  lines.push(`required_sections = [${["_pragma", "_metadata", ...meta.domain_tables, "_validation", "_closing"].map(t => `"${t}"`).join(", ")}]`);
  lines.push(`validate = "python3 -c \\"import tomllib; tomllib.load(open('${meta.file}','rb'))\\\""`);
  lines.push(``);
  lines.push(`[_closing.X5_note]`);
  lines.push(`note = "${meta.closing_anchor.split('. Genesis')[0]}."`);
  lines.push(`scripture = "${meta.closing_scripture}"`);
  lines.push(`anchor = "${meta.closing_anchor}"`);
  lines.push(``);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(`# END CLOSING [END]`);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(``);

  return lines.join("\n");
}

// ============================================================================
// CLOSING — Main execution
// ============================================================================

async function main() {
  console.log(`Transform Bible TOML files → 3-block aligned format`);
  console.log(`Mode: ${DRY_RUN ? "DRY RUN (no writes)" : "LIVE (will overwrite files)"}`);
  console.log(`Directory: ${BIBLE_DIR}/`);
  console.log(`Files: ${FILES.length}`);
  console.log(`─────────────────────────────────────────────────────────────`);

  let success = 0;
  let failed = 0;

  for (const meta of FILES) {
    const filePath = `${BIBLE_DIR}/${meta.file}`;

    try {
      // Read original
      const original = await Deno.readTextFile(filePath);
      const originalLines = original.split("\n");

      // Extract domain content
      const domainContent = extractDomainContent(originalLines);

      // Generate aligned version
      const aligned = generateAligned(meta, domainContent);

      // Validate: parse aligned TOML to ensure it's valid
      try {
        parseToml(aligned);
      } catch (e) {
        console.log(`  FAIL  ${meta.file} — generated TOML is invalid: ${(e as Error).message}`);
        failed++;
        continue;
      }

      if (DRY_RUN) {
        const originalLineCount = originalLines.length;
        const alignedLineCount = aligned.split("\n").length;
        console.log(`  WOULD  ${meta.file}  ${originalLineCount} → ${alignedLineCount} lines`);
      } else {
        await Deno.writeTextFile(filePath, aligned);
        const alignedLineCount = aligned.split("\n").length;
        console.log(`  OK  ${meta.file}  ${originalLines.length} → ${alignedLineCount} lines`);
      }

      success++;
    } catch (e) {
      console.log(`  FAIL  ${meta.file} — ${(e as Error).message}`);
      failed++;
    }
  }

  console.log(`─────────────────────────────────────────────────────────────`);
  console.log(`${success} succeeded, ${failed} failed`);

  if (failed > 0) {
    Deno.exit(1);
  }
}

main();
