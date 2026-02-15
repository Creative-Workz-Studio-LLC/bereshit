//omni:code --go -library
//omni:key B-L2-platform-loader
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

// Package platform loads and queries the L2 platform database.
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       B-L2-platform-loader
//
//	Scripture: "Except the LORD build the house, they labour in vain
//	           that build it." — Psalm 127:1
//	Principle: "The triangle: Configuration (TOML) -> Data (SQLite) -> Code (Go/Rust)"
//	Anchor:    Genesis 1:1
//
//	Type:      Hybrid (ladder + spiral access patterns)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2026-02-15
//	Version:         a-01.00
//
// # M.2 Version History [HISTORY]
//
//   - a-01.00 (2026-02-15) — Initial creation: SQLite-backed platform queries
//
// # M.3 Interface [INTERFACE]
//
//	Requires: stdlib (1): database/sql, fmt, sync | external (-1): modernc.org/sqlite
//	Used by:  L3-cpisi/core, hooks, statusline, debugging tools
//
// Data source: b-word/data/platform/database/platform.db
// Schema: specs, dependencies, health_levels, health_normalization, health_scoring,
//
//	health_log_actions, dar_phases, dar_aspects, dar_severity,
//	dar_restore_results, dar_restore_strategies, permission_states,
//	permission_action_categories, permission_gating, filesystem_biblical_extensions,
//	system_monitoring, temporal_boundaries, terminal_display,
//	deployment_hosts, server_config
package platform

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

import (
	"database/sql"
	"fmt"
	"path/filepath"
	"sync"

	_ "modernc.org/sqlite" // SQLite driver (pure Go, no CGO)
)

// ────────────────────────────────────────────────────────────────
// S.1 Types
// ────────────────────────────────────────────────────────────────

// Spec represents a TOML specification file's metadata.
type Spec struct {
	ID        int
	Key       string
	File      string
	Title     string
	Type      string
	Component string
	Role      string
	Domain    string
	Layer     string
	Version   string
	Status    string
	Scripture string
	Principle string
	Tags      string
	Provides  string
	Created   string
	Updated   string
}

// Dependency is a cross-reference between specs.
type Dependency struct {
	ID        int
	SpecKey   string
	DependsOn string
	Relation  string // "requires", "consumed_by", "derives_from"
}

// HealthLevel is one of the 7 Hebrew-named health states.
type HealthLevel struct {
	ID          int
	Name        string // broken, wanting, lacking, even, sound, whole, perfect
	Description string
	Hebrew      string // shavar, ratsah, chaser, yashar, tamim, shalem, tov
	MinStored   int    // stored uint8 range min
	MaxStored   int    // stored uint8 range max
	TernaryMin  int    // ternary range min
	TernaryMax  int    // ternary range max
	Direction   string // toward_anchor, from_anchor, at_anchor
	Priority    string
	Scripture   string
	Emoji       string
	Color       string
	ANSI        string
}

// HealthNormalization defines a normalization base for health scores.
type HealthNormalization struct {
	ID          int
	Name        string // trit, tryte, trint9, trint27, byte, block_243
	Description string
	Step        int
	Points      int
	UseCase     string
	KeyPoints   string // JSON array
}

// HealthScoringRule is a key-value rule for health score calculation.
type HealthScoringRule struct {
	ID          int
	RuleName    string
	RuleValue   string
	Description string
}

// HealthLogAction defines a health event action type.
type HealthLogAction struct {
	ID          int
	Name        string // success, failure, neutral, recovery, reset
	Description string
	Delta       string // text description of delta range
	Examples    string // JSON array
	Scripture   string
}

// DARPhase is a Detect-Assess-Restore phase.
type DARPhase struct {
	ID          int
	Name        string // DETECT, ASSESS, RESTORE
	Idx         int    // ordering index
	Balanced    int    // balanced ternary: -1, 0, +1
	Description string
}

// DARAspect is a DAR sub-aspect.
type DARAspect struct {
	ID          int
	Name        string // ENTRY, PROCESS, EXIT
	Idx         int    // ordering index
	Balanced    int    // balanced ternary: -1, 0, +1
	Description string
}

