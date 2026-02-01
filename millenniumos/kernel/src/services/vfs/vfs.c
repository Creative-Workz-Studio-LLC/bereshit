// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-vfs-impl
// Title:       MillenniumOS Virtual Filesystem Implementation
// Type:        Source
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

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "services/vfs/vfs.h"
#include "core/memory/memory.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Global VFS State
// -----------------------------------------------------------------------------

static MosVfs g_vfs;

// Root filesystem (in-memory tmpfs-like)
static MosSuperblock g_root_sb;
static MosInode g_root_inode;
static MosDentry g_root_dentry;

// -----------------------------------------------------------------------------
// B.2 String Helpers
// -----------------------------------------------------------------------------

static mos_size_t mos_strlen(const char* s) {
    mos_size_t len = 0;
    while (s[len]) len++;
    return len;
}

static void mos_strcpy(char* dst, const char* src, mos_size_t max) {
    mos_size_t i = 0;
    while (src[i] && i < max - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static int mos_strcmp(const char* a, const char* b) {
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }
    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}

static void mos_memset_vfs(void* ptr, uint8_t val, mos_size_t size) {
    uint8_t* p = (uint8_t*)ptr;
    while (size--) *p++ = val;
}

// -----------------------------------------------------------------------------
// B.3 Root Filesystem Operations (In-Memory)
// -----------------------------------------------------------------------------

static mos_result_t rootfs_mount(MosSuperblock* sb, const char* device) {
    (void)device;  // Not used for rootfs

    sb->magic = 0x4D4F5346;  // "MOSF"
    sb->blocksize = 4096;
    sb->total_blocks = 1024;  // Virtual
    sb->free_blocks = 1024;
    sb->total_inodes = 243;   // 3^5 inodes
    sb->free_inodes = 242;    // Root uses one
    sb->zone = MOS_ZONE_WORD;
    sb->health = MOS_HEALTH_NEUTRAL;

    return MOS_OK;
}

static mos_result_t rootfs_unmount(MosSuperblock* sb) {
    (void)sb;
    // Can't unmount rootfs
    return MOS_ERR_PERM;
}

static mos_result_t rootfs_sync(MosSuperblock* sb) {
    (void)sb;
    // In-memory, nothing to sync
    return MOS_OK;
}

static mos_result_t rootfs_create(MosInode* dir, const char* name, trit5_t mode) {
    if (!dir || !name) return MOS_ERR_INVALID;
    if (dir->type != MOS_FTYPE_DIRECTORY) return MOS_ERR_INVALID;

    // Allocate new inode
    MosInode* inode = (MosInode*)mos_kmalloc(sizeof(MosInode));
    if (!inode) return MOS_ERR_NOMEM;

    // Initialize inode
    mos_memset_vfs(inode, 0, sizeof(MosInode));
    inode->ino = g_root_sb.total_inodes - g_root_sb.free_inodes + 1;
    inode->type = MOS_FTYPE_REGULAR;
    inode->mode = mode;
    inode->zone = MOS_ZONE_WORD;
    inode->uid = 0;
    inode->gid = 0;
    inode->size = 0;
    inode->links = 1;
    inode->sb = &g_root_sb;

    // Allocate dentry
    MosDentry* dentry = (MosDentry*)mos_kmalloc(sizeof(MosDentry));
    if (!dentry) {
        mos_kfree((mos_vaddr_t)inode);
        return MOS_ERR_NOMEM;
    }

    // Initialize dentry
    mos_memset_vfs(dentry, 0, sizeof(MosDentry));
    mos_strcpy(dentry->name, name, MOS_MAX_NAME);
    dentry->inode = inode;

    // Find parent dentry from inode
    MosDentry* parent = (MosDentry*)dir->data;  // Parent dentry stored in inode data
    if (parent) {
        dentry->parent = parent;

        // Add to parent's children list
        if (!parent->children) {
            parent->children = dentry;
        } else {
            MosDentry* last = parent->children;
            while (last->next) last = last->next;
            last->next = dentry;
        }
    }

    dentry->refcount = 1;
    g_root_sb.free_inodes--;

    return MOS_OK;
}

static mos_result_t rootfs_lookup(MosInode* dir, const char* name, MosInode** result) {
    if (!dir || !name || !result) return MOS_ERR_INVALID;
    if (dir->type != MOS_FTYPE_DIRECTORY) return MOS_ERR_INVALID;

    MosDentry* parent = (MosDentry*)dir->data;
    if (!parent) return MOS_ERR_NOTFOUND;

    // Search children
    MosDentry* child = parent->children;
    while (child) {
        if (mos_strcmp(child->name, name) == 0) {
            *result = child->inode;
            return MOS_OK;
        }
        child = child->next;
    }

    return MOS_ERR_NOTFOUND;
}

static mos_result_t rootfs_mkdir(MosInode* dir, const char* name, trit5_t mode) {
    if (!dir || !name) return MOS_ERR_INVALID;
    if (dir->type != MOS_FTYPE_DIRECTORY) return MOS_ERR_INVALID;

    // Allocate new inode
    MosInode* inode = (MosInode*)mos_kmalloc(sizeof(MosInode));
    if (!inode) return MOS_ERR_NOMEM;

    // Initialize as directory
    mos_memset_vfs(inode, 0, sizeof(MosInode));
    inode->ino = g_root_sb.total_inodes - g_root_sb.free_inodes + 1;
    inode->type = MOS_FTYPE_DIRECTORY;
    inode->mode = mode;
    inode->zone = MOS_ZONE_WORD;
    inode->uid = 0;
    inode->gid = 0;
    inode->size = 0;
    inode->links = 2;  // . and parent
    inode->sb = &g_root_sb;

    // Allocate dentry
    MosDentry* dentry = (MosDentry*)mos_kmalloc(sizeof(MosDentry));
    if (!dentry) {
        mos_kfree((mos_vaddr_t)inode);
        return MOS_ERR_NOMEM;
    }

    mos_memset_vfs(dentry, 0, sizeof(MosDentry));
    mos_strcpy(dentry->name, name, MOS_MAX_NAME);
    dentry->inode = inode;
    inode->data = dentry;  // Back-reference for children

    MosDentry* parent = (MosDentry*)dir->data;
    if (parent) {
        dentry->parent = parent;
        if (!parent->children) {
            parent->children = dentry;
        } else {
            MosDentry* last = parent->children;
            while (last->next) last = last->next;
            last->next = dentry;
        }
        parent->inode->links++;  // Parent gains link
    }

    dentry->refcount = 1;
    g_root_sb.free_inodes--;

    return MOS_OK;
}

static mos_result_t rootfs_unlink(MosInode* dir, const char* name) {
    if (!dir || !name) return MOS_ERR_INVALID;

    MosDentry* parent = (MosDentry*)dir->data;
    if (!parent) return MOS_ERR_NOTFOUND;

    // Find and remove from children list
    MosDentry** pp = &parent->children;
    while (*pp) {
        if (mos_strcmp((*pp)->name, name) == 0) {
            MosDentry* found = *pp;
            if (found->inode->type == MOS_FTYPE_DIRECTORY) {
                return MOS_ERR_PERM;  // Use rmdir for directories
            }
            *pp = found->next;

            // Decrement link count
            found->inode->links--;
            if (found->inode->links == 0) {
                mos_kfree((mos_vaddr_t)found->inode);
                g_root_sb.free_inodes++;
            }
            mos_kfree((mos_vaddr_t)found);
            return MOS_OK;
        }
        pp = &(*pp)->next;
    }

    return MOS_ERR_NOTFOUND;
}

static mos_result_t rootfs_rmdir(MosInode* dir, const char* name) {
    if (!dir || !name) return MOS_ERR_INVALID;

    MosDentry* parent = (MosDentry*)dir->data;
    if (!parent) return MOS_ERR_NOTFOUND;

    // Find directory
    MosDentry** pp = &parent->children;
    while (*pp) {
        if (mos_strcmp((*pp)->name, name) == 0) {
            MosDentry* found = *pp;
            if (found->inode->type != MOS_FTYPE_DIRECTORY) {
                return MOS_ERR_INVALID;
            }
            if (found->children) {
                return MOS_ERR_PERM;  // Directory not empty
            }
            *pp = found->next;

            parent->inode->links--;
            mos_kfree((mos_vaddr_t)found->inode);
            mos_kfree((mos_vaddr_t)found);
            g_root_sb.free_inodes++;
            return MOS_OK;
        }
        pp = &(*pp)->next;
    }

    return MOS_ERR_NOTFOUND;
}

static mos_result_t rootfs_link(MosDentry* old, MosInode* dir, const char* name) {
    if (!old || !dir || !name) return MOS_ERR_INVALID;
    if (old->inode->type == MOS_FTYPE_DIRECTORY) return MOS_ERR_INVALID;

    // Create new dentry pointing to same inode
    MosDentry* dentry = (MosDentry*)mos_kmalloc(sizeof(MosDentry));
    if (!dentry) return MOS_ERR_NOMEM;

    mos_memset_vfs(dentry, 0, sizeof(MosDentry));
    mos_strcpy(dentry->name, name, MOS_MAX_NAME);
    dentry->inode = old->inode;
    dentry->inode->links++;

    MosDentry* parent = (MosDentry*)dir->data;
    if (parent) {
        dentry->parent = parent;
        if (!parent->children) {
            parent->children = dentry;
        } else {
            MosDentry* last = parent->children;
            while (last->next) last = last->next;
            last->next = dentry;
        }
    }

    dentry->refcount = 1;
    return MOS_OK;
}

static mos_result_t rootfs_rename(MosInode* old_dir, const char* old_name,
                                   MosInode* new_dir, const char* new_name) {
    MosInode* inode;
    mos_result_t res = rootfs_lookup(old_dir, old_name, &inode);
    if (res != MOS_OK) return res;

    // Create in new location
    if (inode->type == MOS_FTYPE_DIRECTORY) {
        res = rootfs_mkdir(new_dir, new_name, inode->mode);
    } else {
        res = rootfs_create(new_dir, new_name, inode->mode);
    }
    if (res != MOS_OK) return res;

    // Remove from old location
    if (inode->type == MOS_FTYPE_DIRECTORY) {
        return rootfs_rmdir(old_dir, old_name);
    } else {
        return rootfs_unlink(old_dir, old_name);
    }
}

// File operations for rootfs
static mos_result_t rootfs_open(MosFile* file) {
    if (!file) return MOS_ERR_INVALID;
    file->pos = 0;
    return MOS_OK;
}

static mos_result_t rootfs_close(MosFile* file) {
    if (!file) return MOS_ERR_INVALID;
    // Decrease refcount
    if (file->dentry) {
        file->dentry->refcount--;
    }
    return MOS_OK;
}

static mos_size_t rootfs_read(MosFile* file, void* buf, mos_size_t count) {
    if (!file || !buf) return 0;

    MosInode* inode = file->dentry->inode;
    if (!inode->data) return 0;

    // Calculate how much we can read
    mos_size_t avail = ((mos_size_t)file->pos < inode->size) ? (inode->size - (mos_size_t)file->pos) : 0;
    mos_size_t to_read = (count < avail) ? count : avail;

    // Copy data
    uint8_t* src = ((uint8_t*)inode->data) + file->pos;
    uint8_t* dst = (uint8_t*)buf;
    for (mos_size_t i = 0; i < to_read; i++) {
        dst[i] = src[i];
    }

    file->pos += to_read;
    return to_read;
}

static mos_size_t rootfs_write(MosFile* file, const void* buf, mos_size_t count) {
    if (!file || !buf) return 0;

    MosInode* inode = file->dentry->inode;

    // Allocate/grow buffer if needed
    mos_size_t needed = file->pos + count;
    if (needed > inode->size || !inode->data) {
        void* new_data = (void*)mos_kmalloc(needed);
        if (!new_data) return 0;

        if (inode->data) {
            // Copy old data
            uint8_t* old = (uint8_t*)inode->data;
            uint8_t* dst = (uint8_t*)new_data;
            for (mos_size_t i = 0; i < inode->size; i++) {
                dst[i] = old[i];
            }
            mos_kfree((mos_vaddr_t)inode->data);
        }
        inode->data = new_data;
        inode->size = needed;
    }

    // Write data
    const uint8_t* src = (const uint8_t*)buf;
    uint8_t* dst = ((uint8_t*)inode->data) + file->pos;
    for (mos_size_t i = 0; i < count; i++) {
        dst[i] = src[i];
    }

    file->pos += count;
    if ((mos_size_t)file->pos > inode->size) {
        inode->size = file->pos;
    }

    return count;
}

static mos_off_t rootfs_seek(MosFile* file, mos_off_t offset, trit_t whence) {
    if (!file) return -1;

    MosInode* inode = file->dentry->inode;
    int64_t signed_pos;  // Use signed for arithmetic to detect underflow

    switch (whence) {
        case MOS_SEEK_SET:  // From beginning
            signed_pos = (int64_t)offset;
            break;
        case MOS_SEEK_CUR:  // From current
            signed_pos = (int64_t)file->pos + (int64_t)offset;
            break;
        case MOS_SEEK_END:  // From end
            signed_pos = (int64_t)inode->size + (int64_t)offset;
            break;
        default:
            return -1;
    }

    // Clamp to valid range (0 to file size)
    if (signed_pos < 0) signed_pos = 0;
    file->pos = (mos_off_t)signed_pos;
    return file->pos;
}

static mos_result_t rootfs_truncate(MosInode* inode, mos_size_t size) {
    if (!inode) return MOS_ERR_INVALID;

    if (size == 0) {
        if (inode->data) {
            mos_kfree((mos_vaddr_t)inode->data);
            inode->data = NULL;
        }
        inode->size = 0;
    } else if (size < inode->size) {
        inode->size = size;
    }
    // Growing handled by write

    return MOS_OK;
}

// Root filesystem operations table
static MosFilesystemOps g_rootfs_ops = {
    .mount      = rootfs_mount,
    .unmount    = rootfs_unmount,
    .sync       = rootfs_sync,
    .create     = rootfs_create,
    .lookup     = rootfs_lookup,
    .link       = rootfs_link,
    .unlink     = rootfs_unlink,
    .mkdir      = rootfs_mkdir,
    .rmdir      = rootfs_rmdir,
    .rename     = rootfs_rename,
    .open       = rootfs_open,
    .close      = rootfs_close,
    .read       = rootfs_read,
    .write      = rootfs_write,
    .seek       = rootfs_seek,
    .truncate   = rootfs_truncate,
};

// -----------------------------------------------------------------------------
// B.4 VFS Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_vfs_init(void) {
    // Clear global state
    mos_memset_vfs(&g_vfs, 0, sizeof(g_vfs));
    mos_memset_vfs(&g_root_sb, 0, sizeof(g_root_sb));
    mos_memset_vfs(&g_root_inode, 0, sizeof(g_root_inode));
    mos_memset_vfs(&g_root_dentry, 0, sizeof(g_root_dentry));

    // Initialize root inode
    g_root_inode.ino = 1;
    g_root_inode.type = MOS_FTYPE_DIRECTORY;
    g_root_inode.mode = MOS_O_READ | MOS_O_WRITE | MOS_O_EXEC;
    g_root_inode.zone = MOS_ZONE_WORD;
    g_root_inode.links = 2;  // . and /
    g_root_inode.sb = &g_root_sb;
    g_root_inode.data = &g_root_dentry;  // Back-reference

    // Initialize root dentry
    g_root_dentry.name[0] = '/';
    g_root_dentry.name[1] = '\0';
    g_root_dentry.inode = &g_root_inode;
    g_root_dentry.refcount = 1;

    // Initialize root superblock
    g_root_sb.root = &g_root_dentry;
    g_root_sb.ops = &g_rootfs_ops;
    mos_result_t res = g_rootfs_ops.mount(&g_root_sb, NULL);
    if (res != MOS_OK) return res;

    // Register root mount
    g_vfs.root_fs = &g_root_sb;
    g_vfs.root_dentry = &g_root_dentry;
    g_vfs.mounts[0] = &g_root_sb;
    g_vfs.mount_count = 1;
    g_vfs.health = MOS_HEALTH_NEUTRAL;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.5 Path Resolution
// -----------------------------------------------------------------------------

mos_result_t mos_path_lookup(const char* path, MosDentry** result) {
    if (!path || !result) return MOS_ERR_INVALID;
    if (path[0] != '/') return MOS_ERR_INVALID;  // Require absolute paths

    MosDentry* current = g_vfs.root_dentry;

    // Handle root
    if (path[0] == '/' && path[1] == '\0') {
        *result = current;
        return MOS_OK;
    }

    // Walk path components
    const char* p = path + 1;  // Skip leading /
    char component[MOS_MAX_NAME];

    while (*p) {
        // Extract next component
        mos_size_t i = 0;
        while (*p && *p != '/' && i < MOS_MAX_NAME - 1) {
            component[i++] = *p++;
        }
        component[i] = '\0';

        if (i == 0) {
            if (*p == '/') { p++; continue; }  // Skip empty components
            break;
        }

        // Handle . and ..
        if (mos_strcmp(component, ".") == 0) {
            // Stay in current
        } else if (mos_strcmp(component, "..") == 0) {
            if (current->parent) current = current->parent;
        } else {
            // Search in current directory
            MosDentry* child = current->children;
            bool found = false;
            while (child) {
                if (mos_strcmp(child->name, component) == 0) {
                    current = child;
                    found = true;
                    break;
                }
                child = child->next;
            }
            if (!found) return MOS_ERR_NOTFOUND;
        }

        if (*p == '/') p++;
    }

    *result = current;
    return MOS_OK;
}

const char* mos_path_basename(const char* path) {
    if (!path) return NULL;

    const char* last_slash = NULL;
    const char* p = path;
    while (*p) {
        if (*p == '/') last_slash = p;
        p++;
    }

    return last_slash ? (last_slash + 1) : path;
}

MosDentry* mos_path_parent(const char* path) {
    if (!path || path[0] != '/') return NULL;

    // Find last /
    mos_size_t len = mos_strlen(path);
    while (len > 1 && path[len - 1] == '/') len--;

    mos_size_t last_slash = 0;
    for (mos_size_t i = 0; i < len; i++) {
        if (path[i] == '/') last_slash = i;
    }

    // Build parent path
    char parent_path[MOS_MAX_PATH];
    if (last_slash == 0) {
        parent_path[0] = '/';
        parent_path[1] = '\0';
    } else {
        for (mos_size_t i = 0; i < last_slash && i < MOS_MAX_PATH - 1; i++) {
            parent_path[i] = path[i];
        }
        parent_path[last_slash] = '\0';
    }

    MosDentry* result;
    if (mos_path_lookup(parent_path, &result) == MOS_OK) {
        return result;
    }
    return NULL;
}

mos_result_t mos_path_normalize(const char* path, char* result, mos_size_t size) {
    if (!path || !result || size == 0) return MOS_ERR_INVALID;

    // Simple implementation: copy with basic cleanup
    mos_size_t j = 0;
    bool last_was_slash = false;

    for (mos_size_t i = 0; path[i] && j < size - 1; i++) {
        if (path[i] == '/') {
            if (!last_was_slash) {
                result[j++] = '/';
                last_was_slash = true;
            }
        } else {
            result[j++] = path[i];
            last_was_slash = false;
        }
    }

    // Remove trailing slash (except for root)
    if (j > 1 && result[j - 1] == '/') j--;

    result[j] = '\0';
    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.6 File Operations
// -----------------------------------------------------------------------------

mos_result_t mos_open(const char* path, trit5_t flags, MosFile** file) {
    if (!path || !file) return MOS_ERR_INVALID;

    MosDentry* dentry;
    mos_result_t res = mos_path_lookup(path, &dentry);

    if (res != MOS_OK) {
        // File not found - create if requested
        if (!(flags & MOS_O_CREATE)) return res;

        MosDentry* parent = mos_path_parent(path);
        if (!parent) return MOS_ERR_NOTFOUND;

        const char* name = mos_path_basename(path);
        res = g_rootfs_ops.create(parent->inode, name, flags);
        if (res != MOS_OK) return res;

        res = mos_path_lookup(path, &dentry);
        if (res != MOS_OK) return res;
    }

    // Allocate file structure
    MosFile* f = (MosFile*)mos_kmalloc(sizeof(MosFile));
    if (!f) return MOS_ERR_NOMEM;

    mos_memset_vfs(f, 0, sizeof(MosFile));
    f->dentry = dentry;
    f->flags = flags;
    f->pos = 0;
    f->refcount = 1;
    f->zone = MOS_ZONE_WORD;

    dentry->refcount++;

    // Truncate if requested
    if (flags & MOS_O_TRUNC) {
        g_rootfs_ops.truncate(dentry->inode, 0);
    }

    // Position at end if append
    if (flags & MOS_O_APPEND) {
        f->pos = dentry->inode->size;
    }

    if (g_rootfs_ops.open) {
        res = g_rootfs_ops.open(f);
        if (res != MOS_OK) {
            mos_kfree((mos_vaddr_t)f);
            return res;
        }
    }

    g_vfs.total_opens++;
    *file = f;
    return MOS_OK;
}

mos_result_t mos_close(MosFile* file) {
    if (!file) return MOS_ERR_INVALID;

    if (g_rootfs_ops.close) {
        g_rootfs_ops.close(file);
    }

    mos_kfree((mos_vaddr_t)file);
    return MOS_OK;
}

mos_size_t mos_read(MosFile* file, void* buf, mos_size_t count) {
    if (!file || !buf) return 0;
    if (!(file->flags & MOS_O_READ)) return 0;

    mos_size_t result = g_rootfs_ops.read(file, buf, count);
    g_vfs.total_reads++;
    return result;
}

mos_size_t mos_write(MosFile* file, const void* buf, mos_size_t count) {
    if (!file || !buf) return 0;
    if (!(file->flags & MOS_O_WRITE)) return 0;

    mos_size_t result = g_rootfs_ops.write(file, buf, count);
    g_vfs.total_writes++;
    return result;
}

mos_off_t mos_seek(MosFile* file, mos_off_t offset, trit_t whence) {
    if (!file) return -1;
    return g_rootfs_ops.seek(file, offset, whence);
}

// -----------------------------------------------------------------------------
// B.7 Directory Operations
// -----------------------------------------------------------------------------

mos_result_t mos_mkdir(const char* path, trit5_t mode) {
    if (!path) return MOS_ERR_INVALID;

    MosDentry* parent = mos_path_parent(path);
    if (!parent) return MOS_ERR_NOTFOUND;

    const char* name = mos_path_basename(path);
    return g_rootfs_ops.mkdir(parent->inode, name, mode);
}

mos_result_t mos_rmdir(const char* path) {
    if (!path) return MOS_ERR_INVALID;

    MosDentry* parent = mos_path_parent(path);
    if (!parent) return MOS_ERR_NOTFOUND;

    const char* name = mos_path_basename(path);
    return g_rootfs_ops.rmdir(parent->inode, name);
}

mos_result_t mos_readdir(MosFile* dir, char* name, mos_size_t size) {
    if (!dir || !name || size == 0) return MOS_ERR_INVALID;
    if (dir->dentry->inode->type != MOS_FTYPE_DIRECTORY) return MOS_ERR_INVALID;

    // Use pos as index
    MosDentry* child = dir->dentry->children;
    mos_size_t index = 0;

    while (child && index < (mos_size_t)dir->pos) {
        child = child->next;
        index++;
    }

    if (!child) return MOS_ERR_NOTFOUND;

    mos_strcpy(name, child->name, size);
    dir->pos++;
    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.8 Inode Operations
// -----------------------------------------------------------------------------

mos_result_t mos_stat(const char* path, MosInode* stat) {
    if (!path || !stat) return MOS_ERR_INVALID;

    MosDentry* dentry;
    mos_result_t res = mos_path_lookup(path, &dentry);
    if (res != MOS_OK) return res;

    // Copy inode info
    *stat = *(dentry->inode);
    return MOS_OK;
}

mos_result_t mos_link(const char* old_path, const char* new_path) {
    if (!old_path || !new_path) return MOS_ERR_INVALID;

    MosDentry* old_dentry;
    mos_result_t res = mos_path_lookup(old_path, &old_dentry);
    if (res != MOS_OK) return res;

    MosDentry* new_parent = mos_path_parent(new_path);
    if (!new_parent) return MOS_ERR_NOTFOUND;

    const char* new_name = mos_path_basename(new_path);
    return g_rootfs_ops.link(old_dentry, new_parent->inode, new_name);
}

mos_result_t mos_unlink(const char* path) {
    if (!path) return MOS_ERR_INVALID;

    MosDentry* parent = mos_path_parent(path);
    if (!parent) return MOS_ERR_NOTFOUND;

    const char* name = mos_path_basename(path);
    return g_rootfs_ops.unlink(parent->inode, name);
}

mos_result_t mos_rename(const char* old_path, const char* new_path) {
    if (!old_path || !new_path) return MOS_ERR_INVALID;

    MosDentry* old_parent = mos_path_parent(old_path);
    MosDentry* new_parent = mos_path_parent(new_path);
    if (!old_parent || !new_parent) return MOS_ERR_NOTFOUND;

    const char* old_name = mos_path_basename(old_path);
    const char* new_name = mos_path_basename(new_path);

    return g_rootfs_ops.rename(old_parent->inode, old_name,
                               new_parent->inode, new_name);
}

// -----------------------------------------------------------------------------
// B.9 Mount Operations
// -----------------------------------------------------------------------------

mos_result_t mos_mount(const char* device, const char* mountpoint,
                       const char* fstype, trit5_t flags) {
    (void)device;
    (void)mountpoint;
    (void)fstype;
    (void)flags;
    // For now, only root filesystem is supported
    return MOS_ERR_PERM;
}

mos_result_t mos_umount(const char* mountpoint) {
    (void)mountpoint;
    // Can't unmount root
    return MOS_ERR_PERM;
}

const MosSuperblock* mos_mount_info(const char* mountpoint) {
    if (!mountpoint) return NULL;
    if (mountpoint[0] == '/' && mountpoint[1] == '\0') {
        return g_vfs.root_fs;
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// B.10 Information
// -----------------------------------------------------------------------------

const MosVfs* mos_vfs_info(void) {
    return &g_vfs;
}

mos_health_t mos_vfs_health(void) {
    return g_vfs.health;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// VFS provides complete in-memory filesystem:
//   - Root filesystem (tmpfs-like)
//   - Full path resolution with . and ..
//   - File create/read/write/seek
//   - Directory create/remove/list
//   - Hard links
//   - Rename operations
//   - Zone-based security
//
// All operations use ternary semantics where applicable.
//
// "And God called the dry land Earth; and the gathering together
//  of the waters called he Seas: and God saw that it was good."
// — Genesis 1:10
//
// =============================================================================
// END CLOSING
// =============================================================================
