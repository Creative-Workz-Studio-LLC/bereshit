// ============================================================================
// CPI-SI Dashboard — Cube Visualization Panel
// 2D representation of the 27-position cube (3x3 XY grid at current Z)
// ============================================================================

import type { DashboardStore } from '../../state/store';
import type { StateSnapshot } from '../../state/types';

// The 9 named positions for the XY plane (3x3 grid)
// Grid is laid out as: rows = Y (top=future, bottom=past), cols = X (left=past, right=future)
const POSITION_NAMES: Record<string, string> = {
    '-1,-1': 'Origin',
    '0,-1':  'Root',
    '1,-1':  'Seed',
    '-1,0':  'Memory',
    '0,0':   'Center',
    '1,0':   'Vision',
    '-1,1':  'Anchor',
    '0,1':   'Bridge',
    '1,1':   'Horizon',
};

// Z-layer labels
const Z_LABELS: Record<string, string> = {
    '-1': 'Break (-1)',
    '0':  'Foundation (0)',
    '1':  'Build (+1)',
};

export class CubeVisualization {
    private element: HTMLElement;
    private cells: Map<string, HTMLElement> = new Map();
    private trajectoryEl: HTMLElement;
    private commandEl: HTMLElement;
    private zLayerEl: HTMLElement;

    constructor(store: DashboardStore) {
        this.element = document.createElement('div');
        this.element.className = 'panel';

        let gridHtml = '<div class="cube-grid">';
        // Rows from top (y=1/future) to bottom (y=-1/past)
        for (let y = 1; y >= -1; y--) {
            for (let x = -1; x <= 1; x++) {
                const key = `${x},${y}`;
                const name = POSITION_NAMES[key] ?? '???';
                gridHtml += `
                    <div class="cube-cell" id="cube-${x}-${y}">
                        <span class="cell-label">${name}</span>
                        <span class="cell-coord">${x},${y}</span>
                    </div>
                `;
            }
        }
        gridHtml += '</div>';

        this.element.innerHTML = `
            <div class="panel-title">Cube Position</div>
            <div class="panel-content">
                ${gridHtml}
                <div class="cube-info">
                    <div class="cube-info-item">
                        <div class="cube-info-label">Trajectory</div>
                        <div class="cube-info-value" id="cube-trajectory">B.1</div>
                    </div>
                    <div class="cube-info-item">
                        <div class="cube-info-label">Command</div>
                        <div class="cube-info-value" id="cube-command">AWAIT</div>
                    </div>
                    <div class="cube-info-item">
                        <div class="cube-info-label">Z-Layer</div>
                        <div class="cube-info-value" id="cube-zlayer">Foundation (0)</div>
                    </div>
                </div>
            </div>
        `;

        // Cache cell references
        for (let y = 1; y >= -1; y--) {
            for (let x = -1; x <= 1; x++) {
                const el = this.element.querySelector(`#cube-${x}-${y}`);
                if (el) {
                    this.cells.set(`${x},${y}`, el as HTMLElement);
                }
            }
        }

        this.trajectoryEl = this.element.querySelector('#cube-trajectory')!;
        this.commandEl = this.element.querySelector('#cube-command')!;
        this.zLayerEl = this.element.querySelector('#cube-zlayer')!;

        store.addEventListener('state-changed', ((e: CustomEvent<StateSnapshot>) => {
            this.update(e.detail);
        }) as EventListener);
    }

    getElement(): HTMLElement {
        return this.element;
    }

    private update(state: StateSnapshot): void {
        // Parse anchor key "X.Y.Z"
        const parts = state.anchorKey.split('.').map(Number);
        const cx = parts[0] ?? 0;
        const cy = parts[1] ?? 0;
        const cz = parts[2] ?? 0;

        // Update grid highlighting
        this.cells.forEach((el, key) => {
            const [kx, ky] = key.split(',').map(Number);
            if (kx === cx && ky === cy) {
                el.classList.add('active');
            } else {
                el.classList.remove('active');
            }
        });

        // Trajectory section
        this.trajectoryEl.textContent = state.trajectorySection;

        // Command
        this.commandEl.textContent = state.commandKey;
        if (state.commandKey === 'HALT') {
            this.commandEl.style.color = 'var(--accent-red)';
        } else if (state.commandKey === 'PROCEED') {
            this.commandEl.style.color = 'var(--accent-green)';
        } else {
            this.commandEl.style.color = 'var(--accent-blue)';
        }

        // Z-layer
        const zKey = String(cz);
        this.zLayerEl.textContent = Z_LABELS[zKey] ?? `Z=${cz}`;
    }
}