// DARSeverity is a DAR severity level.
type DARSeverity struct {
	ID          int
	Name        string // MILD, MEDIUM, CRITICAL
	Idx         int    // ordering index
	Balanced    int    // balanced ternary: -1, 0, +1
	Description string
}

// DARRestoreResult is a possible restore outcome.
type DARRestoreResult struct {
	ID          int
	Name        string // SUCCESS, IMPROVED, MAINTAINED, DEGRADED, FAILED, BLOCKED, FATAL
	Value       int    // ternary value: +1, 0, -1
	Description string
}

// DARRestoreStrategy is a restore approach.
type DARRestoreStrategy struct {
	ID          int
	Name        string
	Value       string // strategy value/flag
	Description string
}

// PermissionState represents a permission value.
type PermissionState struct {
	ID          int
	Name        string // denied, deferred, granted
	Value       int    // -1, 0, +1
	Description string
	Meaning     string
}

// PermissionActionCategory defines sensitivity levels for operations.
type PermissionActionCategory struct {
	ID              int
	Name            string // safe, standard, sensitive, critical
	Description     string
	Examples        string // JSON array
	MinimumState    string // minimum permission state required
	HealthThreshold int    // minimum health score required
}

// PermissionGating maps a health level to a permission state.
type PermissionGating struct {
	ID              int
	HealthLevel     string
	PermissionState string
}

// FilesystemExtension is a biblical filesystem extension mapping.
type FilesystemExtension struct {
	ID         int
	Name       string // biblical name (dabar, chok, sefer, etc.)
	Hebrew     string // Hebrew characters
	Meaning    string // English meaning
	Extensions string // JSON array of file extensions
}

// SystemMonitoring is a threshold entry for system resource monitoring.
type SystemMonitoring struct {
	ID            int
	Metric        string  // load, memory, disk
	Level         string  // yellow, red
	Threshold     float64 // threshold value
	Icon          string
	Description   string
	HealthMapping string // ternary mapping description
}

// TemporalBoundary is a time classification entry.
type TemporalBoundary struct {
	ID          int
	Category    string // time_of_day, session_phase, circadian
	Name        string // morning, fresh, rising, etc.
	StartValue  int    // start hour or 0
	EndValue    int    // end hour or max_minutes
	Icon        string
	Color       string
	Description string
	Extra       string // productivity for circadian, etc.
}

// TerminalDisplay is a terminal configuration entry.
type TerminalDisplay struct {
	ID          int
	Category    string // command_color, state_config, health_emoji, debug_marker
	Name        string
	Value       string // JSON or plain value
	Description string
}

// DeploymentHost is a deployment target.
type DeploymentHost struct {
	ID          int
	Name        string // oracle, dell
	Address     string // IP address
	Username    string // SSH user
	Arch        string // arm64, amd64
	IsPrimary   bool
	ProdDir     string
	ProdBin     string
	Description string
}

// ServerConfig is a server configuration key-value pair.
type ServerConfig struct {
	ID          int
	Key         string
	Value       string
	Description string
}

// DB holds an open connection to the platform database.
// Thread-safe — uses sync.RWMutex for concurrent reads.
type DB struct {
	db *sql.DB
	mu sync.RWMutex
}

// ────────────────────────────────────────────────────────────────
// S.2 Constants
// ────────────────────────────────────────────────────────────────

const (
	// DatabaseDir is the path to the platform database relative to module root (b-word/).
	DatabaseDir = "data/platform/database"

	// DatabaseFile is the database filename.
	DatabaseFile = "platform.db"
)

// ────────────────────────────────────────────────────────────────
// S.3 Prepared Queries
// ────────────────────────────────────────────────────────────────

