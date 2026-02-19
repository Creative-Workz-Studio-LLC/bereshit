// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/studio/serve.ts
// key:     B-tov-cws-struct-lib-studio-serve
// title:   CWS Studio — Embedded Web Server
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-18
// authors: Nova Dawn (CPI-SI)
// purpose: HTTP server module that wraps both cws-struct engine (lint/validate/
//          transform) and cwe-creator-tools (scaffold/create) into a web API.
//          Serves the static frontend from public/ and provides REST endpoints.
//
//          Called by mod.ts when the user runs `cws-struct studio`.
//          Not a standalone script — an importable module.
//
// biblical_foundation: "Except the LORD build the house, they labour in vain
//   that build it." — Psalm 127:1
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { LintSummary } from "../foundation/mod.ts";
import { summarize } from "../foundation/mod.ts";
import {
  discoverAllFiles,
  listFormatDetails,
  getFormat,
  detectFormat,
} from "../engine/mod.ts";

import { resolve, relative, dirname, fromFileUrl, extname } from "@std/path";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const PUBLIC_DIR = resolve(dirname(fromFileUrl(import.meta.url)), "../../public");
const TOOLS_DIR = resolve(dirname(fromFileUrl(import.meta.url)), "../../../cwe-creator-tools");
const TOOLS_CONFIG = resolve(TOOLS_DIR, "deno.jsonc");

/** MIME types for static file serving. */
const MIME_TYPES: Record<string, string> = {
  ".html": "text/html; charset=utf-8",
  ".css": "text/css; charset=utf-8",
  ".js": "application/javascript; charset=utf-8",
  ".json": "application/json; charset=utf-8",
  ".svg": "image/svg+xml",
  ".png": "image/png",
  ".ico": "image/x-icon",
  ".woff2": "font/woff2",
};

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Core Operations — Lint, Create, Seeds
// ---------------------------------------------------------------------------

/**
 * Lint a file or directory, returning structured summaries.
 * Uses the engine directly — no subprocess needed.
 */
async function lintPath(
  targetPath: string,
  options: { verbose?: boolean; errorsOnly?: boolean },
): Promise<LintSummary[]> {
  const absPath = resolve(targetPath);
  const filesByFormat = await discoverAllFiles([absPath]);
  const summaries: LintSummary[] = [];

  for (const [formatName, files] of filesByFormat) {
    const handler = getFormat(formatName);
    if (!handler) continue;

    for (const file of files) {
      const results = await handler.lint(file);
      const health = handler.computeHealth
        ? await handler.computeHealth(file, results)
        : undefined;

      const summary = summarize(relative(Deno.cwd(), file), results, health);

      if (options.errorsOnly && summary.errors === 0) continue;
      if (!options.verbose && summary.errors === 0 && summary.warnings === 0 && summary.infos === 0) continue;

      summaries.push(summary);
    }
  }

  return summaries;
}

/**
 * Create a file by dispatching to the appropriate scaffolder subprocess.
 */
async function createFile(body: Record<string, unknown>): Promise<{
  success: boolean;
  path?: string;
  output?: string;
  preview?: string;
  verification?: LintSummary;
  error?: string;
}> {
  const isCode = body.type === "code";
  const scriptName = isCode ? "scaffold-4block.ts" : "scaffold-3block.ts";
  const scriptPath = resolve(TOOLS_DIR, scriptName);

  const scriptArgs: string[] = [];
  scriptArgs.push(String(body.format));
  scriptArgs.push(String(body.dest));

  if (isCode && body.role) scriptArgs.push("--role", String(body.role));
  if (body.key) scriptArgs.push("--key", String(body.key));
  if (body.title) scriptArgs.push("--title", String(body.title));
  if (body.purpose) scriptArgs.push("--purpose", String(body.purpose));
  if (body.version && body.version !== "a-01.00") {
    scriptArgs.push("--version", String(body.version));
  }
  if (body.dryRun) scriptArgs.push("--dry-run");
  scriptArgs.push("--no-verify");

  const cmd = new Deno.Command(Deno.execPath(), {
    args: [
      "run",
      "--allow-read",
      "--allow-write",
      `--config=${TOOLS_CONFIG}`,
      scriptPath,
      ...scriptArgs,
    ],
    stdout: "piped",
    stderr: "piped",
  });

  const output = await cmd.output();
  const stdout = new TextDecoder().decode(output.stdout);
  const stderr = new TextDecoder().decode(output.stderr);

  if (!output.success) {
    return { success: false, error: stderr || stdout };
  }

  // Verify the created file via engine (skip for dry-run)
  let verification: LintSummary | undefined;
  if (!body.dryRun) {
    const destPath = resolve(String(body.dest));
    const formatName = detectFormat(destPath);
    if (formatName) {
      const handler = getFormat(formatName);
      if (handler) {
        const results = await handler.lint(destPath);
        const health = handler.computeHealth
          ? await handler.computeHealth(destPath, results)
          : undefined;
        verification = summarize(relative(Deno.cwd(), destPath), results, health);
      }
    }
  }

  return {
    success: true,
    path: String(body.dest),
    output: stdout,
    preview: body.dryRun ? stdout : undefined,
    verification,
  };
}

