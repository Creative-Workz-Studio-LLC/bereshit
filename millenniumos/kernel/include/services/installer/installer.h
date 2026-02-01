// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-installer-header
// Title:       MillenniumOS Installer Subsystem Header
// Type:        Header
// Component:   Kernel/Installer
// Role:        First-boot detection and system installation
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Genesis 1:9 — "Let the waters... be gathered... and let the
//              dry land appear." Establishing the foundation for habitation.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MOS_INSTALLER_H
#define MOS_INSTALLER_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"

// In freestanding kernel mode (CPISI_MODE 0-1), installer service is not available.
// Stubs are provided in kernel.h instead.
#if CPISI_MODE >= 2

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Installation State
// -----------------------------------------------------------------------------

typedef enum {
    MOS_INSTALL_UNKNOWN     = 0,    // Not yet checked
    MOS_INSTALL_FRESH       = 1,    // Fresh system, needs installation
    MOS_INSTALL_COMPLETE    = 2,    // System installed, boot normally
    MOS_INSTALL_REPAIR      = 3,    // Repair mode requested
    MOS_INSTALL_UPGRADE     = 4,    // Upgrade from previous version
} MosInstallState;

typedef enum {
    MOS_INSTALL_STEP_WELCOME    = 0,
    MOS_INSTALL_STEP_LICENSE    = 1,
    MOS_INSTALL_STEP_DISK       = 2,
    MOS_INSTALL_STEP_PARTITION  = 3,
    MOS_INSTALL_STEP_COPY       = 4,
    MOS_INSTALL_STEP_USER       = 5,
    MOS_INSTALL_STEP_COMPLETE   = 6,
} MosInstallStep;

// -----------------------------------------------------------------------------
// B.2 Disk Information
// -----------------------------------------------------------------------------

#define MOS_MAX_DISKS       8
#define MOS_MAX_PARTITIONS  16
#define MOS_DISK_NAME_LEN   32

typedef enum {
    MOS_DISK_TYPE_UNKNOWN   = 0,
    MOS_DISK_TYPE_ATA       = 1,    // ATA/SATA hard disk
    MOS_DISK_TYPE_NVME      = 2,    // NVMe SSD
    MOS_DISK_TYPE_USB       = 3,    // USB storage
    MOS_DISK_TYPE_CDROM     = 4,    // CD/DVD drive
    MOS_DISK_TYPE_FLOPPY    = 5,    // Floppy (legacy)
} MosDiskType;

typedef struct {
    uint8_t     id;                         // Disk index (0-7)
    MosDiskType type;                       // Disk type
    char        name[MOS_DISK_NAME_LEN];    // Human-readable name
    uint64_t    size_bytes;                 // Total size in bytes
    uint64_t    size_sectors;               // Total sectors
    uint16_t    sector_size;                // Bytes per sector (usually 512)
    mos_bool_t  bootable;                   // Has bootable partition
    mos_bool_t  removable;                  // Is removable media
    uint8_t     partition_count;            // Number of partitions
} MosDiskInfo;

typedef struct {
    uint8_t     disk_id;                    // Parent disk
    uint8_t     partition_id;               // Partition index
    uint64_t    start_sector;               // Start LBA
    uint64_t    sector_count;               // Partition size in sectors
    uint8_t     type;                       // Partition type code
    mos_bool_t  bootable;                   // Is bootable flag set
    mos_bool_t  active;                     // Is active partition
    char        label[MOS_DISK_NAME_LEN];   // Partition label if any
} MosPartitionInfo;

// -----------------------------------------------------------------------------
// B.3 Installer Context
// -----------------------------------------------------------------------------

typedef struct {
    MosInstallState     state;              // Current installation state
    MosInstallStep      step;               // Current installer step

    // Detected hardware
    uint8_t             disk_count;         // Number of detected disks
    MosDiskInfo         disks[MOS_MAX_DISKS];

    // User selections
    uint8_t             target_disk;        // Selected installation disk
    uint8_t             target_partition;   // Selected partition (or 0 for whole disk)

    // User account
    char                username[32];       // First user's username
    char                fullname[64];       // Full name
    char                hostname[32];       // System hostname

    // Installation progress
    uint8_t             progress_percent;   // 0-100
    const char*         progress_message;   // Current operation description

    // Error state
    mos_result_t        last_error;         // Last error code
    const char*         error_message;      // Human-readable error
} MosInstallerContext;

// -----------------------------------------------------------------------------
// B.4 Public API
// -----------------------------------------------------------------------------

/**
 * Check if system needs installation.
 * Reads boot device to determine if this is a fresh install.
 *
 * @return MOS_INSTALL_FRESH if installation needed,
 *         MOS_INSTALL_COMPLETE if system is installed
 */
MosInstallState mos_installer_check_state(void);

/**
 * Initialize the installer subsystem.
 * Detects available disks and prepares installer context.
 *
 * @return MOS_OK on success
 */
mos_result_t mos_installer_init(void);

/**
 * Get the current installer context.
 *
 * @return Pointer to installer context (read-only)
 */
const MosInstallerContext* mos_installer_get_context(void);

/**
 * Detect available disks and update context.
 *
 * @return Number of disks detected
 */
uint8_t mos_installer_detect_disks(void);

/**
 * Run the installer wizard.
 * This takes over the UI and guides user through installation.
 *
 * @return MOS_OK if installation completed successfully
 */
mos_result_t mos_installer_run(void);

/**
 * Advance to next installation step.
 *
 * @return MOS_OK if step advanced, error if validation failed
 */
mos_result_t mos_installer_next_step(void);

/**
 * Go back to previous installation step.
 *
 * @return MOS_OK if step went back
 */
mos_result_t mos_installer_prev_step(void);

/**
 * Select target disk for installation.
 *
 * @param disk_id   Disk index to select
 * @return MOS_OK if disk is valid and selected
 */
mos_result_t mos_installer_select_disk(uint8_t disk_id);

/**
 * Set user account information.
 *
 * @param username  Username (login name)
 * @param fullname  Full name (display name)
 * @param hostname  System hostname
 * @return MOS_OK if information is valid
 */
mos_result_t mos_installer_set_user(
    const char* username,
    const char* fullname,
    const char* hostname
);

/**
 * Begin the actual installation process.
 * Formats disk, copies files, creates user, installs bootloader.
 *
 * @return MOS_OK if installation completed successfully
 */
mos_result_t mos_installer_execute(void);

/**
 * Mark installation as complete.
 * Creates marker file so system boots normally next time.
 *
 * @return MOS_OK on success
 */
mos_result_t mos_installer_finalize(void);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// "And God said, Let the waters under the heaven be gathered together unto
// one place, and let the dry land appear: and it was so."
// — Genesis 1:9
//
// The installer gathers the elements and establishes the foundation
// upon which the Kingdom Technology operating system will run.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif /* CPISI_MODE >= 2 - Full installer service */

#endif // MOS_INSTALLER_H
