// ============================================================================
// CPI-SI Dashboard — Analytics Charts Panel
// Tabbed charts: K:ALIGN trend, key distribution, Hebrew frequency, patterns
// ============================================================================

import type { DashboardStore } from '../../state/store';
import type { AnalyticsBundle, AggregatesSummary } from '../../state/types';
import { Chart, registerables } from 'chart.js';

Chart.register(...registerables);

type TabName = 'kalign' | 'keys' | 'hebrew' | 'patterns' | 'work';

const TABS: { id: TabName; label: string }[] = [
    { id: 'kalign', label: 'K:ALIGN' },
    { id: 'keys', label: 'Keys' },
    { id: 'hebrew', label: 'Hebrew' },
    { id: 'patterns', label: 'Patterns' },
    { id: 'work', label: 'Activity' },
];

export class AnalyticsCharts {
    private element: HTMLElement;
    private activeTab: TabName = 'kalign';
    private chartContainer: HTMLElement;
    private canvas: HTMLCanvasElement;
    private chart: Chart | null = null;
    private patternsList: HTMLElement;
    private data: AnalyticsBundle | null = null;
    private loading = false;
    private tabButtons: Map<TabName, HTMLElement> = new Map();

    constructor(_store: DashboardStore) {
        this.element = document.createElement('div');
        this.element.className = 'panel';

        // Build tab bar
        let tabsHtml = '<div class="analytics-tabs">';
        for (const tab of TABS) {
            const active = tab.id === this.activeTab ? ' active' : '';
            tabsHtml += `<button class="analytics-tab${active}" data-tab="${tab.id}">${tab.label}</button>`;
        }
        tabsHtml += '</div>';

        this.element.innerHTML = `
            <div class="panel-title">Analytics</div>
            <div class="panel-content">
                ${tabsHtml}
                <div class="analytics-chart-container" id="ac-chart-container">
                    <canvas id="ac-canvas"></canvas>
                </div>
                <div class="analytics-patterns-list" id="ac-patterns" style="display:none;"></div>
            </div>
        `;

        this.chartContainer = this.element.querySelector('#ac-chart-container')!;
        this.canvas = this.element.querySelector('#ac-canvas')!;
        this.patternsList = this.element.querySelector('#ac-patterns')!;

        // Cache tab buttons and attach click handlers
        const tabBtns = this.element.querySelectorAll('.analytics-tab');
        tabBtns.forEach((btn) => {
            const tabId = (btn as HTMLElement).dataset.tab as TabName;
            this.tabButtons.set(tabId, btn as HTMLElement);
            btn.addEventListener('click', () => this.switchTab(tabId));
        });

        // Fetch analytics data
        this.fetchData();
    }

    getElement(): HTMLElement {
        return this.element;
    }

    private async fetchData(): Promise<void> {
        if (this.loading) return;
        this.loading = true;

        try {
            const resp = await fetch('/api/analytics');
            if (resp.ok) {
                this.data = (await resp.json()) as AnalyticsBundle;
                this.renderCurrentTab();
            }
        } catch {
            // API not available
        } finally {
            this.loading = false;
        }
    }

    private switchTab(tab: TabName): void {
        this.activeTab = tab;

        // Update active button
        this.tabButtons.forEach((btn, id) => {
            btn.classList.toggle('active', id === tab);
        });

        this.renderCurrentTab();
    }

    private renderCurrentTab(): void {
        if (this.activeTab === 'patterns') {
            this.chartContainer.style.display = 'none';
            this.patternsList.style.display = 'block';
            this.renderPatternsList();
            return;
        }

        this.chartContainer.style.display = 'block';
        this.patternsList.style.display = 'none';

        // Destroy old chart
        if (this.chart) {
            this.chart.destroy();
            this.chart = null;
        }

        if (!this.data) {
            return;
        }

        switch (this.activeTab) {
            case 'kalign':
                this.renderKAlignChart();
                break;
            case 'keys':
                this.renderKeysChart();
                break;
            case 'hebrew':
                this.renderHebrewChart();
                break;
            case 'work':
                this.renderWorkChart();
                break;
        }
    }

