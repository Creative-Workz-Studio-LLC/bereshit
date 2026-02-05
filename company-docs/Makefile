# =============================================================================
# CWS Company Identity Manual - Build System
# =============================================================================
#
# Build the Company Identity Manual in various formats.
#
# Prerequisites:
#   - Node.js >= 18
#   - Ruby with gem (for Asciidoctor)
#   - Asciidoctor tools (installed via setup)
#
# Quick Start:
#   make setup    # Install all dependencies
#   make html     # Build HTML version
#   make pdf      # Build PDF version
#   make all      # Build all formats
#
# =============================================================================

.PHONY: all html pdf epub docbook clean setup setup-node setup-asciidoctor \
        watch info check help build-ts preview preview-html \
        cover single

# Default target
all: build-ts
	@cd builder && npm run manual:all

# Individual format targets
html: build-ts
	@cd builder && npm run manual:html

pdf: build-ts
	@cd builder && npm run manual:pdf

epub: build-ts
	@cd builder && npm run manual:epub

docbook: build-ts
	@cd builder && node dist/cli.js --format docbook

# =============================================================================
# PREVIEW TARGETS — Direct asciidoctor builds (no TypeScript builder needed)
# =============================================================================

# Frontmatter preview PDF
preview:
	@mkdir -p output
	asciidoctor-pdf frontmatter-preview.adoc -o output/CWS-Frontmatter-Preview.pdf
	@echo "Built: output/CWS-Frontmatter-Preview.pdf"

# Frontmatter preview HTML
preview-html:
	@mkdir -p output
	asciidoctor frontmatter-preview.adoc -o output/CWS-Frontmatter-Preview.html
	@echo "Built: output/CWS-Frontmatter-Preview.html"

# Single section preview — build one frontmatter file in isolation
# Usage: make cover
cover:
	@mkdir -p output
	asciidoctor-pdf frontmatter/00-COVER.adoc -o output/CWS-Cover-Only.pdf
	@echo "Built: output/CWS-Cover-Only.pdf"

# General single-file build — any .adoc file to PDF
# Usage: make single FILE=frontmatter/00-COVER.adoc
single:
	@mkdir -p output
	@test -n "$(FILE)" || (echo "Usage: make single FILE=path/to/file.adoc" && exit 1)
	asciidoctor-pdf $(FILE) -o output/$$(basename $(FILE) .adoc).pdf
	@echo "Built: output/$$(basename $(FILE) .adoc).pdf"

# =============================================================================
# BUILDER TARGETS — TypeScript builder for full manual
# =============================================================================

# Build TypeScript
build-ts:
	@cd builder && npm run build 2>/dev/null || (npm install && npm run build)

# Watch mode (rebuilds HTML on file changes)
watch: build-ts
	@cd builder && npm run manual:watch

# Show output file info
info: build-ts
	@cd builder && node dist/cli.js --info

# Check prerequisites
check: build-ts
	@cd builder && node dist/cli.js --check

# Clean output directory
clean: build-ts
	@cd builder && npm run clean
	@rm -rf output/
	@echo "Output directory cleaned"

# Full setup - install all dependencies
setup: setup-node setup-asciidoctor
	@echo ""
	@echo "=============================================="
	@echo "  Setup complete!"
	@echo "  Run 'make html' to build the HTML manual"
	@echo "  Run 'make all' to build all formats"
	@echo "=============================================="

# Setup Node.js dependencies
setup-node:
	@echo "Installing Node.js dependencies..."
	@cd builder && npm install
	@cd builder && npm run build
	@echo "Node.js setup complete"

# Setup Asciidoctor tools
setup-asciidoctor:
	@echo ""
	@echo "Installing Asciidoctor tools..."
	@echo "This requires Ruby with gem to be installed."
	@echo ""
	@which gem > /dev/null || (echo "Error: Ruby gem not found. Please install Ruby first." && exit 1)
	gem install asciidoctor --user-install || gem install asciidoctor
	gem install asciidoctor-pdf --user-install || gem install asciidoctor-pdf
	gem install asciidoctor-epub3 --user-install || gem install asciidoctor-epub3
	gem install rouge --user-install || gem install rouge
	@echo "Asciidoctor setup complete"

# Help target
help:
	@echo ""
	@echo "CWS Company Identity Manual - Build System"
	@echo "==========================================="
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Preview (Direct Build):"
	@echo "  preview       Build frontmatter preview PDF    → output/"
	@echo "  preview-html  Build frontmatter preview HTML   → output/"
	@echo "  cover         Build cover section only (PDF)   → output/"
	@echo "  single        Build any .adoc file (PDF)       → output/"
	@echo "                  Usage: make single FILE=frontmatter/00-COVER.adoc"
	@echo ""
	@echo "Full Manual (TypeScript Builder):"
	@echo "  all         Build all formats (HTML, PDF, EPUB, DocBook)"
	@echo "  html        Build HTML format only"
	@echo "  pdf         Build PDF format only"
	@echo "  epub        Build EPUB format only"
	@echo "  docbook     Build DocBook XML format only"
	@echo ""
	@echo "Development:"
	@echo "  watch       Watch for changes and rebuild HTML"
	@echo "  clean       Remove all generated files"
	@echo "  info        Show output file information"
	@echo "  check       Check if prerequisites are installed"
	@echo ""
	@echo "Setup:"
	@echo "  setup       Install all dependencies (Node.js + Asciidoctor)"
	@echo "  setup-node  Install Node.js dependencies only"
	@echo "  setup-asciidoctor  Install Asciidoctor tools only"
	@echo ""
	@echo "Output files are written to: ./output/"
	@echo ""
