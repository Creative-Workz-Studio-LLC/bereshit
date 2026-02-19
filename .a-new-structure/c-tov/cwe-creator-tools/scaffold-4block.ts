#!/usr/bin/env -S deno run --allow-read --allow-write
// ============================================================================
// METADATA
// ============================================================================
//
// file:    scaffold-4block.ts
// key:     B-tools-scaffold-4block
// title:   4-Block Code File Scaffolder — Thin Consumer of cws-struct Engine
// type:    Tool (Scaffolder)
// version: a-01.00
// created: 2026-02-18
// authors: Nova Dawn (CPI-SI)
// purpose: Create new 4-block code files (Go, Rust, C) from seed templates.
//          Thin consumer that: (1) copies from the right seed template,
//          (2) substitutes metadata from CLI args, (3) verifies via the engine.
//
//          Repurposed from transform-toml.ts (generation 1 — migration transformer).
//          transform-toml.ts converted old → new format. That migration is done.
//          This tool creates NEW files — the next life of that code.
//
// biblical_foundation: "For we are his workmanship, created in Christ Jesus
//   unto good works, which God hath before ordained that we should walk in them."
//   — Ephesians 2:10
//   From migration to creation — the work that was prepared beforehand.
//
// usage:
//   deno run --allow-read --allow-write scaffold-4block.ts <lang> <dest> [options]
//   scaffold-4block.ts go   src/server.go     --key B-server --title "Main Server"
//   scaffold-4block.ts rust src/lib.rs        --key B-lib --title "Core Library"
//   scaffold-4block.ts go   pkg/auth/auth.go  --role library --purpose "Auth module"
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Imports — Engine (thin consumer pattern)
// ---------------------------------------------------------------------------

import { resolve, dirname, basename, relative } from "@std/path";

// Foundation — types and result constructors
import type { LintSummary } from "../cws-struct/lib/foundation/mod.ts";
import { summarize } from "../cws-struct/lib/foundation/mod.ts";

// Engine — output, registry
import { COLORS, printFileSummary } from "../cws-struct/lib/engine/mod.ts";
import { getFormat, detectFormat } from "../cws-struct/lib/engine/mod.ts";

// Register handlers for post-scaffold verification
import "../cws-struct/lib/handlers/go.ts";
import "../cws-struct/lib/handlers/rust.ts";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const VERSION = "a-01.00";
const TOOL_NAME = "scaffold-4block";
const TODAY = new Date().toISOString().split("T")[0]!;

/** Seed templates — resolved relative to this file. */
const SEED_ROOT = resolve(dirname(new URL(import.meta.url).pathname), "../../b-word/seed/code/L0");

/** Supported languages and their seed template mapping. */
const TEMPLATES: Record<string, { library: string; executable: string; test: string }> = {
  go: {
    library:    resolve(SEED_ROOT, "go/library.go"),
    executable: resolve(SEED_ROOT, "go/executable.go"),
    test:       resolve(SEED_ROOT, "go/demo-test.go"),
  },
  rust: {
    library:    resolve(SEED_ROOT, "rust/library.rs"),
    executable: resolve(SEED_ROOT, "rust/executable.rs"),
    test:       resolve(SEED_ROOT, "rust/demo-test.rs"),
  },
};

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

