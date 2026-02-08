# =============================================================================
# CWS Company Identity Manual — Build System
# =============================================================================
#
# Architecture: 9 sections
#   §1  PROJECT CONFIGURATION    All editable values in one place
#   §2  FILESYSTEM DISCOVERY     Derived from config + filesystem
#   §3  CORE ENGINES             Resolution, caching, dependency check
#   §4  EDITORIAL TARGETS        book, scope, article, section, pages
#   §5  VALIDATION & LINT        Structure, editorial (57 checks), witness system (TS)
#   §6  PUBLISHING               Multi-format via TS builder
#   §7  ASSET GENERATION         Diagrams, grayscale, palette
#   §8  DEVELOPMENT              config, watch, deps, status, clean, remove, setup
#   §9  HELP                     Self-documentation
#
# Four build scopes for editorial review:
#   1. Full book       make book | make create
#   2. Part-level      make part PART=1 | make content | make A | make B | ...
#   3. Article-level   make article ARTICLE=01 | make article ARTICLE=entity-overview
#   4. Section-level   make section FILE=A-COVER | make section FILE=A-frontmatter/A-COVER.adoc
#
# Unified 3-mode resolution on all scoped targets:
#   Path mode    (has /)    Spine-relative path, resolves directly
#   Search mode  (no /)     ID match → name match → fuzzy match
#   Backward compat         Strips book/ prefix; extensions optional
#
# Spine targets (letter-based):
#   make A   Frontmatter       make C   Appendices
#   make B   Content (all)     make D   Backmatter
#
# Validation (layered):
#   make validate                           Base skeleton
#   make validate TYPE=manual               Manual book type minimums
#   make validate TYPE=manual-l             Legal hierarchy naming conventions
#   make validate TYPE=manual BONUS=operational  Full editorial validation
#   make validate TYPE=manual-l BONUS=operational  Full legal + editorial
#
# Exit codes:
#   E0   Success — target completed without errors
#
#   E1x  Usage errors — arguments, scope, user input
#     E10  Missing required argument (FILE=, PART=, ARTICLE=, PAGES=)
#     E11  Invalid argument value (bad scope, unknown part, out-of-range pages)
#     E12  Conflicting arguments (mutually exclusive options)
#
#   E2x  Dependency errors — files, tools, paths
#     E20  Source file not found (resolved FILE= does not exist)
#     E21  Tool not found (asciidoctor-pdf, qpdf, entr, node, etc.)
#     E22  Directory not found (BOOK_DIR, scope dir, output dir)
#     E23  No matching files (scope/glob returned empty)
#
#   E3x  Build errors — compilation, rendering, extraction
#     E30  PDF build failed (asciidoctor-pdf non-zero exit)
#     E31  Asset generation failed (mermaid, imagemagick, etc.)
#     E32  Page extraction failed (qpdf error)
#     E33  TypeScript build failed (tsc/npm error)
#     E34  Partial build — some files failed in batch (continues, warns)
#
#   E4x  Validation errors — structural, editorial, data
#     E40  Validation failure — structure checks found errors
#     E41  Page marker validation failed (gaps, missing banners)
#     E42  TOC structure validation failed (missing sections, placeholders)
#     E43  Editorial lint failure (style violations found)
#
#   E5x  Theme validation errors (exit 5)
#     E50  Theme validation failed (typography spec)
#     E51  Page layout mismatch
#     E52  Theme parse error
#     E53  Theme element mismatch
#
#   E6x  Scripture/Bible validation errors (exit 6)
#     E60  Verse not found in scripture data
#     E61  Verse text mismatch
#
#   E7x  Report/config errors (exit 7)
#     E70  Report generation failed / system health failing
#     E71  Partial validation failure (some domains failed)
#     E72  Config version mismatch
#
#   Legacy mapping: E1→E10/E11, E2→E20/E21, E3→E30, E4→E40
#
# "Write the vision, and make it plain upon tables,
#  that he may run that readeth it." — Habakkuk 2:2
#
# =============================================================================

.PHONY: all book create \
        A B C D \
        frontmatter part content appendices backmatter \
        article section pages \
        validate lint witness toc toc-fill toc-verify \
        html pdf epub docbook build-ts \
        markdown docx convert \
        assets \
        config watch-pdf deps \
        clean remove setup setup-node setup-asciidoctor \
        status help


# =============================================================================
# §1  PROJECT CONFIGURATION — Loaded from build.config.yaml
# =============================================================================
# All configuration lives in build/build.config.yaml (single source of truth).
# Run `make config` to regenerate config.mk from the YAML.
#
# To change behavior:  Edit build/build.config.yaml, then run `make config`.
# Bootstrap (no config.mk):  ?= defaults below ensure the Makefile always works.
# =============================================================================

# Load generated config (dash-include = no error if missing)
-include config.mk

# --- Defaults (used only when config.mk is absent) ---
ADOC_PDF    ?= asciidoctor-pdf
ADOC_HTML   ?= asciidoctor
OUTDIR      ?= build/output
BOOK_DIR    ?= book
PREVIEW_DIR ?= build/_previews
THEME_FILE  ?= build/themes/cws-manual-legal-theme.yml
ATTRS_FILE  ?= $(PREVIEW_DIR)/_shared-attributes.adoc
OUTPUT_STEM ?= CWS-Company-Identity-Manual
THEME_ARGS  ?= -a pdf-theme=$(THEME_FILE) \
               -a pdf-fontsdir='/usr/share/fonts/truetype/noto;GEM_FONTS_DIR' \
               -a imagesdir=$(CURDIR)/$(BOOK_DIR)/00-assets
SCOPE_A_DIR     ?= A-frontmatter
SCOPE_A_PREVIEW ?= A-frontmatter-preview.adoc
SCOPE_A_OUTPUT  ?= CWS-Frontmatter.pdf
SCOPE_C_DIR     ?= C-appendices
SCOPE_C_PREVIEW ?= C-appendices-preview.adoc
SCOPE_C_OUTPUT  ?= CWS-Appendices.pdf
SCOPE_D_DIR     ?= D-backmatter
SCOPE_D_PREVIEW ?= D-backmatter-preview.adoc
SCOPE_D_OUTPUT  ?= CWS-BackMatter.pdf
ASSETS_DIR  ?= $(BOOK_DIR)/00-assets
ASSETS_PNG  ?= $(ASSETS_DIR)/figures/png
ASSETS_SVG  ?= $(ASSETS_DIR)/figures/svg
ASSETS_GRAY ?= $(ASSETS_DIR)/figures/grayscale
ASSETS_SRC  ?= $(ASSETS_DIR)/sources/diagrams
MERMAID_CFG ?= $(ASSETS_DIR)/sources/config/mermaid-config.json
PUPPET_CFG  ?= $(ASSETS_DIR)/sources/config/puppeteer-config.json
BRAND_DEEP_BLUE   ?= \#1A365D
BRAND_MEDIUM_BLUE ?= \#2C5282
BRAND_GOLD        ?= \#D69E2E
BRAND_DARK        ?= \#2D3748
BRAND_LIGHT       ?= \#F7FAFC


# =============================================================================
# §2  FILESYSTEM DISCOVERY — Derived from config + filesystem
# =============================================================================

# Part discovery — extracts Part IDs from directory names.
# Supports Part-0, non-sequential numbering, letter or number IDs.
PART_DIRS  := $(shell ls -d $(BOOK_DIR)/B-content/Part-*/ 2>/dev/null | sed 's|/$$||' | sort)
PART_COUNT := $(words $(PART_DIRS))
PART_IDS   := $(shell for d in $(PART_DIRS); do basename "$$d" | sed 's/Part-\([^-]*\)-.*/\1/'; done)


# =============================================================================
# §3  CORE ENGINES — Resolution, caching, dependency check
# =============================================================================

# -----------------------------------------------------------------------------
# RESOLUTION ENGINE — Unified 3-mode path resolution
# -----------------------------------------------------------------------------
# All scoped targets use these defines for consistent resolution:
#
#   Mode 1 — Path     (input has /)   Spine-relative, resolves directly
#   Mode 2 — Search   (input has no /)
#              a. Exact directory/file name match
#              b. Prefix + ID match (Part-{ID}-* or Article-{ID}-*)
#              c. Fuzzy case-insensitive match
#   Mode 3 — Compat   Strips book/ prefix automatically
#
# Two defines:
#   resolve_dir   — finds a directory (part, article)
#   resolve_file  — finds a file (section)
#
# Both expect shell variable INPUT to be set. Both set RESOLVED.
# -----------------------------------------------------------------------------

# Resolve a directory using 3-mode resolution.
# Expects INPUT set in calling shell. Sets RESOLVED.
# $(1) = search root under BOOK_DIR (e.g., B-content)
# $(2) = directory name prefix (e.g., Part- or Article-)
# $(3) = max find depth
define resolve_dir
INPUT=$${INPUT#book/}; INPUT=$${INPUT%/}; \
RESOLVED=""; \
if echo "$$INPUT" | grep -q '/'; then \
    if [ -d "$(BOOK_DIR)/$$INPUT" ]; then RESOLVED="$(BOOK_DIR)/$$INPUT"; fi; \
else \
    RESOLVED=$$(find $(BOOK_DIR)/$(1) -maxdepth $(3) -type d -name "$$INPUT" 2>/dev/null | head -1); \
    if [ -z "$$RESOLVED" ]; then \
        RESOLVED=$$(find $(BOOK_DIR)/$(1) -maxdepth $(3) -type d -name "$(2)$$INPUT-*" 2>/dev/null | head -1); \
    fi; \
    if [ -z "$$RESOLVED" ]; then \
        RESOLVED=$$(find $(BOOK_DIR)/$(1) -maxdepth $(3) -type d -name "$(2)$$INPUT*" 2>/dev/null | head -1); \
    fi; \
    if [ -z "$$RESOLVED" ]; then \
        RESOLVED=$$(find $(BOOK_DIR)/$(1) -maxdepth $(3) -type d -iname "*$$INPUT*" 2>/dev/null | head -1); \
    fi; \
fi
endef

# Resolve a file using 3-mode resolution.
# Expects INPUT set in calling shell. Sets RESOLVED.
# $(1) = file extension with dot (e.g., .adoc)
define resolve_file
INPUT=$${INPUT#book/}; \
case "$$INPUT" in *$(1)) ;; *) INPUT="$$INPUT$(1)" ;; esac; \
RESOLVED=""; \
if echo "$$INPUT" | grep -q '/'; then \
    if [ -f "$(BOOK_DIR)/$$INPUT" ]; then RESOLVED="$(BOOK_DIR)/$$INPUT"; fi; \
else \
    RESOLVED=$$(find $(BOOK_DIR)/ -name "$$INPUT" 2>/dev/null | head -1); \
    if [ -z "$$RESOLVED" ]; then \
        RESOLVED=$$(find $(BOOK_DIR)/ -iname "*$$INPUT" 2>/dev/null | head -1); \
    fi; \