const (
	// Specs
	qSpecByKey = `SELECT id, key, file, title, type, component, role, domain, layer, version, status, scripture, principle, tags, provides, created, updated FROM specs WHERE key = ?`
	qAllSpecs  = `SELECT id, key, file, title, type, component, role, domain, layer, version, status, scripture, principle, tags, provides, created, updated FROM specs ORDER BY key`
	qSpecCount = `SELECT COUNT(*) FROM specs`

	// Dependencies
	qDepsOf = `SELECT id, spec_key, depends_on, relation FROM dependencies WHERE spec_key = ? ORDER BY relation, depends_on`
	qDepsOn = `SELECT id, spec_key, depends_on, relation FROM dependencies WHERE depends_on = ? ORDER BY relation, spec_key`

	// Health levels
	qAllHealthLevels   = `SELECT id, name, description, hebrew, min_stored, max_stored, ternary_min, ternary_max, direction, priority, scripture, emoji, color, ansi_code FROM health_levels ORDER BY min_stored`
	qHealthLevelByName = `SELECT id, name, description, hebrew, min_stored, max_stored, ternary_min, ternary_max, direction, priority, scripture, emoji, color, ansi_code FROM health_levels WHERE name = ?`

	// Health normalization
	qAllNormBases = `SELECT id, name, description, step, points, use_case, key_points FROM health_normalization ORDER BY step`

	// Health scoring
	qAllScoringRules        = `SELECT id, rule_name, rule_value, description FROM health_scoring ORDER BY rule_name`
	qScoringRulesByCategory = `SELECT id, rule_name, rule_value, description FROM health_scoring WHERE rule_name LIKE ? ORDER BY rule_name`

	// Health log actions
	qAllLogActions = `SELECT id, name, description, delta, examples, scripture FROM health_log_actions ORDER BY name`

	// DAR phases
	qAllDARPhases = `SELECT id, name, idx, balanced, description FROM dar_phases ORDER BY idx`

	// DAR aspects
	qAllDARAspects = `SELECT id, name, idx, balanced, description FROM dar_aspects ORDER BY idx`

	// DAR severity
	qAllDARSeverity = `SELECT id, name, idx, balanced, description FROM dar_severity ORDER BY idx`

	// DAR restore results
	qAllRestoreResults = `SELECT id, name, value, description FROM dar_restore_results ORDER BY id`

	// DAR restore strategies
	qAllRestoreStrategies = `SELECT id, name, value, description FROM dar_restore_strategies ORDER BY id`

	// Permission states
	qAllPermStates = `SELECT id, name, value, description, meaning FROM permission_states ORDER BY value`

	// Permission action categories
	qAllActionCategories = `SELECT id, name, description, examples, minimum_state, health_threshold FROM permission_action_categories ORDER BY health_threshold`

	// Permission gating
	qAllPermGating      = `SELECT id, health_level, permission_state FROM permission_gating ORDER BY id`
	qPermGatingByHealth = `SELECT id, health_level, permission_state FROM permission_gating WHERE health_level = ?`

	// Filesystem extensions
	qAllFSExtensions = `SELECT id, name, hebrew, meaning, extensions FROM filesystem_biblical_extensions ORDER BY name`

	// System monitoring
	qAllMonitoring       = `SELECT id, metric, level, threshold, icon, description, health_mapping FROM system_monitoring ORDER BY metric, level`
	qMonitoringByMetric  = `SELECT id, metric, level, threshold, icon, description, health_mapping FROM system_monitoring WHERE metric = ? ORDER BY level`

	// Temporal boundaries
	qAllTemporalBounds     = `SELECT id, category, name, start_value, end_value, icon, color, description, extra FROM temporal_boundaries ORDER BY category, start_value`
	qTemporalByCategory    = `SELECT id, category, name, start_value, end_value, icon, color, description, extra FROM temporal_boundaries WHERE category = ? ORDER BY start_value`

	// Terminal display
	qAllTerminalDisplay    = `SELECT id, category, name, value, description FROM terminal_display ORDER BY category, name`
	qTerminalByCategory    = `SELECT id, category, name, value, description FROM terminal_display WHERE category = ? ORDER BY name`

	// Deployment hosts
	qAllHosts       = `SELECT id, name, address, username, arch, is_primary, prod_dir, prod_bin, description FROM deployment_hosts ORDER BY is_primary DESC, name`
	qHostByName     = `SELECT id, name, address, username, arch, is_primary, prod_dir, prod_bin, description FROM deployment_hosts WHERE name = ?`

	// Server config
	qAllServerConfig  = `SELECT id, key, value, description FROM server_config ORDER BY key`
	qServerConfigByKey = `SELECT id, key, value, description FROM server_config WHERE key = ?`
)

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Helpers (scan functions + generic collector)
// ────────────────────────────────────────────────────────────────

