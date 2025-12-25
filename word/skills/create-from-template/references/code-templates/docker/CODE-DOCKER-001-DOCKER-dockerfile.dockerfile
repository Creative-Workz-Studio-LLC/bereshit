# ═══════════════════════════════════════════════════════════════════════════
# TEMPLATE: Dockerfile (4-Block Structure)
# Key: CODE-DOCKER-001
# ═══════════════════════════════════════════════════════════════════════════
#
# This is a TEMPLATE file - copy and modify for new Dockerfiles.
# Replace all [bracketed] placeholders with actual content.
# Rename to Dockerfile (remove CODE-DOCKER-001-DOCKER- prefix).
#
# Derived from: Kingdom Technology standards
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
# Scripture: [Relevant verse grounding this container's purpose]
#
# Principle: [Kingdom principle this container demonstrates]
#
# Anchor: [Supporting verse reinforcing the principle]
#
# # CPI-SI Identity
#
# Component Type: [Ladder/Baton/Rails]
#
#   - Ladder: Foundation container others depend on (base images, shared deps)
#   - Baton: Processing container (compilers, transformers, builders)
#   - Rails: Infrastructure container (logging, monitoring, orchestration)
#
# Role: [Specific responsibility in system architecture]
#
# Paradigm: CPI-SI framework component - container image
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
# Purpose: [What does this container do?]
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
# Container Impact: [How container failures affect system health]
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
#   - Key: [PROJECT]-DOCKER-###
#   - Consumer: Docker Engine (docker build)
#   - Loader: Dockerfile parser
#
# Related Files:
#   - .dockerignore - Exclude files from build context
#   - docker-compose.yaml - Service orchestration
#   - [Other related Dockerfiles if multi-container]
#
# Build Commands:
#   - Build: docker build -t [image-name] .
#   - Build with args: docker build --build-arg [ARG]=[VALUE] -t [name] .
#   - Run: docker run --rm [image-name]
#
# Build Arguments:
#   - [ARG_NAME]: [Description and default]
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
# Choose base image appropriate for your use case:
#
#   - Language runtime: golang, python, node, rust
#   - Linux distro: ubuntu, debian, alpine
#   - Minimal: distroless, scratch
#   - Specialized: qemu, cross-compile toolchains
#
# ARG [VERSION]=default
# FROM [base-image]:[tag] AS [stage-name]

ARG BASE_IMAGE=[base-image]
ARG BASE_TAG=[tag]
FROM ${BASE_IMAGE}:${BASE_TAG} AS [stage-name]

# -----------------------------------------------------------------------------
# Environment Configuration
# -----------------------------------------------------------------------------
# Set environment variables for build and runtime.

ENV [ENV_VAR]=[value]

# -----------------------------------------------------------------------------
# Working Directory
# -----------------------------------------------------------------------------

WORKDIR [/path/to/workdir]

# -----------------------------------------------------------------------------
# Dependencies Installation
# -----------------------------------------------------------------------------
# Install system packages, language dependencies, etc.
# Order from least to most frequently changing for cache efficiency.

# System packages (if needed)
# RUN apt-get update && apt-get install -y --no-install-recommends \
#     [package1] \
#     [package2] \
#     && rm -rf /var/lib/apt/lists/*

# Language-specific dependencies
# COPY [dependency-file] ./
# RUN [install-command]

# -----------------------------------------------------------------------------
# Source Code
# -----------------------------------------------------------------------------
# Copy source code after dependencies for better layer caching.

COPY . .

# -----------------------------------------------------------------------------
# Build Steps
# -----------------------------------------------------------------------------
# Compile, transform, or prepare the application.

# RUN [build-command]

# -----------------------------------------------------------------------------
# Multi-Stage: Runtime Image (Optional)
# -----------------------------------------------------------------------------
# For minimal production images, copy only necessary artifacts.
#
# FROM [minimal-base] AS runtime
# COPY --from=[build-stage] [source] [dest]

# -----------------------------------------------------------------------------
# Runtime Configuration
# -----------------------------------------------------------------------------

# Expose ports (if applicable)
# EXPOSE [port]

# Health check (recommended for services)
# HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
#   CMD [health-check-command]

# User (run as non-root when possible)
# USER [username]

# Entrypoint and default command
# ENTRYPOINT ["[executable]"]
# CMD ["[default-args]"]

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
#   docker run --rm [name]:test [test-command]
#
# Common Issues:
#
#   "COPY failed" - Check file exists in build context
#   "permission denied" - Check USER directive and file permissions
#   "exec format error" - Check architecture matches target
#
# -----------------------------------------------------------------------------
# Image Patterns
# -----------------------------------------------------------------------------
#
# Choose the pattern that fits your use case:
#
#   1. Single-stage: Simple apps, scripts, tools
#   2. Multi-stage: Compiled languages, minimal production images
#   3. Base image: Shared dependencies for multiple containers
#   4. Builder pattern: Complex builds with multiple outputs
#
# -----------------------------------------------------------------------------
# Modification Policy
# -----------------------------------------------------------------------------
#
# Safe to Modify:
#   ✅ Change base image version
#   ✅ Add environment variables
#   ✅ Add build arguments
#   ✅ Change exposed ports
#   ✅ Add health checks
#
# Modify with Care:
#   ⚠️ Changing base image (affects security/compatibility)
#   ⚠️ Adding RUN commands (each creates a layer)
#   ⚠️ Changing COPY order (affects caching)
#
# NEVER Modify:
#   ❌ 4-block documentation structure
#
# -----------------------------------------------------------------------------
# Closing Note
# -----------------------------------------------------------------------------
#
# This Dockerfile defines [describe what it builds/runs].
# Part of Kingdom Technology infrastructure.
#
# "[Relevant Scripture verse]" - [Reference]
#
# =============================================================================
# END CLOSING
# =============================================================================
