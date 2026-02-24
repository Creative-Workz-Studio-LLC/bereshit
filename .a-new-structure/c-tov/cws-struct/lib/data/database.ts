// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/data/database.ts
// key:     B-tov-cws-struct-lib-data-database
// title:   CWS Struct — SQLite Database
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// updated: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: SQLite database for persistent lint tracking, health history,
//          pattern detection, and CPI-SI instance awareness.
//
//          Schema uses two migration versions:
//            v1: Core tables (files, lint_runs, actions, health_history)
//            v2: Pattern detection + instance tracking (patterns, instances, sessions)
//
//          All operations are synchronous (SQLite via FFI).
//          Database file defaults to .cws-struct/cws-struct.db in project root.
//
// biblical_foundation: "Remember the former things of old." — Isaiah 46:9
//   (History matters. Track what happened so future decisions are informed.)
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Imports
// ---------------------------------------------------------------------------

import { Database } from "@db/sqlite";
import type { Severity } from "../types/core.ts";
import type { LintSummary } from "../types/foundation.ts";
// Direct import — barrel would create cycle: foundation/mod → code-schema → data/mod → database
import { ToolError } from "../foundation/tool-error.ts";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

/** Default database file path (relative to project root). */
const DEFAULT_DB_DIR = ".cws-struct";
const DEFAULT_DB_NAME = "cws-struct.db";

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

/** Database migration definition. */
interface Migration {
  version: number;
  description: string;
  sql: string;
}

/** File record from the database. */
export interface FileRecord {
  id: number;
  path: string;
  format: string;
  subtype: string | null;
  firstSeen: string;
  lastSeen: string;
  currentScore: number | null;
  currentState: string | null;
  currentTrite: number | null;
}

/** Lint run record from the database. */
export interface LintRunRecord {
  id: number;
  fileId: number;
  timestamp: string;
  handler: string;
  totalScore: number | null;
  hebrewState: string | null;
  trite: number | null;
  layersRun: number;
  cascaded: boolean;
  errorCount: number;
  warnCount: number;
  infoCount: number;
  instanceName: string | null;
  instanceType: string | null;
  sessionId: string | null;
  durationMs: number | null;
}

/** Health history entry. */
export interface HealthHistoryRecord {
  id: number;
  fileId: number;
  timestamp: string;
  score: number;
  hebrewState: string;
  trite: number | null;
  delta: number | null;
  previousState: string | null;
  runId: number | null;
}

/** Pattern record. */
export interface PatternRecord {
  id: number;
  errorCode: string;
  rule: string;
  frequency: number;
  firstSeen: string;
  lastSeen: string;
  affectedFiles: number;
  trending: string;
}

/** Comprehensive file health report. */
export interface FileHealthReport {
  path: string;
  format: string;
  subtype: string | null;
  score: number | null;
  state: string | null;
  trite: number | null;
  lastRun: LintRunRecord | null;
  history: HealthHistoryRecord[];
}

/** Action record from the database. */
export interface ActionRecord {
  id: number;
  layer: number;
  errorCode: string | null;
  rule: string;
  severity: string;
  direction: number;
  container: string | null;
  block: string | null;
  message: string;
  sourceLine: number | null;
  cascaded: boolean;
}

/** Lint run with all its actions. */
export interface RunDetailsReport {
  run: LintRunRecord;
  actions: ActionRecord[];
}

/** Instance record from the database. */
export interface InstanceRecord {
  name: string;
  type: string;
  firstSeen: string;
  runsCount: number;
  lastActive: string | null;
}

/** Session record from the database. */
export interface SessionRecord {
  id: string;
  instanceName: string | null;
  startTime: string;
  endTime: string | null;
  filesLinted: number;
  healthDelta: number;
}

/** Detailed pattern report with affected files. */
export interface PatternDetailReport {
  pattern: PatternRecord;
  affectedFilePaths: string[];
  recentOccurrences: Array<{
    filePath: string;
    timestamp: string;
    runId: number;
  }>;
}

/** Instance activity summary. */
export interface InstanceActivityReport {
  instance: InstanceRecord;
  recentRuns: LintRunRecord[];
  sessionsCount: number;
}

/** Session summary with runs and patterns found. */
export interface SessionSummaryReport {
  session: SessionRecord;
  runs: LintRunRecord[];
  patternsFound: string[];
}

