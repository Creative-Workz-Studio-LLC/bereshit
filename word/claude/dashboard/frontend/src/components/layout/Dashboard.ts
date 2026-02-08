// ============================================================================
// CPI-SI Dashboard — Main Dashboard Layout
// CSS Grid layout with header, 3-column body, and footer
// ============================================================================

import type { DashboardStore } from '../../state/store';
import { StateOverview } from '../panels/StateOverview';
import { MessageTerminal } from '../panels/MessageTerminal';
import { CubeVisualization } from '../panels/CubeVisualization';
import { TrajectoryTracker } from '../panels/TrajectoryTracker';
import { HebrewStates } from '../panels/HebrewStates';
import { WorkflowProgress } from '../panels/WorkflowProgress';
import { SessionTimeline } from '../panels/SessionTimeline';
import { AnalyticsCharts } from '../panels/AnalyticsCharts';
import { SystemDataBrowser } from '../panels/SystemDataBrowser';

export class Dashboard {
    private element: HTMLElement;
    private connectionDot: HTMLElement;
    private connectionLabel: HTMLElement;

    private stateOverview: StateOverview;
    private messageTerminal: MessageTerminal;
    private cubeVisualization: CubeVisualization;
    private trajectoryTracker: TrajectoryTracker;
    private hebrewStates: HebrewStates;
    private workflowProgress: WorkflowProgress;
    private analyticsCharts: AnalyticsCharts;
    private systemDataBrowser: SystemDataBrowser;
    private sessionTimeline: SessionTimeline;

    constructor(store: DashboardStore) {
        this.element = document.createElement('div');
        this.element.className = 'dashboard';

        // Create header
        const header = document.createElement('div');
        header.className = 'dashboard-header';
        header.innerHTML = `
            <h1>CPI-SI State Machine Dashboard</h1>
            <div class="connection-status">
                <span class="connection-dot" id="conn-dot"></span>
                <span id="conn-label">Disconnected</span>
            </div>
        `;
        this.element.appendChild(header);

        this.connectionDot = header.querySelector('#conn-dot')!;
        this.connectionLabel = header.querySelector('#conn-label')!;

        // Create panels
        this.stateOverview = new StateOverview(store);
        this.messageTerminal = new MessageTerminal(store);
        this.cubeVisualization = new CubeVisualization(store);
        this.trajectoryTracker = new TrajectoryTracker(store);
        this.hebrewStates = new HebrewStates(store);
        this.workflowProgress = new WorkflowProgress(store);
        this.analyticsCharts = new AnalyticsCharts(store);
        this.systemDataBrowser = new SystemDataBrowser(store);
        this.sessionTimeline = new SessionTimeline(store);

        // Row 1: StateOverview | MessageTerminal | CubeVisualization
        this.element.appendChild(this.stateOverview.getElement());
        this.element.appendChild(this.messageTerminal.getElement());
        this.element.appendChild(this.cubeVisualization.getElement());

        // Row 2: TrajectoryTracker | HebrewStates | WorkflowProgress
        this.element.appendChild(this.trajectoryTracker.getElement());
        this.element.appendChild(this.hebrewStates.getElement());
        this.element.appendChild(this.workflowProgress.getElement());

        // Row 3: AnalyticsCharts (2 cols) | SystemDataBrowser (1 col)
        const analyticsEl = this.analyticsCharts.getElement();
        analyticsEl.style.gridColumn = '1 / 3';
        this.element.appendChild(analyticsEl);
        this.element.appendChild(this.systemDataBrowser.getElement());

        // Row 4: SessionTimeline (full width)
        const footer = this.sessionTimeline.getElement();
        footer.classList.add('dashboard-footer');
        footer.style.gridColumn = '1 / -1';
        this.element.appendChild(footer);

        // Listen for connection changes
        store.addEventListener('connection-changed', ((e: CustomEvent<boolean>) => {
            this.updateConnectionStatus(e.detail);
        }) as EventListener);
    }

    mount(target: HTMLElement): void {
        target.appendChild(this.element);
    }

    private updateConnectionStatus(connected: boolean): void {
        if (connected) {
            this.connectionDot.classList.add('connected');
            this.connectionLabel.textContent = 'Connected';
        } else {
            this.connectionDot.classList.remove('connected');
            this.connectionLabel.textContent = 'Disconnected';
        }
    }
}