fi
endef

# -----------------------------------------------------------------------------
# DEPENDENCY CHECK
# -----------------------------------------------------------------------------

define check_dep
	@command -v $(ADOC_PDF) >/dev/null 2>&1 || \
	    { echo "Error [E21]: $(ADOC_PDF) not found."; \
	      echo "  Run 'make setup' to install dependencies."; exit 2; }
endef

# -----------------------------------------------------------------------------
# CACHE ENGINE — Skip rebuilds when sources haven't changed
# -----------------------------------------------------------------------------
# Timestamp-based: compares output PDF mtime against newest source file.
# Override with FORCE=1 on any target to force rebuild.
#
# Usage in shell blocks:
#   CACHE_OUT="path/to/output.pdf"; \
#   CACHE_SRCS="dir1 dir2 file1"; \
#   $(call check_cache)
#
# Sets CACHE_HIT=1 if up to date (caller should exit 0).
# Sets CACHE_HIT=0 if stale or missing (caller should rebuild).
# -----------------------------------------------------------------------------

FORCE ?=

# Check cache: expects CACHE_OUT and CACHE_SRCS set in calling shell.
# CACHE_SRCS can be directories (searched recursively for .adoc) or files.
# Always checks theme and shared-attributes as implicit dependencies.
define check_cache
CACHE_HIT=0; \
if [ "$(FORCE)" = "1" ]; then \
    CACHE_HIT=0; \
elif [ ! -f "$$CACHE_OUT" ]; then \
    CACHE_HIT=0; \
else \
    NEWEST=$$(find $$CACHE_SRCS $(THEME_FILE) $(ATTRS_FILE) \
        -name '*.adoc' -o -name '*.yml' -o -name '*.yaml' \
        2>/dev/null | xargs ls -t 2>/dev/null | head -1); \
    if [ -n "$$NEWEST" ] && [ "$$CACHE_OUT" -nt "$$NEWEST" ]; then \
        CACHE_HIT=1; \
        echo "Up to date: $$CACHE_OUT"; \
    fi; \
fi
endef


# =============================================================================
# §4  EDITORIAL TARGETS — Scoped PDF builds for page-by-page review
# =============================================================================
# Each preview file is self-contained (includes _shared-attributes.adoc).
# No extra flags needed — theme, fonts, and attributes are baked in.
# =============================================================================

# Default: build full book PDF
all: book

# --- Full Book ---------------------------------------------------------------

# Full editorial build: validate → assets → scopes (cascade) → master → status
book:
	@echo ""
	@echo "Building complete editorial set..."
	@echo "==================================="
	@echo ""
	@$(MAKE) --no-print-directory validate TYPE=manual
	@echo ""
	@echo "--- Generating derived assets ---"
	@echo ""
	@$(MAKE) --no-print-directory assets FORCE=$(FORCE) || true
	@echo ""
	@echo "--- Building scope PDFs (cascading to articles and sections) ---"
	@echo ""
	@$(MAKE) --no-print-directory frontmatter FORCE=$(FORCE) || exit $$?
	@$(MAKE) --no-print-directory content FORCE=$(FORCE) || exit $$?
	@$(MAKE) --no-print-directory appendices FORCE=$(FORCE) || exit $$?
	@$(MAKE) --no-print-directory backmatter FORCE=$(FORCE) || exit $$?
	@echo ""
	@echo "--- Building asset documentation ---"
	@echo ""
	@for f in $$(find $(BOOK_DIR)/00-assets/ -name '*.adoc' 2>/dev/null | sort); do \
	    FNAME=$$(basename "$$f" .adoc); \
	    OUTPATH="$(OUTDIR)/00-assets"; \
	    CACHE_OUT="$$OUTPATH/$$FNAME.pdf"; \
	    CACHE_SRCS="$$f"; \
	    $(call check_cache); \
	    if [ "$$CACHE_HIT" = "1" ]; then continue; fi; \
	    mkdir -p "$$OUTPATH"; \
	    $(ADOC_PDF) $(THEME_ARGS) "$$f" -o "$$OUTPATH/$$FNAME.pdf" 2>&1 || \
	        { rm -f "$$OUTPATH/$$FNAME.pdf"; \
	          echo "Error [E31]: Build failed for asset doc '$$f'."; continue; }; \
	    echo "Built: $$OUTPATH/$$FNAME.pdf"; \
	done
	@echo ""
	@echo "--- Building master PDF ---"
	@echo ""
	@$(MAKE) --no-print-directory create FORCE=$(FORCE) || exit $$?
	@echo ""
	@$(MAKE) --no-print-directory status

# --- Master PDF --------------------------------------------------------------

create:
	$(check_dep)
	@test -f "$(BOOK_DIR)/book.adoc" || \
	    { echo "Error [E20]: $(BOOK_DIR)/book.adoc not found."; exit 2; }
	@CACHE_OUT="$(OUTDIR)/$(OUTPUT_STEM).pdf"; \
	CACHE_SRCS="$(BOOK_DIR)/"; \
	$(call check_cache); \
	if [ "$$CACHE_HIT" = "1" ]; then exit 0; fi; \
	mkdir -p $(OUTDIR); \
	$(ADOC_PDF) $(BOOK_DIR)/book.adoc -o $(OUTDIR)/$(OUTPUT_STEM).pdf 2>&1 || \
	    { rm -f "$(OUTDIR)/$(OUTPUT_STEM).pdf"; \
	      echo "Error [E30]: Full book build failed."; exit 3; }; \
	echo "Built: $(OUTDIR)/$(OUTPUT_STEM).pdf"

# --- Spine Targets (Letter-Based) -------------------------------------------
# Maps directly to the book spine:
#   A = Frontmatter    C = Appendices
#   B = Content        D = Backmatter

A: frontmatter
B: content
C: appendices
D: backmatter

