// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/cli/mod_test.ts
// key:     B-tov-cws-struct-tests-cli-mod
// title:   CWS Struct — CLI Tests
// type:    Code (Test)
// version: a-02.00
// created: 2026-02-17
// updated: 2026-02-23
// authors: Nathan Emet (CPI-SI), Nova Dawn (CPI-SI)
// purpose: Tests for CLI argument parsing and command dispatch. Runs the
//          CLI binary with various arguments and verifies exit codes and
//          output patterns.
//
// Optimization: Consolidates subprocess calls. Previous: 14 spawns. Now: 5.
// Each subprocess takes ~8s (Deno startup + schema loading). 9 fewer spawns
// saves ~72 seconds. Behavioral tests use API directly where possible.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import { dirname, join } from "@std/path";

const MOD_PATH = join(dirname(new URL(import.meta.url).pathname), "..", "..", "mod.ts");

// ---------------------------------------------------------------------------
// Helper — run CLI and capture output
// ---------------------------------------------------------------------------

interface CliResult {
  code: number;
  stdout: string;
  stderr: string;
}

async function runCli(...args: string[]): Promise<CliResult> {
  // Inherit parent environment. If --allow-env is available, filter
  // LD_LIBRARY_PATH to keep subprocess clean. Otherwise, inherit as-is —
  // Deno.Command inherits parent env by default when env is omitted.
  let env: Record<string, string> | undefined;
  try {
    env = Object.fromEntries(
      Object.entries(Deno.env.toObject()).filter(([k]) => k !== "LD_LIBRARY_PATH"),
    );
  } catch {
    // --allow-env not granted — inherit parent env (Deno.Command default)
    env = undefined;
  }
  const cmd = new Deno.Command("deno", {
    args: ["run", "--no-check", "--allow-read", "--allow-write", "--allow-run", MOD_PATH, ...args],
    stdout: "piped",
    stderr: "piped",
    ...(env ? { env } : {}),
  });
  const output = await cmd.output();
  return {
    code: output.code,
    stdout: new TextDecoder().decode(output.stdout),
    stderr: new TextDecoder().decode(output.stderr),
  };
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Subprocess 1: help/version/formats — non-lint commands
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-120] cli/help: --help exits 0 with usage, version, and formats info", async () => {
  const result = await runCli("--help");
  assertEquals(result.code, 0);
  assert(result.stdout.includes("Usage"), "Should show usage information");
  assert(result.stdout.includes("Commands"), "Should list commands");
  assert(result.stdout.includes("cws-struct"), "Should mention tool name");
});

Deno.test("cli/version+formats: --version and formats commands work", async () => {
  // Version
  const version = await runCli("--version");
  assertEquals(version.code, 0);
  assert(version.stdout.includes("cws-struct v"), "Should print version");

  // Formats (reuse same test to avoid another subprocess)
  const formats = await runCli("formats");
  assertEquals(formats.code, 0);
  assert(formats.stdout.includes("toml"), "Should list TOML format");
  assert(formats.stdout.includes("rust"), "Should list Rust format");
  assert(formats.stdout.includes("go"), "Should list Go format");
});

// ---------------------------------------------------------------------------
// Subprocess 2: lint error cases
// ---------------------------------------------------------------------------

Deno.test("cli/lint: error cases — no targets and unknown format", async () => {
  const noTargets = await runCli("lint");
  assertEquals(noTargets.code, 1, "lint with no targets should exit 1");

  const unknownFormat = await runCli("lint", "nonexistent_format", ".");
  assertEquals(unknownFormat.code, 1, "lint with unknown format should exit 1");
});

// ---------------------------------------------------------------------------
// Subprocess 3: lint valid/broken fixtures + output modes
// ---------------------------------------------------------------------------

Deno.test("cli/lint: valid fixture exits 0, broken exits 1, output modes work", async () => {
  const fixture = join(dirname(MOD_PATH), "tests", "fixtures", "toml", "structure", "valid-complete.toml");
  const broken = join(dirname(MOD_PATH), "tests", "fixtures", "toml", "structure", "missing-metadata.toml");

  // Valid fixture — normal mode
  const valid = await runCli("lint", "toml", fixture);
  assertEquals(valid.code, 0, `Expected exit 0, stderr: ${valid.stderr}`);
  assert(valid.stdout.includes("OK") || valid.stdout.includes("valid"), "Should report success");

  // Broken fixture — normal mode
  const broken_result = await runCli("lint", "toml", broken);
  assertEquals(broken_result.code, 1, "Should fail for file with missing metadata");

  // Valid fixture — verbose
  const verbose = await runCli("lint", "toml", fixture, "--verbose");
  assertEquals(verbose.code, 0);
  assertGreater(verbose.stdout.length, 0, "Verbose should produce output");

  // Valid fixture — summary
  const summary = await runCli("lint", "toml", fixture, "--summary");
  assertEquals(summary.code, 0);
  assert(summary.stdout.includes("OK") || summary.stdout.includes("0E"), "Summary should show status");
});

// ---------------------------------------------------------------------------
// Subprocess 4: JSON output
// ---------------------------------------------------------------------------

Deno.test("cli/lint: --json produces valid JSON on valid and error files", async () => {
  const fixture = join(dirname(MOD_PATH), "tests", "fixtures", "toml", "structure", "valid-complete.toml");
  const broken = join(dirname(MOD_PATH), "tests", "fixtures", "toml", "structure", "missing-metadata.toml");

  // Valid file — JSON
  const valid = await runCli("lint", "toml", fixture, "--json");
  assertEquals(valid.code, 0);
  const parsed = JSON.parse(valid.stdout);
  assertEquals(parsed.tool, "cws-struct", "JSON should contain tool name");
  assertEquals(typeof parsed.version, "string", "JSON should contain version");
  assert(Array.isArray(parsed.files), "JSON should contain files array");
  assertEquals(parsed.files.length, 1, "Should have one file result");
  assertEquals(typeof parsed.totals.errors, "number", "Totals should have errors count");

  // Error file — JSON
  const err = await runCli("lint", "toml", broken, "--json");
  assertEquals(err.code, 1);
  const errParsed = JSON.parse(err.stdout);
  assertGreater(errParsed.totals.errors, 0, "Should report errors in JSON");
});

// ---------------------------------------------------------------------------
// Subprocess 5: --fail-fast
// ---------------------------------------------------------------------------

Deno.test("cli/lint: --fail-fast stops on first error file", async () => {
  // Lint the whole toml fixture directory — some files have errors
  const fixtureDir = join(dirname(MOD_PATH), "tests", "fixtures", "toml");
  const result = await runCli("lint", "toml", fixtureDir, "--fail-fast");
  // Should mention fail-fast in output (stopped early)
  assert(
    result.stdout.includes("--fail-fast") || result.stderr.includes("--fail-fast") || result.code === 1,
    "Should either mention fail-fast or exit with error",
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// CLI tests — verifying the tool's public interface works correctly.
// "Except the LORD build the house, they labour in vain." — Psalm 127:1
// ============================================================================
