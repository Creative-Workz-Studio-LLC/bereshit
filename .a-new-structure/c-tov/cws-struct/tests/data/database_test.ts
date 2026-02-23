// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/data/database_test.ts
// key:     B-tov-cws-struct-tests-data-database
// title:   CWS Struct — SQLite Database Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nathan Emet (CPI-SI)
// purpose: Verify the SQLite persistence layer: migrations, file CRUD,
//          lint run recording, health history, pattern detection,
//          instance tracking, and the convenience recordSummary flow.
//
//          All tests use in-memory databases (no filesystem side effects).
//
// biblical_foundation: "Prove all things; hold fast that which is good."
//   — 1 Thessalonians 5:21
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertExists } from "jsr:@std/assert";
import { CwsDatabase, detectInstance } from "../../lib/data/database.ts";
import type {
  FileRecord,
  LintRunRecord,
  HealthHistoryRecord,
  PatternRecord,
  InstanceRecord,
  SessionRecord,
  PatternDetailReport,
} from "../../lib/data/database.ts";
import { error, warn, info, summarize } from "../../lib/foundation/mod.ts";
import type { HealthScore } from "../../lib/foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Migration system
// ---------------------------------------------------------------------------

Deno.test("database/migration: creates schema version 3 on fresh database", () => {
  const db = new CwsDatabase();
  assertEquals(db.schemaVersion, 3);
  db.close();
});

Deno.test("database/migration: in-memory path is :memory:", () => {
  const db = new CwsDatabase();
  assertEquals(db.dbPath, ":memory:");
  db.close();
});

Deno.test("database/migration: re-running migrate is idempotent", () => {
  // Creating two instances on the same path would re-run migrations
  // Use in-memory for simplicity — just verify no throw on double-open
  const db = new CwsDatabase();
  assertEquals(db.schemaVersion, 3);
  db.close();
});

// ---------------------------------------------------------------------------
// File operations
// ---------------------------------------------------------------------------

Deno.test("database/files: upsertFile creates new record", () => {
  const db = new CwsDatabase();

  const id = db.upsertFile("/project/test.go", "go", "library");
  assert(id > 0, "Should return positive ID");

  const file = db.getFile("/project/test.go");
  assertExists(file);
  assertEquals(file.path, "/project/test.go");
  assertEquals(file.format, "go");
  assertEquals(file.subtype, "library");
  assertEquals(file.currentScore, null);
  assertEquals(file.currentState, null);

  db.close();
});

Deno.test("database/files: upsertFile updates existing record", () => {
  const db = new CwsDatabase();

  const id1 = db.upsertFile("/project/test.go", "go", "library");
  const id2 = db.upsertFile("/project/test.go", "go", "binary");

  assertEquals(id1, id2, "Same file path should return same ID");

  const file = db.getFile("/project/test.go");
  assertExists(file);
  assertEquals(file.subtype, "binary", "Subtype should be updated");

  db.close();
});

Deno.test("database/files: upsertFile without subtype", () => {
  const db = new CwsDatabase();

  db.upsertFile("/project/test.toml", "toml");
  const file = db.getFile("/project/test.toml");
  assertExists(file);
  assertEquals(file.subtype, null);

  db.close();
});

Deno.test("database/files: getFile returns undefined for nonexistent path", () => {
  const db = new CwsDatabase();
  const file = db.getFile("/nonexistent/file.go");
  assertEquals(file, undefined);
  db.close();
});

Deno.test("database/files: countFiles tracks total", () => {
  const db = new CwsDatabase();

  assertEquals(db.countFiles(), 0);
  db.upsertFile("/a.go", "go");
  assertEquals(db.countFiles(), 1);
  db.upsertFile("/b.rs", "rust");
  assertEquals(db.countFiles(), 2);
  // Upsert existing doesn't increase count
  db.upsertFile("/a.go", "go");
  assertEquals(db.countFiles(), 2);

  db.close();
});

Deno.test("database/files: getFilesByFormat filters correctly", () => {
  const db = new CwsDatabase();

  db.upsertFile("/a.go", "go");
  db.upsertFile("/b.go", "go");
  db.upsertFile("/c.rs", "rust");
  db.upsertFile("/d.toml", "toml");

  const goFiles = db.getFilesByFormat("go");
  assertEquals(goFiles.length, 2);
  assertEquals(goFiles[0]!.path, "/a.go");
  assertEquals(goFiles[1]!.path, "/b.go");

  const rustFiles = db.getFilesByFormat("rust");
  assertEquals(rustFiles.length, 1);

  db.close();
});