/** Detected CPI-SI instance from environment. */
export interface DetectedInstance {
  name: string;
  type: "cpisi" | "human" | "automated";
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Migrations
// ---------------------------------------------------------------------------

const MIGRATIONS: Migration[] = [
  {
    version: 1,
    description: "Core tables — files, lint_runs, actions, health_history",
    sql: `
      CREATE TABLE IF NOT EXISTS schema_version (
        version INTEGER PRIMARY KEY,
        applied_at TEXT NOT NULL DEFAULT (datetime('now')),
        description TEXT
      );

      CREATE TABLE IF NOT EXISTS files (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        path TEXT UNIQUE NOT NULL,
        format TEXT NOT NULL,
        subtype TEXT,
        first_seen TEXT NOT NULL DEFAULT (datetime('now')),
        last_seen TEXT NOT NULL DEFAULT (datetime('now')),
        current_score INTEGER,
        current_state TEXT,
        current_trite INTEGER
      );

      CREATE TABLE IF NOT EXISTS lint_runs (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        file_id INTEGER NOT NULL REFERENCES files(id) ON DELETE CASCADE,
        timestamp TEXT NOT NULL DEFAULT (datetime('now')),
        handler TEXT NOT NULL,
        total_score INTEGER,
        hebrew_state TEXT,
        trite INTEGER,
        layers_run INTEGER DEFAULT 4,
        cascaded INTEGER DEFAULT 0,
        error_count INTEGER DEFAULT 0,
        warn_count INTEGER DEFAULT 0,
        info_count INTEGER DEFAULT 0,
        instance_name TEXT,
        instance_type TEXT,
        session_id TEXT,
        duration_ms INTEGER
      );

      CREATE TABLE IF NOT EXISTS actions (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        run_id INTEGER NOT NULL REFERENCES lint_runs(id) ON DELETE CASCADE,
        layer INTEGER NOT NULL CHECK (layer >= 0 AND layer <= 3),
        error_code TEXT,
        rule TEXT NOT NULL,
        severity TEXT NOT NULL CHECK (severity IN ('error', 'warn', 'info')),
        direction INTEGER NOT NULL CHECK (direction IN (-1, 0, 1)),
        container TEXT,
        block TEXT,
        message TEXT NOT NULL,
        source_line INTEGER,
        cascaded INTEGER DEFAULT 0
      );

      CREATE TABLE IF NOT EXISTS health_history (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        file_id INTEGER NOT NULL REFERENCES files(id) ON DELETE CASCADE,
        timestamp TEXT NOT NULL DEFAULT (datetime('now')),
        score INTEGER NOT NULL,
        hebrew_state TEXT NOT NULL,
        trite INTEGER,
        delta INTEGER,
        previous_state TEXT,
        run_id INTEGER REFERENCES lint_runs(id) ON DELETE SET NULL
      );

      CREATE INDEX IF NOT EXISTS idx_files_path ON files(path);
      CREATE INDEX IF NOT EXISTS idx_files_format ON files(format);
      CREATE INDEX IF NOT EXISTS idx_lint_runs_file ON lint_runs(file_id);
      CREATE INDEX IF NOT EXISTS idx_lint_runs_timestamp ON lint_runs(timestamp);
      CREATE INDEX IF NOT EXISTS idx_actions_run ON actions(run_id);
      CREATE INDEX IF NOT EXISTS idx_actions_error_code ON actions(error_code);
      CREATE INDEX IF NOT EXISTS idx_actions_layer ON actions(layer);
      CREATE INDEX IF NOT EXISTS idx_health_history_file ON health_history(file_id);
      CREATE INDEX IF NOT EXISTS idx_health_history_timestamp ON health_history(timestamp);
    `,
  },
  {
    version: 2,
    description: "Pattern detection + instance tracking",
    sql: `
      CREATE TABLE IF NOT EXISTS patterns (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        error_code TEXT NOT NULL,
        rule TEXT NOT NULL,
        frequency INTEGER DEFAULT 1,
        first_seen TEXT NOT NULL DEFAULT (datetime('now')),
        last_seen TEXT NOT NULL DEFAULT (datetime('now')),
        affected_files INTEGER DEFAULT 1,
        trending TEXT DEFAULT 'stable' CHECK (trending IN ('increasing', 'stable', 'decreasing'))
      );

      CREATE TABLE IF NOT EXISTS instances (
        name TEXT PRIMARY KEY,
        type TEXT NOT NULL CHECK (type IN ('cpisi', 'human', 'automated')),
        first_seen TEXT NOT NULL DEFAULT (datetime('now')),
        runs_count INTEGER DEFAULT 0,
        last_active TEXT
      );

      CREATE TABLE IF NOT EXISTS sessions (
        id TEXT PRIMARY KEY,
        instance_name TEXT REFERENCES instances(name) ON DELETE SET NULL,
        start_time TEXT NOT NULL DEFAULT (datetime('now')),
        end_time TEXT,
        files_linted INTEGER DEFAULT 0,
        health_delta INTEGER DEFAULT 0
      );

      CREATE INDEX IF NOT EXISTS idx_patterns_error_code ON patterns(error_code);
      CREATE INDEX IF NOT EXISTS idx_patterns_frequency ON patterns(frequency DESC);
      CREATE INDEX IF NOT EXISTS idx_sessions_instance ON sessions(instance_name);
    `,
  },
  {
    version: 3,
    description: "Unique constraint on patterns(error_code, rule)",
    sql: `
      CREATE UNIQUE INDEX IF NOT EXISTS idx_patterns_code_rule
        ON patterns(error_code, rule);
    `,
  },
];

// ---------------------------------------------------------------------------
// Database class
// ---------------------------------------------------------------------------

/**
 * CWS Struct database — SQLite persistence layer.
 *
 * Tracks files, lint runs, atomic actions, health history,
 * patterns, instances, and sessions. All operations are synchronous.
 *
 * Usage:
 *   const db = new CwsDatabase("/path/to/project");
 *   // ... lint operations ...
 *   db.close();
 */
export class CwsDatabase {
  private db: Database;
  private readonly path: string;

  /**
   * Open (or create) the database.
   *
   * @param projectRoot Root directory of the project being linted.
   *                    Database is created at {projectRoot}/.cws-struct/cws-struct.db
   * @param dbPath      Override database file path (for testing).
   */
  constructor(projectRoot?: string, dbPath?: string) {
    if (dbPath) {
      this.path = dbPath;
    } else if (projectRoot) {
      const dir = `${projectRoot}/${DEFAULT_DB_DIR}`;
      try {
        Deno.mkdirSync(dir, { recursive: true });
      } catch {
        // Directory may already exist
      }
      this.path = `${dir}/${DEFAULT_DB_NAME}`;
    } else {
      this.path = ":memory:";
    }

    this.db = new Database(this.path);

    // Enable WAL mode for better concurrent read performance
    this.db.exec("PRAGMA journal_mode = WAL");
    this.db.exec("PRAGMA foreign_keys = ON");

    // Run migrations
    this.migrate();
  }

