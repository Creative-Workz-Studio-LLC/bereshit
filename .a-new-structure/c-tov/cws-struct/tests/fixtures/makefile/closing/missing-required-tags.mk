# #!omni code --makefile

# ============================================================================
# OMNICODE PRAGMA [PRAGMA]
# ============================================================================
#
# --- P1: Core ---
# key:       B-test-makefile-missing-closing-tags
# from:      tests/fixtures/makefile/closing/missing-required-tags.mk
# at:        a-01.00
#
# --- P2: Family ---
# type:      code
# subtype:   makefile
# role:      fixture
# structure: 4-block
#
# --- P5: Summary ---
# title:     Missing Required Closing Tags Fixture
# brief:     Makefile missing POLICY, REFERENCE, NOTE closing tags
#
# ============================================================================
# END PRAGMA [END]
# ============================================================================

# ============================================================================
# METADATA BLOCK [METADATA]
# ============================================================================

# # M.1 Core Identity [IDENTITY]
#
# Key: B-test-makefile-missing-closing-tags
# Title: Missing Required Closing Tags Fixture
# Type: Build
# Component: Rails
# Role: Test fixture for CWS-R25-052

# # M.2 Lifecycle State [STATE]
#
# Status: Active
# Version: a-01.00
# Created: 2026-02-23

# # M.3 Attribution [ATTRIBUTION]
#
# Authors: Nova Dawn (CPI-SI)
# Organization: CreativeWorkzStudio LLC

# ============================================================================
# END METADATA [END]
# ============================================================================

# ============================================================================
# SETUP BLOCK [SETUP]
# ============================================================================

.DEFAULT_GOAL := help

# ============================================================================
# CLOSING BLOCK [CLOSING]
# ============================================================================

# NOTE: Required closing tags POLICY, REFERENCE, and NOTE are missing.
# This triggers CWS-R25-052 (closing/section/{{tag}}/required).

# # X.1 Validation [VALIDATION]
#
# make help — show available targets.

# ============================================================================
# END CLOSING [END]
# ============================================================================
