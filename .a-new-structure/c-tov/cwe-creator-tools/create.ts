#!/usr/bin/env -S deno run --allow-read --allow-write --allow-run
// ============================================================================
// METADATA
// ============================================================================
//
// file:    create.ts
// key:     B-tools-create
// title:   CWS Creator — Interactive File Wizard
// type:    Tool (CLI Interface)
// version: a-01.00
// created: 2026-02-18
// authors: Nova Dawn (CPI-SI)
// purpose: Interactive CLI wizard for creating new files from seed templates.
//          The 4th tool in the cwe-creator-tools suite — the human interface.
//          Collects metadata through guided prompts, dispatches to scaffolders,
//          verifies output through the engine.
//
//          "All the person has to do is update the template information surgically."
//
// biblical_foundation: "Look that thou make them after their pattern, which
//   was shewed thee in the mount." — Exodus 25:40
//   The tool shows the pattern. The developer fills in the substance.
//
// usage:
//   deno run --allow-read --allow-write --allow-run create.ts
//   deno task create
//   (interactive — requires terminal)
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Imports
// ---------------------------------------------------------------------------

import { resolve, dirname, basename } from "@std/path";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const VERSION = "a-01.00";

/** ANSI styling. */
const C = {
  reset:  "\x1b[0m",
  bold:   "\x1b[1m",
  dim:    "\x1b[2m",
  cyan:   "\x1b[36m",
  green:  "\x1b[32m",
  yellow: "\x1b[33m",
  red:    "\x1b[31m",
} as const;

/** Terminal control sequences. */
const T = {
  cls:    "\x1b[2J\x1b[H",
  hide:   "\x1b[?25l",
  show:   "\x1b[?25h",
} as const;

/** Script directory — resolves scaffolder paths relative to this file. */
const SCRIPT_DIR = dirname(new URL(import.meta.url).pathname);
const SCAFFOLD_4BLOCK = resolve(SCRIPT_DIR, "scaffold-4block.ts");
const SCAFFOLD_3BLOCK = resolve(SCRIPT_DIR, "scaffold-3block.ts");

/** Menu definitions. */
const CODE_LANGUAGES: MenuItem[] = [
  { label: "Go",   value: "go",   desc: "Go library, executable, or test" },
  { label: "Rust", value: "rust", desc: "Rust library, executable, or test" },
];

const CODE_ROLES: MenuItem[] = [
  { label: "Library",    value: "library",    desc: "Reusable package, imported by others" },
  { label: "Executable", value: "executable", desc: "Standalone binary with main()" },
  { label: "Test",       value: "test",       desc: "Validation and demo testing" },
];

const DATA_FORMATS: MenuItem[] = [
  { label: "TOML",         value: "toml",         desc: "Configuration with OmniCode tables" },
  { label: "JSONC",        value: "jsonc",        desc: "Configuration with _ prefixed keys" },
  { label: "JSON",         value: "json",         desc: "Data file (no comments)" },
  { label: "EditorConfig", value: "editorconfig", desc: "Editor settings" },
  { label: "Gitignore",    value: "gitignore",    desc: "Git ignore patterns" },
  { label: "Git Message",  value: "gitmessage",   desc: "Commit message template" },
  { label: "Env",          value: "env",          desc: "Environment variables" },
];

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

interface MenuItem {
  label: string;
  value: string;
  desc?: string;
}

interface WizardState {
  type: "code" | "data" | null;
  format: string | null;
  role: string | null;
  dest: string | null;
  key: string | null;
  title: string | null;
  purpose: string | null;
  version: string;
}

type StepName = "type" | "format" | "role" | "path" | "metadata" | "confirm";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Terminal I/O
// ---------------------------------------------------------------------------

const enc = new TextEncoder();

function write(s: string): void {
  Deno.stdout.writeSync(enc.encode(s));
}

function cls(): void { write(T.cls); }

/**
 * Read a single keypress from raw stdin.
 * Returns: "up", "down", "enter", "backspace", "ctrl-c", or the character.
 */
