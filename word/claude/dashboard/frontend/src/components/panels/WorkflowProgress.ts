// ============================================================================
// CPI-SI Dashboard — Workflow Progress Panel
// Shows current operation, step checklist, and progress bar
// ============================================================================

import type { DashboardStore } from '../../state/store';
import type { StateSnapshot } from '../../state/types';

export class WorkflowProgress {
    private element: HTMLElement;
    private operationEl: HTMLElement;
    private stepCountEl: HTMLElement;
    private progressFillEl: HTMLElement;
    private stepsContainer: HTMLElement;

    constructor(store: DashboardStore) {
        this.element = document.createElement('div');
        this.element.className = 'panel';
        this.element.innerHTML = `
            <div class="panel-title">Workflow</div>
            <div class="panel-content">
                <div class="workflow-header">
                    <span class="workflow-operation" id="wf-operation">No active workflow</span>
                    <span class="workflow-step-count" id="wf-step-count"></span>
                </div>
                <div class="workflow-progress-bar">
                    <div class="workflow-progress-fill" id="wf-progress-fill" style="width: 0%;"></div>
                </div>
                <div class="workflow-steps" id="wf-steps">
                    <div class="workflow-empty">Waiting for workflow data...</div>
                </div>
            </div>
        `;

        this.operationEl = this.element.querySelector('#wf-operation')!;
        this.stepCountEl = this.element.querySelector('#wf-step-count')!;
        this.progressFillEl = this.element.querySelector('#wf-progress-fill')!;
        this.stepsContainer = this.element.querySelector('#wf-steps')!;

        store.addEventListener('state-changed', ((e: CustomEvent<StateSnapshot>) => {
            this.update(e.detail);
        }) as EventListener);
    }

    getElement(): HTMLElement {
        return this.element;
    }

    private update(state: StateSnapshot): void {
        const wf = state.workflow;

        if (!wf.operation) {
            this.operationEl.textContent = 'No active workflow';
            this.operationEl.style.color = 'var(--text-muted)';
            this.stepCountEl.textContent = '';
            this.progressFillEl.style.width = '0%';
            this.stepsContainer.innerHTML = '<div class="workflow-empty">Waiting for workflow data...</div>';
            return;
        }

        // Operation name
        this.operationEl.textContent = wf.operation;
        this.operationEl.style.color = 'var(--accent-blue)';

        // Step count
        this.stepCountEl.textContent = `Step ${wf.currentStep} of ${wf.totalSteps}`;

        // Progress bar
        const pct = wf.totalSteps > 0 ? (wf.currentStep / wf.totalSteps) * 100 : 0;
        this.progressFillEl.style.width = `${pct}%`;

        if (pct >= 100) {
            this.progressFillEl.style.background = 'var(--accent-green)';
        } else {
            this.progressFillEl.style.background = 'var(--accent-blue)';
        }

        // Steps list
        if (wf.steps.length === 0) {
            this.stepsContainer.innerHTML = '<div class="workflow-empty">No steps defined</div>';
            return;
        }

        let stepsHtml = '';
        for (let i = 0; i < wf.steps.length; i++) {
            const step = wf.steps[i]!;
            let statusClass = 'pending';
            let icon = '\u25CB'; // circle

            if (step.completed) {
                statusClass = 'completed';
                icon = '\u2713'; // checkmark
            } else if (i === wf.currentStep - 1) {
                statusClass = 'in-progress';
                icon = '\u25B6'; // play triangle
            }

            stepsHtml += `
                <div class="workflow-step ${statusClass}">
                    <span class="workflow-step-icon">${icon}</span>
                    <span>${this.escapeHtml(step.action)}</span>
                </div>
            `;
        }
        this.stepsContainer.innerHTML = stepsHtml;
    }

    private escapeHtml(text: string): string {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }
}
