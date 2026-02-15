// #!omni code --go
//
// seed-platform — Populate L2 platform database from TOML specs.
//
// Reads all TOML specifications under L2-platform/ladder/
// and seeds a SQLite database at data/platform/database/platform.db.
//
// The triangle: TOML (Configuration) → SQLite (Data) → Go/Rust (Code)
//
// "And God called the firmament Heaven." — Genesis 1:8

package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strings"

	"github.com/BurntSushi/toml"
	_ "modernc.org/sqlite"
)

// ═══════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════

const schema = `
-- L2 Platform Database Schema
-- "And God called the firmament Heaven." — Genesis 1:8
--
-- Tables from 13 TOML specs across 6 domains:
--   Health (score, log, dar, diagnostics, restore, provider)
--   Permission (access)
--   Filesystem (contract, types)
--   System (monitoring)
--   Temporal (boundaries)
--   Terminal (display)
--   Deploy (hosts)

PRAGMA journal_mode = WAL;
PRAGMA foreign_keys = ON;

-- ─────────────────────────────────────────────────────────────
-- specs: Metadata from every TOML spec file
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS specs (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    key         TEXT    NOT NULL UNIQUE,
    file        TEXT    NOT NULL,
    title       TEXT    NOT NULL,
    type        TEXT    NOT NULL DEFAULT '',
    component   TEXT    NOT NULL DEFAULT '',
    role        TEXT    NOT NULL DEFAULT '',
    domain      TEXT    NOT NULL DEFAULT 'platform',
    layer       TEXT    NOT NULL DEFAULT 'L2',
    version     TEXT    NOT NULL DEFAULT '',
    status      TEXT    NOT NULL DEFAULT 'Active',
    scripture   TEXT    NOT NULL DEFAULT '',
    principle   TEXT    NOT NULL DEFAULT '',
    tags        TEXT    NOT NULL DEFAULT '',
    provides    TEXT    NOT NULL DEFAULT '',
    created     TEXT    NOT NULL DEFAULT '',
    updated     TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- dependencies: Cross-references between specs
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS dependencies (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    spec_key    TEXT    NOT NULL,
    depends_on  TEXT    NOT NULL,
    relation    TEXT    NOT NULL DEFAULT 'requires',
    UNIQUE(spec_key, depends_on, relation)
);

-- ─────────────────────────────────────────────────────────────
-- health_levels: 7 health levels from score.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS health_levels (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    description   TEXT    NOT NULL DEFAULT '',
    hebrew        TEXT    NOT NULL DEFAULT '',
    min_stored    INTEGER NOT NULL,
    max_stored    INTEGER NOT NULL,
    ternary_min   INTEGER NOT NULL,
    ternary_max   INTEGER NOT NULL,
    direction     TEXT    NOT NULL DEFAULT '',
    priority      TEXT    NOT NULL DEFAULT '',
    scripture     TEXT    NOT NULL DEFAULT '',
    emoji         TEXT    NOT NULL DEFAULT '',
    color         TEXT    NOT NULL DEFAULT '',
    ansi_code     TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- health_normalization: Normalization bases from score.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS health_normalization (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    description   TEXT    NOT NULL DEFAULT '',
    step          INTEGER NOT NULL,
    points        INTEGER NOT NULL,
    use_case      TEXT    NOT NULL DEFAULT '',
    key_points    TEXT    NOT NULL DEFAULT '[]'
);

-- ─────────────────────────────────────────────────────────────
-- health_scoring: Calculation rules from score.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS health_scoring (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    rule_name     TEXT    NOT NULL UNIQUE,
    rule_value    TEXT    NOT NULL DEFAULT '',
    description   TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- health_log_actions: Action types from log.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS health_log_actions (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    description   TEXT    NOT NULL DEFAULT '',
    delta         TEXT    NOT NULL DEFAULT '',
    examples      TEXT    NOT NULL DEFAULT '[]',
    scripture     TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- dar_phases: DAR phases from dar.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS dar_phases (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    idx           INTEGER NOT NULL,
    balanced      INTEGER NOT NULL,
    description   TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- dar_aspects: DAR aspects from dar.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS dar_aspects (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    idx           INTEGER NOT NULL,
    balanced      INTEGER NOT NULL,
    description   TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- dar_severity: DAR severity levels from dar.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS dar_severity (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    idx           INTEGER NOT NULL,
    balanced      INTEGER NOT NULL,
    description   TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- dar_restore_results: Restore outcomes from dar.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS dar_restore_results (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    value         INTEGER NOT NULL,
    description   TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- dar_restore_strategies: Restore strategies from dar.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS dar_restore_strategies (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    value         TEXT    NOT NULL DEFAULT '',
    description   TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- permission_states: Permission states from access.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS permission_states (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    value         INTEGER NOT NULL,
    description   TEXT    NOT NULL DEFAULT '',
    meaning       TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- permission_action_categories: Action categories from access.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS permission_action_categories (
    id               INTEGER PRIMARY KEY AUTOINCREMENT,
    name             TEXT    NOT NULL UNIQUE,
    description      TEXT    NOT NULL DEFAULT '',
    examples         TEXT    NOT NULL DEFAULT '[]',
    minimum_state    TEXT    NOT NULL DEFAULT '',
    health_threshold INTEGER NOT NULL DEFAULT 0
);

-- ─────────────────────────────────────────────────────────────
-- permission_gating: Health→permission mapping from access.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS permission_gating (
    id               INTEGER PRIMARY KEY AUTOINCREMENT,
    health_level     TEXT    NOT NULL UNIQUE,
    permission_state TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- filesystem_biblical_extensions: Biblical file types from types.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS filesystem_biblical_extensions (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    hebrew        TEXT    NOT NULL DEFAULT '',
    meaning       TEXT    NOT NULL DEFAULT '',
    extensions    TEXT    NOT NULL DEFAULT '[]'
);

-- ─────────────────────────────────────────────────────────────
-- system_monitoring: Thresholds from monitoring.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS system_monitoring (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    metric        TEXT    NOT NULL,
    level         TEXT    NOT NULL,
    threshold     REAL    NOT NULL,
    icon          TEXT    NOT NULL DEFAULT '',
    description   TEXT    NOT NULL DEFAULT '',
    health_mapping TEXT   NOT NULL DEFAULT '',
    UNIQUE(metric, level)
);

-- ─────────────────────────────────────────────────────────────
-- temporal_boundaries: Time classifications from boundaries.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS temporal_boundaries (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    category      TEXT    NOT NULL,
    name          TEXT    NOT NULL,
    start_value   INTEGER NOT NULL DEFAULT 0,
    end_value     INTEGER NOT NULL DEFAULT 0,
    icon          TEXT    NOT NULL DEFAULT '',
    color         TEXT    NOT NULL DEFAULT '',
    description   TEXT    NOT NULL DEFAULT '',
    extra         TEXT    NOT NULL DEFAULT '',
    UNIQUE(category, name)
);

-- ─────────────────────────────────────────────────────────────
-- terminal_display: Display config from display.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS terminal_display (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    category      TEXT    NOT NULL,
    name          TEXT    NOT NULL,
    value         TEXT    NOT NULL DEFAULT '',
    description   TEXT    NOT NULL DEFAULT '',
    UNIQUE(category, name)
);

-- ─────────────────────────────────────────────────────────────
-- deployment_hosts: Host definitions from hosts.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS deployment_hosts (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    address       TEXT    NOT NULL,
    username      TEXT    NOT NULL,
    arch          TEXT    NOT NULL,
    is_primary    INTEGER NOT NULL DEFAULT 0,
    prod_dir      TEXT    NOT NULL DEFAULT '',
    prod_bin      TEXT    NOT NULL DEFAULT '',
    description   TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- server_config: Server settings from hosts.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS server_config (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    key           TEXT    NOT NULL UNIQUE,
    value         TEXT    NOT NULL DEFAULT '',
    description   TEXT    NOT NULL DEFAULT ''
);

-- Indexes for common queries
CREATE INDEX IF NOT EXISTS idx_specs_domain ON specs(domain);
CREATE INDEX IF NOT EXISTS idx_health_levels_direction ON health_levels(direction);
CREATE INDEX IF NOT EXISTS idx_dar_phases_balanced ON dar_phases(balanced);
CREATE INDEX IF NOT EXISTS idx_permission_gating_state ON permission_gating(permission_state);
CREATE INDEX IF NOT EXISTS idx_dependencies_spec ON dependencies(spec_key);
`

