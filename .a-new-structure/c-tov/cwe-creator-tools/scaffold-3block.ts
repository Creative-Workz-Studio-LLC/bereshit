#!/usr/bin/env -S deno run --allow-read --allow-write
// ============================================================================
// METADATA
// ============================================================================
//
// file:    scaffold-3block.ts
// key:     B-tools-scaffold-3block
// title:   3-Block Data File Scaffolder — Thin Consumer of cws-struct Engine
// type:    Tool (Scaffolder)
// version: a-01.00
// created: 2026-02-18
// authors: Nova Dawn (CPI-SI)
// purpose: Create new 3-block data files (TOML, JSON, JSONC, dotfiles) from
//          seed templates. Thin consumer that: (1) copies from the right seed,
//          (2) substitutes metadata, (3) verifies via the engine.
//
//          Repurposed from transform-json.ts (generation 1 — migration wrapper).
//          transform-json.ts wrapped raw files in 3-block structure. That
//          migration is done. This tool creates NEW files — the next life.
//
// biblical_foundation: "To every thing there is a season, and a time to every
//   purpose under the heaven." — Ecclesiastes 3:1
//   A time to migrate, a time to create. The season has changed.
//
// usage:
//   deno run --allow-read --allow-write scaffold-3block.ts <format> <dest> [options]
//   scaffold-3block.ts toml  config/app.toml        --key B-app-config
//   scaffold-3block.ts jsonc config/format.jsonc     --title "Log Format"
//   scaffold-3block.ts json  data/events.json        --purpose "Event storage"
//   scaffold-3block.ts editorconfig .editorconfig    --purpose "Project root config"
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
import "../cws-struct/lib/handlers/toml.ts";
import "../cws-struct/lib/handlers/json.ts";
import "../cws-struct/lib/handlers/dotfiles.ts";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const VERSION = "a-01.00";
const TOOL_NAME = "scaffold-3block";
const TODAY = new Date().toISOString().split("T")[0]!;

/** Seed templates — resolved relative to this file. */
const SEED_ROOT = resolve(dirname(new URL(import.meta.url).pathname), "../../b-word/seed");

/** Supported formats and their seed template paths. */
const TEMPLATES: Record<string, { path: string; description: string }> = {
  toml: {
    path: resolve(SEED_ROOT, "code/L0/toml/config.toml"),
    description: "TOML configuration (3-block with OmniCode infrastructure)",
  },
  jsonc: {
    path: resolve(SEED_ROOT, "data/L0/json/config.jsonc"),
    description: "JSONC configuration (3-block with _ prefixed infrastructure)",
  },
  json: {
    path: resolve(SEED_ROOT, "data/L0/json/data.json"),
    description: "JSON data file (3-block, no comments)",
  },
  editorconfig: {
    path: resolve(SEED_ROOT, "data/L0/dotfiles/editorconfig.editorconfig"),
    description: "EditorConfig (comment-based 3-block)",
  },
  gitignore: {
    path: resolve(SEED_ROOT, "data/L0/dotfiles/gitignore.gitignore"),
    description: "Gitignore (comment-based 3-block)",
  },
  gitmessage: {
    path: resolve(SEED_ROOT, "data/L0/dotfiles/gitmessage.gitmessage"),
    description: "Git commit template (comment-based 3-block)",
  },
  env: {
    path: resolve(SEED_ROOT, "data/L0/dotfiles/env.env"),
    description: "Environment file (comment-based 3-block)",
  },
};

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

