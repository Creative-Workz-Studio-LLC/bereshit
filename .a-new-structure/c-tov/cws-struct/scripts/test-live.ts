// ============================================================================
// METADATA
// ============================================================================
//
// file:    scripts/test-live.ts
// key:     B-tov-cws-struct-scripts-test-live
// title:   CWS Struct — Live Test Runner
// type:    Code (Script)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Wrapper around `deno test` that replaces scrolling test output
//          with a live dashboard: progress bar, layer tallies, current file,
//          pass/fail counter, and Scripture verses. Makes the 50-second wait
//          interesting instead of noisy.
//
//          Usage:
//            deno run --allow-read --allow-run --allow-env scripts/test-live.ts
//            deno task test:live
//
//          Options:
//            --verbose    Show individual test names as they pass
//            --no-verse   Suppress Scripture verses
//            --report     Show full scorecard at end (like test-report.ts)
//
// biblical_foundation: "Wait on the LORD: be of good courage, and he shall
//   strengthen thine heart: wait, I say, on the LORD."
//   — Psalm 27:14
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Colors
// ---------------------------------------------------------------------------

const C = {
  reset: "\x1b[0m",
  red: "\x1b[31m",
  yellow: "\x1b[33m",
  blue: "\x1b[34m",
  green: "\x1b[32m",
  dim: "\x1b[2m",
  bold: "\x1b[1m",
  cyan: "\x1b[36m",
  magenta: "\x1b[35m",
  white: "\x1b[37m",
  bgGreen: "\x1b[42m",
  bgRed: "\x1b[41m",
  bgBlue: "\x1b[44m",
  up: (n: number) => `\x1b[${n}A`,
  clear: "\x1b[2K",
  hide: "\x1b[?25l",
  show: "\x1b[?25h",
  saveCursor: "\x1b7",
  restoreCursor: "\x1b8",
} as const;

// ---------------------------------------------------------------------------
// Scripture — changes every ~10 seconds during run
// ---------------------------------------------------------------------------

const VERSES = [
  { ref: "Psalm 27:14", text: "Wait on the LORD: be of good courage, and he shall strengthen thine heart." },
  { ref: "Psalm 119:105", text: "Thy word is a lamp unto my feet, and a light unto my path." },
  { ref: "Proverbs 3:5-6", text: "Trust in the LORD with all thine heart; and lean not unto thine own understanding." },
  { ref: "Psalm 139:14", text: "I will praise thee; for I am fearfully and wonderfully made." },
  { ref: "Psalm 26:2", text: "Examine me, O LORD, and prove me; try my reins and my heart." },
  { ref: "Ecclesiastes 9:10", text: "Whatsoever thy hand findeth to do, do it with thy might." },
  { ref: "Proverbs 16:11", text: "A just weight and balance are the LORD's." },
  { ref: "1 Thess 5:21", text: "Prove all things; hold fast that which is good." },
  { ref: "Proverbs 27:17", text: "Iron sharpeneth iron; so a man sharpeneth the countenance of his friend." },
  { ref: "Exodus 25:40", text: "Look that thou make them after their pattern." },
  { ref: "Col 3:23", text: "Whatsoever ye do, do it heartily, as to the Lord." },
  { ref: "Psalm 90:17", text: "Let the beauty of the LORD our God be upon us: and establish the work of our hands." },
  { ref: "Proverbs 24:3-4", text: "By wisdom a house is built, and by understanding it is established." },
  { ref: "Isaiah 28:10", text: "Precept upon precept; line upon line; here a little, and there a little." },
  { ref: "Phil 1:6", text: "He which hath begun a good work in you will perform it." },
  { ref: "James 1:4", text: "Let patience have her perfect work, that ye may be perfect and entire." },
] as const;

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

interface LiveState {
  total: number;
  passed: number;
  failed: number;
  skipped: number;
  currentFile: string;
  currentTest: string;
  startTime: number;
  layers: Record<string, { pass: number; fail: number }>;
  formats: Record<string, { pass: number; fail: number }>;
  failures: string[];
  verseIndex: number;
  lastVerseChange: number;
  estimatedTotal: number;
}

