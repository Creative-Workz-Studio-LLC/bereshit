// R-level: R10 — Format-level tests (single-format, single-concern)
// Migrated from: tests/api/health_query_test.ts
// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R10/api/health_query_test.ts
// key:     B-tov-cws-struct-tests-api-health-query
// title:   CWS Struct — Health Query + Session API Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify health query convenience functions and session lifecycle
//          hooks. Tests directoryHealth, sessionDelta, topFixes, healthMap,
//          startSession, endSession, and quickLint.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import {
  assertEquals,
  assertExists,
} from "jsr:@std/assert";

import {
  lintFile,
  directoryHealth,
  topFixes,
  healthMap,
  quickLint,
  startSession,
  endSession,
} from "@lib/api/mod.ts";

import type {
  DirectoryHealthSummary,
  FixRecommendation,
  HealthTreeNode,
  SessionStartReport,
  SessionEndReport,
} from "@lib/api/mod.ts";

// ---------------------------------------------------------------------------
// Test helpers
// ---------------------------------------------------------------------------

const FIXTURES = new URL("../../../fixtures/", import.meta.url).pathname;

function goFixture(name: string): string {
  return `${FIXTURES}go/${name}`;
}

/**
 * Create an in-memory database, lint some fixture files,
 * record them, and return the db path for querying.
 */
