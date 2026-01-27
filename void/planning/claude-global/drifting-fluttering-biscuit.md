# Doxygen Documentation Polish Plan

## Summary

Polish CPI-SI types headers for professional Doxygen output. Structure is aligned from audit - now add the finishing touches.

## Scope

**Files to polish:**
- `types.h` (foundation - largest, most enums)
- `config/config.h`
- `config/error.h`
- `domain/core/*.h` (layer, flags, timing, log)
- `domain/composite/*.h` (metadata, phase, health)

**Location:** `cornerstone/engine/core/include/cpisi/types/`

---

## Tasks

### Phase 1: Enum Member Documentation (HIGH)

**Problem:** Enum members use `/* value */` C-comments — Doxygen ignores them.

**Fix pattern:**
```c
// BEFORE
typedef enum CpisiSign {
    CPISI_SIGN_NEG = CPISI_NEGATIVE,  /* -1 */
    CPISI_SIGN_ZERO = CPISI_NEUTRAL,  /*  0 */
} CpisiSign;

// AFTER
typedef enum CpisiSign {
    CPISI_SIGN_NEG = CPISI_NEGATIVE,  /**< -1: retreat toward foundation */
    CPISI_SIGN_ZERO = CPISI_NEUTRAL,  /**< 0: hold position (center) */
} CpisiSign;
```

**Files with enums (grep for `typedef enum`):**
1. types.h - CpisiSign, CpisiLayer, CpisiSizeTier, CpisiPhase, etc.
2. error.h - CpisiErrorCode (massive - 256 values)
3. health.h - CpisiHealthState, etc.
4. phase.h - CpisiPhaseId, etc.
5. log.h - CpisiLogLevel, etc.

**Approach:** Process file by file, enum by enum.

---

### Phase 2: Struct Field Documentation (HIGH)

**Problem:** Struct fields lack per-field documentation.

**Fix pattern:**
```c
// BEFORE
typedef struct {
    int poll_ms;
    int retry_max;
} HealthConfig;

// AFTER
typedef struct {
    int poll_ms;      /**< Polling interval in milliseconds */
    int retry_max;    /**< Maximum retry attempts before escalation */
} HealthConfig;
```

**Target structs:** Grep for `typedef struct` in each file.

---

### Phase 3: Doxyfile Configuration Tuning (MEDIUM)

Add to Doxyfile:
```
# Sorting for better navigation
SORT_BRIEF_DOCS        = YES
SORT_MEMBERS_CTORS_1ST = YES
SORT_GROUP_NAMES       = YES

# Main page (optional)
USE_MDFILE_AS_MAINPAGE = README.md

# After polish, enable to catch gaps
WARN_IF_UNDOCUMENTED   = YES
WARN_NO_PARAMDOC       = YES
```

---

## Execution Order

1. **types.h enums** (foundation - sets the pattern)
2. **error.h enums** (CpisiErrorCode is large, but systematic)
3. **Domain files enums** (smaller, follow established pattern)
4. **Struct fields** (all files)
5. **Doxyfile tuning** (after content is polished)
6. **Regenerate and verify**

---

## Verification

1. Run `doxygen Doxyfile`
2. Open `docs/api/html/index.html`
3. Navigate to Modules → verify enum values render with descriptions
4. Check struct pages for field documentation
5. No "undocumented" warnings in output

---

## Critical Files

```
cornerstone/engine/core/
├── Doxyfile                              ← Configuration
└── include/cpisi/types/
    ├── types.h                           ← Foundation (start here)
    ├── config/
    │   ├── config.h
    │   └── error.h                       ← Large enum
    └── domain/
        ├── core/
        │   ├── layer.h
        │   ├── flags.h
        │   ├── timing.h
        │   └── log.h
        └── composite/
            ├── metadata.h
            ├── phase.h
            └── health.h
```

---

## Estimate

- Enum polish: ~2-3 hours (error.h alone is large)
- Struct polish: ~1-2 hours
- Doxyfile tuning: ~30 min
- Verification: ~30 min

**Total: ~4-6 hours** (can be done incrementally)
