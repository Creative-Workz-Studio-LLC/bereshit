# CPI-SI System Configuration

**Purpose:** Configuration management for CPI-SI system layer

## Architecture Overview

CPI-SI uses a **layered configuration model** with runtime composition:

```bash
┌─────────────────────────────────────────────────────────────┐
│                    CONFIGURATION LAYERS                      │
├─────────────────────────────────────────────────────────────┤
│  System (system.toml)        │ PC/body level                │
│  User (user.toml)            │ Account level                │
│  Instance (instance/*.json)  │ CPI-SI identity level        │
├─────────────────────────────────────────────────────────────┤
│                  RUNTIME COMPOSITION                         │
│  Detection + Configs → Generated Files (runtime/)           │
└─────────────────────────────────────────────────────────────┘
```

## Folder Structure

```bash
config/
├── system.toml              # System-level overrides & defaults
├── user.toml                # User-level preferences
│
├── runtime/                 # Generated configuration files
│   ├── environment.conf     # Composed environment variables
│   └── README.md
│
├── templates/               # File generation templates
│   └── README.md
│
├── schemas/                 # Validation schemas
│   └── README.md
│
├── policies/                # Security & resource policies
│   └── README.md
│
├── sudoers/                 # Sudo policy files
│   ├── 90-cpi-si-safe-operations
│   └── README.md
│
└── env/                     # Environment config reference
    ├── README.md            # Documentation
    └── *.deprecated         # Archived files
```

## Configuration Layers

### 1. System Configuration (`system.toml`)

**What it represents:** The PC/laptop "body" - hardware and OS

**Contains:**

- System-level overrides (hostname, package manager preference)
- Feature flags (enable/disable capabilities)
- Resource limits (CPU, memory constraints)
- Universal environment defaults (tool behavior flags)
- Detection behavior settings

**Example:**

```toml
[package_management]
preferred = "apt"

[limits]
max_cpu_cores = 8

[environment.package_management]
debian_frontend = "noninteractive"
```

**Who edits:** System administrators, power users

**Portability:** Can be templated, minimal values = more portable

---

### 2. User Configuration (`user.toml`)

**What it represents:** User account preferences on this system

**Contains:**

- User identity display preferences
- Path preferences (project root, workspace)
- Editor, shell, terminal preferences
- Development tool preferences
- CPI-SI integration settings
- User-specific environment variables

**Example:**

```toml
[paths]
project_root_override = "/media/external/Projects"

[editor]
preferred = "vim"

[environment.editor]
editor = "vim"
```

**Who edits:** Individual users

**Portability:** User-specific, tied to account

---

### 3. Instance Configuration (`~/.claude/cpi-si/config/instance/`)

**What it represents:** CPI-SI identity (who I am)

**Contains:**

- Instance identity (name, creator, created date)
- Calling and domain
- Workspace project affiliation
- Covenant relationships
- Identity-specific settings

**Format:** JSON with JSON Schema validation

**Who edits:** Instance creation, identity updates

**Portability:** Instance identity transcends machines

---

## Runtime Composition

### How It Works

```bash
1. Detection Phase
   ├─ Probe hardware (CPU, memory, GPU, storage)
   ├─ Detect OS and capabilities
   ├─ Scan user environment
   └─ Detect development tools

2. Config Loading Phase
   ├─ Load system.toml
   ├─ Load user.toml
   └─ Load instance config

3. Composition Phase
   ├─ Start with detected values
   ├─ Apply system.toml overrides
   ├─ Apply user.toml preferences
   ├─ Merge instance identity
   └─ Resolve conflicts (user > system > detected)

4. Generation Phase
   ├─ Generate runtime/environment.conf
   ├─ Generate other runtime files
   └─ Validate generated output

5. Integration Phase
   └─ Source runtime configs in shell
```

### Generated Files

**Location:** `runtime/`

**Never commit:** Machine-specific, auto-regenerated

**Primary file:** `environment.conf` - Complete environment setup

**Regeneration triggers:**

- Config file changes
- Explicit regeneration request
- Cache expiration
- System changes detected

