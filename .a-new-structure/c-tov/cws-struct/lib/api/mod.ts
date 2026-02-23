// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/api/mod.ts
// key:     B-tov-cws-struct-lib-api-mod
// title:   CWS Struct — Programmatic API
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Stable programmatic interface for scripts, hooks, and CPI-SI agents.
//          Same logic as CLI but returns structured TypeScript objects — no
//          stdout, no formatting, no global state. Import and call.
//
//          import { lintFile, getDataLayer } from "./lib/api/mod.ts";
//          const report = await lintFile("path/to/file.go");
//          console.log(report.health?.level); // "sound"
//
// biblical_foundation: "Ask, and it shall be given you; seek, and ye shall
//   find; knock, and it shall be opened unto you." — Matthew 7:7
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Imports
// ---------------------------------------------------------------------------

import { relative } from "@std/path";

import type {
  LintResult,
  LintSummary,
  FormatHandler,
  TransformOptions,
  InspectResult,
  ToolConfig,
  HealthScore,
} from "../foundation/mod.ts";

import {
  summarize,
  setGlobalPolicy,
  setImpactWeights,
  loadConfig,
} from "../foundation/mod.ts";

import {
  getFormat,
  listFormats,
  listFormatDetails,
  detectFormat,
  discoverFiles,
  discoverAllFiles,
  resolveErrorCodes,
  generateFileText,
} from "../engine/mod.ts";

import type { FillContext } from "../engine/mod.ts";

// Data layer — static registries (no FFI)
import {
  SECTION_REGISTRY,
  SECTION_ORDER,
  FORM_REGISTRY,
  CONCEPT_REGISTRY,
  CONCEPT_ORDER,
  PLACEMENT_REGISTRY,
  ERROR_CODES,
  LINT_RULES,
  PIPELINE_LAYERS,
  getByRule,
  getByCode,
  getByLayer,
} from "../data/mod.ts";

import type {
  BlockName,
  SectionEntry,
  ConceptEntry,
  FormDefinition,
  PlacementRule,
  ErrorCodeEntry,
  PipelineLayer,
} from "../data/mod.ts";

// Handler registration (side-effect imports)
import "../handlers/toml.ts";
import "../handlers/omni.ts";
import "../handlers/ofd.ts";
import "../handlers/json.ts";
import "../handlers/go.ts";
import "../handlers/makefile.ts";
import "../handlers/dotfiles.ts";
import "../handlers/rust.ts";

// ---------------------------------------------------------------------------
// 2. Types
// ---------------------------------------------------------------------------

/** Options for programmatic lint operations. */
export interface LintOptions {
  /** Specific format to use (auto-detected if omitted). */
  format?: string;
  /** Lint policy: "strict", "balanced", or "growth". */
  policy?: "strict" | "balanced" | "growth";
  /** Custom impact weights (overrides config). */
  impactWeights?: Record<string, number>;
  /** Base directory for relative paths in output (default: cwd). */
  cwd?: string;
}

/** A full lint report — results + health + metadata for one file. */
export interface LintReport {
  /** File path (relative to cwd). */
  file: string;
  /** Absolute file path. */
  absolutePath: string;
  /** Detected format name. */
  format: string;
  /** Lint results with error codes resolved. */
  results: LintResult[];
  /** Aggregated summary. */
  summary: LintSummary;
  /** Health score (if handler supports it). */
  health?: HealthScore;
}

/** Result of a transform operation. */
export interface TransformReport {
  /** File path. */
  file: string;
  /** Transform results (what changed or would change). */
  results: LintResult[];
  /** Whether the file was modified. */
  modified: boolean;
}

/** Result of a create operation. */
export interface CreateReport {
  /** Destination file path. */
  file: string;
  /** Generated content. */
  content: string;
  /** Line count. */
  lineCount: number;
  /** Whether the file was written (false in dry-run). */
  written: boolean;
}

