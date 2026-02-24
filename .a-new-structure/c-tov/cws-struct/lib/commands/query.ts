// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/commands/query.ts
// key:     B-tov-cws-struct-lib-commands-query
// title:   CWS Struct — Query Command
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Health database query commands extracted from mod.ts.
//          All query subcommands: health, worst, best, patterns, trend,
//          instance, session, stats. Plus helpers: parseLimit, qColor,
//          formatHealthLine, openDatabase, recordToDatabase.
//
// biblical_foundation: "The books were opened." — Revelation 20:12
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { CliOptions } from "../foundation/mod.ts";
import { COLORS } from "../engine/mod.ts";
import { trueToLevel, levelToEmoji } from "../foundation/mod.ts";
import { TOOL_NAME, getDbModule } from "./config.ts";
import type { DbHandle } from "./config.ts";

// Re-export shared types/functions so existing imports from query.ts still work
export type { DbHandle } from "./config.ts";
export { openDatabase, recordToDatabase } from "./config.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Shared helpers
// ---------------------------------------------------------------------------

/**
 * Parse --limit value from CLI args. Returns default if not present or invalid.
 */
function parseLimit(args: string[], defaultLimit = 10): number {
  const idx = args.indexOf("--limit");
  if (idx < 0 || idx + 1 >= args.length) return defaultLimit;
  const n = parseInt(args[idx + 1]!, 10);
  return Number.isFinite(n) && n > 0 ? n : defaultLimit;
}

/**
 * Health color for terminal display — matches output.ts Hebrew state thresholds.
 * perfect/whole (34+) = green, sound/even (0-33) = yellow, lacking/wanting/broken (<0) = red.
 */
function qColor(score: number | null): string {
  if (score === null) return COLORS.dim;
  if (score >= 34) return COLORS.green;
  if (score >= 0) return COLORS.yellow;
  return COLORS.red;
}

/** Format a file health line: score + state + emoji. */
function formatHealthLine(score: number | null, _state: string | null): string {
  if (score === null) return `${COLORS.dim}(no score)${COLORS.reset}`;
  const level = trueToLevel(score);
  const emoji = levelToEmoji(level);
  return `${qColor(score)}${score}/\u00b1100 ${level} ${emoji}${COLORS.reset}`;
}

// (openDatabase, recordToDatabase lifted to config.ts — shared tool infrastructure)

// ---------------------------------------------------------------------------
// runQuery — main entry point
// ---------------------------------------------------------------------------

/**
 * Query health database.
 *
 * Usage: cws-struct query <subcommand> [args] --db <path>
 *
 * "The books were opened." — Revelation 20:12
 */
export async function runQuery(opts: CliOptions): Promise<boolean> {
  if (!opts.dbPath) {
    console.error(`${COLORS.red}--db <path> required for query command.${COLORS.reset}`);
    console.error(`Example: ${TOOL_NAME} query stats --db .cws-struct.db`);
    return false;
  }

  if (!opts.queryCommand) {
    console.error(`${COLORS.red}Query subcommand required.${COLORS.reset}`);
    console.error(`Available: health, worst, best, patterns, trend, instance, session, stats`);
    console.error(`Example: ${TOOL_NAME} query stats --db ${opts.dbPath}`);
    return false;
  }

  // Verify database file exists
  try {
    await Deno.stat(opts.dbPath);
  } catch {
    console.error(`${COLORS.red}Database not found: ${opts.dbPath}${COLORS.reset}`);
    console.error(`Run a lint with --db to create it: ${TOOL_NAME} lint . --db ${opts.dbPath}`);
    return false;
  }

  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, opts.dbPath);
  const limit = parseLimit(Deno.args);

  try {
    switch (opts.queryCommand) {
      case "health":
        return queryHealth(db, opts);

      case "worst":
        return queryWorst(db, limit);

      case "best":
        return queryBest(db, limit);

      case "patterns":
        return queryPatterns(db, limit);

      case "trend":
        return queryTrend(db, opts);

      case "instance":
        return queryInstance(db, opts);

      case "session":
        return querySession(db, opts);

      case "stats":
        return queryStats(db);

      default:
        console.error(`${COLORS.red}Unknown query subcommand: ${opts.queryCommand}${COLORS.reset}`);
        console.error(`Available: health, worst, best, patterns, trend, instance, session, stats`);
        return false;
    }
  } finally {
    db.close();
  }
}

