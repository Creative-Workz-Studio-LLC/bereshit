/// @file mouse.h
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
/// @omni:req  key        = MOS-HAL-INPUT-MOUSE
/// @omni:req  from       = bereshit/word/seed/code/c/header.h
/// @omni:req  at         = a-01.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = foundation
/// @omni:inh  role       = input
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = engine/core/input
/// @omni:ins  layer      = kernel (layer 0)
/// @omni:ins  includes   = stdint, stdbool
/// @omni:ins  provides   = [MouseButton, MouseState, mouse_*]
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
/// @omni:req  title      = Mouse Input Interface
/// @omni:req  brief      = Kernel-level mouse input — button, position, and scroll API
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

/* ─────────────────────────────────────────────────────────────────────────────
 * INCLUDE GUARD — Identity protection
 * ───────────────────────────────────────────────────────────────────────────── */

#ifndef MOS_HAL_INPUT_MOUSE_H
#define MOS_HAL_INPUT_MOUSE_H

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Identity and context for this component.
///
/// STRUCTURE: M.1-M.10 sections, grouped for readability:
///   - M.1 IDENTITY:    Core identity (file, brief, key, title, type, component, role)
///   - M.2-M.6:         State, Attribution, Location, Derivation, Classification
///   - M.7-M.10:        Intent, Grounding, Dependencies, Roadmap
///
/// DOMAIN FILES: Use condensed M.2-M.6 and M.7-M.10 groupings.
/// FOUNDATION FILES: May expand individual M.X sections as needed.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @file      mouse.h
 * @brief     Mouse Input — Kernel-level button, position, and scroll tracking.
 *
 * @defgroup mouse_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       MOS-HAL-INPUT-MOUSE
 * Title:     Mouse Input Interface
 * Type:      Header (Foundation)
 * Component: Engine/Core/Input — kernel-level peripheral abstraction
 * Role:      Raw button codes, position tracking, scroll events
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_state M.2 STATE — Lifecycle State
 * @{
 *
 * @version   a-01.00
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
 * @defgroup mouse_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup mouse_location M.4 LOCATION — File Path
 * @{
 *
 * Path: millenniumos/kernel/include/hal/input/mouse.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: bereshit/word/seed/code/c/header.h
 * Pattern:      keyboard.h (sibling input peripheral)
 *
 * Builds FROM:  None (foundation module)
 * Builds TO:    keybind.h (action mapping), display backends
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: header, c, engine, core, input, mouse, buttons, position, scroll, kernel, peripheral
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Kernel-level mouse input abstraction
 * Core Design: MouseButton enum + position + scroll, separate from display
 * Philosophy:  Input devices are peripherals, not display features
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Proverbs 4:25-26 — "Let thine eyes look right on... Ponder the path of thy feet"
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
 * @defgroup mouse_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Standard: stdint.h, stdbool.h
 *   Internal: None (foundation module)
 *
 * What Uses This:
 *   - keybind.h — action mapping layer
 *   - hal.c — PS/2 mouse handler injects mouse events
 *   - shell, window manager — input handling (via keybind)
 *
 * @see keyboard.h for sibling input peripheral
 * @see keybind.h for action abstraction layer
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-26) — Initial creation alongside keyboard.h
 *   a-01.01 (2026-01-26) — GUI injection, multi-touch, gesture recognition
 *   a-01.02 (2026-01-26) — Xterm mouse protocol support (TUI systems)
 *
 * Current:
 *   ✓ MouseButton enum (left, right, middle, extra)
 *   ✓ Position tracking (x, y, delta)
 *   ✓ Scroll events (wheel, horizontal)
 *   ✓ GUI injection API (position, button, scroll, event)
 *   ✓ Multi-touch support (TouchPoint, TouchPhase)
 *   ✓ Gesture recognition (GestureType, GestureState)
 *   ✓ Xterm mouse protocol (X10, normal, SGR, urxvt)
 *
 * Planned:
 *   ⏳ Pressure/tilt support for tablets
 *   ⏳ Raw input mode bypass
 *   ⏳ Cursor confinement regions
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
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - S.1 INCLUDES  — Header dependencies (Standard, Internal, External, CPP)
///   - S.2 DEFINES   — Constants and macros (Sizes, Limits, Flags, Macros)
///   - S.3 TYPES     — Type declarations
///   - S.4 PROTOS    — Function prototypes
///   - S.5 EXTERN    — External declarations
///
/// Section order: Includes → Defines → Types → Prototypes → Extern
/// Flow: dependencies → constants → data model → interface → shared state
///
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * CODE SECTIONS [CODE_SECTIONS]
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard library includes for mouse module.
 *
 * PURPOSE: Foundation module — includes system headers directly.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — System headers (stdint, stdbool)
 *   - S.1b INTERNAL  — [None — foundation module]
 *   - S.1c EXTERNAL  — [None]
 *   - S.1d CPP       — C++ linkage wrapper
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_standard S.1a STANDARD — System Headers
 * @ingroup mouse_includes
 * @brief    C standard library headers required by this module.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include <stdint.h>   /**< Fixed-width integers: uint8_t, uint32_t */
#include <stdbool.h>  /**< Boolean type: bool, true, false */
#include <stddef.h>   /**< Standard definitions: size_t */

/** @} */ /* end S.1a STANDARD */

/** S.1b INTERNAL — [None — foundation module] @defgroup mouse_internal @ingroup mouse_includes @{ @} */

/** S.1c EXTERNAL — [None] @defgroup mouse_external @ingroup mouse_includes @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_cpp S.1d CPP — C++ Linkage
 * @ingroup mouse_includes
 * @brief    C++ compatibility wrapper for kernel input layer.
 *
 * FOUNDATION FILE: Opens extern "C" wrapper (closes in CLOSING block).
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#ifdef __cplusplus
extern "C" {
#endif

/** @} */ /* end S.1d CPP */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Preprocessor Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_defines S.2 DEFINES — Preprocessor Constants
 * @brief    Mouse input constants and limits.
 *
 * Builds FROM: S.1 INCLUDES (stdint types)
 * Builds TO:   S.3 TYPES (MouseButton enum)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/** S.2a SIZES — [Reserved] @defgroup mouse_sizes @ingroup mouse_defines @{ @} */

/**
 * @defgroup mouse_limits S.2b LIMITS — Mouse Button Bounds
 * @ingroup mouse_defines
 * @brief    Mouse button count limits.
 * @{ */
#define MOUSE_BUTTON_MAX                16     /**< Maximum button slots                 */
#define MOUSE_BUTTON_INVALID            0xFF   /**< Invalid button sentinel              */
/** @} */ /* end S.2b LIMITS */

/** S.2c FLAGS — [Reserved] @defgroup mouse_flags @ingroup mouse_defines @{ @} */
/** S.2d MACROS — [Reserved] @defgroup mouse_macros @ingroup mouse_defines @{ @} */
/** S.2e VERIFY — [Reserved] @defgroup mouse_verify @ingroup mouse_defines @{ @} */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_types S.3 TYPES — Type Declarations
 * @ingroup mouse
 * @brief    Mouse input types — MouseButton, MouseState.
 *
 * CONTAINS:
 *   S.3a ENUMS   — MouseButton (button identity), MouseEventType (event kinds)
 *   S.3b STRUCTS — MouseState (full mouse event with position + buttons)
 *   S.3c OPAQUE  — [Reserved]
 *   S.3d ERRORS  — MouseError codes
 *
 * Builds FROM: S.2 DEFINES (button count limits)
 * Builds TO:   S.4 PROTOTYPES (mouse API functions)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a ENUMS — MouseButton and MouseEventType [ENUMS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_enums S.3a ENUMS — Mouse Enumerations
 * @ingroup mouse_types
 * @brief    Button identity (MouseButton) and event types (MouseEventType).
 *
 * ARCHITECTURE:
 *   MouseButton    — WHICH button (identity)
 *   MouseEventType — WHAT happened (move, click, scroll)
 *   MouseState     — Full event (position + buttons + scroll)
 *
 * PATTERN:
 *   MOUSE_BUTTON_NONE  = 0   — invalid/no button (always 0)
 *   MOUSE_BUTTON_COUNT       — sentinel for array sizing
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief MouseButton represents mouse button identity.
 *
 * Hardware-level button identification.
 *
 * @note MOUSE_BUTTON_NONE is always 0 (no button / invalid).
 * @note MOUSE_BUTTON_COUNT is sentinel for array sizing.
 */
typedef enum MouseButton {
    /* ─── Core / Invalid ─── */
    MOUSE_BUTTON_NONE = 0,   /**< No button / invalid (always 0)                  */

    /* ─── Standard Buttons ─── */
    MOUSE_BUTTON_LEFT,       /**< Left mouse button (primary)                     */
    MOUSE_BUTTON_RIGHT,      /**< Right mouse button (secondary)                  */
    MOUSE_BUTTON_MIDDLE,     /**< Middle mouse button (scroll wheel click)        */

    /* ─── Extended Buttons ─── */
    MOUSE_BUTTON_X1,         /**< Extra button 1 (back/side)                      */
    MOUSE_BUTTON_X2,         /**< Extra button 2 (forward/side)                   */
    MOUSE_BUTTON_X3,         /**< Extra button 3                                  */
    MOUSE_BUTTON_X4,         /**< Extra button 4                                  */
    MOUSE_BUTTON_X5,         /**< Extra button 5                                  */

    /* ─── Sentinel ─── */
    MOUSE_BUTTON_COUNT       /**< Sentinel for array sizing (do not use as value) */
} MouseButton;

/**
 * @brief MouseEventType represents the kind of mouse event.
 *
 * Distinguishes between movement, button, and scroll events.
 */
typedef enum MouseEventType {
    MOUSE_EVENT_NONE = 0,    /**< No event / invalid                              */
    MOUSE_EVENT_MOVE,        /**< Mouse moved (position changed)                  */
    MOUSE_EVENT_BUTTON,      /**< Button pressed or released                      */
    MOUSE_EVENT_SCROLL,      /**< Scroll wheel moved                              */
    MOUSE_EVENT_ENTER,       /**< Mouse entered window/region                     */
    MOUSE_EVENT_LEAVE        /**< Mouse left window/region                        */
} MouseEventType;

/** @} */ /* end S.3a ENUMS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b STRUCTS — MouseState Structure [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_structs S.3b STRUCTS — Mouse Event Structures
 * @ingroup mouse_types
 * @brief    Full mouse event with position, buttons, and scroll.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief MouseState represents a complete mouse event.
 *
 * Combines position, button state, scroll delta, and event type
 * for full mouse event information.
 *
 * @note x, y are absolute position in pixels from top-left.
 * @note dx, dy are relative movement since last event.
 * @note scroll_x, scroll_y are scroll wheel deltas.
 * @note buttons is a bitmask of currently pressed buttons.
 */
typedef struct MouseState {
    /* ─── Event Type ─── */
    MouseEventType type;     /**< What kind of event this is                      */

    /* ─── Position ─── */
    int32_t  x;              /**< Absolute X position (pixels from left)          */
    int32_t  y;              /**< Absolute Y position (pixels from top)           */
    int32_t  dx;             /**< Relative X movement (delta)                     */
    int32_t  dy;             /**< Relative Y movement (delta)                     */

    /* ─── Scroll ─── */
    int16_t  scroll_x;       /**< Horizontal scroll delta (tilt wheel)            */
    int16_t  scroll_y;       /**< Vertical scroll delta (standard wheel)          */

    /* ─── Button State ─── */
    MouseButton button;      /**< Button involved (for MOUSE_EVENT_BUTTON)        */
    uint8_t  buttons;        /**< Bitmask of currently pressed buttons            */
    uint8_t  pressed;        /**< 1 = pressed (button down), 0 = released         */
    uint8_t  clicks;         /**< Click count (1=single, 2=double, 3=triple)      */
} MouseState;

/** @} */ /* end S.3b STRUCTS */

/** S.3c OPAQUE — [Reserved] @defgroup mouse_opaque @ingroup mouse_types @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3d ERRORS — Error Codes [ERRORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_errors S.3d ERRORS — Mouse Error Codes
 * @ingroup mouse_types
 * @brief    Mouse subsystem error codes.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief MouseError represents mouse subsystem errors.
 */
typedef enum MouseError {
    MOUSE_OK = 0,            /**< Success / no error                              */
    MOUSE_ERR_INIT,          /**< Initialization failed                           */
    MOUSE_ERR_NOT_INIT,      /**< Mouse not initialized                           */
    MOUSE_ERR_INVALID        /**< Invalid parameter                               */
} MouseError;

/** @} */ /* end S.3d ERRORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3e TOUCH — Multi-touch Support [TOUCH]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_touch S.3e TOUCH — Multi-touch Support
 * @ingroup mouse_types
 * @brief    Types for multi-touch and gesture input.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @brief Maximum simultaneous touch points supported. */
#define MOUSE_TOUCH_MAX  10

/**
 * @brief TouchPhase represents the lifecycle of a touch point.
 */
typedef enum TouchPhase {
    TOUCH_PHASE_NONE = 0,    /**< No touch / invalid                            */
    TOUCH_PHASE_BEGAN,       /**< Finger touched screen                         */
    TOUCH_PHASE_MOVED,       /**< Finger moved while touching                   */
    TOUCH_PHASE_STATIONARY,  /**< Finger touching but not moving                */
    TOUCH_PHASE_ENDED,       /**< Finger lifted from screen                     */
    TOUCH_PHASE_CANCELLED    /**< Touch cancelled (system interrupt)            */
} TouchPhase;

/**
 * @brief TouchPoint represents a single touch contact.
 *
 * Each finger/stylus contact has a unique ID that persists
 * for the duration of the touch.
 */
typedef struct TouchPoint {
    int32_t   id;            /**< Unique touch ID (persists for touch lifetime) */
    int32_t   x;             /**< Current X position                            */
    int32_t   y;             /**< Current Y position                            */
    int32_t   dx;            /**< Movement delta X since last update            */
    int32_t   dy;            /**< Movement delta Y since last update            */
    float     pressure;      /**< Pressure (0.0-1.0, 0 if not supported)        */
    TouchPhase phase;        /**< Current phase of this touch                   */
} TouchPoint;

/**
 * @brief GestureType represents recognized gesture patterns.
 */
typedef enum GestureType {
    GESTURE_NONE = 0,        /**< No gesture / invalid                          */
    GESTURE_TAP,             /**< Single tap                                    */
    GESTURE_DOUBLE_TAP,      /**< Double tap                                    */
    GESTURE_LONG_PRESS,      /**< Long press/hold                               */
    GESTURE_PAN,             /**< Single finger drag                            */
    GESTURE_PINCH,           /**< Two finger pinch (zoom)                       */
    GESTURE_ROTATE,          /**< Two finger rotation                           */
    GESTURE_SWIPE_LEFT,      /**< Quick swipe left                              */
    GESTURE_SWIPE_RIGHT,     /**< Quick swipe right                             */
    GESTURE_SWIPE_UP,        /**< Quick swipe up                                */
    GESTURE_SWIPE_DOWN       /**< Quick swipe down                              */
} GestureType;

/**
 * @brief GestureState represents a recognized gesture event.
 */
typedef struct GestureState {
    GestureType type;        /**< Type of gesture recognized                    */
    int32_t     x;           /**< Center X of gesture                           */
    int32_t     y;           /**< Center Y of gesture                           */
    float       scale;       /**< Scale factor for pinch (1.0 = no change)      */
    float       rotation;    /**< Rotation angle for rotate (radians)           */
    float       velocity_x;  /**< X velocity for swipes/pans                    */
    float       velocity_y;  /**< Y velocity for swipes/pans                    */
    uint8_t     touch_count; /**< Number of touches involved                    */
} GestureState;

/** @} */ /* end S.3e TOUCH */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3f XTERM — Terminal Mouse Protocol [XTERM]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_xterm_types S.3f XTERM — Terminal Mouse Protocol
 * @ingroup mouse_types
 * @brief    Types for Xterm mouse protocol support (TUI/WezTerm).
 *
 * Xterm mouse protocols allow terminal emulators (WezTerm, iTerm2, etc.) to
 * report mouse events via escape sequences. Multiple protocol versions exist
 * with different capabilities:
 *
 *   - X10 (1000): Button press only (no release, no motion)
 *   - Normal (1000): Button press and release
 *   - Button (1002): Press, release, and motion while button held
 *   - Any (1003): All mouse events including motion
 *   - SGR (1006): Extended coordinates (beyond 223)
 *   - urxvt (1015): Alternative extended encoding
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief XtermMouseMode represents the mouse tracking mode.
 *
 * Different modes enable different levels of mouse reporting:
 *   - NONE: No mouse tracking (default)
 *   - X10: Report button presses only (CSI ? 9 h)
 *   - NORMAL: Report press and release (CSI ? 1000 h)
 *   - BUTTON: Report press, release, and motion with button held (CSI ? 1002 h)
 *   - ANY: Report all motion events (CSI ? 1003 h)
 */
typedef enum XtermMouseMode {
    XTERM_MOUSE_NONE = 0,    /**< No mouse tracking (disabled)                  */
    XTERM_MOUSE_X10,         /**< X10 compatibility (button press only)         */
    XTERM_MOUSE_NORMAL,      /**< Normal tracking (press and release)           */
    XTERM_MOUSE_BUTTON,      /**< Button-event tracking (press/release/motion)  */
    XTERM_MOUSE_ANY          /**< Any-event tracking (all motion)               */
} XtermMouseMode;

/**
 * @brief XtermMouseEncoding represents the coordinate encoding format.
 *
 * Different encodings support different coordinate ranges:
 *   - DEFAULT: X10-compatible, coordinates limited to 0-222
 *   - UTF8: UTF-8 encoded (CSI ? 1005 h), supports up to 2015
 *   - SGR: SGR/CSI extended (CSI ? 1006 h), unlimited coordinates
 *   - URXVT: urxvt encoding (CSI ? 1015 h), decimal format
 */
typedef enum XtermMouseEncoding {
    XTERM_ENC_DEFAULT = 0,   /**< X10 compatible (max 223)                      */
    XTERM_ENC_UTF8,          /**< UTF-8 encoding (max ~2015)                    */
    XTERM_ENC_SGR,           /**< SGR extended mode (unlimited)                 */
    XTERM_ENC_URXVT          /**< urxvt decimal encoding                        */
} XtermMouseEncoding;

/**
 * @brief XtermMouseConfig configures terminal mouse tracking.
 *
 * Use with mouse_xterm_configure() to set tracking mode and encoding.
 */
typedef struct XtermMouseConfig {
    XtermMouseMode     mode;       /**< Tracking mode (what events to report)   */
    XtermMouseEncoding encoding;   /**< Coordinate encoding format              */
    bool               focus;      /**< Report focus in/out events              */
} XtermMouseConfig;

/**
 * @brief Maximum size for xterm escape sequence parsing.
 *
 * SGR sequences can be longer than traditional X10 sequences.
 * Format: CSI < Cb ; Cx ; Cy [Mm]
 */
#define XTERM_MOUSE_SEQ_MAX  32

/** @} */ /* end S.3f XTERM */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_prototypes S.4 PROTOTYPES — Function Declarations
 * @ingroup mouse
 * @brief    Mouse input API function declarations.
 *
 * STRUCTURE:
 *   S.4a LIFECYCLE  — mouse_init, mouse_shutdown
 *   S.4b ACCESSORS  — mouse_is_pressed, mouse_get_*
 *   S.4c OPERATIONS — mouse_poll, mouse_warp
 *
 * Builds FROM: S.3 TYPES (MouseButton, MouseState, MouseError)
 * Builds TO:   BODY (implementations in mouse.c)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a LIFECYCLE — Init/Shutdown [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_lifecycle S.4a LIFECYCLE — Init/Shutdown
 * @ingroup mouse_prototypes
 * @brief    Mouse subsystem lifecycle management.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize the mouse input subsystem.
 *
 * Must be called before any other mouse functions.
 * Safe to call multiple times (idempotent).
 *
 * @return MOUSE_OK on success, error code on failure.
 */
MouseError mouse_init(void);

/**
 * @brief Shutdown the mouse input subsystem.
 *
 * Releases all resources. Safe to call if not initialized (no-op).
 * After shutdown, mouse_init() must be called again to use mouse.
 */
void mouse_shutdown(void);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b ACCESSORS — State Queries [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_accessors S.4b ACCESSORS — State Queries
 * @ingroup mouse_prototypes
 * @brief    Query current mouse state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Check if a specific mouse button is currently pressed.
 *
 * @param[in] button  MouseButton to check.
 *
 * @return 1 if button is pressed, 0 if released or invalid.
 */
int mouse_is_pressed(MouseButton button);

/**
 * @brief Get bitmask of all currently pressed buttons.
 *
 * @return Bitmask where bit N corresponds to button N being pressed.
 */
uint8_t mouse_get_buttons(void);

/**
 * @brief Get current mouse position.
 *
 * @param[out] x  Pointer to store X position (pixels from left). May be NULL.
 * @param[out] y  Pointer to store Y position (pixels from top). May be NULL.
 */
void mouse_get_position(int32_t* x, int32_t* y);

/**
 * @brief Get mouse movement delta since last call.
 *
 * Returns relative movement, useful for mouse-look or drag operations.
 * Calling this resets the internal delta accumulator.
 *
 * @param[out] dx  Pointer to store X delta. May be NULL.
 * @param[out] dy  Pointer to store Y delta. May be NULL.
 */
void mouse_get_delta(int32_t* dx, int32_t* dy);

/**
 * @brief Convert MouseButton to human-readable name.
 *
 * @param[in] button  MouseButton to convert.
 *
 * @return Static string with button name (e.g., "LEFT", "RIGHT"), or "UNKNOWN".
 */
const char* mouse_get_button_name(MouseButton button);

/** @} */ /* end S.4b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c OPERATIONS — Input Operations [OPERATIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_operations S.4c OPERATIONS — Input Operations
 * @ingroup mouse_prototypes
 * @brief    Mouse event polling and cursor control.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Poll for mouse events (non-blocking).
 *
 * Checks for pending mouse events without blocking.
 * Call in main loop for responsive input handling.
 *
 * @param[out] state  Pointer to MouseState to fill. Must not be NULL.
 *
 * @return 1 if event available (state filled), 0 if no event pending.
 */
int mouse_poll(MouseState* state);

/**
 * @brief Wait for a mouse event (blocking).
 *
 * Blocks until a mouse event occurs. Use mouse_poll() for non-blocking.
 *
 * @param[out] state  Pointer to MouseState to fill. Must not be NULL.
 *
 * @return MOUSE_OK on success, error code on failure.
 */
MouseError mouse_wait_event(MouseState* state);

/**
 * @brief Flush all pending mouse events.
 *
 * Clears the mouse event queue. Use when transitioning
 * between input contexts to avoid stale events.
 */
void mouse_flush(void);

/**
 * @brief Warp (teleport) the mouse cursor to a position.
 *
 * Moves the cursor without generating movement events.
 * Useful for cursor confinement or centering.
 *
 * @param[in] x  Target X position (pixels from left).
 * @param[in] y  Target Y position (pixels from top).
 *
 * @return MOUSE_OK on success, error code on failure.
 */
MouseError mouse_warp(int32_t x, int32_t y);

/**
 * @brief Show or hide the mouse cursor.
 *
 * @param[in] visible  1 to show cursor, 0 to hide.
 */
void mouse_set_visible(int visible);

/**
 * @brief Check if mouse cursor is currently visible.
 *
 * @return 1 if visible, 0 if hidden.
 */
int mouse_is_visible(void);

/** @} */ /* end S.4c OPERATIONS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4d INJECTION — GUI Backend Support [INJECTION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_injection S.4d INJECTION — GUI Backend Support
 * @ingroup mouse_prototypes
 * @brief    Functions for GUI backends (SDL2, GLFW) to inject mouse events.
 *
 * These functions allow display backends to feed mouse events into the
 * kernel input layer. Call from SDL_MOUSEMOTION, SDL_MOUSEBUTTONDOWN,
 * or GLFW mouse callbacks.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Inject mouse cursor position.
 *
 * Called by GUI backends when receiving mouse motion events.
 *
 * @param[in] x  Absolute X position (pixels from left).
 * @param[in] y  Absolute Y position (pixels from top).
 *
 * EXAMPLE (SDL2):
 * @code{.c}
 * case SDL_MOUSEMOTION:
 *     mouse_inject_position(event.motion.x, event.motion.y);
 *     break;
 * @endcode
 */
void mouse_inject_position(int32_t x, int32_t y);

/**
 * @brief Inject mouse button state.
 *
 * Called by GUI backends for button press/release events.
 *
 * @param[in] button   MouseButton being pressed or released.
 * @param[in] pressed  1 = button down, 0 = button up.
 *
 * EXAMPLE (SDL2):
 * @code{.c}
 * case SDL_MOUSEBUTTONDOWN:
 *     mouse_inject_button(sdl_to_mousebutton(event.button.button), 1);
 *     break;
 * @endcode
 */
void mouse_inject_button(MouseButton button, int pressed);

/**
 * @brief Inject mouse scroll event.
 *
 * Called by GUI backends for scroll wheel events.
 *
 * @param[in] dx  Horizontal scroll delta (tilt wheel).
 * @param[in] dy  Vertical scroll delta (standard wheel).
 *
 * EXAMPLE (SDL2):
 * @code{.c}
 * case SDL_MOUSEWHEEL:
 *     mouse_inject_scroll(event.wheel.x, event.wheel.y);
 *     break;
 * @endcode
 */
void mouse_inject_scroll(int16_t dx, int16_t dy);

/**
 * @brief Inject a complete MouseState event.
 *
 * Full control injection for complex scenarios. Queues the event
 * to be returned by the next mouse_poll() call.
 *
 * @param[in] state  Complete MouseState to inject. Must not be NULL.
 */
void mouse_inject_event(const MouseState* state);

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
 * EXAMPLE (hal.c mouse_handler):
 * @code{.c}
 * mouse_inject_ps2(g_hal.mouse_x, g_hal.mouse_y,
 *                  mouse_event.dx, mouse_event.dy,
 *                  mouse_event.buttons);
 * @endcode
 */
void mouse_inject_ps2(int32_t x, int32_t y, int32_t dx, int32_t dy, uint8_t buttons);

/** @} */ /* end S.4d INJECTION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4e TOUCH — Multi-touch API [TOUCH_API]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_touch_api S.4e TOUCH — Multi-touch API
 * @ingroup mouse_prototypes
 * @brief    Functions for multi-touch input handling.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Get current number of active touch points.
 *
 * @return Number of fingers currently touching (0 if touch not supported).
 */
int mouse_get_touch_count(void);

/**
 * @brief Get touch point by index.
 *
 * @param[in]  index  Touch point index (0 to touch_count-1).
 * @param[out] point  Pointer to TouchPoint to fill. Must not be NULL.
 *
 * @return 1 if touch point exists and was filled, 0 otherwise.
 */
int mouse_get_touch(int index, TouchPoint* point);

/**
 * @brief Get touch point by ID.
 *
 * @param[in]  id     Touch point ID (from TouchPoint.id).
 * @param[out] point  Pointer to TouchPoint to fill. Must not be NULL.
 *
 * @return 1 if touch point exists and was filled, 0 otherwise.
 */
int mouse_get_touch_by_id(int32_t id, TouchPoint* point);

/**
 * @brief Inject a touch event.
 *
 * Called by GUI backends with touch screen support.
 *
 * @param[in] point  Touch point to inject. Must not be NULL.
 */
void mouse_inject_touch(const TouchPoint* point);

/** @} */ /* end S.4e TOUCH */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4f GESTURE — Gesture Recognition API [GESTURE_API]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_gesture_api S.4f GESTURE — Gesture Recognition API
 * @ingroup mouse_prototypes
 * @brief    Functions for gesture recognition.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Poll for gesture events.
 *
 * Checks if a gesture has been recognized since last poll.
 *
 * @param[out] gesture  Pointer to GestureState to fill. Must not be NULL.
 *
 * @return 1 if gesture detected, 0 if no gesture.
 */
int mouse_poll_gesture(GestureState* gesture);

/**
 * @brief Enable or disable gesture recognition.
 *
 * Gesture recognition has CPU overhead; disable if not needed.
 *
 * @param[in] enabled  1 to enable, 0 to disable.
 */
void mouse_set_gesture_enabled(int enabled);

/**
 * @brief Check if gesture recognition is enabled.
 *
 * @return 1 if enabled, 0 if disabled.
 */
int mouse_is_gesture_enabled(void);

/**
 * @brief Inject a gesture event.
 *
 * For systems that provide gesture recognition at the OS level.
 *
 * @param[in] gesture  Gesture to inject. Must not be NULL.
 */
void mouse_inject_gesture(const GestureState* gesture);

/** @} */ /* end S.4f GESTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4g XTERM — Terminal Mouse Protocol API [XTERM_API]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mouse_xterm_api S.4g XTERM — Terminal Mouse Protocol API
 * @ingroup mouse_prototypes
 * @brief    Functions for xterm mouse tracking in TUI mode.
 *
 * Xterm mouse protocols enable mouse support in terminal emulators like
 * WezTerm, iTerm2, Alacritty, kitty, and gnome-terminal. These functions
 * handle enabling/disabling tracking and parsing escape sequences.
 *
 * USAGE (TUI application):
 * @code{.c}
 * // Enable mouse tracking in terminal
 * XtermMouseConfig cfg = {
 *     .mode = XTERM_MOUSE_ANY,
 *     .encoding = XTERM_ENC_SGR,
 *     .focus = true
 * };
 * mouse_xterm_configure(&cfg);
 * mouse_xterm_enable();
 *
 * // In input loop, feed bytes from terminal
 * while (read(STDIN_FILENO, &byte, 1) == 1) {
 *     if (mouse_xterm_parse_byte(byte)) {
 *         MouseState state;
 *         if (mouse_poll(&state)) {
 *             handle_mouse_event(&state);
 *         }
 *     }
 * }
 *
 * // On exit
 * mouse_xterm_disable();
 * @endcode
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Enable xterm mouse tracking.
 *
 * Writes the appropriate escape sequences to stdout to enable mouse
 * tracking based on the current configuration. Call after mouse_init()
 * and mouse_xterm_configure().
 *
 * ESCAPE SEQUENCES WRITTEN (example for SGR + ANY mode):
 *   - CSI ? 1003 h  (enable any-event tracking)
 *   - CSI ? 1006 h  (enable SGR extended mode)
 *   - CSI ? 1004 h  (enable focus reporting, if configured)
 *
 * @note Safe to call multiple times (idempotent).
 * @note Always call mouse_xterm_disable() before program exit.
 */
void mouse_xterm_enable(void);

/**
 * @brief Disable xterm mouse tracking.
 *
 * Writes escape sequences to stdout to disable mouse tracking.
 * Should be called before program exit to restore terminal state.
 *
 * ESCAPE SEQUENCES WRITTEN:
 *   - CSI ? 1003 l  (disable any-event tracking)
 *   - CSI ? 1006 l  (disable SGR mode)
 *   - CSI ? 1004 l  (disable focus reporting)
 *
 * @note Safe to call multiple times (idempotent).
 */
void mouse_xterm_disable(void);

/**
 * @brief Configure xterm mouse tracking parameters.
 *
 * Sets the mode and encoding for mouse tracking. Call before
 * mouse_xterm_enable() to configure, or call while enabled to
 * reconfigure (will automatically update terminal).
 *
 * @param[in] config  Configuration to apply. Must not be NULL.
 *
 * RECOMMENDED CONFIGURATION:
 *   For modern terminals (WezTerm, kitty, etc.):
 *     - mode = XTERM_MOUSE_ANY (all events)
 *     - encoding = XTERM_ENC_SGR (unlimited coordinates)
 *
 *   For legacy terminals:
 *     - mode = XTERM_MOUSE_NORMAL (press/release only)
 *     - encoding = XTERM_ENC_DEFAULT (X10 compatible)
 */
void mouse_xterm_configure(const XtermMouseConfig* config);

/**
 * @brief Get current xterm mouse configuration.
 *
 * @param[out] config  Pointer to fill with current config. Must not be NULL.
 */
void mouse_xterm_get_config(XtermMouseConfig* config);

/**
 * @brief Check if xterm mouse tracking is currently enabled.
 *
 * @return 1 if enabled, 0 if disabled.
 */
int mouse_xterm_is_enabled(void);

/**
 * @brief Parse a single byte of xterm mouse escape sequence.
 *
 * Feed bytes from stdin to this function to parse mouse events.
 * When a complete sequence is parsed, returns 1 and the event
 * is available via mouse_poll().
 *
 * @param[in] byte  Single byte from terminal input.
 *
 * @return 1 if a complete mouse event was parsed (call mouse_poll),
 *         0 if more bytes needed or byte was not part of mouse sequence.
 *
 * STATE MACHINE:
 *   Tracks partial sequences internally. Reset on timeout or error.
 *
 * EXAMPLE SEQUENCES:
 *   X10/Normal: ESC [ M Cb Cx Cy           (6 bytes)
 *   SGR Press:  ESC [ < Cb ; Cx ; Cy M     (variable)
 *   SGR Release: ESC [ < Cb ; Cx ; Cy m    (variable)
 */
int mouse_xterm_parse_byte(uint8_t byte);

/**
 * @brief Parse a complete escape sequence buffer.
 *
 * Alternative to byte-by-byte parsing when you have the full sequence.
 *
 * @param[in]  buf     Buffer containing escape sequence. Must not be NULL.
 * @param[in]  len     Length of buffer in bytes.
 * @param[out] state   Pointer to fill with parsed MouseState. Must not be NULL.
 *
 * @return Number of bytes consumed (> 0 if event parsed),
 *         0 if incomplete sequence,
 *         -1 if not a mouse sequence.
 *
 * @note Does not modify internal state; use for external parsing.
 */
int mouse_xterm_parse_sequence(const uint8_t* buf, size_t len, MouseState* state);

/**
 * @brief Reset the xterm parser state machine.
 *
 * Call when switching input modes or on timeout to clear partial sequences.
 */
void mouse_xterm_reset_parser(void);

/**
 * @brief Write raw escape sequence to enable specific mode.
 *
 * Low-level function for custom terminal control. Normally use
 * mouse_xterm_enable() instead.
 *
 * @param[in] mode_code  DEC private mode code (e.g., 1000, 1003, 1006).
 * @param[in] enable     1 to enable (h), 0 to disable (l).
 *
 * @return Number of bytes written, or -1 on error.
 */
int mouse_xterm_write_mode(int mode_code, int enable);

/** @} */ /* end S.4g XTERM */

/** @} */ /* end S.4 PROTOTYPES */

/** S.5 FILE-LEVEL — [Reserved — no global state needed] @defgroup mouse_file @ingroup mouse @{ @} */
/** S.6 OMISSION — [Reserved] @defgroup mouse_omission @ingroup mouse @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Header file — declarations only, implementations in mouse.c
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * B.1 ORG CHART — Structure Overview
 * Types:     3 enums (MouseButton, MouseEventType, MouseError), 1 struct (MouseState)
 * Functions: 2 lifecycle, 5 accessors, 6 operations
 * Impl:      mouse.c
 * @defgroup mouse_orgchart @ingroup mouse @{ @} */

/** B.2 HELPERS — [Reserved — helpers in mouse.c] @defgroup mouse_helpers @ingroup mouse @{ @} */

/** B.3 CORE OPS — [Reserved — operations in mouse.c] @defgroup mouse_core_ops @ingroup mouse @{ @} */

/** B.4 ERRORS — [Reserved — error handling in keybind.h action layer] @defgroup mouse_errors_handling @ingroup mouse @{ @} */

/** B.5 PUBLIC API — [Declared in S.4 PROTOTYPES] @defgroup mouse_public_api @ingroup mouse @{ @} */
/** B.6 OMISSION — [Reserved] @defgroup mouse_body_omission @ingroup mouse @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Operational guidance for mouse input subsystem.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/** X.1 VALIDATION — [See mouse.c tests] @defgroup mouse_validation @ingroup mouse @{ @} */
/** X.2 EXECUTION — [See X.7 REFERENCE] @defgroup mouse_execution @ingroup mouse @{ @} */
/** X.3 CLEANUP — mouse_init/shutdown manage lifecycle @defgroup mouse_cleanup @ingroup mouse @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidelines [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_policy X.4 POLICY — Modification Guidelines
 * @ingroup mouse
 * @brief    Guide future maintainers — what's safe to change.
 *
 * SAFE TO MODIFY:
 *   - ✅ Add new MouseButton values (before MOUSE_BUTTON_COUNT)
 *   - ✅ Add new MouseEventType values
 *   - ✅ Add new accessor functions (mouse_get_*)
 *   - ✅ Add new fields to MouseState (maintain ABI compatibility)
 *
 * MODIFY WITH CARE:
 *   - ⚠️ MouseButton enum order — may break serialization/config
 *   - ⚠️ MouseState struct layout — affects binary compatibility
 *   - ⚠️ Function signatures — breaks all calling code
 *
 * NEVER MODIFY:
 *   - ❌ MOUSE_BUTTON_NONE = 0 (sentinel behavior)
 *   - ❌ MOUSE_EVENT_NONE = 0 (event identity)
 *   - ❌ mouse_init/shutdown contract
 *   - ❌ 4-block structure
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — Planned Upgrades [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_extension X.5 EXTENSION — Planned Upgrades
 * @ingroup mouse
 * @brief    Roadmap for mouse subsystem expansion.
 *
 * PHASE 1 (Current):
 *   - MouseButton enum (left, right, middle, extended)
 *   - Position tracking (x, y, delta)
 *   - Scroll events (vertical, horizontal)
 *   - Basic polling/waiting API
 *
 * PHASE 2 (Planned):
 *   - Multi-touch support
 *   - Gesture recognition hooks
 *   - Cursor confinement regions
 *   - Raw input mode
 *
 * PHASE 3 (Future):
 *   - Hot-plugging detection
 *   - Multiple mouse support
 *   - Virtual mouse injection
 *   - Tablet/pen pressure support
 *
 * INTEGRATION POINTS:
 *   - keybind.h: Action abstraction layer (consumes MouseState)
 *   - keyboard.h: Sibling input peripheral
 *   - shell: Uses keybind actions, not raw mouse
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @ingroup mouse
 * @brief    Common mouse issues and solutions.
 *
 * "mouse_poll returns 0 but mouse is moving":
 *   → Ensure mouse_init() was called first
 *   → Verify window/display has focus
 *   → Check platform event loop is running
 *
 * "Position is always (0, 0)":
 *   → Position only valid for GUI mode
 *   → Terminal mode may not provide absolute position
 *
 * "Delta values are erratic":
 *   → Call mouse_get_delta() consistently each frame
 *   → Delta accumulates until read
 *
 * "Scroll events not detected":
 *   → Some platforms require focus for scroll
 *   → Check scroll_y for vertical, scroll_x for horizontal
 *
 * "Button events missing":
 *   → Some mice don't have all buttons
 *   → Extended buttons (X1-X5) are optional hardware
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_reference X.7 REFERENCE — Quick Lookup
 * @ingroup mouse
 * @brief    Copy-paste ready mouse examples.
 *
 * BASIC SETUP:
 * @code{.c}
 * #include "input/mouse.h"
 *
 * mouse_init();
 * // ... use mouse ...
 * mouse_shutdown();
 * @endcode
 *
 * POLLING LOOP:
 * @code{.c}
 * MouseState mouse;
 * while (running) {
 *     if (mouse_poll(&mouse)) {
 *         if (mouse.type == MOUSE_EVENT_BUTTON && mouse.pressed) {
 *             handle_click(mouse.button, mouse.x, mouse.y);
 *         }
 *     }
 *     // ... other game logic ...
 * }
 * @endcode
 *
 * POSITION TRACKING:
 * @code{.c}
 * int32_t x, y;
 * mouse_get_position(&x, &y);
 * cursor_draw(x, y);
 * @endcode
 *
 * MOUSE LOOK (FPS camera):
 * @code{.c}
 * int32_t dx, dy;
 * mouse_get_delta(&dx, &dy);
 * camera_rotate(dx * sensitivity, dy * sensitivity);
 * @endcode
 *
 * DRAG OPERATION:
 * @code{.c}
 * if (mouse_is_pressed(MOUSE_BUTTON_LEFT)) {
 *     int32_t dx, dy;
 *     mouse_get_delta(&dx, &dy);
 *     drag_object(selected, dx, dy);
 * }
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Final Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mouse_note X.8 NOTE — Final Guidance
 * @ingroup mouse
 * @brief    Kernel input layer — foundation for all mouse interaction.
 *
 * This header defines the RAW mouse layer. It provides:
 *   - MouseButton: WHICH button (identity)
 *   - MouseEventType: WHAT happened (move, click, scroll)
 *   - MouseState: Full event (position + buttons + scroll)
 *
 * For ACTION-based input (shortcuts, navigation, commands), use keybind.h.
 * Code should ask "is ACTION_CONFIRM pressed?" not "is LEFT_CLICK pressed?"
 * — keybind.h provides this abstraction.
 *
 * ARCHITECTURE:
 *   mouse.h (raw input) → keybind.h (actions) → consumers
 *
 * @par Biblical Grounding
 * "Let thine eyes look right on, and let thine eyelids look straight before thee.
 *  Ponder the path of thy feet, and let all thy ways be established."
 * — Proverbs 4:25-26
 *
 * Where user points reveals intention. Track position with purpose.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * C++ Linkage Closure (Foundation file)
 * ───────────────────────────────────────────────────────────────────────────── */
#ifdef __cplusplus
}
#endif

#endif /* MOS_HAL_INPUT_MOUSE_H */


