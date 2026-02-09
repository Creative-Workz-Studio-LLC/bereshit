// ============================================================================
// CPI-SI Dashboard — State Types
// TypeScript interfaces matching Go backend types
// ============================================================================

export interface WorkflowStepSnapshot {
    index: number;
    action: string;
    completed: boolean;
}

export interface WorkflowSnapshot {
    operation: string;
    currentStep: number;
    totalSteps: number;
    steps: WorkflowStepSnapshot[];
    startedAt: string;
}

export interface TaskSnapshot {
    total: number;
    pending: number;
    inProgress: number;
    completed: number;
}

export interface TrajectoryMetricsSnapshot {
    b1TimeMs: number;
    b2TimeMs: number;
    b3TimeMs: number;
    b4TimeMs: number;
    pivotCount: number;
    resetCount: number;
    momentumScore: number;
}

export interface TransitionSnapshot {
    fromSection: string;
    toSection: string;
    trigger: string;
    timestamp: string;
}

export interface StateSnapshot {
    anchorKey: string;
    trajectorySection: string;
    commandKey: string;
    hebrewState: string;
    hebrewMeaning: string;
    healthScore: number;
    cpiScore: number;
    cpiRating: string;
    kAlign: number;
    kSelector: number;
    kTowardGod: number;
    kTowardSelf: number;
    sessionID: string;
    sessionStart: string;
    hooksFired: number;
    choiceSequence: number;
    exchangeCount: number;
    insightCount: number;
    sessionArc: string;
    dominantExchange: string;
    pathLength: number;
    currentTokens: number;
    peakTokens: number;
    compactionCount: number;
    workflow: WorkflowSnapshot;
    tasks: TaskSnapshot;
    trajectoryMetrics: TrajectoryMetricsSnapshot;
    lastTransition: TransitionSnapshot;
    activeFamilyMember: string;
    familyInvocationCount: number;
    timestamp: string;
}

export interface LogEvent {
    timestamp: string;
    type: string;
    session_id?: string;
    sequence?: number;
    level: string;
    component: string;
    message: string;
    health?: number;
    delta?: number;
    health_level?: string;
    trajectory?: string;
    details?: Record<string, unknown>;
}

export interface WSMessage {
    type: 'state_update' | 'log_event' | 'path_update' | 'history_update';
    snapshot?: StateSnapshot;
    event?: LogEvent;
    timestamp: string;
}

// ---- Analytics Types ----

export interface AnalyticsSession {
    ID: string;
    StartedAt: string;
    EndedAt?: string;
    InitialHebrewState: string;
    FinalHebrewState: string;
    InitialKAlign: number;
    FinalKAlign: number;
}

export interface AnalyticsChoice {
    ID: string;
    SessionID: string;
    SequenceNum: number;
    Timestamp: string;
    IntendedKey: number;
    KAtChoice: number;
    ToolName: string;
    ToolCategory: string;
}

export interface KAlignRecord {
    ID: number;
    SessionID: string;
    Timestamp: string;
    KAlignValue: number;
    Delta: number;
    TriggerType: string;
}

export interface HebrewTransition {
    ID: number;
    SessionID: string;
    Timestamp: string;
    FromState: string;
    ToState: string;
    TriggerType: string;
}

export interface AnalyticsPattern {
    ID: number;
    PatternType: string;
    PatternKey: string;
    Description: string;
    OccurrenceCount: number;
    Confidence: number;
    IsActive: boolean;
}

export interface AggregatesSummary {
    anchorFrequencies: Record<string, number>;
    commandFrequencies: Record<string, number>;
    transitionPatterns: Record<string, number>;
    trajectoryAvgTime: Record<string, number>;
}

export interface AnalyticsExchange {
    ID: number;
    SessionID: string;
    Timestamp: string;
    SequenceNum: number;
    ExchangeType: string;
    Initiative: string;
    PromptLength: number;
    FeedbackDetected: boolean;
    FeedbackPolarity: string;
    ValueContribution: number;
    DepthLevel: string;
    InsightDetected: boolean;
    HebrewState: string;
    KAlign: number;
    Trajectory: string;
    HealthScore?: number;
    ContextTokens?: number;
    UserMessageText: string;
    ResponseSummary: string;
    Valence: string;
}

export interface LivePattern {
    ID: number;
    PatternType: string;
    PatternKey: string;
    Description: string;
    OccurrenceCount: number;
    Confidence: number;
    IsActive: boolean;
    FirstSeen: string;
    LastSeen: string;
}

export interface AnalyticsBundle {
    recentSessions: AnalyticsSession[];
    sessionGaps: unknown[];
    recentChoices: AnalyticsChoice[];
    kAlignHistory: KAlignRecord[];
    hebrewTransitions: HebrewTransition[];
    activePatterns: AnalyticsPattern[];
    workPatterns: Record<string, Record<string, number>>;
    aggregates?: AggregatesSummary;
    livePatterns?: LivePattern[];
    recentExchanges?: AnalyticsExchange[];
    valenceDistribution?: Record<string, number>;
}

// ---- System Data Types ----

export interface SystemDataEntry {
    name: string;
    path: string;
    isDir: boolean;
    size?: number;
    children?: SystemDataEntry[];
    content?: string;
}