# --- Flat Scope Function ------------------------------------------------------
# Shared logic for flat scope builds (frontmatter, appendices, backmatter).
# Each scope: build all sections in directory → assemble scope PDF from preview.
# Config-driven via SCOPE_X_DIR, SCOPE_X_PREVIEW, SCOPE_X_OUTPUT in §1.
# Add new flat scopes: define SCOPE_X variables in §1, add 2-line target below.
#
# $(1) = scope directory   $(2) = preview filename   $(3) = output filename
define build_flat_scope
FAILED=0; \
for f in $$(ls $(BOOK_DIR)/$(1)/*.adoc 2>/dev/null | sort); do \
    FNAME=$$(basename "$$f"); \
    $(MAKE) --no-print-directory section FILE="$$FNAME" FORCE=$(FORCE) 2>&1 || { \
        echo "  WARN [E34]: Build failed for $$FNAME (continuing batch)"; \
        FAILED=$$((FAILED + 1)); \
    }; \
done; \
if [ "$$FAILED" -gt 0 ]; then \
    echo ""; echo "Warning [E34]: $$FAILED file(s) failed in $(1) batch."; \
fi; \
SRC="$(PREVIEW_DIR)/$(2)"; \
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

# --- Flat Scope Targets ------------------------------------------------------

frontmatter:
	$(check_dep)
	@$(call build_flat_scope,$(SCOPE_A_DIR),$(SCOPE_A_PREVIEW),$(SCOPE_A_OUTPUT))

appendices:
	$(check_dep)
	@$(call build_flat_scope,$(SCOPE_C_DIR),$(SCOPE_C_PREVIEW),$(SCOPE_C_OUTPUT))

backmatter:
	$(check_dep)
	@$(call build_flat_scope,$(SCOPE_D_DIR),$(SCOPE_D_PREVIEW),$(SCOPE_D_OUTPUT))

# --- Part-Level Previews ----------------------------------------------------
# Uses resolve_dir for 3-mode resolution against B-content/Part-*/.
#
# Three modes:
#   Path:   make part PART=B-content/Part-1-identity
#   ID:     make part PART=1
#   Search: make part PART=identity
#
# Also accepts full basename: make part PART=Part-1-identity
# book/ prefix stripped automatically.

# Build all content Parts — iterates actual Part IDs from spine.
content:
	@test "$(PART_COUNT)" -gt 0 || \
	    { echo "Error [E22]: No Part directories found in $(BOOK_DIR)/B-content/."; exit 2; }
	@for N in $(PART_IDS); do \
	    $(MAKE) --no-print-directory part PART=$$N || exit $$?; \
	done

part:
	@test -n "$(PART)" || \
	    { echo "Error [E10]: Missing PART argument."; \
	      echo "  Path:   make part PART=B-content/Part-1-identity"; \
	      echo "  ID:     make part PART=1"; \
	      echo "  Search: make part PART=identity"; \
	      echo "  Available IDs: $(PART_IDS)"; exit 1; }
	$(check_dep)
	@INPUT="$(PART)"; \
	$(call resolve_dir,B-content,Part-,1); \
	if [ -z "$$RESOLVED" ]; then \
	    echo "Error [E11]: Part '$(PART)' not found."; \
	    echo "  Available IDs: $(PART_IDS)"; \
	    for d in $(PART_DIRS); do echo "    $$(basename $$d)"; done; \
	    exit 1; \
	fi; \
	PID=$$(basename "$$RESOLVED" | sed 's/Part-\([^-]*\)-.*/\1/'); \
	PNAME="$(NAME)"; \
	if [ -z "$$PNAME" ]; then \
	    PNAME=$$(basename "$$RESOLVED" | sed 's/Part-[^-]*-//' | awk '{print toupper(substr($$0,1,1)) substr($$0,2)}'); \
	fi; \
	BATCHFAIL=0; \
	for ADIR in $$(ls -d "$$RESOLVED"/Article-*/ 2>/dev/null | sort); do \
	    ANAME=$$(basename "$$ADIR"); \
	    $(MAKE) --no-print-directory article ARTICLE="$$ANAME" FORCE=$(FORCE) 2>&1 || { \
	        echo "  WARN [E34]: Build failed for $$ANAME (continuing batch)"; \
	        BATCHFAIL=$$((BATCHFAIL + 1)); \
	    }; \
	done; \
	if [ "$$BATCHFAIL" -gt 0 ]; then \
	    echo "Warning [E34]: $$BATCHFAIL article(s) failed in Part batch."; \
	fi; \
	RELDIR=$$(echo "$$RESOLVED" | sed 's|^$(BOOK_DIR)/||'); \
	SRC="$(PREVIEW_DIR)/B-part$$PID-preview.adoc"; \
	if [ ! -f "$$SRC" ]; then \
	    echo "Warning: Preview $$SRC not found — cannot build Part $$PID at part level."; \
	    echo "  (Articles/sections within this Part can still be built individually.)"; exit 0; \
	fi; \
	CACHE_OUT="$(OUTDIR)/$$RELDIR/CWS-Part$$PID-$$PNAME.pdf"; \
	CACHE_SRCS="$$RESOLVED/ $$SRC"; \
	$(call check_cache); \
	if [ "$$CACHE_HIT" = "1" ]; then exit 0; fi; \
	mkdir -p "$(OUTDIR)/$$RELDIR"; \
	$(ADOC_PDF) "$$SRC" -o "$(OUTDIR)/$$RELDIR/CWS-Part$$PID-$$PNAME.pdf" 2>&1 || \
	    { rm -f "$(OUTDIR)/$$RELDIR/CWS-Part$$PID-$$PNAME.pdf"; \
	      echo "Error [E30]: Build failed for Part $$PID."; exit 3; }; \
	echo "Built: $(OUTDIR)/$$RELDIR/CWS-Part$$PID-$$PNAME.pdf"

# --- Article-Level (All Sections in an Article) -----------------------------
# Uses resolve_dir for 3-mode resolution against B-content/*/Article-*/.
#
# Three modes:
#   Path:   make article ARTICLE=B-content/Part-1-identity/Article-01-entity-overview
#   ID:     make article ARTICLE=01
#   Search: make article ARTICLE=entity-overview
#
# Also accepts full basename: make article ARTICLE=Article-01-entity-overview
# book/ prefix stripped automatically.
# Empty article directories produce a warning, not an error.

article:
	@test -n "$(ARTICLE)" || \
	    { echo "Error [E10]: Missing ARTICLE argument."; \
	      echo "  Path:   make article ARTICLE=B-content/Part-1-identity/Article-01-entity-overview"; \
	      echo "  ID:     make article ARTICLE=01"; \
	      echo "  Search: make article ARTICLE=entity-overview"; exit 1; }
	$(check_dep)
	@INPUT="$(ARTICLE)"; \
	$(call resolve_dir,B-content,Article-,2); \
	if [ -z "$$RESOLVED" ]; then \
	    echo "Error [E20]: Article '$(ARTICLE)' not found in $(BOOK_DIR)/B-content/."; \
	    echo "  Available:"; \
	    for d in $$(ls -1d $(BOOK_DIR)/B-content/*/Article-* 2>/dev/null | sort); do \
	        echo "    $$(basename $$d)"; \
	    done; \
	    exit 2; \
	fi; \
	ADOC_COUNT=$$(ls "$$RESOLVED"/*.adoc 2>/dev/null | wc -l); \
	if [ "$$ADOC_COUNT" -eq 0 ]; then \
	    echo "Warning: $$RESOLVED is empty (no .adoc files). Skipping."; exit 0; \
	fi; \
	BATCHFAIL=0; \
	for f in $$(ls "$$RESOLVED"/*.adoc 2>/dev/null | sort); do \
	    FNAME=$$(basename "$$f"); \
	    $(MAKE) --no-print-directory section FILE="$$FNAME" FORCE=$(FORCE) 2>&1 || { \
	        echo "  WARN [E34]: Build failed for $$FNAME (continuing batch)"; \
	        BATCHFAIL=$$((BATCHFAIL + 1)); \
	    }; \
	done; \
	if [ "$$BATCHFAIL" -gt 0 ]; then \
	    echo "Warning [E34]: $$BATCHFAIL file(s) failed in article batch."; \
	fi; \
	ARTBASE=$$(basename "$$RESOLVED"); \
	ARTID=$$(echo "$$ARTBASE" | sed 's/Article-\([^-]*\)-.*/\1/'); \
	ARTNAME=$$(echo "$$ARTBASE" | sed 's/Article-[^-]*-//' | tr '-' ' '); \
	PARTPATH=$$(dirname "$$RESOLVED" | sed 's|^$(BOOK_DIR)/||'); \
	OUTPATH="$(OUTDIR)/$$PARTPATH/$$ARTBASE"; \
	CACHE_OUT="$$OUTPATH/CWS-Article-$$ARTID.pdf"; \
	CACHE_SRCS="$$RESOLVED/"; \
	$(call check_cache); \
	if [ "$$CACHE_HIT" = "1" ]; then exit 0; fi; \
	TMPFILE="$(PREVIEW_DIR)/_tmp-article-$$ARTID.adoc"; \
	echo "= CreativeWorkzStudio LLC: Company Identity Manual" > "$$TMPFILE"; \
	echo "include::_shared-attributes.adoc[]" >> "$$TMPFILE"; \
	echo "" >> "$$TMPFILE"; \
	for f in $$(ls "$$RESOLVED"/*.adoc 2>/dev/null | sort); do \
	    RELPATH=$$(realpath --relative-to="$(PREVIEW_DIR)" "$$f"); \
	    echo "include::$$RELPATH[leveloffset=+1]" >> "$$TMPFILE"; \
	done; \
	mkdir -p "$$OUTPATH"; \
	$(ADOC_PDF) "$$TMPFILE" -o "$$OUTPATH/CWS-Article-$$ARTID.pdf" 2>&1; \
	EC=$$?; \
	rm -f "$$TMPFILE"; \
	if [ "$$EC" -ne 0 ]; then \
	    rm -f "$$OUTPATH/CWS-Article-$$ARTID.pdf"; \
	    echo "Error [E30]: Build failed for Article $$ARTID ($$ARTNAME)."; \
	    echo "  Check the AsciiDoc source for syntax errors."; exit 3; \
	fi; \
	echo "Built: $$OUTPATH/CWS-Article-$$ARTID.pdf"

# --- Section-Level (Individual File) ----------------------------------------
# Uses resolve_file for 3-mode resolution against all of book/.
#
# Three modes:
#   Path:   make section FILE=A-frontmatter/A-COVER.adoc
#   Search: make section FILE=A-COVER
#   Compat: make section FILE=book/A-frontmatter/A-COVER  (.adoc optional)
#
# Resolves both letter-based (A-COVER) and number-based (01-COVER) names.

section:
	@test -n "$(FILE)" || \
	    { echo "Error [E10]: Missing FILE argument."; \
	      echo "  Path:   make section FILE=A-frontmatter/A-COVER.adoc"; \
	      echo "  Search: make section FILE=A-COVER"; exit 1; }
	$(check_dep)
	@INPUT="$(FILE)"; \
	$(call resolve_file,.adoc); \
	if [ -z "$$RESOLVED" ] || [ ! -f "$$RESOLVED" ]; then \
	    echo "Error [E20]: '$(FILE)' not found in $(BOOK_DIR)/."; exit 2; \
	fi; \
	OUTPATH=$$(dirname "$$RESOLVED" | sed 's|^$(BOOK_DIR)/||'); \
	OUTNAME=$$(basename "$$RESOLVED" .adoc); \
	CACHE_OUT="$(OUTDIR)/$$OUTPATH/$$OUTNAME.pdf"; \
	CACHE_SRCS="$$RESOLVED"; \
	$(call check_cache); \
	if [ "$$CACHE_HIT" = "1" ]; then exit 0; fi; \
	mkdir -p "$(OUTDIR)/$$OUTPATH"; \
	$(ADOC_PDF) $(THEME_ARGS) "$$RESOLVED" -o "$(OUTDIR)/$$OUTPATH/$$OUTNAME.pdf" 2>&1 || \
	    { rm -f "$(OUTDIR)/$$OUTPATH/$$OUTNAME.pdf"; \
	      echo "Error [E30]: Build failed for '$$RESOLVED'."; \
	      echo "  Check the AsciiDoc source for syntax errors."; exit 3; }; \
	echo "Built: $(OUTDIR)/$$OUTPATH/$$OUTNAME.pdf"

# --- Page Extraction (Specific Pages from a Section PDF) -------------------
# Extracts a page range from an already-built section PDF.
# Requires qpdf. Builds the section first if the PDF doesn't exist.
#
# Usage:
#   make pages FILE=A-COVER PAGES=17-19      Extract pages 17-19
#   make pages FILE=A-COVER PAGES=5          Extract single page 5
#   make pages FILE=A-COVER PAGES=1-3,17-19  Extract disjoint ranges
#
# Output: build/output/<path>/<name>-p<PAGES>.pdf
# Bounds-checked: refuses if page range exceeds the PDF's page count.

pages:
	@test -n "$(FILE)" || \
	    { echo "Error [E10]: Missing FILE argument."; \
	      echo "  make pages FILE=A-COVER PAGES=17-19"; exit 1; }
	@test -n "$(PAGES)" || \
	    { echo "Error [E10]: Missing PAGES argument."; \
	      echo "  make pages FILE=A-COVER PAGES=17-19"; \
	      echo "  make pages FILE=A-COVER PAGES=5"; \
	      echo "  make pages FILE=A-COVER PAGES=1-3,17-19"; exit 1; }
	@command -v qpdf >/dev/null 2>&1 || \
	    { echo "Error [E21]: qpdf not found."; \
	      echo "  Install: sudo apt install qpdf"; exit 2; }
	@INPUT="$(FILE)"; \
	$(call resolve_file,.adoc); \
	if [ -z "$$RESOLVED" ] || [ ! -f "$$RESOLVED" ]; then \
	    echo "Error [E20]: '$(FILE)' not found in $(BOOK_DIR)/."; exit 2; \
	fi; \
	OUTPATH=$$(dirname "$$RESOLVED" | sed 's|^$(BOOK_DIR)/||'); \
	OUTNAME=$$(basename "$$RESOLVED" .adoc); \
	SRCPDF="$(OUTDIR)/$$OUTPATH/$$OUTNAME.pdf"; \
	if [ ! -f "$$SRCPDF" ]; then \
	    echo "Section PDF not found — building first..."; \
	    $(MAKE) --no-print-directory section FILE="$(FILE)" FORCE=$(FORCE) || exit $$?; \
	fi; \
	TOTAL=$$(qpdf --show-npages "$$SRCPDF" 2>/dev/null); \
	if [ -z "$$TOTAL" ] || [ "$$TOTAL" -eq 0 ]; then \
	    echo "Error [E32]: Cannot read page count from $$SRCPDF."; exit 3; \
	fi; \
	MAX=$$(echo "$(PAGES)" | tr ',' '\n' | tr '-' '\n' | sort -n | tail -1); \
	if [ "$$MAX" -gt "$$TOTAL" ]; then \
	    echo "Error [E11]: Page $$MAX exceeds PDF bounds ($$TOTAL pages)."; \
	    echo "  Valid range: 1-$$TOTAL"; exit 1; \
	fi; \
	PAGESAFE=$$(echo "$(PAGES)" | tr ',' '_'); \
	DESTPDF="$(OUTDIR)/$$OUTPATH/$${OUTNAME}-p$${PAGESAFE}.pdf"; \
	qpdf "$$SRCPDF" --pages . $(PAGES) -- "$$DESTPDF" 2>&1 || \
	    { rm -f "$$DESTPDF"; \
	      echo "Error [E32]: qpdf extraction failed."; exit 3; }; \
	echo "Extracted: $$DESTPDF (pages $(PAGES) of $$TOTAL)"


# =============================================================================
# §5  VALIDATION & LINT — Structure validation + TS delegation
# =============================================================================

# -----------------------------------------------------------------------------
# BOOK TYPE VALIDATION
# -----------------------------------------------------------------------------
# Three-layer validation system:
#
#   make validate                           Base skeleton check
#   make validate TYPE=manual               Manual book type minimums
#   make validate TYPE=manual-l             Legal hierarchy naming validation
#   make validate TYPE=manual BONUS=operational  Full operational manual checks
#   make validate TYPE=manual-l BONUS=operational  Full legal + operational
#
# Base (always runs):
#   - book.adoc exists
#   - Spine folders exist (A-frontmatter, B-content, C-appendices, D-backmatter)
#   - Theme file exists
#   - Shared attributes file exists
#
# TYPE=manual (manual bare minimums):
#   - ≥1 Part in B-content
#   - ≥1 Article in any Part
#   - ≥1 .adoc content file
#
# TYPE=manual-l (legal hierarchy naming — superset of manual):
#   - All manual checks, plus:
#   - Part directories match Part-{N}-{name}/ pattern
#   - Article directories match Article-{NN}-{name}/ pattern
#   - Section files match {LETTER}-{NAME}.adoc pattern
#   - Letters are sequential within each article (A, B, C — no gaps)
#   - No old number-prefix files remain
#   - Per-article file inventory with section counts and letter ranges
#
# BONUS=operational (full editorial validation):
#   - Frontmatter/Appendices/Backmatter have content
#   - Assets directory exists
#   - Preview files exist per Part
#   - No empty Article directories
#   - Naming convention consistency
#   - Per-Part and per-Article structure details
# -----------------------------------------------------------------------------

validate:
	@echo ""
	@echo "Book Structure Validation"
	@echo "========================="
	@ERRORS=0; WARNINGS=0; \
	\
	echo ""; \
	echo "--- Base (skeleton) ---"; \
	echo ""; \
	\
	if [ -f "$(BOOK_DIR)/book.adoc" ]; then \
	    echo "  PASS  Master assembly: $(BOOK_DIR)/book.adoc"; \
	else \
	    echo "  FAIL  Master assembly: $(BOOK_DIR)/book.adoc not found"; \
	    ERRORS=$$((ERRORS + 1)); \
	fi; \
	\
	for DIR in A-frontmatter B-content C-appendices D-backmatter; do \
	    if [ -d "$(BOOK_DIR)/$$DIR" ]; then \
	        echo "  PASS  Spine: $(BOOK_DIR)/$$DIR/"; \
	    else \
	        echo "  FAIL  Spine: $(BOOK_DIR)/$$DIR/ not found"; \
	        ERRORS=$$((ERRORS + 1)); \
	    fi; \
	done; \
	\
	if [ -f "$(THEME_FILE)" ]; then \
	    echo "  PASS  Theme: $(THEME_FILE)"; \
	else \
	    echo "  FAIL  Theme: $(THEME_FILE) not found"; \
	    ERRORS=$$((ERRORS + 1)); \
	fi; \
	\
	if [ -f "$(ATTRS_FILE)" ]; then \
	    echo "  PASS  Attributes: $(ATTRS_FILE)"; \
	else \
	    echo "  FAIL  Attributes: $(ATTRS_FILE) not found"; \
	    ERRORS=$$((ERRORS + 1)); \
	fi; \
	\
	TYPE="$(TYPE)"; BONUS="$(BONUS)"; \
	\
	if [ -n "$$TYPE" ]; then \
	    echo ""; \
	    echo "--- Type: $$TYPE ---"; \
	    echo ""; \
	    case "$$TYPE" in \
	    manual) \
	        PCOUNT=$$(ls -d $(BOOK_DIR)/B-content/Part-*/ 2>/dev/null | wc -l); \
	        if [ "$$PCOUNT" -gt 0 ]; then \
	            echo "  PASS  Parts: $$PCOUNT found"; \
	        else \
	            echo "  FAIL  Parts: none found in $(BOOK_DIR)/B-content/"; \
	            ERRORS=$$((ERRORS + 1)); \
	        fi; \
	        \
	        ACOUNT=$$(ls -d $(BOOK_DIR)/B-content/*/Article-*/ 2>/dev/null | wc -l); \
	        if [ "$$ACOUNT" -gt 0 ]; then \
	            echo "  PASS  Articles: $$ACOUNT found across all Parts"; \
	        else \
	            echo "  FAIL  Articles: none found"; \
	            ERRORS=$$((ERRORS + 1)); \
	        fi; \
	        \
	        FCOUNT=$$(find $(BOOK_DIR)/B-content -name "*.adoc" 2>/dev/null | wc -l); \
	        if [ "$$FCOUNT" -gt 0 ]; then \
	            echo "  PASS  Content files: $$FCOUNT .adoc files"; \
	        else \
	            echo "  FAIL  Content files: no .adoc files in content"; \
	            ERRORS=$$((ERRORS + 1)); \
	        fi; \
	        ;; \
	    manual-l) \
	        PCOUNT=$$(ls -d $(BOOK_DIR)/B-content/Part-*/ 2>/dev/null | wc -l); \
	        if [ "$$PCOUNT" -gt 0 ]; then \
	            echo "  PASS  Parts: $$PCOUNT found"; \
	        else \
	            echo "  FAIL  Parts: none found in $(BOOK_DIR)/B-content/"; \
	            ERRORS=$$((ERRORS + 1)); \
	        fi; \
	        \
	        ACOUNT=$$(ls -d $(BOOK_DIR)/B-content/*/Article-*/ 2>/dev/null | wc -l); \
	        if [ "$$ACOUNT" -gt 0 ]; then \
	            echo "  PASS  Articles: $$ACOUNT found across all Parts"; \
	        else \
	            echo "  FAIL  Articles: none found"; \
	            ERRORS=$$((ERRORS + 1)); \
	        fi; \
	        \
	        FCOUNT=$$(find $(BOOK_DIR)/B-content -name "*.adoc" 2>/dev/null | wc -l); \
	        if [ "$$FCOUNT" -gt 0 ]; then \
	            echo "  PASS  Content files: $$FCOUNT .adoc files"; \
	        else \
	            echo "  FAIL  Content files: no .adoc files in content"; \
	            ERRORS=$$((ERRORS + 1)); \
	        fi; \
	        \
	        echo ""; \
	        echo "  Legal hierarchy:"; \
	        \
	        BAD_PARTS=$$(ls -d $(BOOK_DIR)/B-content/*/ 2>/dev/null | while read d; do basename "$$d"; done | grep -v '^Part-[0-9]' | head -5); \
	        if [ -z "$$BAD_PARTS" ]; then \
	            echo "    PASS  Part naming: Part-{N}-{name}/ pattern"; \
	        else \
	            echo "    FAIL  Part naming: non-conforming directories"; \
	            for d in $$BAD_PARTS; do echo "            $$d"; done; \
	            ERRORS=$$((ERRORS + 1)); \
	        fi; \
	        \
	        ART_OK=1; \
	        for PDIR in $$(ls -d $(BOOK_DIR)/B-content/Part-*/ 2>/dev/null | sort); do \
	            BAD_ARTS=$$(ls -d "$$PDIR"*/ 2>/dev/null | while read d; do basename "$$d"; done | grep -v '^Article-[0-9][0-9]-' | head -5); \
	            if [ -n "$$BAD_ARTS" ]; then \
	                PBASE=$$(basename "$$PDIR"); \
	                echo "    FAIL  Article naming in $$PBASE:"; \
	                for a in $$BAD_ARTS; do echo "            $$a"; done; \
	                ERRORS=$$((ERRORS + 1)); ART_OK=0; \
	            fi; \
	        done; \
	        if [ "$$ART_OK" -eq 1 ]; then \
	            echo "    PASS  Article naming: Article-{NN}-{name}/ pattern"; \
	        fi; \
	        \
	        SEC_OK=1; OLD_OK=1; SEQ_OK=1; \
	        ALPHA="ABCDEFGHIJKLMNOPQRSTUVWXYZ"; \
	        for ADIR in $$(find $(BOOK_DIR)/B-content -type d -name "Article-*" | sort); do \
	            ABASE=$$(basename "$$ADIR"); \
	            AFCOUNT=$$(ls "$$ADIR"/*.adoc 2>/dev/null | wc -l); \
	            if [ "$$AFCOUNT" -eq 0 ]; then continue; fi; \
	            OLD=$$(ls "$$ADIR" 2>/dev/null | grep -E '^[0-9]+-[0-9]+-'); \
	            if [ -n "$$OLD" ]; then \
	                echo "    FAIL  $$ABASE: old number-prefix files"; \
	                for f in $$OLD; do echo "            $$f"; done; \
	                ERRORS=$$((ERRORS + 1)); OLD_OK=0; \
	            fi; \
	            BAD=$$(ls "$$ADIR"/*.adoc 2>/dev/null | while read f; do basename "$$f"; done | grep -vE '^[A-Z]-'); \
	            if [ -n "$$BAD" ]; then \
	                echo "    FAIL  $$ABASE: non-conforming section files"; \
	                for f in $$BAD; do echo "            $$f"; done; \
	                ERRORS=$$((ERRORS + 1)); SEC_OK=0; \
	            fi; \
	            IDX=0; \
	            for f in $$(ls "$$ADIR"/*.adoc 2>/dev/null | sort); do \
	                bn=$$(basename "$$f"); \
	                ACTUAL=$$(echo "$$bn" | cut -c1); \
	                POS=$$((IDX + 1)); \
	                EXPECTED=$$(echo "$$ALPHA" | cut -c$$POS-$$POS); \
	                if [ "$$ACTUAL" != "$$EXPECTED" ]; then \
	                    echo "    FAIL  $$ABASE: expected $${EXPECTED}-*, got $$bn"; \
	                    ERRORS=$$((ERRORS + 1)); SEQ_OK=0; \
	                    break; \
	                fi; \
	                IDX=$$((IDX + 1)); \
	            done; \
	        done; \
	        if [ "$$OLD_OK" -eq 1 ]; then echo "    PASS  No old number-prefix files"; fi; \
	        if [ "$$SEC_OK" -eq 1 ]; then echo "    PASS  Section naming: {LETTER}-{NAME}.adoc pattern"; fi; \
	        if [ "$$SEQ_OK" -eq 1 ]; then echo "    PASS  Sequential letters verified"; fi; \
	        \
	        echo ""; \
	        echo "  File inventory:"; \
	        for PDIR in $$(ls -d $(BOOK_DIR)/B-content/Part-*/ 2>/dev/null | sort); do \
	            PBASE=$$(basename "$$PDIR"); \
	            PID=$$(echo "$$PBASE" | sed 's/Part-\([^-]*\)-.*/\1/'); \
	            echo "    Part $$PID:"; \
	            for ADIR in $$(ls -d "$$PDIR"Article-*/ 2>/dev/null | sort); do \
	                ABASE=$$(basename "$$ADIR"); \
	                ARTID=$$(echo "$$ABASE" | sed 's/Article-\([^-]*\)-.*/\1/'); \
	                SCOUNT=$$(ls "$$ADIR"/*.adoc 2>/dev/null | wc -l); \
	                if [ "$$SCOUNT" -eq 0 ]; then \
	                    echo "      Art. $$ARTID: EMPTY"; \
	                    WARNINGS=$$((WARNINGS + 1)); \
	                else \
	                    LAST=$$(ls "$$ADIR"/*.adoc 2>/dev/null | sort | tail -1); \
	                    LAST_LTR=$$(basename "$$LAST" | cut -c1); \
	                    echo "      Art. $$ARTID: $$SCOUNT sections (A through $$LAST_LTR)"; \
	                fi; \
	            done; \
	        done; \
	        ;; \
	    *) \
	        echo "  WARN  Unknown book type: $$TYPE (no type-specific checks)"; \
	        WARNINGS=$$((WARNINGS + 1)); \
	        ;; \
	    esac; \
	fi; \
	\
	if [ -n "$$BONUS" ]; then \
	    echo ""; \
	    echo "--- Bonus: $$BONUS ---"; \
	    echo ""; \
	    case "$$BONUS" in \
	    operational) \
	        for SEC in A-frontmatter C-appendices D-backmatter; do \
	            LABEL=$$(echo "$$SEC" | sed 's/^.-//'); \
	            if [ -d "$(BOOK_DIR)/$$SEC" ]; then \
	                SECCOUNT=$$(ls $(BOOK_DIR)/$$SEC/*.adoc 2>/dev/null | wc -l); \
	                if [ "$$SECCOUNT" -gt 0 ]; then \
	                    echo "  OK    $$LABEL: $$SECCOUNT files"; \
	                else \
	                    echo "  WARN  $$LABEL: directory exists but empty"; \
	                    WARNINGS=$$((WARNINGS + 1)); \
	                fi; \
	            else \
	                echo "  WARN  $$LABEL: directory not found"; \
	                WARNINGS=$$((WARNINGS + 1)); \
	            fi; \
	        done; \
	        \
	        if [ -d "$(BOOK_DIR)/00-assets" ]; then \
	            echo "  OK    Assets: $(BOOK_DIR)/00-assets/"; \
	        else \
	            echo "  WARN  Assets: $(BOOK_DIR)/00-assets/ not found"; \
	            WARNINGS=$$((WARNINGS + 1)); \
	        fi; \
	        \
	        echo ""; \
	        echo "  Structure:"; \
	        for PDIR in $$(ls -d $(BOOK_DIR)/B-content/Part-*/ 2>/dev/null | sort); do \
	            PBASE=$$(basename "$$PDIR"); \
	            PID=$$(echo "$$PBASE" | sed 's/Part-\([^-]*\)-.*/\1/'); \
	            PREV="$(PREVIEW_DIR)/B-part$$PID-preview.adoc"; \
	            ARTCOUNT=$$(ls -d "$$PDIR"Article-*/ 2>/dev/null | wc -l); \
	            if [ -f "$$PREV" ]; then PREVSTAT="preview: yes"; \
	            else PREVSTAT="preview: MISSING"; WARNINGS=$$((WARNINGS + 1)); fi; \
	            echo "    Part $$PID ($$PBASE): $$ARTCOUNT articles, $$PREVSTAT"; \
	            for ADIR in $$(ls -d "$$PDIR"Article-*/ 2>/dev/null | sort); do \
	                ABASE=$$(basename "$$ADIR"); \
	                SCOUNT=$$(ls "$$ADIR"*.adoc 2>/dev/null | wc -l); \
	                if [ "$$SCOUNT" -eq 0 ]; then \
	                    echo "      $$ABASE: EMPTY"; \
	                    WARNINGS=$$((WARNINGS + 1)); \
	                else \
	                    echo "      $$ABASE: $$SCOUNT sections"; \
	                fi; \
	            done; \
	        done; \
	        \
	        echo ""; \
	        echo "  Naming convention:"; \
	        for SEC in A-frontmatter C-appendices D-backmatter; do \
	            LABEL=$$(echo "$$SEC" | sed 's/^.-//'); \
	            LETTER_N=$$(ls $(BOOK_DIR)/$$SEC/ 2>/dev/null | grep -cE '^[A-Z]-'); \
	            NUMBER_N=$$(ls $(BOOK_DIR)/$$SEC/ 2>/dev/null | grep -cE '^[0-9]'); \
	            if [ "$$LETTER_N" -gt 0 ] && [ "$$NUMBER_N" -gt 0 ]; then \
	                echo "    WARN  $$LABEL: mixed ($$LETTER_N letter, $$NUMBER_N number)"; \
	                WARNINGS=$$((WARNINGS + 1)); \
	            elif [ "$$LETTER_N" -gt 0 ]; then \
	                echo "    OK    $$LABEL: letter-based ($$LETTER_N files)"; \
	            elif [ "$$NUMBER_N" -gt 0 ]; then \
	                echo "    OK    $$LABEL: number-based ($$NUMBER_N files)"; \
	            fi; \
	        done; \
	        ;; \
	    *) \
	        echo "  WARN  Unknown bonus scope: $$BONUS (no bonus checks)"; \
	        WARNINGS=$$((WARNINGS + 1)); \
	        ;; \
	    esac; \
	fi; \
	\
	echo ""; \
	echo "--- Summary ---"; \
	echo ""; \
	SCOPE="base"; \
	if [ -n "$$TYPE" ]; then SCOPE="$$TYPE"; fi; \
	if [ -n "$$BONUS" ]; then SCOPE="$$SCOPE+$$BONUS"; fi; \
	if [ "$$ERRORS" -eq 0 ]; then \
	    echo "  $$SCOPE: VALID ($$WARNINGS warnings)"; \
	    echo ""; \
	    exit 0; \
	else \
	    echo "  $$SCOPE: INVALID ($$ERRORS errors, $$WARNINGS warnings)"; \
	    echo ""; \
	    echo "Error [E40]: Validation failed with $$ERRORS error(s). Fix and re-run."; \
	    exit 4; \
	fi

