# Directory Ordering Design Note

**Key:** L2-filesystem-ordering
**Status:** Design
**Version:** a-00.10
**Created:** 2026-02-01
**Authors:** Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)

---

> *"And God saw every thing that he had made, and, behold, it was very good."* — Genesis 1:31

---

## Problem

Standard filesystems return directory entries in arbitrary or alphabetical order. This forces naming hacks (prefixes like `0-`, `a-`, `01-`) to achieve semantic ordering.

**Example:** Genesis structure should flow `void → word → tov` but alphabetically sorts as `tov → void → word`.

Current workaround: `a-void/`, `b-word/`, `c-tov/` — functional but not ideal.

---

## Solution: `.omni-order` Specification

### File Location

```
directory/
├── .omni-order       # Ordering specification
├── child-a/
├── child-b/
└── child-c/
```

Alternative: Store in `.omni/` folder metadata if present.

### Format (TOML)

```toml
# .omni-order - Directory ordering specification
# Part of OmniCode filesystem semantics

[order]
# Explicit ordering - children listed in display order
children = [
    "a-void",
    "b-word",
    "c-tov"
]

# Ordering mode for unlisted children
# explicit = only show listed (hide others)
# alphabetic = listed first, then alphabetic
# modified = listed first, then by modification time
# created = listed first, then by creation time
fallback = "alphabetic"

[metadata]
reason = "Genesis flow: formless → definition → completion"
updated = 2026-02-01
```

### Ordering Modes

| Mode | Behavior |
|------|----------|
| `explicit` | Only return listed children, in order |
| `alphabetic` | Listed children first (in order), then remaining alphabetically |
| `modified` | Listed children first, then remaining by mtime |
| `created` | Listed children first, then remaining by ctime |

---

## FUSE Implementation

### Location

`b-word/L2-platform/fuse/src/readdir.c`

### Pseudocode

```c
int bereshit_readdir(const char *path, void *buf,
                     fuse_fill_dir_t filler, off_t offset,
                     struct fuse_file_info *fi) {

    // 1. Check for .omni-order in directory
    char order_path[PATH_MAX];
    snprintf(order_path, PATH_MAX, "%s/.omni-order", path);

    OmniOrder *order = omni_order_load(order_path);

    if (order) {
        // 2. Return entries in specified order
        for (int i = 0; i < order->count; i++) {
            filler(buf, order->children[i], NULL, 0);
        }

        // 3. Handle fallback for unlisted entries
        if (order->fallback != OMNI_ORDER_EXPLICIT) {
            // Add remaining entries per fallback mode
            add_remaining_entries(path, buf, filler, order);
        }

        omni_order_free(order);
    } else {
        // 4. No order file - default alphabetic
        return default_readdir(path, buf, filler, offset, fi);
    }

    return 0;
}
```

---

## Integration Points

| Layer | Integration |
|-------|-------------|
| **L0-universal** | `libtrit/toml.c` parses .omni-order files |
| **L1-omnicode** | `.omni/` folder metadata can include ordering |
| **L2-platform** | FUSE `readdir()` respects ordering |
| **L3-cpisi** | Skills can set semantic ordering |

---

## Benefits

1. **Semantic ordering** — Directories reflect meaning, not alphabet
2. **No naming hacks** — Keep clean names (`void/` not `a-void/`)
3. **Filesystem-native** — Works with any tool using our FUSE layer
4. **Backward compatible** — Missing `.omni-order` = standard behavior

---

## Migration Path

| Phase | Action |
|-------|--------|
| **Now** | Use prefix naming (`a-void/`, `b-word/`, `c-tov/`) |
| **L2 FUSE** | Implement `.omni-order` support |
| **Post-L2** | Remove prefixes, rely on `.omni-order` |

---

## Open Questions

1. Should `.omni-order` be hidden (`.omni-order`) or visible (`omni-order`)?
2. Store in separate file or within `.omni/` folder metadata?
3. Support recursive ordering (order applies to subdirectories)?

---

**Next Steps:** Implement when L2-platform/fuse development begins.
