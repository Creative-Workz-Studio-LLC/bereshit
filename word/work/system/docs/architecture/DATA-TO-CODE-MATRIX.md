# Data-to-Code-to-Config Matrix

**Purpose:** Complete mapping of which data files connect to which code libraries and configs
**Format:** Three-column relationship mapping (Data ↔ Code ↔ Config)

---

## Session Management Stack

### SessionState Data

**Data:** `system/data/session/current.json`
**Schema:** `system/data/session/schemas/session-state.json`
**Code:** `sessiontime/sessiontime.go`

**Current State:**

- ✅ Code loads from `current.json` correctly
- ✅ Schema exists for validation
- ❌ **Path hardcoded** in sessiontime.go (line ~95)

**Config Needed:**

```toml
# system/config/paths.toml
[session]
state_file = "system/data/session/current.json"
schema_dir = "system/data/session/schemas/"
```

**Functions Using This Data:**

- `LoadSessionState()` - Reads current.json
- `SaveSessionState()` - Writes current.json
- `GetSessionDuration()` - Calculates from start_time in state
- `UpdateLastActivity()` - Updates last_activity_time

---

### Pattern Memory Data

**Data:** `system/data/session/patterns.json`
**Schema:** `system/data/session/schemas/patterns.json`
**Code:** `patterns/patterns.go`

**Current State:**

- ✅ Code loads/saves patterns.json correctly
- ✅ Schema exists for validation
- ❌ **Path hardcoded** (likely in patterns.go)

**Config Needed:**

```toml
# system/config/paths.toml
[patterns]
memory_file = "system/data/session/patterns.json"
schema = "system/data/session/schemas/patterns.json"
```

**Functions Using This Data:**

- `LoadPatterns()` - Reads patterns.json
- `SavePatterns()` - Writes patterns.json
- `RecognizePattern()` - Matches against stored patterns
- `RecordPattern()` - Adds new pattern to memory

---

## Temporal System Stack

### Calendar Data (Base)

**Data:** `system/data/temporal/appointed/base/2025/*.json`
**Schema:** `system/data/temporal/appointed/schemas/calendar.json` (+ 6 more)
**Code:** `calendar/calendar.go`

**Current State:**

- ✅ Code reads calendar JSON files correctly
- ✅ Schemas exist for validation (7 total)
- ❌ **Path hardcoded** to `$HOME/.claude/cpi-si/system/calendar/base/`
- ⚠️ Note: Path uses `/system/calendar/` but data is in `/system/data/temporal/appointed/`

**Mismatch Alert:**
Code expects: `~/.claude/cpi-si/system/calendar/base/2025/`
Data located: `~/.claude/cpi-si/system/data/temporal/appointed/base/2025/`

**Either:**

1. Move data to match code expectations, OR
2. Update code to point to correct data location

**Config Needed:**

```toml
# system/config/temporal.toml
[calendar]
base_path = "system/data/temporal/appointed/base/"
personal_path = "system/data/temporal/appointed/personal/"
shared_path = "system/data/temporal/appointed/shared/"
projects_path = "system/data/temporal/appointed/projects/"
schema_dir = "system/data/temporal/appointed/schemas/"
```

**Functions Using This Data:**

- `LoadCalendar(year, month)` - Reads monthly calendar JSON
- `GetEventsForDate(date)` - Extracts events for specific date
- `GetMilestones()` - Retrieves milestone data

---

### Planner Templates

**Data:** `system/planner/templates/*.json`
**Schema:** ❌ **MISSING** (No planner schemas exist)
**Code:** `planner/planner.go`

**Current State:**

- ✅ Code reads planner template JSON files
- ❌ **No schemas** for validation
- ❌ **Path hardcoded** to `$HOME/.claude/cpi-si/system/planner/templates/`
- ❌ **User hardcoded** in temporal.go as `"seanje"`

**Config Needed:**

```toml
# system/config/temporal.toml
[planner]
templates_path = "system/planner/templates/"
schema_dir = "system/data/temporal/planner/schemas/"  # NEEDS CREATION

# User context (loaded from instance config instead of hardcoded)
[temporal.user]
username = "seanje-lenox-wise"  # From User config
```

**Schemas Needed:**