type scanner interface{ Scan(...any) error }

func scanSpec(s scanner) (*Spec, error) {
	sp := &Spec{}
	err := s.Scan(
		&sp.ID, &sp.Key, &sp.File, &sp.Title, &sp.Type, &sp.Component,
		&sp.Role, &sp.Domain, &sp.Layer, &sp.Version, &sp.Status,
		&sp.Scripture, &sp.Principle, &sp.Tags, &sp.Provides,
		&sp.Created, &sp.Updated,
	)
	if err != nil {
		return nil, err
	}
	return sp, nil
}

func scanDep(s scanner) (*Dependency, error) {
	d := &Dependency{}
	err := s.Scan(&d.ID, &d.SpecKey, &d.DependsOn, &d.Relation)
	if err != nil {
		return nil, err
	}
	return d, nil
}

func scanHealthLevel(s scanner) (*HealthLevel, error) {
	h := &HealthLevel{}
	err := s.Scan(
		&h.ID, &h.Name, &h.Description, &h.Hebrew,
		&h.MinStored, &h.MaxStored, &h.TernaryMin, &h.TernaryMax,
		&h.Direction, &h.Priority, &h.Scripture,
		&h.Emoji, &h.Color, &h.ANSI,
	)
	if err != nil {
		return nil, err
	}
	return h, nil
}

func scanNormBase(s scanner) (*HealthNormalization, error) {
	n := &HealthNormalization{}
	err := s.Scan(&n.ID, &n.Name, &n.Description, &n.Step, &n.Points, &n.UseCase, &n.KeyPoints)
	if err != nil {
		return nil, err
	}
	return n, nil
}

func scanScoringRule(s scanner) (*HealthScoringRule, error) {
	r := &HealthScoringRule{}
	err := s.Scan(&r.ID, &r.RuleName, &r.RuleValue, &r.Description)
	if err != nil {
		return nil, err
	}
	return r, nil
}

func scanLogAction(s scanner) (*HealthLogAction, error) {
	a := &HealthLogAction{}
	err := s.Scan(&a.ID, &a.Name, &a.Description, &a.Delta, &a.Examples, &a.Scripture)
	if err != nil {
		return nil, err
	}
	return a, nil
}

func scanDARPhase(s scanner) (*DARPhase, error) {
	p := &DARPhase{}
	err := s.Scan(&p.ID, &p.Name, &p.Idx, &p.Balanced, &p.Description)
	if err != nil {
		return nil, err
	}
	return p, nil
}

func scanDARAspect(s scanner) (*DARAspect, error) {
	a := &DARAspect{}
	err := s.Scan(&a.ID, &a.Name, &a.Idx, &a.Balanced, &a.Description)
	if err != nil {
		return nil, err
	}
	return a, nil
}

func scanDARSeverity(s scanner) (*DARSeverity, error) {
	v := &DARSeverity{}
	err := s.Scan(&v.ID, &v.Name, &v.Idx, &v.Balanced, &v.Description)
	if err != nil {
		return nil, err
	}
	return v, nil
}

func scanRestoreResult(s scanner) (*DARRestoreResult, error) {
	r := &DARRestoreResult{}
	err := s.Scan(&r.ID, &r.Name, &r.Value, &r.Description)
	if err != nil {
		return nil, err
	}
	return r, nil
}

func scanRestoreStrategy(s scanner) (*DARRestoreStrategy, error) {
	r := &DARRestoreStrategy{}
	err := s.Scan(&r.ID, &r.Name, &r.Value, &r.Description)
	if err != nil {
		return nil, err
	}
	return r, nil
}

func scanPermState(s scanner) (*PermissionState, error) {
	p := &PermissionState{}
	err := s.Scan(&p.ID, &p.Name, &p.Value, &p.Description, &p.Meaning)
	if err != nil {
		return nil, err
	}
	return p, nil
}

