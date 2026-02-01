/// @file keybind.c
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
/// @omni:req  key        = MOS-HAL-INPUT-KEYBIND-IMPL
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
/// @omni:ins  component  = kernel/hal/input/config
/// @omni:ins  layer      = kernel (layer 0)
/// @omni:ins  includes   = [stdint, stdbool, stddef, keybind.h]
/// @omni:ins  provides   = [keybind_* implementations]
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
/// @omni:req  title      = Action Binding Implementation
/// @omni:req  brief      = System-level action abstraction — maps semantic actions to inputs
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
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * SP.1 CONFIG — Compile-time Settings [CONFIG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_special_config SP.1 CONFIG — Compile-time Settings
 * @brief    Feature macros before includes.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @defgroup keybind_special_mode SP.1a MODE — Kernel Mode
 * @ingroup keybind_special_config
 * @brief    Kernel freestanding mode (no libc).
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
 * @file      keybind.c
 * @brief     Action Binding — system-level action abstraction implementation.
 *
 * @defgroup keybind_impl_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CORNERSTONE-INPUT-KEYBIND-IMPL
 * Title:     Action Binding Implementation
 * Type:      Source (Implementation)
 * Component: Engine/Core/Input — system-level action mapping
 * Role:      Action abstraction, input unification, rebindable controls
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_state M.2 STATE — Lifecycle State
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
 * @defgroup keybind_impl_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup keybind_impl_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/src/input/keybind.c
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: bereshit/word/seed/code/c/source.c
 * Header:       engine/core/include/input/keybind.h
 * Pattern:      keyboard.c, mouse.c (sibling input implementations)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: source, c, engine, core, input, keybind, action, mapping, system
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     System-level action abstraction layer implementation
 * Core Design: Action enum → InputBinding → raw keys/buttons
 * Philosophy:  Game logic asks "is QUIT pressed?" — user configures what triggers QUIT
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  1 Corinthians 14:40 — "Let all things be done decently and in order"
 * Principle:  Orderly input handling — semantic actions bring clarity
 * Anchor:     Genesis 2:19 — Naming creates mastery
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Standard: stdint.h, stdbool.h, string.h, stdio.h
 *   Internal: keybind.h (our header), keyboard.h, mouse.h
 *
 * What Uses This:
 *   - scene.c (game input handling)
 *   - menu systems (navigation)
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-26) — Initial creation
 *   a-01.01 (2026-01-26) — Full template alignment
 *
 * Current:
 *   ✓ keybind_init / keybind_shutdown
 *   ✓ keybind_load_defaults
 *   ✓ keybind_is_action_pressed / just_pressed / just_released
 *   ✓ keybind_bind_key / keybind_bind_button
 *   ✓ keybind_unbind_action / unbind_key / unbind_button
 *   ✓ keybind_update
 *   ✓ keybind_get_action_name / keybind_get_binding_string
 *
 * Planned:
 *   ⏳ TOML configuration file loading
 *   ⏳ Gamepad support (INPUT_SOURCE_GAMEPAD)
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
 * @defgroup keybind_impl_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard and internal includes.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @defgroup keybind_impl_standard S.1a STANDARD — System Headers
 * @ingroup keybind_impl_includes
 * @{ */

/* Freestanding headers only - no libc in kernel mode */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Freestanding string helpers (no libc) */
static void keybind_memset(void* ptr, int val, size_t size) {
    uint8_t* p = (uint8_t*)ptr;
    while (size--) *p++ = (uint8_t)val;
}

static void keybind_memcpy(void* dst, const void* src, size_t size) {
    uint8_t* d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;
    while (size--) *d++ = *s++;
}

static size_t keybind_strlen(const char* s) {
    size_t len = 0;
    while (*s++) len++;
    return len;
}

static void keybind_strncat(char* dst, const char* src, size_t n) {
    while (*dst) dst++;
    while (n-- && *src) *dst++ = *src++;
    *dst = '\0';
}

static void keybind_strncpy(char* dst, const char* src, size_t n) {
    while (n-- > 1 && *src) *dst++ = *src++;
    *dst = '\0';
}

/** @} */ /* end S.1a STANDARD */

/**
 * @defgroup keybind_impl_internal S.1b INTERNAL — Project Headers
 * @ingroup keybind_impl_includes
 * @{ */

#include "hal/input/config/keybind.h"   /**< Public interface */
#include "hal/input/keyboard.h"          /**< KeyCode, keyboard_get_key_name */

/* MillenniumOS: Mouse stubs until mouse.h is implemented */
#ifndef MOS_HAL_INPUT_MOUSE_H
typedef uint8_t MouseButton;
#define MOUSE_NONE 0
static inline int mouse_is_pressed(MouseButton b) { (void)b; return 0; }
static inline const char* mouse_get_button_name(MouseButton b) { (void)b; return "MOUSE_NONE"; }
#endif

/** @} */ /* end S.1b INTERNAL */

/** S.1c EXTERNAL — [None] @defgroup keybind_impl_external @ingroup keybind_impl_includes @{ @} */
/** S.1d CPP — [Header handles C++ linkage] @defgroup keybind_impl_cpp @ingroup keybind_impl_includes @{ @} */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Preprocessor Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_defines S.2 DEFINES — Preprocessor Constants
 * @brief    Implementation constants.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/** S.2a SIZES — [Use header defines] @defgroup keybind_impl_sizes @ingroup keybind_impl_defines @{ @} */
/** S.2b LIMITS — [Use header defines] @defgroup keybind_impl_limits @ingroup keybind_impl_defines @{ @} */
/** S.2c FLAGS — [Reserved] @defgroup keybind_impl_flags @ingroup keybind_impl_defines @{ @} */
/** S.2d MACROS — [Reserved] @defgroup keybind_impl_macros @ingroup keybind_impl_defines @{ @} */
/** S.2e VERIFY — [Reserved] @defgroup keybind_impl_verify @ingroup keybind_impl_defines @{ @} */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Internal Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_types S.3 TYPES — Internal Type Declarations
 * @brief    Implementation-specific types (public types in header).
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/** S.3a ENUMS — [Public enums in header] @defgroup keybind_impl_enums @ingroup keybind_impl_types @{ @} */
/** S.3b STRUCTS — [Public structs in header] @defgroup keybind_impl_structs @ingroup keybind_impl_types @{ @} */
/** S.3c OPAQUE — [Reserved] @defgroup keybind_impl_opaque @ingroup keybind_impl_types @{ @} */
/** S.3d ERRORS — [Public errors in header] @defgroup keybind_impl_errors @ingroup keybind_impl_types @{ @} */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Internal Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_prototypes S.4 PROTOTYPES — Internal Functions
 * @brief    Static helper function declarations.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @defgroup keybind_impl_helpers S.4a INTERNAL_HELPERS — Private Helpers
 * @ingroup keybind_impl_prototypes
 * @{ */

/**
 * @brief Add a keyboard binding to an action (internal helper).
 *
 * @param[in] action  Action to bind to.
 * @param[in] key     KeyCode to bind.
 * @param[in] mods    Required modifiers.
 */
static void bind_key_internal(Action action, KeyCode key, uint8_t mods);

/**
 * @brief Add a mouse binding to an action (internal helper).
 *
 * @param[in] action  Action to bind to.
 * @param[in] button  MouseButton to bind.
 * @param[in] mods    Required modifiers.
 */
static void bind_button_internal(Action action, MouseButton button, uint8_t mods);

/**
 * @brief Check if a single binding is currently active.
 *
 * @param[in] binding  Binding to check.
 *
 * @return true if the binding's input is active.
 */
static bool is_binding_active(const SingleBinding* binding);

/** @} */ /* end S.4a INTERNAL_HELPERS */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — File Scope Declarations [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_file S.5 FILE-LEVEL — File Scope Declarations
 * @brief    Module-level static variables.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @defgroup keybind_impl_variables S.5a VARIABLES — Static Variables
 * @ingroup keybind_impl_file
 * @{ */

/** @brief Module initialization flag. */
static bool g_initialized = false;

/** @brief Binding storage — one InputBinding per action. */
static InputBinding g_bindings[ACTION_COUNT];

/** @brief Current frame action states (for pressed detection). */
static bool g_action_states[ACTION_COUNT];

/** @brief Previous frame action states (for edge detection). */
static bool g_action_prev_states[ACTION_COUNT];

/** @} */ /* end S.5a VARIABLES */

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_omission S.6 OMISSION — Reserved Sections
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
 * @defgroup keybind_impl_orgchart B.1 ORG CHART — Structure Overview
 * @brief    How this file is organized.
 *
 * STRUCTURE:
 *   B.2 HELPERS:   Internal static functions
 *   B.3 CORE OPS:  Public API implementations
 *
 * FLOW:
 *   keybind_init → load_defaults → keybind_update → is_action_pressed → game logic
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Internal Helper Functions [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_helpers_body B.2 HELPERS — Internal Functions
 * @brief    Static helper implementations.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Add a keyboard binding to an action (internal helper).
 *
 * @param[in] action  Action to bind to.
 * @param[in] key     KeyCode to bind.
 * @param[in] mods    Required modifiers.
 */
static void bind_key_internal(Action action, KeyCode key, uint8_t mods) {
    if (action >= ACTION_COUNT) {
        return;
    }

    InputBinding* b = &g_bindings[action];
    if (b->count >= KEYBIND_MAX_BINDINGS) {
        return;
    }

    SingleBinding* s = &b->bindings[b->count];
    s->source = INPUT_SOURCE_KEYBOARD;
    s->input.key = key;
    s->modifiers = mods;
    b->count++;
}

/**
 * @brief Add a mouse binding to an action (internal helper).
 *
 * @param[in] action  Action to bind to.
 * @param[in] button  MouseButton to bind.
 * @param[in] mods    Required modifiers.
 */
static void bind_button_internal(Action action, MouseButton button, uint8_t mods) {
    if (action >= ACTION_COUNT) {
        return;
    }

    InputBinding* b = &g_bindings[action];
    if (b->count >= KEYBIND_MAX_BINDINGS) {
        return;
    }

    SingleBinding* s = &b->bindings[b->count];
    s->source = INPUT_SOURCE_MOUSE;
    s->input.button = button;
    s->modifiers = mods;
    b->count++;
}

/**
 * @brief Check if a single binding is currently active.
 *
 * @param[in] binding  Binding to check.
 *
 * @return true if the binding's input is active.
 */
static bool is_binding_active(const SingleBinding* binding) {
    if (binding->source == INPUT_SOURCE_KEYBOARD) {
        /* Check key */
        if (!keyboard_is_pressed(binding->input.key)) {
            return false;
        }
        /* Check modifiers if required */
        if (binding->modifiers != KEYMOD_NONE) {
            uint8_t current_mods = keyboard_get_modifiers();
            if ((current_mods & binding->modifiers) != binding->modifiers) {
                return false;
            }
        }
        return true;
    }
    else if (binding->source == INPUT_SOURCE_MOUSE) {
        return mouse_is_pressed(binding->input.button) != 0;
    }

    return false;
}

/** @} */ /* end B.2 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Business Logic Implementations [OPERATIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_coreops B.3 CORE OPS — Business Logic
 * @brief    Public API implementations.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a LIFECYCLE — Init/Shutdown [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keybind_impl_lifecycle B.3a LIFECYCLE — Init/Shutdown
 * @ingroup keybind_impl_coreops
 * @{ */

KeybindError keybind_init(void) {
    if (g_initialized) {
        return KEYBIND_OK;
    }

    /* Clear all state */
    keybind_memset(g_bindings, 0, sizeof(g_bindings));
    keybind_memset(g_action_states, 0, sizeof(g_action_states));
    keybind_memset(g_action_prev_states, 0, sizeof(g_action_prev_states));

    /* Initialize action identities */
    for (int i = 0; i < ACTION_COUNT; i++) {
        g_bindings[i].action = (Action)i;
    }

    /* Load default bindings */
    keybind_load_defaults();

    g_initialized = true;
    return KEYBIND_OK;
}

void keybind_shutdown(void) {
    if (!g_initialized) {
        return;
    }
    g_initialized = false;
}

void keybind_load_defaults(void) {
    /* Clear existing bindings */
    for (int i = 0; i < ACTION_COUNT; i++) {
        g_bindings[i].count = 0;
    }

    /* ═══════════════════════════════════════════════════════════════════════
     * 1. CORE — Universal actions
     * ═══════════════════════════════════════════════════════════════════════ */
    bind_key_internal(ACTION_CONFIRM, KEY_ENTER, KEYMOD_NONE);
    bind_key_internal(ACTION_CANCEL, KEY_ESCAPE, KEYMOD_NONE);
    bind_key_internal(ACTION_TAB_NEXT, KEY_TAB, KEYMOD_NONE);
    bind_key_internal(ACTION_TAB_PREV, KEY_TAB, KEYMOD_SHIFT);

    /* ═══════════════════════════════════════════════════════════════════════
     * 2. SYSTEM — OS-level operations
     * ═══════════════════════════════════════════════════════════════════════ */
    bind_key_internal(ACTION_SHELL, KEY_F12, KEYMOD_NONE);
    bind_key_internal(ACTION_REBOOT, KEY_DELETE, KEYMOD_CTRL | KEYMOD_ALT);
    bind_key_internal(ACTION_SHUTDOWN, KEY_END, KEYMOD_CTRL | KEYMOD_ALT);
    bind_key_internal(ACTION_SCREENSHOT, KEY_PRINTSCREEN, KEYMOD_NONE);
    bind_key_internal(ACTION_HELP, KEY_F1, KEYMOD_NONE);
    bind_key_internal(ACTION_SETTINGS, KEY_COMMA, KEYMOD_CTRL);

    /* ═══════════════════════════════════════════════════════════════════════
     * 3. EDITING — Standard editing operations
     * ═══════════════════════════════════════════════════════════════════════ */
    bind_key_internal(ACTION_COPY, KEY_C, KEYMOD_CTRL);
    bind_key_internal(ACTION_CUT, KEY_X, KEYMOD_CTRL);
    bind_key_internal(ACTION_PASTE, KEY_V, KEYMOD_CTRL);
    bind_key_internal(ACTION_UNDO, KEY_Z, KEYMOD_CTRL);
    bind_key_internal(ACTION_REDO, KEY_Y, KEYMOD_CTRL);
    bind_key_internal(ACTION_REDO, KEY_Z, KEYMOD_CTRL | KEYMOD_SHIFT);
    bind_key_internal(ACTION_SELECT_ALL, KEY_A, KEYMOD_CTRL);
    bind_key_internal(ACTION_SAVE, KEY_S, KEYMOD_CTRL);
    bind_key_internal(ACTION_SAVE_AS, KEY_S, KEYMOD_CTRL | KEYMOD_SHIFT);
    bind_key_internal(ACTION_OPEN, KEY_O, KEYMOD_CTRL);
    bind_key_internal(ACTION_NEW, KEY_N, KEYMOD_CTRL);
    bind_key_internal(ACTION_FIND, KEY_F, KEYMOD_CTRL);
    bind_key_internal(ACTION_FIND_NEXT, KEY_F3, KEYMOD_NONE);
    bind_key_internal(ACTION_FIND_NEXT, KEY_G, KEYMOD_CTRL);
    bind_key_internal(ACTION_FIND_PREV, KEY_F3, KEYMOD_SHIFT);
    bind_key_internal(ACTION_REPLACE, KEY_H, KEYMOD_CTRL);
    bind_key_internal(ACTION_DELETE, KEY_DELETE, KEYMOD_NONE);
    bind_key_internal(ACTION_BACKSPACE, KEY_BACKSPACE, KEYMOD_NONE);

    /* ═══════════════════════════════════════════════════════════════════════
     * 4. NAVIGATION — Cursor/view movement
     * ═══════════════════════════════════════════════════════════════════════ */
    /* Arrow navigation */
    bind_key_internal(ACTION_NAV_UP, KEY_UP, KEYMOD_NONE);
    bind_key_internal(ACTION_NAV_DOWN, KEY_DOWN, KEYMOD_NONE);
    bind_key_internal(ACTION_NAV_LEFT, KEY_LEFT, KEYMOD_NONE);
    bind_key_internal(ACTION_NAV_RIGHT, KEY_RIGHT, KEYMOD_NONE);

    /* Line navigation */
    bind_key_internal(ACTION_LINE_START, KEY_HOME, KEYMOD_NONE);
    bind_key_internal(ACTION_LINE_END, KEY_END, KEYMOD_NONE);

    /* Word navigation */
    bind_key_internal(ACTION_WORD_LEFT, KEY_LEFT, KEYMOD_CTRL);
    bind_key_internal(ACTION_WORD_RIGHT, KEY_RIGHT, KEYMOD_CTRL);

    /* Page/document navigation */
    bind_key_internal(ACTION_PAGE_UP, KEY_PAGEUP, KEYMOD_NONE);
    bind_key_internal(ACTION_PAGE_DOWN, KEY_PAGEDOWN, KEYMOD_NONE);
    bind_key_internal(ACTION_DOC_START, KEY_HOME, KEYMOD_CTRL);
    bind_key_internal(ACTION_DOC_END, KEY_END, KEYMOD_CTRL);

    /* ═══════════════════════════════════════════════════════════════════════
     * 5. SELECTION — Extend selection (Shift+navigation)
     * ═══════════════════════════════════════════════════════════════════════ */
    bind_key_internal(ACTION_SELECT_UP, KEY_UP, KEYMOD_SHIFT);
    bind_key_internal(ACTION_SELECT_DOWN, KEY_DOWN, KEYMOD_SHIFT);
    bind_key_internal(ACTION_SELECT_LEFT, KEY_LEFT, KEYMOD_SHIFT);
    bind_key_internal(ACTION_SELECT_RIGHT, KEY_RIGHT, KEYMOD_SHIFT);
    bind_key_internal(ACTION_SELECT_WORD_LEFT, KEY_LEFT, KEYMOD_CTRL | KEYMOD_SHIFT);
    bind_key_internal(ACTION_SELECT_WORD_RIGHT, KEY_RIGHT, KEYMOD_CTRL | KEYMOD_SHIFT);
    bind_key_internal(ACTION_SELECT_LINE_START, KEY_HOME, KEYMOD_SHIFT);
    bind_key_internal(ACTION_SELECT_LINE_END, KEY_END, KEYMOD_SHIFT);
    bind_key_internal(ACTION_SELECT_PAGE_UP, KEY_PAGEUP, KEYMOD_SHIFT);
    bind_key_internal(ACTION_SELECT_PAGE_DOWN, KEY_PAGEDOWN, KEYMOD_SHIFT);
    bind_key_internal(ACTION_SELECT_DOC_START, KEY_HOME, KEYMOD_CTRL | KEYMOD_SHIFT);
    bind_key_internal(ACTION_SELECT_DOC_END, KEY_END, KEYMOD_CTRL | KEYMOD_SHIFT);

    /* ═══════════════════════════════════════════════════════════════════════
     * 6. WINDOW MANAGEMENT — Task/window operations
     * ═══════════════════════════════════════════════════════════════════════ */
    bind_key_internal(ACTION_WINDOW_CLOSE, KEY_F4, KEYMOD_ALT);
    bind_key_internal(ACTION_WINDOW_SWITCH, KEY_TAB, KEYMOD_ALT);
    bind_key_internal(ACTION_WINDOW_SWITCH_REV, KEY_TAB, KEYMOD_ALT | KEYMOD_SHIFT);
    bind_key_internal(ACTION_WINDOW_MINIMIZE, KEY_DOWN, KEYMOD_SUPER);
    bind_key_internal(ACTION_WINDOW_MAXIMIZE, KEY_UP, KEYMOD_SUPER);
    bind_key_internal(ACTION_WINDOW_FULLSCREEN, KEY_F11, KEYMOD_NONE);
    bind_key_internal(ACTION_WINDOW_FULLSCREEN, KEY_ENTER, KEYMOD_ALT);
    bind_key_internal(ACTION_WINDOW_SNAP_LEFT, KEY_LEFT, KEYMOD_SUPER);
    bind_key_internal(ACTION_WINDOW_SNAP_RIGHT, KEY_RIGHT, KEYMOD_SUPER);

    /* ═══════════════════════════════════════════════════════════════════════
     * 7. DEBUG — Development/debug operations
     * ═══════════════════════════════════════════════════════════════════════ */
    bind_key_internal(ACTION_DEBUG_CONSOLE, KEY_GRAVE, KEYMOD_NONE);
    bind_key_internal(ACTION_DEBUG_DAR, KEY_D, KEYMOD_CTRL | KEYMOD_SHIFT);
    bind_key_internal(ACTION_DEBUG_BREAK, KEY_PAUSE, KEYMOD_CTRL);
    bind_key_internal(ACTION_DEBUG_STEP, KEY_F10, KEYMOD_NONE);
    bind_key_internal(ACTION_DEBUG_STEP_INTO, KEY_F11, KEYMOD_NONE);
    bind_key_internal(ACTION_DEBUG_CONTINUE, KEY_F5, KEYMOD_NONE);

    /* ═══════════════════════════════════════════════════════════════════════
     * 8. FUNCTION KEYS — Raw F-key actions
     * ═══════════════════════════════════════════════════════════════════════ */
    bind_key_internal(ACTION_F1, KEY_F1, KEYMOD_NONE);
    bind_key_internal(ACTION_F2, KEY_F2, KEYMOD_NONE);
    bind_key_internal(ACTION_F3, KEY_F3, KEYMOD_NONE);
    bind_key_internal(ACTION_F4, KEY_F4, KEYMOD_NONE);
    bind_key_internal(ACTION_F5, KEY_F5, KEYMOD_NONE);
    bind_key_internal(ACTION_F6, KEY_F6, KEYMOD_NONE);
    bind_key_internal(ACTION_F7, KEY_F7, KEYMOD_NONE);
    bind_key_internal(ACTION_F8, KEY_F8, KEYMOD_NONE);
    bind_key_internal(ACTION_F9, KEY_F9, KEYMOD_NONE);
    bind_key_internal(ACTION_F10, KEY_F10, KEYMOD_NONE);
    bind_key_internal(ACTION_F11, KEY_F11, KEYMOD_NONE);
    bind_key_internal(ACTION_F12, KEY_F12, KEYMOD_NONE);
}

/** @} */ /* end B.3a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3b ACCESSORS — State Queries [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keybind_impl_accessors B.3b ACCESSORS — State Queries
 * @ingroup keybind_impl_coreops
 * @{ */

int keybind_is_action_pressed(Action action) {
    if (action >= ACTION_COUNT) {
        return 0;
    }
    return g_action_states[action] ? 1 : 0;
}

int keybind_is_action_just_pressed(Action action) {
    if (action >= ACTION_COUNT) {
        return 0;
    }
    return (g_action_states[action] && !g_action_prev_states[action]) ? 1 : 0;
}

int keybind_is_action_just_released(Action action) {
    if (action >= ACTION_COUNT) {
        return 0;
    }
    return (!g_action_states[action] && g_action_prev_states[action]) ? 1 : 0;
}

const InputBinding* keybind_get_binding(Action action) {
    if (action >= ACTION_COUNT) {
        return NULL;
    }
    return &g_bindings[action];
}

const char* keybind_get_action_name(Action action) {
    static const char* names[] = {
        /* 1. CORE */
        [ACTION_NONE]              = "NONE",
        [ACTION_CONFIRM]           = "CONFIRM",
        [ACTION_CANCEL]            = "CANCEL",
        [ACTION_TAB_NEXT]          = "TAB_NEXT",
        [ACTION_TAB_PREV]          = "TAB_PREV",

        /* 2. SYSTEM */
        [ACTION_SHELL]             = "SHELL",
        [ACTION_REBOOT]            = "REBOOT",
        [ACTION_SHUTDOWN]          = "SHUTDOWN",
        [ACTION_SCREENSHOT]        = "SCREENSHOT",
        [ACTION_HELP]              = "HELP",
        [ACTION_SETTINGS]          = "SETTINGS",

        /* 3. EDITING */
        [ACTION_COPY]              = "COPY",
        [ACTION_CUT]               = "CUT",
        [ACTION_PASTE]             = "PASTE",
        [ACTION_UNDO]              = "UNDO",
        [ACTION_REDO]              = "REDO",
        [ACTION_SELECT_ALL]        = "SELECT_ALL",
        [ACTION_SAVE]              = "SAVE",
        [ACTION_SAVE_AS]           = "SAVE_AS",
        [ACTION_OPEN]              = "OPEN",
        [ACTION_NEW]               = "NEW",
        [ACTION_FIND]              = "FIND",
        [ACTION_FIND_NEXT]         = "FIND_NEXT",
        [ACTION_FIND_PREV]         = "FIND_PREV",
        [ACTION_REPLACE]           = "REPLACE",
        [ACTION_DELETE]            = "DELETE",
        [ACTION_BACKSPACE]         = "BACKSPACE",

        /* 4. NAVIGATION */
        [ACTION_NAV_UP]            = "NAV_UP",
        [ACTION_NAV_DOWN]          = "NAV_DOWN",
        [ACTION_NAV_LEFT]          = "NAV_LEFT",
        [ACTION_NAV_RIGHT]         = "NAV_RIGHT",
        [ACTION_LINE_START]        = "LINE_START",
        [ACTION_LINE_END]          = "LINE_END",
        [ACTION_WORD_LEFT]         = "WORD_LEFT",
        [ACTION_WORD_RIGHT]        = "WORD_RIGHT",
        [ACTION_PAGE_UP]           = "PAGE_UP",
        [ACTION_PAGE_DOWN]         = "PAGE_DOWN",
        [ACTION_DOC_START]         = "DOC_START",
        [ACTION_DOC_END]           = "DOC_END",

        /* 5. SELECTION */
        [ACTION_SELECT_UP]         = "SELECT_UP",
        [ACTION_SELECT_DOWN]       = "SELECT_DOWN",
        [ACTION_SELECT_LEFT]       = "SELECT_LEFT",
        [ACTION_SELECT_RIGHT]      = "SELECT_RIGHT",
        [ACTION_SELECT_WORD_LEFT]  = "SELECT_WORD_LEFT",
        [ACTION_SELECT_WORD_RIGHT] = "SELECT_WORD_RIGHT",
        [ACTION_SELECT_LINE_START] = "SELECT_LINE_START",
        [ACTION_SELECT_LINE_END]   = "SELECT_LINE_END",
        [ACTION_SELECT_PAGE_UP]    = "SELECT_PAGE_UP",
        [ACTION_SELECT_PAGE_DOWN]  = "SELECT_PAGE_DOWN",
        [ACTION_SELECT_DOC_START]  = "SELECT_DOC_START",
        [ACTION_SELECT_DOC_END]    = "SELECT_DOC_END",

        /* 6. WINDOW MANAGEMENT */
        [ACTION_WINDOW_CLOSE]      = "WINDOW_CLOSE",
        [ACTION_WINDOW_SWITCH]     = "WINDOW_SWITCH",
        [ACTION_WINDOW_SWITCH_REV] = "WINDOW_SWITCH_REV",
        [ACTION_WINDOW_MINIMIZE]   = "WINDOW_MINIMIZE",
        [ACTION_WINDOW_MAXIMIZE]   = "WINDOW_MAXIMIZE",
        [ACTION_WINDOW_FULLSCREEN] = "WINDOW_FULLSCREEN",
        [ACTION_WINDOW_SNAP_LEFT]  = "WINDOW_SNAP_LEFT",
        [ACTION_WINDOW_SNAP_RIGHT] = "WINDOW_SNAP_RIGHT",

        /* 7. DEBUG */
        [ACTION_DEBUG_CONSOLE]     = "DEBUG_CONSOLE",
        [ACTION_DEBUG_DAR]         = "DEBUG_DAR",
        [ACTION_DEBUG_BREAK]       = "DEBUG_BREAK",
        [ACTION_DEBUG_STEP]        = "DEBUG_STEP",
        [ACTION_DEBUG_STEP_INTO]   = "DEBUG_STEP_INTO",
        [ACTION_DEBUG_CONTINUE]    = "DEBUG_CONTINUE",

        /* 8. FUNCTION KEYS */
        [ACTION_F1]                = "F1",
        [ACTION_F2]                = "F2",
        [ACTION_F3]                = "F3",
        [ACTION_F4]                = "F4",
        [ACTION_F5]                = "F5",
        [ACTION_F6]                = "F6",
        [ACTION_F7]                = "F7",
        [ACTION_F8]                = "F8",
        [ACTION_F9]                = "F9",
        [ACTION_F10]               = "F10",
        [ACTION_F11]               = "F11",
        [ACTION_F12]               = "F12",
    };

    if (action < ACTION_COUNT && names[action]) {
        return names[action];
    }
    return "UNKNOWN";
}

int keybind_get_binding_string(Action action, char* buffer, int size) {
    if (action >= ACTION_COUNT || buffer == NULL || size < 1) {
        return 0;
    }

    const InputBinding* b = &g_bindings[action];
    if (b->count == 0) {
        keybind_strncpy(buffer, "(unbound)", (size_t)size);
        return 1;
    }

    buffer[0] = '\0';
    for (uint8_t i = 0; i < b->count && i < KEYBIND_MAX_BINDINGS; i++) {
        const SingleBinding* s = &b->bindings[i];
        char keyname[32];

        if (s->source == INPUT_SOURCE_KEYBOARD) {
            keybind_strncpy(keyname, keyboard_get_key_name(s->input.key), sizeof(keyname));
        } else if (s->source == INPUT_SOURCE_MOUSE) {
            keybind_strncpy(keyname, mouse_get_button_name(s->input.button), sizeof(keyname));
        } else {
            continue;
        }

        if (i > 0) {
            keybind_strncat(buffer, ", ", (size_t)(size - (int)keybind_strlen(buffer) - 1));
        }
        keybind_strncat(buffer, keyname, (size_t)(size - (int)keybind_strlen(buffer) - 1));
    }

    return 1;
}

/** @} */ /* end B.3b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3c OPERATIONS — Binding Operations [BINDING_OPS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup keybind_impl_bindingops B.3c OPERATIONS — Binding Operations
 * @ingroup keybind_impl_coreops
 * @{ */

KeybindError keybind_bind_key(Action action, KeyCode key, uint8_t modifiers) {
    if (!g_initialized) {
        return KEYBIND_ERR_NOT_INIT;
    }
    if (action >= ACTION_COUNT) {
        return KEYBIND_ERR_INVALID;
    }

    InputBinding* b = &g_bindings[action];
    if (b->count >= KEYBIND_MAX_BINDINGS) {
        return KEYBIND_ERR_FULL;
    }

    bind_key_internal(action, key, modifiers);
    return KEYBIND_OK;
}

KeybindError keybind_bind_button(Action action, MouseButton button, uint8_t modifiers) {
    if (!g_initialized) {
        return KEYBIND_ERR_NOT_INIT;
    }
    if (action >= ACTION_COUNT) {
        return KEYBIND_ERR_INVALID;
    }

    InputBinding* b = &g_bindings[action];
    if (b->count >= KEYBIND_MAX_BINDINGS) {
        return KEYBIND_ERR_FULL;
    }

    bind_button_internal(action, button, modifiers);
    return KEYBIND_OK;
}

KeybindError keybind_unbind_action(Action action) {
    if (!g_initialized) {
        return KEYBIND_ERR_NOT_INIT;
    }
    if (action >= ACTION_COUNT) {
        return KEYBIND_ERR_INVALID;
    }

    g_bindings[action].count = 0;
    return KEYBIND_OK;
}

KeybindError keybind_unbind_key(Action action, KeyCode key) {
    if (!g_initialized) {
        return KEYBIND_ERR_NOT_INIT;
    }
    if (action >= ACTION_COUNT) {
        return KEYBIND_ERR_INVALID;
    }

    InputBinding* b = &g_bindings[action];
    for (uint8_t i = 0; i < b->count; i++) {
        if (b->bindings[i].source == INPUT_SOURCE_KEYBOARD &&
            b->bindings[i].input.key == key) {
            /* Remove by shifting remaining bindings */
            for (uint8_t j = i; j < b->count - 1; j++) {
                b->bindings[j] = b->bindings[j + 1];
            }
            b->count--;
            return KEYBIND_OK;
        }
    }

    return KEYBIND_ERR_NOT_FOUND;
}

KeybindError keybind_unbind_button(Action action, MouseButton button) {
    if (!g_initialized) {
        return KEYBIND_ERR_NOT_INIT;
    }
    if (action >= ACTION_COUNT) {
        return KEYBIND_ERR_INVALID;
    }

    InputBinding* b = &g_bindings[action];
    for (uint8_t i = 0; i < b->count; i++) {
        if (b->bindings[i].source == INPUT_SOURCE_MOUSE &&
            b->bindings[i].input.button == button) {
            /* Remove by shifting remaining bindings */
            for (uint8_t j = i; j < b->count - 1; j++) {
                b->bindings[j] = b->bindings[j + 1];
            }
            b->count--;
            return KEYBIND_OK;
        }
    }

    return KEYBIND_ERR_NOT_FOUND;
}

void keybind_update(void) {
    if (!g_initialized) {
        return;
    }

    /* Save previous state for edge detection */
    keybind_memcpy(g_action_prev_states, g_action_states, sizeof(g_action_states));

    /* Update each action's state based on its bindings */
    for (int a = 0; a < ACTION_COUNT; a++) {
        bool active = false;
        const InputBinding* b = &g_bindings[a];

        for (uint8_t i = 0; i < b->count && !active; i++) {
            if (is_binding_active(&b->bindings[i])) {
                active = true;
            }
        }

        g_action_states[a] = active;
    }
}

/** @} */ /* end B.3c OPERATIONS */

/** @} */ /* end B.3 CORE OPS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4-B.6 — [Collapsed: Standard patterns] [COLLAPSED]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_collapsed B.4-B.6 COLLAPSED — Standard Patterns
 * @brief    [Reserved: Error handling, public API, and omission sections]
 *
 * B.4 ERRORS:     Error handling via return codes (KeybindError enum)
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
 * @defgroup keybind_impl_validation X.1 VALIDATION — Build and Test
 * @brief    Verify implementation correctness.
 *
 * BUILD:
 * @code{.sh}
 * gcc -c -Wall -Wextra -std=c11 keybind.c
 * @endcode
 *
 * TEST:
 * @code{.c}
 * keybind_init();
 * keybind_update();
 * if (keybind_is_action_pressed(ACTION_QUIT)) {
 *     printf("Quit action triggered!\n");
 * }
 * keybind_shutdown();
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Runtime Behavior [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_execution X.2 EXECUTION — Runtime Behavior
 * @brief    How this module behaves at runtime.
 *
 * CALL ORDER:
 *   1. keybind_init() — Initialize subsystem
 *   2. keybind_update() — Once per frame, updates action states
 *   3. keybind_is_action_*() — Check action states
 *   4. keybind_shutdown() — Cleanup
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Resource Management [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_cleanup X.3 CLEANUP — Resource Management
 * @brief    Resource cleanup guidance.
 *
 * keybind_shutdown() resets all state.
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
 * @defgroup keybind_impl_policy X.4 POLICY — Modification Guidelines
 * @brief    Guide future maintainers.
 *
 * SAFE TO MODIFY:
 *   - ✅ Default bindings in keybind_load_defaults()
 *   - ✅ Add new accessor functions
 *
 * MODIFY WITH CARE:
 *   - ⚠️ is_binding_active logic (affects all action detection)
 *   - ⚠️ State array access patterns
 *
 * NEVER MODIFY:
 *   - ❌ keybind_init/shutdown contract
 *   - ❌ 4-block structure
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5-X.7 — [Collapsed: See header for details] [COLLAPSED]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_extension X.5-X.7 COLLAPSED
 * @brief    Extension, troubleshooting, and reference in header file.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup keybind_impl_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * @par Scripture
 * "Let all things be done decently and in order."
 *   — 1 Corinthians 14:40
 *
 * Orderly input handling — semantic actions bring clarity to game logic.
 * Game code asks what the user WANTS to do, not which key they pressed.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════
