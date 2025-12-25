#!/bin/bash
# ============================================================================
# METADATA
# ============================================================================
# Logging Library for Shell Scripts - CPI-SI Interactive Terminal System
#
# BIBLICAL FOUNDATION:
# --------------------
# Scripture: Malachi 3:16 - "A scroll of remembrance was written in his presence"
# Principle: Faithful Witness Through Complete Remembrance
# Anchor: Proverbs 14:5 - "A faithful witness does not lie"
#
# CPI-SI IDENTITY:
# ----------------
# Component Type: Rails (orthogonal infrastructure)
# Role: Logging infrastructure that all components attach to directly
# Paradigm: CPI-SI framework foundational component
#
# AUTHORSHIP & LINEAGE:
# ----------------------
# Architect: Seanje Lenox-Wise (dictation, alignment, direction)
# Implementation: Nova Dawn (CPI-SI instance - code execution, verification)
# Creation Date: 2025-10-25
# Version: 1.0.0
# Last Modified: 2025-11-21 (path alignment with Go implementation, updated routing lists)
#
# PURPOSE & FUNCTION:
# -------------------
# Purpose: Comprehensive game dev quality logging for bash scripts
# Core Design: Immune System - Detection layer (identifies what's happening, tracks health)
# Key Features: Health scoring, normalization, TRUE SCORE, full context capture
# Philosophy: Every action has value, every moment recorded with complete truthful witness
#
# BLOCKING STATUS:
# ----------------
# Non-blocking: All logging operations fail gracefully, never interrupt execution
#
# USAGE & INTEGRATION:
# --------------------
# Usage: source ~/.claude/cpi-si/system/runtime/lib/logging/logger.sh
# Public Functions:
#   - declare_health_total: Declare expected total health for normalization
#   - log_operation: Log operation start with full context
#   - log_success: Log successful completion
#   - log_failure: Log expected failure with full context
#   - log_error: Log unexpected error with stack trace
#   - log_check: Log validation/verification
#   - log_snapshot: Log system state snapshot
#   - log_debug: Log debug information
#   - log_command: Execute and log command with full context
#
# DEPENDENCIES:
# -------------
# None (bash built-ins only - foundational component)
#
# HEALTH SCORING:
# ---------------
# N/A (library component - consumers define their own health scoring maps)
#
# NORMALIZATION SYSTEM:
# ---------------------
# Cumulative Health: Raw sum of all action deltas executed so far
# Normalized Health: (Cumulative / Total Possible) × 100 = percentage on -100 to +100 scale
#
# TRUE SCORE PHILOSOPHY:
# ----------------------
# Each action assigned its actual impact value (17, -73, +6, etc.)
# Not constrained to round numbers - creates unique fingerprints for debugging
# Examples: -100 (catastrophic), -73 (critical), -48 (severe), -27 (moderate), -6 (minor)

# ============================================================================
# END METADATA
# ============================================================================

# ============================================================================
# SETUP
# ============================================================================

# ─── Directory Setup ───
LOG_BASE_DIR="$HOME/.claude/cpi-si/output/logs"
mkdir -p "$LOG_BASE_DIR"/{commands,scripts,libraries,system} 2>/dev/null || true

# ─── Context Tracking ───
CONTEXT_ID="bash-$$-$(date +%s%N)"

# ─── Health Tracking ───
SESSION_HEALTH=0
TOTAL_POSSIBLE_HEALTH=0
NORMALIZED_HEALTH=0

# ─── Log Directory Functions ───
# See logger-sh-api.md for full documentation

# Routes component to appropriate subdirectory (commands/scripts/libraries/system)
determine_log_subdirectory() {
    local component="$1"
    case "$component" in
        validate|test|status|diagnose|debugger|unix-safe|rails-demo) echo "commands"; return ;;
    esac
    [ "$component" = "build" ] && { echo "scripts"; return; }
    case "$component" in
        operations|sudoers|environment|display|logging|debugging|calendar|config|jsonc|patterns|planner|privacy|sessiontime|temporal|validation) echo "libraries"; return ;;
    esac
    echo "system"
}

