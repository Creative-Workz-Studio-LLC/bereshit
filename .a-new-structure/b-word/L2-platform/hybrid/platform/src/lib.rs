//omni:code --rust -library
//omni:key B-L2-platform-loader-rs
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

//! # Platform Loader (Rust)
//!
//! L2 platform access to the database that materializes all TOML specs.
//!
//! The Triangle: Configuration (TOML) -> Data (SQLite) -> Code (this crate).
//! Every health level, DAR phase, permission state, scoring rule, and
//! filesystem extension is discoverable by query rather than hardcoded.
//! The platform knows itself.
//!
//! # Lifecycle Pattern
//!
//! **Hybrid** — combines ladder (sequential lookup by key/name) and spiral
//! (iterative search across categories, dependency traversal) access patterns.
//!
//! # Tables
//!
//! ```text
//! specs, dependencies, health_levels, health_normalization, health_scoring,
//! health_log_actions, dar_phases, dar_aspects, dar_severity,
//! dar_restore_results, dar_restore_strategies, permission_states,
//! permission_action_categories, permission_gating, filesystem_biblical_extensions,
//! system_monitoring, temporal_boundaries, terminal_display,
//! deployment_hosts, server_config
//! ```
//!
//! # Usage
//!
//! ```no_run
//! use bereshit_l2_platform::DB;
//!
//! let db = DB::open("/path/to/b-word/data/platform/database")?;
//! let levels = db.all_health_levels()?;
//! assert_eq!(levels.len(), 7);
//! # Ok::<(), bereshit_l2_platform::PlatformError>(())
//! ```
//!
//! Data source: `b-word/data/platform/database/platform.db`

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// S.1 Imports
// ────────────────────────────────────────────────────────────────

use rusqlite::{Connection, OpenFlags};
use std::fmt;
use std::path::{Path, PathBuf};

// ────────────────────────────────────────────────────────────────
// S.2 Types
// ────────────────────────────────────────────────────────────────

/// A TOML specification file's metadata.
#[derive(Debug, Clone)]
pub struct Spec {
    pub id: i64,
    pub key: String,
    pub file: String,
    pub title: String,
    pub spec_type: String,
    pub component: String,
    pub role: String,
    pub domain: String,
    pub layer: String,
    pub version: String,
    pub status: String,
    pub scripture: String,
    pub principle: String,
    pub tags: String,
    pub provides: String,
    pub created: String,
    pub updated: String,
}

/// A cross-reference between specs.
#[derive(Debug, Clone)]
pub struct Dependency {
    pub id: i64,
    pub spec_key: String,
    pub depends_on: String,
    pub relation: String,
}

/// One of the 7 Hebrew-named health states.
#[derive(Debug, Clone)]
pub struct HealthLevel {
    pub id: i64,
    pub name: String,
    pub description: String,
    pub hebrew: String,
    pub min_stored: i64,
    pub max_stored: i64,
    pub ternary_min: i64,
    pub ternary_max: i64,
    pub direction: String,
    pub priority: String,
    pub scripture: String,
    pub emoji: String,
    pub color: String,
    pub ansi_code: String,
}

/// A normalization base for health scores.
#[derive(Debug, Clone)]
pub struct HealthNormalization {
    pub id: i64,
    pub name: String,
    pub description: String,
    pub step: i64,
    pub points: i64,
    pub use_case: String,
    pub key_points: String,
}

/// A key-value rule for health score calculation.
#[derive(Debug, Clone)]
pub struct HealthScoringRule {
    pub id: i64,
    pub rule_name: String,
    pub rule_value: String,
    pub description: String,
}

/// A health event action type.
#[derive(Debug, Clone)]
pub struct HealthLogAction {
    pub id: i64,
    pub name: String,
    pub description: String,
    pub delta: String,
    pub examples: String,
    pub scripture: String,
}

/// A Detect-Assess-Restore phase.
#[derive(Debug, Clone)]
pub struct DARPhase {
    pub id: i64,
    pub name: String,
    pub idx: i64,
    pub balanced: i64,
    pub description: String,
}

/// A DAR sub-aspect.
#[derive(Debug, Clone)]
pub struct DARAspect {
    pub id: i64,
    pub name: String,
    pub idx: i64,
    pub balanced: i64,
    pub description: String,
}

/// A DAR severity level.
#[derive(Debug, Clone)]
pub struct DARSeverity {
    pub id: i64,
    pub name: String,
    pub idx: i64,
    pub balanced: i64,
    pub description: String,
}

/// A possible restore outcome.
#[derive(Debug, Clone)]
pub struct DARRestoreResult {
    pub id: i64,
    pub name: String,
    pub value: i64,
    pub description: String,
}

/// A restore approach.
#[derive(Debug, Clone)]
pub struct DARRestoreStrategy {
    pub id: i64,
    pub name: String,
    pub value: String,
    pub description: String,
}

/// A permission value.
#[derive(Debug, Clone)]
pub struct PermissionState {
    pub id: i64,
    pub name: String,
    pub value: i64,
    pub description: String,
    pub meaning: String,
}

/// Sensitivity levels for operations.
#[derive(Debug, Clone)]
pub struct PermissionActionCategory {
    pub id: i64,
    pub name: String,
    pub description: String,
    pub examples: String,
    pub minimum_state: String,
    pub health_threshold: i64,
}

/// Maps a health level to a permission state.
#[derive(Debug, Clone)]
pub struct PermissionGating {
    pub id: i64,
    pub health_level: String,
    pub permission_state: String,
}

/// A biblical filesystem extension mapping.
#[derive(Debug, Clone)]
pub struct FilesystemExtension {
    pub id: i64,
    pub name: String,
    pub hebrew: String,
    pub meaning: String,
    pub extensions: String,
}