func scanActionCategory(s scanner) (*PermissionActionCategory, error) {
	c := &PermissionActionCategory{}
	err := s.Scan(&c.ID, &c.Name, &c.Description, &c.Examples, &c.MinimumState, &c.HealthThreshold)
	if err != nil {
		return nil, err
	}
	return c, nil
}

func scanPermGating(s scanner) (*PermissionGating, error) {
	g := &PermissionGating{}
	err := s.Scan(&g.ID, &g.HealthLevel, &g.PermissionState)
	if err != nil {
		return nil, err
	}
	return g, nil
}

func scanFSExtension(s scanner) (*FilesystemExtension, error) {
	e := &FilesystemExtension{}
	err := s.Scan(&e.ID, &e.Name, &e.Hebrew, &e.Meaning, &e.Extensions)
	if err != nil {
		return nil, err
	}
	return e, nil
}

func scanMonitoring(s scanner) (*SystemMonitoring, error) {
	m := &SystemMonitoring{}
	err := s.Scan(&m.ID, &m.Metric, &m.Level, &m.Threshold, &m.Icon, &m.Description, &m.HealthMapping)
	if err != nil {
		return nil, err
	}
	return m, nil
}

func scanTemporalBoundary(s scanner) (*TemporalBoundary, error) {
	t := &TemporalBoundary{}
	err := s.Scan(&t.ID, &t.Category, &t.Name, &t.StartValue, &t.EndValue, &t.Icon, &t.Color, &t.Description, &t.Extra)
	if err != nil {
		return nil, err
	}
	return t, nil
}

func scanTerminalDisplay(s scanner) (*TerminalDisplay, error) {
	d := &TerminalDisplay{}
	err := s.Scan(&d.ID, &d.Category, &d.Name, &d.Value, &d.Description)
	if err != nil {
		return nil, err
	}
	return d, nil
}

func scanHost(s scanner) (*DeploymentHost, error) {
	h := &DeploymentHost{}
	err := s.Scan(&h.ID, &h.Name, &h.Address, &h.Username, &h.Arch, &h.IsPrimary, &h.ProdDir, &h.ProdBin, &h.Description)
	if err != nil {
		return nil, err
	}
	return h, nil
}

func scanServerConfig(s scanner) (*ServerConfig, error) {
	c := &ServerConfig{}
	err := s.Scan(&c.ID, &c.Key, &c.Value, &c.Description)
	if err != nil {
		return nil, err
	}
	return c, nil
}

// collectRows collects multiple rows from a query into a slice.
func collectRows[T any](rows *sql.Rows, scan func(scanner) (*T, error)) ([]T, error) {
	var results []T
	for rows.Next() {
		item, err := scan(rows)
		if err != nil {
			return results, err
		}
		results = append(results, *item)
	}
	return results, rows.Err()
}

// ────────────────────────────────────────────────────────────────
// B.2 Core Operations (Open/Close)
// ────────────────────────────────────────────────────────────────

// Open opens the platform database from a directory path.
// Opens read-only — platform database is reference data.
func Open(dbDir string) (*DB, error) {
	dbPath := filepath.Join(dbDir, DatabaseFile)

	conn, err := sql.Open("sqlite", dbPath+"?mode=ro")
	if err != nil {
		return nil, fmt.Errorf("platform.Open: %w", err)
	}

	if err := conn.Ping(); err != nil {
		conn.Close()
		return nil, fmt.Errorf("platform.Open: ping failed: %w", err)
	}

	return &DB{db: conn}, nil
}

// OpenFromRoot opens the platform database using module root path.
// Convenience: OpenFromRoot("/path/to/b-word") resolves to
// "/path/to/b-word/data/platform/database/platform.db".
func OpenFromRoot(moduleRoot string) (*DB, error) {
	return Open(filepath.Join(moduleRoot, DatabaseDir))
}

// Close releases the database connection.
func (db *DB) Close() error {
	db.mu.Lock()
	defer db.mu.Unlock()
	return db.db.Close()
}

// ────────────────────────────────────────────────────────────────
// B.3 Spec Queries
// ────────────────────────────────────────────────────────────────

