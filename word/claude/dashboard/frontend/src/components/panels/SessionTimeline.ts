// ============================================================================
// CPI-SI Dashboard — Session Timeline Panel
// Session duration, exchanges, tokens, and summary stats
// ============================================================================

import type { DashboardStore } from '../../state/store';
import type { StateSnapshot } from '../../state/types';

export class SessionTimeline {
    private element: HTMLElement;
    private durationEl: HTMLElement;
    private exchangesEl: HTMLElement;
    private tokensEl: HTMLElement;
    private compactionsEl: HTMLElement;
    private sessionStartTime: Date | null = null;
    private durationTimer: ReturnType<typeof setInterval> | null = null;

    constructor(store: DashboardStore) {
        this.element = document.createElement('div');
        this.element.className = 'panel';
        this.element.innerHTML = `
            <div class="panel-title">Session</div>
            <div class="panel-content">
                <div class="session-timeline">
                    <div class="timeline-stat">
                        <div class="timeline-stat-value" id="st-duration">00:00:00</div>
                        <div class="timeline-stat-label">Duration</div>
                    </div>
                    <div class="timeline-stat">
                        <div class="timeline-stat-value" id="st-exchanges">0</div>
                        <div class="timeline-stat-label">Exchanges</div>
                    </div>
                    <div class="timeline-stat">
                        <div class="timeline-stat-value" id="st-tokens">0</div>
                        <div class="timeline-stat-label">Current Tokens</div>
                    </div>
                    <div class="timeline-stat">
                        <div class="timeline-stat-value" id="st-compactions">0</div>
                        <div class="timeline-stat-label">Compactions</div>
                    </div>
                </div>
            </div>
        `;

        this.durationEl = this.element.querySelector('#st-duration')!;
        this.exchangesEl = this.element.querySelector('#st-exchanges')!;
        this.tokensEl = this.element.querySelector('#st-tokens')!;
        this.compactionsEl = this.element.querySelector('#st-compactions')!;

        // Update duration every second
        this.durationTimer = setInterval(() => {
            this.updateDuration();
        }, 1000);

        store.addEventListener('state-changed', ((e: CustomEvent<StateSnapshot>) => {
            this.update(e.detail);
        }) as EventListener);
    }

    getElement(): HTMLElement {
        return this.element;
    }

    private update(state: StateSnapshot): void {
        // Track session start
        if (state.sessionStart) {
            this.sessionStartTime = new Date(state.sessionStart);
        }

        // Exchanges
        this.exchangesEl.textContent = String(state.exchangeCount);

        // Tokens (format with K suffix for readability)
        this.tokensEl.textContent = this.formatTokens(state.currentTokens);
        if (state.currentTokens > 150000) {
            this.tokensEl.style.color = 'var(--accent-red)';
        } else if (state.currentTokens > 100000) {
            this.tokensEl.style.color = 'var(--accent-gold)';
        } else {
            this.tokensEl.style.color = 'var(--accent-blue)';
        }

        // Compactions
        this.compactionsEl.textContent = String(state.compactionCount);

        this.updateDuration();
    }

    private updateDuration(): void {
        if (!this.sessionStartTime) {
            this.durationEl.textContent = '00:00:00';
            return;
        }

        const now = new Date();
        const diff = Math.floor((now.getTime() - this.sessionStartTime.getTime()) / 1000);

        const hours = Math.floor(diff / 3600);
        const minutes = Math.floor((diff % 3600) / 60);
        const seconds = diff % 60;

        this.durationEl.textContent =
            `${String(hours).padStart(2, '0')}:${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
    }

    private formatTokens(tokens: number): string {
        if (tokens >= 1000) {
            return `${(tokens / 1000).toFixed(1)}K`;
        }
        return String(tokens);
    }

    destroy(): void {
        if (this.durationTimer) {
            clearInterval(this.durationTimer);
            this.durationTimer = null;
        }
    }
}