async function readKey(): Promise<string> {
  const buf = new Uint8Array(8);
  Deno.stdin.setRaw(true);
  try {
    const n = await Deno.stdin.read(buf);
    if (n === null || n === 0) return "";

    if (buf[0] === 27) {
      if (n >= 3 && buf[1] === 91) {
        if (buf[2] === 65) return "up";
        if (buf[2] === 66) return "down";
      }
      return "escape";
    }
    if (buf[0] === 13 || buf[0] === 10) return "enter";
    if (buf[0] === 127 || buf[0] === 8) return "backspace";
    if (buf[0] === 3) return "ctrl-c";

    return new TextDecoder().decode(buf.subarray(0, n));
  } finally {
    Deno.stdin.setRaw(false);
  }
}

// ---------------------------------------------------------------------------
// UI — Header
// ---------------------------------------------------------------------------

function getSteps(state: WizardState): StepName[] {
  if (state.type === "code") {
    return ["type", "format", "role", "path", "metadata", "confirm"];
  }
  return ["type", "format", "path", "metadata", "confirm"];
}

function printHeader(state: WizardState, stepIdx: number, total: number): void {
  const stepText = `Step ${stepIdx + 1}/${total}`;
  const pad = Math.max(60 - 13 - stepText.length, 1);

  console.log();
  console.log(`  ${C.dim}${"═".repeat(60)}${C.reset}`);
  console.log(`  ${C.bold}CWS Creator${C.reset}${" ".repeat(pad)}${C.dim}${stepText}${C.reset}`);
  console.log(`  ${C.dim}"Look that thou make them after their pattern." — Ex 25:40${C.reset}`);
  console.log(`  ${C.dim}${"═".repeat(60)}${C.reset}`);
  console.log();

  // Completed selections
  if (state.type !== null) {
    const label = state.type === "code" ? "Code (4-block)" : "Data (3-block)";
    console.log(`  ${C.green}✓${C.reset} Type:     ${label}`);
  }
  if (state.format !== null) {
    const kind = state.type === "code" ? "Language" : "Format";
    console.log(`  ${C.green}✓${C.reset} ${kind}:  ${state.format}`);
  }
  if (state.role !== null) {
    console.log(`  ${C.green}✓${C.reset} Role:     ${state.role}`);
  }
  if (state.dest !== null) {
    console.log(`  ${C.green}✓${C.reset} Path:     ${state.dest}`);
  }
  if (state.key !== null) {
    console.log(`  ${C.green}✓${C.reset} Key:      ${state.key}`);
  }
  if (state.title !== null) {
    console.log(`  ${C.green}✓${C.reset} Title:    ${state.title}`);
  }

  // Blank line after completed items
  const any = state.type || state.format || state.role || state.dest || state.key;
  if (any) console.log();
}

// ---------------------------------------------------------------------------
// UI — Menu Selection
// ---------------------------------------------------------------------------

async function selectMenu(
  title: string,
  options: MenuItem[],
  state: WizardState,
  stepIdx: number,
  total: number,
): Promise<string | null> {
  let sel = 0;

  const render = (): void => {
    cls();
    printHeader(state, stepIdx, total);
    console.log(`  ${C.bold}${title}${C.reset}`);
    console.log();
    for (let i = 0; i < options.length; i++) {
      const o = options[i]!;
      const d = o.desc ? ` ${C.dim}— ${o.desc}${C.reset}` : "";
      if (i === sel) {
        console.log(`  ${C.cyan}❯${C.reset} ${C.bold}${o.label}${C.reset}${d}`);
      } else {
        console.log(`    ${C.dim}${o.label}${d}${C.reset}`);
      }
    }
    console.log();
    console.log(`  ${C.dim}↑↓ select · enter confirm · backspace back · ctrl+c cancel${C.reset}`);
  };

  render();
  while (true) {
    const key = await readKey();
    if (key === "up" && sel > 0) { sel--; render(); }
    else if (key === "down" && sel < options.length - 1) { sel++; render(); }
    else if (key === "enter") return options[sel]!.value;
    else if (key === "backspace") return null;
    else if (key === "ctrl-c") { write(T.show); console.log(); Deno.exit(0); }
  }
}