// ---------------------------------------------------------------------------
// Query subcommand implementations
// ---------------------------------------------------------------------------

function queryHealth(db: DbHandle, opts: CliOptions): boolean {
  const filePath = opts.targets[0];
  if (!filePath) {
    console.error(`${COLORS.red}File path required. Usage: ${TOOL_NAME} query health <path> --db <db>${COLORS.reset}`);
    return false;
  }

  const report = db.getFileHealth(filePath);
  if (!report) {
    console.error(`${COLORS.yellow}No data for: ${filePath}${COLORS.reset}`);
    console.error(`${COLORS.dim}Run a lint with --db first to track this file.${COLORS.reset}`);
    return false;
  }

  console.log(`${COLORS.bold}File health:${COLORS.reset} ${report.path}`);
  console.log(`  Score: ${formatHealthLine(report.score, report.state)}`);
  if (report.trite) {
    console.log(`  ${COLORS.dim}Trite: ${report.trite}${COLORS.reset}`);
  }
  console.log(`  Format: ${report.format}${report.subtype ? ` (${report.subtype})` : ""}`);

  if (report.lastRun) {
    console.log(`  Last run: ${COLORS.dim}${report.lastRun.timestamp} by ${report.lastRun.instanceName ?? "unknown"}${COLORS.reset}`);
    console.log(`    ${report.lastRun.errorCount}E ${report.lastRun.warnCount}W ${report.lastRun.infoCount}I`);
  }

  if (report.history.length > 1) {
    console.log(`\n${COLORS.bold}History:${COLORS.reset} (${report.history.length} entries)`);
    for (const h of report.history.slice(0, 10)) {
      const delta = h.delta !== null && h.delta !== 0
        ? ` ${h.delta > 0 ? COLORS.green + "+" : COLORS.red}${h.delta}${COLORS.reset}`
        : "";
      console.log(
        `  ${COLORS.dim}${h.timestamp}${COLORS.reset}  ${formatHealthLine(h.score, h.hebrewState)}${delta}`,
      );
    }
  }

  return true;
}

function queryWorst(db: DbHandle, limit: number): boolean {
  const files = db.getUnhealthyFiles(100); // everything below +100 (i.e., everything)
  if (files.length === 0) {
    console.log(`${COLORS.green}No unhealthy files tracked.${COLORS.reset}`);
    return true;
  }

  // Sort by score ascending (worst first) — getUnhealthyFiles already does this
  const show = files.slice(0, limit);
  console.log(`${COLORS.bold}Worst health files:${COLORS.reset} (${show.length}/${files.length})\n`);

  for (let i = 0; i < show.length; i++) {
    const f = show[i]!;
    console.log(
      `  ${COLORS.dim}${String(i + 1).padStart(2)}.${COLORS.reset} ${formatHealthLine(f.currentScore, f.currentState)}  ${f.path}`,
    );
  }

  return true;
}

function queryBest(db: DbHandle, limit: number): boolean {
  const files = db.getBestFiles(limit);
  if (files.length === 0) {
    console.log(`${COLORS.dim}No scored files tracked.${COLORS.reset}`);
    return true;
  }

  console.log(`${COLORS.bold}Best health files:${COLORS.reset} (${files.length})\n`);

  for (let i = 0; i < files.length; i++) {
    const f = files[i]!;
    console.log(
      `  ${COLORS.dim}${String(i + 1).padStart(2)}.${COLORS.reset} ${formatHealthLine(f.currentScore, f.currentState)}  ${f.path}`,
    );
  }

  return true;
}

