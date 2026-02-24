// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/commands/parse.ts
// key:     B-tov-cws-struct-lib-commands-parse
// title:   CWS Struct — CLI Parsing & Validation
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: CLI argument parsing, input validation, explicit-file detection,
//          lint-focus parsing, and exit code constants. Extracted from mod.ts
//          during Phase 1 God Object dismantling.
//
// biblical_foundation: "Prove all things; hold fast that which is good."
//   — 1 Thessalonians 5:21
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type {
  CliOptions,
  LintPolicy,
  LintGridOptions,
  LintLayer,
  BlockName,
} from "../foundation/mod.ts";
import { getFormat } from "../engine/mod.ts";
import { COLORS } from "../engine/mod.ts";

// Re-export exit codes from centralized config (canonical source)
export { EXIT_OK, EXIT_LINT_ERRORS, EXIT_TOOL_ERROR } from "./config.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Explicit file detection
// ---------------------------------------------------------------------------

/**
 * Identify which CLI targets are explicit files (not directories).
 *
 * When a user runs `cws-struct lint go path/to/file.go`, they explicitly
 * chose that file. If it has no structural markers, that's a warning — not
 * just a silent skip. Discovery (directory walks) can silently skip; explicit
 * targets should not.
 *
 * Returns resolved absolute paths so they match what discovery returns.
 */
export async function identifyExplicitFiles(targets: string[]): Promise<Set<string>> {
  const explicit = new Set<string>();
  for (const target of targets) {
    try {
      const resolved = await Deno.realPath(target);
      const stat = await Deno.stat(resolved);
      if (stat.isFile) {
        explicit.add(resolved);
      }
    } catch {
      // If we can't stat/resolve, discovery will report the error
    }
  }
  return explicit;
}

// ---------------------------------------------------------------------------
// Lint focus parsing
// ---------------------------------------------------------------------------

/**
 * Parse --check value into a LintGridOptions for block × layer slicing.
 *
 * Accepts:
 *   "metadata"       → vertical slice (focusBlock only)
 *   "R25"            → horizontal slice (focusLayer only)
 *   "metadata:R25"   → single cell (both)
 *   undefined        → full grid (no focus)
 */
export function parseLintFocus(check?: string): LintGridOptions | undefined {
  if (!check) return undefined;

  const LAYER_MAP: Record<string, LintLayer> = { r50: 0, r25: 1, r10: 2, r5: 3 };
  const BLOCK_NAMES = new Set<string>(["metadata", "setup", "body", "closing"]);

  const parts = check.split(":");
  const opts: LintGridOptions = {};

  for (const part of parts) {
    const lower = part.toLowerCase();
    if (BLOCK_NAMES.has(lower)) opts.focusBlock = lower as BlockName;
    else if (LAYER_MAP[lower] !== undefined) opts.focusLayer = LAYER_MAP[lower];
    else if (part) opts.focusContainer = part; // Preserve original case
  }

  return (opts.focusBlock || opts.focusLayer !== undefined || opts.focusContainer) ? opts : undefined;
}

// ---------------------------------------------------------------------------
// Input validation
// ---------------------------------------------------------------------------

/** Known CLI flags — anything else starting with "--" or "-" is suspicious. */
const KNOWN_FLAGS = new Set([
  "--verbose", "-v",
  "--errors-only",
  "--summary",
  "--dry-run",
  "--extensions",
  "--json",
  "--fail-fast",
  "--force",
  "--steps",
  "--policy",
  "--port",
  "--key",
  "--title",
  "--purpose",
  "--log",
  "--check",
  "--db",
  "--limit",
  "--debug",
  "--trace",
  "--inspect",
  "--why",
  "--changed",
  "--since",
  "--staged",
  "--suggest",
  "--deep", "-vv",
  "--help", "-h",
  "--version",
]);

/**
 * Validate CLI arguments. Returns an error message if invalid, undefined if clean.
 * Guards against obvious misuse before dispatching to commands.
 */
