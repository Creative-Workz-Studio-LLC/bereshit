/// @file json.h
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
/// @omni:req  key        = CPISI-IO-JSON
/// @omni:req  from       = bereshit/word/seed/code/c/header.h
/// @omni:req  at         = b-03.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = io
/// @omni:inh  role       = serialization
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = io
/// @omni:ins  includes   = [types/types.h, kernel/types/composite/type/health.type.h, omni_cpisi.h]
/// @omni:ins  provides   = [CpisiJsonOutputFn, json_serialization, state_output]
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
/// @omni:req  title      = CPI-SI JSON Response Builders
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
/// ═══════════════════════════════════════════════════════════════════════════════

/** SP.1 CONFIG — [Reserved: no POSIX features needed] @defgroup cpisi_io_json_special @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * INCLUDE GUARD — Identity protection
 * ───────────────────────────────────────────────────────────────────────────── */

#ifndef CPISI_IO_JSON_H
/** @def CPISI_IO_JSON_H
 *  @brief Include guard — prevents multiple inclusion of this JSON header.
 */
#define CPISI_IO_JSON_H

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
 * @file      json.h
 * @brief     CPI-SI JSON Response Builders — structured serialization of cognitive state.
 *
 * @defgroup cpisi_io_json_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-IO-JSON
 * Title:     CPI-SI JSON Response Builders
 * Type:      Header (IO)
 * Component: Serialization — JSON output for CPI-SI state
 * Role:      Game state, cube state, cognitive state → JSON
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_state M.2 STATE — Lifecycle State
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
 * M.3 ATTRIBUTION — Authors and Copyright [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_io_json_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/io/json.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Template:     bereshit/word/seed/code/c/header.h
 * Derives_from: types/types.h (HebrewState, HealthScore, etc.)
 *
 * Builds FROM:  types/types.h (cognitive types), omni_cpisi.h (CPISICube)
 * Builds TO:    cpisi.h (uses JSON for status reporting)
 *
 * Architecture: types/ → io/json.h → cpisi.h (serialization layer)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: io, json, serialization, output, state, cube, cognitive
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide JSON serialization for CPI-SI cognitive state and output.
 * Core Design: Structured output enables machine parsing — "a word fitly spoken."
 * Philosophy:  The structure (JSON) presents content in receivable form.
 *
 * Key Features:
 *   - Game state output (scene, choices, text)
 *   - Cube state output (position, keys, Hebrew state)
 *   - Cognitive state serialization
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
 * @defgroup cpisi_io_json_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Proverbs 25:11 — "A word fitly spoken is like apples of gold
 *             in pictures of silver"
 * Principle:  Structure (silver frame) presents content (golden apples) properly.
 * Anchor:     Genesis 11:1 — "The whole earth was of one language"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types/types.h (HebrewState, KFactor, MultiScore)
 *   Internal: kernel/types/composite/type/health.type.h (HealthScore, HealthLevel)
 *   Internal: omni_cpisi.h (CPISICube for cube state serialization)
 *   Standard: stddef.h, stdint.h, stdbool.h
 *
 * What Uses This:
 *   - cpisi.h — unified orchestrator (status reporting)
 *   - REPL mode — JSON output for interactive sessions
 *   - Socket mode — JSON protocol for external clients
 *
 * @see types/types.h for cognitive type definitions
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-20) — Initial creation
 *   a-02.00 (2026-01-22) — Added cognitive state serialization
 *   b-03.00 (2026-01-25) — Full header.h template alignment
 *
 * Current:
 *   ✓ Game state JSON output
 *   ✓ Cube state JSON output
 *   ✓ Cognitive state serialization
 *   ✓ Configurable output target
 *
 * Planned:
 *   ⏳ Streaming JSON for large outputs
 *   ⏳ JSON schema validation
 *
 * @} end M.10 ROADMAP
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END METADATA [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * CODE SECTIONS [CODE_SECTIONS]
 * ───────────────────────────────────────────────────────────────────────────── */

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
///   - S.2 DEFINES   — [Reserved]
///   - S.3 TYPES     — CpisiJsonOutputFn callback type
///   - S.4 PROTOS    — [Reserved]
///   - S.5 FILE-LEVEL — [Reserved]
///   - S.6 OMISSION  — Reserved sections
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_json_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_io_json_includes
 * @brief    C standard library headers required by this module.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include <stdbool.h>  /**< Boolean type: bool, true, false */
#include <stddef.h>   /**< Standard definitions: size_t, NULL */
#include <stdint.h>   /**< Fixed-width integers: int32_t */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_json_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_io_json_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * WHY: JSON serialization needs types to serialize (HebrewState, HealthScore,
 *      CPISICube). These come from types.h and omni_cpisi.h.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel/types/types.h"                    /**< HebrewState, KFactor, MultiScore */
