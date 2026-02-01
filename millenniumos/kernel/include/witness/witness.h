// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-witness-header
// Title:       MillenniumOS DAR Witness Header
// Type:        Header
// Component:   Stage 1 (Kernel) / Witness Subsystem
// Role:        DAR Witness API declarations for kernel use
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-02-01
// Updated:     2026-02-01
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Proverbs 24:16 — "A just man falleth seven times, and riseth up again"
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MOS_WITNESS_H
#define MOS_WITNESS_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>
#include "kernel.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Witness State Structure
// -----------------------------------------------------------------------------

/**
 * @brief Witness system state
 *
 * Tracks DAR health, Hebrew state, and event statistics.
 * Available via mos_witness_info() for diagnostics.
 */
typedef struct MosWitnessState {
    // Health tracking
    int32_t     health;             /**< Current health (-100 to +100) */
    uint8_t     hebrew_state;       /**< Hebrew state (0=SHAVAR to 6=TOV) */
    int8_t      k_factor;           /**< Direction (-1, 0, +1) */

    // Event counts by DAR phase
    uint32_t    detect_count;       /**< DETECT events logged */
    uint32_t    assess_count;       /**< ASSESS cycles run */
    uint32_t    restore_count;      /**< RESTORE actions taken */

    // Severity counts
    uint32_t    critical_count;     /**< Critical events */
    uint32_t    severe_count;       /**< Severe events */
    uint32_t    moderate_count;     /**< Moderate events */

    // State flags
    bool        initialized;        /**< Witness ready */
    bool        cpisi_ready;        /**< CPISI subsystem ready */
    bool        vfs_ready;          /**< VFS available for logging */

    // Early event buffer tracking
    uint32_t    early_head;         /**< Ring buffer head */
    uint32_t    early_tail;         /**< Ring buffer tail */
    uint32_t    early_overflow;     /**< Overflow count */
} MosWitnessState;

// -----------------------------------------------------------------------------
// B.2 Witness API
// -----------------------------------------------------------------------------

/**
 * @brief Initialize witness system
 * @return MOS_OK on success
 */
mos_result_t mos_witness_init(void);

/**
 * @brief Report early event (before CPISI ready)
 * @param phase        DAR phase (MOS_DAR_DETECT, MOS_DAR_ASSESS, MOS_DAR_RESTORE)
 * @param severity     Severity level (MOS_SEV_*)
 * @param source       Source component name
 * @param message      Event message
 * @param health_delta Health change (+/- delta)
 */
void mos_early_event_impl(int phase, int severity, const char* source,
                          const char* message, int health_delta);

/**
 * @brief Report witness event (after CPISI ready)
 * @param phase        DAR phase (MOS_DAR_DETECT, MOS_DAR_ASSESS, MOS_DAR_RESTORE)
 * @param severity     Severity level (MOS_SEV_*)
 * @param source       Source component name
 * @param message      Event message
 * @param health_delta Health change (+/- delta)
 */
void mos_witness_event_impl(int phase, int severity, const char* source,
                            const char* message, int health_delta);

/**
 * @brief Import boot events from Stage 0 buffer
 * @param boot_info Boot information with event buffer
 * @return MOS_OK on success
 */
mos_result_t mos_witness_import_boot_events_impl(const MosBootInfo* boot_info);

/**
 * @brief Import early kernel events into witness system
 * @return MOS_OK on success
 */
mos_result_t mos_witness_import_early_events_impl(void);

/**
 * @brief Mark CPISI as ready for direct witness calls
 */
void mos_cpisi_mark_ready_impl(void);

/**
 * @brief Get witness state (for diagnostics)
 * @return Pointer to witness state structure
 */
const MosWitnessState* mos_witness_info(void);

/**
 * @brief Get current health score
 * @return Health value (-100 to +100)
 */
int32_t mos_witness_health(void);

/**
 * @brief Get current Hebrew state
 * @return Hebrew state (0=SHAVAR to 6=TOV)
 */
uint8_t mos_witness_hebrew_state(void);

/**
 * @brief Get Hebrew state name
 * @return State name string
 */
const char* mos_witness_hebrew_name(void);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Usage:
//   #include "witness/witness.h"
//
//   // Report events
//   mos_early_event_impl(MOS_DAR_DETECT, MOS_SEV_MILD, "memory", "Init OK", +5);
//   mos_witness_event_impl(MOS_DAR_DETECT, MOS_SEV_CRITICAL, "disk", "Failed", -20);
//
//   // Query state
//   int32_t health = mos_witness_health();
//   const char* state = mos_witness_hebrew_name();
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif /* MOS_WITNESS_H */
