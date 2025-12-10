# syntax=docker/dockerfile:1
#!omni template --docker
# ═══════════════════════════════════════════════════════════════════════════
# TEMPLATE: Dockerfile (4-Block Structure)
# Key: B-word-seed-data-docker-dockerfile
# ═══════════════════════════════════════════════════════════════════════════
#
# TEMPLATE: This file is a template - copy and modify before using.
# Replace all [bracketed] placeholders with actual content.
# Remove the "TEMPLATE" markers when ready to build.
#
# DEPENDENCY CLASSIFICATION: [PURE/DEPENDED] ([deps if DEPENDED])
#   - PURE: Base image only - no external package requirements
#   - DEPENDED: Needs external packages - list them: (needs: curl, git, make)
#
# Derives from: B-word-omni-seed-data-4block
#               (bereshit/word/omni/seed/data-4block.omni)
# See: standards/code/4-block/ for complete documentation
# See: https://docs.docker.com/reference/dockerfile/
#
# ═══════════════════════════════════════════════════════════════════════════

# =============================================================================
# METADATA
# =============================================================================
#
# Package:     [organization/project-name]
# File:        Dockerfile
# Key:         [PROJECT-DOCKER-###] (Container image definition)
#
# -----------------------------------------------------------------------------
# CORE IDENTITY (Required)
# -----------------------------------------------------------------------------
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
#   Architect: [Who designed this container]
#   Implementation: [Who created and maintains this file]
#   Created: [YYYY-MM-DD]
#   Version: [MAJOR.MINOR.PATCH]
#   Modified: [YYYY-MM-DD - what changed]
#
# Version History:
#
#   [X.Y.Z] ([YYYY-MM-DD]) - [Brief description of changes]
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
# Philosophy: [Guiding principle for this container design]
#
# -----------------------------------------------------------------------------
# INTERFACE (Expected)
# -----------------------------------------------------------------------------
#
# # Dependencies
#
# What This Needs:
#
#   - Base Image: [base-image:tag]
#   - Build Tools: [If any build-time dependencies]
#   - Runtime: [Runtime dependencies if any]
#
# What Uses This:
#
#   - [Services/containers that use this image]
#   - [docker-compose.yaml that references this]
#
# Integration Points:
#
#   - [Volume mounts, port mappings, network connections]
#
# # Usage & Integration
#
# Build Commands:
#
#   docker build -t [image-name] .
#   docker build --build-arg [ARG]=[VALUE] -t [name] .
#
# Run Commands:
#
#   docker run --rm [image-name]
#   docker run -d -p [host]:[container] [image-name]
#
# -----------------------------------------------------------------------------
# OPERATIONAL (Contextual)
# -----------------------------------------------------------------------------
#
# # Blocking Status
#
# [OMIT: Dockerfile - build errors stop image creation, no runtime blocking concern]
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
# -----------------------------------------------------------------------------
# METADATA Omission Guide
# -----------------------------------------------------------------------------
#
# Tier 1 (CORE IDENTITY): Never omit - every file needs these.
#
# Tier 2 (INTERFACE): May omit with [OMIT: reason] notation.
#   - Dependencies: Required - documents base images and runtime dependencies
#   - Usage & Integration: Required - shows docker build/run commands
#
# Tier 3 (OPERATIONAL): Include when applicable to file type.
#   - Blocking Status: [OMIT: Dockerfile - build-time only, not runtime blocking]
#   - Health Scoring: Include if container has HEALTHCHECK directive
#
# Unlike SETUP (all sections required), METADATA omission signals component characteristics.
#
# =============================================================================
# END METADATA
# =============================================================================

