# Semantic Metadata Architecture for Restoration Routing

**Status**: Design Phase
**Purpose**: Enable debugger to route divergences to appropriate restoration antibodies
**Dependencies**: Detection (logging), Assessment (debugging)

---

## The Problem

Divergence detection identifies **THAT** something went wrong and **HOW MUCH** health was lost. But restoration needs to know:

- **WHAT** operation failed (operation taxonomy)
- **WHY** it failed (error classification)
- **HOW** to fix it (antibody routing)

Without semantic metadata, the debugger can detect `partial-success` with `-20 health` but cannot determine if this needs:
- File permission fixes
- Missing dependency installation
- Configuration correction
- Manual intervention

---

## Operation Taxonomy

Operations are classified by domain to route to appropriate restoration antibodies.

### Primary Categories

| Category | Examples | Restoration Domain |
|----------|----------|-------------------|
| **file_validation** | Check file exists, verify permissions, validate syntax | File system antibodies |
| **file_transformation** | CRLF→LF conversion, encoding fixes, format changes | Transformation antibodies |
| **config_validation** | Sudoers syntax, environment variables, shell integration | Configuration antibodies |
| **system_operation** | Package install, service control, permission changes | System antibodies |
| **parsing** | Log parsing, config parsing, data extraction | Parsing antibodies |
| **analysis** | Health assessment, pattern recognition, divergence detection | Analysis antibodies (rarely fail) |
| **io_operation** | File read/write, directory scan, network access | I/O antibodies |

### Granular Sub-types

Each category can specify sub-types for precise routing:

```go
operation_type: "file_validation"
operation_subtype: "syntax_check"  // vs "permission_check", "existence_check"

operation_type: "system_operation"
operation_subtype: "package_install"  // vs "service_control", "permission_change"
```

---

## Error Classification

Error types determine which antibodies can address the problem.

### Core Error Types

| Error Type | Meaning | Typical Causes | Recovery Strategy |
|------------|---------|----------------|-------------------|
| **file_not_found** | Required file missing | Not created, wrong path, deleted | Create file, verify path |
| **permission_denied** | Insufficient permissions | Wrong file mode, not in group | Fix permissions, add to group |
| **parse_error** | Data malformed | Syntax error, wrong format, corruption | Fix syntax, regenerate file |
| **validation_error** | Data incorrect but parseable | Wrong values, missing fields | Update configuration |
| **missing_dependency** | Required resource unavailable | Package not installed, service down | Install package, start service |
| **timeout** | Operation took too long | Slow I/O, hanging process | Retry, increase timeout |
| **unexpected_value** | Got valid but wrong data | Logic error, stale data | Recalculate, refresh data |
| **resource_exhausted** | System resource depleted | Disk full, memory exhausted | Free resources, expand capacity |

### Error Details Structure

Beyond the error type, capture details for restoration:

```go
error_type: "permission_denied"
error_details: {
    "file": "/etc/sudoers.d/90-cpi-si-safe-operations",
    "required_mode": "0440",
    "actual_mode": "0644",
    "required_owner": "root:root",
    "actual_owner": "root:seanje"
}
```

---

## Recovery Hints

Recovery hints map divergence patterns to antibody selection.

### Hint Categories

| Hint | Meaning | Antibody Domain |
|------|---------|-----------------|
| **automated_fix** | Known issue, safe to auto-fix | File/config antibodies |
| **install_dependency** | Missing package/service | System antibodies |
| **update_config** | Configuration needs correction | Config antibodies |
| **retry** | Transient failure, retry may succeed | Retry antibodies |
| **manual_intervention** | Requires human decision | Alert system, no auto-fix |
| **investigate** | Novel failure, needs analysis | Meta-learning antibodies |

### Routing Logic