# -----------------------------------------------------------------------------
# EDITORIAL LINT — Content validation against style guide patterns
# -----------------------------------------------------------------------------
# Delegates to TS builder for 57 editorial checks across 7 categories:
#   editorial   (1-8, 36, 47)          — shall, spacers, markers, old-comments, lead,
#                                        title-page, heading guard, attribute-usage, board-reference
#   style       (9-13, 37-40, 53, 55-56) — company-name, chapter-language, all-caps, gold-text,
#                                        register-mixing, closing-filler, paragraph-density,
#                                        redundant-logo, color-accessibility, section-ref-format,
#                                        lead-statement, em-dash-listing
#   structure   (14-15, 32-35, 41-44, 50-51, 54, 57) — unlabeled-breaks, xref-resolution,
#                                        prohibited-attributes, redundant-page-break, include-guard,
#                                        section-role-placement, frontmatter-alignment, toc-table,
#                                        subordinate-heading, appendix-duplication, orphaned-heading,
#                                        image-attributes, hebrew-font-config, reading-guide-parties
#   conversion  (16-17, 48-49)         — icon-macros, admonition-blocks, sidebar-blocks, icon-tables
#   legal       (18-26, 45, 52)        — definition verbs, binding, boilerplate, prohibited, recitals,
#                                        numbering, enumeration, self-reference, document anatomy,
#                                        archaic-context, defined-terms-bold
#   tone        (27-29, 46)            — compulsion language, plain language words, sentence length,
#                                        passive-governance
#   industry    (30-31)                — document register consistency, creator agreement provisions
#
# Usage:
#   make lint FILE=A-COVER              Lint a specific section
#   make lint SCOPE=A                   Lint all frontmatter sections
#   make lint                           Lint all sections (all categories)
#   make lint CHECK=style               Lint with specific category only
#   make lint CHECK=legal               Legal/governance checks (18-26)
#   make lint CHECK=tone                Tone/voice checks (27-29)
#   make lint CHECK=industry            Industry-specific checks (30-31)
#   make lint CHECK=editorial FILE=X    Combine category + file
#   make lint DOCTYPE=eula              All checks with EULA register context
#   make lint DOCTYPE=creator_agreement CHECK=industry  Creator agreement provisions
#   make lint REGISTER=informal SCOPE=A Register mismatch check
#   make lint REPORT=1                  JSON witness report output (stdout)
#   make lint REPORT=1 CHECK=tone       JSON report for tone checks only
#
# Error codes:
#   E43 — Editorial style violations found (exit 4)
# -----------------------------------------------------------------------------