# =============================================================================
# SETUP
# =============================================================================
#
# For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md
#
# -----------------------------------------------------------------------------
# SETUP Sections Overview
# -----------------------------------------------------------------------------
#
# 1. FILE IDENTITY (Dependencies)
#    Purpose: Establish what consumes this Dockerfile and how it's loaded
#    Subsections: Consumer → Loader → Build Context
#
# 2. CONSTANTS
#    [Reserved: Dockerfiles define constants inline via ENV - documented in BODY]
#
# 3. VARIABLES
#    [Reserved: ARG/ENV directives are inline with build stages in BODY]
#
# 4. RELATED FILES (Types)
#    Purpose: Document related configuration files
#    Subsections: Build Context → Orchestration → Configuration
#
# 5. TYPE BEHAVIORS
#    [Reserved: Dockerfiles define behaviors inline via RUN/CMD/ENTRYPOINT in BODY]
#
# 6. USAGE/BEHAVIOR (Rails Infrastructure)
#    Purpose: How this Dockerfile is consumed and built
#    Subsections: Build Commands → Run Commands → Build Arguments
#
# Section order: File Identity → [Reserved] → [Reserved] → Related Files → [Reserved] → Usage/Behavior
# This flows: what uses this → (constants inline) → (variables inline) → related files → (behaviors inline) → how it's built
#
# Universal mapping (see standards for cross-language patterns):
#   File Identity ≈ Dependencies (who consumes this)
#   Constants ≈ Constants [Reserved: ENV defined inline in stages]
#   Variables ≈ Variables [Reserved: ARG defined inline in stages]
#   Related Files ≈ Types (what files relate to this)
#   Type Behaviors ≈ Type Methods [Reserved: RUN/CMD defined inline]
#   Usage/Behavior ≈ Package-Level State (build/run pattern)
#
# -----------------------------------------------------------------------------
# File Identity (Dependencies)
# -----------------------------------------------------------------------------
#
# Key: [PROJECT]-DOCKER-### (unique identifier for this Dockerfile)
# Consumer: Docker Engine (docker build, docker buildx)
# Loader: Dockerfile parser (BuildKit or legacy builder)
# =============================================================================

# -----------------------------------------------------------------------------
# Constants
# -----------------------------------------------------------------------------
#
# [Reserved: Dockerfiles define environment constants via ENV directive.
#  These are placed inline with the build stages in BODY rather than in SETUP
#  because they belong to specific stages (builder vs runtime).]
# =============================================================================

# -----------------------------------------------------------------------------
# Variables
# -----------------------------------------------------------------------------
#
# [Reserved: Dockerfiles define build-time variables via ARG directive.
#  These are placed inline with the build stages in BODY where they're used
#  because ARGs have stage-specific scope.]
# =============================================================================

# -----------------------------------------------------------------------------
# Related Files (Types)
# -----------------------------------------------------------------------------
#
# Build Context:
#   - .dockerignore: Exclude files from build context
#   - Source files: Copied into container during build
#
# Orchestration:
#   - docker-compose.yaml: Service orchestration using this image
#   - Makefile: Build automation scripts
#
# Configuration:
#   - Config files: Application configuration copied into image
# =============================================================================

# -----------------------------------------------------------------------------
# Type Behaviors
# -----------------------------------------------------------------------------
#
# [Reserved: Dockerfiles define behaviors via RUN, CMD, and ENTRYPOINT.
#  These are placed inline with the build stages in BODY where they execute
#  because they depend on stage context (builder vs runtime).]
# =============================================================================

# -----------------------------------------------------------------------------
# Usage/Behavior (Rails Infrastructure)
# -----------------------------------------------------------------------------
#
# Build Commands:
#   docker build -t [image-name] .
#   docker build --build-arg [ARG]=[VALUE] -t [name] .
#   docker buildx build --platform linux/amd64 -t [name] .
#
# Run Commands:
#   docker run --rm [image-name]
#   docker run -d -p [host]:[container] [image-name]
#   docker run --rm -it [image-name] /bin/sh
#
# Build Arguments (defined via ARG in stages):
#   - [ARG_NAME]: [Description and default value]
# =============================================================================

