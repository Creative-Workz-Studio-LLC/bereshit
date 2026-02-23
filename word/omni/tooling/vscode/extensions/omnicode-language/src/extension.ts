// ============================================================================
// METADATA
// ============================================================================
//
// file:    src/extension.ts
// key:     B-omni-tooling-vscode-language-extension
// title:   OmniCode Language Extension — Entry Point
// type:    Code (TypeScript)
// version: a-01.00
// created: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: VSCode extension entry point. Activates on OmniCode/OmniFold files.
//          Wires cws-struct diagnostics, health status bar, and code actions.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import * as vscode from 'vscode';
import { DiagnosticsManager } from './diagnostics';
import { HealthStatusBar } from './statusbar';
import { OmniCodeActionProvider } from './codeactions';
import { getConfig } from './config';

// ============================================================================
// BODY
// ============================================================================

/**
 * Extension activation — called when VSCode loads the extension.
 *
 * Registers:
 * - DiagnosticsManager: cws-struct inline linting on save/open
 * - HealthStatusBar: ternary health score for active file
 * - OmniCodeActionProvider: QuickFix code actions from fix suggestions
 * - Commands: lintFile, lintAllFiles, showHealthDetails
 */
export function activate(context: vscode.ExtensionContext): void {
    let config = getConfig();
    const outputChannel = vscode.window.createOutputChannel('OmniCode');
    const diagnostics = new DiagnosticsManager(config);
    const statusBar = new HealthStatusBar(diagnostics);
    const codeActions = new OmniCodeActionProvider(diagnostics);

    diagnostics.setOutputChannel(outputChannel);

    // Document selectors for all supported formats
    const selector: vscode.DocumentSelector = [
        { language: 'omnicode', scheme: 'file' },
        { language: 'omnifold', scheme: 'file' },
        { language: 'go', scheme: 'file' },
        { language: 'rust', scheme: 'file' },
        { language: 'toml', scheme: 'file' },
        { language: 'json', scheme: 'file' },
        { language: 'jsonc', scheme: 'file' },
        { language: 'makefile', scheme: 'file' },
    ];

    context.subscriptions.push(
        // Output channel
        outputChannel,

        // Core diagnostic collection
        diagnostics.collection,

        // Status bar item
        statusBar.item,

        // Code action provider (QuickFix)
        vscode.languages.registerCodeActionsProvider(selector, codeActions, {
            providedCodeActionKinds: [vscode.CodeActionKind.QuickFix],
        }),

        // ---------------------------------------------------------------
        // Commands
        // ---------------------------------------------------------------

        // Lint active file
        vscode.commands.registerCommand('omnicode.lintFile', () => {
            const editor = vscode.window.activeTextEditor;
            if (editor) {
                diagnostics.lint(editor.document);
                outputChannel.appendLine(`Linting: ${editor.document.uri.fsPath}`);
            }
        }),

        // Lint all open files
        vscode.commands.registerCommand('omnicode.lintAllFiles', () => {
            let count = 0;
            for (const doc of vscode.workspace.textDocuments) {
                diagnostics.lint(doc);
                count++;
            }
            outputChannel.appendLine(`Linting ${count} open files`);
        }),

        // Show health details in output channel
        vscode.commands.registerCommand('omnicode.showHealthDetails', () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor) {
                return;
            }

            const data = diagnostics.getFileData(editor.document.uri);
            if (!data || !data.health) {
                outputChannel.appendLine('No health data for active file');
                outputChannel.show(true);
                return;
            }

            const health = data.health;
            const sign = health.total >= 0 ? '+' : '';
            outputChannel.appendLine('');
            outputChannel.appendLine(`═══ OmniCode Health Report ═══`);
            outputChannel.appendLine(`File: ${editor.document.uri.fsPath}`);
            outputChannel.appendLine(`Score: ${sign}${health.total}`);
            outputChannel.appendLine(`Actions: ${health.totalActions} total`);
            outputChannel.appendLine(`  Aligned:    ${health.alignedCount}`);
            outputChannel.appendLine(`  Neutral:    ${health.neutralCount}`);
            outputChannel.appendLine(`  Misaligned: ${health.misalignedCount}`);
            outputChannel.appendLine('');
            outputChannel.appendLine('Block Scores:');
            for (const block of health.blocks) {
                const blockSign = block.score >= 0 ? '+' : '';
                outputChannel.appendLine(`  ${block.block}: ${blockSign}${block.score}`);
            }
            if (health.containers && health.containers.length > 0) {
                outputChannel.appendLine('');
                outputChannel.appendLine('Container Scores:');
                for (const container of health.containers) {
                    const cSign = container.score >= 0 ? '+' : '';
                    outputChannel.appendLine(`  ${container.container}: ${cSign}${container.score}`);
                }
            }
            outputChannel.appendLine(`══════════════════════════════`);
            outputChannel.show(true);
        }),

        // Show output channel
        vscode.commands.registerCommand('omnicode.showOutput', () => {
            outputChannel.show(true);
        }),

        // ---------------------------------------------------------------
        // Events
        // ---------------------------------------------------------------

        // Lint on save
        vscode.workspace.onDidSaveTextDocument((doc) => {
            if (config.lintOnSave) {
                diagnostics.lint(doc);
            }
        }),

        // Lint on open
        vscode.workspace.onDidOpenTextDocument((doc) => {
            if (config.lintOnOpen) {
                diagnostics.lint(doc);
            }
        }),

        // Clear diagnostics on close
        vscode.workspace.onDidCloseTextDocument((doc) => {
            diagnostics.clear(doc);
        }),

        // Respond to config changes
        vscode.workspace.onDidChangeConfiguration((e) => {
            if (e.affectsConfiguration('omnicode')) {
                config = getConfig();
                diagnostics.updateConfig(config);
                statusBar.refresh();
            }
        }),

        // Update status bar on active editor change + after lint
        vscode.window.onDidChangeActiveTextEditor(() => {
            statusBar.refresh();
        }),

        // Refresh status bar after diagnostics change
        vscode.languages.onDidChangeDiagnostics(() => {
            statusBar.refresh();
        }),
    );

    // Lint all currently open documents
    for (const doc of vscode.workspace.textDocuments) {
        if (config.lintOnOpen) {
            diagnostics.lint(doc);
        }
    }

    outputChannel.appendLine(`OmniCode Language v0.3.0 activated`);
    outputChannel.appendLine(`cws-struct: ${config.cwsStructPath}`);
    outputChannel.appendLine(`Policy: ${config.policy} | Lint on save: ${config.lintOnSave} | Lint on open: ${config.lintOnOpen}`);
}

/**
 * Extension deactivation — cleanup.
 */
export function deactivate(): void {
    // Disposables handled by context.subscriptions
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Entry point wires all components. Each module is independent:
//   diagnostics.ts — cws-struct spawning + diagnostic mapping
//   statusbar.ts   — health score display
//   codeactions.ts — QuickFix from fix suggestions
//   config.ts      — settings management
//
// "Look that thou make them after their pattern." — Exodus 25:40
// ============================================================================