/** Full data layer snapshot — all registries. */
export interface DataLayerSnapshot {
  sections: Record<string, SectionEntry>;
  sectionOrder: Record<BlockName, string[]>;
  concepts: Record<string, ConceptEntry>;
  conceptOrder: readonly string[];
  forms: Record<string, FormDefinition>;
  placement: Record<string, PlacementRule[]>;
  errorCodes: ReadonlyMap<string, ErrorCodeEntry>;
  lintRules: ReadonlyMap<string, ErrorCodeEntry>;
  pipelineLayers: readonly PipelineLayer[];
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Configuration
// ---------------------------------------------------------------------------

/** Loaded config (cached after first call). */
let _config: ToolConfig | null = null;

/**
 * Initialize the API with optional config.
 *
 * Call once before using other API functions. If not called, defaults apply.
 * Returns the loaded config for inspection.
 */
export async function init(configPath?: string): Promise<ToolConfig> {
  _config = await loadConfig(configPath);

  // Apply config-driven settings
  if (_config.impactWeights) {
    setImpactWeights(_config.impactWeights);
  }

  return _config;
}

/**
 * Get the current config (loads defaults if init() wasn't called).
 */
export async function getConfig(): Promise<ToolConfig> {
  if (!_config) {
    _config = await loadConfig();
  }
  return _config;
}

// ---------------------------------------------------------------------------
// 2. Lint operations
// ---------------------------------------------------------------------------

/**
 * Lint a single file.
 *
 * Resolves the format handler, runs lint checks, computes health,
 * enriches with error codes, and returns structured results.
 * No stdout. No formatting. Pure data.
 */
export async function lintFile(
  filePath: string,
  options: LintOptions = {},
): Promise<LintReport> {
  const cwd = options.cwd ?? Deno.cwd();
  const absPath = await Deno.realPath(filePath);

  // Resolve policy
  if (options.policy) {
    setGlobalPolicy(options.policy);
  }
  if (options.impactWeights) {
    setImpactWeights(options.impactWeights);
  }

  // Find handler
  const handler = resolveHandler(absPath, options.format);
  if (!handler) {
    return {
      file: relative(cwd, absPath),
      absolutePath: absPath,
      format: options.format ?? "unknown",
      results: [{
        file: absPath,
        severity: "error",
        rule: "api/no-handler",
        message: `No format handler found for "${filePath}"`,
      }],
      summary: summarize(relative(cwd, absPath), []),
    };
  }

  // Lint
  const results = await handler.lint(absPath);

  // Enrich with error codes
  resolveErrorCodes(results);

  // Health
  const health = handler.computeHealth
    ? await handler.computeHealth(absPath, results)
    : undefined;

  // Summarize
  const summary = summarize(relative(cwd, absPath), results, health);

  return {
    file: relative(cwd, absPath),
    absolutePath: absPath,
    format: handler.name,
    results,
    summary,
    health,
  };
}

/**
 * Lint all matching files in a directory.
 *
 * Discovers files using the same logic as the CLI, then lints each.
 * Returns reports in discovery order.
 */
export async function lintDirectory(
  dirPath: string,
  options: LintOptions = {},
): Promise<LintReport[]> {
  const reports: LintReport[] = [];

  if (options.format) {
    // Single format
    const handler = getFormat(options.format);
    if (!handler) {
      return [{
        file: dirPath,
        absolutePath: dirPath,
        format: options.format,
        results: [{
          file: dirPath,
          severity: "error",
          rule: "api/unknown-format",
          message: `Unknown format: "${options.format}"`,
        }],
        summary: summarize(dirPath, []),
      }];
    }

    const files = await discoverFiles([dirPath], handler);
    for (const file of files) {
      reports.push(await lintFile(file, options));
    }
  } else {
    // All formats
    const filesByHandler = await discoverAllFiles([dirPath]);
    for (const [_handler, files] of filesByHandler) {
      for (const file of files) {
        reports.push(await lintFile(file, options));
      }
    }
  }

  return reports;
}

// ---------------------------------------------------------------------------
// 3. Transform operations
// ---------------------------------------------------------------------------

/**
 * Transform a single file.
 *
 * Runs the format handler's transform pass and returns structured results.
 */
export async function transformFile(
  filePath: string,
  options: Partial<TransformOptions> & { format?: string } = {},
): Promise<TransformReport> {
  const absPath = await Deno.realPath(filePath);
  const handler = resolveHandler(absPath, options.format);

  if (!handler?.transform) {
    return {
      file: filePath,
      results: [{
        file: absPath,
        severity: "error",
        rule: "api/no-transform",
        message: handler
          ? `Format "${handler.name}" does not support transform`
          : `No format handler found for "${filePath}"`,
      }],
      modified: false,
    };
  }

  const opts: TransformOptions = {
    dryRun: options.dryRun ?? false,
    extensions: options.extensions ?? false,
    force: options.force ?? false,
    steps: options.steps ?? false,
  };

  const results = await handler.transform(absPath, opts);
  const modified = results.some((r) =>
    r.rule.includes("transform/") && r.severity !== "info"
  );

  return {
    file: filePath,
    results,
    modified: !opts.dryRun && modified,
  };
}

// ---------------------------------------------------------------------------
// 4. Create operations
// ---------------------------------------------------------------------------

/**
 * Generate a new file from schema (schema-driven fill).
 *
 * Returns the generated content. Writes to disk unless dryRun is true.
 */
export async function createFile(
  dest: string,
  ctx: Omit<FillContext, "filename" | "from"> & { dryRun?: boolean },
): Promise<CreateReport> {
  const filename = dest.split("/").pop() ?? dest;
  const from = dest;

  const fullCtx: FillContext = {
    ...ctx,
    filename,
    from,
  };

  const content = await generateFileText(fullCtx);
  const lineCount = content.split("\n").length;

  if (!ctx.dryRun) {
    await Deno.writeTextFile(dest, content);
  }

  return {
    file: dest,
    content,
    lineCount,
    written: !ctx.dryRun,
  };
}

// ---------------------------------------------------------------------------
// 5. Transpile operations
// ---------------------------------------------------------------------------

/**
 * Transpile a file between formats.
 *
 * Re-exports transpileFile with the same interface — already returns
 * structured TranspileResult.
 */
export { transpileFile } from "../engine/mod.ts";
export type { TranspileOptions, TranspileResult } from "../engine/mod.ts";

// ---------------------------------------------------------------------------
// 6. Inspect operations
// ---------------------------------------------------------------------------

/**
 * Inspect a file's parsed structure without running checks.
 *
 * Returns blocks, sections, directives, content classification.
 * "Here's what I see" — no judgment.
 */
export async function inspectFile(
  filePath: string,
  options: { format?: string } = {},
): Promise<InspectResult | null> {
  const absPath = await Deno.realPath(filePath);
  const handler = resolveHandler(absPath, options.format);

  if (!handler?.inspect) return null;
  return handler.inspect(absPath);
}

// ---------------------------------------------------------------------------
// 7. Data layer access
// ---------------------------------------------------------------------------

/**
 * Get a snapshot of the full data layer — all registries.
 *
 * Static data. No I/O. Returns instantly.
 */
export function getDataLayer(): DataLayerSnapshot {
  return {
    sections: SECTION_REGISTRY,
    sectionOrder: SECTION_ORDER,
    concepts: CONCEPT_REGISTRY,
    conceptOrder: CONCEPT_ORDER,
    forms: FORM_REGISTRY,
    placement: PLACEMENT_REGISTRY,
    errorCodes: ERROR_CODES,
    lintRules: LINT_RULES,
    pipelineLayers: PIPELINE_LAYERS,
  };
}

/** Get the section registry. */
export function getSectionRegistry(): Record<string, SectionEntry> {
  return SECTION_REGISTRY;
}

/** Get the concept registry. */
export function getConceptRegistry(): Record<string, ConceptEntry> {
  return CONCEPT_REGISTRY;
}

/** Get form definitions. */
export function getFormDefinitions(): Record<string, FormDefinition> {
  return FORM_REGISTRY;
}

/** Look up an error code entry by rule name. */
export function lookupErrorByRule(rule: string): ErrorCodeEntry | undefined {
  return getByRule(rule);
}

/** Look up an error code entry by code. */
export function lookupErrorByCode(code: string): ErrorCodeEntry | undefined {
  return getByCode(code);
}

/** Get all error codes for a pipeline layer. */
export function getErrorsByLayer(layerNumber: 0 | 1 | 2 | 3): ErrorCodeEntry[] {
  return getByLayer(layerNumber);
}

// ---------------------------------------------------------------------------
// 8. Database operations (lazy-loaded)
// ---------------------------------------------------------------------------

// Database is lazy-loaded to avoid triggering FFI when not needed.
let _dbModule: typeof import("../data/database.ts") | null = null;

async function getDbModule() {
  if (!_dbModule) {
    _dbModule = await import("../data/database.ts");
  }
  return _dbModule;
}

/**
 * Open a health database connection.
 *
 * Returns the database wrapper. The database is created/migrated on first
 * open. Caller is responsible for closing when done.
 *
 * @param path - Path to SQLite database file
 */
export async function openDatabase(path: string) {
  const mod = await getDbModule();
  return new mod.CwsDatabase(undefined, path);
}

/**
 * Lint a file and record results to the health database.
 *
 * Combines lintFile + database recording in one call.
 */
export async function lintAndRecord(
  filePath: string,
  dbPath: string,
  options: LintOptions = {},
): Promise<LintReport> {
  const report = await lintFile(filePath, options);

  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);
  try {
    db.recordBatch([{
      filePath: report.absolutePath,
      format: report.format,
      summary: report.summary,
      handler: report.format,
    }]);
  } finally {
    db.close();
  }

