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
#   make validate TYPE=manual BONUS=operational  Full editorial validation
#
# Exit codes:
#   0  Success
#   1  Usage error (missing or invalid arguments)
#   2  Missing dependency (asciidoctor-pdf, preview file, source file)
#   3  Build failure (asciidoctor-pdf returned an error)
#
# "Write the vision, and make it plain upon tables,
#  that he may run that readeth it." — Habakkuk 2:2
#
# =============================================================================

.PHONY: all book create \
        A B C D \
        frontmatter part content appendices backmatter \
        article section \
        validate \
        html pdf epub docbook \
        watch info check clean setup setup-node setup-asciidoctor \
        build-ts help


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
# EDITORIAL TARGETS — Scoped PDF builds for page-by-page review
# =============================================================================
# Each preview file is self-contained (includes _shared-attributes.adoc).
# No extra flags needed — theme, fonts, and attributes are baked in.
# =============================================================================

# Default: build full book PDF
all: book

# --- Full Book ---------------------------------------------------------------

book: create
create:
	$(check_dep)
	@test -f "$(BOOK_DIR)/book.adoc" || \
	    { echo "Error [E2]: $(BOOK_DIR)/book.adoc not found."; exit 2; }
	@mkdir -p $(OUTDIR)
	@$(ADOC_PDF) $(BOOK_DIR)/book.adoc -o $(OUTDIR)/CWS-Company-Identity-Manual.pdf 2>&1 || \
	    { rm -f "$(OUTDIR)/CWS-Company-Identity-Manual.pdf"; \
	      echo "Error [E3]: Full book build failed."; exit 3; }
	@echo "Built: $(OUTDIR)/CWS-Company-Identity-Manual.pdf"

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
	@SRC="$(PREVIEW_DIR)/frontmatter-preview.adoc"; \
	test -f "$$SRC" || \
	    { echo "Error [E2]: $$SRC not found."; exit 2; }; \
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
	RELDIR=$$(echo "$$RESOLVED" | sed 's|^$(BOOK_DIR)/||'); \
	SRC="$(PREVIEW_DIR)/part$$PID-preview.adoc"; \
	if [ ! -f "$$SRC" ]; then \
	    echo "Warning: Preview $$SRC not found — cannot build Part $$PID at part level."; \
	    echo "  (Articles/sections within this Part can still be built individually.)"; exit 0; \
	fi; \
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
	@SRC="$(PREVIEW_DIR)/appendices-preview.adoc"; \
	test -f "$$SRC" || \
	    { echo "Error [E2]: $$SRC not found."; exit 2; }; \
	mkdir -p "$(OUTDIR)/C-appendices"; \
	$(ADOC_PDF) "$$SRC" -o "$(OUTDIR)/C-appendices/CWS-Appendices.pdf" 2>&1 || \
	    { rm -f "$(OUTDIR)/C-appendices/CWS-Appendices.pdf"; \
	      echo "Error [E3]: Appendices build failed."; exit 3; }; \
	echo "Built: $(OUTDIR)/C-appendices/CWS-Appendices.pdf"

backmatter:
	$(check_dep)
	@SRC="$(PREVIEW_DIR)/backmatter-preview.adoc"; \
	test -f "$$SRC" || \
	    { echo "Error [E2]: $$SRC not found."; exit 2; }; \
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
	ARTBASE=$$(basename "$$RESOLVED"); \
	ARTID=$$(echo "$$ARTBASE" | sed 's/Article-\([^-]*\)-.*/\1/'); \
	ARTNAME=$$(echo "$$ARTBASE" | sed 's/Article-[^-]*-//' | tr '-' ' '); \
	PARTPATH=$$(dirname "$$RESOLVED" | sed 's|^$(BOOK_DIR)/||'); \
	OUTPATH="$(OUTDIR)/$$PARTPATH/$$ARTBASE"; \
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
#   make validate TYPE=manual BONUS=operational  Full operational manual checks
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
	            PREV="$(PREVIEW_DIR)/part$$PID-preview.adoc"; \
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
	else \
	    echo "  $$SCOPE: INVALID ($$ERRORS errors, $$WARNINGS warnings)"; \
	fi; \
	echo ""; \
	exit $$ERRORS


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
# DEVELOPMENT TARGETS
# =============================================================================

# Watch for changes and rebuild HTML
watch: build-ts
	@cd build/builder && npm run manual:watch

# Show output file information
info: build-ts
	@cd build/builder && node dist/cli.js --info

# Check prerequisites
check: build-ts
	@cd build/builder && node dist/cli.js --check

# Clean all generated output
clean:
	@rm -rf $(OUTDIR)/
	@echo "Output directory cleaned"

# Full dependency setup
setup: setup-node setup-asciidoctor
	@echo ""
	@echo "=============================================="
	@echo "  Setup complete!"
	@echo "  Run 'make book' to build the full manual"
	@echo "  Run 'make help' to see all targets"
	@echo "=============================================="

setup-node:
	@echo "Installing Node.js dependencies..."
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
	@echo "All scoped targets support 3-mode resolution:"
	@echo "  Path mode    (has /)     Spine-relative path, resolves directly"
	@echo "  Search mode  (no /)     ID → name → fuzzy match"
	@echo "  Backward compat         Strips book/ prefix; extensions optional"
	@echo ""
	@echo "Editorial Review (PDF) — output mirrors book spine:"
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
	@echo "                      make article ARTICLE=B-content/Part-1-identity/Article-01-entity-overview"
	@echo ""
	@echo "  section FILE=     Any single .adoc file (themed)"
	@echo "                      make section FILE=A-COVER"
	@echo "                      make section FILE=A-frontmatter/A-COVER.adoc"
	@echo ""
	@echo "Validation:"
	@echo "  validate                          Base skeleton check"
	@echo "  validate TYPE=manual              Manual book type minimums"
	@echo "  validate TYPE=manual BONUS=operational  Full editorial validation"
	@echo ""
	@echo "Exit codes:  0=success  1=usage  2=missing dep/file  3=build failure"
	@echo ""
	@echo "Publishing (TypeScript Builder):"
	@echo "  html       Build HTML format"
	@echo "  pdf        Build PDF via builder"
	@echo "  epub       Build EPUB format"
	@echo "  docbook    Build DocBook XML"
	@echo ""
	@echo "Development:"
	@echo "  watch      Watch for changes, rebuild HTML"
	@echo "  clean      Remove all generated files"
	@echo "  info       Show output file information"
	@echo "  check      Check prerequisites"
	@echo "  setup      Install all dependencies"
	@echo ""
	@echo "Output: ./build/output/"
	@echo ""