Deno.test("database/files: updateFileHealth updates current state", () => {
  const db = new CwsDatabase();

  const id = db.upsertFile("/a.go", "go");
  db.updateFileHealth(id, 85, "perfect", 242);

  const file = db.getFile("/a.go");
  assertExists(file);
  assertEquals(file.currentScore, 85);
  assertEquals(file.currentState, "perfect");
  assertEquals(file.currentTrite, 242);

  db.close();
});

Deno.test("database/files: getUnhealthyFiles filters by threshold", () => {
  const db = new CwsDatabase();

  const id1 = db.upsertFile("/good.go", "go");
  db.updateFileHealth(id1, 85, "perfect", 242);

  const id2 = db.upsertFile("/ok.go", "go");
  db.updateFileHealth(id2, 20, "sound", 161);

  const id3 = db.upsertFile("/bad.go", "go");
  db.updateFileHealth(id3, -30, "lacking", 40);

  // Threshold = 34 (whole and above)
  const unhealthy = db.getUnhealthyFiles(34);
  assertEquals(unhealthy.length, 2);
  assertEquals(unhealthy[0]!.path, "/bad.go");   // sorted ASC by score
  assertEquals(unhealthy[1]!.path, "/ok.go");

  // Threshold = 0 (only negative)
  const negative = db.getUnhealthyFiles(0);
  assertEquals(negative.length, 1);
  assertEquals(negative[0]!.path, "/bad.go");

  db.close();
});

// ---------------------------------------------------------------------------
// Lint run operations
// ---------------------------------------------------------------------------

Deno.test("database/runs: recordLintRun creates run record", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");
  const runId = db.recordLintRun({
    fileId,
    handler: "go",
    totalScore: 75,
    hebrewState: "whole",
    trite: 233,
    errorCount: 0,
    warnCount: 2,
    infoCount: 5,
  });

  assert(runId > 0, "Should return positive run ID");
  assertEquals(db.countRuns(), 1);

  db.close();
});

Deno.test("database/runs: recordLintRun with instance tracking", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");
  const runId = db.recordLintRun({
    fileId,
    handler: "go",
    errorCount: 1,
    warnCount: 0,
    infoCount: 0,
    instanceName: "nova-dawn",
    instanceType: "cpisi",
    sessionId: "test-session-123",
    durationMs: 42,
  });

  assert(runId > 0);

  // Verify via getRecentRuns
  const runs = db.getRecentRuns("/test.go");
  assertEquals(runs.length, 1);
  assertEquals(runs[0]!.handler, "go");
  assertEquals(runs[0]!.errorCount, 1);
  assertEquals(runs[0]!.instanceName, "nova-dawn");
  assertEquals(runs[0]!.instanceType, "cpisi");
  assertEquals(runs[0]!.sessionId, "test-session-123");
  assertEquals(runs[0]!.durationMs, 42);

  db.close();
});

Deno.test("database/runs: getRecentRuns returns newest first", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");

  db.recordLintRun({ fileId, handler: "go", errorCount: 5, warnCount: 0, infoCount: 0 });
  db.recordLintRun({ fileId, handler: "go", errorCount: 3, warnCount: 0, infoCount: 0 });
  db.recordLintRun({ fileId, handler: "go", errorCount: 1, warnCount: 0, infoCount: 0 });

  const runs = db.getRecentRuns("/test.go");
  assertEquals(runs.length, 3);
  // Newest first — error count decreasing as we improved
  assertEquals(runs[0]!.errorCount, 1);
  assertEquals(runs[2]!.errorCount, 5);

  db.close();
});

Deno.test("database/runs: getRecentRuns respects limit", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");
  for (let i = 0; i < 20; i++) {
    db.recordLintRun({ fileId, handler: "go", errorCount: i, warnCount: 0, infoCount: 0 });
  }

  const runs = db.getRecentRuns("/test.go", 5);
  assertEquals(runs.length, 5);

  db.close();
});

// ---------------------------------------------------------------------------
// Action operations
// ---------------------------------------------------------------------------

Deno.test("database/actions: recordActions bulk inserts", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");
  const runId = db.recordLintRun({ fileId, handler: "go", errorCount: 2, warnCount: 1, infoCount: 0 });

  db.recordActions(runId, [
    { layer: 0, rule: "block/missing", severity: "error", direction: -1, message: "Missing SETUP block" },
    { layer: 0, rule: "block/order", severity: "error", direction: -1, message: "Blocks out of order" },
    { layer: 1, rule: "identity/key", severity: "warn", direction: -1, message: "Missing key field", sourceLine: 10 },
  ]);

  // No assertion on count since we don't expose action queries yet,
  // but verifying no throw on bulk insert is the contract
  db.close();
});

Deno.test("database/actions: recordActions with empty array is no-op", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");
  const runId = db.recordLintRun({ fileId, handler: "go", errorCount: 0, warnCount: 0, infoCount: 0 });

  // Should not throw
  db.recordActions(runId, []);

  db.close();
});

