# Plan: Full Makefile Restructure — Config-Driven, Data-Driven, Universal

## Context

The Makefile (1538 lines post-lint-refactor) is the primary editorial build system. It works but has grown organically — config is interleaved with logic, three flat scope targets are copy-paste identical, and sections aren't organized by purpose. The goal: separate WHAT (configuration) from HOW (logic), eliminate duplication, and organize top-to-bottom so the file reads like a system, not an accident.

**Not a behavioral change.** Every target produces identical output. This is structural refactoring.

## Architecture: 10 Sections

```
§1  PROJECT CONFIGURATION    All editable values in one place
§2  FILESYSTEM DISCOVERY     Derived from config + filesystem
§3  CORE ENGINES             Resolution, caching, dependency check
§4  BUILD FUNCTIONS          Shared logic (build_flat_scope)
§5  EDITORIAL TARGETS        book, scope, article, section, pages
§6  VALIDATION & LINT        Structure validation + TS delegation
§7  PUBLISHING               Multi-format via TS builder
§8  ASSET GENERATION         Diagrams, grayscale, palette
§9  DEVELOPMENT              watch, deps, status, clean, remove, setup
§10 HELP                     Self-documentation
```

## What Changes

### 1. Configuration Consolidation (§1)

All project-specific values consolidated at the top. Currently scattered across lines 78-112.

**New additions — flat scope definitions as variables:**
```makefile
SCOPE_A_DIR     = A-frontmatter
SCOPE_A_PREVIEW = A-frontmatter-preview.adoc
SCOPE_A_OUTPUT  = CWS-Frontmatter.pdf

SCOPE_C_DIR     = C-appendices
SCOPE_C_PREVIEW = C-appendices-preview.adoc
SCOPE_C_OUTPUT  = CWS-Appendices.pdf

SCOPE_D_DIR     = D-backmatter
SCOPE_D_PREVIEW = D-backmatter-preview.adoc
SCOPE_D_OUTPUT  = CWS-BackMatter.pdf
```

**New addition — output stem:**
```makefile
OUTPUT_STEM = CWS-Company-Identity-Manual
```

Used in `create` target instead of hardcoded filename.

### 2. Scope Deduplication (§4 + §5)

**The problem:** frontmatter (465-489), appendices (547-571), backmatter (573-597) are 73 lines of identical logic differing only in directory, preview file, output name.

**The fix:** One shared `define build_flat_scope` function (~25 lines), three 1-line callers:

```makefile
define build_flat_scope
	$(check_dep)
	@FAILED=0; \
	for f in $$(ls $(BOOK_DIR)/$(1)/*.adoc 2>/dev/null | sort); do \
	    FNAME=$$(basename "$$f"); \
	    $(MAKE) --no-print-directory section FILE="$$FNAME" FORCE=$(FORCE) 2>&1 || { \
	        echo "  WARN [E34]: Build failed for $$FNAME (continuing batch)"; \
	        FAILED=$$((FAILED + 1)); \
	    }; \
	done; \
	if [ "$$FAILED" -gt 0 ]; then \
	    echo ""; echo "Warning [E34]: $$FAILED file(s) failed in $(1) batch."; \
	fi
	@SRC="$(PREVIEW_DIR)/$(2)"; \
	test -f "$$SRC" || \
	    { echo "Error [E20]: $$SRC not found."; exit 2; }; \
	CACHE_OUT="$(OUTDIR)/$(1)/$(3)"; \
	CACHE_SRCS="$(BOOK_DIR)/$(1)/ $(PREVIEW_DIR)/$(2)"; \
	$(call check_cache); \
	if [ "$$CACHE_HIT" = "1" ]; then exit 0; fi; \
	mkdir -p "$(OUTDIR)/$(1)"; \
	$(ADOC_PDF) "$$SRC" -o "$(OUTDIR)/$(1)/$(3)" 2>&1 || \
	    { rm -f "$(OUTDIR)/$(1)/$(3)"; \
	      echo "Error [E30]: $(1) build failed."; exit 3; }; \
	echo "Built: $(OUTDIR)/$(1)/$(3)"
endef

frontmatter:
	$(call build_flat_scope,$(SCOPE_A_DIR),$(SCOPE_A_PREVIEW),$(SCOPE_A_OUTPUT))

appendices:
	$(call build_flat_scope,$(SCOPE_C_DIR),$(SCOPE_C_PREVIEW),$(SCOPE_C_OUTPUT))

backmatter:
	$(call build_flat_scope,$(SCOPE_D_DIR),$(SCOPE_D_PREVIEW),$(SCOPE_D_OUTPUT))
```