# -----------------------------------------------------------------------------
# SETUP Omission Guide
# -----------------------------------------------------------------------------
#
# Unlike METADATA (where sections can be omitted with [OMIT: reason]),
# ALL six SETUP sections must be present for structural alignment.
#
# If a section has no content for this file:
#   - Keep the section header
#   - Add [Reserved: reason] comment explaining why empty
#   - This maintains the 6-section structure across all templates
#
# Dockerfile-specific guidance:
#   - File Identity: Always required (key, consumer, loader)
#   - Constants: [Reserved: ENV defined inline in build stages]
#   - Variables: [Reserved: ARG defined inline in build stages]
#   - Related Files: Required - documents .dockerignore and compose
#   - Type Behaviors: [Reserved: RUN/CMD/ENTRYPOINT defined inline]
#   - Usage/Behavior: Required - shows docker build/run commands
#
# The goal is structural consistency - every CONFIG template has the same
# 6-section SETUP structure, making navigation and understanding predictable.
# =============================================================================

# =============================================================================
# END SETUP
# =============================================================================

# =============================================================================
# BODY
# =============================================================================
#
# For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md
#
# -----------------------------------------------------------------------------
# BODY Sections Overview
# -----------------------------------------------------------------------------
#
# 1. CONFIGURATION STRUCTURE MAP (Internal Organization)
#    Purpose: Document the build stages and instruction flow
#    Subsections: Build Stages → Instruction Flow → Layer Optimization
#
# 2. HELPERS/UTILITIES
#    [Reserved: Declarative config - no helper functions in Dockerfiles]
#
# 3. CONFIGURATION CONTENT (Core Data)
#    Purpose: The actual Dockerfile instructions
#    Subsections: Base Image → Environment → Dependencies → Build → Runtime
#
# 4. ERROR HANDLING
#    [Reserved: Declarative config - errors occur at build time via docker build]
#
# 5. PUBLIC INTERFACE
#    [Reserved: Declarative config - EXPOSE/CMD/ENTRYPOINT defined in content]
#
# Section order: Structure Map → [Reserved] → Content → [Reserved] → [Reserved]
# This flows: understand stages → (no functions) → actual instructions → (docker validates) → (interface in content)
#
# Universal mapping (see standards for cross-language patterns):
#   Configuration Structure Map ≈ Organizational Chart (document stages/flow)
#   Helpers/Utilities ≈ Helpers [Reserved: declarative config]
#   Configuration Content ≈ Core Operations (the actual Dockerfile)
#   Error Handling ≈ Error Handling [Reserved: docker build validates]
#   Public Interface ≈ Public APIs [Reserved: EXPOSE/CMD in content]

# ────────────────────────────────────────────────────────────────
# Configuration Structure Map - Internal Organization
# ────────────────────────────────────────────────────────────────
#
# Build Stages:
#   Stage 1: [stage-name] - [Purpose - e.g., build environment]
#   Stage 2: [runtime] - [Purpose - e.g., minimal production image]
#
# Instruction Flow:
#   FROM → ARG/ENV → WORKDIR → COPY deps → RUN install → COPY src → RUN build
#   → [Multi-stage: FROM minimal] → COPY --from → EXPOSE → USER → CMD/ENTRYPOINT
#
# Layer Optimization:
#   - Least frequently changing instructions first (base, env)
#   - Dependencies before source code
#   - Combine RUN commands to reduce layers
#   - Clean up in same layer as installation

# ────────────────────────────────────────────────────────────────
# Helpers/Utilities
# ────────────────────────────────────────────────────────────────
#
# [Reserved: Declarative config - Dockerfiles have no helper functions.
# Docker provides build-time operations via RUN instructions.
# Any "helpers" are shell commands within RUN directives.]

# ────────────────────────────────────────────────────────────────
# Configuration Content - Core Data
# ────────────────────────────────────────────────────────────────
#
# Dockerfile instructions organized by build phase.
# Each section follows Docker best practices for layer caching.

# ═══ Base Image Selection ═══
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

