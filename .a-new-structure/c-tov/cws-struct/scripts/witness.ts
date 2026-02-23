// ============================================================================
// METADATA
// ============================================================================
//
// file:    scripts/witness.ts
// key:     B-tov-cws-struct-scripts-witness
// title:   CWS Struct — Dimensional Witness Coverage Analyzer
// type:    Code (Script)
// version: a-02.00
// created: 2026-02-23
// updated: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// purpose: Static analysis of test-to-error-code coverage with dimensional
//          navigation. Greps test files for [CWS-*] bracket tags and CWS-*
//          references, maps against the error catalog, reports coverage in
//          the 3D grid: Layer (Wisdom) x Domain (Understanding) x Severity.
//
//          Usage:
//            deno run --allow-read scripts/witness.ts [options]
//
//          Options:
//            --verbose, -v       Show unwitnessed codes grouped by layer
//            --grid              Show layer x domain coverage grid
//            --layer <R50|R25|R10|R05|T00>   Filter by layer
//            --domain <BLK|IDE|CON|...>      Filter by domain code
//            --files             Show which files witness each code
//
//          Examples:
//            witness              Summary only
//            witness --grid       Dimensional grid (layer x domain)
//            witness --layer R25  Coverage for R25 only
//            witness --domain IDE All identity codes across layers
//            witness --files -v   Verbose + file locations
//
// biblical_foundation: "Through wisdom is an house builded; and by
//   understanding it is established: And by knowledge shall the chambers
//   be filled with all precious and pleasant riches." — Proverbs 24:3-4
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Imports
// ---------------------------------------------------------------------------

import {
  ERROR_CODES,
  categoryToDomain,
  dimensionalCode,
  CATEGORY_DOMAIN,
  LAYER_COUNTS,
  CATALOG_SIZE,
} from "../lib/data/errors.ts";
import type { ErrorCodeEntry } from "../lib/data/types.ts";
import { walk } from "jsr:@std/fs/walk";

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

interface WitnessEntry {
  code: string;
  dimensional: string;
  file: string;
  line: number;
  context: string;
  type: "bracket" | "assertion" | "reference";
}

interface DimensionalReport {
  /** Total error codes in catalog. */
  totalCodes: number;
  /** Codes with at least one bracket-tagged witness. */
  bracketWitnessed: string[];
  /** Codes with assertion coverage but no bracket tag. */
  assertionOnly: string[];
  /** Codes referenced but not asserted or tagged. */
  referenceOnly: string[];
  /** Codes with no test reference at all. */
  unwitnessed: string[];
  /** All witness entries found. */
  entries: WitnessEntry[];
  /** Layer x Domain coverage grid. */
  grid: Map<string, Map<string, GridCell>>;
}

interface GridCell {
  total: number;
  witnessed: number;
  codes: string[];           // flat codes
  witnessedCodes: string[];  // flat codes that have witnesses
}

// ---------------------------------------------------------------------------
// CLI Args
// ---------------------------------------------------------------------------

interface CliOptions {
  verbose: boolean;
  grid: boolean;
  files: boolean;
  layer: string | null;
  domain: string | null;
}

