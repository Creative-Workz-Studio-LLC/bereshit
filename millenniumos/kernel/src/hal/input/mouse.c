/// @file mouse.c
/// @omni code --c --source
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
/// @omni:req  key        = MOS-HAL-INPUT-MOUSE-IMPL
/// @omni:req  from       = bereshit/word/seed/code/c/source.c
/// @omni:req  at         = a-01.01
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = source
/// @omni:inh  subtype    = implementation
/// @omni:inh  role       = input
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = engine/core/input
/// @omni:ins  layer      = kernel (layer 0)
/// @omni:ins  includes   = [stdint, stdbool, string, mouse.h]
/// @omni:ins  provides   = [mouse_*, touch_*, gesture_* implementations]
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
/// @omni:req  title      = Mouse Input Implementation
/// @omni:req  brief      = Kernel-level mouse, touch, and gesture implementation
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// END PRAGMA [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// SPECIAL BLOCK [SPECIAL]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Pre-include configuration — MUST appear before any system includes.
///
/// NOTE: Freestanding kernel mode — no POSIX feature macros.
/// This kernel runs without libc, using HAL for hardware access.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/* SP.1 CONFIG — Freestanding kernel mode: No POSIX configuration needed */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Identity and context for this component.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @file      mouse.c
 * @brief     Mouse Input — kernel-level mouse, touch, and gesture implementation.
 *
 * @defgroup mouse_impl_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       MOS-HAL-INPUT-MOUSE-IMPL
 * Title:     Mouse Input Implementation
 * Type:      Source (Implementation)
 * Component: Engine/Core/Input — kernel-level peripheral abstraction
 * Role:      Button tracking, position, delta, scroll, touch, gesture
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_state M.2 STATE — Lifecycle State
 * @{
 *
 * @version   a-01.01
 * @date      2026-01-26
 *
 * Status:    Active
 * Created:   2026-01-26
 * Updated:   2026-01-26
 *
 * @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authors and Copyright [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_attribution M.3 ATTRIBUTION — Authors and Copyright
 * @{
 *
 * @author    Seanje Lenox-Wise (Architect)
 * @author    Nova Dawn (Implementation)
 * @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * @} end M.3 ATTRIBUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.4 LOCATION — File Path [LOCATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_location M.4 LOCATION — File Path
 * @{
 *
 * Path: millenniumos/kernel/src/hal/input/mouse.c
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: bereshit/word/seed/code/c/source.c
 * Header:       engine/core/include/input/mouse.h
 * Pattern:      keyboard.c (sibling input implementation)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: source, c, engine, core, input, mouse, touch, gesture, kernel, peripheral
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Kernel-level mouse input abstraction with touch/gesture
 * Core Design: GUI backends inject events, polling returns them to consumers
 * Philosophy:  Input devices are peripherals — separate from display
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Proverbs 4:25-26 — "Let thine eyes look right on...
 *             Ponder the path of thy feet"
 * Principle:  Pointing reveals direction — where user aims, attention follows
 * Anchor:     James 1:19 — "Swift to hear" — responsive input handling
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Standard: stdint.h, stdbool.h, string.h
 *   Internal: mouse.h (our header)
 *
 * What Uses This:
 *   - keybind.c (action mapping layer)
 *   - hal.c (PS/2 mouse handler)
 *   - shell (via keybind)
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-26) — Initial creation
 *   a-01.01 (2026-01-26) — Full template alignment, GUI injection, touch, gesture
 *   a-01.02 (2026-01-26) — Xterm mouse protocol support (TUI/WezTerm)
 *
 * Current:
 *   ✓ Button state tracking
 *   ✓ Position and delta tracking
 *   ✓ Scroll wheel support
 *   ✓ GUI injection API
 *   ✓ Multi-touch framework
 *   ✓ Gesture recognition framework
 *   ✓ Xterm mouse protocol (X10, normal, SGR, urxvt)
 *
 * Planned:
 *   ⏳ Pressure sensitivity implementation
 *   ⏳ Custom gesture recognition
 *
 * @} end M.10 ROADMAP
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END METADATA [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// SETUP BLOCK [SETUP]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Configuration and declarations before body.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard and internal includes.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @defgroup mouse_impl_standard S.1a STANDARD — System Headers
 * @ingroup mouse_impl_includes
 * @brief    Freestanding headers only — no libc.
 * @{ */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>   /**< size_t for freestanding */

/** @} */ /* end S.1a STANDARD */

/**
 * @defgroup mouse_impl_internal S.1b INTERNAL — Project Headers
 * @ingroup mouse_impl_includes
 * @{ */

#include "hal/input/mouse.h"

/** @} */ /* end S.1b INTERNAL */

/**
 * @defgroup mouse_impl_compat S.1c COMPAT — Freestanding Compatibility
 * @ingroup mouse_impl_includes
 * @brief    Inline implementations for functions normally from libc.
 * @{ */

/**
 * @brief Freestanding memset implementation.
 */
static void* mos_memset(void* dest, int c, size_t n) {
    unsigned char* p = (unsigned char*)dest;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return dest;
}

/** @} */ /* end S.1c COMPAT */

/** S.1c EXTERNAL — [None] @defgroup mouse_impl_external @ingroup mouse_impl_includes @{ @} */
/** S.1d CPP — [Header handles C++ linkage] @defgroup mouse_impl_cpp @ingroup mouse_impl_includes @{ @} */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Preprocessor Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_defines S.2 DEFINES — Preprocessor Constants
 * @brief    Compile-time constants for mouse implementation.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/** S.2a SIZES — [Reserved] @defgroup mouse_impl_sizes @ingroup mouse_impl_defines @{ @} */

/**
 * @defgroup mouse_impl_limits S.2b LIMITS — Implementation Limits
 * @ingroup mouse_impl_defines
 * @{ */

/** @brief Gesture recognition timeout in milliseconds. */
#define GESTURE_TIMEOUT_MS          300

/** @brief Minimum distance for swipe detection (pixels). */
#define GESTURE_SWIPE_MIN_DIST      50

/** @brief Minimum velocity for swipe detection (pixels/second). */
#define GESTURE_SWIPE_MIN_VELOCITY  200.0f

/** @brief Maximum time for double-tap detection (ms). */
#define GESTURE_DOUBLE_TAP_MS       300

/** @brief Long press threshold (ms). */
#define GESTURE_LONG_PRESS_MS       500

/** @} */ /* end S.2b LIMITS */

/** S.2c FLAGS — [Reserved] @defgroup mouse_impl_flags @ingroup mouse_impl_defines @{ @} */
/** S.2d MACROS — [Reserved] @defgroup mouse_impl_macros @ingroup mouse_impl_defines @{ @} */
/** S.2e VERIFY — [Reserved] @defgroup mouse_impl_verify @ingroup mouse_impl_defines @{ @} */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Internal Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_types S.3 TYPES — Internal Type Declarations
 * @brief    Implementation-specific types (public types in header).
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/** S.3a ENUMS — [Public enums in header] @defgroup mouse_impl_enums @ingroup mouse_impl_types @{ @} */

/**
 * @defgroup mouse_impl_structs S.3b STRUCTS — Internal Structures
 * @ingroup mouse_impl_types
 * @{ */

/**
 * @brief Internal touch tracking state.
 */
typedef struct TouchTracker {
    TouchPoint points[MOUSE_TOUCH_MAX];  /**< @brief Active touch points. */
    int        count;                     /**< @brief Number of active touches. */
    int32_t    start_x;                   /**< @brief Gesture start X. */
    int32_t    start_y;                   /**< @brief Gesture start Y. */
    uint32_t   start_time;                /**< @brief Gesture start timestamp (ms). */
    uint32_t   last_tap_time;             /**< @brief Last tap timestamp for double-tap. */
    int32_t    last_tap_x;                /**< @brief Last tap X position. */
    int32_t    last_tap_y;                /**< @brief Last tap Y position. */
} TouchTracker;

/** @} */ /* end S.3b STRUCTS */

/** S.3c OPAQUE — [Reserved] @defgroup mouse_impl_opaque @ingroup mouse_impl_types @{ @} */
/** S.3d ERRORS — [Public errors in header] @defgroup mouse_impl_errors @ingroup mouse_impl_types @{ @} */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Internal Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_prototypes S.4 PROTOTYPES — Internal Functions
 * @brief    Static helper function declarations.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @defgroup mouse_impl_helpers S.4a INTERNAL_HELPERS — Private Helpers
 * @ingroup mouse_impl_prototypes
 * @{ */

/**
 * @brief Process touch points for gesture recognition.
 * @return Recognized GestureType, or GESTURE_NONE.
 */
static GestureType recognize_gesture(void);

/**
 * @brief Get current timestamp in milliseconds.
 * @return Milliseconds since some epoch.
 */
static uint32_t get_timestamp_ms(void);

/** @} */ /* end S.4a INTERNAL_HELPERS */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — File Scope Declarations [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_file S.5 FILE-LEVEL — File Scope Declarations
 * @brief    Module-level static variables.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @defgroup mouse_impl_variables S.5a VARIABLES — Static Variables
 * @ingroup mouse_impl_file
 * @{ */

/** @brief Module initialization flag. */
static bool g_initialized = false;

/** @brief Per-button pressed state tracking. */
static bool g_button_states[MOUSE_BUTTON_COUNT];

/** @brief Current absolute X position. */
static int32_t g_pos_x = 0;

/** @brief Current absolute Y position. */
static int32_t g_pos_y = 0;

/** @brief Movement delta X since last poll. */
static int32_t g_delta_x = 0;

/** @brief Movement delta Y since last poll. */
static int32_t g_delta_y = 0;

/** @brief Previous X position (for delta calculation). */
static int32_t g_prev_x = 0;

/** @brief Previous Y position (for delta calculation). */
static int32_t g_prev_y = 0;

/** @brief Accumulated scroll X. */
static int16_t g_scroll_x = 0;

/** @brief Accumulated scroll Y. */
static int16_t g_scroll_y = 0;

/** @brief Cursor visibility flag. */
static bool g_visible = true;

/** @brief Pending event from GUI injection. */
static MouseState g_pending_event = {0};

/** @brief Flag indicating a pending mouse event. */
static bool g_has_pending_event = false;

/** @brief Touch tracking state. */
static TouchTracker g_touch = {0};

/** @brief Gesture recognition enabled flag. */
static bool g_gesture_enabled = true;

/** @brief Pending gesture event. */
static GestureState g_pending_gesture = {0};

/** @brief Flag indicating a pending gesture. */
static bool g_has_pending_gesture = false;

/* ─── Xterm Mouse State ─── */

/** @brief Xterm mouse tracking enabled flag. */
static bool g_xterm_enabled = false;

/** @brief Xterm mouse configuration. */
static XtermMouseConfig g_xterm_config = {
    .mode = XTERM_MOUSE_NONE,
    .encoding = XTERM_ENC_DEFAULT,
    .focus = false
};

/** @brief Xterm escape sequence parser state. */
typedef enum XtermParserState {
    XTERM_PARSE_IDLE = 0,    /**< Waiting for ESC */
    XTERM_PARSE_ESC,         /**< Got ESC, waiting for [ */
    XTERM_PARSE_CSI,         /**< Got [, waiting for < or M */
    XTERM_PARSE_SGR,         /**< Parsing SGR sequence (< Cb ; Cx ; Cy M/m) */
    XTERM_PARSE_X10          /**< Parsing X10 sequence (M Cb Cx Cy) */
} XtermParserState;

/** @brief Current parser state. */
static XtermParserState g_xterm_parser_state = XTERM_PARSE_IDLE;

/** @brief Xterm sequence buffer. */
static uint8_t g_xterm_seq_buf[XTERM_MOUSE_SEQ_MAX];

/** @brief Current position in sequence buffer. */
static size_t g_xterm_seq_len = 0;

/** @} */ /* end S.5a VARIABLES */

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_omission S.6 OMISSION — Reserved Sections
 * @brief    All SETUP sections populated.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Core implementations.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_orgchart B.1 ORG CHART — Structure Overview
 * @brief    How this file is organized.
 *
 * STRUCTURE:
 *   B.2 HELPERS:   Internal static functions
 *   B.3 CORE OPS:  Public API implementations
 *
 * FLOW:
 *   GUI backend → inject_* → g_pending_* → poll() → consumer
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Internal Helper Functions [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_helpers_body B.2 HELPERS — Internal Functions
 * @brief    Static helper implementations.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Get current timestamp in milliseconds.
 * @return Milliseconds since some epoch.
 */
static uint32_t get_timestamp_ms(void) {
    /* Simplified: would use clock_gettime in real implementation */
    static uint32_t counter = 0;
    return counter++;
}

/**
 * @brief Process touch points for gesture recognition.
 * @return Recognized GestureType, or GESTURE_NONE.
 */
static GestureType recognize_gesture(void) {
    if (!g_gesture_enabled || g_touch.count == 0) {
        return GESTURE_NONE;
    }

    /* Single touch gestures */
    if (g_touch.count == 1) {
        TouchPoint* pt = &g_touch.points[0];

        if (pt->phase == TOUCH_PHASE_ENDED) {
            uint32_t now = get_timestamp_ms();
            uint32_t duration = now - g_touch.start_time;
            int32_t dx = pt->x - g_touch.start_x;
            int32_t dy = pt->y - g_touch.start_y;
            int32_t dist_sq = dx * dx + dy * dy;

            /* Tap detection (short duration, minimal movement) */
            if (duration < GESTURE_TIMEOUT_MS && dist_sq < 100) {
                /* Check for double tap */
                if (now - g_touch.last_tap_time < GESTURE_DOUBLE_TAP_MS) {
                    g_touch.last_tap_time = 0;
                    return GESTURE_DOUBLE_TAP;
                }
                g_touch.last_tap_time = now;
                g_touch.last_tap_x = pt->x;
                g_touch.last_tap_y = pt->y;
                return GESTURE_TAP;
            }

            /* Swipe detection */
            if (dist_sq >= GESTURE_SWIPE_MIN_DIST * GESTURE_SWIPE_MIN_DIST) {
                if (dx > GESTURE_SWIPE_MIN_DIST && dx > dy && dx > -dy) {
                    return GESTURE_SWIPE_RIGHT;
                }
                if (dx < -GESTURE_SWIPE_MIN_DIST && -dx > dy && -dx > -dy) {
                    return GESTURE_SWIPE_LEFT;
                }
                if (dy > GESTURE_SWIPE_MIN_DIST && dy > dx && dy > -dx) {
                    return GESTURE_SWIPE_DOWN;
                }
                if (dy < -GESTURE_SWIPE_MIN_DIST && -dy > dx && -dy > -dx) {
                    return GESTURE_SWIPE_UP;
                }
            }

            /* Long press detection */
            if (duration >= GESTURE_LONG_PRESS_MS && dist_sq < 100) {
                return GESTURE_LONG_PRESS;
            }
        }

        /* Pan detection (moving while touching) */
        if (pt->phase == TOUCH_PHASE_MOVED) {
            return GESTURE_PAN;
        }
    }

    /* Two-finger gestures (pinch, rotate) */
    if (g_touch.count == 2) {
        /* Would calculate pinch scale and rotation angle here */
        return GESTURE_PINCH;
    }

    return GESTURE_NONE;
}

/** @} */ /* end B.2 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Business Logic Implementations [OPERATIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_coreops B.3 CORE OPS — Business Logic
 * @brief    Public API implementations.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a LIFECYCLE — Init/Shutdown [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_impl_lifecycle B.3a LIFECYCLE — Init/Shutdown
 * @ingroup mouse_impl_coreops
 * @{ */

MouseError mouse_init(void) {
    if (g_initialized) {
        return MOUSE_OK;
    }

    mos_memset(g_button_states, 0, sizeof(g_button_states));
    g_pos_x = 0;
    g_pos_y = 0;
    g_delta_x = 0;
    g_delta_y = 0;
    g_prev_x = 0;
    g_prev_y = 0;
    g_scroll_x = 0;
    g_scroll_y = 0;
    g_visible = true;
    g_has_pending_event = false;
    g_has_pending_gesture = false;
    g_gesture_enabled = true;

    mos_memset(&g_touch, 0, sizeof(g_touch));
    mos_memset(&g_pending_event, 0, sizeof(g_pending_event));
    mos_memset(&g_pending_gesture, 0, sizeof(g_pending_gesture));

    g_initialized = true;
    return MOUSE_OK;
}

void mouse_shutdown(void) {
    if (!g_initialized) {
        return;
    }
    g_initialized = false;
}

/** @} */ /* end B.3a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3b ACCESSORS — State Queries [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_impl_accessors B.3b ACCESSORS — State Queries
 * @ingroup mouse_impl_coreops
 * @{ */

int mouse_is_pressed(MouseButton button) {
    if (!g_initialized || button >= MOUSE_BUTTON_COUNT) {
        return 0;
    }
    return g_button_states[button] ? 1 : 0;
}

uint8_t mouse_get_buttons(void) {
    if (!g_initialized) {
        return 0;
    }
    uint8_t mask = 0;
    for (int i = 0; i < MOUSE_BUTTON_COUNT && i < 8; i++) {
        if (g_button_states[i]) {
            mask |= (1 << i);
        }
    }
    return mask;
}

void mouse_get_position(int32_t* x, int32_t* y) {
    if (x) *x = g_pos_x;
    if (y) *y = g_pos_y;
}

void mouse_get_delta(int32_t* dx, int32_t* dy) {
    if (dx) *dx = g_delta_x;
    if (dy) *dy = g_delta_y;
}

const char* mouse_get_button_name(MouseButton button) {
    static const char* names[] = {
        [MOUSE_BUTTON_NONE]   = "NONE",
        [MOUSE_BUTTON_LEFT]   = "LEFT",
        [MOUSE_BUTTON_RIGHT]  = "RIGHT",
        [MOUSE_BUTTON_MIDDLE] = "MIDDLE",
        [MOUSE_BUTTON_X1]     = "X1",
        [MOUSE_BUTTON_X2]     = "X2",
        [MOUSE_BUTTON_X3]     = "X3",
        [MOUSE_BUTTON_X4]     = "X4",
        [MOUSE_BUTTON_X5]     = "X5",
    };

    if (button < MOUSE_BUTTON_COUNT && names[button]) {
        return names[button];
    }
    return "UNKNOWN";
}

/** @} */ /* end B.3b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3c OPERATIONS — Input Operations [INPUT_OPS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_impl_inputops B.3c OPERATIONS — Input Operations
 * @ingroup mouse_impl_coreops
 * @{ */

int mouse_poll(MouseState* state) {
    if (!g_initialized || state == NULL) {
        return 0;
    }

    /* Check for GUI-injected events first */
    if (g_has_pending_event) {
        *state = g_pending_event;
        g_has_pending_event = false;
        return 1;
    }

    /* Calculate delta from position change */
    g_delta_x = g_pos_x - g_prev_x;
    g_delta_y = g_pos_y - g_prev_y;
    g_prev_x = g_pos_x;
    g_prev_y = g_pos_y;

    /* No mouse events in terminal mode without xterm mouse support */
    return 0;
}

MouseError mouse_wait_event(MouseState* state) {
    if (!g_initialized) {
        return MOUSE_ERR_NOT_INIT;
    }
    if (state == NULL) {
        return MOUSE_ERR_INVALID;
    }
    /* No blocking mouse in terminal mode */
    return MOUSE_ERR_INVALID;
}

void mouse_flush(void) {
    if (!g_initialized) {
        return;
    }

    mos_memset(g_button_states, 0, sizeof(g_button_states));
    g_delta_x = 0;
    g_delta_y = 0;
    g_scroll_x = 0;
    g_scroll_y = 0;
    g_has_pending_event = false;
    g_has_pending_gesture = false;
}

MouseError mouse_warp(int32_t x, int32_t y) {
    if (!g_initialized) {
        return MOUSE_ERR_NOT_INIT;
    }
    g_pos_x = x;
    g_pos_y = y;
    return MOUSE_OK;
}

void mouse_set_visible(int visible) {
    g_visible = (visible != 0);
}

int mouse_is_visible(void) {
    return g_visible ? 1 : 0;
}

/** @} */ /* end B.3c OPERATIONS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3d INJECTION — GUI Backend Support [INJECTION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_impl_injection B.3d INJECTION — GUI Backend Support
 * @ingroup mouse_impl_coreops
 * @{ */

void mouse_inject_position(int32_t x, int32_t y) {
    if (!g_initialized) {
        return;
    }
    g_pos_x = x;
    g_pos_y = y;

    /* Queue a move event */
    g_pending_event.type = MOUSE_EVENT_MOVE;
    g_pending_event.x = x;
    g_pending_event.y = y;
    g_pending_event.dx = x - g_prev_x;
    g_pending_event.dy = y - g_prev_y;
    g_pending_event.buttons = mouse_get_buttons();
    g_has_pending_event = true;
}

void mouse_inject_button(MouseButton button, int pressed) {
    if (!g_initialized || button >= MOUSE_BUTTON_COUNT) {
        return;
    }

    g_button_states[button] = (pressed != 0);

    /* Queue a button event */
    g_pending_event.type = MOUSE_EVENT_BUTTON;
    g_pending_event.x = g_pos_x;
    g_pending_event.y = g_pos_y;
    g_pending_event.dx = 0;
    g_pending_event.dy = 0;
    g_pending_event.button = button;
    g_pending_event.buttons = mouse_get_buttons();
    g_pending_event.pressed = (pressed != 0) ? 1 : 0;
    g_pending_event.clicks = 1;
    g_has_pending_event = true;
}

void mouse_inject_scroll(int16_t dx, int16_t dy) {
    if (!g_initialized) {
        return;
    }

    g_scroll_x += dx;
    g_scroll_y += dy;

    /* Queue a scroll event */
    g_pending_event.type = MOUSE_EVENT_SCROLL;
    g_pending_event.x = g_pos_x;
    g_pending_event.y = g_pos_y;
    g_pending_event.scroll_x = dx;
    g_pending_event.scroll_y = dy;
    g_pending_event.buttons = mouse_get_buttons();
    g_has_pending_event = true;
}

void mouse_inject_event(const MouseState* state) {
    if (!g_initialized || state == NULL) {
        return;
    }

    /* Update internal state based on event type */
    switch (state->type) {
        case MOUSE_EVENT_MOVE:
            g_pos_x = state->x;
            g_pos_y = state->y;
            break;
        case MOUSE_EVENT_BUTTON:
            if (state->button < MOUSE_BUTTON_COUNT) {
                g_button_states[state->button] = state->pressed;
            }
            break;
        case MOUSE_EVENT_SCROLL:
            g_scroll_x = state->scroll_x;
            g_scroll_y = state->scroll_y;
            break;
        default:
            break;
    }

    g_pending_event = *state;
    g_has_pending_event = true;
}

/**
 * @brief Inject PS/2 mouse packet data.
 *
 * Called by HAL PS/2 mouse driver to feed raw mouse events into the
 * mouse subsystem. Converts PS/2-style data to MouseState.
 *
 * @param[in] x        Absolute X position (after HAL clamping).
 * @param[in] y        Absolute Y position (after HAL clamping).
 * @param[in] dx       X movement delta.
 * @param[in] dy       Y movement delta.
 * @param[in] buttons  Button bitmask (bit 0=left, 1=right, 2=middle).
 *
 * USAGE (hal.c mouse_handler):
 * @code{.c}
 * mouse_inject_ps2(g_hal.mouse_x, g_hal.mouse_y,
 *                  mouse_event.dx, mouse_event.dy,
 *                  mouse_event.buttons);
 * @endcode
 */
void mouse_inject_ps2(int32_t x, int32_t y, int32_t dx, int32_t dy, uint8_t buttons) {
    if (!g_initialized) {
        return;
    }

    /* Update internal position */
    g_pos_x = x;
    g_pos_y = y;

    /* Track button state changes */
    bool left_now = (buttons & 0x01) != 0;
    bool right_now = (buttons & 0x02) != 0;
    bool middle_now = (buttons & 0x04) != 0;

    bool left_was = g_button_states[MOUSE_BUTTON_LEFT];
    bool right_was = g_button_states[MOUSE_BUTTON_RIGHT];
    bool middle_was = g_button_states[MOUSE_BUTTON_MIDDLE];

    g_button_states[MOUSE_BUTTON_LEFT] = left_now;
    g_button_states[MOUSE_BUTTON_RIGHT] = right_now;
    g_button_states[MOUSE_BUTTON_MIDDLE] = middle_now;

    /* Determine event type */
    MouseEventType event_type = MOUSE_EVENT_NONE;
    MouseButton changed_button = MOUSE_BUTTON_NONE;
    uint8_t pressed = 0;

    /* Check for button changes */
    if (left_now != left_was) {
        event_type = MOUSE_EVENT_BUTTON;
        changed_button = MOUSE_BUTTON_LEFT;
        pressed = left_now ? 1 : 0;
    } else if (right_now != right_was) {
        event_type = MOUSE_EVENT_BUTTON;
        changed_button = MOUSE_BUTTON_RIGHT;
        pressed = right_now ? 1 : 0;
    } else if (middle_now != middle_was) {
        event_type = MOUSE_EVENT_BUTTON;
        changed_button = MOUSE_BUTTON_MIDDLE;
        pressed = middle_now ? 1 : 0;
    } else if (dx != 0 || dy != 0) {
        event_type = MOUSE_EVENT_MOVE;
    }

    if (event_type == MOUSE_EVENT_NONE) {
        return;  /* No event to report */
    }

    /* Build MouseState event */
    g_pending_event.type = event_type;
    g_pending_event.x = x;
    g_pending_event.y = y;
    g_pending_event.dx = dx;
    g_pending_event.dy = dy;
    g_pending_event.scroll_x = 0;
    g_pending_event.scroll_y = 0;
    g_pending_event.button = changed_button;
    g_pending_event.buttons = buttons;
    g_pending_event.pressed = pressed;
    g_pending_event.clicks = (pressed && event_type == MOUSE_EVENT_BUTTON) ? 1 : 0;

    g_has_pending_event = true;
}

/** @} */ /* end B.3d INJECTION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3e TOUCH — Multi-touch Support [TOUCH]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_impl_touch B.3e TOUCH — Multi-touch Support
 * @ingroup mouse_impl_coreops
 * @{ */

int mouse_get_touch_count(void) {
    if (!g_initialized) {
        return 0;
    }
    return g_touch.count;
}

int mouse_get_touch(int index, TouchPoint* point) {
    if (!g_initialized || point == NULL || index < 0 || index >= g_touch.count) {
        return 0;
    }
    *point = g_touch.points[index];
    return 1;
}

int mouse_get_touch_by_id(int32_t id, TouchPoint* point) {
    if (!g_initialized || point == NULL) {
        return 0;
    }
    for (int i = 0; i < g_touch.count; i++) {
        if (g_touch.points[i].id == id) {
            *point = g_touch.points[i];
            return 1;
        }
    }
    return 0;
}

void mouse_inject_touch(const TouchPoint* point) {
    if (!g_initialized || point == NULL) {
        return;
    }

    /* Find existing touch with this ID or add new */
    int found = -1;
    for (int i = 0; i < g_touch.count; i++) {
        if (g_touch.points[i].id == point->id) {
            found = i;
            break;
        }
    }

    if (point->phase == TOUCH_PHASE_BEGAN) {
        /* New touch */
        if (g_touch.count < MOUSE_TOUCH_MAX) {
            g_touch.points[g_touch.count] = *point;
            g_touch.count++;

            /* Record gesture start position on first touch */
            if (g_touch.count == 1) {
                g_touch.start_x = point->x;
                g_touch.start_y = point->y;
                g_touch.start_time = get_timestamp_ms();
            }
        }
    } else if (found >= 0) {
        /* Update existing touch */
        TouchPoint* existing = &g_touch.points[found];
        existing->dx = point->x - existing->x;
        existing->dy = point->y - existing->y;
        existing->x = point->x;
        existing->y = point->y;
        existing->pressure = point->pressure;
        existing->phase = point->phase;

        /* Remove ended/cancelled touches */
        if (point->phase == TOUCH_PHASE_ENDED ||
            point->phase == TOUCH_PHASE_CANCELLED) {

            /* Check for gesture */
            GestureType gesture = recognize_gesture();
            if (gesture != GESTURE_NONE) {
                g_pending_gesture.type = gesture;
                g_pending_gesture.x = point->x;
                g_pending_gesture.y = point->y;
                g_pending_gesture.touch_count = (uint8_t)g_touch.count;
                g_has_pending_gesture = true;
            }

            /* Remove from array */
            for (int i = found; i < g_touch.count - 1; i++) {
                g_touch.points[i] = g_touch.points[i + 1];
            }
            g_touch.count--;
        }
    }
}

/** @} */ /* end B.3e TOUCH */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3f GESTURE — Gesture Recognition [GESTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_impl_gesture B.3f GESTURE — Gesture Recognition
 * @ingroup mouse_impl_coreops
 * @{ */

int mouse_poll_gesture(GestureState* gesture) {
    if (!g_initialized || gesture == NULL || !g_gesture_enabled) {
        return 0;
    }

    if (g_has_pending_gesture) {
        *gesture = g_pending_gesture;
        g_has_pending_gesture = false;
        return 1;
    }

    return 0;
}

void mouse_set_gesture_enabled(int enabled) {
    g_gesture_enabled = (enabled != 0);
}

int mouse_is_gesture_enabled(void) {
    return g_gesture_enabled ? 1 : 0;
}

void mouse_inject_gesture(const GestureState* gesture) {
    if (!g_initialized || gesture == NULL) {
        return;
    }
    g_pending_gesture = *gesture;
    g_has_pending_gesture = true;
}

/** @} */ /* end B.3f GESTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3g XTERM — Terminal Mouse Protocol [XTERM]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_impl_xterm B.3g XTERM — Terminal Mouse Protocol
 * @ingroup mouse_impl_coreops
 * @brief    Xterm mouse tracking for TUI applications.
 *
 * Xterm mouse protocols allow terminal emulators to report mouse events
 * via escape sequences. This supports WezTerm, iTerm2, Alacritty, kitty,
 * gnome-terminal, and other modern terminals.
 *
 * ESCAPE SEQUENCES:
 *   Enable:  CSI ? <mode> h   (e.g., CSI ? 1003 h for any-event)
 *   Disable: CSI ? <mode> l
 *
 * MODES:
 *   1000 = Normal (press/release)
 *   1002 = Button (press/release/motion with button)
 *   1003 = Any-event (all motion)
 *   1006 = SGR encoding (unlimited coordinates)
 *
 * @{ */

/**
 * @brief Write a DEC private mode escape sequence.
 *
 * @param[in] mode_code  Mode number (e.g., 1000, 1003, 1006).
 * @param[in] enable     1 to enable (h), 0 to disable (l).
 * @return Number of bytes written, or -1 on error.
 *
 * @note In freestanding kernel mode, xterm protocols don't apply.
 *       We access PS/2 mouse hardware directly via HAL.
 *       This function is stubbed for API compatibility.
 */
int mouse_xterm_write_mode(int mode_code, int enable) {
    /* Freestanding kernel — no terminal emulator, no xterm protocols.
     * Direct hardware access via PS/2 mouse driver in hal.c.
     * Stub for API compatibility with userspace builds. */
    (void)mode_code;
    (void)enable;
    return 0;
}

/**
 * @brief Get the DEC mode code for an XtermMouseMode.
 *
 * @param[in] mode  XtermMouseMode value.
 * @return DEC private mode code, or 0 if none.
 */
static int xterm_mode_to_code(XtermMouseMode mode) {
    switch (mode) {
        case XTERM_MOUSE_X10:    return 9;
        case XTERM_MOUSE_NORMAL: return 1000;
        case XTERM_MOUSE_BUTTON: return 1002;
        case XTERM_MOUSE_ANY:    return 1003;
        default:                 return 0;
    }
}

/**
 * @brief Get the DEC mode code for an XtermMouseEncoding.
 *
 * @param[in] encoding  XtermMouseEncoding value.
 * @return DEC private mode code, or 0 if none.
 */
static int xterm_encoding_to_code(XtermMouseEncoding encoding) {
    switch (encoding) {
        case XTERM_ENC_UTF8:  return 1005;
        case XTERM_ENC_SGR:   return 1006;
        case XTERM_ENC_URXVT: return 1015;
        default:              return 0;  /* DEFAULT uses no extra mode */
    }
}

void mouse_xterm_enable(void) {
    if (g_xterm_enabled) {
        return;
    }

    /* Enable tracking mode */
    int mode_code = xterm_mode_to_code(g_xterm_config.mode);
    if (mode_code > 0) {
        mouse_xterm_write_mode(mode_code, 1);
    }

    /* Enable encoding mode */
    int enc_code = xterm_encoding_to_code(g_xterm_config.encoding);
    if (enc_code > 0) {
        mouse_xterm_write_mode(enc_code, 1);
    }

    /* Enable focus reporting if configured */
    if (g_xterm_config.focus) {
        mouse_xterm_write_mode(1004, 1);
    }

    /* Freestanding kernel — no stdout to flush */

    g_xterm_enabled = true;
}

void mouse_xterm_disable(void) {
    if (!g_xterm_enabled) {
        return;
    }

    /* Disable focus reporting */
    if (g_xterm_config.focus) {
        mouse_xterm_write_mode(1004, 0);
    }

    /* Disable encoding mode */
    int enc_code = xterm_encoding_to_code(g_xterm_config.encoding);
    if (enc_code > 0) {
        mouse_xterm_write_mode(enc_code, 0);
    }

    /* Disable tracking mode */
    int mode_code = xterm_mode_to_code(g_xterm_config.mode);
    if (mode_code > 0) {
        mouse_xterm_write_mode(mode_code, 0);
    }

    /* Freestanding kernel — no stdout to flush */

    g_xterm_enabled = false;
}

void mouse_xterm_configure(const XtermMouseConfig* config) {
    if (config == NULL) {
        return;
    }

    bool was_enabled = g_xterm_enabled;

    /* Disable current config if enabled */
    if (was_enabled) {
        mouse_xterm_disable();
    }

    /* Apply new config */
    g_xterm_config = *config;

    /* Re-enable with new config */
    if (was_enabled) {
        mouse_xterm_enable();
    }
}

void mouse_xterm_get_config(XtermMouseConfig* config) {
    if (config != NULL) {
        *config = g_xterm_config;
    }
}

int mouse_xterm_is_enabled(void) {
    return g_xterm_enabled ? 1 : 0;
}

void mouse_xterm_reset_parser(void) {
    g_xterm_parser_state = XTERM_PARSE_IDLE;
    g_xterm_seq_len = 0;
}

/**
 * @brief Convert xterm button code to MouseButton.
 *
 * @param[in] cb         Button code from xterm sequence.
 * @param[out] button    MouseButton result.
 * @param[out] pressed   Press state (1=pressed, 0=released).
 * @param[out] is_scroll Scroll wheel event flag.
 */
static void xterm_decode_button(int cb, MouseButton* button, int* pressed, int* is_scroll) {
    *is_scroll = 0;
    *pressed = 1;

    /* Low 2 bits: button number (0=left, 1=middle, 2=right, 3=release) */
    int btn = cb & 0x03;

    /* Bit 5 (32): motion event */
    /* Bit 6 (64): scroll event */

    if (cb & 64) {
        /* Scroll wheel */
        *is_scroll = 1;
        *button = MOUSE_BUTTON_NONE;
    } else if (btn == 0) {
        *button = MOUSE_BUTTON_LEFT;
    } else if (btn == 1) {
        *button = MOUSE_BUTTON_MIDDLE;
    } else if (btn == 2) {
        *button = MOUSE_BUTTON_RIGHT;
    } else {
        /* btn == 3 means release in X10 mode */
        *button = MOUSE_BUTTON_NONE;
        *pressed = 0;
    }
}

/**
 * @brief Parse SGR-style mouse sequence (CSI < Cb ; Cx ; Cy M/m).
 *
 * @param[in]  buf    Sequence buffer (after CSI <).
 * @param[in]  len    Buffer length.
 * @param[out] state  MouseState to fill.
 * @return Bytes consumed, 0 if incomplete, -1 if invalid.
 */
static int parse_sgr_sequence(const uint8_t* buf, size_t len, MouseState* state) {
    /* Find terminator (M for press, m for release) */
    size_t term_pos = 0;
    bool found_term = false;
    bool is_release = false;

    for (size_t i = 0; i < len; i++) {
        if (buf[i] == 'M') {
            term_pos = i;
            found_term = true;
            is_release = false;
            break;
        }
        if (buf[i] == 'm') {
            term_pos = i;
            found_term = true;
            is_release = true;
            break;
        }
    }

    if (!found_term) {
        return 0;  /* Incomplete */
    }

    /* Parse Cb;Cx;Cy */
    int cb = 0, cx = 0, cy = 0;
    int field = 0;
    int value = 0;

    for (size_t i = 0; i < term_pos; i++) {
        if (buf[i] >= '0' && buf[i] <= '9') {
            value = value * 10 + (buf[i] - '0');
        } else if (buf[i] == ';') {
            if (field == 0) cb = value;
            else if (field == 1) cx = value;
            field++;
            value = 0;
        }
    }
    cy = value;  /* Last field */

    /* Convert to MouseState */
    mos_memset(state, 0, sizeof(*state));

    MouseButton button;
    int pressed;
    int is_scroll;
    xterm_decode_button(cb, &button, &pressed, &is_scroll);

    /* SGR uses 'm' for release */
    if (is_release) {
        pressed = 0;
    }

    /* Coordinates are 1-based in xterm, convert to 0-based */
    state->x = cx - 1;
    state->y = cy - 1;

    if (is_scroll) {
        state->type = MOUSE_EVENT_SCROLL;
        /* Scroll direction from bit 0 of cb */
        state->scroll_y = (cb & 1) ? -1 : 1;
    } else if (cb & 32) {
        /* Motion event */
        state->type = MOUSE_EVENT_MOVE;
    } else {
        state->type = MOUSE_EVENT_BUTTON;
        state->button = button;
        state->pressed = pressed ? 1 : 0;
    }

    return (int)(term_pos + 1);
}

/**
 * @brief Parse X10-style mouse sequence (CSI M Cb Cx Cy).
 *
 * @param[in]  buf    Sequence buffer (after CSI M, includes Cb Cx Cy).
 * @param[in]  len    Buffer length.
 * @param[out] state  MouseState to fill.
 * @return Bytes consumed, 0 if incomplete, -1 if invalid.
 */
static int parse_x10_sequence(const uint8_t* buf, size_t len, MouseState* state) {
    if (len < 3) {
        return 0;  /* Incomplete */
    }

    /* Cb Cx Cy are raw bytes, 32-based encoding */
    int cb = buf[0] - 32;
    int cx = buf[1] - 32;
    int cy = buf[2] - 32;

    mos_memset(state, 0, sizeof(*state));

    MouseButton button;
    int pressed;
    int is_scroll;
    xterm_decode_button(cb, &button, &pressed, &is_scroll);

    /* Convert to 0-based coordinates */
    state->x = cx - 1;
    state->y = cy - 1;

    if (is_scroll) {
        state->type = MOUSE_EVENT_SCROLL;
        state->scroll_y = (cb & 1) ? -1 : 1;
    } else if (cb & 32) {
        state->type = MOUSE_EVENT_MOVE;
    } else {
        state->type = MOUSE_EVENT_BUTTON;
        state->button = button;
        state->pressed = pressed ? 1 : 0;
    }

    return 3;
}

int mouse_xterm_parse_byte(uint8_t byte) {
    if (!g_initialized) {
        return 0;
    }

    switch (g_xterm_parser_state) {
        case XTERM_PARSE_IDLE:
            if (byte == 0x1B) {  /* ESC */
                g_xterm_parser_state = XTERM_PARSE_ESC;
                g_xterm_seq_len = 0;
            }
            return 0;

        case XTERM_PARSE_ESC:
            if (byte == '[') {
                g_xterm_parser_state = XTERM_PARSE_CSI;
            } else {
                g_xterm_parser_state = XTERM_PARSE_IDLE;
            }
            return 0;

        case XTERM_PARSE_CSI:
            if (byte == '<') {
                /* SGR extended mode */
                g_xterm_parser_state = XTERM_PARSE_SGR;
                g_xterm_seq_len = 0;
            } else if (byte == 'M') {
                /* X10 mode */
                g_xterm_parser_state = XTERM_PARSE_X10;
                g_xterm_seq_len = 0;
            } else {
                /* Not a mouse sequence */
                g_xterm_parser_state = XTERM_PARSE_IDLE;
            }
            return 0;

        case XTERM_PARSE_SGR:
            if (g_xterm_seq_len < XTERM_MOUSE_SEQ_MAX - 1) {
                g_xterm_seq_buf[g_xterm_seq_len++] = byte;
            }

            if (byte == 'M' || byte == 'm') {
                /* Complete SGR sequence */
                MouseState state;
                if (parse_sgr_sequence(g_xterm_seq_buf, g_xterm_seq_len, &state) > 0) {
                    mouse_inject_event(&state);
                    g_xterm_parser_state = XTERM_PARSE_IDLE;
                    return 1;
                }
            }

            /* Timeout or overflow */
            if (g_xterm_seq_len >= XTERM_MOUSE_SEQ_MAX - 1) {
                g_xterm_parser_state = XTERM_PARSE_IDLE;
            }
            return 0;

        case XTERM_PARSE_X10:
            if (g_xterm_seq_len < 3) {
                g_xterm_seq_buf[g_xterm_seq_len++] = byte;
            }

            if (g_xterm_seq_len == 3) {
                /* Complete X10 sequence */
                MouseState state;
                if (parse_x10_sequence(g_xterm_seq_buf, 3, &state) > 0) {
                    mouse_inject_event(&state);
                    g_xterm_parser_state = XTERM_PARSE_IDLE;
                    return 1;
                }
                g_xterm_parser_state = XTERM_PARSE_IDLE;
            }
            return 0;
    }

    return 0;
}

int mouse_xterm_parse_sequence(const uint8_t* buf, size_t len, MouseState* state) {
    if (buf == NULL || state == NULL || len < 3) {
        return -1;
    }

    /* Check for ESC [ */
    if (buf[0] != 0x1B || buf[1] != '[') {
        return -1;
    }

    /* SGR: ESC [ < Cb ; Cx ; Cy M/m */
    if (len > 3 && buf[2] == '<') {
        int consumed = parse_sgr_sequence(buf + 3, len - 3, state);
        if (consumed > 0) {
            return consumed + 3;
        }
        return consumed;
    }

    /* X10: ESC [ M Cb Cx Cy */
    if (len >= 6 && buf[2] == 'M') {
        int consumed = parse_x10_sequence(buf + 3, len - 3, state);
        if (consumed > 0) {
            return consumed + 3;
        }
        return consumed;
    }

    return -1;
}

/** @} */ /* end B.3g XTERM */

/** @} */ /* end B.3 CORE OPS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4-B.6 — [Collapsed: Standard patterns] [COLLAPSED]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_collapsed B.4-B.6 COLLAPSED — Standard Patterns
 * @brief    [Reserved: Error handling, public API, and omission sections]
 *
 * B.4 ERRORS:     Error handling via return codes (MouseError enum)
 * B.5 PUBLIC API: All public functions in B.3 CORE OPS
 * B.6 OMISSION:   All BODY sections populated
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/* [Reserved: B.4-B.6 collapsed — standard patterns apply] */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Operational guidance and closing notes.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Build and Test [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_validation X.1 VALIDATION — Build and Test
 * @brief    Verify implementation correctness.
 *
 * BUILD:
 * @code{.sh}
 * gcc -c -Wall -Wextra -std=c11 mouse.c
 * @endcode
 *
 * TEST:
 * @code{.c}
 * mouse_init();
 * mouse_inject_position(100, 200);
 * MouseState state;
 * if (mouse_poll(&state)) {
 *     printf("Mouse at (%d, %d)\n", state.x, state.y);
 * }
 * mouse_shutdown();
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Runtime Behavior [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_execution X.2 EXECUTION — Runtime Behavior
 * @brief    How this module behaves at runtime.
 *
 * Terminal mode: Stub implementation (no direct mouse access)
 * GUI mode:      Use mouse_inject_* functions from SDL2/GLFW event handlers
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Resource Management [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_cleanup X.3 CLEANUP — Resource Management
 * @brief    Resource cleanup guidance.
 *
 * mouse_shutdown() resets all state.
 * Safe to call multiple times (idempotent).
 * No dynamic allocation — no memory leaks possible.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidelines [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_policy X.4 POLICY — Modification Guidelines
 * @brief    Guide future maintainers.
 *
 * SAFE TO MODIFY:
 *   - ✅ Add new gesture types
 *   - ✅ Tune gesture detection thresholds
 *   - ✅ Add touch pressure implementation
 *   - ✅ Add xterm mouse support
 *
 * MODIFY WITH CARE:
 *   - ⚠️ inject_* function behavior (backends depend on it)
 *   - ⚠️ Touch point management (gesture recognition depends on it)
 *
 * NEVER MODIFY:
 *   - ❌ mouse_init/shutdown contract
 *   - ❌ 4-block structure
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — Planned Upgrades [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_extension X.5 EXTENSION — Planned Upgrades
 * @brief    Roadmap for expansion.
 *
 * ADDING XTERM MOUSE SUPPORT:
 *   1. Parse CSI M sequences in terminal input
 *   2. Convert xterm button codes to MouseButton
 *   3. Update mouse_poll to check terminal input
 *
 * ADDING PRESSURE SUPPORT:
 *   1. GUI backends provide pressure value
 *   2. Store in TouchPoint.pressure
 *   3. Expose via API
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_troubleshoot X.6 TROUBLESHOOTING — Common Problems
 * @brief    Common issues and solutions.
 *
 * "mouse_poll returns 0 but mouse is moving":
 *   - Ensure GUI backend is calling mouse_inject_position
 *   - Verify mouse_init() was called
 *
 * "Gestures not detected":
 *   - Check mouse_is_gesture_enabled()
 *   - Verify touch events have correct phase lifecycle
 *
 * "Position jumps around":
 *   - Don't call both mouse_inject_position and mouse_inject_event
 *   - Pick one injection method
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_reference X.7 REFERENCE — Quick Lookup
 * @brief    Copy-paste ready examples.
 *
 * @par SDL2 Integration
 * @code{.c}
 * while (SDL_PollEvent(&event)) {
 *     switch (event.type) {
 *         case SDL_MOUSEMOTION:
 *             mouse_inject_position(event.motion.x, event.motion.y);
 *             break;
 *         case SDL_MOUSEBUTTONDOWN:
 *             mouse_inject_button(event.button.button, 1);
 *             break;
 *         case SDL_MOUSEBUTTONUP:
 *             mouse_inject_button(event.button.button, 0);
 *             break;
 *         case SDL_MOUSEWHEEL:
 *             mouse_inject_scroll(event.wheel.x, event.wheel.y);
 *             break;
 *     }
 * }
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_impl_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * @par Scripture
 * "Let thine eyes look right on, and let thine eyelids look straight before thee.
 *  Ponder the path of thy feet, and let all thy ways be established."
 *   — Proverbs 4:25-26
 *
 * Pointing reveals direction — where user aims, attention follows.
 * Track position with purpose.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════
