// #!omni code --c -executable
// #!omni meta.key = B-word-work-pkg-fuse-bereshit-fs
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// bereshit_fs.c — FUSE Filesystem for Bereshit
// Mounts void/word/tov as semantic zones

// Enable POSIX features for lstat, pread, pwrite, readlink, etc.
#define _GNU_SOURCE

#define FUSE_USE_VERSION 31

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-fuse-bereshit-fs
// Title: Bereshit FUSE Filesystem
// Type: Executable
// Role: Mount Bereshit structure as FUSE filesystem with zone semantics

// Grounding: Genesis 1:1 - "In the beginning God created"
//            Creation is structured - void → word → tov

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <fuse3/fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "zone.h"

// Mount options
struct bereshit_options {
    const char* source;      // Source Bereshit directory
    int show_help;
    int show_version;
};

static struct bereshit_options options;

// FUSE option specification
#define OPTION(t, p) { t, offsetof(struct bereshit_options, p), 1 }

static const struct fuse_opt option_spec[] = {
    OPTION("--source=%s", source),
    OPTION("-s %s", source),
    OPTION("-h", show_help),
    OPTION("--help", show_help),
    OPTION("-V", show_version),
    OPTION("--version", show_version),
    FUSE_OPT_END
};

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// Helper Functions
// -----------------------------------------------------------------------------

// Build full source path from FUSE path
static void build_source_path(char* dest, size_t size, const char* path) {
    const char* root = zone_get_bereshit_root();
    if (path[0] == '/' && path[1] == '\0') {
        // Root path
        snprintf(dest, size, "%s", root);
    } else {
        snprintf(dest, size, "%s%s", root, path);
    }
}

// Get file attributes
static int bereshit_getattr(const char* path, struct stat* stbuf,
                            struct fuse_file_info* fi) {
    (void) fi;

    char source_path[4096];
    build_source_path(source_path, sizeof(source_path), path);

    int res = lstat(source_path, stbuf);
    if (res == -1) {
        return -errno;
    }

    // Add zone info to extended attributes later
    BereshitZone zone = zone_from_path(path);
    (void) zone;  // For now, just passthrough

    return 0;
}

// Read directory
static int bereshit_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
                            off_t offset, struct fuse_file_info* fi,
                            enum fuse_readdir_flags flags) {
    (void) offset;
    (void) fi;
    (void) flags;

    char source_path[4096];
    build_source_path(source_path, sizeof(source_path), path);

    DIR* dp = opendir(source_path);
    if (dp == NULL) {
        return -errno;
    }

    struct dirent* de;
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if (filler(buf, de->d_name, &st, 0, 0)) {
            break;
        }
    }

    closedir(dp);
    return 0;
}

// Open file
static int bereshit_open(const char* path, struct fuse_file_info* fi) {
    char source_path[4096];
    build_source_path(source_path, sizeof(source_path), path);

    int res = open(source_path, fi->flags);
    if (res == -1) {
        return -errno;
    }

    fi->fh = res;
    return 0;
}

// Read file
static int bereshit_read(const char* path, char* buf, size_t size, off_t offset,
                         struct fuse_file_info* fi) {
    (void) path;

    int res = pread(fi->fh, buf, size, offset);
    if (res == -1) {
        return -errno;
    }

    return res;
}

// Write file
static int bereshit_write(const char* path, const char* buf, size_t size,
                          off_t offset, struct fuse_file_info* fi) {
    // Check zone policy
    int policy = zone_check_policy(path, ZONE_OP_WRITE);
    if (policy != 0) {
        zone_health_failure(path);
        return policy;
    }

    int res = pwrite(fi->fh, buf, size, offset);
    if (res == -1) {
        zone_health_failure(path);
        return -errno;
    }

    zone_health_success(path);
    return res;
}

// Release (close) file
static int bereshit_release(const char* path, struct fuse_file_info* fi) {
    (void) path;
    close(fi->fh);
    return 0;
}

// Create file
static int bereshit_create(const char* path, mode_t mode,
                           struct fuse_file_info* fi) {
    // Check zone policy
    int policy = zone_check_policy(path, ZONE_OP_CREATE);
    if (policy != 0) {
        zone_health_failure(path);
        return policy;
    }

    char source_path[4096];
    build_source_path(source_path, sizeof(source_path), path);

    int res = open(source_path, fi->flags, mode);
    if (res == -1) {
        zone_health_failure(path);
        return -errno;
    }

    zone_health_success(path);
    fi->fh = res;
    return 0;
}