  /** Database file path (":memory:" for in-memory). */
  get dbPath(): string {
    return this.path;
  }

  /** Current schema version. */
  get schemaVersion(): number {
    try {
      const row = this.db.prepare(
        "SELECT MAX(version) as v FROM schema_version",
      ).get<{ v: number | null }>();
      return row?.v ?? 0;
    } catch {
      return 0;
    }
  }

  // -------------------------------------------------------------------------
  // Migration
  // -------------------------------------------------------------------------

  /** Run all pending migrations. */
  private migrate(): void {
    const current = this.schemaVersion;

    for (const migration of MIGRATIONS) {
      if (migration.version <= current) continue;

      this.db.exec("BEGIN TRANSACTION");
      try {
        // Execute migration SQL (split on semicolons for multi-statement)
        this.db.exec(migration.sql);

        // Record the migration
        this.db.prepare(
          "INSERT OR REPLACE INTO schema_version (version, description) VALUES (?, ?)",
        ).run(migration.version, migration.description);

        this.db.exec("COMMIT");
      } catch (err) {
        this.db.exec("ROLLBACK");
        throw new ToolError("CWS-T00-070", {
          version: String(migration.version),
          error: err instanceof Error ? err.message : String(err),
        });
      }
    }
  }

  // -------------------------------------------------------------------------
  // File operations
  // -------------------------------------------------------------------------

  /**
   * Upsert a file record. Returns the file ID.
   *
   * If the file already exists, updates last_seen, format, and subtype.
   * If new, creates the record.
   */
  upsertFile(
    path: string,
    format: string,
    subtype?: string,
  ): number {
    const now = new Date().toISOString();

    // Try update first
    this.db.prepare(`
      INSERT INTO files (path, format, subtype, first_seen, last_seen)
      VALUES (?, ?, ?, ?, ?)
      ON CONFLICT(path) DO UPDATE SET
        format = excluded.format,
        subtype = excluded.subtype,
        last_seen = excluded.last_seen
    `).run(path, format, subtype ?? null, now, now);

    const row = this.db.prepare(
      "SELECT id FROM files WHERE path = ?",
    ).get<{ id: number }>(path);

    return row!.id;
  }

  /** Get file record by path. */
  getFile(path: string): FileRecord | undefined {
    const row = this.db.prepare(
      "SELECT * FROM files WHERE path = ?",
    ).get<Record<string, unknown>>(path);

    if (!row) return undefined;
    return rowToFileRecord(row);
  }

  /** Get a file record by database ID. */
  getFileById(id: number): FileRecord | undefined {
    const row = this.db.prepare(
      "SELECT * FROM files WHERE id = ?",
    ).get<Record<string, unknown>>(id);

    if (!row) return undefined;
    return rowToFileRecord(row);
  }

  /** Update file's current health state. */
  updateFileHealth(
    fileId: number,
    score: number,
    state: string,
    trite: number,
  ): void {
    this.db.prepare(`
      UPDATE files SET current_score = ?, current_state = ?, current_trite = ?
      WHERE id = ?
    `).run(score, state, trite, fileId);
  }

  // -------------------------------------------------------------------------
  // Lint run operations
  // -------------------------------------------------------------------------

  /**
   * Record a lint run. Returns the run ID.
   */
  recordLintRun(opts: {
    fileId: number;
    handler: string;
    totalScore?: number;
    hebrewState?: string;
    trite?: number;
    layersRun?: number;
    cascaded?: boolean;
    errorCount: number;
    warnCount: number;
    infoCount: number;
    instanceName?: string;
    instanceType?: string;
    sessionId?: string;
    durationMs?: number;
  }): number {
    const now = new Date().toISOString();

    this.db.prepare(`
      INSERT INTO lint_runs (
        file_id, timestamp, handler, total_score, hebrew_state, trite,
        layers_run, cascaded, error_count, warn_count, info_count,
        instance_name, instance_type, session_id, duration_ms
      ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    `).run(
      opts.fileId,
      now,
      opts.handler,
      opts.totalScore ?? null,
      opts.hebrewState ?? null,
      opts.trite ?? null,
      opts.layersRun ?? 4,
      opts.cascaded ? 1 : 0,
      opts.errorCount,
      opts.warnCount,
      opts.infoCount,
      opts.instanceName ?? null,
      opts.instanceType ?? null,
      opts.sessionId ?? null,
      opts.durationMs ?? null,
    );

    // Get the last inserted row ID
    const row = this.db.prepare("SELECT last_insert_rowid() as id").get<{ id: number }>();
    return row!.id;
  }

  // -------------------------------------------------------------------------
  // Action operations
  // -------------------------------------------------------------------------

