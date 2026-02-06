# =============================================================================
# CWS Company Identity Manual — Build System
# =============================================================================
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
#   E0  Success — target completed without errors
#   E1  Usage error — missing or invalid arguments (bad PART=, ARTICLE=, FILE=)
#   E2  Missing dependency — required tool or source file not found
#   E3  Build failure — asciidoctor-pdf returned a non-zero exit code
#   E4  Validation failure — structure checks found errors (validate target only)
#
# "Write the vision, and make it plain upon tables,
#  that he may run that readeth it." — Habakkuk 2:2
#
# =============================================================================

.PHONY: all book create \
        A B C D \
        frontmatter part content appendices backmatter \
        article section \
        assets \
        validate \
        watch-pdf deps \
        html pdf epub docbook build-ts \
        clean remove setup setup-node setup-asciidoctor \
        status help


# =============================================================================
# VARIABLES
# =============================================================================

ADOC_PDF    = asciidoctor-pdf
ADOC_HTML   = asciidoctor
OUTDIR      = build/output
BOOK_DIR    = book
PREVIEW_DIR = build/_previews
THEME_FILE  = build/themes/cws-manual-theme.yml
ATTRS_FILE  = $(PREVIEW_DIR)/_shared-attributes.adoc

# Theme arguments for standalone section builds (files without their own header).
# imagesdir uses absolute path so files in subdirectories find assets correctly.
THEME_ARGS = -a pdf-theme=$(THEME_FILE) \
             -a pdf-fontsdir='/usr/share/fonts/truetype/noto;GEM_FONTS_DIR' \
             -a imagesdir=$(CURDIR)/$(BOOK_DIR)/00-assets

# Asset generation paths
ASSETS_DIR  = $(BOOK_DIR)/00-assets
ASSETS_PNG  = $(ASSETS_DIR)/figures/png
ASSETS_SVG  = $(ASSETS_DIR)/figures/svg
ASSETS_GRAY = $(ASSETS_DIR)/figures/grayscale
ASSETS_SRC  = $(ASSETS_DIR)/sources/diagrams
MERMAID_CFG = $(ASSETS_DIR)/sources/config/mermaid-config.json
PUPPET_CFG  = $(ASSETS_DIR)/sources/config/puppeteer-config.json

# Brand colors (canonical source: Brand Color Palette / mermaid-config.json)
BRAND_DEEP_BLUE   = \#1A365D
BRAND_MEDIUM_BLUE = \#2C5282
BRAND_GOLD        = \#D69E2E
BRAND_DARK        = \#2D3748
BRAND_LIGHT       = \#F7FAFC

# Part discovery — extracts Part IDs from directory names.
# Supports Part-0, non-sequential numbering, letter or number IDs.
PART_DIRS  := $(shell ls -d $(BOOK_DIR)/B-content/Part-*/ 2>/dev/null | sed 's|/$$||' | sort)
PART_COUNT := $(words $(PART_DIRS))
PART_IDS   := $(shell for d in $(PART_DIRS); do basename "$$d" | sed 's/Part-\([^-]*\)-.*/\1/'; done)


# =============================================================================
# RESOLUTION ENGINE — Unified 3-mode path resolution
# =============================================================================
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
# =============================================================================

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


# =============================================================================
# DEPENDENCY CHECK
# =============================================================================

define check_dep
	@command -v $(ADOC_PDF) >/dev/null 2>&1 || \
	    { echo "Error [E2]: $(ADOC_PDF) not found."; \
	      echo "  Run 'make setup' to install dependencies."; exit 2; }
endef


# =============================================================================
# CACHE ENGINE — Skip rebuilds when sources haven't changed
# =============================================================================
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
# =============================================================================

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
# EDITORIAL TARGETS — Scoped PDF builds for page-by-page review
# =============================================================================
# Each preview file is self-contained (includes _shared-attributes.adoc).
# No extra flags needed — theme, fonts, and attributes are baked in.
# =============================================================================

# =============================================================================
# ASSET GENERATION — Derived visual assets from source files
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


# Default: build full book PDF
all: book

# --- Full Book ---------------------------------------------------------------

# Full editorial build: validate → scopes (cascade to articles/sections) →
# asset docs → master → status
#
# Cascade: scope → articles → sections (each parent builds its children first)
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
	          echo "Error [E3]: Build failed for asset doc '$$f'."; continue; }; \
	    echo "Built: $$OUTPATH/$$FNAME.pdf"; \
	done
	@echo ""
	@echo "--- Building master PDF ---"
	@echo ""
	@$(MAKE) --no-print-directory create FORCE=$(FORCE) || exit $$?
	@echo ""
	@$(MAKE) --no-print-directory status

