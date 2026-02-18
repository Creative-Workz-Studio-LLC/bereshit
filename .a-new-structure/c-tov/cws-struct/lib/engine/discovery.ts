// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/discovery.ts
// key:     B-tov-cws-struct-lib-engine-discovery
// title:   CWS Struct — File Discovery
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-11
// authors: Nova Dawn (CPI-SI)
// purpose: Walk directories, match files by extension or basename, resolve
//          targets. Shared by all format handlers instead of each reimplementing
//          the same walk + filter logic.
//
// biblical_foundation: "Seek, and ye shall find." — Matthew 7:7
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { walk } from "@std/fs/walk";
import { resolve, basename, extname, relative } from "@std/path";
import type { FormatHandler } from "../foundation/mod.ts";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

/**
 * Maximum number of files to discover per run.
 * Prevents memory exhaustion on excessively large directory trees.
 */
const MAX_DISCOVERED_FILES = 10_000;

/**
 * Regex patterns for directories always excluded from walking — version control,
 * dependencies, build artifacts. These are never structural source files.
 * Used with @std/fs/walk's `skip` option which takes RegExp[].
 */
const EXCLUDED_DIR_PATTERNS: RegExp[] = [
  /[/\\]\.git$/,
  /[/\\]node_modules$/,
  /[/\\]\.cache$/,
  /[/\\]__pycache__$/,
  /[/\\]target$/,   // Rust/Cargo build output
  /[/\\]bin$/,       // compiled binaries
];

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Path safety
// ---------------------------------------------------------------------------

/**
 * Resolve a target path and validate it is safe to access.
 *
 * Guards against:
 * - Null bytes in paths (injection via C-string termination)
 *
 * Resolves symlinks to get canonical paths. Does NOT enforce cwd containment
 * because this is a CLI tool — users explicitly choose which files to lint,
 * and the tool is meant to operate across the entire project tree, not just
 * from within its own directory.
 *
 * Returns the real (symlink-resolved) absolute path, or null if unsafe.
 */
async function safePath(target: string): Promise<string | null> {
  // Reject null bytes — prevents path injection in underlying C APIs
  if (target.includes("\0")) {
    console.error(`Rejected: path contains null byte`);
    return null;
  }

  const resolved = resolve(target);

  try {
    // Resolve symlinks to get the real physical path
    return await Deno.realPath(resolved);
  } catch {
    // Path doesn't exist or is not accessible — let the caller handle stat errors
    return resolved;
  }
}

// ---------------------------------------------------------------------------
// Discovery
// ---------------------------------------------------------------------------

/**
 * Discover files matching a format handler's extensions/basenames.
 *
 * Accepts an array of target paths (files or directories).
 * Returns absolute paths sorted for consistent output.
 *
 * Security:
 * - Resolves symlinks and validates paths stay within cwd
 * - Excludes common non-source directories (.git, node_modules, target)
 * - Caps total discovered files at MAX_DISCOVERED_FILES
 * - Rejects paths containing null bytes
 */
export async function discoverFiles(
  targets: string[],
  handler: FormatHandler,
): Promise<string[]> {
  const files: string[] = [];
  const maxDepth = handler.maxDepth ?? 3;

  for (const target of targets) {
    // Path safety check — resolve symlinks, reject null bytes
    const safe = await safePath(target);
    if (safe === null) continue;

    try {
      const stat = await Deno.stat(safe);

      if (stat.isFile && matchesHandler(safe, handler)) {
        files.push(safe);
      } else if (stat.isDirectory) {
        for await (const entry of walk(safe, {
          maxDepth,
          skip: EXCLUDED_DIR_PATTERNS,
        })) {
          if (entry.isFile && matchesHandler(entry.path, handler)) {
            files.push(entry.path);

            // Cap discovered files to prevent memory exhaustion
            if (files.length >= MAX_DISCOVERED_FILES) {
              console.error(
                `Warning: File limit reached (${MAX_DISCOVERED_FILES}). Some files may be skipped.`,
              );
              break;
            }
          }
        }
      }
    } catch {
      console.error(`Cannot access: ${target}`);
    }

    if (files.length >= MAX_DISCOVERED_FILES) break;
  }

  files.sort();
  return files;
}

/**
 * Check if a file matches a handler's extensions or basenames.
 */
function matchesHandler(filePath: string, handler: FormatHandler): boolean {
  const ext = extname(filePath);
  const base = basename(filePath);

  // Extension match (e.g., ".toml", ".json", ".jsonc")
  if (ext && handler.extensions.includes(ext)) {
    return true;
  }

  // Basename match (e.g., ".gitignore", ".editorconfig" — root dotfiles)
  if (handler.basenames?.includes(base)) {
    return true;
  }

  return false;
}

/**
 * Convert absolute paths to relative (for display).
 */
export function relativePaths(
  files: string[],
  cwd?: string,
): string[] {
  const base = cwd ?? Deno.cwd();
  return files.map((f) => relative(base, f));
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Discovery is the first step — find the files, then lint them.
// "Seek, and ye shall find." — Matthew 7:7
// ============================================================================