```go
// Example: Permission divergence
recovery_hint: "automated_fix"
recovery_strategy: "fix_file_permissions"
recovery_params: {
    "file": "/path/to/file",
    "target_mode": "0440",
    "target_owner": "root:root"
}

// Example: Missing dependency
recovery_hint: "install_dependency"
recovery_strategy: "install_package"
recovery_params: {
    "package": "visudo",
    "check_command": "which visudo"
}

// Example: Novel failure
recovery_hint: "investigate"
recovery_strategy: "manual_review"
recovery_params: {
    "alert_level": "high",
    "context_file": "/path/to/full/logs"
}
```

---

## Integration with Logger and Inspector

### Logger Enhancements

The logger needs methods to capture semantic metadata:

```go
// Enhanced Check with semantic metadata
logger.CheckWithMetadata("validate-sudoers-syntax", true, 10, map[string]any{
    "result": true,
}, Metadata{
    OperationType:    "file_validation",
    OperationSubtype: "syntax_check",
})

// Enhanced Failure with semantic metadata
logger.FailureWithMetadata("sudoers-permission-denied", "incorrect file permissions", -10, map[string]any{
    "file": "/etc/sudoers.d/90-cpi-si-safe-operations",
    "expected_mode": "0440",
    "actual_mode": "0644",
}, Metadata{
    OperationType:    "file_validation",
    OperationSubtype: "permission_check",
    ErrorType:        "permission_denied",
    ErrorDetails: map[string]any{
        "file": "/etc/sudoers.d/90-cpi-si-safe-operations",
        "required_mode": "0440",
        "actual_mode": "0644",
    },
    RecoveryHint:     "automated_fix",
    RecoveryStrategy: "fix_file_permissions",
    RecoveryParams: map[string]any{
        "file": "/etc/sudoers.d/90-cpi-si-safe-operations",
        "target_mode": "0440",
        "target_owner": "root:root",
    },
})
```

### Inspector Enhancements

The inspector needs methods to capture expected vs actual contracts:

```go
// Enhanced ExpectedState with semantic metadata
inspector.ExpectedStateWithMetadata("sudoers-file-exists", true, fileExists, map[string]any{
    "file": sudoersPath,
    "exists": fileExists,
}, Metadata{
    OperationType:    "file_validation",
    OperationSubtype: "existence_check",
    Expected: map[string]any{
        "exists": true,
        "mode": "0440",
        "owner": "root:root",
    },
    Actual: map[string]any{
        "exists": fileExists,
        "mode": actualMode,
        "owner": actualOwner,
    },
})
```

---

## Metadata Structure

```go
// Metadata captures semantic information for restoration routing
type Metadata struct {
    // Operation classification
    OperationType    string // Primary category (file_validation, system_operation, etc.)
    OperationSubtype string // Granular sub-type (syntax_check, permission_check, etc.)

    // Error information (only for failures)
    ErrorType    string         // Error classification (permission_denied, file_not_found, etc.)
    ErrorDetails map[string]any // Structured error context

    // Recovery routing
    RecoveryHint     string         // Hint for restoration routing (automated_fix, manual_intervention, etc.)
    RecoveryStrategy string         // Specific antibody to use (fix_file_permissions, install_package, etc.)
    RecoveryParams   map[string]any // Parameters for antibody execution

    // State contracts (inspector usage)
    Expected map[string]any // Expected state
    Actual   map[string]any // Actual state
}
```

---

## Log Format Enhancements

Semantic metadata is captured in logs for debugger consumption:

```
[2025-10-27 22:30:15.123] FAILURE | validate | user@host:12345 | validate-12345-token | HEALTH: -10
  EVENT: sudoers-permission-denied
  DETAILS:
    file: /etc/sudoers.d/90-cpi-si-safe-operations
    expected_mode: 0440
    actual_mode: 0644
    reason: incorrect file permissions
  SEMANTIC:
    operation_type: file_validation
    operation_subtype: permission_check
    error_type: permission_denied
    error_details:
      file: /etc/sudoers.d/90-cpi-si-safe-operations
      required_mode: 0440
      actual_mode: 0644
    recovery_hint: automated_fix
    recovery_strategy: fix_file_permissions
    recovery_params:
      file: /etc/sudoers.d/90-cpi-si-safe-operations
      target_mode: 0440
      target_owner: root:root
---
```