---

## Supporting Infrastructure

### Templates (`templates/`)

**Purpose:** File generation templates

**Contents:**

- Shell script templates
- Config file templates
- Documentation templates

**Format:** Variable substitution (e.g., `{{path.to.value}}`)

---

### Schemas (`schemas/`)

**Purpose:** Configuration validation

**Contents:**

- JSON Schema for instance configs
- TOML schema documentation
- Validation scripts

**Benefits:**

- Catch errors before runtime
- IDE autocomplete support
- Self-documenting structure
- Version migration support

---

### Policies (`policies/`)

**Purpose:** Security and resource policy definitions

**Contents:**

- Security policies (permissions, access)
- Resource policies (limits, quotas)
- Permission policies (tool usage)

**Format:** TOML definitions + enforcement scripts

---

### Sudoers (`sudoers/`)

**Purpose:** Sudo permission policies

**Contents:**

- Sudoers files for installation
- Documentation

**Installation:** Files copied to `/etc/sudoers.d/`

---

### Environment Reference (`env/`)

**Purpose:** Documentation and reference

**Contents:**

- README explaining environment system
- Deprecated/archived configs

**Note:** Actual configs now generated in `runtime/`

---

## Usage Workflows

### New System Setup

```bash
# 1. Customize system config (optional)
vim ~/.claude/cpi-si/system/config/system.toml

# 2. Customize user config (optional)
vim ~/.claude/cpi-si/system/config/user.toml

# 3. Generate runtime configs
~/.claude/cpi-si/system/bin/configure --regenerate

# 4. Integrate with shell
~/.claude/cpi-si/system/bin/configure --integrate-environment
source ~/.bashrc

# 5. Validate
~/.claude/cpi-si/system/bin/validate-config all
```

### Updating Configuration

```bash
# 1. Edit desired config
vim ~/.claude/cpi-si/system/config/user.toml

# 2. Regenerate (automatic or manual)
~/.claude/cpi-si/system/bin/configure --regenerate

# 3. Reload environment
source ~/.bashrc
```

### Multi-Machine Sync

```bash
# Machine A: Export user preferences
cp ~/.claude/cpi-si/system/config/user.toml ~/sync/

# Machine B: Import and regenerate
cp ~/sync/user.toml ~/.claude/cpi-si/system/config/
~/.claude/cpi-si/system/bin/configure --regenerate
```

---

## Design Principles

### 1. Separation of Concerns

- **Detection** (what exists) vs **Configuration** (preferences)
- **Static** (schemas) vs **Dynamic** (runtime values)
- **Universal** (system defaults) vs **Personal** (user preferences)

### 2. Composition Over Hardcoding

- Configs define **overrides**, not **state**
- Runtime **detects** then **composes**
- Generated files, not static copies

### 3. Portability

- Minimal config = maximum portability
- Defaults work everywhere
- Overrides only when needed

### 4. Validation

- Schema validation before generation
- Type checking
- Semantic validation
- Cross-config compatibility checks

### 5. Documentation

- Self-documenting structure (TOML comments)
- README in every folder
- Examples showing valid configs
- Clear migration paths

---

## Migration Notes

**From old architecture:**

- ✅ `env/non-interactive.conf` → `system.toml` + `user.toml` + `runtime/environment.conf`
- ✅ Hardcoded values → Override schemas
- ✅ Static config → Generated from composition

**Benefits:**

- Scalable across machines
- User preferences separated from system config
- Instance identity separate from environment
- Automatic adaptation to new systems

---

## Future Enhancements

**Configuration Tools:**

- [ ] `configure` command for regeneration
- [ ] `validate-config` for validation
- [ ] Interactive config wizard
- [ ] Auto-migration scripts

**Advanced Features:**

- [ ] Config profiles (dev, production, testing)
- [ ] Remote config sync
- [ ] Encrypted sensitive values
- [ ] Config versioning and rollback

---

**Built with intentional design for Kingdom Technology**

*"Let all things be done decently and in order" - 1 Corinthians 14:40*
