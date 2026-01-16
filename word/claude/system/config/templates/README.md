# Configuration Templates

**Purpose:** Template files used to generate runtime configurations

## What Lives Here

### Environment Templates

- Shell script templates for environment variable exports
- Format strings for different shell types (bash, zsh, fish)

### Config File Templates

- Templates for generating user-facing config files
- Placeholder-based templates with variable substitution

### Documentation Templates

- Generated documentation from config schemas
- Usage guides created from TOML definitions

## Template Format

Templates use a simple variable substitution format:

```bash
# Example: environment.conf.template
export DEBIAN_FRONTEND={{environment.package_management.debian_frontend}}
export PIP_NO_INPUT={{environment.package_management.pip_no_input}}

# User preferences
export EDITOR={{user.environment.editor.editor || "nano"}}
export GOPATH={{user.environment.development.gopath || "$HOME/go"}}

# Instance-specific (from instance config)
export CPI_SI_WORKSPACE={{instance.workspace.absolute_path}}
```

**Variable syntax:**

- `{{path.to.value}}` - Direct substitution
- `{{value || "default"}}` - With fallback
- `{{#if condition}}...{{/if}}` - Conditional blocks
- `{{#each items}}...{{/each}}` - Iteration

## Generation Process

```bash
Template File
    +
System TOML values
    +
User TOML values
    +
Instance JSON values
    +
Runtime detection
    ↓
Generated File (in runtime/)
```

## Template Types

| Type | Extension | Purpose | Output Location |
|------|-----------|---------|-----------------|
| Shell | `.sh.template` | Environment scripts | `runtime/` |
| Config | `.conf.template` | Configuration files | `runtime/` |
| Documentation | `.md.template` | Generated docs | `runtime/` or user-facing |

## Creating Templates

1. **Identify what needs generation** - Does this vary by system/user/instance?
2. **Create template file** - Use appropriate extension
3. **Add variable placeholders** - Reference config paths
4. **Document required values** - What must be defined for valid generation?
5. **Test generation** - Verify output with sample configs

## Example Templates

**`environment.conf.template`** - Main environment file:

- Composes system, user, and instance environment variables
- Exports all necessary variables for shell sessions
- Used by integration scripts

**`config-summary.md.template`** - Configuration documentation:

- Generated summary of current configuration
- Shows detected vs overridden values
- Helps users understand their setup

---

**Built with intentional design for Kingdom Technology**