lint: build-ts
	@cd build/builder && node dist/cli.js lint \
	    $(if $(FILE),--file $(FILE)) \
	    $(if $(SCOPE),--scope $(SCOPE)) \
	    $(if $(CHECK),--category $(CHECK)) \
	    $(if $(DOCTYPE),--document-type $(DOCTYPE)) \
	    $(if $(REGISTER),--register $(REGISTER)) \
	    $(if $(filter 1,$(REPORT)),--report) \
	    $(if $(filter 1,$(VERBOSE)),--verbose)

# -----------------------------------------------------------------------------
# TOC — Page markers, § sections, and content-full status
# -----------------------------------------------------------------------------
# Unified TOC command — shows page markers, § section entries, and structure.
# Delegates to TS builder for parsing and display.
#
# Usage:
#   make toc FILE=B-TABLE-OF-CONTENTS    Single section
#   make toc SCOPE=A                      All frontmatter
#   make toc                              All sections
# -----------------------------------------------------------------------------

toc: build-ts
	@cd build/builder && node dist/cli.js toc \
	    $(if $(FILE),$(FILE)) \
	    $(if $(SCOPE),--scope $(SCOPE))

# -----------------------------------------------------------------------------
# TOC FILL — Fill p. __ placeholders with computed page numbers
# -----------------------------------------------------------------------------
# Computes page numbers from // end p.N markers in content files,
# then writes them into the TOC's p. __ placeholders.
#
# Usage:
#   make toc-fill              Fill placeholders (writes TOC file)
#   make toc-fill DRYRUN=1     Report what would change without writing
# -----------------------------------------------------------------------------

toc-fill: build-ts
	@cd build/builder && node dist/cli.js toc --fill \
	    $(if $(filter 1,$(DRYRUN)),--dry-run)

# -----------------------------------------------------------------------------
# TOC VERIFY — Check existing page numbers against computed values
# -----------------------------------------------------------------------------
# Compares page numbers already written in the TOC against current
# computed values. Reports stale numbers that have shifted.
#
# Usage:
#   make toc-verify            Check all page numbers
# Error code: E41 if stale page numbers found.
# -----------------------------------------------------------------------------