function parseArgs(args: string[]): CliOptions {
  const opts: CliOptions = {
    verbose: false,
    grid: false,
    files: false,
    layer: null,
    domain: null,
  };

  for (let i = 0; i < args.length; i++) {
    switch (args[i]) {
      case "--verbose":
      case "-v":
        opts.verbose = true;
        break;
      case "--grid":
        opts.grid = true;
        break;
      case "--files":
        opts.files = true;
        break;
      case "--layer":
        opts.layer = args[++i]?.toUpperCase() ?? null;
        break;
      case "--domain":
        opts.domain = args[++i]?.toUpperCase() ?? null;
        break;
    }
  }

  return opts;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Scan test files for error code references
// ---------------------------------------------------------------------------

/** Scan a single test file for CWS error code references. */
async function scanFile(path: string): Promise<WitnessEntry[]> {
  const text = await Deno.readTextFile(path);
  const lines = text.split("\n");
  const entries: WitnessEntry[] = [];

  const bracketPattern = /\[CWS-[RT]\d{2}-\d{3}\]/g;
  const assertPattern = /(?:hasErrorCode|byErrorCode|getByCode|matchRule|resolveErrorCode).*?(CWS-[RT]\d{2}-\d{3})/g;
  const referencePattern = /CWS-[RT]\d{2}-\d{3}/g;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];

    // Bracket tags in test names (strongest signal)
    for (const match of line.matchAll(bracketPattern)) {
      const code = match[0].slice(1, -1); // Remove brackets
      const entry = ERROR_CODES.get(code);
      const dim = entry ? dimensionalCode(entry) : "???.???.???";
      entries.push({ code, dimensional: dim, file: path, line: i + 1, context: line.trim(), type: "bracket" });
    }

    // Assertion calls (strong signal — test verifies this code)
    for (const match of line.matchAll(assertPattern)) {
      const code = match[1];
      const entry = ERROR_CODES.get(code);
      const dim = entry ? dimensionalCode(entry) : "???.???.???";
      entries.push({ code, dimensional: dim, file: path, line: i + 1, context: line.trim(), type: "assertion" });
    }

    // General references (weak signal — may be comment or comparison)
    for (const match of line.matchAll(referencePattern)) {
      const code = match[0];
      // Skip if already caught by bracket or assertion
      const alreadyCaught = entries.some(
        (e) => e.line === i + 1 && e.code === code && (e.type === "bracket" || e.type === "assertion"),
      );
      if (!alreadyCaught) {
        const entry = ERROR_CODES.get(code);
        const dim = entry ? dimensionalCode(entry) : "???.???.???";
        entries.push({ code, dimensional: dim, file: path, line: i + 1, context: line.trim(), type: "reference" });
      }
    }
  }

  return entries;
}

// ---------------------------------------------------------------------------
// 2. Build dimensional coverage grid
// ---------------------------------------------------------------------------

function buildGrid(
  allEntries: [string, ErrorCodeEntry][],
  witnessedCodes: Set<string>,
): Map<string, Map<string, GridCell>> {
  const grid = new Map<string, Map<string, GridCell>>();

  // Layer order for display
  const layers = ["R50", "R25", "R10", "R05", "T00"];
  for (const layer of layers) {
    grid.set(layer, new Map());
  }

  for (const [_, entry] of allEntries) {
    const layer = entry.layerName;
    const domain = categoryToDomain(entry.category);

    if (!grid.has(layer)) grid.set(layer, new Map());
    const layerMap = grid.get(layer)!;

    if (!layerMap.has(domain)) {
      layerMap.set(domain, { total: 0, witnessed: 0, codes: [], witnessedCodes: [] });
    }

    const cell = layerMap.get(domain)!;
    cell.total++;
    cell.codes.push(entry.code);

    if (witnessedCodes.has(entry.code)) {
      cell.witnessed++;
      cell.witnessedCodes.push(entry.code);
    }
  }

  return grid;
}

// ---------------------------------------------------------------------------
// 3. Aggregate and report
// ---------------------------------------------------------------------------