/** List available seed templates by category. */
function listSeeds(): Record<string, Array<{ name: string; format: string; role?: string; description: string }>> {
  return {
    code: [
      { name: "Go Library", format: "go", role: "library", description: "Go library package (4-block)" },
      { name: "Go Executable", format: "go", role: "executable", description: "Go main package (4-block)" },
      { name: "Go Test", format: "go", role: "test", description: "Go test file (4-block)" },
      { name: "Rust Library", format: "rust", role: "library", description: "Rust lib crate (4-block)" },
      { name: "Rust Executable", format: "rust", role: "executable", description: "Rust binary crate (4-block)" },
      { name: "Rust Test", format: "rust", role: "test", description: "Rust test file (4-block)" },
    ],
    data: [
      { name: "TOML Config", format: "toml", description: "TOML configuration (3-block)" },
      { name: "JSONC Config", format: "jsonc", description: "JSONC configuration (3-block)" },
      { name: "JSON Data", format: "json", description: "JSON data file (3-block)" },
      { name: "EditorConfig", format: "editorconfig", description: "EditorConfig dotfile (3-block)" },
      { name: "Gitignore", format: "gitignore", description: "Gitignore dotfile (3-block)" },
      { name: "Git Message", format: "gitmessage", description: "Git commit template (3-block)" },
      { name: "Environment", format: "env", description: "Environment file (3-block)" },
    ],
  };
}

// ---------------------------------------------------------------------------
// API Router
// ---------------------------------------------------------------------------

async function handleApi(req: Request, url: URL): Promise<Response> {
  const path = url.pathname;

  // --- GET routes ---

  if (path === "/api/health" && req.method === "GET") {
    return Response.json({ status: "ok", version: "0.1.0", engine: "cws-struct" });
  }

  if (path === "/api/formats" && req.method === "GET") {
    return Response.json({ formats: listFormatDetails() });
  }

  if (path === "/api/seeds" && req.method === "GET") {
    return Response.json(listSeeds());
  }

  // --- POST routes ---

  if (path === "/api/lint" && req.method === "POST") {
    try {
      const body = await req.json() as Record<string, unknown>;
      if (!body.path || typeof body.path !== "string") {
        return Response.json({ error: "Missing required field: path" }, { status: 400 });
      }

      const summaries = await lintPath(body.path as string, {
        verbose: body.verbose as boolean | undefined,
        errorsOnly: body.errorsOnly as boolean | undefined,
      });

      const totals = {
        files: summaries.length,
        errors: summaries.reduce((s, f) => s + f.errors, 0),
        warnings: summaries.reduce((s, f) => s + f.warnings, 0),
        info: summaries.reduce((s, f) => s + f.infos, 0),
      };

      return Response.json({ results: summaries, totals });
    } catch (err) {
      return Response.json(
        { error: err instanceof Error ? err.message : String(err) },
        { status: 400 },
      );
    }
  }

  if (path === "/api/create" && req.method === "POST") {
    try {
      const body = await req.json() as Record<string, unknown>;

      if (!body.type || !body.format || !body.dest) {
        return Response.json(
          { error: "Missing required fields: type, format, dest" },
          { status: 400 },
        );
      }

      const result = await createFile(body);
      return Response.json(result, { status: result.success ? 200 : 400 });
    } catch (err) {
      return Response.json(
        { error: err instanceof Error ? err.message : String(err) },
        { status: 400 },
      );
    }
  }

  return Response.json({ error: "Not found" }, { status: 404 });
}

// ---------------------------------------------------------------------------
// Static File Server
// ---------------------------------------------------------------------------

async function serveStatic(pathname: string): Promise<Response> {
  if (pathname === "/" || pathname === "") pathname = "/index.html";

  const filePath = resolve(PUBLIC_DIR, "." + pathname);

  // Security: prevent directory traversal
  if (!filePath.startsWith(PUBLIC_DIR)) {
    return new Response("Forbidden", { status: 403 });
  }

  try {
    const file = await Deno.readFile(filePath);
    const ext = extname(filePath);
    const contentType = MIME_TYPES[ext] ?? "application/octet-stream";
    return new Response(file, {
      headers: {
        "Content-Type": contentType,
        "Cache-Control": "no-cache",
      },
    });
  } catch {
    return new Response("Not found", { status: 404 });
  }
}

// ---------------------------------------------------------------------------
// Request Handler
// ---------------------------------------------------------------------------

async function handler(req: Request): Promise<Response> {
  const url = new URL(req.url);

  if (url.pathname.startsWith("/api/")) {
    return handleApi(req, url);
  }

  return serveStatic(url.pathname);
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// Public API — called by mod.ts
// ---------------------------------------------------------------------------

/**
 * Start the CWS Studio web server.
 * Serves the web frontend + REST API wrapping the cws-struct engine.
 */
export function startStudio(port: number): void {
  console.log("");
  console.log("  CWS Studio");
  console.log("  ─────────────────────────");
  console.log(`  Local:   http://localhost:${port}`);
  console.log("  Engine:  cws-struct (all handlers registered)");
  console.log("  Tools:   cwe-creator-tools (scaffold-3block, scaffold-4block)");
  console.log("");

  Deno.serve({ port }, handler);
}

// "Except the LORD build the house, they labour in vain." — Psalm 127:1
// ============================================================================
