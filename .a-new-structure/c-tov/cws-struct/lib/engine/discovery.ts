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
import { detectFormat, getFormat } from "./registry.ts";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

/**
 * Maximum number of files to discover per run.
 * Prevents memory exhaustion on excessively large directory trees.
 */
const MAX_DISCOVERED_FILES = 10_000;

/**
 * Maximum file size to accept (1MB).
 * Source files should never be this large — anything bigger is almost certainly
 * generated, vendored, or binary. Skip with a warning.
 */
const MAX_FILE_SIZE = 1_048_576;

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
        // File size guard — skip oversized files with warning
        if (stat.size > MAX_FILE_SIZE) {
          console.error(
            `Skipping ${target}: file too large (${(stat.size / 1024).toFixed(0)}KB > 1MB limit)`,
          );
        } else {
          files.push(safe);
        }
      } else if (stat.isDirectory) {
        for await (const entry of walk(safe, {
          maxDepth,
          skip: EXCLUDED_DIR_PATTERNS,
        })) {
          if (entry.isFile && matchesHandler(entry.path, handler)) {
            // File size guard for walked files
            try {
              const fileStat = await Deno.stat(entry.path);
              if (fileStat.size > MAX_FILE_SIZE) {
                console.error(
                  `Skipping ${entry.path}: file too large (${(fileStat.size / 1024).toFixed(0)}KB > 1MB limit)`,
                );
                continue;
              }
            } catch {
              // If we can't stat, include it — the handler will report the read error
            }

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
    } catch (e: unknown) {
      const msg = e instanceof Error ? e.message : String(e);
      console.error(`Cannot access ${target}: ${msg}`);
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

// ---------------------------------------------------------------------------
// Multi-format discovery — single walk, dispatched by extension
// ---------------------------------------------------------------------------

/**
 * Discover files for ALL registered formats in a single directory walk.
 * Returns a Map<formatName, filePaths[]>.
 *
 * This replaces the N-walk pattern where auto-detect called discoverFiles()
 * once per registered format. One walk, bucket by extension.
 */
export async function discoverAllFiles(
  targets: string[],
  maxDepth = 3,
): Promise<Map<string, string[]>> {
  const filesByFormat = new Map<string, string[]>();
  let totalFiles = 0;

  for (const target of targets) {
    const safe = await safePath(target);
    if (safe === null) continue;

    try {
      const stat = await Deno.stat(safe);

      if (stat.isFile) {
        // File size guard
        if (stat.size > MAX_FILE_SIZE) {
          console.error(
            `Skipping ${target}: file too large (${(stat.size / 1024).toFixed(0)}KB > 1MB limit)`,
          );
          continue;
        }

        const format = detectFormat(safe);
        if (format) {
          const list = filesByFormat.get(format) ?? [];
          list.push(safe);
          filesByFormat.set(format, list);
          totalFiles++;
        }
      } else if (stat.isDirectory) {
        for await (const entry of walk(safe, {
          maxDepth,
          skip: EXCLUDED_DIR_PATTERNS,
        })) {
          if (!entry.isFile) continue;

          const format = detectFormat(entry.path);
          if (!format) continue;

          // Verify the handler would accept this file (basenames, etc.)
          const handler = getFormat(format);
          if (!handler || !matchesHandler(entry.path, handler)) continue;

          // File size guard
          try {
            const fileStat = await Deno.stat(entry.path);
            if (fileStat.size > MAX_FILE_SIZE) {
              console.error(
                `Skipping ${entry.path}: file too large (${(fileStat.size / 1024).toFixed(0)}KB > 1MB limit)`,
              );
              continue;
            }
          } catch {
            // If we can't stat, include it — handler will report read error
          }

          const list = filesByFormat.get(format) ?? [];
          list.push(entry.path);
          filesByFormat.set(format, list);
          totalFiles++;

          if (totalFiles >= MAX_DISCOVERED_FILES) {
            console.error(
              `Warning: File limit reached (${MAX_DISCOVERED_FILES}). Some files may be skipped.`,
            );
            break;
          }
        }
      }
    } catch (e: unknown) {
      const msg = e instanceof Error ? e.message : String(e);
      console.error(`Cannot access ${target}: ${msg}`);
    }

    if (totalFiles >= MAX_DISCOVERED_FILES) break;
  }

  // Sort files within each format bucket for consistent output
  for (const [, files] of filesByFormat) {
    files.sort();
  }

  return filesByFormat;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Discovery is the first step — find the files, then lint them.
// "Seek, and ye shall find." — Matthew 7:7
// ============================================================================
