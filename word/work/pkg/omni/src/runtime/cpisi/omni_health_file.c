// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-health-file-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_health_file.c — Health File Integration Implementation
// Bridges DAR health files to CPI-SI logging system.

// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key: B-word-work-pkg-omni-health-file-impl
// Title: Health File Integration Implementation
// Type: Source
// Role: Bridge .health files to cpisi_log for persistent state tracking
//
// Scripture: Jeremiah 17:14 — Heal me, O LORD, and I shall be healed
// Principle: Health must be tracked, assessed, and recoverable
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "omni_health_file.h"

// Include cpisi_log for integration (forward declared in header)
// Direct include when building from cornerstone -Iengine/core/include/util
#include "kernel/dar/phase/detect.phase.h"

// # S.1 Internal Helpers [HELPERS]

// V1 format encoding (legacy compatibility)
#define V1_ENCODE(h)    ((uint8_t)((h) + 100))
#define V1_DECODE(b)    ((int8_t)((b) - 100))

// Safe string copy
static void safe_strcpy(char* dest, size_t size, const char* src) {
    if (!dest || size == 0) return;
    if (!src) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}

// Build path to .health file in directory
// HEALTH_FILE_NAME is ".health" (7 chars) plus "/" (1 char) plus null (1 char) = 9 bytes needed
static void build_health_path(char* out, size_t size, const char* dir) {
    if (!out || size == 0 || !dir) return;

    // Ensure we have room for dir + "/" + ".health" + null
    size_t dir_len = strlen(dir);
    size_t needed = dir_len + 1 + sizeof(HEALTH_FILE_NAME);  // dir + "/" + ".health\0"
    if (needed > size) {
        // Truncate dir portion to fit
        size_t max_dir = size > 9 ? size - 9 : 0;  // Reserve 9 for "/.health\0"
        snprintf(out, size, "%.*s/%s", (int)max_dir, dir, HEALTH_FILE_NAME);
    } else {
        snprintf(out, size, "%s/%s", dir, HEALTH_FILE_NAME);
    }
}

// Get current timestamp
static uint64_t current_timestamp(void) {
    return (uint64_t)time(NULL);
}

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Lifecycle [LIFECYCLE]

void health_file_init(HealthFileContext* ctx) {
    if (!ctx) return;

    memset(ctx, 0, sizeof(HealthFileContext));
    ctx->zones.void_health = 0;
    ctx->zones.word_health = 0;
    ctx->zones.tov_health = 0;
    ctx->zones.aggregate = 0;
    ctx->auto_sync = false;
    ctx->log_ctx = NULL;
    ctx->last_health = 0;
    ctx->last_sync = 0;
}

void health_file_shutdown(HealthFileContext* ctx) {
    if (!ctx) return;

    // Save zones before shutdown
    if (ctx->zones.loaded) {
        health_zones_save(ctx);
    }

    // Disconnect from log
    if (ctx->log_ctx) {
        health_file_disconnect_log(ctx);
    }
}

// # B.2 File Operations [FILE_OPS]

HealthFileError health_file_read(const char* path, int8_t* health) {
    if (!path || !health) return HEALTH_FILE_ERR_NULL;

    FILE* f = fopen(path, "rb");
    if (!f) return HEALTH_FILE_ERR_OPEN;

    // Get file size to determine format
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size == 1) {
        // V1 format: single byte
        uint8_t byte = 0;
        size_t read = fread(&byte, 1, 1, f);
        fclose(f);

        if (read != 1) return HEALTH_FILE_ERR_READ;

        int8_t decoded = V1_DECODE(byte);

        // Clamp to valid range (V1 used -100 to +100, we use -99 to +99)
        *health = health_clamp(decoded);
        return HEALTH_FILE_OK;
    }

    if (size >= (long)sizeof(HealthFileHeader)) {
        // V2 format: header structure
        HealthFileHeader header;
        size_t read = fread(&header, sizeof(HealthFileHeader), 1, f);
        fclose(f);

        if (read != 1) return HEALTH_FILE_ERR_READ;

        // Validate magic
        if (header.magic != HEALTH_FILE_MAGIC_V2) {
            // Unknown format - try to extract first byte as V1
            *health = health_clamp(V1_DECODE((uint8_t)(header.magic & 0xFF)));
            return HEALTH_FILE_OK;
        }

        // Validate version
        if (header.version > HEALTH_FILE_VERSION) {
            return HEALTH_FILE_ERR_VERSION;
        }

        *health = health_clamp(header.health);
        return HEALTH_FILE_OK;
    }

    // Unknown format
    fclose(f);
    *health = 0;
    return HEALTH_FILE_ERR_CORRUPT;
}

