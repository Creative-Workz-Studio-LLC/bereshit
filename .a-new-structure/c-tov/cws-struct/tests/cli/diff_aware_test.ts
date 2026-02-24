// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/cli/diff_aware_test.ts
// key:     B-tov-cws-struct-tests-cli-diff-aware
// title:   CWS Struct — Diff-Aware Linting Tests
// type:    Code (Test)
// version: a-02.00
// created: 2026-02-22
// updated: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// purpose: Verify --changed, --staged, and --since CLI flags for diff-aware
//          linting. Tests verify the CLI parsing and behavior of the flags.
//          Watch mode is not tested here (requires interactive fs watcher).
//
// Optimization: Consolidates help flag checks into a single subprocess call.
// Previous: 8 subprocess spawns. Now: 2. (~4x faster)
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import {
  assertEquals,
  assertStringIncludes,
} from "jsr:@std/assert";

// ---------------------------------------------------------------------------
// Test helpers
// ---------------------------------------------------------------------------

const MOD_PATH = new URL("../../mod.ts", import.meta.url).pathname;

/** Run the CLI with given args, capture stdout + stderr. */
async function runCli(
  args: string[],
): Promise<{ code: number; stdout: string; stderr: string }> {
  const cmd = new Deno.Command("deno", {
    args: [
      "run",
      "--no-check",
      "--allow-read",
      "--allow-write",
      "--allow-run",
      "--allow-env",
      "--allow-ffi",
      "--allow-net",
      MOD_PATH,
      ...args,
    ],
    stdout: "piped",
    stderr: "piped",
  });

  const { code, stdout, stderr } = await cmd.output();
  return {
    code,
    stdout: new TextDecoder().decode(stdout),
    stderr: new TextDecoder().decode(stderr),
  };
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. CLI parsing — help shows new flags (SINGLE subprocess)
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-121] cli/help: shows --changed, --since, --staged flags and watch command", async () => {
  // Diff flags live in "help options" (or "help lint"), not main help.
  // Watch appears in main help as a command.
  const { stdout: mainHelp } = await runCli(["help"]);
  assertStringIncludes(mainHelp, "watch");

  const { stdout: optionsHelp } = await runCli(["help", "options"]);
  assertStringIncludes(optionsHelp, "--changed");
  assertStringIncludes(optionsHelp, "--since");
  assertStringIncludes(optionsHelp, "--staged");
});

// ---------------------------------------------------------------------------
// 2. Diff-aware lint — no changed files = clean exit (SINGLE subprocess)
// ---------------------------------------------------------------------------

Deno.test({
  name: "cli/lint --staged: exits cleanly when no files staged",
  async fn() {
    const { code } = await runCli([
      "lint", "--staged", "tests/fixtures/go/structure/",
    ]);

    assertEquals(code, 0);
  },
  sanitizeResources: false,
  sanitizeOps: false,
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Diff-aware linting tests — speed through precision.
// Only lint what changed. Time is a gift; don't waste it.
//
// "Redeeming the time, because the days are evil." — Ephesians 5:16
// ============================================================================
