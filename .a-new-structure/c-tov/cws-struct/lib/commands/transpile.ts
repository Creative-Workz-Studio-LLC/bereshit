// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/commands/transpile.ts
// key:     B-tov-cws-struct-lib-commands-transpile
// title:   CWS Struct — Transpile Command
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Three-cord format conversion — transpile structural skeletons
//          between code formats. Extracted from mod.ts during Phase 1.
//
// biblical_foundation: "A threefold cord is not quickly broken."
//   — Ecclesiastes 4:12
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { CliOptions } from "../foundation/mod.ts";
import type { CodeFormat } from "../foundation/mod.ts";
import { COLORS, transpileFile } from "../engine/mod.ts";
import { TOOL_NAME } from "./config.ts";

// ============================================================================
// BODY
// ============================================================================

/**
 * Transpile a source file from one format to another.
 *
 * Usage: cws-struct transpile <source-format> <target-format> <file> [--dry-run]
 *
 * The transpiler produces a structural skeleton in the target format:
 * correct 4-block layout, section headers, identity, function stubs.
 * Method bodies are TODO stubs — this proves architecture, not parsing.
 */
export async function runTranspile(opts: CliOptions): Promise<boolean> {
  // Parse: transpile <source-format> <target-format> <file>
  // opts.format has the first non-flag (source format)
  // opts.targets has the remaining non-flags
  const sourceFormat = opts.format as CodeFormat | undefined;
  if (!sourceFormat) {
    console.error(`${COLORS.red}Source format required. Usage: ${TOOL_NAME} transpile <source> <target> <file>${COLORS.reset}`);
    return false;
  }

  // Target format is first target, source file is second
  if (opts.targets.length < 2) {
    console.error(`${COLORS.red}Target format and source file required. Usage: ${TOOL_NAME} transpile ${sourceFormat} <target> <file>${COLORS.reset}`);
    return false;
  }

  const targetFormat = opts.targets[0] as CodeFormat;
  const sourceFile = opts.targets[1]!;

  // Verify the source file exists
  try {
    await Deno.stat(sourceFile);
  } catch {
    console.error(`${COLORS.red}Source file not found: ${sourceFile}${COLORS.reset}`);
    return false;
  }

  console.log(`${COLORS.bold}${TOOL_NAME}${COLORS.reset} — Transpile ${sourceFormat} → ${targetFormat}`);
  console.log(`${COLORS.dim}Source: ${sourceFile}${COLORS.reset}`);
  if (opts.dryRun) {
    console.log(`${COLORS.dim}Mode: dry run (preview only)${COLORS.reset}`);
  }
  console.log();

  const result = await transpileFile(sourceFile, sourceFormat, targetFormat, {
    dryRun: opts.dryRun,
  });

  // Print diagnostics
  for (const d of result.diagnostics) {
    const color = d.severity === "error" ? COLORS.red
      : d.severity === "warn" ? COLORS.yellow
      : COLORS.dim;
    const icon = d.severity === "error" ? "X"
      : d.severity === "warn" ? "!"
      : "i";
    const lineRef = d.sourceLine ? `:${d.sourceLine}` : "";
    console.log(`  ${color}${icon}${COLORS.reset} ${d.message}${lineRef}`);
  }

  if (!result.success) {
    console.error(`\n${COLORS.red}Transpilation failed.${COLORS.reset}`);
    return false;
  }

  // Print stats
  const s = result.stats;
  console.log(`\n${COLORS.bold}Stats:${COLORS.reset}`);
  console.log(`  Chunks extracted:  ${s.chunksExtracted}`);
  console.log(`  Items extracted:   ${s.itemsExtracted}`);
  console.log(`  Sections mapped:   ${s.sectionsMapped}`);
  console.log(`  Stubs generated:   ${s.stubsGenerated}`);
  console.log(`  Direct transfers:  ${s.directTransfers}`);
  console.log(`  Output lines:      ${result.lineCount}`);

  if (opts.dryRun) {
    console.log(`\n${COLORS.bold}Preview:${COLORS.reset}\n`);
    console.log(result.output);
  } else if (result.targetFile) {
    console.log(`\n${COLORS.green}Written${COLORS.reset} ${result.targetFile}`);
  }

  return true;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// "A threefold cord is not quickly broken." — Ecclesiastes 4:12
// ============================================================================
