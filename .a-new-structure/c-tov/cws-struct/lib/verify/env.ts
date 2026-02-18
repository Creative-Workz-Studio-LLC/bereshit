// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/verify/env.ts
// key:     B-tov-cws-struct-lib-verify-env
// title:   CWS Struct — Environment Verification
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-11
// authors: Nova Dawn (CPI-SI)
// purpose: Verify development environment — tool presence, minimum versions,
//          and configuration. Elevated from setup-dev-environment.sh --verify-only
//          into proper code for reliability and expandability.
//
// biblical_foundation: "Prove all things; hold fast that which is good."
//   — 1 Thessalonians 5:21
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { COLORS } from "../engine/mod.ts";

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

/** How critical is this tool? */
type Requirement = "required" | "recommended" | "optional";

/** Single tool to verify. */
interface ToolSpec {
  name: string;
  category: string;
  command: string;
  args: string[];
  extractVersion: (output: string) => string;
  minVersion?: string;
  requirement: Requirement;
  installHint?: string;
}

/** Result of checking one tool. */
interface ToolResult {
  name: string;
  category: string;
  requirement: Requirement;
  found: boolean;
  version: string;
  meetsMinimum: boolean;
  minVersion?: string;
  installHint?: string;
}

// ---------------------------------------------------------------------------
// Constants — Tool definitions
// ---------------------------------------------------------------------------

const TOOLS: ToolSpec[] = [
  // --- Compilers ---
  {
    name: "GCC",
    category: "Compilers",
    command: "gcc",
    args: ["--version"],
    extractVersion: (o) => o.split("\n")[0]?.match(/(\d+\.\d+\.\d+)/)?.[1] ?? "",
    minVersion: "13.0.0",
    requirement: "required",
    installHint: "pkexec apt install build-essential gcc g++",
  },
  {
    name: "Clang",
    category: "Compilers",
    command: "clang",
    args: ["--version"],
    extractVersion: (o) => o.split("\n")[0]?.match(/(\d+\.\d+\.\d+)/)?.[1] ?? "",
    minVersion: "16.0.0",
    requirement: "required",
    installHint: "pkexec apt install clang",
  },

  // --- Languages (compiled) ---
  {
    name: "Go",
    category: "Languages (compiled)",
    command: "go",
    args: ["version"],
    extractVersion: (o) => o.match(/go(\d+\.\d+(?:\.\d+)?)/)?.[1] ?? "",
    minVersion: "1.22.0",
    requirement: "required",
    installHint: "https://go.dev/dl/",
  },
  {
    name: "Rust",
    category: "Languages (compiled)",
    command: "rustc",
    args: ["--version"],
    extractVersion: (o) => o.match(/(\d+\.\d+\.\d+)/)?.[1] ?? "",
    minVersion: "1.75.0",
    requirement: "required",
    installHint: "curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh",
  },

  // --- Languages (scripting) ---
  {
    name: "Deno",
    category: "Languages (scripting)",
    command: "deno",
    args: ["--version"],
    extractVersion: (o) => o.split("\n")[0]?.match(/(\d+\.\d+\.\d+)/)?.[1] ?? "",
    minVersion: "2.0.0",
    requirement: "recommended",
    installHint: "curl -fsSL https://deno.land/install.sh | sh",
  },
  {
    name: "Node.js",
    category: "Languages (scripting)",
    command: "node",
    args: ["--version"],
    extractVersion: (o) => o.trim().replace(/^v/, ""),
    requirement: "recommended",
    installHint: "Install via nvm or apt",
  },
  {
    name: "Bun",
    category: "Languages (scripting)",
    command: "bun",
    args: ["--version"],
    extractVersion: (o) => o.trim(),
    requirement: "recommended",
    installHint: "curl -fsSL https://bun.sh/install | bash",
  },
  {
    name: "Python",
    category: "Languages (scripting)",
    command: "python3",
    args: ["--version"],
    extractVersion: (o) => o.match(/(\d+\.\d+\.\d+)/)?.[1] ?? "",
    requirement: "recommended",
    installHint: "pkexec apt install python3 python3-pip python3-venv",
  },

  // --- Databases ---
  {
    name: "SQLite",
    category: "Databases",
    command: "sqlite3",
    args: ["--version"],
    extractVersion: (o) => o.split(" ")[0] ?? "",
    requirement: "recommended",
    installHint: "pkexec apt install sqlite3 libsqlite3-dev",
  },
  {
    name: "PostgreSQL",
    category: "Databases",
    command: "psql",
    args: ["--version"],
    extractVersion: (o) => o.match(/(\d+\.\d+)/)?.[1] ?? "",
    requirement: "recommended",
    installHint: "pkexec apt install postgresql-client libpq-dev",
  },
  {
    name: "Redis",
    category: "Databases",
    command: "redis-server",
    args: ["--version"],
    extractVersion: (o) => o.match(/v=(\d+\.\d+\.\d+)/)?.[1] ?? "",
    requirement: "recommended",
    installHint: "pkexec apt install redis-tools redis-server",
  },

  // --- Build tools ---
  {
    name: "Make",
    category: "Build tools",
    command: "make",
    args: ["--version"],
    extractVersion: (o) => o.split("\n")[0]?.match(/(\d+\.\d+(?:\.\d+)?)/)?.[1] ?? "",
    requirement: "required",
    installHint: "pkexec apt install make",
  },
  {
    name: "CMake",
    category: "Build tools",
    command: "cmake",
    args: ["--version"],
    extractVersion: (o) => o.split("\n")[0]?.match(/(\d+\.\d+\.\d+)/)?.[1] ?? "",
    requirement: "recommended",
    installHint: "pkexec apt install cmake",
  },
  {
    name: "Git",
    category: "Build tools",
    command: "git",
    args: ["--version"],
    extractVersion: (o) => o.match(/(\d+\.\d+\.\d+)/)?.[1] ?? "",
    minVersion: "2.30.0",
    requirement: "required",
    installHint: "pkexec apt install git",
  },

  // --- Documentation ---
  {
    name: "Asciidoctor",
    category: "Documentation",
    command: "asciidoctor",
    args: ["--version"],
    extractVersion: (o) => o.split("\n")[0]?.match(/(\d+\.\d+\.\d+)/)?.[1] ?? "",
    requirement: "recommended",
    installHint: "pkexec apt install asciidoctor ruby-asciidoctor-pdf",
  },

  // --- GPU ---
  {
    name: "CUDA",
    category: "GPU (optional)",
    command: "nvcc",
    args: ["--version"],
    extractVersion: (o) => o.match(/release (\d+\.\d+)/)?.[1] ?? "",
    requirement: "optional",
    installHint: "pkexec apt install nvidia-cuda-toolkit nvidia-cuda-dev",
  },
];

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers — version comparison
// ---------------------------------------------------------------------------