// ═══════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════

func main() {
	root := findRoot()
	if root == "" {
		log.Fatal("Cannot find b-word root. Run from within the b-word directory tree.")
	}

	dbPath := filepath.Join(root, "data", "platform", "database", "platform.db")
	fmt.Printf("Seeding platform database: %s\n", dbPath)

	// Ensure directory exists
	os.MkdirAll(filepath.Dir(dbPath), 0755)

	// Remove existing database to rebuild fresh
	os.Remove(dbPath)

	db, err := sql.Open("sqlite", dbPath)
	if err != nil {
		log.Fatalf("open database: %v", err)
	}
	defer db.Close()

	if _, err := db.Exec(schema); err != nil {
		log.Fatalf("create schema: %v", err)
	}

	ladderDir := filepath.Join(root, "L2-platform", "ladder")

	// Seed specs from all 13 TOML files
	specFiles := []string{
		"os/health/score.toml",
		"os/health/log.toml",
		"os/health/dar.toml",
		"os/health/diagnostics.toml",
		"os/health/restore.toml",
		"os/health/provider.toml",
		"os/permission/access.toml",
		"filesystem/contract.toml",
		"filesystem/types.toml",
		"os/system/monitoring.toml",
		"os/temporal/boundaries.toml",
		"os/terminal/display.toml",
		"os/deploy/hosts.toml",
	}
	for _, f := range specFiles {
		path := filepath.Join(ladderDir, f)
		if _, err := os.Stat(path); err == nil {
			seedSpec(db, path)
		}
	}

	// Seed domain-specific tables
	seedHealthLevels(db, ladderDir)
	seedHealthNormalization(db, ladderDir)
	seedHealthScoring(db, ladderDir)
	seedHealthLogActions(db, ladderDir)
	seedDARPhases(db, ladderDir)
	seedDARRestoreResults(db, ladderDir)
	seedDARRestoreStrategies(db, ladderDir)
	seedPermissionStates(db, ladderDir)
	seedPermissionActionCategories(db, ladderDir)
	seedPermissionGating(db, ladderDir)
	seedFilesystemBiblicalExtensions(db, ladderDir)

	// New domain tables (Phase 1B)
	seedSystemMonitoring(db, ladderDir)
	seedTemporalBoundaries(db, ladderDir)
	seedTerminalDisplay(db, ladderDir)
	seedDeploymentHosts(db, ladderDir)
	seedServerConfig(db, ladderDir)

	fmt.Println("\nDone.")
	printSummary(db)
}