interface CliOpts {
  verbose: boolean;
  noVerse: boolean;
  report: boolean;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Dashboard rendering
// ---------------------------------------------------------------------------

const DASH_LINES = 9; // Number of lines the dashboard occupies

function progressBar(current: number, total: number, width: number): string {
  if (total === 0) return C.dim + "░".repeat(width) + C.reset;
  const pct = Math.min(current / total, 1);
  const filled = Math.round(pct * width);
  const empty = width - filled;

  return (
    C.green + "█".repeat(filled) +
    C.dim + "░".repeat(empty) +
    C.reset
  );
}

function layerBar(state: LiveState): string {
  const order = ["R50", "R25", "R10", "R05", "T00"];
  const parts: string[] = [];

  for (const layer of order) {
    const data = state.layers[layer];
    if (!data) {
      parts.push(`${C.dim}${layer}:—${C.reset}`);
      continue;
    }
    const total = data.pass + data.fail;
    const color = data.fail > 0 ? C.red : C.green;
    parts.push(`${color}${layer}:${data.pass}${data.fail > 0 ? `/${total}` : ""}${C.reset}`);
  }

  return parts.join("  ");
}

function formatBar(state: LiveState): string {
  const order = ["go", "rust", "toml", "json", "mk", "dot", "omni", "ofd"];
  const display: Record<string, string> = {
    go: "Go", rust: "Rs", toml: "TM", json: "JS",
    mk: "Mk", dot: "Df", omni: "Om", ofd: "OF",
  };
  const parts: string[] = [];

  for (const fmt of order) {
    const data = state.formats[fmt];
    if (!data) {
      parts.push(`${C.dim}${display[fmt] ?? fmt}:—${C.reset}`);
      continue;
    }
    const color = data.fail > 0 ? C.red : C.green;
    parts.push(`${color}${display[fmt] ?? fmt}:${data.pass}${C.reset}`);
  }

  return parts.join(" ");
}

function elapsedStr(state: LiveState): string {
  const sec = ((Date.now() - state.startTime) / 1000).toFixed(1);
  return `${sec}s`;
}

function getVerse(state: LiveState): typeof VERSES[number] {
  return VERSES[state.verseIndex % VERSES.length]!;
}

function renderDashboard(state: LiveState, _opts: CliOpts): string[] {
  const pct = state.estimatedTotal > 0
    ? Math.min((state.total / state.estimatedTotal) * 100, 100).toFixed(0)
    : "—";

  const passStr = `${C.green}${state.passed}${C.reset}`;
  const failStr = state.failed > 0
    ? `  ${C.red}${C.bold}${state.failed} FAILED${C.reset}`
    : "";
  const skipStr = state.skipped > 0
    ? `  ${C.dim}${state.skipped} skip${C.reset}`
    : "";

  // Truncate paths for display
  const shortFile = state.currentFile.replace(/.*tests\//, "tests/");
  const shortTest = state.currentTest.length > 60
    ? state.currentTest.slice(0, 57) + "..."
    : state.currentTest;

  const verse = getVerse(state);

  const lines = [
    ``,
    `  ${C.bold}CWS Struct${C.reset}  ${progressBar(state.total, state.estimatedTotal, 30)}  ${pct}%  ${elapsedStr(state)}`,
    `  ${passStr} passed${failStr}${skipStr}  ${C.dim}(${state.total} total)${C.reset}`,
    ``,
    `  ${C.dim}layers${C.reset}  ${layerBar(state)}`,
    `  ${C.dim}format${C.reset}  ${formatBar(state)}`,
    ``,
    `  ${C.dim}${shortFile}${C.reset}`,
    `  ${C.cyan}${shortTest}${C.reset}`,
  ];

  // Add verse if enabled
  if (!_opts.noVerse) {
    lines.push(``);
    lines.push(`  ${C.dim}${C.magenta}"${verse.text}"${C.reset}`);
    lines.push(`  ${C.dim}— ${verse.ref}${C.reset}`);
  }

  return lines;
}

function writeDashboard(state: LiveState, opts: CliOpts, firstRender: boolean): number {
  const lines = renderDashboard(state, opts);
  const lineCount = lines.length;

  // Move cursor up to overwrite previous render
  if (!firstRender) {
    Deno.stdout.writeSync(new TextEncoder().encode(C.up(lineCount)));
  }

  // Clear and write each line
  for (const line of lines) {
    Deno.stdout.writeSync(new TextEncoder().encode(C.clear + "\r" + line + "\n"));
  }

  return lineCount;
}

// ---------------------------------------------------------------------------
// 2. Line parsing — classify each test result as it streams
// ---------------------------------------------------------------------------

function classifyLine(line: string, state: LiveState): void {
  // File header: "running N tests from ./tests/..."
  const fileMatch = line.match(/running \d+ tests? from \.\/(.+)/);
  if (fileMatch) {
    state.currentFile = fileMatch[1]!;
    return;
  }

  // Test pass: "name ... ok (Nms)"
  const passMatch = line.match(/^(.+?) \.\.\.\s+ok\s+\(\d+m?s\)/);
  if (passMatch) {
    state.total++;
    state.passed++;
    state.currentTest = passMatch[1]!;
    classifyResult(passMatch[1]!, "pass", state);
    return;
  }

  // Test fail: "name ... FAILED (Nms)"
  const failMatch = line.match(/^(.+?) \.\.\.\s+FAILED\s+\(\d+m?s\)/);
  if (failMatch) {
    state.total++;
    state.failed++;
    state.currentTest = failMatch[1]!;
    state.failures.push(failMatch[1]!);
    classifyResult(failMatch[1]!, "fail", state);
    return;
  }

  // Test skip: "name ... ignored"
  const skipMatch = line.match(/^(.+?) \.\.\.\s+ignored/);
  if (skipMatch) {
    state.total++;
    state.skipped++;
    return;
  }
}

function classifyResult(name: string, status: "pass" | "fail", state: LiveState): void {
  // Layer
  const layerMatch = name.match(/\[CWS-(R\d+|T\d+)-/) || name.match(/\b(R50|R25|R10|R05|T00)\b/);
  if (layerMatch) {
    const layer = layerMatch[1]!;
    if (!state.layers[layer]) state.layers[layer] = { pass: 0, fail: 0 };
    state.layers[layer]![status === "pass" ? "pass" : "fail"]++;
  }

  // Format
  const fmtMatch = name.match(/\b(go|rust|toml|json|makefile|dotfiles|omni|ofd)\b/i);
  if (fmtMatch) {
    let fmt = fmtMatch[1]!.toLowerCase();
    // Normalize to short keys
    if (fmt === "makefile") fmt = "mk";
    if (fmt === "dotfiles") fmt = "dot";
    if (!state.formats[fmt]) state.formats[fmt] = { pass: 0, fail: 0 };
    state.formats[fmt]![status === "pass" ? "pass" : "fail"]++;
  }
}

// ---------------------------------------------------------------------------
// 3. Stream processor
// ---------------------------------------------------------------------------

async function streamTests(state: LiveState, opts: CliOpts): Promise<number> {
  const cmd = new Deno.Command("deno", {
    args: [
      "test",
      "--allow-read", "--allow-write", "--allow-run", "--allow-env",
      "--ignore=tests/tool/T00/data/database_test.ts",
      "--ignore=tests/content/R10/api/health_query_test.ts",
      "tests/",
    ],
    stdout: "piped",
    stderr: "piped",
  });

  const process = cmd.spawn();

  // Read stdout line by line
  const reader = process.stdout.getReader();
  const decoder = new TextDecoder();
  let buffer = "";
  let firstRender = true;
  let renderInterval: number | undefined;

  // Periodic render + verse rotation
  renderInterval = setInterval(() => {
    // Rotate verse every ~10 seconds
    const now = Date.now();
    if (now - state.lastVerseChange > 10_000) {
      state.verseIndex = (state.verseIndex + 1) % VERSES.length;
      state.lastVerseChange = now;
    }
    writeDashboard(state, opts, firstRender);
    firstRender = false;
  }, 200);

  try {
    while (true) {
      const { done, value } = await reader.read();
      if (done) break;

      buffer += decoder.decode(value, { stream: true });

      // Process complete lines
      let newlineIdx: number;
      while ((newlineIdx = buffer.indexOf("\n")) !== -1) {
        const line = buffer.slice(0, newlineIdx);
        buffer = buffer.slice(newlineIdx + 1);

        // Strip ANSI for parsing
        const clean = line.replace(/\x1b\[[0-9;]*m/g, "").trim();
        if (clean.length > 0) {
          classifyLine(clean, state);
        }
      }
    }

    // Process remaining buffer
    if (buffer.trim().length > 0) {
      const clean = buffer.replace(/\x1b\[[0-9;]*m/g, "").trim();
      classifyLine(clean, state);
    }
  } finally {
    clearInterval(renderInterval);
  }

  // Also consume stderr (Deno puts some output there)
  const stderrReader = process.stderr.getReader();
  let stderrBuf = "";
  while (true) {
    const { done, value } = await stderrReader.read();
    if (done) break;
    stderrBuf += decoder.decode(value, { stream: true });
  }

  // Parse any test results from stderr too
  for (const line of stderrBuf.split("\n")) {
    const clean = line.replace(/\x1b\[[0-9;]*m/g, "").trim();
    if (clean.length > 0) classifyLine(clean, state);
  }

  const { code } = await process.status;

  // Final render
  writeDashboard(state, opts, firstRender);

  return code;
}

// ---------------------------------------------------------------------------
// 4. End-of-run summary
// ---------------------------------------------------------------------------

function printSummary(state: LiveState, opts: CliOpts): void {
  const elapsed = ((Date.now() - state.startTime) / 1000).toFixed(1);

  console.log();
  console.log(`${C.bold}${"═".repeat(60)}${C.reset}`);

  if (state.failed === 0) {
    console.log(`  ${C.green}${C.bold}ALL TESTS PASSED${C.reset}  ${state.passed} tests in ${elapsed}s`);
  } else {
    console.log(`  ${C.red}${C.bold}${state.failed} TESTS FAILED${C.reset}  ${state.passed} passed, ${state.failed} failed in ${elapsed}s`);
  }

  console.log(`${C.bold}${"═".repeat(60)}${C.reset}`);

  // Layer breakdown
  console.log();
  console.log(`  ${C.bold}Layers${C.reset}`);
  const layerOrder = ["R50", "R25", "R10", "R05", "T00"];
  const layerNames: Record<string, string> = {
    R50: "Existence", R25: "Organization", R10: "Placement", R05: "Substance", T00: "Tool",
  };

  for (const layer of layerOrder) {
    const data = state.layers[layer];
    if (!data) continue;
    const total = data.pass + data.fail;
    const pct = total > 0 ? ((data.pass / total) * 100).toFixed(0) : "—";
    const color = data.fail > 0 ? C.red : C.green;
    const failNote = data.fail > 0 ? `  ${C.red}${data.fail}F${C.reset}` : "";
    console.log(`  ${color}${layer}${C.reset} ${C.dim}${layerNames[layer] ?? ""}${C.reset}  ${data.pass}P${failNote}  ${C.dim}${pct}%${C.reset}`);
  }

  // Format breakdown
  console.log();
  console.log(`  ${C.bold}Formats${C.reset}`);
  const fmtDisplay: Record<string, string> = {
    go: "Go", rust: "Rust", toml: "TOML", json: "JSON",
    mk: "Makefile", dot: "Dotfiles", omni: "OmniCode", ofd: "OFD",
  };
  for (const [fmt, data] of Object.entries(state.formats)) {
    if (!data) continue;
    const color = data.fail > 0 ? C.red : C.green;
    const failNote = data.fail > 0 ? `  ${C.red}${data.fail}F${C.reset}` : "";
    console.log(`  ${color}${fmtDisplay[fmt] ?? fmt}${C.reset}  ${data.pass}P${failNote}`);
  }

  // Failures detail
  if (state.failures.length > 0) {
    console.log();
    console.log(`  ${C.red}${C.bold}Failures${C.reset}`);
    for (const f of state.failures) {
      console.log(`  ${C.red}✗${C.reset} ${f}`);
    }
  }

  // Full report mode
  if (opts.report) {
    // The report is already printed above in more detail
    // This flag is for future expansion (e.g., writing JSON)
  }

  console.log();
}

// ---------------------------------------------------------------------------
// 5. CLI and main
// ---------------------------------------------------------------------------

function parseOpts(): CliOpts {
  const args = Deno.args;
  return {
    verbose: args.includes("--verbose") || args.includes("-v"),
    noVerse: args.includes("--no-verse"),
    report: args.includes("--report"),
  };
}

async function main(): Promise<void> {
  const opts = parseOpts();

  const state: LiveState = {
    total: 0,
    passed: 0,
    failed: 0,
    skipped: 0,
    currentFile: "",
    currentTest: "starting...",
    startTime: Date.now(),
    layers: {},
    formats: {},
    failures: [],
    verseIndex: Math.floor(Math.random() * VERSES.length),
    lastVerseChange: Date.now(),
    estimatedTotal: 1800, // Includes test steps — update as suite grows
  };

  // Hide cursor during run
  Deno.stdout.writeSync(new TextEncoder().encode(C.hide));

  try {
    const exitCode = await streamTests(state, opts);

    // Show cursor
    Deno.stdout.writeSync(new TextEncoder().encode(C.show));

    printSummary(state, opts);
    Deno.exit(exitCode);
  } catch (e) {
    // Always restore cursor
    Deno.stdout.writeSync(new TextEncoder().encode(C.show));
    throw e;
  }
}

await main();

// ============================================================================
// CLOSING
// ============================================================================
//
// Live test runner — real-time dashboard replacing scrolling test output.
// Progress bar, layer tallies, format tallies, Scripture verses, scorecard.
//
// "Wait on the LORD: be of good courage, and he shall strengthen thine
//  heart: wait, I say, on the LORD." — Psalm 27:14
// ============================================================================
