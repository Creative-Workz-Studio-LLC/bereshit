// R-level: R10 (engine layer — structured health logging)
// Migrated from: tests/engine/logging_test.ts
// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R10/engine/logging_test.ts
// key:     B-tov-cws-struct-tests-engine-logging
// title:   CWS Struct — Structured Health Logging Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// updated: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for health logging — TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL
//          format matching log.toml spec.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals } from "jsr:@std/assert";
import { formatLogEntry, formatLogEntries } from "@lib/engine/present/logging.ts";
import type { AtomicAction } from "@lib/foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// formatLogEntry — single action → pipe-delimited log line
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-086] logging/formatLogEntry: success action → success|+N", () => {
  const action: AtomicAction = {
    check: "block/METADATA",
    container: "blocks",
    block: "structural",
    direction: 1,
    impact: "warn",
  };
  const entry = formatLogEntry(action, "go", "2026-02-22T00:00:00Z");
  assertEquals(entry, "2026-02-22T00:00:00Z|success|+1|go/block/METADATA|blocks/structural");
});

Deno.test("logging/formatLogEntry: failure action → failure|-N", () => {
  const action: AtomicAction = {
    check: "identity/Pragma/I1.key",
    container: "identity",
    block: "metadata",
    direction: -1,
    impact: "error",
    reason: "Missing I1.key field",
  };
  const entry = formatLogEntry(action, "go", "2026-02-22T00:00:00Z");
  assertEquals(entry, "2026-02-22T00:00:00Z|failure|-2|go/identity/Pragma/I1.key|identity/metadata|Missing I1.key field");
});

Deno.test("logging/formatLogEntry: neutral action → neutral|+0", () => {
  const action: AtomicAction = {
    check: "cascade/setup",
    container: "cascade",
    block: "setup",
    direction: 0,
    impact: "warn",
  };
  const entry = formatLogEntry(action, "rust", "2026-02-22T00:00:00Z");
  assertEquals(entry, "2026-02-22T00:00:00Z|neutral|+0|rust/cascade/setup|cascade/setup");
});

Deno.test("logging/formatLogEntry: info impact rounds to 0 for failure", () => {
  const action: AtomicAction = {
    check: "style/comment",
    container: "style",
    block: "metadata",
    direction: -1,
    impact: "info",
  };
  const entry = formatLogEntry(action, "go", "2026-02-22T00:00:00Z");
  // info weight = 0.25, rounded to 0 → -0 → displayed as +0
  assertEquals(entry, "2026-02-22T00:00:00Z|failure|+0|go/style/comment|style/metadata");
});

Deno.test("logging/formatLogEntry: reason appended after pipe", () => {
  const action: AtomicAction = {
    check: "body/subtype",
    container: "content",
    block: "body",
    direction: -1,
    impact: "warn",
    reason: "Missing canonical subsection",
  };
  const entry = formatLogEntry(action, "go", "2026-02-22T00:00:00Z");
  assertEquals(
    entry.endsWith("|content/body|Missing canonical subsection"),
    true,
  );
});

// ---------------------------------------------------------------------------
// formatLogEntries — all actions → array with file header
// ---------------------------------------------------------------------------

Deno.test("logging/formatLogEntries: includes file header comment", () => {
  const actions: AtomicAction[] = [
    { check: "block/METADATA", container: "blocks", block: "structural", direction: 1, impact: "warn" },
  ];
  const lines = formatLogEntries(actions, "go", "test/file.go");
  assertEquals(lines[0], "# test/file.go");
  assertEquals(lines.length, 2); // header + 1 action
});

Deno.test("logging/formatLogEntries: empty actions → header only", () => {
  const lines = formatLogEntries([], "go", "test/file.go");
  assertEquals(lines.length, 1);
  assertEquals(lines[0], "# test/file.go");
});

Deno.test("logging/formatLogEntries: multiple actions → one line each", () => {
  const actions: AtomicAction[] = [
    { check: "a", container: "c", block: "b", direction: 1, impact: "warn" },
    { check: "b", container: "c", block: "b", direction: -1, impact: "error" },
    { check: "c", container: "c", block: "b", direction: 0, impact: "info" },
  ];
  const lines = formatLogEntries(actions, "go", "test.go");
  assertEquals(lines.length, 4); // header + 3 actions
  // Each action line starts with timestamp
  for (let i = 1; i < lines.length; i++) {
    assertEquals(lines[i]!.includes("|"), true);
  }
});

// ---------------------------------------------------------------------------
// Format matches log.toml spec: TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL
// ---------------------------------------------------------------------------

Deno.test("logging/format: 5 pipe-delimited fields", () => {
  const action: AtomicAction = {
    check: "test/check",
    container: "test",
    block: "block",
    direction: 1,
    impact: "warn",
  };
  const entry = formatLogEntry(action, "go", "2026-02-22T00:00:00Z");
  const fields = entry.split("|");
  assertEquals(fields.length, 5);
  assertEquals(fields[0], "2026-02-22T00:00:00Z"); // TIMESTAMP
  assertEquals(fields[1], "success");               // ACTION
  assertEquals(fields[2], "+1");                     // DELTA
  assertEquals(fields[3], "go/test/check");          // SOURCE
  assertEquals(fields[4], "test/block");             // DETAIL
});

Deno.test("logging/format: failure with reason has 6 fields (detail pipe-splits)", () => {
  const action: AtomicAction = {
    check: "test/check",
    container: "test",
    block: "block",
    direction: -1,
    impact: "error",
    reason: "Missing field",
  };
  const entry = formatLogEntry(action, "go", "2026-02-22T00:00:00Z");
  const fields = entry.split("|");
  assertEquals(fields.length, 6); // reason adds one more pipe field
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Structured health logging tests — TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL.
// Each AtomicAction maps to exactly one log entry.
// "And the books were opened." — Revelation 20:12
// ============================================================================
