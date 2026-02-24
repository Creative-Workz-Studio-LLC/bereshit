#!/usr/bin/env -S deno run --allow-read
// ============================================================================
// METADATA
// ============================================================================
//
// file:    mod.ts
// key:     B-tov-cws-struct-mod
// title:   CWS Struct — CLI Entrypoint
// type:    Code (Executable)
// version: a-01.00
// created: 2026-02-11
// authors: Nova Dawn (CPI-SI)
// purpose: Unified CLI for structural alignment — lint and transform files
//          across all OmniCode formats. Dispatches to registered format
//          handlers. Replaces 8 standalone Deno scripts with one expandable tool.
//
// biblical_foundation: "Except the LORD build the house, they labour in
//   vain that build it." — Psalm 127:1
//
// usage:
//   cws-struct <operation> [format] <targets...> [options]
//   cws-struct lint [format] <targets...>       # validate (format auto-detected)
//   cws-struct transform [format] <targets...>  # fix (format auto-detected)
//   cws-struct verify env [--verbose]           # check dev environment
//   cws-struct formats                          # list registered formats
//   cws-struct help
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { CliOptions } from "./lib/foundation/mod.ts";
import { setGlobalPolicy, setLintFocus, setImpactWeights, loadConfig } from "./lib/foundation/mod.ts";
import { COLORS, setDiscoveryLimits } from "./lib/engine/mod.ts";
import {
  initDebug,
  debugConfig,
  debugPolicy,
} from "./lib/engine/debug.ts";
import { verifyEnvironment } from "./lib/verify/env.ts";
import { startStudio } from "./lib/studio/serve.ts";

// Centralized tool config — one source of truth for all commands
import {
  TOOL_NAME, VERSION,
  setToolConfig,
  EXIT_OK, EXIT_LINT_ERRORS, EXIT_TOOL_ERROR,
  openDatabase,
} from "./lib/commands/config.ts";

// Register all format handlers (side-effect imports)
import "./lib/handlers/toml.ts";
import "./lib/handlers/omni.ts";
import "./lib/handlers/ofd.ts";
import "./lib/handlers/json.ts";
import "./lib/handlers/go.ts";
import "./lib/handlers/makefile.ts";
import "./lib/handlers/dotfiles.ts";
import "./lib/handlers/rust.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Commands
// ---------------------------------------------------------------------------

// Commands — extracted from this file during God Object dismantling
import { showHelp, showFormats } from "./lib/commands/help.ts";
import { runQuery } from "./lib/commands/query.ts";
import { parseArgs, validateArgs, parseLintFocus } from "./lib/commands/parse.ts";
import { runCreate } from "./lib/commands/create.ts";
import { runTranspile } from "./lib/commands/transpile.ts";
import { runTransform } from "./lib/commands/transform.ts";
import { runLint } from "./lib/commands/lint.ts";
import { runWatch } from "./lib/commands/watch.ts";


// (lintWithHandler, runLint extracted to lib/commands/lint.ts)
// (emitHealthLog extracted to lib/engine/logging.ts)
// (emitJson extracted to lib/engine/output.ts)
// (runInspect extracted to lib/engine/inspect.ts)
// (getChangedFiles extracted to lib/engine/git.ts)
// (transformWithHandler, runTransform extracted to lib/commands/transform.ts)
// (runCreate extracted to lib/commands/create.ts)
// (runTranspile extracted to lib/commands/transpile.ts)
// (runWatch extracted to lib/commands/watch.ts)
// (parseLintFocus extracted to lib/commands/parse.ts)
// (Query, database, and helper functions extracted to lib/commands/query.ts)

// ============================================================================
// CLOSING
// ============================================================================

// (Exit codes, KNOWN_FLAGS, validateArgs extracted to lib/commands/parse.ts)

// ---------------------------------------------------------------------------
// Main — dispatch with error boundary
// ---------------------------------------------------------------------------

async function main(): Promise<void> {
  // Load tool configuration from deno.jsonc. Tripwire: missing → defaults.
  const config = await loadConfig();
  setToolConfig(TOOL_NAME, config.version, config.concurrency);
  setImpactWeights(config.impactWeights);
  setDiscoveryLimits({
    maxFiles: config.maxFiles,
    maxFileSize: config.maxFileSize,
    excludedDirs: config.excludedDirs,
  });

  // Input validation
  const validationError = validateArgs(Deno.args);
  if (validationError) {
    console.error(`${COLORS.red}${TOOL_NAME}: ${validationError}${COLORS.reset}`);
    Deno.exit(EXIT_TOOL_ERROR);
  }

  const opts = parseArgs(Deno.args);

  // Initialize debug/trace output (must be before any debug calls).
  initDebug(opts);
  debugConfig(config);
  debugPolicy(opts.policy);

  // Set session-wide ternary policy before dispatching to handlers.
  setGlobalPolicy(opts.policy);

  // Set session-wide lint grid focus before dispatching to handlers.
  // Handlers call getLintFocus() inside runLintGrid() to read the focus.
  setLintFocus(parseLintFocus(opts.check));

  switch (opts.command) {
    case "help":
      showHelp(opts.helpTopic);
      Deno.exit(EXIT_OK);
      break;

    case "version":
      console.log(`${TOOL_NAME} v${VERSION}`);
      Deno.exit(EXIT_OK);
      break;

    case "lint": {
      const db = await openDatabase(opts);
      const ok = await runLint(opts, db);
      if (db) {
        // Update pattern trends after the full lint run
        try {
          db.recalculateAffectedFiles();
          db.updateTrends();
        } catch { /* best-effort pattern analysis */ }
        console.log(`${COLORS.dim}Database: ${opts.dbPath} (${db.getStats().totalRuns} runs recorded)${COLORS.reset}`);
        db.close();
      }
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "transform": {
      const ok = await runTransform(opts);
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "query": {
      const ok = await runQuery(opts);
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "create": {
      const ok = await runCreate(opts);
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "transpile": {
      const ok = await runTranspile(opts);
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "watch": {
      await runWatch(opts);
      break;
    }

    case "verify": {
      const ok = await verifyEnvironment(opts.verbose);
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "formats":
      showFormats();
      Deno.exit(EXIT_OK);
      break;

    case "studio": {
      const portIdx = Deno.args.indexOf("--port");
      const port = portIdx >= 0 && Deno.args[portIdx + 1]
        ? parseInt(Deno.args[portIdx + 1]!, 10)
        : 4200;
      startStudio(port);
      // Studio runs indefinitely — Deno.serve keeps the process alive
      break;
    }
  }
}

// Top-level error boundary — no unhandled exceptions crash the CLI
main().catch((err: unknown) => {
  console.error(
    `${COLORS.red}${TOOL_NAME}: Unexpected error: ${err instanceof Error ? err.message : String(err)}${COLORS.reset}`,
  );
  if (err instanceof Error && err.stack) {
    console.error(`${COLORS.dim}${err.stack}${COLORS.reset}`);
  }
  Deno.exit(EXIT_TOOL_ERROR);
});
