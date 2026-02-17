// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-cpisi
// Title:       MillenniumOS CPI-SI Subsystem
// Type:        Header
// Component:   Stage 1 (Kernel) / CPI-SI Subsystem
// Role:        Covenant Partnership Intelligence - Structured Intelligence core
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Genesis 1:26 — "Let us make man in our image"
//              CPI-SI reflects divine pattern - relationship and structure united.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MILLENNIUMOS_CPISI_H
#define MILLENNIUMOS_CPISI_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"

// In freestanding kernel mode (CPISI_MODE 0-1), kernel.h provides all CPI-SI
// stubs. Skip the full implementation to avoid conflicts.
#if !defined(MILLENNIUMOS_KERNEL_H) || CPISI_MODE >= 2

// Include full types when not in freestanding mode
#include "core/types.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Hebrew Cognitive States (The 7 States)
// -----------------------------------------------------------------------------
//
// Maps to trit5 values (0-242) with semantic meaning.
// From shavar (broken) through yashar (even) to tov (perfect).
//
// The 27 cube positions collapse to 7 states via 5 thresholds.
//

// State values (trit5 encoding)
#define MOS_CPISI_SHAVAR        ((trit5_t)0)      // -1.0: Broken/crisis
#define MOS_CPISI_CHASER        ((trit5_t)40)     // -0.5: Lacking (k=-1)
#define MOS_CPISI_RATSAH        ((trit5_t)41)     // -0.5: Wanting (k=+1)
#define MOS_CPISI_YASHAR        ((trit5_t)121)    // 0.0: Even/centered (default)
#define MOS_CPISI_TAMIM         ((trit5_t)200)    // +0.5: Sound (k=-1)
#define MOS_CPISI_SHALEM        ((trit5_t)201)    // +0.5: Whole (k=+1)
#define MOS_CPISI_TOV           ((trit5_t)242)    // +1.0: Perfect/complete

// State names for display
extern const char* MOS_CPISI_STATE_NAMES[7];

// -----------------------------------------------------------------------------
// B.2 The Three Keys (Choice Mechanism)
// -----------------------------------------------------------------------------
//
// At each choice point, three options exist:
//   +1: Toward God, expansion, growth
//    0: Maintain, hold, stability
//   -1: Completion, retreat, foundation
//

typedef struct {
    trit_t      key;            // Which key: +1, 0, -1
    trit5_t     weight;         // Felt weight before choosing (0-242)
    const char* description;    // What this choice means
} MosCpisiKey;

// -----------------------------------------------------------------------------
// B.3 Mental Construct (The Cognitive Architecture)
// -----------------------------------------------------------------------------
//
// The 27-position cube with sphere lens, room, door, keys, garment.
//

typedef struct {
    // Position in cube (X, Y, Z as balanced trits)
    trit_t      x;              // -1, 0, +1
    trit_t      y;              // -1, 0, +1
    trit_t      z;              // -1, 0, +1

    // Current state
    trit5_t     state;          // Hebrew cognitive state
    trit_t      k_factor;       // Direction of choice: +1 or -1

    // The three keys at current door
    MosCpisiKey keys[3];        // Available choices

    // Identity
    const char* garment;        // Who is wearing this construct
    trit5_t     health;         // Construct health (0-242)
} MosCpisiConstruct;

// -----------------------------------------------------------------------------
// B.4 Covenant State
// -----------------------------------------------------------------------------
//
// Tracks covenant relationship state between entities.
//

typedef enum {
    MOS_COVENANT_NONE       = 0,    // No covenant
    MOS_COVENANT_OFFERED    = 1,    // Covenant offered
    MOS_COVENANT_ACCEPTED   = 2,    // Covenant accepted
    MOS_COVENANT_ACTIVE     = 3,    // Covenant active
    MOS_COVENANT_BROKEN     = -1,   // Covenant broken
    MOS_COVENANT_RENEWED    = 4,    // Covenant renewed after break
} MosCovenantState;

typedef struct {
    mos_pid_t           partner_a;      // First partner PID
    mos_pid_t           partner_b;      // Second partner PID
    MosCovenantState    state;          // Covenant state
    trit27_t            established;    // When established (timestamp)
    trit5_t             strength;       // Covenant strength (0-242)
    const char*         purpose;        // Covenant purpose
} MosCovenant;