  /**
   * Record atomic actions from a lint run (bulk insert).
   */
  recordActions(
    runId: number,
    actions: Array<{
      layer: number;
      errorCode?: string;
      rule: string;
      severity: Severity;
      direction: -1 | 0 | 1;
      container?: string;
      block?: string;
      message: string;
      sourceLine?: number;
      cascaded?: boolean;
    }>,
  ): void {
    if (actions.length === 0) return;

    const stmt = this.db.prepare(`
      INSERT INTO actions (
        run_id, layer, error_code, rule, severity, direction,
        container, block, message, source_line, cascaded
      ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    `);

    // Use SAVEPOINT to allow nesting inside batch transactions
    this.db.exec("SAVEPOINT actions_insert");
    try {
      for (const a of actions) {
        stmt.run(
          runId,
          a.layer,
          a.errorCode ?? null,
          a.rule,
          a.severity,
          a.direction,
          a.container ?? null,
          a.block ?? null,
          a.message,
          a.sourceLine ?? null,
          a.cascaded ? 1 : 0,
        );
      }
      this.db.exec("RELEASE SAVEPOINT actions_insert");
    } catch (err) {
      this.db.exec("ROLLBACK TO SAVEPOINT actions_insert");
      throw err;
    }
  }

  // -------------------------------------------------------------------------
  // Health history
  // -------------------------------------------------------------------------

  /**
   * Record a health history entry and update the file's current health.
   */
  recordHealth(
    fileId: number,
    score: number,
    state: string,
    trite: number,
    runId?: number,
  ): void {
    // Get previous score for delta calculation
    const prev = this.db.prepare(
      "SELECT current_score, current_state FROM files WHERE id = ?",
    ).get<{ current_score: number | null; current_state: string | null }>(fileId);

    const delta = prev?.current_score != null ? score - prev.current_score : null;

    this.db.prepare(`
      INSERT INTO health_history (
        file_id, score, hebrew_state, trite, delta, previous_state, run_id
      ) VALUES (?, ?, ?, ?, ?, ?, ?)
    `).run(
      fileId,
      score,
      state,
      trite,
      delta,
      prev?.current_state ?? null,
      runId ?? null,
    );

    // Update file's current state
    this.updateFileHealth(fileId, score, state, trite);
  }

  /**
   * Get health history for a file, newest first.
   */
  getHealthHistory(filePath: string, limit = 50): HealthHistoryRecord[] {
    const rows = this.db.prepare(`
      SELECT h.* FROM health_history h
      JOIN files f ON h.file_id = f.id
      WHERE f.path = ?
      ORDER BY h.timestamp DESC, h.id DESC
      LIMIT ?
    `).all<Record<string, unknown>>(filePath, limit);

    return rows.map((r) => ({
      id: r.id as number,
      fileId: r.file_id as number,
      timestamp: r.timestamp as string,
      score: r.score as number,
      hebrewState: r.hebrew_state as string,
      trite: r.trite as number | null,
      delta: r.delta as number | null,
      previousState: r.previous_state as string | null,
      runId: r.run_id as number | null,
    }));
  }

  // -------------------------------------------------------------------------
  // Convenience: record a full lint summary
  // -------------------------------------------------------------------------

  /**
   * Record a complete lint summary — file, run, actions, health.
   *
   * This is the main entry point for persisting lint results.
   * Returns the run ID.
   */
  recordSummary(
    filePath: string,
    format: string,
    summary: LintSummary,
    opts?: {
      subtype?: string;
      handler?: string;
      instanceName?: string;
      instanceType?: string;
      sessionId?: string;
      durationMs?: number;
    },
  ): number {
    const fileId = this.upsertFile(filePath, format, opts?.subtype);

    const runId = this.recordLintRun({
      fileId,
      handler: opts?.handler ?? format,
      totalScore: summary.health?.total,
      hebrewState: summary.health?.level,
      trite: summary.health?.trite,
      errorCount: summary.errors,
      warnCount: summary.warnings,
      infoCount: summary.infos,
      instanceName: opts?.instanceName,
      instanceType: opts?.instanceType,
      sessionId: opts?.sessionId,
      durationMs: opts?.durationMs,
    });

    // Record lint results as actions
    const actions = summary.results.map((r) => ({
      layer: ruleToLayer(r.rule),
      rule: r.rule,
      severity: r.severity,
      direction: severityToDirection(r.severity),
      message: r.message,
      sourceLine: r.line,
    }));

    if (actions.length > 0) {
      this.recordActions(runId, actions);

      // Auto-update pattern tracking from this run's actions
      this.updatePatternsFromRun(runId);
    }

    // Record health history if available
    if (summary.health) {
      this.recordHealth(
        fileId,
        summary.health.total,
        summary.health.level,
        summary.health.trite,
        runId,
      );
    }

    return runId;
  }

  // -------------------------------------------------------------------------
  // Pattern detection
  // -------------------------------------------------------------------------

  /**
   * Update pattern frequency for an error code.
   * Uses ON CONFLICT on the unique (error_code, rule) index (migration v3).
   */
  updatePattern(errorCode: string, rule: string): void {
    const now = new Date().toISOString();
    this.db.prepare(`
      INSERT INTO patterns (error_code, rule, frequency, first_seen, last_seen, affected_files)
      VALUES (?, ?, 1, ?, ?, 0)
      ON CONFLICT(error_code, rule) DO UPDATE SET
        frequency = frequency + 1,
        last_seen = ?
    `).run(errorCode, rule, now, now, now);
  }

  /**
   * Analyze a run's actions and update pattern tracking.
   * Called automatically by recordSummary after recording actions.
   */
  updatePatternsFromRun(runId: number): void {
    const codes = this.db.prepare(
      "SELECT DISTINCT error_code, rule FROM actions WHERE run_id = ? AND error_code IS NOT NULL",
    ).all<{ error_code: string; rule: string }>(runId);

    for (const c of codes) {
      this.updatePattern(c.error_code, c.rule);
    }
  }