HealthFileError health_file_write(const char* path, int8_t health, int8_t zone) {
    if (!path) return HEALTH_FILE_ERR_NULL;

    FILE* f = fopen(path, "wb");
    if (!f) return HEALTH_FILE_ERR_OPEN;

    // Build V2 header
    HealthFileHeader header = {
        .magic = HEALTH_FILE_MAGIC_V2,
        .version = HEALTH_FILE_VERSION,
        .health = health_clamp(health),
        .hebrew_state = (int8_t)health_to_hebrew(health),
        .zone = zone,
        .timestamp = current_timestamp(),
    };

    size_t written = fwrite(&header, sizeof(HealthFileHeader), 1, f);
    fclose(f);

    if (written != 1) return HEALTH_FILE_ERR_WRITE;

    return HEALTH_FILE_OK;
}

HealthFileError health_file_read_dir(const char* dir_path, int8_t* health) {
    if (!dir_path || !health) return HEALTH_FILE_ERR_NULL;

    // Extra space for "/.health" suffix (9 chars + null)
    char path[HEALTH_FILE_PATH_MAX + 16];
    build_health_path(path, sizeof(path), dir_path);

    return health_file_read(path, health);
}

HealthFileError health_file_write_dir(const char* dir_path, int8_t health, int8_t zone) {
    if (!dir_path) return HEALTH_FILE_ERR_NULL;

    // Extra space for "/.health" suffix (9 chars + null)
    char path[HEALTH_FILE_PATH_MAX + 16];
    build_health_path(path, sizeof(path), dir_path);

    return health_file_write(path, health, zone);
}

// # B.3 Zone Operations [ZONE_OPS]

HealthFileError health_zones_load(HealthFileContext* ctx, const char* base_path) {
    if (!ctx || !base_path) return HEALTH_FILE_ERR_NULL;

    safe_strcpy(ctx->zones.base_path, sizeof(ctx->zones.base_path), base_path);

    // Buffer large enough for base_path + "/void" or similar
    char path[HEALTH_FILE_PATH_MAX + 16];
    HealthFileError err;

    // Load void health
    snprintf(path, sizeof(path), "%s/void", base_path);
    err = health_file_read_dir(path, &ctx->zones.void_health);
    if (err != HEALTH_FILE_OK && err != HEALTH_FILE_ERR_OPEN) {
        // File doesn't exist is OK, other errors are not
        if (err != HEALTH_FILE_ERR_OPEN) {
            ctx->zones.void_health = 0;
        }
    }

    // Load word health
    snprintf(path, sizeof(path), "%s/word", base_path);
    err = health_file_read_dir(path, &ctx->zones.word_health);
    if (err != HEALTH_FILE_OK && err != HEALTH_FILE_ERR_OPEN) {
        ctx->zones.word_health = 0;
    }

    // Load tov health
    snprintf(path, sizeof(path), "%s/tov", base_path);
    err = health_file_read_dir(path, &ctx->zones.tov_health);
    if (err != HEALTH_FILE_OK && err != HEALTH_FILE_ERR_OPEN) {
        ctx->zones.tov_health = 0;
    }

    // Calculate aggregate (weighted: word is center, void/tov are gates)
    // Weight: void=1, word=2, tov=1 → total=4
    int sum = ctx->zones.void_health + (ctx->zones.word_health * 2) + ctx->zones.tov_health;
    ctx->zones.aggregate = health_clamp(sum / 4);

    ctx->zones.loaded = true;

    return HEALTH_FILE_OK;
}

