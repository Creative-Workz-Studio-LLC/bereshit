// ============================================================================
// CPI-SI Dashboard — Trajectory Tracker Panel
// Horizontal B.1 -> B.2 -> B.3 -> B.4 progress with metrics
// ============================================================================

import type { DashboardStore } from '../../state/store';
import type { StateSnapshot } from '../../state/types';

const SECTIONS = ['B.1', 'B.2', 'B.3', 'B.4'];

export class TrajectoryTracker {
    private element: HTMLElement;
    private sectionEls: Map<string, HTMLElement> = new Map();
    private timeEls: Map<string, HTMLElement> = new Map();
    private pivotEl: HTMLElement;
    private resetEl: HTMLElement;
    private momentumEl: HTMLElement;
    private momentumFillEl: HTMLElement;

    constructor(store: DashboardStore) {
        this.element = document.createElement('div');
        this.element.className = 'panel';

        let sectionsHtml = '<div class="trajectory-sections">';
        for (const sec of SECTIONS) {
            sectionsHtml += `
                <div class="trajectory-section" id="ts-${sec}">
                    <div class="section-label">${sec}</div>
                    <div class="section-time" id="ts-time-${sec}">0s</div>
                </div>
            `;
        }
        sectionsHtml += '</div>';

        this.element.innerHTML = `
            <div class="panel-title">Trajectory</div>
            <div class="panel-content">
                ${sectionsHtml}

                <div class="trajectory-metrics">
                    <div class="metric-card">
                        <div class="metric-value" id="ts-pivots">0</div>
                        <div class="metric-label">Pivots</div>
                    </div>
                    <div class="metric-card">
                        <div class="metric-value" id="ts-resets">0</div>
                        <div class="metric-label">Resets</div>
                    </div>
                    <div class="metric-card">
                        <div class="metric-value" id="ts-momentum">0.0</div>
                        <div class="metric-label">Momentum</div>
                    </div>
                </div>

                <div class="momentum-bar-container">
                    <div class="stat-row" style="margin-bottom: 4px;">
                        <span class="stat-label">Momentum Score</span>
                    </div>
                    <div class="momentum-bar-track">
                        <div class="momentum-bar-fill" id="ts-momentum-fill" style="width: 50%;"></div>
                    </div>
                </div>
            </div>
        `;

        // Cache element references
        for (const sec of SECTIONS) {
            const sEl = this.element.querySelector(`#ts-${sec}`);
            const tEl = this.element.querySelector(`#ts-time-${sec}`);
            if (sEl) this.sectionEls.set(sec, sEl as HTMLElement);
            if (tEl) this.timeEls.set(sec, tEl as HTMLElement);
        }

        this.pivotEl = this.element.querySelector('#ts-pivots')!;
        this.resetEl = this.element.querySelector('#ts-resets')!;
        this.momentumEl = this.element.querySelector('#ts-momentum')!;
        this.momentumFillEl = this.element.querySelector('#ts-momentum-fill')!;

        store.addEventListener('state-changed', ((e: CustomEvent<StateSnapshot>) => {
            this.update(e.detail);
        }) as EventListener);
    }

    getElement(): HTMLElement {
        return this.element;
    }

    private update(state: StateSnapshot): void {
        const current = state.trajectorySection;
        const metrics = state.trajectoryMetrics;
        const currentIdx = SECTIONS.indexOf(current);

        // Update section highlighting
        SECTIONS.forEach((sec, idx) => {
            const el = this.sectionEls.get(sec);
            if (!el) return;

            el.classList.remove('active', 'completed');
            if (sec === current) {
                el.classList.add('active');
            } else if (idx < currentIdx) {
                el.classList.add('completed');
            }
        });

        // Update times
        const times: Record<string, number> = {
            'B.1': metrics.b1TimeMs,
            'B.2': metrics.b2TimeMs,
            'B.3': metrics.b3TimeMs,
            'B.4': metrics.b4TimeMs,
        };

        for (const sec of SECTIONS) {
            const el = this.timeEls.get(sec);
            if (el) {
                el.textContent = this.formatDuration(times[sec] ?? 0);
            }
        }

        // Metrics
        this.pivotEl.textContent = String(metrics.pivotCount);
        this.resetEl.textContent = String(metrics.resetCount);
        this.momentumEl.textContent = metrics.momentumScore.toFixed(1);

        // Momentum bar: score ranges from -1 to +1, map to 0%-100%
        const momentumPct = ((metrics.momentumScore + 1) / 2) * 100;
        this.momentumFillEl.style.width = `${momentumPct}%`;

        if (metrics.momentumScore > 0.3) {
            this.momentumFillEl.style.background = 'var(--accent-green)';
        } else if (metrics.momentumScore > -0.3) {
            this.momentumFillEl.style.background = 'var(--accent-blue)';
        } else {
            this.momentumFillEl.style.background = 'var(--accent-red)';
        }
    }

    private formatDuration(ms: number): string {
        const seconds = ms / 1000;
        if (seconds < 60) return `${Math.round(seconds)}s`;
        if (seconds < 3600) return `${Math.round(seconds / 60)}m`;
        return `${(seconds / 3600).toFixed(1)}h`;
    }
}