---

## Debugger Consumption

The debugger reads semantic metadata and generates restoration routing:

```go
// Enhanced HealthDivergence with restoration routing
type HealthDivergence struct {
    // Existing fields
    CheckName       string
    Expected        int
    Actual          int
    Gap             int
    Severity        string
    Pattern         string

    // New: Restoration routing
    OperationType    string
    OperationSubtype string
    ErrorType        string
    ErrorDetails     map[string]any
    RecoveryHint     string
    RecoveryStrategy string
    RecoveryParams   map[string]any

    // Antibody selection
    AntibodyID       string // Which antibody to invoke
    Priority         int    // Execution priority (critical first)
    Confidence       string // "high", "medium", "low" - how confident is the routing?
}
```

---

## Restoration Output Format

The debugger generates restoration decisions:

```json
{
  "timestamp": "2025-10-27T22:30:15Z",
  "component": "validate",
  "divergences": [
    {
      "check_name": "sudoers-permission-denied",
      "pattern": "complete-failure",
      "severity": "critical",
      "health_gap": -10,
      "operation": {
        "type": "file_validation",
        "subtype": "permission_check"
      },
      "error": {
        "type": "permission_denied",
        "details": {
          "file": "/etc/sudoers.d/90-cpi-si-safe-operations",
          "required_mode": "0440",
          "actual_mode": "0644"
        }
      },
      "restoration": {
        "hint": "automated_fix",
        "strategy": "fix_file_permissions",
        "antibody_id": "file-permission-fixer",
        "priority": 1,
        "confidence": "high",
        "params": {
          "file": "/etc/sudoers.d/90-cpi-si-safe-operations",
          "target_mode": "0440",
          "target_owner": "root:root"
        }
      }
    }
  ]
}
```

---

## Implementation Phases

### Phase 1: Logger and Inspector Enhancement
- Add Metadata struct to lib/logging and lib/debugging
- Add WithMetadata methods to logger and inspector
- Update log format to capture SEMANTIC section
- Maintain backward compatibility (non-metadata methods still work)

### Phase 2: Debugger Enhancement
- Parse SEMANTIC sections from logs
- Populate restoration routing fields in HealthDivergence
- Generate restoration output format
- Add antibody selection logic

### Phase 3: Restoration Layer
- Build lib/restoration/ with antibody registry
- Implement core antibodies (file permissions, config updates, etc.)
- Build cmd/restorer/ to execute routing decisions
- Add dry-run mode for safety

### Phase 4: Meta-Learning
- Track antibody success rates
- Identify patterns requiring new antibodies
- Refine routing confidence over time

---

## Success Criteria

1. **Debugger can route divergences** - Each divergence includes restoration routing
2. **Antibodies can execute safely** - Automated fixes work without human intervention for known patterns
3. **Novel failures escalate properly** - Unknown patterns route to manual review
4. **Meta-learning improves over time** - System recognizes new patterns and suggests new antibodies

---

## Open Questions

1. **Metadata verbosity** - How much semantic metadata is too much?
2. **Backward compatibility** - Should old logs without SEMANTIC section still work?
3. **Confidence thresholds** - At what confidence level do we auto-execute vs alert?
4. **Antibody safety** - What operations are safe for automated execution?

---

## Next Steps

1. ✅ Design complete
2. Implement Metadata struct in lib/logging and lib/debugging
3. Add WithMetadata methods to logger
4. Update log format with SEMANTIC section
5. Test with divergence-demo
6. Update debugger to parse SEMANTIC and generate routing
7. Begin restoration layer prototyping