    private renderKAlignChart(): void {
        const records = this.data?.kAlignHistory ?? [];

        if (records.length === 0) {
            this.renderEmpty('No K:ALIGN data for current session');
            return;
        }

        const labels = records.map((_r, i) => `#${i + 1}`);
        const values = records.map((r) => r.KAlignValue);

        this.chart = new Chart(this.canvas, {
            type: 'line',
            data: {
                labels,
                datasets: [{
                    label: 'K:ALIGN',
                    data: values,
                    borderColor: '#4fc3f7',
                    backgroundColor: 'rgba(79, 195, 247, 0.1)',
                    fill: true,
                    tension: 0.3,
                    pointRadius: 3,
                    pointBackgroundColor: '#4fc3f7',
                }],
            },
            options: this.getChartOptions('K:ALIGN Evolution', -1, 1),
        });
    }

    private renderKeysChart(): void {
        const agg = this.data?.aggregates;
        if (!agg?.commandFrequencies || Object.keys(agg.commandFrequencies).length === 0) {
            this.renderEmpty('No key distribution data yet');
            return;
        }

        const entries = Object.entries(agg.commandFrequencies).sort((a, b) => b[1] - a[1]);
        const labels = entries.map(([k]) => k);
        const values = entries.map(([, v]) => v);

        const colors = labels.map((k) => {
            if (k.includes('+') || k === 'expansion') return '#66bb6a';
            if (k.includes('-') || k === 'finality') return '#ef5350';
            return '#4fc3f7';
        });

        this.chart = new Chart(this.canvas, {
            type: 'bar',
            data: {
                labels,
                datasets: [{
                    label: 'Frequency',
                    data: values,
                    backgroundColor: colors.map((c) => c + '80'),
                    borderColor: colors,
                    borderWidth: 1,
                }],
            },
            options: this.getChartOptions('Key / Command Distribution'),
        });
    }

