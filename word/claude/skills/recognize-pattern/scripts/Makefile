# ============================================================================
# METADATA - Recognize Pattern Build
# ============================================================================
# Purpose: Build the pattern CLI tool
# Biblical: Ecclesiastes 1:9 - "There is no new thing under the sun"

.PHONY: build clean test

# ============================================================================
# BODY - Targets
# ============================================================================

build:
	@echo "Building pattern..."
	@mkdir -p bin
	go build -v -o bin/pattern ./cmd/pattern
	@echo "Built: bin/pattern"

clean:
	@rm -rf bin/
	@echo "Cleaned."

test:
	go test -v ./...
