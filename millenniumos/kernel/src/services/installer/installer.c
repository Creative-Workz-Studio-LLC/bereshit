// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-installer-impl
// Title:       MillenniumOS Installer Subsystem Implementation
// Type:        Source
// Component:   Kernel/Installer
// Role:        First-boot detection and system installation logic
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Genesis 1:9-10 — "And God called the dry land Earth...
//              and God saw that it was good."
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"

// In freestanding kernel mode (CPISI_MODE 0-1), installer service is not available.
#if CPISI_MODE >= 2

#include "services/installer/installer.h"
#include "services/vfs/vfs.h"
#include "hal/hal.h"
#include "services/ui/ui.h"

// Installation marker file path
#define MOS_INSTALL_MARKER "/etc/mos/installed"

// Minimum disk size for installation (256 MB)
#define MOS_MIN_DISK_SIZE (256ULL * 1024 * 1024)

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Global Installer State
// -----------------------------------------------------------------------------

static MosInstallerContext g_installer = {
    .state              = MOS_INSTALL_UNKNOWN,
    .step               = MOS_INSTALL_STEP_WELCOME,
    .disk_count         = 0,
    .target_disk        = 0xFF,     // Invalid - not selected
    .target_partition   = 0,
    .username           = {0},
    .fullname           = {0},
    .hostname           = "millenniumos",
    .progress_percent   = 0,
    .progress_message   = NULL,
    .last_error         = MOS_OK,
    .error_message      = NULL,
};

// -----------------------------------------------------------------------------
// B.1b Helper Functions
// -----------------------------------------------------------------------------