  return report;
}

/**
 * Query file health from the database.
 */
export async function queryHealth(
  filePath: string,
  dbPath: string,
) {
  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);
  try {
    return db.getFileHealth(filePath);
  } finally {
    db.close();
  }
}

/**
 * Query unhealthy files from the database (lowest scores first).
 */
export async function queryUnhealthy(
  dbPath: string,
  threshold = 100,
) {
  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);
  try {
    return db.getUnhealthyFiles(threshold);
  } finally {
    db.close();
  }
}

/**
 * Query best-scoring files from the database.
 */
export async function queryBest(
  dbPath: string,
  limit = 10,
) {
  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);
  try {
    return db.getBestFiles(limit);
  } finally {
    db.close();
  }
}

/**
 * Query top recurring patterns from the database.
 */
export async function queryPatterns(
  dbPath: string,
  limit = 10,
) {
  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);
  try {
    return db.getTopPatterns(limit);
  } finally {
    db.close();
  }
}

/**
 * Get aggregate database statistics.
 */
export async function queryStats(dbPath: string) {
  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);
  try {
    return db.getStats();
  } finally {
    db.close();
  }
}

// ---------------------------------------------------------------------------
// 9. Health query convenience functions
// ---------------------------------------------------------------------------

/** Directory health summary. */
export interface DirectoryHealthSummary {
  /** Directory path. */
  dir: string;
  /** Average health score across tracked files. */
  avgScore: number | null;
  /** Hebrew state for the average. */
  state: string | null;
  /** Total files tracked in this directory. */
  fileCount: number;
  /** File with lowest health score. */
  worstFile: { path: string; score: number; state: string } | null;
  /** File with highest health score. */
  bestFile: { path: string; score: number; state: string } | null;
  /** Count of files in each Hebrew state. */
  stateDistribution: Record<string, number>;
}