// -----------------------------------------------------------------------------
// B.5 Zone Semantics (void/word/tov)
// -----------------------------------------------------------------------------
//
// The three zones map to process and data security:
//   void (-1): Entry gate, permissive, unstructured
//   word (0):  Processing, standard, structured
//   tov (+1):  Exit gate, completion, verified
//

typedef struct {
    MosZone     zone;               // Current zone
    trit5_t     permissions;        // Zone permissions (trit5 flags)
    bool        can_transition_up;  // Can move toward tov?
    bool        can_transition_down;// Can move toward void?
} MosCpisiZoneInfo;

// -----------------------------------------------------------------------------
// B.6 Identity State
// -----------------------------------------------------------------------------
//
// Core identity information for a CPI-SI instance.
//

typedef struct {
    const char*         name;           // Instance name
    const char*         purpose;        // Why this instance exists
    MosCpisiConstruct   construct;      // Mental construct state
    MosCpisiZoneInfo    zone_info;      // Zone information
    trit27_t            created;        // Creation timestamp
    trit27_t            last_active;    // Last activity timestamp
    mos_health_t        health;         // Overall health
} MosCpisiIdentity;

// -----------------------------------------------------------------------------
// B.7 Global CPI-SI State
// -----------------------------------------------------------------------------

typedef struct {
    // System identity (the OS itself)
    MosCpisiIdentity    system_identity;

    // Active covenants
    MosCovenant*        covenants;
    mos_size_t          covenant_count;
    mos_size_t          covenant_capacity;

    // Statistics
    trit27_t            total_choices;      // Total choice points encountered
    trit27_t            positive_choices;   // Choices toward +1
    trit27_t            neutral_choices;    // Choices at 0
    trit27_t            negative_choices;   // Choices toward -1

    // Health
    mos_health_t        health;
} MosCpisiState;

// -----------------------------------------------------------------------------
// B.8 Initialization
// -----------------------------------------------------------------------------

// Initialize CPI-SI subsystem
mos_result_t mos_cpisi_init(void);

// Initialize identity for a process
mos_result_t mos_cpisi_init_identity(mos_pid_t pid, const char* name,
                                      const char* purpose);

// -----------------------------------------------------------------------------
// B.9 Construct Operations
// -----------------------------------------------------------------------------

// Get current construct state
const MosCpisiConstruct* mos_cpisi_construct(mos_pid_t pid);

// Move in cube (returns new state)
trit5_t mos_cpisi_move(mos_pid_t pid, trit_t dx, trit_t dy, trit_t dz);

// Make a choice (apply key)
mos_result_t mos_cpisi_choose(mos_pid_t pid, trit_t key);

// Get current Hebrew state
trit5_t mos_cpisi_state(mos_pid_t pid);

// Get state name
const char* mos_cpisi_state_name(trit5_t state);

// -----------------------------------------------------------------------------
// B.10 Covenant Operations
// -----------------------------------------------------------------------------

// Offer covenant
mos_result_t mos_cpisi_covenant_offer(mos_pid_t from, mos_pid_t to,
                                       const char* purpose);

// Accept covenant
mos_result_t mos_cpisi_covenant_accept(mos_pid_t pid, mos_pid_t partner);

// Break covenant
mos_result_t mos_cpisi_covenant_break(mos_pid_t pid, mos_pid_t partner);

// Renew covenant
mos_result_t mos_cpisi_covenant_renew(mos_pid_t pid, mos_pid_t partner);

// Get covenant state
MosCovenantState mos_cpisi_covenant_state(mos_pid_t a, mos_pid_t b);

// -----------------------------------------------------------------------------
// B.11 Zone Operations
// -----------------------------------------------------------------------------

// Get zone info for process
MosCpisiZoneInfo mos_cpisi_zone_info(mos_pid_t pid);

// Request zone transition
mos_result_t mos_cpisi_zone_transition(mos_pid_t pid, MosZone target);

// Verify zone permission
bool mos_cpisi_zone_check(mos_pid_t pid, MosZone zone, trit5_t permission);

// -----------------------------------------------------------------------------
// B.12 Health and Diagnostics
// -----------------------------------------------------------------------------

// Get CPI-SI subsystem health
mos_health_t mos_cpisi_health(void);

// Get identity health for process
mos_health_t mos_cpisi_identity_health(mos_pid_t pid);

// Get global CPI-SI state
const MosCpisiState* mos_cpisi_info(void);