/// A system monitoring threshold entry.
#[derive(Debug, Clone)]
pub struct SystemMonitoring {
    pub id: i64,
    pub metric: String,
    pub level: String,
    pub threshold: f64,
    pub icon: String,
    pub description: String,
    pub health_mapping: String,
}

/// A time classification entry.
#[derive(Debug, Clone)]
pub struct TemporalBoundary {
    pub id: i64,
    pub category: String,
    pub name: String,
    pub start_value: i64,
    pub end_value: i64,
    pub icon: String,
    pub color: String,
    pub description: String,
    pub extra: String,
}

/// A terminal configuration entry.
#[derive(Debug, Clone)]
pub struct TerminalDisplay {
    pub id: i64,
    pub category: String,
    pub name: String,
    pub value: String,
    pub description: String,
}

/// A deployment target.
#[derive(Debug, Clone)]
pub struct DeploymentHost {
    pub id: i64,
    pub name: String,
    pub address: String,
    pub username: String,
    pub arch: String,
    pub is_primary: bool,
    pub prod_dir: String,
    pub prod_bin: String,
    pub description: String,
}

/// A server configuration key-value pair.
#[derive(Debug, Clone)]
pub struct ServerConfig {
    pub id: i64,
    pub key: String,
    pub value: String,
    pub description: String,
}

/// Errors from platform database operations.
#[derive(Debug)]
pub enum PlatformError {
    Open(String, rusqlite::Error),
    Query(String, rusqlite::Error),
    NotFound(String),
}

impl fmt::Display for PlatformError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            PlatformError::Open(path, e) => write!(f, "platform.open({path}): {e}"),
            PlatformError::Query(ctx, e) => write!(f, "platform.{ctx}: {e}"),
            PlatformError::NotFound(ctx) => write!(f, "platform.{ctx}: not found"),
        }
    }
}

impl std::error::Error for PlatformError {
    fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
        match self {
            PlatformError::Open(_, e) | PlatformError::Query(_, e) => Some(e),
            PlatformError::NotFound(_) => None,
        }
    }
}

/// Open connection to the platform database.
pub struct DB {
    conn: Connection,
}

// ────────────────────────────────────────────────────────────────
// S.3 Constants
// ────────────────────────────────────────────────────────────────

/// Path to platform database relative to module root (b-word/).
pub const DATABASE_DIR: &str = "data/platform/database";

/// Database filename.
pub const DATABASE_FILE: &str = "platform.db";

// ────────────────────────────────────────────────────────────────
// S.4 SQL Queries
// ────────────────────────────────────────────────────────────────

// Specs
const Q_SPEC_BY_KEY: &str =
    "SELECT id, key, file, title, type, component, role, domain, layer, version, status, \
     scripture, principle, tags, provides, created, updated FROM specs WHERE key = ?1";
const Q_ALL_SPECS: &str =
    "SELECT id, key, file, title, type, component, role, domain, layer, version, status, \
     scripture, principle, tags, provides, created, updated FROM specs ORDER BY key";
const Q_SPEC_COUNT: &str = "SELECT COUNT(*) FROM specs";

// Dependencies
const Q_DEPS_OF: &str =
    "SELECT id, spec_key, depends_on, relation FROM dependencies \
     WHERE spec_key = ?1 ORDER BY relation, depends_on";
const Q_DEPS_ON: &str =
    "SELECT id, spec_key, depends_on, relation FROM dependencies \
     WHERE depends_on = ?1 ORDER BY relation, spec_key";

// Health levels
const Q_ALL_HEALTH_LEVELS: &str =
    "SELECT id, name, description, hebrew, min_stored, max_stored, ternary_min, ternary_max, \
     direction, priority, scripture, emoji, color, ansi_code \
     FROM health_levels ORDER BY min_stored";
const Q_HEALTH_LEVEL_BY_NAME: &str =
    "SELECT id, name, description, hebrew, min_stored, max_stored, ternary_min, ternary_max, \
     direction, priority, scripture, emoji, color, ansi_code \
     FROM health_levels WHERE name = ?1";

// Health normalization
const Q_ALL_NORM_BASES: &str =
    "SELECT id, name, description, step, points, use_case, key_points \
     FROM health_normalization ORDER BY step";

// Health scoring
const Q_ALL_SCORING_RULES: &str =
    "SELECT id, rule_name, rule_value, description FROM health_scoring ORDER BY rule_name";
const Q_SCORING_RULES_BY_NAME_PREFIX: &str =
    "SELECT id, rule_name, rule_value, description \
     FROM health_scoring WHERE rule_name LIKE ?1 ORDER BY rule_name";

// Health log actions
const Q_ALL_LOG_ACTIONS: &str =
    "SELECT id, name, description, delta, examples, scripture \
     FROM health_log_actions ORDER BY name";

// DAR phases
const Q_ALL_DAR_PHASES: &str =
    "SELECT id, name, idx, balanced, description FROM dar_phases ORDER BY idx";

// DAR aspects
const Q_ALL_DAR_ASPECTS: &str =
    "SELECT id, name, idx, balanced, description FROM dar_aspects ORDER BY idx";

// DAR severity
const Q_ALL_DAR_SEVERITY: &str =
    "SELECT id, name, idx, balanced, description FROM dar_severity ORDER BY idx";

// DAR restore results
const Q_ALL_RESTORE_RESULTS: &str =
    "SELECT id, name, value, description FROM dar_restore_results ORDER BY id";

// DAR restore strategies
const Q_ALL_RESTORE_STRATEGIES: &str =
    "SELECT id, name, value, description FROM dar_restore_strategies ORDER BY id";

// Permission states
const Q_ALL_PERM_STATES: &str =
    "SELECT id, name, value, description, meaning FROM permission_states ORDER BY value";

// Permission action categories
const Q_ALL_ACTION_CATEGORIES: &str =
    "SELECT id, name, description, examples, minimum_state, health_threshold \
     FROM permission_action_categories ORDER BY health_threshold";