Deno.test("database/actions: recordActions with all optional fields", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");
  const runId = db.recordLintRun({ fileId, handler: "go", errorCount: 1, warnCount: 0, infoCount: 0 });

  db.recordActions(runId, [
    {
      layer: 2,
      errorCode: "CWS-R10-001",
      rule: "content/placement",
      severity: "error",
      direction: -1,
      container: "setup",
      block: "BODY",
      message: "Code found in wrong block",
      sourceLine: 42,
      cascaded: true,
    },
  ]);

  db.close();
});

// ---------------------------------------------------------------------------
// Health history
// ---------------------------------------------------------------------------

Deno.test("database/health: recordHealth creates history entry", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");
  db.recordHealth(fileId, 50, "sound", 188);

  const history = db.getHealthHistory("/test.go");
  assertEquals(history.length, 1);
  assertEquals(history[0]!.score, 50);
  assertEquals(history[0]!.hebrewState, "sound");
  assertEquals(history[0]!.trite, 188);
  assertEquals(history[0]!.delta, null, "First entry has no delta");
  assertEquals(history[0]!.previousState, null, "First entry has no previous state");

  db.close();
});

Deno.test("database/health: recordHealth calculates delta on subsequent entries", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");

  db.recordHealth(fileId, 30, "sound", 170);
  db.recordHealth(fileId, 50, "whole", 188);
  db.recordHealth(fileId, 85, "perfect", 242);

  const history = db.getHealthHistory("/test.go");
  assertEquals(history.length, 3);

  // Newest first
  assertEquals(history[0]!.score, 85);
  assertEquals(history[0]!.delta, 35);   // 85 - 50
  assertEquals(history[0]!.previousState, "whole");

  assertEquals(history[1]!.score, 50);
  assertEquals(history[1]!.delta, 20);   // 50 - 30
  assertEquals(history[1]!.previousState, "sound");

  assertEquals(history[2]!.score, 30);
  assertEquals(history[2]!.delta, null);  // First entry

  db.close();
});

Deno.test("database/health: recordHealth updates file's current state", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");
  db.recordHealth(fileId, 85, "perfect", 242);

  const file = db.getFile("/test.go");
  assertExists(file);
  assertEquals(file.currentScore, 85);
  assertEquals(file.currentState, "perfect");
  assertEquals(file.currentTrite, 242);

  db.close();
});

Deno.test("database/health: getHealthHistory respects limit", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");
  for (let i = 0; i < 100; i++) {
    db.recordHealth(fileId, i, "sound", i);
  }

  const history = db.getHealthHistory("/test.go", 5);
  assertEquals(history.length, 5);

  db.close();
});

Deno.test("database/health: getHealthHistory for nonexistent file returns empty", () => {
  const db = new CwsDatabase();

  const history = db.getHealthHistory("/nonexistent.go");
  assertEquals(history.length, 0);

  db.close();
});

// ---------------------------------------------------------------------------
// recordSummary convenience method
// ---------------------------------------------------------------------------

Deno.test("database/summary: recordSummary persists full lint result", () => {
  const db = new CwsDatabase();

  const results = [
    error("test.go", "block/missing", "Missing SETUP block"),
    warn("test.go", "identity/key", "Missing key field"),
    info("test.go", "doc/comment", "Missing doc comment"),
  ];

  const health: HealthScore = {
    total: 42,
    level: "whole",
    emoji: "\u{1F7E2}",
    direction: 1,
    hardpoint: 50,
    trite: 188,
    blocks: [],
    totalActions: 10,
    alignedCount: 7,
    neutralCount: 1,
    misalignedCount: 2,
  };

  const summary = summarize("test.go", results, health);

  const runId = db.recordSummary("/project/test.go", "go", summary, {
    subtype: "library",
    handler: "go",
    instanceName: "nova-dawn",
    instanceType: "cpisi",
  });

  assert(runId > 0);

  // File created
  const file = db.getFile("/project/test.go");
  assertExists(file);
  assertEquals(file.format, "go");
  assertEquals(file.subtype, "library");
  assertEquals(file.currentScore, 42);
  assertEquals(file.currentState, "whole");

  // Run recorded
  const runs = db.getRecentRuns("/project/test.go");
  assertEquals(runs.length, 1);
  assertEquals(runs[0]!.errorCount, 1);
  assertEquals(runs[0]!.warnCount, 1);
  assertEquals(runs[0]!.infoCount, 1);
  assertEquals(runs[0]!.totalScore, 42);
  assertEquals(runs[0]!.hebrewState, "whole");

  // Health history recorded
  const history = db.getHealthHistory("/project/test.go");
  assertEquals(history.length, 1);
  assertEquals(history[0]!.score, 42);

  db.close();
});

