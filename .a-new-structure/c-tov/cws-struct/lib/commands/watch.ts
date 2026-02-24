// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/commands/watch.ts
// key:     B-tov-cws-struct-lib-commands-watch
// title:   CWS Struct — Watch Command
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Watch directories for file changes and re-lint automatically.
//          Debounces rapid changes (300ms), shows compact delta output,
//          and optionally records to health database.
//
// biblical_foundation: "Watch ye, stand fast in the faith."
//   — 1 Corinthians 16:13
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { relative } from "@std/path";
import type { CliOptions, FormatHandler } from "../foundation/mod.ts";
import { summarize } from "../foundation/mod.ts";
import { trueToLevel, levelToEmoji } from "../foundation/mod.ts";
import {
  COLORS,
  resolveErrorCodes,
  getFormat,
  listFormatDetails,
} from "../engine/mod.ts";
import {
  TOOL_NAME,
  EXIT_TOOL_ERROR,
  getDbModule, openDatabase, recordToDatabase,
} from "./config.ts";

// ============================================================================
// BODY
// ============================================================================

/**
 * Watch directories for file changes and re-lint automatically.
 * Debounces rapid changes (300ms) and shows compact delta output.
 *
 * "Watch ye, stand fast in the faith." — 1 Corinthians 16:13
 */
export async function runWatch(opts: CliOptions): Promise<void> {
  if (opts.targets.length === 0) {
    console.error(
      `${COLORS.red}No targets specified. Provide directories to watch.${COLORS.reset}`,
    );
    Deno.exit(EXIT_TOOL_ERROR);
  }

  const db = await openDatabase(opts);
  const instanceName = db ? (await getDbModule()).detectInstance().name : "";

  // Build a set of valid extensions from the format filter or all registered formats
  const validExtensions = new Set<string>();
  if (opts.format) {
    const handler = getFormat(opts.format);
    if (!handler) {
      console.error(`${COLORS.red}Unknown format: ${opts.format}${COLORS.reset}`);
      Deno.exit(EXIT_TOOL_ERROR);
    }
    for (const ext of handler.extensions) validExtensions.add(ext);
  } else {
    for (const detail of listFormatDetails()) {
      for (const ext of detail.extensions) validExtensions.add(ext);
    }
  }

  // Track previous health scores for delta display
  const prevScores = new Map<string, number | undefined>();

  console.log(
    `${COLORS.bold}${TOOL_NAME}${COLORS.reset} watching ${opts.targets.join(", ")}` +
    (opts.format ? ` (${opts.format} only)` : "") +
    `\n${COLORS.dim}Press Ctrl+C to stop.${COLORS.reset}\n`,
  );

  // Debounce: collect changed paths, lint after 300ms quiet
  let debounceTimer: number | undefined;
  const pending = new Set<string>();

  const watcher = Deno.watchFs(opts.targets, { recursive: true });

  for await (const event of watcher) {
    if (event.kind !== "modify" && event.kind !== "create") continue;

    for (const path of event.paths) {
      // Filter by extension
      const ext = "." + path.split(".").pop();
      if (!validExtensions.has(ext)) continue;
      pending.add(path);
    }

    // Debounce — wait 300ms after last change
    if (debounceTimer !== undefined) clearTimeout(debounceTimer);
    debounceTimer = setTimeout(async () => {
      const files = [...pending];
      pending.clear();

      for (const file of files) {
        const relPath = relative(Deno.cwd(), file);
        const now = new Date();
        const timeStr = `${String(now.getHours()).padStart(2, "0")}:${String(now.getMinutes()).padStart(2, "0")}:${String(now.getSeconds()).padStart(2, "0")}`;

        try {
          // Detect format from extension
          const ext = "." + file.split(".").pop();
          let handler: FormatHandler | undefined;
          if (opts.format) {
            handler = getFormat(opts.format);
          } else {
            // Find handler by extension
            for (const detail of listFormatDetails()) {
              if (detail.extensions.includes(ext)) {
                handler = getFormat(detail.name);
                break;
              }
            }
          }

          if (!handler) continue;

          // Lint the single file
          const results = await handler.lint(file);
          resolveErrorCodes(results);
          const health = handler.computeHealth
            ? await handler.computeHealth(file, results)
            : undefined;
          const summary = summarize(relPath, results, health);

          // Build compact output line
          const prev = prevScores.get(file);
          const score = health?.total;
          const level = health ? trueToLevel(health.total) : undefined;
          const emoji = level ? levelToEmoji(level) : "";

          let delta = "";
          if (score !== undefined && prev !== undefined) {
            const diff = score - prev;
            if (diff > 0) delta = ` ${COLORS.green}(+${diff})${COLORS.reset}`;
            else if (diff < 0) delta = ` ${COLORS.red}(${diff})${COLORS.reset}`;
            else delta = ` (no change)`;
          }

          const scoreStr = score !== undefined
            ? `${score} ${level} ${emoji}`
            : `${summary.errors}E ${summary.warnings}W`;

          const statusColor = summary.errors > 0 ? COLORS.red : COLORS.green;
          console.log(
            `${COLORS.dim}[${timeStr}]${COLORS.reset} ${statusColor}${relPath}${COLORS.reset} → ${scoreStr}${delta}`,
          );

          prevScores.set(file, score);

          // Record to database if enabled
          if (db) {
            recordToDatabase(db, [summary], handler.name, instanceName);
          }
        } catch (e) {
          console.error(
            `${COLORS.dim}[${timeStr}]${COLORS.reset} ${COLORS.red}${relPath}: ${e instanceof Error ? e.message : String(e)}${COLORS.reset}`,
          );
        }
      }
    }, 300) as unknown as number;
  }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Watch mode — re-lint on file changes. Debounced, compact, delta-aware.
// "Watch ye, stand fast in the faith." — 1 Corinthians 16:13
// ============================================================================