- `system/data/temporal/planner/schemas/template.json`
- `system/data/temporal/planner/schemas/daily-pattern.json`
- `system/data/temporal/planner/schemas/weekly-pattern.json`

**Functions Using This Data:**

- `LoadPlannerTemplate(user)` - Reads user's planner template
- `GetDailyPattern()` - Extracts daily schedule structure
- `GetWeeklyPattern()` - Extracts weekly rhythm

---

## Config Validation Stack

### Multi-Language Formatters

**Data:** `system/data/config/validation/formatters.jsonc`
**Schema:** `system/data/config/validation/schemas/formatters.json`
**Code:** `validation/formatter.go`

**Current State:**

- ✅ **FULLY CONFIG-DRIVEN** (exemplar implementation)
- ✅ Loads formatters.jsonc with graceful fallback
- ✅ Schema exists for validation
- ✅ Path construction uses relative logic (not fully hardcoded)

**Config Pattern to Replicate:**

```go
// validation/formatter.go shows good pattern:
homeDir, _ := os.UserHomeDir()
basePath := filepath.Join(homeDir, ".claude", "system", "data", "config", "validation")
formattersPath := filepath.Join(basePath, "formatters.jsonc")
```

**Better Pattern (for config-driven):**

```go
// Load path from config instead:
paths := config.LoadPaths()  // from paths.toml
formattersPath := paths.Config.Formatters
```

**Functions Using This Data:**

- `LoadFormatters()` - Reads formatters.jsonc
- `GetFormatterForLanguage(lang)` - Retrieves formatter config
- `FormatCode(language, code)` - Applies formatter

---

## Instance/User Configuration Stack

### User Configuration

**Data:** `config/user/seanje-lenox-wise/config.jsonc`
**Schema:** ❌ **MISSING** (No user config schema)
**Code:** `config/config.go`

**Current State:**

- ✅ Code parses JSONC correctly
- ✅ Supports inheritance (User → Instance → Project)
- ❌ No validation schema
- ⚠️ Paths constructed at runtime (not fully hardcoded, but not config-driven)

**Schema Needed:**

```bash
system/config/schemas/user-config.json
system/config/schemas/instance-config.json
system/config/schemas/project-config.json
```

**Functions Using This Data:**

- `LoadUserConfig(username)` - Loads user config with fallback to default
- `LoadInstanceConfig(instance)` - Loads instance config with fallback
- `MergeConfigs()` - Applies inheritance chain

---

### Instance Configuration

**Data:** `config/instance/nova_dawn/config.jsonc`
**Schema:** ❌ **MISSING**
**Code:** `instance/config.go`

**Current State:**

- ✅ Code loads instance config
- ❌ **Hardcoded default config** as Go struct (should load from default/config.jsonc)
- ❌ No validation schema
- ❌ Path construction hardcoded

**Config Needed:**
Load default from file instead of hardcoded struct:

```bash
config/instance/default/config.jsonc (exists)
  ↓
instance/config.go should load this as default
```

**Functions Using This Data:**

- `LoadInstanceConfig()` - Loads instance-specific config
- `GetDefaultConfig()` - **Should load from file, not hardcoded**

---

## Logging/Debugging Stack

### Log Files

**Data:** `system/logs/{commands,libraries,scripts,system}/*.log`
**Config:** ❌ **MISSING** `system/config/logging.toml`
**Code:** `logging/logger.go`

**Current State:**

- ✅ Logs written correctly
- ✅ Health scoring integrated
- ❌ **Component routing hardcoded** in variables (commandComponents, libraryComponents)
- ❌ **Log paths hardcoded** (directory structure in constants)
- ❌ **Health visualization config hardcoded** (healthRanges variable)

**Config Needed:**

```toml
# system/config/logging.toml
[paths]
base_dir = "system/logs"
subdirs = ["commands", "libraries", "scripts", "system"]

[routing]
commands = ["validate", "test", "status", "diagnose"]
libraries = ["operations", "sudoers", "environment", "display"]
# Add more as needed

[health.visualization]
ranges = [
  { threshold = 90, emoji = "💚", description = "Excellent" },
  { threshold = 80, emoji = "💙", description = "Very Good" },
  # ... (21 ranges total)
]
```

**Functions Needing Config:**