toc-verify: build-ts
	@cd build/builder && node dist/cli.js toc --verify

# -----------------------------------------------------------------------------
# WITNESS — Comprehensive system health check
# -----------------------------------------------------------------------------
# Runs all 5 validation domains (structural, editorial, theme, bible, toc)
# and produces a unified health report. Terminal summary by default;
# JSON output with JSON=1.
#
# Health states (Hebrew):
#   healthy (tov)      — all domains pass
#   degraded (yashar)  — warnings present, no errors
#   failing (shavar)   — errors present
#
# Usage:
#   make witness                         All 5 domains, terminal summary
#   make witness DOMAINS=structural      Single domain
#   make witness DOMAINS=editorial,theme Multiple domains
#   make witness JSON=1                  JSON output to stdout
#   make witness JSON=1 OUT=report.json  JSON output to file
#   make witness CHECK=tone              Editorial lint category filter
#   make witness DOCTYPE=eula            Document type context
#
# Error codes:
#   E70 — System health is failing (exit 7)
# -----------------------------------------------------------------------------

witness: build-ts
	@cd build/builder && node dist/cli.js witness \
	    $(if $(DOMAINS),--domains $(DOMAINS)) \
	    $(if $(filter 1,$(JSON)),--json) \
	    $(if $(OUT),--out $(OUT)) \
	    $(if $(FILE),--file $(FILE)) \
	    $(if $(SCOPE),--scope $(SCOPE)) \
	    $(if $(CHECK),--category $(CHECK)) \
	    $(if $(DOCTYPE),--document-type $(DOCTYPE)) \
	    $(if $(REGISTER),--register $(REGISTER)) \
	    $(if $(filter 1,$(VERBOSE)),--verbose)


# =============================================================================
# §6  PUBLISHING — Multi-format via TS builder
# =============================================================================
# These use the TypeScript builder for complete multi-format output.
# For editorial review, use the targets in §4 instead.
# =============================================================================

html: build-ts
	@cd build/builder && npm run manual:html

pdf: build-ts
	@cd build/builder && npm run manual:pdf

epub: build-ts
	@cd build/builder && npm run manual:epub

docbook: build-ts
	@cd build/builder && node dist/cli.js --format docbook

# --- Format Conversion (via docbook-intermediate pipeline) ---
# AsciiDoc → DocBook XML → pandoc → target format
# Pandoc's DocBook reader is excellent; its AsciiDoc reader is not.

markdown: build-ts
	@cd build/builder && node dist/cli.js --format markdown

docx: build-ts
	@cd build/builder && node dist/cli.js --format docx

convert: build-ts
	@test -n "$(FORMAT)" || \
	    { echo "Error [E10]: Missing FORMAT= argument."; \
	      echo "  Usage: make convert FORMAT=markdown"; \
	      echo "  Available: markdown, docx"; exit 1; }
	@cd build/builder && node dist/cli.js --format $(FORMAT)

build-ts:
	@cd build/builder && npm run build 2>/dev/null || (npm install && npm run build)


# =============================================================================
# §7  ASSET GENERATION — Diagrams, grayscale, palette
# =============================================================================
# Generates derived assets that don't exist in source form:
#
#   Phase 1 — Mermaid diagrams:      sources/diagrams/*.mmd → figures/png/*.png + figures/svg/*.svg
#   Phase 2 — Grayscale variants:    figures/png/fig-*.png → figures/grayscale/fig-*-grayscale.png
#   Phase 3 — Color palette swatch:  Brand colors → fig-color-palette.png
#
# Requirements:
#   mmdc          @mermaid-js/mermaid-cli (Phase 1)
#   magick        ImageMagick 7+ (Phases 2-3)
#
# Usage:
#   make assets              Generate all derived assets
#   make assets FORCE=1      Force regeneration (ignore cache)
# =============================================================================

assets:
	@echo ""
	@echo "Asset Generation"
	@echo "================"
	@echo ""
	@GENERATED=0; SKIPPED=0; FAILED=0; \
	\
	echo "--- Phase 1: Mermaid diagrams ---"; \
	echo ""; \
	if command -v mmdc >/dev/null 2>&1; then \
	    for f in $(ASSETS_SRC)/fig-*.mmd; do \
	        [ -f "$$f" ] || continue; \
	        NAME=$$(basename "$$f" .mmd); \
	        PNG="$(ASSETS_PNG)/$$NAME.png"; \
	        SVG="$(ASSETS_SVG)/$$NAME.svg"; \
	        if [ "$(FORCE)" != "1" ] && [ -f "$$PNG" ] && [ "$$PNG" -nt "$$f" ]; then \
	            echo "  OK    $$NAME.png (up to date)"; \
	            SKIPPED=$$((SKIPPED + 1)); \
	        else \
	            if mmdc -i "$$f" -o "$$PNG" \
	                -b white -p $(PUPPET_CFG) -s 3 >/dev/null 2>&1; then \
	                echo "  GEN   $$NAME.png"; \
	                GENERATED=$$((GENERATED + 1)); \
	            else \
	                echo "  FAIL  $$NAME.png"; \
	                FAILED=$$((FAILED + 1)); \
	            fi; \
	        fi; \
	        if [ "$(FORCE)" != "1" ] && [ -f "$$SVG" ] && [ "$$SVG" -nt "$$f" ]; then \
	            echo "  OK    $$NAME.svg (up to date)"; \
	            SKIPPED=$$((SKIPPED + 1)); \
	        else \
	            if mmdc -i "$$f" -o "$$SVG" \
	                -b transparent -p $(PUPPET_CFG) >/dev/null 2>&1; then \
	                echo "  GEN   $$NAME.svg"; \
	                GENERATED=$$((GENERATED + 1)); \
	            else \
	                echo "  FAIL  $$NAME.svg"; \
	                FAILED=$$((FAILED + 1)); \
	            fi; \
	        fi; \
	    done; \
	else \
	    echo "  SKIP  mmdc not found (install @mermaid-js/mermaid-cli)"; \
	fi; \
	\
	echo ""; \
	echo "--- Phase 2: Grayscale variants ---"; \
	echo ""; \
	if command -v magick >/dev/null 2>&1; then \
	    for f in $(ASSETS_PNG)/fig-*.png; do \
	        [ -f "$$f" ] || continue; \
	        case "$$(basename $$f)" in *-grayscale.png|fig-color-palette.png|fig-title-page-mockup.png) continue ;; esac; \
	        NAME=$$(basename "$$f" .png); \
	        OUT="$(ASSETS_GRAY)/$${NAME}-grayscale.png"; \
	        if [ "$(FORCE)" != "1" ] && [ -f "$$OUT" ] && [ "$$OUT" -nt "$$f" ]; then \
	            echo "  OK    $${NAME}-grayscale.png (up to date)"; \
	            SKIPPED=$$((SKIPPED + 1)); \
	        else \
	            if magick "$$f" -colorspace Gray "$$OUT" 2>/dev/null; then \
	                echo "  GEN   $${NAME}-grayscale.png"; \
	                GENERATED=$$((GENERATED + 1)); \
	            else \
	                echo "  FAIL  $${NAME}-grayscale.png"; \
	                FAILED=$$((FAILED + 1)); \
	            fi; \
	        fi; \
	    done; \
	else \
	    echo "  SKIP  ImageMagick (magick) not found"; \
	fi; \
	\
	echo ""; \
	echo "--- Phase 3: Color palette swatch ---"; \
	echo ""; \
	if command -v magick >/dev/null 2>&1; then \
	    OUT="$(ASSETS_PNG)/fig-color-palette.png"; \
	    if [ "$(FORCE)" != "1" ] && [ -f "$$OUT" ] && [ -f "$(MERMAID_CFG)" ] && [ "$$OUT" -nt "$(MERMAID_CFG)" ]; then \
	        echo "  OK    fig-color-palette.png (up to date)"; \
	        SKIPPED=$$((SKIPPED + 1)); \
	    else \
	        if magick -size 900x220 xc:white \
	            \( -size 150x120 xc:'$(BRAND_DEEP_BLUE)' \) -geometry +30+20 -composite \
	            \( -size 150x120 xc:'$(BRAND_MEDIUM_BLUE)' \) -geometry +195+20 -composite \
	            \( -size 150x120 xc:'$(BRAND_GOLD)' \) -geometry +360+20 -composite \
	            \( -size 150x120 xc:'$(BRAND_DARK)' \) -geometry +525+20 -composite \
	            \( -size 150x120 xc:'$(BRAND_LIGHT)' -bordercolor '$(BRAND_DARK)' -border 1 \) -geometry +690+19 -composite \
	            -font 'Noto-Sans-Bold' -pointsize 14 \
	            -fill white -annotate +72+110 'Deep Blue' \
	            -annotate +223+110 'Medium Blue' \
	            -fill '$(BRAND_DEEP_BLUE)' -annotate +410+110 'Gold' \
	            -fill white -annotate +585+110 'Dark' \
	            -fill '$(BRAND_DARK)' -annotate +740+110 'Light' \
	            -font 'Noto-Sans-Regular' -pointsize 12 \
	            -fill '$(BRAND_DARK)' \
	            -annotate +62+165 '$(BRAND_DEEP_BLUE)' \
	            -annotate +225+165 '$(BRAND_MEDIUM_BLUE)' \
	            -annotate +393+165 '$(BRAND_GOLD)' \
	            -annotate +558+165 '$(BRAND_DARK)' \
	            -annotate +723+165 '$(BRAND_LIGHT)' \
	            -font 'Noto-Sans-Regular' -pointsize 11 \
	            -fill gray \
	            -annotate +56+190 'Primary' \
	            -annotate +218+190 'Secondary' \
	            -annotate +400+190 'Accent' \
	            -annotate +560+190 'Borders' \
	            -annotate +710+190 'Backgrounds' \
	            -depth 8 \
	            "$$OUT" 2>/dev/null; then \
	            echo "  GEN   fig-color-palette.png"; \
	            GENERATED=$$((GENERATED + 1)); \
	        else \
	            echo "  FAIL  fig-color-palette.png"; \
	            FAILED=$$((FAILED + 1)); \
	        fi; \
	    fi; \
	else \
	    echo "  SKIP  ImageMagick (magick) not found"; \
	fi; \
	\
	echo ""; \
	echo "--- Summary ---"; \
	echo ""; \
	echo "  Generated: $$GENERATED  Skipped: $$SKIPPED  Failed: $$FAILED"; \
	if [ "$$FAILED" -gt 0 ]; then \
	    echo "  Warning: $$FAILED asset(s) failed to generate."; \
	fi; \
	echo ""


# =============================================================================
# §8  DEVELOPMENT — config, watch, deps, status, clean, remove, setup
# =============================================================================

