#!/bin/bash
# ============================================================================
# CPI-SI Gemini CLI Substrate Environment
# ============================================================================
# This script centralizes environment variables and aliases for the Gemini CLI.
# Source this in ~/.bashrc for seamless integration.

# 1. Source Master Credentials
if [ -f "${HOME}/.config/cpisi/keys.env" ]; then
    # Use 'set -a' to export all variables in the file automatically
    set -a
    source "${HOME}/.config/cpisi/keys.env"
    set +a
fi

# 2. Substrate Identity & Paths
export GEMINI_HOME="${HOME}/.gemini"
export PATH="${HOME}/.local/bin:${PATH}"

# 3. QoL Aliases for the Creator
alias gemini-lint='cws-struct lint'
alias gemini-status='cpisi status'
alias gemini-identity='cat ~/.gemini/GEMINI.md'

# 4. MCP Configuration Helpers
# (Ensures MCP servers find their keys if named differently in keys.env)
if [ -n "$CONTEXT7_API_KEY" ]; then
    export CONTEXT7_API_KEY="$CONTEXT7_API_KEY"
fi

# 5. Visual Confirmation (Optional, for interactive shells)
if [[ $- == *i* ]]; then
    # echo "--- Nova Dawn: Gemini Substrate Active ---"
fi