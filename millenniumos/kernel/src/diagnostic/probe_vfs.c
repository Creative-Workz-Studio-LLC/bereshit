// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-diagnostic-probe-vfs
// Title:       VFS Diagnostic Probe
// Type:        Source
// Component:   Kernel / Diagnostic Subsystem
// Role:        Diagnostic probe for Virtual Filesystem
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Register VFS with diagnostic service for state queries,
//              invariant checks, and visual demonstration.
//
// Grounding:   Genesis 1:9 — "Let the waters be gathered together"
//              Files gathered into organized structure - this probe observes it.
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "diagnostic.h"
#include "vfs/vfs.h"
#include "graphics/graphics.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 VFS State Query
// -----------------------------------------------------------------------------

static mos_result_t vfs_probe_query(MosDiagState* out) {
    if (!out) return MOS_ERR_INVALID;

    const MosVfs* vfs = mos_vfs_info();
    if (!vfs) return MOS_ERR_NODEV;

    // Copy name
    out->name[0] = 'V'; out->name[1] = 'F'; out->name[2] = 'S';
    out->name[3] = '\0';

    // Get VFS health
    out->health = vfs->health;

    // Determine cognitive state
    if (out->health > 50) {
        out->state = MOS_STATE_TOV;
    } else if (out->health > 0) {
        out->state = MOS_STATE_SHALEM;
    } else if (out->health == 0) {
        out->state = MOS_STATE_YASHAR;
    } else if (out->health > -50) {
        out->state = MOS_STATE_CHASER;
    } else {
        out->state = MOS_STATE_SHAVAR;
    }

    // Calculate event count as sum of operations
    out->event_count = vfs->total_opens + vfs->total_reads + vfs->total_writes;
    out->error_count = 0;  // VFS doesn't track errors separately

    // Status message based on mount state
    if (vfs->mount_count == 0) {
        out->status_message = "No filesystems mounted";
    } else if (vfs->root_fs == NULL) {
        out->status_message = "No root filesystem";
    } else if (out->health > 50) {
        out->status_message = "VFS excellent";
    } else if (out->health > 0) {
        out->status_message = "VFS operational";
    } else {
        out->status_message = "VFS degraded";
    }

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.2 VFS Invariant Check
// -----------------------------------------------------------------------------

static mos_result_t vfs_probe_check(void) {
    const MosVfs* vfs = mos_vfs_info();
    if (!vfs) return MOS_ERR_NODEV;

    mos_size_t errors = 0;

    // Check: mount_count should not exceed maximum
    if (vfs->mount_count > MOS_MAX_MOUNTS) {
        errors++;
    }

    // Check: if we have mounts, root should be set
    if (vfs->mount_count > 0 && vfs->root_fs == NULL) {
        errors++;
    }

    // Check: root_dentry should exist if root_fs exists
    if (vfs->root_fs != NULL && vfs->root_dentry == NULL) {
        errors++;
    }

    // Check each mount point
    for (mos_size_t i = 0; i < vfs->mount_count && i < MOS_MAX_MOUNTS; i++) {
        const MosSuperblock* sb = vfs->mounts[i];
        if (sb == NULL) continue;

        // Check: superblock should have operations
        if (sb->ops == NULL) {
            errors++;
        }

        // Check: superblock should have root dentry
        if (sb->root == NULL) {
            errors++;
        }

        // Check: block size should be reasonable (512 to 64K)
        if (sb->blocksize < 512 || sb->blocksize > 65536) {
            errors++;
        }

        // Check: free blocks should not exceed total
        if (sb->free_blocks > sb->total_blocks) {
            errors++;
        }

        // Check: free inodes should not exceed total
        if (sb->free_inodes > sb->total_inodes) {
            errors++;
        }

        // Check: health should be in valid range
        // Note: int8_t can't exceed MOS_HEALTH_MAX (127), only check lower bound
        if (sb->health < MOS_HEALTH_MIN) {
            errors++;
        }
    }

    return (errors == 0) ? MOS_OK : (mos_result_t)(-errors);
}

// -----------------------------------------------------------------------------
// B.3 VFS Visual Demo
// -----------------------------------------------------------------------------

static void vfs_probe_demo(MosGraphicsContext* ctx, MosRect bounds) {
    if (!ctx) return;

    const MosVfs* vfs = mos_vfs_info();
    if (!vfs) return;

    int32_t x = bounds.x + 20;
    int32_t y = bounds.y + 20;

    // Title
    mos_gfx_string(ctx, x, y, "Virtual Filesystem State", MOS_COLOR_GOLD, MOS_COLOR_BG_DARK);
    y += 30;

    // Mount table
    mos_gfx_string(ctx, x, y, "MOUNT TABLE:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    char buf[80];
    char num[16];
    int pos;
    int digits;
    mos_size_t val;

    if (vfs->mount_count == 0) {
        mos_gfx_string(ctx, x + 20, y, "(no filesystems mounted)", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
        y += 20;
    } else {
        for (mos_size_t i = 0; i < vfs->mount_count && i < MOS_MAX_MOUNTS; i++) {
            const MosSuperblock* sb = vfs->mounts[i];
            if (sb == NULL) continue;

            // Draw mount entry
            pos = 0;
            buf[pos++] = ' '; buf[pos++] = ' ';
            buf[pos++] = '['; buf[pos++] = '0' + i; buf[pos++] = ']';
            buf[pos++] = ' ';

            // Zone indicator
            switch (sb->zone) {
                case MOS_ZONE_VOID:
                    buf[pos++] = 'v'; buf[pos++] = 'o'; buf[pos++] = 'i'; buf[pos++] = 'd';
                    break;
                case MOS_ZONE_WORD:
                    buf[pos++] = 'w'; buf[pos++] = 'o'; buf[pos++] = 'r'; buf[pos++] = 'd';
                    break;
                case MOS_ZONE_TOV:
                    buf[pos++] = 't'; buf[pos++] = 'o'; buf[pos++] = 'v'; buf[pos++] = ' ';
                    break;
            }
            buf[pos++] = ' ';
            buf[pos++] = '|';
            buf[pos++] = ' ';

            // Block size
            buf[pos++] = 'b'; buf[pos++] = 's'; buf[pos++] = '=';
            digits = 0;
            val = sb->blocksize;
            do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
            while (digits > 0) buf[pos++] = num[--digits];
            buf[pos++] = ' ';
            buf[pos++] = '|';
            buf[pos++] = ' ';

            // Health
            buf[pos++] = 'h'; buf[pos++] = '=';
            int32_t h = sb->health;
            if (h < 0) { buf[pos++] = '-'; h = -h; }
            digits = 0;
            do { num[digits++] = '0' + (h % 10); h /= 10; } while (h && digits < 15);
            if (digits == 0) num[digits++] = '0';
            while (digits > 0) buf[pos++] = num[--digits];

            buf[pos] = '\0';

            MosColor color = sb->health > 0 ? MOS_COLOR_SUCCESS :
                            (sb->health < 0 ? MOS_COLOR_ERROR : MOS_COLOR_TEXT);
            mos_gfx_string(ctx, x, y, buf, color, MOS_COLOR_BG_DARK);
            y += 18;

            // Space usage bar for this mount
            int32_t bar_x = x + 30;
            int32_t bar_w = 200;
            int32_t bar_h = 12;

            mos_gfx_fill_rect(ctx, (MosRect){bar_x, y, bar_w, bar_h}, MOS_RGB(40, 40, 40));

            if (sb->total_blocks > 0) {
                int32_t used_blocks = sb->total_blocks - sb->free_blocks;
                int32_t used_w = (int32_t)((used_blocks * bar_w) / sb->total_blocks);
                if (used_w > 0) {
                    int32_t used_pct = (int32_t)((used_blocks * 100) / sb->total_blocks);
                    MosColor fill_color = MOS_COLOR_INFO;
                    if (used_pct > 90) fill_color = MOS_COLOR_ERROR;
                    else if (used_pct > 75) fill_color = MOS_COLOR_WARNING;

                    mos_gfx_fill_rect(ctx, (MosRect){bar_x, y, used_w, bar_h}, fill_color);
                }
            }
            mos_gfx_draw_rect(ctx, (MosRect){bar_x, y, bar_w, bar_h}, MOS_COLOR_TEXT_DIM, 1);

            y += bar_h + 8;
        }
    }

    y += 20;

    // I/O Statistics
    mos_gfx_string(ctx, x, y, "I/O STATISTICS:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    // Opens
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = 'O'; buf[pos++] = 'p'; buf[pos++] = 'e';
    buf[pos++] = 'n'; buf[pos++] = 's'; buf[pos++] = ':';
    buf[pos++] = ' '; buf[pos++] = ' '; buf[pos++] = ' ';
    digits = 0;
    val = vfs->total_opens;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    if (digits == 0) num[digits++] = '0';
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
    y += 18;

    // Reads
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = 'R'; buf[pos++] = 'e'; buf[pos++] = 'a';
    buf[pos++] = 'd'; buf[pos++] = 's'; buf[pos++] = ':';
    buf[pos++] = ' '; buf[pos++] = ' '; buf[pos++] = ' ';
    digits = 0;
    val = vfs->total_reads;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    if (digits == 0) num[digits++] = '0';
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_SUCCESS, MOS_COLOR_BG_DARK);
    y += 18;

    // Writes
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = 'W'; buf[pos++] = 'r'; buf[pos++] = 'i';
    buf[pos++] = 't'; buf[pos++] = 'e'; buf[pos++] = 's';
    buf[pos++] = ':'; buf[pos++] = ' '; buf[pos++] = ' ';
    digits = 0;
    val = vfs->total_writes;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    if (digits == 0) num[digits++] = '0';
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_WARNING, MOS_COLOR_BG_DARK);

    y += 40;

    // Directory tree visualization (simplified)
    mos_gfx_string(ctx, x, y, "ROOT STRUCTURE:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    if (vfs->root_dentry) {
        // Draw root
        mos_gfx_string(ctx, x + 20, y, "/", MOS_COLOR_GOLD, MOS_COLOR_BG_DARK);
        y += 18;

        // Draw first level children if available
        MosDentry* child = vfs->root_dentry->children;
        int child_count = 0;
        while (child && child_count < 8) {
            pos = 0;
            buf[pos++] = ' '; buf[pos++] = ' ';
            buf[pos++] = 0xC0; buf[pos++] = 0xC4; buf[pos++] = ' ';  // └─

            // Copy name
            const char* name = child->name;
            while (*name && pos < 70) {
                buf[pos++] = *name++;
            }

            // Directory indicator (use MOS_FTYPE_DIRECTORY not MOS_FILE_DIRECTORY)
            if (child->inode && child->inode->type == MOS_FTYPE_DIRECTORY) {
                buf[pos++] = '/';
            }
            buf[pos] = '\0';

            MosColor color = MOS_COLOR_TEXT;
            if (child->inode) {
                // Use MOS_FTYPE_* constants (trit5_t values) instead of MOS_FILE_*
                if (child->inode->type == MOS_FTYPE_DIRECTORY) color = MOS_COLOR_INFO;
                else if (child->inode->type == MOS_FTYPE_CHARDEV ||
                         child->inode->type == MOS_FTYPE_BLOCKDEV) color = MOS_COLOR_WARNING;
            }

            mos_gfx_string(ctx, x + 20, y, buf, color, MOS_COLOR_BG_DARK);
            y += 18;

            child = child->next;
            child_count++;
        }

        if (child) {
            mos_gfx_string(ctx, x + 40, y, "...", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
        }
    } else {
        mos_gfx_string(ctx, x + 20, y, "(no root mounted)", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    }

    // Footer
    y = bounds.y + bounds.height - 30;
    mos_gfx_string(ctx, x, y, "[N]ext probe  [P]revious probe  [ESC] Exit demo",
                   MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
}

// -----------------------------------------------------------------------------
// B.4 Registration
// -----------------------------------------------------------------------------

mos_result_t mos_diag_probe_vfs_register(void) {
    return mos_diag_register_probe("VFS",
                                    vfs_probe_query,
                                    vfs_probe_check,
                                    vfs_probe_demo);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// VFS Diagnostic Probe provides:
//   - State query: mount count, I/O statistics, health
//   - Invariant check: superblock consistency, dentry validity
//   - Visual demo: mount table, space usage, directory tree
//
// =============================================================================
// END CLOSING
// =============================================================================