// ---------------------------------------------------------------------------
// UI — Text Input
// ---------------------------------------------------------------------------

async function textInput(
  prompt: string,
  defaultVal: string,
  state: WizardState,
  stepIdx: number,
  total: number,
): Promise<string | null> {
  let value = "";

  const render = (): void => {
    cls();
    printHeader(state, stepIdx, total);
    console.log(`  ${C.bold}${prompt}${C.reset}`);
    console.log();

    // Show input with placeholder or typed value
    if (value.length > 0) {
      console.log(`  ${C.cyan}▸${C.reset} ${value}${C.dim}█${C.reset}`);
    } else if (defaultVal) {
      console.log(`  ${C.cyan}▸${C.reset} ${C.dim}${defaultVal}█${C.reset}`);
    } else {
      console.log(`  ${C.cyan}▸${C.reset} ${C.dim}█${C.reset}`);
    }

    console.log();
    const hint = defaultVal
      ? "enter accept · type to change · backspace back"
      : "type · enter confirm · backspace back";
    console.log(`  ${C.dim}${hint}${C.reset}`);
  };

  render();
  while (true) {
    const key = await readKey();
    if (key === "enter") {
      const result = value || defaultVal;
      if (!result) continue; // Don't accept empty with no default
      return result;
    }
    if (key === "backspace") {
      if (value.length > 0) { value = value.slice(0, -1); render(); }
      else return null;
    }
    else if (key === "ctrl-c") { write(T.show); console.log(); Deno.exit(0); }
    else if (key.length === 1 && key.charCodeAt(0) >= 32 && key.charCodeAt(0) < 127) {
      value += key;
      render();
    }
  }
}

// ---------------------------------------------------------------------------
// UI — Confirm with Preview
// ---------------------------------------------------------------------------

async function confirmCreate(
  state: WizardState,
  stepIdx: number,
  total: number,
  previewLines: string[],
): Promise<"yes" | "no" | null> {
  let sel = 0;

  const render = (): void => {
    cls();
    printHeader(state, stepIdx, total);

    // Preview
    console.log(`  ${C.bold}Preview${C.reset} ${C.dim}(first ${previewLines.length} lines)${C.reset}`);
    console.log(`  ${C.dim}${"─".repeat(58)}${C.reset}`);
    for (const line of previewLines) {
      if (line.trimStart().startsWith("//")) {
        console.log(`  ${C.dim}${line}${C.reset}`);
      } else {
        console.log(`  ${line}`);
      }
    }
    console.log(`  ${C.dim}${"─".repeat(58)}${C.reset}`);
    console.log();

    console.log(`  ${C.bold}Create this file?${C.reset}`);
    console.log();
    if (sel === 0) {
      console.log(`  ${C.cyan}❯${C.reset} ${C.bold}${C.green}Yes — create it${C.reset}`);
      console.log(`    ${C.dim}No — cancel${C.reset}`);
    } else {
      console.log(`    ${C.dim}Yes — create it${C.reset}`);
      console.log(`  ${C.cyan}❯${C.reset} ${C.bold}${C.red}No — cancel${C.reset}`);
    }
    console.log();
    console.log(`  ${C.dim}↑↓ select · enter confirm · backspace back${C.reset}`);
  };

  render();
  while (true) {
    const key = await readKey();
    if (key === "up" || key === "down") { sel = sel === 0 ? 1 : 0; render(); }
    else if (key === "enter") return sel === 0 ? "yes" : "no";
    else if (key === "backspace") return null;
    else if (key === "ctrl-c") { write(T.show); console.log(); Deno.exit(0); }
  }
}

// ---------------------------------------------------------------------------
// Derivation Helpers
// ---------------------------------------------------------------------------

function deriveKey(dest: string): string {
  const parts = dest
    .replace(/\.(go|rs|toml|json|jsonc|editorconfig|gitignore|gitmessage|env)$/, "")
    .split("/")
    .filter((p) => p && p !== "." && p !== "..");
  return "B-" + parts.join("-");
}

