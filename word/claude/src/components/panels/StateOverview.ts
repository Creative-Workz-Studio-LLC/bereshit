// ============================================================================
// CPI-SI Dashboard — State Overview Panel
// Current anchor position, Hebrew state, health, CPI, K-align
// ============================================================================

import type { DashboardStore } from '../../state/store';
import type { StateSnapshot } from '../../state/types';

export class StateOverview {
    private element: HTMLElement;
    private anchorEl: HTMLElement;
    private hebrewBadgeEl: HTMLElement;
    private healthFillEl: HTMLElement;
    private healthLabelEl: HTMLElement;
    private cpiValueEl: HTMLElement;
    private cpiRatingEl: HTMLElement;
    private kAlignMarkerEl: HTMLElement;
    private arcBadgeEl: HTMLElement;
    private exchangeEl: HTMLElement;
    private insightEl: HTMLElement;
    private commandEl: HTMLElement;
    private hooksEl: HTMLElement;

    constructor(store: DashboardStore) {
        this.element = document.createElement('div');
        this.element.className = 'panel';
        this.element.innerHTML = `
            <div class="panel-title">State Overview</div>
            <div class="panel-content state-overview">
                <div class="anchor-display">
                    <div class="anchor-position" id="so-anchor">0.0.0</div>
                    <div class="anchor-label">Anchor Position</div>
                </div>

                <div style="text-align: center;">
                    <span class="hebrew-badge yashar" id="so-hebrew">YASHAR (EVEN)</span>
                </div>

                <div>
                    <div class="stat-row">
                        <span class="stat-label">Health</span>
                        <span class="stat-value" id="so-health-label">0</span>
                    </div>
                    <div class="health-bar-container">
                        <div class="health-bar-track">
                            <div class="health-bar-center"></div>
                            <div class="health-bar-fill" id="so-health-fill"></div>
                        </div>
                        <div class="health-bar-labels">
                            <span>-100</span>
                            <span>0</span>
                            <span>+100</span>
                        </div>
                    </div>
                </div>

                <div class="cpi-gauge">
                    <div class="cpi-gauge-value" id="so-cpi-value">0.50</div>
                    <div class="cpi-gauge-rating" id="so-cpi-rating">Neutral</div>
                </div>

                <div>
                    <div class="stat-row" style="margin-bottom: 4px;">
                        <span class="stat-label">K:ALIGN</span>
                    </div>
                    <div class="k-align-bar">
                        <span class="k-label-left">-1</span>
                        <div class="k-align-track">
                            <div class="k-align-marker" id="so-k-marker" style="left: 50%;"></div>
                        </div>
                        <span class="k-label-right">+1</span>
                    </div>
                </div>

                <div class="stat-row">
                    <span class="stat-label">Session Arc</span>
                    <span class="arc-badge" id="so-arc">unknown</span>
                </div>

                <div class="stat-row">
                    <span class="stat-label">Command</span>
                    <span class="stat-value" id="so-command">AWAIT</span>
                </div>

                <div class="stat-row">
                    <span class="stat-label">Exchanges</span>
                    <span class="stat-value" id="so-exchanges">0</span>
                </div>

                <div class="stat-row">
                    <span class="stat-label">Insights</span>
                    <span class="stat-value" id="so-insights">0</span>
                </div>

                <div class="stat-row">
                    <span class="stat-label">Hooks Fired</span>
                    <span class="stat-value" id="so-hooks">0</span>
                </div>
            </div>
        `;

        this.anchorEl = this.element.querySelector('#so-anchor')!;
        this.hebrewBadgeEl = this.element.querySelector('#so-hebrew')!;
        this.healthFillEl = this.element.querySelector('#so-health-fill')!;
        this.healthLabelEl = this.element.querySelector('#so-health-label')!;
        this.cpiValueEl = this.element.querySelector('#so-cpi-value')!;
        this.cpiRatingEl = this.element.querySelector('#so-cpi-rating')!;
        this.kAlignMarkerEl = this.element.querySelector('#so-k-marker')!;
        this.arcBadgeEl = this.element.querySelector('#so-arc')!;
        this.exchangeEl = this.element.querySelector('#so-exchanges')!;
        this.insightEl = this.element.querySelector('#so-insights')!;
        this.commandEl = this.element.querySelector('#so-command')!;
        this.hooksEl = this.element.querySelector('#so-hooks')!;

        store.addEventListener('state-changed', ((e: CustomEvent<StateSnapshot>) => {
            this.update(e.detail);
        }) as EventListener);
    }

    getElement(): HTMLElement {
        return this.element;
    }

    private update(state: StateSnapshot): void {
        // Anchor position
        this.anchorEl.textContent = state.anchorKey;

        // Hebrew state badge
        const hebrewLower = state.hebrewState.toLowerCase();
        this.hebrewBadgeEl.className = `hebrew-badge ${hebrewLower}`;
        this.hebrewBadgeEl.textContent = `${state.hebrewState} (${state.hebrewMeaning})`;

        // Health bar
        const health = state.healthScore;
        this.healthLabelEl.textContent = health > 0 ? `+${health}` : String(health);
        this.healthLabelEl.style.color = health > 0
            ? 'var(--accent-green)'
            : health < 0
                ? 'var(--accent-red)'
                : 'var(--text-primary)';

        // Health bar fill: center is 50%, fill extends from center
        const absHealth = Math.abs(health);
        const fillWidth = (absHealth / 100) * 50;
        if (health >= 0) {
            this.healthFillEl.style.left = '50%';
            this.healthFillEl.style.width = `${fillWidth}%`;
            this.healthFillEl.style.background = 'var(--accent-green)';
        } else {
            this.healthFillEl.style.left = `${50 - fillWidth}%`;
            this.healthFillEl.style.width = `${fillWidth}%`;
            this.healthFillEl.style.background = 'var(--accent-red)';
        }

        // CPI gauge
        const cpi = state.cpiScore;
        this.cpiValueEl.textContent = cpi.toFixed(2);
        if (cpi >= 0.7) {
            this.cpiValueEl.style.color = 'var(--accent-green)';
        } else if (cpi >= 0.4) {
            this.cpiValueEl.style.color = 'var(--accent-blue)';
        } else {
            this.cpiValueEl.style.color = 'var(--accent-red)';
        }
        this.cpiRatingEl.textContent = state.cpiRating;

        // K-Align marker: kAlign ranges -1 to +1, map to 0%-100%
        const kPercent = ((state.kAlign + 1) / 2) * 100;
        this.kAlignMarkerEl.style.left = `${kPercent}%`;
        if (state.kAlign > 0.2) {
            this.kAlignMarkerEl.style.background = 'var(--accent-green)';
        } else if (state.kAlign < -0.2) {
            this.kAlignMarkerEl.style.background = 'var(--accent-red)';
        } else {
            this.kAlignMarkerEl.style.background = 'var(--accent-blue)';
        }

        // Session arc
        this.arcBadgeEl.textContent = state.sessionArc;

        // Command
        this.commandEl.textContent = state.commandKey;
        if (state.commandKey === 'HALT') {
            this.commandEl.style.color = 'var(--accent-red)';
        } else if (state.commandKey === 'PROCEED') {
            this.commandEl.style.color = 'var(--accent-green)';
        } else {
            this.commandEl.style.color = 'var(--accent-blue)';
        }

        // Counters
        this.exchangeEl.textContent = String(state.exchangeCount);
        this.insightEl.textContent = String(state.insightCount);
        this.hooksEl.textContent = String(state.hooksFired);
    }
}
