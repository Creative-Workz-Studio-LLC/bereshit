// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-vfs
// Title:       MillenniumOS Virtual Filesystem
// Type:        Header
// Component:   Stage 1 (Kernel) / VFS Subsystem
// Role:        Filesystem abstraction using ternary semantics
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Genesis 1:9 — "Let the waters be gathered together"
//              Files gathered into organized structure.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MILLENNIUMOS_VFS_H
#define MILLENNIUMOS_VFS_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 VFS Constants (Ternary-Organized)
// -----------------------------------------------------------------------------
//
// File descriptors use trit9 (19,683 values):
//   Negative range: System/special files
//   Zero: Null descriptor
//   Positive range: User file descriptors
//

#define MOS_MAX_FDS             243     // 3^5 file descriptors per process
#define MOS_MAX_MOUNTS          27      // 3^3 mount points
// MOS_MAX_PATH and MOS_MAX_NAME defined in types.h

// File types (ternary semantic)
#define MOS_FILE_REGULAR        TRIT_ZERO   // 0: Regular file
#define MOS_FILE_DIRECTORY      TRIT_POS    // +1: Directory
#define MOS_FILE_SPECIAL        TRIT_NEG    // -1: Special/device

// Extended file types using trit5
#define MOS_FTYPE_UNKNOWN       ((trit5_t)0)
#define MOS_FTYPE_REGULAR       ((trit5_t)40)
#define MOS_FTYPE_DIRECTORY     ((trit5_t)80)
#define MOS_FTYPE_SYMLINK       ((trit5_t)120)
#define MOS_FTYPE_CHARDEV       ((trit5_t)160)
#define MOS_FTYPE_BLOCKDEV      ((trit5_t)200)
#define MOS_FTYPE_PIPE          ((trit5_t)240)
#define MOS_FTYPE_SOCKET        ((trit5_t)241)
#define MOS_FTYPE_INVALID       MOS_SPARE_INVALID   // 243

// File flags (using trit5 for permissions)
#define MOS_O_READ              ((trit5_t)1)        // Read permission
#define MOS_O_WRITE             ((trit5_t)3)        // Write permission
#define MOS_O_EXEC              ((trit5_t)9)        // Execute permission
#define MOS_O_CREATE            ((trit5_t)27)       // Create if not exists
#define MOS_O_TRUNC             ((trit5_t)81)       // Truncate on open
#define MOS_O_APPEND            ((trit5_t)162)      // Append mode

// Seek modes (ternary)
#define MOS_SEEK_SET            TRIT_NEG    // -1: From beginning
#define MOS_SEEK_CUR            TRIT_ZERO   // 0: From current
#define MOS_SEEK_END            TRIT_POS    // +1: From end

// -----------------------------------------------------------------------------
// B.2 VFS Structures
// -----------------------------------------------------------------------------

// Forward declarations
struct MosInode;
struct MosDentry;
struct MosFile;
struct MosSuperblock;
struct MosFilesystemOps;

// Inode - represents a file on disk
typedef struct MosInode {
    mos_ino_t           ino;            // Inode number (trit27)
    trit5_t             type;           // File type
    trit5_t             mode;           // Permissions (ternary)
    MosZone             zone;           // Security zone
    mos_uid_t           uid;            // Owner user ID
    mos_gid_t           gid;            // Owner group ID
    mos_size_t          size;           // File size
    mos_time_t          atime;          // Access time
    mos_time_t          mtime;          // Modification time
    mos_time_t          ctime;          // Creation time
    trit27_t            links;          // Hard link count
    trit27_t            blocks;         // Allocated blocks
    void*               data;           // Filesystem-specific data
    struct MosSuperblock* sb;           // Owning superblock
} MosInode;

// Directory entry - name to inode mapping
typedef struct MosDentry {
    char                name[MOS_MAX_NAME];
    struct MosInode*    inode;
    struct MosDentry*   parent;
    struct MosDentry*   children;       // First child (for directories)
    struct MosDentry*   next;           // Next sibling
    trit5_t             flags;
    trit27_t            refcount;
} MosDentry;

// Open file descriptor
typedef struct MosFile {
    struct MosDentry*   dentry;         // Associated dentry
    trit5_t             flags;          // Open flags
    mos_off_t           pos;            // Current position
    trit27_t            refcount;       // Reference count
    MosZone             zone;           // Access zone
    void*               private_data;   // Filesystem-specific
} MosFile;

// Superblock - filesystem instance
typedef struct MosSuperblock {
    trit27_t            magic;          // Filesystem magic number
    trit27_t            blocksize;      // Block size
    trit27_t            total_blocks;   // Total blocks
    trit27_t            free_blocks;    // Free blocks
    trit27_t            total_inodes;   // Total inodes
    trit27_t            free_inodes;    // Free inodes
    struct MosDentry*   root;           // Root dentry
    struct MosFilesystemOps* ops;       // Filesystem operations
    void*               fs_data;        // Filesystem-specific data
    MosZone             zone;           // Mount zone
    mos_health_t        health;         // Filesystem health
} MosSuperblock;

// -----------------------------------------------------------------------------
// B.3 Filesystem Operations
// -----------------------------------------------------------------------------

