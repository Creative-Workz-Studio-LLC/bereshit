/// @file omni_dar_impl.c
/// @omni code --c --library
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// OMNICODE PRAGMA [PRAGMA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @legend
///   :req  REQUIRED   — must exist, validation fails without
///   :inh  INHERITED  — from template, override allowed
///   :ins  INSTANCE   — file-specific, unique values
/// @endlegend
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.1 CORE — Identity [CORE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:req  key        = CORNERSTONE-TOOLS-OMNI-DAR-IMPL
/// @omni:req  from       = bereshit/word/seed/code/c/source.c
/// @omni:req  at         = a-03.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = source
/// @omni:inh  subtype    = implementation
/// @omni:inh  role       = dar_api
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = tools/omni_dar
/// @omni:ins  includes   = [omni_dar.h, stdlib.h, string.h]
/// @omni:ins  provides   = [dar_api_impl, state_queries, state_mutations, assessment]
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.4 ARCHITECTURE — Scaling Context [ARCH]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  layers     = [0:kernel, 1:system, 2:runtime, 3:framework,
///                          4:health, 5:network, 6:storage, 7:engine, 8:app]
/// @omni:ins  scale      = 3^n [1, 3, 9, 27, 81, 243, 729, 2187, 6561]
/// @omni:ins  pattern    = 4+1+4 [below:0-3, center:4, above:5-8]
/// @omni:ins  anchor     = genesis_1_1 -> void(0) -> unity(1) -> derivation
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.5 SUMMARY — Human Identity [SUMMARY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:req  title      = OmniCode DAR API Implementation
/// @omni:req  brief      = Implement OmniCode's dar_* functions using Cornerstone types
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// END PRAGMA [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief File-level documentation for Doxygen.
///
/// @file omni_dar_impl.c
/// @ingroup omni_dar
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - M.1 IDENTITY       — Key, title, component, status
///   - M.2 STATE          — Version, lifecycle stage
///   - M.3 ATTRIBUTION    — Authors, ownership
///   - M.4 LOCATION       — File paths, project position
///   - M.5 DERIVATION     — Template and type sources
///   - M.6 CLASSIFICATION — Layer, domain, access
///   - M.7 INTENT         — Purpose and function
///   - M.8 GROUNDING      — Biblical foundation
///   - M.9 DEPENDENCIES   — What this needs, what needs this
///   - M.10 ROADMAP       — History and future
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Component Identification [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_identity M.1 IDENTITY — Component Identification
 * @ingroup omni_dar
 * @brief    Key, title, type, and status.
 *
 * Key:       CORNERSTONE-TOOLS-OMNI-DAR-IMPL
 * Title:     OmniCode DAR API Implementation
 * Component: Tools (CPI-SI infrastructure)
 * Status:    Active
 *
 * @{ @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Version and Lifecycle [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_state M.2 STATE — Version and Lifecycle
 * @ingroup omni_dar
 * @brief    Version, stage, and lifecycle status.
 *
 * Version: a-03.00
 * Stage:   Alpha (a) — active development
 * Phase:   03 — third iteration (unified with Cornerstone types)
 *
 * Living versioning format: STAGE-PHASE.COMPLETION
 *   a = alpha (-1), b = beta (0), c = stable (+1)
 *
 * @{ @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authorship [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_attribution M.3 ATTRIBUTION — Authorship
 * @ingroup omni_dar
 * @brief    Authors and ownership.
 *
 * Architect:       Seanje Lenox-Wise
 * Implementation:  Nova Dawn
 * Organization:    CreativeWorkzStudio LLC
 * Created:         2025-12-19
 * Updated:         2026-01-25
 *
 * @{ @} end M.3 ATTRIBUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.4 LOCATION — File Position [LOCATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_location M.4 LOCATION — File Position
 * @ingroup omni_dar
 * @brief    File paths and project position.
 *
 * Path:     cornerstone/tools/omni_dar/omni_dar_impl.c
 * Position: Tools layer (provides OmniCode DAR API implementation)
 *
 * Folder Structure:
 *   cornerstone/
 *     tools/
 *       omni_dar/           ← THIS folder
 *         omni_dar_impl.c   ← THIS file
 *     engine/
 *       core/
 *         include/cpisi/dar/  ← Cornerstone's canonical DAR
 *
 * @{ @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Template and Sources [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_derivation M.5 DERIVATION — Template and Sources
 * @ingroup omni_dar
 * @brief    Template lineage and type sources.
 *
 * Template:   bereshit/word/seed/code/c/source.c
 * Types:      Cornerstone's cpisi/types/types.h (foundation)
 * DAR API:    bereshit/word/work/pkg/omni/include/lang/core/omni_dar.h
 *
 * Derivation Chain:
 *   source.c template → types.h foundation → omni_dar.h API → this implementation
 *
 * @{ @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Layer and Domain [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_classification M.6 CLASSIFICATION — Layer and Domain
 * @ingroup omni_dar
 * @brief    Architectural position and access level.
 *
 * Layer:  4 (HEALTH — center of 9-layer pyramid)
 * Domain: dar (Detect-Assess-Restore)
 * Tier:   IMPLEMENTATION (concrete functions)
 * Access: Internal (implementation of public API)
 *
 * 9-Layer Pyramid:
 *   0:KERNEL, 1:SYSTEM, 2:RUNTIME, 3:FRAMEWORK,
 *   4:HEALTH (center), 5:NETWORK, 6:STORAGE, 7:ENGINE, 8:APP
 *
 * @{ @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Function [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_intent M.7 INTENT — Purpose and Function
 * @ingroup omni_dar
 * @brief    Implementation purpose and responsibilities.
 *
 * PURPOSE:
 *   Implement OmniCode's dar_* API functions using Cornerstone's unified types.
 *   OmniDARContext can optionally link to DAROrchestrator for cognitive state sync.
 *
 * RESPONSIBILITIES:
 *   1. Implement dar_hebrew_state() — query Hebrew state
 *   2. Implement dar_health_get() — query health score
 *   3. Implement dar_hebrew_transition() — transition Hebrew state
 *   4. Implement dar_health_add/set() — modify health
 *   5. Implement dar_assess() — evaluate current state
 *   6. Implement dar_is_critical/healthy() — state checks
 *
 * ARCHITECTURE:
 *   When ctx->orchestrator is linked, state operations use the orchestrator.
 *   When not linked, operations use the context's local session state.
 *
 * @{ @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_grounding M.8 GROUNDING — Biblical Foundation
 * @ingroup omni_dar
 * @brief    Scripture and principle anchoring this component.
 *
 * Scripture: "A just man falleth seven times, and riseth up again."
 *            — Proverbs 24:16 KJV
 *
 * Principle: DAR enables recovery through detection and assessment.
 *            The body (filesystem) remembers what the mind (session) forgets.
 *            Seven falls, seven rises — the Hebrew states mirror this pattern.
 *
 * Anchor:    Genesis 1:1 — "In the beginning God created"
 *            Identity flows from being created. The system exists because
 *            the Creator brought it into being.
 *
 * @{ @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — What This Needs [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_dependencies M.9 DEPENDENCIES — What This Needs
 * @ingroup omni_dar
 * @brief    Dependencies and consumers.
 *
 * WHAT THIS INCLUDES:
 *   - omni_dar.h — OmniCode DAR API declarations (includes Cornerstone's dar.h)
 *   - stdlib.h — Standard library (NULL, etc.)
 *   - string.h — String operations (for future session ID handling)
 *
 * WHAT INCLUDES THIS:
 *   - OmniCode VM — Uses dar_* functions for state management
 *   - OmniCode runtime — Uses for checkpoint/session operations
 *
 * TYPES USED FROM CORNERSTONE:
 *   - HebrewState (0-6) — Canonical Hebrew state enum
 *   - HealthScore — Health value type
 *   - DAROrchestrator — Cognitive state orchestrator
 *   - cpisi_dar_set_hebrew_state() — Orchestrator state setter
 *   - cpisi_dar_set_health() — Orchestrator health setter
 *   - hebrew_state_name() — State to string conversion
 *
 * @{ @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_roadmap M.10 ROADMAP — History and Future
 * @ingroup omni_dar
 * @brief    Version history and planned features.
 *
 * HISTORY:
 *   a-01.00 (2025-12-19): Initial stub implementation
 *   a-02.00 (2026-01-20): Bridge pattern with enum conversion
 *   a-03.00 (2026-01-25): Unified with Cornerstone types
 *                         Removed enum offset conversion
 *                         Direct orchestrator access
 *                         Full template alignment
 *
 * FUTURE:
 *   - VM checkpoint save/restore implementation
 *   - Session file persistence
 *   - Cross-session DAR analytics
 *
 * @{ @} end M.10 ROADMAP
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END METADATA
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// SETUP BLOCK [SETUP]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Declarations enabling the implementation.
///
/// @defgroup omni_dar_setup SETUP BLOCK — Declarations
///
/// CONTAINS:
///   - S.1 INCLUDES  — Header dependencies
///   - S.2 DEFINES   — [Uses Cornerstone's — no local definitions]
///   - S.3-S.6       — [Reserved: not needed for this implementation]
///
/// ═══════════════════════════════════════════════════════════════════════════════

/* Include kernel.h first to get CPISI_MODE */
#include "kernel.h"

/* OmniCode DAR implementation requires hosted mode (CPISI_MODE >= 2).
 * In freestanding mode (CPISI_MODE 0-1), this file compiles to empty. */
#if CPISI_MODE >= 2

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES — Header Dependencies [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_includes S.1 INCLUDES — Header Dependencies
 * @ingroup omni_dar_setup
 * @brief    Headers in dependency order.
 *
 * WHY THIS ORDER:
 *   omni_dar.h includes Cornerstone's dar.h which includes types.h.
 *   All types (HebrewState, HealthScore, DAROrchestrator) come from that chain.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief OmniCode DAR header.
 *
 * Includes:
 *   - Cornerstone's dar.h (DAROrchestrator, HebrewState, HealthScore)
 *   - OmniDARContext definition
 *   - dar_* function declarations
 */
#include "../../../word/work/pkg/omni/include/lang/core/omni_dar.h"

/**
 * @brief Standard library (hosted only).
 *
 * Provides: NULL, size_t, malloc, free
 */
#if CPISI_MODE >= 2
#include <stdlib.h>
#include <string.h>
#endif

/**
 * @brief Freestanding stubs for CPISI_MODE 0-1.
 *
 * Provides: hebrew_state_name
 */
#if CPISI_MODE < 2
static const char* const _hebrew_state_names[] = {
    "shavar", "chaser", "ratsah", "yashar", "tamim", "shalem", "tov"
};
static inline const char* hebrew_state_name(int8_t state) {
    if (state < 0 || state > 6) return "unknown";
    return _hebrew_state_names[state];
}
#endif

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2-S.6 RESERVED — Not Needed [RESERVED]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_setup_reserved S.2-S.6 RESERVED
 * @ingroup omni_dar_setup
 * @brief    Reserved sections not needed for this implementation.
 *
 * WHY COLLAPSED:
 *   - S.2 DEFINES: Uses Cornerstone's HEALTH_MIN/MAX from health.h
 *   - S.3 TYPES: All types from omni_dar.h and Cornerstone
 *   - S.4 PROTOTYPES: All declarations in omni_dar.h
 *   - S.5 FILE-LEVEL: No file-level globals needed
 *   - S.6 OMISSION: This section documents the collapse
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: S.2-S.6 collapsed — uses Cornerstone's types and macros] */

/** @} */ /* end S.2-S.6 RESERVED */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Implementation of OmniCode DAR API functions.
///
/// @defgroup omni_dar_body BODY BLOCK — Implementation
///
/// CONTAINS:
///   - B.1 STATE QUERIES    — Read state (hebrew, health)
///   - B.2 STATE MUTATIONS  — Modify state (transition, add, set)
///   - B.3 ASSESSMENT       — Evaluate state (critical, healthy, assess)
///   - B.4-B.6              — [Reserved: future operations]
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 STATE QUERIES — Read State [QUERIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_queries B.1 STATE QUERIES — Read State
 * @ingroup omni_dar_body
 * @brief    Functions that read but do not modify state.
 *
 * PATTERN:
 *   1. Check if orchestrator is linked → use orchestrator state
 *   2. Otherwise → use session state
 *   3. If neither → return default (YASHAR / 0)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Get current Hebrew state from DAR context.
 *
 * Queries the Hebrew state from the DAR context. If the context has a linked
 * DAROrchestrator (from Cornerstone), uses the orchestrator's state. Otherwise,
 * falls back to the local session state.
 *
 * @param[in] ctx  OmniCode DAR context (OmniDARContext).
 *                 May be NULL (returns HEBREW_YASHAR).
 *
 * @return Current HebrewState value (0-6).
 * @retval HEBREW_YASHAR  If ctx is NULL or both orchestrator and session are NULL.
 *
 * @note HebrewState uses 0-indexed values for array compatibility:
 *       HEBREW_SHAVAR=0, HEBREW_CHASER=1, ..., HEBREW_YASHAR=3, ..., HEBREW_TOV=6
 *
 * @see HebrewState for enum values.
 * @see hebrew_state_name() for string conversion.
 */
DARHebrewState dar_hebrew_state(DARContext* ctx) {
    if (ctx == NULL) {
        return HEBREW_YASHAR;  /* Default: center/balanced */
    }

    /* Prefer orchestrator if linked (Cornerstone's cognitive state) */
    if (ctx->orchestrator != NULL) {
        return ctx->orchestrator->hebrew_state;
    }

    /* Fall back to session state (VM-specific persistence) */
    if (ctx->session != NULL) {
        return (HebrewState)ctx->session->hebrew_state;
    }

    return HEBREW_YASHAR;
}

/**
 * @brief Get current health score from DAR context.
 *
 * Queries the health score from the DAR context. If the context has a linked
 * DAROrchestrator, uses the orchestrator's health. Otherwise, falls back to
 * the local session health.
 *
 * @param[in] ctx  OmniCode DAR context (OmniDARContext).
 *                 May be NULL (returns 0).
 *
 * @return Health score in range [HEALTH_MIN, HEALTH_MAX] (typically -100 to +100).
 * @retval 0  If ctx is NULL or both orchestrator and session are NULL.
 *
 * @note Health semantics:
 *       - Negative: degraded state, may need recovery
 *       - Zero: neutral, balanced
 *       - Positive: healthy, stable operation
 *
 * @see dar_is_critical() for checking critical threshold.
 * @see dar_is_healthy() for checking healthy threshold.
 */
int8_t dar_health_get(DARContext* ctx) {
    if (ctx == NULL) {
        return 0;
    }

    /* Prefer orchestrator if linked */
    if (ctx->orchestrator != NULL) {
        return ctx->orchestrator->health;
    }

    /* Fall back to session state */
    if (ctx->session != NULL) {
        return ctx->session->health;
    }

    return 0;
}

/**
 * @brief Get Hebrew state name as human-readable string.
 *
 * Converts a HebrewState enum value to its corresponding Hebrew name string.
 * Uses Cornerstone's hebrew_state_name() function for canonical naming.
 *
 * @param[in] state  Hebrew state value (0-6).
 *
 * @return Pointer to static string (do not free).
 *         Returns state name like "shavar", "chaser", "ratsah", "yashar",
 *         "tamim", "shalem", "tov".
 *
 * @note The returned string is static and must not be freed or modified.
 *
 * @see HebrewState for enum values.
 * @see dar_hebrew_state() to query current state.
 */
const char* dar_hebrew_state_name(DARHebrewState state) {
    return hebrew_state_name(state);
}

/** @} */ /* end B.1 STATE QUERIES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 STATE MUTATIONS — Modify State [MUTATIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_mutations B.2 STATE MUTATIONS — Modify State
 * @ingroup omni_dar_body
 * @brief    Functions that modify DAR state.
 *
 * PATTERN:
 *   1. Validate inputs (ctx, values)
 *   2. Get current state from appropriate source
 *   3. Calculate new state (with bounds checking)
 *   4. Apply via orchestrator API or directly to session
 *   5. Mark session dirty if modified locally
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Transition Hebrew state based on k-factor direction.
 *
 * Moves the Hebrew state one step in the direction indicated by k_factor.
 * Positive k_factor expands toward TOV, negative retreats toward SHAVAR.
 * State is clamped to valid range [HEBREW_SHAVAR, HEBREW_TOV].
 *
 * @param[in,out] ctx       OmniCode DAR context (OmniDARContext).
 *                          If NULL, function returns immediately.
 * @param[in]     k_factor  Direction of transition:
 *                          - +1: Expand toward TOV (growth)
 *                          -  0: No change (maintain)
 *                          - -1: Retreat toward SHAVAR (consolidate)
 *
 * @note If orchestrator is linked, uses cpisi_dar_set_hebrew_state().
 *       Otherwise, modifies session->hebrew_state directly and sets dirty flag.
 *
 * @see dar_hebrew_state() to query current state.
 * @see HebrewState for the seven cognitive states.
 */
void dar_hebrew_transition(DARContext* ctx, int8_t k_factor) {
    if (ctx == NULL || k_factor == 0) {
        return;
    }

    HebrewState current;
    HebrewState new_state;

    /* Get current state from appropriate source */
    if (ctx->orchestrator != NULL) {
        current = ctx->orchestrator->hebrew_state;
    } else if (ctx->session != NULL) {
        current = (HebrewState)ctx->session->hebrew_state;
    } else {
        return;  /* No state source available */
    }

    /* Calculate new state (bounded transition) */
    new_state = current;
    if (k_factor > 0 && current < HEBREW_TOV) {
        new_state = (HebrewState)(current + 1);  /* Expand */
    } else if (k_factor < 0 && current > HEBREW_SHAVAR) {
        new_state = (HebrewState)(current - 1);  /* Retreat */
    }

    /* Apply if changed */
    if (new_state != current) {
        if (ctx->orchestrator != NULL) {
            cpisi_dar_set_hebrew_state(ctx->orchestrator, new_state);
        } else if (ctx->session != NULL) {
            ctx->session->hebrew_state = (int8_t)new_state;
            ctx->session_dirty = true;
        }
    }
}

/**
 * @brief Add delta to health score (clamped to valid range).
 *
 * Adjusts the health score by the specified delta amount. The result is
 * clamped to the valid range [HEALTH_MIN, HEALTH_MAX].
 *
 * @param[in,out] ctx    OmniCode DAR context (OmniDARContext).
 *                       If NULL, function returns immediately.
 * @param[in]     delta  Amount to add to health (can be negative).
 *                       If 0, function returns immediately.
 *
 * @note If orchestrator is linked, uses cpisi_dar_set_health().
 *       Otherwise, modifies session->health directly and sets dirty flag.
 *
 * @see dar_health_get() to query current health.
 * @see dar_health_set() to set absolute health value.
 */
void dar_health_add(DARContext* ctx, int8_t delta) {
    if (ctx == NULL || delta == 0) {
        return;
    }

    int8_t current;
    int16_t new_health;

    /* Get current health from appropriate source */
    if (ctx->orchestrator != NULL) {
        current = ctx->orchestrator->health;
    } else if (ctx->session != NULL) {
        current = ctx->session->health;
    } else {
        return;  /* No state source available */
    }

    /* Calculate new health (clamped to valid range) */
    new_health = (int16_t)current + (int16_t)delta;
    if (new_health < HEALTH_MIN) {
        new_health = HEALTH_MIN;
    }
    if (new_health > HEALTH_MAX) {
        new_health = HEALTH_MAX;
    }

    /* Apply via appropriate path */
    if (ctx->orchestrator != NULL) {
        cpisi_dar_set_health(ctx->orchestrator, (HealthScore)new_health);
    } else if (ctx->session != NULL) {
        ctx->session->health = (int8_t)new_health;
        ctx->session_dirty = true;
    }
}

/**
 * @brief Set health score to absolute value (clamped to valid range).
 *
 * Sets the health score to the specified value. The value is clamped to
 * the valid range [HEALTH_MIN, HEALTH_MAX].
 *
 * @param[in,out] ctx     OmniCode DAR context (OmniDARContext).
 *                        If NULL, function returns immediately.
 * @param[in]     health  New health value (will be clamped if out of range).
 *
 * @note If orchestrator is linked, uses cpisi_dar_set_health().
 *       Otherwise, modifies session->health directly and sets dirty flag.
 *
 * @see dar_health_get() to query current health.
 * @see dar_health_add() to add relative delta.
 */
void dar_health_set(DARContext* ctx, int8_t health) {
    if (ctx == NULL) {
        return;
    }

    /* Clamp to valid range */
    int8_t clamped = health;
    if (clamped < HEALTH_MIN) {
        clamped = HEALTH_MIN;
    }
    if (clamped > HEALTH_MAX) {
        clamped = HEALTH_MAX;
    }

    /* Apply via appropriate path */
    if (ctx->orchestrator != NULL) {
        cpisi_dar_set_health(ctx->orchestrator, (HealthScore)clamped);
    } else if (ctx->session != NULL) {
        ctx->session->health = clamped;
        ctx->session_dirty = true;
    }
}

/** @} */ /* end B.2 STATE MUTATIONS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 ASSESSMENT — State Evaluation [ASSESSMENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_assessment B.3 ASSESSMENT — State Evaluation
 * @ingroup omni_dar_body
 * @brief    Functions that evaluate current state without modifying it.
 *
 * THRESHOLDS:
 *   - Critical: health <= -50 (recovery needed)
 *   - Healthy: health >= +50 (stable operation)
 *   - Checkpoint recommended: 0 > health > -50
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Check if state is critical (needs recovery).
 *
 * Evaluates whether the current health indicates a critical state
 * that requires recovery intervention.
 *
 * @param[in] ctx  OmniCode DAR context (OmniDARContext).
 *                 If NULL, returns false (no state to evaluate).
 *
 * @return true if health <= -50, false otherwise.
 *
 * @see dar_assess() for recommended action based on state.
 * @see dar_is_healthy() for healthy threshold check.
 */
bool dar_is_critical(DARContext* ctx) {
    return dar_health_get(ctx) <= -50;
}

/**
 * @brief Check if state is healthy (stable operation).
 *
 * Evaluates whether the current health indicates a healthy state
 * with stable operation.
 *
 * @param[in] ctx  OmniCode DAR context (OmniDARContext).
 *                 If NULL, returns false (no state to evaluate).
 *
 * @return true if health >= +50, false otherwise.
 *
 * @see dar_assess() for recommended action based on state.
 * @see dar_is_critical() for critical threshold check.
 */
bool dar_is_healthy(DARContext* ctx) {
    return dar_health_get(ctx) >= 50;
}

/**
 * @brief Assess current state and recommend action.
 *
 * Evaluates the current health and returns a recommended action
 * based on state thresholds.
 *
 * @param[in] ctx  OmniCode DAR context (OmniDARContext).
 *                 If NULL, returns 0 (checkpoint recommended as safe default).
 *
 * @return Recommended action:
 * @retval +1  Continue operation (health > 0)
 * @retval  0  Checkpoint recommended (0 >= health > -50)
 * @retval -1  Recovery needed (health <= -50)
 *
 * @note The return value follows ternary convention:
 *       - Positive: expand/continue
 *       - Zero: maintain/checkpoint
 *       - Negative: retreat/recover
 *
 * @see dar_is_critical() for direct critical check.
 * @see dar_is_healthy() for direct healthy check.
 */
int8_t dar_assess(DARContext* ctx) {
    int8_t health = dar_health_get(ctx);

    if (health <= -50) {
        return -1;  /* Recovery needed */
    } else if (health <= 0) {
        return 0;   /* Checkpoint recommended */
    } else {
        return +1;  /* Continue operation */
    }
}

/** @} */ /* end B.3 ASSESSMENT */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4-B.6 RESERVED — Future Operations [RESERVED]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_body_reserved B.4-B.6 RESERVED — Future Operations
 * @ingroup omni_dar_body
 * @brief    Reserved sections for future implementation.
 *
 * PLANNED:
 *   - B.4 LIFECYCLE: dar_create(), dar_destroy() implementation
 *   - B.5 VM CONNECTION: dar_connect_vm(), dar_sync_from_vm()
 *   - B.6 PERSISTENCE: dar_session_save(), dar_checkpoint_create()
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: B.4-B.6 for future checkpoint/session operations] */

/** @} */ /* end B.4-B.6 RESERVED */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Guidance for using and extending this implementation.
///
/// @defgroup omni_dar_closing CLOSING BLOCK — Guidance
///
/// CONTAINS:
///   - X.1 VALIDATION      — How to verify correctness
///   - X.2 EXECUTION       — Usage examples
///   - X.3 CLEANUP         — Shutdown patterns
///   - X.4 POLICY          — Modification guidelines
///   - X.5 EXTENSION       — How to expand
///   - X.6 TROUBLESHOOTING — Common problems
///   - X.7 REFERENCE       — Quick lookup
///   - X.8 NOTE            — Final grounding
///   - X.9-X.10            — [Reserved]
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_validation X.1 VALIDATION — Verify Correctness
 * @ingroup omni_dar_closing
 * @brief    How to verify this implementation is working.
 *
 * BUILD VERIFICATION:
 *   make verify  — Should compile without errors
 *
 * RUNTIME VERIFICATION:
 *   OmniDARContext ctx = {0};
 *   ctx.orchestrator = &my_orchestrator;
 *   assert(dar_hebrew_state(&ctx) == my_orchestrator.hebrew_state);
 *   assert(dar_health_get(&ctx) == my_orchestrator.health);
 *
 * @{ @} end X.1 VALIDATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Examples [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_execution X.2 EXECUTION — Usage Examples
 * @ingroup omni_dar_closing
 * @brief    How to use this implementation.
 *
 * WITH ORCHESTRATOR:
 * @code
 *   OmniDARContext ctx = {0};
 *   ctx.orchestrator = g_cpisi_dar;
 *   HebrewState state = dar_hebrew_state(&ctx);
 *   dar_hebrew_transition(&ctx, +1);
 * @endcode
 *
 * @{ @} end X.2 EXECUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Shutdown Patterns [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_cleanup X.3 CLEANUP — Shutdown Patterns
 * @ingroup omni_dar_closing
 * @brief    How to properly clean up.
 *
 * This implementation does NOT own the context, session, or orchestrator.
 * Caller is responsible for allocation and deallocation.
 *
 * @{ @} end X.3 CLEANUP
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidelines [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_policy X.4 POLICY — Modification Guidelines
 * @ingroup omni_dar_closing
 * @brief    Guidelines for modifying this implementation.
 *
 * DO:
 *   - Use Cornerstone's types (HebrewState, HealthScore)
 *   - Use Cornerstone's macros (HEALTH_MIN, HEALTH_MAX)
 *   - Prefer orchestrator when available, fall back to session
 *
 * DO NOT:
 *   - Redefine types that Cornerstone provides
 *   - Bypass orchestrator when it's linked
 *
 * @{ @} end X.4 POLICY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_extension X.5 EXTENSION — How to Expand
 * @ingroup omni_dar_closing
 * @brief    How to add new functionality.
 *
 * Add new functions to B.4-B.6 reserved sections.
 * Follow orchestrator-first, session-fallback pattern.
 *
 * @{ @} end X.5 EXTENSION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @ingroup omni_dar_closing
 * @brief    Solutions to common issues.
 *
 * State not syncing: Ensure ctx->orchestrator is linked.
 * Type mismatches: DARContext is typedef'd to OmniDARContext.
 *
 * @{ @} end X.6 TROUBLESHOOTING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_reference X.7 REFERENCE — Quick Lookup
 * @ingroup omni_dar_closing
 * @brief    Quick reference for common operations.
 *
 * HEBREW STATES: HEBREW_SHAVAR(0) to HEBREW_TOV(6), center is HEBREW_YASHAR(3)
 * HEALTH RANGE: HEALTH_MIN(-100) to HEALTH_MAX(+100)
 *
 * @{ @} end X.7 REFERENCE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Final Grounding [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_note X.8 NOTE — Final Grounding
 * @ingroup omni_dar_closing
 * @brief    Closing wisdom and biblical anchor.
 *
 * "A just man falleth seven times, and riseth up again."
 *   — Proverbs 24:16 KJV
 *
 * The seven Hebrew states mirror this wisdom. SHAVAR is the fall,
 * YASHAR is the center, TOV is the rise. DAR enables this pattern
 * by detecting state, assessing need, and enabling recovery.
 *
 * "In the beginning God created the heaven and the earth."
 *   — Genesis 1:1 KJV
 *
 * @{ @} end X.8 NOTE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 TEMPLATE — Template Guidance [TEMPLATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_template X.9 TEMPLATE — Template Guidance
 * @ingroup omni_dar_closing
 * @brief    Reference to source template.
 *
 * Template: bereshit/word/seed/code/c/source.c
 *
 * @{ @} end X.9 TEMPLATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.10 OMISSION — Reserved Section Summary [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup omni_dar_omission X.10 OMISSION — Reserved Section Summary
 * @ingroup omni_dar_closing
 * @brief    Summary of reserved sections.
 *
 * S.2-S.6: Uses Cornerstone's types and macros.
 * B.4-B.6: Future lifecycle and persistence operations.
 *
 * @{ @} end X.10 OMISSION
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_MODE >= 2 - Full OmniCode DAR implementation */