#include "kernel/types/composite/type/health.type.h"  /**< HealthScore, HealthLevel */

/**
 * @brief CPISICube from Bereshit/OmniCode — the complete mental construct.
 *
 * This provides the full 27-cube implementation for state serialization.
 * The path reaches into the Bereshit word/work/pkg structure.
 */
#include "omni_cpisi.h"

/** @} */ /* end S.1b INTERNAL */

/** S.1c EXTERNAL — [Reserved: no external libraries] @defgroup cpisi_io_json_external @ingroup cpisi_io_json_includes @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_json_cpp S.1d CPP — C++ Linkage
 * @ingroup cpisi_io_json_includes
 * @brief    C++ compatibility wrapper.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#ifdef __cplusplus
extern "C" {
#endif

/** @} */ /* end S.1d CPP */

/** @} */ /* end S.1 INCLUDES */

/** S.2 DEFINES — [Reserved: no preprocessor constants] @defgroup cpisi_io_json_defines @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_types S.3 TYPES — Type Declarations
 * @brief    Output callback type for JSON serialization.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a CALLBACKS — Output Function Type [CALLBACKS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_json_callbacks S.3a CALLBACKS — Output Function Type
 * @ingroup cpisi_io_json_types
 * @brief    Callback type for configurable JSON output target.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Output callback type for JSON responses.
 *
 * Allows different output targets:
 *   - stdout (default)
 *   - Socket connection
 *   - Buffer capture for testing
 *
 * @param msg The JSON string to output (null-terminated).
 */
typedef void (*CpisiJsonOutputFn)(const char* msg);

/** @} */ /* end S.3a CALLBACKS */

/** S.3b ENUMS — [Reserved: no enums] @defgroup cpisi_io_json_enums @ingroup cpisi_io_json_types @{ @} */
/** S.3c STRUCTS — [Reserved: no structs] @defgroup cpisi_io_json_structs @ingroup cpisi_io_json_types @{ @} */
/** S.3d ERRORS — [Reserved: uses cpisi_json_error function] @defgroup cpisi_io_json_errors @ingroup cpisi_io_json_types @{ @} */

/** @} */ /* end S.3 TYPES */

/** S.4 PROTOTYPES — [Reserved: prototypes in BODY] @defgroup cpisi_io_json_protos @{ @} */
/** S.5 FILE-LEVEL — [Reserved: no global state] @defgroup cpisi_io_json_file @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_s6_omission S.6 OMISSION — Excluded Content
 * @brief    Documents what SETUP sections are intentionally reserved.
 *
 * | Section | Status | Reason |
 * |---------|--------|--------|
 * | S.1c EXTERNAL | Reserved | No external libraries |
 * | S.2 DEFINES | Reserved | No preprocessor constants |
 * | S.3b-d | Reserved | No enums, structs, error types |
 * | S.4 PROTOTYPES | Reserved | Prototypes in BODY |
 * | S.5 FILE-LEVEL | Reserved | No global state |
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: S.6 is documentation-only] */

/** @} */ /* end S.6 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief The actual work — JSON serialization functions.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART   — Structure overview
///   - B.2 HELPERS     — String utilities
///   - B.3 CORE OPS    — Output configuration
///   - B.4 ERRORS      — [Reserved]
///   - B.5 PUBLIC API  — JSON output functions
///   - B.6 OMISSION    — Reserved sections
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_orgchart B.1 ORG CHART — Structure Overview
 * @brief    Map structure — types, functions, file organization.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Interface Overview [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_json_structure B.1a STRUCTURE — Interface Overview
 * @ingroup cpisi_io_json_orgchart
 * @brief    Types, functions, and externs declared in this header.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      1 callback (CpisiJsonOutputFn) */
