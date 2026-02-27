# ============================================================================
# METADATA
# ============================================================================
#
# Key: cws-gui-makefile
# Purpose: Build, install, and desktop integration for CWS Universal Base GUI
# Biblical: Proverbs 24:27 - "Prepare thy work without, and make it fit
#           for thyself in the field; and afterwards build thine house."
#
# ============================================================================
# END METADATA

# ============================================================================
# SETUP
# ============================================================================

BINARY   := cws-gui
BINDIR   := bin
PKG      := ./cmd/cws-gui
GOFLAGS  := -v

# Application ID (reverse DNS)
APPID    := com.creativeworkzstudio.cws.gui

# Install paths (overridable)
PREFIX   ?= /usr/local
DESTDIR  ?=

# User-local paths (no root needed)
USER_BINDIR    := $(HOME)/.local/bin
USER_DATADIR   := $(HOME)/.local/share
USER_APPDIR    := $(USER_DATADIR)/applications
USER_ICONDIR   := $(USER_DATADIR)/icons/hicolor/scalable/apps
USER_METADIR   := $(USER_DATADIR)/metainfo

# System-wide paths
INSTALL_BINDIR     := $(DESTDIR)$(PREFIX)/bin
INSTALL_DATADIR    := $(DESTDIR)$(PREFIX)/share
INSTALL_APPDIR     := $(INSTALL_DATADIR)/applications
INSTALL_ICONDIR    := $(INSTALL_DATADIR)/icons/hicolor/scalable/apps
INSTALL_METADIR    := $(INSTALL_DATADIR)/metainfo

# CGo required for GTK4 bindings
export CGO_ENABLED := 1

# Disable workspace for standalone builds
export GOWORK := off

# ============================================================================
# END SETUP

# ============================================================================
# BODY
# ============================================================================

.PHONY: all build run clean vet tidy
.PHONY: desktop install install-user uninstall uninstall-user check-install
.PHONY: help

# --- Build ---

## all: Build the application (default)
all: build

## build: Compile the binary
build:
	@mkdir -p $(BINDIR)
	go build $(GOFLAGS) -o $(BINDIR)/$(BINARY) $(PKG)
	@chmod +x $(BINDIR)/$(BINARY) 2>/dev/null || true

## run: Build and launch (copies to /tmp for exfat compatibility)
## Override substrate with: make run SUBSTRATE=claude
SUBSTRATE ?= gemini
run: build
	@cp $(BINDIR)/$(BINARY) /tmp/$(BINARY) && chmod +x /tmp/$(BINARY) && /tmp/$(BINARY) --substrate=$(SUBSTRATE)

## vet: Run go vet
vet:
	go vet ./...

## tidy: Run go mod tidy
tidy:
	go mod tidy

## clean: Remove build artifacts and generated files
clean:
	rm -rf $(BINDIR)
	rm -f data/$(APPID).desktop

# --- Desktop Integration ---

## desktop: Generate .desktop file from template (for system install)
desktop:
	@sed 's|@BINDIR@|$(PREFIX)/bin|g' data/$(APPID).desktop.in > data/$(APPID).desktop
	@echo "Generated data/$(APPID).desktop (Exec=$(PREFIX)/bin/$(BINARY))"

## desktop-user: Generate .desktop file pointing to ~/.local/bin
desktop-user:
	@sed 's|@BINDIR@|$(USER_BINDIR)|g' data/$(APPID).desktop.in > data/$(APPID).desktop
	@echo "Generated data/$(APPID).desktop (Exec=$(USER_BINDIR)/$(BINARY))"

# --- User Install (recommended — no root needed) ---

## install-user: Install to ~/.local (no root needed, appears in app launcher)
install-user: build desktop-user
	@echo "Installing CWS Universal Base to ~/.local..."
	@mkdir -p $(USER_BINDIR) $(USER_APPDIR) $(USER_ICONDIR) $(USER_METADIR)
	cp $(BINDIR)/$(BINARY) $(USER_BINDIR)/$(BINARY)
	chmod +x $(USER_BINDIR)/$(BINARY)
	cp data/$(APPID).desktop $(USER_APPDIR)/$(APPID).desktop
	cp data/icons/$(APPID).svg $(USER_ICONDIR)/$(APPID).svg
	cp data/$(APPID).metainfo.xml $(USER_METADIR)/$(APPID).metainfo.xml
	@echo ""
	-update-desktop-database $(USER_APPDIR) 2>/dev/null || true
	-gtk-update-icon-cache -f -t $(USER_DATADIR)/icons/hicolor 2>/dev/null || true
	@echo "Done. Search 'CWS' in your app launcher."
	@echo "Or run: $(BINARY)"

