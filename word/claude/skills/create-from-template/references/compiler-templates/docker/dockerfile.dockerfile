# ═══════════════════════════════════════════════════════════════════════════
# TEMPLATE: Dockerfile for OmniCode Compiler Projects (4-Block Structure)
# Key: LANG-TEMPLATE-016
# ═══════════════════════════════════════════════════════════════════════════
#
# This is a TEMPLATE file - copy and modify for new compiler Dockerfiles.
# Replace all [bracketed] placeholders with actual content.
# Rename to Dockerfile (remove template prefix).
#
# Derived from: templates/code/docker/CODE-DOCKER-001-DOCKER-dockerfile.dockerfile
# See: standards/code/4-block/ for complete documentation
# See: https://docs.docker.com/reference/dockerfile/
#
# ═══════════════════════════════════════════════════════════════════════════

# =============================================================================
# METADATA
# =============================================================================
#
# # Biblical Foundation
#
# Scripture: "In the beginning was the Word, and the Word was with God,
# and the Word was God." - John 1:1
#
# Principle: The λόγος (logos) - divine order and reason expressed through
# language. This Dockerfile builds containers for the OmniCode compiler,
# the tool that transforms source into executable truth.
#
# Anchor: "For which of you, intending to build a tower, sitteth not down
# first, and counteth the cost?" - Luke 14:28
#
# # CPI-SI Identity
#
# Component Type: [Ladder/Baton/Rails]
#
#   - Ladder: Base compiler image others depend on
#   - Baton: Compilation container (transforms source to binary)
#   - Rails: Build infrastructure (CI/CD, testing)
#
# Role: [Specific responsibility in compiler architecture]
#
# Paradigm: CPI-SI framework component - compiler infrastructure
#
# # Authorship & Lineage
#
#   - Architect: [Who designed this container]
#   - Implementation: [Who created and maintains this file]
#   - Created: [YYYY-MM-DD]
#   - Version: [MAJOR.MINOR.PATCH]
#   - Modified: [YYYY-MM-DD - what changed]
#
# # Purpose & Function
#
# Purpose: [What does this container do for the compiler?]
#
# Core Design: [Single-stage / Multi-stage / Base image pattern]
#
# Key Features:
#
#   - [Major capability 1]
#   - [Major capability 2]
#   - [Major capability 3]
#
# # Health Scoring
#
# Container Impact: [How container failures affect compiler system health]
#
# Validation Behavior:
#   - Build failure: [What happens]
#   - Runtime failure: [What happens]
#   - Health check: [How health is verified]
#
# =============================================================================
# END METADATA
# =============================================================================

# =============================================================================
# SETUP
# =============================================================================
#
# File Identity:
#   - Key: LANG-DOCKER-###
#   - Consumer: Docker Engine (docker build)
#   - Loader: Dockerfile parser
#
# Related Files:
#   - .dockerignore - Exclude files from build context
#   - docker-compose.yaml - Service orchestration (root level)
#   - Makefile - Build automation
#   - go.mod/go.sum - Go dependencies
#
# Build Commands:
#   - Build: docker build -t [image-name] .
#   - Build with arg: docker build --build-arg GO_VERSION=1.24 -t [name] .
#   - Run: docker run --rm [image-name]
#
# Build Arguments:
#   - GO_VERSION: Go compiler version (default: 1.24)
#   - CGO_ENABLED: C interop (0=pure Go, 1=allow cgo)
#
# =============================================================================
# END SETUP
# =============================================================================

# =============================================================================
# BODY
# =============================================================================

# -----------------------------------------------------------------------------
# Base Image Selection
# -----------------------------------------------------------------------------
# For OmniCode compiler projects, typically use:
#
#   - golang:1.24-bookworm: Full Go environment for building
#   - gcr.io/distroless/static-debian12: Minimal runtime
#   - scratch: Absolute minimal (just the binary)
#
# ARG GO_VERSION=1.24
# FROM golang:${GO_VERSION}-bookworm AS builder

ARG BASE_IMAGE=[base-image]
ARG BASE_TAG=[tag]
FROM ${BASE_IMAGE}:${BASE_TAG} AS [stage-name]