async function buildReport(opts: CliOptions): Promise<DimensionalReport> {
  // Collect all codes from catalog
  const allEntries: [string, ErrorCodeEntry][] = [...ERROR_CODES.entries()];
  const allCodes = new Set<string>(allEntries.map(([_, e]) => e.code));

  const entries: WitnessEntry[] = [];

  // Walk all test files
  for await (const entry of walk("tests/", {
    exts: [".ts"],
    match: [/_test\.ts$/],
  })) {
    const fileEntries = await scanFile(entry.path);
    entries.push(...fileEntries);
  }

  // Also scan root-level test files
  for await (const entry of walk("tests/", {
    exts: [".ts"],
    match: [/^tests\/[^/]+\.ts$/],
    maxDepth: 1,
  })) {
    if (entry.path.endsWith("_test.ts") || entry.path.endsWith("helpers.ts")) continue; // Already caught
    const fileEntries = await scanFile(entry.path);
    entries.push(...fileEntries);
  }

  // Classify codes by strongest signal
  const bracketCodes = new Set<string>();
  const assertionCodes = new Set<string>();
  const referenceCodes = new Set<string>();

  for (const e of entries) {
    if (e.type === "bracket") bracketCodes.add(e.code);
    else if (e.type === "assertion") assertionCodes.add(e.code);
    else referenceCodes.add(e.code);
  }

  // Filter by layer/domain if requested
  let filteredCodes = allCodes;
  if (opts.layer || opts.domain) {
    filteredCodes = new Set<string>();
    for (const code of allCodes) {
      const entry = ERROR_CODES.get(code);
      if (!entry) continue;
      if (opts.layer && entry.layerName !== opts.layer) continue;
      if (opts.domain && categoryToDomain(entry.category) !== opts.domain) continue;
      filteredCodes.add(code);
    }
  }

  const bracketWitnessed = [...filteredCodes].filter((c) => bracketCodes.has(c));
  const assertionOnly = [...filteredCodes].filter(
    (c) => !bracketCodes.has(c) && assertionCodes.has(c),
  );
  const referenceOnly = [...filteredCodes].filter(
    (c) => !bracketCodes.has(c) && !assertionCodes.has(c) && referenceCodes.has(c),
  );
  const unwitnessed = [...filteredCodes].filter(
    (c) => !bracketCodes.has(c) && !assertionCodes.has(c) && !referenceCodes.has(c),
  );

  // Build witnessed set for grid
  const allWitnessed = new Set<string>([...bracketCodes, ...assertionCodes, ...referenceCodes]);
  const grid = buildGrid(allEntries, allWitnessed);

  return {
    totalCodes: filteredCodes.size,
    bracketWitnessed,
    assertionOnly,
    referenceOnly,
    unwitnessed,
    entries,
    grid,
  };
}

// ---------------------------------------------------------------------------
// 4. Output formatting
// ---------------------------------------------------------------------------

function pad(s: string | number, width: number): string {
  return String(s).padStart(width);
}

function rpad(s: string, width: number): string {
  return String(s).padEnd(width);
}

function printReport(report: DimensionalReport, opts: CliOptions): void {
  const { totalCodes, bracketWitnessed, assertionOnly, referenceOnly, unwitnessed, grid, entries } = report;

  // Header
  console.log("╔══════════════════════════════════════════════════════════════╗");
  console.log("║          CWS Struct — Witness Coverage Report               ║");
  console.log("║          Proverbs 24:3-4 — Wisdom x Understanding x Knowledge ║");
  console.log("╚══════════════════════════════════════════════════════════════╝");
  console.log();

  // Filter notice
  if (opts.layer || opts.domain) {
    const filters: string[] = [];
    if (opts.layer) filters.push(`layer=${opts.layer}`);
    if (opts.domain) filters.push(`domain=${opts.domain}`);
    console.log(`  Filter: ${filters.join(", ")}`);
    console.log();
  }

  // Summary
  const tagged = bracketWitnessed.length;
  const asserted = assertionOnly.length;
  const referenced = referenceOnly.length;
  const gaps = unwitnessed.length;

  console.log(`  Total codes:          ${totalCodes}`);
  console.log(`  [Bracket] tagged:     ${tagged}  (strongest — test name includes [CWS-xxx])`);
  console.log(`  Assertion covered:    ${asserted}  (test asserts error code in body)`);
  console.log(`  Reference only:       ${referenced}  (code mentioned but not asserted)`);
  console.log(`  Unwitnessed:          ${gaps}  (no test reference at all)`);
  console.log();

  const coverage = totalCodes > 0 ? ((tagged + asserted) / totalCodes * 100).toFixed(1) : "0.0";
  console.log(`  Coverage: ${coverage}% (${tagged + asserted}/${totalCodes} codes tested)`);
  console.log();

  // Grid view — layer x domain
  if (opts.grid) {
    printGrid(grid, opts);
  }

  // Verbose — unwitnessed details
  if (opts.verbose) {
    if (assertionOnly.length > 0) {
      console.log("── Assertion-covered (upgrade to bracket tags) ────────────────");
      for (const code of assertionOnly.sort()) {
        const entry = ERROR_CODES.get(code);
        if (!entry) continue;
        const dim = dimensionalCode(entry);
        console.log(`  ${code}  ${dim}  (${entry.category})`);
      }
      console.log();
    }

    if (unwitnessed.length > 0) {
      console.log("── Unwitnessed codes (dimensional) ───────────────────────────");
      // Group by layer, then by domain within layer
      const byLayerDomain = new Map<string, Map<string, string[]>>();
      for (const code of unwitnessed.sort()) {
        const entry = ERROR_CODES.get(code);
        if (!entry) continue;
        const layer = entry.layerName;
        const domain = categoryToDomain(entry.category);
        const dim = dimensionalCode(entry);

        if (!byLayerDomain.has(layer)) byLayerDomain.set(layer, new Map());
        const layerMap = byLayerDomain.get(layer)!;
        if (!layerMap.has(domain)) layerMap.set(domain, []);
        layerMap.get(domain)!.push(dim);
      }

      for (const [layer, domains] of byLayerDomain) {
        console.log(`  ${layer}:`);
        for (const [domain, codes] of [...domains].sort(([a], [b]) => a.localeCompare(b))) {
          console.log(`    ${domain}: ${codes.join(", ")}`);
        }
      }
      console.log();
    }
  } else {
    if (gaps > 0) {
      console.log(`  Run with --verbose to see unwitnessed codes`);
      console.log(`  Run with --grid to see dimensional coverage grid`);
      console.log();
    }
  }

  // File locations for witnessed codes
  if (opts.files) {
    printFileLocations(entries, opts);
  }

  // Final status
  if (gaps > 0) {
    console.log(`  ${gaps} error codes have no test coverage`);
  } else {
    console.log(`  All error codes have test coverage`);
  }
}