function deriveTitle(dest: string): string {
  return basename(dest)
    .replace(/\.(go|rs|toml|json|jsonc|editorconfig|gitignore|gitmessage|env)$/, "")
    .replace(/[-_]/g, " ")
    .replace(/\b\w/g, (c) => c.toUpperCase());
}

function capitalize(s: string): string {
  return s.charAt(0).toUpperCase() + s.slice(1);
}

// ---------------------------------------------------------------------------
// Scaffolder Dispatch
// ---------------------------------------------------------------------------

function buildScaffolderArgs(state: WizardState, dryRun: boolean): { script: string; args: string[] } {
  const args: string[] = [];

  if (state.type === "code") {
    args.push(state.format!);
    args.push(state.dest!);
    if (state.role) args.push("--role", state.role);
  } else {
    args.push(state.format!);
    args.push(state.dest!);
  }

  if (state.key) args.push("--key", state.key);
  if (state.title) args.push("--title", state.title);
  if (state.purpose) args.push("--purpose", state.purpose);
  if (state.version !== "a-01.00") args.push("--version", state.version);
  if (dryRun) args.push("--dry-run");

  return {
    script: state.type === "code" ? SCAFFOLD_4BLOCK : SCAFFOLD_3BLOCK,
    args,
  };
}

async function runScaffolder(
  state: WizardState,
  dryRun: boolean,
): Promise<{ stdout: string; stderr: string; code: number }> {
  const { script, args } = buildScaffolderArgs(state, dryRun);

  const cmd = new Deno.Command(Deno.execPath(), {
    args: ["run", "--allow-read", "--allow-write", script, ...args],
    stdout: "piped",
    stderr: "piped",
    cwd: Deno.cwd(),
  });

  const output = await cmd.output();
  return {
    stdout: new TextDecoder().decode(output.stdout),
    stderr: new TextDecoder().decode(output.stderr),
    code: output.code,
  };
}