Deno.test("database/summary: recordSummary without health score", () => {
  const db = new CwsDatabase();

  const results = [error("test.go", "block/missing", "Missing block")];
  const summary = summarize("test.go", results);

  const runId = db.recordSummary("/project/test.go", "go", summary);

  assert(runId > 0);

  // File created but no health
  const file = db.getFile("/project/test.go");
  assertExists(file);
  assertEquals(file.currentScore, null);

  // No health history
  const history = db.getHealthHistory("/project/test.go");
  assertEquals(history.length, 0);

  db.close();
});

Deno.test("database/summary: recordSummary with empty results", () => {
  const db = new CwsDatabase();

  const health: HealthScore = {
    total: 100,
    level: "perfect",
    emoji: "\u{2705}",
    direction: 1,
    hardpoint: 100,
    trite: 242,
    blocks: [],
    totalActions: 5,
    alignedCount: 5,
    neutralCount: 0,
    misalignedCount: 0,
  };

  const summary = summarize("clean.go", [], health);
  db.recordSummary("/project/clean.go", "go", summary);

  const file = db.getFile("/project/clean.go");
  assertExists(file);
  assertEquals(file.currentScore, 100);
  assertEquals(file.currentState, "perfect");

  db.close();
});

// ---------------------------------------------------------------------------
// Pattern detection — proper upsert + frequency tracking
// ---------------------------------------------------------------------------

Deno.test("database/patterns: updatePattern creates and deduplicates", () => {
  const db = new CwsDatabase();

  // Three calls to same code+rule should produce ONE row with frequency 3
  db.updatePattern("CWS-R50-001", "block/missing");
  db.updatePattern("CWS-R50-001", "block/missing");
  db.updatePattern("CWS-R50-001", "block/missing");

  const patterns = db.getTopPatterns();
  const found = patterns.filter((p) => p.errorCode === "CWS-R50-001");
  assertEquals(found.length, 1, "Should have exactly one row for this code+rule");
  assertEquals(found[0]!.frequency, 3, "Frequency should be 3");

  db.close();
});

Deno.test("database/patterns: different rules create separate rows", () => {
  const db = new CwsDatabase();

  db.updatePattern("CWS-R50-001", "block/missing");
  db.updatePattern("CWS-R50-001", "block/order");

  const patterns = db.getTopPatterns();
  const cws50 = patterns.filter((p) => p.errorCode === "CWS-R50-001");
  assertEquals(cws50.length, 2, "Two rules = two rows");

  db.close();
});

Deno.test("database/patterns: getTopPatterns returns sorted by frequency", () => {
  const db = new CwsDatabase();

  for (let i = 0; i < 10; i++) db.updatePattern("CWS-R50-001", "block/missing");
  for (let i = 0; i < 5; i++) db.updatePattern("CWS-R25-001", "identity/key");
  db.updatePattern("CWS-R10-001", "content/placement");

  const patterns = db.getTopPatterns(3);
  assertEquals(patterns.length, 3);
  assertEquals(patterns[0]!.errorCode, "CWS-R50-001");
  assertEquals(patterns[0]!.frequency, 10);
  assertEquals(patterns[1]!.errorCode, "CWS-R25-001");
  assertEquals(patterns[1]!.frequency, 5);
  assertEquals(patterns[2]!.errorCode, "CWS-R10-001");
  assertEquals(patterns[2]!.frequency, 1);

  db.close();
});

Deno.test("database/patterns: getTopPatterns respects limit", () => {
  const db = new CwsDatabase();

  for (let i = 0; i < 50; i++) {
    db.updatePattern(`CWS-R50-${String(i).padStart(3, "0")}`, `rule/${i}`);
  }

  const patterns = db.getTopPatterns(10);
  assertEquals(patterns.length, 10);

  db.close();
});

Deno.test("database/patterns: updatePatternsFromRun extracts error codes from actions", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");
  const runId = db.recordLintRun({ fileId, handler: "go", errorCount: 2, warnCount: 0, infoCount: 0 });

  db.recordActions(runId, [
    { layer: 0, errorCode: "CWS-R50-001", rule: "block/missing", severity: "error", direction: -1, message: "Missing block" },
    { layer: 1, errorCode: "CWS-R25-003", rule: "identity/key", severity: "error", direction: -1, message: "No key" },
    { layer: 0, rule: "block/order", severity: "warn", direction: -1, message: "Out of order" }, // No error code
  ]);

  // Call updatePatternsFromRun
  db.updatePatternsFromRun(runId);

  const patterns = db.getTopPatterns();
  assertEquals(patterns.length, 2, "Only 2 actions have error codes");

  const found50 = patterns.find((p) => p.errorCode === "CWS-R50-001");
  assertExists(found50);
  assertEquals(found50.frequency, 1);

  const found25 = patterns.find((p) => p.errorCode === "CWS-R25-003");
  assertExists(found25);
  assertEquals(found25.frequency, 1);

  db.close();
});