function queryPatterns(db: DbHandle, limit: number): boolean {
  const patterns = db.getTopPatterns(limit);
  if (patterns.length === 0) {
    console.log(`${COLORS.dim}No patterns tracked yet.${COLORS.reset}`);
    return true;
  }

  // Also get trending patterns
  const trending = db.getTrendingPatterns(limit);
  const trendingCodes = new Set(trending.map((p) => `${p.errorCode}:${p.rule}`));

  console.log(`${COLORS.bold}Top recurring issues:${COLORS.reset} (${patterns.length})\n`);

  for (let i = 0; i < patterns.length; i++) {
    const p = patterns[i]!;
    const trendIcon = trendingCodes.has(`${p.errorCode}:${p.rule}`)
      ? `${COLORS.red}\u2191${COLORS.reset}` // ↑
      : p.trending === "decreasing"
        ? `${COLORS.green}\u2193${COLORS.reset}` // ↓
        : `${COLORS.dim}\u2192${COLORS.reset}`; // →

    console.log(
      `  ${COLORS.dim}${String(i + 1).padStart(2)}.${COLORS.reset} ` +
      `${COLORS.bold}${p.errorCode}${COLORS.reset} (${p.rule}) ` +
      `\u2014 ${p.affectedFiles} file${p.affectedFiles !== 1 ? "s" : ""}, ` +
      `${p.frequency}x ${trendIcon}`,
    );
  }

  return true;
}

function queryTrend(db: DbHandle, opts: CliOptions): boolean {
  const filePath = opts.targets[0];
  if (!filePath) {
    console.error(`${COLORS.red}File path required. Usage: ${TOOL_NAME} query trend <path> --db <db>${COLORS.reset}`);
    return false;
  }

  const history = db.getHealthHistory(filePath, 20);
  if (history.length === 0) {
    console.error(`${COLORS.yellow}No health history for: ${filePath}${COLORS.reset}`);
    return false;
  }

  console.log(`${COLORS.bold}Health trend:${COLORS.reset} ${filePath}\n`);

  // Show newest first (reverse chronological is more scannable)
  const reversed = [...history].reverse();
  for (const h of reversed) {
    const delta = h.delta !== null && h.delta !== 0
      ? ` (${h.delta > 0 ? "+" : ""}${h.delta})`
      : "";
    const dateStr = h.timestamp.split("T")[0] ?? h.timestamp;
    console.log(
      `  ${COLORS.dim}${dateStr}${COLORS.reset}  ${formatHealthLine(h.score, h.hebrewState)}${delta}`,
    );
  }

  // Overall trend summary
  if (history.length >= 2) {
    const first = history[history.length - 1]!; // oldest
    const last = history[0]!; // newest
    const totalDelta = last.score - first.score;
    const deltaColor = totalDelta > 0 ? COLORS.green : totalDelta < 0 ? COLORS.red : COLORS.dim;
    console.log(
      `\n  ${COLORS.bold}Net:${COLORS.reset} ${deltaColor}${totalDelta > 0 ? "+" : ""}${totalDelta}${COLORS.reset} over ${history.length} entries`,
    );
  }

  return true;
}