HealthFileError health_zones_save(HealthFileContext* ctx) {
    if (!ctx || !ctx->zones.loaded) return HEALTH_FILE_ERR_NULL;

    // Buffer large enough for base_path + "/void" or similar
    char path[HEALTH_FILE_PATH_MAX + 16];
    HealthFileError err;

    // Save void health
    snprintf(path, sizeof(path), "%s/void", ctx->zones.base_path);
    err = health_file_write_dir(path, ctx->zones.void_health, HEALTH_ZONE_VOID);
    if (err != HEALTH_FILE_OK) return err;

    // Save word health
    snprintf(path, sizeof(path), "%s/word", ctx->zones.base_path);
    err = health_file_write_dir(path, ctx->zones.word_health, HEALTH_ZONE_WORD);
    if (err != HEALTH_FILE_OK) return err;

    // Save tov health
    snprintf(path, sizeof(path), "%s/tov", ctx->zones.base_path);
    err = health_file_write_dir(path, ctx->zones.tov_health, HEALTH_ZONE_TOV);
    if (err != HEALTH_FILE_OK) return err;

    return HEALTH_FILE_OK;
}

int8_t health_zones_get(HealthFileContext* ctx, int8_t zone) {
    if (!ctx) return 0;

    switch (zone) {
        case HEALTH_ZONE_VOID: return ctx->zones.void_health;
        case HEALTH_ZONE_WORD: return ctx->zones.word_health;
        case HEALTH_ZONE_TOV:  return ctx->zones.tov_health;
        default:               return ctx->zones.aggregate;
    }
}

HealthFileError health_zones_set(HealthFileContext* ctx, int8_t zone, int8_t health) {
    if (!ctx) return HEALTH_FILE_ERR_NULL;

    int8_t clamped = health_clamp(health);

    switch (zone) {
        case HEALTH_ZONE_VOID:
            ctx->zones.void_health = clamped;
            break;
        case HEALTH_ZONE_WORD:
            ctx->zones.word_health = clamped;
            break;
        case HEALTH_ZONE_TOV:
            ctx->zones.tov_health = clamped;
            break;
        default:
            return HEALTH_FILE_ERR_RANGE;
    }

    // Recalculate aggregate
    int sum = ctx->zones.void_health + (ctx->zones.word_health * 2) + ctx->zones.tov_health;
    ctx->zones.aggregate = health_clamp(sum / 4);

    // Auto-sync to log if enabled
    if (ctx->auto_sync && ctx->log_ctx) {
        health_file_sync_to_log(ctx);
    }

    return HEALTH_FILE_OK;
}

int8_t health_zones_aggregate(HealthFileContext* ctx) {
    if (!ctx) return 0;
    return ctx->zones.aggregate;
}

// # B.4 Hebrew State Operations [HEBREW_OPS]

HealthHebrewState health_to_hebrew(int8_t health) {
    if (health <= -50) return HEALTH_SHAVAR;
    if (health <= -25) return HEALTH_CHASER;
    if (health <= +24) return HEALTH_YASHAR;
    if (health <= +49) return HEALTH_SHALEM;
    return HEALTH_TOV;
}

const char* health_hebrew_name(HealthHebrewState state) {
    switch (state) {
        case HEALTH_SHAVAR: return "shavar";
        case HEALTH_CHASER: return "chaser";
        case HEALTH_YASHAR: return "yashar";
        case HEALTH_SHALEM: return "shalem";
        case HEALTH_TOV:    return "tov";
        default:            return "unknown";
    }
}

