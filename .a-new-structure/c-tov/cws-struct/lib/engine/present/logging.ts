// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/logging.ts
// key:     B-tov-cws-struct-lib-engine-logging
// title:   CWS Struct — Structured Health Logging
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-21
// updated: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: Emit health observations as structured log entries using the
//          existing CPI-SI logging format from log.toml.
//
//          Format: TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL
//
//          Each AtomicAction maps to one log entry. The format is identical
//          to what Go/Rust will use — easy switch when it moves up.
//          "The books are opened" — Revelation 20:12.
//
//          NOT a new logging system. Uses the existing spec.
//          NOT read in normal operation. For debugging and auditability.
//
// biblical_foundation: "And the books were opened: and another book was
//   opened, which is the book of life: and the dead were judged out of
//   those things which were written in the books." — Revelation 20:12
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { AtomicAction, LintSummary } from "../../foundation/mod.ts";
import { IMPACT_WEIGHT } from "../../foundation/mod.ts";
import type { LintResult } from "../../foundation/mod.ts";
import { COLORS } from "./output.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Log entry formatting — AtomicAction → TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL
// ---------------------------------------------------------------------------
//
// Maps AtomicAction to the log.toml entry format:
//   direction +1 → ACTION "success", DELTA "+weight"
//   direction -1 → ACTION "failure", DELTA "-weight"
//   direction  0 → ACTION "neutral", DELTA "0"
//
// SOURCE = format/check (e.g., "go/identity/Pragma/I1.key")
// DETAIL = container/block + reason
//
// "And I saw the dead, small and great, stand before God; and the books
//  were opened." Every observation is recorded. The score is measured,
//  not invented.

/**
 * Map AtomicAction direction to log ACTION field.
 *
 * From log.toml [entries.action]:
 *   success = +1 (check passed, aligned)
 *   failure = -1 (check failed, misaligned)
 *   neutral = 0  (cascaded, can't assess)
 */
function actionName(direction: -1 | 0 | 1): string {
  if (direction > 0) return "success";
  if (direction < 0) return "failure";
  return "neutral";
}

/**
 * Compute DELTA for a log entry.
 *
 * From log.toml: success always +1 (narrow gate).
 * Failure = -(impact_weight), rounded to integer.
 * Neutral = 0.
 */
function computeDelta(action: AtomicAction): number {
  if (action.direction === 0) return 0;
  const weight = IMPACT_WEIGHT[action.impact ?? "warn"] ?? 1.0;
  return action.direction > 0
    ? Math.round(weight)
    : -Math.round(weight);
}

/**
 * Format a single AtomicAction as a log entry.
 *
 * Returns: "TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL"
 *
 * @param action - The atomic health observation.
 * @param format - The file format (e.g., "go", "rust", "toml").
 * @param timestamp - ISO8601 timestamp for the run.
 */
export function formatLogEntry(
  action: AtomicAction,
  format: string,
  timestamp: string,
): string {
  const act = actionName(action.direction);
  const delta = computeDelta(action);
  const deltaStr = delta >= 0 ? `+${delta}` : `${delta}`;
  const source = `${format}/${action.check}`;
  const detail = action.reason
    ? `${action.container}/${action.block}|${action.reason}`
    : `${action.container}/${action.block}`;

  return `${timestamp}|${act}|${deltaStr}|${source}|${detail}`;
}

/**
 * Format all actions from a health score into log entries.
 *
 * @param actions - All atomic actions from the health computation.
 * @param format - The file format.
 * @param filePath - The file being linted (included as header comment).
 * @returns Array of log entry strings.
 */
export function formatLogEntries(
  actions: AtomicAction[],
  format: string,
  filePath: string,
): string[] {
  const timestamp = new Date().toISOString().replace(/\.\d{3}Z$/, "Z");
  const lines: string[] = [
    `# ${filePath}`,
  ];

  for (const action of actions) {
    lines.push(formatLogEntry(action, format, timestamp));
  }

  return lines;
}

/**
 * Write a run-level header to the log file.
 *
 * Called once at the start of a run. Includes format, file count,
 * and version for run-level context.
 *
 * @param logPath - Path to the log file.
 * @param format - Format being linted.
 * @param fileCount - Number of files being processed.
 * @param version - Tool version.
 */