# Master PDF only (used by book target and standalone)
create:
	$(check_dep)
	@test -f "$(BOOK_DIR)/book.adoc" || \
	    { echo "Error [E2]: $(BOOK_DIR)/book.adoc not found."; exit 2; }
	@CACHE_OUT="$(OUTDIR)/CWS-Company-Identity-Manual.pdf"; \
	CACHE_SRCS="$(BOOK_DIR)/"; \
	$(call check_cache); \
	if [ "$$CACHE_HIT" = "1" ]; then exit 0; fi; \
	mkdir -p $(OUTDIR); \
	$(ADOC_PDF) $(BOOK_DIR)/book.adoc -o $(OUTDIR)/CWS-Company-Identity-Manual.pdf 2>&1 || \
	    { rm -f "$(OUTDIR)/CWS-Company-Identity-Manual.pdf"; \
	      echo "Error [E3]: Full book build failed."; exit 3; }; \
	echo "Built: $(OUTDIR)/CWS-Company-Identity-Manual.pdf"

# --- Spine Targets (Letter-Based) -------------------------------------------
# Maps directly to the book spine:
#   A = Frontmatter    C = Appendices
#   B = Content        D = Backmatter

A: frontmatter
B: content
C: appendices
D: backmatter

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

frontmatter:
	$(check_dep)
	@for f in $$(ls $(BOOK_DIR)/A-frontmatter/*.adoc 2>/dev/null | sort); do \
	    FNAME=$$(basename "$$f"); \
	    $(MAKE) --no-print-directory section FILE="$$FNAME" FORCE=$(FORCE) 2>&1 || true; \
	done
	@SRC="$(PREVIEW_DIR)/A-frontmatter-preview.adoc"; \
	test -f "$$SRC" || \
	    { echo "Error [E2]: $$SRC not found."; exit 2; }; \
	CACHE_OUT="$(OUTDIR)/A-frontmatter/CWS-Frontmatter.pdf"; \
	CACHE_SRCS="$(BOOK_DIR)/A-frontmatter/ $(PREVIEW_DIR)/A-frontmatter-preview.adoc"; \
	$(call check_cache); \
	if [ "$$CACHE_HIT" = "1" ]; then exit 0; fi; \
	mkdir -p "$(OUTDIR)/A-frontmatter"; \
	$(ADOC_PDF) "$$SRC" -o "$(OUTDIR)/A-frontmatter/CWS-Frontmatter.pdf" 2>&1 || \
	    { rm -f "$(OUTDIR)/A-frontmatter/CWS-Frontmatter.pdf"; \
	      echo "Error [E3]: Frontmatter build failed."; exit 3; }; \
	echo "Built: $(OUTDIR)/A-frontmatter/CWS-Frontmatter.pdf"

part:
	@test -n "$(PART)" || \
	    { echo "Error [E1]: Missing PART argument."; \
	      echo "  Path:   make part PART=B-content/Part-1-identity"; \
	      echo "  ID:     make part PART=1"; \
	      echo "  Search: make part PART=identity"; \
	      echo "  Available IDs: $(PART_IDS)"; exit 1; }
	$(check_dep)
	@INPUT="$(PART)"; \
	$(call resolve_dir,B-content,Part-,1); \
	if [ -z "$$RESOLVED" ]; then \
	    echo "Error [E1]: Part '$(PART)' not found."; \
	    echo "  Available IDs: $(PART_IDS)"; \
	    for d in $(PART_DIRS); do echo "    $$(basename $$d)"; done; \
	    exit 1; \
	fi; \
	PID=$$(basename "$$RESOLVED" | sed 's/Part-\([^-]*\)-.*/\1/'); \
	PNAME="$(NAME)"; \
	if [ -z "$$PNAME" ]; then \
	    PNAME=$$(basename "$$RESOLVED" | sed 's/Part-[^-]*-//' | awk '{print toupper(substr($$0,1,1)) substr($$0,2)}'); \
	fi; \
	for ADIR in $$(ls -d "$$RESOLVED"/Article-*/ 2>/dev/null | sort); do \
	    ANAME=$$(basename "$$ADIR"); \
	    $(MAKE) --no-print-directory article ARTICLE="$$ANAME" FORCE=$(FORCE) 2>&1 || true; \
	done; \
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
	      echo "Error [E3]: Build failed for Part $$PID."; exit 3; }; \
	echo "Built: $(OUTDIR)/$$RELDIR/CWS-Part$$PID-$$PNAME.pdf"

# Build all content Parts — iterates actual Part IDs from spine.
content:
	@test "$(PART_COUNT)" -gt 0 || \
	    { echo "Error [E2]: No Part directories found in $(BOOK_DIR)/B-content/."; exit 2; }
	@for N in $(PART_IDS); do \
	    $(MAKE) --no-print-directory part PART=$$N || exit $$?; \
	done