// Permission gating
const Q_ALL_PERM_GATING: &str =
    "SELECT id, health_level, permission_state FROM permission_gating ORDER BY id";
const Q_PERM_GATING_BY_HEALTH: &str =
    "SELECT id, health_level, permission_state FROM permission_gating WHERE health_level = ?1";

// Filesystem extensions
const Q_ALL_FS_EXTENSIONS: &str =
    "SELECT id, name, hebrew, meaning, extensions \
     FROM filesystem_biblical_extensions ORDER BY name";

// System monitoring
const Q_ALL_MONITORING: &str =
    "SELECT id, metric, level, threshold, icon, description, health_mapping \
     FROM system_monitoring ORDER BY metric, level";
const Q_MONITORING_BY_METRIC: &str =
    "SELECT id, metric, level, threshold, icon, description, health_mapping \
     FROM system_monitoring WHERE metric = ?1 ORDER BY level";

// Temporal boundaries
const Q_ALL_TEMPORAL_BOUNDS: &str =
    "SELECT id, category, name, start_value, end_value, icon, color, description, extra \
     FROM temporal_boundaries ORDER BY category, start_value";
const Q_TEMPORAL_BY_CATEGORY: &str =
    "SELECT id, category, name, start_value, end_value, icon, color, description, extra \
     FROM temporal_boundaries WHERE category = ?1 ORDER BY start_value";

// Terminal display
const Q_ALL_TERMINAL_DISPLAY: &str =
    "SELECT id, category, name, value, description \
     FROM terminal_display ORDER BY category, name";
const Q_TERMINAL_BY_CATEGORY: &str =
    "SELECT id, category, name, value, description \
     FROM terminal_display WHERE category = ?1 ORDER BY name";

// Deployment hosts
const Q_ALL_HOSTS: &str =
    "SELECT id, name, address, username, arch, is_primary, prod_dir, prod_bin, description \
     FROM deployment_hosts ORDER BY is_primary DESC, name";
const Q_HOST_BY_NAME: &str =
    "SELECT id, name, address, username, arch, is_primary, prod_dir, prod_bin, description \
     FROM deployment_hosts WHERE name = ?1";

// Server config
const Q_ALL_SERVER_CONFIG: &str =
    "SELECT id, key, value, description FROM server_config ORDER BY key";
const Q_SERVER_CONFIG_BY_KEY: &str =
    "SELECT id, key, value, description FROM server_config WHERE key = ?1";

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Helpers (scan functions + generic collectors)
// ────────────────────────────────────────────────────────────────

fn scan_spec(row: &rusqlite::Row<'_>) -> rusqlite::Result<Spec> {
    Ok(Spec {
        id:        row.get(0)?,
        key:       row.get(1)?,
        file:      row.get(2)?,
        title:     row.get(3)?,
        spec_type: row.get(4)?,
        component: row.get(5)?,
        role:      row.get(6)?,
        domain:    row.get(7)?,
        layer:     row.get(8)?,
        version:   row.get(9)?,
        status:    row.get(10)?,
        scripture: row.get(11)?,
        principle: row.get(12)?,
        tags:      row.get(13)?,
        provides:  row.get(14)?,
        created:   row.get(15)?,
        updated:   row.get(16)?,
    })
}

fn scan_dep(row: &rusqlite::Row<'_>) -> rusqlite::Result<Dependency> {
    Ok(Dependency {
        id:         row.get(0)?,
        spec_key:   row.get(1)?,
        depends_on: row.get(2)?,
        relation:   row.get(3)?,
    })
}

fn scan_health_level(row: &rusqlite::Row<'_>) -> rusqlite::Result<HealthLevel> {
    Ok(HealthLevel {
        id:          row.get(0)?,
        name:        row.get(1)?,
        description: row.get(2)?,
        hebrew:      row.get(3)?,
        min_stored:  row.get(4)?,
        max_stored:  row.get(5)?,
        ternary_min: row.get(6)?,
        ternary_max: row.get(7)?,
        direction:   row.get(8)?,
        priority:    row.get(9)?,
        scripture:   row.get(10)?,
        emoji:       row.get(11)?,
        color:       row.get(12)?,
        ansi_code:   row.get(13)?,
    })
}

fn scan_norm_base(row: &rusqlite::Row<'_>) -> rusqlite::Result<HealthNormalization> {
    Ok(HealthNormalization {
        id:         row.get(0)?,
        name:       row.get(1)?,
        description: row.get(2)?,
        step:       row.get(3)?,
        points:     row.get(4)?,
        use_case:   row.get(5)?,
        key_points: row.get(6)?,
    })
}

fn scan_scoring_rule(row: &rusqlite::Row<'_>) -> rusqlite::Result<HealthScoringRule> {
    Ok(HealthScoringRule {
        id:          row.get(0)?,
        rule_name:   row.get(1)?,
        rule_value:  row.get(2)?,
        description: row.get(3)?,
    })
}

fn scan_log_action(row: &rusqlite::Row<'_>) -> rusqlite::Result<HealthLogAction> {
    Ok(HealthLogAction {
        id:          row.get(0)?,
        name:        row.get(1)?,
        description: row.get(2)?,
        delta:       row.get(3)?,
        examples:    row.get(4)?,
        scripture:   row.get(5)?,
    })
}

fn scan_dar_phase(row: &rusqlite::Row<'_>) -> rusqlite::Result<DARPhase> {
    Ok(DARPhase {
        id:          row.get(0)?,
        name:        row.get(1)?,
        idx:         row.get(2)?,
        balanced:    row.get(3)?,
        description: row.get(4)?,
    })
}

fn scan_dar_aspect(row: &rusqlite::Row<'_>) -> rusqlite::Result<DARAspect> {
    Ok(DARAspect {
        id:          row.get(0)?,
        name:        row.get(1)?,
        idx:         row.get(2)?,
        balanced:    row.get(3)?,
        description: row.get(4)?,
    })
}

