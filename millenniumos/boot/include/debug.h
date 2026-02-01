/// @file debug.h
/// @omni code --header
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
/// @omni:req  key        = MOS-boot-debug
/// @omni:req  from       = bereshit/word/seed/code/c/header.h
/// @omni:req  at         = code
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = interface
/// @omni:inh  role       = debug
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = boot
/// @omni:ins  layer      = stage0
/// @omni:ins  includes   = [stdint.h, serial.h, bootinfo.h]
/// @omni:ins  provides   = [MOS_WITNESS, MOS_DETECT, MOS_ASSESS, MOS_RESTORE, LOG_*]
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
/// @omni:req  title      = MillenniumOS Boot Debug System (Witness-Compatible)
/// @omni:req  brief      = Early boot logging compatible with DAR Witness System
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// END PRAGMA [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// SPECIAL BLOCK [SPECIAL]
/// ═══════════════════════════════════════════════════════════════════════════════

/** [Reserved: Debug header — no pre-include configuration required] */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

// ─────────────────────────────────────────────────────────────────────────────────
// INCLUDE GUARD — Identity protection
// ─────────────────────────────────────────────────────────────────────────────────

#ifndef MOS_DEBUG_H
#define MOS_DEBUG_H

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @file      debug.h
 * @brief     MillenniumOS Boot Debug System — Witness-Compatible Early Logging
 *
 * @defgroup  mos_debug M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       MOS-boot-debug
 * Title:     MillenniumOS Boot Debug System (Witness-Compatible)
 * Type:      Header (Interface)
 * Component: Boot — shared debug logging
 * Role:      Early boot logging compatible with DAR Witness System
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2-M.6 — State, Attribution, Location, Derivation, Classification [CONDENSED]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_debug_state M.2-M.6 CONDENSED — State and Attribution
 * @{
 *
 * @version   a-03.00
 * @date      2026-01-31
 *
 * Status:       Active
 * Created:      2026-01-31
 * Updated:      2026-01-31
 *
 * @author    Seanje Lenox-Wise (Architect)
 * @author    Nova Dawn (Author)
 * @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * Path:         millenniumos/boot/include/debug.h
 * Derives_from: bereshit/word/seed/code/c/header.h
 *
 * Tags:         boot, debug, witness, DAR, ternary, health, logging
 *
 * @} end M.2-M.6 CONDENSED
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_debug_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Early boot logging FORMAT-COMPATIBLE with kernel DAR Witness System
 * Core Design: DAR pattern (DETECT → ASSESS → RESTORE) instead of log levels
 * Philosophy:  The system learns through serving, not surveillance
 *
 * Key Features:
 *   - DAR phases (DETECT/ASSESS/RESTORE) follow ternary semantics
 *   - Health impact tracking (+/-) on every event
 *   - Parseable serial output format for kernel ingestion
 *   - Boot event buffer for kernel Witness system integration
 *   - Severity filtering at compile time
 *
 * DAR PATTERN: DETECT (witness) → ASSESS (evaluate) → RESTORE (recover)
 *   - DETECT (-1): Witness the event, observe what happened
 *   - ASSESS (0):  Evaluate significance, understand impact
 *   - RESTORE (+1): Apply learning, recover/correct
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_debug_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Primary Scripture:
 *   Lamentations 3:40 — "Let us search and try our ways, and turn again
 *                        to the LORD."
 *
 * WHY DAR FOR DEBUGGING:
 *   The DAR pattern mirrors the biblical call to self-examination and return.
 *   DETECT = "search our ways" — honestly witness what happened
 *   ASSESS = "try our ways" — evaluate significance and impact
 *   RESTORE = "turn again" — apply learning, course correct
 *
 *   This is not surveillance but discipleship — the system learns through
 *   serving, examining its own state to serve better, not to control.
 *
 * Structural Parallel:
 *   Scripture                 │ DAR Witness System
 *   ──────────────────────────┼──────────────────────────────────
 *   "search our ways"         │ DETECT — witness events
 *   "try our ways"            │ ASSESS — evaluate significance
 *   "turn again to the LORD"  │ RESTORE — apply learning
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_debug_deps M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Standard: stdint.h (uint8_t, uint16_t, uint32_t, uint64_t, int8_t)
 *   Internal: serial.h (mos_inb, mos_outb, MOS_COM1_*, MOS_LSR_*)
 *             bootinfo.h (boot info reserved area for event buffer)
 *
 * What Uses This:
 *   - boot/uefi/uefi_boot.c   — UEFI bootloader logging
 *   - boot/stage0/[*].c       — Stage0 boot components
 *   - kernel/kernel.c         — Early kernel logging (before full Witness init)
 *
 * Kernel Integration:
 *   Once kernel initializes, it can:
 *     1. Parse the boot event buffer
 *     2. Ingest events into MosWitnessState
 *     3. Begin pattern recognition on boot sequence
 *
 * @see serial.h for underlying I/O primitives
 * @see bootinfo.h for boot info structure
 * @see kernel/cpisi/cpisi.h for kernel Witness system
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_debug_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-31) — Initial DAR-compatible debug header
 *   a-02.00 (2026-01-31) — Added boot event buffer for kernel ingestion
 *   a-03.00 (2026-01-31) — Full OmniCode 4-block template alignment
 *
 * Current:
 *   ✓ DAR phases with ternary semantics (-1, 0, +1)
 *   ✓ Health delta tracking on all events
 *   ✓ Parseable serial output format
 *   ✓ Boot event buffer (512 bytes, 16 events)
 *   ✓ Compile-time severity filtering
 *   ✓ Full OmniCode template compliance
 *
 * Planned:
 *   ⏳ Ring buffer mode for continuous logging
 *   ⏳ Pattern detection at boot level
 *   ⏳ Structured event metadata
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
///   - S.2 DEFINES   — DAR phases, severity levels, health deltas, config
///   - S.3 TYPES     — Boot event structures
///   - S.4 PROTOS    — [Reserved: Inline functions in BODY]
///   - S.5 EXTERN    — Boot event buffer pointer
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
 * S.1 INCLUDES — Header Dependencies [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_debug_includes S.1 INCLUDES — Header Dependencies
 * @brief     Required headers for debug system.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include <stdint.h>  /**< Fixed-width integers for event structures */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "serial.h"   /**< Serial I/O primitives (mos_outb, mos_inb) */
#include "bootinfo.h" /**< Boot info structure for event buffer location */

/** @} */ /* end S.1b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Boot debug header — no external dependencies] */

/** @} */ /* end S.1c EXTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Boot header — no C++ linkage required] */

/** @} */ /* end S.1d CPP */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Preprocessor Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_debug_defines S.2 DEFINES — Preprocessor Constants
 * @brief     DAR phases, severity levels, health deltas, configuration.
 *
 * CONTAINS:
 *   - S.2a DAR       — DAR phase constants (DETECT, ASSESS, RESTORE)
 *   - S.2b SEVERITY  — Severity level constants (MILD, MEDIUM, CRITICAL)
 *   - S.2c HEALTH    — Health delta constants
 *   - S.2d CONFIG    — Compile-time configuration
 *   - S.2e BUFFER    — Boot event buffer constants
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a DAR — DAR Phase Constants [DAR]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_debug_dar S.2a DAR — DAR Phase Constants
 * @ingroup   mos_debug_defines
 * @brief     The three DAR phases follow ternary semantics.
 *
 * TERNARY SEMANTICS:
 *   DETECT  (-1): Witness the event, observe what happened
 *   ASSESS  (0):  Evaluate significance, understand impact
 *   RESTORE (+1): Apply learning, recover/correct
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#define MOS_DAR_DETECT      (-1)    /**< Witness/observe */
#define MOS_DAR_ASSESS      (0)     /**< Evaluate/understand */
#define MOS_DAR_RESTORE     (1)     /**< Apply/recover */

/** DAR phase names for serial output */
static const char* const MOS_DAR_NAMES[] = {
    "DETECT",   /* -1 (index 0 after +1 offset) */
    "ASSESS",   /* 0 */
    "RESTORE"   /* +1 */
};

/** Get DAR phase name from value (-1, 0, +1) */
#define MOS_DAR_NAME(phase) MOS_DAR_NAMES[(phase) + 1]

/** @} */ /* end S.2a DAR */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b SEVERITY — Severity Level Constants [SEVERITY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_debug_severity S.2b SEVERITY — Severity Level Constants
 * @ingroup   mos_debug_defines
 * @brief     Event severity follows ternary semantics.
 *
 * TERNARY SEMANTICS:
 *   MILD     (-1): Informational, normal operation
 *   MEDIUM   (0):  Noteworthy, deserves attention
 *   CRITICAL (+1): Significant, requires action
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#define MOS_SEV_MILD        (-1)    /**< Informational */
#define MOS_SEV_MEDIUM      (0)     /**< Noteworthy */
#define MOS_SEV_CRITICAL    (1)     /**< Significant */

/** Severity markers for compact output */
static const char MOS_SEV_CHAR[] = {
    'I',    /* MILD (Info) - index 0 after +1 offset */
    'N',    /* MEDIUM (Note) */
    'C'     /* CRITICAL */
};

/** Get severity character marker from value (-1, 0, +1) */
#define MOS_SEV_MARKER(sev) MOS_SEV_CHAR[(sev) + 1]

/** @} */ /* end S.2b SEVERITY */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c HEALTH — Health Delta Constants [HEALTH]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_debug_health S.2c HEALTH — Health Delta Constants
 * @ingroup   mos_debug_defines
 * @brief     Health impact of events (ternary-inspired scaling).
 *
 * IMPACT SCALE:
 *   Positive = improvement, Negative = degradation, Zero = neutral
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#define MOS_HEALTH_NEUTRAL      0       /**< No health impact */
#define MOS_HEALTH_SLIGHT_UP    5       /**< Slight positive impact */
#define MOS_HEALTH_SLIGHT_DOWN  (-5)    /**< Slight negative impact */
#define MOS_HEALTH_MODERATE_UP  15      /**< Moderate positive impact */
#define MOS_HEALTH_MODERATE_DOWN (-15)  /**< Moderate negative impact */
#define MOS_HEALTH_MAJOR_UP     30      /**< Major positive impact */
#define MOS_HEALTH_MAJOR_DOWN   (-30)   /**< Major negative impact */

/** @} */ /* end S.2c HEALTH */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d CONFIG — Compile-Time Configuration [CONFIG]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_debug_config S.2d CONFIG — Compile-Time Configuration
 * @ingroup   mos_debug_defines
 * @brief     Configuration options set at compile time.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#ifndef MOS_DEBUG_COMPONENT
    #define MOS_DEBUG_COMPONENT "boot"  /**< Default component name */
#endif

/** Minimum severity to output (compile-time filter) */
#ifndef MOS_DEBUG_MIN_SEVERITY
    #ifdef NDEBUG
        #define MOS_DEBUG_MIN_SEVERITY MOS_SEV_MEDIUM  /**< Release: medium+ */
    #else
        #define MOS_DEBUG_MIN_SEVERITY MOS_SEV_MILD    /**< Debug: all levels */
    #endif
#endif

/** @} */ /* end S.2d CONFIG */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e BUFFER — Boot Event Buffer Constants [BUFFER]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_debug_buffer_const S.2e BUFFER — Boot Event Buffer Constants
 * @ingroup   mos_debug_defines
 * @brief     Constants for boot event buffer structure.
 *
 * BUFFER FORMAT:
 *   Early boot events are stored in a buffer within the boot info reserved area.
 *   The kernel can parse this buffer to ingest events into the Witness system.
 *
 *   Per event (32 bytes, fixed size for easy parsing):
 *     Offset 0:  uint32_t sequence    - Event sequence number
 *     Offset 4:  int8_t   dar_phase   - DAR phase (-1, 0, +1)
 *     Offset 5:  int8_t   severity    - Severity (-1, 0, +1)
 *     Offset 6:  int8_t   health_delta- Health impact
 *     Offset 7:  uint8_t  source_len  - Source string length
 *     Offset 8:  char[8]  source      - Source name (truncated to 8 chars)
 *     Offset 16: char[16] event       - Event message (truncated to 16 chars)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#define MOS_BOOT_EVENT_SIZE     32          /**< Bytes per event record */
#define MOS_BOOT_EVENT_MAX      16          /**< Max events (512 bytes total) */
#define MOS_BOOT_EVENT_MAGIC    0x45564E54  /**< "EVNT" in little-endian */

/** @} */ /* end S.2e BUFFER */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_debug_types S.3 TYPES — Type Declarations
 * @brief     Boot event structures for kernel ingestion.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Single boot event record (32 bytes, packed).
 *
 * Fixed-size format for easy kernel parsing.
 */
typedef struct __attribute__((packed)) MosDebugEvent {
    uint32_t    sequence;       /**< 0x00: Event sequence number */
    int8_t      dar_phase;      /**< 0x04: DAR phase (-1, 0, +1) */
    int8_t      severity;       /**< 0x05: Severity level (-1, 0, +1) */
    int8_t      health_delta;   /**< 0x06: Health impact */
    uint8_t     source_len;     /**< 0x07: Source string length */
    char        source[8];      /**< 0x08: Source name (null-padded) */
    char        event[16];      /**< 0x10: Event message (null-padded) */
} MosDebugEvent;

/**
 * @brief Boot event buffer header (stored in boot_info reserved area).
 *
 * Contains header fields followed by array of events.
 */
typedef struct __attribute__((packed)) MosDebugEventBuffer {
    uint32_t        magic;      /**< MOS_BOOT_EVENT_MAGIC for validation */
    uint32_t        count;      /**< Number of events stored */
    uint32_t        sequence;   /**< Next sequence number */
    uint32_t        reserved;   /**< Alignment padding */
    MosDebugEvent    events[MOS_BOOT_EVENT_MAX]; /**< Event array */
} MosDebugEventBuffer;

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOS — Function Prototypes [PROTOS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_debug_protos S.4 PROTOS — Function Prototypes
 * @{
 *
 * [Reserved: Inline functions defined in BODY — no separate prototypes]
 *
 * @} end S.4 PROTOS
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 EXTERN — External Declarations [EXTERN]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_debug_extern S.5 EXTERN — External Declarations
 * @brief     Global state for debug system.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/** Boot event buffer pointer (set by mos_debug_init_buffer) */
static MosDebugEventBuffer* mos_boot_event_buffer = NULL;

/** Event sequence counter (incremented per event) */
static uint32_t mos_debug_seq = 0;

/** @} */ /* end S.5 EXTERN */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief The actual work — declarations (headers), implementations (sources).
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART    — Structure overview, flow, counts
///   - B.2 HELPERS      — Serial output primitives (putc, puts, hex, dec)
///   - B.3 CORE OPS     — Witness output, buffer operations
///   - B.4 ERRORS       — [Reserved: Error handling via DAR pattern]
///   - B.5 PUBLIC API   — MOS_WITNESS macro and convenience macros
///   - B.6 OMISSION     — What is reserved and why
///
/// Section order: Org Chart → Helpers → Core Ops → Errors → Public API → Omission
/// Flow: map structure → foundations → business logic → error handling → exported interface
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @defgroup mos_debug_body BODY — Debug Functions
 * @ingroup mos_debug
 * @{ */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_debug_org_chart B.1 ORG CHART — Structure Overview
 * @ingroup mos_debug_body
 * @brief   Map structure — interface (headers), internals (sources).
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Interface Overview [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      2 structs (MosDebugEvent, MosDebugEventBuffer)
 * Functions:  10 inline (putc, puts, hex_digit, hex8, hex32, hex64, dec, sdec,
 *             witness_serial, buffer_event, init_buffer)
 * Macros:     15+ (MOS_WITNESS, MOS_DETECT, MOS_ASSESS, MOS_RESTORE, LOG_*)
 * Constants:  20+ defines (DAR, severity, health, buffer)
 * Externs:    2 static (mos_boot_event_buffer, mos_debug_seq) */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Implementation Location [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Implementation: All inline in this header
 * Why:            Boot debug must work before any .c files are linked
 * Dependencies:   serial.h for underlying I/O */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — Summary Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      2
 * Functions:  11 inline
 * Macros:     15+
 * Constants:  20+
 * Externs:    2 static */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Foundation Functions [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_debug_helpers B.2 HELPERS — Foundation Functions
 * @ingroup mos_debug_body
 * @brief    Serial output primitives — building blocks for witness output.
 *
 * CONTAINS:
 *   - B.2a PURE       — [Reserved: I/O operations have side effects]
 *   - B.2b UTILITIES  — Serial output utilities (putc, puts, hex, dec)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a PURE — Side-Effect-Free Functions [PURE]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Serial output inherently has side effects] */

/** @} */ /* end B.2a PURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b UTILITIES — Common Operations [UTILITIES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_debug_utilities B.2b UTILITIES — Serial Output Primitives
 * @ingroup mos_debug_helpers
 * @brief    Low-level serial output functions.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Wait for transmit ready and send character.
 * @param c Character to send
 */
static inline void mos_debug_putc(char c) {
    while ((mos_inb(MOS_COM1_STATUS) & MOS_LSR_TX_EMPTY) == 0);
    mos_outb(MOS_COM1_DATA, c);
}

/**
 * @brief Output null-terminated string.
 * @param s String to output (converts \n to \r\n)
 */
static inline void mos_debug_puts(const char* s) {
    while (*s) {
        if (*s == '\n') mos_debug_putc('\r');
        mos_debug_putc(*s++);
    }
}

/**
 * @brief Output single hex digit (0-F).
 * @param val Value (only low 4 bits used)
 */
static inline void mos_debug_hex_digit(uint8_t val) {
    val &= 0x0F;
    mos_debug_putc(val < 10 ? '0' + val : 'A' + val - 10);
}

/**
 * @brief Output 8-bit value as 2 hex digits.
 * @param val Value to output
 */
static inline void mos_debug_hex8(uint8_t val) {
    mos_debug_hex_digit(val >> 4);
    mos_debug_hex_digit(val);
}

/**
 * @brief Output 32-bit value as 8 hex digits.
 * @param val Value to output
 */
static inline void mos_debug_hex32(uint32_t val) {
    mos_debug_hex8(val >> 24);
    mos_debug_hex8(val >> 16);
    mos_debug_hex8(val >> 8);
    mos_debug_hex8(val);
}

/**
 * @brief Output 64-bit value as 16 hex digits.
 * @param val Value to output
 */
static inline void mos_debug_hex64(uint64_t val) {
    mos_debug_hex32(val >> 32);
    mos_debug_hex32(val);
}

/**
 * @brief Output unsigned decimal (simple, no padding).
 * @param val Value to output
 */
static inline void mos_debug_dec(uint32_t val) {
    char buf[12];
    int i = 0;
    if (val == 0) { mos_debug_putc('0'); return; }
    while (val > 0) { buf[i++] = '0' + (val % 10); val /= 10; }
    while (i > 0) mos_debug_putc(buf[--i]);
}

/**
 * @brief Output signed decimal.
 * @param val Value to output
 */
static inline void mos_debug_sdec(int32_t val) {
    if (val < 0) { mos_debug_putc('-'); val = -val; }
    mos_debug_dec((uint32_t)val);
}

/** @} */ /* end B.2b UTILITIES */

/** @} */ /* end B.2 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Component-Specific Functionality [OPERATIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_debug_core_ops B.3 CORE OPS — Witness Operations
 * @ingroup mos_debug_body
 * @brief    Witness output and buffer operations.
 *
 * CONTAINS:
 *   - B.3a SERIAL   — Serial witness output (parseable format)
 *   - B.3b BUFFER   — Boot event buffer operations
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a SERIAL — Witness Serial Output [SERIAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_debug_serial B.3a SERIAL — Witness Serial Output
 * @ingroup mos_debug_core_ops
 * @brief    Output witness events in parseable format.
 *
 * SERIAL FORMAT (parseable by kernel):
 *   [seq] <DAR:phase> <SEV:level> [component] message (health:delta)
 *
 * Example:
 *   [1] <DAR:DETECT> <SEV:I> [uefi] GOP initialized (health:+5)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Output witness event to serial in parseable format.
 *
 * @param dar_phase    DAR phase (-1=DETECT, 0=ASSESS, +1=RESTORE)
 * @param severity     Severity (-1=MILD, 0=MEDIUM, +1=CRITICAL)
 * @param source       Component/source name (e.g., "uefi", "boot")
 * @param event        Event message
 * @param health_delta Health impact (+/- value)
 */
static inline void mos_witness_serial(int8_t dar_phase, int8_t severity,
                                       const char* source, const char* event,
                                       int8_t health_delta) {
    /* Filter by severity */
    if (severity < MOS_DEBUG_MIN_SEVERITY) return;

    /* Increment sequence */
    mos_debug_seq++;

    /* Output: [seq] */
    mos_debug_putc('[');
    mos_debug_dec(mos_debug_seq);
    mos_debug_putc(']');
    mos_debug_putc(' ');

    /* Output: <DAR:phase> */
    mos_debug_putc('<');
    mos_debug_puts("DAR:");
    mos_debug_puts(MOS_DAR_NAME(dar_phase));
    mos_debug_putc('>');
    mos_debug_putc(' ');

    /* Output: <SEV:X> */
    mos_debug_putc('<');
    mos_debug_puts("SEV:");
    mos_debug_putc(MOS_SEV_MARKER(severity));
    mos_debug_putc('>');
    mos_debug_putc(' ');

    /* Output: [source] */
    mos_debug_putc('[');
    mos_debug_puts(source);
    mos_debug_putc(']');
    mos_debug_putc(' ');

    /* Output: message */
    mos_debug_puts(event);

    /* Output: (health:delta) */
    mos_debug_puts(" (health:");
    if (health_delta >= 0) mos_debug_putc('+');
    mos_debug_sdec(health_delta);
    mos_debug_putc(')');

    /* Newline */
    mos_debug_putc('\r');
    mos_debug_putc('\n');
}

/** @} */ /* end B.3a SERIAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3b BUFFER — Boot Event Buffer Operations [BUFFER]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_debug_buffer B.3b BUFFER — Boot Event Buffer Operations
 * @ingroup mos_debug_core_ops
 * @brief    Store events in memory buffer for kernel ingestion.
 *
 * Buffer pointer should be set to boot_info extended area.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize boot event buffer.
 *
 * @param buffer_ptr Pointer to buffer area (in boot_info reserved area)
 */
static inline void mos_debug_init_buffer(void* buffer_ptr) {
    mos_boot_event_buffer = (MosDebugEventBuffer*)buffer_ptr;
    if (mos_boot_event_buffer) {
        mos_boot_event_buffer->magic = MOS_BOOT_EVENT_MAGIC;
        mos_boot_event_buffer->count = 0;
        mos_boot_event_buffer->sequence = 0;
        mos_boot_event_buffer->reserved = 0;
    }
}

/**
 * @brief Store event in buffer (if initialized).
 *
 * @param dar_phase    DAR phase
 * @param severity     Severity level
 * @param source       Component name
 * @param event        Event message
 * @param health_delta Health impact
 */
static inline void mos_debug_buffer_event(int8_t dar_phase, int8_t severity,
                                           const char* source, const char* event,
                                           int8_t health_delta) {
    if (!mos_boot_event_buffer) return;
    if (mos_boot_event_buffer->count >= MOS_BOOT_EVENT_MAX) return;

    MosDebugEvent* evt = &mos_boot_event_buffer->events[mos_boot_event_buffer->count];

    evt->sequence = mos_boot_event_buffer->sequence++;
    evt->dar_phase = dar_phase;
    evt->severity = severity;
    evt->health_delta = health_delta;

    /* Copy source (truncate to 7 chars + null) */
    int i = 0;
    while (source[i] && i < 7) { evt->source[i] = source[i]; i++; }
    evt->source_len = i;
    while (i < 8) evt->source[i++] = '\0';

    /* Copy event (truncate to 15 chars + null) */
    i = 0;
    while (event[i] && i < 15) { evt->event[i] = event[i]; i++; }
    while (i < 16) evt->event[i++] = '\0';

    mos_boot_event_buffer->count++;
}

/** @} */ /* end B.3b BUFFER */

/** @} */ /* end B.3 CORE OPS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 ERRORS — Error Handling [ERRORS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_debug_errors B.4 ERRORS — Error Handling
 * @ingroup mos_debug_body
 * @{
 *
 * [Reserved: Error handling is done via the DAR pattern itself.
 *  DETECT witnesses errors, ASSESS evaluates, RESTORE corrects.
 *  The debug system doesn't need separate error handling.]
 *
 * @} end B.4 ERRORS
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Exported Interface [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_debug_api B.5 PUBLIC API — Exported Interface
 * @ingroup mos_debug_body
 * @brief    Primary logging interface — MOS_WITNESS and convenience macros.
 *
 * CONTAINS:
 *   - B.5a LIFECYCLE   — Buffer initialization
 *   - B.5b OPERATIONS  — MOS_WITNESS unified macro
 *   - B.5c HEALTH      — [Reserved: Health tracked per-event]
 *   - B.5d UTILITIES   — Convenience macros (MOS_DETECT, LOG_*, etc.)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5a LIFECYCLE — Buffer Initialization [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Primary lifecycle: mos_debug_init_buffer() — defined in B.3b BUFFER */

/** @} */ /* end B.5a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5b OPERATIONS — Primary Functionality [OPS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_debug_ops B.5b OPERATIONS — Unified Witness Macro
 * @ingroup mos_debug_api
 * @brief    MOS_WITNESS — outputs to serial AND stores in buffer.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def MOS_WITNESS
 * @brief Primary logging interface — outputs to serial AND stores in buffer.
 *
 * @param dar_phase    DAR phase (MOS_DAR_DETECT, MOS_DAR_ASSESS, MOS_DAR_RESTORE)
 * @param severity     Severity (MOS_SEV_MILD, MOS_SEV_MEDIUM, MOS_SEV_CRITICAL)
 * @param source       Component name string
 * @param event        Event message string
 * @param health_delta Health impact value
 */
#define MOS_WITNESS(dar_phase, severity, source, event, health_delta) \
    do { \
        mos_witness_serial((dar_phase), (severity), (source), (event), (health_delta)); \
        mos_debug_buffer_event((dar_phase), (severity), (source), (event), (health_delta)); \
    } while (0)

/** @} */ /* end B.5b OPERATIONS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5c HEALTH — Health Queries [HEALTH]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Health is tracked per-event via health_delta parameter] */

/** @} */ /* end B.5c HEALTH */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5d UTILITIES — Consumer Helpers [UTIL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_debug_util B.5d UTILITIES — Convenience Macros
 * @ingroup mos_debug_api
 * @brief    Convenience macros for common logging patterns.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* ─────────────────────────────────────────────────────────────────────────────
 * DAR Phase Convenience Macros (using default component)
 * ───────────────────────────────────────────────────────────────────────────── */

/** @def MOS_DETECT Witness an event (DETECT phase) */
#define MOS_DETECT(sev, msg, health) \
    MOS_WITNESS(MOS_DAR_DETECT, (sev), MOS_DEBUG_COMPONENT, (msg), (health))

/** @def MOS_ASSESS Evaluate significance (ASSESS phase) */
#define MOS_ASSESS(sev, msg, health) \
    MOS_WITNESS(MOS_DAR_ASSESS, (sev), MOS_DEBUG_COMPONENT, (msg), (health))

/** @def MOS_RESTORE Apply learning/recover (RESTORE phase) */
#define MOS_RESTORE(sev, msg, health) \
    MOS_WITNESS(MOS_DAR_RESTORE, (sev), MOS_DEBUG_COMPONENT, (msg), (health))

/* ─────────────────────────────────────────────────────────────────────────────
 * Simple Logging Macros (DETECT phase, neutral health)
 * ───────────────────────────────────────────────────────────────────────────── */

/** @def MOS_LOG_INFO Informational message (neutral health) */
#define MOS_LOG_INFO(msg)   MOS_DETECT(MOS_SEV_MILD, msg, MOS_HEALTH_NEUTRAL)

/** @def MOS_LOG_NOTE Noteworthy message (neutral health) */
#define MOS_LOG_NOTE(msg)   MOS_DETECT(MOS_SEV_MEDIUM, msg, MOS_HEALTH_NEUTRAL)

/** @def MOS_LOG_CRIT Critical message (neutral health) */
#define MOS_LOG_CRIT(msg)   MOS_DETECT(MOS_SEV_CRITICAL, msg, MOS_HEALTH_NEUTRAL)

/* ─────────────────────────────────────────────────────────────────────────────
 * Logging Macros with Health Impact
 * ───────────────────────────────────────────────────────────────────────────── */

/** @def MOS_LOG_OK Success message (positive health) */
#define MOS_LOG_OK(msg)     MOS_DETECT(MOS_SEV_MILD, msg, MOS_HEALTH_SLIGHT_UP)

/** @def MOS_LOG_WARN Warning message (slight negative health) */
#define MOS_LOG_WARN(msg)   MOS_DETECT(MOS_SEV_MEDIUM, msg, MOS_HEALTH_SLIGHT_DOWN)

/** @def MOS_LOG_ERR Error message (moderate negative health) */
#define MOS_LOG_ERR(msg)    MOS_DETECT(MOS_SEV_CRITICAL, msg, MOS_HEALTH_MODERATE_DOWN)

/** @def MOS_LOG_FATAL Fatal message (major negative health) */
#define MOS_LOG_FATAL(msg)  MOS_DETECT(MOS_SEV_CRITICAL, msg, MOS_HEALTH_MAJOR_DOWN)

/* ─────────────────────────────────────────────────────────────────────────────
 * Short Aliases
 * ───────────────────────────────────────────────────────────────────────────── */

#define LOG_I(msg)  MOS_LOG_INFO(msg)   /**< Short: Info */
#define LOG_N(msg)  MOS_LOG_NOTE(msg)   /**< Short: Note */
#define LOG_W(msg)  MOS_LOG_WARN(msg)   /**< Short: Warning */
#define LOG_E(msg)  MOS_LOG_ERR(msg)    /**< Short: Error */
#define LOG_F(msg)  MOS_LOG_FATAL(msg)  /**< Short: Fatal */

/** @} */ /* end B.5d UTILITIES */

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_debug_omission B.6 OMISSION — Reserved Section Guide
 * @ingroup mos_debug_body
 * @brief    What can be reserved and why.
 *
 * NOT IN THIS HEADER:
 *   - Pattern recognition — planned for kernel Witness system
 *   - Event persistence — handled by boot buffer → kernel handoff
 *   - Learning algorithms — kernel Witness responsibility
 *
 * WHY SEPARATE:
 *   Boot debug is intentionally minimal — capture and transmit.
 *   Full Witness functionality belongs in the kernel where resources
 *   are available for learning and pattern recognition.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: B.6 is documentation-only — no code content] */

/** @} */ /* end B.6 OMISSION */

/** @} */ /* end mos_debug_body */

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
///   - X.2 EXECUTION       — Usage patterns and examples
///   - X.3 CLEANUP         — Ownership and memory patterns
///   - X.4 POLICY          — Guide future maintainers
///   - X.5 EXTENSION       — How to expand this component
///   - X.6 TROUBLESHOOTING — Common problems and solutions
///   - X.7 REFERENCE       — Quick lookup (copy-paste ready)
///   - X.8 NOTE            — Final guidance and grounding
///   - X.9 OMISSION        — What can be reserved and why
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_debug_validation X.1 VALIDATION — Verify Correctness
 * @ingroup mos_debug
 *
 * X.1a BUILD:
 * @code{.sh}
 * gcc -ffreestanding -fsyntax-only -Wall -Wextra debug.h
 * @endcode
 *
 * X.1b CHECKS:
 *   - Self-contained (includes serial.h and bootinfo.h)
 *   - No missing type definitions
 *   - MOS_DEBUG_COMPONENT has default value
 *
 * X.1c SELF_TEST:
 * @code{.c}
 * #include "debug.h"
 * int main(void) {
 *     MOS_LOG_INFO("test");
 *     return 0;
 * }
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
 * @defgroup mos_debug_execution X.2 EXECUTION — Usage Patterns
 * @ingroup mos_debug
 *
 * X.2a INCLUDE:
 * @code{.c}
 * #define MOS_DEBUG_COMPONENT "mycomp"  // Optional: set component name
 * #include "debug.h"
 * @endcode
 *
 * X.2c USAGE:
 * @code{.c}
 * // Initialize buffer (optional, for kernel handoff)
 * mos_debug_init_buffer(boot_info->reserved_area);
 *
 * // Simple logging
 * MOS_LOG_INFO("Starting boot...");
 * MOS_LOG_OK("Component initialized");
 * MOS_LOG_WARN("Minor issue detected");
 * MOS_LOG_ERR("Operation failed");
 *
 * // Full DAR pattern
 * MOS_DETECT(MOS_SEV_MILD, "Event observed", MOS_HEALTH_NEUTRAL);
 * MOS_ASSESS(MOS_SEV_MEDIUM, "Evaluating impact", MOS_HEALTH_NEUTRAL);
 * MOS_RESTORE(MOS_SEV_MILD, "Applied correction", MOS_HEALTH_SLIGHT_UP);
 *
 * // Full control
 * MOS_WITNESS(MOS_DAR_DETECT, MOS_SEV_CRITICAL, "custom", "message", -10);
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/* [Execution documented in defgroup above] */
/** @} */ /* end X.2 EXECUTION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership and Memory [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_debug_cleanup X.3 CLEANUP — Ownership and Memory
 * @ingroup mos_debug
 *
 * OWNERSHIP:
 *   - Buffer pointer (mos_boot_event_buffer) — caller provides and owns memory
 *   - Event data — copied into buffer, caller retains original
 *   - String parameters — not stored, only transmitted
 *
 * MEMORY:
 *   - All functions are inline — no dynamic allocation
 *   - Boot event buffer is fixed size (16 events max)
 *   - Buffer overflow silently drops events
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/* [Cleanup documented in defgroup above] */
/** @} */ /* end X.3 CLEANUP */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidelines [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_debug_policy X.4 POLICY — Modification Guidelines
 * @ingroup mos_debug
 *
 * SAFE TO MODIFY:
 *   - Add new convenience macros
 *   - Add new health delta constants
 *   - Increase MOS_BOOT_EVENT_MAX (affects buffer size)
 *
 * MODIFY WITH CARE:
 *   - Serial output format — kernel parser depends on it
 *   - Boot event structure — affects kernel ingestion
 *   - DAR/severity values — affects ternary semantics
 *
 * NEVER MODIFY:
 *   - 4-block structure (METADATA, SETUP, BODY, CLOSING)
 *   - DAR ternary semantics (-1, 0, +1)
 *   - MOS_BOOT_EVENT_MAGIC (kernel uses for validation)
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
 * @defgroup mos_debug_extension X.5 EXTENSION — How to Expand
 * @ingroup mos_debug
 *
 * ADDING MACROS:
 *   - Add to B.5d UTILITIES
 *   - Follow naming: MOS_LOG_* for simple, MOS_* for DAR-aware
 *
 * PLANNED EXTENSIONS:
 *   - Ring buffer mode (overwrite oldest when full)
 *   - Timestamp support
 *   - Stack trace capture
 *
 * @see M.10 ROADMAP for full list.
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
 * @defgroup mos_debug_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @ingroup mos_debug
 *
 * "No serial output":
 *   → Include serial.h before debug.h
 *   → Call mos_serial_init() before any logging
 *   → Check MOS_DEBUG_MIN_SEVERITY isn't filtering
 *
 * "Buffer events not captured":
 *   → Call mos_debug_init_buffer() with valid pointer
 *   → Check buffer hasn't reached MOS_BOOT_EVENT_MAX
 *
 * "Kernel doesn't see events":
 *   → Verify buffer magic is MOS_BOOT_EVENT_MAGIC
 *   → Check buffer pointer passed to kernel
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
 * @defgroup mos_debug_reference X.7 REFERENCE — Quick Lookup
 * @ingroup mos_debug
 *
 * QUICK LOGGING:
 * @code{.c}
 * LOG_I("info");      // Info (I)
 * LOG_N("note");      // Note (N)
 * LOG_W("warning");   // Warning, slight health loss
 * LOG_E("error");     // Error, moderate health loss
 * LOG_F("fatal");     // Fatal, major health loss
 * @endcode
 *
 * DAR VALUES:
 *   MOS_DAR_DETECT  = -1
 *   MOS_DAR_ASSESS  =  0
 *   MOS_DAR_RESTORE = +1
 *
 * SEVERITY VALUES:
 *   MOS_SEV_MILD     = -1 (I)
 *   MOS_SEV_MEDIUM   =  0 (N)
 *   MOS_SEV_CRITICAL = +1 (C)
 *
 * HEALTH VALUES:
 *   MOS_HEALTH_NEUTRAL       =   0
 *   MOS_HEALTH_SLIGHT_UP     =  +5
 *   MOS_HEALTH_SLIGHT_DOWN   =  -5
 *   MOS_HEALTH_MODERATE_UP   = +15
 *   MOS_HEALTH_MODERATE_DOWN = -15
 *   MOS_HEALTH_MAJOR_UP      = +30
 *   MOS_HEALTH_MAJOR_DOWN    = -30
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/* [Reference documented in defgroup above] */
/** @} */ /* end X.7 REFERENCE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Final Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_debug_note X.8 NOTE — Final Guidance
 * @ingroup mos_debug
 *
 * Boot Debug System — Witness-Compatible
 *
 * This header provides early boot logging that integrates with the kernel's
 * DAR Witness System. Key features:
 *
 *   - Uses DAR phases (DETECT/ASSESS/RESTORE) instead of log levels
 *   - Tracks health impact of events (ternary-inspired)
 *   - Outputs parseable format to serial
 *   - Stores events in buffer for kernel ingestion
 *
 * Once the kernel initializes, it can:
 *   1. Parse the boot event buffer
 *   2. Ingest events into MosWitnessState
 *   3. Begin pattern recognition on boot sequence
 *
 * @par Biblical Grounding
 * "Let us search and try our ways, and turn again to the LORD."
 * — Lamentations 3:40
 *
 * The system learns through serving, not surveillance.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/* [Note documented in defgroup above] */
/** @} */ /* end X.8 NOTE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_debug_closing_omission X.9 OMISSION — Reserved Section Guide
 * @ingroup mos_debug
 *
 * SECTION STATUS:
 *   - X.1 Validation:      Active
 *   - X.2 Execution:       Active
 *   - X.3 Cleanup:         Active
 *   - X.4 Policy:          Active
 *   - X.5 Extension:       Active
 *   - X.6 Troubleshooting: Active
 *   - X.7 Reference:       Active
 *   - X.8 Closing Note:    Active
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/* [Reserved: X.9 is documentation-only — no code content] */
/** @} */ /* end X.9 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* MOS_DEBUG_H */