export function validateArgs(args: string[]): string | undefined {
  for (const arg of args) {
    // Reject excessively long arguments (path or otherwise) — 4096 is generous
    if (arg.length > 4096) {
      return `Argument too long (${arg.length} chars, max 4096)`;
    }

    // Warn on unknown flags (but don't block — could be future flags)
    // The parseArgs function already ignores unknown flags, this is belt-and-suspenders
    if ((arg.startsWith("--") || (arg.startsWith("-") && arg.length === 2)) && !KNOWN_FLAGS.has(arg)) {
      // Check if it looks like a known flag with a typo
      const command = args[0];
      if (command !== "help" && command !== "version" && command !== "formats") {
        console.error(
          `${COLORS.yellow}Warning: Unknown flag "${arg}" — ignoring.${COLORS.reset}`,
        );
      }
    }

    // Reject null bytes in arguments — prevents path injection via null terminators
    if (arg.includes("\0")) {
      return `Invalid argument: contains null byte`;
    }
  }
  return undefined;
}

// ---------------------------------------------------------------------------
// CLI argument parsing
// ---------------------------------------------------------------------------

export function parseArgs(args: string[]): CliOptions {
  if (args.length === 0 || args.includes("--help") || args.includes("-h")) {
    return {
      command: "help",
      targets: [],
      verbose: false,
      errorsOnly: false,
      summaryOnly: false,
      dryRun: false,
      extensions: false,
      json: false,
      failFast: false,
      force: false,
      steps: false,
      policy: "balanced",
      debug: false,
      trace: false,
      inspect: false,
      deep: false,
      why: false,
      changed: false,
      staged: false,
    };
  }

  if (args.includes("--version")) {
    return {
      command: "version",
      targets: [],
      verbose: false,
      errorsOnly: false,
      summaryOnly: false,
      dryRun: false,
      extensions: false,
      json: false,
      failFast: false,
      force: false,
      steps: false,
      policy: "balanced",
      debug: false,
      trace: false,
      inspect: false,
      deep: false,
      why: false,
      changed: false,
      staged: false,
    };
  }

  const command = args[0] as CliOptions["command"];
  const rest = args.slice(1);

  // Check if second arg is a format name
  let format: string | undefined;

  // Identify indices consumed as values of value-taking flags (--key X, --policy X, etc.)
  // so they don't leak into nonFlags → targets.
  const valueTakingFlags = new Set(["--key", "--title", "--purpose", "--policy", "--port", "--log", "--check", "--db", "--limit", "--since", "--suggest"]);
  const consumedIndices = new Set<number>();
  for (let i = 0; i < rest.length; i++) {
    if (valueTakingFlags.has(rest[i]!) && i + 1 < rest.length) {
      consumedIndices.add(i + 1);
    }
  }
  const nonFlags = rest.filter((a, i) => !a.startsWith("-") && !consumedIndices.has(i));
  if (nonFlags.length > 0) {
    // Try to match first non-flag as format name
    const candidate = nonFlags[0]!;
    if (getFormat(candidate)) {
      format = candidate;
    } else if (!candidate.includes("/") && !candidate.includes("\\") && !candidate.includes(".")) {
      // Looks like a format name (no path separators or extensions) but isn't registered.
      // Treat as unknown format so runLint/runTransform can report the error.
      format = candidate;
    }
  }

  const targets = nonFlags.filter((a) => a !== format);

  // Extract --key, --title, --purpose, --policy values
  const keyIdx = rest.indexOf("--key");
  const titleIdx = rest.indexOf("--title");
  const purposeIdx = rest.indexOf("--purpose");
  const policyIdx = rest.indexOf("--policy");
  const logIdx = rest.indexOf("--log");
  const checkIdx = rest.indexOf("--check");
  const dbIdx = rest.indexOf("--db");
  const sinceIdx = rest.indexOf("--since");

  const keyVal = keyIdx >= 0 ? rest[keyIdx + 1] : undefined;
  const titleVal = titleIdx >= 0 ? rest[titleIdx + 1] : undefined;
  const purposeVal = purposeIdx >= 0 ? rest[purposeIdx + 1] : undefined;
  const policyRaw = policyIdx >= 0 ? rest[policyIdx + 1] : undefined;
  const logPath = logIdx >= 0 ? rest[logIdx + 1] : undefined;
  const check = checkIdx >= 0 ? rest[checkIdx + 1] : undefined;
  const dbPath = dbIdx >= 0 ? rest[dbIdx + 1] : undefined;
  const sinceVal = sinceIdx >= 0 ? rest[sinceIdx + 1] : undefined;
  const suggestIdx = rest.indexOf("--suggest");
  const suggestRaw = suggestIdx >= 0 ? rest[suggestIdx + 1] : undefined;
  // --suggest with no value or non-numeric value = default (5), numeric = that count
  const suggest = suggestIdx >= 0
    ? (suggestRaw && /^\d+$/.test(suggestRaw) ? parseInt(suggestRaw, 10) : 5)
    : undefined;
  const policy: LintPolicy = policyRaw === "strict" || policyRaw === "growth"
    ? policyRaw
    : "balanced";

  // For create command: args are `create <format> <subtype> <dest>`
  // format is already extracted above; subtype is the next non-flag after format
  let subtype: string | undefined;
  if (command === "create" && format) {
    const afterFormat = nonFlags.filter((a) => a !== format);
    // First non-path arg is subtype, rest are targets
    if (afterFormat.length >= 2) {
      const candidateSubtype = afterFormat[0]!;
      if (!candidateSubtype.includes("/") && !candidateSubtype.includes("\\") && !candidateSubtype.includes(".")) {
        subtype = candidateSubtype;
      }
    }
  }

  // Filter subtype from targets for create command
  const finalTargets = command === "create" && subtype
    ? targets.filter((t) => t !== subtype)
    : targets;

  // For query command: first non-flag after "query" is the subcommand
  // e.g., `cws-struct query health path/to/file.go`
  //        command="query", queryCommand="health", targets=["path/to/file.go"]
  let queryCommand: string | undefined;
  let queryTargets = finalTargets;
  if (command === "query" && finalTargets.length > 0) {
    const sub = finalTargets[0]!;
    const validSubs = new Set(["health", "worst", "best", "patterns", "trend", "instance", "session", "stats"]);
    if (validSubs.has(sub)) {
      queryCommand = sub;
      queryTargets = finalTargets.slice(1);
    }
  }

  const isDebug = rest.includes("--debug");
  const isTrace = rest.includes("--trace");
  const isInspect = rest.includes("--inspect");
  const isWhy = rest.includes("--why");

  return {
    command: (command === "lint" || command === "transform" || command === "create" || command === "transpile" || command === "query" || command === "watch" || command === "formats" || command === "verify" || command === "studio" || command === "help") ? command : "help",
    helpTopic: command === "help" ? nonFlags[0] : undefined,
    format,
    targets: command === "query" ? queryTargets : finalTargets,
    verbose: rest.includes("--verbose") || rest.includes("-v") || rest.includes("-vv") || rest.includes("--deep"),
    deep: rest.includes("--deep") || rest.includes("-vv"),
    errorsOnly: rest.includes("--errors-only"),
    summaryOnly: rest.includes("--summary"),
    dryRun: rest.includes("--dry-run"),
    extensions: rest.includes("--extensions"),
    json: rest.includes("--json"),
    failFast: rest.includes("--fail-fast"),
    force: rest.includes("--force"),
    steps: rest.includes("--steps"),
    policy,
    subtype,
    key: keyVal,
    title: titleVal,
    purpose: purposeVal,
    logPath,
    check,
    dbPath,
    queryCommand,
    debug: isDebug || isTrace, // --trace implies --debug
    trace: isTrace,
    inspect: isInspect,
    why: isWhy,
    changed: rest.includes("--changed"),
    since: sinceVal,
    staged: rest.includes("--staged"),
    suggest,
  };
}

// ============================================================================
// CLOSING
// ============================================================================
//
// CLI parsing is the first gate — structure before content.
// Validate input, parse arguments, then dispatch to commands.
//
// "Prove all things; hold fast that which is good."
//   — 1 Thessalonians 5:21
// ============================================================================
