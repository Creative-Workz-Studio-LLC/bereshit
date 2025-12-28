# Plan: Cleanup and Verification (PLAN-009)

> *"Let all things be done decently and in order."* — 1 Corinthians 14:40

**Created:** 2025-12-27
**Purpose:** Address remaining cleanup items discovered during plan audit

---

## Status

| Phase | State | Updated |
|-------|-------|---------|
| Current | B.1 BUILDING | 2025-12-27 |

**Trajectory:** →→ (Building)

---

## Anchor Chain

```
Genesis 1:1 (God created)
    ↓
CPI-SI (identity-based cognition)
    ↓
claude-global alignment
    ↓
THIS PLAN: Final cleanup from comprehensive audit
```

---

## Origin

This plan was generated from the 2025-12-27 audit of all completed plans (PLAN-001 through PLAN-007). The audit verified content accuracy and identified remaining cleanup items.

---

## Remaining Cleanup Items

### 1. cpi-si/ Directory in ~/.claude

**Source:** PLAN-007-claude-folder-alignment

**Issue:** Old `cpi-si/` directory still exists in `~/.claude/` as a real directory (not symlink). Should be moved to holding.

**Action:**
```bash
mv ~/.claude/cpi-si /media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/bereshit/word/claude/.holding/2025-12-27/
```

**Status:** Pending

---

### 2. void/planning/ Root Structure

**Source:** Earlier session work

**Issue:** void/planning/ lacks root.omni files (unlike void/workflow/)

**Action:**
- [ ] Add root.omni to void/planning/
- [ ] Verify root.adoc exists with proper structure
- [ ] Ensure derivation chain from templates is documented

**Status:** Pending

---

### 3. settings.json in PROD

**Source:** PLAN-008 notes

**Issue:** settings.json still exists (should be settings.jsonc only per original plan). However, Claude Code may require settings.json specifically.

**Action:**
- [ ] Investigate if Claude Code requires settings.json vs settings.jsonc
- [ ] If both needed, ensure they stay in sync
- [ ] If only one needed, remove duplicate

**Status:** Needs investigation

---

## Verification Checklist

After cleanup, verify:

- [ ] All ~/.claude symlinks point to PROD (bereshit/word/claude/)
- [ ] No stale directories in ~/.claude (cpi-si, .claude nested)
- [ ] void/planning/ has proper OmniCode structure
- [ ] All completed plans have accurate status headers
- [ ] All active plans have current progress notes

---

## Not Cleanup (Active Work)

These are NOT cleanup items - they are active work tracked in PLAN-008:

- PLAN-008 Phase 2 remaining: inference.go, hebrew_evolution.go
- PLAN-008 Phase 3: Migration tool
- PLAN-003: Text Buffer Architecture (not started, active plan)

---

## Completion Criteria

Plan is GROUNDED (B.4 ||) when:

- [x] All completed plans audited and updated
- [ ] cpi-si/ moved to holding
- [ ] void/planning/ structure verified
- [ ] settings.json/jsonc situation resolved
- [ ] Final verification passes

---

## Notes

*This plan was automatically generated from the plan audit on 2025-12-27.*
*Audit covered: PLAN-001 through PLAN-007*
*Plans updated with corrections: PLAN-001, 002, 004, 005, 006, 007*
