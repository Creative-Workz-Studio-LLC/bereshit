/// @file keyboard.h
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
/// @omni:req  key        = MOS-HAL-INPUT-KEYBOARD
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
/// @omni:ins  component  = kernel/hal/input
/// @omni:ins  layer      = kernel (layer 0)
/// @omni:ins  includes   = stdint, stdbool
/// @omni:ins  provides   = [KeyCode, KeyModifier, keyboard_*]
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
/// @omni:req  title      = Keyboard Input Interface
/// @omni:req  brief      = Kernel-level keyboard input — KeyCode enum and state API
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
 * @defgroup keyboard_special_config SP.1 CONFIG — Compile-time Settings
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
 * @defgroup keyboard_special_posix SP.1a POSIX — Feature Test
 * @ingroup keyboard_special_config
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

/** @} */ /* end SP.1a POSIX */

/** @} */ /* end SP.1 CONFIG */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * INCLUDE GUARD — Identity protection
 * ───────────────────────────────────────────────────────────────────────────── */

#ifndef MOS_HAL_INPUT_KEYBOARD_H
#define MOS_HAL_INPUT_KEYBOARD_H

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
 * @file      keyboard.h
 * @brief     Keyboard Input — Kernel-level key codes and state tracking.
 *
 * @defgroup keyboard_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       MOS-HAL-INPUT-KEYBOARD
 * Title:     Keyboard Input Interface
 * Type:      Header (Foundation)
 * Component: Engine/Core/Input — kernel-level peripheral abstraction
 * Role:      Raw key codes, modifier tracking, edge detection
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_state M.2 STATE — Lifecycle State
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
 * @defgroup keyboard_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup keyboard_location M.4 LOCATION — File Path
 * @{
 *
 * Path: millenniumos/kernel/include/hal/input/keyboard.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: bereshit/word/seed/code/c/header.h
 * Extracted:    2026-01-26 from display.h DisplayKey enum
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
 * @defgroup keyboard_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: header, c, engine, core, input, keyboard, keys, kernel, peripheral
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Kernel-level keyboard input abstraction
 * Core Design: KeyCode enum + state tracking, separate from display
 * Philosophy:  Input devices are peripherals, not display features
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Psalm 119:105 — "Thy word is a lamp unto my feet"
 * Principle:  Input illuminates intent — keys reveal what user wants
 * Anchor:     James 1:19 — "Swift to hear" — responsive input handling
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Standard: stdint.h, stdbool.h
 *   Internal: None (foundation module)
 *
 * What Uses This:
 *   - keybind.h — action mapping layer (semantic actions)
 *   - shell — terminal input handling
 *   - window manager — keyboard focus and shortcuts
 *   - text editors — character input and navigation
 *
 * @see keybind.h for action abstraction layer
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-26) — Initial creation from display.h extraction
 *   a-01.01 (2026-01-26) — Evdev scancode tables, key repeat config, compose keys
 *
 * Current:
 *   ✓ KeyCode enum (all keys)
 *   ✓ KeyModifier flags
 *   ✓ State query API
 *   ✓ Evdev-style scancode mapping (104-key US layout)
 *   ✓ Key repeat configuration (delay, rate, enabled)
 *   ✓ Dead key / compose sequence framework
 *
 * Planned:
 *   ⏳ International keyboard layouts
 *   ⏳ IME (Input Method Editor) support
 *   ⏳ Multiple keyboard instances
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
 * @defgroup keyboard_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard library includes for keyboard module.
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
 * @defgroup keyboard_standard S.1a STANDARD — System Headers
 * @ingroup keyboard_includes
 * @brief    C standard library headers required by this module.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include <stdint.h>   /**< Fixed-width integers: uint8_t, uint32_t */
#include <stdbool.h>  /**< Boolean type: bool, true, false */

/** @} */ /* end S.1a STANDARD */

/** S.1b INTERNAL — [None — foundation module] @defgroup keyboard_internal @ingroup keyboard_includes @{ @} */

/** S.1c EXTERNAL — [None] @defgroup keyboard_external @ingroup keyboard_includes @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_cpp S.1d CPP — C++ Linkage
 * @ingroup keyboard_includes
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
 * @defgroup keyboard_defines S.2 DEFINES — Preprocessor Constants
 * @brief    Keyboard input constants and limits.
 *
 * Builds FROM: S.1 INCLUDES (stdint types)
 * Builds TO:   S.3 TYPES (KeyCode enum size validation)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/** S.2a SIZES — [Reserved] @defgroup keyboard_sizes @ingroup keyboard_defines @{ @} */