# -----------------------------------------------------------------------------
# Environment Configuration
# -----------------------------------------------------------------------------
# Standard compiler build environment variables.

ENV CGO_ENABLED=[0|1]
ENV GOPROXY=https://proxy.golang.org,direct
ENV GOOS=linux
ENV GOARCH=amd64

# -----------------------------------------------------------------------------
# Working Directory
# -----------------------------------------------------------------------------

WORKDIR [/path/to/workdir]

# -----------------------------------------------------------------------------
# Dependencies Installation
# -----------------------------------------------------------------------------
# For Go projects, copy go.mod/go.sum first for layer caching.

# COPY go.mod go.sum ./
# RUN go mod download && go mod verify

# -----------------------------------------------------------------------------
# Source Code
# -----------------------------------------------------------------------------
# Copy source code after dependencies for better layer caching.
# Include internal/maps/ for embedded biblical/language data.

COPY . .

# -----------------------------------------------------------------------------
# Build Steps
# -----------------------------------------------------------------------------
# Build the compiler or tool.
#
# RUN go build -v -ldflags="-w -s" -o /app/[binary] ./cmd/[binary]

# -----------------------------------------------------------------------------
# Multi-Stage: Runtime Image (Optional)
# -----------------------------------------------------------------------------
# For minimal production images, copy only necessary artifacts.
#
# FROM gcr.io/distroless/static-debian12:nonroot AS runtime
# COPY --from=builder /app/[binary] /app/[binary]

# -----------------------------------------------------------------------------
# Runtime Configuration
# -----------------------------------------------------------------------------

# Expose ports (if applicable - e.g., LSP server)
# EXPOSE [port]

# Health check (recommended for services)
# HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
#   CMD [health-check-command]

# User (run as non-root when possible)
# USER nonroot:nonroot

# Entrypoint and default command
# ENTRYPOINT ["/app/[binary]"]
# CMD ["--help"]

# =============================================================================
# END BODY
# =============================================================================

# =============================================================================
# CLOSING
# =============================================================================
#
# -----------------------------------------------------------------------------
# Validation
# -----------------------------------------------------------------------------
#
# Verify Dockerfile is working:
#
#   # Lint Dockerfile
#   docker run --rm -i hadolint/hadolint < Dockerfile
#
#   # Build image
#   docker build -t [name]:test .
#
#   # Check image size
#   docker images [name]:test
#
#   # Test run
#   docker run --rm [name]:test --version
#
# Common Issues:
#
#   "COPY failed" - Check file exists in build context
#   "go mod download failed" - Check go.mod/go.sum are valid
#   "exec format error" - Check GOOS/GOARCH match target platform
#
# -----------------------------------------------------------------------------
# Compiler-Specific Patterns
# -----------------------------------------------------------------------------
#
# OmniCode compiler containers typically follow these patterns:
#
#   1. Builder: Compile omnic binary with embedded maps
#   2. Runtime: Minimal image with just the compiler
#   3. Development: Full environment for compiler development
#   4. Testing: Environment for running compiler tests
#
# Embedded data considerations:
#   - internal/maps/ must be included in build context
#   - Biblical texts at runtime vs build time
#   - go:embed directives require source availability
#
# -----------------------------------------------------------------------------
# Modification Policy
# -----------------------------------------------------------------------------
#
# Safe to Modify:
#   ✅ Change Go version (ARG GO_VERSION)
#   ✅ Add build flags to go build
#   ✅ Add environment variables
#   ✅ Add HEALTHCHECK directive
#
# Modify with Care:
#   ⚠️ Changing base image (affects security/size)
#   ⚠️ Adding RUN commands (each creates a layer)
#   ⚠️ Excluding internal/maps/ (breaks embedded data)
#
# NEVER Modify:
#   ❌ Remove multi-stage build (bloats image)
#   ❌ Run as root in production
#   ❌ 4-block documentation structure
#
# -----------------------------------------------------------------------------
# Closing Note
# -----------------------------------------------------------------------------
#
# This Dockerfile template builds OmniCode compiler components,
# following Kingdom Technology principles.
#
# "In the beginning was the Word" - John 1:1
#
# =============================================================================
# END CLOSING
# =============================================================================