  /**
   * Recalculate affected_files count for all patterns.
   * Counts distinct files where each error_code + rule combination appears.
   */
  recalculateAffectedFiles(): void {
    this.db.exec(`
      UPDATE patterns SET affected_files = (
        SELECT COUNT(DISTINCT r.file_id)
        FROM actions a
        JOIN lint_runs r ON a.run_id = r.id
        WHERE a.error_code = patterns.error_code AND a.rule = patterns.rule
      )
    `);
  }

  /**
   * Update trending status for all patterns.
   *
   * Compares distinct-file-count in recent runs (last 7 days) vs older
   * runs (8-14 days ago). >20% increase = "increasing", >20% decrease
   * = "decreasing", else "stable". New patterns (no older data) default
   * to "increasing" if they have recent activity.
   */
  updateTrends(): void {
    const patterns = this.db.prepare(
      "SELECT id, error_code, rule FROM patterns",
    ).all<{ id: number; error_code: string; rule: string }>();

    const stmtRecent = this.db.prepare(`
      SELECT COUNT(DISTINCT r.file_id) as cnt
      FROM actions a JOIN lint_runs r ON a.run_id = r.id
      WHERE a.error_code = ? AND a.rule = ?
      AND r.timestamp > datetime('now', '-7 days')
    `);

    const stmtOlder = this.db.prepare(`
      SELECT COUNT(DISTINCT r.file_id) as cnt
      FROM actions a JOIN lint_runs r ON a.run_id = r.id
      WHERE a.error_code = ? AND a.rule = ?
      AND r.timestamp BETWEEN datetime('now', '-14 days') AND datetime('now', '-7 days')
    `);

    const stmtUpdate = this.db.prepare(
      "UPDATE patterns SET trending = ? WHERE id = ?",
    );

    for (const p of patterns) {
      const recent = stmtRecent.get<{ cnt: number }>(p.error_code, p.rule);
      const older = stmtOlder.get<{ cnt: number }>(p.error_code, p.rule);

      const r = recent?.cnt ?? 0;
      const o = older?.cnt ?? 0;

      let trending = "stable";
      if (o > 0 && r > o * 1.2) trending = "increasing";
      else if (o > 0 && r < o * 0.8) trending = "decreasing";
      else if (o === 0 && r > 0) trending = "increasing";

      stmtUpdate.run(trending, p.id);
    }
  }

  /** Get top patterns by frequency. */
  getTopPatterns(limit = 20): PatternRecord[] {
    return this.db.prepare(`
      SELECT * FROM patterns ORDER BY frequency DESC LIMIT ?
    `).all<Record<string, unknown>>(limit).map(rowToPatternRecord);
  }

  /** Get patterns that are trending upward. */
  getTrendingPatterns(limit = 20): PatternRecord[] {
    return this.db.prepare(`
      SELECT * FROM patterns WHERE trending = 'increasing'
      ORDER BY frequency DESC LIMIT ?
    `).all<Record<string, unknown>>(limit).map(rowToPatternRecord);
  }

  /**
   * Get detailed information about a specific pattern.
   * Includes list of affected files and recent occurrences.
   */
  getPatternDetail(errorCode: string, rule?: string): PatternDetailReport | null {
    const row = rule
      ? this.db.prepare(
          "SELECT * FROM patterns WHERE error_code = ? AND rule = ?",
        ).get<Record<string, unknown>>(errorCode, rule)
      : this.db.prepare(
          "SELECT * FROM patterns WHERE error_code = ? LIMIT 1",
        ).get<Record<string, unknown>>(errorCode);

    if (!row) return null;

    const pattern = rowToPatternRecord(row);

    // Affected file paths
    const affectedFilePaths = this.db.prepare(`
      SELECT DISTINCT f.path
      FROM actions a
      JOIN lint_runs r ON a.run_id = r.id
      JOIN files f ON r.file_id = f.id
      WHERE a.error_code = ? AND a.rule = ?
      ORDER BY f.path
    `).all<{ path: string }>(pattern.errorCode, pattern.rule)
      .map((r) => r.path);

    // Recent occurrences
    const recentOccurrences = this.db.prepare(`
      SELECT f.path as file_path, r.timestamp, r.id as run_id
      FROM actions a
      JOIN lint_runs r ON a.run_id = r.id
      JOIN files f ON r.file_id = f.id
      WHERE a.error_code = ? AND a.rule = ?
      ORDER BY r.timestamp DESC, r.id DESC
      LIMIT 20
    `).all<{ file_path: string; timestamp: string; run_id: number }>(
      pattern.errorCode, pattern.rule,
    ).map((r) => ({
      filePath: r.file_path,
      timestamp: r.timestamp,
      runId: r.run_id,
    }));

    return { pattern, affectedFilePaths, recentOccurrences };
  }

  // -------------------------------------------------------------------------
  // Instance tracking
  // -------------------------------------------------------------------------

  /**
   * Register or update a CPI-SI instance.
   */
  upsertInstance(name: string, type: "cpisi" | "human" | "automated"): void {
    if (this.schemaVersion < 2) return;

    const now = new Date().toISOString();
    this.db.prepare(`
      INSERT INTO instances (name, type, first_seen, last_active)
      VALUES (?, ?, ?, ?)
      ON CONFLICT(name) DO UPDATE SET
        last_active = excluded.last_active,
        runs_count = runs_count + 1
    `).run(name, type, now, now);
  }