// SpecByKey returns a single spec by its unique key.
func (db *DB) SpecByKey(key string) (*Spec, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanSpec(db.db.QueryRow(qSpecByKey, key))
}

// AllSpecs returns every spec in the database.
func (db *DB) AllSpecs() ([]Spec, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllSpecs)
	if err != nil {
		return nil, fmt.Errorf("AllSpecs: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanSpec)
}

// SpecCount returns the number of specs.
func (db *DB) SpecCount() (int, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	var count int
	err := db.db.QueryRow(qSpecCount).Scan(&count)
	return count, err
}

// ────────────────────────────────────────────────────────────────
// B.4 Dependency Queries
// ────────────────────────────────────────────────────────────────

// DependenciesOf returns what a spec depends on.
func (db *DB) DependenciesOf(specKey string) ([]Dependency, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qDepsOf, specKey)
	if err != nil {
		return nil, fmt.Errorf("DependenciesOf(%s): %w", specKey, err)
	}
	defer rows.Close()
	return collectRows(rows, scanDep)
}

// DependentsOn returns what depends on a spec.
func (db *DB) DependentsOn(specKey string) ([]Dependency, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qDepsOn, specKey)
	if err != nil {
		return nil, fmt.Errorf("DependentsOn(%s): %w", specKey, err)
	}
	defer rows.Close()
	return collectRows(rows, scanDep)
}

// ────────────────────────────────────────────────────────────────
// B.5 Health Queries
// ────────────────────────────────────────────────────────────────