// ---------------------------------------------------------------------------
// 4a. Grid display — the dimensional view
// ---------------------------------------------------------------------------

function printGrid(grid: Map<string, Map<string, GridCell>>, opts: CliOptions): void {
  // Collect all domains across all layers
  const allDomains = new Set<string>();
  for (const [_, layerMap] of grid) {
    for (const [domain] of layerMap) {
      allDomains.add(domain);
    }
  }
  const domains = [...allDomains].sort();

  // Filter domains if requested
  const displayDomains = opts.domain
    ? domains.filter((d) => d === opts.domain)
    : domains;

  if (displayDomains.length === 0) {
    console.log("  No domains match filter.");
    console.log();
    return;
  }

  // Filter layers if requested
  const layers = opts.layer
    ? [opts.layer]
    : ["R50", "R25", "R10", "R05", "T00"];

  console.log("── Coverage Grid (Layer x Domain) ─────────────────────────────");
  console.log();

  // Column widths: domain code is 3 chars, cells are "n/m" format
  const colWidth = 7;
  const labelWidth = 5;

  // Header row
  let header = "  " + rpad("", labelWidth) + " ";
  for (const domain of displayDomains) {
    header += pad(domain, colWidth);
  }
  header += pad("TOTAL", colWidth + 1);
  console.log(header);

  // Separator
  const sepLen = labelWidth + 2 + displayDomains.length * colWidth + colWidth + 1;
  console.log("  " + "─".repeat(sepLen));

  // Data rows
  for (const layer of layers) {
    const layerMap = grid.get(layer);
    if (!layerMap) continue;

    let row = "  " + rpad(layer, labelWidth) + " ";
    let layerTotal = 0;
    let layerWitnessed = 0;

    for (const domain of displayDomains) {
      const cell = layerMap.get(domain);
      if (cell) {
        const cellStr = `${cell.witnessed}/${cell.total}`;
        row += pad(cellStr, colWidth);
        layerTotal += cell.total;
        layerWitnessed += cell.witnessed;
      } else {
        row += pad("·", colWidth);
      }
    }

    const totalStr = `${layerWitnessed}/${layerTotal}`;
    row += " " + pad(totalStr, colWidth);

    // Add coverage percent
    const pct = layerTotal > 0 ? ((layerWitnessed / layerTotal) * 100).toFixed(0) : "—";
    row += ` (${pct}%)`;

    console.log(row);
  }

  // Domain totals row
  let totalsRow = "  " + rpad("TOTAL", labelWidth) + " ";
  let grandTotal = 0;
  let grandWitnessed = 0;

  for (const domain of displayDomains) {
    let domTotal = 0;
    let domWit = 0;
    for (const layer of layers) {
      const cell = grid.get(layer)?.get(domain);
      if (cell) {
        domTotal += cell.total;
        domWit += cell.witnessed;
      }
    }
    totalsRow += pad(`${domWit}/${domTotal}`, colWidth);
    grandTotal += domTotal;
    grandWitnessed += domWit;
  }

  totalsRow += " " + pad(`${grandWitnessed}/${grandTotal}`, colWidth);
  const grandPct = grandTotal > 0 ? ((grandWitnessed / grandTotal) * 100).toFixed(0) : "—";
  totalsRow += ` (${grandPct}%)`;

  console.log("  " + "─".repeat(sepLen));
  console.log(totalsRow);
  console.log();

  // Legend
  console.log("  Grid key: witnessed/total  ·=no codes in cell");
  console.log("  Domains: " + displayDomains.map((d) => {
    const cats = Object.entries(CATEGORY_DOMAIN).filter(([_, v]) => v === d).map(([k]) => k);
    return `${d}=${cats[0] ?? "?"}`;
  }).join("  "));
  console.log();
}