func findRoot() string {
	dir, _ := os.Getwd()
	for {
		if _, err := os.Stat(filepath.Join(dir, "Cargo.toml")); err == nil {
			if _, err := os.Stat(filepath.Join(dir, "go.mod")); err == nil {
				return dir
			}
		}
		parent := filepath.Dir(dir)
		if parent == dir {
			return ""
		}
		dir = parent
	}
}

// ─────────────────────────────────────────────────────────────
// Seed specs + dependencies (metadata from all TOML files)
// ─────────────────────────────────────────────────────────────

func seedSpec(db *sql.DB, path string) {
	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		log.Printf("Warning: parsing %s: %v", path, err)
		return
	}

	pragma := getMap(raw, "_pragma")
	meta := getMap(raw, "_metadata")

	p1 := getMap(pragma, "P1_core")
	key := getString(p1, "key")
	if key == "" {
		key = filepath.Base(path)
	}

	m1 := getMap(meta, "M1_identity")
	m2 := getMap(meta, "M2_state")
	m4 := getMap(meta, "M4_grounding")
	m5 := getMap(meta, "M5_dependencies")
	m7 := getMap(meta, "M7_classification")

	p3 := getMap(pragma, "P3_instance")
	provides := getStringArray(p3, "provides")

	title := getString(m1, "title")
	if title == "" {
		title = getString(getMap(pragma, "P5_summary"), "title")
	}

	file := getString(m1, "file")
	if file == "" {
		file = filepath.Base(path)
	}

	_, err := db.Exec(`INSERT OR REPLACE INTO specs
		(key, file, title, type, component, role, domain, layer, version, status,
		 scripture, principle, tags, provides, created, updated)
		VALUES (?, ?, ?, ?, ?, ?, 'platform', 'L2', ?, ?, ?, ?, ?, ?, ?, ?)`,
		key, file, title,
		getString(m1, "type"),
		getString(m1, "component"),
		getString(m1, "role"),
		getString(m2, "version"),
		getString(m2, "status"),
		getString(m4, "scripture"),
		getString(m4, "principle"),
		getString(m7, "tags"),
		strings.Join(provides, ", "),
		getString(m2, "created"),
		getString(m2, "updated"),
	)
	if err != nil {
		log.Printf("Warning: inserting spec %s: %v", key, err)
	}

	// Seed dependencies
	if m5 != nil {
		for _, dep := range getStringArray(m5, "requires") {
			db.Exec(`INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?, ?, 'requires')`,
				key, dep)
		}
		for _, dep := range getStringArray(m5, "consumers") {
			db.Exec(`INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?, ?, 'consumed_by')`,
				dep, key)
		}
		if d := getString(m5, "derives_from"); d != "" {
			db.Exec(`INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?, ?, 'derives_from')`,
				key, d)
		}
	}

	fmt.Printf("  spec: %s\n", key)
}

// ─────────────────────────────────────────────────────────────
// Seed health levels from score.toml
// ─────────────────────────────────────────────────────────────