**73 lines → 28 lines. -45 lines.**

### 3. Section Reorganization

Current order is jumbled: assets (250) before editorial targets (380), validation (757) before lint delegation (1076), publishing (1132) between lint and dev tools. New order follows the user's mental model.

### 4. create target uses OUTPUT_STEM

```makefile
# Before:
$(ADOC_PDF) $(BOOK_DIR)/book.adoc -o $(OUTDIR)/CWS-Company-Identity-Manual.pdf

# After:
$(ADOC_PDF) $(BOOK_DIR)/book.adoc -o $(OUTDIR)/$(OUTPUT_STEM).pdf
```

### 5. Help updated with §1-§10 structure reference

## What Does NOT Change

| Component | Lines | Why |
|-----------|-------|-----|
| Resolution engine (resolve_dir, resolve_file) | ~40 | Proven, elegant |
| Cache engine (check_cache) | ~20 | Works for all targets |
| Dependency check (check_dep) | ~5 | Simple, correct |
| `book` target cascade | ~40 | Working orchestration |
| `create` target | ~13 | Master PDF build |
| `content` target | ~6 | Part iteration |
| `part` target | ~47 | Hierarchical build with resolution |
| `article` target | ~59 | Dynamic assembly |
| `section` target | ~23 | Universal single-file build |
| `pages` target | ~39 | qpdf extraction |
| `validate` target | ~277 | Structure validation |
| `lint`, `toc-status`, `toc-parse` | ~20 | TS builder delegation |
| Publishing targets | ~30 | TS builder delegation |
| Asset generation | ~128 | 3-phase pipeline |
| `watch-pdf`, `deps`, `status` | ~80 | Dev tools |
| `clean`, `remove`, `setup` | ~100 | Infrastructure |

**Everything stays functionally identical.** Logic preserved line-for-line.

## Estimated Line Count

| Section | After |
|---------|-------|
| Header + .PHONY | ~70 |
| §1 Config | ~55 |
| §2 Discovery | ~10 |
| §3 Engines | ~90 |
| §4 Functions | ~30 |
| §5 Targets | ~280 |
| §6 Validation | ~300 |
| §7 Publishing | ~40 |
| §8 Assets | ~130 |
| §9 Development | ~200 |
| §10 Help | ~105 |
| **Total** | **~1310** |

Down from 1538. Net savings: ~230 lines (-15%).

## Implementation

Single pass, top to bottom:

1. Write header documentation + .PHONY
2. Write §1 (consolidate variables + add scope definitions + OUTPUT_STEM)
3. Write §2 (copy part discovery)
4. Write §3 (copy engines verbatim — resolve_dir, resolve_file, check_dep, check_cache)
5. Write §4 (create build_flat_scope)
6. Write §5 (all, book, create using OUTPUT_STEM, spine shortcuts, scope targets using $(call), content, part, article, section, pages)
7. Write §6 (validate verbatim, lint/toc delegation verbatim)
8. Write §7 (publishing targets verbatim)
9. Write §8 (assets verbatim)
10. Write §9 (watch-pdf, deps, status, clean, remove, setup verbatim)
11. Write §10 (help with §1-§10 structure + updated lint categories)

## Files Modified

| File | Change |
|------|--------|
| `company-docs/Makefile` | Full restructure — same behavior, new organization |

One file. That's it.

## Verification

```bash
make book                              # Full editorial build
make frontmatter                       # Flat scope via shared function
make appendices                        # Second flat scope
make backmatter                        # Third flat scope
make section FILE=A-COVER              # Single file
make article ARTICLE=01                # Article assembly
make part PART=1                       # Part cascade
make lint CHECK=style                  # TS delegation
make validate TYPE=manual-l            # Structure validation
make markdown                          # Format conversion
make status                            # Dashboard
make deps                              # Dependency check
make help                              # Self-documentation
make convert                           # Error handling (E10)
```

All produce identical output to current Makefile.
