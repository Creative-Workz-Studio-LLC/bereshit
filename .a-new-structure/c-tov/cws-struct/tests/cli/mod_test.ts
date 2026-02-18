// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/cli/mod_test.ts
// key:     B-tov-cws-struct-tests-cli-mod
// title:   CWS Struct — CLI Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-17
// authors: Nathan Emet (CPI-SI)
// purpose: Tests for CLI argument parsing and command dispatch. Runs the
//          CLI binary with various arguments and verifies exit codes and
//          output patterns.
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
// help command
// ---------------------------------------------------------------------------

Deno.test("cli: --help exits 0 with usage info", async () => {
  const result = await runCli("--help");
  assertEquals(result.code, 0);
  assert(result.stdout.includes("Usage"), "Should show usage information");
  assert(result.stdout.includes("Operations"), "Should list operations");
});

Deno.test("cli: help exits 0", async () => {
  const result = await runCli("help");
  assertEquals(result.code, 0);
  assert(result.stdout.includes("cws-struct"), "Should mention tool name");
});

Deno.test("cli: no args shows help", async () => {
  const result = await runCli();
  assertEquals(result.code, 0);
  assert(result.stdout.includes("Usage"), "No args should show help");
});

// ---------------------------------------------------------------------------
// version command
// ---------------------------------------------------------------------------

Deno.test("cli: --version exits 0 with version string", async () => {
  const result = await runCli("--version");
  assertEquals(result.code, 0);
  assert(result.stdout.includes("cws-struct v"), "Should print version");
});

// ---------------------------------------------------------------------------
// formats command
// ---------------------------------------------------------------------------

Deno.test("cli: formats exits 0 and lists registered formats", async () => {
  const result = await runCli("formats");
  assertEquals(result.code, 0);
  assert(result.stdout.includes("toml"), "Should list TOML format");
  assert(result.stdout.includes("rust"), "Should list Rust format");
  assert(result.stdout.includes("go"), "Should list Go format");
});

// ---------------------------------------------------------------------------
// lint command — error cases
// ---------------------------------------------------------------------------

Deno.test("cli: lint with no targets exits 1", async () => {
  const result = await runCli("lint");
  assertEquals(result.code, 1);
});

Deno.test("cli: lint unknown format exits 1", async () => {
  const result = await runCli("lint", "nonexistent_format", ".");
  assertEquals(result.code, 1);
});

// ---------------------------------------------------------------------------
// lint command — valid invocation
// ---------------------------------------------------------------------------

Deno.test("cli: lint toml on valid fixture exits 0", async () => {
  const fixture = join(dirname(MOD_PATH), "tests", "fixtures", "toml", "valid-complete.toml");
  const result = await runCli("lint", "toml", fixture);
  assertEquals(result.code, 0, `Expected exit 0, stderr: ${result.stderr}`);
  assert(result.stdout.includes("OK") || result.stdout.includes("valid"), "Should report success");
});

Deno.test("cli: lint toml on broken fixture exits 1", async () => {
  const fixture = join(dirname(MOD_PATH), "tests", "fixtures", "toml", "missing-metadata.toml");
  const result = await runCli("lint", "toml", fixture);
  assertEquals(result.code, 1, "Should fail for file with missing metadata");
});

// ---------------------------------------------------------------------------
// lint command — flags
// ---------------------------------------------------------------------------

Deno.test("cli: lint --verbose shows info-level results", async () => {
  const fixture = join(dirname(MOD_PATH), "tests", "fixtures", "toml", "valid-complete.toml");
  const result = await runCli("lint", "toml", fixture, "--verbose");
  assertEquals(result.code, 0);
  // Verbose mode should show more output than default
  assertGreater(result.stdout.length, 0, "Should produce output");
});

Deno.test("cli: lint --summary shows only summary", async () => {
  const fixture = join(dirname(MOD_PATH), "tests", "fixtures", "toml", "valid-complete.toml");
  const result = await runCli("lint", "toml", fixture, "--summary");
  assertEquals(result.code, 0);
  assert(result.stdout.includes("OK") || result.stdout.includes("0E"), "Summary should show status");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// CLI tests — verifying the tool's public interface works correctly.
// "Except the LORD build the house, they labour in vain." — Psalm 127:1
// ============================================================================