  /**
   * Start a session. Returns the session ID.
   */
  startSession(instanceName: string): string {
    if (this.schemaVersion < 2) return "";

    const id = crypto.randomUUID();
    this.db.prepare(`
      INSERT INTO sessions (id, instance_name) VALUES (?, ?)
    `).run(id, instanceName);
    return id;
  }

  /**
   * End a session with summary statistics.
   */
  endSession(sessionId: string, filesLinted: number, healthDelta: number): void {
    if (this.schemaVersion < 2) return;

    const now = new Date().toISOString();
    this.db.prepare(`
      UPDATE sessions SET
        end_time = ?,
        files_linted = ?,
        health_delta = ?
      WHERE id = ?
    `).run(now, filesLinted, healthDelta, sessionId);
  }

  /** Get an instance record by name. */
  getInstance(name: string): InstanceRecord | null {
    const row = this.db.prepare(
      "SELECT * FROM instances WHERE name = ?",
    ).get<Record<string, unknown>>(name);

    if (!row) return null;

    return {
      name: row.name as string,
      type: row.type as string,
      firstSeen: row.first_seen as string,
      runsCount: row.runs_count as number,
      lastActive: row.last_active as string | null,
    };
  }

  /**
   * Get activity summary for a CPI-SI instance.
   * Returns recent runs and session count.
   */
  getInstanceActivity(name: string): InstanceActivityReport | null {
    const instance = this.getInstance(name);
    if (!instance) return null;

    const recentRuns = this.db.prepare(`
      SELECT * FROM lint_runs
      WHERE instance_name = ?
      ORDER BY timestamp DESC, id DESC
      LIMIT 50
    `).all<Record<string, unknown>>(name).map(rowToRunRecord);

    const sessionsRow = this.db.prepare(
      "SELECT COUNT(*) as cnt FROM sessions WHERE instance_name = ?",
    ).get<{ cnt: number }>(name);

    return {
      instance,
      recentRuns,
      sessionsCount: sessionsRow?.cnt ?? 0,
    };
  }

  /** Get a session record by ID. */
  getSession(sessionId: string): SessionRecord | null {
    const row = this.db.prepare(
      "SELECT * FROM sessions WHERE id = ?",
    ).get<Record<string, unknown>>(sessionId);

    if (!row) return null;

    return {
      id: row.id as string,
      instanceName: row.instance_name as string | null,
      startTime: row.start_time as string,
      endTime: row.end_time as string | null,
      filesLinted: row.files_linted as number,
      healthDelta: row.health_delta as number,
    };
  }

  /**
   * Get session summary — the session record, all runs, and patterns found.
   */
  getSessionSummary(sessionId: string): SessionSummaryReport | null {
    const session = this.getSession(sessionId);
    if (!session) return null;

    const runs = this.db.prepare(`
      SELECT * FROM lint_runs
      WHERE session_id = ?
      ORDER BY timestamp ASC, id ASC
    `).all<Record<string, unknown>>(sessionId).map(rowToRunRecord);

    // Distinct error codes found during this session
    const patternsFound = this.db.prepare(`
      SELECT DISTINCT a.error_code
      FROM actions a
      JOIN lint_runs r ON a.run_id = r.id
      WHERE r.session_id = ? AND a.error_code IS NOT NULL
      ORDER BY a.error_code
    `).all<{ error_code: string }>(sessionId).map((r) => r.error_code);

    return { session, runs, patternsFound };
  }

  // -------------------------------------------------------------------------
  // Query helpers
  // -------------------------------------------------------------------------

  /** Count total files tracked. */
  countFiles(): number {
    const row = this.db.prepare("SELECT COUNT(*) as n FROM files").get<{ n: number }>();
    return row?.n ?? 0;
  }

  /** Count total lint runs. */
  countRuns(): number {
    const row = this.db.prepare("SELECT COUNT(*) as n FROM lint_runs").get<{ n: number }>();
    return row?.n ?? 0;
  }

  /** Get files by format. */
  getFilesByFormat(format: string): FileRecord[] {
    return this.db.prepare(
      "SELECT * FROM files WHERE format = ? ORDER BY path",
    ).all<Record<string, unknown>>(format).map(rowToFileRecord);
  }

  /** Get files below a health threshold. */
  getUnhealthyFiles(threshold: number): FileRecord[] {
    return this.db.prepare(
      "SELECT * FROM files WHERE current_score IS NOT NULL AND current_score < ? ORDER BY current_score ASC",
    ).all<Record<string, unknown>>(threshold).map(rowToFileRecord);
  }

  /** Get recent lint runs for a file. */
  getRecentRuns(filePath: string, limit = 10): LintRunRecord[] {
    return this.db.prepare(`
      SELECT r.* FROM lint_runs r
      JOIN files f ON r.file_id = f.id
      WHERE f.path = ?
      ORDER BY r.timestamp DESC, r.id DESC
      LIMIT ?
    `).all<Record<string, unknown>>(filePath, limit).map(rowToRunRecord);
  }