appendices:
	$(check_dep)
	@for f in $$(ls $(BOOK_DIR)/C-appendices/*.adoc 2>/dev/null | sort); do \
	    FNAME=$$(basename "$$f"); \
	    $(MAKE) --no-print-directory section FILE="$$FNAME" FORCE=$(FORCE) 2>&1 || true; \
	done
	@SRC="$(PREVIEW_DIR)/C-appendices-preview.adoc"; \
	test -f "$$SRC" || \
	    { echo "Error [E2]: $$SRC not found."; exit 2; }; \
	CACHE_OUT="$(OUTDIR)/C-appendices/CWS-Appendices.pdf"; \
	CACHE_SRCS="$(BOOK_DIR)/C-appendices/ $(PREVIEW_DIR)/C-appendices-preview.adoc"; \
	$(call check_cache); \
	if [ "$$CACHE_HIT" = "1" ]; then exit 0; fi; \
	mkdir -p "$(OUTDIR)/C-appendices"; \
	$(ADOC_PDF) "$$SRC" -o "$(OUTDIR)/C-appendices/CWS-Appendices.pdf" 2>&1 || \
	    { rm -f "$(OUTDIR)/C-appendices/CWS-Appendices.pdf"; \
	      echo "Error [E3]: Appendices build failed."; exit 3; }; \
	echo "Built: $(OUTDIR)/C-appendices/CWS-Appendices.pdf"

backmatter:
	$(check_dep)
	@for f in $$(ls $(BOOK_DIR)/D-backmatter/*.adoc 2>/dev/null | sort); do \
	    FNAME=$$(basename "$$f"); \
	    $(MAKE) --no-print-directory section FILE="$$FNAME" FORCE=$(FORCE) 2>&1 || true; \
	done
	@SRC="$(PREVIEW_DIR)/D-backmatter-preview.adoc"; \
	test -f "$$SRC" || \
	    { echo "Error [E2]: $$SRC not found."; exit 2; }; \
	CACHE_OUT="$(OUTDIR)/D-backmatter/CWS-BackMatter.pdf"; \
	CACHE_SRCS="$(BOOK_DIR)/D-backmatter/ $(PREVIEW_DIR)/D-backmatter-preview.adoc"; \
	$(call check_cache); \
	if [ "$$CACHE_HIT" = "1" ]; then exit 0; fi; \
	mkdir -p "$(OUTDIR)/D-backmatter"; \
	$(ADOC_PDF) "$$SRC" -o "$(OUTDIR)/D-backmatter/CWS-BackMatter.pdf" 2>&1 || \
	    { rm -f "$(OUTDIR)/D-backmatter/CWS-BackMatter.pdf"; \
	      echo "Error [E3]: Backmatter build failed."; exit 3; }; \
	echo "Built: $(OUTDIR)/D-backmatter/CWS-BackMatter.pdf"

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
	    { echo "Error [E1]: Missing ARTICLE argument."; \
	      echo "  Path:   make article ARTICLE=B-content/Part-1-identity/Article-01-entity-overview"; \
	      echo "  ID:     make article ARTICLE=01"; \
	      echo "  Search: make article ARTICLE=entity-overview"; exit 1; }
	$(check_dep)
	@INPUT="$(ARTICLE)"; \
	$(call resolve_dir,B-content,Article-,2); \
	if [ -z "$$RESOLVED" ]; then \
	    echo "Error [E2]: Article '$(ARTICLE)' not found in $(BOOK_DIR)/B-content/."; \
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
	for f in $$(ls "$$RESOLVED"/*.adoc 2>/dev/null | sort); do \
	    FNAME=$$(basename "$$f"); \
	    $(MAKE) --no-print-directory section FILE="$$FNAME" FORCE=$(FORCE) 2>&1 || true; \
	done; \
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
	    echo "Error [E3]: Build failed for Article $$ARTID ($$ARTNAME)."; \
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
	    { echo "Error [E1]: Missing FILE argument."; \
	      echo "  Path:   make section FILE=A-frontmatter/A-COVER.adoc"; \
	      echo "  Search: make section FILE=A-COVER"; exit 1; }
	$(check_dep)
	@INPUT="$(FILE)"; \
	$(call resolve_file,.adoc); \
	if [ -z "$$RESOLVED" ] || [ ! -f "$$RESOLVED" ]; then \
	    echo "Error [E2]: '$(FILE)' not found in $(BOOK_DIR)/."; exit 2; \
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
	      echo "Error [E3]: Build failed for '$$RESOLVED'."; \
	      echo "  Check the AsciiDoc source for syntax errors."; exit 3; }; \
	echo "Built: $(OUTDIR)/$$OUTPATH/$$OUTNAME.pdf"


# =============================================================================
# BOOK TYPE VALIDATION
# =============================================================================
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
# =============================================================================

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
	    exit 4; \
	fi


# =============================================================================
# FULL BUILD TARGETS — TypeScript builder for publishing
# =============================================================================
# These use the TypeScript builder for complete multi-format output.
# For editorial review, use the targets above instead.
# =============================================================================

html: build-ts
	@cd build/builder && npm run manual:html

pdf: build-ts
	@cd build/builder && npm run manual:pdf

epub: build-ts
	@cd build/builder && npm run manual:epub

docbook: build-ts
	@cd build/builder && node dist/cli.js --format docbook

build-ts:
	@cd build/builder && npm run build 2>/dev/null || (npm install && npm run build)


# =============================================================================
# EDITORIAL DEVELOPMENT TARGETS
# =============================================================================

# Watch for changes and rebuild full book PDF (requires entr)
watch-pdf:
	@command -v entr >/dev/null 2>&1 || \
	    { echo "Error [E2]: entr not found."; \
	      echo "  Install: sudo apt install entr"; exit 2; }
	$(check_dep)
	@echo "Watching for changes... (Ctrl+C to stop)"
	@echo "  Rebuilding: $(OUTDIR)/CWS-Company-Identity-Manual.pdf"
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
	        echo "Error [E1]: '$(FILE)' not found."; exit 1; \
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
	        echo "Error [E1]: Article '$(ARTICLE)' not found."; exit 1; \
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
	        echo "Error [E1]: Part '$(PART)' not found."; exit 1; \
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
	        A|frontmatter) TARGET="$(OUTDIR)/A-frontmatter" ;; \
	        B|content)     TARGET="$(OUTDIR)/B-content" ;; \
	        C|appendices)  TARGET="$(OUTDIR)/C-appendices" ;; \
	        D|backmatter)  TARGET="$(OUTDIR)/D-backmatter" ;; \
	        assets)        TARGET="$(OUTDIR)/00-assets" ;; \
	        master)        TARGET="$(OUTDIR)/CWS-Company-Identity-Manual.pdf" ;; \
	        *) echo "Error [E1]: Unknown scope '$(SCOPE)'."; \
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
	    echo "Error [E1]: Missing argument."; \
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
	        CWS-Company-Identity-Manual.pdf) SCOPE_DIR="$(BOOK_DIR)/" ;; \
	        A-frontmatter/*) SCOPE_DIR="$(BOOK_DIR)/A-frontmatter/" ;; \
	        C-appendices/*) SCOPE_DIR="$(BOOK_DIR)/C-appendices/" ;; \
	        D-backmatter/*) SCOPE_DIR="$(BOOK_DIR)/D-backmatter/" ;; \
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
	@which gem > /dev/null || (echo "Error [E2]: Ruby gem not found. Please install Ruby first." && exit 2)
	gem install asciidoctor --user-install || gem install asciidoctor
	gem install asciidoctor-pdf --user-install || gem install asciidoctor-pdf
	gem install asciidoctor-epub3 --user-install || gem install asciidoctor-epub3
	gem install rouge --user-install || gem install rouge
	@echo "Asciidoctor setup complete"


# =============================================================================
# HELP
# =============================================================================

help:
	@echo ""
	@echo "CWS Company Identity Manual — Build System"
	@echo "==========================================="
	@echo ""
	@echo "Resolution (all scoped targets):"
	@echo "  Path mode    (has /)    Spine-relative path, resolves directly"
	@echo "  Search mode  (no /)    ID → name → fuzzy match"
	@echo "  Backward compat        Strips book/ prefix; extensions optional"
	@echo ""
	@echo "Editorial Review (PDF):"
	@echo "  book | create     Full book PDF                 → build/output/"
	@echo "  A | frontmatter   Frontmatter                   → build/output/A-frontmatter/"
	@echo "  B | content       All content Parts             → build/output/B-content/<part>/"
	@echo "  C | appendices    Appendices                    → build/output/C-appendices/"
	@echo "  D | backmatter    Back matter                   → build/output/D-backmatter/"
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
	@echo "Validation:"
	@echo "  validate                                        Base skeleton"
	@echo "  validate TYPE=manual                            Book type minimums"
	@echo "  validate TYPE=manual-l                          Legal hierarchy naming"
	@echo "  validate TYPE=manual   BONUS=operational        Full editorial"
	@echo "  validate TYPE=manual-l BONUS=operational        Full legal + editorial"
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
	@echo "Exit Codes:"
	@echo "  E0 (0)  Success           Target completed without errors"
	@echo "  E1 (1)  Usage error       Missing or invalid arguments"
	@echo "  E2 (2)  Missing dep       Required tool or source file not found"
	@echo "  E3 (3)  Build failure     asciidoctor-pdf returned an error"
	@echo "  E4 (4)  Validation fail   Structure checks found errors (validate only)"
	@echo ""
	@echo "Output: ./build/output/"
	@echo ""