// Unlink (delete) file
static int bereshit_unlink(const char* path) {
    // Check zone policy
    int policy = zone_check_policy(path, ZONE_OP_DELETE);
    if (policy != 0) {
        return policy;
    }

    char source_path[4096];
    build_source_path(source_path, sizeof(source_path), path);

    int res = unlink(source_path);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

// Make directory
static int bereshit_mkdir(const char* path, mode_t mode) {
    // Check zone policy
    int policy = zone_check_policy(path, ZONE_OP_MKDIR);
    if (policy != 0) {
        zone_health_failure(path);
        return policy;
    }

    char source_path[4096];
    build_source_path(source_path, sizeof(source_path), path);

    int res = mkdir(source_path, mode);
    if (res == -1) {
        zone_health_failure(path);
        return -errno;
    }

    zone_health_success(path);
    return 0;
}

// Remove directory
static int bereshit_rmdir(const char* path) {
    // Check zone policy
    int policy = zone_check_policy(path, ZONE_OP_RMDIR);
    if (policy != 0) {
        return policy;
    }

    char source_path[4096];
    build_source_path(source_path, sizeof(source_path), path);

    int res = rmdir(source_path);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

// Rename file/directory
static int bereshit_rename(const char* from, const char* to, unsigned int flags) {
    if (flags) {
        return -EINVAL;
    }

    char source_from[4096];
    char source_to[4096];
    build_source_path(source_from, sizeof(source_from), from);
    build_source_path(source_to, sizeof(source_to), to);

    int res = rename(source_from, source_to);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

// Truncate file
static int bereshit_truncate(const char* path, off_t size,
                             struct fuse_file_info* fi) {
    int res;

    if (fi != NULL) {
        res = ftruncate(fi->fh, size);
    } else {
        char source_path[4096];
        build_source_path(source_path, sizeof(source_path), path);
        res = truncate(source_path, size);
    }

    if (res == -1) {
        return -errno;
    }

    return 0;
}

// Change permissions
static int bereshit_chmod(const char* path, mode_t mode,
                          struct fuse_file_info* fi) {
    (void) fi;

    char source_path[4096];
    build_source_path(source_path, sizeof(source_path), path);

    int res = chmod(source_path, mode);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

// Change ownership
static int bereshit_chown(const char* path, uid_t uid, gid_t gid,
                          struct fuse_file_info* fi) {
    (void) fi;

    char source_path[4096];
    build_source_path(source_path, sizeof(source_path), path);

    int res = lchown(source_path, uid, gid);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

// Update timestamps
static int bereshit_utimens(const char* path, const struct timespec ts[2],
                            struct fuse_file_info* fi) {
    (void) fi;

    char source_path[4096];
    build_source_path(source_path, sizeof(source_path), path);

    int res = utimensat(0, source_path, ts, AT_SYMLINK_NOFOLLOW);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

// Read symlink
static int bereshit_readlink(const char* path, char* buf, size_t size) {
    char source_path[4096];
    build_source_path(source_path, sizeof(source_path), path);

    int res = readlink(source_path, buf, size - 1);
    if (res == -1) {
        return -errno;
    }

    buf[res] = '\0';
    return 0;
}

// Create symlink
static int bereshit_symlink(const char* from, const char* to) {
    char source_to[4096];
    build_source_path(source_to, sizeof(source_to), to);

    int res = symlink(from, source_to);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

// Create hard link
static int bereshit_link(const char* from, const char* to) {
    char source_from[4096];
    char source_to[4096];
    build_source_path(source_from, sizeof(source_from), from);
    build_source_path(source_to, sizeof(source_to), to);

    int res = link(source_from, source_to);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

// Sync file
static int bereshit_fsync(const char* path, int isdatasync,
                          struct fuse_file_info* fi) {
    (void) path;
    (void) isdatasync;

    int res = fsync(fi->fh);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

// Get filesystem statistics
static int bereshit_statfs(const char* path, struct statvfs* stbuf) {
    char source_path[4096];
    build_source_path(source_path, sizeof(source_path), path);

    int res = statvfs(source_path, stbuf);
    if (res == -1) {
        return -errno;
    }

    return 0;
}

// Initialize filesystem
static void* bereshit_init(struct fuse_conn_info* conn,
                           struct fuse_config* cfg) {
    (void) conn;
    cfg->kernel_cache = 1;
    cfg->use_ino = 1;

    // Log zone info at mount time
    fprintf(stderr, "Bereshit FUSE mounted\n");
    fprintf(stderr, "  Source: %s\n", zone_get_bereshit_root());
    fprintf(stderr, "  Zone Policies:\n");
    fprintf(stderr, "    void (-1): %s\n", zone_policy_description(ZONE_VOID));
    fprintf(stderr, "    word (0):  %s\n", zone_policy_description(ZONE_WORD));
    fprintf(stderr, "    tov (+1):  %s\n", zone_policy_description(ZONE_TOV));

    return NULL;
}

// -----------------------------------------------------------------------------
// FUSE Operations Table
// -----------------------------------------------------------------------------

static const struct fuse_operations bereshit_oper = {
    .init       = bereshit_init,
    .getattr    = bereshit_getattr,
    .readlink   = bereshit_readlink,
    .readdir    = bereshit_readdir,
    .mkdir      = bereshit_mkdir,
    .symlink    = bereshit_symlink,
    .unlink     = bereshit_unlink,
    .rmdir      = bereshit_rmdir,
    .rename     = bereshit_rename,
    .link       = bereshit_link,
    .chmod      = bereshit_chmod,
    .chown      = bereshit_chown,
    .truncate   = bereshit_truncate,
    .open       = bereshit_open,
    .read       = bereshit_read,
    .write      = bereshit_write,
    .statfs     = bereshit_statfs,
    .release    = bereshit_release,
    .fsync      = bereshit_fsync,
    .create     = bereshit_create,
    .utimens    = bereshit_utimens,
};

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

static void show_help(const char* progname) {
    fprintf(stderr, "usage: %s [options] <mountpoint>\n\n", progname);
    fprintf(stderr, "Bereshit FUSE Filesystem - Mount void/word/tov zones\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    --source=DIR       Source Bereshit directory (required)\n");
    fprintf(stderr, "    -s DIR             Short form of --source\n");
    fprintf(stderr, "    -h, --help         Show this help\n");
    fprintf(stderr, "    -V, --version      Show version\n\n");
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "    %s --source=/path/to/Bereshit /mnt/bereshit\n\n", progname);
    fprintf(stderr, "Zone Semantics:\n");
    fprintf(stderr, "    void (-1)  Entry gate - formless, planning, imports\n");
    fprintf(stderr, "    word (0)   Processing - definitions, implementations\n");
    fprintf(stderr, "    tov (+1)   Exit gate - completion, demos, licenses\n");
}

int main(int argc, char* argv[]) {
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    // Set defaults
    options.source = NULL;
    options.show_help = 0;
    options.show_version = 0;

    // Parse options
    if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1) {
        return 1;
    }

    if (options.show_help) {
        show_help(argv[0]);
        fuse_opt_add_arg(&args, "--help");
        args.argv[0][0] = '\0';
    }

    if (options.show_version) {
        fprintf(stderr, "Bereshit FUSE v0.1.0\n");
        fprintf(stderr, "FUSE library version %d.%d\n",
                FUSE_MAJOR_VERSION, FUSE_MINOR_VERSION);
        return 0;
    }

    // Require source directory
    if (!options.source) {
        fprintf(stderr, "Error: --source is required\n");
        fprintf(stderr, "Use --help for usage information\n");
        return 1;
    }

    // Validate source directory
    struct stat st;
    if (stat(options.source, &st) == -1 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Error: Source directory does not exist: %s\n",
                options.source);
        return 1;
    }

    // Set global Bereshit root
    zone_set_bereshit_root(options.source);

    // Run FUSE
    int ret = fuse_main(args.argc, args.argv, &bereshit_oper, NULL);

    fuse_opt_free_args(&args);
    return ret;
}

// Closing: "In the beginning God created the heaven and the earth."
//          — Genesis 1:1

// =============================================================================
// END CLOSING
// =============================================================================