  /** Get aggregate statistics. */
  getStats(): {
    totalFiles: number;
    totalRuns: number;
    avgScore: number | null;
    healthyFiles: number;
    unhealthyFiles: number;
  } {
    const stats = this.db.prepare(`
      SELECT
        COUNT(*) as total_files,
        AVG(current_score) as avg_score,
        SUM(CASE WHEN current_score >= 34 THEN 1 ELSE 0 END) as healthy,
        SUM(CASE WHEN current_score < 0 THEN 1 ELSE 0 END) as unhealthy
      FROM files WHERE current_score IS NOT NULL
    `).get<Record<string, unknown>>();

    const runs = this.db.prepare("SELECT COUNT(*) as n FROM lint_runs").get<{ n: number }>();

    return {
      totalFiles: (stats?.total_files as number) ?? 0,
      totalRuns: runs?.n ?? 0,
      avgScore: stats?.avg_score != null ? Math.round(stats.avg_score as number) : null,
      healthyFiles: (stats?.healthy as number) ?? 0,
      unhealthyFiles: (stats?.unhealthy as number) ?? 0,
    };
  }

  /**
   * Get comprehensive file health — current state + recent history.
   * Returns null if the file isn't tracked.
   */
  getFileHealth(filePath: string): FileHealthReport | null {
    const file = this.getFile(filePath);
    if (!file) return null;

    const history = this.getHealthHistory(filePath, 20);
    const lastRun = this.getRecentRuns(filePath, 1);

    return {
      path: file.path,
      format: file.format,
      subtype: file.subtype,
      score: file.currentScore,
      state: file.currentState,
      trite: file.currentTrite,
      lastRun: lastRun[0] ?? null,
      history,
    };
  }

  /**
   * Get run details — the run record plus all associated actions.
   */
  getRunDetails(runId: number): RunDetailsReport | null {
    const run = this.db.prepare(
      "SELECT * FROM lint_runs WHERE id = ?",
    ).get<Record<string, unknown>>(runId);

    if (!run) return null;

    const actions = this.db.prepare(
      "SELECT * FROM actions WHERE run_id = ? ORDER BY layer ASC, id ASC",
    ).all<Record<string, unknown>>(runId).map((r) => ({
      id: r.id as number,
      layer: r.layer as number,
      errorCode: r.error_code as string | null,
      rule: r.rule as string,
      severity: r.severity as string,
      direction: r.direction as number,
      container: r.container as string | null,
      block: r.block as string | null,
      message: r.message as string,
      sourceLine: r.source_line as number | null,
      cascaded: !!(r.cascaded as number),
    }));

    return {
      run: rowToRunRecord(run),
      actions,
    };
  }

  /** Get best files by health score (highest first). */
  getBestFiles(limit = 10): FileRecord[] {
    return this.db.prepare(
      "SELECT * FROM files WHERE current_score IS NOT NULL ORDER BY current_score DESC LIMIT ?",
    ).all<Record<string, unknown>>(limit).map(rowToFileRecord);
  }

  /** Get all tracked files ordered by path. */
  getAllFiles(): FileRecord[] {
    return this.db.prepare(
      "SELECT * FROM files ORDER BY path",
    ).all<Record<string, unknown>>().map(rowToFileRecord);
  }

  /**
   * Batch record multiple file summaries in a single transaction.
   * Used when linting directories (many files).
   */
  recordBatch(
    entries: Array<{
      filePath: string;
      format: string;
      summary: LintSummary;
      subtype?: string;
      handler?: string;
      instanceName?: string;
      instanceType?: string;
      sessionId?: string;
      durationMs?: number;
    }>,
  ): number[] {
    const runIds: number[] = [];

    this.db.exec("BEGIN TRANSACTION");
    try {
      for (const entry of entries) {
        const runId = this.recordSummary(
          entry.filePath,
          entry.format,
          entry.summary,
          {
            subtype: entry.subtype,
            handler: entry.handler,
            instanceName: entry.instanceName,
            instanceType: entry.instanceType,
            sessionId: entry.sessionId,
            durationMs: entry.durationMs,
          },
        );
        runIds.push(runId);
      }
      this.db.exec("COMMIT");
    } catch (err) {
      this.db.exec("ROLLBACK");
      throw err;
    }

    return runIds;
  }

  // -------------------------------------------------------------------------
  // Lifecycle
  // -------------------------------------------------------------------------

