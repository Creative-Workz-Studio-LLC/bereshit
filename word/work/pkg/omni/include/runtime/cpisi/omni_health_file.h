// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-health-file
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_health_file.h — Health File Integration
// Bridges DAR health files to CPI-SI logging system.

#ifndef BERESHIT_OMNI_HEALTH_FILE_H
#define BERESHIT_OMNI_HEALTH_FILE_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key: B-word-work-pkg-omni-health-file
// Title: Health File Integration
// Type: Header
// Role: Bridge .health files to cpisi_log for persistent state tracking
//
// Scripture: Jeremiah 17:14 — Heal me, O LORD, and I shall be healed
// Principle: Health must be tracked, assessed, and recoverable
// Anchor: Proverbs 24:16 — A just man falleth seven times, and riseth up again
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>

// # S.1 Defines [DEFINES]

// File format magic numbers
#define HEALTH_FILE_MAGIC_V1   0x00      // Single byte (legacy)
#define HEALTH_FILE_MAGIC_V2   0x48465632  // "HFV2" - new format

#define HEALTH_FILE_VERSION    2
#define HEALTH_FILE_NAME       ".health"
#define HEALTH_FILE_PATH_MAX   1024  // OS-level path support

// Health value range (Base100: -100 to +100)
// Guard against redefinition if cpisi_log.h is included first
#ifndef HEALTH_MIN
#define HEALTH_MIN             (-100)
#endif
#ifndef HEALTH_MAX
#define HEALTH_MAX             (+100)
#endif
#ifndef HEALTH_BALANCED
#define HEALTH_BALANCED        (0)
#endif

// Zone identifiers
#define HEALTH_ZONE_VOID       (-1)      // Entry gate
#define HEALTH_ZONE_WORD       (0)       // Processing
#define HEALTH_ZONE_TOV        (+1)      // Exit gate

// # S.2 Types [TYPES]

// # S.2a Error Codes [ERRORS]

typedef enum HealthFileError {
    HEALTH_FILE_OK = 0,
    HEALTH_FILE_ERR_NULL,
    HEALTH_FILE_ERR_OPEN,
    HEALTH_FILE_ERR_READ,
    HEALTH_FILE_ERR_WRITE,
    HEALTH_FILE_ERR_CORRUPT,
    HEALTH_FILE_ERR_VERSION,
    HEALTH_FILE_ERR_RANGE,
} HealthFileError;

// # S.2b Hebrew State Encoding [HEBREW]
//
// Maps health score to Hebrew states for cpisi_log integration.
//
//   -99 to -50 → shavar (broken, crisis)
//   -49 to -25 → chaser (lacking, needs attention)
//   -24 to  +24 → yashar (even, normal operation)
//   +25 to  +49 → shalem (whole, healthy growth)
//   +50 to  +99 → tov (excellent, peak performance)

typedef enum HealthHebrewState {
    HEALTH_SHAVAR = -2,   // Broken (שָׁבַר)
    HEALTH_CHASER = -1,   // Lacking (חָסֵר)
    HEALTH_YASHAR =  0,   // Even (יָשָׁר)
    HEALTH_SHALEM =  1,   // Whole (שָׁלֵם)
    HEALTH_TOV    =  2,   // Good (טוֹב)
} HealthHebrewState;

// # S.2c Health File Header (V2 format) [HEADER]
//
// 16-byte header for .health files (V2 format)

typedef struct {
    uint32_t    magic;          // HEALTH_FILE_MAGIC_V2
    uint8_t     version;        // HEALTH_FILE_VERSION
    int8_t      health;         // Health score (-99 to +99)
    int8_t      hebrew_state;   // Derived Hebrew state
    int8_t      zone;           // Zone identifier (-1, 0, +1)
    uint64_t    timestamp;      // Unix timestamp of last update
} HealthFileHeader;

// # S.2d Zone Health State [ZONE]
//
// Aggregated health across all zones

typedef struct {
    int8_t      void_health;    // void/.health score
    int8_t      word_health;    // word/.health score
    int8_t      tov_health;     // tov/.health score
    int8_t      aggregate;      // Weighted average
    char        base_path[HEALTH_FILE_PATH_MAX];  // Root directory
    bool        loaded;         // Whether zone health is loaded
} ZoneHealth;

