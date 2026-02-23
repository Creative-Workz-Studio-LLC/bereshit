// ============================================================================
// METADATA
// ============================================================================
//
// file:    src/codeactions.ts
// key:     B-omni-tooling-vscode-language-codeactions
// title:   OmniCode Language Extension — Code Action Provider
// type:    Code (TypeScript)
// version: a-01.00
// created: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: Provide QuickFix code actions from cws-struct fix suggestions.
//          Maps FixAction[] to VSCode WorkspaceEdit operations.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import * as vscode from 'vscode';
import { DiagnosticsManager } from './diagnostics';
import { CwsStructResult, FixAction } from './types';

// ============================================================================
// BODY
// ============================================================================

/**
 * Provides QuickFix code actions when cws-struct results include fix suggestions.
 */
export class OmniCodeActionProvider implements vscode.CodeActionProvider {
    private diagnostics: DiagnosticsManager;

    constructor(diagnostics: DiagnosticsManager) {
        this.diagnostics = diagnostics;
    }

    provideCodeActions(
        document: vscode.TextDocument,
        _range: vscode.Range | vscode.Selection,
        context: vscode.CodeActionContext,
        _token: vscode.CancellationToken,
    ): vscode.CodeAction[] {
        const actions: vscode.CodeAction[] = [];
        const data = this.diagnostics.getFileData(document.uri);
        if (!data) {
            return actions;
        }

        // For each diagnostic in the context that has a matching fix
        for (const diagnostic of context.diagnostics) {
            if (diagnostic.source !== 'cws-struct') {
                continue;
            }

            const result = this.findMatchingResult(data.results, diagnostic);
            if (!result || !result.fix) {
                continue;
            }

            const fix = result.fix;

            // Only create code action if there are actionable fix steps
            if (fix.actions && fix.actions.length > 0) {
                const action = this.createFixAction(document, diagnostic, fix.description, fix.actions);
                if (action) {
                    actions.push(action);
                }
            }
        }

        return actions;
    }

    private findMatchingResult(
        results: CwsStructResult[],
        diagnostic: vscode.Diagnostic,
    ): CwsStructResult | undefined {
        return results.find((r) =>
            r.rule === diagnostic.code && r.message === diagnostic.message,
        );
    }

    private createFixAction(
        document: vscode.TextDocument,
        diagnostic: vscode.Diagnostic,
        description: string,
        fixActions: FixAction[],
    ): vscode.CodeAction | undefined {
        const edit = new vscode.WorkspaceEdit();
        let hasEdits = false;

        for (const action of fixActions) {
            if (action.type === 'remove' && action.startLine != null) {
                const startLine = Math.max(0, action.startLine - 1);
                const endLine = action.endLine != null
                    ? Math.min(document.lineCount, action.endLine)
                    : startLine + 1;

                edit.delete(
                    document.uri,
                    new vscode.Range(startLine, 0, endLine, 0),
                );
                hasEdits = true;
            } else if (action.type === 'replace' && action.startLine != null && action.content) {
                // Replace lines with new content
                const startLine = Math.max(0, action.startLine - 1);
                const endLine = action.endLine != null
                    ? Math.min(document.lineCount, action.endLine)
                    : startLine + 1;
                const text = action.content.join('\n') + '\n';

                edit.replace(
                    document.uri,
                    new vscode.Range(startLine, 0, endLine, 0),
                    text,
                );
                hasEdits = true;
            } else if (action.type === 'insert' && action.afterLine != null && action.content) {
                const insertLine = action.afterLine; // 1-based, insert after
                const position = new vscode.Position(insertLine, 0);
                const text = action.content.join('\n') + '\n';

                edit.insert(document.uri, position, text);
                hasEdits = true;
            }
        }

        if (!hasEdits) {
            return undefined;
        }

        const codeAction = new vscode.CodeAction(
            description,
            vscode.CodeActionKind.QuickFix,
        );
        codeAction.edit = edit;
        codeAction.diagnostics = [diagnostic];
        codeAction.isPreferred = true;

        return codeAction;
    }
}

// ============================================================================
// CLOSING
// ============================================================================