fn scan_dar_severity(row: &rusqlite::Row<'_>) -> rusqlite::Result<DARSeverity> {
    Ok(DARSeverity {
        id:          row.get(0)?,
        name:        row.get(1)?,
        idx:         row.get(2)?,
        balanced:    row.get(3)?,
        description: row.get(4)?,
    })
}

fn scan_restore_result(row: &rusqlite::Row<'_>) -> rusqlite::Result<DARRestoreResult> {
    Ok(DARRestoreResult {
        id:          row.get(0)?,
        name:        row.get(1)?,
        value:       row.get(2)?,
        description: row.get(3)?,
    })
}

fn scan_restore_strategy(row: &rusqlite::Row<'_>) -> rusqlite::Result<DARRestoreStrategy> {
    Ok(DARRestoreStrategy {
        id:          row.get(0)?,
        name:        row.get(1)?,
        value:       row.get(2)?,
        description: row.get(3)?,
    })
}

fn scan_perm_state(row: &rusqlite::Row<'_>) -> rusqlite::Result<PermissionState> {
    Ok(PermissionState {
        id:          row.get(0)?,
        name:        row.get(1)?,
        value:       row.get(2)?,
        description: row.get(3)?,
        meaning:     row.get(4)?,
    })
}

fn scan_action_category(row: &rusqlite::Row<'_>) -> rusqlite::Result<PermissionActionCategory> {
    Ok(PermissionActionCategory {
        id:               row.get(0)?,
        name:             row.get(1)?,
        description:      row.get(2)?,
        examples:         row.get(3)?,
        minimum_state:    row.get(4)?,
        health_threshold: row.get(5)?,
    })
}

fn scan_perm_gating(row: &rusqlite::Row<'_>) -> rusqlite::Result<PermissionGating> {
    Ok(PermissionGating {
        id:               row.get(0)?,
        health_level:     row.get(1)?,
        permission_state: row.get(2)?,
    })
}

fn scan_fs_extension(row: &rusqlite::Row<'_>) -> rusqlite::Result<FilesystemExtension> {
    Ok(FilesystemExtension {
        id:         row.get(0)?,
        name:       row.get(1)?,
        hebrew:     row.get(2)?,
        meaning:    row.get(3)?,
        extensions: row.get(4)?,
    })
}

fn scan_monitoring(row: &rusqlite::Row<'_>) -> rusqlite::Result<SystemMonitoring> {
    Ok(SystemMonitoring {
        id:             row.get(0)?,
        metric:         row.get(1)?,
        level:          row.get(2)?,
        threshold:      row.get(3)?,
        icon:           row.get(4)?,
        description:    row.get(5)?,
        health_mapping: row.get(6)?,
    })
}

fn scan_temporal_boundary(row: &rusqlite::Row<'_>) -> rusqlite::Result<TemporalBoundary> {
    Ok(TemporalBoundary {
        id:          row.get(0)?,
        category:    row.get(1)?,
        name:        row.get(2)?,
        start_value: row.get(3)?,
        end_value:   row.get(4)?,
        icon:        row.get(5)?,
        color:       row.get(6)?,
        description: row.get(7)?,
        extra:       row.get(8)?,
    })
}

fn scan_terminal_display(row: &rusqlite::Row<'_>) -> rusqlite::Result<TerminalDisplay> {
    Ok(TerminalDisplay {
        id:          row.get(0)?,
        category:    row.get(1)?,
        name:        row.get(2)?,
        value:       row.get(3)?,
        description: row.get(4)?,
    })
}

fn scan_host(row: &rusqlite::Row<'_>) -> rusqlite::Result<DeploymentHost> {
    Ok(DeploymentHost {
        id:          row.get(0)?,
        name:        row.get(1)?,
        address:     row.get(2)?,
        username:    row.get(3)?,
        arch:        row.get(4)?,
        is_primary:  row.get(5)?,
        prod_dir:    row.get(6)?,
        prod_bin:    row.get(7)?,
        description: row.get(8)?,
    })
}

fn scan_server_config(row: &rusqlite::Row<'_>) -> rusqlite::Result<ServerConfig> {
    Ok(ServerConfig {
        id:          row.get(0)?,
        key:         row.get(1)?,
        value:       row.get(2)?,
        description: row.get(3)?,
    })
}

