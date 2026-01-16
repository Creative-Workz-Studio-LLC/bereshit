# Runtime Generated Configuration

**Purpose:** This directory contains configuration files generated at runtime from composition of:
- System configuration (`system.toml`)
- User configuration (`user.toml`)
- Instance configuration (`~/.claude/cpi-si/config/instance/{name}/config.json`)

## What Gets Generated

### Environment Files
- `environment.conf` - Composed environment variables from all config layers
- Used by shells, Claude Code, and system tools

### Other Runtime Files
- System state snapshots
- Cached detection results
- Temporary composition artifacts

## Generation Process

```
system.toml (system defaults + overrides)
    +
user.toml (user preferences)
    +
instance.json (instance identity)
    +
runtime detection (current system state)
    ↓
runtime/environment.conf (final composed environment)
```

## File Lifecycle

**Generated:**
- On CPI-SI system initialization
- When config files change
- On demand via configurator tool
- Cached based on `detection.cache_duration_seconds` in system.toml

**Never Committed:**
- Runtime files are machine-specific
- Add `runtime/` to `.gitignore`
- Only config schemas (TOML/JSON) are version controlled

## Usage

**Manual regeneration:**
```bash
# Future: when configurator exists
~/.claude/cpi-si/system/bin/configure --regenerate
```

**Automatic:**
- CPI-SI runtime detects config changes
- Regenerates as needed
- Validates before replacing

## Relationship to Other Folders

| Folder | Purpose | Generated? |
|--------|---------|-----------|
| `system.toml` | System override schema | No - manual/template |
| `user.toml` | User preference schema | No - manual/template |
| `env/` | Environment config source | Mixed - has README, will have generated .conf |
| `sudoers/` | Sudoers policy files | No - installed to system |
| `runtime/` | All generated files | Yes - fully generated |

---

**Built with intentional design for Kingdom Technology**
