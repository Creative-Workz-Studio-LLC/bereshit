// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/git.ts
// key:     B-tov-cws-struct-lib-engine-git
// title:   CWS Struct — Git Integration
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Git diff-awareness infrastructure for any command that needs to
//          operate on changed files. Not lint-specific — watch, transform,
//          and future commands can all use diff-aware file detection.
//
// biblical_foundation: "To every thing there is a season, and a time to
//   every purpose under the heaven." — Ecclesiastes 3:1
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { COLORS } from "./output.ts";

// ============================================================================
// BODY
// ============================================================================

/**
 * Get files changed according to git.
 *
 * Modes:
 * - staged: `git diff --name-only --cached`
 * - since <ref>: `git diff --name-only <ref>`
 * - default: `git diff --name-only HEAD` (uncommitted changes)
 *
 * Returns absolute paths. Filters to existing files only.
 * Any command can use this — lint, watch, transform, future commands.
 */
export async function getChangedFiles(opts: {
  staged?: boolean;
  since?: string;
  targets: string[];
}): Promise<string[]> {
  const args = ["diff", "--name-only"];

  if (opts.staged) {
    args.push("--cached");
  } else if (opts.since) {
    args.push(opts.since);
  } else {
    args.push("HEAD");
  }

  // Add target directories as pathspec (after --)
  if (opts.targets.length > 0) {
    args.push("--", ...opts.targets);
  }

  try {
    const cmd = new Deno.Command("git", { args, stdout: "piped", stderr: "piped" });
    const { code, stdout, stderr } = await cmd.output();

    if (code !== 0) {
      const errText = new TextDecoder().decode(stderr).trim();
      console.error(
        `${COLORS.yellow}Warning: git diff failed (${errText}). Falling back to full run.${COLORS.reset}`,
      );
      return [];
    }

    const output = new TextDecoder().decode(stdout).trim();
    if (!output) return [];

    const cwd = Deno.cwd();
    const paths: string[] = [];

    for (const line of output.split("\n")) {
      const trimmed = line.trim();
      if (!trimmed) continue;

      // git outputs paths relative to repo root — resolve to absolute
      const abs = trimmed.startsWith("/") ? trimmed : `${cwd}/${trimmed}`;
      try {
        await Deno.stat(abs);
        paths.push(abs);
      } catch {
        // File was deleted — skip
      }
    }

    return paths;
  } catch {
    console.error(
      `${COLORS.yellow}Warning: Could not run git. Falling back to full run.${COLORS.reset}`,
    );
    return [];
  }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Git integration — diff-aware file detection for the whole tool.
// "To every thing there is a season." — Ecclesiastes 3:1
// ============================================================================