## uninstall-user: Remove user-local install
uninstall-user:
	@echo "Removing CWS Universal Base from ~/.local..."
	rm -f $(USER_BINDIR)/$(BINARY)
	rm -f $(USER_APPDIR)/$(APPID).desktop
	rm -f $(USER_ICONDIR)/$(APPID).svg
	rm -f $(USER_METADIR)/$(APPID).metainfo.xml
	-update-desktop-database $(USER_APPDIR) 2>/dev/null || true
	@echo "Uninstalled."

# --- System Install (requires pkexec) ---

## install: Install system-wide to /usr/local (needs pkexec)
install: build desktop
	@echo "Installing CWS Universal Base to $(PREFIX)..."
	install -Dm755 $(BINDIR)/$(BINARY) $(INSTALL_BINDIR)/$(BINARY)
	install -Dm644 data/$(APPID).desktop $(INSTALL_APPDIR)/$(APPID).desktop
	install -Dm644 data/icons/$(APPID).svg $(INSTALL_ICONDIR)/$(APPID).svg
	install -Dm644 data/$(APPID).metainfo.xml $(INSTALL_METADIR)/$(APPID).metainfo.xml
	@echo ""
	-update-desktop-database $(INSTALL_APPDIR) 2>/dev/null || true
	-gtk-update-icon-cache -f -t $(INSTALL_DATADIR)/icons/hicolor 2>/dev/null || true
	@echo "Done. Launch with: $(BINARY)"

## uninstall: Remove system-wide install
uninstall:
	@echo "Removing CWS Universal Base from $(PREFIX)..."
	rm -f $(INSTALL_BINDIR)/$(BINARY)
	rm -f $(INSTALL_APPDIR)/$(APPID).desktop
	rm -f $(INSTALL_ICONDIR)/$(APPID).svg
	rm -f $(INSTALL_METADIR)/$(APPID).metainfo.xml
	-update-desktop-database $(INSTALL_APPDIR) 2>/dev/null || true
	@echo "Uninstalled."

## check-install: Verify installation (checks both user and system)
check-install:
	@echo "Checking CWS Universal Base installation..."
	@echo ""
	@echo "User (~/.local):"
	@printf "  Binary:    "; test -x $(USER_BINDIR)/$(BINARY) && echo "OK ($(USER_BINDIR)/$(BINARY))" || echo "not installed"
	@printf "  Desktop:   "; test -f $(USER_APPDIR)/$(APPID).desktop && echo "OK" || echo "not installed"
	@printf "  Icon:      "; test -f $(USER_ICONDIR)/$(APPID).svg && echo "OK" || echo "not installed"
	@echo ""
	@echo "System ($(PREFIX)):"
	@printf "  Binary:    "; test -x $(INSTALL_BINDIR)/$(BINARY) && echo "OK ($(INSTALL_BINDIR)/$(BINARY))" || echo "not installed"
	@printf "  Desktop:   "; test -f $(INSTALL_APPDIR)/$(APPID).desktop && echo "OK" || echo "not installed"
	@printf "  Icon:      "; test -f $(INSTALL_ICONDIR)/$(APPID).svg && echo "OK" || echo "not installed"
	@echo ""
	@printf "  Version:   "; which $(BINARY) >/dev/null 2>&1 && $(BINARY) --version 2>/dev/null | head -1 || echo "not in PATH"

# ============================================================================
# END BODY

# ============================================================================
# CLOSING
# ============================================================================

## help: Show available targets
help:
	@echo "CWS Universal Base GUI"
	@echo ""
	@echo "Build:"
	@echo "  make              Build the binary"
	@echo "  make run          Build and launch"
	@echo "  make vet          Run go vet"
	@echo "  make tidy         Run go mod tidy"
	@echo "  make clean        Remove build artifacts"
	@echo ""
	@echo "Install (recommended):"
	@echo "  make install-user     Install to ~/.local (no root, app launcher)"
	@echo "  make uninstall-user   Remove from ~/.local"
	@echo ""
	@echo "Install (system-wide):"
	@echo "  pkexec make install   Install to $(PREFIX)"
	@echo "  pkexec make uninstall Remove from $(PREFIX)"
	@echo ""
	@echo "Verify:"
	@echo "  make check-install    Check both user and system installs"
	@echo ""
	@echo "Options:"
	@echo "  PREFIX=/path    System install prefix (default: /usr/local)"
	@echo "  DESTDIR=/path   Staging directory for packaging"

# ============================================================================
# END CLOSING