interface ScaffoldOptions {
  language: string;
  dest: string;
  role: "library" | "executable" | "test";
  key: string;
  title: string;
  purpose: string;
  version: string;
  dryRun: boolean;
  verify: boolean;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Template Reading and Substitution
// ---------------------------------------------------------------------------

/**
 * Read a seed template and substitute metadata placeholders.
 *
 * Seed templates use a consistent set of placeholder values that mark
 * "this needs to be replaced." We find them and swap with user-provided values.
 */
async function scaffoldFromTemplate(opts: ScaffoldOptions): Promise<string> {
  const templates = TEMPLATES[opts.language];
  if (!templates) {
    throw new Error(`Unsupported language: ${opts.language}. Supported: ${Object.keys(TEMPLATES).join(", ")}`);
  }

  const templatePath = templates[opts.role];
  let content: string;
  try {
    content = await Deno.readTextFile(templatePath);
  } catch (e) {
    throw new Error(`Cannot read template ${templatePath}: ${e}`);
  }

  // --- Metadata substitutions ---
  // Seed templates have specific metadata values that need replacement.
  // We replace key, title, purpose, version, file path, and dates.

  const destBasename = basename(opts.dest);

  // Replace key (in metadata comment block)
  content = content.replace(
    /^(\/\/\s*key:\s+).*$/m,
    `$1${opts.key}`,
  );

  // Replace title
  content = content.replace(
    /^(\/\/\s*title:\s+).*$/m,
    `$1${opts.title}`,
  );

  // Replace purpose (may be multi-line, replace first line)
  content = content.replace(
    /^(\/\/\s*purpose:\s+).*$/m,
    `$1${opts.purpose}`,
  );

  // Replace version
  content = content.replace(
    /^(\/\/\s*version:\s+).*$/m,
    `$1${opts.version}`,
  );

  // Replace file path
  content = content.replace(
    /^(\/\/\s*file:\s+).*$/m,
    `$1${destBasename}`,
  );

  // Replace created date
  content = content.replace(
    /^(\/\/\s*created:\s+).*$/m,
    `$1${TODAY}`,
  );

  return content;
}

/**
 * Verify the scaffolded file using the engine's lint handler.
 */
async function verifyScaffolded(filePath: string): Promise<LintSummary | null> {
  const formatName = detectFormat(filePath);
  if (!formatName) return null;

  const handler = getFormat(formatName);
  if (!handler) return null;

  const results = await handler.lint(filePath);
  const health = handler.computeHealth
    ? await handler.computeHealth(filePath, results)
    : undefined;

  return summarize(relative(Deno.cwd(), filePath), results, health);
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// CLI
// ---------------------------------------------------------------------------

function showHelp(): void {
  const supported = Object.keys(TEMPLATES).join(", ");
  console.log(`
${COLORS.bold}${TOOL_NAME}${COLORS.reset} v${VERSION} — 4-Block Code File Scaffolder

${COLORS.dim}"For we are his workmanship, created unto good works."${COLORS.reset}
${COLORS.dim}— Ephesians 2:10${COLORS.reset}

Create new 4-block code files from seed templates.
Copies from the right template, substitutes metadata, verifies via engine.

${COLORS.bold}Usage:${COLORS.reset}
  ${TOOL_NAME} <language> <destination> [options]

${COLORS.bold}Languages:${COLORS.reset}  ${supported}

${COLORS.bold}Options:${COLORS.reset}
  --role <lib|exe|test>   Template role (default: auto-detect from filename)
  --key <key>             OmniCode key (default: derived from path)
  --title <title>         File title (default: derived from filename)
  --purpose <text>        Purpose description
  --version <ver>         Version (default: a-01.00)
  --dry-run               Preview without writing
  --no-verify             Skip post-scaffold lint verification
  --help, -h              Show this help

${COLORS.bold}Examples:${COLORS.reset}
  ${TOOL_NAME} go   src/server.go     --key B-server --title "HTTP Server"
  ${TOOL_NAME} rust src/lib.rs        --key B-core-lib --title "Core Library"
  ${TOOL_NAME} go   pkg/auth/auth.go  --purpose "Authentication module"
  ${TOOL_NAME} rust main.rs           --role exe --dry-run
`);
}

/**
 * Auto-detect role from filename patterns.
 */
function detectRole(filename: string): "library" | "executable" | "test" {
  const lower = filename.toLowerCase();
  if (lower.includes("test") || lower.includes("_test") || lower.includes("spec")) return "test";
  if (lower === "main.go" || lower === "main.rs" || lower.includes("cmd")) return "executable";
  return "library";
}

/**
 * Derive an OmniCode key from the destination path.
 */
function deriveKey(dest: string): string {
  const parts = dest
    .replace(/\.(go|rs|c|h)$/, "")
    .split("/")
    .filter((p) => p && p !== "." && p !== "..");
  return "B-" + parts.join("-");
}

/**
 * Derive a title from the filename.
 */
function deriveTitle(dest: string): string {
  const name = basename(dest).replace(/\.(go|rs|c|h)$/, "");
  return name
    .replace(/[-_]/g, " ")
    .replace(/\b\w/g, (c) => c.toUpperCase());
}

function parseArgs(args: string[]): ScaffoldOptions | null {
  if (args.length < 2 || args.includes("--help") || args.includes("-h")) {
    return null;
  }

  const language = args[0]!;
  const dest = args[1]!;
  const dryRun = args.includes("--dry-run");
  const noVerify = args.includes("--no-verify");

  // Extract flag values
  function flagValue(flag: string): string | undefined {
    const idx = args.indexOf(flag);
    return idx >= 0 && idx + 1 < args.length ? args[idx + 1] : undefined;
  }

  const roleStr = flagValue("--role");
  const role: "library" | "executable" | "test" =
    roleStr === "exe" || roleStr === "executable" ? "executable" :
    roleStr === "test" ? "test" :
    roleStr === "lib" || roleStr === "library" ? "library" :
    detectRole(basename(dest));

  return {
    language,
    dest: resolve(dest),
    role,
    key: flagValue("--key") ?? deriveKey(dest),
    title: flagValue("--title") ?? deriveTitle(dest),
    purpose: flagValue("--purpose") ?? `Implementation for ${deriveTitle(dest)}`,
    version: flagValue("--version") ?? "a-01.00",
    dryRun,
    verify: !noVerify,
  };
}

async function main(): Promise<void> {
  const opts = parseArgs(Deno.args);
  if (!opts) {
    showHelp();
    Deno.exit(0);
  }

  // Validate language
  if (!TEMPLATES[opts.language]) {
    console.error(`${COLORS.red}Unknown language: ${opts.language}${COLORS.reset}`);
    console.error(`Supported: ${Object.keys(TEMPLATES).join(", ")}`);
    Deno.exit(1);
  }

  // Check destination doesn't already exist
  try {
    await Deno.stat(opts.dest);
    console.error(`${COLORS.red}File already exists: ${opts.dest}${COLORS.reset}`);
    console.error("Remove it first or choose a different path.");
    Deno.exit(1);
  } catch {
    // Good — file doesn't exist
  }

  console.log(`${COLORS.bold}Scaffolding ${opts.language} ${opts.role}${COLORS.reset}`);
  console.log(`  ${COLORS.dim}dest:${COLORS.reset}    ${opts.dest}`);
  console.log(`  ${COLORS.dim}key:${COLORS.reset}     ${opts.key}`);
  console.log(`  ${COLORS.dim}title:${COLORS.reset}   ${opts.title}`);
  console.log(`  ${COLORS.dim}role:${COLORS.reset}    ${opts.role}`);
  console.log(`  ${COLORS.dim}version:${COLORS.reset} ${opts.version}`);

  // Scaffold
  const content = await scaffoldFromTemplate(opts);

  if (opts.dryRun) {
    console.log(`\n${COLORS.yellow}--- DRY RUN (would write) ---${COLORS.reset}\n`);
    // Show first 40 lines
    const lines = content.split("\n");
    const preview = lines.slice(0, 40).join("\n");
    console.log(preview);
    if (lines.length > 40) {
      console.log(`${COLORS.dim}... (${lines.length - 40} more lines)${COLORS.reset}`);
    }
    Deno.exit(0);
  }

  // Ensure parent directory exists
  const parent = dirname(opts.dest);
  try {
    await Deno.mkdir(parent, { recursive: true });
  } catch {
    // Already exists
  }

  // Write
  await Deno.writeTextFile(opts.dest, content);
  console.log(`\n${COLORS.green}Created: ${opts.dest}${COLORS.reset}`);

  // Verify via engine
  if (opts.verify) {
    console.log(`\n${COLORS.bold}Verifying...${COLORS.reset}\n`);
    const summary = await verifyScaffolded(opts.dest);
    if (summary) {
      printFileSummary(summary, false);
      if (summary.errors > 0) {
        console.log(`\n${COLORS.yellow}Scaffolded file has ${summary.errors} errors — review and fix.${COLORS.reset}`);
      }
    } else {
      console.log(`${COLORS.dim}No handler for verification.${COLORS.reset}`);
    }
  }
}

main().catch((err: unknown) => {
  console.error(
    `${COLORS.red}${TOOL_NAME}: ${err instanceof Error ? err.message : String(err)}${COLORS.reset}`,
  );
  Deno.exit(2);
});