interface ScaffoldOptions {
  format: string;
  dest: string;
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
 * Different formats use different metadata styles:
 *   - TOML: [_pragma], [_metadata] tables with key = "value"
 *   - JSONC: "_P1_key", "_M1_identity" etc.
 *   - Dotfiles: # comment-based metadata
 *
 * We do simple text replacement on known patterns within each format.
 */
async function scaffoldFromTemplate(opts: ScaffoldOptions): Promise<string> {
  const template = TEMPLATES[opts.format];
  if (!template) {
    throw new Error(`Unsupported format: ${opts.format}. Supported: ${Object.keys(TEMPLATES).join(", ")}`);
  }

  let content: string;
  try {
    content = await Deno.readTextFile(template.path);
  } catch (e) {
    throw new Error(`Cannot read template ${template.path}: ${e}`);
  }

  const destBasename = basename(opts.dest);

  // --- Format-specific substitutions ---

  if (opts.format === "toml") {
    // TOML metadata lives in comment blocks and [_pragma]/[_metadata] tables
    content = substituteTomlMeta(content, opts, destBasename);
  } else if (opts.format === "jsonc" || opts.format === "json") {
    // JSON/JSONC metadata lives in _ prefixed keys
    content = substituteJsonMeta(content, opts, destBasename);
  } else {
    // Dotfiles — comment-based metadata
    content = substituteDotfileMeta(content, opts, destBasename);
  }

  return content;
}

/**
 * TOML metadata substitution.
 */
function substituteTomlMeta(content: string, opts: ScaffoldOptions, destFile: string): string {
  // Key in [_pragma.P1_core]
  content = content.replace(
    /(key\s*=\s*)"[^"]*"/,
    `$1"${opts.key}"`,
  );

  // Title in [_pragma.P5_summary]
  content = content.replace(
    /(title\s*=\s*)"[^"]*"(\s*#.*P5)/,
    `$1"${opts.title}"$2`,
  );

  // File in [_metadata.M1_identity]
  content = content.replace(
    /(file\s*=\s*)"[^"]*"(\s*#.*M1)/,
    `$1"${destFile}"$2`,
  );

  // Version
  content = content.replace(
    /(version\s*=\s*)"[^"]*"(\s*#.*M2)/,
    `$1"${opts.version}"$2`,
  );

  // Created date
  content = content.replace(
    /(created\s*=\s*)"[^"]*"(\s*#.*M2)/,
    `$1"${TODAY}"$2`,
  );

  return content;
}

/**
 * JSON/JSONC metadata substitution.
 */
function substituteJsonMeta(content: string, opts: ScaffoldOptions, destFile: string): string {
  // _P1_key
  content = content.replace(
    /("_P1_key"\s*:\s*)"[^"]*"/,
    `$1"${opts.key}"`,
  );

  // _P5_title
  content = content.replace(
    /("_P5_title"\s*:\s*)"[^"]*"/,
    `$1"${opts.title}"`,
  );

  // M1 file
  content = content.replace(
    /("file"\s*:\s*)"[^"]*"([\s\S]*?_M1)/,
    `$1"${destFile}"$2`,
  );

  // M1 key (inside _M1_identity object)
  content = content.replace(
    /("key"\s*:\s*)"[^"]*"([\s\S]*?_M1)/,
    `$1"${opts.key}"$2`,
  );

  // M1 title (inside _M1_identity object)
  content = content.replace(
    /("title"\s*:\s*)"[^"]*"([\s\S]*?_M1)/,
    `$1"${opts.title}"$2`,
  );

  return content;
}

/**
 * Dotfile metadata substitution (comment-based).
 */
