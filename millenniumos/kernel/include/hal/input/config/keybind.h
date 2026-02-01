/// @file keybind.h
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
/// @omni:req  key        = MOS-HAL-INPUT-KEYBIND
/// @omni:req  from       = bereshit/word/seed/code/c/header.h
/// @omni:req  at         = a-01.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = abstraction
/// @omni:inh  role       = input
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = kernel/hal/input/config
/// @omni:ins  layer      = kernel (layer 0)
/// @omni:ins  includes   = stdint, stdbool, keyboard.h, mouse.h
/// @omni:ins  provides   = [Action, InputBinding, keybind_*]
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
/// @omni:req  title      = Input Action Binding Interface
/// @omni:req  brief      = Action abstraction layer — maps actions to keys/buttons
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
/// WHY SPECIAL EXISTS:
///   Feature macros and compile-time settings must be defined BEFORE #include.
///   This is not PRAGMA (metadata), not METADATA (docs), not SETUP (includes).
///
/// CONTAINS:
///   - SP.1 CONFIG  — Compile-time settings (POSIX, domain-specific macros)
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * SP.1 CONFIG — Compile-time Settings [CONFIG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_special_config SP.1 CONFIG — Compile-time Settings
 * @brief    Feature macros and environment configuration before includes.
 *
 * WHY: Settings that affect what headers provide must precede \#include.
 *
 * CONTAINS:
 *   - SP.1a POSIX  — Platform feature test (universal)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * SP.1a POSIX — Feature Test [POSIX]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keybind_special_posix SP.1a POSIX — Feature Test
 * @ingroup keybind_special_config
 * @brief    Enables POSIX.1-2008 functions in standard headers.
 *
 * WHY: Time functions, file operations, and POSIX APIs require this macro.
 *      Must be defined BEFORE any system includes or they won't see it.
 *
 * SCOPE: Universal — most C files need POSIX features.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* MillenniumOS: Freestanding kernel - no POSIX */
#ifndef CPISI_MODE
#define CPISI_MODE 0  /**< Kernel mode - freestanding */
#endif

/** @} */ /* end SP.1a MODE */

/** @} */ /* end SP.1 CONFIG */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * INCLUDE GUARD — Identity protection
 * ───────────────────────────────────────────────────────────────────────────── */

#ifndef MOS_HAL_INPUT_KEYBIND_H
#define MOS_HAL_INPUT_KEYBIND_H

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
 * @file      keybind.h
 * @brief     Action Binding — Maps semantic actions to keys/buttons.
 *
 * @defgroup keybind_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       MOS-HAL-INPUT-KEYBIND
 * Title:     Input Action Binding Interface
 * Type:      Header (Abstraction Layer)
 * Component: Kernel/HAL/Input — system-level action mapping
 * Role:      Action abstraction, input unification, rebindable controls
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_state M.2 STATE — Lifecycle State
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
 * @defgroup keybind_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup keybind_location M.4 LOCATION — File Path
 * @{
 *
 * Path: millenniumos/kernel/include/hal/input/config/keybind.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: bereshit/word/seed/code/c/header.h
 * Design:       2026-01-26 action abstraction layer
 *
 * Builds FROM:  keyboard.h (KeyCode, KeyState), mouse.h (MouseButton, MouseState)
 * Builds TO:    shell, window manager, editors, all input consumers
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: header, c, engine, core, input, keybind, actions, system, abstraction
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     System-level action abstraction layer
 * Core Design: Action enum + InputBinding mapping, unifies keyboard/mouse
 * Philosophy:  Code asks "is QUIT pressed?" not "is ESC pressed?"
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Isaiah 28:10 — "Precept upon precept, line upon line"
 * Principle:  Layered abstraction — raw input → named actions → behaviors
 * Anchor:     Genesis 2:19 — Adam named the animals; naming enables mastery
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Standard: stdint.h, stdbool.h
 *   Internal: keyboard.h (KeyCode, KeyState), mouse.h (MouseButton, MouseState)
 *
 * What Uses This:
 *   - shell — command input and navigation
 *   - window manager — shortcuts and focus control
 *   - text editors — editing operations (copy, paste, etc.)
 *   - any component that cares about "what user wants to do"
 *
 * @see keyboard.h for raw keyboard input
 * @see mouse.h for raw mouse input
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-26) — Initial creation as abstraction layer
 *
 * Current:
 *   ✓ Action enum (semantic actions)
 *   ✓ InputBinding struct (action → key/button mapping)
 *   ✓ State query API (keybind_is_action_pressed, etc.)
 *
 * Planned:
 *   ⏳ Runtime rebinding support
 *   ⏳ TOML config loading (keybindings.toml)
 *   ⏳ Gamepad/controller input sources
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
 * @defgroup keybind_includes S.1 INCLUDES — Header Dependencies
 * @brief    Header dependencies for action binding module.
 *
 * PURPOSE: Abstraction layer — includes raw input headers for unification.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — System headers (stdint, stdbool)
 *   - S.1b INTERNAL  — keyboard.h, mouse.h (raw input layers)
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
 * @defgroup keybind_standard S.1a STANDARD — System Headers
 * @ingroup keybind_includes
 * @brief    C standard library headers required by this module.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include <stdint.h>   /**< Fixed-width integers: uint8_t, uint32_t */
#include <stdbool.h>  /**< Boolean type: bool, true, false */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Input Layer Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keybind_internal S.1b INTERNAL — Input Layer Headers
 * @ingroup keybind_includes
 * @brief    Raw input layers that keybind abstracts over.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "hal/input/keyboard.h"  /**< KeyCode, KeyState, KeyModifier */
#include "hal/input/mouse.h"     /**< MouseButton, MouseState */

/** @} */ /* end S.1b INTERNAL */

/** S.1c EXTERNAL — [None] @defgroup keybind_external @ingroup keybind_includes @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keybind_cpp S.1d CPP — C++ Linkage
 * @ingroup keybind_includes
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
 * @defgroup keybind_defines S.2 DEFINES — Preprocessor Constants
 * @brief    Action binding constants and limits.
 *
 * Builds FROM: S.1 INCLUDES (stdint types, keyboard.h, mouse.h)
 * Builds TO:   S.3 TYPES (Action enum, InputBinding size validation)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/** S.2a SIZES — [Reserved] @defgroup keybind_sizes @ingroup keybind_defines @{ @} */

/**
 * @defgroup keybind_limits S.2b LIMITS — Action Binding Limits
 * @ingroup keybind_defines
 * @brief    Action binding constants.
 * @{ */
#define KEYBIND_MAX_BINDINGS            4      /**< Max inputs bound to one action      */
#define KEYBIND_ACTION_MAX              64     /**< Maximum number of actions           */
#define KEYBIND_ACTION_INVALID          0xFF   /**< Invalid action sentinel             */
/** @} */ /* end S.2b LIMITS */

/** S.2c FLAGS — [Reserved] @defgroup keybind_flags @ingroup keybind_defines @{ @} */
/** S.2d MACROS — [Reserved] @defgroup keybind_macros @ingroup keybind_defines @{ @} */
/** S.2e VERIFY — [Reserved] @defgroup keybind_verify @ingroup keybind_defines @{ @} */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_types S.3 TYPES — Type Declarations
 * @ingroup keybind
 * @brief    Action binding types — Action, InputSource, InputBinding.
 *
 * CONTAINS:
 *   S.3a ENUMS   — Action (semantic actions), InputSource (key vs button)
 *   S.3b STRUCTS — InputBinding (action → input mapping)
 *   S.3c OPAQUE  — [Reserved]
 *   S.3d ERRORS  — KeybindError (binding system errors)
 *
 * Builds FROM: S.2 DEFINES (action limits), S.1 INCLUDES (keyboard.h, mouse.h)
 * Builds TO:   S.4 PROTOTYPES (keybind API functions)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a ENUMS — Action and InputSource [ENUMS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keybind_enums S.3a ENUMS — Action Enumerations
 * @ingroup keybind_types
 * @brief    Semantic actions (Action) and input source types (InputSource).
 *
 * ARCHITECTURE:
 *   Action      — WHAT the user wants to do (semantic, configurable)
 *   InputSource — WHERE the input comes from (keyboard, mouse, gamepad)
 *   InputBinding — Maps Action → raw inputs (KeyCode, MouseButton)
 *
 * PATTERN:
 *   ACTION_NONE  = 0   — invalid/no action (always 0)
 *   ACTION_COUNT       — sentinel for array sizing
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Action represents semantic user intent for the OS.
 *
 * Organized by domain for clean scaling:
 *   1. Core (invalid/confirm/cancel)
 *   2. System (shell, reboot, screenshot)
 *   3. Editing (copy, paste, undo, redo)
 *   4. Navigation (arrows, page, document)
 *   5. Selection (select all, extend selection)
 *   6. Window Management (switch, close, minimize)
 *   7. Debug (console, DAR panel)
 *   8. Function Keys (F1-F12 as raw actions)
 *
 * @note ACTION_NONE is always 0 (no action / invalid).
 * @note ACTION_COUNT is sentinel for array sizing.
 * @note Game-specific actions belong in app-layer, not kernel.
 */
typedef enum Action {
    /* ═══════════════════════════════════════════════════════════════════════
     * 1. CORE — Universal actions (every app needs these)
     * ═══════════════════════════════════════════════════════════════════════ */
    ACTION_NONE = 0,         /**< No action / invalid (always 0)              */
    ACTION_CONFIRM,          /**< Confirm / OK / Submit (default: Enter)      */
    ACTION_CANCEL,           /**< Cancel / Dismiss / Back (default: Escape)   */
    ACTION_TAB_NEXT,         /**< Next field/element (default: Tab)           */
    ACTION_TAB_PREV,         /**< Previous field/element (default: Shift+Tab) */

    /* ═══════════════════════════════════════════════════════════════════════
     * 2. SYSTEM — OS-level operations
     * ═══════════════════════════════════════════════════════════════════════ */
    ACTION_SHELL,            /**< Open terminal/shell (default: F12)          */
    ACTION_REBOOT,           /**< Reboot system (default: Ctrl+Alt+Del)       */
    ACTION_SHUTDOWN,         /**< Shutdown system (default: Ctrl+Alt+End)     */
    ACTION_SCREENSHOT,       /**< Capture screen (default: PrintScreen)       */
    ACTION_HELP,             /**< Show help (default: F1)                     */
    ACTION_SETTINGS,         /**< Open settings (default: Ctrl+,)             */

    /* ═══════════════════════════════════════════════════════════════════════
     * 3. EDITING — Standard editing operations (Ctrl+key)
     * ═══════════════════════════════════════════════════════════════════════ */
    ACTION_COPY,             /**< Copy selection (default: Ctrl+C)            */
    ACTION_CUT,              /**< Cut selection (default: Ctrl+X)             */
    ACTION_PASTE,            /**< Paste clipboard (default: Ctrl+V)           */
    ACTION_UNDO,             /**< Undo last action (default: Ctrl+Z)          */
    ACTION_REDO,             /**< Redo last action (default: Ctrl+Y)          */
    ACTION_SELECT_ALL,       /**< Select all (default: Ctrl+A)                */
    ACTION_SAVE,             /**< Save current (default: Ctrl+S)              */
    ACTION_SAVE_AS,          /**< Save as (default: Ctrl+Shift+S)             */
    ACTION_OPEN,             /**< Open file (default: Ctrl+O)                 */
    ACTION_NEW,              /**< New file/document (default: Ctrl+N)         */
    ACTION_FIND,             /**< Find/search (default: Ctrl+F)               */
    ACTION_FIND_NEXT,        /**< Find next (default: F3 or Ctrl+G)           */
    ACTION_FIND_PREV,        /**< Find previous (default: Shift+F3)           */
    ACTION_REPLACE,          /**< Find and replace (default: Ctrl+H)          */
    ACTION_DELETE,           /**< Delete selection (default: Delete)          */
    ACTION_BACKSPACE,        /**< Backspace (default: Backspace)              */

    /* ═══════════════════════════════════════════════════════════════════════
     * 4. NAVIGATION — Cursor/view movement
     * ═══════════════════════════════════════════════════════════════════════ */
    /* Arrow navigation */
    ACTION_NAV_UP,           /**< Move up (default: Up arrow)                 */
    ACTION_NAV_DOWN,         /**< Move down (default: Down arrow)             */
    ACTION_NAV_LEFT,         /**< Move left (default: Left arrow)             */
    ACTION_NAV_RIGHT,        /**< Move right (default: Right arrow)           */

    /* Line navigation */
    ACTION_LINE_START,       /**< Go to line start (default: Home)            */
    ACTION_LINE_END,         /**< Go to line end (default: End)               */

    /* Word navigation */
    ACTION_WORD_LEFT,        /**< Move word left (default: Ctrl+Left)         */
    ACTION_WORD_RIGHT,       /**< Move word right (default: Ctrl+Right)       */

    /* Page/document navigation */
    ACTION_PAGE_UP,          /**< Page up (default: PageUp)                   */
    ACTION_PAGE_DOWN,        /**< Page down (default: PageDown)               */
    ACTION_DOC_START,        /**< Document start (default: Ctrl+Home)         */
    ACTION_DOC_END,          /**< Document end (default: Ctrl+End)            */

    /* ═══════════════════════════════════════════════════════════════════════
     * 5. SELECTION — Extend selection (Shift+navigation)
     * ═══════════════════════════════════════════════════════════════════════ */
    ACTION_SELECT_UP,        /**< Extend selection up (default: Shift+Up)     */
    ACTION_SELECT_DOWN,      /**< Extend selection down (default: Shift+Down) */
    ACTION_SELECT_LEFT,      /**< Extend selection left (default: Shift+Left) */
    ACTION_SELECT_RIGHT,     /**< Extend selection right (default: Shift+Right)*/
    ACTION_SELECT_WORD_LEFT, /**< Select word left (default: Ctrl+Shift+Left) */
    ACTION_SELECT_WORD_RIGHT,/**< Select word right (default: Ctrl+Shift+Right)*/
    ACTION_SELECT_LINE_START,/**< Select to line start (default: Shift+Home)  */
    ACTION_SELECT_LINE_END,  /**< Select to line end (default: Shift+End)     */
    ACTION_SELECT_PAGE_UP,   /**< Select page up (default: Shift+PageUp)      */
    ACTION_SELECT_PAGE_DOWN, /**< Select page down (default: Shift+PageDown)  */
    ACTION_SELECT_DOC_START, /**< Select to doc start (default: Ctrl+Shift+Home)*/
    ACTION_SELECT_DOC_END,   /**< Select to doc end (default: Ctrl+Shift+End) */

    /* ═══════════════════════════════════════════════════════════════════════
     * 6. WINDOW MANAGEMENT — Task/window operations
     * ═══════════════════════════════════════════════════════════════════════ */
    ACTION_WINDOW_CLOSE,     /**< Close window/app (default: Alt+F4)          */
    ACTION_WINDOW_SWITCH,    /**< Switch window (default: Alt+Tab)            */
    ACTION_WINDOW_SWITCH_REV,/**< Switch window reverse (default: Alt+Shift+Tab)*/
    ACTION_WINDOW_MINIMIZE,  /**< Minimize window (default: Super+Down)       */
    ACTION_WINDOW_MAXIMIZE,  /**< Maximize/restore (default: Super+Up)        */
    ACTION_WINDOW_FULLSCREEN,/**< Toggle fullscreen (default: F11)            */
    ACTION_WINDOW_SNAP_LEFT, /**< Snap window left (default: Super+Left)      */
    ACTION_WINDOW_SNAP_RIGHT,/**< Snap window right (default: Super+Right)    */

    /* ═══════════════════════════════════════════════════════════════════════
     * 7. DEBUG — Development/debug operations
     * ═══════════════════════════════════════════════════════════════════════ */
    ACTION_DEBUG_CONSOLE,    /**< Toggle debug console (default: `)           */
    ACTION_DEBUG_DAR,        /**< Toggle DAR panel (default: Ctrl+Shift+D)    */
    ACTION_DEBUG_BREAK,      /**< Break into debugger (default: Ctrl+Break)   */
    ACTION_DEBUG_STEP,       /**< Step debugger (default: F10)                */
    ACTION_DEBUG_STEP_INTO,  /**< Step into (default: F11)                    */
    ACTION_DEBUG_CONTINUE,   /**< Continue execution (default: F5)            */

    /* ═══════════════════════════════════════════════════════════════════════
     * 8. FUNCTION KEYS — Raw F-key actions (for app-specific binding)
     * ═══════════════════════════════════════════════════════════════════════ */
    ACTION_F1,               /**< F1 key action                               */
    ACTION_F2,               /**< F2 key action                               */
    ACTION_F3,               /**< F3 key action                               */
    ACTION_F4,               /**< F4 key action                               */
    ACTION_F5,               /**< F5 key action                               */
    ACTION_F6,               /**< F6 key action                               */
    ACTION_F7,               /**< F7 key action                               */
    ACTION_F8,               /**< F8 key action                               */
    ACTION_F9,               /**< F9 key action                               */
    ACTION_F10,              /**< F10 key action                              */
    ACTION_F11,              /**< F11 key action                              */
    ACTION_F12,              /**< F12 key action                              */

    /* ═══════════════════════════════════════════════════════════════════════
     * SENTINEL
     * ═══════════════════════════════════════════════════════════════════════ */
    ACTION_COUNT             /**< Sentinel for array sizing (do not use)      */
} Action;

/**
 * @brief InputSource identifies where an input comes from.
 *
 * Used in InputBinding to specify whether a binding is keyboard,
 * mouse, or gamepad input.
 */
typedef enum InputSource {
    INPUT_SOURCE_NONE = 0,   /**< No source / invalid                         */
    INPUT_SOURCE_KEYBOARD,   /**< Keyboard key (KeyCode)                       */
    INPUT_SOURCE_MOUSE,      /**< Mouse button (MouseButton)                   */
    INPUT_SOURCE_GAMEPAD     /**< Gamepad button (future)                      */
} InputSource;

/** @} */ /* end S.3a ENUMS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b STRUCTS — InputBinding Structure [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keybind_structs S.3b STRUCTS — Binding Structures
 * @ingroup keybind_types
 * @brief    Action to input mapping structures.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief SingleBinding represents one input bound to an action.
 *
 * A single key or button that triggers an action. Multiple SingleBindings
 * can map to the same action (e.g., both W and UP for ACTION_MOVE_UP).
 */
typedef struct SingleBinding {
    InputSource  source;     /**< Where input comes from (keyboard/mouse)        */
    union {
        KeyCode      key;    /**< Keyboard key (if source == INPUT_SOURCE_KEYBOARD) */
        MouseButton  button; /**< Mouse button (if source == INPUT_SOURCE_MOUSE)    */
        uint8_t      code;   /**< Generic code for future sources                   */
    } input;
    uint8_t      modifiers;  /**< Required modifiers (KeyModifier flags)          */
} SingleBinding;

/**
 * @brief InputBinding represents all inputs bound to one action.
 *
 * Actions can have multiple bindings (e.g., WASD and arrow keys).
 * Up to KEYBIND_MAX_BINDINGS inputs per action.
 */
typedef struct InputBinding {
    Action         action;                        /**< The semantic action        */
    SingleBinding  bindings[KEYBIND_MAX_BINDINGS]; /**< All inputs for this action */
    uint8_t        count;                         /**< Number of active bindings   */
} InputBinding;

/** @} */ /* end S.3b STRUCTS */

/** S.3c OPAQUE — [Reserved] @defgroup keybind_opaque @ingroup keybind_types @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3d ERRORS — Error Codes [ERRORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keybind_errors S.3d ERRORS — Keybind Error Codes
 * @ingroup keybind_types
 * @brief    Keybind subsystem error codes.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief KeybindError represents keybind subsystem errors.
 */
typedef enum KeybindError {
    KEYBIND_OK = 0,          /**< Success / no error                           */
    KEYBIND_ERR_INIT,        /**< Initialization failed                        */
    KEYBIND_ERR_NOT_INIT,    /**< Keybind system not initialized               */
    KEYBIND_ERR_INVALID,     /**< Invalid parameter (action, key, etc.)        */
    KEYBIND_ERR_FULL,        /**< Binding slots full for action                */
    KEYBIND_ERR_NOT_FOUND    /**< Action or binding not found                  */
} KeybindError;

/** @} */ /* end S.3d ERRORS */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_prototypes S.4 PROTOTYPES — Function Declarations
 * @ingroup keybind
 * @brief    Action binding API function declarations.
 *
 * STRUCTURE:
 *   S.4a LIFECYCLE  — keybind_init, keybind_shutdown
 *   S.4b ACCESSORS  — keybind_is_action_pressed, keybind_get_*
 *   S.4c OPERATIONS — keybind_bind, keybind_unbind, keybind_update
 *
 * Builds FROM: S.3 TYPES (Action, InputBinding, KeybindError)
 * Builds TO:   BODY (implementations in keybind.c)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a LIFECYCLE — Init/Shutdown [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keybind_lifecycle S.4a LIFECYCLE — Init/Shutdown
 * @ingroup keybind_prototypes
 * @brief    Keybind subsystem lifecycle management.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize the keybind subsystem.
 *
 * Must be called before any other keybind functions.
 * Automatically initializes keyboard and mouse subsystems.
 * Loads default bindings for all actions.
 * Safe to call multiple times (idempotent).
 *
 * @return KEYBIND_OK on success, error code on failure.
 */
KeybindError keybind_init(void);

/**
 * @brief Shutdown the keybind subsystem.
 *
 * Releases all resources. Does NOT shutdown keyboard/mouse (caller manages).
 * Safe to call if not initialized (no-op).
 * After shutdown, keybind_init() must be called again.
 */
void keybind_shutdown(void);

/**
 * @brief Load default keybindings for all actions.
 *
 * Resets all bindings to defaults (standard OS shortcuts and navigation).
 * Call this to restore defaults or initialize fresh bindings.
 */
void keybind_load_defaults(void);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b ACCESSORS — State Queries [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keybind_accessors S.4b ACCESSORS — State Queries
 * @ingroup keybind_prototypes
 * @brief    Query action states and binding information.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Check if an action is currently triggered.
 *
 * Checks all inputs bound to the action.
 * Handles keyboard keys and mouse buttons transparently.
 *
 * @param[in] action  Action to check.
 *
 * @return 1 if any bound input is active, 0 otherwise.
 */
int keybind_is_action_pressed(Action action);

/**
 * @brief Check if an action was just pressed this frame.
 *
 * Returns true only on the frame when input transitions from up to down.
 * Use for single-trigger actions (menu selection, jumping).
 *
 * @param[in] action  Action to check.
 *
 * @return 1 if action was just pressed, 0 otherwise.
 */
int keybind_is_action_just_pressed(Action action);

/**
 * @brief Check if an action was just released this frame.
 *
 * Returns true only on the frame when input transitions from down to up.
 * Use for release-triggered actions (button up events).
 *
 * @param[in] action  Action to check.
 *
 * @return 1 if action was just released, 0 otherwise.
 */
int keybind_is_action_just_released(Action action);

/**
 * @brief Get the binding information for an action.
 *
 * Returns pointer to the InputBinding struct for an action.
 * Do not modify the returned binding directly; use keybind_bind/unbind.
 *
 * @param[in] action  Action to query.
 *
 * @return Pointer to InputBinding, or NULL if action is invalid.
 */
const InputBinding* keybind_get_binding(Action action);

/**
 * @brief Convert Action to human-readable name.
 *
 * @param[in] action  Action to convert.
 *
 * @return Static string with action name (e.g., "ACTION_QUIT"), or "ACTION_UNKNOWN".
 */
const char* keybind_get_action_name(Action action);

/**
 * @brief Get display string for an action's bindings.
 *
 * Returns a string like "ESC / Q" showing all bound inputs.
 * Useful for displaying controls in UI.
 *
 * @param[in]  action  Action to query.
 * @param[out] buffer  Buffer to write string to.
 * @param[in]  size    Size of buffer.
 *
 * @return Number of characters written (excluding null terminator).
 */
int keybind_get_binding_string(Action action, char* buffer, int size);

/** @} */ /* end S.4b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c OPERATIONS — Binding Operations [OPERATIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keybind_operations S.4c OPERATIONS — Binding Operations
 * @ingroup keybind_prototypes
 * @brief    Bind, unbind, and update action mappings.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Bind a keyboard key to an action.
 *
 * Adds a new binding for the action. Each action can have up to
 * KEYBIND_MAX_BINDINGS inputs. Does not remove existing bindings.
 *
 * @param[in] action     Action to bind to.
 * @param[in] key        KeyCode to bind.
 * @param[in] modifiers  Required modifiers (0 for none).
 *
 * @return KEYBIND_OK on success, error code on failure.
 */
KeybindError keybind_bind_key(Action action, KeyCode key, uint8_t modifiers);

/**
 * @brief Bind a mouse button to an action.
 *
 * Adds a new binding for the action. Each action can have up to
 * KEYBIND_MAX_BINDINGS inputs. Does not remove existing bindings.
 *
 * @param[in] action     Action to bind to.
 * @param[in] button     MouseButton to bind.
 * @param[in] modifiers  Required modifiers (0 for none).
 *
 * @return KEYBIND_OK on success, error code on failure.
 */
KeybindError keybind_bind_button(Action action, MouseButton button, uint8_t modifiers);

/**
 * @brief Remove all bindings for an action.
 *
 * Clears all keyboard and mouse bindings for the specified action.
 *
 * @param[in] action  Action to unbind.
 *
 * @return KEYBIND_OK on success, error code on failure.
 */
KeybindError keybind_unbind_action(Action action);

/**
 * @brief Remove a specific key binding.
 *
 * Removes only the specified key from the action's bindings.
 * Other bindings for the same action remain.
 *
 * @param[in] action  Action to modify.
 * @param[in] key     KeyCode to unbind.
 *
 * @return KEYBIND_OK on success, KEYBIND_ERR_NOT_FOUND if not bound.
 */
KeybindError keybind_unbind_key(Action action, KeyCode key);

/**
 * @brief Remove a specific mouse button binding.
 *
 * Removes only the specified button from the action's bindings.
 * Other bindings for the same action remain.
 *
 * @param[in] action  Action to modify.
 * @param[in] button  MouseButton to unbind.
 *
 * @return KEYBIND_OK on success, KEYBIND_ERR_NOT_FOUND if not bound.
 */
KeybindError keybind_unbind_button(Action action, MouseButton button);

/**
 * @brief Update keybind state from input systems.
 *
 * Call once per frame to update action states based on current
 * keyboard and mouse input. Must be called before checking actions.
 */
void keybind_update(void);

/** @} */ /* end S.4c OPERATIONS */

/** @} */ /* end S.4 PROTOTYPES */

/** S.5 FILE-LEVEL — [Reserved — no global state needed] @defgroup keybind_file @ingroup keybind @{ @} */
/** S.6 OMISSION — [Reserved] @defgroup keybind_omission @ingroup keybind @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Header file — declarations only, implementations in keybind.c
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * B.1 ORG CHART — Structure Overview
 * Types:     3 enums (Action, InputSource, KeybindError), 2 structs (SingleBinding, InputBinding)
 * Functions: 3 lifecycle, 6 accessors, 6 operations
 * Impl:      keybind.c
 * @defgroup keybind_orgchart @ingroup keybind @{ @} */

/** B.2 HELPERS — [Reserved — helpers in keybind.c] @defgroup keybind_helpers @ingroup keybind @{ @} */

/** B.3 CORE OPS — [Reserved — operations in keybind.c] @defgroup keybind_core_ops @ingroup keybind @{ @} */

/** B.4 ERRORS — [Error handling through KeybindError return codes] @defgroup keybind_errors_handling @ingroup keybind @{ @} */

/** B.5 PUBLIC API — [Declared in S.4 PROTOTYPES] @defgroup keybind_public_api @ingroup keybind @{ @} */
/** B.6 OMISSION — [Reserved] @defgroup keybind_body_omission @ingroup keybind @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Operational guidance for action binding subsystem.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/** X.1 VALIDATION — [See keybind.c tests] @defgroup keybind_validation @ingroup keybind @{ @} */
/** X.2 EXECUTION — [See X.7 REFERENCE] @defgroup keybind_execution @ingroup keybind @{ @} */
/** X.3 CLEANUP — keybind_init/shutdown manage lifecycle @defgroup keybind_cleanup @ingroup keybind @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidelines [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_policy X.4 POLICY — Modification Guidelines
 * @ingroup keybind
 * @brief    Guide future maintainers — what's safe to change.
 *
 * SAFE TO MODIFY:
 *   - ✅ Add new Action values (before ACTION_COUNT)
 *   - ✅ Add new InputSource types (for gamepad, etc.)
 *   - ✅ Add new accessor functions (keybind_get_*)
 *   - ✅ Adjust default bindings in keybind_load_defaults()
 *
 * MODIFY WITH CARE:
 *   - ⚠️ Action enum order — may break config files
 *   - ⚠️ InputBinding struct layout — affects binary compatibility
 *   - ⚠️ Function signatures — breaks all calling code
 *
 * NEVER MODIFY:
 *   - ❌ ACTION_NONE = 0 (sentinel behavior)
 *   - ❌ INPUT_SOURCE_NONE = 0 (source identity)
 *   - ❌ keybind_init/shutdown contract
 *   - ❌ 4-block structure
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — Planned Upgrades [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_extension X.5 EXTENSION — Planned Upgrades
 * @ingroup keybind
 * @brief    Roadmap for keybind subsystem expansion.
 *
 * PHASE 1 (Current):
 *   - Action enum with OS actions (editing, navigation, system)
 *   - InputBinding for keyboard and mouse
 *   - Default bindings (standard OS shortcuts)
 *   - Action state query API
 *
 * PHASE 2 (Planned):
 *   - TOML config loading (keybindings.toml)
 *   - Runtime rebinding UI support
 *   - Conflict detection (same key bound twice)
 *   - Binding contexts (shell, editor, window manager)
 *
 * PHASE 3 (Future):
 *   - Gamepad/controller support (INPUT_SOURCE_GAMEPAD)
 *   - Analog axis actions (for scrolling, etc.)
 *   - Chords (Ctrl+Shift+S as single binding)
 *   - Context-sensitive bindings
 *
 * INTEGRATION POINTS:
 *   - keyboard.h: Raw key events (consumed by keybind)
 *   - mouse.h: Raw mouse events (consumed by keybind)
 *   - shell: Uses actions for commands and navigation
 *   - editors: Uses actions for text operations
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @ingroup keybind
 * @brief    Common keybind issues and solutions.
 *
 * "keybind_is_action_pressed always returns 0":
 *   → Ensure keybind_init() was called first
 *   → Call keybind_update() once per frame before checking actions
 *   → Verify keyboard_init() and mouse_init() succeeded
 *
 * "Just pressed/released not triggering":
 *   → Call keybind_update() exactly once per frame
 *   → Edge detection requires frame-to-frame state change
 *   → Check that action has bindings (keybind_get_binding)
 *
 * "Modifier bindings not working":
 *   → Modifiers must match exactly (CTRL+S requires CTRL held)
 *   → Use 0 for modifiers if you don't want modifier requirements
 *
 * "Binding slots full error":
 *   → Actions limited to KEYBIND_MAX_BINDINGS inputs
 *   → Use keybind_unbind_action() to clear before rebinding
 *
 * "Same key triggers multiple actions":
 *   → This is allowed — same key can map to multiple actions
 *   → Use binding groups for context-sensitive controls
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_reference X.7 REFERENCE — Quick Lookup
 * @ingroup keybind
 * @brief    Copy-paste ready keybind examples.
 *
 * BASIC SETUP:
 * @code{.c}
 * #include "input/keybind.h"
 *
 * keybind_init();  // Also initializes keyboard and mouse
 * // ... use actions ...
 * keybind_shutdown();
 * @endcode
 *
 * EVENT LOOP:
 * @code{.c}
 * while (running) {
 *     keybind_update();  // Must call once per iteration!
 *
 *     if (keybind_is_action_pressed(ACTION_CANCEL)) {
 *         running = false;
 *     }
 *     if (keybind_is_action_pressed(ACTION_NAV_UP)) {
 *         cursor_move(0, -1);
 *     }
 *     // ... other application logic ...
 * }
 * @endcode
 *
 * SINGLE-TRIGGER ACTIONS:
 * @code{.c}
 * // Use just_pressed for actions that should fire once
 * if (keybind_is_action_just_pressed(ACTION_COPY)) {
 *     copy_selection();
 * }
 * if (keybind_is_action_just_pressed(ACTION_CONFIRM)) {
 *     submit_form();
 * }
 * @endcode
 *
 * CUSTOM BINDINGS:
 * @code{.c}
 * // Clear defaults and set custom binding
 * keybind_unbind_action(ACTION_QUIT);
 * keybind_bind_key(ACTION_QUIT, KEY_Q, KEYMOD_CTRL);  // Ctrl+Q to quit
 *
 * // Add mouse binding to existing action
 * keybind_bind_button(ACTION_PRIMARY, MOUSE_BUTTON_LEFT, 0);
 * @endcode
 *
 * DISPLAY BINDINGS:
 * @code{.c}
 * char buffer[64];
 * keybind_get_binding_string(ACTION_QUIT, buffer, sizeof(buffer));
 * printf("Press %s to quit\n", buffer);  // "Press ESC / Q to quit"
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Final Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_note X.8 NOTE — Final Guidance
 * @ingroup keybind
 * @brief    System-level action abstraction — the RIGHT way to handle input.
 *
 * This header defines the ACTION layer. It provides:
 *   - Action: WHAT the user wants to do (semantic intent)
 *   - InputBinding: HOW the action is triggered (configurable mapping)
 *   - KeybindError: Error handling for binding operations
 *
 * Application code should NEVER check raw keys directly. Instead:
 *   - ❌ if (key.code == KEY_C && mods & KEYMOD_CTRL) copy();
 *   - ✅ if (keybind_is_action_pressed(ACTION_COPY)) copy();
 *
 * This enables:
 *   - User rebinding (accessibility)
 *   - Multiple input sources (keyboard OR mouse OR gamepad)
 *   - Platform abstraction (same actions, different defaults)
 *   - Clean separation of concerns
 *
 * ARCHITECTURE:
 *   keyboard.h (raw keys) + mouse.h (raw mouse) → keybind.h (actions) → apps
 *
 * @par Biblical Grounding
 * "And out of the ground the LORD God formed every beast of the field,
 *  and every fowl of the air; and brought them unto Adam to see what
 *  he would call them: and whatsoever Adam called every living creature,
 *  that was the name thereof." — Genesis 2:19
 *
 * Naming creates mastery. Raw inputs become named actions, enabling control.
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

#endif /* MOS_HAL_INPUT_KEYBIND_H */


