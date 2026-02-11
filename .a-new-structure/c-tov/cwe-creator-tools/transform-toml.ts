#!/usr/bin/env -S deno run --allow-read --allow-write
// ============================================================================
// METADATA
// ============================================================================
//
// file:    transform-toml.ts
// key:     B-tools-transform-toml
// title:   General TOML 3-Block Alignment Transformer
// type:    Tool (Transformer)
// version: a-01.00
// created: 2026-02-10
// updated: 2026-02-10
// authors: Nova Dawn (CPI-SI)
// purpose: Transform old comment-based TOML files into proper OmniCode
//          3-block aligned format. Parses comment metadata automatically,
//          preserves domain content verbatim, wraps in proper infrastructure.
//
// usage:   deno run --allow-read --allow-write tools/transform-toml.ts <file-or-dir> [--dry-run] [--verbose]
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { parse as parseToml } from "jsr:@std/toml";

// ─────────────────────────────────────────────────────────────────────────────
// Types
// ─────────────────────────────────────────────────────────────────────────────

interface ParsedMeta {
  // From header
  key: string;
  oldKey: string;
  title: string;
  formatType: string; // "data", "contract"

  // Biblical foundation
  scripture: string;
  principle: string;
  anchor: string;

  // CPI-SI identity
  componentType: string; // "Ladder", "Rung", "Contract", "Foundation", "Top"
  role: string;

  // Authorship
  architect: string;
  implementation: string;
  created: string;
  version: string;

  // Purpose
  purpose: string;
  coreDesign: string;
  philosophy: string;

  // Dependencies
  requires: string[];
  consumers: string[];

  // Closing
  never: string[];
  safe: string[];
  careful: string[];
  closingNote: string;
  closingScripture: string;
}

interface LayerInfo {
  layerNum: string;   // "L0", "L1", "L2", "L3", "L4"
  layerName: string;  // "L0-universal", "L1-omnicode", etc.
  subdir: string;     // "language", "filesystem", etc.
  subtype: string;    // same as subdir for now
}

// ─────────────────────────────────────────────────────────────────────────────
// Constants
// ─────────────────────────────────────────────────────────────────────────────

const DRY_RUN = Deno.args.includes("--dry-run");
const VERBOSE = Deno.args.includes("--verbose");

const LAYER_MAP: Record<string, string> = {
  "L0": "L0-universal",
  "L1": "L1-omnicode",
  "L2": "L2-platform",
  "L3": "L3-cpisi",
  "L4": "L4-faithnet",
  "L5": "L5-applications",
};

const TODAY = new Date().toISOString().split("T")[0];

// ============================================================================
// BODY
// ============================================================================

// ─────────────────────────────────────────────────────────────────────────────
// Section Detection — find metadata/content/closing boundaries
// ─────────────────────────────────────────────────────────────────────────────

interface SectionBounds {
  firstTable: number;    // First [table] line (start of domain content)
  closingStart: number;  // Start of closing section
  contentEnd: number;    // End of domain content (before closing or END CONTENT)
}

function findSections(lines: string[]): SectionBounds {
  let firstTable = -1;
  let closingStart = -1;
  let contentEnd = -1;

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i].trim();

    // First TOML table definition
    if (firstTable === -1 && trimmed.startsWith("[") && !trimmed.startsWith("[[")) {
      firstTable = i;
    }

    // Also check for array-of-tables
    if (firstTable === -1 && trimmed.startsWith("[[")) {
      firstTable = i;
    }

    // END CONTENT marker
    if (trimmed.includes("END CONTENT")) {
      contentEnd = i;
    }

    // CLOSING section start (look for "# CLOSING" after a separator line)
    if (closingStart === -1 && /^#\s*CLOSING\s*$/.test(trimmed)) {
      // Back up to the separator line before it
      closingStart = i > 0 && /^# =+$/.test(lines[i - 1].trim()) ? i - 1 : i;
    }
  }

  // Check if there are TOML tables between END CONTENT and CLOSING
  // (old format puts [_generate] between them — include it)
  if (contentEnd !== -1 && closingStart !== -1 && contentEnd < closingStart) {
    for (let i = contentEnd; i < closingStart; i++) {
      if (lines[i].trim().startsWith("[") && !lines[i].trim().startsWith("[[")) {
        // Tables exist between END CONTENT and CLOSING — extend to include them
        contentEnd = closingStart;
        break;
      }
    }
  }

  // If no explicit END CONTENT, use the line before closing
  if (contentEnd === -1 && closingStart !== -1) {
    contentEnd = closingStart;
  }

  // If no closing found, use end of file
  if (closingStart === -1) {
    closingStart = lines.length;
    contentEnd = lines.length;
  }

  return { firstTable, closingStart, contentEnd };
}