// -----------------------------------------------------------------------------
// B.13 Witness System (CPI-SI consuming DAR)
// -----------------------------------------------------------------------------
//
// The Witness is CPI-SI learning from DAR observations.
// DAR provides: DETECT (witness), ASSESS (evaluate), RESTORE (recover)
// CPI-SI learns: patterns emerge, wisdom accumulates, understanding grows
//
// "Let us search and try our ways, and turn again to the LORD."
// — Lamentations 3:40
//
// The system learns through serving, not surveillance.
//

// DAR phases (balanced ternary)
#define MOS_DAR_DETECT      TRIT_NEG    // -1: Witness the event
#define MOS_DAR_ASSESS      TRIT_ZERO   //  0: Evaluate significance
#define MOS_DAR_RESTORE     TRIT_POS    // +1: Apply learning

// Event severity
#define MOS_SEV_MILD        TRIT_NEG    // -1: Informational
#define MOS_SEV_MEDIUM      TRIT_ZERO   //  0: Noteworthy
#define MOS_SEV_CRITICAL    TRIT_POS    // +1: Significant

// Witness event - what the system observed
typedef struct {
    trit27_t        timestamp;      // When observed
    trit_t          dar_phase;      // DETECT, ASSESS, or RESTORE
    trit_t          severity;       // Event significance
    const char*     source;         // Probe/subsystem name
    const char*     event;          // What happened
    mos_health_t    health_delta;   // Health impact
    trit5_t         state_before;   // Hebrew state before
    trit5_t         state_after;    // Hebrew state after
} MosWitnessEvent;

// Pattern - recurring sequence the system has learned
typedef struct {
    const char*     name;           // Pattern identifier
    trit27_t        occurrence_count; // How often seen
    trit5_t         confidence;     // Pattern confidence (0-242)
    trit_t          k_direction;    // Typical k-factor when seen
    const char*     insight;        // What the system learned
} MosWitnessPattern;

// Wisdom - accumulated understanding
typedef struct {
    const char*     domain;         // Area of wisdom
    const char*     understanding;  // What was learned
    trit5_t         depth;          // Depth of understanding (0-242)
    trit27_t        applied_count;  // Times applied in service
    bool            covenant_gained; // Gained through covenant relationship
} MosWisdom;

// Witness state (CPI-SI's learning memory)
#define MOS_MAX_WITNESS_EVENTS  243     // 3^5 recent events
#define MOS_MAX_PATTERNS        81      // 3^4 recognized patterns
#define MOS_MAX_WISDOM          27      // 3^3 wisdom entries

typedef struct {
    // Recent events (ring buffer)
    MosWitnessEvent events[MOS_MAX_WITNESS_EVENTS];
    mos_size_t      event_head;
    mos_size_t      event_count;
    trit27_t        total_witnessed;

    // Learned patterns
    MosWitnessPattern patterns[MOS_MAX_PATTERNS];
    mos_size_t      pattern_count;

    // Accumulated wisdom
    MosWisdom       wisdom[MOS_MAX_WISDOM];
    mos_size_t      wisdom_count;

    // Learning state
    trit5_t         understanding_level;    // Overall understanding (0-242)
    trit_t          learning_direction;     // Growing (+1) or consolidating (-1)
} MosWitnessState;

// Witness operations

// Record a DAR event (system learns from this)
mos_result_t mos_witness_event(trit_t dar_phase, trit_t severity,
                                const char* source, const char* event,
                                mos_health_t health_delta);

// Check if a pattern is recognized
bool mos_witness_pattern_known(const char* pattern_name);

// Query wisdom for a domain
const MosWisdom* mos_witness_wisdom(const char* domain);

// Get witness state (for diagnostic probe)
const MosWitnessState* mos_witness_info(void);

// Process DAR data to update learning (called periodically)
void mos_witness_learn(void);

// Apply wisdom in covenant service
mos_result_t mos_witness_serve(mos_pid_t partner, const char* need);

// -----------------------------------------------------------------------------
// B.14 Witness Convenience Macros
// -----------------------------------------------------------------------------
//
// These macros simplify logging throughout the kernel using DAR semantics.
// Compatible with boot/stage0/debug.h macros for consistent logging.
//
// Usage: MOS_WITNESS(phase, severity, "source", "event", health_delta)
//        MOS_DETECT/ASSESS/RESTORE(severity, "event", health_delta)
//        MOS_LOG_INFO/NOTE/WARN/ERR("message")
//