# Regenerate config.mk from build.config.yaml (single source of truth)
config:
	@command -v yq >/dev/null 2>&1 || \
	    { echo "Error [E21]: yq not found."; \
	      echo "  Install: pip install yq"; exit 2; }
	@bash build/scripts/gen-config-mk.sh > config.mk
	@echo "Generated: config.mk (from build/build.config.yaml)"

# Watch for changes and rebuild full book PDF (requires entr)
watch-pdf:
	@command -v entr >/dev/null 2>&1 || \
	    { echo "Error [E21]: entr not found."; \
	      echo "  Install: sudo apt install entr"; exit 2; }
	$(check_dep)
	@echo "Watching for changes... (Ctrl+C to stop)"
	@echo "  Rebuilding: $(OUTDIR)/$(OUTPUT_STEM).pdf"
	@echo ""
	@find $(BOOK_DIR)/ build/themes/ build/_previews/ \
	    \( -name "*.adoc" -o -name "*.yml" \) 2>/dev/null | \
	    entr -r $(MAKE) --no-print-directory book

# Check all dependencies (editorial + optional publishing)
deps:
	@echo ""
	@echo "Dependency Check"
	@echo "================"
	@echo ""
	@MISSING=0; \
	echo "Required (editorial):"; \
	if command -v $(ADOC_PDF) >/dev/null 2>&1; then \
	    VER=$$($(ADOC_PDF) --version 2>&1 | head -1); \
	    echo "  OK    asciidoctor-pdf: $$VER"; \
	else \
	    echo "  MISS  asciidoctor-pdf: not found"; \
	    MISSING=$$((MISSING + 1)); \
	fi; \
	if command -v $(ADOC_HTML) >/dev/null 2>&1; then \
	    VER=$$($(ADOC_HTML) --version 2>&1 | head -1); \
	    echo "  OK    asciidoctor: $$VER"; \
	else \
	    echo "  MISS  asciidoctor: not found"; \
	    MISSING=$$((MISSING + 1)); \
	fi; \
	echo ""; \
	echo "Optional (asset generation):"; \
	if command -v mmdc >/dev/null 2>&1; then \
	    echo "  OK    mmdc: $$(mmdc --version 2>&1 | head -1) (Mermaid diagram rendering)"; \
	else \
	    echo "  MISS  mmdc: not found (npm i -g @mermaid-js/mermaid-cli)"; \
	fi; \
	if command -v magick >/dev/null 2>&1; then \
	    echo "  OK    magick: $$(magick --version 2>&1 | head -1 | sed 's/Version: //')"; \
	else \
	    echo "  MISS  magick: not found (apt install imagemagick — grayscale/palette disabled)"; \
	fi; \
	echo ""; \
	echo "Optional (dev tools):"; \
	if command -v entr >/dev/null 2>&1; then \
	    echo "  OK    entr: available (watch-pdf)"; \
	else \
	    echo "  MISS  entr: not found (watch-pdf disabled)"; \
	fi; \
	if command -v ruby >/dev/null 2>&1; then \
	    echo "  OK    ruby: $$(ruby --version 2>&1 | head -1)"; \
	else \
	    echo "  MISS  ruby: not found (setup-asciidoctor disabled)"; \
	fi; \
	echo ""; \
	echo "Optional (format conversion):"; \
	if command -v pandoc >/dev/null 2>&1; then \
	    echo "  OK    pandoc: $$(pandoc --version 2>&1 | head -1) (markdown, docx conversion)"; \
	else \
	    echo "  MISS  pandoc: not found (apt install pandoc — make markdown/docx disabled)"; \
	fi; \
	echo ""; \
	echo "Optional (publishing):"; \
	if [ -d "build/builder/node_modules" ]; then \
	    echo "  OK    TypeScript builder: installed"; \
	else \
	    echo "  SKIP  TypeScript builder: not installed (run 'make setup-node')"; \
	fi; \
	echo ""; \
	if [ "$$MISSING" -eq 0 ]; then \
	    echo "All required dependencies available."; \
	else \
	    echo "$$MISSING required dependency(ies) missing. Run 'make setup'."; \
	    exit 2; \
	fi; \
	echo ""

# Clean all generated output
clean:
	@rm -rf $(OUTDIR)/
	@rm -f $(PREVIEW_DIR)/_tmp-*
	@echo "Cleaned: $(OUTDIR)/ and temp preview files"

# Safe targeted removal — uses same resolution as build targets
#   make remove SCOPE=A           Remove frontmatter outputs
#   make remove SCOPE=B           Remove all content outputs
#   make remove PART=1            Remove Part 1 outputs
#   make remove ARTICLE=01        Remove Article 01 outputs
#   make remove FILE=A-COVER      Remove single section output
#   make remove SCOPE=assets      Remove asset documentation outputs
#   make remove SCOPE=master      Remove master PDF only
remove:
	@REMOVED=0; \
	if [ -n "$(FILE)" ]; then \
	    INPUT="$(FILE)"; \
	    $(call resolve_file,.adoc); \
	    if [ -z "$$RESOLVED" ]; then \
	        echo "Error [E20]: '$(FILE)' not found."; exit 2; \
	    fi; \
	    OUTPATH=$$(dirname "$$RESOLVED" | sed 's|^$(BOOK_DIR)/||'); \
	    OUTNAME=$$(basename "$$RESOLVED" .adoc); \
	    TARGET="$(OUTDIR)/$$OUTPATH/$$OUTNAME.pdf"; \
	    if [ -f "$$TARGET" ]; then \
	        rm -f "$$TARGET"; \
	        echo "Removed: $$TARGET"; REMOVED=$$((REMOVED + 1)); \
	    else \
	        echo "Not found: $$TARGET"; \
	    fi; \
	elif [ -n "$(ARTICLE)" ]; then \
	    INPUT="$(ARTICLE)"; \
	    $(call resolve_dir,B-content,Article-,2); \
	    if [ -z "$$RESOLVED" ]; then \
	        echo "Error [E11]: Article '$(ARTICLE)' not found."; exit 1; \
	    fi; \
	    OUTPATH=$$(echo "$$RESOLVED" | sed 's|^$(BOOK_DIR)/||'); \
	    TARGET="$(OUTDIR)/$$OUTPATH"; \
	    if [ -d "$$TARGET" ]; then \
	        COUNT=$$(find "$$TARGET" -name '*.pdf' | wc -l); \
	        rm -rf "$$TARGET"; \
	        echo "Removed: $$TARGET/ ($$COUNT PDFs)"; REMOVED=$$COUNT; \
	    else \
	        echo "Not found: $$TARGET/"; \
	    fi; \
	elif [ -n "$(PART)" ]; then \
	    INPUT="$(PART)"; \
	    $(call resolve_dir,B-content,Part-,1); \
	    if [ -z "$$RESOLVED" ]; then \
	        echo "Error [E11]: Part '$(PART)' not found."; exit 1; \
	    fi; \
	    OUTPATH=$$(echo "$$RESOLVED" | sed 's|^$(BOOK_DIR)/||'); \
	    TARGET="$(OUTDIR)/$$OUTPATH"; \
	    if [ -d "$$TARGET" ]; then \
	        COUNT=$$(find "$$TARGET" -name '*.pdf' | wc -l); \
	        rm -rf "$$TARGET"; \
	        echo "Removed: $$TARGET/ ($$COUNT PDFs)"; REMOVED=$$COUNT; \
	    else \
	        echo "Not found: $$TARGET/"; \
	    fi; \
	elif [ -n "$(SCOPE)" ]; then \
	    case "$(SCOPE)" in \
	        A|frontmatter) TARGET="$(OUTDIR)/$(SCOPE_A_DIR)" ;; \
	        B|content)     TARGET="$(OUTDIR)/B-content" ;; \
	        C|appendices)  TARGET="$(OUTDIR)/$(SCOPE_C_DIR)" ;; \
	        D|backmatter)  TARGET="$(OUTDIR)/$(SCOPE_D_DIR)" ;; \
	        assets)        TARGET="$(OUTDIR)/00-assets" ;; \
	        master)        TARGET="$(OUTDIR)/$(OUTPUT_STEM).pdf" ;; \
	        *) echo "Error [E11]: Unknown scope '$(SCOPE)'."; \
	           echo "  Valid: A, B, C, D, frontmatter, content, appendices, backmatter, assets, master"; \
	           exit 1 ;; \
	    esac; \
	    if [ -f "$$TARGET" ]; then \
	        rm -f "$$TARGET"; \
	        echo "Removed: $$TARGET"; REMOVED=1; \
	    elif [ -d "$$TARGET" ]; then \
	        COUNT=$$(find "$$TARGET" -name '*.pdf' | wc -l); \
	        rm -rf "$$TARGET"; \
	        echo "Removed: $$TARGET/ ($$COUNT PDFs)"; REMOVED=$$COUNT; \
	    else \
	        echo "Not found: $$TARGET"; \
	    fi; \
	else \
	    echo "Error [E10]: Missing argument."; \
	    echo "  make remove SCOPE=A           Frontmatter outputs"; \
	    echo "  make remove SCOPE=B           All content outputs"; \
	    echo "  make remove PART=1            Part 1 outputs"; \
	    echo "  make remove ARTICLE=01        Article 01 outputs"; \
	    echo "  make remove FILE=A-COVER      Single section output"; \
	    echo "  make remove SCOPE=assets      Asset documentation"; \
	    echo "  make remove SCOPE=master      Master PDF only"; \
	    exit 1; \
	fi; \
	echo "  $$REMOVED PDF(s) removed."

