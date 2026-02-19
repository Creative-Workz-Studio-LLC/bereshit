#!/usr/bin/env -S deno run --allow-read
// ============================================================================
// METADATA
// ============================================================================
//
// file:    lint-seed.ts
// key:     B-tools-lint-seed
// title:   Seed Template Linter — Thin Consumer of cws-struct Engine
// type:    Tool (Linter)
// version: a-01.00
// created: 2026-02-18
// authors: Nova Dawn (CPI-SI)
// purpose: Cross-format seed template validator. Thin consumer that pulls from
//          the cws-struct engine to lint all seed templates and add seed-specific
//          value: coverage matrices, Go/Rust equivalence checks, template marker
//          detection. Generation 3: engine does the work, this tool does the thinking.
//
//          Repurposed from lint-toml.ts (generation 1 — standalone monolith).
//          lint-toml.ts is superseded by the TOML handler in cws-struct.
//          This tool is what lint-toml.ts becomes in a living system.
//
// biblical_foundation: "And let us not be weary in well doing: for in due
//   season we shall reap, if we faint not." — Galatians 6:9
//   Dead code reaps nothing. Repurposed code bears new fruit.
//
// usage:   deno run --allow-read lint-seed.ts [options]
//          deno task seed              # lint all seeds
//          deno task seed:code         # lint code seeds only
//          deno task seed:data         # lint data seeds only
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Imports — Engine (thin consumer pattern)
// ---------------------------------------------------------------------------

import { resolve, relative, basename, dirname } from "@std/path";
import { walk } from "@std/fs/walk";

// Foundation — types and result constructors
import type { LintSummary, LintResult as EngineResult } from "../cws-struct/lib/foundation/mod.ts";
import { summarize, info, warn } from "../cws-struct/lib/foundation/mod.ts";

// Engine — discovery, output, registry
import {
  COLORS,
  printFileSummary,
  printTotals,
} from "../cws-struct/lib/engine/mod.ts";
import {
  getFormat,
  detectFormat,
} from "../cws-struct/lib/engine/mod.ts";

// Register all format handlers (side-effect imports)
// These are the engine's handlers — lint-seed.ts doesn't reimplement them.
import "../cws-struct/lib/handlers/go.ts";
import "../cws-struct/lib/handlers/rust.ts";
import "../cws-struct/lib/handlers/toml.ts";
import "../cws-struct/lib/handlers/json.ts";
import "../cws-struct/lib/handlers/makefile.ts";
import "../cws-struct/lib/handlers/dotfiles.ts";
import "../cws-struct/lib/handlers/omni.ts";
import "../cws-struct/lib/handlers/ofd.ts";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const VERSION = "a-01.00";
const TOOL_NAME = "lint-seed";

/** Seed template directories — resolved relative to this file's location. */
const SEED_ROOT = resolve(dirname(new URL(import.meta.url).pathname), "../../b-word/seed");
const CODE_SEEDS = resolve(SEED_ROOT, "code/L0");
const DATA_SEEDS = resolve(SEED_ROOT, "data/L0");

/**
 * Expected parallel structures between languages.
 * Go and Rust should have matching template sets — if Go has library/executable/demo-test,
 * Rust should too. This is the cross-format equivalence check.
 */
const CODE_EQUIVALENCES: Record<string, Record<string, string>> = {
  "go":   { library: "library.go",   executable: "executable.go",   test: "demo-test.go" },
  "rust": { library: "library.rs",   executable: "executable.rs",   test: "demo-test.rs" },
};

/**
 * Expected metadata files per seed directory.
 * Each seed language directory should have an .omni manifest.
 * Go and Rust additionally have .ofd (folder descriptor).
 */
const EXPECTED_METADATA: Record<string, string[]> = {
  "go":   ["root.omni", "root.ofd"],
  "rust": ["root.omni", "root.ofd"],
  "c":    ["root.omni"],
  "asm":  ["root.omni"],
  "cmake":  ["root.omni"],
  "linker": ["root.omni"],
};

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