# ────────────────────────────────────────────────────────────────
# Error Handling
# ────────────────────────────────────────────────────────────────
#
# [Reserved: Declarative config - Dockerfile errors occur at build time.
# Docker provides error detection via 'docker build':
#   - Syntax errors: Reported immediately
#   - Missing files: COPY/ADD failures
#   - Command failures: Non-zero exit codes in RUN
# See CLOSING "Configuration Validation" for debugging commands.]

# ────────────────────────────────────────────────────────────────
# Public Interface
# ────────────────────────────────────────────────────────────────
#
# [Reserved: Declarative config - interface defined in Configuration Content.
# The Dockerfile's "public interface" includes:
#   - EXPOSE: Network ports available
#   - CMD/ENTRYPOINT: How to run the container
#   - VOLUME: Mount points available
# These are documented inline where they appear in the build stages.]

# -----------------------------------------------------------------------------
# BODY Omission Guide
# -----------------------------------------------------------------------------
#
# ALL five sections MUST be present. Content may be reserved with reason:
#
#   - Configuration Structure Map: Rarely reserved - documents stages and flow
#   - Helpers/Utilities: [Reserved: Declarative config - no functions]
#   - Configuration Content: Rarely reserved - contains Dockerfile instructions
#   - Error Handling: [Reserved: Declarative config - docker build validates]
#   - Public Interface: [Reserved: EXPOSE/CMD documented in Configuration Content]
#
# Unlike METADATA (sections omitted entirely with [OMIT:]), BODY preserves
# all section headers with [Reserved:] notation for unused sections.
#
# For Dockerfiles, Configuration Content is the main section. Structure Map
# documents the multi-stage architecture. Other sections are [Reserved].

# =============================================================================
# END BODY
# =============================================================================

