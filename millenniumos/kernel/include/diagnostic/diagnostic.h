// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-diagnostic
// Title:       MillenniumOS Diagnostic Service
// Type:        Header
// Component:   Kernel / Diagnostic Subsystem
// Role:        Assertions, probes, state queries, visual debugging
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Demo-based verification system. Not tests with pass/fail -
//              assertions for invariants, visual demos for debugging,
//              and queryable diagnostic services.
//
// Grounding:   Proverbs 27:23 — "Be thou diligent to know the state of thy flocks"
//              Know your system's state. Observe, understand, diagnose.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MILLENNIUMOS_DIAGNOSTIC_H
#define MILLENNIUMOS_DIAGNOSTIC_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"
#include "services/graphics/graphics.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Assertion Macros
// -----------------------------------------------------------------------------
//
// Assertions are NOT tests. They verify INVARIANTS - things that MUST be true.
// When an assertion fails, the system halts with diagnostic information.
// This is fail-fast: better to halt than corrupt state.
//

// Core assertion - halts with file/line/message
#define MOS_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            mos_diag_assertion_failed(__FILE__, __LINE__, #cond, msg); \
        } \
    } while (0)

// Equality assertion
#define MOS_ASSERT_EQ(a, b, msg) \
    do { \
        if ((a) != (b)) { \
            mos_diag_assert_eq_failed(__FILE__, __LINE__, #a, #b, \
                                       (int64_t)(a), (int64_t)(b), msg); \
        } \
    } while (0)

// Not-equal assertion
#define MOS_ASSERT_NE(a, b, msg) \
    do { \
        if ((a) == (b)) { \
            mos_diag_assert_ne_failed(__FILE__, __LINE__, #a, #b, \
                                       (int64_t)(a), msg); \
        } \
    } while (0)

// Range assertion (value in [min, max])
#define MOS_ASSERT_RANGE(val, min, max, msg) \
    do { \
        if ((val) < (min) || (val) > (max)) { \
            mos_diag_assert_range_failed(__FILE__, __LINE__, #val, \
                                          (int64_t)(val), (int64_t)(min), \
                                          (int64_t)(max), msg); \
        } \
    } while (0)