/* Functions:  17 total organized into 6 categories */
/* Externs:    0 */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Implementation Location [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_json_flow B.1b FLOW — Implementation Location
 * @ingroup cpisi_io_json_orgchart
 * @brief    Where implementations live.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Implementation: cpisi/io/json.c */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — Summary Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_json_counts B.1c COUNTS — Summary Statistics
 * @ingroup cpisi_io_json_orgchart
 * @brief    Interface element counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      1 total (CpisiJsonOutputFn) */
/* Functions:  17 total */
/*   - Config:     1 (set_output) */
/*   - Utilities:  1 (escape) */
/*   - Game:       3 (state, state_pty, choices) */
/*   - Cube:       3 (cube_state, cube_keys, cube_action) */
/*   - Cognitive:  2 (thought, response) */
/*   - Status:     5 (ok, error, ready, complete, goodbye) */
/*   - Help:       1 (help) */
/*   - State:      2 (hebrew_state, health) */
/* Externs:    0 total */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — String Utilities [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_helpers B.2 HELPERS — String Utilities
 * @brief    JSON string escaping and utility functions.
 *
 * WHY: JSON requires proper escaping of special characters (quotes, backslash,
 *      newlines, control characters). These utilities ensure valid JSON output.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Escape a string for JSON output.
 *
 * Escapes special characters: \", \\, \n, \r, \t, and control characters.
 * Ensures output is valid JSON string content.
 *
 * @param[out] buf      Output buffer for escaped string.
 * @param[in]  buf_size Size of output buffer.
 * @param[in]  src      Source string to escape.
 *
 * @return Number of characters written (excluding null terminator).
 */
int cpisi_json_escape(char* buf, size_t buf_size, const char* src);

/** @} */ /* end B.2 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Output Configuration [CORE_OPS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_core_ops B.3 CORE OPS — Output Configuration
 * @brief    Configure where JSON output goes.
 *
 * WHY: Different contexts need different output targets — stdout for REPL,
 *      sockets for clients, buffers for testing. Configurable output enables all.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Set the output callback for JSON responses.
 *
 * Allows redirection of JSON output to different targets:
 *   - NULL: Restores default stdout output
 *   - Custom function: Socket, buffer, or other target
 *
 * @param[in] fn Output function to use (NULL restores default stdout).
 *
 * @warning Not thread-safe. Set once at startup before any JSON output.
 */
void cpisi_json_set_output(CpisiJsonOutputFn fn);

/** @} */ /* end B.3 CORE OPS */

/** B.4 ERRORS — [Reserved: uses cpisi_json_error function] @defgroup cpisi_io_json_b4_errors @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — JSON Output Functions [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_api B.5 PUBLIC API — JSON Output Functions
 * @brief    JSON serialization functions for all CPI-SI state types.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5a GAME — Game State Output [GAME]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_json_game B.5a GAME — Game State Output
 * @ingroup cpisi_io_json_api
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
 * Format: {"status":"ok","scene":"...","act":N,"line":N,"text":"...","choices":[]}
 */
void cpisi_json_state(void);

/**
 * @brief Output PTY subprocess screen as JSON.
 *
 * Used when CPI-SI is wrapping a terminal application.
 *
 * @param[in] lines      Array of screen line strings.
 * @param[in] line_count Number of lines.
 */
void cpisi_json_state_pty(const char** lines, int line_count);

/**
 * @brief Output available choices as JSON.
 *
 * Format: {"status":"ok","count":N,"choices":[{"index":1,"text":"..."},...]}
 */
void cpisi_json_choices(void);

/** @} */ /* end B.5a GAME */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5b CUBE — Cube State Output [CUBE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_json_cube B.5b CUBE — Cube State Output
 * @ingroup cpisi_io_json_api
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
 * Includes: position (x,y,z), Hebrew state, k-factor, health score.
 *
 * @param[in] cube The CPI-SI cube (NULL returns error JSON).
 */
void cpisi_json_cube_state(CPISICube* cube);

/**
 * @brief Output available keys as JSON.
 *
 * Format: {"status":"ok","keys":[{"key":"+","weight":"light"},...]
 *
 * @param[in] cube The CPI-SI cube (NULL returns error JSON).
 */
void cpisi_json_cube_keys(CPISICube* cube);

/**
 * @brief Output cube action result as JSON.
 *
 * @param[in] action Action name ("expand", "retreat", "center").
 * @param[in] x      New X position after action.
 * @param[in] y      New Y position after action.
 * @param[in] z      New Z position after action.
 */
void cpisi_json_cube_action(const char* action, int x, int y, int z);

/** @} */ /* end B.5b CUBE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5c COGNITIVE — Thought Processing Output [COGNITIVE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_json_cognitive B.5c COGNITIVE — Thought Processing
 * @ingroup cpisi_io_json_api
 * @brief    JSON responses for thought processing and pattern matching.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Output thinking result as JSON.
 *
 * @param[in] input      The input text.
 * @param[in] state_name Hebrew state name.
 * @param[in] tone       Tone value (-1 to +1).
 * @param[in] depth      Depth value (-1 to +1).
 * @param[in] directness Directness value (-1 to +1).
 * @param[in] confidence Confidence value (-1 to +1).
 * @param[in] verbosity  Verbosity value (-1 to +1).
 */
void cpisi_json_thought(const char* input, const char* state_name,
                        int tone, int depth, int directness,
                        int confidence, int verbosity);

/**
 * @brief Output pattern response as JSON.
 *
 * @param[in] pattern_name Name of pattern used.
 * @param[in] output       Generated output text.
 */
void cpisi_json_response(const char* pattern_name, const char* output);

/** @} */ /* end B.5c COGNITIVE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5d STATUS — Protocol Status Responses [STATUS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_json_status B.5d STATUS — Protocol Status Responses
 * @ingroup cpisi_io_json_api
 * @brief    Simple status messages for protocol communication.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Output a simple success response.
 *
 * @param[in] extra Optional extra JSON fields (without braces), or NULL.
 */
void cpisi_json_ok(const char* extra);

/**
 * @brief Output an error message as JSON.
 *
 * @param[in] code    Error code string (e.g., "NOT_FOUND", "INVALID_INPUT").
 * @param[in] message Human-readable error message.
 */
void cpisi_json_error(const char* code, const char* message);

/**
 * @brief Output ready status as JSON.
 *
 * @param[in] mode_name Name of the mode (REPL, Socket, etc.).
 */
void cpisi_json_ready(const char* mode_name);

/**
 * @brief Output completion status as JSON.
 *
 * @param[in] message Completion message.
 */
void cpisi_json_complete(const char* message);

/**
 * @brief Output goodbye status as JSON.
 */
void cpisi_json_goodbye(void);

/**
 * @brief Output command help as JSON.
 *
 * @param[in] command   Command name.
 * @param[in] help_text Help text for the command.
 */
void cpisi_json_help(const char* command, const char* help_text);

/** @} */ /* end B.5d STATUS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5e STATE — Cognitive State Serialization [STATE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_json_state_serial B.5e STATE — Cognitive State Serialization
 * @ingroup cpisi_io_json_api
 * @brief    Serialize full cognitive state from types.h structures.
 *
 * WHY: Complements cube output with detailed state info. External systems
 *      may need Hebrew state semantics or multi-scale health representation.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Output Hebrew state details as JSON.
 *
 * @param[in] state DARHebrewState value from omni_hebrew.h (-3 to +3).
 */
void cpisi_json_hebrew_state(DARHebrewState state);

/**
 * @brief Output health information as JSON.
 *
 * Includes: score (0-100), level, multi-scale, ternary representations.
 *
 * @param[in] health HealthScore value (0-100).
 * @param[in] level  HealthLevel value (7 levels).
 * @param[in] score  MultiScore pointer (optional, can be NULL).
 */
void cpisi_json_health(HealthScore health, HealthLevel level, const MultiScore* score);

/** @} */ /* end B.5e STATE */

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_b6_omission B.6 OMISSION — Reserved BODY Sections
 * @brief    Documents what BODY sections are intentionally reserved.
 *
 * | Section | Status | Reason |
 * |---------|--------|--------|
 * | B.4 ERRORS | Reserved | Uses cpisi_json_error function |
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: B.6 is documentation-only] */

/** @} */ /* end B.6 OMISSION */

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
 * @defgroup cpisi_io_json_x1_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build and syntax verification.
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * gcc -fsyntax-only -Wall -Wextra cpisi/io/json.h    # syntax check
 * cppcheck --enable=all cpisi/io/json.h              # static analysis
 * @endcode
 *
 * @par X.1b SELFTEST — Minimal Isolation Test
 * @code{.c}
 * #include "kernel/cpisi/io/json.h"
 * int main(void) { cpisi_json_ok(NULL); return 0; }
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Validation documented in defgroup above] */

/** @} */ /* end X.1 VALIDATION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_x2_execution X.2 EXECUTION — Usage Patterns
 * @brief    Include pattern and usage examples.
 *
 * @par X.2a INCLUDE — Include Pattern
 * @code{.c}
 * #include "kernel/cpisi/io/json.h"
 * @endcode
 *
 * @par X.2b USAGE — JSON Output Examples
 * @code{.c}
 * // Simple status
 * cpisi_json_ok(NULL);
 *
 * // Error response
 * cpisi_json_error("NOT_FOUND", "Scene not found");
 *
 * // Cube state
 * cpisi_json_cube_state(&cube);
 *
 * // Custom output target
 * void socket_write(const char* msg) { send(sock, msg, strlen(msg), 0); }
 * cpisi_json_set_output(socket_write);
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Execution documented in defgroup above] */

/** @} */ /* end X.2 EXECUTION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership Patterns [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_x3_cleanup X.3 CLEANUP — Ownership Patterns
 * @brief    Ownership notes — all JSON output is ephemeral.
 *
 * OWNERSHIP:
 *   - JSON strings: Internally managed, output immediately
 *   - Input pointers: Caller retains ownership
 *   - Output callback: Caller responsibility to set/restore
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Cleanup documented in defgroup above] */

/** @} */ /* end X.3 CLEANUP */

/* ─────────────────────────────────────────────────────────────────────────────
 * END CODE SECTIONS
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidance [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_x4_policy X.4 POLICY — Modification Guidance
 * @brief    Guide maintainers — modification safety levels.
 *
 * SAFE TO MODIFY:
 *   - Add new JSON output functions
 *   - Add new fields to existing JSON outputs
 *
 * MODIFY WITH CARE:
 *   - Change JSON field names (breaks clients)
 *   - Change output format structure
 *
 * NEVER MODIFY:
 *   - Error response format (clients depend on it)
 *   - 4-block structure
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Policy documented in defgroup above] */

/** @} */ /* end X.4 POLICY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Add JSON Functions [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_x5_extension X.5 EXTENSION — How to Add JSON Functions
 * @brief    How to add new JSON output functions.
 *
 * ADDING NEW JSON OUTPUT:
 *   1. Add function declaration to appropriate B.5 subsection
 *   2. Use cpisi_json_escape() for all string values
 *   3. Follow {"status":"ok",...} or {"status":"error",...} pattern
 *   4. Update B.1c COUNTS
 *   5. Add implementation to json.c
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Extension documented in defgroup above] */

/** @} */ /* end X.5 EXTENSION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOT — Common Problems [TROUBLESHOOT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_x6_troubleshoot X.6 TROUBLESHOOT — Common Problems
 * @brief    Common problems and solutions.
 *
 * "Undefined CPISICube":
 *   → Ensure omni_cpisi.h path is correct for your build system
 *
 * "Invalid JSON output":
 *   → Use cpisi_json_escape() for all string values
 *
 * "No output appearing":
 *   → Check if custom output callback was set correctly
 *   → Call cpisi_json_set_output(NULL) to restore stdout
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Troubleshoot documented in defgroup above] */

/** @} */ /* end X.6 TROUBLESHOOT */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_x7_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick lookup — common JSON response patterns.
 *
 * SUCCESS PATTERN:
 *   {"status":"ok", ...data...}
 *
 * ERROR PATTERN:
 *   {"status":"error","code":"CODE","message":"..."}
 *
 * CUBE STATE PATTERN:
 *   {"status":"ok","x":N,"y":N,"z":N,"state":"yashar","k":0,"health":50}
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reference documented in defgroup above] */

/** @} */ /* end X.7 REFERENCE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_x8_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * Scripture: Proverbs 25:11
 *
 * This header provides structured JSON output for CPI-SI.
 * "A word fitly spoken" — the structure (JSON) presents content
 * in a form that can be properly received and parsed.
 *
 * Architecture: types/ → io/json.h → external clients
 * This file is the SERIALIZATION layer — reads state, outputs JSON.
 *
 * "A word fitly spoken is like apples of gold in pictures of silver."
 * — Proverbs 25:11
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Note documented in defgroup above] */

/** @} */ /* end X.8 NOTE */

/** X.9 TEMPLATE — [Reserved: instance file] @defgroup cpisi_io_json_x9_template @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.10 OMISSION — Reserved Sections [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_json_x10_omission X.10 OMISSION — Reserved Sections
 * @brief    CLOSING sections guidance.
 *
 * All CLOSING sections present except X.9 (template guidance not needed
 * for instance files).
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Omission documented in defgroup above] */

/** @} */ /* end X.10 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * C++ Linkage Closure
 * ───────────────────────────────────────────────────────────────────────────── */
#ifdef __cplusplus
}
#endif

#endif /* CPISI_IO_JSON_H */
