/// @file keyboard.c
/// @omni document --source
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
/// @omni:req  key        = MOS-HAL-INPUT-KEYBOARD-IMPL
/// @omni:req  from       = bereshit/word/seed/code/c/source.c
/// @omni:req  at         = document
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = source
/// @omni:inh  subtype    = implementation
/// @omni:inh  role       = peripheral
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = kernel/hal/input
/// @omni:ins  layer      = 0:kernel
/// @omni:ins  includes   = [keyboard.h]
/// @omni:ins  provides   = [KEYBOARD_SUBSYSTEM]
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.4 ARCHITECTURE — Scaling Context [ARCH]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  layers     = [0:kernel, 1:system, 2:runtime, 3:framework,
///                          4:health, 5:network, 6:storage, 7:engine, 8:app]
/// @omni:ins  scale      = 3^7 = 2187 (engine layer)
/// @omni:ins  pattern    = 4+1+4 [below:0-3, center:4, above:5-8]
/// @omni:ins  anchor     = genesis_1_1 -> peripheral -> input -> keyboard
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.5 SUMMARY — Human Identity [SUMMARY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:req  title      = Keyboard Input Implementation
/// @omni:req  brief      = Raw key codes, state tracking, terminal input handling
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
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * SP.1 CONFIG — Compile-time Settings [CONFIG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_special_config SP.1 CONFIG — Compile-time Settings
 * @brief    Feature macros and environment configuration before includes.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * SP.1a POSIX — Feature Test [POSIX]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_special_posix SP.1a MODE — Kernel Mode
 * @ingroup keyboard_special_config
 * @brief    Kernel freestanding mode (no POSIX, uses PS/2 HAL).
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

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Identity and context for this component.
///
/// STRUCTURE: M.1-M.10 sections, grouped for readability.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @file      keyboard.c
 * @brief     Keyboard Input — kernel-level key codes and state tracking.
 *
 * @defgroup keyboard_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       MOS-HAL-INPUT-KEYBOARD-IMPL
 * Title:     Keyboard Input Implementation
 * Type:      Source (Implementation)
 * Component: Engine (Core Input) — peripheral abstraction
 * Role:      Raw key codes, modifier tracking, terminal input handling
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
 * @version   a-01.50
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
 * M.3 ATTRIBUTION — Authorship & Rights [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_attribution M.3 ATTRIBUTION — Authorship & Rights
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
 * M.4 LOCATION — File Position [LOCATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_location M.4 LOCATION — File Position
 * @{
 *
 * Path:      cornerstone/engine/core/src/input/keyboard.c
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Template Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_derivation M.5 DERIVATION — Template Lineage
 * @{
 *
 * Derives:   bereshit/word/seed/code/c/source.c (template)
 * Header:    engine/core/include/input/keyboard.h
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_classification M.6 CLASSIFICATION — Categorization
 * @{
 *
 * Tags:      source, c, engine, core, input, keyboard, keys, kernel, peripheral
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose Statement [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_intent M.7 INTENT — Purpose Statement
 * @{
 *
 * Purpose:      Kernel-level keyboard input abstraction
 * Core Design:  KeyCode enum + state tracking, separate from display
 * Philosophy:   Input devices are peripherals, not display features
 *
 * Extracted from display backends to:
 *   - Separate input from output concerns
 *   - Provide unified keyboard API across all modes
 *   - Enable keybind action mapping layer
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
 * Scripture: Psalm 119:105 — "Thy word is a lamp unto my feet"
 * Principle: Input illuminates intent — keys reveal what user wants
 * Anchor:    James 1:19 — "Swift to hear" — responsive input handling
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_dependencies M.9 DEPENDENCIES — Required Components
 * @{
 *
 * What This Needs:
 *   Standard:  stdint.h, stdbool.h, stddef.h (freestanding)
 *   HAL:       hal/hal.h (MosEvent, mos_hal_poll_event)
 *   Internal:  keyboard.h (our header)
 *
 * What Uses This:
 *   - shell/terminal/shell.c (terminal input)
 *   - kernel.c (main loop)
 *   - keybind.c (action mapping layer)
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — Version History [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_roadmap M.10 ROADMAP — Version History
 * @{
 *
 * History:
 *   a-01.00 (2026-01-26) — Initial implementation with terminal raw mode
 *   a-01.50 (2026-01-26) — Added scancode tables, key repeat config, dead key stubs
 *
 * Implemented:
 *   ✓ KeyCode enum (all keys in header)
 *   ✓ keyboard_init / keyboard_shutdown
 *   ✓ keyboard_wait_key (blocking)
 *   ✓ keyboard_poll (non-blocking)
 *   ✓ keyboard_get_key_name
 *   ✓ Terminal raw mode handling
 *   ✓ ANSI escape sequence parsing
 *   ✓ Scancode mapping tables (evdev-style)
 *   ✓ Key repeat configuration (rate, delay)
 *   ✓ Dead key / compose sequence framework
 *
 * Planned:
 *   ⏳ SDL2 scancode integration (for GUI mode)
 *   ⏳ Full compose sequence database
 *   ⏳ Input method editor (IME) support
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
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES — Header Dependencies [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
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
 * @brief    C standard library and POSIX headers.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Freestanding headers only - no libc in kernel mode */
#include <stdint.h>      /**< uint8_t, uint16_t, uint32_t */
#include <stdbool.h>     /**< bool, true, false */
#include <stddef.h>      /**< size_t, NULL */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b PROJECT — Project Headers [PROJECT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_project S.1b PROJECT — Project Headers
 * @ingroup keyboard_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "hal/input/keyboard.h"  /**< Public interface — KeyCode, KeyState, API */
#include "hal/hal.h"             /**< MosEvent, mos_hal_poll_event, mos_hal_wait_event */
#include "kernel.h"              /**< mos_inb, mos_outb, boot path */

/** @} */ /* end S.1b PROJECT */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup keyboard_includes
 * @brief    [Reserved: No external headers — POSIX only]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: SDL2 headers will be added for GUI mode] */

/** @} */ /* end S.1c EXTERNAL */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Preprocessor Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_defines S.2 DEFINES — Preprocessor Constants
 * @brief    Constants and macros for this component.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a SIZES — Buffer and Array Sizes [SIZES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_sizes S.2a SIZES — Buffer and Array Sizes
 * @ingroup keyboard_defines
 * @brief    Buffer sizes and array capacities.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Maximum bytes in a key sequence buffer.
 *
 * Handles extended escape sequences (e.g., ESC [ 1 ; 5 A for Ctrl+Up).
 */
#define KEY_BUFFER_SIZE     16

/**
 * @brief Maximum dead key compose sequences.
 */
#define DEAD_KEY_MAX_COMPOSE  4

/** @} */ /* end S.2a SIZES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b LIMITS — Value Bounds [LIMITS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_limits S.2b LIMITS — Value Bounds
 * @ingroup keyboard_defines
 * @brief    Timing and rate limits.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief 50ms timeout for escape sequence completion.
 *
 * When an ESC byte is received, we wait this long for additional bytes
 * to determine if it's part of an ANSI escape sequence or standalone ESC.
 */
#define ESCAPE_TIMEOUT_US       50000

/**
 * @brief Default key repeat delay in milliseconds.
 *
 * Time before first repeat event fires after initial key press.
 */
#define KEY_REPEAT_DELAY_DEFAULT    500

/**
 * @brief Default key repeat rate in milliseconds.
 *
 * Time between subsequent repeat events.
 */
#define KEY_REPEAT_RATE_DEFAULT     30

/**
 * @brief Minimum key repeat delay (ms).
 */
#define KEY_REPEAT_DELAY_MIN        100

/**
 * @brief Maximum key repeat delay (ms).
 */
#define KEY_REPEAT_DELAY_MAX        2000

/**
 * @brief Minimum key repeat rate (ms).
 */
#define KEY_REPEAT_RATE_MIN         10

/**
 * @brief Maximum key repeat rate (ms).
 */
#define KEY_REPEAT_RATE_MAX         500

/** @} */ /* end S.2b LIMITS */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_types S.3 TYPES — Type Declarations
 * @brief    Internal types for key repeat and dead key handling.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a HEADER TYPES — Types from Header [HEADER_TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_header_types S.3a HEADER TYPES — Types from Header
 * @ingroup keyboard_types
 * @brief    Types available from keyboard.h — DO NOT REDEFINE.
 *
 * Types from header.h:
 *   - KeyCode         — Key enumeration
 *   - KeyState        — Key event data
 *   - KeyboardError   — Error codes
 *   - KEYMOD_*        — Modifier flags
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* All public types come from keyboard.h — no redefinition needed */

/** @} */ /* end S.3a HEADER TYPES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b INTERNAL — Internal-Only Types [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_internal_types S.3b INTERNAL — Internal-Only Types
 * @ingroup keyboard_types
 * @brief    Types used only within this compilation unit.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* KeyRepeatConfig is defined in keyboard.h — use that typedef directly */

/**
 * @brief Dead key compose state.
 *
 * Tracks multi-key compose sequences (e.g., ` + e = è).
 */
typedef struct DeadKeyState {
    KeyCode  pending[DEAD_KEY_MAX_COMPOSE];  /**< @brief Pending dead keys. */
    uint8_t  count;                           /**< @brief Number of pending keys. */
    bool     active;                          /**< @brief Compose mode active. */
} DeadKeyState;

/**
 * @brief Scancode-to-KeyCode mapping entry.
 *
 * Used for evdev-style scancode tables.
 */
typedef struct ScancodeEntry {
    uint16_t scancode;  /**< @brief Hardware scancode (evdev). */
    KeyCode  keycode;   /**< @brief Mapped KeyCode. */
} ScancodeEntry;

/** @} */ /* end S.3b INTERNAL */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Internal Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_prototypes S.4 PROTOTYPES — Internal Function Declarations
 * @brief    Forward declarations for internal static functions.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a INTERNAL_HELPERS — Private Helper Functions [INTERNAL_HELPERS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_internal_helpers S.4a INTERNAL_HELPERS — Private Helpers
 * @ingroup keyboard_prototypes
 * @brief    Static functions used internally by this compilation unit.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Enable keyboard input (kernel mode - no-op).
 * @return true always.
 */
static bool enable_raw_mode(void);

/**
 * @brief Disable keyboard input (kernel mode - no-op).
 */
static void disable_raw_mode(void);

/**
 * @brief Poll for key event from HAL event queue.
 *
 * @param[out] event  Pointer to MosEvent to fill.
 *
 * @return true if event available, false if no input.
 */
static bool poll_hal_key_event(MosEvent* event);

/**
 * @brief Parse ANSI escape sequence to KeyCode.
 *
 * @param[in] buf  Escape sequence bytes (starting with ESC).
 * @param[in] len  Number of bytes in sequence.
 *
 * @return Decoded KeyCode, or KEY_ESCAPE for unknown sequences.
 */
static KeyCode parse_escape_sequence(const uint8_t* buf, size_t len);

/**
 * @brief Parse single byte to KeyCode.
 *
 * @param[in] byte  Single input byte.
 *
 * @return Decoded KeyCode.
 */
static KeyCode parse_single_byte(uint8_t byte);

/**
 * @brief Lookup KeyCode from evdev-style scancode table.
 *
 * @param[in] scancode  Hardware scancode (evdev).
 *
 * @return Mapped KeyCode, or KEY_NONE if not found.
 */
static KeyCode lookup_scancode_evdev(uint16_t scancode);

/**
 * @brief Process dead key compose sequence.
 *
 * @param[in] code  Input KeyCode to compose.
 *
 * @return Composed KeyCode, or KEY_NONE if composition pending.
 */
static KeyCode process_dead_key(KeyCode code);

/** @} */ /* end S.4a INTERNAL_HELPERS */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — File Scope Declarations [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_file S.5 FILE-LEVEL — File Scope Declarations
 * @brief    Static variables and file-level state.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5a VARIABLES — Static Variables [VARIABLES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_vars S.5a VARIABLES — Static Variables
 * @ingroup keyboard_file
 * @brief    File-scope variable declarations.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @brief Module initialization flag. */
static bool g_initialized = false;

/* MillenniumOS: No termios - we use PS/2 hardware directly via hal.c */

/** @brief Per-key pressed state tracking. */
static bool g_key_states[KEY_COUNT];

/** @brief Previous frame key states (for edge detection). */
static bool g_prev_key_states[KEY_COUNT];

/** @brief Current modifier key state (SHIFT, CTRL, ALT). */
static uint8_t g_modifiers = KEYMOD_NONE;

/** @brief Last character code for KEY_CHAR events. */
static uint32_t g_last_char = 0;

/** @brief Key repeat configuration. */
static KeyRepeatConfig g_repeat_config = {
    .delay_ms = KEY_REPEAT_DELAY_DEFAULT,
    .rate_ms  = KEY_REPEAT_RATE_DEFAULT,
    .enabled  = true
};

/** @brief Dead key compose state. */
static DeadKeyState g_dead_key_state = {
    .pending = {KEY_NONE},
    .count   = 0,
    .active  = false
};

/** @brief Pending event from GUI injection (for keyboard_poll). */
static KeyState g_pending_event = {0};

/** @brief Flag indicating a pending event is queued. */
static bool g_has_pending_event = false;

/** @} */ /* end S.5a VARIABLES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5b SCANCODE TABLES — Hardware Scancode Mappings [SCANCODE_TABLES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_scancode_tables S.5b SCANCODE TABLES — Hardware Mappings
 * @ingroup keyboard_file
 * @brief    Evdev-style scancode to KeyCode mappings.
 *
 * Linux evdev scancodes are used as the reference. These allow proper
 * scancode handling when using evdev or SDL2 backends.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Evdev scancode to KeyCode mapping table.
 *
 * Reference: linux/input-event-codes.h
 * Common scancodes for standard 104-key US layout.
 */
static const ScancodeEntry g_evdev_scancode_table[] = {
    /* Row 1: Escape and Function Keys */
    {  1, KEY_ESCAPE },
    { 59, KEY_F1  }, { 60, KEY_F2  }, { 61, KEY_F3  }, { 62, KEY_F4  },
    { 63, KEY_F5  }, { 64, KEY_F6  }, { 65, KEY_F7  }, { 66, KEY_F8  },
    { 67, KEY_F9  }, { 68, KEY_F10 }, { 87, KEY_F11 }, { 88, KEY_F12 },

    /* Row 2: Number Row */
    { 41, KEY_GRAVE  },
    { 2,  KEY_1 }, { 3,  KEY_2 }, { 4,  KEY_3 }, { 5,  KEY_4 },
    { 6,  KEY_5 }, { 7,  KEY_6 }, { 8,  KEY_7 }, { 9,  KEY_8 },
    { 10, KEY_9 }, { 11, KEY_0 },
    { 12, KEY_MINUS  }, { 13, KEY_EQUALS },
    { 14, KEY_BACKSPACE },

    /* Row 3: QWERTY Row */
    { 15, KEY_TAB },
    { 16, KEY_Q }, { 17, KEY_W }, { 18, KEY_E }, { 19, KEY_R },
    { 20, KEY_T }, { 21, KEY_Y }, { 22, KEY_U }, { 23, KEY_I },
    { 24, KEY_O }, { 25, KEY_P },
    { 26, KEY_LBRACKET }, { 27, KEY_RBRACKET }, { 43, KEY_BACKSLASH },

    /* Row 4: ASDF Row */
    { 58, KEY_CAPSLOCK },
    { 30, KEY_A }, { 31, KEY_S }, { 32, KEY_D }, { 33, KEY_F },
    { 34, KEY_G }, { 35, KEY_H }, { 36, KEY_J }, { 37, KEY_K },
    { 38, KEY_L },
    { 39, KEY_SEMICOLON }, { 40, KEY_APOSTROPHE },
    { 28, KEY_ENTER },

    /* Row 5: ZXCV Row */
    { 42, KEY_LSHIFT },
    { 44, KEY_Z }, { 45, KEY_X }, { 46, KEY_C }, { 47, KEY_V },
    { 48, KEY_B }, { 49, KEY_N }, { 50, KEY_M },
    { 51, KEY_COMMA }, { 52, KEY_PERIOD }, { 53, KEY_SLASH },
    { 54, KEY_RSHIFT },

    /* Row 6: Bottom Row */
    { 29, KEY_LCTRL  }, { 125, KEY_LSUPER }, { 56, KEY_LALT },
    { 57, KEY_SPACE },
    { 100, KEY_RALT }, { 126, KEY_RSUPER }, { 127, KEY_MENU }, { 97, KEY_RCTRL },

    /* Navigation Cluster */
    { 110, KEY_INSERT }, { 102, KEY_HOME  }, { 104, KEY_PAGEUP   },
    { 111, KEY_DELETE }, { 107, KEY_END   }, { 109, KEY_PAGEDOWN },

    /* Arrow Keys */
    { 103, KEY_UP }, { 105, KEY_LEFT }, { 108, KEY_DOWN }, { 106, KEY_RIGHT },

    /* Numpad */
    { 69, KEY_NUMLOCK },
    { 98, KEY_NUMPAD_DIVIDE }, { 55, KEY_NUMPAD_MULTIPLY }, { 74, KEY_NUMPAD_MINUS },
    { 71, KEY_NUMPAD_7 }, { 72, KEY_NUMPAD_8 }, { 73, KEY_NUMPAD_9 }, { 78, KEY_NUMPAD_PLUS },
    { 75, KEY_NUMPAD_4 }, { 76, KEY_NUMPAD_5 }, { 77, KEY_NUMPAD_6 },
    { 79, KEY_NUMPAD_1 }, { 80, KEY_NUMPAD_2 }, { 81, KEY_NUMPAD_3 }, { 96, KEY_NUMPAD_ENTER },
    { 82, KEY_NUMPAD_0 }, { 83, KEY_NUMPAD_DECIMAL },

    /* Terminator */
    { 0, KEY_NONE }
};

/**
 * @brief Number of entries in evdev scancode table.
 */
static const size_t g_evdev_scancode_count =
    (sizeof(g_evdev_scancode_table) / sizeof(g_evdev_scancode_table[0])) - 1;

/** @} */ /* end S.5b SCANCODE TABLES */

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_omission S.6 OMISSION — Reserved Section Guide
 * @brief    [Reserved: All SETUP sections populated]
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/* [Reserved: S.6 is documentation-only — no code content] */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief The actual work — implementations.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Ladder Hierarchy [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_orgchart B.1 ORG CHART — Ladder Hierarchy
 * @brief    Map internal structure — ladder hierarchy, baton flow.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par B.1a STRUCTURE — Function Call Hierarchy
 * @code
 * Public APIs (Top Rungs - Orchestration)
 * ├── keyboard_init()                  → enable_raw_mode() (no-op in kernel)
 * ├── keyboard_shutdown()              → disable_raw_mode() (no-op in kernel)
 * ├── keyboard_poll()                  → poll_hal_key_event() → HAL event queue
 * ├── keyboard_wait_key()              → mos_hal_wait_event() → HLT wait
 * ├── keyboard_flush()                 → mos_hal_poll_event() drain
 * ├── keyboard_is_pressed()            → g_key_states lookup
 * ├── keyboard_get_modifiers()         → g_modifiers
 * ├── keyboard_get_char()              → g_last_char
 * ├── keyboard_get_key_name()          → static lookup table
 * ├── keyboard_scancode_to_keycode()   → lookup_scancode_evdev()
 * ├── keyboard_keycode_to_scancode()   → reverse mapping
 * ├── keyboard_set_repeat_config()     → g_repeat_config
 * ├── keyboard_get_repeat_config()     → g_repeat_config
 * └── keyboard_compose_key()           → process_dead_key()
 *
 * Internal Helpers (Bottom Rungs - Foundations)
 * ├── enable_raw_mode()                → no-op (PS/2 driver in hal.c)
 * ├── disable_raw_mode()               → no-op (PS/2 driver stays active)
 * ├── poll_hal_key_event()             → mos_hal_poll_event()
 * ├── parse_escape_sequence()          → ANSI CSI/SS3 decode (terminal compat)
 * ├── parse_single_byte()              → ASCII → KeyCode
 * ├── lookup_scancode_evdev()          → g_evdev_scancode_table
 * └── process_dead_key()               → g_dead_key_state
 * @endcode
 *
 * @par B.1b COUNTS — APU Statistics
 * - Public APIs:      14
 * - Internal Helpers:  7
 * - Total Functions:  21
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Foundation Implementations [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_helpers B.2 HELPERS — Foundation Implementations
 * @brief    Internal static helper functions.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a TERMINAL — Terminal Mode Control [TERMINAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_terminal B.2a TERMINAL — Terminal Mode Control
 * @ingroup keyboard_helpers
 * @brief    Raw mode enable/disable for character-by-character input.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Enable keyboard input (kernel mode).
 *
 * MillenniumOS: In kernel mode, we use the PS/2 keyboard driver
 * from hal.c which is initialized via mos_hal_init_input().
 * This function is a no-op - the HAL handles hardware setup.
 *
 * @return true always (HAL handles initialization).
 */
static bool enable_raw_mode(void) {
    /* Kernel mode: PS/2 driver in hal.c handles hardware */
    return true;
}

/**
 * @brief Disable keyboard input (kernel mode).
 *
 * MillenniumOS: In kernel mode, keyboard hardware stays active.
 * This is a no-op - the HAL manages hardware lifetime.
 */
static void disable_raw_mode(void) {
    /* Kernel mode: PS/2 driver stays active */
}

/** @} */ /* end B.2a TERMINAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b INPUT — Input Reading [INPUT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_input B.2b INPUT — Input Reading
 * @ingroup keyboard_helpers
 * @brief    Read key sequences from terminal with escape handling.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Poll for key event from HAL (kernel mode).
 *
 * MillenniumOS: In kernel mode, we poll the HAL event queue which
 * is populated by the PS/2 keyboard interrupt handler.
 *
 * @param[out] event  Pointer to MosEvent to fill with key data.
 *
 * @return true if event available, false if no input.
 */
static bool poll_hal_key_event(MosEvent* event) {
    if (!event) return false;

    /* Poll HAL event queue - returns key events from PS/2 driver */
    while (mos_hal_poll_event(event)) {
        /* Filter for keyboard events only */
        if (event->type == MOS_EVENT_KEY) {
            return true;
        }
        /* Discard non-keyboard events (mouse, etc.) */
    }

    return false;
}

/** @} */ /* end B.2b INPUT */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2c PARSING — Key Parsing [PARSING]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_parsing B.2c PARSING — Key Parsing
 * @ingroup keyboard_helpers
 * @brief    Decode bytes to KeyCode values.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Parse ANSI escape sequence to KeyCode.
 *
 * Supports:
 * - CSI sequences: ESC [ A (arrows), ESC [ n ~ (function keys)
 * - SS3 sequences: ESC O P (F1-F4)
 *
 * @param[in] buf  Escape sequence bytes (starting with ESC).
 * @param[in] len  Number of bytes in sequence.
 *
 * @return Decoded KeyCode, or KEY_ESCAPE for unknown sequences.
 */
__attribute__((unused))
static KeyCode parse_escape_sequence(const uint8_t* buf, size_t len) {
    if (len < 2) {
        return KEY_ESCAPE;  /* Standalone escape */
    }

    /* CSI sequences: ESC [ ... */
    if (buf[1] == '[') {
        if (len == 3) {
            switch (buf[2]) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
                case 'H': return KEY_HOME;
                case 'F': return KEY_END;
            }
        }
        /* Extended sequences: ESC [ n ~ */
        if (len >= 4 && buf[len - 1] == '~') {
            int num = 0;
            for (size_t i = 2; i < len - 1 && buf[i] >= '0' && buf[i] <= '9'; i++) {
                num = num * 10 + (buf[i] - '0');
            }
            switch (num) {
                case 1:  return KEY_HOME;
                case 2:  return KEY_INSERT;
                case 3:  return KEY_DELETE;
                case 4:  return KEY_END;
                case 5:  return KEY_PAGEUP;
                case 6:  return KEY_PAGEDOWN;
                case 15: return KEY_F5;
                case 17: return KEY_F6;
                case 18: return KEY_F7;
                case 19: return KEY_F8;
                case 20: return KEY_F9;
                case 21: return KEY_F10;
                case 23: return KEY_F11;
                case 24: return KEY_F12;
            }
        }
    }

    /* SS3 sequences: ESC O ... */
    if (buf[1] == 'O' && len == 3) {
        switch (buf[2]) {
            case 'P': return KEY_F1;
            case 'Q': return KEY_F2;
            case 'R': return KEY_F3;
            case 'S': return KEY_F4;
            case 'H': return KEY_HOME;
            case 'F': return KEY_END;
        }
    }

    return KEY_ESCAPE;  /* Unknown sequence */
}

/**
 * @brief Parse single byte to KeyCode.
 *
 * Maps ASCII and control characters to KeyCode values.
 * Updates g_modifiers for shift detection.
 *
 * @param[in] byte  Single input byte.
 *
 * @return Decoded KeyCode.
 */
static KeyCode parse_single_byte(uint8_t byte) {
    /* Control characters */
    if (byte == 0x00) return KEY_NONE;
    if (byte == 0x09) return KEY_TAB;
    if (byte == 0x0A || byte == 0x0D) return KEY_ENTER;
    if (byte == 0x1B) return KEY_ESCAPE;
    if (byte == 0x7F || byte == 0x08) return KEY_BACKSPACE;

    /* Space */
    if (byte == ' ') return KEY_SPACE;

    /* Numbers 0-9 */
    if (byte >= '0' && byte <= '9') {
        return (KeyCode)(KEY_0 + (byte - '0'));
    }

    /* Letters A-Z (uppercase or lowercase) */
    if (byte >= 'a' && byte <= 'z') {
        return (KeyCode)(KEY_A + (byte - 'a'));
    }
    if (byte >= 'A' && byte <= 'Z') {
        g_modifiers |= KEYMOD_SHIFT;
        return (KeyCode)(KEY_A + (byte - 'A'));
    }

    /* Punctuation */
    switch (byte) {
        case '-': return KEY_MINUS;
        case '=': return KEY_EQUALS;
        case '[': return KEY_LBRACKET;
        case ']': return KEY_RBRACKET;
        case '\\': return KEY_BACKSLASH;
        case ';': return KEY_SEMICOLON;
        case '\'': return KEY_APOSTROPHE;
        case '`': return KEY_GRAVE;
        case ',': return KEY_COMMA;
        case '.': return KEY_PERIOD;
        case '/': return KEY_SLASH;
    }

    /* Printable character */
    if (byte >= 32 && byte < 127) {
        g_last_char = byte;
        return KEY_CHAR;
    }

    return KEY_NONE;
}

/** @} */ /* end B.2c PARSING */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2d SCANCODE — Scancode Lookup [SCANCODE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_scancode B.2d SCANCODE — Scancode Lookup
 * @ingroup keyboard_helpers
 * @brief    Hardware scancode to KeyCode mapping.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Lookup KeyCode from evdev-style scancode table.
 *
 * Performs linear search through scancode table. Table is small enough
 * that binary search overhead isn't worth it.
 *
 * @param[in] scancode  Hardware scancode (evdev).
 *
 * @return Mapped KeyCode, or KEY_NONE if not found.
 */
static KeyCode lookup_scancode_evdev(uint16_t scancode) {
    for (size_t i = 0; i < g_evdev_scancode_count; i++) {
        if (g_evdev_scancode_table[i].scancode == scancode) {
            return g_evdev_scancode_table[i].keycode;
        }
    }
    return KEY_NONE;
}

/** @} */ /* end B.2d SCANCODE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2e COMPOSE — Dead Key Composition [COMPOSE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_compose B.2e COMPOSE — Dead Key Composition
 * @ingroup keyboard_helpers
 * @brief    Dead key / compose sequence handling.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Process dead key compose sequence.
 *
 * Dead keys are keys that don't produce output immediately but modify
 * the next key pressed. Examples:
 * - ` + e = è (grave accent)
 * - ' + e = é (acute accent)
 * - ^ + a = â (circumflex)
 *
 * @param[in] code  Input KeyCode to compose.
 *
 * @return Composed KeyCode, or KEY_NONE if composition pending/failed.
 *
 * @note Current implementation is a framework stub. Full compose
 *       sequence database will be added in future version.
 */
static KeyCode process_dead_key(KeyCode code) {
    /* Check if this is a dead key (accent keys in US-International layout) */
    bool is_dead_key = false;
    switch (code) {
        case KEY_GRAVE:      /* ` - grave accent */
        case KEY_APOSTROPHE: /* ' - acute accent */
            is_dead_key = true;
            break;
        default:
            break;
    }

    if (is_dead_key && !g_dead_key_state.active) {
        /* Start compose sequence */
        g_dead_key_state.active = true;
        g_dead_key_state.pending[0] = code;
        g_dead_key_state.count = 1;
        return KEY_NONE;  /* Consume the dead key */
    }

    if (g_dead_key_state.active && g_dead_key_state.count > 0) {
        /* Compose with pending dead key */
        KeyCode dead = g_dead_key_state.pending[0];

        /* Reset compose state */
        g_dead_key_state.active = false;
        g_dead_key_state.count = 0;

        /* Basic compose lookup (framework — extend as needed) */
        if (dead == KEY_GRAVE) {
            /* Grave accent compositions would set g_last_char to composed char */
            /* For now, just return the base key */
            (void)dead;
        }

        return code;  /* Return base key for now */
    }

    return code;  /* No composition */
}

/** @} */ /* end B.2e COMPOSE */

/** @} */ /* end B.2 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Business Logic Implementations [OPERATIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_coreops B.3 CORE OPS — Business Logic Implementations
 * @brief    Public API implementations.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a LIFECYCLE — Creation/Destruction [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_lifecycle B.3a LIFECYCLE — Creation/Destruction
 * @ingroup keyboard_coreops
 * @brief    Initialization and shutdown.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize the keyboard input subsystem.
 *
 * MillenniumOS: Initializes keyboard state. The PS/2 driver is
 * initialized by mos_hal_init_input() in hal.c.
 * Safe to call multiple times (idempotent).
 *
 * @return KEYBOARD_OK on success, KEYBOARD_ERR_INIT on failure.
 *
 * @post g_initialized == true.
 */
KeyboardError keyboard_init(void) {
    if (g_initialized) {
        return KEYBOARD_OK;  /* Already initialized */
    }

    /* Clear state (freestanding - zero each element) */
    for (size_t i = 0; i < KEY_COUNT; i++) {
        g_key_states[i] = false;
        g_prev_key_states[i] = false;
    }
    g_modifiers = KEYMOD_NONE;
    g_last_char = 0;

    /* Reset repeat config to defaults */
    g_repeat_config.delay_ms = KEY_REPEAT_DELAY_DEFAULT;
    g_repeat_config.rate_ms  = KEY_REPEAT_RATE_DEFAULT;
    g_repeat_config.enabled  = true;

    /* Reset dead key state */
    g_dead_key_state.active = false;
    g_dead_key_state.count  = 0;

    /* Kernel mode: PS/2 driver in hal.c handles hardware via mos_hal_init_input() */
    if (!enable_raw_mode()) {
        return KEYBOARD_ERR_INIT;
    }

    g_initialized = true;
    return KEYBOARD_OK;
}

/**
 * @brief Shutdown the keyboard input subsystem.
 *
 * Restores terminal to original settings.
 * Safe to call multiple times (idempotent).
 *
 * @post Terminal restored, g_initialized == false.
 */
void keyboard_shutdown(void) {
    if (!g_initialized) {
        return;
    }

    disable_raw_mode();
    g_initialized = false;
}

/**
 * @brief Update keyboard state for new frame.
 *
 * Copies current key states to previous for edge detection.
 * Must be called at start of each frame before polling.
 */
void keyboard_update(void) {
    for (size_t i = 0; i < KEY_COUNT; i++) {
        g_prev_key_states[i] = g_key_states[i];
    }
}

/** @} */ /* end B.3a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3b ACCESSORS — State Queries [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_accessors B.3b ACCESSORS — State Queries
 * @ingroup keyboard_coreops
 * @brief    Query keyboard state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Check if a specific key is currently pressed.
 *
 * @param[in] code  KeyCode to check.
 *
 * @return 1 if pressed, 0 if not pressed or invalid code.
 */
int keyboard_is_pressed(KeyCode code) {
    if (code >= KEY_COUNT) {
        return 0;
    }
    return g_key_states[code] ? 1 : 0;
}

/**
 * @brief Check if key was just pressed this frame.
 *
 * @param[in] code  KeyCode to check.
 *
 * @return 1 if just pressed (transition from released to pressed), 0 otherwise.
 */
int keyboard_is_just_pressed(KeyCode code) {
    if (code >= KEY_COUNT) {
        return 0;
    }
    return (g_key_states[code] && !g_prev_key_states[code]) ? 1 : 0;
}

/**
 * @brief Check if key was just released this frame.
 *
 * @param[in] code  KeyCode to check.
 *
 * @return 1 if just released (transition from pressed to released), 0 otherwise.
 */
int keyboard_is_just_released(KeyCode code) {
    if (code >= KEY_COUNT) {
        return 0;
    }
    return (!g_key_states[code] && g_prev_key_states[code]) ? 1 : 0;
}

/**
 * @brief Get current modifier key state.
 *
 * @return Bitmask of KEYMOD_* flags (SHIFT, CTRL, ALT).
 */
uint8_t keyboard_get_modifiers(void) {
    return g_modifiers;
}

/**
 * @brief Get character for the last KEY_CHAR event.
 *
 * @return Unicode codepoint (ASCII for terminal mode).
 */
uint32_t keyboard_get_char(void) {
    return g_last_char;
}

/**
 * @brief Convert KeyCode to human-readable name.
 *
 * @param[in] code  KeyCode to convert.
 *
 * @return Static string with key name, or "KEY_UNKNOWN".
 */
const char* keyboard_get_key_name(KeyCode code) {
    static const char* names[] = {
        [KEY_NONE] = "KEY_NONE",
        [KEY_ENTER] = "KEY_ENTER",
        [KEY_ESCAPE] = "KEY_ESCAPE",
        [KEY_TAB] = "KEY_TAB",
        [KEY_BACKSPACE] = "KEY_BACKSPACE",
        [KEY_DELETE] = "KEY_DELETE",
        [KEY_INSERT] = "KEY_INSERT",
        [KEY_HOME] = "KEY_HOME",
        [KEY_END] = "KEY_END",
        [KEY_PAGEUP] = "KEY_PAGEUP",
        [KEY_PAGEDOWN] = "KEY_PAGEDOWN",
        [KEY_UP] = "KEY_UP",
        [KEY_DOWN] = "KEY_DOWN",
        [KEY_LEFT] = "KEY_LEFT",
        [KEY_RIGHT] = "KEY_RIGHT",
        [KEY_SPACE] = "KEY_SPACE",
        [KEY_0] = "KEY_0", [KEY_1] = "KEY_1", [KEY_2] = "KEY_2",
        [KEY_3] = "KEY_3", [KEY_4] = "KEY_4", [KEY_5] = "KEY_5",
        [KEY_6] = "KEY_6", [KEY_7] = "KEY_7", [KEY_8] = "KEY_8",
        [KEY_9] = "KEY_9",
        [KEY_A] = "KEY_A", [KEY_B] = "KEY_B", [KEY_C] = "KEY_C",
        [KEY_D] = "KEY_D", [KEY_E] = "KEY_E", [KEY_F] = "KEY_F",
        [KEY_G] = "KEY_G", [KEY_H] = "KEY_H", [KEY_I] = "KEY_I",
        [KEY_J] = "KEY_J", [KEY_K] = "KEY_K", [KEY_L] = "KEY_L",
        [KEY_M] = "KEY_M", [KEY_N] = "KEY_N", [KEY_O] = "KEY_O",
        [KEY_P] = "KEY_P", [KEY_Q] = "KEY_Q", [KEY_R] = "KEY_R",
        [KEY_S] = "KEY_S", [KEY_T] = "KEY_T", [KEY_U] = "KEY_U",
        [KEY_V] = "KEY_V", [KEY_W] = "KEY_W", [KEY_X] = "KEY_X",
        [KEY_Y] = "KEY_Y", [KEY_Z] = "KEY_Z",
        [KEY_F1] = "KEY_F1", [KEY_F2] = "KEY_F2", [KEY_F3] = "KEY_F3",
        [KEY_F4] = "KEY_F4", [KEY_F5] = "KEY_F5", [KEY_F6] = "KEY_F6",
        [KEY_F7] = "KEY_F7", [KEY_F8] = "KEY_F8", [KEY_F9] = "KEY_F9",
        [KEY_F10] = "KEY_F10", [KEY_F11] = "KEY_F11", [KEY_F12] = "KEY_F12",
        [KEY_MINUS] = "KEY_MINUS", [KEY_EQUALS] = "KEY_EQUALS",
        [KEY_LBRACKET] = "KEY_LBRACKET", [KEY_RBRACKET] = "KEY_RBRACKET",
        [KEY_BACKSLASH] = "KEY_BACKSLASH", [KEY_SEMICOLON] = "KEY_SEMICOLON",
        [KEY_APOSTROPHE] = "KEY_APOSTROPHE", [KEY_GRAVE] = "KEY_GRAVE",
        [KEY_COMMA] = "KEY_COMMA", [KEY_PERIOD] = "KEY_PERIOD",
        [KEY_SLASH] = "KEY_SLASH",
        [KEY_CAPSLOCK] = "KEY_CAPSLOCK", [KEY_NUMLOCK] = "KEY_NUMLOCK",
        [KEY_SCROLLLOCK] = "KEY_SCROLLLOCK",
        [KEY_LSHIFT] = "KEY_LSHIFT", [KEY_RSHIFT] = "KEY_RSHIFT",
        [KEY_LCTRL] = "KEY_LCTRL", [KEY_RCTRL] = "KEY_RCTRL",
        [KEY_LALT] = "KEY_LALT", [KEY_RALT] = "KEY_RALT",
        [KEY_LSUPER] = "KEY_LSUPER", [KEY_RSUPER] = "KEY_RSUPER",
        [KEY_MENU] = "KEY_MENU",
        [KEY_CHAR] = "KEY_CHAR",
    };

    if (code < KEY_COUNT && code < sizeof(names)/sizeof(names[0]) && names[code]) {
        return names[code];
    }
    return "KEY_UNKNOWN";
}

/**
 * @brief Map hardware scancode to KeyCode.
 *
 * Uses evdev-style scancode table for mapping.
 *
 * @param[in] scancode  Hardware scancode (evdev).
 *
 * @return Corresponding KeyCode, or KEY_NONE.
 */
KeyCode keyboard_scancode_to_keycode(uint16_t scancode) {
    /* First try evdev table lookup */
    KeyCode code = lookup_scancode_evdev(scancode);
    if (code != KEY_NONE) {
        return code;
    }

    /* Fallback to ASCII parsing for terminal mode */
    if (scancode < 256) {
        return parse_single_byte((uint8_t)scancode);
    }

    return KEY_NONE;
}

/**
 * @brief Map KeyCode to hardware scancode.
 *
 * Reverse lookup in evdev scancode table.
 *
 * @param[in] code  KeyCode to convert.
 *
 * @return Hardware scancode (evdev), or 0 if no mapping.
 */
uint16_t keyboard_keycode_to_scancode(KeyCode code) {
    /* Search evdev table for reverse mapping */
    for (size_t i = 0; i < g_evdev_scancode_count; i++) {
        if (g_evdev_scancode_table[i].keycode == code) {
            return g_evdev_scancode_table[i].scancode;
        }
    }

    /* Fallback for basic ASCII keys */
    if (code >= KEY_A && code <= KEY_Z) {
        return (uint16_t)('a' + (code - KEY_A));
    }
    if (code >= KEY_0 && code <= KEY_9) {
        return (uint16_t)('0' + (code - KEY_0));
    }
    if (code == KEY_SPACE) return ' ';
    if (code == KEY_ENTER) return '\n';
    if (code == KEY_TAB) return '\t';
    if (code == KEY_ESCAPE) return 0x1B;
    if (code == KEY_BACKSPACE) return 0x7F;

    return 0;  /* No mapping */
}

/** @} */ /* end B.3b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3c INPUT OPS — Input Operations [INPUT_OPS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_input_ops B.3c INPUT OPS — Input Operations
 * @ingroup keyboard_coreops
 * @brief    Poll and wait for keyboard events.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Poll for keyboard events (non-blocking).
 *
 * MillenniumOS: Polls the HAL event queue which is populated by
 * the PS/2 keyboard interrupt handler in hal.c.
 *
 * @param[out] state  Pointer to KeyState to fill.
 *
 * @return 1 if event available, 0 if no input.
 *
 * @pre  keyboard_init() called successfully.
 * @post state filled with key event data if return is 1.
 */
int keyboard_poll(KeyState* state) {
    if (!g_initialized || state == NULL) {
        return 0;
    }

    /* Check for injected events first */
    if (g_has_pending_event) {
        *state = g_pending_event;
        g_has_pending_event = false;
        return 1;
    }

    /* Poll HAL event queue for keyboard events */
    MosEvent event;
    if (!poll_hal_key_event(&event)) {
        return 0;  /* No input */
    }

    /* Convert HAL MosKeyEvent to keyboard KeyState */
    MosKeyEvent* key = &event.key;

    /* Map HAL keycode to our KeyCode enum via scancode */
    KeyCode code = lookup_scancode_evdev(key->scancode);
    if (code == KEY_NONE && key->keycode < 128) {
        /* Fallback: use ASCII character as keycode */
        code = parse_single_byte((uint8_t)key->keycode);
    }

    /* Update modifier tracking from HAL modifiers */
    g_modifiers = 0;
    if (key->modifiers & MOS_MOD_SHIFT) g_modifiers |= KEYMOD_SHIFT;
    if (key->modifiers & MOS_MOD_CTRL)  g_modifiers |= KEYMOD_CTRL;
    if (key->modifiers & MOS_MOD_ALT)   g_modifiers |= KEYMOD_ALT;

    /* Store character if printable */
    if (key->character >= 32 && key->character < 127) {
        g_last_char = (uint32_t)key->character;
    }

    /* Process dead key composition if enabled */
    code = process_dead_key(code);
    if (code == KEY_NONE) {
        /* Dead key consumed, no event to report yet */
        return 0;
    }

    /* Fill state */
    state->code = code;
    state->scancode = key->scancode;
    state->modifiers = g_modifiers;
    state->pressed = (key->state == MOS_KEY_DOWN) ? 1 : 0;
    state->repeat_count = 0;

    /* Update key state tracking */
    if (code < KEY_COUNT) {
        g_key_states[code] = (key->state == MOS_KEY_DOWN);
    }

    return 1;
}

/**
 * @brief Wait for keyboard event (blocking).
 *
 * MillenniumOS: Uses mos_hal_wait_event() which blocks using HLT
 * until an interrupt (including keyboard) fires.
 *
 * @param[out] state  Pointer to KeyState to fill.
 *
 * @return KEYBOARD_OK on success, error code on failure.
 *
 * @pre  keyboard_init() called successfully.
 * @post state filled with key event data.
 */
KeyboardError keyboard_wait_key(KeyState* state) {
    if (!g_initialized) {
        return KEYBOARD_ERR_NOT_INIT;
    }
    if (state == NULL) {
        return KEYBOARD_ERR_INVALID;
    }

    /* Block until keyboard event available */
    MosEvent event;
    while (1) {
        /* mos_hal_wait_event uses HLT to wait for interrupts */
        if (mos_hal_wait_event(&event) != MOS_OK) {
            return KEYBOARD_ERR_INIT;
        }

        /* Filter for keyboard events */
        if (event.type == MOS_EVENT_KEY) {
            break;
        }
        /* Continue waiting if not a keyboard event */
    }

    /* Convert HAL event to keyboard state */
    MosKeyEvent* key = &event.key;

    KeyCode code = lookup_scancode_evdev(key->scancode);
    if (code == KEY_NONE && key->keycode < 128) {
        code = parse_single_byte((uint8_t)key->keycode);
    }

    g_modifiers = 0;
    if (key->modifiers & MOS_MOD_SHIFT) g_modifiers |= KEYMOD_SHIFT;
    if (key->modifiers & MOS_MOD_CTRL)  g_modifiers |= KEYMOD_CTRL;
    if (key->modifiers & MOS_MOD_ALT)   g_modifiers |= KEYMOD_ALT;

    if (key->character >= 32 && key->character < 127) {
        g_last_char = (uint32_t)key->character;
    }

    state->code = code;
    state->scancode = key->scancode;
    state->modifiers = g_modifiers;
    state->pressed = (key->state == MOS_KEY_DOWN) ? 1 : 0;
    state->repeat_count = 0;

    if (code < KEY_COUNT) {
        g_key_states[code] = (key->state == MOS_KEY_DOWN);
    }

    return KEYBOARD_OK;
}

/**
 * @brief Clear all pending keyboard events.
 *
 * MillenniumOS: Drains the HAL event queue and resets key state tracking.
 *
 * @pre  keyboard_init() called successfully.
 * @post Input buffer empty, all key states cleared.
 */
void keyboard_flush(void) {
    if (!g_initialized) return;

    /* Drain HAL event queue */
    MosEvent event;
    while (mos_hal_poll_event(&event)) {
        /* Discard */
    }

    /* Clear state (freestanding loop) */
    for (size_t i = 0; i < KEY_COUNT; i++) {
        g_key_states[i] = false;
    }
    g_modifiers = KEYMOD_NONE;
    g_last_char = 0;

    /* Reset dead key compose state */
    g_dead_key_state.active = false;
    g_dead_key_state.count = 0;
}

/**
 * @brief Clear all key states.
 *
 * Resets all key states to "not pressed" without flushing input buffer.
 *
 * @pre  keyboard_init() called successfully.
 * @post All key states cleared (not pressed).
 */
void keyboard_clear_states(void) {
    if (!g_initialized) return;

    for (size_t i = 0; i < KEY_COUNT; i++) {
        g_key_states[i] = false;
    }
}

/** @} */ /* end B.3c INPUT OPS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3d CONFIGURATION — Key Repeat and Compose [CONFIG]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_config B.3d CONFIGURATION — Key Repeat and Compose
 * @ingroup keyboard_coreops
 * @brief    Configure key repeat and dead key behavior.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Set key repeat configuration.
 *
 * Configures auto-repeat behavior for held keys.
 *
 * @param[in] delay_ms  Delay before first repeat (ms). Clamped to valid range.
 * @param[in] rate_ms   Time between repeats (ms). Clamped to valid range.
 * @param[in] enabled   Enable/disable key repeat.
 *
 * @return KEYBOARD_OK on success, KEYBOARD_ERR_NOT_INIT if not initialized.
 *
 * @note Terminal mode doesn't support true key repeat detection.
 *       This configuration is stored for future GUI mode integration.
 */
KeyboardError keyboard_set_repeat_config(uint16_t delay_ms, uint16_t rate_ms, bool enabled) {
    if (!g_initialized) {
        return KEYBOARD_ERR_NOT_INIT;
    }

    /* Clamp values to valid ranges */
    if (delay_ms < KEY_REPEAT_DELAY_MIN) delay_ms = KEY_REPEAT_DELAY_MIN;
    if (delay_ms > KEY_REPEAT_DELAY_MAX) delay_ms = KEY_REPEAT_DELAY_MAX;
    if (rate_ms < KEY_REPEAT_RATE_MIN) rate_ms = KEY_REPEAT_RATE_MIN;
    if (rate_ms > KEY_REPEAT_RATE_MAX) rate_ms = KEY_REPEAT_RATE_MAX;

    g_repeat_config.delay_ms = delay_ms;
    g_repeat_config.rate_ms  = rate_ms;
    g_repeat_config.enabled  = enabled;

    return KEYBOARD_OK;
}

/**
 * @brief Get current key repeat configuration.
 *
 * @param[out] delay_ms  Receives current delay setting.
 * @param[out] rate_ms   Receives current rate setting.
 * @param[out] enabled   Receives current enabled state.
 *
 * @return KEYBOARD_OK on success, KEYBOARD_ERR_NOT_INIT if not initialized.
 */
KeyboardError keyboard_get_repeat_config(uint16_t* delay_ms, uint16_t* rate_ms, bool* enabled) {
    if (!g_initialized) {
        return KEYBOARD_ERR_NOT_INIT;
    }

    if (delay_ms) *delay_ms = g_repeat_config.delay_ms;
    if (rate_ms)  *rate_ms  = g_repeat_config.rate_ms;
    if (enabled)  *enabled  = g_repeat_config.enabled;

    return KEYBOARD_OK;
}

/**
 * @brief Process compose sequence for a key.
 *
 * Allows manual invocation of dead key composition.
 *
 * @param[in] code  KeyCode to compose.
 *
 * @return Composed KeyCode, or KEY_NONE if composition is pending.
 */
KeyCode keyboard_compose_key(KeyCode code) {
    return process_dead_key(code);
}

/**
 * @brief Cancel active compose sequence.
 *
 * Clears any pending dead key state.
 */
void keyboard_cancel_compose(void) {
    g_dead_key_state.active = false;
    g_dead_key_state.count = 0;
}

/** @} */ /* end B.3d CONFIGURATION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3e INJECTION — GUI Backend Support [INJECTION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keyboard_injection_impl B.3e INJECTION — GUI Backend Support
 * @ingroup keyboard_coreops
 * @brief    Functions for GUI backends (SDL2, GLFW) to inject keyboard events.
 *
 * These functions allow display backends to feed keyboard events into the
 * kernel input layer. Call from SDL_KEYDOWN/SDL_KEYUP or GLFW key callbacks.
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
 */
void keyboard_inject_key(KeyCode code, int pressed) {
    if (!g_initialized || code >= KEY_COUNT) {
        return;
    }

    g_key_states[code] = (pressed != 0);

    /* Update modifier flags based on key */
    switch (code) {
        case KEY_LSHIFT:
        case KEY_RSHIFT:
            if (pressed) g_modifiers |= KEYMOD_SHIFT;
            else if (!g_key_states[KEY_LSHIFT] && !g_key_states[KEY_RSHIFT])
                g_modifiers &= ~KEYMOD_SHIFT;
            break;
        case KEY_LCTRL:
        case KEY_RCTRL:
            if (pressed) g_modifiers |= KEYMOD_CTRL;
            else if (!g_key_states[KEY_LCTRL] && !g_key_states[KEY_RCTRL])
                g_modifiers &= ~KEYMOD_CTRL;
            break;
        case KEY_LALT:
        case KEY_RALT:
            if (pressed) g_modifiers |= KEYMOD_ALT;
            else if (!g_key_states[KEY_LALT] && !g_key_states[KEY_RALT])
                g_modifiers &= ~KEYMOD_ALT;
            break;
        case KEY_LSUPER:
        case KEY_RSUPER:
            if (pressed) g_modifiers |= KEYMOD_SUPER;
            else if (!g_key_states[KEY_LSUPER] && !g_key_states[KEY_RSUPER])
                g_modifiers &= ~KEYMOD_SUPER;
            break;
        case KEY_CAPSLOCK:
            if (pressed) g_modifiers ^= KEYMOD_CAPS;  /* Toggle on press */
            break;
        case KEY_NUMLOCK:
            if (pressed) g_modifiers ^= KEYMOD_NUM;   /* Toggle on press */
            break;
        default:
            break;
    }

    /* Queue a pending event */
    g_pending_event.code = code;
    g_pending_event.scancode = keyboard_keycode_to_scancode(code);
    g_pending_event.modifiers = g_modifiers;
    g_pending_event.pressed = (pressed != 0) ? 1 : 0;
    g_pending_event.repeat_count = 0;
    g_has_pending_event = true;
}

/**
 * @brief Inject a raw hardware scancode event.
 *
 * For platforms providing raw scancodes. The subsystem will translate
 * to KeyCode internally using the scancode table.
 *
 * @param[in] scancode  Hardware scancode (platform-specific).
 * @param[in] pressed   1 = key down, 0 = key up.
 */
void keyboard_inject_scancode(uint16_t scancode, int pressed) {
    KeyCode code = keyboard_scancode_to_keycode(scancode);
    if (code != KEY_NONE) {
        keyboard_inject_key(code, pressed);
        /* Override scancode with the original */
        g_pending_event.scancode = scancode;
    }
}

/**
 * @brief Inject modifier key state.
 *
 * Sets the current modifier state directly. Useful when modifier state
 * comes from the windowing system rather than individual key events.
 *
 * @param[in] modifiers  Bitmask of KeyModifier flags.
 */
void keyboard_inject_modifiers(uint8_t modifiers) {
    if (!g_initialized) {
        return;
    }
    g_modifiers = modifiers;
}

/**
 * @brief Inject a text input character.
 *
 * For text input events (SDL_TEXTINPUT, GLFW character callback).
 * Sets up a KEY_CHAR event that can be retrieved via keyboard_get_char().
 *
 * @param[in] codepoint  Unicode codepoint (UTF-32).
 */
void keyboard_inject_char(uint32_t codepoint) {
    if (!g_initialized) {
        return;
    }

    g_last_char = codepoint;

    /* Queue a KEY_CHAR event */
    g_pending_event.code = KEY_CHAR;
    g_pending_event.scancode = 0;
    g_pending_event.modifiers = g_modifiers;
    g_pending_event.pressed = 1;
    g_pending_event.repeat_count = 0;
    g_has_pending_event = true;
}

/**
 * @brief Inject a complete KeyState event.
 *
 * Full control injection for complex scenarios. Queues the event
 * to be returned by the next keyboard_poll() call.
 *
 * @param[in] state  Complete KeyState to inject. Must not be NULL.
 */
void keyboard_inject_event(const KeyState* state) {
    if (!g_initialized || state == NULL) {
        return;
    }

    /* Update internal state to match injected event */
    if (state->code < KEY_COUNT) {
        g_key_states[state->code] = state->pressed;
    }
    g_modifiers = state->modifiers;

    /* Queue the event */
    g_pending_event = *state;
    g_has_pending_event = true;
}

/** @} */ /* end B.3e INJECTION */

/** @} */ /* end B.3 CORE OPS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4-B.6 — [Collapsed: Standard patterns] [COLLAPSED]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_body_collapsed B.4-B.6 COLLAPSED — Standard Patterns
 * @brief    [Reserved: Error handling, public API, and omission sections]
 *
 * B.4 ERRORS:     Error handling via return codes (KeyboardError enum)
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
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build, runtime, and memory verification.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * gcc -c -Wall -Wextra -Werror -std=c11 keyboard.c
 * make -C engine/core
 * @endcode
 *
 * @par X.1b SELFTEST — Minimal Isolation Test
 * @code{.c}
 * #include "hal/input/device/keyboard.dev.h"
 * int main(void) {
 *     if (keyboard_init() != KEYBOARD_OK) return 1;
 *     KeyState state;
 *     printf("Press any key (ESC to exit)...\n");
 *     while (1) {
 *         if (keyboard_poll(&state) && state.code == KEY_ESCAPE) break;
 *     }
 *     keyboard_shutdown();
 *     return 0;
 * }
 * @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Entry Point and Flow [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_execution X.2 EXECUTION — Entry Point and Flow
 * @brief    [Reserved: Library — no main()]
 *
 * This is a library module, not an executable.
 * See keyboard.h X.7 REFERENCE for usage examples.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/* [Reserved: Library module — no entry point] */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership, Memory, Shutdown [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_cleanup X.3 CLEANUP — Ownership, Memory, Shutdown
 * @brief    Terminal restoration and state cleanup.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Ownership
 * - No dynamic allocation — all state is static
 * - No ownership transfer — all pointers are caller-owned
 *
 * @par Shutdown
 * keyboard_shutdown() MUST be called before program exit:
 * - Restores terminal to original cooked mode
 * - Failure leaves terminal in raw mode (unusable shell)
 *
 * @par Recovery
 * If terminal left in raw mode, run: `reset` or `stty sane`
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidance [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_policy X.4 POLICY — Modification Guidance
 * @brief    Guide future maintainers — what's safe to change.
 *
 * SAFE TO MODIFY:
 *   - Add entries to scancode table (g_evdev_scancode_table)
 *   - Extend dead key composition (process_dead_key)
 *   - Add new key names to keyboard_get_key_name
 *
 * MODIFY WITH CARE:
 *   - Escape sequence parsing (terminal compatibility)
 *   - Raw mode configuration (affects all terminal I/O)
 *
 * NEVER MODIFY:
 *   - Public API signatures (breaks calling code)
 *   - KeyboardError codes (breaks error handling)
 *   - Block structure (4-block pattern)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_extension X.5 EXTENSION — How to Expand
 * @brief    How to expand this component — mirrors M.10 Roadmap.
 *
 * ADDING SDL2 SUPPORT:
 *   1. Add SDL.h include in S.1c EXTERNAL
 *   2. Add SDL scancode table in S.5b
 *   3. Add lookup function in B.2d SCANCODE
 *   4. Update keyboard_scancode_to_keycode to use SDL table
 *
 * ADDING COMPOSE SEQUENCES:
 *   1. Expand compose lookup table in process_dead_key
 *   2. Map dead_key + base_key → composed_character
 *   3. Set g_last_char to composed Unicode codepoint
 *
 * ADDING KEY REPEAT:
 *   1. Track key press timestamps
 *   2. Generate synthetic repeat events after delay
 *   3. Use g_repeat_config for timing
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_troubleshoot X.6 TROUBLESHOOTING — Common Problems
 * @brief    Common problems and solutions for runtime issues.
 *
 * "No keyboard events":
 *   - mos_hal_init_input() not called
 *   - PS/2 keyboard IRQ not enabled (IRQ1)
 *   - Verify keyboard handler registered in hal.c
 *
 * "keyboard_init returns KEYBOARD_ERR_INIT":
 *   - HAL not initialized before keyboard_init()
 *   - Call mos_hal_init() and mos_hal_init_input() first
 *
 * "Keys not mapping correctly":
 *   - Scancode table may need PS/2 Set 1 codes
 *   - Check lookup_scancode_evdev() mappings
 *   - Verify g_evdev_scancode_table entries
 *
 * "Modifier keys not working":
 *   - HAL modifier tracking in keyboard_handler()
 *   - Check MOS_MOD_SHIFT/CTRL/ALT bit flags
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_reference X.7 REFERENCE — Quick Lookup
 * @brief    Copy-paste ready examples for common operations.
 *
 * @par Basic Input Loop
 * @code{.c}
 * keyboard_init();
 * KeyState state;
 * while (running) {
 *     if (keyboard_poll(&state)) {
 *         printf("Key: %s\n", keyboard_get_key_name(state.code));
 *         if (state.code == KEY_ESCAPE) break;
 *     }
 * }
 * keyboard_shutdown();
 * @endcode
 *
 * @par Blocking Wait
 * @code{.c}
 * KeyState state;
 * keyboard_wait_key(&state);  // Blocks until key pressed
 * @endcode
 *
 * @par Configure Key Repeat
 * @code{.c}
 * keyboard_set_repeat_config(300, 50, true);  // 300ms delay, 50ms rate
 * @endcode
 *
 * @par Compile
 * @code{.sh}
 * gcc -c -Wall -Wextra -std=c11 keyboard.c
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keyboard_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding — echoes M.8 Scripture.
 *
 * @par Scripture
 * "Thy word is a lamp unto my feet, and a light unto my path."
 *   — Psalm 119:105
 *
 * Input illuminates intent — keys reveal what user wants to do.
 * Swift to hear, responsive to act.
 *
 * @par Design Philosophy
 * Input is revelation. Each keypress is the user expressing intention.
 * Our job: faithful translation from hardware to meaning, without delay
 * or distortion.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════