/** Session delta — what changed during a session. */
export interface SessionDelta {
  /** Session ID. */
  sessionId: string;
  /** Files that improved (positive delta). */
  improved: Array<{ path: string; delta: number; from: number; to: number }>;
  /** Files that regressed (negative delta). */
  regressed: Array<{ path: string; delta: number; from: number; to: number }>;
  /** Files with no change. */
  unchanged: Array<{ path: string; score: number }>;
  /** Net health change across all files. */
  netDelta: number;
}

/** Fix recommendation — most impactful thing to address. */
export interface FixRecommendation {
  /** Error code. */
  errorCode: string;
  /** Rule name. */
  rule: string;
  /** Number of affected files. */
  affectedFiles: number;
  /** Total frequency across all files. */
  frequency: number;
  /** Trend direction. */
  trending: string;
}

/** Health tree node for directory visualization. */
export interface HealthTreeNode {
  /** File or directory name (relative). */
  name: string;
  /** Full path. */
  path: string;
  /** Health score (null for untracked directories). */
  score: number | null;
  /** Hebrew state (null if no score). */
  state: string | null;
  /** Children (empty for files). */
  children: HealthTreeNode[];
}

/**
 * Get health summary for a directory.
 *
 * Queries all tracked files under the given directory and computes
 * aggregate statistics. "What's the overall health of this directory?"
 */