// ---------------------------------------------------------------------------
// 4b. File locations
// ---------------------------------------------------------------------------

function printFileLocations(entries: WitnessEntry[], opts: CliOptions): void {
  console.log("── File Locations ────────────────────────────────────────────");
  console.log();

  // Group by code
  const byCode = new Map<string, WitnessEntry[]>();
  for (const e of entries) {
    if (opts.layer) {
      const entry = ERROR_CODES.get(e.code);
      if (entry && entry.layerName !== opts.layer) continue;
    }
    if (opts.domain) {
      const entry = ERROR_CODES.get(e.code);
      if (entry && categoryToDomain(entry.category) !== opts.domain) continue;
    }
    if (!byCode.has(e.code)) byCode.set(e.code, []);
    byCode.get(e.code)!.push(e);
  }

  for (const [code, witnesses] of [...byCode].sort(([a], [b]) => a.localeCompare(b))) {
    const entry = ERROR_CODES.get(code);
    const dim = entry ? dimensionalCode(entry) : "???.???.???";
    console.log(`  ${code}  ${dim}`);
    for (const w of witnesses) {
      const signal = w.type === "bracket" ? "[B]" : w.type === "assertion" ? "[A]" : "[R]";
      const shortPath = w.file.replace(/^tests\//, "");
      console.log(`    ${signal} ${shortPath}:${w.line}`);
    }
  }
  console.log();
}

// ---------------------------------------------------------------------------
// 5. Main
// ---------------------------------------------------------------------------

const opts = parseArgs(Deno.args);
const report = await buildReport(opts);
printReport(report, opts);

// Set exit code based on coverage gaps
if (report.unwitnessed.length > 0) {
  Deno.exit(1);
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Dimensional Witness Coverage Analyzer — 3D queryable coverage reporting.
//
// Three dimensions (Proverbs 24:3-4):
//   Layer (Wisdom):        R50, R25, R10, R05, T00
//   Domain (Understanding): SYS, DIR, BLK, IDE, CON, ...
//   Severity:              error, warn, info
//
// Static analysis. No test execution needed. Complements the runtime
// WITNESS_REGISTRY in helpers.ts. Both tell the same story from different
// angles — this one doesn't require running tests, just reading them.
//
// "Through wisdom is an house builded; and by understanding it is
//  established: And by knowledge shall the chambers be filled with all
//  precious and pleasant riches." — Proverbs 24:3-4
// ============================================================================