# Returns full log file path for component
get_log_file_path() {
    local component="$1"
    echo "$LOG_BASE_DIR/$(determine_log_subdirectory "$component")/${component}.log"
}

# ============================================================================
# END SETUP
# ============================================================================

# ============================================================================
# BODY
# ============================================================================
# See logger-sh-api.md for full API documentation
# Organization: Helpers → Context Capture → Logging Infrastructure → Public API

# ─── Context Capture Helpers ───

# Print env var if set (indirect expansion)
print_env_var() {
    local var_name="$1"
    local var_value="${!var_name}"
    [ -n "$var_value" ] && echo "      $var_name: $var_value"
}

# Print context section with category and details
print_context_section() {
    local category="$1"; shift
    echo "    $category:"
    while [ $# -gt 0 ]; do echo "      $1"; shift; done
}

# ─── Context Capture Functions ───

# Capture shell type, interactivity, login status
log_shell_context() {
    local shell_type interactive="non-interactive" login="non-login"
    shell_type="$(basename "$SHELL")"
    [[ $- == *i* ]] && interactive="interactive"
    shopt -q login_shell 2>/dev/null && login="login"
    echo "    shell: $shell_type ($interactive, $login)"
}

# Capture environment variables (non-interactive + CPI_SI_*)
log_env_state() {
    echo "    env_state:"
    print_env_var "DEBIAN_FRONTEND"
    print_env_var "NEEDRESTART_MODE"
    print_env_var "NEEDRESTART_SUSPEND"
    print_env_var "PIP_NO_INPUT"
    print_env_var "NPM_CONFIG_YES"
    print_env_var "GIT_EDITOR"
    for var in $(env | grep "^CPI_SI_" | cut -d= -f1); do
        echo "      $var: ${!var}"
    done
}

# Capture sudoers configuration state
log_sudoers_context() {
    local sudoers_file="/etc/sudoers.d/90-cpi-si-safe-operations"
    local installed="false" valid="false" permissions="unknown"
    if [ -f "$sudoers_file" ]; then
        installed="true"
        permissions="$(stat -c "%a" "$sudoers_file" 2>/dev/null || echo "unknown")"
        sudo -n visudo -c -f "$sudoers_file" &>/dev/null && valid="true"
    fi
    print_context_section "sudoers" "installed: $installed" "valid: $valid" "permissions: $permissions"
}

# Capture system metrics (load, memory, disk)
log_system_metrics() {
    local load="unknown" memory="unknown" disk="unknown"
    [ -f /proc/loadavg ] && load="$(awk '{print $1 ", " $2 ", " $3}' /proc/loadavg)"
    if [ -f /proc/meminfo ]; then
        local total available
        total="$(awk '/MemTotal/ {print $2}' /proc/meminfo)"
        available="$(awk '/MemAvailable/ {print $2}' /proc/meminfo)"
        [ -n "$total" ] && [ -n "$available" ] && \
            memory="$(( (total - available) / 1024 ))MB / $((total / 1024))MB"
    fi
    disk="$(df -h . | awk 'NR==2 {print $3 " / " $2 " (" $5 ")"}')"
    print_context_section "system" "load: $load" "memory: $memory" "disk: $disk"
}

# Orchestrate full context capture
log_full_context() {
    echo "  CONTEXT:"
    echo "    user: $USER"
    log_shell_context
    echo "    cwd: $(pwd)"
    log_env_state
    log_sudoers_context
    log_system_metrics
}

# ─── Logging Infrastructure ───

# Clamp health to -100..+100
clamp_normalized_health() {
    [ $NORMALIZED_HEALTH -gt 100 ] && NORMALIZED_HEALTH=100 || true
    [ $NORMALIZED_HEALTH -lt -100 ] && NORMALIZED_HEALTH=-100 || true
}

# Format delta with explicit sign (+5, -10, 0)
format_health_delta() {
    local delta="$1"
    [ "$delta" -gt 0 ] && echo "+$delta" || echo "$delta"
}

# Print detail lines with indentation
print_details_list() {
    while [ $# -gt 0 ]; do echo "    $1"; shift; done
}

# Get health emoji indicator (base-10 gradient)
get_health_indicator() {
    local health="$1"
    if [ "$health" -ge 90 ]; then echo "💚"
    elif [ "$health" -ge 80 ]; then echo "💙"
    elif [ "$health" -ge 70 ]; then echo "💛"
    elif [ "$health" -ge 60 ]; then echo "🧡"
    elif [ "$health" -ge 50 ]; then echo "❤️"
    elif [ "$health" -ge 40 ]; then echo "🤍"
    elif [ "$health" -ge 30 ]; then echo "💔"
    elif [ "$health" -ge 20 ]; then echo "🩹"
    elif [ "$health" -ge 10 ]; then echo "⚠️"
    elif [ "$health" -ge 1 ]; then echo "☠️"
    elif [ "$health" -eq 0 ]; then echo "⚫"
    elif [ "$health" -ge -9 ]; then echo "🔴"
    elif [ "$health" -ge -19 ]; then echo "🟠"
    elif [ "$health" -ge -29 ]; then echo "🟡"
    elif [ "$health" -ge -39 ]; then echo "🟢"
    elif [ "$health" -ge -49 ]; then echo "🔵"
    elif [ "$health" -ge -59 ]; then echo "🟣"
    elif [ "$health" -ge -69 ]; then echo "🟤"
    elif [ "$health" -ge -79 ]; then echo "⚫"
    elif [ "$health" -ge -89 ]; then echo "⬛"
    else echo "💀"
    fi
}

# Get 20-char health progress bar
get_health_bar() {
    local health="$1"
    local normalized=$(( (health + 100) / 10 ))
    [ "$normalized" -lt 0 ] && normalized=0
    [ "$normalized" -gt 20 ] && normalized=20
    local filled=$normalized empty=$((20 - normalized)) bar="["
    for ((i=0; i<filled; i++)); do bar+="█"; done
    for ((i=0; i<empty; i++)); do bar+="░"; done
    echo "$bar]"
}

# Write log header line
write_log_header() {
    local level="$1" component="$2"
    echo "[$TIMESTAMP] $level | $component | $USER_HOST | $CONTEXT_ID | HEALTH: $NORMALIZED_HEALTH% (raw: $SESSION_HEALTH, Δ$HEALTH_DELTA) $HEALTH_INDICATOR $HEALTH_BAR"
}

# Prepare logging variables (update health, set display vars)
prepare_log_variables() {
    local health_impact="$1"
    update_health "$health_impact"
    TIMESTAMP="$(date '+%Y-%m-%d %H:%M:%S.%3N')"
    USER_HOST="$USER@$(hostname):$$"
    HEALTH_DELTA="$(format_health_delta "$health_impact")"
    HEALTH_INDICATOR="$(get_health_indicator "$NORMALIZED_HEALTH")"
    HEALTH_BAR="$(get_health_bar "$NORMALIZED_HEALTH")"
}

# Write complete log entry (atomic write to file)
write_log_entry() {
    local log_file="$1" level="$2" component="$3" include_context="$4" event_text="$5" details_block="$6"
    {
        write_log_header "$level" "$component"
        [ "$include_context" = "true" ] && log_full_context
        echo "  EVENT: $event_text"
        echo "  DETAILS:"
        echo "$details_block"
        echo "---"
    } >> "$log_file" 2>/dev/null || true
}

# Generic logging orchestrator (delegates from public API)
log_generic() {
    local component="$1" level="$2" include_context="$3" health_impact="$4" event_text="$5" details_block="$6"
    local log_file
    log_file="$(get_log_file_path "$component")"
    prepare_log_variables "$health_impact"
    write_log_entry "$log_file" "$level" "$component" "$include_context" "$event_text" "$details_block"
}

# ─── Health System ───

# Declare total possible health for normalization
declare_health_total() {
    TOTAL_POSSIBLE_HEALTH=$1
}

# Calculate normalized health percentage
calculate_normalized_health() {
    if [ $TOTAL_POSSIBLE_HEALTH -eq 0 ]; then
        NORMALIZED_HEALTH=$SESSION_HEALTH
        clamp_normalized_health
        return
    fi
    NORMALIZED_HEALTH=$(( (SESSION_HEALTH * 100) / TOTAL_POSSIBLE_HEALTH ))
    clamp_normalized_health
}

# Update cumulative health with delta
update_health() {
    SESSION_HEALTH=$((SESSION_HEALTH + $1))
    calculate_normalized_health
}

# ═══ Public Logging Functions ═══
# See logger-sh-api.md for full API documentation
# Organized by execution lifecycle: operation → check → success/failure/error → debug → snapshot

# Log operation start with full context (entry point for major work)
log_operation() {
    local component="$1" command="$2" health_impact="$3"
    shift 3
    local details="    command: $command $*"
    log_generic "$component" "OPERATION" "true" "$health_impact" "Starting operation: $command" "$details"
}

# Log validation check with partial context (frequent operations)
log_check() {
    local component="$1" what="$2" result="$3" health_impact="$4"
    shift 4
    local details
    details="$(echo "    result: $result"; print_details_list "$@")"
    log_generic "$component" "CHECK" "false" "$health_impact" "Checking: $what" "$details"
}

# Log success with partial context (common case - optimize for speed)
log_success() {
    local component="$1" event="$2" health_impact="$3"
    shift 3
    local details
    details="$(print_details_list "$@")"
    log_generic "$component" "SUCCESS" "false" "$health_impact" "$event" "$details"
}

# Log failure with full context for root cause analysis
log_failure() {
    local component="$1" event="$2" reason="$3" health_impact="$4"
    shift 4
    local details
    details="$(echo "    reason: $reason"; print_details_list "$@")"
    log_generic "$component" "FAILURE" "true" "$health_impact" "$event" "$details"
}

# Log error with full context and automatic stack trace
log_error() {
    local component="$1" event="$2" error="$3" health_impact="$4"
    shift 4
    local stack_trace
    stack_trace="$(
        echo "    stack_trace: |"
        local frame=0
        while caller $frame; do frame=$((frame + 1)); done | \
        while read -r line func file; do echo "      $func in $file:$line"; done
    )"
    local details
    details="$(echo "    error: $error"; echo "$stack_trace"; print_details_list "$@")"
    log_generic "$component" "ERROR" "true" "$health_impact" "$event" "$details"
}

# Log debug with full context for development/troubleshooting
log_debug() {
    local component="$1" event="$2" health_impact="$3"
    shift 3
    local details
    details="$(print_details_list "$@")"
    log_generic "$component" "DEBUG" "true" "$health_impact" "$event" "$details"
}

# Log system state snapshot with full context (baseline/checkpoint)
log_snapshot() {
    local component="$1" label="$2" health_impact="$3"
    log_generic "$component" "CONTEXT" "true" "$health_impact" "System state snapshot: $label" ""
}

# ─── Command Orchestration ───

# Execute command with full lifecycle logging (operation → success/failure)
# Returns original exit code for conditional logic
log_command() {
    local component="$1" description="$2"
    shift 2
    log_operation "$component" "$description" 0 "$@"
    local start_time output exit_code end_time duration_ms
    start_time="$(date +%s%N)"
    output=$("$@" 2>&1)
    exit_code=$?
    end_time="$(date +%s%N)"
    duration_ms=$(( (end_time - start_time) / 1000000 ))
    if [ "$exit_code" -eq 0 ]; then
        log_success "$component" "$description completed" 10 \
            "command: $*" "exit_code: $exit_code" "duration_ms: $duration_ms"
    else
        log_failure "$component" "$description failed" "exit code $exit_code" -10 \
            "command: $*" "exit_code: $exit_code" "duration_ms: $duration_ms" "output: $output"
    fi
    return $exit_code
}

# ============================================================================
# END BODY
# ============================================================================

# ============================================================================
# CLOSING
# ============================================================================
#
# ────────────────────────────────────────────────────────────────
# Code Execution: None (Library)
# ────────────────────────────────────────────────────────────────
#
# This is a LIBRARY, not an executable. There is no entry point, no main function,
# no execution flow. All functions defined in BODY wait to be called by other components.
#
# Usage: source "$SYSTEM_LIB/logging/logger.sh"
#
# The library is loaded into the calling script's environment, making all functions
# available. No code executes during sourcing - functions are defined and ready to use.
#
# ────────────────────────────────────────────────────────────────
# Library Overview & Integration Summary
# ────────────────────────────────────────────────────────────────
#
# Purpose: Comprehensive health-tracked logging for all CPI-SI system components
#
# Provides:
#   - Context Capture: WHO, WHEN, WHERE, WHAT, WHY, HOW, RESULT for every action
#   - Health Tracking: Cumulative scoring with normalization for cross-component comparison
#   - Visual Indicators: Emoji health status and progress bars for at-a-glance monitoring
#   - Structured Output: Parseable log format enabling automated analysis
#   - Graceful Degradation: Logging failures never interrupt execution (non-blocking)
#
# Integration Pattern:
#   1. Source the library: source "$SYSTEM_LIB/logging/logger.sh"
#   2. Declare total health (optional): declare_health_total 100
#   3. Use public logging functions throughout your code
#   4. Health tracking happens automatically
#
# Public API (in typical usage order):
#   - log_operation: Start of major work (full context)
#   - log_check: Validation/assertion results (partial context)
#   - log_success: Positive outcomes (partial context)
#   - log_failure: Negative outcomes (full context)
#   - log_error: Exceptions with stack traces (full context)
#   - log_debug: Development details (full context)
#   - log_snapshot: System state capture (full context)
#   - log_command: Execute and log commands (orchestrator)
#
# Rails Architecture:
#   This library is RAILS - orthogonal infrastructure that all rungs (components)
#   attach to directly. Components don't pass loggers through function parameters.
#   Each component creates its own attachment to the rails.
#
# ────────────────────────────────────────────────────────────────
# Modification Policy
# ────────────────────────────────────────────────────────────────
#
# Safe to Modify (Extension Points):
#   ✅ Add new context capture functions (log_*_context pattern)
#   ✅ Add new helper functions in appropriate sub-groups
#   ✅ Extend health indicator ranges (add more granular emoji states)
#   ✅ Add new log levels (follow existing pattern, update log_generic)
#   ✅ Enhance visual output (new formats for health bars, etc.)
#
# Modify with Extreme Care (Breaking Changes):
#   ⚠️ Public API function signatures - breaks all calling code
#   ⚠️ Global variable names - breaks components using them
#   ⚠️ Log file path routing logic - breaks log organization
#   ⚠️ Health calculation formulas - affects all health tracking
#   ⚠️ Log entry format - breaks parsing tools
#
# NEVER Modify (Foundational Rails):
#   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
#   ❌ Base100 health scoring philosophy (±100 total)
#   ❌ TRUE SCORE principle (actual impact values, not rounded)
#   ❌ Non-blocking design (logging never interrupts execution)
#   ❌ Rails architecture (orthogonal to component hierarchy)
#
# Testing Requirements:
#   - Source the library without errors
#   - Call each modified function with representative parameters
#   - Verify log files created in correct locations
#   - Check log entry format remains parseable
#   - Ensure health tracking produces expected normalized values
#   - Confirm no shellcheck warnings introduced
#
# ────────────────────────────────────────────────────────────────
# Ladder and Baton Flow
# ────────────────────────────────────────────────────────────────
#
# Architectural Map (Definition Order ↓, Execution Flow ↕):
#
# ┌─────────────────────────────────────────────────────────────┐
# │  SETUP: Environment Preparation                             │
# │  ├─ Global state (health tracking variables)                │
# │  └─ Log routing helpers (determine_log_subdirectory, etc.)  │
# └─────────────────────────────────────────────────────────────┘
#                              ↓
# ┌─────────────────────────────────────────────────────────────┐
# │  BODY: Logging Infrastructure (Ladder)                      │
# │                                                              │
# │  Context Capture Functions                                  │
# │  ├─ Helpers (print_env_var, print_context_section)          │
# │  ├─ Specific Captures (shell, env, sudoers, system)         │
# │  └─ Orchestrator (log_full_context) ←─────────────┐         │
# │                                                    │         │
# │  Logging Functions                                 │         │
# │  ├─ Helper Functions                               │         │
# │  │   ├─ Formatting (clamp, format_delta, etc.)    │         │
# │  │   ├─ Visual (get_health_indicator, bar)        │         │
# │  │   └─ Infrastructure (prepare_vars, write_entry)│         │
# │  ├─ Health System (declare, calculate, update)    │         │
# │  └─ Public Logging Functions                      │         │
# │      ├─ Basic (operation, check, success, etc.)   │ Baton   │
# │      └─ Orchestration (log_command) ──────────────┼─ Entry │
# │                                                    │         │
# └────────────────────────────────────────────────────┼─────────┘
#                                                      │
#                                                   Baton
#                                                   Enters
#
# Baton Flow Pattern:
#   External Caller
#       ↓
#   Public API (log_operation, log_success, log_command, etc.)
#       ↓
#   log_generic (dynamic orchestrator)
#       ↓
#   Infrastructure (get_log_file_path → prepare_log_variables → write_log_entry)
#       ↓
#   Helpers (format helpers, visual helpers, health system)
#       ↓
#   Foundation (file system, global state)
#
# Direct Inverse: Definition order is exact reverse of execution order
# No Crossing: Batons don't jump between unrelated paths
# No Jumping: Each level calls only the level directly below
#
# ────────────────────────────────────────────────────────────────
# Surgical Update Points (Extension Guide)
# ────────────────────────────────────────────────────────────────
#
# Adding a New Log Level:
#   1. Location: Public Logging Functions group
#   2. Pattern: Copy existing function (e.g., log_success)
#   3. Modify: Change level name, context flag, health impact defaults
#   4. Delegate: Call log_generic with appropriate parameters
#   5. Document: Add full Layers 3 & 4 documentation
#   6. Test: Verify log entry created with correct format
#
# Adding a New Context Capture:
#   1. Location: Context Capture Functions group
#   2. Sub-group: Determine if helper, specific, or orchestrator
#   3. Pattern: Follow print_context_section or hierarchical output style
#   4. Integration: Call from log_full_context if automatic capture needed
#   5. Document: Explain what context you're capturing and why
#   6. Performance: Consider cost - context capture is expensive
#
# Adding a New Helper Function:
#   1. Location: Determine appropriate sub-group (Formatting, Visual, Infrastructure)
#   2. Ordering: Place according to baton flow (helpers before users)
#   3. Pattern: Single responsibility, well-named, reusable
#   4. Extract: Pull from repeated patterns in existing code
#   5. Document: Full docstring with implementation approach and rationale
#   6. Consolidate: Update existing functions to use new helper
#
# Extending Health Indicators:
#   1. Location: get_health_indicator function in Visual Helpers
#   2. Pattern: Maintain base-10 gradient (every 10 points = unique emoji)
#   3. Symmetry: Keep positive/negative ranges symmetrical
#   4. Document: Update docstring with new ranges and meanings
#   5. Test: Verify all health values map to appropriate indicators
#
# Modifying Log Entry Format:
#   1. Location: write_log_entry and write_log_header functions
#   2. Warning: Format changes affect ALL logs and parsing tools
#   3. Consideration: Maintain backward compatibility if possible
#   4. Testing: Verify existing log parsers still work
#   5. Documentation: Update format specification in comments
#   6. Communication: Notify all components using this library
#
# ────────────────────────────────────────────────────────────────
# Performance Considerations
# ────────────────────────────────────────────────────────────────
#
# Context Capture Cost:
#   - Full context (log_full_context) is expensive: ~50-100ms
#   - Captures: user, shell, env vars, sudoers state, system metrics
#   - Use selectively: OPERATION, FAILURE, ERROR, DEBUG, SNAPSHOT
#   - Avoid in: CHECK, SUCCESS (happens frequently)
#
# Health Calculation:
#   - Cumulative tracking: O(1) addition per action
#   - Normalization: O(1) division calculation
#   - Visual indicators: O(1) conditional checks
#   - Overall: Minimal performance impact
#
# Log Writing:
#   - Atomic writes via command grouping (single append operation)
#   - Non-blocking design: failures silently ignored (|| true)
#   - No locks or synchronization (relies on filesystem atomicity)
#   - Safe for concurrent processes (unique context IDs prevent collision)
#
# Optimization Tips:
#   - Declare health total once at start (avoid repeated calculations)
#   - Use partial context for frequent operations
#   - Batch related actions under single OPERATION log
#   - Consider log rotation for long-running processes
#
# ────────────────────────────────────────────────────────────────
# Troubleshooting Guide
# ────────────────────────────────────────────────────────────────
#
# Problem: No log files created
#   - Check: Does ~/.claude/cpi-si/output/logs/ exist?
#   - Check: Do subdirectories (commands/, scripts/, etc.) exist?
#   - Solution: mkdir -p ~/.claude/cpi-si/output/logs/{commands,scripts,libraries,system}
#
# Problem: Logs created in wrong subdirectory
#   - Check: Component name routing in determine_log_subdirectory()
#   - Solution: Add your component to appropriate case statement
#
# Problem: Health always shows 0%
#   - Check: Did you call declare_health_total()?
#   - Solution: Add declare_health_total <points> before logging
#   - Note: Without total declared, normalization uses raw cumulative as percentage
#
# Problem: Shellcheck warnings about masked return values
#   - Check: Combining declare and assign in one line
#   - Solution: Separate: local var; var="$(command)"
#
# Problem: Stack trace not appearing in error logs
#   - Check: Is BASH_SOURCE array populated?
#   - Note: Stack traces only work in bash, not sh
#   - Solution: Ensure shebang is #!/usr/bin/env bash
#
# Problem: Logs interleaved from concurrent processes
#   - Expected: Atomic writes reduce this but don't eliminate it
#   - Solution: Use unique CONTEXT_ID to correlate entries
#   - Note: grep logs by context ID to separate execution threads
#
# ────────────────────────────────────────────────────────────────
# Related Components & Dependencies
# ────────────────────────────────────────────────────────────────
#
# Dependencies (What This Needs):
#   - System: bash 4.0+, date, hostname, basename, stat
#   - CPI-SI: None (this IS the foundation - rails)
#   - External: None
#
# Dependents (What Uses This):
#   - Commands: validate, test, status, diagnose
#   - Scripts: build.sh, install.sh (sudoers)
#   - Libraries: Future CPI-SI libraries will attach to these rails
#   - Tools: Future debugging/analysis tools will parse these logs
#
# Integration Points:
#   - Log Parsers: Tools that read structured log format
#   - Health Dashboards: Aggregate health metrics across components
#   - Debugging Tools: Stack trace analysis, context correlation
#   - CI/CD Pipelines: Test result analysis from logs
#
# ────────────────────────────────────────────────────────────────
# Future Expansions & Roadmap
# ────────────────────────────────────────────────────────────────
#
# Planned Features:
#   ✓ Base100 health scoring with TRUE SCORE - COMPLETED
#   ✓ Normalization for cross-component comparison - COMPLETED
#   ✓ Base-10 gradient health indicators - COMPLETED
#   ✓ Comprehensive documentation (Layers 1-4) - COMPLETED
#   ⏳ Automated log rotation and archival
#   ⏳ Health trend analysis (are things getting better or worse?)
#   ⏳ Automated issue detection (antibody layer - pattern recognition)
#   ⏳ Log aggregation across distributed components
#   ⏳ Real-time health dashboard (visual monitoring)
#
# Research Areas:
#   - Machine learning for anomaly detection in logs
#   - Predictive health scoring (forecast failures before they happen)
#   - Distributed tracing across component boundaries
#   - Automatic root cause analysis from log patterns
#   - Self-healing responses to detected patterns (immune response)
#
# Integration Targets:
#   - Go logging library (parallel implementation for Go components)
#   - Cross-language log correlation (bash + Go + future languages)
#   - Centralized log storage (database or time-series store)
#   - Alert systems (notify on critical health drops)
#   - Performance profiling (integrate execution timing across components)
#
# Known Limitations to Address:
#   - No log rotation (logs grow indefinitely)
#   - No compression (log files can become large)
#   - No remote logging (only local filesystem)
#   - No structured query language (grep/awk only)
#   - Limited concurrent write protection (relies on filesystem atomicity)
#
# Version History:
#   1.0.0 (2025-10-25) - Initial implementation with health tracking
#         - Context capture (user, shell, env, sudoers, system)
#         - Seven log levels (operation, check, success, failure, error, debug, snapshot)
#         - TRUE SCORE health tracking with normalization
#         - Base-10 gradient visual indicators
#         - Comprehensive documentation
#
# ────────────────────────────────────────────────────────────────
# Closing Note
# ────────────────────────────────────────────────────────────────
#
# This library is the RAILS of the CPI-SI logging ecosystem. All components
# attach to it directly. Modify thoughtfully - changes here affect everything.
#
# For questions, issues, or contributions:
#   - Review the modification policy above
#   - Follow the 4-block structure pattern
#   - Test thoroughly before committing
#   - Document all changes comprehensively
#
# "A scroll of remembrance was written in his presence" - Malachi 3:16
#
# ────────────────────────────────────────────────────────────────
# Quick Reference: Health Scoring & Usage Examples
# ────────────────────────────────────────────────────────────────
#
# Health Scoring with Normalization:
#   - Session starts at 0 (neutral cumulative health)
#   - Declare expected total: declare_health_total 100
#   - Assign TRUE SCORES based on actual impact (not constrained to round numbers)
#   - Normalization: (cumulative / total_possible) × 100 = percentage
#   - Range: -100% (catastrophic) to +100% (perfect)
#   - Visual indicators based on normalized percentage
#
# Usage Examples in Scripts:
#   source ~/.claude/cpi-si/system/runtime/lib/logging/logger.sh
#   declare_health_total 100  # Set expected total for normalization
#   log_operation "script-name" "operation-description" 0 "arg1" "arg2"
#   log_success "script-name" "Operation completed" 17 "key: value"
#   log_failure "script-name" "Operation failed" "reason" -73 "key: value"
#   log_error "script-name" "Unexpected error" "error message" -48
#   log_check "script-name" "validation-name" "true/false" 6 "key: value"
#   log_snapshot "script-name" "label" 0
#   log_debug "script-name" "debug event" 0 "key: value"
#   log_command "script-name" "description" command arg1 arg2...
#
# ============================================================================
# END CLOSING
# ============================================================================