- `NewLogger(component)` - Routing logic needs config
- `determineLogSubdirectory(component)` - Uses hardcoded lists
- `getHealthIndicator(health)` - Uses hardcoded ranges

---

### Debug State Files

**Data:** `system/logs/` (reads from log files)
**Config:** ❌ **MISSING** `system/config/debugging.toml`
**Code:** `debugging/inspector.go`

**Current State:**

- ✅ Reads and analyzes log files correctly
- ❌ **Format constants hardcoded** (output styling)
- ❌ **Paths hardcoded** (log directory paths)

**Config Needed:**

```toml
# system/config/debugging.toml
[paths]
log_base = "system/logs"

[output]
detail_level = "full"  # or "summary", "minimal"
color_enabled = true
```

---

## Privacy Filtering Stack

### Privacy Patterns

**Data:** ❌ **NO DATA FILE** (patterns hardcoded in code)
**Config:** ❌ **MISSING** `system/config/privacy.toml`
**Code:** `privacy/privacy.go`

**Current State:**

- ⚠️ Privacy filter patterns **hardcoded in Go code**
- ❌ No data file for filter patterns
- ❌ No config for privacy rules

**Config Needed:**

```toml
# system/config/privacy.toml
[filters.patterns]
sensitive_keys = ["password", "api_key", "secret", "token"]
redaction_text = "[REDACTED]"

[filters.paths]
exclude_patterns = [".env", "credentials.json", "secrets.yaml"]
```

**Data File Needed:**

```json
// system/data/privacy/filters.json
{
  "sensitive_patterns": ["password", "secret", "token"],
  "path_exclusions": [".env", "credentials.*"],
  "redaction_policy": "full"  // or "partial", "hash"
}
```

**Functions Needing Config:**

- `FilterSensitive()` - Uses hardcoded patterns
- `RedactValue()` - Uses hardcoded redaction text
- `IsSensitivePath()` - Needs pattern matching

---

## Summary Matrix

| Data Location | Current Code | Config Status | Priority |
|---------------|--------------|---------------|----------|
| session/current.json | sessiontime/ | ❌ Hardcoded path | High |
| session/patterns.json | patterns/ | ❌ Hardcoded path | High |
| temporal/appointed/base/ | calendar/ | ❌ Hardcoded + wrong path | **CRITICAL** |
| planner/templates/ | planner/ + temporal/ | ❌ Hardcoded path + user | **CRITICAL** |
| config/validation/formatters.jsonc | validation/ | ✅ Config-driven | ✅ Done |
| config/user/*/config.jsonc | config/ | ⚠️ Runtime paths | Medium |
| config/instance/*/config.jsonc | instance/ | ❌ Hardcoded default | High |
| logs/*.log | logging/ | ❌ Hardcoded routing | High |
| logs/*.log | debugging/ | ❌ Hardcoded paths | Medium |
| (none) privacy patterns | privacy/ | ❌ Hardcoded in code | Medium |

---

## Critical Data Mismatches

### Calendar Path Mismatch

**Code expects:** `~/.claude/cpi-si/system/calendar/base/2025/`
**Data located:** `~/.claude/cpi-si/system/data/temporal/appointed/base/2025/`

**Resolution:** Update calendar.go to point to correct data location

### Planner User Hardcoding

**temporal.go line ~120:** `planner.LoadPlannerTemplate("seanje")`
**Should be:** Load username from User config or instance context

### Instance Default Config

**instance/config.go:** Hardcoded default struct
**Should be:** Load from `config/instance/default/config.jsonc` (file exists!)

---

## Config-Driven Refactoring Priority

### 🔴 **Phase 1: Critical Fixes (Do First)**

1. Fix calendar path mismatch (calendar.go)
2. Create paths.toml with centralized path config
3. Update planner/temporal to load user from config
4. Fix instance/config.go to load default from file

### 🟡 **Phase 2: Major Hardcoding (Do Second)**

1. Create logging.toml for component routing
2. Refactor logging/logger.go to load routing config
3. Create temporal.toml for temporal system config
4. Update sessiontime/ and patterns/ to load paths from config

### 🟢 **Phase 3: Polish (Do Third)**

1. Create privacy.toml and privacy filters data file
2. Create debugging.toml for debug output config
3. Add missing schemas (planner, user config, instance config)

---

This matrix provides surgical precision for updating each library to be config-driven.