const char* health_hebrew_emoji(HealthHebrewState state) {
    switch (state) {
        case HEALTH_SHAVAR: return "💔";  // Broken heart
        case HEALTH_CHASER: return "⚠️";   // Warning
        case HEALTH_YASHAR: return "⚖️";   // Balanced
        case HEALTH_SHALEM: return "✨";  // Whole
        case HEALTH_TOV:    return "🌟";  // Excellent
        default:            return "❓";  // Unknown
    }
}

// # B.5 cpisi_log Integration [LOG_OPS]

void health_file_connect_log(HealthFileContext* ctx, void* log_ctx) {
    if (!ctx) return;

    ctx->log_ctx = log_ctx;
    ctx->auto_sync = true;

    // Initial sync
    if (ctx->zones.loaded) {
        health_file_sync_to_log(ctx);
    }
}

void health_file_disconnect_log(HealthFileContext* ctx) {
    if (!ctx) return;

    // Final sync before disconnect
    if (ctx->auto_sync && ctx->log_ctx && ctx->zones.loaded) {
        health_file_sync_from_log(ctx);
    }

    ctx->log_ctx = NULL;
    ctx->auto_sync = false;
}

void health_file_sync_to_log(HealthFileContext* ctx) {
    if (!ctx || !ctx->log_ctx) return;

    LogContext* log = (LogContext*)ctx->log_ctx;
    int8_t aggregate = ctx->zones.aggregate;

    // Only sync if health changed
    if (aggregate != ctx->last_health) {
        cpisi_log_set_health(log, aggregate);
        ctx->last_health = aggregate;
        ctx->last_sync = current_timestamp();
    }
}

void health_file_sync_from_log(HealthFileContext* ctx) {
    if (!ctx || !ctx->log_ctx) return;

    LogContext* log = (LogContext*)ctx->log_ctx;
    int8_t log_health = cpisi_log_get_health(log);

    // Distribute health change across zones proportionally
    if (log_health != ctx->last_health) {
        int8_t delta = log_health - ctx->last_health;

        // Distribute delta: word gets 50%, void/tov get 25% each
        int8_t word_delta = (delta * 2) / 4;
        int8_t gate_delta = delta / 4;

        ctx->zones.word_health = health_clamp(ctx->zones.word_health + word_delta);
        ctx->zones.void_health = health_clamp(ctx->zones.void_health + gate_delta);
        ctx->zones.tov_health = health_clamp(ctx->zones.tov_health + gate_delta);

        // Recalculate aggregate
        int sum = ctx->zones.void_health + (ctx->zones.word_health * 2) + ctx->zones.tov_health;
        ctx->zones.aggregate = health_clamp(sum / 4);

        ctx->last_health = ctx->zones.aggregate;
        ctx->last_sync = current_timestamp();
    }
}

// # B.6 Utilities [UTILITIES]

int8_t health_clamp(int value) {
    if (value < HEALTH_MIN) return HEALTH_MIN;
    if (value > HEALTH_MAX) return HEALTH_MAX;
    return (int8_t)value;
}

const char* health_file_error_string(HealthFileError err) {
    switch (err) {
        case HEALTH_FILE_OK:          return "success";
        case HEALTH_FILE_ERR_NULL:    return "null argument";
        case HEALTH_FILE_ERR_OPEN:    return "cannot open file";
        case HEALTH_FILE_ERR_READ:    return "read error";
        case HEALTH_FILE_ERR_WRITE:   return "write error";
        case HEALTH_FILE_ERR_CORRUPT: return "corrupt file";
        case HEALTH_FILE_ERR_VERSION: return "unsupported version";
        case HEALTH_FILE_ERR_RANGE:   return "value out of range";
        default:                      return "unknown error";
    }
}

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
// The health file integration bridges persistent filesystem state
// to the runtime logging system. The body remembers what the mind forgets.
//
// =============================================================================
// END CLOSING
// =============================================================================
