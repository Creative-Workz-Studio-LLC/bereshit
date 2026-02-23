// ============================================================================
// METADATA
// ============================================================================
//
// file:    src/config.ts
// key:     B-omni-tooling-vscode-language-config
// title:   OmniCode Language Extension — Settings Management
// type:    Code (TypeScript)
// version: a-01.00
// created: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: Read and manage extension configuration from VSCode settings.
//          Resolves cws-struct binary path with fallback chain.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

// ============================================================================
// BODY
// ============================================================================

/** Extension configuration. */
export interface OmniCodeConfig {
    /** Path to cws-struct binary. */
    cwsStructPath: string;
    /** Lint on file save. */
    lintOnSave: boolean;
    /** Lint when opening files. */
    lintOnOpen: boolean;
    /** Ternary lint policy. */
    policy: 'strict' | 'balanced' | 'growth';
    /** Debounce delay in milliseconds. */
    debounceMs: number;
}

/** Default CPI-SI binary location. */
const DEFAULT_BINARY = path.join(
    process.env.HOME || '~',
    '.local', 'share', 'claude', 'bin', 'cws-struct',
);

/**
 * Read current extension configuration from VSCode settings.
 */
export function getConfig(): OmniCodeConfig {
    const cfg = vscode.workspace.getConfiguration('omnicode');

    return {
        cwsStructPath: resolveBinaryPath(cfg.get<string>('cwsStructPath', 'cws-struct')),
        lintOnSave: cfg.get<boolean>('lintOnSave', true),
        lintOnOpen: cfg.get<boolean>('lintOnOpen', true),
        policy: cfg.get<'strict' | 'balanced' | 'growth'>('policy', 'balanced'),
        debounceMs: cfg.get<number>('debounceMs', 300),
    };
}

/**
 * Resolve cws-struct binary path.
 *
 * Resolution order:
 * 1. Explicit path from settings (if absolute and exists)
 * 2. PATH lookup (which/where)
 * 3. Default CPI-SI location (~/.local/share/claude/bin/cws-struct)
 * 4. Fall back to the setting value as-is (let spawn fail with clear error)
 */
function resolveBinaryPath(setting: string): string {
    // Absolute path — check if it exists
    if (path.isAbsolute(setting) && fs.existsSync(setting)) {
        return setting;
    }

    // Default CPI-SI location
    if (fs.existsSync(DEFAULT_BINARY)) {
        return DEFAULT_BINARY;
    }

    // Fall back to whatever was configured (PATH resolution happens at spawn time)
    return setting;
}

// ---------------------------------------------------------------------------
// Format detection — map file extensions to cws-struct format names
// ---------------------------------------------------------------------------

/** Map of file extension/basename to cws-struct format argument. */
const FORMAT_MAP: Record<string, string> = {
    '.go': 'go',
    '.rs': 'rust',
    '.toml': 'toml',
    '.json': 'json',
    '.jsonc': 'json',
    '.omni': 'omni',
    '.ofd': 'ofd',
};

/** Basenames that map to formats. */
const BASENAME_MAP: Record<string, string> = {
    'Makefile': 'makefile',
    'GNUmakefile': 'makefile',
    'makefile': 'makefile',
};

/**
 * Detect cws-struct format from a document URI.
 * Returns undefined for unsupported file types.
 */
export function detectFormat(uri: vscode.Uri): string | undefined {
    const ext = path.extname(uri.fsPath);
    const base = path.basename(uri.fsPath);

    // Check basename first (Makefile, dotfiles)
    if (BASENAME_MAP[base]) {
        return BASENAME_MAP[base];
    }

    // Check extension
    if (FORMAT_MAP[ext]) {
        return FORMAT_MAP[ext];
    }

    // Dotfiles (no extension or starts with .)
    if (base.startsWith('.') && !ext) {
        return 'dotfiles';
    }

    return undefined;
}

// ============================================================================
// CLOSING
// ============================================================================
