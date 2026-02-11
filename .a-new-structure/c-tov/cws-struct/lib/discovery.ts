// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/discovery.ts
// key:     B-tov-cws-struct-lib-discovery
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
import type { FormatHandler } from "./types.ts";

// ============================================================================
// BODY
// ============================================================================

/**
 * Discover files matching a format handler's extensions/basenames.
 *
 * Accepts an array of target paths (files or directories).
 * Returns absolute paths sorted for consistent output.
 */
export async function discoverFiles(
  targets: string[],
  handler: FormatHandler,
): Promise<string[]> {
  const files: string[] = [];
  const maxDepth = handler.maxDepth ?? 3;

  for (const target of targets) {
    const resolved = resolve(target);

    try {
      const stat = await Deno.stat(resolved);

      if (stat.isFile && matchesHandler(resolved, handler)) {
        files.push(resolved);
      } else if (stat.isDirectory) {
        for await (const entry of walk(resolved, { maxDepth })) {
          if (entry.isFile && matchesHandler(entry.path, handler)) {
            files.push(entry.path);
          }
        }
      }
    } catch {
      console.error(`Cannot access: ${target}`);
    }
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
