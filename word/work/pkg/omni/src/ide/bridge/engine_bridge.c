// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         OMNI-IDE-ENGINE-BRIDGE
// Title:       Engine Bridge for OmniCode IDE
// Type:        Source
// Component:   OmniCode IDE
// Role:        Bridge layer between OmniCode IDE and Cornerstone Engine
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     OmniCode IDE and Cornerstone Engine share the same foundation
//              (HAL, DAR, Framework) but serve different purposes:
//
//              - Cornerstone Engine: Game runtime with manifest, discovery, checkpoints
//              - OmniCode IDE: Code editor for OmniCode files
//
//              This bridge provides IDE-appropriate implementations for engine
//              functions that game services call. Rather than stubbing them out,
//              we provide meaningful IDE-mode behavior where possible.
//
// Architecture:
//              ┌─────────────────────────────────────────────────────────────┐
//              │                    OmniCode IDE                              │
//              │         (Code Editor for OmniCode Language)                  │
//              └──────────────────────────┬──────────────────────────────────┘
//                                         │
//              ┌──────────────────────────▼──────────────────────────────────┐
//              │                   ENGINE BRIDGE                              │
//              │     (This file - IDE-mode engine function implementations)   │
//              └──────────────────────────┬──────────────────────────────────┘
//                                         │
//              ┌──────────────────────────▼──────────────────────────────────┐
//              │              Cornerstone Stage Libraries                     │
//              │   Stage 4 (App) → Stage 3 (Framework) → Stage 2 (HAL)       │
//              │                    → Stage 1 (Kernel)                        │
//              └─────────────────────────────────────────────────────────────┘
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// -----------------------------------------------------------------------------
// S.1 System Includes
// -----------------------------------------------------------------------------

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

// -----------------------------------------------------------------------------
// S.2 Forward Declarations
// -----------------------------------------------------------------------------
// Declare types to avoid pulling in heavy engine headers.
// The IDE doesn't need the full type definitions - just enough to satisfy
// the linker for functions it won't actually use.

typedef struct Manifest Manifest;
typedef struct DiscoveredGame DiscoveredGame;

// Engine checkpoint types (mirrors cornerstone.c)
typedef enum {
    ENGINE_CHECKPOINT_STARTUP = 0,
    ENGINE_CHECKPOINT_HAL_INIT,
    ENGINE_CHECKPOINT_FRAMEWORK_INIT,
    ENGINE_CHECKPOINT_GAME_LOAD,
    ENGINE_CHECKPOINT_SCENE_READY,
    ENGINE_CHECKPOINT_SHUTDOWN
} EngineCheckpoint;

// -----------------------------------------------------------------------------
// S.3 Bridge State
// -----------------------------------------------------------------------------
// IDE-specific state that bridges engine expectations.

static struct {
    bool initialized;
    const char* ide_name;
    const char* ide_version;
} ide_bridge_state = {
    .initialized = false,
    .ide_name = "OmniCode IDE",
    .ide_version = "a-01.00"
};

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Bridge Initialization
// -----------------------------------------------------------------------------

/**
 * Initialize the engine bridge for IDE mode.
 * Call this early in IDE startup.
 */
void engine_bridge_init(void) {
    ide_bridge_state.initialized = true;
}

/**
 * Check if bridge is initialized.
 */
bool engine_bridge_is_initialized(void) {
    return ide_bridge_state.initialized;
}

// -----------------------------------------------------------------------------
// B.2 Manifest Bridge
// -----------------------------------------------------------------------------
// The game manifest contains game metadata (title, version, assets).
// The IDE doesn't have a game manifest - it edits code files.

/**
 * Get game manifest.
 *
 * In IDE mode: Returns NULL (no game loaded).
 * Game services that call this will check for NULL and skip game-specific logic.
 *
 * @return NULL in IDE mode (no manifest)
 */
Manifest* engine_get_manifest(void) {
    // IDE has no game manifest
    // Services receiving NULL will operate in "no game" mode
    return NULL;
}

// -----------------------------------------------------------------------------
// B.3 Game Discovery Bridge
// -----------------------------------------------------------------------------
// Game discovery finds available games in the games/ directory.
// The IDE doesn't discover games - it works with the current project.

/**
 * Get discovered games.
 *
 * In IDE mode: Returns empty list (no games discovered).
 * The game selector and title screen check this and behave appropriately.
 *
 * @param count Output: number of games (0 in IDE mode)
 * @return NULL in IDE mode (no discovered games)
 */
const DiscoveredGame* engine_get_discovered_games(int* count) {
    if (count) {
        *count = 0;
    }
    // IDE discovers projects, not games
    // Could later return list of OmniCode projects in workspace
    return NULL;
}

// -----------------------------------------------------------------------------
// B.4 Checkpoint Bridge
// -----------------------------------------------------------------------------
// Engine checkpoints mark progress through the boot sequence.
// The IDE has its own startup sequence but can log checkpoint equivalents.

/**
 * Engine checkpoint.
 *
 * In IDE mode: Logs the checkpoint type for debugging but doesn't
 * affect engine state (since there's no game engine running).
 *
 * @param type Checkpoint type
 * @return false in IDE mode (checkpoint not processed by game engine)
 */
bool engine_checkpoint(EngineCheckpoint type) {
    (void)type;

    // IDE doesn't process game engine checkpoints
    // Could log these for debugging:
    // DAR_LOG_DEBUG("IDE received engine checkpoint: %d", type);

    return false;
}

// -----------------------------------------------------------------------------
// B.5 Future Bridge Points
// -----------------------------------------------------------------------------
// As the codebase evolves, additional bridge functions may be needed.
// Document them here as they're identified.
//
// Potential future bridges:
// - engine_get_current_scene() -> IDE scene preview
// - engine_get_game_state() -> IDE state inspection
// - engine_send_event() -> IDE event simulation

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// BRIDGE ARCHITECTURE NOTES:
//
// This bridge exists because OmniCode IDE and Cornerstone Engine share
// foundation code but serve different purposes:
//
// SHARED (Stage 1-3):
// - HAL (display, input, audio)
// - DAR (logging, debugging, recovery)
// - Framework utilities (fonts, rendering, UI widgets)
//
// DIVERGENT (Stage 4-5):
// - Game Engine: Manifest, discovery, game loop, scenes
// - Code IDE: Project files, syntax highlighting, editing
//
// The bridge allows the IDE to link against stage libraries that include
// game services, with those services gracefully degrading when they
// detect "no game" mode (NULL manifest, zero discovered games, etc.).
//
// FUTURE DIRECTION:
// As the stack matures, consider:
// 1. Separating "engine core" from "game runtime" in stage libraries
// 2. Creating a proper plugin architecture where IDE and Game are plugins
// 3. Unifying IDE and editor.sys into a single "creative tools" layer
//
// =============================================================================
// END CLOSING
// =============================================================================
