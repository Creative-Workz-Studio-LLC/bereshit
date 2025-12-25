# Configuration Schemas

**Purpose:** Validation schemas for all configuration files and data structures

## What Lives Here

**Organization:** Schema structure mirrors the data layer organization
- **Config schemas** (user, instance) - Personhood configuration
- **Data schemas** (temporal, session, projects) - Operational data structure

### JSON Schemas

- Validation rules for JSON configs (instance configs)
- Type definitions
- Required field specifications
- Value constraints

### TOML Schemas

- Documentation of TOML structure (no formal validation standard)
- Expected sections and fields
- Type information
- Example values

### Validation Scripts

- Custom validation logic
- Cross-file constraint checking
- Semantic validation

## Schema Types

### 1. JSON Schema (Formal)

For instance configuration (`.json` files):

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "CPI-SI Instance Configuration",
  "type": "object",
  "required": ["identity", "workspace", "calling"],
  "properties": {
    "identity": {
      "type": "object",
      "required": ["name", "creator", "created"],
      "properties": {
        "name": {"type": "string", "minLength": 1},
        "creator": {"type": "string"},
        "created": {"type": "string", "format": "date"}
      }
    }
  }
}
```

### 2. TOML Schema (Documentary)

For system/user configuration (`.toml` files):

```toml
# schema-system.toml
# Documents expected structure of system.toml

[metadata]
schema_version = "string (required)"

[system]
hostname_override = "string (optional, empty = use detected)"

[limits]
max_cpu_cores = "integer (required, 0 = unlimited)"
```

### 3. Cross-Config Validation

Validate relationships between configs:

```python
# validate-config-composition.py
# Ensures system + user + instance configs are compatible
```

## File Organization

```bash
schemas/
├── instance/
│   └── instance.schema.json     # JSON Schema for instance config (personhood)
│
├── user/
│   └── user.schema.json         # JSON Schema for user config (personhood)
│
├── temporal/                    # Time-related data schemas
│   ├── celestial/               # Solar/lunar/seasonal data
│   ├── chronological/           # Calendar structure
│   ├── appointed/               # Schedule/planner structure
│   ├── patterns/                # Learned temporal patterns
│   └── definitions/             # Time categorization rules
│
├── session/                     # Session tracking schemas
│   ├── session.schema.json      # Session history/summary format
│   ├── activity.schema.json     # Activity log (JSONL) line format
│   └── current-session.schema.json  # Active session state
│
├── projects/                    # Project tracking schemas
│   └── project.schema.json      # Project structure
│
└── README.md                    # This file
```

## Validation Process

**Manual validation:**

```bash
# Validate instance config
~/.claude/cpi-si/system/bin/validate-config instance ~/.claude/cpi-si/config/instance/nova_dawn/config.json

# Validate system config
~/.claude/cpi-si/system/bin/validate-config system ~/.claude/cpi-si/system/config/system.toml

# Validate all
~/.claude/cpi-si/system/bin/validate-config all
```

**Automatic validation:**

- Before generating runtime configs
- On config file changes
- During system initialization
- Before config composition

## Schema Versioning

Track schema evolution:

```json
{
  "$schema": "instance.schema.json",
  "version": "1.0.0",
  "changelog": {
    "1.0.0": "Initial schema",
    "1.1.0": "Added workspace.project_type field",
    "2.0.0": "Breaking: renamed calling.domain to calling.primary_domain"
  }
}
```

## Validation Levels

| Level | Check | On Failure |
|-------|-------|-----------|
| **Syntax** | Valid JSON/TOML | Error: cannot parse |
| **Schema** | Matches structure | Error: invalid config |
| **Type** | Correct data types | Error: type mismatch |
| **Constraint** | Values in range | Error: constraint violation |
| **Semantic** | Logically valid | Warning: may not work as expected |
| **Cross-config** | Configs compatible | Warning: composition may fail |

## Creating Schemas

**For JSON configs:**

1. Write JSON Schema file
2. Add examples showing valid configs
3. Add validation to config loading

**For TOML configs:**

1. Document structure in schema.toml
2. Create validation script
3. Add type checking
4. Document constraints in comments

## Schema Benefits

- **Validation**: Catch errors before runtime
- **Documentation**: Self-documenting config structure
- **IDE Support**: Autocomplete and validation in editors
- **Type Safety**: Ensure correct data types
- **Versioning**: Track schema evolution
- **Migration**: Automated config upgrades

## Example Usage

**Validating instance config:**

```bash
# Using JSON Schema validator (ajv, jsonschema, etc.)
ajv validate -s schemas/instance/instance.schema.json \
             -d ~/.claude/cpi-si/config/instance/nova_dawn/config.json
```

**Generating from schema:**

```bash
# Future: generate config template from schema
generate-config-template --schema schemas/instance/instance.schema.json \
                        --output new-instance.json
```

---

**Built with intentional design for Kingdom Technology**