function stripAnsi(s: string): string {
  return s.replace(/\x1b\[[0-9;]*m/g, "");
}

function extractPreview(stdout: string): string[] {
  const lines = stdout.split("\n");
  const idx = lines.findIndex((l) => l.includes("DRY RUN"));
  if (idx === -1) return lines.slice(0, 20);
  const start = idx + 2; // skip header + blank line
  return lines.slice(start, start + 25);
}

// ---------------------------------------------------------------------------
// Wizard Steps
// ---------------------------------------------------------------------------

/**
 * Metadata collection — 4 sequential fields with internal back navigation.
 * Returns true to advance, false to go back to previous wizard step.
 */
async function stepMetadata(
  state: WizardState,
  stepIdx: number,
  total: number,
): Promise<boolean> {
  // --- Key ---
  state.key = null;
  const defaultKey = deriveKey(state.dest!);
  const keyResult = await textInput("OmniCode key", defaultKey, state, stepIdx, total);
  if (keyResult === null) return false;
  state.key = keyResult;

  // --- Title ---
  state.title = null;
  const defaultTitle = deriveTitle(state.dest!);
  const titleResult = await textInput("File title", defaultTitle, state, stepIdx, total);
  if (titleResult === null) {
    state.key = null;
    return await stepMetadata(state, stepIdx, total); // Restart from key
  }
  state.title = titleResult;

  // --- Purpose ---
  const defaultPurpose = state.type === "code"
    ? `${capitalize(state.role ?? "library")} package for ${state.title}`
    : `${state.title} configuration`;
  const purposeResult = await textInput("Purpose", defaultPurpose, state, stepIdx, total);
  if (purposeResult === null) {
    state.title = null;
    state.key = null;
    return await stepMetadata(state, stepIdx, total); // Restart
  }
  state.purpose = purposeResult;

  // --- Version ---
  const versionResult = await textInput("Version", state.version, state, stepIdx, total);
  if (versionResult === null) {
    state.purpose = null;
    state.title = null;
    state.key = null;
    return await stepMetadata(state, stepIdx, total); // Restart
  }
  state.version = versionResult;

  return true;
}

// ---------------------------------------------------------------------------
// Edit Guidance
// ---------------------------------------------------------------------------

function printEditGuidance(state: WizardState): void {
  console.log(`  ${C.bold}What to edit next:${C.reset}`);
  console.log();

  if (state.type === "code") {
    console.log(`  ${C.cyan}1.${C.reset} ${C.bold}METADATA${C.reset} — Update Pragma (I1-I4) and Metadata (C1-C7)`);
    console.log(`  ${C.cyan}2.${C.reset} ${C.bold}SETUP${C.reset}    — Add your imports, constants, types`);
    console.log(`  ${C.cyan}3.${C.reset} ${C.bold}BODY${C.reset}     — Write your functions (helpers → core → public)`);
    console.log(`  ${C.cyan}4.${C.reset} ${C.bold}CLOSING${C.reset}  — Add tests, benchmarks, main (if executable)`);
  } else {
    console.log(`  ${C.cyan}1.${C.reset} ${C.bold}METADATA${C.reset} — Update identity and context`);
    console.log(`  ${C.cyan}2.${C.reset} ${C.bold}CONTENT${C.reset}  — Add your configuration or data`);
    console.log(`  ${C.cyan}3.${C.reset} ${C.bold}CLOSING${C.reset}  — Update validation rules if needed`);
  }

  console.log();
  console.log(`  ${C.dim}Then verify:${C.reset} cws-struct lint ${state.dest}`);
}

// ---------------------------------------------------------------------------
// Main Wizard Flow
// ---------------------------------------------------------------------------

async function runWizard(): Promise<void> {
  const state: WizardState = {
    type: null, format: null, role: null, dest: null,
    key: null, title: null, purpose: null, version: "a-01.00",
  };

  let stepIdx = 0;

  while (true) {
    const steps = getSteps(state);
    if (stepIdx < 0) break; // Backed out of first step
    if (stepIdx >= steps.length) break; // Past last step (shouldn't happen)

    const stepName = steps[stepIdx]!;
    const total = steps.length;

    switch (stepName) {
      // --- Type ---
      case "type": {
        const result = await selectMenu("What are you creating?", [
          { label: "Code file", value: "code", desc: "4-block: METADATA → SETUP → BODY → CLOSING" },
          { label: "Data file", value: "data", desc: "3-block: METADATA → CONTENT → CLOSING" },
        ], state, stepIdx, total);

        if (result === null) { stepIdx--; break; }
        state.type = result as "code" | "data";
        state.format = null; state.role = null; state.dest = null;
        state.key = null; state.title = null; state.purpose = null;
        stepIdx++;
        break;
      }

      // --- Format / Language ---
      case "format": {
        const options = state.type === "code" ? CODE_LANGUAGES : DATA_FORMATS;
        const title = state.type === "code" ? "What language?" : "What format?";
        const result = await selectMenu(title, options, state, stepIdx, total);

        if (result === null) {
          state.type = null;
          stepIdx--;
          break;
        }
        state.format = result;
        state.role = null; state.dest = null;
        state.key = null; state.title = null;
        stepIdx++;
        break;
      }

      // --- Role (code only) ---
      case "role": {
        const result = await selectMenu("What role?", CODE_ROLES, state, stepIdx, total);

        if (result === null) {
          state.format = null;
          stepIdx--;
          break;
        }
        state.role = result;
        state.dest = null; state.key = null; state.title = null;
        stepIdx++;
        break;
      }

      // --- Path ---
      case "path": {
        const result = await textInput(
          "Where should the file be created?",
          "",
          state, stepIdx, total,
        );

        if (result === null) {
          if (state.type === "code") state.role = null;
          else state.format = null;
          stepIdx--;
          break;
        }
        state.dest = result;
        state.key = null; state.title = null; state.purpose = null;
        stepIdx++;
        break;
      }

      // --- Metadata ---
      case "metadata": {
        const ok = await stepMetadata(state, stepIdx, total);
        if (!ok) {
          state.dest = null;
          stepIdx--;
          break;
        }
        stepIdx++;
        break;
      }

      // --- Confirm ---
      case "confirm": {
        // Get preview via dry-run
        const dryRun = await runScaffolder(state, true);
        const previewLines = extractPreview(dryRun.stdout).map(stripAnsi);

        const result = await confirmCreate(state, stepIdx, total, previewLines);

        if (result === null) {
          // Back — clear metadata
          state.key = null; state.title = null; state.purpose = null;
          stepIdx--;
          break;
        }

        if (result === "no") {
          cls();
          console.log(`\n  ${C.yellow}Cancelled.${C.reset} No file created.\n`);
          return;
        }

        // --- Create the file ---
        cls();
        console.log();
        console.log(`  ${C.dim}${"═".repeat(60)}${C.reset}`);
        console.log(`  ${C.bold}Creating...${C.reset}`);
        console.log(`  ${C.dim}${"═".repeat(60)}${C.reset}`);
        console.log();

        const createResult = await runScaffolder(state, false);

        if (createResult.code !== 0) {
          const errMsg = stripAnsi(createResult.stderr || createResult.stdout).trim();
          console.log(`  ${C.red}Error:${C.reset} ${errMsg}`);
          console.log();
          console.log(`  ${C.dim}Press any key to go back...${C.reset}`);
          await readKey();
          stepIdx--;
          break;
        }

        // --- Success ---
        cls();
        console.log();
        console.log(`  ${C.dim}${"═".repeat(60)}${C.reset}`);
        console.log(`  ${C.bold}${C.green}CWS Creator — Complete!${C.reset}`);
        console.log(`  ${C.dim}${"═".repeat(60)}${C.reset}`);
        console.log();

        // Parse scaffolder output
        const output = stripAnsi(createResult.stdout);
        const outLines = output.split("\n");

        console.log(`  ${C.green}✓${C.reset} Created: ${C.bold}${state.dest}${C.reset}`);

        const healthLine = outLines.find((l) => l.includes("health:"));
        if (healthLine) {
          console.log(`  ${C.green}✓${C.reset} ${healthLine.trim()}`);
        } else {
          const okLine = outLines.find((l) => l.includes("0E") || l.includes("errors"));
          if (okLine) console.log(`  ${C.green}✓${C.reset} ${okLine.trim()}`);
          else console.log(`  ${C.green}✓${C.reset} Verified`);
        }

        console.log();
        printEditGuidance(state);
        console.log();
        return; // Done!
      }
    }
  }

  // Backed out of step 1
  cls();
  console.log(`\n  ${C.dim}Exited.${C.reset}\n`);
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// Help
// ---------------------------------------------------------------------------

function showHelp(): void {
  console.log(`
${C.bold}CWS Creator${C.reset} v${VERSION} — Interactive File Wizard

${C.dim}"Look that thou make them after their pattern." — Exodus 25:40${C.reset}

Interactive wizard for creating new files from seed templates.
Collects metadata, generates from templates, verifies through the engine.
All you do is surgical edits to the content.

${C.bold}Usage:${C.reset}
  create.ts              Launch interactive wizard (requires terminal)
  create.ts --help       Show this help

${C.bold}Non-interactive:${C.reset}
  scaffold-4block.ts     Create code files (Go, Rust)
  scaffold-3block.ts     Create data files (TOML, JSON, dotfiles)

${C.bold}Navigation:${C.reset}
  ↑↓           Select option
  Enter         Confirm / accept default
  Backspace     Go back one step
  Ctrl+C        Cancel and exit
`);
}

// ---------------------------------------------------------------------------
// Entry Point
// ---------------------------------------------------------------------------

async function main(): Promise<void> {
  if (Deno.args.includes("--help") || Deno.args.includes("-h")) {
    showHelp();
    Deno.exit(0);
  }

  if (!Deno.stdin.isTerminal()) {
    console.error(`${C.red}Interactive mode requires a terminal.${C.reset}`);
    console.error("Use scaffold-4block.ts or scaffold-3block.ts for non-interactive use.");
    Deno.exit(1);
  }

  write(T.hide);
  try {
    await runWizard();
  } finally {
    write(T.show);
  }
}

main().catch((err: unknown) => {
  write(T.show);
  console.error(
    `${C.red}create: ${err instanceof Error ? err.message : String(err)}${C.reset}`,
  );
  Deno.exit(2);
});