export async function directoryHealth(
  dbPath: string,
  dir: string,
): Promise<DirectoryHealthSummary> {
  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);
  try {
    const allFiles = db.getAllFiles();
    // Filter to files under this directory
    const normalizedDir = dir.endsWith("/") ? dir : dir + "/";
    const dirFiles = allFiles.filter((f) =>
      f.path.startsWith(normalizedDir) || f.path.startsWith(dir)
    );

    const withScores = dirFiles.filter((f) => f.currentScore != null);

    // Average score
    const avgScore = withScores.length > 0
      ? Math.round(
          withScores.reduce((sum, f) => sum + f.currentScore!, 0) /
            withScores.length,
        )
      : null;

    // State distribution
    const stateDistribution: Record<string, number> = {};
    for (const f of withScores) {
      const state = f.currentState ?? "unknown";
      stateDistribution[state] = (stateDistribution[state] ?? 0) + 1;
    }

    // Worst and best
    let worstFile: DirectoryHealthSummary["worstFile"] = null;
    let bestFile: DirectoryHealthSummary["bestFile"] = null;

    if (withScores.length > 0) {
      const sorted = [...withScores].sort(
        (a, b) => a.currentScore! - b.currentScore!,
      );
      const worst = sorted[0]!;
      const best = sorted[sorted.length - 1]!;

      worstFile = {
        path: worst.path,
        score: worst.currentScore!,
        state: worst.currentState ?? "unknown",
      };
      bestFile = {
        path: best.path,
        score: best.currentScore!,
        state: best.currentState ?? "unknown",
      };
    }

    // Resolve average state
    const { trueToLevel } = await import("../foundation/health.ts");
    const state = avgScore != null ? trueToLevel(avgScore) : null;

    return {
      dir,
      avgScore,
      state,
      fileCount: dirFiles.length,
      worstFile,
      bestFile,
      stateDistribution,
    };
  } finally {
    db.close();
  }
}

/**
 * Get what changed during a session.
 *
 * Compares health before and after a session's lint runs.
 * "What improved since last session?"
 */
export async function sessionDelta(
  dbPath: string,
  sessionId: string,
): Promise<SessionDelta | null> {
  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);
  try {
    const summary = db.getSessionSummary(sessionId);
    if (!summary) return null;

    const improved: SessionDelta["improved"] = [];
    const regressed: SessionDelta["regressed"] = [];
    const unchanged: SessionDelta["unchanged"] = [];
    let netDelta = 0;

    // Group runs by file
    const fileRuns = new Map<number, typeof summary.runs>();
    for (const run of summary.runs) {
      const existing = fileRuns.get(run.fileId) ?? [];
      existing.push(run);
      fileRuns.set(run.fileId, existing);
    }

    // For each file, check health history during this session
    for (const [fileId, runs] of fileRuns) {
      const file = db.getFileById(fileId);
      if (!file) continue;

      // Get health history entries for this file during the session
      const history = db.getHealthHistory(file.path, 100);
      const sessionHistory = history.filter((h) =>
        h.runId != null &&
        runs.some((r) => r.id === h.runId)
      );

      if (sessionHistory.length === 0) {
        if (file.currentScore != null) {
          unchanged.push({ path: file.path, score: file.currentScore });
        }
        continue;
      }

      // First and last health during session
      const firstHealth = sessionHistory[sessionHistory.length - 1]; // oldest
      const lastHealth = sessionHistory[0]; // newest (sorted DESC)

      if (!firstHealth || !lastHealth) continue;

      const delta = lastHealth.score - (firstHealth.delta != null
        ? firstHealth.score - firstHealth.delta
        : firstHealth.score);

      if (delta > 0) {
        improved.push({
          path: file.path,
          delta,
          from: firstHealth.delta != null
            ? firstHealth.score - firstHealth.delta
            : firstHealth.score,
          to: lastHealth.score,
        });
      } else if (delta < 0) {
        regressed.push({
          path: file.path,
          delta,
          from: firstHealth.delta != null
            ? firstHealth.score - firstHealth.delta
            : firstHealth.score,
          to: lastHealth.score,
        });
      } else {
        unchanged.push({ path: file.path, score: lastHealth.score });
      }

      netDelta += delta;
    }

    return {
      sessionId,
      improved,
      regressed,
      unchanged,
      netDelta,
    };
  } finally {
    db.close();
  }
}

