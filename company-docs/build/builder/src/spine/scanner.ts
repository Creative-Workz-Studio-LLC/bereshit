/**
 * CWS Manual Builder — Spine Scanner
 *
 * Walks the book/ directory tree and discovers the complete structure:
 * scopes (A-D), parts, articles, sections. Pure filesystem discovery —
 * no validation logic, just reads what's there.
 *
 * Maps to Makefile's spine targets and PART_DIRS/PART_IDS discovery.
 *
 * "Write the vision, and make it plain upon tables,
 *  that he may run that readeth it." — Habakkuk 2:2
 */

import { existsSync } from 'fs';
import { readdir, stat } from 'fs/promises';
import { join, basename } from 'path';
import {
  SCOPE_DIRS,
  SCOPE_NAMES,
  type SpineScope,
  type SpineTree,
  type SpineScopeNode,
  type SpinePart,
  type SpineArticle,
  type SpineSection,
} from './types.js';

// =============================================================================
// Constants
// =============================================================================

/** Scopes in spine order */
const SCOPES: SpineScope[] = ['A', 'B', 'C', 'D'];

/** Pattern for Part directories: Part-{N}-{name} */
const PART_PATTERN = /^Part-(\d+)-(.+)$/;

/** Pattern for Article directories: Article-{NN}-{name} */
const ARTICLE_PATTERN = /^Article-(\d+)-(.+)$/;

/** Pattern for Section files: {LETTER}-{NAME}.adoc */
const SECTION_PATTERN = /^([A-Z])-(.+)\.adoc$/;

// =============================================================================
// Section Scanner
// =============================================================================

/**
 * Scan a directory for .adoc section files.
 * Returns sections sorted by letter.
 */
async function scanSections(dirPath: string): Promise<SpineSection[]> {
  const sections: SpineSection[] = [];

  let entries: string[];
  try {
    entries = await readdir(dirPath);
  } catch {
    return sections;
  }

  for (const entry of entries.sort()) {
    if (!entry.endsWith('.adoc')) continue;

    const filePath = join(dirPath, entry);
    const fileStat = await stat(filePath);
    if (!fileStat.isFile()) continue;

    const match = SECTION_PATTERN.exec(entry);
    if (match?.[1] && match[2]) {
      sections.push({
        letter: match[1],
        name: match[2],
        filename: entry,
        path: filePath,
        size: fileStat.size,
      });
    } else {
      // Non-conforming .adoc file — still include it
      sections.push({
        letter: entry.charAt(0),
        name: entry.replace(/\.adoc$/, ''),
        filename: entry,
        path: filePath,
        size: fileStat.size,
      });
    }
  }

  return sections;
}

// =============================================================================
// Article Scanner
// =============================================================================

/**
 * Scan a Part directory for Article subdirectories.
 * Returns articles sorted by ID.
 */
async function scanArticles(partPath: string): Promise<SpineArticle[]> {
  const articles: SpineArticle[] = [];

  let entries: string[];
  try {
    entries = await readdir(partPath);
  } catch {
    return articles;
  }

  for (const entry of entries.sort()) {
    const entryPath = join(partPath, entry);
    const entryStat = await stat(entryPath);
    if (!entryStat.isDirectory()) continue;

    const match = ARTICLE_PATTERN.exec(entry);
    if (match?.[1] && match[2]) {
      const sections = await scanSections(entryPath);
      articles.push({
        id: match[1],
        name: match[2],
        dirname: entry,
        path: entryPath,
        sections,
      });
    }
  }

  return articles;
}

// =============================================================================
// Part Scanner
// =============================================================================

/**
 * Scan the B-content directory for Part subdirectories.
 * Returns parts sorted by ID.
 */
async function scanParts(contentPath: string): Promise<SpinePart[]> {
  const parts: SpinePart[] = [];

  let entries: string[];
  try {
    entries = await readdir(contentPath);
  } catch {
    return parts;
  }

  for (const entry of entries.sort()) {
    const entryPath = join(contentPath, entry);
    const entryStat = await stat(entryPath);
    if (!entryStat.isDirectory()) continue;

    const match = PART_PATTERN.exec(entry);
    if (match?.[1] && match[2]) {
      const articles = await scanArticles(entryPath);
      parts.push({
        id: match[1],
        name: match[2],
        dirname: entry,
        path: entryPath,
        articles,
      });
    }
  }

  return parts;
}

// =============================================================================
// Scope Scanner
// =============================================================================

/**
 * Scan a single scope directory.
 */
async function scanScope(bookDir: string, scope: SpineScope): Promise<SpineScopeNode> {
  const dirName = SCOPE_DIRS[scope];
  const scopePath = join(bookDir, dirName);

  const node: SpineScopeNode = {
    scope,
    name: SCOPE_NAMES[scope],
    dirname: dirName,
    path: scopePath,
    parts: [],
    files: [],
  };

  if (!existsSync(scopePath)) {
    return node;
  }

  if (scope === 'B') {
    // Content scope has Parts
    node.parts = await scanParts(scopePath);
  } else {
    // Other scopes have loose files
    node.files = await scanSections(scopePath);
  }

  return node;
}

// =============================================================================
// Full Spine Scanner
// =============================================================================

/**
 * Scan the entire book directory and build the spine tree.
 * Pure discovery — reads what exists without judging conformance.
 */
export async function scanSpine(bookDir: string): Promise<SpineTree> {
  const scopes = {} as Record<SpineScope, SpineScopeNode>;

  for (const scope of SCOPES) {
    scopes[scope] = await scanScope(bookDir, scope);
  }

  return {
    bookDir,
    scopes,
    hasAssets: existsSync(join(bookDir, '00-assets')),
    hasMaster: existsSync(join(bookDir, 'book.adoc')),
    scannedAt: new Date(),
  };
}

/**
 * Get all Part IDs from the spine (matches Makefile's PART_IDS).
 */
export function getPartIds(tree: SpineTree): string[] {
  return tree.scopes.B.parts.map(p => p.id);
}

/**
 * Get a specific Part by ID.
 */
export function getPart(tree: SpineTree, partId: string): SpinePart | undefined {
  return tree.scopes.B.parts.find(p => p.id === partId);
}

/**
 * Get the scope directory name for display.
 */
export function getScopeDir(scope: SpineScope): string {
  return SCOPE_DIRS[scope];
}

/**
 * Get scope name from a directory basename.
 */
export function scopeFromDir(dirName: string): SpineScope | undefined {
  const letter = dirName.charAt(0) as SpineScope;
  return SCOPES.includes(letter) ? letter : undefined;
}

/**
 * Get the book directory path from any path within the book.
 */
export function resolveBookDir(inputPath: string): string {
  // Strip trailing slash
  const cleanPath = inputPath.replace(/\/$/, '');
  // If it ends with /book, use it directly
  if (basename(cleanPath) === 'book') {
    return cleanPath;
  }
  // Otherwise assume it IS the book dir
  return cleanPath;
}