// Null check assertion
#define MOS_ASSERT_NOT_NULL(ptr, msg) \
    do { \
        if ((ptr) == NULL) { \
            mos_diag_assert_null_failed(__FILE__, __LINE__, #ptr, msg); \
        } \
    } while (0)

// Trit value assertion (must be -1, 0, or +1)
#define MOS_ASSERT_TRIT(val, msg) \
    MOS_ASSERT_RANGE(val, -1, 1, msg)

// Health bounds assertion
#define MOS_ASSERT_HEALTH(h, msg) \
    MOS_ASSERT_RANGE(h, MOS_HEALTH_MIN, MOS_HEALTH_MAX, msg)

// -----------------------------------------------------------------------------
// B.2 Assertion Handlers (Called by macros - implement in diagnostic.c)
// -----------------------------------------------------------------------------

// These functions do NOT return - they halt the system after dumping state
void mos_diag_assertion_failed(const char* file, int line,
                                const char* cond, const char* msg)
    __attribute__((noreturn));

void mos_diag_assert_eq_failed(const char* file, int line,
                                const char* a_expr, const char* b_expr,
                                int64_t a_val, int64_t b_val, const char* msg)
    __attribute__((noreturn));

void mos_diag_assert_ne_failed(const char* file, int line,
                                const char* a_expr, const char* b_expr,
                                int64_t val, const char* msg)
    __attribute__((noreturn));

void mos_diag_assert_range_failed(const char* file, int line,
                                   const char* expr, int64_t val,
                                   int64_t min, int64_t max, const char* msg)
    __attribute__((noreturn));

void mos_diag_assert_null_failed(const char* file, int line,
                                  const char* expr, const char* msg)
    __attribute__((noreturn));

// -----------------------------------------------------------------------------
// B.3 Diagnostic Probe Types
// -----------------------------------------------------------------------------
//
// Each subsystem registers a diagnostic probe. Probes provide:
//   - State query: CPI-SI can ask "what's your current state?"
//   - Health check: Current health score
//   - Visual demo: Interactive visualization of the subsystem
//   - Invariant check: Verify all invariants hold (non-halting scan)
//

#define MOS_DIAG_MAX_PROBES     27      // 3^3 subsystem probes
#define MOS_DIAG_NAME_LEN       32      // Max probe name length

// Probe state query result
typedef struct {
    char            name[MOS_DIAG_NAME_LEN];
    mos_health_t    health;
    MosHebrewState  state;              // Cognitive state
    trit27_t        event_count;        // Events processed
    trit27_t        error_count;        // Errors encountered
    const char*     status_message;     // Human-readable status
} MosDiagState;

// Probe function types
typedef mos_result_t (*MosDiagQueryFn)(MosDiagState* out);
typedef mos_result_t (*MosDiagCheckFn)(void);  // Check invariants, return error count
typedef void (*MosDiagDemoFn)(MosGraphicsContext* ctx, MosRect bounds);

// Diagnostic probe registration
typedef struct {
    char            name[MOS_DIAG_NAME_LEN];
    MosDiagQueryFn  query;              // Query current state
    MosDiagCheckFn  check;              // Check invariants (non-halting)
    MosDiagDemoFn   demo;               // Visual demonstration
    bool            active;
    mos_health_t    last_health;
    trit27_t        query_count;        // Times queried
} MosDiagProbe;

// -----------------------------------------------------------------------------
// B.4 Diagnostic Service State
// -----------------------------------------------------------------------------

typedef struct {
    // Registered probes
    MosDiagProbe    probes[MOS_DIAG_MAX_PROBES];
    mos_size_t      probe_count;

    // Assertion statistics
    trit27_t        assertions_checked;
    trit27_t        assertions_failed;  // Should be 0 if system is healthy

    // Last assertion failure (for post-mortem)
    const char*     last_fail_file;
    int             last_fail_line;
    const char*     last_fail_msg;

    // Overall system health (aggregate of all probes)
    mos_health_t    system_health;
    MosHebrewState  system_state;

    // Demo mode
    bool            demo_active;
    mos_size_t      demo_probe_index;
} MosDiagService;

// -----------------------------------------------------------------------------
// B.5 Initialization
// -----------------------------------------------------------------------------

// Initialize diagnostic service
mos_result_t mos_diag_init(void);

// -----------------------------------------------------------------------------
// B.6 Probe Registration
// -----------------------------------------------------------------------------

// Register a diagnostic probe for a subsystem
mos_result_t mos_diag_register_probe(const char* name,
                                      MosDiagQueryFn query,
                                      MosDiagCheckFn check,
                                      MosDiagDemoFn demo);

// Unregister a probe
mos_result_t mos_diag_unregister_probe(const char* name);

// Find probe by name
MosDiagProbe* mos_diag_find_probe(const char* name);

// -----------------------------------------------------------------------------
// B.7 State Queries
// -----------------------------------------------------------------------------

// Query a specific probe's state
mos_result_t mos_diag_query(const char* probe_name, MosDiagState* out);

// Query all probes (fills array, returns count)
mos_size_t mos_diag_query_all(MosDiagState* out, mos_size_t max_count);

// Get aggregate system health
mos_health_t mos_diag_system_health(void);

// Get aggregate system cognitive state
MosHebrewState mos_diag_system_state(void);

// Format state as string for display
void mos_diag_format_state(const MosDiagState* state, char* buf, mos_size_t size);

// -----------------------------------------------------------------------------
// B.8 Invariant Checking
// -----------------------------------------------------------------------------

// Run invariant checks on all probes (non-halting)
// Returns total error count across all probes
mos_size_t mos_diag_check_all(void);

// Run invariant check on specific probe
mos_result_t mos_diag_check(const char* probe_name);

// -----------------------------------------------------------------------------
// B.9 Visual Demos
// -----------------------------------------------------------------------------

// Start demo mode (interactive visualization)
mos_result_t mos_diag_demo_start(const char* probe_name);

// Stop demo mode
void mos_diag_demo_stop(void);

// Render current demo (call each frame when demo is active)
void mos_diag_demo_render(MosGraphicsContext* ctx);

// Handle input in demo mode (returns true if input was consumed)
bool mos_diag_demo_input(const MosEvent* event);

// Cycle to next/previous probe demo
void mos_diag_demo_next(void);
void mos_diag_demo_prev(void);

// -----------------------------------------------------------------------------
// B.10 CPI-SI Integration
// -----------------------------------------------------------------------------

// Query interface for CPI-SI (returns JSON-like formatted string)
mos_result_t mos_diag_cpisi_query(const char* query, char* response, mos_size_t size);

// Log diagnostic event for CPI-SI witness
void mos_diag_log(const char* probe_name, const char* event, mos_health_t health_delta);

// -----------------------------------------------------------------------------
// B.11 Debug Output
// -----------------------------------------------------------------------------

// Print diagnostic banner to graphics context
void mos_diag_print_banner(MosGraphicsContext* ctx, int32_t x, int32_t y);

// Print probe status table
void mos_diag_print_status(MosGraphicsContext* ctx, int32_t x, int32_t y);

// Dump full diagnostic state (for crash reports)
void mos_diag_dump(void);

// -----------------------------------------------------------------------------
// B.12 Service Access
// -----------------------------------------------------------------------------

// Get diagnostic service state (read-only)
const MosDiagService* mos_diag_info(void);

// -----------------------------------------------------------------------------
// B.13 Built-in Probe Registration
// -----------------------------------------------------------------------------
//
// Each subsystem provides its own probe. Call these during kernel init
// to register probes with the diagnostic service.
//

// HAL probe - displays, input devices, events
mos_result_t mos_diag_probe_hal_register(void);

// Graphics probe - color palette, primitives, fonts
mos_result_t mos_diag_probe_gfx_register(void);

// Memory probe - frame usage, allocation stats
mos_result_t mos_diag_probe_memory_register(void);

// VFS probe - mounts, I/O stats, directory structure
mos_result_t mos_diag_probe_vfs_register(void);

// CPI-SI probe - cognitive state, choices, covenants
mos_result_t mos_diag_probe_cpisi_register(void);

// Witness probe - learning state, patterns, wisdom
mos_result_t mos_diag_probe_witness_register(void);

// Register all built-in probes at once
mos_result_t mos_diag_register_all_probes(void);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Diagnostic Service provides demo-based verification:
//
//   ASSERTIONS: Invariants that MUST hold
//     - MOS_ASSERT, MOS_ASSERT_EQ, MOS_ASSERT_RANGE
//     - Fail-fast with state dump on violation
//     - Not tests - invariant enforcement
//
//   PROBES: Per-subsystem diagnostic interfaces
//     - State query for CPI-SI
//     - Health reporting
//     - Non-halting invariant checks
//
//   DEMOS: Visual debugging
//     - Interactive visualization of subsystem state
//     - See how systems interact
//     - Help narrow down WHERE issues occur
//
// "Be thou diligent to know the state of thy flocks,
//  and look well to thy herds."
// — Proverbs 27:23
//
// Know your system. Observe it. Understand it.
// Demos aren't tests - they're windows into the system.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // MILLENNIUMOS_DIAGNOSTIC_H