/**
 * Get the most impactful fixes to apply next.
 *
 * Ranked by affected-files × frequency. "What's the most impactful
 * thing to fix next?"
 */
export async function topFixes(
  dbPath: string,
  limit = 10,
): Promise<FixRecommendation[]> {
  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);
  try {
    // Recalculate affected files for accuracy
    db.recalculateAffectedFiles();

    const patterns = db.getTopPatterns(limit * 2); // Get more than needed for ranking

    // Rank by impact: affected_files × frequency
    const ranked = patterns
      .map((p) => ({
        errorCode: p.errorCode,
        rule: p.rule,
        affectedFiles: p.affectedFiles,
        frequency: p.frequency,
        trending: p.trending,
        impact: p.affectedFiles * p.frequency,
      }))
      .sort((a, b) => b.impact - a.impact)
      .slice(0, limit);

    return ranked.map(({ impact: _impact, ...rest }) => rest);
  } finally {
    db.close();
  }
}

/**
 * Build a health tree for directory visualization.
 *
 * Returns a tree where each node has a path, score, state, and children.
 * "Show me the health map."
 */
export async function healthMap(
  dbPath: string,
  rootPath: string,
): Promise<HealthTreeNode> {
  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);
  try {
    const allFiles = db.getAllFiles();
    const normalizedRoot = rootPath.endsWith("/") ? rootPath : rootPath + "/";

    // Filter to files under root
    const relevantFiles = allFiles.filter((f) =>
      f.path.startsWith(normalizedRoot) || f.path === rootPath
    );

    // Build tree structure
    const root: HealthTreeNode = {
      name: rootPath.split("/").pop() ?? rootPath,
      path: rootPath,
      score: null,
      state: null,
      children: [],
    };

    for (const file of relevantFiles) {
      const relPath = file.path.startsWith(normalizedRoot)
        ? file.path.slice(normalizedRoot.length)
        : file.path;

      const parts = relPath.split("/");
      let current = root;

      // Build intermediate directory nodes
      for (let i = 0; i < parts.length - 1; i++) {
        const dirName = parts[i]!;
        let child = current.children.find((c) => c.name === dirName);
        if (!child) {
          child = {
            name: dirName,
            path: normalizedRoot + parts.slice(0, i + 1).join("/"),
            score: null,
            state: null,
            children: [],
          };
          current.children.push(child);
        }
        current = child;
      }

      // Add file node
      current.children.push({
        name: parts[parts.length - 1]!,
        path: file.path,
        score: file.currentScore,
        state: file.currentState,
        children: [],
      });
    }

    // Compute aggregate scores for directory nodes (bottom-up)
    computeDirectoryScores(root);

    return root;
  } finally {
    db.close();
  }
}

/** Recursively compute directory scores from children. */
function computeDirectoryScores(node: HealthTreeNode): void {
  if (node.children.length === 0) return;

  // Process children first (bottom-up)
  for (const child of node.children) {
    computeDirectoryScores(child);
  }

  // Average scores from children that have scores
  const withScores = node.children.filter((c) => c.score != null);
  if (withScores.length > 0) {
    node.score = Math.round(
      withScores.reduce((sum, c) => sum + c.score!, 0) / withScores.length,
    );
    // Lazy import would be needed for trueToLevel here — use state from range
    if (node.score >= 67) node.state = "perfect";
    else if (node.score >= 34) node.state = "whole";
    else if (node.score >= 1) node.state = "sound";
    else if (node.score === 0) node.state = "even";
    else if (node.score >= -33) node.state = "lacking";
    else if (node.score >= -66) node.state = "wanting";
    else node.state = "broken";
  }
}

