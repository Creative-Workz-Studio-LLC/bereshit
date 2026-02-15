//! Data types for the configuration system — manifests, specs, and results.

use serde::Deserialize;
use std::collections::BTreeMap;
use std::path::PathBuf;

use crate::error::ConfigError;

// ────────────────────────────────────────────────────────────────
// Manifest Types (Serde-deserialized from index.toml)
// ────────────────────────────────────────────────────────────────

/// Parsed index.toml manifest — the single source of truth for spec loading.
#[derive(Debug, Clone, Deserialize)]
pub struct IndexManifest {
    /// Systems in dependency order (sorted by `order` after load).
    pub systems: Vec<SystemEntry>,
}

/// A system within the manifest (e.g., math, types, bible).
#[derive(Debug, Clone, Deserialize)]
pub struct SystemEntry {
    /// Human-readable name (e.g., "math").
    pub name: String,
    /// Path relative to module root (e.g., "L0-universal/ladder/foundation/math").
    pub path: String,
    /// Dependency order (0 = anchor, higher = depends on lower).
    pub order: i32,
    /// Specs within this system.
    pub specs: Vec<SpecEntry>,
}

/// A single spec file within a system.
#[derive(Debug, Clone, Deserialize)]
pub struct SpecEntry {
    /// Filename (e.g., "ternary.toml").
    pub file: String,
    /// OmniCode key (e.g., "B-L0-math-ternary").
    pub key: String,
    /// Dependencies as "system/file.toml" paths.
    #[serde(default)]
    pub depends_on: Vec<String>,
}

// ────────────────────────────────────────────────────────────────
// Loaded Config Types
// ────────────────────────────────────────────────────────────────

/// A loaded TOML configuration file with extracted identity.
#[derive(Debug, Clone)]
pub struct ConfigFile {
    /// Filename (e.g., "ternary.toml").
    pub name: String,
    /// Full filesystem path.
    pub path: PathBuf,
    /// Raw TOML data for dynamic access.
    pub data: toml::Table,
    /// Sorted top-level section names.
    pub keys: Vec<String>,
    /// Extracted `[_pragma]` identity table (P1-P5).
    pub pragma: Option<BTreeMap<String, String>>,
    /// Extracted `[_metadata]` identity table (M1-M10).
    pub metadata: Option<BTreeMap<String, String>>,
}

// ────────────────────────────────────────────────────────────────
// Result Types
// ────────────────────────────────────────────────────────────────

/// Batch loading result — summary of what loaded and what failed.
#[derive(Debug, Clone)]
pub struct LoadResult {
    /// True if all specs loaded without errors.
    pub valid: bool,
    /// Loaded configs keyed by system name.
    pub configs: BTreeMap<String, Vec<ConfigFile>>,
    /// All errors encountered during loading.
    pub errors: Vec<ConfigError>,
    /// Quick reference: system name -> list of loaded filenames.
    pub summary: BTreeMap<String, Vec<String>>,
}

impl LoadResult {
    pub(crate) fn new() -> Self {
        Self {
            valid: true,
            configs: BTreeMap::new(),
            errors: Vec::new(),
            summary: BTreeMap::new(),
        }
    }
}

/// Manifest vs filesystem comparison — tripwire detection.
#[derive(Debug, Clone)]
pub struct DiscoveryResult {
    /// Files declared in the manifest.
    pub manifest: Vec<String>,
    /// Files found on disk.
    pub discovered: Vec<String>,
    /// ERROR: in manifest but not on disk.
    pub missing: Vec<String>,
    /// WARNING: on disk but not in manifest.
    pub unexpected: Vec<String>,
    /// True if no missing files.
    pub valid: bool,
}

impl DiscoveryResult {
    pub(crate) fn new() -> Self {
        Self {
            manifest: Vec::new(),
            discovered: Vec::new(),
            missing: Vec::new(),
            unexpected: Vec::new(),
            valid: true,
        }
    }
}

/// A node in the dependency graph (DAG).
#[derive(Debug, Clone)]
pub struct DependencyNode {
    /// Spec identifier ("system_path/file.toml").
    pub spec: String,
    /// Dependencies in same format.
    pub depends_on: Vec<String>,
    /// Tracking flag for load order validation.
    pub loaded: bool,
}