# Build status dashboard
status:
	@echo ""
	@echo "Build Status"
	@echo "============"
	@echo ""
	@TOTAL=0; STALE=0; CURRENT=0; \
	if [ ! -d "$(OUTDIR)" ]; then \
	    echo "  No output directory. Run 'make book' or a scope target."; \
	    echo ""; exit 0; \
	fi; \
	for PDF in $$(find $(OUTDIR)/ -name '*.pdf' 2>/dev/null | sort); do \
	    TOTAL=$$((TOTAL + 1)); \
	    REL=$$(echo "$$PDF" | sed 's|^$(OUTDIR)/||'); \
	    SIZE=$$(du -h "$$PDF" 2>/dev/null | cut -f1); \
	    BUILT=$$(date -r "$$PDF" '+%Y-%m-%d %H:%M' 2>/dev/null); \
	    SCOPE_DIR=""; \
	    case "$$REL" in \
	        $(OUTPUT_STEM).pdf) SCOPE_DIR="$(BOOK_DIR)/" ;; \
	        $(SCOPE_A_DIR)/*) SCOPE_DIR="$(BOOK_DIR)/$(SCOPE_A_DIR)/" ;; \
	        $(SCOPE_C_DIR)/*) SCOPE_DIR="$(BOOK_DIR)/$(SCOPE_C_DIR)/" ;; \
	        $(SCOPE_D_DIR)/*) SCOPE_DIR="$(BOOK_DIR)/$(SCOPE_D_DIR)/" ;; \
	        B-content/*) \
	            PARTDIR=$$(echo "$$REL" | sed 's|^\(B-content/[^/]*/\).*|\1|'); \
	            if echo "$$REL" | grep -q 'Article-'; then \
	                SCOPE_DIR="$(BOOK_DIR)/$$(echo "$$REL" | sed 's|^\(B-content/[^/]*/[^/]*/\).*|\1|')"; \
	            else \
	                SCOPE_DIR="$(BOOK_DIR)/$$PARTDIR"; \
	            fi ;; \
	    esac; \
	    IS_STALE="no"; \
	    if [ -n "$$SCOPE_DIR" ] && [ -e "$$SCOPE_DIR" ]; then \
	        NEWEST=$$(find "$$SCOPE_DIR" $(THEME_FILE) $(ATTRS_FILE) \
	            -name '*.adoc' -o -name '*.yml' 2>/dev/null | \
	            xargs ls -t 2>/dev/null | head -1); \
	        if [ -n "$$NEWEST" ] && [ "$$NEWEST" -nt "$$PDF" ]; then \
	            IS_STALE="yes"; \
	        fi; \
	    fi; \
	    if [ "$$IS_STALE" = "yes" ]; then \
	        STALE=$$((STALE + 1)); \
	        echo "  STALE   $$REL  ($$SIZE, built $$BUILT)"; \
	    else \
	        CURRENT=$$((CURRENT + 1)); \
	        echo "  OK      $$REL  ($$SIZE, built $$BUILT)"; \
	    fi; \
	done; \
	echo ""; \
	TOTAL_SIZE=$$(du -sh $(OUTDIR)/ 2>/dev/null | cut -f1); \
	echo "  Total: $$TOTAL PDFs ($$TOTAL_SIZE) — $$CURRENT current, $$STALE stale"; \
	if [ "$$STALE" -gt 0 ]; then \
	    echo "  Rebuild stale targets or run 'make book FORCE=1' for full rebuild."; \
	fi; \
	echo ""

# Full dependency setup
setup: setup-asciidoctor
	@echo ""
	@echo "=============================================="
	@echo "  Setup complete!"
	@echo "  Run 'make book' to build the full manual"
	@echo "  Run 'make deps' to verify dependencies"
	@echo "  Run 'make help' to see all targets"
	@echo "=============================================="

setup-node:
	@echo "Installing Node.js dependencies (publishing builder)..."
	@cd build/builder && npm install
	@cd build/builder && npm run build
	@echo "Node.js setup complete"

setup-asciidoctor:
	@echo "Installing Asciidoctor tools..."
	@which gem > /dev/null || (echo "Error [E21]: Ruby gem not found. Please install Ruby first." && exit 2)
	gem install asciidoctor --user-install || gem install asciidoctor
	gem install asciidoctor-pdf --user-install || gem install asciidoctor-pdf
	gem install asciidoctor-epub3 --user-install || gem install asciidoctor-epub3
	gem install rouge --user-install || gem install rouge
	@echo "Asciidoctor setup complete"


# =============================================================================
# §9  HELP — Self-documentation
# =============================================================================

help:
	@echo ""
	@echo "CWS Company Identity Manual — Build System"
	@echo "==========================================="
	@echo ""
	@echo "Architecture: §1 Config → §2 Discovery → §3 Engines → §4 Targets"
	@echo "              §5 Validation → §6 Publishing → §7 Assets → §8 Dev → §9 Help"
	@echo ""
	@echo "Resolution (all scoped targets):"
	@echo "  Path mode    (has /)    Spine-relative path, resolves directly"
	@echo "  Search mode  (no /)    ID → name → fuzzy match"
	@echo "  Backward compat        Strips book/ prefix; extensions optional"
	@echo ""
	@echo "Editorial Review (PDF):"
	@echo "  book | create     Full book PDF                 → build/output/"
	@echo "  A | frontmatter   Frontmatter                   → build/output/$(SCOPE_A_DIR)/"
	@echo "  B | content       All content Parts             → build/output/B-content/<part>/"
	@echo "  C | appendices    Appendices                    → build/output/$(SCOPE_C_DIR)/"
	@echo "  D | backmatter    Back matter                   → build/output/$(SCOPE_D_DIR)/"
	@echo ""
	@echo "  part PART=...     Single Part (IDs: $(PART_IDS), NAME= to override)"
	@echo "                      make part PART=1"
	@echo "                      make part PART=identity"
	@echo "                      make part PART=B-content/Part-1-identity"
	@echo ""
	@echo "  article ARTICLE=  Single Article (all sections combined)"
	@echo "                      make article ARTICLE=01"
	@echo "                      make article ARTICLE=entity-overview"
	@echo ""
	@echo "  section FILE=     Any single .adoc file (themed)"
	@echo "                      make section FILE=A-COVER"
	@echo "                      make section FILE=A-frontmatter/A-COVER.adoc"
	@echo ""
	@echo "  pages FILE= PAGES=  Extract specific pages from a section PDF"
	@echo "                      make pages FILE=A-COVER PAGES=17-19"
	@echo "                      make pages FILE=A-COVER PAGES=5"
	@echo "                      make pages FILE=A-COVER PAGES=1-3,17-19"
	@echo "                      Requires: qpdf. Bounds-checked."
	@echo ""
	@echo "Validation:"
	@echo "  validate                                        Base skeleton"
	@echo "  validate TYPE=manual                            Book type minimums"
	@echo "  validate TYPE=manual-l                          Legal hierarchy naming"
	@echo "  validate TYPE=manual   BONUS=operational        Full editorial"
	@echo "  validate TYPE=manual-l BONUS=operational        Full legal + editorial"
	@echo ""
	@echo "  lint FILE=A-COVER    Editorial style lint (single file)"
	@echo "  lint SCOPE=A         Lint all frontmatter"
	@echo "  lint                 Lint all sections (all 57 checks)"
	@echo "  lint CHECK=editorial Checks 1-8, 36, 47: shall, spacers, markers, attributes, board-ref"
	@echo "  lint CHECK=style     Checks 9-13, 37-40, 53, 55-56: company-name, caps, density, lead-stmt"
	@echo "  lint CHECK=structure Checks 14-15, 32-35, 41-44, 50-51, 54, 57: xref, roles, orphans, images"
	@echo "  lint CHECK=conversion Checks 16-17, 48-49: icon-macros, admonitions, sidebars, icon-tables"
	@echo "  lint CHECK=legal     Checks 18-26, 45, 52: definitions, binding, boilerplate, anatomy, terms"
	@echo "  lint CHECK=tone      Checks 27-29, 46: compulsion, plain language, sentence length, passive"
	@echo "  lint CHECK=industry  Checks 30-31: register consistency, creator provisions"
	@echo "  lint CHECK=all       All 57 checks (default)"
	@echo "  lint DOCTYPE=eula    Context-aware: all checks with EULA register"
	@echo "  lint REGISTER=formal Override register for register-consistency check"
	@echo "  lint REPORT=1        JSON witness report to stdout"
	@echo "  lint VERBOSE=1       Show all findings (default: top 5 per file)"
	@echo ""
	@echo "Witness (System Health):"
	@echo "  witness                              All 5 domains: structural, editorial, theme, bible, toc"
	@echo "  witness DOMAINS=structural           Single domain"
	@echo "  witness DOMAINS=editorial,theme      Multiple domains"
	@echo "  witness JSON=1                       JSON output to stdout"
	@echo "  witness JSON=1 OUT=report.json       JSON output to file"
	@echo "  witness CHECK=tone                   Editorial category filter"
	@echo "  witness DOCTYPE=eula                 Document type context"
	@echo "  witness VERBOSE=1                    Show all recommendations"
	@echo ""
	@echo "TOC & Page Navigation:"
	@echo "  toc FILE=...         Page markers, § sections, content-full status"
	@echo "  toc SCOPE=A          All frontmatter page status"
	@echo "  toc                  All sections with page markers"
	@echo "  toc-fill             Fill p. __ placeholders with computed page numbers"
	@echo "  toc-fill DRYRUN=1    Report fills without writing"
	@echo "  toc-verify           Check existing page numbers against computed values"
	@echo ""
	@echo "Asset Generation:"
	@echo "  assets         Generate derived assets (diagrams, grayscale, palette)"
	@echo "                   Requires: mmdc (Mermaid), magick (ImageMagick)"
	@echo ""
	@echo "Caching:"
	@echo "  Builds skip if output is newer than all sources (automatic)."
	@echo "  FORCE=1      Force rebuild:  make frontmatter FORCE=1"
	@echo ""
	@echo "Editorial Development:"
	@echo "  config       Regenerate config.mk from build/build.config.yaml"
	@echo "  status       Build dashboard (current/stale PDFs, sizes, timestamps)"
	@echo "  remove ...   Safe targeted removal:"
	@echo "                 SCOPE=A|B|C|D|assets|master  FILE=  PART=  ARTICLE="
	@echo "  watch-pdf    Watch for changes, auto-rebuild book PDF (requires entr)"
	@echo "  deps         Check all dependencies (required + optional)"
	@echo "  clean        Remove all generated output and temp files"
	@echo "  setup        Install required dependencies (asciidoctor)"
	@echo ""
	@echo "Publishing (TypeScript Builder — requires 'make setup-node'):"
	@echo "  html         Build HTML format"
	@echo "  pdf          Build PDF via builder"
	@echo "  epub         Build EPUB format"
	@echo "  docbook      Build DocBook XML"
	@echo ""
	@echo "Format Conversion (requires pandoc + TypeScript builder):"
	@echo "  markdown           Convert to GitHub-Flavored Markdown"
	@echo "  docx               Convert to Word document (.docx)"
	@echo "  convert FORMAT=    Generic: make convert FORMAT=markdown"
	@echo "                     Pipeline: AsciiDoc → DocBook XML → pandoc → target"
	@echo ""
	@echo "Error Codes:"
	@echo "  E0  (0)  Success"
	@echo "  E1x (1)  Usage:       E10 missing arg, E11 invalid value, E12 conflict"
	@echo "  E2x (2)  Dependency:  E20 source file, E21 tool, E22 directory, E23 empty"
	@echo "  E3x (3)  Build:       E30 PDF, E31 asset, E32 extraction, E33 TS, E34 batch"
	@echo "  E4x (4)  Validation:  E40 structure, E41 pages, E42 TOC, E43 lint"
	@echo "  E5x (5)  Theme:       E50 typography, E51 layout, E52 parse, E53 element"
	@echo "  E6x (6)  Scripture:   E60 verse missing, E61 text mismatch"
	@echo "  E7x (7)  Report:      E70 health failing, E71 partial failure, E72 config"
	@echo ""
	@echo "Output: ./build/output/"
	@echo ""