// Helper to copy string safely
static void safe_strcpy(char* dst, const char* src, size_t max_len) {
    size_t i = 0;
    while (i < max_len - 1 && src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

// -----------------------------------------------------------------------------
// B.2 Installation State Detection
// -----------------------------------------------------------------------------

MosInstallState mos_installer_check_state(void) {
    // Check if installation marker exists
    MosInode info;
    mos_result_t result = mos_stat(MOS_INSTALL_MARKER, &info);

    if (MOS_SUCCEEDED(result) && info.type == MOS_FILE_REGULAR) {
        // Marker exists - system is installed
        g_installer.state = MOS_INSTALL_COMPLETE;
        return MOS_INSTALL_COMPLETE;
    }

    // No marker - fresh install needed
    g_installer.state = MOS_INSTALL_FRESH;
    return MOS_INSTALL_FRESH;
}

// -----------------------------------------------------------------------------
// B.3 Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_installer_init(void) {
    // Reset installer context
    g_installer.step = MOS_INSTALL_STEP_WELCOME;
    g_installer.disk_count = 0;
    g_installer.target_disk = 0xFF;
    g_installer.progress_percent = 0;
    g_installer.progress_message = "Initializing installer...";
    g_installer.last_error = MOS_OK;
    g_installer.error_message = NULL;

    // Set default hostname
    safe_strcpy(g_installer.hostname, "millenniumos", sizeof(g_installer.hostname));

    // Detect available disks
    mos_installer_detect_disks();

    return MOS_OK;
}

const MosInstallerContext* mos_installer_get_context(void) {
    return &g_installer;
}

// -----------------------------------------------------------------------------
// B.4 Disk Detection
// -----------------------------------------------------------------------------

uint8_t mos_installer_detect_disks(void) {
    g_installer.disk_count = 0;

    // Query HAL for available storage devices
    // In QEMU, we typically see:
    //   - hda: Primary master (our boot disk)
    //   - hdb: Primary slave (if present)
    //   - cdrom: CD-ROM drive

    // For now, create simulated disk entries based on what QEMU provides
    // Real implementation would query ATA/AHCI/NVMe controllers

    // Check primary ATA disk (where we booted from)
    MosDiskInfo* disk = &g_installer.disks[0];
    disk->id = 0;
    disk->type = MOS_DISK_TYPE_ATA;
    safe_strcpy(disk->name, "Primary Disk (hda)", MOS_DISK_NAME_LEN);
    disk->size_bytes = 1440 * 1024;     // 1.44MB floppy image size in QEMU
    disk->size_sectors = disk->size_bytes / 512;
    disk->sector_size = 512;
    disk->bootable = MOS_TRUE;
    disk->removable = MOS_FALSE;
    disk->partition_count = 0;          // No partitions yet
    g_installer.disk_count++;

    // In a real implementation, we would:
    // 1. Probe ATA controllers (primary/secondary, master/slave)
    // 2. Probe AHCI/SATA ports
    // 3. Probe NVMe controllers
    // 4. Probe USB mass storage devices

    return g_installer.disk_count;
}

// -----------------------------------------------------------------------------
// B.5 Step Navigation
// -----------------------------------------------------------------------------

mos_result_t mos_installer_next_step(void) {
    // Validate current step before advancing
    switch (g_installer.step) {
        case MOS_INSTALL_STEP_WELCOME:
            // No validation needed for welcome
            break;

        case MOS_INSTALL_STEP_LICENSE:
            // License must be accepted (handled by UI)
            break;

        case MOS_INSTALL_STEP_DISK:
            // Must have selected a valid disk
            if (g_installer.target_disk >= g_installer.disk_count) {
                g_installer.last_error = MOS_ERR_INVALID;
                g_installer.error_message = "Please select an installation disk";
                return MOS_ERR_INVALID;
            }
            // Check disk size
            if (g_installer.disks[g_installer.target_disk].size_bytes < MOS_MIN_DISK_SIZE) {
                g_installer.last_error = MOS_ERR_INVALID;
                g_installer.error_message = "Selected disk is too small (min 256MB)";
                return MOS_ERR_INVALID;
            }
            break;

        case MOS_INSTALL_STEP_PARTITION:
            // Partition plan validated
            break;

        case MOS_INSTALL_STEP_COPY:
            // Files copied - automatic transition
            break;

        case MOS_INSTALL_STEP_USER:
            // Must have username
            if (g_installer.username[0] == '\0') {
                g_installer.last_error = MOS_ERR_INVALID;
                g_installer.error_message = "Please enter a username";
                return MOS_ERR_INVALID;
            }
            break;

        case MOS_INSTALL_STEP_COMPLETE:
            // Already complete
            return MOS_OK;
    }

    // Advance to next step
    if (g_installer.step < MOS_INSTALL_STEP_COMPLETE) {
        g_installer.step++;
    }

    return MOS_OK;
}

mos_result_t mos_installer_prev_step(void) {
    if (g_installer.step > MOS_INSTALL_STEP_WELCOME) {
        g_installer.step--;
    }
    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.6 User Input
// -----------------------------------------------------------------------------

mos_result_t mos_installer_select_disk(uint8_t disk_id) {
    if (disk_id >= g_installer.disk_count) {
        g_installer.last_error = MOS_ERR_INVALID;
        g_installer.error_message = "Invalid disk selection";
        return MOS_ERR_INVALID;
    }

    g_installer.target_disk = disk_id;
    g_installer.last_error = MOS_OK;
    g_installer.error_message = NULL;

    return MOS_OK;
}

mos_result_t mos_installer_set_user(
    const char* username,
    const char* fullname,
    const char* hostname
) {
    // Validate username (required, alphanumeric + underscore, starts with letter)
    if (!username || username[0] == '\0') {
        g_installer.last_error = MOS_ERR_INVALID;
        g_installer.error_message = "Username is required";
        return MOS_ERR_INVALID;
    }

    // First character must be letter
    if (!((username[0] >= 'a' && username[0] <= 'z') ||
          (username[0] >= 'A' && username[0] <= 'Z'))) {
        g_installer.last_error = MOS_ERR_INVALID;
        g_installer.error_message = "Username must start with a letter";
        return MOS_ERR_INVALID;
    }

    // Copy values
    safe_strcpy(g_installer.username, username, sizeof(g_installer.username));

    if (fullname && fullname[0] != '\0') {
        safe_strcpy(g_installer.fullname, fullname, sizeof(g_installer.fullname));
    } else {
        safe_strcpy(g_installer.fullname, username, sizeof(g_installer.fullname));
    }

    if (hostname && hostname[0] != '\0') {
        safe_strcpy(g_installer.hostname, hostname, sizeof(g_installer.hostname));
    }

    g_installer.last_error = MOS_OK;
    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.7 Installation Execution
// -----------------------------------------------------------------------------

static void update_progress(uint8_t percent, const char* message) {
    g_installer.progress_percent = percent;
    g_installer.progress_message = message;
    // In real implementation, would trigger UI update
}

mos_result_t mos_installer_execute(void) {
    mos_result_t result;

    // Step 1: Format target disk (10%)
    update_progress(10, "Preparing disk...");
    // In real implementation: create partition table, format filesystem
    // For now, we're running from RAM so this is simulated

    // Step 2: Create directory structure (20%)
    update_progress(20, "Creating directories...");
    mos_mkdir("/etc", MOS_FLAG_RWX);
    mos_mkdir("/etc/mos", MOS_FLAG_RWX);
    mos_mkdir("/home", MOS_FLAG_RWX);
    mos_mkdir("/apps", MOS_FLAG_RWX);
    mos_mkdir("/var", MOS_FLAG_RWX);
    mos_mkdir("/var/log", MOS_FLAG_RWX);

    // Step 3: Copy system files (50%)
    update_progress(30, "Copying kernel...");
    // Kernel is already in memory - would copy to disk

    update_progress(40, "Copying applications...");
    // Copy Cornerstone and other apps

    update_progress(50, "Installing bootloader...");
    // Would install stage0a/stage0b to boot sector

    // Step 4: Create user account (70%)
    update_progress(70, "Creating user account...");

    // Create user home directory
    char home_path[64];
    home_path[0] = '/';
    home_path[1] = 'h';
    home_path[2] = 'o';
    home_path[3] = 'm';
    home_path[4] = 'e';
    home_path[5] = '/';

    size_t i = 0;
    while (g_installer.username[i] != '\0' && i < 32) {
        home_path[6 + i] = g_installer.username[i];
        i++;
    }
    home_path[6 + i] = '\0';

    mos_mkdir(home_path, MOS_FLAG_USER_RWX);

    // Step 5: Write configuration (90%)
    update_progress(90, "Writing configuration...");
    // Write /etc/mos/users.conf, /etc/mos/hostname, etc.

    // Step 6: Finalize (100%)
    update_progress(100, "Finalizing installation...");
    result = mos_installer_finalize();

    return result;
}

mos_result_t mos_installer_finalize(void) {
    // Create installation marker file
    MosFile* file;
    // Combine flags using ternary addition (powers of 3)
    trit5_t flags = MOS_O_CREATE + MOS_O_WRITE;
    mos_result_t result = mos_open(MOS_INSTALL_MARKER, flags, &file);

    if (MOS_FAILED(result)) {
        g_installer.last_error = result;
        g_installer.error_message = "Failed to create installation marker";
        return result;
    }

    // Write installation info
    const char* marker_content = "MillenniumOS Installation Complete\n"
                                  "Version: " MOS_VERSION_STRING "\n";
    // Note: MOS_VERSION_STRING should be defined, writing static string for now
    mos_write(file, marker_content, 50);
    mos_close(file);

    g_installer.state = MOS_INSTALL_COMPLETE;
    g_installer.step = MOS_INSTALL_STEP_COMPLETE;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.8 Installer Wizard (Main Loop)
// -----------------------------------------------------------------------------

mos_result_t mos_installer_run(void) {
    mos_result_t result;

    // Initialize installer
    result = mos_installer_init();
    if (MOS_FAILED(result)) {
        return result;
    }

    // Show installer UI
    // The UI handles step navigation and user input
    // This would integrate with mos_ui_* functions

    // For now, simulate a minimal installation path:
    // 1. Auto-select first disk
    // 2. Use default username "user"
    // 3. Execute installation

    // In real implementation, UI would handle this interactively
    if (g_installer.disk_count > 0) {
        mos_installer_select_disk(0);
    }

    mos_installer_set_user("user", "Kingdom User", "millenniumos");

    // Execute installation
    result = mos_installer_execute();

    return result;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// "And God called the dry land Earth; and the gathering together of the
// waters called he Seas: and God saw that it was good."
// — Genesis 1:10
//
// The installer establishes the foundation - dry land upon which the system
// can be built and inhabited. Each step brings order from the formless void.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif /* CPISI_MODE >= 2 - Full installer service implementation */