/**
 * @defgroup keyboard_limits S.2b LIMITS — Key Count Bounds
 * @ingroup keyboard_defines
 * @brief    Keyboard key count limits.
 * @{ */
#define KEYBOARD_KEY_COUNT              256    /**< Total key slots                     */
#define KEYBOARD_KEY_INVALID            0xFF   /**< Invalid key sentinel                */
/** @} */ /* end S.2b LIMITS */

/** S.2c FLAGS — [Reserved] @defgroup keyboard_flags @ingroup keyboard_defines @{ @} */
/** S.2d MACROS — [Reserved] @defgroup keyboard_macros @ingroup keyboard_defines @{ @} */
/** S.2e VERIFY — [Reserved] @defgroup keyboard_verify @ingroup keyboard_defines @{ @} */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_types S.3 TYPES — Type Declarations
 * @ingroup keyboard
 * @brief    Keyboard input types — KeyCode, KeyModifier, KeyState.
 *
 * CONTAINS:
 *   S.3a ENUMS   — KeyCode (raw key identity), KeyModifier (modifier flags)
 *   S.3b STRUCTS — KeyState (full key event with code + modifiers)
 *   S.3c OPAQUE  — [Reserved]
 *   S.3d ERRORS  — [Reserved — errors in keybind.h action layer]
 *
 * Builds FROM: S.2 DEFINES (key count limits)
 * Builds TO:   S.4 PROTOTYPES (keyboard API functions)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a ENUMS — KeyCode and KeyModifier [ENUMS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_enums S.3a ENUMS — Key Enumerations
 * @ingroup keyboard_types
 * @brief    Raw key identity (KeyCode) and modifier flags (KeyModifier).
 *
 * ARCHITECTURE:
 *   KeyCode     — WHAT key (identity, hardware-level)
 *   KeyModifier — HOW pressed (shift, ctrl, alt)
 *   KeyState    — Full event (code + modifiers + pressed/released)
 *
 * PATTERN:
 *   KEY_NONE  = 0   — invalid/no key (always 0)
 *   KEY_COUNT       — sentinel for array sizing
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief KeyCode represents raw physical key identity.
 *
 * Hardware-level key identification. Does NOT include modifier state —
 * use KeyModifier flags for Shift/Ctrl/Alt combinations.
 *
 * @note KEY_NONE is always 0 (no key / invalid).
 * @note KEY_COUNT is sentinel for array sizing.
 */
typedef enum KeyCode {
    /* ─── Core / Invalid ─── */
    KEY_NONE = 0,        /**< No key / invalid (always 0)                      */

    /* ─── Navigation ─── */
    KEY_ENTER,           /**< Enter / Return                                   */
    KEY_ESCAPE,          /**< Escape                                           */
    KEY_TAB,             /**< Tab                                              */
    KEY_BACKSPACE,       /**< Backspace                                        */
    KEY_DELETE,          /**< Delete                                           */
    KEY_INSERT,          /**< Insert                                           */
    KEY_HOME,            /**< Home                                             */
    KEY_END,             /**< End                                              */
    KEY_PAGEUP,          /**< Page Up                                          */
    KEY_PAGEDOWN,        /**< Page Down                                        */

    /* ─── Arrow Keys ─── */
    KEY_UP,              /**< Arrow Up                                         */
    KEY_DOWN,            /**< Arrow Down                                       */
    KEY_LEFT,            /**< Arrow Left                                       */
    KEY_RIGHT,           /**< Arrow Right                                      */

    /* ─── Whitespace ─── */
    KEY_SPACE,           /**< Spacebar                                         */

    /* ─── Numbers (top row) ─── */
    KEY_0,               /**< 0 (top row)                                      */
    KEY_1,               /**< 1                                                */
    KEY_2,               /**< 2                                                */
    KEY_3,               /**< 3                                                */
    KEY_4,               /**< 4                                                */
    KEY_5,               /**< 5                                                */
    KEY_6,               /**< 6                                                */
    KEY_7,               /**< 7                                                */
    KEY_8,               /**< 8                                                */
    KEY_9,               /**< 9                                                */

    /* ─── Letters A-Z ─── */
    KEY_A,               /**< A                                                */
    KEY_B,               /**< B                                                */
    KEY_C,               /**< C                                                */
    KEY_D,               /**< D                                                */
    KEY_E,               /**< E                                                */
    KEY_F,               /**< F                                                */
    KEY_G,               /**< G                                                */
    KEY_H,               /**< H                                                */
    KEY_I,               /**< I                                                */
    KEY_J,               /**< J                                                */
    KEY_K,               /**< K                                                */
    KEY_L,               /**< L                                                */
    KEY_M,               /**< M                                                */
    KEY_N,               /**< N                                                */
    KEY_O,               /**< O                                                */
    KEY_P,               /**< P                                                */
    KEY_Q,               /**< Q                                                */
    KEY_R,               /**< R                                                */
    KEY_S,               /**< S                                                */
    KEY_T,               /**< T                                                */
    KEY_U,               /**< U                                                */
    KEY_V,               /**< V                                                */
    KEY_W,               /**< W                                                */
    KEY_X,               /**< X                                                */
    KEY_Y,               /**< Y                                                */
    KEY_Z,               /**< Z                                                */

    /* ─── Function Keys ─── */
    KEY_F1,              /**< F1                                               */
    KEY_F2,              /**< F2                                               */
    KEY_F3,              /**< F3                                               */
    KEY_F4,              /**< F4                                               */
    KEY_F5,              /**< F5                                               */
    KEY_F6,              /**< F6                                               */
    KEY_F7,              /**< F7                                               */
    KEY_F8,              /**< F8                                               */
    KEY_F9,              /**< F9                                               */
    KEY_F10,             /**< F10                                              */
    KEY_F11,             /**< F11                                              */
    KEY_F12,             /**< F12                                              */

    /* ─── Numpad ─── */
    KEY_NUMPAD_0,        /**< Numpad 0                                         */
    KEY_NUMPAD_1,        /**< Numpad 1                                         */
    KEY_NUMPAD_2,        /**< Numpad 2                                         */
    KEY_NUMPAD_3,        /**< Numpad 3                                         */
    KEY_NUMPAD_4,        /**< Numpad 4                                         */
    KEY_NUMPAD_5,        /**< Numpad 5                                         */
    KEY_NUMPAD_6,        /**< Numpad 6                                         */
    KEY_NUMPAD_7,        /**< Numpad 7                                         */
    KEY_NUMPAD_8,        /**< Numpad 8                                         */
    KEY_NUMPAD_9,        /**< Numpad 9                                         */
    KEY_NUMPAD_ENTER,    /**< Numpad Enter                                     */
    KEY_NUMPAD_PLUS,     /**< Numpad +                                         */
    KEY_NUMPAD_MINUS,    /**< Numpad -                                         */
    KEY_NUMPAD_MULTIPLY, /**< Numpad *                                         */
    KEY_NUMPAD_DIVIDE,   /**< Numpad /                                         */
    KEY_NUMPAD_DECIMAL,  /**< Numpad .                                         */

    /* ─── Punctuation / Symbols ─── */
    KEY_MINUS,           /**< - (minus/hyphen)                                 */
    KEY_EQUALS,          /**< = (equals)                                       */
    KEY_LBRACKET,        /**< [ (left bracket)                                 */
    KEY_RBRACKET,        /**< ] (right bracket)                                */
    KEY_BACKSLASH,       /**< \ (backslash)                                    */
    KEY_SEMICOLON,       /**< ; (semicolon)                                    */
    KEY_APOSTROPHE,      /**< ' (apostrophe/quote)                             */
    KEY_GRAVE,           /**< ` (grave/backtick)                               */
    KEY_COMMA,           /**< , (comma)                                        */
    KEY_PERIOD,          /**< . (period)                                       */
    KEY_SLASH,           /**< / (forward slash)                                */

    /* ─── Lock Keys ─── */
    KEY_CAPSLOCK,        /**< Caps Lock                                        */
    KEY_NUMLOCK,         /**< Num Lock                                         */
    KEY_SCROLLLOCK,      /**< Scroll Lock                                      */

    /* ─── Modifier Keys (as key codes, not flags) ─── */
    KEY_LSHIFT,          /**< Left Shift                                       */
    KEY_RSHIFT,          /**< Right Shift                                      */
    KEY_LCTRL,           /**< Left Control                                     */
    KEY_RCTRL,           /**< Right Control                                    */
    KEY_LALT,            /**< Left Alt                                         */
    KEY_RALT,            /**< Right Alt                                        */
    KEY_LSUPER,          /**< Left Super/Win/Cmd                               */
    KEY_RSUPER,          /**< Right Super/Win/Cmd                              */

    /* ─── System Keys ─── */
    KEY_PRINTSCREEN,     /**< Print Screen                                     */
    KEY_PAUSE,           /**< Pause/Break                                      */
    KEY_MENU,            /**< Menu/Application key                             */

    /* ─── Special Events ─── */
    KEY_CHAR,            /**< Printable character (check keyboard_get_char)    */

    /* ─── Sentinel ─── */
    KEY_COUNT            /**< Sentinel for array sizing (do not use as value)  */
} KeyCode;

/**
 * @brief KeyModifier represents modifier key state flags.
 *
 * Bit flags indicating which modifier keys are active during a key event.
 * Combine with bitwise OR for multiple modifiers (e.g., KEYMOD_CTRL | KEYMOD_SHIFT).
 *
 * @note KEYMOD_NONE is always 0 (no modifiers).
 */
typedef enum KeyModifier {
    KEYMOD_NONE  = 0,        /**< No modifiers                                 */
    KEYMOD_SHIFT = (1 << 0), /**< Shift key (left or right)                    */
    KEYMOD_CTRL  = (1 << 1), /**< Control key (left or right)                  */
    KEYMOD_ALT   = (1 << 2), /**< Alt key (left or right)                      */
    KEYMOD_SUPER = (1 << 3), /**< Super/Win/Cmd key (left or right)            */
    KEYMOD_CAPS  = (1 << 4), /**< Caps Lock active                             */
    KEYMOD_NUM   = (1 << 5)  /**< Num Lock active                              */
} KeyModifier;

/** @} */ /* end S.3a ENUMS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b STRUCTS — KeyState Structure [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_structs S.3b STRUCTS — Key Event Structures
 * @ingroup keyboard_types
 * @brief    Full key event with code, modifiers, and state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief KeyState represents a complete keyboard event.
 *
 * Combines key identity (KeyCode), hardware scancode, modifier state,
 * and pressed/released state for full event information.
 *
 * @note For KEY_CHAR events, use keyboard_get_char() for the actual character.
 * @note scancode is platform-specific hardware code (useful for raw input).
 * @note repeat_count tracks how many times the key has auto-repeated.
 */
typedef struct KeyState {
    KeyCode   code;         /**< Which key (logical identity)                     */
    uint16_t  scancode;     /**< Hardware scancode (platform-specific)            */
    uint8_t   modifiers;    /**< Modifier flags (KeyModifier OR'd together)       */
    uint8_t   pressed;      /**< 1 = pressed (key down), 0 = released (key up)    */
    uint16_t  repeat_count; /**< 0 = initial press, >0 = auto-repeat count        */
} KeyState;

/**
 * @brief KeyRepeatConfig holds key repeat timing settings.
 *
 * Controls how keyboard auto-repeat behaves:
 *   - delay_ms: Time after initial press before repeat starts
 *   - rate_ms:  Interval between repeated events
 *   - enabled:  Whether repeat is active
 *
 * @note Typical desktop defaults: delay=500ms, rate=30ms (33 chars/sec)
 */
typedef struct KeyRepeatConfig {
    uint16_t delay_ms;  /**< Delay before repeat starts (ms)                    */
    uint16_t rate_ms;   /**< Interval between repeats (ms)                      */
    bool     enabled;   /**< Whether key repeat is enabled                      */
} KeyRepeatConfig;

/** @} */ /* end S.3b STRUCTS */

/** S.3c OPAQUE — [Reserved] @defgroup keyboard_opaque @ingroup keyboard_types @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3d ERRORS — Error Codes [ERRORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_errors S.3d ERRORS — Keyboard Error Codes
 * @ingroup keyboard_types
 * @brief    Keyboard subsystem error codes.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief KeyboardError represents keyboard subsystem errors.
 */
typedef enum KeyboardError {
    KEYBOARD_OK = 0,         /**< Success / no error                           */
    KEYBOARD_ERR_INIT,       /**< Initialization failed                        */
    KEYBOARD_ERR_NOT_INIT,   /**< Keyboard not initialized                     */
    KEYBOARD_ERR_INVALID     /**< Invalid parameter                            */
} KeyboardError;

/** @} */ /* end S.3d ERRORS */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_prototypes S.4 PROTOTYPES — Function Declarations
 * @ingroup keyboard
 * @brief    Keyboard input API function declarations.
 *
 * STRUCTURE:
 *   S.4a LIFECYCLE  — keyboard_init, keyboard_shutdown
 *   S.4b ACCESSORS  — keyboard_is_pressed, keyboard_get_*
 *   S.4c OPERATIONS — keyboard_poll, keyboard_wait_key
 *
 * Builds FROM: S.3 TYPES (KeyCode, KeyState, KeyModifier)
 * Builds TO:   BODY (implementations in keyboard.c)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a LIFECYCLE — Init/Shutdown [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_lifecycle S.4a LIFECYCLE — Init/Shutdown
 * @ingroup keyboard_prototypes
 * @brief    Keyboard subsystem lifecycle management.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize the keyboard input subsystem.
 *
 * Must be called before any other keyboard functions.
 * Safe to call multiple times (idempotent).
 *
 * @return KEYBOARD_OK on success, error code on failure.
 */
KeyboardError keyboard_init(void);

/**
 * @brief Shutdown the keyboard input subsystem.
 *
 * Releases all resources. Safe to call if not initialized (no-op).
 * After shutdown, keyboard_init() must be called again to use keyboard.
 */
void keyboard_shutdown(void);

/**
 * @brief Update keyboard state for new frame.
 *
 * Call this at the start of each frame before polling input.
 * Copies current key states to previous for edge detection.
 * Required for keyboard_is_just_pressed/released to work correctly.
 */
void keyboard_update(void);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b ACCESSORS — State Queries [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_accessors S.4b ACCESSORS — State Queries
 * @ingroup keyboard_prototypes
 * @brief    Query current keyboard state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Check if a specific key is currently pressed.
 *
 * @param[in] code  KeyCode to check.
 *
 * @return 1 if key is pressed, 0 if released or invalid.
 */
int keyboard_is_pressed(KeyCode code);

/**
 * @brief Check if a key was just pressed this frame.
 *
 * Returns true only on the frame the key transitioned from released to pressed.
 * Useful for actions that should fire once per key press.
 *
 * @param[in] code  KeyCode to check.
 *
 * @return 1 if key was just pressed, 0 otherwise.
 */
int keyboard_is_just_pressed(KeyCode code);

/**
 * @brief Check if a key was just released this frame.
 *
 * Returns true only on the frame the key transitioned from pressed to released.
 *
 * @param[in] code  KeyCode to check.
 *
 * @return 1 if key was just released, 0 otherwise.
 */
int keyboard_is_just_released(KeyCode code);

/**
 * @brief Get current modifier key state.
 *
 * @return Bitmask of currently active KeyModifier flags.
 */
uint8_t keyboard_get_modifiers(void);

/**
 * @brief Get the character for the last KEY_CHAR event.
 *
 * Only valid immediately after receiving a KEY_CHAR event
 * from keyboard_poll() or keyboard_wait_key().
 *
 * @return UTF-8 character code, or 0 if no character available.
 */
uint32_t keyboard_get_char(void);

/**
 * @brief Convert KeyCode to human-readable name.
 *
 * @param[in] code  KeyCode to convert.
 *
 * @return Static string with key name (e.g., "KEY_ESCAPE"), or "KEY_UNKNOWN".
 */
const char* keyboard_get_key_name(KeyCode code);

/**
 * @brief Convert hardware scancode to KeyCode.
 *
 * Maps platform-specific scancodes to logical KeyCodes.
 * Useful for custom key mapping or raw input handling.
 *
 * @param[in] scancode  Hardware scancode to convert.
 *
 * @return Corresponding KeyCode, or KEY_NONE if unmapped.
 */
KeyCode keyboard_scancode_to_keycode(uint16_t scancode);

/**
 * @brief Get hardware scancode for a KeyCode.
 *
 * Reverse lookup - find the default scancode for a logical key.
 * Platform-specific; may return 0 for keys without hardware mapping.
 *
 * @param[in] code  KeyCode to look up.
 *
 * @return Hardware scancode, or 0 if no mapping exists.
 */
uint16_t keyboard_keycode_to_scancode(KeyCode code);

/**
 * @brief Set key repeat timing configuration.
 *
 * Configures how keyboard auto-repeat behaves. Setting enabled=false
 * disables repeat entirely.
 *
 * @param[in] delay_ms  Delay before repeat starts (typical: 500ms)
 * @param[in] rate_ms   Interval between repeats (typical: 30ms)
 * @param[in] enabled   Whether to enable key repeat
 *
 * @return KEYBOARD_OK on success, KEYBOARD_ERR_NOT_INIT if not initialized.
 */
KeyboardError keyboard_set_repeat_config(uint16_t delay_ms, uint16_t rate_ms, bool enabled);

/**
 * @brief Get current key repeat configuration.
 *
 * @param[out] delay_ms  Receives current delay setting (may be NULL)
 * @param[out] rate_ms   Receives current rate setting (may be NULL)
 * @param[out] enabled   Receives current enabled state (may be NULL)
 *
 * @return KEYBOARD_OK on success, KEYBOARD_ERR_NOT_INIT if not initialized.
 */
KeyboardError keyboard_get_repeat_config(uint16_t* delay_ms, uint16_t* rate_ms, bool* enabled);

/** @} */ /* end S.4b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c OPERATIONS — Input Operations [OPERATIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_operations S.4c OPERATIONS — Input Operations
 * @ingroup keyboard_prototypes
 * @brief    Key event polling and waiting.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Poll for keyboard events (non-blocking).
 *
 * Checks for pending keyboard events without blocking.
 * Call in main loop for responsive input handling.
 *
 * @param[out] state  Pointer to KeyState to fill. Must not be NULL.
 *
 * @return 1 if event available (state filled), 0 if no event pending.
 */
int keyboard_poll(KeyState* state);

/**
 * @brief Wait for a keyboard event (blocking).
 *
 * Blocks until a key event occurs. Use keyboard_poll() for non-blocking.
 *
 * @param[out] state  Pointer to KeyState to fill. Must not be NULL.
 *
 * @return KEYBOARD_OK on success, error code on failure.
 */
KeyboardError keyboard_wait_key(KeyState* state);

/**
 * @brief Flush all pending keyboard events.
 *
 * Clears the keyboard event queue. Use when transitioning
 * between input contexts to avoid stale events.
 */
void keyboard_flush(void);

/**
 * @brief Clear all key states.
 *
 * Resets all key states to "not pressed". Use in terminal mode at the
 * start of each frame since terminals only send key press events, not
 * release events. This allows keys to behave as single-frame pulses.
 */
void keyboard_clear_states(void);

/**
 * @brief Process a key through compose/dead-key sequences.
 *
 * For dead key input (accented characters). Feed keys through this
 * function to build compose sequences. Returns the composed character
 * when a sequence completes.
 *
 * EXAMPLE (typing ` then e to get è):
 * @code{.c}
 * KeyCode result = keyboard_compose_key(KEY_GRAVE);  // Returns KEY_NONE (waiting)
 * result = keyboard_compose_key(KEY_E);              // Returns composed 'è' as KEY_CHAR
 * @endcode
 *
 * @param[in] code  KeyCode to add to compose sequence
 *
 * @return Composed KeyCode, or KEY_NONE if sequence incomplete
 */
KeyCode keyboard_compose_key(KeyCode code);

/**
 * @brief Cancel any in-progress compose sequence.
 *
 * Resets the compose state machine. Use when user presses Escape
 * or switches input context.
 */
void keyboard_cancel_compose(void);

/** @} */ /* end S.4c OPERATIONS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4d INJECTION — GUI Backend Support [INJECTION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_injection S.4d INJECTION — GUI Backend Support
 * @ingroup keyboard_prototypes
 * @brief    Functions for GUI backends (SDL2, GLFW) to inject keyboard events.
 *
 * These functions allow display backends to feed keyboard events into the
 * kernel input layer. Call from SDL_KEYDOWN/SDL_KEYUP handlers or
 * GLFW key callbacks.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Inject a key press or release event.
 *
 * Called by GUI backends when receiving key events from SDL2, GLFW, etc.
 *
 * @param[in] code     KeyCode being pressed or released.
 * @param[in] pressed  1 = key down (pressed), 0 = key up (released).
 *
 * EXAMPLE (SDL2):
 * @code{.c}
 * case SDL_KEYDOWN:
 *     keyboard_inject_key(sdl_to_keycode(event.key.keysym.scancode), 1);
 *     break;
 * case SDL_KEYUP:
 *     keyboard_inject_key(sdl_to_keycode(event.key.keysym.scancode), 0);
 *     break;
 * @endcode
 */
void keyboard_inject_key(KeyCode code, int pressed);

/**
 * @brief Inject a raw hardware scancode event.
 *
 * For platforms providing raw scancodes. The subsystem will translate
 * to KeyCode internally using the scancode table.
 *
 * @param[in] scancode  Hardware scancode (platform-specific).
 * @param[in] pressed   1 = key down, 0 = key up.
 */
void keyboard_inject_scancode(uint16_t scancode, int pressed);

/**
 * @brief Inject modifier key state.
 *
 * Sets the current modifier state directly. Useful when modifier state
 * comes from the windowing system rather than individual key events.
 *
 * @param[in] modifiers  Bitmask of KeyModifier flags.
 *
 * EXAMPLE (SDL2):
 * @code{.c}
 * SDL_Keymod sdl_mods = SDL_GetModState();
 * uint8_t mods = 0;
 * if (sdl_mods & KMOD_SHIFT) mods |= KEYMOD_SHIFT;
 * if (sdl_mods & KMOD_CTRL)  mods |= KEYMOD_CTRL;
 * if (sdl_mods & KMOD_ALT)   mods |= KEYMOD_ALT;
 * keyboard_inject_modifiers(mods);
 * @endcode
 */
void keyboard_inject_modifiers(uint8_t modifiers);

/**
 * @brief Inject a text input character.
 *
 * For text input events (SDL_TEXTINPUT, GLFW character callback).
 * Sets up a KEY_CHAR event that can be retrieved via keyboard_get_char().
 *
 * @param[in] codepoint  Unicode codepoint (UTF-32).
 *
 * EXAMPLE (SDL2):
 * @code{.c}
 * case SDL_TEXTINPUT:
 *     // SDL provides UTF-8, decode to codepoint
 *     uint32_t cp = utf8_decode(event.text.text);
 *     keyboard_inject_char(cp);
 *     break;
 * @endcode
 */
void keyboard_inject_char(uint32_t codepoint);

/**
 * @brief Inject a complete KeyState event.
 *
 * Full control injection for complex scenarios. Queues the event
 * to be returned by the next keyboard_poll() call.
 *
 * @param[in] state  Complete KeyState to inject. Must not be NULL.
 */
void keyboard_inject_event(const KeyState* state);

/** @} */ /* end S.4d INJECTION */

/** @} */ /* end S.4 PROTOTYPES */

/** S.5 FILE-LEVEL — [Reserved — no global state needed] @defgroup keyboard_file @ingroup keyboard @{ @} */
/** S.6 OMISSION — [Reserved] @defgroup keyboard_omission @ingroup keyboard @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Header file — declarations only, implementations in keyboard.c
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * B.1 ORG CHART — Structure Overview
 * Types:     3 enums (KeyCode, KeyModifier, KeyboardError), 1 struct (KeyState)
 * Functions: 2 lifecycle, 6 accessors, 3 operations
 * Impl:      keyboard.c
 * @defgroup keyboard_orgchart @ingroup keyboard @{ @} */

/** B.2 HELPERS — [Reserved — helpers in keyboard.c] @defgroup keyboard_helpers @ingroup keyboard @{ @} */

/** B.3 CORE OPS — [Reserved — operations in keyboard.c] @defgroup keyboard_core_ops @ingroup keyboard @{ @} */

/** B.4 ERRORS — [Reserved — error handling in keybind.h action layer] @defgroup keyboard_errors_handling @ingroup keyboard @{ @} */

/** B.5 PUBLIC API — [Declared in S.4 PROTOTYPES] @defgroup keyboard_public_api @ingroup keyboard @{ @} */
/** B.6 OMISSION — [Reserved] @defgroup keyboard_body_omission @ingroup keyboard @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Operational guidance for keyboard input subsystem.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/** X.1 VALIDATION — [See keyboard.c tests] @defgroup keyboard_validation @ingroup keyboard @{ @} */
/** X.2 EXECUTION — [See X.7 REFERENCE] @defgroup keyboard_execution @ingroup keyboard @{ @} */
/** X.3 CLEANUP — keyboard_init/shutdown manage lifecycle @defgroup keyboard_cleanup @ingroup keyboard @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidelines [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_policy X.4 POLICY — Modification Guidelines
 * @ingroup keyboard
 * @brief    Guide future maintainers — what's safe to change.
 *
 * SAFE TO MODIFY:
 *   - ✅ Add new KeyCode values (before KEY_COUNT)
 *   - ✅ Add new KeyModifier flags (use next available bit)
 *   - ✅ Add new accessor functions (keyboard_get_*)
 *   - ✅ Add new fields to KeyState (maintain ABI compatibility)
 *
 * MODIFY WITH CARE:
 *   - ⚠️ KeyCode enum order — may break serialization/config
 *   - ⚠️ KeyState struct layout — affects binary compatibility
 *   - ⚠️ Function signatures — breaks all calling code
 *
 * NEVER MODIFY:
 *   - ❌ KEY_NONE = 0 (sentinel behavior)
 *   - ❌ KEYMOD_NONE = 0 (flag identity)
 *   - ❌ keyboard_init/shutdown contract
 *   - ❌ 4-block structure
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — Planned Upgrades [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_extension X.5 EXTENSION — Planned Upgrades
 * @ingroup keyboard
 * @brief    Roadmap for keyboard subsystem expansion.
 *
 * PHASE 1 (Current):
 *   - KeyCode enum with standard keys
 *   - KeyModifier flags
 *   - KeyState with scancode + repeat support
 *   - Basic polling/waiting API
 *
 * PHASE 2 (Planned):
 *   - Platform-specific scancode tables (Linux, Windows, macOS)
 *   - Key name localization
 *   - Dead key / compose key support
 *   - International keyboard layouts
 *
 * PHASE 3 (Future):
 *   - Hot-plugging detection
 *   - Multiple keyboard support
 *   - Virtual keyboard injection
 *   - Raw HID access for custom devices
 *
 * INTEGRATION POINTS:
 *   - keybind.h: Action abstraction layer (consumes KeyState)
 *   - hal.h: PS/2 hardware driver provides raw events
 *   - shell: Uses keybind actions for navigation and commands
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @ingroup keyboard
 * @brief    Common keyboard issues and solutions.
 *
 * "keyboard_poll returns 0 but keys are being pressed":
 *   → Ensure keyboard_init() was called first
 *   → Check if terminal is in raw mode (required for non-blocking)
 *   → Verify stdin is connected to a terminal (isatty)
 *
 * "KEY_CHAR event but keyboard_get_char() returns 0":
 *   → Call keyboard_get_char() immediately after receiving event
 *   → Character is only valid until next poll/wait call
 *
 * "Scancode is always 0":
 *   → Platform may not provide scancodes in terminal mode
 *   → Scancodes require direct device access in kernel mode
 *
 * "Modifiers not detected":
 *   → Some terminals don't report modifier-only key events
 *   → Use keyboard_get_modifiers() for current state
 *
 * "Repeat events not working":
 *   → Terminal key repeat is OS-controlled
 *   → Use repeat_count field to detect auto-repeat
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_reference X.7 REFERENCE — Quick Lookup
 * @ingroup keyboard
 * @brief    Copy-paste ready keyboard examples.
 *
 * BASIC SETUP:
 * @code{.c}
 * #include "input/keyboard.h"
 *
 * keyboard_init();
 * // ... use keyboard ...
 * keyboard_shutdown();
 * @endcode
 *
 * POLLING LOOP:
 * @code{.c}
 * KeyState key;
 * while (running) {
 *     if (keyboard_poll(&key)) {
 *         if (key.pressed && key.code == KEY_ESCAPE) {
 *             running = false;
 *         }
 *     }
 *     // ... other application logic ...
 * }
 * @endcode
 *
 * BLOCKING INPUT:
 * @code{.c}
 * KeyState key;
 * keyboard_wait_key(&key);
 * if (key.code == KEY_ENTER) {
 *     // User confirmed
 * }
 * @endcode
 *
 * MODIFIER CHECK:
 * @code{.c}
 * if (key.modifiers & KEYMOD_CTRL) {
 *     if (key.code == KEY_S) {
 *         save_file();  // Ctrl+S
 *     }
 * }
 * @endcode
 *
 * CHARACTER INPUT:
 * @code{.c}
 * if (key.code == KEY_CHAR) {
 *     uint32_t ch = keyboard_get_char();
 *     if (ch != 0) {
 *         buffer_append(ch);
 *     }
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
 * @defgroup keyboard_note X.8 NOTE — Final Guidance
 * @ingroup keyboard
 * @brief    Kernel input layer — foundation for all keyboard interaction.
 *
 * This header defines the RAW keyboard layer. It provides:
 *   - KeyCode: WHAT key (identity)
 *   - KeyModifier: HOW pressed (shift, ctrl, alt)
 *   - KeyState: Full event (code + modifiers + scancode + repeat)
 *
 * For ACTION-based input (shortcuts, navigation, commands), use keybind.h.
 * Code should ask "is COPY pressed?" not "is Ctrl+C pressed?" — keybind.h
 * provides this abstraction for rebindable, user-friendly controls.
 *
 * ARCHITECTURE:
 *   keyboard.h (raw keys) → keybind.h (actions) → consumers
 *
 * @par Biblical Grounding
 * "For precept must be upon precept, precept upon precept;
 *  line upon line, line upon line; here a little, and there a little."
 * — Isaiah 28:10
 *
 * Build input handling layer by layer: raw keys → actions → behaviors.
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

#endif /* MOS_HAL_INPUT_KEYBOARD_H */


