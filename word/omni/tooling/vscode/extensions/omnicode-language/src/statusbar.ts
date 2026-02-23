// ============================================================================
// METADATA
// ============================================================================
//
// file:    src/statusbar.ts
// key:     B-omni-tooling-vscode-language-statusbar
// title:   OmniCode Language Extension — Health Status Bar
// type:    Code (TypeScript)
// version: a-01.00
// created: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: Display ternary health score in the VSCode status bar.
//          Color-coded: green (50+), yellow (0-49), red (<0).
//          Click opens output channel with detailed breakdown.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import * as vscode from 'vscode';
import { DiagnosticsManager } from './diagnostics';
import { HealthScore } from './types';

// ============================================================================
// BODY
// ============================================================================

/**
 * Status bar item showing the health score of the active file.
 *
 * Ternary scale: -100 (shavar) → 0 (yashar) → +100 (tov)
 */
export class HealthStatusBar {
    public readonly item: vscode.StatusBarItem;
    private diagnostics: DiagnosticsManager;

    constructor(diagnostics: DiagnosticsManager) {
        this.diagnostics = diagnostics;
        this.item = vscode.window.createStatusBarItem(
            vscode.StatusBarAlignment.Right,
            100,
        );
        this.item.command = 'omnicode.showHealthDetails';
        this.refresh();
    }

    /** Refresh the status bar for the current active editor. */
    refresh(): void {
        const editor = vscode.window.activeTextEditor;
        if (!editor) {
            this.item.hide();
            return;
        }

        const data = this.diagnostics.getFileData(editor.document.uri);
        if (!data || !data.health) {
            this.item.hide();
            return;
        }

        this.updateDisplay(data.health);
        this.item.show();
    }

    private updateDisplay(health: HealthScore): void {
        const score = health.total;
        const sign = score >= 0 ? '+' : '';

        this.item.text = `$(pulse) OmniCode: ${sign}${score}`;

        // Color coding: green (50+), yellow (0-49), red (<0)
        if (score >= 50) {
            this.item.backgroundColor = undefined;
            this.item.color = '#3fb950';
        } else if (score >= 0) {
            this.item.backgroundColor = undefined;
            this.item.color = '#d29922';
        } else {
            this.item.backgroundColor = new vscode.ThemeColor(
                'statusBarItem.errorBackground',
            );
            this.item.color = undefined;
        }

        // Tooltip with block breakdown
        const blocks = health.blocks
            .map((b) => `  ${b.block}: ${b.score >= 0 ? '+' : ''}${b.score}`)
            .join('\n');

        this.item.tooltip = new vscode.MarkdownString(
            `**OmniCode Health: ${sign}${score}**\n\n` +
            `Actions: ${health.totalActions} ` +
            `(${health.alignedCount} aligned, ${health.neutralCount} neutral, ${health.misalignedCount} misaligned)\n\n` +
            '```\n' + blocks + '\n```\n\n' +
            '_Click for full details_',
        );
    }
}

// ============================================================================
// CLOSING
// ============================================================================