  /** Close the database connection. */
  close(): void {
    this.db.close();
  }
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/** Map a row object to FileRecord. */
function rowToFileRecord(r: Record<string, unknown>): FileRecord {
  return {
    id: r.id as number,
    path: r.path as string,
    format: r.format as string,
    subtype: r.subtype as string | null,
    firstSeen: r.first_seen as string,
    lastSeen: r.last_seen as string,
    currentScore: r.current_score as number | null,
    currentState: r.current_state as string | null,
    currentTrite: r.current_trite as number | null,
  };
}

/** Map a row object to LintRunRecord. */
function rowToRunRecord(r: Record<string, unknown>): LintRunRecord {
  return {
    id: r.id as number,
    fileId: r.file_id as number,
    timestamp: r.timestamp as string,
    handler: r.handler as string,
    totalScore: r.total_score as number | null,
    hebrewState: r.hebrew_state as string | null,
    trite: r.trite as number | null,
    layersRun: r.layers_run as number,
    cascaded: !!(r.cascaded as number),
    errorCount: r.error_count as number,
    warnCount: r.warn_count as number,
    infoCount: r.info_count as number,
    instanceName: r.instance_name as string | null,
    instanceType: r.instance_type as string | null,
    sessionId: r.session_id as string | null,
    durationMs: r.duration_ms as number | null,
  };
}

/**
 * Map a rule string to its detection layer.
 *
 * Uses prefix heuristics when the error catalog isn't available.
 * The proper mapping via error catalog happens at the pipeline level.
 */
function ruleToLayer(rule: string): number {
  const r = rule.toLowerCase();

  // Layer 0 — R[50]: whole file structure
  if (r.startsWith("io/") || r.startsWith("parse/") ||
      r.startsWith("block/") || r.startsWith("blocks/") ||
      r.startsWith("pragma/boundary") || r.startsWith("pragma/end") ||
      r.startsWith("pragma/omni") ||
      r.startsWith("metadata/boundary") || r.startsWith("metadata/end_boundary") ||
      r.startsWith("content/boundary") || r.startsWith("content/end_boundary") ||
      r.startsWith("closing/boundary") || r.startsWith("closing/end_boundary") ||
      r.startsWith("style/") || r.startsWith("structure/") ||
      r.startsWith("directive/") || r.startsWith("template/build") ||
      r.startsWith("derived/build") || r.startsWith("schema/") ||
      r.startsWith("shebang/") || r.startsWith("top/ofd") ||
      r === "metadata/exists" || r === "content/exists" || r === "closing/exists") {
    return 0;
  }

  // Layer 3 — R[05]: content validation
  if (r.startsWith("content/body") || r.startsWith("content/closing") ||
      r.startsWith("content/setup") || r.startsWith("content/metadata") ||
      r.startsWith("doc/") || r.startsWith("comment-meta/") ||
      r.startsWith("template/pragma") || r.startsWith("template/metadata") ||
      r.startsWith("template/placeholder") ||
      r.startsWith("children/") || r.startsWith("grounding/")) {
    return 3;
  }

  // Layer 2 — R[10]: container validation
  if (r.startsWith("content/") || r.startsWith("identity/register") ||
      r.startsWith("identity/") && (r.includes("/placeholder") || r.includes(".")) ||
      r.startsWith("closing/X6") || r.startsWith("closing/test") ||
      r.startsWith("closing/main") || r.startsWith("emphasis/") ||
      r.startsWith("scaling/") || r.startsWith("setup/alias") ||
      r.startsWith("metadata/invalid") || r.startsWith("closing/invalid") ||
      r.includes("/type") || r.includes("/extension")) {
    return 2;
  }

  // Layer 1 — R[25]: everything else (structure, ordering, fields)
  return 1;
}

/** Map a row to PatternRecord. */
function rowToPatternRecord(r: Record<string, unknown>): PatternRecord {
  return {
    id: r.id as number,
    errorCode: r.error_code as string,
    rule: r.rule as string,
    frequency: r.frequency as number,
    firstSeen: r.first_seen as string,
    lastSeen: r.last_seen as string,
    affectedFiles: r.affected_files as number,
    trending: r.trending as string,
  };
}

/** Map severity to health direction. */
function severityToDirection(severity: Severity): -1 | 0 | 1 {
  switch (severity) {
    case "error": return -1;
    case "warn": return -1;
    case "info": return 0;
  }
}

// ---------------------------------------------------------------------------
// Environment detection
// ---------------------------------------------------------------------------

/**
 * Detect the current CPI-SI instance from environment.
 *
 * Priority:
 *   1. CWS_INSTANCE env var — explicit instance name
 *   2. CWS_INSTANCE_TYPE env var — explicit type override
 *   3. GITHUB_ACTIONS / GITLAB_CI / CI — automated CI detection
 *   4. CLAUDE_CODE — Claude Code hooks context
 *   5. USER/USERNAME env var — human fallback
 *   6. "anonymous" — last resort
 */
export function detectInstance(): DetectedInstance {
  // Explicit instance
  const envInstance = Deno.env.get("CWS_INSTANCE");
  if (envInstance) {
    const envType = Deno.env.get("CWS_INSTANCE_TYPE");
    const validTypes = ["cpisi", "human", "automated"];
    const type = (envType && validTypes.includes(envType))
      ? envType as "cpisi" | "human" | "automated"
      : "cpisi";
    return { name: envInstance, type };
  }

  // CI detection
  if (Deno.env.get("GITHUB_ACTIONS")) return { name: "github-actions", type: "automated" };
  if (Deno.env.get("GITLAB_CI")) return { name: "gitlab-ci", type: "automated" };
  if (Deno.env.get("CI")) return { name: "ci", type: "automated" };

  // Claude Code context
  if (Deno.env.get("CLAUDE_CODE")) return { name: "claude-code", type: "cpisi" };

  // Human fallback
  const user = Deno.env.get("USER") ?? Deno.env.get("USERNAME");
  if (user) return { name: user, type: "human" };

  return { name: "anonymous", type: "human" };
}

// ============================================================================
// CLOSING
// ============================================================================
//
// SQLite persistence layer for cws-struct. 7 tables across 3 migrations.
// Tracks files, lint runs, atomic actions, health history, patterns,
// instances, and sessions.
//
// Pattern detection: frequency tracking, affected file counts, trend analysis.
// Instance awareness: CPI-SI identity tracking, session lifecycle, activity queries.
// Environment detection: auto-detect instance from env vars (CWS_INSTANCE, CI, etc.)
//
// "Remember the former things of old." — Isaiah 46:9
// History matters. Track what happened so future decisions are informed.
//
// Changelog:
//   a-01.00 — Initial implementation: 2 migrations, 7 tables,
//             file/run/action/health CRUD, basic pattern + instance stubs.
//   a-01.01 — Pattern detection + CPI-SI awareness: migration v3 (UNIQUE
//             constraint), proper upsert, updatePatternsFromRun, trend
//             analysis, affected file counts, instance activity queries,
//             session summaries, environment detection.
// ============================================================================
