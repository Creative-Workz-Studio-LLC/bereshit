// ============================================================================
// METADATA
// ============================================================================
//
// file:    src/diagnostics.ts
// key:     B-omni-tooling-vscode-language-diagnostics
// title:   OmniCode Language Extension — Diagnostics Engine
// type:    Code (TypeScript)
// version: a-01.00
// created: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: Spawn cws-struct, parse JSON output, map to VSCode diagnostics.
//          Debounced on save. Tracks results per-file for code actions
//          and status bar integration.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import * as vscode from 'vscode';
import * as cp from 'child_process';
import * as path from 'path';
import { OmniCodeConfig, detectFormat } from './config';
import { CwsStructOutput, CwsStructFile, CwsStructResult, HealthScore } from './types';

// ============================================================================
// BODY
// ============================================================================

/** Severity mapping: cws-struct → VSCode. */
const SEVERITY_MAP: Record<string, vscode.DiagnosticSeverity> = {
    'error': vscode.DiagnosticSeverity.Error,
    'warn': vscode.DiagnosticSeverity.Warning,
    'info': vscode.DiagnosticSeverity.Information,
};

/** Stored per-file lint data for code actions and status bar. */
export interface FileLintData {
    results: CwsStructResult[];
    health: HealthScore | null;
    timestamp: number;
}

/**
 * Manages cws-struct diagnostics for the extension.
 *
 * Responsibilities:
 * - Spawn cws-struct as child process on lint requests
 * - Parse JSON output and map to VSCode Diagnostic objects
 * - Debounce rapid saves
 * - Store per-file results for code actions and status bar
 */
export class DiagnosticsManager {
    public readonly collection: vscode.DiagnosticCollection;

    private config: OmniCodeConfig;
    private debounceTimers: Map<string, NodeJS.Timeout> = new Map();
    private fileData: Map<string, FileLintData> = new Map();
    private outputChannel: vscode.OutputChannel | undefined;

    constructor(config: OmniCodeConfig) {
        this.config = config;
        this.collection = vscode.languages.createDiagnosticCollection('omnicode');
    }

    /** Update configuration (called on settings change). */
    updateConfig(config: OmniCodeConfig): void {
        this.config = config;
    }

    /** Set output channel for error logging. */
    setOutputChannel(channel: vscode.OutputChannel): void {
        this.outputChannel = channel;
    }

    /** Get stored lint data for a file URI. */
    getFileData(uri: vscode.Uri): FileLintData | undefined {
        return this.fileData.get(uri.toString());
    }

    /**
     * Lint a document. Debounced to avoid rapid re-lints.
     */
    lint(document: vscode.TextDocument): void {
        // Only lint file:// URIs
        if (document.uri.scheme !== 'file') {
            return;
        }

        // Detect format — skip unsupported files
        const format = detectFormat(document.uri);
        if (!format) {
            return;
        }

        const key = document.uri.toString();

        // Clear existing debounce timer
        const existing = this.debounceTimers.get(key);
        if (existing) {
            clearTimeout(existing);
        }

        // Debounce
        const timer = setTimeout(() => {
            this.debounceTimers.delete(key);
            this.runLint(document, format);
        }, this.config.debounceMs);

        this.debounceTimers.set(key, timer);
    }

    /** Clear diagnostics for a document. */
    clear(document: vscode.TextDocument): void {
        this.collection.delete(document.uri);
        this.fileData.delete(document.uri.toString());

        const timer = this.debounceTimers.get(document.uri.toString());
        if (timer) {
            clearTimeout(timer);
            this.debounceTimers.delete(document.uri.toString());
        }
    }

    // -----------------------------------------------------------------------
    // Private — lint execution
    // -----------------------------------------------------------------------

    private runLint(document: vscode.TextDocument, format: string): void {
        const filePath = document.uri.fsPath;
        const args = ['lint', format, filePath, '--json'];

        if (this.config.policy !== 'balanced') {
            args.push('--policy', this.config.policy);
        }

        const cwd = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath
            || path.dirname(filePath);

        try {
            cp.execFile(
                this.config.cwsStructPath,
                args,
                {
                    cwd,
                    maxBuffer: 1024 * 1024, // 1MB
                    timeout: 30000, // 30s
                },
                (error, stdout, stderr) => {
                    // cws-struct exits 1 for lint errors (normal), 2 for tool errors
                    if (error && (error as NodeJS.ErrnoException).code === 'ENOENT') {
                        this.logError(`cws-struct not found at: ${this.config.cwsStructPath}`);
                        this.logError('Install with: make install (from cws-struct source)');
                        return;
                    }

                    if (stderr && stderr.trim()) {
                        this.logError(stderr.trim());
                    }

                    if (!stdout || !stdout.trim()) {
                        return;
                    }

                    try {
                        const output: CwsStructOutput = JSON.parse(stdout);
                        this.applyDiagnostics(document, output);
                    } catch (parseError) {
                        this.logError(`Failed to parse cws-struct output: ${parseError}`);
                    }
                },
            );
        } catch (spawnError) {
            this.logError(`Failed to spawn cws-struct: ${spawnError}`);
        }
    }

    private applyDiagnostics(document: vscode.TextDocument, output: CwsStructOutput): void {
        const fileResult = this.findFileResult(document.uri.fsPath, output);
        if (!fileResult) {
            this.collection.set(document.uri, []);
            return;
        }

        // Store for code actions and status bar
        this.fileData.set(document.uri.toString(), {
            results: fileResult.results,
            health: fileResult.health,
            timestamp: Date.now(),
        });

        // Map results to diagnostics
        const diagnostics: vscode.Diagnostic[] = fileResult.results.map((result) =>
            this.resultToDiagnostic(document, result),
        );

        this.collection.set(document.uri, diagnostics);
    }

    private findFileResult(filePath: string, output: CwsStructOutput): CwsStructFile | undefined {
        // cws-struct returns relative paths — try matching against the end of the path
        const normalized = path.resolve(filePath);
        return output.files.find((f) => {
            const resolved = path.resolve(f.file);
            return resolved === normalized || normalized.endsWith(f.file) || f.file.endsWith(path.basename(filePath));
        });
    }

    private resultToDiagnostic(document: vscode.TextDocument, result: CwsStructResult): vscode.Diagnostic {
        let range: vscode.Range;

        if (result.line != null && result.line > 0) {
            // Line-level diagnostic (1-based → 0-based)
            const lineIndex = Math.min(result.line - 1, document.lineCount - 1);
            const line = document.lineAt(lineIndex);
            range = line.range;
        } else {
            // File-level diagnostic — first line
            range = new vscode.Range(0, 0, 0, 0);
        }

        const severity = SEVERITY_MAP[result.severity] ?? vscode.DiagnosticSeverity.Warning;

        const diagnostic = new vscode.Diagnostic(range, result.message, severity);
        diagnostic.source = 'cws-struct';
        diagnostic.code = result.rule;

        return diagnostic;
    }

    private logError(message: string): void {
        if (this.outputChannel) {
            this.outputChannel.appendLine(`[ERROR] ${message}`);
        }
    }
}

// ============================================================================
// CLOSING
// ============================================================================