    private renderHebrewChart(): void {
        const transitions = this.data?.hebrewTransitions ?? [];

        if (transitions.length === 0) {
            // Fall back to aggregate anchor frequencies
            const agg = this.data?.aggregates;
            if (agg?.anchorFrequencies && Object.keys(agg.anchorFrequencies).length > 0) {
                this.renderAnchorFrequencies(agg);
                return;
            }
            this.renderEmpty('No Hebrew state transitions yet');
            return;
        }

        // Count state occurrences from transitions
        const stateCounts = new Map<string, number>();
        for (const t of transitions) {
            stateCounts.set(t.ToState, (stateCounts.get(t.ToState) ?? 0) + 1);
        }

        const stateOrder = ['shavar', 'chaser', 'ratsah', 'yashar', 'tamim', 'shalem', 'tov'];
        const stateColors: Record<string, string> = {
            shavar: '#ef5350', chaser: '#ffa726', ratsah: '#ffa726',
            yashar: '#4fc3f7', tamim: '#ce93d8', shalem: '#66bb6a', tov: '#ffd54f',
        };

        const labels = stateOrder.filter((s) => stateCounts.has(s));
        const values = labels.map((s) => stateCounts.get(s) ?? 0);
        const colors = labels.map((s) => stateColors[s] ?? '#4fc3f7');

        this.chart = new Chart(this.canvas, {
            type: 'doughnut',
            data: {
                labels: labels.map((s) => s.charAt(0).toUpperCase() + s.slice(1)),
                datasets: [{
                    data: values,
                    backgroundColor: colors.map((c) => c + '80'),
                    borderColor: colors,
                    borderWidth: 2,
                }],
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: {
                        position: 'right',
                        labels: { color: '#a0a0a0', font: { size: 11 } },
                    },
                    title: {
                        display: true,
                        text: 'Hebrew State Distribution',
                        color: '#e0e0e0',
                        font: { size: 13 },
                    },
                },
            },
        });
    }

    private renderAnchorFrequencies(agg: AggregatesSummary): void {
        const entries = Object.entries(agg.anchorFrequencies).sort((a, b) => b[1] - a[1]);
        const labels = entries.map(([k]) => k);
        const values = entries.map(([, v]) => v);

        this.chart = new Chart(this.canvas, {
            type: 'bar',
            data: {
                labels,
                datasets: [{
                    label: 'Visits',
                    data: values,
                    backgroundColor: 'rgba(79, 195, 247, 0.5)',
                    borderColor: '#4fc3f7',
                    borderWidth: 1,
                }],
            },
            options: this.getChartOptions('Anchor Position Frequency'),
        });
    }

    private renderWorkChart(): void {
        const workPatterns = this.data?.workPatterns;
        if (!workPatterns || Object.keys(workPatterns).length === 0) {
            this.renderEmpty('No activity pattern data yet');
            return;
        }

        // Work patterns: day (0-6) -> hour (0-23) -> count
        // Show as bar chart of hourly totals
        const hourTotals = new Array(24).fill(0);
        for (const dayData of Object.values(workPatterns)) {
            for (const [hour, count] of Object.entries(dayData)) {
                hourTotals[parseInt(hour)] += count;
            }
        }

        const labels = hourTotals.map((_, i) => `${i}:00`);
        const maxVal = Math.max(...hourTotals);
        const colors = hourTotals.map((v) => {
            const intensity = maxVal > 0 ? v / maxVal : 0;
            return `rgba(79, 195, 247, ${0.2 + intensity * 0.6})`;
        });

        this.chart = new Chart(this.canvas, {
            type: 'bar',
            data: {
                labels,
                datasets: [{
                    label: 'Session Activity',
                    data: hourTotals,
                    backgroundColor: colors,
                    borderColor: '#4fc3f7',
                    borderWidth: 1,
                }],
            },
            options: this.getChartOptions('Hourly Activity Distribution'),
        });
    }

    private renderPatternsList(): void {
        const patterns = this.data?.activePatterns ?? [];

        if (patterns.length === 0) {
            this.patternsList.innerHTML = '<div class="analytics-empty">No active patterns detected</div>';
            return;
        }

        let html = '';
        for (const p of patterns) {
            const confidence = Math.round(p.Confidence * 100);
            const confColor = confidence >= 70 ? 'var(--accent-green)'
                : confidence >= 40 ? 'var(--accent-gold)'
                : 'var(--text-muted)';

            html += `
                <div class="pattern-card">
                    <div class="pattern-header">
                        <span class="pattern-type">${this.escapeHtml(p.PatternType)}</span>
                        <span class="pattern-confidence" style="color:${confColor}">${confidence}%</span>
                    </div>
                    <div class="pattern-key">${this.escapeHtml(p.PatternKey)}</div>
                    <div class="pattern-desc">${this.escapeHtml(p.Description)}</div>
                    <div class="pattern-meta">
                        <span>${p.OccurrenceCount} occurrences</span>
                    </div>
                </div>
            `;
        }
        this.patternsList.innerHTML = html;
    }

    private renderEmpty(message: string): void {
        if (this.chart) {
            this.chart.destroy();
            this.chart = null;
        }
        // Clear canvas and show message
        const ctx = this.canvas.getContext('2d');
        if (ctx) {
            ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
            ctx.fillStyle = '#6a7080';
            ctx.font = '12px system-ui';
            ctx.textAlign = 'center';
            ctx.fillText(message, this.canvas.width / 2, this.canvas.height / 2);
        }
    }

    private getChartOptions(title: string, suggestedMin?: number, suggestedMax?: number): Record<string, unknown> {
        return {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: { display: false },
                title: {
                    display: true,
                    text: title,
                    color: '#e0e0e0',
                    font: { size: 13 },
                },
            },
            scales: {
                x: {
                    ticks: { color: '#6a7080', font: { size: 10 } },
                    grid: { color: 'rgba(255,255,255,0.03)' },
                },
                y: {
                    ticks: { color: '#6a7080', font: { size: 10 } },
                    grid: { color: 'rgba(255,255,255,0.05)' },
                    suggestedMin,
                    suggestedMax,
                },
            },
        };
    }

    private escapeHtml(text: string): string {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }
}
