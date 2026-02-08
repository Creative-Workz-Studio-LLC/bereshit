// ============================================================================
// CPI-SI Dashboard — System Data Browser Panel
// Tree view of system/data/ with file preview
// ============================================================================

import type { DashboardStore } from '../../state/store';
import type { SystemDataEntry } from '../../state/types';

export class SystemDataBrowser {
    private element: HTMLElement;
    private treeContainer: HTMLElement;
    private previewContainer: HTMLElement;
    private breadcrumb: HTMLElement;
    private currentPath: string[] = [];

    constructor(_store: DashboardStore) {
        this.element = document.createElement('div');
        this.element.className = 'panel';

        this.element.innerHTML = `
            <div class="panel-title">System Data</div>
            <div class="panel-content">
                <div class="sdb-breadcrumb" id="sdb-breadcrumb">
                    <span class="sdb-crumb sdb-crumb-link" data-path="">data/</span>
                </div>
                <div class="sdb-split">
                    <div class="sdb-tree" id="sdb-tree">
                        <div class="analytics-empty">Loading...</div>
                    </div>
                    <div class="sdb-preview" id="sdb-preview">
                        <div class="sdb-preview-placeholder">Select a file to preview</div>
                    </div>
                </div>
            </div>
        `;

        this.treeContainer = this.element.querySelector('#sdb-tree')!;
        this.previewContainer = this.element.querySelector('#sdb-preview')!;
        this.breadcrumb = this.element.querySelector('#sdb-breadcrumb')!;

        // Load root
        this.loadPath('');
    }

    getElement(): HTMLElement {
        return this.element;
    }

    private async loadPath(relativePath: string): Promise<void> {
        try {
            const url = `/api/systemdata?path=${encodeURIComponent(relativePath)}`;
            const resp = await fetch(url);
            if (!resp.ok) {
                this.treeContainer.innerHTML = '<div class="analytics-empty">Failed to load</div>';
                return;
            }
            const entry = (await resp.json()) as SystemDataEntry;

            if (entry.isDir) {
                this.renderDirectory(entry, relativePath);
            } else {
                this.renderFilePreview(entry);
            }
        } catch {
            this.treeContainer.innerHTML = '<div class="analytics-empty">System data unavailable</div>';
        }
    }

    private renderDirectory(entry: SystemDataEntry, path: string): void {
        // Update breadcrumb
        this.currentPath = path ? path.split('/') : [];
        this.updateBreadcrumb();

        const children = entry.children ?? [];
        if (children.length === 0) {
            this.treeContainer.innerHTML = '<div class="analytics-empty">Empty directory</div>';
            return;
        }

        // Sort: directories first, then files
        const sorted = [...children].sort((a, b) => {
            if (a.isDir && !b.isDir) return -1;
            if (!a.isDir && b.isDir) return 1;
            return a.name.localeCompare(b.name);
        });

        let html = '';
        for (const child of sorted) {
            const icon = child.isDir ? '\uD83D\uDCC1' : this.fileIcon(child.name);
            const size = child.isDir ? '' : this.formatSize(child.size ?? 0);

            html += `
                <div class="sdb-entry ${child.isDir ? 'sdb-dir' : 'sdb-file'}" data-path="${this.escapeAttr(child.path)}">
                    <span class="sdb-icon">${icon}</span>
                    <span class="sdb-name">${this.escapeHtml(child.name)}</span>
                    <span class="sdb-size">${size}</span>
                </div>
            `;
        }
        this.treeContainer.innerHTML = html;

        // Attach click handlers
        const entries = this.treeContainer.querySelectorAll('.sdb-entry');
        entries.forEach((el) => {
            el.addEventListener('click', () => {
                const p = (el as HTMLElement).dataset.path ?? '';
                if (el.classList.contains('sdb-dir')) {
                    this.loadPath(p);
                } else {
                    this.loadPath(p);
                }
            });
        });
    }

    private renderFilePreview(entry: SystemDataEntry): void {
        const content = entry.content ?? '';
        const ext = entry.name.split('.').pop()?.toLowerCase() ?? '';
        const isJson = ext === 'json' || ext === 'jsonc' || ext === 'jsonl';

        let displayContent = content;
        if (isJson) {
            try {
                displayContent = JSON.stringify(JSON.parse(content), null, 2);
            } catch {
                // Keep as-is if not valid JSON
            }
        }

        // Truncate very long content
        const maxLen = 10000;
        const truncated = displayContent.length > maxLen;
        if (truncated) {
            displayContent = displayContent.slice(0, maxLen);
        }

        this.previewContainer.innerHTML = `
            <div class="sdb-preview-header">
                <span class="sdb-preview-name">${this.escapeHtml(entry.name)}</span>
                <span class="sdb-preview-size">${this.formatSize(entry.size ?? 0)}</span>
            </div>
            <pre class="sdb-preview-content">${this.escapeHtml(displayContent)}${truncated ? '\n\n... (truncated)' : ''}</pre>
        `;
    }

    private updateBreadcrumb(): void {
        let html = '<span class="sdb-crumb sdb-crumb-link" data-path="">data/</span>';
        let accumulated = '';

        for (const segment of this.currentPath) {
            accumulated += (accumulated ? '/' : '') + segment;
            html += `<span class="sdb-crumb-sep">/</span><span class="sdb-crumb sdb-crumb-link" data-path="${this.escapeAttr(accumulated)}">${this.escapeHtml(segment)}</span>`;
        }

        this.breadcrumb.innerHTML = html;

        // Attach breadcrumb click handlers
        const links = this.breadcrumb.querySelectorAll('.sdb-crumb-link');
        links.forEach((el) => {
            el.addEventListener('click', () => {
                const p = (el as HTMLElement).dataset.path ?? '';
                this.loadPath(p);
            });
        });
    }

    private fileIcon(name: string): string {
        const ext = name.split('.').pop()?.toLowerCase() ?? '';
        switch (ext) {
            case 'json': case 'jsonc': case 'jsonl': return '\uD83D\uDCCB';
            case 'toml': case 'yaml': case 'yml': return '\u2699\uFE0F';
            case 'md': return '\uD83D\uDCC4';
            case 'go': return '\uD83D\uDCE6';
            case 'db': case 'sqlite': return '\uD83D\uDDC4\uFE0F';
            default: return '\uD83D\uDCC4';
        }
    }

    private formatSize(bytes: number): string {
        if (bytes === 0) return '';
        if (bytes < 1024) return `${bytes}B`;
        if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)}KB`;
        return `${(bytes / (1024 * 1024)).toFixed(1)}MB`;
    }

    private escapeHtml(text: string): string {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }

    private escapeAttr(text: string): string {
        return text.replace(/&/g, '&amp;').replace(/"/g, '&quot;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
    }
}