# =============================================================================
# CLOSING
# =============================================================================
#
# For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
#
# -----------------------------------------------------------------------------
# CLOSING Sections Overview
# -----------------------------------------------------------------------------
#
# GROUP 1: VALIDATION (Verify Configuration)
#
# 1. CONFIGURATION VALIDATION (Testing & Verification)
#    Purpose: Verify Dockerfile builds correctly and image works
#    Subsections: Lint → Build → Test Run → Common Issues
#
# 2. CONFIGURATION EXECUTION
#    [Reserved: Declarative config - execution via docker run, not Dockerfile]
#
# 3. CONFIGURATION CLEANUP
#    [Reserved: Cleanup handled by docker system prune, not in Dockerfile]
#
# GROUP 2: FINAL DOCUMENTATION (Synthesis - Reference Back to Earlier Blocks)
#
# 4. CONFIGURATION OVERVIEW (Summary with Back-References)
#    Purpose: High-level summary pointing back to METADATA for details
#
# 5. MODIFICATION POLICY (Safe/Careful/Never)
#    Purpose: Guide future maintainers on what's safe to change
#
# 6. DEPENDENCY FLOW
#    [Reserved: Declarative config - see BODY Configuration Structure Map]
#
# 7. EXTENSION POINTS (How to Add Configuration)
#    Purpose: Guide for adding stages, instructions, patterns
#
# 8. PERFORMANCE CONSIDERATIONS
#    Purpose: Layer caching, image size optimization
#
# 9. TROUBLESHOOTING GUIDE (Common Issues)
#    Purpose: Solutions for common Dockerfile problems
#
# 10. RELATED COMPONENTS (Back-Reference to METADATA)
#     Purpose: Point to .dockerignore, compose, related images
#
# 11. FUTURE EXPANSIONS (Roadmap)
#     [Reserved: Features determined by application needs]
#
# 12. CLOSING NOTE (Summary & Scripture)
#     Purpose: Final summary and grounding
#
# 13. QUICK REFERENCE (Usage Examples)
#     Purpose: Copy-paste ready build/run commands
#
# ════════════════════════════════════════════════════════════════
# GROUP 1: VALIDATION
# ════════════════════════════════════════════════════════════════
#
# ────────────────────────────────────────────────────────────────
# Configuration Validation: Dockerfile (Container Image)
# ────────────────────────────────────────────────────────────────
#
# Lint Verification:
#   docker run --rm -i hadolint/hadolint < Dockerfile
#
# Build Verification:
#   docker build -t [name]:test .
#   docker images [name]:test  # Check image size
#
# Test Run:
#   docker run --rm [name]:test [test-command]
#   docker run --rm -it [name]:test /bin/sh  # Interactive shell
#
# Common Issues:
#   "COPY failed" - Check file exists in build context
#   "permission denied" - Check USER directive and file permissions
#   "exec format error" - Check architecture matches target
#
# ────────────────────────────────────────────────────────────────
# Configuration Execution
# ────────────────────────────────────────────────────────────────
#
# [Reserved: Declarative config - Dockerfiles define images, not execution.
# Execution happens via 'docker run' or docker-compose.
# See METADATA "Usage & Integration" for run commands.]
#
# ────────────────────────────────────────────────────────────────
# Configuration Cleanup
# ────────────────────────────────────────────────────────────────
#
# [Reserved: Cleanup happens at docker level, not in Dockerfile.
# Docker provides cleanup commands:
#   docker system prune   - Remove unused data
#   docker image prune    - Remove unused images
#   docker builder prune  - Clear build cache]
#
# ════════════════════════════════════════════════════════════════
# GROUP 2: FINAL DOCUMENTATION
# ════════════════════════════════════════════════════════════════
#
# ────────────────────────────────────────────────────────────────
# Configuration Overview & Usage Summary
# ────────────────────────────────────────────────────────────────
#
# Purpose: See METADATA "Purpose & Function" section above
#
# Provides: See METADATA "Key Features" list above
#
# Quick summary (high-level only - details in METADATA):
#   - Builds [describe what] container image
#   - Uses [single/multi]-stage build pattern
#   - Results in [minimal/full] runtime image
#
# Architecture: See METADATA "CPI-SI Identity" section above
#
# ────────────────────────────────────────────────────────────────
# Modification Policy
# ────────────────────────────────────────────────────────────────
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
# Validation After Modifications:
#   See "Configuration Validation" section in GROUP 1 above.
#
# ────────────────────────────────────────────────────────────────
# Dependency Flow
# ────────────────────────────────────────────────────────────────
#
# [Reserved: Declarative config - see BODY "Configuration Structure Map"
# section above for build stages and instruction flow.
#
# Quick summary (details in BODY):
#   FROM base → install deps → copy source → build → [multi-stage] → runtime]
#
# ────────────────────────────────────────────────────────────────
# Extension Points (How to Add Configuration)
# ────────────────────────────────────────────────────────────────
#
# Image Patterns (choose appropriate for use case):
#   1. Single-stage: Simple apps, scripts, tools
#   2. Multi-stage: Compiled languages, minimal production images
#   3. Base image: Shared dependencies for multiple containers
#   4. Builder pattern: Complex builds with multiple outputs
#
# Adding Build Arguments:
#   ARG [NAME]=[default]  # In Dockerfile
#   docker build --build-arg [NAME]=[value] .  # At build time
#
# Adding Environment Variables:
#   ENV [NAME]=[value]  # Build-time and runtime
#
# ────────────────────────────────────────────────────────────────
# Performance Considerations
# ────────────────────────────────────────────────────────────────
#
# Layer Caching:
#   - Order instructions from least to most frequently changing
#   - Dependencies before source code
#   - Combine RUN commands to reduce layers
#
# Image Size:
#   - Use multi-stage builds for compiled languages
#   - Use slim/alpine base images when possible
#   - Remove unnecessary files in same RUN layer as installation
#   - Use .dockerignore to exclude files from build context
#
# Build Speed:
#   - Leverage layer caching with proper instruction order
#   - Use buildkit for parallel builds: DOCKER_BUILDKIT=1
#
# ────────────────────────────────────────────────────────────────
# Troubleshooting Guide
# ────────────────────────────────────────────────────────────────
#
# Problem: "COPY failed: file not found"
#   - Cause: File not in build context or in .dockerignore
#   - Solution: Verify file exists, check .dockerignore patterns
#
# Problem: "permission denied"
#   - Cause: USER directive running as non-root without access
#   - Solution: Ensure files have correct permissions, or chown in RUN
#
# Problem: "exec format error"
#   - Cause: Binary architecture doesn't match container architecture
#   - Solution: Build for correct platform: --platform linux/amd64
#
# Problem: Image too large
#   - Cause: Unnecessary files, multiple layers, wrong base image
#   - Solution: Use multi-stage, clean in same RUN, use slim base
#
# ────────────────────────────────────────────────────────────────
# Related Components & Dependencies
# ────────────────────────────────────────────────────────────────
#
# See METADATA "Dependencies" section above for complete information.
#
# Related files:
#   - .dockerignore: Exclude files from build context
#   - docker-compose.yaml: Multi-container orchestration
#   - [related Dockerfiles]: Base images, builder images
#
# ────────────────────────────────────────────────────────────────
# Future Expansions & Roadmap
# ────────────────────────────────────────────────────────────────
#
# [Reserved: Dockerfile features determined by application needs.
# Potential expansions:
#   ⏳ Add health checks
#   ⏳ Add multi-architecture support
#   ⏳ Optimize for smaller image size]
#
# ────────────────────────────────────────────────────────────────
# Closing Note
# ────────────────────────────────────────────────────────────────
#
# This Dockerfile defines [describe what it builds/runs].
# Part of Kingdom Technology infrastructure.
#
# For contributions:
#   - Review the modification policy above
#   - Follow the 4-block structure pattern
#   - Test with 'docker build -t test .' before committing
#
# "[Relevant Scripture verse]" - [Reference]
#
# ────────────────────────────────────────────────────────────────
# Quick Reference: Usage Examples
# ────────────────────────────────────────────────────────────────
#
# Build:
#   docker build -t [name] .
#   docker build --build-arg VERSION=1.0 -t [name] .
#   DOCKER_BUILDKIT=1 docker build -t [name] .  # Faster builds
#
# Run:
#   docker run --rm [name]
#   docker run -d -p 8080:8080 [name]  # Daemon with port mapping
#   docker run --rm -it [name] /bin/sh  # Interactive shell
#
# Debug:
#   docker build --progress=plain -t [name] .  # Verbose output
#   docker history [name]  # Show layer history
#   docker inspect [name]  # Show image details
#
# -----------------------------------------------------------------------------
# CLOSING Omission Guide
# -----------------------------------------------------------------------------
#
# ALL thirteen sections MUST be present. Content may be reserved with reason:
#
# GROUP 1: VALIDATION
#   - Configuration Validation: Rarely reserved - all Dockerfiles need testing
#   - Configuration Execution: [Reserved: Execution via docker run, not Dockerfile]
#   - Configuration Cleanup: [Reserved: Cleanup via docker system prune]
#
# GROUP 2: FINAL DOCUMENTATION (mostly back-references for CONFIG)
#   - Configuration Overview: Rarely reserved - always provides summary
#   - Modification Policy: Rarely reserved - always guides maintainers
#   - Dependency Flow: [Reserved: See BODY Structure Map]
#   - Extension Points: Rarely reserved - shows patterns and how to extend
#   - Performance Considerations: Rarely reserved - layer/size optimization important
#   - Troubleshooting Guide: Rarely reserved - common issues help users
#   - Related Components: Rarely reserved - shows .dockerignore, compose
#   - Future Expansions: [Reserved: Features determined by application needs]
#   - Closing Note: Rarely reserved - summary and grounding
#   - Quick Reference: Rarely reserved - build/run commands help users
#
# Dockerfiles have more content than Go config files since they're
# actively built and have performance/size considerations.

# =============================================================================
# END CLOSING
# =============================================================================
