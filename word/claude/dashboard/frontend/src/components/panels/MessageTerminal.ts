// ============================================================================
// CPI-SI Dashboard — Message Terminal Panel
// Scrolling event log with terminal-style presentation
// ============================================================================

import type { DashboardStore } from '../../state/store';
import type { LogEvent } from '../../state/types';

export class MessageTerminal {
    private element: HTMLElement;
    private container: HTMLElement;
    private content: HTMLElement;
    private indicator: HTMLElement;
    private autoScroll = true;
    private pendingCount = 0;

    constructor(store: DashboardStore) {
        this.element = document.createElement('div');
        this.element.className = 'panel';
        this.element.innerHTML = `
            <div class="panel-title">Event Log</div>
            <div class="panel-content" style="padding: 0; position: relative; overflow: hidden;">
                <div class="terminal-container" id="mt-container">
                    <div class="terminal-content" id="mt-content"></div>
                </div>
                <div class="new-events-indicator" id="mt-indicator">New events below</div>
            </div>
        `;

        this.container = this.element.querySelector('#mt-container')!;
        this.content = this.element.querySelector('#mt-content')!;
        this.indicator = this.element.querySelector('#mt-indicator')!;

        // Detect user scroll
        this.container.addEventListener('scroll', () => {
            const { scrollTop, scrollHeight, clientHeight } = this.container;
            const atBottom = scrollHeight - scrollTop - clientHeight < 30;
            this.autoScroll = atBottom;

            if (atBottom) {
                this.pendingCount = 0;
                this.indicator.classList.remove('visible');
            }
        });

        // Click indicator to scroll to bottom
        this.indicator.addEventListener('click', () => {
            this.scrollToBottom();
            this.pendingCount = 0;
            this.indicator.classList.remove('visible');
            this.autoScroll = true;
        });

        store.addEventListener('events-changed', ((e: CustomEvent<LogEvent>) => {
            this.addLine(e.detail);
        }) as EventListener);
    }

    getElement(): HTMLElement {
        return this.element;
    }

    private addLine(event: LogEvent): void {
        const line = document.createElement('div');
        line.className = 'terminal-line';

        const ts = this.formatTimestamp(event.timestamp);
        const level = event.level || 'INFO';
        const component = event.component || 'system';
        const message = event.message || '';

        let deltaHtml = '';
        if (event.delta !== undefined && event.delta !== 0) {
            const sign = event.delta > 0 ? '+' : '';
            const cls = event.delta > 0 ? 'delta-positive' : 'delta-negative';
            deltaHtml = ` <span class="${cls}">[${sign}${event.delta}]</span>`;
        }

        line.innerHTML =
            `<span class="ts">[${ts}]</span> ` +
            `<span class="level-${level}">[${level}]</span> ` +
            `<span class="component">${this.escapeHtml(component)}</span>: ` +
            `<span class="message">${this.escapeHtml(message)}</span>` +
            deltaHtml;

        this.content.appendChild(line);

        // Cap at 500 visible lines
        while (this.content.children.length > 500) {
            this.content.removeChild(this.content.firstChild!);
        }

        if (this.autoScroll) {
            this.scrollToBottom();
        } else {
            this.pendingCount++;
            this.indicator.textContent = `${this.pendingCount} new event${this.pendingCount > 1 ? 's' : ''} below`;
            this.indicator.classList.add('visible');
        }
    }

    private scrollToBottom(): void {
        requestAnimationFrame(() => {
            this.container.scrollTop = this.container.scrollHeight;
        });
    }

    private formatTimestamp(iso: string): string {
        if (!iso) return '00:00:00';
        try {
            const d = new Date(iso);
            return d.toLocaleTimeString('en-US', { hour12: false });
        } catch {
            return iso.substring(11, 19) || '00:00:00';
        }
    }

    private escapeHtml(text: string): string {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }
}