interface SeedEntry {
  /** Language/format directory name (e.g., "go", "rust", "dotfiles") */
  language: string;
  /** Full file path */
  path: string;
  /** File name */
  name: string;
  /** Whether this is a code seed (vs data seed) */
  isCode: boolean;
}


// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Seed Discovery
// ---------------------------------------------------------------------------

/**
 * Walk a seed root (code or data) and collect all files, grouped by language.
 */
async function discoverSeeds(root: string, isCode: boolean): Promise<SeedEntry[]> {
  const entries: SeedEntry[] = [];

  try {
    await Deno.stat(root);
  } catch {
    return entries;
  }

  for await (const entry of walk(root, { maxDepth: 4 })) {
    if (!entry.isFile) continue;

    // Derive language from first directory under L0/
    const rel = relative(root, entry.path);
    const parts = rel.split("/");
    if (parts.length < 2) continue; // skip files directly in L0/

    const language = parts[0]!;
    entries.push({
      language,
      path: entry.path,
      name: basename(entry.path),
      isCode,
    });
  }

  return entries;
}

// ---------------------------------------------------------------------------
// Cross-Format Checks — the seed-specific value
// ---------------------------------------------------------------------------

/**
 * Check Go ↔ Rust template equivalence.
 *
 * If Go has library/executable/demo-test templates, Rust should too.
 * Reports mismatches as warnings — not errors, because languages may
 * legitimately differ (e.g., C doesn't have demo-test.c yet).
 */
function checkEquivalence(entries: SeedEntry[]): EngineResult[] {
  const results: EngineResult[] = [];

  // Group code seeds by language
  const byLang = new Map<string, Set<string>>();
  for (const e of entries) {
    if (!e.isCode) continue;
    if (!byLang.has(e.language)) byLang.set(e.language, new Set());
    byLang.get(e.language)!.add(e.name);
  }

  // Check equivalences between paired languages
  const pairs = Object.keys(CODE_EQUIVALENCES);
  for (let i = 0; i < pairs.length; i++) {
    for (let j = i + 1; j < pairs.length; j++) {
      const langA = pairs[i]!;
      const langB = pairs[j]!;
      const filesA = byLang.get(langA);
      const filesB = byLang.get(langB);

      if (!filesA || !filesB) continue;

      const equivA = CODE_EQUIVALENCES[langA]!;
      const equivB = CODE_EQUIVALENCES[langB]!;

      for (const role of Object.keys(equivA)) {
        const fileA = equivA[role]!;
        const fileB = equivB[role];

        if (!fileB) continue; // role doesn't apply to langB

        const hasA = filesA.has(fileA);
        const hasB = filesB.has(fileB);

        if (hasA && !hasB) {
          results.push(warn(`seed/${langB}`, "equivalence/missing",
            `${langA} has ${fileA} but ${langB} is missing ${fileB} (role: ${role})`));
        } else if (!hasA && hasB) {
          results.push(warn(`seed/${langA}`, "equivalence/missing",
            `${langB} has ${fileB} but ${langA} is missing ${fileA} (role: ${role})`));
        }
      }
    }
  }

  return results;
}

/**
 * Check that expected metadata files exist in each language directory.
 */
