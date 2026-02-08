// ============================================================================
// CPI-SI Dashboard — Reactive Store
// Simple EventTarget-based reactive state management
// ============================================================================

import type { StateSnapshot, LogEvent } from './types';

const MAX_LOG_EVENTS = 500;

export class DashboardStore extends EventTarget {
    private _state: StateSnapshot | null = null;
    private _events: LogEvent[] = [];
    private _connected = false;

    get state(): StateSnapshot | null {
        return this._state;
    }

    get events(): readonly LogEvent[] {
        return this._events;
    }

    get connected(): boolean {
        return this._connected;
    }

    updateState(snapshot: StateSnapshot): void {
        this._state = snapshot;
        this.dispatchEvent(new CustomEvent('state-changed', { detail: snapshot }));
    }

    addEvent(event: LogEvent): void {
        this._events.push(event);
        if (this._events.length > MAX_LOG_EVENTS) {
            this._events = this._events.slice(-MAX_LOG_EVENTS);
        }
        this.dispatchEvent(new CustomEvent('events-changed', { detail: event }));
    }

    setConnected(connected: boolean): void {
        this._connected = connected;
        this.dispatchEvent(new CustomEvent('connection-changed', { detail: connected }));
    }
}

export function createDefaultState(): StateSnapshot {
    return {
        anchorKey: '0.0.0',
        trajectorySection: 'B.1',
        commandKey: 'AWAIT',
        hebrewState: 'yashar',
        hebrewMeaning: 'EVEN',
        healthScore: 0,
        cpiScore: 0.5,
        cpiRating: 'Neutral',
        kAlign: 0,
        kSelector: 0,
        kTowardGod: 0,
        kTowardSelf: 0,
        sessionID: '',
        sessionStart: new Date().toISOString(),
        hooksFired: 0,
        choiceSequence: 0,
        exchangeCount: 0,
        insightCount: 0,
        sessionArc: 'unknown',
        dominantExchange: 'none',
        pathLength: 0,
        currentTokens: 0,
        peakTokens: 0,
        compactionCount: 0,
        workflow: {
            operation: '',
            currentStep: 0,
            totalSteps: 0,
            steps: [],
            startedAt: '',
        },
        tasks: {
            total: 0,
            pending: 0,
            inProgress: 0,
            completed: 0,
        },
        trajectoryMetrics: {
            b1TimeMs: 0,
            b2TimeMs: 0,
            b3TimeMs: 0,
            b4TimeMs: 0,
            pivotCount: 0,
            resetCount: 0,
            momentumScore: 0,
        },
        lastTransition: {
            fromSection: '',
            toSection: '',
            trigger: '',
            timestamp: '',
        },
        activeFamilyMember: '',
        familyInvocationCount: 0,
        timestamp: new Date().toISOString(),
    };
}