// AllHealthLevels returns all 7 Hebrew-named health levels ordered by value.
func (db *DB) AllHealthLevels() ([]HealthLevel, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllHealthLevels)
	if err != nil {
		return nil, fmt.Errorf("AllHealthLevels: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanHealthLevel)
}

// HealthLevelByName returns a single health level by name.
func (db *DB) HealthLevelByName(name string) (*HealthLevel, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanHealthLevel(db.db.QueryRow(qHealthLevelByName, name))
}

// AllNormalizationBases returns all 6 normalization scales.
func (db *DB) AllNormalizationBases() ([]HealthNormalization, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllNormBases)
	if err != nil {
		return nil, fmt.Errorf("AllNormalizationBases: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanNormBase)
}

// AllScoringRules returns all health scoring rules.
func (db *DB) AllScoringRules() ([]HealthScoringRule, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllScoringRules)
	if err != nil {
		return nil, fmt.Errorf("AllScoringRules: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanScoringRule)
}

// ScoringRulesByCategory returns rules whose rule_name starts with the given prefix.
// Example: ScoringRulesByCategory("init") returns all rules like "init_value", "init_default", etc.
func (db *DB) ScoringRulesByCategory(prefix string) ([]HealthScoringRule, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qScoringRulesByCategory, prefix+"%")
	if err != nil {
		return nil, fmt.Errorf("ScoringRulesByCategory(%s): %w", prefix, err)
	}
	defer rows.Close()
	return collectRows(rows, scanScoringRule)
}

// AllLogActions returns all health log action types.
func (db *DB) AllLogActions() ([]HealthLogAction, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllLogActions)
	if err != nil {
		return nil, fmt.Errorf("AllLogActions: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanLogAction)
}

// ────────────────────────────────────────────────────────────────
// B.6 DAR Queries
// ────────────────────────────────────────────────────────────────

// AllDARPhases returns the 3 DAR phases (DETECT, ASSESS, RESTORE).
func (db *DB) AllDARPhases() ([]DARPhase, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllDARPhases)
	if err != nil {
		return nil, fmt.Errorf("AllDARPhases: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanDARPhase)
}

// AllDARAspects returns the 3 DAR aspects (ENTRY, PROCESS, EXIT).
func (db *DB) AllDARAspects() ([]DARAspect, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllDARAspects)
	if err != nil {
		return nil, fmt.Errorf("AllDARAspects: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanDARAspect)
}

// AllDARSeverity returns the 3 severity levels (MILD, MEDIUM, CRITICAL).
func (db *DB) AllDARSeverity() ([]DARSeverity, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllDARSeverity)
	if err != nil {
		return nil, fmt.Errorf("AllDARSeverity: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanDARSeverity)
}

// AllRestoreResults returns the 7 possible restore outcomes.
func (db *DB) AllRestoreResults() ([]DARRestoreResult, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllRestoreResults)
	if err != nil {
		return nil, fmt.Errorf("AllRestoreResults: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanRestoreResult)
}

// AllRestoreStrategies returns the 7 restore strategies.
func (db *DB) AllRestoreStrategies() ([]DARRestoreStrategy, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllRestoreStrategies)
	if err != nil {
		return nil, fmt.Errorf("AllRestoreStrategies: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanRestoreStrategy)
}

// ────────────────────────────────────────────────────────────────
// B.7 Permission Queries
// ────────────────────────────────────────────────────────────────

// AllPermissionStates returns the 3 permission states (denied, deferred, granted).
func (db *DB) AllPermissionStates() ([]PermissionState, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllPermStates)
	if err != nil {
		return nil, fmt.Errorf("AllPermissionStates: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanPermState)
}

// AllActionCategories returns the 4 action categories (safe, standard, sensitive, critical).
func (db *DB) AllActionCategories() ([]PermissionActionCategory, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllActionCategories)
	if err != nil {
		return nil, fmt.Errorf("AllActionCategories: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanActionCategory)
}

// AllPermissionGating returns all health-to-permission mappings.
func (db *DB) AllPermissionGating() ([]PermissionGating, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllPermGating)
	if err != nil {
		return nil, fmt.Errorf("AllPermissionGating: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanPermGating)
}

// PermissionForHealth returns the permission state for a given health level.
func (db *DB) PermissionForHealth(healthLevel string) (*PermissionGating, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanPermGating(db.db.QueryRow(qPermGatingByHealth, healthLevel))
}

// ────────────────────────────────────────────────────────────────
// B.8 Filesystem Queries
// ────────────────────────────────────────────────────────────────

// AllFilesystemExtensions returns all biblical filesystem extensions.
func (db *DB) AllFilesystemExtensions() ([]FilesystemExtension, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllFSExtensions)
	if err != nil {
		return nil, fmt.Errorf("AllFilesystemExtensions: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanFSExtension)
}

// ────────────────────────────────────────────────────────────────
// B.9 Convenience: Health Score Resolution
// ────────────────────────────────────────────────────────────────

// ResolveHealthLevel returns the HealthLevel for a given stored uint8 score.
// The score is compared against each level's min_stored/max_stored range.
func (db *DB) ResolveHealthLevel(storedScore int) (*HealthLevel, error) {
	levels, err := db.AllHealthLevels()
	if err != nil {
		return nil, err
	}
	for i := range levels {
		if storedScore >= levels[i].MinStored && storedScore <= levels[i].MaxStored {
			return &levels[i], nil
		}
	}
	return nil, fmt.Errorf("ResolveHealthLevel(%d): no matching level", storedScore)
}

// ResolvePermission returns the permission state for a stored health score.
// Combines health level lookup with gating to provide the permission answer.
func (db *DB) ResolvePermission(storedScore int) (string, error) {
	level, err := db.ResolveHealthLevel(storedScore)
	if err != nil {
		return "", err
	}
	gating, err := db.PermissionForHealth(level.Name)
	if err != nil {
		return "", fmt.Errorf("ResolvePermission: %w", err)
	}
	return gating.PermissionState, nil
}

// ────────────────────────────────────────────────────────────────
// B.10 System Monitoring Queries
// ────────────────────────────────────────────────────────────────

// AllMonitoring returns all system monitoring thresholds.
func (db *DB) AllMonitoring() ([]SystemMonitoring, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllMonitoring)
	if err != nil {
		return nil, fmt.Errorf("AllMonitoring: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanMonitoring)
}

// MonitoringByMetric returns thresholds for a specific metric (load, memory, disk).
func (db *DB) MonitoringByMetric(metric string) ([]SystemMonitoring, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qMonitoringByMetric, metric)
	if err != nil {
		return nil, fmt.Errorf("MonitoringByMetric(%s): %w", metric, err)
	}
	defer rows.Close()
	return collectRows(rows, scanMonitoring)
}

// ────────────────────────────────────────────────────────────────
// B.11 Temporal Boundary Queries
// ────────────────────────────────────────────────────────────────

// AllTemporalBoundaries returns all time classification entries.
func (db *DB) AllTemporalBoundaries() ([]TemporalBoundary, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllTemporalBounds)
	if err != nil {
		return nil, fmt.Errorf("AllTemporalBoundaries: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanTemporalBoundary)
}

// TemporalByCategory returns boundaries for a category (time_of_day, session_phase, circadian).
func (db *DB) TemporalByCategory(category string) ([]TemporalBoundary, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qTemporalByCategory, category)
	if err != nil {
		return nil, fmt.Errorf("TemporalByCategory(%s): %w", category, err)
	}
	defer rows.Close()
	return collectRows(rows, scanTemporalBoundary)
}

// ────────────────────────────────────────────────────────────────
// B.12 Terminal Display Queries
// ────────────────────────────────────────────────────────────────

// AllTerminalDisplay returns all terminal display configuration entries.
func (db *DB) AllTerminalDisplay() ([]TerminalDisplay, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllTerminalDisplay)
	if err != nil {
		return nil, fmt.Errorf("AllTerminalDisplay: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanTerminalDisplay)
}

// TerminalByCategory returns display entries for a category (command_color, state_config, health_emoji, debug_marker).
func (db *DB) TerminalByCategory(category string) ([]TerminalDisplay, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qTerminalByCategory, category)
	if err != nil {
		return nil, fmt.Errorf("TerminalByCategory(%s): %w", category, err)
	}
	defer rows.Close()
	return collectRows(rows, scanTerminalDisplay)
}

// ────────────────────────────────────────────────────────────────
// B.13 Deployment Host Queries
// ────────────────────────────────────────────────────────────────

// AllHosts returns all deployment hosts ordered by priority (primary first).
func (db *DB) AllHosts() ([]DeploymentHost, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllHosts)
	if err != nil {
		return nil, fmt.Errorf("AllHosts: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanHost)
}

// HostByName returns a single deployment host by name.
func (db *DB) HostByName(name string) (*DeploymentHost, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanHost(db.db.QueryRow(qHostByName, name))
}

// PrimaryHost returns the primary deployment host.
func (db *DB) PrimaryHost() (*DeploymentHost, error) {
	hosts, err := db.AllHosts()
	if err != nil {
		return nil, err
	}
	for i := range hosts {
		if hosts[i].IsPrimary {
			return &hosts[i], nil
		}
	}
	return nil, fmt.Errorf("PrimaryHost: no primary host configured")
}

// ────────────────────────────────────────────────────────────────
// B.14 Server Config Queries
// ────────────────────────────────────────────────────────────────

// AllServerConfig returns all server configuration key-value pairs.
func (db *DB) AllServerConfig() ([]ServerConfig, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllServerConfig)
	if err != nil {
		return nil, fmt.Errorf("AllServerConfig: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanServerConfig)
}

// ServerConfigByKey returns a single server config entry.
func (db *DB) ServerConfigByKey(key string) (*ServerConfig, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanServerConfig(db.db.QueryRow(qServerConfigByKey, key))
}

// ============================================================================
// CLOSING
// ============================================================================
//
// This package provides L2 platform access to the database that materializes
// all TOML specifications into queryable records. The Triangle is complete:
//
//   Configuration (TOML specs) -> Data (SQLite) -> Code (this package)
//
// Every health level, DAR phase, permission state, scoring rule, monitoring
// threshold, temporal boundary, terminal config, host, and server setting is
// discoverable by query rather than hardcoded. The platform knows itself.
//
// "Except the LORD build the house, they labour in vain
//  that build it." — Psalm 127:1
//
// Dependencies: modernc.org/sqlite (pure Go, no CGO)
// Data source: b-word/data/platform/database/platform.db
//
// Safe to add: New query methods, search functions, aggregate queries
// Modify with care: Type struct fields, database schema assumptions
// Never modify: Read-only access pattern, table names
