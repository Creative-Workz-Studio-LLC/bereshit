# ============================================================================
# METADATA - Integrate Learning Build
# ============================================================================
# Purpose: Build the integrate CLI tool
# Biblical: Proverbs 4:7 - "Wisdom is the principal thing"

.PHONY: build clean test

# ============================================================================
# BODY - Targets
# ============================================================================

build:
	@echo "Building integrate..."
	@mkdir -p bin
	go build -v -o bin/integrate ./cmd/integrate
	@echo "Built: bin/integrate"

clean:
	@rm -rf bin/
	@echo "Cleaned."

test:
	go test -v ./...