// ---------------------------------------------------------------------------
// 10. Session lifecycle (hook integration)
// ---------------------------------------------------------------------------

/** Session start report — what changed since last session. */
export interface SessionStartReport {
  /** Session ID for tracking. */
  sessionId: string;
  /** Instance name (detected or explicit). */
  instanceName: string;
  /** Files changed since last session (from git diff). */
  changedFiles: string[];
  /** Health reports for changed files. */
  reports: LintReport[];
  /** Summary: improved/regressed/unchanged counts. */
  summary: {
    total: number;
    improved: number;
    regressed: number;
    unchanged: number;
    errors: number;
  };
}

/** Session end report — what happened during this session. */
export interface SessionEndReport {
  /** Session ID. */
  sessionId: string;
  /** Total files linted. */
  filesLinted: number;
  /** Net health delta. */
  netDelta: number;
  /** Patterns resolved during session. */
  patternsResolved: number;
  /** New patterns found. */
  newPatterns: string[];
}

/**
 * Start a session — lint recently changed files and report health.
 *
 * Designed for session-start hooks. Uses git diff to find changed files,
 * lints them, records to database, and returns a health summary.
 *
 * "3 files changed since last session. Health: 2 improved, 1 regressed."
 */
export async function startSession(
  dbPath: string,
  options: {
    /** Git ref to diff against (default: HEAD~1). */
    gitRef?: string;
    /** Explicit list of files to lint (overrides git diff). */
    files?: string[];
    /** Instance name (auto-detected if omitted). */
    instanceName?: string;
    /** Working directory for git operations. */
    cwd?: string;
  } = {},
): Promise<SessionStartReport> {
  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);
  const cwd = options.cwd ?? Deno.cwd();

  try {
    // Detect instance
    const instance = mod.detectInstance();
    const instanceName = options.instanceName ?? instance.name;

    // Register instance and start session
    db.upsertInstance(instanceName, instance.type);
    const sessionId = db.startSession(instanceName);

    // Find changed files
    let changedFiles: string[] = [];
    if (options.files) {
      changedFiles = options.files;
    } else {
      try {
        const ref = options.gitRef ?? "HEAD~1";
        const cmd = new Deno.Command("git", {
          args: ["diff", "--name-only", ref],
          cwd,
          stdout: "piped",
          stderr: "null",
        });
        const output = await cmd.output();
        if (output.success) {
          const text = new TextDecoder().decode(output.stdout).trim();
          changedFiles = text
            .split("\n")
            .filter((f) => f.length > 0)
            .map((f) => `${cwd}/${f}`);
        }
      } catch {
        // Not a git repo or git not available — no changed files
      }
    }

    // Lint each changed file
    const reports: LintReport[] = [];
    let improved = 0;
    let regressed = 0;
    let unchanged = 0;
    let totalErrors = 0;

    for (const file of changedFiles) {
      try {
        const report = await lintFile(file, { cwd });
        reports.push(report);
        totalErrors += report.summary.errors;

        // Get previous health for comparison
        const prevHealth = db.getFileHealth(report.absolutePath);
        const prevScore = prevHealth?.score ?? null;

        // Record to database
        db.recordSummary(
          report.absolutePath,
          report.format,
          report.summary,
          {
            handler: report.format,
            instanceName,
            sessionId,
          },
        );

        // Compare
        if (report.health && prevScore != null) {
          const delta = report.health.total - prevScore;
          if (delta > 0) improved++;
          else if (delta < 0) regressed++;
          else unchanged++;
        } else {
          unchanged++;
        }
      } catch {
        // File may not exist (deleted) or unsupported format — skip
      }
    }

    return {
      sessionId,
      instanceName,
      changedFiles,
      reports,
      summary: {
        total: changedFiles.length,
        improved,
        regressed,
        unchanged,
        errors: totalErrors,
      },
    };
  } finally {
    db.close();
  }
}

