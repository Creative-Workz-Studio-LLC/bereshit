/// @file layer.h
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
/// @omni:req  key        = CPISI-TYPES-LAYER
/// @omni:req  from       = templates/code/c/header.h
/// @omni:req  at         = a-01.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = domain
/// @omni:inh  role       = types
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = types/layer
/// @omni:ins  includes   = types.h
/// @omni:ins  provides   = [S.2_LAYER]
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
/// @omni:req  title      = CPI-SI Layer Domain
/// @omni:req  brief      = Layer navigation and classification macros
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
///   Feature macros and compile-time settings must be defined BEFORE \#include.
///   This is not PRAGMA (metadata), not METADATA (docs), not SETUP (includes).
///
/// CONTAINS:
///   - SP.1 CONFIG  — Compile-time settings (POSIX)
///
/// INHERITS (from types.h):
///   - SP.1b MODE   — Layer selection (defined in foundation)
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * SP.1 CONFIG — Compile-time Settings [CONFIG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_special_config SP.1 CONFIG — Compile-time Settings
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
 * @defgroup cpisi_layer_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_layer_special_config
 * @brief    Enables POSIX.1-2008 functions in standard headers.
 *
 * WHY: Time functions, file operations, and POSIX APIs require this macro.
 *      Must be defined BEFORE any system includes or they won't see it.
 *
 * SCOPE: Universal — most C files need POSIX features.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L  /**< POSIX.1-2008 — enables strdup, nanosleep, etc. */
#endif

/** @} */ /* end SP.1a POSIX */

/** @} */ /* end SP.1 CONFIG */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * INCLUDE GUARD — Identity protection
 * ───────────────────────────────────────────────────────────────────────────── */

#ifndef CPISI_TYPES_LAYER_H
#define CPISI_TYPES_LAYER_H

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
 * @file      layer.h
 * @brief     CPI-SI Layer Domain — 9-layer architecture navigation macros.
 *
 * @defgroup cpisi_layer_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-TYPES-LAYER
 * Title:     CPI-SI Layer Domain Types
 * Type:      Header (Domain)
 * Component: Rails — provides layer navigation for 9-layer architecture
 * Role:      Layer constants, navigation macros, tier selection.
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_state M.2 STATE — Lifecycle State
 * @{
 *
 * @version   b-03.00
 * @date      2026-01-25
 *
 * Status:    Active
 * Created:   2026-01-21
 * Updated:   2026-01-25
 *
 * @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authors and Copyright [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_layer_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/types/layer/layer.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: types/types.h (foundation)
 * Template:     bereshit/word/seed/code/c/type-header.h
 * Extracted:    2026-01-21 from types.h layer sections
 *
 * Builds FROM:  types.h (foundation types)
 * Builds TO:    All layer-aware components, MODE selection
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: types, layer, domain, 9-layer, navigation, 4+1+4
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide layer navigation macros for 9-layer architecture.
 * Core Design: 4+1+4 pattern with CENTER at layer 4 (HEALTH).
 * Philosophy:  Clear boundaries between layers enable proper navigation.
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Genesis 1:6-8 — God separated the waters (layers of creation)
 * Principle:  Clear boundaries enable proper navigation.
 * Anchor:     Genesis 1:4 — "God divided the light from the darkness"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types.h (foundation types)
 *
 * What Uses This:
 *   - All layer-aware components
 *   - MODE selection system
 *
 * @see types.h for foundation types
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-21) — Extracted from types.h layer sections
 *   a-01.25 (2026-01-23) — Expanded METADATA to full M.1-M.10 structure
 *   b-03.00 (2026-01-25) — Template alignment: type-header.h, CLOSING X.1-X.9
 *
 * Current:
 *   ✓ M.1-M.10 METADATA structure
 *   ✓ Layer navigation macros defined
 *   ✓ X.1-X.9 CLOSING guidance
 *
 * Planned:
 *   ⏳ Layer transition validation
 *   ⏳ Cross-layer communication patterns
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
/// @brief Configuration and dependencies before body.
///
/// CONTAINS:
///   - S.1 INCLUDES  — Foundation headers (types.h)
///   - S.2 DEFINES   — Layer domain (Bounds*, Macros, Verify*, Lookup*, Types*)
///
/// Note: (*) = reserved (inherited from types.h or not applicable)
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
 * @defgroup cpisi_layer_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * PURPOSE: Establish all header dependencies in consistent order.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — System headers (inherited from types.h)
 *   - S.1b INTERNAL  — Project headers (types.h foundation)
 *   - S.1c EXTERNAL  — Third-party headers (none)
 *   - S.1d CPP       — C++ linkage (inherited from types.h)
 *
 * Section order: Standard → Internal → External → CPP linkage
 * Flow: system foundation → our code → third-party → C++ compatibility
 *
 * Builds FROM: SETUP block (configuration context)
 * Builds TO:   S.2 DEFINES (domain-specific constants)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/** S.1a STANDARD — [Inherited from types.h S.1a] @defgroup cpisi_layer_includes_standard @ingroup cpisi_layer_includes @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_layer_includes_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_layer_includes
 * @brief    Internal CPI-SI headers this file depends on.
 *
 * PURPOSE: Include foundation header providing S.1 system headers and S.2 defines.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel/types/config/cfg/domain.cfg.h"  /**< TIER 1: config anchors (brings types.h transitively) */

/** @} */ /* end S.1b INTERNAL */

/** S.1c EXTERNAL — [None required] @defgroup cpisi_layer_includes_external @ingroup cpisi_layer_includes @{ @} */

/** S.1d CPP — [Inherited from types.h S.1d] @defgroup cpisi_layer_includes_cpp @ingroup cpisi_layer_includes @{ @} */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Layer Domain Definitions [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_domain S.2 DEFINES — Layer Domain
 * @brief    Layer navigation macros for 9-layer Genesis pyramid.
 *
 * PURPOSE: Define layer-specific constants, operations, verification, and types.
 *
 * Builds FROM: S.1 INCLUDES (types.h foundation via config.h)
 * Builds TO:   All layer-aware components, MODE selection
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — [Reserved: no domain-specific preludes]
 *   - S.2a  BOUNDS  — [Reserved: inherited from types.h]
 *   - S.2b  MACROS  — Layer navigation operations
 *   - S.2c VERIFY  — [Reserved: inherited from types.h]
 *   - S.2d LOOKUP  — [Reserved: inherited from types.h]
 *   - S.2e TYPES   — [Reserved: inherited from types.h]
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  LAYER DOMAIN — 9-Layer Genesis Pyramid Navigation                         │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │                                                                             │
 * │  UNSIGNED INDEX (0-8):    BALANCED (-4 to +4):                              │
 * │    0 = KERNEL              -4 = bottom                                      │
 * │    4 = HEALTH (center)      0 = center                                      │
 * │    8 = APP                 +4 = top                                         │
 * │                                                                             │
 * │  CONVERSION:                                                                │
 * │    balanced = index - 4                                                     │
 * │    index = balanced + 4                                                     │
 * │                                                                             │
 * └─────────────────────────────────────────────────────────────────────────────┘
 * @{
 */

/** S.2.0 PRELUDE — [Reserved: inherits types.h] @defgroup cpisi_layer_prelude @ingroup cpisi_layer_domain @{ @} */

/** S.2a BOUNDS — [Reserved: inherits types.h] @defgroup cpisi_layer_bounds @ingroup cpisi_layer_domain @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b MACROS — Layer Operations [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2b_LAYER S.2b LAYER MACROS — Operations
 * @ingroup cpisi_layer_domain
 * @brief    Layer-to-balanced and balanced-to-layer conversions, validation.
 *
 * PURPOSE: Provide layer navigation operations for the 9-layer pyramid
 *          (KERNEL through APP) using BOUNDS from types.h.
 *
 * Builds FROM: types.h (S.2a.2 DIMENSIONS, S.2a.3 LAYERS, S.2b.2 PRIMITIVES)
 * Builds TO:   Memory allocation (layer-scaled sizing), error handling (layer context)
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC extensions to S.2b MACROS defined in
 *   types.h. The foundation macros (TO_BALANCED, TO_INDEX, IN_RANGE) come from
 *   types.h. This file adds layer navigation convenience macros.
 *
 * STRUCTURE:
 *   S.2b.7a LAYER DOMAIN — Layer navigation macros
 *     - LAYER_TO_BAL     — Index (0-8) → balanced (-4 to +4)
 *     - BAL_TO_LAYER     — Balanced → index
 *     - IS_BELOW_CENTER  — Check if layer < 4
 *     - IS_ABOVE_CENTER  — Check if layer > 4
 *     - IS_CENTER        — Check if layer == 4 (HEALTH)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2b_7a_LAYER_DOMAIN S.2b.7a LAYER DOMAIN — Will split to types-layer.h
 * @ingroup S2b_7_DOMAIN
 * @brief Layer navigation convenience macros for 9-layer pyramid.
 *
 * **Target: types-layer.h**
 *
 * BUILDS FROM: S.2a.2 DIMENSIONS (FACE=9), S.2a.3 LAYERS (MIN/MAX/CENTER)
 * DEPENDS ON:  S.2b.2 PRIMITIVES (TO_BALANCED, TO_INDEX, IN_RANGE)
 * @{ */

/**
 * @brief Center power alias — 81 (TESSERACT = 3⁴).
 *
 * BOUNDS USED:
 *   - CPISI_POW_CENTER = 81 (TESSERACT, center of power sequence)
 *
 * USE CASE: Provides semantic name for 81 in layer-focused code.
 */
#define CPISI_CENTER              CPISI_POW_CENTER

/**
 * @brief Convert unsigned layer index to balanced layer.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (total layers)
 *   - CPISI_LAYER_HALF = (FACE-1)/2 = 4 (center offset)
 *
 * FORMULA: bal = layer - LAYER_HALF = layer - 4
 *
 * DERIVATION:
 *   - 9 layers span [0, 8] in index form
 *   - Center at position 4 should map to 0 in balanced form
 *   - Subtracting 4 shifts: 0→-4, 4→0, 8→+4
 *
 * EXAMPLES:
 *   LAYER_TO_BAL(0) = 0 - 4 = -4  (KERNEL, bottom)
 *   LAYER_TO_BAL(4) = 4 - 4 =  0  (HEALTH, center)
 *   LAYER_TO_BAL(8) = 8 - 4 = +4  (APP, top)
 *
 * @param layer Unsigned layer index (0-8)
 * @return Balanced layer value (-4 to +4)
 */
#define CPISI_LAYER_TO_BAL(layer)   CPISI_TO_BALANCED((layer), CPISI_FACE)

/**
 * @brief Convert balanced layer to unsigned layer index.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (total layers)
 *   - CPISI_LAYER_HALF = 4 (center offset)
 *
 * FORMULA: layer = bal + LAYER_HALF = bal + 4
 *
 * DERIVATION:
 *   - Balanced form spans [-4, +4] centered on 0
 *   - Index form must span [0, 8] for array access
 *   - Adding 4 shifts: -4→0, 0→4, +4→8
 *
 * EXAMPLES:
 *   BAL_TO_LAYER(-4) = -4 + 4 = 0  (KERNEL)
 *   BAL_TO_LAYER( 0) =  0 + 4 = 4  (HEALTH)
 *   BAL_TO_LAYER(+4) = +4 + 4 = 8  (APP)
 *
 * @param bal Balanced layer value (-4 to +4)
 * @return Unsigned layer index (0-8)
 */
#define CPISI_BAL_TO_LAYER(bal)     CPISI_TO_INDEX((bal), CPISI_FACE)

/**
 * @brief Check if unsigned layer index is valid (0-8).
 *
 * BOUNDS USED:
 *   - CPISI_LAYER_MIN = 0 (KERNEL)
 *   - CPISI_LAYER_MAX = 8 (APP)
 *
 * FORMULA: valid = (LAYER_MIN ≤ n ≤ LAYER_MAX)
 *
 * USE CASE: Guard before array access using layer as index.
 *
 * @param n Value to check
 * @return 1 if valid layer index, 0 otherwise
 */
#define CPISI_IS_LAYER(n)           CPISI_IN_RANGE((n), CPISI_LAYER_MIN, CPISI_LAYER_MAX)

/**
 * @brief Check if balanced layer value is valid (-4 to +4).
 *
 * BOUNDS USED:
 *   - CPISI_BALANCED_MIN = -4 (bottom of balanced range)
 *   - CPISI_BALANCED_MAX = +4 (top of balanced range)
 *
 * FORMULA: valid = (BALANCED_MIN ≤ bal ≤ BALANCED_MAX)
 *
 * USE CASE: Guard before layer arithmetic that expects balanced form.
 *
 * @param bal Value to check
 * @return 1 if valid balanced layer, 0 otherwise
 */
#define CPISI_IS_BAL_LAYER(bal)     CPISI_IN_RANGE((bal), CPISI_BALANCED_MIN, CPISI_BALANCED_MAX)


/** @} */ /* end S.2b.7a LAYER DOMAIN */

/** @} */ /* end S.2b LAYER MACROS */

/** S.2c VERIFY — [Reserved: inherits types.h] @defgroup cpisi_layer_verify @ingroup cpisi_layer_domain @{ @} */

/** S.2d LOOKUP — [Reserved: inherits types.h] @defgroup cpisi_layer_lookup @ingroup cpisi_layer_domain @{ @} */

/** S.2e TYPES — [Reserved: inherits types.h] @defgroup cpisi_layer_types_s2e @ingroup cpisi_layer_domain @{ @} */

/** @} */ /* end cpisi_layer_domain (S.2 DEFINES) */

/** S.3 TYPES — [Reserved: inherits types.h] @defgroup cpisi_layer_s3_types @{ @} */

/** S.4 PROTOTYPES — [Reserved: macros in S.2b] @defgroup cpisi_layer_s4_prototypes @{ @} */

/** S.5 FILE-LEVEL — [Reserved: no mutable state] @defgroup cpisi_layer_s5_file_level @{ @} */

/** S.6 OMISSION — S.3-S.5 reserved (layer operations in S.2b macros) @defgroup cpisi_layer_s6_omission @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief [Reserved: all layer logic in SETUP S.2b macros]
///
/// ═══════════════════════════════════════════════════════════════════════════════

/** B.1 ORG CHART — [Reserved: types/macros in SETUP] @defgroup cpisi_layer_orgchart @{ @} */
/** B.2 HELPERS — [Reserved: macros in S.2b] @defgroup cpisi_layer_helpers @{ @} */
/** B.3 RESERVED — [Reserved: inherits types.h B.3] @defgroup cpisi_layer_b3_reserved @{ @} */
/** B.4 RESERVED — [Reserved: inherits types.h B.4] @defgroup cpisi_layer_b4_reserved @{ @} */
/** B.5 RESERVED — [Reserved: inherits types.h B.5] @defgroup cpisi_layer_b5_reserved @{ @} */
/** B.6 RESERVED — [Reserved: inherits types.h B.6] @defgroup cpisi_layer_b6_reserved @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Final assembly — X.1-X.9 sections (template-aligned).
///
/// CONTAINS:
///   - X.1 VALIDATION    — Build verification
///   - X.2 EXECUTION     — Usage patterns
///   - X.3 CLEANUP       — Ownership notes
///   - X.4 POLICY        — Modification guidance
///   - X.5 EXTENSION     — How to extend layer domain
///   - X.6 TROUBLESHOOT  — Common problems
///   - X.7 REFERENCE     — Quick lookup
///   - X.8 NOTE          — Closing guidance
///   - X.9 OMISSION      — Reserved sections
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build and self-containment verification.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.1a BUILD — Compilation Commands
 *   @code
 *   # Syntax check (from include/ directory):
 *   gcc -fsyntax-only -Wall -Wextra cpisi/types/domain/core/layer.h
 *   @endcode
 *
 * @par X.1b CHECKS — Verification Checklist
 *   - ✓ Compiles without warnings (-Wall -Wextra)
 *   - ✓ Self-contained (includes config.h → types.h)
 *   - ✓ Layer macros use types.h primitives
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_execution X.2 EXECUTION — Usage Patterns
 * @brief    How to use layer macros in code.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.2a INCLUDE — Include Directive
 *   @code
 *   #include "kernel/cpisi/types/domain/core/layer.h"
 *   @endcode
 *
 * @par X.2b USAGE — Layer Navigation
 *   @code
 *   int bal = CPISI_LAYER_TO_BAL(4);  // 4 → 0 (center)
 *   if (CPISI_IS_BELOW_CENTER(2)) { ... }
 *   @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership Patterns [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_cleanup X.3 CLEANUP — Ownership Patterns
 * @brief    Memory and resource ownership.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.3a OWNERSHIP — No Dynamic Allocation
 *   Layer macros are compile-time operations.
 *   No dynamic memory allocation. No cleanup required.
 *
 * @} */

/* ─────────────────────────────────────────────────────────────────────────────
 * END CODE SECTIONS
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidance [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_policy X.4 POLICY — Modification Guidance
 * @brief    Rules for modifying this file.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par DO NOT CHANGE
 *   - Layer count (9 layers, architectural constant)
 *   - Balanced range (-4 to +4)
 *
 * @par MAY ADD
 *   - New navigation convenience macros
 *   - Layer relationship queries
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Extend [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_extension X.5 EXTENSION — How to Extend
 * @brief    How to add new layer operations.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Adding Navigation Macros
 *   - Build on types.h primitives (TO_BALANCED, IN_RANGE)
 *   - Document bounds used
 *   - Add to S.2b.7a LAYER DOMAIN section
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOT — Common Problems [TROUBLESHOOT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_troubleshoot X.6 TROUBLESHOOT — Common Problems
 * @brief    Common problems and solutions.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par "Layer index out of range"
 *   Use CPISI_IS_LAYER() to validate before conversion.
 *   Layer indices are 0-8, balanced are -4 to +4.
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick lookup — layer values and conversions.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Layer Indices (0-8)
 *   0=KERNEL, 1=SYSTEM, 2=RUNTIME, 3=FRAMEWORK,
 *   4=HEALTH(center), 5=NETWORK, 6=STORAGE, 7=ENGINE, 8=APP
 *
 * @par Balanced Values (-4 to +4)
 *   LAYER_TO_BAL(idx) = idx - 4
 *   BAL_TO_LAYER(bal) = bal + 4
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * Scripture: Genesis 1:1
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * Layer navigation provides the 9-layer pyramid architecture:
 * 4 below center, 1 center (HEALTH), 4 above center.
 * This 4+1+4 pattern reflects Genesis creation structure.
 *
 * "In the beginning God created the heaven and the earth."
 *   — Genesis 1:1
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 OMISSION — Reserved Sections [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_layer_omission X.9 OMISSION — Reserved Sections
 * @brief    Reserved section documentation.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par SETUP Reserved Sections
 *   - S.2a, S.2c-S.2e — Inherited from types.h
 *   - S.3-S.6 — Layer operations are macros in S.2b
 *
 * @par BODY Reserved Sections
 *   - B.1-B.6 — All logic in SETUP S.2b macros
 *
 * @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_TYPES_LAYER_H */