async function setupTestDb(): Promise<string> {
  const { CwsDatabase, detectInstance } = await import("@lib/data/database.ts");
  const db = new CwsDatabase(); // in-memory
  const dbPath = db.dbPath; // ":memory:"

  // We can't use :memory: across different connections...
  // Use a temp file instead
  db.close();

  const tmpDir = await Deno.makeTempDir();
  const tmpDb = `${tmpDir}/test-health.db`;
  const db2 = new CwsDatabase(undefined, tmpDb);

  // Register instance and start session
  db2.upsertInstance("test-instance", "cpisi");
  const sessionId = db2.startSession("test-instance");

  // Lint some Go fixture files and record them
  const files = [
    goFixture("structure/valid-library.go"),
    goFixture("structure/missing-block.go"),
  ];

  for (const file of files) {
    try {
      const report = await lintFile(file);
      db2.recordSummary(
        report.absolutePath,
        report.format,
        report.summary,
        {
          handler: report.format,
          instanceName: "test-instance",
          sessionId,
        },
      );
    } catch {
      // Skip files that can't be read
    }
  }

  db2.endSession(sessionId, files.length, 0);
  db2.close();

  return tmpDb;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. directoryHealth
// ---------------------------------------------------------------------------

Deno.test({
  name: "api/directoryHealth: returns summary for fixture directory",
  async fn() {
    const dbPath = await setupTestDb();
    try {
      const result: DirectoryHealthSummary = await directoryHealth(
        dbPath,
        FIXTURES + "go/",
      );

      assertExists(result);
      assertEquals(result.dir, FIXTURES + "go/");
      assertEquals(typeof result.fileCount, "number");
      assertEquals(result.fileCount > 0, true, "Should have tracked files");

      if (result.avgScore != null) {
        assertEquals(typeof result.avgScore, "number");
        assertExists(result.state);
      }

      assertExists(result.stateDistribution);
    } finally {
      await Deno.remove(dbPath).catch(() => {});
    }
  },
  sanitizeResources: false,
  sanitizeOps: false,
});

Deno.test({
  name: "api/directoryHealth: returns empty summary for untracked directory",
  async fn() {
    const dbPath = await setupTestDb();
    try {
      const result = await directoryHealth(dbPath, "/nonexistent/path/");

      assertEquals(result.fileCount, 0);
      assertEquals(result.avgScore, null);
      assertEquals(result.worstFile, null);
      assertEquals(result.bestFile, null);
    } finally {
      await Deno.remove(dbPath).catch(() => {});
    }
  },
  sanitizeResources: false,
  sanitizeOps: false,
});

// ---------------------------------------------------------------------------
// 2. topFixes
// ---------------------------------------------------------------------------

Deno.test({
  name: "api/topFixes: returns ranked fix recommendations",
  async fn() {
    const dbPath = await setupTestDb();
    try {
      const fixes: FixRecommendation[] = await topFixes(dbPath, 5);

      assertExists(fixes);
      assertEquals(Array.isArray(fixes), true);

      // Each fix should have the right shape
      for (const fix of fixes) {
        assertExists(fix.errorCode);
        assertExists(fix.rule);
        assertEquals(typeof fix.affectedFiles, "number");
        assertEquals(typeof fix.frequency, "number");
        assertExists(fix.trending);
      }
    } finally {
      await Deno.remove(dbPath).catch(() => {});
    }
  },
  sanitizeResources: false,
  sanitizeOps: false,
});

// ---------------------------------------------------------------------------
// 3. healthMap
// ---------------------------------------------------------------------------

Deno.test({
  name: "api/healthMap: returns tree structure",
  async fn() {
    const dbPath = await setupTestDb();
    try {
      const tree: HealthTreeNode = await healthMap(dbPath, FIXTURES + "go/");

      assertExists(tree);
      assertExists(tree.name);
      assertExists(tree.path);
      assertEquals(Array.isArray(tree.children), true);
    } finally {
      await Deno.remove(dbPath).catch(() => {});
    }
  },
  sanitizeResources: false,
  sanitizeOps: false,
});

// ---------------------------------------------------------------------------
// 4. quickLint
// ---------------------------------------------------------------------------

Deno.test({
  name: "api/quickLint: lints file and records to database",
  async fn() {
    const tmpDir = await Deno.makeTempDir();
    const dbPath = `${tmpDir}/quick-lint-test.db`;

    try {
      const report = await quickLint(
        goFixture("structure/valid-library.go"),
        dbPath,
        { instanceName: "test-quick" },
      );

      assertExists(report);
      assertEquals(report.format, "go");
      assertExists(report.summary);

      // Verify it was recorded
      const { CwsDatabase } = await import("@lib/data/database.ts");
      const db = new CwsDatabase(undefined, dbPath);
      try {
        const stats = db.getStats();
        assertEquals(stats.totalFiles > 0, true, "Should have recorded at least 1 file");
        assertEquals(stats.totalRuns > 0, true, "Should have recorded at least 1 run");
      } finally {
        db.close();
      }
    } finally {
      await Deno.remove(tmpDir, { recursive: true }).catch(() => {});
    }
  },
  sanitizeResources: false,
  sanitizeOps: false,
});

// ---------------------------------------------------------------------------
// 5. startSession + endSession
// ---------------------------------------------------------------------------

Deno.test({
  name: "api/startSession: creates session with explicit files",
  async fn() {
    const tmpDir = await Deno.makeTempDir();
    const dbPath = `${tmpDir}/session-test.db`;

    try {
      const result: SessionStartReport = await startSession(dbPath, {
        files: [goFixture("structure/valid-library.go")],
        instanceName: "test-session",
      });

      assertExists(result);
      assertExists(result.sessionId);
      assertEquals(result.instanceName, "test-session");
      assertEquals(result.changedFiles.length, 1);
      assertEquals(result.reports.length, 1);
      assertEquals(result.summary.total, 1);
    } finally {
      await Deno.remove(tmpDir, { recursive: true }).catch(() => {});
    }
  },
  sanitizeResources: false,
  sanitizeOps: false,
});

Deno.test({
  name: "api/endSession: closes session with summary",
  async fn() {
    const tmpDir = await Deno.makeTempDir();
    const dbPath = `${tmpDir}/session-end-test.db`;

    try {
      // Start session first
      const start = await startSession(dbPath, {
        files: [goFixture("structure/valid-library.go")],
        instanceName: "test-end-session",
      });

      // End it
      const result: SessionEndReport = await endSession(dbPath, start.sessionId);

      assertExists(result);
      assertEquals(result.sessionId, start.sessionId);
      assertEquals(typeof result.filesLinted, "number");
      assertEquals(typeof result.netDelta, "number");
      assertEquals(Array.isArray(result.newPatterns), true);
    } finally {
      await Deno.remove(tmpDir, { recursive: true }).catch(() => {});
    }
  },
  sanitizeResources: false,
  sanitizeOps: false,
});

Deno.test({
  name: "api/startSession: handles empty file list gracefully",
  async fn() {
    const tmpDir = await Deno.makeTempDir();
    const dbPath = `${tmpDir}/session-empty-test.db`;

    try {
      const result = await startSession(dbPath, {
        files: [],
        instanceName: "test-empty",
      });

      assertEquals(result.changedFiles.length, 0);
      assertEquals(result.reports.length, 0);
      assertEquals(result.summary.total, 0);
    } finally {
      await Deno.remove(tmpDir, { recursive: true }).catch(() => {});
    }
  },
  sanitizeResources: false,
  sanitizeOps: false,
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Health query + session lifecycle tests.
// The same engine, viewed through the lens of time and measurement.
//
// "Remember the former things of old." — Isaiah 46:9
// ============================================================================