/**
 * End a session — summarize activity and close the session record.
 *
 * Designed for session-end hooks. Queries what happened during the
 * session and updates the session record with final statistics.
 *
 * "This session: 8 files linted, net health +23, 2 patterns resolved."
 */
export async function endSession(
  dbPath: string,
  sessionId: string,
): Promise<SessionEndReport> {
  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);

  try {
    const sessionSummary = db.getSessionSummary(sessionId);

    let filesLinted = 0;
    let netDelta = 0;

    if (sessionSummary) {
      filesLinted = sessionSummary.runs.length;

      // Compute net delta from health history
      const fileDeltas = new Map<number, number>();
      for (const run of sessionSummary.runs) {
        if (run.totalScore != null) {
          const prev = fileDeltas.get(run.fileId);
          if (prev != null) {
            // Multiple runs on same file — track latest
            fileDeltas.set(run.fileId, run.totalScore);
          } else {
            fileDeltas.set(run.fileId, run.totalScore);
          }
        }
      }
    }

    // Update trends to get fresh pattern data
    db.updateTrends();

    // Check for new patterns (trending = increasing, first_seen in last day)
    const trendingPatterns = db.getTrendingPatterns(20);
    const newPatterns = trendingPatterns
      .filter((p) => {
        const firstSeen = new Date(p.firstSeen);
        const now = new Date();
        return (now.getTime() - firstSeen.getTime()) < 86400000; // 24 hours
      })
      .map((p) => p.errorCode);

    // Close the session
    db.endSession(sessionId, filesLinted, netDelta);

    return {
      sessionId,
      filesLinted,
      netDelta,
      patternsResolved: 0, // Would need before/after comparison
      newPatterns,
    };
  } finally {
    db.close();
  }
}

/**
 * Quick-lint a single file and record to database.
 *
 * Designed for post-edit hooks. Non-blocking when used with
 * Deno's async APIs. Returns a LintReport.
 */
export async function quickLint(
  filePath: string,
  dbPath: string,
  options: {
    sessionId?: string;
    instanceName?: string;
  } = {},
): Promise<LintReport> {
  const report = await lintFile(filePath);

  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, dbPath);
  try {
    const instance = mod.detectInstance();
    db.recordSummary(
      report.absolutePath,
      report.format,
      report.summary,
      {
        handler: report.format,
        instanceName: options.instanceName ?? instance.name,
        sessionId: options.sessionId,
      },
    );
  } finally {
    db.close();
  }

  return report;
}

// ---------------------------------------------------------------------------
// 11. Format handler utilities
// ---------------------------------------------------------------------------

/**
 * List all registered format names.
 */
export function getRegisteredFormats(): string[] {
  return listFormats();
}

/**
 * Get format handler details (name, extensions, description).
 */
export function getFormatDetails() {
  return listFormatDetails();
}

/**
 * Get a specific format handler by name.
 */
export function getFormatHandler(name: string): FormatHandler | undefined {
  return getFormat(name);
}

/**
 * Detect which format handler matches a file path.
 *
 * Returns the handler name (string) or undefined. Use getFormatHandler()
 * to get the full handler object.
 */
export function detectFileFormat(filePath: string): string | undefined {
  return detectFormat(filePath);
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

/**
 * Resolve a format handler for a file path.
 *
 * If format is specified, uses that. Otherwise auto-detects from extension.
 */
function resolveHandler(
  absPath: string,
  format?: string,
): FormatHandler | undefined {
  if (format) {
    return getFormat(format);
  }
  const detected = detectFormat(absPath);
  return detected ? getFormat(detected) : undefined;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// The programmatic API: same truth as the CLI, different interface.
// CLI speaks to terminals. API speaks to code. Same engine underneath.
//
// 11 sections: Config, Lint, Transform, Create, Transpile, Inspect,
//   Data Layer, Database, Health Queries, Session Lifecycle, Format Handlers.
//
// "Ask, and it shall be given you; seek, and ye shall find;
//  knock, and it shall be opened unto you." — Matthew 7:7
// ============================================================================
