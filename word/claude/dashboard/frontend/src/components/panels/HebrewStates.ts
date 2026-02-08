// ============================================================================
// CPI-SI Dashboard — Hebrew States Panel
// 7 Hebrew states displayed vertically: shavar(-1) to tov(+1)
// ============================================================================

import type { DashboardStore } from '../../state/store';
import type { StateSnapshot } from '../../state/types';

interface HebrewStateInfo {
    name: string;
    meaning: string;
    value: string;
    cssClass: string;
}

// States ordered from tov (top) to shavar (bottom) for display
const STATES: HebrewStateInfo[] = [
    { name: 'tov',    meaning: 'PERFECT', value: '+1.0',  cssClass: 'tov' },
    { name: 'shalem', meaning: 'WHOLE',   value: '+0.5',  cssClass: 'shalem' },
    { name: 'tamim',  meaning: 'SOUND',   value: '+0.5',  cssClass: 'tamim' },
    { name: 'yashar', meaning: 'EVEN',    value: '0.0',   cssClass: 'yashar' },
    { name: 'ratsah', meaning: 'WANTING', value: '-0.5',  cssClass: 'ratsah' },
    { name: 'chaser', meaning: 'LACKING', value: '-0.5',  cssClass: 'chaser' },
    { name: 'shavar', meaning: 'BROKEN',  value: '-1.0',  cssClass: 'shavar' },
];

export class HebrewStates {
    private element: HTMLElement;
    private rows: Map<string, HTMLElement> = new Map();

    constructor(store: DashboardStore) {
        this.element = document.createElement('div');
        this.element.className = 'panel';

        let listHtml = '<div class="hebrew-states-list">';
        for (const state of STATES) {
            listHtml += `
                <div class="hebrew-state-row" id="hs-${state.name}">
                    <div class="state-dot ${state.cssClass}"></div>
                    <span class="state-name">${state.name}</span>
                    <span class="state-meaning">${state.meaning}</span>
                    <span class="state-value">${state.value}</span>
                </div>
            `;
        }
        listHtml += '</div>';

        this.element.innerHTML = `
            <div class="panel-title">Hebrew States</div>
            <div class="panel-content">
                ${listHtml}
            </div>
        `;

        // Cache row references
        for (const state of STATES) {
            const el = this.element.querySelector(`#hs-${state.name}`);
            if (el) {
                this.rows.set(state.name, el as HTMLElement);
            }
        }

        store.addEventListener('state-changed', ((e: CustomEvent<StateSnapshot>) => {
            this.update(e.detail);
        }) as EventListener);
    }

    getElement(): HTMLElement {
        return this.element;
    }

    private update(state: StateSnapshot): void {
        const currentState = state.hebrewState.toLowerCase();

        this.rows.forEach((el, name) => {
            if (name === currentState) {
                el.classList.add('active');
            } else {
                el.classList.remove('active');
            }
        });
    }
}