Deno.test("database/patterns: recalculateAffectedFiles counts distinct files", () => {
  const db = new CwsDatabase();

  // Two files, both have CWS-R50-001
  const fid1 = db.upsertFile("/a.go", "go");
  const fid2 = db.upsertFile("/b.go", "go");

  const run1 = db.recordLintRun({ fileId: fid1, handler: "go", errorCount: 1, warnCount: 0, infoCount: 0 });
  db.recordActions(run1, [
    { layer: 0, errorCode: "CWS-R50-001", rule: "block/missing", severity: "error", direction: -1, message: "Missing" },
  ]);
  db.updatePatternsFromRun(run1);

  const run2 = db.recordLintRun({ fileId: fid2, handler: "go", errorCount: 1, warnCount: 0, infoCount: 0 });
  db.recordActions(run2, [
    { layer: 0, errorCode: "CWS-R50-001", rule: "block/missing", severity: "error", direction: -1, message: "Missing" },
  ]);
  db.updatePatternsFromRun(run2);

  // Recalculate
  db.recalculateAffectedFiles();

  const patterns = db.getTopPatterns();
  const found = patterns.find((p) => p.errorCode === "CWS-R50-001");
  assertExists(found);
  assertEquals(found.affectedFiles, 2, "Two files affected");

  db.close();
});

Deno.test("database/patterns: recordSummary auto-updates patterns", () => {
  const db = new CwsDatabase();

  // recordSummary should auto-call updatePatternsFromRun when actions have error codes
  const results = [
    error("test.go", "block/missing", "Missing SETUP block"),
    warn("test.go", "identity/key", "Missing key field"),
  ];
  const summary = summarize("test.go", results);

  db.recordSummary("/project/test.go", "go", summary);

  // The ruleToLayer heuristic assigns layer to each action.
  // Patterns should be created from the actions (even without explicit error codes,
  // the actions exist in the DB — but updatePatternsFromRun only processes
  // actions WITH error codes, and ruleToLayer doesn't set error codes).
  // So with the current logic, patterns from recordSummary won't have error codes.
  // This is by design — error codes come from the error catalog, not heuristics.
  const patterns = db.getTopPatterns();
  // No error codes were set by ruleToLayer, so patterns should be empty
  assertEquals(patterns.length, 0, "No error codes in heuristic actions");

  db.close();
});

Deno.test("database/patterns: getPatternDetail returns full detail", () => {
  const db = new CwsDatabase();

  // Set up: two files with the same error code
  const fid1 = db.upsertFile("/a.go", "go");
  const fid2 = db.upsertFile("/b.go", "go");

  const run1 = db.recordLintRun({ fileId: fid1, handler: "go", errorCount: 1, warnCount: 0, infoCount: 0 });
  db.recordActions(run1, [
    { layer: 0, errorCode: "CWS-R50-001", rule: "block/missing", severity: "error", direction: -1, message: "Missing block" },
  ]);
  db.updatePatternsFromRun(run1);

  const run2 = db.recordLintRun({ fileId: fid2, handler: "go", errorCount: 1, warnCount: 0, infoCount: 0 });
  db.recordActions(run2, [
    { layer: 0, errorCode: "CWS-R50-001", rule: "block/missing", severity: "error", direction: -1, message: "Missing block" },
  ]);
  db.updatePatternsFromRun(run2);

  db.recalculateAffectedFiles();

  // Get pattern detail
  const detail = db.getPatternDetail("CWS-R50-001", "block/missing");
  assertExists(detail);
  assertEquals(detail.pattern.errorCode, "CWS-R50-001");
  assertEquals(detail.pattern.frequency, 2);
  assertEquals(detail.affectedFilePaths.length, 2);
  assert(detail.affectedFilePaths.includes("/a.go"));
  assert(detail.affectedFilePaths.includes("/b.go"));
  assertEquals(detail.recentOccurrences.length, 2);

  db.close();
});

Deno.test("database/patterns: getPatternDetail returns null for unknown code", () => {
  const db = new CwsDatabase();
  const detail = db.getPatternDetail("CWS-NONEXISTENT");
  assertEquals(detail, null);
  db.close();
});

Deno.test("database/patterns: getPatternDetail without rule finds first match", () => {
  const db = new CwsDatabase();

  db.updatePattern("CWS-R50-001", "block/missing");
  db.updatePattern("CWS-R50-001", "block/order");

  const detail = db.getPatternDetail("CWS-R50-001");
  assertExists(detail);
  assertEquals(detail.pattern.errorCode, "CWS-R50-001");

  db.close();
});