/// Collect rows from a prepared statement into a Vec using a scan function.
fn collect<T>(
    conn: &Connection,
    sql: &str,
    params: &[&dyn rusqlite::types::ToSql],
    scan: fn(&rusqlite::Row<'_>) -> rusqlite::Result<T>,
    ctx: &str,
) -> Result<Vec<T>, PlatformError> {
    let mut stmt = conn.prepare(sql)
        .map_err(|e| PlatformError::Query(ctx.into(), e))?;
    let rows = stmt.query_map(params, scan)
        .map_err(|e| PlatformError::Query(ctx.into(), e))?;
    let mut results = Vec::new();
    for row in rows {
        results.push(row.map_err(|e| PlatformError::Query(ctx.into(), e))?);
    }
    Ok(results)
}

/// Single-row query returning NotFound if missing.
fn query_one<T>(
    conn: &Connection,
    sql: &str,
    params: &[&dyn rusqlite::types::ToSql],
    scan: fn(&rusqlite::Row<'_>) -> rusqlite::Result<T>,
    ctx: String,
) -> Result<T, PlatformError> {
    conn.query_row(sql, params, scan)
        .map_err(|e| match e {
            rusqlite::Error::QueryReturnedNoRows => PlatformError::NotFound(ctx),
            other => PlatformError::Query(ctx, other),
        })
}

// ────────────────────────────────────────────────────────────────
// B.2 Core Operations (Open/Close)
// ────────────────────────────────────────────────────────────────

impl DB {
    /// Open the platform database from a directory path.
    pub fn open(db_dir: impl AsRef<Path>) -> Result<Self, PlatformError> {
        let db_path: PathBuf = db_dir.as_ref().join(DATABASE_FILE);
        let path_str = db_path.display().to_string();

        let conn = Connection::open_with_flags(
            &db_path,
            OpenFlags::SQLITE_OPEN_READ_ONLY | OpenFlags::SQLITE_OPEN_NO_MUTEX,
        )
        .map_err(|e| PlatformError::Open(path_str, e))?;

        Ok(DB { conn })
    }

    /// Open the platform database using module root path.
    pub fn open_from_root(module_root: impl AsRef<Path>) -> Result<Self, PlatformError> {
        Self::open(module_root.as_ref().join(DATABASE_DIR))
    }
}

// ────────────────────────────────────────────────────────────────
// B.3 Spec Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// Lookup a single spec by its unique key.
    pub fn spec_by_key(&self, key: &str) -> Result<Spec, PlatformError> {
        query_one(&self.conn, Q_SPEC_BY_KEY, &[&key], scan_spec, format!("spec_by_key({key})"))
    }

    /// Every spec in the database.
    pub fn all_specs(&self) -> Result<Vec<Spec>, PlatformError> {
        collect(&self.conn, Q_ALL_SPECS, &[], scan_spec, "all_specs")
    }

    /// Number of specs.
    pub fn spec_count(&self) -> Result<i64, PlatformError> {
        self.conn
            .query_row(Q_SPEC_COUNT, [], |row| row.get(0))
            .map_err(|e| PlatformError::Query("spec_count".into(), e))
    }
}

// ────────────────────────────────────────────────────────────────
// B.4 Dependency Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// What a spec depends on.
    pub fn dependencies_of(&self, spec_key: &str) -> Result<Vec<Dependency>, PlatformError> {
        collect(&self.conn, Q_DEPS_OF, &[&spec_key], scan_dep, &format!("dependencies_of({spec_key})"))
    }

    /// What depends on a spec.
    pub fn dependents_on(&self, spec_key: &str) -> Result<Vec<Dependency>, PlatformError> {
        collect(&self.conn, Q_DEPS_ON, &[&spec_key], scan_dep, &format!("dependents_on({spec_key})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.5 Health Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All 7 Hebrew-named health levels ordered by stored range.
    pub fn all_health_levels(&self) -> Result<Vec<HealthLevel>, PlatformError> {
        collect(&self.conn, Q_ALL_HEALTH_LEVELS, &[], scan_health_level, "all_health_levels")
    }

    /// Single health level by name (e.g., "broken", "even", "perfect").
    pub fn health_level_by_name(&self, name: &str) -> Result<HealthLevel, PlatformError> {
        query_one(&self.conn, Q_HEALTH_LEVEL_BY_NAME, &[&name], scan_health_level, format!("health_level_by_name({name})"))
    }

    /// All normalization bases ordered by step size.
    pub fn all_normalization_bases(&self) -> Result<Vec<HealthNormalization>, PlatformError> {
        collect(&self.conn, Q_ALL_NORM_BASES, &[], scan_norm_base, "all_normalization_bases")
    }

    /// All health scoring rules.
    pub fn all_scoring_rules(&self) -> Result<Vec<HealthScoringRule>, PlatformError> {
        collect(&self.conn, Q_ALL_SCORING_RULES, &[], scan_scoring_rule, "all_scoring_rules")
    }

    /// Scoring rules matching a name prefix (e.g., "storage_%" for storage rules).
    pub fn scoring_rules_by_category(&self, prefix: &str) -> Result<Vec<HealthScoringRule>, PlatformError> {
        let pattern = format!("{prefix}%");
        collect(&self.conn, Q_SCORING_RULES_BY_NAME_PREFIX, &[&pattern], scan_scoring_rule, &format!("scoring_rules_by_category({prefix})"))
    }

    /// All health log action types.
    pub fn all_log_actions(&self) -> Result<Vec<HealthLogAction>, PlatformError> {
        collect(&self.conn, Q_ALL_LOG_ACTIONS, &[], scan_log_action, "all_log_actions")
    }
}

// ────────────────────────────────────────────────────────────────
// B.6 DAR Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// The 3 DAR phases (DETECT, ASSESS, RESTORE).
    pub fn all_dar_phases(&self) -> Result<Vec<DARPhase>, PlatformError> {
        collect(&self.conn, Q_ALL_DAR_PHASES, &[], scan_dar_phase, "all_dar_phases")
    }

    /// The 3 DAR aspects (ENTRY, PROCESS, EXIT).
    pub fn all_dar_aspects(&self) -> Result<Vec<DARAspect>, PlatformError> {
        collect(&self.conn, Q_ALL_DAR_ASPECTS, &[], scan_dar_aspect, "all_dar_aspects")
    }

    /// The 3 severity levels (MILD, MEDIUM, CRITICAL).
    pub fn all_dar_severity(&self) -> Result<Vec<DARSeverity>, PlatformError> {
        collect(&self.conn, Q_ALL_DAR_SEVERITY, &[], scan_dar_severity, "all_dar_severity")
    }

    /// The 7 possible restore outcomes.
    pub fn all_restore_results(&self) -> Result<Vec<DARRestoreResult>, PlatformError> {
        collect(&self.conn, Q_ALL_RESTORE_RESULTS, &[], scan_restore_result, "all_restore_results")
    }

    /// The 7 restore strategies.
    pub fn all_restore_strategies(&self) -> Result<Vec<DARRestoreStrategy>, PlatformError> {
        collect(&self.conn, Q_ALL_RESTORE_STRATEGIES, &[], scan_restore_strategy, "all_restore_strategies")
    }
}

// ────────────────────────────────────────────────────────────────
// B.7 Permission Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// The 3 permission states (denied, deferred, granted).
    pub fn all_permission_states(&self) -> Result<Vec<PermissionState>, PlatformError> {
        collect(&self.conn, Q_ALL_PERM_STATES, &[], scan_perm_state, "all_permission_states")
    }

    /// The 4 action categories (safe, standard, sensitive, critical).
    pub fn all_action_categories(&self) -> Result<Vec<PermissionActionCategory>, PlatformError> {
        collect(&self.conn, Q_ALL_ACTION_CATEGORIES, &[], scan_action_category, "all_action_categories")
    }

    /// All health-to-permission gating mappings.
    pub fn all_permission_gating(&self) -> Result<Vec<PermissionGating>, PlatformError> {
        collect(&self.conn, Q_ALL_PERM_GATING, &[], scan_perm_gating, "all_permission_gating")
    }

    /// Permission state for a given health level name.
    pub fn permission_for_health(&self, health_level: &str) -> Result<PermissionGating, PlatformError> {
        query_one(&self.conn, Q_PERM_GATING_BY_HEALTH, &[&health_level], scan_perm_gating, format!("permission_for_health({health_level})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.8 Filesystem Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All biblical filesystem extensions.
    pub fn all_filesystem_extensions(&self) -> Result<Vec<FilesystemExtension>, PlatformError> {
        collect(&self.conn, Q_ALL_FS_EXTENSIONS, &[], scan_fs_extension, "all_filesystem_extensions")
    }
}

// ────────────────────────────────────────────────────────────────
// B.9 Convenience: Health Score Resolution
// ────────────────────────────────────────────────────────────────

impl DB {
    /// Returns the HealthLevel for a given stored uint8 score.
    ///
    /// The score is compared against each level's min_stored/max_stored range.
    pub fn resolve_health_level(&self, stored_score: i64) -> Result<HealthLevel, PlatformError> {
        let levels = self.all_health_levels()?;
        for level in levels {
            if stored_score >= level.min_stored && stored_score <= level.max_stored {
                return Ok(level);
            }
        }
        Err(PlatformError::NotFound(format!("resolve_health_level({stored_score})")))
    }

    /// Returns the permission state name for a stored health score.
    ///
    /// Combines health level lookup with gating to provide the permission answer.
    pub fn resolve_permission(&self, stored_score: i64) -> Result<String, PlatformError> {
        let level = self.resolve_health_level(stored_score)?;
        let gating = self.permission_for_health(&level.name)?;
        Ok(gating.permission_state)
    }
}

// ────────────────────────────────────────────────────────────────
// B.10 System Monitoring Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All system monitoring thresholds.
    pub fn all_monitoring(&self) -> Result<Vec<SystemMonitoring>, PlatformError> {
        collect(&self.conn, Q_ALL_MONITORING, &[], scan_monitoring, "all_monitoring")
    }

    /// Thresholds for a specific metric (load, memory, disk).
    pub fn monitoring_by_metric(&self, metric: &str) -> Result<Vec<SystemMonitoring>, PlatformError> {
        collect(&self.conn, Q_MONITORING_BY_METRIC, &[&metric], scan_monitoring, &format!("monitoring_by_metric({metric})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.11 Temporal Boundary Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All temporal boundary entries.
    pub fn all_temporal_boundaries(&self) -> Result<Vec<TemporalBoundary>, PlatformError> {
        collect(&self.conn, Q_ALL_TEMPORAL_BOUNDS, &[], scan_temporal_boundary, "all_temporal_boundaries")
    }

    /// Boundaries for a category (time_of_day, session_phase, circadian).
    pub fn temporal_by_category(&self, category: &str) -> Result<Vec<TemporalBoundary>, PlatformError> {
        collect(&self.conn, Q_TEMPORAL_BY_CATEGORY, &[&category], scan_temporal_boundary, &format!("temporal_by_category({category})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.12 Terminal Display Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All terminal display configuration entries.
    pub fn all_terminal_display(&self) -> Result<Vec<TerminalDisplay>, PlatformError> {
        collect(&self.conn, Q_ALL_TERMINAL_DISPLAY, &[], scan_terminal_display, "all_terminal_display")
    }

    /// Display entries by category (command_color, state_config, health_emoji, debug_marker).
    pub fn terminal_by_category(&self, category: &str) -> Result<Vec<TerminalDisplay>, PlatformError> {
        collect(&self.conn, Q_TERMINAL_BY_CATEGORY, &[&category], scan_terminal_display, &format!("terminal_by_category({category})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.13 Deployment Host Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All deployment hosts (primary first).
    pub fn all_hosts(&self) -> Result<Vec<DeploymentHost>, PlatformError> {
        collect(&self.conn, Q_ALL_HOSTS, &[], scan_host, "all_hosts")
    }

    /// Single deployment host by name.
    pub fn host_by_name(&self, name: &str) -> Result<DeploymentHost, PlatformError> {
        query_one(&self.conn, Q_HOST_BY_NAME, &[&name], scan_host, format!("host_by_name({name})"))
    }

    /// The primary deployment host.
    pub fn primary_host(&self) -> Result<DeploymentHost, PlatformError> {
        let hosts = self.all_hosts()?;
        hosts.into_iter()
            .find(|h| h.is_primary)
            .ok_or_else(|| PlatformError::NotFound("primary_host".into()))
    }
}

// ────────────────────────────────────────────────────────────────
// B.14 Server Config Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All server configuration key-value pairs.
    pub fn all_server_config(&self) -> Result<Vec<ServerConfig>, PlatformError> {
        collect(&self.conn, Q_ALL_SERVER_CONFIG, &[], scan_server_config, "all_server_config")
    }

    /// Single server config entry by key.
    pub fn server_config_by_key(&self, key: &str) -> Result<ServerConfig, PlatformError> {
        query_one(&self.conn, Q_SERVER_CONFIG_BY_KEY, &[&key], scan_server_config, format!("server_config_by_key({key})"))
    }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// This crate provides L2 platform access from Rust. The Triangle complete:
//   Configuration (TOML specs) -> Data (SQLite) -> Code (this crate)
//
// Every health level, DAR phase, permission state, and scoring rule is
// discoverable by query rather than hardcoded. The platform knows itself.
//
// "Except the LORD build the house, they labour in vain
//  that build it." — Psalm 127:1
//
// Dependencies: rusqlite (bundled SQLite — no system dep)
// Data source: b-word/data/platform/database/platform.db
//
// Safe to add: New query methods, search functions, aggregate queries
// Modify with care: Type struct fields, database schema assumptions
// Never modify: Read-only access pattern, table names

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::PathBuf;

    fn db_dir() -> PathBuf {
        let manifest = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
        manifest
            .parent().unwrap()  // hybrid/
            .parent().unwrap()  // L2-platform/
            .parent().unwrap()  // b-word/
            .join(DATABASE_DIR)
    }

    #[test]
    fn open_db() {
        let db = DB::open(db_dir());
        assert!(db.is_ok(), "Failed to open platform DB: {:?}", db.err());
    }

    #[test]
    fn spec_count() {
        let db = DB::open(db_dir()).unwrap();
        let count = db.spec_count().unwrap();
        assert!(count >= 1, "Expected >= 1 specs, got {count}");
    }

    #[test]
    fn all_specs() {
        let db = DB::open(db_dir()).unwrap();
        let specs = db.all_specs().unwrap();
        assert!(!specs.is_empty(), "Expected at least one spec");
    }

    #[test]
    fn all_health_levels() {
        let db = DB::open(db_dir()).unwrap();
        let levels = db.all_health_levels().unwrap();
        assert_eq!(levels.len(), 7, "Expected 7 health levels, got {}", levels.len());
    }

    #[test]
    fn health_level_by_name() {
        let db = DB::open(db_dir()).unwrap();
        let even = db.health_level_by_name("even").unwrap();
        assert_eq!(even.hebrew, "מֹאזְנַיִם");
    }

    #[test]
    fn all_normalization_bases() {
        let db = DB::open(db_dir()).unwrap();
        let bases = db.all_normalization_bases().unwrap();
        assert_eq!(bases.len(), 6, "Expected 6 normalization bases, got {}", bases.len());
    }

    #[test]
    fn all_scoring_rules() {
        let db = DB::open(db_dir()).unwrap();
        let rules = db.all_scoring_rules().unwrap();
        assert!(!rules.is_empty(), "Expected at least one scoring rule");
    }

    #[test]
    fn scoring_rules_by_category() {
        let db = DB::open(db_dir()).unwrap();
        let storage = db.scoring_rules_by_category("storage").unwrap();
        assert!(!storage.is_empty(), "Expected storage scoring rules");
    }

    #[test]
    fn all_log_actions() {
        let db = DB::open(db_dir()).unwrap();
        let actions = db.all_log_actions().unwrap();
        assert_eq!(actions.len(), 5, "Expected 5 log actions, got {}", actions.len());
    }

    #[test]
    fn all_dar_phases() {
        let db = DB::open(db_dir()).unwrap();
        let phases = db.all_dar_phases().unwrap();
        assert_eq!(phases.len(), 3, "Expected 3 DAR phases, got {}", phases.len());
    }

    #[test]
    fn all_dar_aspects() {
        let db = DB::open(db_dir()).unwrap();
        let aspects = db.all_dar_aspects().unwrap();
        assert_eq!(aspects.len(), 3, "Expected 3 DAR aspects, got {}", aspects.len());
    }

    #[test]
    fn all_dar_severity() {
        let db = DB::open(db_dir()).unwrap();
        let severity = db.all_dar_severity().unwrap();
        assert_eq!(severity.len(), 3, "Expected 3 severity levels, got {}", severity.len());
    }

    #[test]
    fn all_restore_results() {
        let db = DB::open(db_dir()).unwrap();
        let results = db.all_restore_results().unwrap();
        assert_eq!(results.len(), 7, "Expected 7 restore results, got {}", results.len());
    }

    #[test]
    fn all_restore_strategies() {
        let db = DB::open(db_dir()).unwrap();
        let strategies = db.all_restore_strategies().unwrap();
        assert_eq!(strategies.len(), 7, "Expected 7 restore strategies, got {}", strategies.len());
    }

    #[test]
    fn all_permission_states() {
        let db = DB::open(db_dir()).unwrap();
        let states = db.all_permission_states().unwrap();
        assert_eq!(states.len(), 3, "Expected 3 permission states, got {}", states.len());
    }

    #[test]
    fn all_action_categories() {
        let db = DB::open(db_dir()).unwrap();
        let categories = db.all_action_categories().unwrap();
        assert_eq!(categories.len(), 4, "Expected 4 action categories, got {}", categories.len());
    }

    #[test]
    fn all_permission_gating() {
        let db = DB::open(db_dir()).unwrap();
        let gating = db.all_permission_gating().unwrap();
        assert_eq!(gating.len(), 7, "Expected 7 gating entries, got {}", gating.len());
    }

    #[test]
    fn permission_for_health() {
        let db = DB::open(db_dir()).unwrap();
        let gating = db.permission_for_health("even").unwrap();
        assert_eq!(gating.permission_state, "deferred");
    }

    #[test]
    fn all_filesystem_extensions() {
        let db = DB::open(db_dir()).unwrap();
        let exts = db.all_filesystem_extensions().unwrap();
        assert_eq!(exts.len(), 6, "Expected 6 filesystem extensions, got {}", exts.len());
    }

    #[test]
    fn resolve_health_level_midpoint() {
        let db = DB::open(db_dir()).unwrap();
        // Even (yashar) range is 128-170 based on schema
        let level = db.resolve_health_level(128).unwrap();
        assert_eq!(level.name, "even");
    }

    #[test]
    fn resolve_permission_midpoint() {
        let db = DB::open(db_dir()).unwrap();
        let perm = db.resolve_permission(128).unwrap();
        assert_eq!(perm, "deferred");
    }

    #[test]
    fn resolve_health_level_extremes() {
        let db = DB::open(db_dir()).unwrap();
        // Lowest score (0) should resolve to "broken"
        let broken = db.resolve_health_level(0).unwrap();
        assert_eq!(broken.name, "broken");

        // Highest score should resolve to "perfect"
        let levels = db.all_health_levels().unwrap();
        let max_score = levels.last().unwrap().max_stored;
        let perfect = db.resolve_health_level(max_score).unwrap();
        assert_eq!(perfect.name, "perfect");
    }

    #[test]
    fn resolve_health_level_out_of_range() {
        let db = DB::open(db_dir()).unwrap();
        let result = db.resolve_health_level(99999);
        assert!(matches!(result, Err(PlatformError::NotFound(_))));
    }

    #[test]
    fn dependencies() {
        let db = DB::open(db_dir()).unwrap();
        let specs = db.all_specs().unwrap();
        if let Some(spec) = specs.first() {
            let _ = db.dependencies_of(&spec.key); // shouldn't error
        }
    }

    #[test]
    fn not_found() {
        let db = DB::open(db_dir()).unwrap();
        let result = db.health_level_by_name("nonexistent_level");
        assert!(matches!(result, Err(PlatformError::NotFound(_))));
    }

    // ── System Monitoring ──────────────────────────────────────────

    #[test]
    fn all_monitoring() {
        let db = DB::open(db_dir()).unwrap();
        let items = db.all_monitoring().unwrap();
        assert_eq!(items.len(), 6, "3 metrics × 2 levels = 6");
    }

    #[test]
    fn monitoring_by_metric() {
        let db = DB::open(db_dir()).unwrap();
        let items = db.monitoring_by_metric("load").unwrap();
        assert_eq!(items.len(), 2, "load should have yellow + red");
    }

    #[test]
    fn monitoring_unknown_metric() {
        let db = DB::open(db_dir()).unwrap();
        let items = db.monitoring_by_metric("nonexistent").unwrap();
        assert!(items.is_empty());
    }

    // ── Temporal Boundaries ────────────────────────────────────────

    #[test]
    fn all_temporal_boundaries() {
        let db = DB::open(db_dir()).unwrap();
        let items = db.all_temporal_boundaries().unwrap();
        assert_eq!(items.len(), 18, "7 + 5 + 6 = 18 boundaries");
    }

    #[test]
    fn temporal_by_category_time_of_day() {
        let db = DB::open(db_dir()).unwrap();
        let items = db.temporal_by_category("time_of_day").unwrap();
        assert_eq!(items.len(), 7);
    }

    #[test]
    fn temporal_by_category_session_phase() {
        let db = DB::open(db_dir()).unwrap();
        let items = db.temporal_by_category("session_phase").unwrap();
        assert_eq!(items.len(), 5);
    }

    #[test]
    fn temporal_by_category_circadian() {
        let db = DB::open(db_dir()).unwrap();
        let items = db.temporal_by_category("circadian").unwrap();
        assert_eq!(items.len(), 6);
    }

    // ── Terminal Display ───────────────────────────────────────────

    #[test]
    fn all_terminal_display() {
        let db = DB::open(db_dir()).unwrap();
        let items = db.all_terminal_display().unwrap();
        assert!(!items.is_empty(), "Expected terminal display entries");
    }

    #[test]
    fn terminal_command_colors() {
        let db = DB::open(db_dir()).unwrap();
        let items = db.terminal_by_category("command_color").unwrap();
        assert_eq!(items.len(), 3, "HALT, AWAIT, PROCEED");
    }

    #[test]
    fn terminal_health_emoji() {
        let db = DB::open(db_dir()).unwrap();
        let items = db.terminal_by_category("health_emoji").unwrap();
        assert_eq!(items.len(), 6, "6 health emoji levels");
    }

    // ── Deployment Hosts ───────────────────────────────────────────

    #[test]
    fn all_hosts() {
        let db = DB::open(db_dir()).unwrap();
        let hosts = db.all_hosts().unwrap();
        assert_eq!(hosts.len(), 2, "oracle + dell");
        assert!(hosts[0].is_primary, "Primary host should be first");
    }

    #[test]
    fn host_by_name_oracle() {
        let db = DB::open(db_dir()).unwrap();
        let host = db.host_by_name("oracle").unwrap();
        assert_eq!(host.arch, "arm64");
        assert!(host.is_primary);
    }

    #[test]
    fn host_by_name_dell() {
        let db = DB::open(db_dir()).unwrap();
        let host = db.host_by_name("dell").unwrap();
        assert_eq!(host.arch, "amd64");
        assert!(!host.is_primary);
    }

    #[test]
    fn primary_host() {
        let db = DB::open(db_dir()).unwrap();
        let host = db.primary_host().unwrap();
        assert_eq!(host.name, "oracle");
    }

    // ── Server Config ──────────────────────────────────────────────

    #[test]
    fn all_server_config() {
        let db = DB::open(db_dir()).unwrap();
        let configs = db.all_server_config().unwrap();
        assert!(!configs.is_empty());
    }

    #[test]
    fn server_config_port() {
        let db = DB::open(db_dir()).unwrap();
        let cfg = db.server_config_by_key("port").unwrap();
        assert_eq!(cfg.value, "3847");
    }

    #[test]
    fn server_config_health_endpoint() {
        let db = DB::open(db_dir()).unwrap();
        let cfg = db.server_config_by_key("health_endpoint").unwrap();
        assert_eq!(cfg.value, "/health");
    }
}
