// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/debug.ts
// key:     B-tov-cws-struct-lib-engine-debug
// title:   CWS Struct — Debug & Trace Output
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Diagnostic output for --debug and --trace CLI modes.
//
//          --debug: Shows internal state — parser detections, data layer
//          lookups, per-check timing, config values, layer transitions.
//
//          --trace: Step-by-step execution — function entry/exit, every
//          decision point, every action produced, cascade decisions.
//
//          Both write to stderr so stdout stays clean for normal/JSON output.
//          Compatible with --log and --json.
//
// biblical_foundation: "For there is nothing covered, that shall not be
//   revealed; neither hid, that shall not be known." — Luke 12:2
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { AtomicAction, HealthScore } from "../foundation/mod.ts";
import type { LintResult, CliOptions } from "../foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Debug context — singleton state set once per CLI invocation
// ---------------------------------------------------------------------------

/** Debug/trace state — set once per session, read everywhere. */
interface DebugState {
  debug: boolean;
  trace: boolean;
  /** Timing map: check name → cumulative ms. */
  timings: Map<string, number>;
  /** Per-file timing start. */
  fileStart: number;
}

const _state: DebugState = {
  debug: false,
  trace: false,
  timings: new Map(),
  fileStart: 0,
};

/**
 * Initialize debug/trace from CLI options.
 * Call once at startup before any debug output.
 */
export function initDebug(opts: CliOptions): void {
  _state.debug = opts.debug;
  _state.trace = opts.trace;
}

/** Check if debug output is enabled. */
export function isDebug(): boolean {
  return _state.debug;
}

/** Check if trace output is enabled. */
export function isTrace(): boolean {
  return _state.trace;
}

// ---------------------------------------------------------------------------
// Stderr writer — all debug/trace goes to stderr
// ---------------------------------------------------------------------------

const encoder = new TextEncoder();

/** Write a line to stderr. Prefixed with [debug] or [trace]. */
function dbg(msg: string): void {
  Deno.stderr.writeSync(encoder.encode(`[debug] ${msg}\n`));
}

function trc(msg: string): void {
  Deno.stderr.writeSync(encoder.encode(`[trace] ${msg}\n`));
}

// ---------------------------------------------------------------------------
// Debug output — --debug level
// ---------------------------------------------------------------------------

/**
 * Emit config values at startup.
 * Shows what the tool is running with — versions, concurrency, limits.
 */
export function debugConfig(config: {
  version: string;
  concurrency: number;
  maxFiles: number;
  maxFileSize: number;
  excludedDirs: string[];
  impactWeights: Record<string, number>;
}): void {
  if (!_state.debug) return;
  dbg(`config: version=${config.version}, concurrency=${config.concurrency}`);
  dbg(`config: maxFiles=${config.maxFiles}, maxFileSize=${config.maxFileSize}`);
  dbg(`config: excludedDirs=[${config.excludedDirs.join(", ")}]`);
  dbg(`config: impactWeights={error:${config.impactWeights.error}, warn:${config.impactWeights.warn}, info:${config.impactWeights.info}}`);
}

/**
 * Emit policy setting.
 */
export function debugPolicy(policy: string): void {
  if (!_state.debug) return;
  dbg(`policy: ${policy}`);
}

/**
 * Emit file discovery results.
 */
export function debugDiscovery(formatName: string, fileCount: number): void {
  if (!_state.debug) return;
  dbg(`discovery: ${formatName} — ${fileCount} file(s)`);
}

/**
 * Mark the start of linting a file (for timing).
 */
export function debugFileStart(filePath: string): void {
  if (!_state.debug) return;
  _state.fileStart = performance.now();
  _state.timings.clear();
  dbg(`── file: ${filePath}`);
}

/**
 * Emit file completion with timing.
 */
export function debugFileEnd(filePath: string, resultCount: number): void {
  if (!_state.debug) return;
  const elapsed = (performance.now() - _state.fileStart).toFixed(1);
  dbg(`── done: ${filePath} — ${resultCount} results in ${elapsed}ms`);

  // Print timing breakdown if trace
  if (_state.trace && _state.timings.size > 0) {
    const sorted = [..._state.timings.entries()].sort((a, b) => b[1] - a[1]);
    trc(`  timing breakdown:`);
    for (const [check, ms] of sorted) {
      trc(`    ${check}: ${ms.toFixed(1)}ms`);
    }
  }
}