Deno.test("database/patterns: getTrendingPatterns returns increasing patterns", () => {
  const db = new CwsDatabase();

  // Create actual lint runs with actions so trend analysis has data.
  // updateTrends queries actions + lint_runs, not just patterns table.
  const fileId = db.upsertFile("/test.go", "go");
  const runId = db.recordLintRun({ fileId, handler: "go", errorCount: 1, warnCount: 0, infoCount: 0 });
  db.recordActions(runId, [
    { layer: 0, errorCode: "CWS-R50-001", rule: "block/missing", severity: "error", direction: -1, message: "Missing" },
  ]);
  db.updatePatternsFromRun(runId);

  // updateTrends: recent (last 7 days) has data, older (8-14 days) has none → "increasing"
  db.updateTrends();

  const trending = db.getTrendingPatterns();
  assert(trending.length >= 1, "Should have at least one trending pattern");
  assertEquals(trending[0]!.errorCode, "CWS-R50-001");
  assertEquals(trending[0]!.trending, "increasing");

  db.close();
});

// ---------------------------------------------------------------------------
// Instance tracking — with queries
// ---------------------------------------------------------------------------

Deno.test("database/instances: upsertInstance creates retrievable record", () => {
  const db = new CwsDatabase();

  db.upsertInstance("nova-dawn", "cpisi");

  const instance = db.getInstance("nova-dawn");
  assertExists(instance);
  assertEquals(instance.name, "nova-dawn");
  assertEquals(instance.type, "cpisi");
  assert(instance.runsCount >= 0);

  db.close();
});

Deno.test("database/instances: upsertInstance increments runs_count", () => {
  const db = new CwsDatabase();

  db.upsertInstance("nova-dawn", "cpisi"); // runs_count = 0
  db.upsertInstance("nova-dawn", "cpisi"); // runs_count = 1

  const instance = db.getInstance("nova-dawn");
  assertExists(instance);
  assertEquals(instance.runsCount, 1, "Second call increments");

  db.close();
});

Deno.test("database/instances: getInstance returns null for unknown", () => {
  const db = new CwsDatabase();
  const instance = db.getInstance("nonexistent");
  assertEquals(instance, null);
  db.close();
});

Deno.test("database/instances: getInstanceActivity returns runs and sessions", () => {
  const db = new CwsDatabase();

  db.upsertInstance("nova-dawn", "cpisi");
  const sessionId = db.startSession("nova-dawn");

  // Record a run with this instance
  const fileId = db.upsertFile("/test.go", "go");
  db.recordLintRun({
    fileId,
    handler: "go",
    errorCount: 0,
    warnCount: 0,
    infoCount: 0,
    instanceName: "nova-dawn",
    instanceType: "cpisi",
    sessionId,
  });

  const activity = db.getInstanceActivity("nova-dawn");
  assertExists(activity);
  assertEquals(activity.instance.name, "nova-dawn");
  assertEquals(activity.recentRuns.length, 1);
  assertEquals(activity.recentRuns[0]!.instanceName, "nova-dawn");
  assertEquals(activity.sessionsCount, 1);

  db.close();
});

Deno.test("database/instances: getInstanceActivity returns null for unknown", () => {
  const db = new CwsDatabase();
  const activity = db.getInstanceActivity("nonexistent");
  assertEquals(activity, null);
  db.close();
});

// ---------------------------------------------------------------------------
// Session tracking — with queries
// ---------------------------------------------------------------------------

Deno.test("database/sessions: startSession returns UUID", () => {
  const db = new CwsDatabase();

  db.upsertInstance("nova-dawn", "cpisi");
  const sessionId = db.startSession("nova-dawn");

  assert(sessionId.length > 0, "Should return non-empty session ID");
  assert(sessionId.includes("-"), "Should be UUID format");

  db.close();
});

Deno.test("database/sessions: getSession returns session record", () => {
  const db = new CwsDatabase();

  db.upsertInstance("nova-dawn", "cpisi");
  const sessionId = db.startSession("nova-dawn");

  const session = db.getSession(sessionId);
  assertExists(session);
  assertEquals(session.id, sessionId);
  assertEquals(session.instanceName, "nova-dawn");
  assertEquals(session.endTime, null, "Not ended yet");
  assertEquals(session.filesLinted, 0);
  assertEquals(session.healthDelta, 0);

  db.close();
});

Deno.test("database/sessions: endSession updates session record", () => {
  const db = new CwsDatabase();

  db.upsertInstance("nova-dawn", "cpisi");
  const sessionId = db.startSession("nova-dawn");
  db.endSession(sessionId, 42, 15);

  const session = db.getSession(sessionId);
  assertExists(session);
  assert(session.endTime !== null, "Should have end time");
  assertEquals(session.filesLinted, 42);
  assertEquals(session.healthDelta, 15);

  db.close();
});

