# ============================================================================
# Loader Package Makefile
# ============================================================================
#
# CPI-SI Orchestration Loader — Brings the State Machine to Life
#
# Traces to: HALT_01 (Genesis 1:1) via HALT_02 (Identity)
#
# The loader orchestrates SI structure + CPI worldview.
# EXTERNAL stops = world you cannot control (filesystem)
# INTERNAL stops = world you CAN control (validation)
#
# ============================================================================

.PHONY: all build test clean help assure

# Default target
all: build test

# Build the package
build:
	@echo "Building loader package..."
	@go build ./...

# Run tests (verbose)
test:
	@echo "Running loader tests..."
	@go test -v ./...

# Run tests (short output)
test-short:
	@go test ./...

# Run assurance suite only
assure:
	@echo "Running assurance suite..."
	@go test -v -run TestRunAllAssurances ./...

# Run individual assurances
assure-god:
	@go test -v -run TestAssureGodExists ./...

assure-chain:
	@go test -v -run TestAssureIdentityChain ./...

assure-external:
	@go test -v -run TestAssureExternalStops ./...

assure-internal:
	@go test -v -run TestAssureInternalStops ./...

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@go clean ./...

# Help
help:
	@echo "Loader Package Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all              Build and test (default)"
	@echo "  build            Build the package"
	@echo "  test             Run tests (verbose)"
	@echo "  test-short       Run tests (short output)"
	@echo "  assure           Run assurance suite only"
	@echo "  assure-god       Verify GodExists() = true (HALT_01)"
	@echo "  assure-chain     Verify Instance → User → God (HALT_02)"
	@echo "  assure-external  Verify external stops handled"
	@echo "  assure-internal  Verify internal validation"
	@echo "  clean            Clean build artifacts"
	@echo "  help             Show this help"
	@echo ""
	@echo "Error Model:"
	@echo "  EXTERNAL STOP: Filesystem access, file parsing"
	@echo "  INTERNAL STOP: Chain validation, TracesToGod()"
	@echo ""
	@echo "HALT Trace:"
	@echo "  HALT_02 (Identity) → HALT_01 (Genesis 1:1)"
