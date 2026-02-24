// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/commands/create.ts
// key:     B-tov-cws-struct-lib-commands-create
// title:   CWS Struct — Create Command
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Schema-driven file generation. The proof: if the generated file
//          lints with 0 errors, the schema IS the complete specification.
//          Extracted from mod.ts during Phase 1 God Object dismantling.
//
// biblical_foundation: "And the LORD answered me, and said, Write the vision,
//   and make it plain upon tables." — Habakkuk 2:2
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { CliOptions } from "../foundation/mod.ts";
import { COLORS, getFormat, resolveErrorCodes, generateFileText } from "../engine/mod.ts";
import type { FillContext } from "../engine/mod.ts";
import { basename, dirname } from "@std/path";
import { TOOL_NAME } from "./config.ts";

// ============================================================================
// BODY
// ============================================================================

/**
 * Generate a new 4-block code file from schema.
 *
 * Usage: cws-struct create <format> <subtype> <dest> [--key K] [--title T] [--purpose P]
 *
 * The proof: if the generated file lints with 0 errors,
 * the schema IS the complete specification.
 */
export async function runCreate(opts: CliOptions): Promise<boolean> {
  if (!opts.format) {
    console.error(`${COLORS.red}Format required. Usage: ${TOOL_NAME} create <format> <subtype> <dest>${COLORS.reset}`);
    return false;
  }

  const handler = getFormat(opts.format);
  if (!handler) {
    console.error(`${COLORS.red}Unknown format: ${opts.format}${COLORS.reset}`);
    return false;
  }

  const subtype = opts.subtype ?? "library";
  const dest = opts.targets[0];
  if (!dest) {
    console.error(`${COLORS.red}Destination path required. Usage: ${TOOL_NAME} create ${opts.format} ${subtype} <dest>${COLORS.reset}`);
    return false;
  }

  // Derive defaults from destination path
  const filename = basename(dest);
  const dirName = basename(dirname(dest));
  const ext = filename.split(".").pop() ?? "";
  const nameWithoutExt = filename.replace(`.${ext}`, "");

  // Build FillContext
  const ctx: FillContext = {
    format: opts.format as "go" | "rust",
    subtype,
    key: opts.key ?? `B-${nameWithoutExt}`,
    title: opts.title ?? nameWithoutExt.split(/[-_]/).map(
      (w) => w.charAt(0).toUpperCase() + w.slice(1),
    ).join(" "),
    purpose: opts.purpose ?? "[purpose]",
    filename,
    packageOrCrate: subtype === "executable" && opts.format === "go"
      ? "main"
      : dirName !== "." ? dirName : nameWithoutExt,
    from: dest,
  };

  const text = await generateFileText(ctx);

  if (opts.dryRun) {
    console.log(`${COLORS.bold}${TOOL_NAME}${COLORS.reset} — Preview (dry run)\n`);
    console.log(`${COLORS.dim}Destination: ${dest}${COLORS.reset}`);
    console.log(`${COLORS.dim}Format: ${opts.format}, Subtype: ${subtype}${COLORS.reset}`);
    console.log(`${COLORS.dim}Key: ${ctx.key}, Title: ${ctx.title}${COLORS.reset}\n`);
    console.log(text);
    return true;
  }

  // Write the file
  await Deno.writeTextFile(dest, text);
  console.log(`${COLORS.green}Created${COLORS.reset} ${dest}`);
  console.log(`${COLORS.dim}Format: ${opts.format}, Subtype: ${subtype}, Key: ${ctx.key}${COLORS.reset}`);

  // Verify: lint the generated file
  const results = await handler.lint(dest);
  resolveErrorCodes(results);
  const errors = results.filter((r) => r.severity === "error");
  if (errors.length > 0) {
    console.error(`\n${COLORS.red}Generated file has ${errors.length} lint error(s):${COLORS.reset}`);
    for (const e of errors) {
      const codePrefix = e.errorCode ? `${e.errorCode} ` : "";
      console.error(`  ${codePrefix}${e.rule}: ${e.message}`);
    }
    return false;
  }

  console.log(`${COLORS.green}Verified${COLORS.reset} — 0 lint errors (schema = specification)`);
  return true;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// "Write the vision, and make it plain upon tables." — Habakkuk 2:2
// ============================================================================