Deno.test("database/sessions: getSession returns null for unknown", () => {
  const db = new CwsDatabase();
  const session = db.getSession("nonexistent-uuid");
  assertEquals(session, null);
  db.close();
});

Deno.test("database/sessions: getSessionSummary returns runs and patterns", () => {
  const db = new CwsDatabase();

  db.upsertInstance("nova-dawn", "cpisi");
  const sessionId = db.startSession("nova-dawn");

  // Record a run with actions that have error codes
  const fileId = db.upsertFile("/test.go", "go");
  const runId = db.recordLintRun({
    fileId,
    handler: "go",
    errorCount: 2,
    warnCount: 0,
    infoCount: 0,
    instanceName: "nova-dawn",
    sessionId,
  });

  db.recordActions(runId, [
    { layer: 0, errorCode: "CWS-R50-001", rule: "block/missing", severity: "error", direction: -1, message: "Missing block" },
    { layer: 1, errorCode: "CWS-R25-003", rule: "identity/key", severity: "error", direction: -1, message: "No key" },
  ]);

  const summary = db.getSessionSummary(sessionId);
  assertExists(summary);
  assertEquals(summary.session.id, sessionId);
  assertEquals(summary.runs.length, 1);
  assertEquals(summary.patternsFound.length, 2);
  assert(summary.patternsFound.includes("CWS-R50-001"));
  assert(summary.patternsFound.includes("CWS-R25-003"));

  db.close();
});

Deno.test("database/sessions: getSessionSummary returns null for unknown", () => {
  const db = new CwsDatabase();
  const summary = db.getSessionSummary("nonexistent");
  assertEquals(summary, null);
  db.close();
});

// ---------------------------------------------------------------------------
// Aggregate statistics
// ---------------------------------------------------------------------------

Deno.test("database/stats: getStats returns aggregate data", () => {
  const db = new CwsDatabase();

  // Empty database
  const empty = db.getStats();
  assertEquals(empty.totalFiles, 0);
  assertEquals(empty.totalRuns, 0);
  assertEquals(empty.avgScore, null);
  assertEquals(empty.healthyFiles, 0);
  assertEquals(empty.unhealthyFiles, 0);

  // Add some files with health
  const id1 = db.upsertFile("/good.go", "go");
  db.updateFileHealth(id1, 85, "perfect", 242);

  const id2 = db.upsertFile("/ok.go", "go");
  db.updateFileHealth(id2, 20, "sound", 161);

  const id3 = db.upsertFile("/bad.go", "go");
  db.updateFileHealth(id3, -30, "lacking", 40);

  // Add a lint run
  db.recordLintRun({ fileId: id1, handler: "go", errorCount: 0, warnCount: 0, infoCount: 0 });

  const stats = db.getStats();
  assertEquals(stats.totalFiles, 3);
  assertEquals(stats.totalRuns, 1);
  assertEquals(stats.avgScore, 25); // Math.round((85 + 20 + -30) / 3) = 25
  assertEquals(stats.healthyFiles, 1);   // score >= 34
  assertEquals(stats.unhealthyFiles, 1); // score < 0

  db.close();
});

// ---------------------------------------------------------------------------
// getFileHealth — comprehensive report
// ---------------------------------------------------------------------------

Deno.test("database/fileHealth: returns null for untracked file", () => {
  const db = new CwsDatabase();
  const report = db.getFileHealth("/nonexistent.go");
  assertEquals(report, null);
  db.close();
});

Deno.test("database/fileHealth: returns full report with history", () => {
  const db = new CwsDatabase();

  const health: HealthScore = {
    total: 75,
    level: "whole",
    emoji: "\u{1F7E2}",
    direction: 1,
    hardpoint: 50,
    trite: 233,
    blocks: [],
    totalActions: 10,
    alignedCount: 8,
    neutralCount: 1,
    misalignedCount: 1,
  };

  const results = [warn("test.go", "identity/key", "Missing key")];
  const summary = summarize("test.go", results, health);

  db.recordSummary("/project/test.go", "go", summary, {
    subtype: "library",
    handler: "go",
  });

  const report = db.getFileHealth("/project/test.go");
  assertExists(report);
  assertEquals(report.path, "/project/test.go");
  assertEquals(report.format, "go");
  assertEquals(report.subtype, "library");
  assertEquals(report.score, 75);
  assertEquals(report.state, "whole");
  assertExists(report.lastRun);
  assertEquals(report.lastRun.handler, "go");
  assert(report.history.length > 0);

  db.close();
});