typedef struct MosFilesystemOps {
    // Superblock operations
    mos_result_t (*mount)(MosSuperblock* sb, const char* device);
    mos_result_t (*unmount)(MosSuperblock* sb);
    mos_result_t (*sync)(MosSuperblock* sb);

    // Inode operations
    mos_result_t (*create)(MosInode* dir, const char* name, trit5_t mode);
    mos_result_t (*lookup)(MosInode* dir, const char* name, MosInode** result);
    mos_result_t (*link)(MosDentry* old, MosInode* dir, const char* name);
    mos_result_t (*unlink)(MosInode* dir, const char* name);
    mos_result_t (*mkdir)(MosInode* dir, const char* name, trit5_t mode);
    mos_result_t (*rmdir)(MosInode* dir, const char* name);
    mos_result_t (*rename)(MosInode* old_dir, const char* old_name,
                           MosInode* new_dir, const char* new_name);

    // File operations
    mos_result_t (*open)(MosFile* file);
    mos_result_t (*close)(MosFile* file);
    mos_size_t   (*read)(MosFile* file, void* buf, mos_size_t count);
    mos_size_t   (*write)(MosFile* file, const void* buf, mos_size_t count);
    mos_off_t    (*seek)(MosFile* file, mos_off_t offset, trit_t whence);
    mos_result_t (*truncate)(MosInode* inode, mos_size_t size);
} MosFilesystemOps;

// -----------------------------------------------------------------------------
// B.4 VFS State
// -----------------------------------------------------------------------------

typedef struct {
    // Mount table
    MosSuperblock*      mounts[MOS_MAX_MOUNTS];
    mos_size_t          mount_count;

    // Root filesystem
    MosSuperblock*      root_fs;
    MosDentry*          root_dentry;

    // Statistics
    trit27_t            total_opens;
    trit27_t            total_reads;
    trit27_t            total_writes;
    mos_health_t        health;
} MosVfs;

// -----------------------------------------------------------------------------
// B.5 Initialization
// -----------------------------------------------------------------------------

// Initialize VFS subsystem
mos_result_t mos_vfs_init(void);

// -----------------------------------------------------------------------------
// B.6 Mount Operations
// -----------------------------------------------------------------------------

// Mount filesystem
mos_result_t mos_mount(const char* device, const char* mountpoint,
                       const char* fstype, trit5_t flags);

// Unmount filesystem
mos_result_t mos_umount(const char* mountpoint);

// Get mount info
const MosSuperblock* mos_mount_info(const char* mountpoint);

// -----------------------------------------------------------------------------
// B.7 Path Operations
// -----------------------------------------------------------------------------

// Resolve path to dentry
mos_result_t mos_path_lookup(const char* path, MosDentry** result);

// Get parent directory
MosDentry* mos_path_parent(const char* path);

// Get basename from path
const char* mos_path_basename(const char* path);

// Normalize path (resolve . and ..)
mos_result_t mos_path_normalize(const char* path, char* result, mos_size_t size);

// -----------------------------------------------------------------------------
// B.8 File Operations
// -----------------------------------------------------------------------------

// Open file
mos_result_t mos_open(const char* path, trit5_t flags, MosFile** file);

// Close file
mos_result_t mos_close(MosFile* file);

// Read from file
mos_size_t mos_read(MosFile* file, void* buf, mos_size_t count);

// Write to file
mos_size_t mos_write(MosFile* file, const void* buf, mos_size_t count);

// Seek in file
mos_off_t mos_seek(MosFile* file, mos_off_t offset, trit_t whence);

// -----------------------------------------------------------------------------
// B.9 Directory Operations
// -----------------------------------------------------------------------------

// Create directory
mos_result_t mos_mkdir(const char* path, trit5_t mode);

// Remove directory
mos_result_t mos_rmdir(const char* path);

// Read directory entry
mos_result_t mos_readdir(MosFile* dir, char* name, mos_size_t size);

// -----------------------------------------------------------------------------
// B.10 Inode Operations
// -----------------------------------------------------------------------------

// Get file status
mos_result_t mos_stat(const char* path, MosInode* stat);

// Create hard link
mos_result_t mos_link(const char* old_path, const char* new_path);

// Remove file
mos_result_t mos_unlink(const char* path);

// Rename file
mos_result_t mos_rename(const char* old_path, const char* new_path);

// -----------------------------------------------------------------------------
// B.11 Information
// -----------------------------------------------------------------------------

// Get VFS state
const MosVfs* mos_vfs_info(void);

// Get VFS health
mos_health_t mos_vfs_health(void);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// VFS uses ternary concepts throughout:
//   - File descriptors as trit9 (negative=system, zero=null, positive=user)
//   - File types as trit5 with spare metadata values
//   - Permissions as ternary combinations (read/write/exec)
//   - Seek modes as trit (beginning/current/end)
//   - Zones for security (void/word/tov)
//
// "And God said, Let the waters under the heaven be gathered together
//  unto one place, and let the dry land appear: and it was so."
// — Genesis 1:9
//
// Files are gathered into organized structure, each accessible by path.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // MILLENNIUMOS_VFS_H