// # S.2e Health File Context [CONTEXT]
//
// Tracks health file state and provides sync to cpisi_log

typedef struct HealthFileContext {
    ZoneHealth  zones;          // Zone health state
    bool        auto_sync;      // Auto-sync to cpisi_log
    void*       log_ctx;        // Pointer to LogContext (cpisi_log.h)
    int8_t      last_health;    // Last synced health value
    uint64_t    last_sync;      // Timestamp of last sync
} HealthFileContext;

// # S.3 Function Prototypes [PROTOTYPES]

// # S.3a Lifecycle [LIFECYCLE]

// Initialize health file context
void health_file_init(HealthFileContext* ctx);

// Shutdown health file context
void health_file_shutdown(HealthFileContext* ctx);

// # S.3b File Operations [FILE_OPS]

// Read health from file (handles V1 and V2 formats)
// Returns health value (-99 to +99), or 0 on error
HealthFileError health_file_read(const char* path, int8_t* health);

// Write health to file (V2 format)
HealthFileError health_file_write(const char* path, int8_t health, int8_t zone);

// Read health from directory's .health file
HealthFileError health_file_read_dir(const char* dir_path, int8_t* health);

// Write health to directory's .health file
HealthFileError health_file_write_dir(const char* dir_path, int8_t health, int8_t zone);

// # S.3c Zone Operations [ZONE_OPS]

// Load all zone health files
HealthFileError health_zones_load(HealthFileContext* ctx, const char* base_path);

// Save all zone health files
HealthFileError health_zones_save(HealthFileContext* ctx);

// Get zone health by zone ID
int8_t health_zones_get(HealthFileContext* ctx, int8_t zone);

// Set zone health by zone ID
HealthFileError health_zones_set(HealthFileContext* ctx, int8_t zone, int8_t health);

// Get aggregate health
int8_t health_zones_aggregate(HealthFileContext* ctx);

// # S.3d Hebrew State Operations [HEBREW_OPS]

// Convert health score to Hebrew state
HealthHebrewState health_to_hebrew(int8_t health);

// Get Hebrew state name
const char* health_hebrew_name(HealthHebrewState state);

// Get Hebrew state emoji
const char* health_hebrew_emoji(HealthHebrewState state);

// # S.3e cpisi_log Integration [LOG_OPS]

// Connect to cpisi_log context (enables auto-sync)
void health_file_connect_log(HealthFileContext* ctx, void* log_ctx);

// Disconnect from cpisi_log
void health_file_disconnect_log(HealthFileContext* ctx);

// Sync health to cpisi_log (manual trigger)
void health_file_sync_to_log(HealthFileContext* ctx);

// Sync health from cpisi_log (manual trigger)
void health_file_sync_from_log(HealthFileContext* ctx);

// # S.3f Utilities [UTILITIES]

// Clamp health to valid range
int8_t health_clamp(int value);

// Get error string
const char* health_file_error_string(HealthFileError err);

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================
//
// Organization:
//   Types: 3 enums (error, hebrew, zone), 3 structs (header, zone, context)
//   Functions: 2 lifecycle, 4 file ops, 5 zone ops, 3 hebrew ops, 4 log ops, 3 utils
//
// Integration Flow:
//
//   .health files ←→ HealthFileContext ←→ cpisi_log (LogContext)
//        ↓                   ↓                    ↓
//   Persistent        Zone aggregation      Logging + state
//   filesystem        + Hebrew states       tracking
//
// V1 vs V2 Format:
//
//   V1: Single byte (health + 100). Range 0x00-0xC8 (health -100 to +100)
//   V2: 16-byte header with magic, version, hebrew state, zone, timestamp
//
//   health_file_read() handles both formats automatically.
//   health_file_write() always writes V2 format.
//
// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// "Heal me, O LORD, and I shall be healed; save me, and I shall be saved."
//   — Jeremiah 17:14
//
// Health tracking enables detection, assessment, and recovery.
// The body remembers what the mind forgets.
//
// Related:
//   - omni_dar.h — DAR (Detect-Assess-Recover) system
//   - cpisi_log.h — CPI-SI logging with Hebrew states
//   - session.h — Session persistence
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_HEALTH_FILE_H
