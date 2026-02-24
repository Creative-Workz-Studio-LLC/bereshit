// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/commands/config.ts
// key:     B-tov-cws-struct-lib-commands-config
// title:   CWS Struct — Shared Tool Configuration
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Centralized tool state that ALL commands share. One source of truth
//          for TOOL_NAME, VERSION, CONCURRENCY, exit codes, and database access.
//          Eliminates per-command setter duplication — the linter's infrastructure
//          serves the whole tool, not just lint.
//
// biblical_foundation: "There is one body, and one Spirit... one Lord, one
//   faith, one baptism." — Ephesians 4:4-5
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { CliOptions, LintSummary } from "../foundation/mod.ts";
import { COLORS } from "../engine/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Tool identity — set once by main(), read by all commands
// ---------------------------------------------------------------------------

export let TOOL_NAME = "cws-struct";
export let VERSION = "0.1.0";
export let CONCURRENCY = 8;

/** Set tool identity. Called once during main() initialization. */
export function setToolConfig(
  name: string,
  version: string,
  concurrency: number,
): void {
  TOOL_NAME = name;
  VERSION = version;
  CONCURRENCY = concurrency;
}

// ---------------------------------------------------------------------------
// Exit codes — consistent across all commands
// ---------------------------------------------------------------------------

/** Exit code 0: Clean run, no structural errors found. */
export const EXIT_OK = 0;

/** Exit code 1: Structural errors found in target files. */
export const EXIT_LINT_ERRORS = 1;

/** Exit code 2: Tool error — CLI misuse, I/O failure, or unhandled exception. */
export const EXIT_TOOL_ERROR = 2;

// ---------------------------------------------------------------------------
// Lazy database loading — shared across lint, query, watch
// ---------------------------------------------------------------------------
//
// The import() is called only when --db flag or query command is used.
// This prevents `cws-struct help` from requiring --allow-ffi --allow-env.

let _dbModule: typeof import("../data/database.ts") | null = null;

export async function getDbModule() {
  if (!_dbModule) {
    _dbModule = await import("../data/database.ts");
  }
  return _dbModule;
}

type DbModule = Awaited<ReturnType<typeof getDbModule>>;
export type DbHandle = InstanceType<DbModule["CwsDatabase"]>;

// ---------------------------------------------------------------------------
// Database lifecycle — shared across lint, query, watch
// ---------------------------------------------------------------------------

/**
 * Open a database connection and return it. Returns null if --db not specified.
 * "The books were opened." — Revelation 20:12
 */
export async function openDatabase(opts: CliOptions): Promise<DbHandle | null> {
  if (!opts.dbPath) return null;

  try {
    const mod = await getDbModule();
    return new mod.CwsDatabase(undefined, opts.dbPath);
  } catch (e) {
    console.error(
      `${COLORS.yellow}Warning: Could not open database ${opts.dbPath}: ${e instanceof Error ? e.message : String(e)}${COLORS.reset}`,
    );
    return null;
  }
}

/**
 * Record lint summaries to the database.
 * Handles the full lifecycle: update file record, record lint run, record actions,
 * record health history. Called after each lint batch.
 */
export function recordToDatabase(
  db: DbHandle,
  summaries: LintSummary[],
  formatName: string,
  instanceName: string,
  sessionId?: string,
): void {
  try {
    db.recordBatch(
      summaries.map((s) => ({
        filePath: s.file,
        format: formatName,
        summary: s,
        handler: formatName,
        instanceName,
        instanceType: "cpisi",
        sessionId,
      })),
    );
  } catch (e) {
    console.error(
      `${COLORS.yellow}Warning: Database recording failed: ${e instanceof Error ? e.message : String(e)}${COLORS.reset}`,
    );
  }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// One body, one Spirit. One tool config, shared by all commands.
// The linter's infrastructure serves the whole tool.
//
// "There is one body, and one Spirit." — Ephesians 4:4
// ============================================================================