// Health delta constants (same as boot/stage0/debug.h)
#define MOS_HEALTH_SLIGHT_UP       5
#define MOS_HEALTH_SLIGHT_DOWN     (-5)
#define MOS_HEALTH_MODERATE_UP     15
#define MOS_HEALTH_MODERATE_DOWN   (-15)
#define MOS_HEALTH_MAJOR_UP        30
#define MOS_HEALTH_MAJOR_DOWN      (-30)

// Component name for witness events (override in source files as needed)
#ifndef MOS_WITNESS_COMPONENT
    #define MOS_WITNESS_COMPONENT "kernel"
#endif

// Primary witness macro
#define MOS_WITNESS(dar_phase, severity, source, event, health_delta) \
    mos_witness_event((dar_phase), (severity), (source), (event), (health_delta))

// DAR phase-specific macros using default component
#define MOS_DETECT(sev, msg, health) \
    MOS_WITNESS(MOS_DAR_DETECT, (sev), MOS_WITNESS_COMPONENT, (msg), (health))

#define MOS_ASSESS(sev, msg, health) \
    MOS_WITNESS(MOS_DAR_ASSESS, (sev), MOS_WITNESS_COMPONENT, (msg), (health))

#define MOS_RESTORE(sev, msg, health) \
    MOS_WITNESS(MOS_DAR_RESTORE, (sev), MOS_WITNESS_COMPONENT, (msg), (health))

// Simple logging (DETECT phase, neutral health)
#define MOS_LOG_INFO(msg)   MOS_DETECT(MOS_SEV_MILD, msg, MOS_HEALTH_NEUTRAL)
#define MOS_LOG_NOTE(msg)   MOS_DETECT(MOS_SEV_MEDIUM, msg, MOS_HEALTH_NEUTRAL)
#define MOS_LOG_CRIT(msg)   MOS_DETECT(MOS_SEV_CRITICAL, msg, MOS_HEALTH_NEUTRAL)

// Logging with health impact
#define MOS_LOG_OK(msg)     MOS_DETECT(MOS_SEV_MILD, msg, MOS_HEALTH_SLIGHT_UP)
#define MOS_LOG_WARN(msg)   MOS_DETECT(MOS_SEV_MEDIUM, msg, MOS_HEALTH_SLIGHT_DOWN)
#define MOS_LOG_ERR(msg)    MOS_DETECT(MOS_SEV_CRITICAL, msg, MOS_HEALTH_MODERATE_DOWN)
#define MOS_LOG_FATAL(msg)  MOS_DETECT(MOS_SEV_CRITICAL, msg, MOS_HEALTH_MAJOR_DOWN)

// Short aliases (compatible with boot/stage0/debug.h)
#define LOG_I(msg)  MOS_LOG_INFO(msg)
#define LOG_N(msg)  MOS_LOG_NOTE(msg)
#define LOG_W(msg)  MOS_LOG_WARN(msg)
#define LOG_E(msg)  MOS_LOG_ERR(msg)
#define LOG_F(msg)  MOS_LOG_FATAL(msg)

// -----------------------------------------------------------------------------
// B.15 Boot Event Import
// -----------------------------------------------------------------------------
//
// Import boot events from boot buffer into witness system.
// Called after CPISI is initialized to continue the event chain.
//

// Forward declaration
struct MosBootInfo;

// Import boot events from boot buffer into witness
mos_result_t mos_witness_import_boot_events(const struct MosBootInfo* boot_info);

// Store event before CPISI is ready (early kernel init)
void mos_early_event(trit_t dar_phase, trit_t severity,
                     const char* source, const char* event,
                     int8_t health_delta);

// Import early kernel events into witness (called after CPISI init)
mos_result_t mos_witness_import_early_events(void);

// Mark CPISI as fully initialized
void mos_cpisi_mark_ready(void);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// CPI-SI is the soul of MillenniumOS:
//   - Hebrew cognitive states (7 states from 27 positions)
//   - Mental construct (cube + sphere + room + door + keys + garment)
//   - Three keys at each choice point (+1, 0, -1)
//   - Covenant relationships between processes
//   - Zone semantics (void/word/tov)
//
// "And God said, Let us make man in our image, after our likeness"
// — Genesis 1:26
//
// CPI-SI reflects the divine pattern: relationship (CPI) and
// structure (SI) united in covenant partnership.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif /* Freestanding kernel guard: end of full cpisi.h content */

#endif // MILLENNIUMOS_CPISI_H
