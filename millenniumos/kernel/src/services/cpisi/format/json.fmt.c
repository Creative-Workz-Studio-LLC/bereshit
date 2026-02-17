/// @file json.c
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
/// @omni:req  key        = CORNERSTONE-CORE-CPISI-JSON-IMPL
/// @omni:req  from       = bereshit/word/seed/code/c/source.c
/// @omni:req  at         = b-03.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = source
/// @omni:inh  subtype    = io
/// @omni:inh  role       = serialization
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = io/json
/// @omni:ins  includes   = [json.h, stdio.h, string.h, scene.h, omni_cpisi.h]
/// @omni:ins  provides   = [json_serialization, state_output, cube_output]
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
/// @omni:req  title      = CPI-SI JSON Response Builders Implementation
/// @omni:req  brief      = JSON serialization for cognitive state, cube, and game output
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
///   - SP.1 CONFIG  — Compile-time settings (POSIX)
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * SP.1 CONFIG — Compile-time Settings [CONFIG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_special_config SP.1 CONFIG — Compile-time Settings
 * @brief    Feature macros and environment configuration before includes.
 *
 * WHY: Settings that affect what headers provide must precede #include.
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
 * @defgroup cpisi_json_impl_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_json_impl_special_config
 * @brief    Enables POSIX.1-2008 functions in standard headers.
 *
 * WHY: String functions like strcasecmp require POSIX features.
 *      Must be defined BEFORE any system includes or they won't see it.
 *
 * SCOPE: Universal — most C files need POSIX features.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L  /**< POSIX.1-2008 — enables strdup, etc. */
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
 * @file      json.c
 * @brief     CPI-SI JSON Response Builders — structured output for cognitive state.
 *
 * @defgroup cpisi_json_impl_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CORNERSTONE-CORE-CPISI-JSON-IMPL
 * Title:     CPI-SI JSON Response Builders Implementation
 * Type:      Source (Library)
 * Component: IO — provides JSON serialization for CPI-SI state
 * Role:      Serialize cognitive state, cube position, game state to JSON
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_state M.2 STATE — Lifecycle State
 * @{
 *
 * @version   b-03.00
 * @date      2026-01-25
 *
 * Status:    Active
 * Created:   2026-01-20
 * Updated:   2026-01-25
 *
 * @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authorship & Rights [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_attribution M.3 ATTRIBUTION — Authorship & Rights
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
 * @defgroup cpisi_json_impl_location M.4 LOCATION — File Position
 * @{
 *
 * Path: cornerstone/engine/core/src/cpisi/io/json.c
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Template Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_derivation M.5 DERIVATION — Template Lineage
 * @{
 *
 * Template:    bereshit/word/seed/code/c/source.c
 * Implements:  cpisi/io/json.h
 * Spec:        Proverbs 25:11 — "A word fitly spoken"
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Layer:  STORAGE (6) — serialization to output
 * Domain: IO
 * Tags:   json, serialization, output, state, cube, cognitive
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Implement JSON serialization for all CPI-SI state types.
 *
 * Core Design: Structure (JSON) presents content in receivable form.
 *              "A word fitly spoken is like apples of gold in pictures of silver."
 *
 * Key Features:
 *   - Game state output (scene, choices, text)
 *   - Cube state output (position, keys, Hebrew state)
 *   - Cognitive state serialization (Hebrew state, health)
 *   - Error and status responses
 *   - Configurable output target (stdout, socket, buffer)
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Proverbs 25:11 — "A word fitly spoken is like apples of gold
 *             in pictures of silver"
 *
 * Principle:  Structure (silver frame) presents content (golden apples) properly.
 *             JSON is the frame; cognitive state is the content.
 *
 * Anchor:     Genesis 11:1 — "The whole earth was of one language"
 *             Structured output enables universal understanding.
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Ladder Chain [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_dependencies M.9 DEPENDENCIES — Ladder Chain
 * @{
 *
 * What This Needs:
 *   Internal: cpisi/io/json.h (declarations)
 *   Internal: types/types.h (HebrewState, HealthScore, etc.)
 *   Internal: omni_cpisi.h (CPISICube)
 *   Internal: scene.h (game state queries)
 *   Standard: stdio.h, string.h
 *
 * What Uses This:
 *   - REPL mode — JSON output for interactive sessions
 *   - Socket mode — JSON protocol for external clients
 *   - cpisi.c — orchestrator status reporting
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-20) — Initial creation
 *   a-02.00 (2026-01-22) — Added cube state serialization
 *   b-03.00 (2026-01-25) — Full source.c template alignment
 *                          Added hebrew_state and health functions
 *                          DAR witness system integration
 *
 * Current:
 *   ✓ All json.h functions implemented
 *   ✓ Full template alignment
 *   ✓ Proper docstrings
 *
 * Planned:
 *   ⏳ Streaming JSON for large outputs
 *   ⏳ DAR phase logging integration
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
///   - S.1 INCLUDES  — Header dependencies
///   - S.2 DEFINES   — Buffer size constants
///   - S.3 TYPES     — [Reserved: uses json.h types]
///   - S.4 PROTOS    — Static function prototypes
///   - S.5 FILE-LEVEL — Global output callback
///   - S.6 OMISSION  — Reserved sections
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES — Header Dependencies [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

// Include kernel.h first to get CPISI_MODE
#include "kernel.h"

// In freestanding kernel mode (CPISI_MODE 0-1), JSON format is not available.
// The full implementation requires hosted environment with framework.
#if CPISI_MODE >= 2

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_json_impl_includes
 * @brief    C standard library headers required by this module.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include <stdio.h>   /**< snprintf, printf, fflush, stdout */
#include <string.h>  /**< strlen, strncpy, memset */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_json_impl_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * WHY: JSON implementation needs the header declarations and types.
 *      Scene API for game state. CPISICube for cube state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * LADDER: json.h → types.h, health.h, omni_cpisi.h
 * Brings CPISICube via omni_cpisi.h include in header.
 */
#include "services/cpisi/format/json.fmt.h"  /**< LADDER: json.h brings types, health, omni_cpisi */
#include "framework/core/scene/type/scene.type.h"          /**< Game state queries (scene_get_current, etc.) */
#include "framework/core/engine/manager/engine.mgr.h"  /**< engine_get_manifest for manifest state */

/** @} */ /* end S.1b INTERNAL */

/**
 * S.1c EXTERNAL — [Reserved: no external libraries]
 * @defgroup cpisi_json_impl_external S.1c EXTERNAL
 * @ingroup cpisi_json_impl_includes
 * @{ */
/* [Reserved: No external libraries needed] */
/** @} */

/**
 * S.1d CPP — [Reserved: source file, not header]
 * @defgroup cpisi_json_impl_cpp S.1d CPP
 * @ingroup cpisi_json_impl_includes
 * @{ */
/* [Reserved: C++ linkage handled in header] */
/** @} */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Buffer Size Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_defines S.2 DEFINES — Buffer Size Constants
 * @brief    Buffer sizes for JSON output to avoid magic numbers.
 *
 * WHY: Consistent buffer sizes prevent overflow and clarify intent.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a BUFFERS — Output Buffer Sizes [BUFFERS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_buffers S.2a BUFFERS — Output Buffer Sizes
 * @ingroup cpisi_json_impl_defines
 * @brief    Buffer sizes for various JSON output types.
 *
 * WHY: Named constants clarify intent and ensure consistency.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @brief Large buffer for complex JSON output (state, screens). */
#define JSON_BUF_LARGE      4096

/** @brief Medium buffer for standard responses. */
#define JSON_BUF_MEDIUM     1024

/** @brief Small buffer for simple responses. */
#define JSON_BUF_SMALL       512

/** @brief Buffer for escaped text content. */
#define JSON_BUF_TEXT       2048

/** @brief Buffer for escaped choice text. */
#define JSON_BUF_CHOICE      256

/** @brief Buffer for escaped short strings. */
#define JSON_BUF_SHORT       128

/** @brief Buffer for escaped minimal strings. */
#define JSON_BUF_MINI         64

/** @brief Safety margin for buffer operations. */
#define JSON_BUF_SAFETY      300

/** @brief Escape buffer safety margin. */
#define JSON_ESCAPE_SAFETY    10

/** @} */ /* end S.2a BUFFERS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b LIMITS — Array and Count Limits [LIMITS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_limits S.2b LIMITS — Array and Count Limits
 * @ingroup cpisi_json_impl_defines
 * @brief    Array size limits for JSON output.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @brief Hebrew state name array size (7 states). */
#define JSON_HEBREW_STATE_COUNT     7

/** @brief Health level name array size. */
#define JSON_HEALTH_LEVEL_COUNT     7

/** @brief Number of keys in cube (3). */
#define JSON_KEY_COUNT              3

/** @} */ /* end S.2b LIMITS */

/**
 * S.2c-f — [Reserved sections]
 * @defgroup cpisi_json_impl_s2c S.2c FLAGS @{ @}
 * @defgroup cpisi_json_impl_s2d S.2d MACROS @{ @}
 * @defgroup cpisi_json_impl_s2e S.2e VERIFY @{ @}
 */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_types S.3 TYPES — Type Declarations
 * @brief    [Reserved: Types defined in json.h]
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: CpisiJsonOutputFn defined in json.h] */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Static Function Prototypes [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_prototypes S.4 PROTOTYPES — Static Function Prototypes
 * @brief    Internal function declarations.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a HELPERS — Internal Helper Prototypes [HELPERS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_protos_helpers S.4a HELPERS — Internal Helpers
 * @ingroup cpisi_json_impl_prototypes
 * @brief    Static helper function declarations.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Internal JSON output function.
 *
 * Routes output through callback or to stdout.
 *
 * @param[in] msg  JSON string to output (null-terminated).
 */
static void json_output(const char* msg);

/**
 * @brief Get scene type name string.
 *
 * @param[in] scene  SceneType enum value.
 *
 * @return Static string for scene name.
 */
static const char* cpisi_get_scene_name(SceneType scene);

/**
 * @brief Get Hebrew state name string.
 *
 * @param[in] state  DARHebrewState enum value (-3 to +3).
 *
 * @return Static string for state name.
 */
static const char* get_hebrew_state_name(DARHebrewState state);

/**
 * @brief Get health level name string.
 *
 * @param[in] level  HealthLevel enum value.
 *
 * @return Static string for level name.
 */
static const char* get_health_level_name(HealthLevel level);

/** @} */ /* end S.4a HELPERS */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — Global State [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_file S.5 FILE-LEVEL — Global State
 * @brief    File-scope variables.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5a GLOBALS — File-Scope Variables [GLOBALS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_globals S.5a GLOBALS — File-Scope Variables
 * @ingroup cpisi_json_impl_file
 * @brief    Static variables for output configuration.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Global output callback.
 *
 * WHY: Allows redirection of JSON output to different targets (stdout, socket,
 *      buffer). NULL means stdout (default).
 *
 * @warning Not thread-safe. Set once at startup.
 */
static CpisiJsonOutputFn g_json_output = NULL;

/** @} */ /* end S.5a GLOBALS */

/**
 * S.5b FUNC_TYPES — [Reserved: no callback types]
 * @defgroup cpisi_json_impl_func_types S.5b FUNC_TYPES @{ @}
 */

/**
 * S.5c INFRASTRUCTURE — [Reserved: no logger]
 * @defgroup cpisi_json_impl_infra S.5c INFRASTRUCTURE @{ @}
 */

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_omission S.6 OMISSION — Reserved Section Guide
 * @brief    Documents what sections are reserved and why.
 *
 * Reserved Sections:
 *   - S.1c EXTERNAL:   No external libraries needed
 *   - S.1d CPP:        C++ handled in header
 *   - S.2c-f:          No flags, macros, verification needed
 *   - S.3 TYPES:       Types in json.h
 *   - S.5b FUNC_TYPES: No callback types here
 *   - S.5c INFRA:      No logger in JSON module
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
/// @brief The actual work — implementations.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART    — Ladder hierarchy, baton flow, APU counts
///   - B.2 HELPERS      — Internal helper implementations
///   - B.3 CORE OPS     — Output configuration
///   - B.4 ERRORS       — [Reserved: uses cpisi_json_error]
///   - B.5 PUBLIC API   — JSON output function implementations
///   - B.6 OMISSION     — Reserved section guide
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Ladder Hierarchy [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_orgchart B.1 ORG CHART — Ladder Hierarchy
 * @brief    Map internal structure — ladder hierarchy, baton flow, APU counts.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Ladder Hierarchy [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_structure B.1a STRUCTURE — Ladder Hierarchy
 * @ingroup cpisi_json_impl_orgchart
 * @brief    Function call hierarchy — who calls whom.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/*
 * Public APIs (Top Rungs - Output)
 * ├── cpisi_json_state()         → json_output(), cpisi_json_escape()
 * ├── cpisi_json_state_pty()     → json_output(), cpisi_json_escape()
 * ├── cpisi_json_choices()       → json_output(), cpisi_json_escape()
 * ├── cpisi_json_cube_state()    → json_output(), cpisi_json_error()
 * ├── cpisi_json_cube_keys()     → json_output(), cpisi_json_error()
 * ├── cpisi_json_cube_action()   → json_output()
 * ├── cpisi_json_thought()       → json_output(), cpisi_json_escape()
 * ├── cpisi_json_response()      → json_output(), cpisi_json_escape()
 * ├── cpisi_json_hebrew_state()  → json_output(), get_hebrew_state_name()
 * ├── cpisi_json_health()        → json_output(), get_health_level_name()
 * └── Status functions           → json_output()
 *
 * Helpers (Bottom Rungs - Foundations)
 * ├── json_output()              → g_json_output or printf
 * ├── cpisi_json_escape()        → (string processing)
 * ├── cpisi_get_scene_name()     → (static lookup)
 * ├── get_hebrew_state_name()    → (static lookup)
 * └── get_health_level_name()    → (static lookup)
 */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Baton Execution Path [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_flow B.1b FLOW — Baton Execution Path
 * @ingroup cpisi_json_impl_orgchart
 * @brief    How execution flows through the file.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/*
 * Entry → cpisi_json_*() public function
 *   ↓
 * Build JSON string in local buffer
 *   ↓
 * Call json_output() with buffer
 *   ↓
 * json_output() routes to callback or stdout
 *   ↓
 * Exit → caller continues
 */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — APU Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_counts B.1c COUNTS — APU Statistics
 * @ingroup cpisi_json_impl_orgchart
 * @brief    Atomic Processing Unit counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Functions:       21 total                                       */
/* Helpers:          4 (json_output, escape, scene_name, lookups)  */
/* Core Operations:  1 (set_output)                                */
/* Public APIs:     16 (all declared in json.h)                    */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Foundation Implementations [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_helpers B.2 HELPERS — Foundation Implementations
 * @brief    Internal helper function implementations — building blocks.
 *
 * CONTAINS:
 *   - B.2a OUTPUT    — Output routing
 *   - B.2b ESCAPE    — String escaping
 *   - B.2c LOOKUP    — Name lookups
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a OUTPUT — Output Routing [OUTPUT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_output B.2a OUTPUT — Output Routing
 * @ingroup cpisi_json_impl_helpers
 * @brief    Route JSON output to callback or stdout.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Internal JSON output function.
 *
 * Routes output through the configured callback or to stdout if no callback.
 * All JSON output goes through this function for consistent handling.
 *
 * @param[in] msg  JSON string to output (null-terminated).
 *
 * @par DAR DETECT
 *   Witnesses output event — the JSON message being sent.
 */
static void json_output(const char* msg) {
    if (g_json_output != NULL) {
        /* Route through callback */
        g_json_output(msg);
    } else {
        /* Default: stdout */
        printf("%s", msg);
        fflush(stdout);
    }
}

/** @} */ /* end B.2a OUTPUT */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b LOOKUP — Name String Lookups [LOOKUP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_lookup B.2b LOOKUP — Name Lookups
 * @ingroup cpisi_json_impl_helpers
 * @brief    Static name lookups for enums.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Get scene type name string.
 *
 * Converts SceneType enum to human-readable string for JSON output.
 *
 * @param[in] scene  SceneType enum value.
 *
 * @return Static string (do not free).
 */
static const char* cpisi_get_scene_name(SceneType scene) {
    switch (scene) {
        /* System scenes */
        case SCENE_NONE:          return "NONE";
        case SCENE_ERROR:         return "ERROR";

        /* Infrastructure scenes */
        case SCENE_STARTUP:       return "STARTUP";
        case SCENE_LAUNCHER:      return "LAUNCHER";
        case SCENE_MODE_SELECTOR: return "MODE_SELECTOR";
        case SCENE_TITLE:         return "TITLE";
        case SCENE_LOADING:       return "LOADING";
        case SCENE_GAME_MENU:     return "GAME_MENU";

        /* World scenes */
        case SCENE_OVERWORLD:     return "OVERWORLD";
        case SCENE_OBJECTS:       return "OBJECTS";

        /* Gameplay scenes */
        case SCENE_DIALOGUE:      return "DIALOGUE";
        case SCENE_CHOICE:        return "CHOICE";
        case SCENE_COMBAT:        return "COMBAT";

        /* Overlay scenes */
        case SCENE_PAUSE:         return "PAUSE";
        case SCENE_MODAL:         return "MODAL";
        case SCENE_DEMO_COMPLETE: return "DEMO_COMPLETE";

        default:                  return "UNKNOWN";
    }
}

/**
 * @brief Get Hebrew state name string.
 *
 * Converts HebrewState value (-3 to +3) to Hebrew name.
 *
 * @param[in] state  HebrewState value.
 *
 * @return Static string (do not free).
 *
 * @par Hebrew States
 *   -3: shavar (broken)
 *   -2: chaser (lacking)
 *   -1: ratsah (wanting)
 *    0: yashar (even) — center
 *   +1: tamim (sound)
 *   +2: shalem (whole)
 *   +3: tov (perfect)
 */
static const char* get_hebrew_state_name(DARHebrewState state) {
    static const char* names[JSON_HEBREW_STATE_COUNT] = {
        "shavar",  /* -3: broken */
        "chaser",  /* -2: lacking */
        "ratsah",  /* -1: wanting */
        "yashar",  /*  0: even (center) */
        "tamim",   /* +1: sound */
        "shalem",  /* +2: whole */
        "tov"      /* +3: perfect */
    };

    /* Map -3..+3 to 0..6 index */
    int index = state + 3;
    if (index >= 0 && index < JSON_HEBREW_STATE_COUNT) {
        return names[index];
    }
    return "unknown";
}

/**
 * @brief Get health level name string.
 *
 * Converts HealthLevel enum to human-readable string.
 *
 * @param[in] level  HealthLevel enum value.
 *
 * @return Static string (do not free).
 */
static const char* get_health_level_name(HealthLevel level) {
    static const char* names[JSON_HEALTH_LEVEL_COUNT] = {
        "critical",   /* HEALTH_LEVEL_CRITICAL */
        "poor",       /* HEALTH_LEVEL_POOR */
        "low",        /* HEALTH_LEVEL_LOW */
        "fair",       /* HEALTH_LEVEL_FAIR */
        "good",       /* HEALTH_LEVEL_GOOD */
        "excellent",  /* HEALTH_LEVEL_EXCELLENT */
        "optimal"     /* HEALTH_LEVEL_OPTIMAL */
    };

    if (level >= 0 && level < JSON_HEALTH_LEVEL_COUNT) {
        return names[level];
    }
    return "unknown";
}

/** @} */ /* end B.2b LOOKUP */

/** @} */ /* end B.2 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Output Configuration [OPERATIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_coreops B.3 CORE OPS — Output Configuration
 * @brief    Configuration functions for JSON output.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a CONFIG — Output Callback Configuration [CONFIG]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_config B.3a CONFIG — Output Callback
 * @ingroup cpisi_json_impl_coreops
 * @brief    Set the output callback for JSON responses.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Set the output callback for JSON responses.
 *
 * Allows redirection of JSON output to different targets:
 *   - NULL: Restores default stdout output
 *   - Custom function: Socket, buffer, or other target
 *
 * @param[in] fn  Output function to use (NULL restores default stdout).
 *
 * @warning Not thread-safe. Set once at startup before any JSON output.
 *
 * @par DAR ASSESS
 *   Evaluates output configuration — where JSON will go.
 */
void cpisi_json_set_output(CpisiJsonOutputFn fn) {
    g_json_output = fn;
}

/** @} */ /* end B.3a CONFIG */

/** @} */ /* end B.3 CORE OPS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 ERRORS — Error Handling Patterns [ERRORS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_errors B.4 ERRORS — Error Handling
 * @brief    [Reserved: Errors handled via cpisi_json_error() function]
 *
 * Error Patterns:
 *   - NULL pointer: Call cpisi_json_error() with appropriate code
 *   - Invalid enum: Return "unknown" from lookup functions
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ─────────────────────────────────────────────────────────────────────────────
/// CODE SECTIONS [CODE_SECTIONS]
/// ─────────────────────────────────────────────────────────────────────────────

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Exported Interface Implementations [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_api B.5 PUBLIC API — Exported Interface
 * @brief    Public API implementations matching json.h declarations.
 *
 * CONTAINS:
 *   - B.5a ESCAPE    — String escaping
 *   - B.5b GAME      — Game state output
 *   - B.5c CUBE      — Cube state output
 *   - B.5d COGNITIVE — Thought processing output
 *   - B.5e STATUS    — Protocol status responses
 *   - B.5f STATE     — Cognitive state serialization
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5a ESCAPE — String Escaping [ESCAPE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_escape B.5a ESCAPE — String Escaping
 * @ingroup cpisi_json_impl_api
 * @brief    JSON string escaping implementation.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Escape a string for JSON output.
 *
 * Escapes special characters that have meaning in JSON:
 *   - Double quote (") → \"
 *   - Backslash (\) → \\
 *   - Newline → \n
 *   - Carriage return → \r
 *   - Tab → \t
 *
 * @param[out] buf       Output buffer for escaped string.
 * @param[in]  buf_size  Size of output buffer.
 * @param[in]  src       Source string to escape.
 *
 * @return Number of characters written (excluding null terminator).
 *
 * @par Safety
 *   Leaves JSON_ESCAPE_SAFETY margin at end of buffer.
 *   Always null-terminates output.
 */
int cpisi_json_escape(char* buf, size_t buf_size, const char* src) {
    if (!src || buf_size < 1) {
        return 0;
    }

    int pos = 0;
    int limit = (int)buf_size - JSON_ESCAPE_SAFETY;

    for (const char* p = src; *p && pos < limit; p++) {
        if (*p == '"') {
            buf[pos++] = '\\';
            buf[pos++] = '"';
        } else if (*p == '\\') {
            buf[pos++] = '\\';
            buf[pos++] = '\\';
        } else if (*p == '\n') {
            buf[pos++] = '\\';
            buf[pos++] = 'n';
        } else if (*p == '\r') {
            buf[pos++] = '\\';
            buf[pos++] = 'r';
        } else if (*p == '\t') {
            buf[pos++] = '\\';
            buf[pos++] = 't';
        } else {
            buf[pos++] = *p;
        }
    }

    buf[pos] = '\0';
    return pos;
}

/** @} */ /* end B.5a ESCAPE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5b GAME — Game State Output [GAME]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_game B.5b GAME — Game State Output
 * @ingroup cpisi_json_impl_api
 * @brief    JSON responses for game/story engine state.
 *
 * WHY: Story engine needs structured output for scene navigation, text display,
 *      and choice presentation. JSON enables UI decoupling from engine.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Output current game state as JSON.
 *
 * Queries scene system for current state and outputs structured JSON:
 *   - Scene type
 *   - Act and line numbers
 *   - Current text
 *   - Available choices
 *
 * Format: {"status":"ok","scene":"...","act":N,"line":N,"text":"...","choices":[]}
 *
 * @par DAR DETECT
 *   Witnesses game state for output.
 */
void cpisi_json_state(void) {
    char buf[JSON_BUF_LARGE];
    char text_escaped[JSON_BUF_TEXT];
    char choice_escaped[JSON_BUF_CHOICE];

    SceneType scene = scene_get_current();
    Manifest* m = engine_get_manifest();
    int32_t act = m ? manifest_get_current_act(m) : 0;
    int32_t line = scene_get_current_line();
    const char* text = scene_get_current_text();
    int32_t choice_count = scene_get_choice_count();

    /* For combat scene, get combat-specific info */
    bool combat_active = scene_get_combat_active();
    const char* combat_msg = "";
    const char* enemy_name = "";
    int32_t player_hp = 0, player_max_hp = 0;
    int32_t enemy_hp = 0, enemy_max_hp = 0;
    bool player_turn = false;

    if (scene == SCENE_COMBAT && combat_active) {
        combat_msg = scene_get_combat_message();
        enemy_name = scene_get_enemy_name();
        scene_get_player_hp(&player_hp, &player_max_hp);
        scene_get_enemy_hp(&enemy_hp, &enemy_max_hp);
        player_turn = scene_is_player_turn();
        /* Use combat message as text for COMBAT scene */
        text = combat_msg;
    }

    /* Escape text for JSON */
    cpisi_json_escape(text_escaped, sizeof(text_escaped), text ? text : "");

    /* Build JSON response */
    int pos;
    if (scene == SCENE_COMBAT && combat_active) {
        char enemy_escaped[JSON_BUF_CHOICE];
        cpisi_json_escape(enemy_escaped, sizeof(enemy_escaped), enemy_name ? enemy_name : "");

        pos = snprintf(buf, sizeof(buf),
            "{\"status\":\"ok\",\"scene\":\"%s\",\"act\":%d,\"line\":%d,"
            "\"text\":\"%s\",\"choices\":[],"
            "\"combat\":{\"active\":true,\"player_turn\":%s,"
            "\"player_hp\":%d,\"player_max_hp\":%d,"
            "\"enemy_hp\":%d,\"enemy_max_hp\":%d,"
            "\"enemy_name\":\"%s\"}}",
            cpisi_get_scene_name(scene), act, line, text_escaped,
            player_turn ? "true" : "false",
            player_hp, player_max_hp, enemy_hp, enemy_max_hp, enemy_escaped);

        /* Combat output is complete, skip choices loop */
        buf[pos++] = '\n';
        buf[pos] = '\0';
        json_output(buf);
        return;
    }

    /* Check for menu options (works for LAUNCHER, GAME_MENU, PAUSE, etc.) */
    int32_t option_count = scene_get_option_count();
    int32_t selected_option = scene_get_selected_option();

    pos = snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"scene\":\"%s\",\"act\":%d,\"line\":%d,"
        "\"text\":\"%s\"",
        cpisi_get_scene_name(scene), act, line, text_escaped);

    /* Add menu options if this is a menu scene */
    if (option_count > 0) {
        pos += snprintf(buf + pos, sizeof(buf) - pos,
            ",\"selected\":%d,\"options\":[", selected_option);

        for (int32_t i = 0; i < option_count && pos < (int)sizeof(buf) - JSON_BUF_SAFETY; i++) {
            const char* option = scene_get_option_text(i);
            cpisi_json_escape(choice_escaped, sizeof(choice_escaped), option ? option : "");

            if (i > 0) {
                buf[pos++] = ',';
            }
            pos += snprintf(buf + pos, sizeof(buf) - pos, "\"%s\"", choice_escaped);
        }
        pos += snprintf(buf + pos, sizeof(buf) - pos, "]");
    }

    /* Add dialogue choices if available */
    pos += snprintf(buf + pos, sizeof(buf) - pos, ",\"choices\":[");
    for (int32_t i = 0; i < choice_count && pos < (int)sizeof(buf) - JSON_BUF_SAFETY; i++) {
        const char* choice = scene_get_choice_text(i);
        cpisi_json_escape(choice_escaped, sizeof(choice_escaped), choice ? choice : "");

        if (i > 0) {
            buf[pos++] = ',';
        }
        pos += snprintf(buf + pos, sizeof(buf) - pos, "\"%s\"", choice_escaped);
    }

    snprintf(buf + pos, sizeof(buf) - pos, "]}\n");
    json_output(buf);
}

/**
 * @brief Output PTY subprocess screen as JSON.
 *
 * Used when CPI-SI is wrapping a terminal application.
 * Outputs the screen buffer as an array of line strings.
 *
 * @param[in] lines       Array of screen line strings.
 * @param[in] line_count  Number of lines.
 *
 * Format: {"status":"ok","mode":"pty","screen":["line1","line2",...]}
 */
void cpisi_json_state_pty(const char** lines, int line_count) {
    char buf[JSON_BUF_LARGE];
    char text_escaped[JSON_BUF_CHOICE];

    int pos = snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"mode\":\"pty\",\"screen\":[");

    for (int row = 0; row < line_count && pos < (int)sizeof(buf) - JSON_BUF_SAFETY; row++) {
        cpisi_json_escape(text_escaped, sizeof(text_escaped), lines[row]);

        if (row > 0) {
            buf[pos++] = ',';
        }
        pos += snprintf(buf + pos, sizeof(buf) - pos, "\"%s\"", text_escaped);
    }

    snprintf(buf + pos, sizeof(buf) - pos, "]}\n");
    json_output(buf);
}

/**
 * @brief Output available choices as JSON.
 *
 * Queries scene system for available choices and outputs structured JSON.
 *
 * Format: {"status":"ok","count":N,"choices":[{"index":1,"text":"..."},...]
 */
void cpisi_json_choices(void) {
    char buf[JSON_BUF_LARGE];
    char choice_escaped[JSON_BUF_CHOICE];
    int32_t count = scene_get_choice_count();

    int pos = snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"count\":%d,\"choices\":[", count);

    for (int32_t i = 0; i < count && pos < (int)sizeof(buf) - JSON_BUF_SAFETY; i++) {
        const char* choice = scene_get_choice_text(i);
        cpisi_json_escape(choice_escaped, sizeof(choice_escaped), choice ? choice : "");

        if (i > 0) {
            buf[pos++] = ',';
        }
        pos += snprintf(buf + pos, sizeof(buf) - pos,
            "{\"index\":%d,\"text\":\"%s\"}", i + 1, choice_escaped);
    }

    snprintf(buf + pos, sizeof(buf) - pos, "]}\n");
    json_output(buf);
}

/** @} */ /* end B.5b GAME */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5c CUBE — Cube State Output [CUBE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_cube B.5c CUBE — Cube State Output
 * @ingroup cpisi_json_impl_api
 * @brief    JSON responses for CPI-SI mental construct (27-cube).
 *
 * WHY: The 27-cube is the core cognitive model. External systems need
 *      visibility into cube position, available keys, and Hebrew state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Output cube state as JSON.
 *
 * Outputs comprehensive cube state including:
 *   - Position (x, y, z)
 *   - Hebrew state name
 *   - Response style (tone, depth, directness, confidence, verbosity)
 *   - Recommended key
 *   - Centered status
 *
 * @param[in] cube  The CPI-SI cube (NULL returns error JSON).
 *
 * @par DAR DETECT
 *   Witnesses cube state for external visibility.
 */
void cpisi_json_cube_state(CPISICube* cube) {
    if (cube == NULL) {
        cpisi_json_error("CUBE_NOT_INIT", "CPI-SI cube not initialized");
        return;
    }

    char buf[JSON_BUF_MEDIUM];
    CPISIPosition pos = cpisi_position(cube);
    CPISIResponseStyle style = cpisi_response_style(cube);
    DARHebrewState hebrew = cpisi_hebrew_state(cube);
    int8_t rec = cpisi_recommended_key(cube);

    const char* hebrew_name = get_hebrew_state_name(hebrew);

    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"cube\":{"
        "\"position\":[%d,%d,%d],"
        "\"hebrew\":\"%s\","
        "\"style\":{\"tone\":%d,\"depth\":%d,\"directness\":%d,"
        "\"confidence\":%d,\"verbosity\":%d},"
        "\"recommended_key\":%d,"
        "\"centered\":%s}}\n",
        pos.x, pos.y, pos.z,
        hebrew_name,
        style.tone, style.depth, style.directness,
        style.confidence, style.verbosity,
        rec,
        cpisi_is_centered(cube) ? "true" : "false");

    json_output(buf);
}

/**
 * @brief Output available keys as JSON.
 *
 * Outputs the three keys available at current cube position.
 *
 * @param[in] cube  The CPI-SI cube (NULL returns error JSON).
 *
 * Format: {"status":"ok","keys":[{"direction":N,"weight":N,"label":"..."},...]
 */
void cpisi_json_cube_keys(CPISICube* cube) {
    if (cube == NULL) {
        cpisi_json_error("CUBE_NOT_INIT", "CPI-SI cube not initialized");
        return;
    }

    char buf[JSON_BUF_SMALL];
    const CPISIKey* keys = cpisi_get_keys(cube);

    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"keys\":["
        "{\"direction\":%d,\"weight\":%d,\"label\":\"%s\"},"
        "{\"direction\":%d,\"weight\":%d,\"label\":\"%s\"},"
        "{\"direction\":%d,\"weight\":%d,\"label\":\"%s\"}]}\n",
        keys[0].direction, keys[0].weight, keys[0].label,
        keys[1].direction, keys[1].weight, keys[1].label,
        keys[2].direction, keys[2].weight, keys[2].label);

    json_output(buf);
}

/**
 * @brief Output cube action result as JSON.
 *
 * Reports the result of a cube navigation action.
 *
 * @param[in] action  Action name ("expand", "retreat", "center").
 * @param[in] x       New X position after action.
 * @param[in] y       New Y position after action.
 * @param[in] z       New Z position after action.
 *
 * Format: {"status":"ok","action":"...","position":[x,y,z]}
 */
void cpisi_json_cube_action(const char* action, int x, int y, int z) {
    char buf[JSON_BUF_CHOICE];

    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"action\":\"%s\",\"position\":[%d,%d,%d]}\n",
        action ? action : "unknown", x, y, z);

    json_output(buf);
}

/** @} */ /* end B.5c CUBE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5d COGNITIVE — Thought Processing Output [COGNITIVE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_cognitive B.5d COGNITIVE — Thought Processing
 * @ingroup cpisi_json_impl_api
 * @brief    JSON responses for thought processing and pattern matching.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Output thinking result as JSON.
 *
 * Reports the result of processing input through the cognitive model.
 *
 * @param[in] input       The input text.
 * @param[in] state_name  Hebrew state name.
 * @param[in] tone        Tone value (-1 to +1).
 * @param[in] depth       Depth value (-1 to +1).
 * @param[in] directness  Directness value (-1 to +1).
 * @param[in] confidence  Confidence value (-1 to +1).
 * @param[in] verbosity   Verbosity value (-1 to +1).
 *
 * @par DAR ASSESS
 *   Evaluates cognitive processing result.
 */
void cpisi_json_thought(const char* input, const char* state_name,
                        int tone, int depth, int directness,
                        int confidence, int verbosity) {
    char buf[JSON_BUF_SMALL];
    char input_escaped[JSON_BUF_SHORT];

    cpisi_json_escape(input_escaped, sizeof(input_escaped), input ? input : "");

    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"thought\":{"
        "\"input\":\"%.100s\","
        "\"state\":\"%s\","
        "\"style\":{\"tone\":%d,\"depth\":%d,\"directness\":%d,"
        "\"confidence\":%d,\"verbosity\":%d}}}\n",
        input_escaped,
        state_name ? state_name : "unknown",
        tone, depth, directness, confidence, verbosity);

    json_output(buf);
}

/**
 * @brief Output pattern response as JSON.
 *
 * Reports a pattern-matched response.
 *
 * @param[in] pattern_name  Name of pattern used.
 * @param[in] output        Generated output text.
 */
void cpisi_json_response(const char* pattern_name, const char* output) {
    char buf[JSON_BUF_MEDIUM];
    char output_escaped[JSON_BUF_SMALL];

    cpisi_json_escape(output_escaped, sizeof(output_escaped), output ? output : "");

    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"response\":{"
        "\"pattern\":\"%s\","
        "\"output\":\"%.500s\"}}\n",
        pattern_name ? pattern_name : "unknown",
        output_escaped);

    json_output(buf);
}

/** @} */ /* end B.5d COGNITIVE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5e STATUS — Protocol Status Responses [STATUS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_status B.5e STATUS — Protocol Status Responses
 * @ingroup cpisi_json_impl_api
 * @brief    Simple status messages for protocol communication.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Output a simple success response.
 *
 * @param[in] extra  Optional extra JSON fields (without braces), or NULL.
 *
 * Format without extra: {"status":"ok"}
 * Format with extra: {"status":"ok",...extra...}
 */
void cpisi_json_ok(const char* extra) {
    char buf[JSON_BUF_SMALL];

    if (extra && *extra) {
        /* Limit extra to leave room for wrapper: {"status":"ok",%s}\n = 17 chars */
        char extra_safe[JSON_BUF_SMALL - 20];
        snprintf(extra_safe, sizeof(extra_safe), "%s", extra);
        snprintf(buf, sizeof(buf), "{\"status\":\"ok\",%s}\n", extra_safe);
    } else {
        snprintf(buf, sizeof(buf), "{\"status\":\"ok\"}\n");
    }

    json_output(buf);
}

/**
 * @brief Output an error message as JSON.
 *
 * Standard error format for all JSON error responses.
 *
 * @param[in] code     Error code string (e.g., "NOT_FOUND", "INVALID_INPUT").
 * @param[in] message  Human-readable error message.
 *
 * Format: {"status":"error","code":"...","message":"..."}
 *
 * @par DAR RESTORE
 *   Provides structured error for recovery by caller.
 */
void cpisi_json_error(const char* code, const char* message) {
    char buf[JSON_BUF_MEDIUM];
    char code_safe[JSON_BUF_MINI];
    char msg_safe[JSON_BUF_CHOICE];

    /* Truncate inputs to prevent buffer overflow */
    snprintf(code_safe, sizeof(code_safe), "%s", code ? code : "ERROR");
    snprintf(msg_safe, sizeof(msg_safe), "%s", message ? message : "Unknown error");

    snprintf(buf, sizeof(buf),
        "{\"status\":\"error\",\"code\":\"%s\",\"message\":\"%s\"}\n",
        code_safe, msg_safe);

    json_output(buf);
}

/**
 * @brief Output ready status as JSON.
 *
 * Indicates system is ready and what mode it's in.
 *
 * @param[in] mode_name  Name of the mode (REPL, Socket, etc.).
 *
 * Format: {"status":"ready","mode":"..."}
 */
void cpisi_json_ready(const char* mode_name) {
    char buf[JSON_BUF_SHORT];

    snprintf(buf, sizeof(buf),
        "{\"status\":\"ready\",\"mode\":\"%s\"}\n",
        mode_name ? mode_name : "Unknown");

    json_output(buf);
}

/**
 * @brief Output completion status as JSON.
 *
 * Indicates operation or session completion.
 *
 * @param[in] message  Completion message.
 *
 * Format: {"status":"complete","message":"..."}
 */
void cpisi_json_complete(const char* message) {
    char buf[JSON_BUF_CHOICE];

    snprintf(buf, sizeof(buf),
        "{\"status\":\"complete\",\"message\":\"%s\"}\n",
        message ? message : "complete");

    json_output(buf);
}

/**
 * @brief Output goodbye status as JSON.
 *
 * Final message before session termination.
 *
 * Format: {"status":"goodbye"}
 */
void cpisi_json_goodbye(void) {
    json_output("{\"status\":\"goodbye\"}\n");
}

/**
 * @brief Output command help as JSON.
 *
 * Provides help text for commands.
 *
 * @param[in] command    Command name (reserved for future command-specific help).
 * @param[in] help_text  Help text for the command.
 *
 * Format: {"status":"ok","help":"..."}
 */
void cpisi_json_help(const char* command, const char* help_text) {
    (void)command;  /* Reserved for future use (command-specific help) */

    char buf[JSON_BUF_CHOICE];

    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"help\":\"%s\"}\n",
        help_text ? help_text : "No help available");

    json_output(buf);
}

/** @} */ /* end B.5e STATUS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5f STATE — Cognitive State Serialization [STATE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_json_impl_state_serial B.5f STATE — Cognitive State Serialization
 * @ingroup cpisi_json_impl_api
 * @brief    Serialize cognitive state from types.h structures.
 *
 * WHY: Complements cube output with detailed state info. External systems
 *      may need Hebrew state semantics or multi-scale health representation.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Output Hebrew state details as JSON.
 *
 * Provides detailed Hebrew state information including:
 *   - Numeric value (-3 to +3)
 *   - Hebrew name
 *   - K-factor direction interpretation
 *   - Position in 7-state spectrum
 *
 * @param[in] state  DARHebrewState value from omni_hebrew.h (-3 to +3).
 *
 * Format: {"status":"ok","hebrew":{"value":N,"name":"...","direction":"..."}}
 *
 * @par Hebrew State Mapping
 *   - shavar (-3): Broken — crisis state
 *   - chaser (-2): Lacking — regressing (k=-1)
 *   - ratsah (-1): Wanting — hungry for growth (k=+1)
 *   - yashar (0):  Even — center/default
 *   - tamim (+1):  Sound — consolidating (k=-1)
 *   - shalem (+2): Whole — expanding (k=+1)
 *   - tov (+3):    Perfect — completion state
 */
void cpisi_json_hebrew_state(DARHebrewState state) {
    char buf[JSON_BUF_SMALL];

    const char* name = get_hebrew_state_name(state);

    /* Determine direction interpretation (0-indexed HebrewState)
     * HEBREW_SHAVAR (0) = crisis/breaking
     * HEBREW_CHASER/RATSAH (1,2) = lacking/hungry
     * HEBREW_YASHAR (3) = centered
     * HEBREW_TAMIM/SHALEM (4,5) = building
     * HEBREW_TOV (6) = complete */
    const char* direction;
    if (state == HEBREW_SHAVAR) {
        direction = "breaking";  /* Crisis state */
    } else if (state <= HEBREW_RATSAH) {
        direction = "hungry";    /* Wanting growth */
    } else if (state == HEBREW_YASHAR) {
        direction = "centered";  /* At rest */
    } else if (state <= HEBREW_SHALEM) {
        direction = "building";  /* Sound, growing */
    } else {
        direction = "complete";  /* TOV - complete */
    }

    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"hebrew\":{"
        "\"value\":%d,"
        "\"name\":\"%s\","
        "\"direction\":\"%s\","
        "\"min\":-3,"
        "\"max\":3,"
        "\"center\":0}}\n",
        state, name, direction);

    json_output(buf);
}

/**
 * @brief Output health information as JSON.
 *
 * Provides comprehensive health information including:
 *   - Raw score (0-100)
 *   - Level (7 levels: critical → optimal)
 *   - Multi-scale representation (if provided)
 *
 * @param[in] health  HealthScore value (0-100).
 * @param[in] level   HealthLevel value (7 levels).
 * @param[in] score   MultiScore pointer (optional, can be NULL).
 *
 * Format: {"status":"ok","health":{"score":N,"level":"...","scales":{...}}}
 *
 * @par Health Level Mapping
 *   - critical: 0-14    — Immediate intervention needed
 *   - poor:     15-29   — Significant problems
 *   - low:      30-44   — Below normal
 *   - fair:     45-59   — Acceptable
 *   - good:     60-74   — Normal operation
 *   - excellent: 75-89  — Above normal
 *   - optimal:  90-100  — Peak performance
 */
void cpisi_json_health(HealthScore health, HealthLevel level, const MultiScore* score) {
    char buf[JSON_BUF_MEDIUM];

    const char* level_name = get_health_level_name(level);

    if (score != NULL) {
        /* Include multi-scale representation */
        snprintf(buf, sizeof(buf),
            "{\"status\":\"ok\",\"health\":{"
            "\"score\":%d,"
            "\"level\":\"%s\","
            "\"min\":0,"
            "\"max\":100,"
            "\"scales\":{"
            "\"base3\":%d,"
            "\"base5\":%d,"
            "\"base10\":%d,"
            "\"base20\":%d}}}\n",
            health, level_name,
            score->base3, score->base5, score->base10, score->base20);
    } else {
        /* Basic health info */
        snprintf(buf, sizeof(buf),
            "{\"status\":\"ok\",\"health\":{"
            "\"score\":%d,"
            "\"level\":\"%s\","
            "\"min\":0,"
            "\"max\":100}}\n",
            health, level_name);
    }

    json_output(buf);
}

/** @} */ /* end B.5f STATE */

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_b6_omission B.6 OMISSION — Reserved BODY Sections
 * @brief    Documents what BODY sections are intentionally reserved.
 *
 * | Section | Status | Reason |
 * |---------|--------|--------|
 * | B.4 ERRORS | Reserved | Uses cpisi_json_error function |
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

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
/// CONTAINS:
///   - X.1 VALIDATION      — Verify correctness
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
 * @defgroup cpisi_json_impl_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build and test verification.
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * gcc -c -Wall -Wextra cpisi/io/json.c -o json.o
 * @endcode
 *
 * @par X.1b SELFTEST — Minimal Isolation Test
 * @code{.c}
 * #include "kernel/cpisi/format/json.fmt.h"
 * int main(void) { cpisi_json_ok(NULL); return 0; }
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Validation documented in defgroup above] */

/** @} */ /* end X.1 VALIDATION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Examples [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_execution X.2 EXECUTION — Usage Examples
 * @brief    How to use JSON output functions.
 *
 * @par Basic Usage
 * @code{.c}
 * #include "kernel/cpisi/format/json.fmt.h"
 *
 * // Simple status
 * cpisi_json_ok(NULL);
 * // Output: {"status":"ok"}
 *
 * // Error response
 * cpisi_json_error("NOT_FOUND", "File not found");
 * // Output: {"status":"error","code":"NOT_FOUND","message":"File not found"}
 *
 * // Cube state
 * cpisi_json_cube_state(my_cube);
 * // Output: {"status":"ok","cube":{...}}
 *
 * // Hebrew state details
 * cpisi_json_hebrew_state(DAR_YASHAR);
 * // Output: {"status":"ok","hebrew":{"value":0,"name":"yashar",...}}
 * @endcode
 *
 * @par Custom Output Target
 * @code{.c}
 * void my_socket_output(const char* msg) {
 *     send(socket_fd, msg, strlen(msg), 0);
 * }
 *
 * cpisi_json_set_output(my_socket_output);
 * // All JSON now goes to socket
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Execution documented in defgroup above] */

/** @} */ /* end X.2 EXECUTION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Shutdown Patterns [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_cleanup X.3 CLEANUP — Shutdown Patterns
 * @brief    Cleanup and shutdown procedures.
 *
 * CLEANUP ORDER:
 *   1. Send cpisi_json_goodbye() if appropriate
 *   2. Restore default output: cpisi_json_set_output(NULL)
 *
 * WHY: No heap allocations in JSON module, so no explicit cleanup needed.
 *      The output callback is stateless — just reset if needed.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Cleanup documented in defgroup above] */

/** @} */ /* end X.3 CLEANUP */

/// ─────────────────────────────────────────────────────────────────────────────
/// END CODE SECTIONS [END_CODE_SECTIONS]
/// ─────────────────────────────────────────────────────────────────────────────

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidelines [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_policy X.4 POLICY — Modification Guidelines
 * @brief    Guidelines for modifying this component.
 *
 * DO:
 *   - Add new JSON output functions in appropriate B.5 subsection
 *   - Use JSON_BUF_* defines for buffer sizes
 *   - Document with full Doxygen docstring
 *   - Route all output through json_output()
 *   - Escape user-provided strings with cpisi_json_escape()
 *
 * DO NOT:
 *   - Use magic numbers for buffer sizes
 *   - Output directly to stdout (use json_output())
 *   - Add heap allocations (keep it simple)
 *   - Assume thread safety (document if changing)
 *
 * VERSION UPDATES:
 *   - Bug fixes: increment completion (b-03.25 → b-03.26)
 *   - New functions: increment phase (b-03.25 → b-04.00)
 *   - Breaking changes: increment stage (b-03.25 → c-01.00)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Policy documented in defgroup above] */

/** @} */ /* end X.4 POLICY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_extension X.5 EXTENSION — How to Expand
 * @brief    How to add new JSON output functions.
 *
 * ADDING A NEW JSON OUTPUT FUNCTION:
 *   1. Add declaration to json.h in appropriate B.5 subsection
 *   2. Add implementation here in matching B.5 subsection
 *   3. Use JSON_BUF_* defines for buffer sizes
 *   4. Call json_output() for final output
 *   5. Update B.1c COUNTS
 *
 * ADDING A NEW LOOKUP:
 *   1. Add helper function in B.2b LOOKUP
 *   2. Add prototype in S.4a HELPERS
 *   3. Use static array for lookup table
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Extension documented in defgroup above] */

/** @} */ /* end X.5 EXTENSION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @brief    Solutions to common issues.
 *
 * PROBLEM: JSON output goes nowhere
 *   CAUSE: Output callback set to function that doesn't work
 *   FIX: Call cpisi_json_set_output(NULL) to restore stdout
 *
 * PROBLEM: Truncated JSON output
 *   CAUSE: Input string too long for escape buffer
 *   FIX: Increase appropriate JSON_BUF_* define
 *
 * PROBLEM: Invalid JSON (unescaped characters)
 *   CAUSE: User string not passed through cpisi_json_escape()
 *   FIX: Always escape user-provided strings before including in JSON
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Troubleshooting documented in defgroup above] */

/** @} */ /* end X.6 TROUBLESHOOTING */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick reference for common values.
 *
 * BUFFER SIZES:
 *   JSON_BUF_LARGE    = 4096  (complex output)
 *   JSON_BUF_MEDIUM   = 1024  (standard responses)
 *   JSON_BUF_SMALL    =  512  (simple responses)
 *   JSON_BUF_TEXT     = 2048  (escaped text)
 *   JSON_BUF_CHOICE   =  256  (escaped choices)
 *
 * HEBREW STATES:
 *   -3: shavar (broken)
 *   -2: chaser (lacking)
 *   -1: ratsah (wanting)
 *    0: yashar (even)
 *   +1: tamim (sound)
 *   +2: shalem (whole)
 *   +3: tov (perfect)
 *
 * HEALTH LEVELS:
 *   0: critical (0-14)
 *   1: poor (15-29)
 *   2: low (30-44)
 *   3: fair (45-59)
 *   4: good (60-74)
 *   5: excellent (75-89)
 *   6: optimal (90-100)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reference documented in defgroup above] */

/** @} */ /* end X.7 REFERENCE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Final Grounding [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_json_impl_note X.8 NOTE — Final Grounding
 * @brief    Closing wisdom and biblical anchor.
 *
 * "A word fitly spoken is like apples of gold in pictures of silver."
 *   — Proverbs 25:11 KJV
 *
 * JSON is the silver frame. Cognitive state is the golden content.
 * Structure enables understanding. Format serves communication.
 *
 * This module ensures CPI-SI state is presented in receivable form —
 * structured for machines, meaningful for systems, traceable for debugging.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Note documented in defgroup above] */

/** @} */ /* end X.8 NOTE */

/**
 * X.9 TEMPLATE — [Reserved: this is an instance]
 * @defgroup cpisi_json_impl_template X.9 TEMPLATE @{ @}
 */

/**
 * X.10 OMISSION — [Reserved: see S.6 OMISSION]
 * @defgroup cpisi_json_impl_x10_omission X.10 OMISSION @{ @}
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_MODE >= 2 - Full JSON format implementation */
