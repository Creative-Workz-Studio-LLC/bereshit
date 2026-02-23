// ============================================================================
// METADATA
// ============================================================================
//
// file:    src/types.ts
// key:     B-omni-tooling-vscode-language-types
// title:   OmniCode Language Extension — cws-struct Output Types
// type:    Code (TypeScript)
// version: a-01.00
// created: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: TypeScript type definitions mirroring cws-struct JSON output.
//          Authoritative source: cws-struct/lib/foundation/types.ts
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// cws-struct JSON output types
// ---------------------------------------------------------------------------

/** Top-level JSON output from `cws-struct lint --json`. */
export interface CwsStructOutput {
    tool: string;
    version: string;
    files: CwsStructFile[];
    totals: {
        files: number;
        errors: number;
        warnings: number;
        infos: number;
    };
}

/** Per-file lint results. */
export interface CwsStructFile {
    file: string;
    errors: number;
    warnings: number;
    infos: number;
    health: HealthScore | null;
    results: CwsStructResult[];
    pragma?: string;
}

/** Single lint finding. */
export interface CwsStructResult {
    severity: 'error' | 'warn' | 'info';
    rule: string;
    message: string;
    /** 1-based line number (null for file-level findings). */
    line: number | null;
    /** Optional fix suggestion. */
    fix: FixSuggestion | null;
}

// ---------------------------------------------------------------------------
// Fix suggestion types
// ---------------------------------------------------------------------------

/** Suggested correction for a lint finding. */
export interface FixSuggestion {
    description: string;
    toml?: string;
    location?: string;
    actions?: FixAction[];
}

/** Single atomic fix action. */
export interface FixAction {
    type: 'remove' | 'insert' | 'replace';
    /** For remove/replace: first line (1-based). */
    startLine?: number;
    /** For remove/replace: last line (1-based, inclusive). */
    endLine?: number;
    /** For insert: line after which to insert (1-based). */
    afterLine?: number;
    /** For insert/replace: lines of content. */
    content?: string[];
}

// ---------------------------------------------------------------------------
// Health scoring types
// ---------------------------------------------------------------------------

/** File-level health score. */
export interface HealthScore {
    /** Overall: -100 (shavar) to +100 (tov). 0 = yashar. */
    total: number;
    blocks: BlockScore[];
    /** Flattened container scores (from all blocks). */
    containers?: ContainerScore[];
    totalActions: number;
    alignedCount: number;
    neutralCount: number;
    misalignedCount: number;
}

/** Per-block health score. */
export interface BlockScore {
    block: string;
    containers: ContainerScore[];
    score: number;
}

/** Per-container (section) health score. */
export interface ContainerScore {
    section: string;
    block: string;
    total: number;
    aligned: number;
    neutral: number;
    misaligned: number;
    score: number;
}

// ============================================================================
// CLOSING
// ============================================================================
