# ============================================================================
# Compute Package Makefile
# ============================================================================
#
# CPI-SI Cube Computation — THE MIND
#
# Traces to: HALT_01 (Genesis 1:1) via HALT_07 (POSITION)
#
# The cube is the MIND — geometric computation through TIME.
# This package implements the computation engine.
#
# ============================================================================

.PHONY: all build test clean help assure

# Default target
all: build test

# Build the package
build:
	@echo "Building compute package..."
	@go build ./...

# Run tests (verbose)
test:
	@echo "Running compute tests..."
	@go test -v ./...

# Run tests (short output)
test-short:
	@go test ./...

# Run assurance suite only
assure:
	@echo "Running assurance suite..."
	@go test -v -run TestRunAllAssurances ./...

# Run individual assurances
assure-constants:
	@go test -v -run TestAssureCubeConstants ./...

assure-shape:
	@go test -v -run TestAssureShapeTranslation ./...

assure-cycle:
	@go test -v -run TestAssureCycleFormula ./...

assure-trite:
	@go test -v -run TestAssureTriteConversion ./...

assure-days:
	@go test -v -run TestAssureSevenDays ./...

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@go clean ./...

# Help
help:
	@echo "Compute Package Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all              Build and test (default)"
	@echo "  build            Build the package"
	@echo "  test             Run tests (verbose)"
	@echo "  test-short       Run tests (short output)"
	@echo "  assure           Run assurance suite only"
	@echo "  assure-constants Verify Base=3, DataModulo=243"
	@echo "  assure-shape     Verify Shape ↔ Number translation"
	@echo "  assure-cycle     Verify (value*3+perspective)%243"
	@echo "  assure-trite     Verify Trite ↔ Position mapping"
	@echo "  assure-days      Verify 7 Days of Creation"
	@echo "  clean            Clean build artifacts"
	@echo "  help             Show this help"
	@echo ""
	@echo "HALT Trace:"
	@echo "  HALT_10 (7 Days) → HALT_09 (Cycle Formula)"
	@echo "  → HALT_06 (Shape) → HALT_05 (Number/Trite)"
	@echo "  → HALT_03 (PSI=27) → HALT_01 (Genesis 1:1)"