/**
 * Compare semver-ish version strings.
 * Returns: negative if a < b, 0 if equal, positive if a > b.
 */
function compareVersions(a: string, b: string): number {
  const pa = a.split(".").map(Number);
  const pb = b.split(".").map(Number);
  const len = Math.max(pa.length, pb.length);

  for (let i = 0; i < len; i++) {
    const va = pa[i] ?? 0;
    const vb = pb[i] ?? 0;
    if (va !== vb) return va - vb;
  }

  return 0;
}

// ---------------------------------------------------------------------------
// Core — check a single tool
// ---------------------------------------------------------------------------

async function checkTool(spec: ToolSpec): Promise<ToolResult> {
  const result: ToolResult = {
    name: spec.name,
    category: spec.category,
    requirement: spec.requirement,
    found: false,
    version: "",
    meetsMinimum: true,
    minVersion: spec.minVersion,
    installHint: spec.installHint,
  };

  try {
    const cmd = new Deno.Command(spec.command, {
      args: spec.args,
      stdout: "piped",
      stderr: "piped",
    });

    const output = await cmd.output();
    const text = new TextDecoder().decode(output.stdout) +
      new TextDecoder().decode(output.stderr);

    result.found = true;
    result.version = spec.extractVersion(text);

    if (spec.minVersion && result.version) {
      result.meetsMinimum = compareVersions(result.version, spec.minVersion) >= 0;
    }
  } catch {
    result.found = false;
  }

  return result;
}