func seedHealthLevels(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/health/score.toml")
	if raw == nil {
		return
	}

	levels := getMap(raw, "levels")
	display := getMap(raw, "display")

	levelNames := []string{"broken", "wanting", "lacking", "even", "sound", "whole", "perfect"}
	count := 0

	for _, name := range levelNames {
		lvl := getMap(levels, name)
		if lvl == nil {
			continue
		}

		// Get display info
		disp := getMap(display, name)
		emoji := getString(disp, "emoji")
		color := getString(disp, "color")
		ansiCode := getString(disp, "ansi_code")

		// Get ternary range
		ternaryRange := getIntArray(lvl, "ternary_range")
		ternaryMin, ternaryMax := 0, 0
		if len(ternaryRange) >= 2 {
			ternaryMin = ternaryRange[0]
			ternaryMax = ternaryRange[1]
		}

		_, err := db.Exec(`INSERT OR IGNORE INTO health_levels
			(name, description, hebrew, min_stored, max_stored,
			 ternary_min, ternary_max, direction, priority, scripture,
			 emoji, color, ansi_code)
			VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`,
			name,
			getString(lvl, "description"),
			getString(lvl, "hebrew"),
			getInt(lvl, "min_stored"),
			getInt(lvl, "max_stored"),
			ternaryMin, ternaryMax,
			getString(lvl, "direction"),
			getString(lvl, "priority"),
			getString(lvl, "scripture"),
			emoji, color, ansiCode,
		)
		if err != nil {
			log.Printf("Warning: inserting health level %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  health_levels: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed health normalization bases from score.toml
// ─────────────────────────────────────────────────────────────

func seedHealthNormalization(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/health/score.toml")
	if raw == nil {
		return
	}

	health := getMap(raw, "health")
	normalized := getMap(health, "normalized")
	bases := getMap(normalized, "bases")
	if bases == nil {
		return
	}

	baseNames := []string{"base1", "base5", "base10", "base20", "base25", "base50"}
	count := 0

	for _, name := range baseNames {
		base := getMap(bases, name)
		if base == nil {
			continue
		}

		keyPoints := toJSON(getIntArray(base, "key_points"))

		_, err := db.Exec(`INSERT OR IGNORE INTO health_normalization
			(name, description, step, points, use_case, key_points)
			VALUES (?, ?, ?, ?, ?, ?)`,
			name,
			getString(base, "description"),
			getInt(base, "step"),
			getInt(base, "points"),
			getString(base, "use_case"),
			keyPoints,
		)
		if err != nil {
			log.Printf("Warning: inserting normalization %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  health_normalization: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed health scoring rules from score.toml
// ─────────────────────────────────────────────────────────────

func seedHealthScoring(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/health/score.toml")
	if raw == nil {
		return
	}

	count := 0
	insertRule := func(name, value, desc string) {
		_, err := db.Exec(`INSERT OR IGNORE INTO health_scoring (rule_name, rule_value, description) VALUES (?, ?, ?)`,
			name, value, desc)
		if err != nil {
			log.Printf("Warning: inserting scoring rule %s: %v", name, err)
		}
		count++
	}

	// Storage fundamentals
	health := getMap(raw, "health")
	insertRule("storage_min", fmt.Sprintf("%d", getInt(health, "storage_min")), "Minimum stored value")
	insertRule("storage_max", fmt.Sprintf("%d", getInt(health, "storage_max")), "Maximum stored value")
	insertRule("storage_default", fmt.Sprintf("%d", getInt(health, "storage_default")), "Default stored value (center)")

	// Ternary interpretation
	ternary := getMap(health, "ternary")
	insertRule("ternary_min", fmt.Sprintf("%d", getInt(ternary, "interpretation_min")), "Minimum ternary value")
	insertRule("ternary_max", fmt.Sprintf("%d", getInt(ternary, "interpretation_max")), "Maximum ternary value")
	insertRule("ternary_center", fmt.Sprintf("%d", getInt(ternary, "interpretation_center")), "Ternary center (even)")

	// Default normalization base
	normalized := getMap(health, "normalized")
	insertRule("default_base", fmt.Sprintf("%d", getInt(normalized, "default_base")), "Default normalization base")

	// Calculation rules
	calc := getMap(raw, "calculation")
	init := getMap(calc, "initialization")
	insertRule("init_default", getString(init, "default_value"), "Default health value for new files")

	increase := getMap(calc, "increase")
	insertRule("increase_max_per_event", fmt.Sprintf("%d", getInt(increase, "max_increase_per_event")), "Max increase per event")
	insertRule("increase_ceiling", fmt.Sprintf("%d", getInt(increase, "ceiling")), "Ceiling (cannot exceed)")

	decrease := getMap(calc, "decrease")
	insertRule("decrease_max_per_event", fmt.Sprintf("%d", getInt(decrease, "max_decrease_per_event")), "Max decrease per event")
	insertRule("decrease_floor", fmt.Sprintf("%d", getInt(decrease, "floor")), "Floor (cannot go below)")

	staleness := getMap(calc, "staleness")
	insertRule("decay_rate", fmt.Sprintf("%d", getInt(staleness, "decay_rate")), "Decay per period")
	insertRule("decay_period", getString(staleness, "decay_period"), "Decay period")
	insertRule("decay_minimum", fmt.Sprintf("%d", getInt(staleness, "minimum_after_decay")), "Min after decay (never below even)")

	// Hierarchy
	hier := getMap(raw, "hierarchy")
	agg := getMap(hier, "aggregation")
	insertRule("hierarchy_method", getString(agg, "method"), "Aggregation method")

	fmt.Printf("  health_scoring: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed health log actions from log.toml
// ─────────────────────────────────────────────────────────────

func seedHealthLogActions(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/health/log.toml")
	if raw == nil {
		return
	}

	actions := getMap(raw, "actions")
	if actions == nil {
		return
	}

	actionNames := []string{"success", "failure", "neutral", "recovery", "reset"}
	count := 0

	for _, name := range actionNames {
		act := getMap(actions, name)
		if act == nil {
			continue
		}

		// Delta can be int or string ("special" for reset)
		delta := ""
		if d, ok := act["delta"]; ok {
			delta = fmt.Sprintf("%v", d)
		} else if _, ok := act["delta_range"]; ok {
			delta = "range"
		}

		examples := toJSONStrings(getStringArray(act, "examples"))

		_, err := db.Exec(`INSERT OR IGNORE INTO health_log_actions
			(name, description, delta, examples, scripture)
			VALUES (?, ?, ?, ?, ?)`,
			name,
			getString(act, "description"),
			delta,
			examples,
			getString(act, "scripture"),
		)
		if err != nil {
			log.Printf("Warning: inserting log action %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  health_log_actions: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed DAR phases, aspects, severity from dar.toml
// ─────────────────────────────────────────────────────────────

func seedDARPhases(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/health/dar.toml")
	if raw == nil {
		return
	}

	dar := getMap(raw, "dar")

	// Phases
	phases := getMap(dar, "phases")
	phaseCount := 0
	for _, name := range []string{"DETECT", "ASSESS", "RESTORE"} {
		entry := getMap(phases, name)
		if entry == nil {
			continue
		}
		_, err := db.Exec(`INSERT OR IGNORE INTO dar_phases (name, idx, balanced, description) VALUES (?, ?, ?, ?)`,
			name, getInt(entry, "index"), getInt(entry, "balanced"), getString(entry, "description"))
		if err != nil {
			log.Printf("Warning: inserting DAR phase %s: %v", name, err)
		}
		phaseCount++
	}
	fmt.Printf("  dar_phases: %d entries\n", phaseCount)

	// Aspects
	aspects := getMap(dar, "aspects")
	aspectCount := 0
	for _, name := range []string{"ENTRY", "PROCESS", "EXIT"} {
		entry := getMap(aspects, name)
		if entry == nil {
			continue
		}
		_, err := db.Exec(`INSERT OR IGNORE INTO dar_aspects (name, idx, balanced, description) VALUES (?, ?, ?, ?)`,
			name, getInt(entry, "index"), getInt(entry, "balanced"), getString(entry, "description"))
		if err != nil {
			log.Printf("Warning: inserting DAR aspect %s: %v", name, err)
		}
		aspectCount++
	}
	fmt.Printf("  dar_aspects: %d entries\n", aspectCount)

	// Severity
	severity := getMap(dar, "severity")
	sevCount := 0
	for _, name := range []string{"MILD", "MEDIUM", "CRITICAL"} {
		entry := getMap(severity, name)
		if entry == nil {
			continue
		}
		_, err := db.Exec(`INSERT OR IGNORE INTO dar_severity (name, idx, balanced, description) VALUES (?, ?, ?, ?)`,
			name, getInt(entry, "index"), getInt(entry, "balanced"), getString(entry, "description"))
		if err != nil {
			log.Printf("Warning: inserting DAR severity %s: %v", name, err)
		}
		sevCount++
	}
	fmt.Printf("  dar_severity: %d entries\n", sevCount)
}

// ─────────────────────────────────────────────────────────────
// Seed DAR restore results from dar.toml
// ─────────────────────────────────────────────────────────────

func seedDARRestoreResults(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/health/dar.toml")
	if raw == nil {
		return
	}

	dar := getMap(raw, "dar")
	results := getMap(dar, "restore_result")
	if results == nil {
		return
	}

	names := []string{"SUCCESS", "PARTIAL", "DEGRADED", "DEFERRED", "BLOCKED", "FAILED", "FATAL"}
	count := 0

	for _, name := range names {
		entry := getMap(results, name)
		if entry == nil {
			continue
		}
		_, err := db.Exec(`INSERT OR IGNORE INTO dar_restore_results (name, value, description) VALUES (?, ?, ?)`,
			name, getInt(entry, "value"), getString(entry, "description"))
		if err != nil {
			log.Printf("Warning: inserting restore result %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  dar_restore_results: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed DAR restore strategies from dar.toml
// ─────────────────────────────────────────────────────────────

func seedDARRestoreStrategies(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/health/dar.toml")
	if raw == nil {
		return
	}

	dar := getMap(raw, "dar")
	strategies := getMap(dar, "restore_strategy")
	if strategies == nil {
		return
	}

	names := []string{"NONE", "FILL_DEFAULTS", "LOCATE_FILES", "CREATE_DIRS", "CLAMP_VALUES", "FALLBACK", "DEGRADE"}
	count := 0

	for _, name := range names {
		entry := getMap(strategies, name)
		if entry == nil {
			continue
		}
		_, err := db.Exec(`INSERT OR IGNORE INTO dar_restore_strategies (name, value, description) VALUES (?, ?, ?)`,
			name, getString(entry, "value"), getString(entry, "description"))
		if err != nil {
			log.Printf("Warning: inserting restore strategy %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  dar_restore_strategies: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed permission states from access.toml
// ─────────────────────────────────────────────────────────────

func seedPermissionStates(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/permission/access.toml")
	if raw == nil {
		return
	}

	states := getMap(raw, "states")
	if states == nil {
		return
	}

	stateNames := []string{"denied", "deferred", "granted"}
	count := 0

	for _, name := range stateNames {
		entry := getMap(states, name)
		if entry == nil {
			continue
		}
		_, err := db.Exec(`INSERT OR IGNORE INTO permission_states (name, value, description, meaning) VALUES (?, ?, ?, ?)`,
			name, getInt(entry, "value"), getString(entry, "description"), getString(entry, "meaning"))
		if err != nil {
			log.Printf("Warning: inserting permission state %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  permission_states: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed permission action categories from access.toml
// ─────────────────────────────────────────────────────────────

func seedPermissionActionCategories(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/permission/access.toml")
	if raw == nil {
		return
	}

	actions := getMap(raw, "actions")
	categories := getMap(actions, "categories")
	if categories == nil {
		return
	}

	catNames := []string{"safe", "standard", "sensitive", "critical"}
	count := 0

	for _, name := range catNames {
		cat := getMap(categories, name)
		if cat == nil {
			continue
		}

		examples := toJSONStrings(getStringArray(cat, "examples"))

		_, err := db.Exec(`INSERT OR IGNORE INTO permission_action_categories
			(name, description, examples, minimum_state, health_threshold)
			VALUES (?, ?, ?, ?, ?)`,
			name,
			getString(cat, "description"),
			examples,
			getString(cat, "minimum_state"),
			getInt(cat, "health_threshold"),
		)
		if err != nil {
			log.Printf("Warning: inserting action category %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  permission_action_categories: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed permission gating from access.toml
// ─────────────────────────────────────────────────────────────

func seedPermissionGating(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/permission/access.toml")
	if raw == nil {
		return
	}

	gating := getMap(raw, "gating")
	mapping := getMap(gating, "mapping")
	if mapping == nil {
		return
	}

	count := 0
	healthLevels := []string{"broken", "wanting", "lacking", "even", "sound", "whole", "perfect"}

	for _, level := range healthLevels {
		state := getString(mapping, level)
		if state == "" {
			continue
		}

		_, err := db.Exec(`INSERT OR IGNORE INTO permission_gating (health_level, permission_state) VALUES (?, ?)`,
			level, state)
		if err != nil {
			log.Printf("Warning: inserting gating %s: %v", level, err)
		}
		count++
	}

	fmt.Printf("  permission_gating: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed filesystem biblical extensions from types.toml
// ─────────────────────────────────────────────────────────────

func seedFilesystemBiblicalExtensions(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "filesystem/types.toml")
	if raw == nil {
		return
	}

	biblical := getMap(raw, "biblical")
	if biblical == nil {
		return
	}

	names := []string{"dabar", "chok", "sefer", "tavnit", "zera", "tselem"}
	count := 0

	for _, name := range names {
		entry := getMap(biblical, name)
		if entry == nil {
			continue
		}

		exts := toJSONStrings(getStringArray(entry, "extensions"))

		_, err := db.Exec(`INSERT OR IGNORE INTO filesystem_biblical_extensions
			(name, hebrew, meaning, extensions)
			VALUES (?, ?, ?, ?)`,
			name,
			getString(entry, "hebrew"),
			getString(entry, "meaning"),
			exts,
		)
		if err != nil {
			log.Printf("Warning: inserting biblical extension %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  filesystem_biblical_extensions: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed system monitoring thresholds from monitoring.toml
// ─────────────────────────────────────────────────────────────

func seedSystemMonitoring(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/system/monitoring.toml")
	if raw == nil {
		return
	}

	metrics := getMap(raw, "metrics")
	if metrics == nil {
		return
	}

	count := 0
	for _, metricName := range []string{"load", "memory", "disk"} {
		metric := getMap(metrics, metricName)
		if metric == nil {
			continue
		}

		icon := getString(metric, "icon")

		for _, level := range []string{"yellow", "red"} {
			lvl := getMap(metric, level)
			if lvl == nil {
				continue
			}

			threshold := 0.0
			if v, ok := lvl["threshold"]; ok {
				switch n := v.(type) {
				case float64:
					threshold = n
				case int64:
					threshold = float64(n)
				}
			}

			_, err := db.Exec(`INSERT OR IGNORE INTO system_monitoring
				(metric, level, threshold, icon, description, health_mapping)
				VALUES (?, ?, ?, ?, ?, ?)`,
				metricName, level, threshold, icon,
				getString(lvl, "description"),
				getString(lvl, "health_mapping"),
			)
			if err != nil {
				log.Printf("Warning: inserting monitoring %s/%s: %v", metricName, level, err)
			}
			count++
		}
	}

	fmt.Printf("  system_monitoring: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed temporal boundaries from boundaries.toml
// ─────────────────────────────────────────────────────────────

func seedTemporalBoundaries(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/temporal/boundaries.toml")
	if raw == nil {
		return
	}

	count := 0

	// Time of day ranges
	tod := getMap(raw, "time_of_day")
	for _, name := range []string{"morning", "mid_morning", "midday", "afternoon", "evening", "night", "late_night"} {
		entry := getMap(tod, name)
		if entry == nil {
			continue
		}

		_, err := db.Exec(`INSERT OR IGNORE INTO temporal_boundaries
			(category, name, start_value, end_value, icon, color, description)
			VALUES (?, ?, ?, ?, ?, ?, ?)`,
			"time_of_day", name,
			getInt(entry, "start_hour"),
			getInt(entry, "end_hour"),
			getString(entry, "icon"),
			getString(entry, "color"),
			getString(entry, "description"),
		)
		if err != nil {
			log.Printf("Warning: inserting time_of_day %s: %v", name, err)
		}
		count++
	}

	// Session phases
	sp := getMap(raw, "session_phase")
	for _, name := range []string{"fresh", "active", "sustained", "long", "extended"} {
		entry := getMap(sp, name)
		if entry == nil {
			continue
		}

		_, err := db.Exec(`INSERT OR IGNORE INTO temporal_boundaries
			(category, name, start_value, end_value, icon, color, description)
			VALUES (?, ?, 0, ?, '', ?, ?)`,
			"session_phase", name,
			getInt(entry, "max_minutes"),
			getString(entry, "color"),
			getString(entry, "description"),
		)
		if err != nil {
			log.Printf("Warning: inserting session_phase %s: %v", name, err)
		}
		count++
	}

	// Circadian phases
	circ := getMap(raw, "circadian")
	for _, name := range []string{"rising", "peak", "dip", "recovery", "evening", "rest"} {
		entry := getMap(circ, name)
		if entry == nil {
			continue
		}

		_, err := db.Exec(`INSERT OR IGNORE INTO temporal_boundaries
			(category, name, start_value, end_value, icon, color, description, extra)
			VALUES (?, ?, ?, ?, '', '', ?, ?)`,
			"circadian", name,
			getInt(entry, "start_hour"),
			getInt(entry, "end_hour"),
			getString(entry, "note"),
			getString(entry, "productivity"),
		)
		if err != nil {
			log.Printf("Warning: inserting circadian %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  temporal_boundaries: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed terminal display config from display.toml
// ─────────────────────────────────────────────────────────────

func seedTerminalDisplay(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/terminal/display.toml")
	if raw == nil {
		return
	}

	count := 0

	// Command colors
	colors := getMap(raw, "command_colors")
	for _, cmd := range []string{"HALT", "AWAIT", "PROCEED"} {
		entry := getMap(colors, cmd)
		if entry == nil {
			continue
		}

		// Store as JSON object for the three color values
		colorJSON := fmt.Sprintf(`{"foreground":"%s","background":"%s","cursor":"%s"}`,
			getString(entry, "foreground"),
			getString(entry, "background"),
			getString(entry, "cursor"),
		)

		_, err := db.Exec(`INSERT OR IGNORE INTO terminal_display
			(category, name, value, description)
			VALUES (?, ?, ?, ?)`,
			"command_color", cmd, colorJSON,
			getString(entry, "description"),
		)
		if err != nil {
			log.Printf("Warning: inserting command_color %s: %v", cmd, err)
		}
		count++
	}

	// State file config
	sf := getMap(raw, "state_file")
	if sf != nil {
		for _, key := range []string{"runtime_dir_env", "fallback_dir", "filename", "permissions"} {
			val := getString(sf, key)
			if val != "" {
				db.Exec(`INSERT OR IGNORE INTO terminal_display
					(category, name, value, description)
					VALUES (?, ?, ?, ?)`,
					"state_config", key, val, "")
				count++
			}
		}
	}

	// Health emoji mapping
	if emojiList, ok := raw["health_emoji"]; ok {
		if arr, ok := emojiList.([]map[string]any); ok {
			for _, entry := range arr {
				name := fmt.Sprintf("health_%d", getInt(entry, "min_percent"))
				val := fmt.Sprintf(`{"min_percent":%d,"emoji":"%s","label":"%s"}`,
					getInt(entry, "min_percent"),
					getString(entry, "emoji"),
					getString(entry, "label"),
				)
				db.Exec(`INSERT OR IGNORE INTO terminal_display
					(category, name, value, description)
					VALUES (?, ?, ?, ?)`,
					"health_emoji", name, val, getString(entry, "label"))
				count++
			}
		}
	}

	// Debug markers
	dm := getMap(raw, "debug_markers")
	if dm != nil {
		for _, key := range []string{"session_start", "session_end", "section_separator", "box_top", "box_bottom", "box_divider"} {
			val := getString(dm, key)
			if val != "" {
				db.Exec(`INSERT OR IGNORE INTO terminal_display
					(category, name, value, description)
					VALUES (?, ?, ?, ?)`,
					"debug_marker", key, val, "")
				count++
			}
		}
	}

	fmt.Printf("  terminal_display: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed deployment hosts from hosts.toml
// ─────────────────────────────────────────────────────────────

func seedDeploymentHosts(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/deploy/hosts.toml")
	if raw == nil {
		return
	}

	hosts := getMap(raw, "hosts")
	paths := getMap(raw, "paths")
	prodDir := getString(paths, "prod_dir")
	prodBin := getString(paths, "prod_bin")

	count := 0
	for _, name := range []string{"oracle", "dell"} {
		host := getMap(hosts, name)
		if host == nil {
			continue
		}

		isPrimary := 0
		if v, ok := host["is_primary"]; ok {
			if b, ok := v.(bool); ok && b {
				isPrimary = 1
			}
		}

		_, err := db.Exec(`INSERT OR IGNORE INTO deployment_hosts
			(name, address, username, arch, is_primary, prod_dir, prod_bin, description)
			VALUES (?, ?, ?, ?, ?, ?, ?, ?)`,
			name,
			getString(host, "address"),
			getString(host, "username"),
			getString(host, "arch"),
			isPrimary,
			prodDir,
			prodBin,
			getString(host, "description"),
		)
		if err != nil {
			log.Printf("Warning: inserting host %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  deployment_hosts: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed server config from hosts.toml
// ─────────────────────────────────────────────────────────────

func seedServerConfig(db *sql.DB, ladderDir string) {
	raw := loadFile(ladderDir, "os/deploy/hosts.toml")
	if raw == nil {
		return
	}

	server := getMap(raw, "server")
	defaults := getMap(server, "defaults")
	features := getMap(server, "features")

	count := 0
	insertKV := func(key, value, desc string) {
		_, err := db.Exec(`INSERT OR IGNORE INTO server_config (key, value, description) VALUES (?, ?, ?)`,
			key, value, desc)
		if err != nil {
			log.Printf("Warning: inserting server_config %s: %v", key, err)
		}
		count++
	}

	if defaults != nil {
		insertKV("port", fmt.Sprintf("%d", getInt(defaults, "port")), "Default server port")
		insertKV("env_port", getString(defaults, "env_port"), "Environment variable for port override")
		insertKV("env_builder_dir", getString(defaults, "env_builder_dir"), "Environment variable for builder directory")
	}

	if features != nil {
		for _, key := range []string{"health_endpoint", "config_endpoint", "preferences_endpoint"} {
			val := getString(features, key)
			if val != "" {
				insertKV(key, val, "HTTP endpoint path")
			}
		}
	}

	fmt.Printf("  server_config: %d entries\n", count)
}

// ═══════════════════════════════════════════════════════════════
// HELPERS
// ═══════════════════════════════════════════════════════════════

// fileCache stores parsed TOML files to avoid redundant I/O.
var fileCache = map[string]map[string]any{}

func loadFile(ladderDir, relPath string) map[string]any {
	path := filepath.Join(ladderDir, relPath)
	if cached, ok := fileCache[path]; ok {
		return cached
	}

	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		log.Printf("Warning: cannot read %s: %v", relPath, err)
		return nil
	}
	fileCache[path] = raw
	return raw
}

func printSummary(db *sql.DB) {
	fmt.Println("\n--- Platform Database Summary ---")
	tables := []string{
		"specs", "dependencies",
		"health_levels", "health_normalization", "health_scoring",
		"health_log_actions",
		"dar_phases", "dar_aspects", "dar_severity",
		"dar_restore_results", "dar_restore_strategies",
		"permission_states", "permission_action_categories", "permission_gating",
		"filesystem_biblical_extensions",
		"system_monitoring", "temporal_boundaries",
		"terminal_display", "deployment_hosts", "server_config",
	}
	total := 0
	for _, t := range tables {
		var count int
		db.QueryRow("SELECT COUNT(*) FROM " + t).Scan(&count)
		fmt.Printf("  %-35s %d rows\n", t, count)
		total += count
	}
	fmt.Printf("  %-35s %d rows\n", "TOTAL", total)
}

func getMap(m map[string]any, key string) map[string]any {
	if m == nil {
		return nil
	}
	v, ok := m[key]
	if !ok {
		return nil
	}
	mm, ok := v.(map[string]any)
	if !ok {
		return nil
	}
	return mm
}

func getString(m map[string]any, key string) string {
	if m == nil {
		return ""
	}
	v, ok := m[key]
	if !ok {
		return ""
	}
	s, ok := v.(string)
	if !ok {
		return fmt.Sprintf("%v", v)
	}
	return s
}

func getInt(m map[string]any, key string) int {
	if m == nil {
		return 0
	}
	v, ok := m[key]
	if !ok {
		return 0
	}
	switch n := v.(type) {
	case int64:
		return int(n)
	case float64:
		return int(n)
	case int:
		return n
	default:
		return 0
	}
}

func getIntArray(m map[string]any, key string) []int {
	if m == nil {
		return nil
	}
	v, ok := m[key]
	if !ok {
		return nil
	}
	arr, ok := v.([]any)
	if !ok {
		return nil
	}
	result := make([]int, 0, len(arr))
	for _, item := range arr {
		switch n := item.(type) {
		case int64:
			result = append(result, int(n))
		case float64:
			result = append(result, int(n))
		case int:
			result = append(result, n)
		}
	}
	return result
}

func getStringArray(m map[string]any, key string) []string {
	if m == nil {
		return nil
	}
	v, ok := m[key]
	if !ok {
		return nil
	}
	arr, ok := v.([]any)
	if !ok {
		return nil
	}
	result := make([]string, 0, len(arr))
	for _, item := range arr {
		if s, ok := item.(string); ok {
			result = append(result, s)
		}
	}
	return result
}

func toJSON(arr []int) string {
	if arr == nil {
		return "[]"
	}
	b, err := json.Marshal(arr)
	if err != nil {
		return "[]"
	}
	return string(b)
}

func toJSONStrings(arr []string) string {
	if arr == nil {
		return "[]"
	}
	b, err := json.Marshal(arr)
	if err != nil {
		return "[]"
	}
	return string(b)
}

// ═══════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════
