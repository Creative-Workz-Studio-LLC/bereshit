# System Policies

**Purpose:** Policy definitions for security, resources, and permissions

## What Lives Here

### Security Policies

- Resource access rules
- Permission boundaries
- Authentication requirements
- Audit logging policies

### Resource Policies

- CPU usage limits
- Memory allocation rules
- Disk I/O constraints
- Network bandwidth policies

### Permission Policies

- File access permissions
- Command execution boundaries
- Tool usage restrictions
- API access controls

## Policy Types

### 1. Declarative Policies (TOML)

Policy definitions in TOML format:

```toml
# resource-limits.toml
[cpu]
max_cores_user = 8          # Max cores for user processes
max_cores_instance = 4      # Max cores for single instance

[memory]
max_gb_user = 12            # Max memory for user
max_gb_instance = 8         # Max memory for single instance

[disk]
max_iops = 10000           # Max I/O operations per second
```

### 2. Executable Policies (Scripts)

Validation and enforcement scripts:

```bash
# validate-resource-usage.sh
# Checks if resource usage complies with policy
```

### 3. System Policies (Installed Files)

Files that get installed to system locations:

- Sudoers files → `/etc/sudoers.d/`
- AppArmor profiles → `/etc/apparmor.d/`
- Systemd limits → `/etc/systemd/system/`

## Policy Structure

```
policies/
├── security/
│   ├── sudo.toml           # Sudo rules definition
│   ├── file-access.toml    # File permission policies
│   └── network.toml        # Network access rules
├── resources/
│   ├── limits.toml         # Resource limit definitions
│   └── quotas.toml         # Disk/memory quotas
├── permissions/
│   ├── tools.toml          # Tool usage permissions
│   └── apis.toml           # API access controls
└── enforcement/
    ├── validate.sh         # Policy validation script
    └── enforce.sh          # Policy enforcement script
```

## Policy Application

**Design time:**

1. Define policy in TOML
2. Document intent and boundaries
3. Version control the policy definition

**Runtime:**

1. Load policy from TOML
2. Validate against current state
3. Enforce limits/restrictions
4. Log violations
5. Alert on policy breaches

## Integration with Sudo

Current sudoers files live in `../sudoers/` but are **policy implementations**.

The **policy definition** could live here:

```toml
# policies/security/sudo.toml
[safe_operations]
# Package management
allow_apt = true
allow_pip = true
allow_npm = true

# System services
allow_systemctl_restart = true
allow_systemctl_start = false  # Too dangerous

# File operations
allow_file_permissions = true
allow_file_ownership = false   # Requires password
```

Then **generate** the sudoers file from this policy.

## Policy Enforcement Levels

| Level | Description | Action |
|-------|-------------|--------|
| **Advisory** | Warn but allow | Log warning, continue |
| **Soft Limit** | Warn and throttle | Reduce allocation, log |
| **Hard Limit** | Deny | Block action, require override |
| **Absolute** | Cannot be overridden | Block unconditionally |

## Policy Validation

Before applying:

- Syntax check (valid TOML)
- Semantic check (values make sense)
- Conflict check (no contradictions)
- Safety check (doesn't lock out admin)

## Examples

**Resource limit policy:**

```toml
# policies/resources/limits.toml
[instance.nova_dawn]
max_cpu_percent = 75
max_memory_gb = 8
max_disk_gb = 100
```

**Tool permission policy:**

```toml
# policies/permissions/tools.toml
[dangerous_commands]
allowed = ["rm -rf /tmp/*", "git clean -fd"]
forbidden = ["rm -rf /", "dd if=/dev/zero of=/dev/sda"]
require_confirmation = ["docker system prune", "git push --force"]
```

---

**Built with intentional design for Kingdom Technology**