// ---------------------------------------------------------------------------
// Display — format and print results
// ---------------------------------------------------------------------------

function requirementTag(r: Requirement): string {
  switch (r) {
    case "required":
      return "";
    case "recommended":
      return ` ${COLORS.dim}(recommended)${COLORS.reset}`;
    case "optional":
      return ` ${COLORS.dim}(optional)${COLORS.reset}`;
  }
}

function printResult(r: ToolResult): void {
  if (r.found && r.meetsMinimum) {
    const ver = r.version ? ` ${r.version}` : "";
    console.log(
      `  ${COLORS.green}[OK]${COLORS.reset} ${r.name}:${ver}${requirementTag(r.requirement)}`,
    );
  } else if (r.found && !r.meetsMinimum) {
    console.log(
      `  ${COLORS.yellow}[!!]${COLORS.reset} ${r.name}: ${r.version} — ` +
        `minimum ${r.minVersion} required`,
    );
  } else if (r.requirement === "required") {
    console.log(
      `  ${COLORS.red}[XX]${COLORS.reset} ${r.name}: NOT FOUND — REQUIRED`,
    );
    if (r.installHint) {
      console.log(`       Install: ${r.installHint}`);
    }
  } else {
    console.log(
      `  ${COLORS.yellow}[--]${COLORS.reset} ${r.name}: not installed${requirementTag(r.requirement)}`,
    );
  }
}

// ---------------------------------------------------------------------------
// Public — run full environment verification
// ---------------------------------------------------------------------------

export async function verifyEnvironment(verbose: boolean): Promise<boolean> {
  console.log(
    `${COLORS.bold}cws-struct${COLORS.reset} — Environment Verification\n`,
  );
  console.log(
    `${COLORS.dim}"Prove all things; hold fast that which is good." — 1 Thess 5:21${COLORS.reset}\n`,
  );

  // All tool checks run concurrently — each spawns an independent subprocess
  const results = await Promise.all(TOOLS.map((spec) => checkTool(spec)));

  // Group by category and print
  const categories = [...new Set(TOOLS.map((t) => t.category))];

  for (const cat of categories) {
    console.log(`${COLORS.bold}${cat}:${COLORS.reset}`);
    const catResults = results.filter((r) => r.category === cat);

    for (const r of catResults) {
      if (verbose || r.requirement !== "optional" || r.found) {
        printResult(r);
      }
    }
    console.log("");
  }

  // Summary
  const found = results.filter((r) => r.found).length;
  const missing = results.filter((r) => !r.found && r.requirement === "required");
  const belowMin = results.filter((r) => r.found && !r.meetsMinimum);
  const missingRecommended = results.filter(
    (r) => !r.found && r.requirement === "recommended",
  );

  console.log(`${COLORS.bold}Summary:${COLORS.reset}`);
  console.log(`  ${found}/${results.length} tools found`);

  if (missing.length > 0) {
    console.log(
      `  ${COLORS.red}${missing.length} required tool(s) missing: ${missing.map((r) => r.name).join(", ")}${COLORS.reset}`,
    );
  }
  if (belowMin.length > 0) {
    console.log(
      `  ${COLORS.yellow}${belowMin.length} tool(s) below minimum: ${belowMin.map((r) => r.name).join(", ")}${COLORS.reset}`,
    );
  }
  if (missingRecommended.length > 0 && verbose) {
    console.log(
      `  ${COLORS.dim}${missingRecommended.length} recommended tool(s) not installed${COLORS.reset}`,
    );
  }

  if (missing.length === 0 && belowMin.length === 0) {
    console.log(
      `\n${COLORS.green}Environment ready for development.${COLORS.reset}`,
    );
    return true;
  }

  console.log(
    `\n${COLORS.red}Environment has issues. Fix required tools before proceeding.${COLORS.reset}`,
  );
  console.log(
    `${COLORS.dim}For full setup: .a-new-structure/c-tov/scripts/setup-dev-environment.sh${COLORS.reset}`,
  );

  return false;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Environment verification elevated from shell to code. The shell script
// installs; this module verifies. Separation of concerns — the script
// does what only shell can do (apt install), the tool does what code
// does better (version comparison, structured output, expandability).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
