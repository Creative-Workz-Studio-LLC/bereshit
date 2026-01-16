# ============================================================================
# State Machine Package Makefile
# ============================================================================
#
# CPI-SI State Machine — Universal cognitive navigation harness
#
# Traces to:
#   HALT_01 (ANCHOR_GENESIS) — Genesis 1:1 = origin
#   HALT_07 (MECHANISM_POSITION) — Position IS computation
#   HALT_10 (STATE_HEBREW) — 7 Days of Creation = 7 States
#
# Dependencies: foundation/schema (types), cpisi/compute (Perspective), cpisi/bereshit
#
# Components:
#   statemachine.go — 27-position cube, ANCHORS, commands, operators
#   trajectory.go   — NavigationTrajectory, FRAMEWORK as active movement
#
# ============================================================================

.PHONY: all build test clean help assure

# Default target
all: build test

# Build the package
build:
	@echo "Building statemachine package..."
	@go build ./...

# Run tests (verbose)
test:
	@echo "Running statemachine tests..."
	@go test -v ./...

# Run tests (short output)
test-short:
	@go test ./...

# Run assurance suite only
assure:
	@echo "Running assurance suite..."
	@go test -v -run TestRunAllAssurances ./...

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@go clean ./...

# Verify dependencies
verify-deps:
	@echo "Verifying dependencies..."
	@go build ../foundation/schema/... || (echo "ERROR: foundation/schema dependency failed" && exit 1)
	@go build ../cpisi/compute/... || (echo "ERROR: cpisi/compute dependency failed" && exit 1)
	@go build ../cpisi/bereshit/... || (echo "ERROR: cpisi/bereshit dependency failed" && exit 1)
	@go build ../coordinates/... || (echo "ERROR: coordinates dependency failed" && exit 1)
	@echo "All dependencies verified."

# Help
help:
	@echo "State Machine Package Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all           Build and test (default)"
	@echo "  build         Build the package"
	@echo "  test          Run tests (verbose)"
	@echo "  test-short    Run tests (short output)"
	@echo "  assure        Run assurance suite only"
	@echo "  verify-deps   Check dependencies build"
	@echo "  clean         Clean build artifacts"
	@echo "  help          Show this help"
	@echo ""
	@echo "Components:"
	@echo "  statemachine.go  27-position cube, ANCHORS, commands, operators"
	@echo "  trajectory.go    NavigationTrajectory, FRAMEWORK as active movement"