// ─────────────────────────────────────────────────────────────────────────────
// Check if file is already aligned (has [_pragma] tables)
// ─────────────────────────────────────────────────────────────────────────────

function isAlreadyAligned(content: string): boolean {
  return content.includes("[_pragma]") || content.includes("[_pragma.P1_core]");
}

// ─────────────────────────────────────────────────────────────────────────────
// Metadata Extraction — parse comment-based metadata fields
// ─────────────────────────────────────────────────────────────────────────────

function extractField(lines: string[], pattern: RegExp, multiline = false): string {
  const parts: string[] = [];
  let found = false;

  for (const line of lines) {
    const stripped = line.replace(/^#\s?/, "");

    if (!found) {
      const match = stripped.match(pattern);
      if (match) {
        found = true;
        parts.push(match[1].trim());
        if (!multiline) break; // Single-line: take first match only
        continue;
      }
    } else {
      // Collecting continuation lines for multi-line field
      const cont = stripped.trim();
      if (
        cont === "" ||
        cont.startsWith("---") ||
        cont.startsWith("===") ||
        cont.startsWith("# ") ||
        /^(Scripture|Principle|Anchor|Component|Role|Paradigm|Architect|Implementation|Created|Version|Purpose|Core Design|Philosophy|Package|File|Key|What|Integration|Derived|References|DEPENDENCY):/.test(cont)
      ) {
        break; // End of multi-line value
      }
      parts.push(cont);
    }
  }

  return parts.join(" ").trim().replace(/^["']|["']$/g, "");
}

function extractListItems(lines: string[], startPattern: RegExp): string[] {
  const items: string[] = [];
  let collecting = false;
  let emptyCount = 0;

  for (const line of lines) {
    const stripped = line.replace(/^#\s?/, "").trim();

    if (startPattern.test(stripped)) {
      collecting = true;
      emptyCount = 0;
      continue;
    }

    if (collecting) {
      if (stripped === "") {
        emptyCount++;
        if (emptyCount >= 2) break; // Two blank lines = done
        continue;
      }
      emptyCount = 0;

      // Stop on section breaks or new headers
      if (
        stripped.startsWith("===") ||
        stripped.startsWith("---") ||
        /^#\s/.test(stripped) ||
        /^(What|Integration|OPERATIONAL|INTERFACE|CLOSING|CONTENT|Safe|NEVER|Requires)/.test(stripped)
      ) {
        break;
      }

      // Match list items: "- text", "❌ text", "✅ text", "⚠️ text"
      const match = stripped.match(/^(?:[-]|❌|✅|⚠️)\s*(.+)/);
      if (match) {
        items.push(match[1].trim());
      }
    }
  }

  return items;
}

function parseCommentMetadata(lines: string[]): ParsedMeta {
  // Get comment-only lines (everything before first [table])
  const commentLines = lines.filter((l) => l.trim().startsWith("#") || l.trim() === "");

  // ── Header extraction ──
  // Title: line between ═══ delimiters that isn't a key or dependency
  let title = "";
  for (const line of lines.slice(0, 15)) {
    const stripped = line.replace(/^#\s?/, "").trim();
    if (
      stripped.length > 10 &&
      !stripped.startsWith("═") &&
      !stripped.startsWith("=") &&
      !stripped.startsWith("#!omni") &&
      !stripped.startsWith("Key:") &&
      !stripped.startsWith("DEPENDENCY")
    ) {
      // Extract title before parenthetical
      const titleMatch = stripped.match(/^(.+?)\s*\(.*\)\s*$/);
      title = titleMatch ? titleMatch[1].trim() : stripped;
      break;
    }
  }

  // Format type from #!omni pragma
  const pragmaLine = lines[0] || "";
  let formatType = "data";
  if (pragmaLine.includes("contract")) formatType = "contract";
  else if (pragmaLine.includes("specification")) formatType = "specification";

  // Key from # Key: line
  const oldKey = extractField(commentLines, /^Key:\s*(.+)/);

  // Biblical foundation
  const scripture = extractField(commentLines, /^Scripture:\s*(.+)/, true);
  const principle = extractField(commentLines, /^Principle:\s*(.+)/, true);
  const rawAnchor = extractField(commentLines, /^Anchor:\s*(.+)/, true);

  // CPI-SI identity
  const rawComponentType = extractField(commentLines, /^Component Type:\s*(.+)/);
  const componentType = rawComponentType.split("(")[0].trim() || "Data";
  const role = extractField(commentLines, /^Role:\s*(.+)/, true);

  // Authorship
  const architect = extractField(commentLines, /^\s*Architect:\s*(.+)/) || "Seanje Lenox-Wise";
  const implementation = extractField(commentLines, /^\s*Implementation:\s*(.+)/) || "Nova Dawn";
  const created = extractField(commentLines, /^\s*Created:\s*(.+)/) || TODAY;
  const version = extractField(commentLines, /^\s*Version:\s*(.+)/) || "a-01.00";

  // Purpose
  const purpose = extractField(commentLines, /^Purpose:\s*(.+)/, true);
  const coreDesign = extractField(commentLines, /^Core Design:\s*(.+)/, true);
  const philosophy = extractField(commentLines, /^Philosophy:\s*(.+)/, true);

  // Dependencies — extract from "What This Needs" and "What Uses This"
  const requires = extractListItems(commentLines, /What This Needs/);
  const consumers = extractListItems(commentLines, /What Uses This/);

  // Closing info — extract from CLOSING section (compute bounds once)
  const bounds = findSections(lines);
  const closingLines = lines.slice(bounds.closingStart);

  const never = extractListItems(closingLines, /NEVER\s+Modify/);
  const safe = extractListItems(closingLines, /Safe\s+to\s+Add/);
  const careful = extractListItems(closingLines, /Requires\s+Coordination/);

  // Closing note and scripture — from Synthesis section
  let closingNote = "";
  let closingScripture = "";
  let inSynthesis = false;
  const noteLines: string[] = [];

  for (const line of closingLines) {
    const stripped = line.replace(/^#\s?/, "").trim();
    if (stripped.includes("Synthesis") || stripped.includes("Closing Note")) {
      inSynthesis = true;
      continue;
    }
    if (inSynthesis) {
      if (stripped.startsWith("===") || stripped.startsWith("END CLOSING")) break;
      if (stripped.startsWith('"') || stripped.startsWith("'")) {
        // Scripture quote line
        closingScripture += stripped.replace(/^["']|["']$/g, "") + " ";
      } else if (stripped.startsWith("—") || stripped.startsWith("--")) {
        // Attribution line — append to scripture
        closingScripture += stripped + " ";
      } else if (stripped.length > 0 && !stripped.startsWith("---")) {
        noteLines.push(stripped);
      }
    }
  }

  closingNote = noteLines.join(" ").trim();
  closingScripture = closingScripture.trim().replace(/^["']|["']$/g, "");

  // Ensure anchor always references Genesis 1:1
  const anchor = "Genesis 1:1 — In the beginning God created the heaven and the earth";

  return {
    key: oldKey, // Will be replaced with new key format
    oldKey,
    title,
    formatType,
    scripture: cleanQuotes(scripture),
    principle,
    anchor,
    componentType,
    role,
    architect,
    implementation,
    created,
    version,
    purpose,
    coreDesign,
    philosophy,
    requires,
    consumers,
    never,
    safe,
    careful,
    closingNote,
    closingScripture: cleanQuotes(closingScripture),
  };
}

function cleanQuotes(s: string): string {
  return s.replace(/^["']+|["']+$/g, "").trim();
}

// ─────────────────────────────────────────────────────────────────────────────
// Domain Content Extraction
// ─────────────────────────────────────────────────────────────────────────────

function extractDomainContent(lines: string[], bounds: SectionBounds): string {
  if (bounds.firstTable === -1) {
    throw new Error("No TOML tables found in file");
  }

  // Extract from first table through content end
  const domainLines = lines.slice(bounds.firstTable, bounds.contentEnd);

  // Trim trailing empty lines and comment-only blocks at the end
  let endIdx = domainLines.length;
  while (endIdx > 0) {
    const line = domainLines[endIdx - 1].trim();
    if (line === "" || line.startsWith("#")) {
      endIdx--;
    } else {
      break;
    }
  }

  return domainLines.slice(0, endIdx).join("\n");
}

function fixUnderscorePrefix(content: string): string {
  return content.replace(/\[_generate/g, "[generate");
}

function detectDomainTables(content: string): string[] {
  const tables = new Set<string>();
  for (const line of content.split("\n")) {
    // Match [table] and [table.sub] but not [[array]]
    const m = line.match(/^\[([a-z_][a-z0-9_]*)/);
    if (m) {
      // After underscore fix, generate won't have underscore
      const name = m[1].replace(/^_/, "");
      tables.add(name);
    }
  }
  return [...tables].sort();
}

// ─────────────────────────────────────────────────────────────────────────────
// Layer Info Derivation
// ─────────────────────────────────────────────────────────────────────────────

function deriveLayerInfo(filepath: string): LayerInfo {
  // Expected path pattern: .../toml/.staging/L<n>/<subdir>/<file>.toml
  const parts = filepath.replace(/\\/g, "/").split("/");

  let layerNum = "L0";
  let subdir = "unknown";

  for (let i = 0; i < parts.length; i++) {
    if (/^L\d$/.test(parts[i])) {
      layerNum = parts[i];
      if (i + 1 < parts.length && !parts[i + 1].endsWith(".toml")) {
        subdir = parts[i + 1];
      }
      break;
    }
  }

  const layerName = LAYER_MAP[layerNum] || `${layerNum}-unknown`;

  return {
    layerNum,
    layerName,
    subdir,
    subtype: subdir,
  };
}

function deriveNewKey(filepath: string, layerInfo: LayerInfo): string {
  const basename = filepath.split("/").pop()!.replace(".toml", "");
  return `B-${layerInfo.layerNum}-${layerInfo.subdir}-${basename}`;
}

// ─────────────────────────────────────────────────────────────────────────────
// Output Generation — build the 3-block aligned file
// ─────────────────────────────────────────────────────────────────────────────

function escToml(s: string): string {
  // Escape double quotes and backslashes for TOML string values
  return s.replace(/\\/g, "\\\\").replace(/"/g, '\\"');
}

function generateAligned(
  meta: ParsedMeta,
  domainContent: string,
  layerInfo: LayerInfo,
  filepath: string,
): string {
  const filename = filepath.split("/").pop()!;
  const basename = filename.replace(".toml", "");
  const newKey = deriveNewKey(filepath, layerInfo);
  const domainTables = detectDomainTables(fixUnderscorePrefix(domainContent));

  // Derive brief from role or purpose
  const brief = (meta.role || meta.purpose || meta.title).split(" — ")[0];

  // Derive provides
  const provides = `${basename.toUpperCase().replace(/-/g, "_")}_DATA`;

  // Build requires list (file paths only)
  const requirePaths = meta.requires
    .map((r) => {
      const pathMatch = r.match(/(word\/\S+\.toml)/);
      return pathMatch ? `b-${pathMatch[1]}` : null;
    })
    .filter(Boolean) as string[];

  // Build consumers list (component names)
  const consumerNames = meta.consumers
    .map((c) => {
      // Extract just the component name before the colon
      const nameMatch = c.match(/^([^:]+)/);
      return nameMatch ? nameMatch[1].trim() : c.trim();
    })
    .filter((c) => c.length > 0);

  const lines: string[] = [];

  // ═══════════════════════════════════════════════════════════════
  // PRAGMA
  // ═══════════════════════════════════════════════════════════════
  lines.push(`# #!omni ${meta.formatType} --toml`);
  lines.push(``);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(`# OMNICODE PRAGMA [PRAGMA]`);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(``);
  lines.push(`[_pragma]`);
  lines.push(`format = "@omni ${meta.formatType} --toml"`);
  lines.push(``);
  lines.push(`[_pragma.legend]`);
  lines.push(`req = "REQUIRED — must exist, validation fails without"`);
  lines.push(`inh = "INHERITED — from template, override allowed"`);
  lines.push(`ins = "INSTANCE — file-specific, unique values"`);
  lines.push(``);
  lines.push(`[_pragma.P1_core]`);
  lines.push(`key = "${newKey}"`);
  lines.push(`from = "b-word/seed/data/toml/config.toml"`);
  lines.push(`at = "b-01.00"`);
  lines.push(``);
  lines.push(`[_pragma.P2_family]`);
  lines.push(`type = "${meta.formatType}"`);
  lines.push(`subtype = "${layerInfo.subtype}"`);
  lines.push(`role = "${meta.componentType.toLowerCase()}"`);
  lines.push(`structure = "3-block"`);
  lines.push(``);
  lines.push(`[_pragma.P3_instance]`);
  lines.push(`component = "${layerInfo.subdir}"`);
  lines.push(`layer = "${layerInfo.layerName}/${layerInfo.subdir}"`);
  lines.push(`includes = "none"`);
  lines.push(`provides = ["${provides}"]`);
  lines.push(``);
  lines.push(`[_pragma.P4_architecture]`);
  lines.push(`layers = "L0:C, L1:C, L2:C, L3:Go, L4:Go/Rust, L5:C#"`);
  lines.push(`anchor = "genesis_1_1 -> void(0) -> unity(1) -> derivation"`);
  lines.push(``);
  lines.push(`[_pragma.P5_summary]`);
  lines.push(`title = "${escToml(meta.title)}"`);
  lines.push(`brief = "${escToml(brief)}"`);
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
  lines.push(`file = "${filename}"`);
  lines.push(`key = "${newKey}"`);
  lines.push(`title = "${escToml(meta.title)}"`);
  lines.push(`type = "${escToml(meta.formatType === "contract" ? `Contract (${meta.componentType})` : `Data (${meta.componentType})`)}"`);
  lines.push(`component = "${escToml(meta.componentType)}"`);
  lines.push(`role = "${escToml(meta.role || meta.purpose)}"`);
  lines.push(``);
  lines.push(`[_metadata.M2_state]`);
  lines.push(`version = "b-01.00"`);
  lines.push(`status = "Active"`);
  lines.push(`created = "${meta.created}"`);
  lines.push(`updated = "${TODAY}"`);
  lines.push(``);
  lines.push(`[_metadata.M3_attribution]`);
  lines.push(`architect = "${meta.architect}"`);
  lines.push(`implementation = "${meta.implementation} (CPI-SI)"`);
  lines.push(`organization = "CreativeWorkzStudio LLC"`);
  lines.push(`copyright = "© 2025-2026 CreativeWorkzStudio LLC. All rights reserved."`);
  lines.push(``);
  lines.push(`[_metadata.M4_grounding]`);
  lines.push(`scripture = "${escToml(meta.scripture || "In the beginning God created the heaven and the earth. — Genesis 1:1")}"`);
  lines.push(`principle = "${escToml(meta.principle || meta.philosophy || "Every OmniCode construct traces to Scripture.")}"`);
  lines.push(`anchor = "${meta.anchor}"`);
  lines.push(``);
  lines.push(`[_metadata.M5_dependencies]`);
  if (requirePaths.length > 0) {
    lines.push(`requires = [${requirePaths.map((r) => `"${r}"`).join(", ")}]`);
  } else {
    lines.push(`requires = ["b-word/L0-universal/foundation/types/primitives.toml"]`);
  }
  if (consumerNames.length > 0) {
    lines.push(`consumers = [${consumerNames.map((c) => `"${escToml(c)}"`).join(", ")}]`);
  } else {
    lines.push(`consumers = ["parsers", "code generators", "documentation"]`);
  }
  lines.push(`derives_from = "b-word/seed/data/toml/config.toml"`);
  lines.push(`integration = "Loaded at build time by code generators and tools."`);
  lines.push(`if_missing = "${escToml(meta.title)} unavailable — dependent systems cannot function"`);
  lines.push(``);
  lines.push(`[_metadata.M5_dependencies.cross_format]`);
  lines.push(`role = "${escToml(meta.title)} in TOML. No JSONC/JSON counterpart."`);
  lines.push(`jsonc = "None"`);
  lines.push(`json = "None"`);
  lines.push(`relationship = "TOML defines ${layerInfo.subtype} data. Generated code implements lookups."`);
  lines.push(``);
  lines.push(`[_metadata.M6_location]`);
  lines.push(`path = "b-word/${layerInfo.layerName}/${layerInfo.subdir}/${filename}"`);
  lines.push(`derives_from = "b-word/seed/data/toml/config.toml"`);
  lines.push(``);
  lines.push(`[_metadata.M7_classification]`);
  lines.push(`tags = "${basename}, ${layerInfo.subdir}, ${layerInfo.layerNum}, ${meta.componentType.toLowerCase()}, omnicode"`);
  lines.push(``);
  lines.push(`[_metadata.M8_intent]`);
  lines.push(`purpose = "${escToml(meta.purpose || meta.role)}"`);
  lines.push(`core_design = "${escToml(meta.coreDesign || meta.purpose)}"`);
  lines.push(`philosophy = "${escToml(meta.philosophy || "The Bible is our codex. OmniCode constructs are discovered from Scripture, not invented.")}"`);
  lines.push(``);
  lines.push(`[_metadata.M9_roadmap]`);
  lines.push(`history = [`);
  lines.push(`    "${meta.version} (${meta.created}) — Initial creation",`);
  lines.push(`    "b-01.00 (${TODAY}) — TOML-native 3-block format alignment",`);
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
  lines.push(`constraint = "${escToml(meta.formatType === "contract" ? `Contract file — ${meta.title.toLowerCase()} for the OmniCode system` : `${meta.componentType} data file — ${meta.title.toLowerCase()} for the OmniCode system`)}"`);
  lines.push(``);

  // Domain content — preserved verbatim (with underscore fix)
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

  // Operations label
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
  lines.push(`commands = ["python3 -c \\"import tomllib; tomllib.load(open('${filename}','rb'))\\\""]`);
  lines.push(``);
  lines.push(`[_validation.schema]`);
  lines.push(`required = [${domainTables.map((t) => `"${t}"`).join(", ")}]`);
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

  // Never
  if (meta.never.length > 0) {
    lines.push(`never = [`);
    for (const n of meta.never) {
      lines.push(`    "${escToml(n)}",`);
    }
    lines.push(`]`);
  } else {
    lines.push(`never = [`);
    lines.push(`    "Scripture references — canonical",`);
    lines.push(`    "Core mappings — foundational to system",`);
    lines.push(`    "3-block structure (METADATA → CONTENT → CLOSING)",`);
    lines.push(`]`);
  }

  // Careful
  if (meta.careful.length > 0) {
    lines.push(`careful = [`);
    for (const c of meta.careful) {
      lines.push(`    "${escToml(c)}",`);
    }
    lines.push(`]`);
  } else {
    lines.push(`careful = [`);
    lines.push(`    "Adding new entries — must maintain consistency with related files",`);
    lines.push(`    "Modifying domain tables — affects all consumers",`);
    lines.push(`]`);
  }

  // Safe
  if (meta.safe.length > 0) {
    lines.push(`safe = [`);
    for (const s of meta.safe) {
      lines.push(`    "${escToml(s)}",`);
    }
    lines.push(`]`);
  } else {
    lines.push(`safe = [`);
    lines.push(`    "Comments for clarity",`);
    lines.push(`    "Additional metadata",`);
    lines.push(`    "Implementation notes",`);
    lines.push(`]`);
  }
  lines.push(``);

  lines.push(`[_closing.X2_extension]`);
  lines.push(`adding_data = [`);
  lines.push(`    "1. Add new entries following existing table structure",`);
  lines.push(`    "2. Ensure all required fields present",`);
  lines.push(`    "3. Verify consistency with related files",`);
  lines.push(`    "4. Run TOML validation",`);
  lines.push(`    "5. Run linter to verify alignment",`);
  lines.push(`]`);
  lines.push(``);

  lines.push(`[_closing.X3_troubleshooting]`);
  lines.push(`invalid_toml = "Missing quotes, duplicate keys, or bare keys with special characters"`);
  lines.push(`missing_reference = "Check all references point to valid sources"`);
  lines.push(`consistency_error = "Verify bidirectional consistency with related files"`);
  lines.push(``);

  lines.push(`[_closing.X4_reference]`);
  lines.push(`related_files = [`);
  // Add requires as related files
  for (const r of requirePaths) {
    lines.push(`    "${r}",`);
  }
  if (requirePaths.length === 0) {
    lines.push(`    "b-word/L0-universal/foundation/types/primitives.toml — Primitive types",`);
  }
  lines.push(`]`);
  const reqSections = ["_pragma", "_metadata", ...domainTables, "_validation", "_closing"];
  lines.push(`required_sections = [${reqSections.map((t) => `"${t}"`).join(", ")}]`);
  lines.push(`validate = "python3 -c \\"import tomllib; tomllib.load(open('${filename}','rb'))\\\""`);
  lines.push(``);

  lines.push(`[_closing.X5_note]`);
  const closingNoteText = meta.closingNote || `${meta.title} — foundation for the OmniCode system.`;
  lines.push(`note = "${escToml(closingNoteText)}"`);
  lines.push(`scripture = "${escToml(meta.closingScripture || meta.scripture || "In the beginning God created the heaven and the earth. — Genesis 1:1")}"`);
  lines.push(`anchor = "${escToml(closingNoteText.includes("Genesis") ? closingNoteText : closingNoteText + " Genesis 1:1 anchors everything.")}"`);
  lines.push(``);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(`# END CLOSING [END]`);
  lines.push(`# ═══════════════════════════════════════════════════════════════`);
  lines.push(``);

  return lines.join("\n");
}

// ─────────────────────────────────────────────────────────────────────────────
// Transform a single file
// ─────────────────────────────────────────────────────────────────────────────

async function transformFile(filepath: string): Promise<boolean> {
  try {
    const content = await Deno.readTextFile(filepath);

    // Skip already-aligned files
    if (isAlreadyAligned(content)) {
      if (VERBOSE) console.log(`  SKIP  ${filepath} — already aligned`);
      return true;
    }

    // Skip .original backup files
    if (filepath.endsWith(".original")) {
      if (VERBOSE) console.log(`  SKIP  ${filepath} — backup file`);
      return true;
    }

    const lines = content.split("\n");
    const bounds = findSections(lines);

    if (bounds.firstTable === -1) {
      console.log(`  SKIP  ${filepath} — no TOML tables found`);
      return true;
    }

    // Parse metadata from comments
    const meta = parseCommentMetadata(lines);

    // Extract domain content
    const domainContent = extractDomainContent(lines, bounds);

    // Derive layer info from path
    const layerInfo = deriveLayerInfo(filepath);

    if (VERBOSE) {
      console.log(`  Parsed: key=${meta.oldKey}, title="${meta.title}"`);
      console.log(`  Layer: ${layerInfo.layerName}/${layerInfo.subdir}`);
      console.log(`  Domain tables: ${detectDomainTables(fixUnderscorePrefix(domainContent)).join(", ")}`);
    }

    // Generate aligned output
    const aligned = generateAligned(meta, domainContent, layerInfo, filepath);

    // Validate TOML
    try {
      parseToml(aligned);
    } catch (e) {
      console.log(`  FAIL  ${filepath} — generated TOML invalid: ${(e as Error).message}`);
      if (VERBOSE) {
        // Show problematic area
        const errorLines = aligned.split("\n");
        const errMsg = (e as Error).message;
        const lineMatch = errMsg.match(/line (\d+)/);
        if (lineMatch) {
          const errLine = parseInt(lineMatch[1]);
          console.log(`  Near line ${errLine}:`);
          for (let i = Math.max(0, errLine - 3); i < Math.min(errorLines.length, errLine + 3); i++) {
            console.log(`    ${i + 1}: ${errorLines[i]}`);
          }
        }
      }
      return false;
    }

    const originalLineCount = lines.length;
    const alignedLineCount = aligned.split("\n").length;

    if (DRY_RUN) {
      console.log(`  WOULD  ${filepath}  ${originalLineCount} → ${alignedLineCount} lines`);
    } else {
      await Deno.writeTextFile(filepath, aligned);
      console.log(`  OK  ${filepath}  ${originalLineCount} → ${alignedLineCount} lines`);
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
    console.log("Usage: transform-toml.ts <file-or-dir> [--dry-run] [--verbose]");
    console.log("");
    console.log("Transforms old comment-based TOML files into 3-block aligned format.");
    console.log("Skips files that are already aligned (have [_pragma] tables).");
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
      // Collect all .toml files in directory (recursive)
      for await (const entry of Deno.readDir(target)) {
        if (entry.isFile && entry.name.endsWith(".toml") && !entry.name.endsWith(".original")) {
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

  console.log(`Transform TOML files → 3-block aligned format`);
  console.log(`Mode: ${DRY_RUN ? "DRY RUN (no writes)" : "LIVE (will overwrite files)"}`);
  console.log(`Files: ${files.length}`);
  console.log(`─────────────────────────────────────────────────────────────`);

  let success = 0;
  let failed = 0;
  let skipped = 0;

  for (const file of files) {
    const content = await Deno.readTextFile(file);
    if (isAlreadyAligned(content) || file.endsWith(".original")) {
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