export async function writeLogHeader(
  logPath: string,
  format: string,
  fileCount: number,
  version: string,
): Promise<void> {
  const timestamp = new Date().toISOString().replace(/\.\d{3}Z$/, "Z");
  const header = [
    `# cws-struct ${version} — ${format} lint run`,
    `# ${timestamp} — ${fileCount} file(s)`,
    `# format: TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL`,
    "",
  ].join("\n");

  await Deno.writeTextFile(logPath, header, { append: true });
}

/**
 * Write log entries to a file. Appends to existing content.
 *
 * @param logPath - Path to the log file.
 * @param actions - Atomic actions to log.
 * @param format - File format name.
 * @param filePath - The file that was linted.
 */
export async function writeLogEntries(
  logPath: string,
  actions: AtomicAction[],
  format: string,
  filePath: string,
): Promise<void> {
  const entries = formatLogEntries(actions, format, filePath);
  const content = entries.join("\n") + "\n";

  // Append to log file — "the books are opened" (append-only).
  await Deno.writeTextFile(logPath, content, { append: true });
}

// ---------------------------------------------------------------------------
// Transform logging
// ---------------------------------------------------------------------------

/**
 * Format transform results as log entries.
 *
 * Transform results are LintResult[] (not AtomicAction[]) — they describe
 * what was changed, not health observations. Maps severity to action:
 *   info → "transform" (neutral — informational)
 *   warn → "transform_warn" (attention needed)
 *   error → "transform_error" (something went wrong)
 *
 * @param results - Transform diagnostics.
 * @param format - The file format (e.g., "go", "rust").
 * @param filePath - The file that was transformed.
 * @returns Array of log entry strings.
 */
export function formatTransformLogEntries(
  results: LintResult[],
  format: string,
  filePath: string,
): string[] {
  const timestamp = new Date().toISOString().replace(/\.\d{3}Z$/, "Z");
  const lines: string[] = [`# transform ${filePath}`];

  for (const r of results) {
    const action = r.severity === "error"
      ? "transform_error"
      : r.severity === "warn"
        ? "transform_warn"
        : "transform";
    const delta = r.severity === "error" ? "-1" : r.severity === "warn" ? "0" : "+1";
    const source = `${format}/${r.rule}`;
    const detail = r.line ? `line:${r.line}|${r.message}` : r.message;
    lines.push(`${timestamp}|${action}|${delta}|${source}|${detail}`);
  }

  return lines;
}

/**
 * Write transform log entries to a file.
 *
 * @param logPath - Path to the log file.
 * @param results - Transform diagnostics.
 * @param format - File format name.
 * @param filePath - The file that was transformed.
 */
export async function writeTransformLogEntries(
  logPath: string,
  results: LintResult[],
  format: string,
  filePath: string,
): Promise<void> {
  const entries = formatTransformLogEntries(results, format, filePath);
  if (entries.length <= 1) return; // Only header, no actual entries
  const content = entries.join("\n") + "\n";
  await Deno.writeTextFile(logPath, content, { append: true });
}

// ---------------------------------------------------------------------------
// Health log emission — extract actions from summaries and write to log
// ---------------------------------------------------------------------------

/**
 * Emit atomic health observations to a structured log file.
 * Format: TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL (from log.toml spec).
 *
 * Not lint-specific — any command that produces LintSummary with health
 * can write its observations to the audit trail.
 *
 * "The books are opened." — Revelation 20:12
 */
export async function emitHealthLog(
  logPath: string,
  summaries: LintSummary[],
  formatName: string,
): Promise<void> {
  for (const s of summaries) {
    if (!s.health) continue;
    // Collect all atomic actions from health blocks → containers → actions
    const actions = s.health.blocks.flatMap((b) =>
      b.containers.flatMap((c) => c.actions)
    );
    if (actions.length > 0) {
      try {
        await writeLogEntries(logPath, actions, formatName, s.file);
      } catch (e) {
        if (e instanceof Deno.errors.NotCapable || e instanceof Deno.errors.PermissionDenied) {
          console.error(
            `${COLORS.red}--log requires write permission. Run with: deno run --allow-read --allow-write mod.ts ...${COLORS.reset}`,
          );
          return; // Stop trying — permission won't change mid-run
        }
        throw e;
      }
    }
  }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Structured logging — TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL.
// Uses the existing CPI-SI log format from log.toml.
// Each AtomicAction becomes one log entry. Append-only.
// Transform results also get logged for full audit trail.
// Health log emission extracts from any command's summaries.
// The books are opened. Every observation recorded.
//
// "And the books were opened." — Revelation 20:12
// ============================================================================