function checkMetadataPresence(entries: SeedEntry[]): EngineResult[] {
  const results: EngineResult[] = [];

  // Group by language
  const byLang = new Map<string, Set<string>>();
  for (const e of entries) {
    if (!e.isCode) continue;
    if (!byLang.has(e.language)) byLang.set(e.language, new Set());
    byLang.get(e.language)!.add(e.name);
  }

  for (const [lang, expected] of Object.entries(EXPECTED_METADATA)) {
    const files = byLang.get(lang);
    if (!files) continue;

    for (const meta of expected) {
      if (!files.has(meta)) {
        results.push(info(`seed/${lang}`, "metadata/missing",
          `Expected metadata file ${meta} not found in ${lang}/ seeds`));
      }
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Coverage Matrix
// ---------------------------------------------------------------------------

/**
 * Print a visual coverage matrix showing which languages have which template types.
 */
function printCoverageMatrix(entries: SeedEntry[]): void {
  // Group by language, separate code and data
  const codeLangs = new Map<string, string[]>();
  const dataLangs = new Map<string, string[]>();

  for (const e of entries) {
    const map = e.isCode ? codeLangs : dataLangs;
    if (!map.has(e.language)) map.set(e.language, []);
    map.get(e.language)!.push(e.name);
  }

  console.log(`\n${COLORS.bold}Seed Coverage Matrix${COLORS.reset}\n`);

  if (codeLangs.size > 0) {
    console.log(`${COLORS.bold}  Code Seeds (${CODE_SEEDS})${COLORS.reset}`);
    const sorted = [...codeLangs.entries()].sort((a, b) => a[0].localeCompare(b[0]));
    for (const [lang, files] of sorted) {
      const count = files.length;
      const fileList = files.sort().join(", ");
      console.log(`    ${COLORS.bold}${lang}${COLORS.reset} ${COLORS.dim}(${count})${COLORS.reset}: ${fileList}`);
    }
  }

  if (dataLangs.size > 0) {
    console.log(`\n${COLORS.bold}  Data Seeds (${DATA_SEEDS})${COLORS.reset}`);
    const sorted = [...dataLangs.entries()].sort((a, b) => a[0].localeCompare(b[0]));
    for (const [lang, files] of sorted) {
      const count = files.length;
      const fileList = files.sort().join(", ");
      console.log(`    ${COLORS.bold}${lang}${COLORS.reset} ${COLORS.dim}(${count})${COLORS.reset}: ${fileList}`);
    }
  }

  const totalLangs = new Set([...codeLangs.keys(), ...dataLangs.keys()]).size;
  const totalFiles = entries.length;
  console.log(`\n  ${COLORS.dim}${totalLangs} languages, ${totalFiles} seed files${COLORS.reset}`);
}

// ---------------------------------------------------------------------------
// Lint Orchestrator — the thin part
// ---------------------------------------------------------------------------

/**
 * Lint a single seed file using the engine's registered format handler.
 * Returns null if no handler matches (the file is outside engine scope).
 */
async function lintSeedFile(entry: SeedEntry): Promise<LintSummary | null> {
  const formatName = detectFormat(entry.path);
  if (!formatName) return null;

  const handler = getFormat(formatName);
  if (!handler) return null;

  const results = await handler.lint(entry.path);
  const health = handler.computeHealth
    ? await handler.computeHealth(entry.path, results)
    : undefined;

  return summarize(relative(SEED_ROOT, entry.path), results, health);
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// CLI
// ---------------------------------------------------------------------------

function showHelp(): void {
  console.log(`
${COLORS.bold}${TOOL_NAME}${COLORS.reset} v${VERSION} — Seed Template Linter

${COLORS.dim}"And let us not be weary in well doing: for in due season we shall reap."${COLORS.reset}
${COLORS.dim}— Galatians 6:9${COLORS.reset}

Thin consumer of the cws-struct engine. Validates all seed templates
with cross-format equivalence checks and coverage reporting.

${COLORS.bold}Usage:${COLORS.reset}
  deno run --allow-read lint-seed.ts [options]

${COLORS.bold}Options:${COLORS.reset}
  --code          Lint code seeds only (code/L0/)
  --data          Lint data seeds only (data/L0/)
  --verbose, -v   Show all results including info
  --errors-only   Show only errors
  --summary       Show only file-level summary
  --matrix        Show coverage matrix only (skip linting)
  --help, -h      Show this help

${COLORS.bold}Examples:${COLORS.reset}
  deno task seed              ${COLORS.dim}# lint all seeds${COLORS.reset}
  deno task seed:verbose      ${COLORS.dim}# full detail${COLORS.reset}
  deno task seed:code         ${COLORS.dim}# code seeds only${COLORS.reset}
  deno task seed:data         ${COLORS.dim}# data seeds only${COLORS.reset}
`);
}

async function main(): Promise<void> {
  const args = Deno.args;

  if (args.includes("--help") || args.includes("-h")) {
    showHelp();
    Deno.exit(0);
  }

  const verbose = args.includes("--verbose") || args.includes("-v");
  const errorsOnly = args.includes("--errors-only");
  const summaryOnly = args.includes("--summary");
  const matrixOnly = args.includes("--matrix");
  const codeOnly = args.includes("--code");
  const dataOnly = args.includes("--data");

  // --- Discover seeds ---
  const entries: SeedEntry[] = [];

  if (!dataOnly) {
    entries.push(...await discoverSeeds(CODE_SEEDS, true));
  }
  if (!codeOnly) {
    entries.push(...await discoverSeeds(DATA_SEEDS, false));
  }

  if (entries.length === 0) {
    console.error(`${COLORS.red}No seed files found.${COLORS.reset}`);
    console.error(`Expected seeds at: ${SEED_ROOT}`);
    Deno.exit(1);
  }

  // --- Coverage matrix (always shown, or alone with --matrix) ---
  printCoverageMatrix(entries);

  if (matrixOnly) {
    Deno.exit(0);
  }

  // --- Cross-format checks ---
  const crossResults: EngineResult[] = [];
  crossResults.push(...checkEquivalence(entries));
  crossResults.push(...checkMetadataPresence(entries));

  if (crossResults.length > 0) {
    console.log(`\n${COLORS.bold}Cross-Format Checks${COLORS.reset}\n`);
    for (const r of crossResults) {
      const color = r.severity === "error" ? COLORS.red
        : r.severity === "warn" ? COLORS.yellow
        : COLORS.blue;
      const icon = r.severity === "error" ? "x"
        : r.severity === "warn" ? "!"
        : "i";
      console.log(`  ${color}${icon}${COLORS.reset} ${COLORS.dim}[${r.rule}]${COLORS.reset} ${r.file}: ${r.message}`);
    }
  }

  // --- Engine lint — dispatch each file to its handler ---
  console.log(`\n${COLORS.bold}Linting ${entries.length} seed files...${COLORS.reset}\n`);

  const summaries: LintSummary[] = [];
  let skipped = 0;

  for (const entry of entries) {
    const summary = await lintSeedFile(entry);
    if (!summary) {
      skipped++;
      continue;
    }
    summaries.push(summary);

    if (!summaryOnly) {
      if (errorsOnly) {
        if (summary.errors > 0) printFileSummary(summary, verbose);
      } else {
        printFileSummary(summary, verbose);
      }
    }
  }

  if (summaryOnly) {
    for (const s of summaries) {
      const status = s.errors === 0
        ? `${COLORS.green}OK${COLORS.reset}`
        : `${COLORS.red}FAIL${COLORS.reset}`;
      console.log(`${status}  ${s.file}  (${s.errors}E ${s.warnings}W ${s.infos}I)`);
    }
  }

  // --- Totals ---
  printTotals(summaries);

  if (skipped > 0) {
    console.log(`${COLORS.dim}${skipped} file(s) skipped (no registered handler)${COLORS.reset}`);
  }

  // Exit code
  const hasErrors = summaries.some((s) => s.errors > 0);
  Deno.exit(hasErrors ? 1 : 0);
}

main().catch((err: unknown) => {
  console.error(
    `${COLORS.red}${TOOL_NAME}: ${err instanceof Error ? err.message : String(err)}${COLORS.reset}`,
  );
  Deno.exit(2);
});