/**
 * Emit layer transition.
 * Shows when the pipeline moves between detection layers.
 */
export function debugLayerTransition(
  layer: number,
  resolution: string,
  name: string,
  checkCount: number,
  allPassed: boolean,
): void {
  if (!_state.debug) return;
  const status = allPassed ? "pass" : "FAIL";
  dbg(`  Layer ${layer} (${resolution}) — ${name}: ${checkCount} checks, ${status}`);
}

/**
 * Emit cascade decision.
 * Shows when and why cascade happens.
 */
export function debugCascade(
  reason: string,
  affectedCount: number,
): void {
  if (!_state.debug) return;
  dbg(`  CASCADE: ${reason} — ${affectedCount} actions neutralized`);
}

/**
 * Emit health score computation result.
 */
export function debugHealth(health: HealthScore): void {
  if (!_state.debug) return;
  dbg(`  health: ${health.total}/±100 ${health.level} ${health.emoji} — ${health.totalActions} actions (${health.alignedCount}+, ${health.neutralCount}○, ${health.misalignedCount}-)`);
}

// ---------------------------------------------------------------------------
// Trace output — --trace level (very verbose)
// ---------------------------------------------------------------------------

/**
 * Trace function entry.
 * Shows every function call in the lint pipeline.
 */
export function traceEnter(fn: string, context?: string): void {
  if (!_state.trace) return;
  const ctx = context ? ` | ${context}` : "";
  trc(`  → ${fn}${ctx}`);
}

/**
 * Trace function exit with results.
 */
export function traceExit(fn: string, actionCount: number): void {
  if (!_state.trace) return;
  trc(`  ← ${fn}: ${actionCount} actions`);
}

/**
 * Trace a check execution with timing.
 * Call this around each check function.
 */
export function traceCheckStart(_checkName: string): number {
  if (!_state.trace) return 0;
  return performance.now();
}

export function traceCheckEnd(checkName: string, startTime: number, actionCount: number): void {
  if (!_state.trace) return;
  const elapsed = performance.now() - startTime;
  _state.timings.set(checkName, (_state.timings.get(checkName) ?? 0) + elapsed);
  trc(`    ${checkName}: ${actionCount} actions (${elapsed.toFixed(1)}ms)`);
}

/**
 * Trace a parser detection.
 * Shows what the parser finds — block boundaries, section markers, etc.
 */
export function traceDetection(what: string, line: number, detail: string): void {
  if (!_state.trace) return;
  trc(`    detect: ${what} at line ${line} — ${detail}`);
}

/**
 * Trace a data layer lookup.
 * Shows when the pipeline consults registries.
 */
export function traceLookup(registry: string, key: string, result: string): void {
  if (!_state.trace) return;
  trc(`    lookup: ${registry}.get("${key}") → ${result}`);
}

/**
 * Trace an individual action produced.
 */
export function traceAction(action: AtomicAction): void {
  if (!_state.trace) return;
  const dir = action.direction > 0 ? "+1" : action.direction < 0 ? "-1" : " 0";
  const label = action.direction > 0 ? "aligned" : action.direction < 0 ? "misaligned" : "neutral";
  const layer = action.layer !== undefined ? ` L${action.layer}` : "";
  const reason = action.reason ? ` — ${action.reason}` : "";
  trc(`    action: ${dir} ${label}${layer}, ${action.block}/${action.container}/${action.check}${reason}`);
}

/**
 * Trace a cascade decision point.
 */
export function traceCascadeDecision(
  trigger: string,
  cascadedBlock: string,
  actionsBefore: number,
  actionsAfter: number,
): void {
  if (!_state.trace) return;
  const neutralized = actionsBefore - actionsAfter;
  trc(`    cascade: ${trigger} → ${cascadedBlock} block: ${neutralized}/${actionsBefore} actions neutralized`);
}

/**
 * Trace a lint result produced.
 */
export function traceResult(result: LintResult): void {
  if (!_state.trace) return;
  const lineRef = result.line ? `:${result.line}` : "";
  const code = result.errorCode ? ` [${result.errorCode}]` : "";
  trc(`    result: ${result.severity}${code} ${result.rule}${lineRef}: ${result.message}`);
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Debug/trace output — stderr-only, compatible with --json and --log.
// Two levels: --debug for diagnostics, --trace for full execution log.
// --trace implies --debug. Both are additive (more verbose, not different).
//
// "Nothing covered that shall not be revealed." — Luke 12:2
// ============================================================================