function substituteDotfileMeta(content: string, opts: ScaffoldOptions, destFile: string): string {
  // Comment-based key
  content = content.replace(
    /^(#\s*key:\s+).*$/m,
    `$1${opts.key}`,
  );

  // Comment-based title
  content = content.replace(
    /^(#\s*title:\s+).*$/m,
    `$1${opts.title}`,
  );

  // Comment-based file
  content = content.replace(
    /^(#\s*file:\s+).*$/m,
    `$1${destFile}`,
  );

  // Comment-based version
  content = content.replace(
    /^(#\s*version:\s+).*$/m,
    `$1${opts.version}`,
  );

  // Comment-based created
  content = content.replace(
    /^(#\s*created:\s+).*$/m,
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
  console.log(`
${COLORS.bold}${TOOL_NAME}${COLORS.reset} v${VERSION} — 3-Block Data File Scaffolder

${COLORS.dim}"To every thing there is a season, and a time to every purpose."${COLORS.reset}
${COLORS.dim}— Ecclesiastes 3:1${COLORS.reset}

Create new 3-block data files from seed templates.
Copies from the right template, substitutes metadata, verifies via engine.

${COLORS.bold}Usage:${COLORS.reset}
  ${TOOL_NAME} <format> <destination> [options]

${COLORS.bold}Formats:${COLORS.reset}`);

  for (const [name, tmpl] of Object.entries(TEMPLATES)) {
    console.log(`  ${COLORS.bold}${name}${COLORS.reset}  — ${tmpl.description}`);
  }

  console.log(`
${COLORS.bold}Options:${COLORS.reset}
  --key <key>             OmniCode key (default: derived from path)
  --title <title>         File title (default: derived from filename)
  --purpose <text>        Purpose description
  --version <ver>         Version (default: a-01.00)
  --dry-run               Preview without writing
  --no-verify             Skip post-scaffold lint verification
  --help, -h              Show this help

${COLORS.bold}Examples:${COLORS.reset}
  ${TOOL_NAME} toml  config/app.toml        --key B-app-config --title "App Config"
  ${TOOL_NAME} jsonc config/format.jsonc     --title "Log Format Definition"
  ${TOOL_NAME} json  data/events.json        --purpose "Event stream storage"
  ${TOOL_NAME} editorconfig .editorconfig    --dry-run
`);
}

/**
 * Derive an OmniCode key from the destination path.
 */
function deriveKey(dest: string): string {
  const parts = dest
    .replace(/\.(toml|json|jsonc|editorconfig|gitignore|gitmessage|env)$/, "")
    .split("/")
    .filter((p) => p && p !== "." && p !== "..");
  return "B-" + parts.join("-");
}

/**
 * Derive a title from the filename.
 */
function deriveTitle(dest: string): string {
  const name = basename(dest).replace(/\.(toml|json|jsonc|editorconfig|gitignore|gitmessage|env)$/, "");
  return name
    .replace(/[-_]/g, " ")
    .replace(/\b\w/g, (c) => c.toUpperCase());
}

function parseArgs(args: string[]): ScaffoldOptions | null {
  if (args.length < 2 || args.includes("--help") || args.includes("-h")) {
    return null;
  }

  const format = args[0]!;
  const dest = args[1]!;
  const dryRun = args.includes("--dry-run");
  const noVerify = args.includes("--no-verify");

  function flagValue(flag: string): string | undefined {
    const idx = args.indexOf(flag);
    return idx >= 0 && idx + 1 < args.length ? args[idx + 1] : undefined;
  }

  return {
    format,
    dest: resolve(dest),
    key: flagValue("--key") ?? deriveKey(dest),
    title: flagValue("--title") ?? deriveTitle(dest),
    purpose: flagValue("--purpose") ?? `Data configuration for ${deriveTitle(dest)}`,
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

  // Validate format
  if (!TEMPLATES[opts.format]) {
    console.error(`${COLORS.red}Unknown format: ${opts.format}${COLORS.reset}`);
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

  console.log(`${COLORS.bold}Scaffolding ${opts.format} data file${COLORS.reset}`);
  console.log(`  ${COLORS.dim}dest:${COLORS.reset}    ${opts.dest}`);
  console.log(`  ${COLORS.dim}key:${COLORS.reset}     ${opts.key}`);
  console.log(`  ${COLORS.dim}title:${COLORS.reset}   ${opts.title}`);
  console.log(`  ${COLORS.dim}version:${COLORS.reset} ${opts.version}`);

  // Scaffold
  const content = await scaffoldFromTemplate(opts);

  if (opts.dryRun) {
    console.log(`\n${COLORS.yellow}--- DRY RUN (would write) ---${COLORS.reset}\n`);
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