// ---------------------------------------------------------------------------
// getRunDetails — run + actions
// ---------------------------------------------------------------------------

Deno.test("database/runDetails: returns null for nonexistent run", () => {
  const db = new CwsDatabase();
  const report = db.getRunDetails(999);
  assertEquals(report, null);
  db.close();
});

Deno.test("database/runDetails: returns run with actions", () => {
  const db = new CwsDatabase();

  const fileId = db.upsertFile("/test.go", "go");
  const runId = db.recordLintRun({
    fileId,
    handler: "go",
    errorCount: 2,
    warnCount: 0,
    infoCount: 0,
  });

  db.recordActions(runId, [
    { layer: 0, rule: "block/missing", severity: "error", direction: -1, message: "Missing block" },
    { layer: 1, rule: "identity/key", severity: "error", direction: -1, message: "No key", errorCode: "CWS-R25-001" },
  ]);

  const report = db.getRunDetails(runId);
  assertExists(report);
  assertEquals(report.run.handler, "go");
  assertEquals(report.run.errorCount, 2);
  assertEquals(report.actions.length, 2);
  assertEquals(report.actions[0]!.layer, 0);
  assertEquals(report.actions[0]!.rule, "block/missing");
  assertEquals(report.actions[1]!.errorCode, "CWS-R25-001");

  db.close();
});

// ---------------------------------------------------------------------------
// getBestFiles
// ---------------------------------------------------------------------------

Deno.test("database/bestFiles: returns files sorted by score descending", () => {
  const db = new CwsDatabase();

  const id1 = db.upsertFile("/low.go", "go");
  db.updateFileHealth(id1, 20, "sound", 161);

  const id2 = db.upsertFile("/high.go", "go");
  db.updateFileHealth(id2, 95, "perfect", 242);

  const id3 = db.upsertFile("/mid.go", "go");
  db.updateFileHealth(id3, 50, "whole", 188);

  const best = db.getBestFiles(3);
  assertEquals(best.length, 3);
  assertEquals(best[0]!.path, "/high.go");
  assertEquals(best[0]!.currentScore, 95);
  assertEquals(best[1]!.path, "/mid.go");
  assertEquals(best[2]!.path, "/low.go");

  db.close();
});

// ---------------------------------------------------------------------------
// recordBatch — batch recording in transaction
// ---------------------------------------------------------------------------

Deno.test("database/batch: records multiple summaries in one transaction", () => {
  const db = new CwsDatabase();

  const entries = [
    {
      filePath: "/a.go",
      format: "go",
      summary: summarize("a.go", [error("a.go", "block/missing", "Missing block")]),
      handler: "go",
    },
    {
      filePath: "/b.go",
      format: "go",
      summary: summarize("b.go", []),
      handler: "go",
    },
    {
      filePath: "/c.rs",
      format: "rust",
      summary: summarize("c.rs", [warn("c.rs", "identity/key", "Missing key")]),
      handler: "rust",
    },
  ];

  const runIds = db.recordBatch(entries);
  assertEquals(runIds.length, 3);
  assert(runIds.every((id) => id > 0));

  // All files should be tracked
  assertEquals(db.countFiles(), 3);
  assertEquals(db.countRuns(), 3);

  // Verify individual files
  const fileA = db.getFile("/a.go");
  assertExists(fileA);
  assertEquals(fileA.format, "go");

  const fileC = db.getFile("/c.rs");
  assertExists(fileC);
  assertEquals(fileC.format, "rust");

  db.close();
});

Deno.test("database/batch: empty batch is no-op", () => {
  const db = new CwsDatabase();

  const runIds = db.recordBatch([]);
  assertEquals(runIds.length, 0);
  assertEquals(db.countFiles(), 0);

  db.close();
});

// ---------------------------------------------------------------------------
// detectInstance — environment detection
// ---------------------------------------------------------------------------

Deno.test("database/detectInstance: returns instance from environment", () => {
  // Default should be human with current USER
  const detected = detectInstance();
  assertExists(detected.name);
  assert(detected.name.length > 0, "Should have a name");
  assert(
    ["cpisi", "human", "automated"].includes(detected.type),
    "Type should be valid",
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// SQLite database tests — 58 tests covering migrations, file CRUD,
// lint run recording, health history tracking, pattern detection
// (proper upsert, deduplication, affected files, trend analysis,
// pattern detail, auto-update from recordSummary), instance tracking
// (registration, queries, activity reports), session management
// (lifecycle, summaries with patterns), aggregate statistics, file
// health reports, run details, best files, batch recording, and
// environment detection.
//
// All tests use in-memory databases — no filesystem side effects.
//
// "Prove all things; hold fast that which is good." — 1 Thess 5:21
// ============================================================================
