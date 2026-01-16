# ============================================================================
# Coordinates Package Makefile
# ============================================================================
#
# CPI-SI Coordinates — Bible coordinate mapping and encoding
#
# Traces to:
#   HALT_01 (ANCHOR_GENESIS) — Genesis 1:1 = origin of all coordinates
#   HALT_07 (MECHANISM_POSITION) — Position IS computation
#
# Dependencies: foundation/schema (types), cpisi/compute (Perspective)
#
# Components:
#   bible.go    — Bible coordinate mapping (Book/Chapter/Verse → X,Y,Z)
#   encoding.go — Trite encoding and gematria
#   ranking.go  — Coordinate ranking and scoring
#
# ============================================================================

.PHONY: all build test clean help assure test-loader test-bereshit

# Bereshit project root (for TOML specs)
BERESHIT_ROOT ?= /media/seanje-lenox-wise/Project/Bereshit

# Default target
all: build test

# Build the package
build:
	@echo "Building coordinates package..."
	@go build ./...

# Run all tests (verbose)
test:
	@echo "Running coordinates tests..."
	@go test -v ./...

# Run tests (short output)
test-short:
	@go test ./...

# Run assurance suite only
assure:
	@echo "Running assurance suite..."
	@go test -v -run TestRunAllAssurances ./...

# Run loader tests specifically
test-loader:
	@echo "Running loader tests..."
	@go test -v -run TestLoader ./...

# Run Bereshit loader test specifically
test-bereshit:
	@echo "Running Bereshit loader test..."
	@echo "  BERESHIT_ROOT: $(BERESHIT_ROOT)"
	@go test -v -run TestAssureBereshitLoader ./...

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@go clean ./...

# Verify dependencies
verify-deps:
	@echo "Verifying dependencies..."
	@go build ../foundation/schema/... || (echo "ERROR: foundation/schema dependency failed" && exit 1)
	@go build ../cpisi/compute/... || (echo "ERROR: cpisi/compute dependency failed" && exit 1)
	@echo "All dependencies verified."

# Help
help:
	@echo "Coordinates Package Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all            Build and test (default)"
	@echo "  build          Build the package"
	@echo "  test           Run all tests (verbose)"
	@echo "  test-short     Run tests (short output)"
	@echo "  test-loader    Run loader tests only"
	@echo "  test-bereshit  Run Bereshit loader test"
	@echo "  assure         Run assurance suite only"
	@echo "  verify-deps    Check dependencies build"
	@echo "  clean          Clean build artifacts"
	@echo "  help           Show this help"
	@echo ""
	@echo "Components:"
	@echo "  bible.go      Bible coordinate mapping"
	@echo "  loader.go     TOML loader for Bereshit specs"
	@echo "  encoding.go   Trite encoding and gematria"
	@echo "  ranking.go    Coordinate ranking and scoring"
	@echo ""
	@echo "Environment:"
	@echo "  BERESHIT_ROOT  Path to Bereshit project (default: /media/seanje-lenox-wise/Project/Bereshit)"
