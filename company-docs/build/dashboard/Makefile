# ── CWS Dashboard Makefile ──────────────────────────────────────────
# Build, install, and manage the CWS Manual Builder Dashboard.
#
# Targets:
#   make dev         — Start Express dev server (browser mode)
#   make dev-tauri   — Start Tauri dev mode (native window)
#   make build       — Build TypeScript frontend
#   make build-tauri — Build native Tauri binary
#   make icons       — Regenerate PNG icons from SVG
#   make install     — Install desktop entry + icons system-wide
#   make uninstall   — Remove desktop entry + icons
#   make clean       — Remove build artifacts
#   make help        — Show this help
# ────────────────────────────────────────────────────────────────────

SHELL     := /bin/bash
.DEFAULT_GOAL := help

# ── Paths ───────────────────────────────────────────────────────────

ROOT      := $(shell pwd)
TAURI_DIR := $(ROOT)/src-tauri
ICON_DIR  := $(TAURI_DIR)/icons
SVG_SRC   := $(ICON_DIR)/cws-dashboard.svg
APP_ID    := com.creativeworkzstudio.dashboard

# XDG directories
PREFIX    ?= /usr/local
BINDIR    := $(PREFIX)/bin
DATADIR   := $(PREFIX)/share
ICONDIR   := $(DATADIR)/icons/hicolor
APPDIR    := $(DATADIR)/applications

# Icon sizes to generate
SIZES     := 16 24 32 48 64 128 256 512

# Tauri build target (exfat workaround)
CARGO_TARGET := /home/seanje-lenox-wise/.cargo-targets/cws-dashboard
TAURI_BIN    := $(CARGO_TARGET)/release/cws-dashboard

# ── Development ─────────────────────────────────────────────────────

.PHONY: frontend
frontend: ## Compile TypeScript frontend to JS (in-place)
	npx tsc -p tsconfig.web.json

# CWS Server binary (built from Bereshit/server/)
CWS_SERVER := /home/seanje-lenox-wise/.local/bin/cws-server

.PHONY: dev
dev: frontend ## Start CWS Server (http://localhost:3847)
	$(CWS_SERVER) --dev --port 3847 --builder-dir $(realpath $(ROOT)/../..)

.PHONY: dev-express
dev-express: frontend ## Start legacy Express dev server
	node --loader ts-node/esm server/server.ts

.PHONY: dev-tauri
dev-tauri: ## Start Tauri dev mode (native window, auto-compiles frontend)
	cd $(TAURI_DIR) && cargo tauri dev

# ── Build ───────────────────────────────────────────────────────────

.PHONY: build
build: frontend build-tauri ## Build frontend + native Tauri binary

.PHONY: build-tauri
build-tauri: frontend ## Build native Tauri binary (.deb + .appimage)
	cd $(TAURI_DIR) && cargo tauri build

.PHONY: build-release
build-release: frontend ## Build optimized Rust binary only (no packaging)
	cd $(TAURI_DIR) && cargo build --release --target-dir $(CARGO_TARGET)

.PHONY: typecheck
typecheck: ## Type-check all TypeScript without emitting
	npx tsc --noEmit

# ── Icons ───────────────────────────────────────────────────────────

ICON_PNGS := $(foreach s,$(SIZES),$(ICON_DIR)/$(s)x$(s).png)

.PHONY: icons
icons: $(ICON_PNGS) $(ICON_DIR)/icon.png ## Regenerate PNG icons from SVG

$(ICON_DIR)/icon.png: $(ICON_DIR)/32x32.png
	convert $< -define png:color-type=6 PNG32:$@

$(ICON_DIR)/%x%.png: $(SVG_SRC)
	inkscape $< --export-type=png --export-filename=$@ \
		--export-width=$(firstword $(subst x, ,$*)) \
		--export-height=$(firstword $(subst x, ,$*)) 2>/dev/null

# ── Install / Uninstall ────────────────────────────────────────────

.PHONY: install
install: ## Install desktop entry + icons (needs elevated permissions)
	@echo "Installing CWS Dashboard..."
	@# Desktop entry
	@sed 's|@BINDIR@|$(BINDIR)|g' $(APP_ID).desktop.in > /tmp/$(APP_ID).desktop
	install -Dm644 /tmp/$(APP_ID).desktop $(APPDIR)/$(APP_ID).desktop
	@rm -f /tmp/$(APP_ID).desktop
	@# Icons at each size
	@for size in $(SIZES); do \
		install -Dm644 $(ICON_DIR)/$${size}x$${size}.png \
			$(ICONDIR)/$${size}x$${size}/apps/$(APP_ID).png; \
	done
	@# SVG for scalable
	install -Dm644 $(SVG_SRC) $(ICONDIR)/scalable/apps/$(APP_ID).svg
	@# Binary (if built)
	@if [ -f "$(TAURI_BIN)" ]; then \
		install -Dm755 $(TAURI_BIN) $(BINDIR)/cws-dashboard; \
		echo "Binary installed to $(BINDIR)/cws-dashboard"; \
	else \
		echo "Note: No binary found at $(TAURI_BIN)"; \
		echo "      Run 'make build-tauri' first, then 'make install' again."; \
	fi
	@# Update icon cache
	-gtk-update-icon-cache -f -t $(ICONDIR) 2>/dev/null || true
	@echo "Done. You may need to log out/in for the icon to appear."

.PHONY: uninstall
uninstall: ## Remove desktop entry + icons
	@echo "Uninstalling CWS Dashboard..."
	rm -f $(APPDIR)/$(APP_ID).desktop
	rm -f $(BINDIR)/cws-dashboard
	@for size in $(SIZES); do \
		rm -f $(ICONDIR)/$${size}x$${size}/apps/$(APP_ID).png; \
	done
	rm -f $(ICONDIR)/scalable/apps/$(APP_ID).svg
	-gtk-update-icon-cache -f -t $(ICONDIR) 2>/dev/null || true
	@echo "Done."

# ── Clean ───────────────────────────────────────────────────────────

.PHONY: clean
clean: ## Remove build artifacts
	rm -rf dist/
	rm -rf server/dist/
	find src-web -name '*.js' -o -name '*.js.map' | xargs rm -f 2>/dev/null || true
	cd $(TAURI_DIR) && cargo clean --target-dir $(CARGO_TARGET) 2>/dev/null || true

# ── Help ────────────────────────────────────────────────────────────

.PHONY: help
help: ## Show available targets
	@echo "CWS Dashboard — Build Targets"
	@echo "──────────────────────────────────────────────"
	@grep -E '^[a-zA-Z_-]+:.*##' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*##"}; {printf "  \033[36m%-16s\033[0m %s\n", $$1, $$2}'
	@echo ""
	@echo "Examples:"
	@echo "  make dev          # Start web server on :3847"
	@echo "  make build-tauri  # Build native binary"
	@echo "  make icons        # Regenerate all icon sizes"
	@echo "  make install      # Install desktop entry + icons"