function queryInstance(db: DbHandle, opts: CliOptions): boolean {
  const name = opts.targets[0];
  if (!name) {
    console.error(`${COLORS.red}Instance name required. Usage: ${TOOL_NAME} query instance <name> --db <db>${COLORS.reset}`);
    return false;
  }

  const activity = db.getInstanceActivity(name);
  if (!activity) {
    console.error(`${COLORS.yellow}No data for instance: ${name}${COLORS.reset}`);
    return false;
  }

  const inst = activity.instance;
  console.log(`${COLORS.bold}Instance:${COLORS.reset} ${inst.name} (${inst.type})`);
  console.log(`  First seen: ${COLORS.dim}${inst.firstSeen}${COLORS.reset}`);
  console.log(`  Runs: ${inst.runsCount}  Sessions: ${activity.sessionsCount}`);
  if (inst.lastActive) {
    console.log(`  Last active: ${COLORS.dim}${inst.lastActive}${COLORS.reset}`);
  }

  if (activity.recentRuns.length > 0) {
    console.log(`\n${COLORS.bold}Recent runs:${COLORS.reset}`);
    for (const run of activity.recentRuns.slice(0, 5)) {
      const scoreStr = run.totalScore !== null
        ? formatHealthLine(run.totalScore, run.hebrewState)
        : `${COLORS.dim}(no score)${COLORS.reset}`;
      const file = db.getFileById(run.fileId);
      const filePath = file?.path ?? `file#${run.fileId}`;
      console.log(
        `  ${COLORS.dim}${run.timestamp}${COLORS.reset}  ${run.errorCount}E ${run.warnCount}W  ${scoreStr}  ${COLORS.dim}${filePath}${COLORS.reset}`,
      );
    }
  }

  return true;
}

function querySession(db: DbHandle, opts: CliOptions): boolean {
  const sessionId = opts.targets[0];
  if (!sessionId) {
    console.error(`${COLORS.red}Session ID required. Usage: ${TOOL_NAME} query session <id> --db <db>${COLORS.reset}`);
    return false;
  }

  const summary = db.getSessionSummary(sessionId);
  if (!summary) {
    console.error(`${COLORS.yellow}No data for session: ${sessionId}${COLORS.reset}`);
    return false;
  }

  const s = summary.session;
  console.log(`${COLORS.bold}Session:${COLORS.reset} ${s.id}`);
  if (s.instanceName) {
    console.log(`  Instance: ${s.instanceName}`);
  }
  console.log(`  Started: ${COLORS.dim}${s.startTime}${COLORS.reset}`);
  if (s.endTime) {
    console.log(`  Ended: ${COLORS.dim}${s.endTime}${COLORS.reset}`);
  }
  console.log(`  Files linted: ${s.filesLinted}  Health delta: ${s.healthDelta > 0 ? "+" : ""}${s.healthDelta}`);

  if (summary.runs.length > 0) {
    console.log(`\n${COLORS.bold}Runs:${COLORS.reset} ${summary.runs.length}`);
    for (const run of summary.runs.slice(0, 10)) {
      const file = db.getFileById(run.fileId);
      const filePath = file?.path ?? `file#${run.fileId}`;
      console.log(
        `  ${COLORS.dim}${run.timestamp}${COLORS.reset}  ${run.errorCount}E ${run.warnCount}W  ${COLORS.dim}${filePath}${COLORS.reset}`,
      );
    }
  }

  if (summary.patternsFound.length > 0) {
    console.log(`\n${COLORS.bold}Patterns found:${COLORS.reset} ${summary.patternsFound.join(", ")}`);
  }

  return true;
}

function queryStats(db: DbHandle): boolean {
  const stats = db.getStats();

  console.log(`${COLORS.bold}Database statistics:${COLORS.reset}\n`);
  console.log(`  Files tracked: ${stats.totalFiles}`);
  console.log(`  Total runs: ${stats.totalRuns}`);

  if (stats.avgScore !== null) {
    console.log(`  Avg health: ${formatHealthLine(stats.avgScore, null)}`);
    console.log(`  Healthy (>=34): ${COLORS.green}${stats.healthyFiles}${COLORS.reset}`);
    console.log(`  Unhealthy (<0): ${COLORS.red}${stats.unhealthyFiles}${COLORS.reset}`);
  } else {
    console.log(`  ${COLORS.dim}No health scores recorded yet.${COLORS.reset}`);
  }

  return true;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Query command — database health queries.
// All query subcommands + database lifecycle helpers.
//
// "The books were opened." — Revelation 20:12
// ============================================================================
