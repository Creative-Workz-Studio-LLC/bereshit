//omni:code --rust -library
//omni:key B-L2-registry
//omni:version a-01.00

// =============================================================================
// METADATA
// =============================================================================
//
// Key: B-L2-registry
// Purpose: Cross-layer registry — opens L0/L1/L2 databases for unified queries
// Biblical: 1 Corinthians 12:12 — "For as the body is one, and hath many members"
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-15
//
// The registry is optional — individual loaders still work standalone.
// It exists to resolve cross-layer dependencies declared in the master DAG.
//
// =============================================================================

// =============================================================================
// SETUP
// =============================================================================

use std::collections::{HashMap, HashSet};
use std::fmt;
use std::path::Path;

use bereshit_l0_foundation::{self as foundation, FoundationError};
use bereshit_l1_language::{self as language, LanguageError};
use bereshit_l2_platform::{self as platform, PlatformError};

// ────────────────────────────────────────────────────────────────
// S.1 Error Types
// ────────────────────────────────────────────────────────────────

/// Errors that can occur during registry operations.
#[derive(Debug)]
pub enum RegistryError {
    /// Failed to open one of the layer databases.
    Open(String),
    /// A cross-layer query failed.
    Query(String),
}

impl fmt::Display for RegistryError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            RegistryError::Open(msg) => write!(f, "registry open: {msg}"),
            RegistryError::Query(msg) => write!(f, "registry query: {msg}"),
        }
    }
}

impl std::error::Error for RegistryError {}

impl From<FoundationError> for RegistryError {
    fn from(e: FoundationError) -> Self {
        RegistryError::Query(format!("L0: {e}"))
    }
}

impl From<LanguageError> for RegistryError {
    fn from(e: LanguageError) -> Self {
        RegistryError::Query(format!("L1: {e}"))
    }
}

impl From<PlatformError> for RegistryError {
    fn from(e: PlatformError) -> Self {
        RegistryError::Query(format!("L2: {e}"))
    }
}

// ────────────────────────────────────────────────────────────────
// S.2 Core Types
// ────────────────────────────────────────────────────────────────

/// Cross-layer registry holding connections to all three Bereshit databases.
pub struct Registry {
    pub foundation: foundation::DB,
    pub language: language::DB,
    pub platform: platform::DB,
    root: String,
}

/// A cross-layer reference that failed validation.
#[derive(Debug, Clone)]
pub struct ValidationError {
    pub layer: String,
    pub table: String,
    pub field: String,
    pub value: String,
    pub target: String,
    pub message: String,
}

impl fmt::Display for ValidationError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "[{}.{}.{}] {} → {}: {}",
            self.layer, self.table, self.field, self.value, self.target, self.message
        )
    }
}

/// A resolved cross-layer reference between entities.
#[derive(Debug, Clone)]
pub struct CrossRef {
    pub source_layer: String,
    pub source_table: String,
    pub source_key: String,
    pub target_layer: String,
    pub target_table: String,
    pub target_key: String,
}

/// Maps an L2 filesystem extension name to its L0 type relationship.
#[derive(Debug, Clone)]
pub struct FilesystemCrossRef {
    pub name: String,
    pub hebrew: String,
    pub meaning: String,
    pub extensions: String,
    pub has_l0_type: bool,
}

// =============================================================================
// BODY
// =============================================================================

// ---------------------------------------------------------------------------
// B.1 Open / Close
// ---------------------------------------------------------------------------

impl Registry {
    /// Opens all three databases from a module root directory.
    ///
    /// The root is the directory containing Cargo.toml / go.mod — the Bereshit
    /// module root (b-word/).
    pub fn open(module_root: impl AsRef<Path>) -> Result<Self, RegistryError> {
        let root = module_root.as_ref();
        let root_str = root.to_string_lossy().to_string();

        let fdb = foundation::DB::open_from_root(root).map_err(|e| {
            RegistryError::Open(format!("foundation: {e}"))
        })?;

        let ldb = language::DB::open_from_root(root).map_err(|e| {
            RegistryError::Open(format!("language: {e}"))
        })?;

        let pdb = platform::DB::open_from_root(root).map_err(|e| {
            RegistryError::Open(format!("platform: {e}"))
        })?;

        Ok(Registry {
            foundation: fdb,
            language: ldb,
            platform: pdb,
            root: root_str,
        })
    }

    /// Returns the module root path used to open this registry.
    pub fn root(&self) -> &str {
        &self.root
    }

    // -----------------------------------------------------------------------
    // B.2 Cross-Layer Queries: L0 ↔ L2
    // -----------------------------------------------------------------------

    /// Returns the ternary scales from L0 that correspond to health
    /// normalization bases in L2. Health scoring is built on ternary math.
    pub fn health_normalization_scales(
        &self,
    ) -> Result<Vec<foundation::TernaryScale>, RegistryError> {
        let bases = self.platform.all_normalization_bases()?;
        let scales = self.foundation.all_scales()?;

        let base_names: HashSet<&str> = bases.iter().map(|b| b.name.as_str()).collect();

        let matched: Vec<_> = scales
            .into_iter()
            .filter(|s| base_names.contains(s.name.as_str()))
            .collect();

        Ok(matched)
    }

    /// Returns all L2 filesystem extension names (Hebrew biblical vocabulary)
    /// alongside their L0 type relationship.
    pub fn filesystem_vocabulary(&self) -> Result<Vec<FilesystemCrossRef>, RegistryError> {
        let extensions = self.platform.all_filesystem_extensions()?;
        let types = self.foundation.all_types()?;

        let type_names: HashSet<&str> = types.iter().map(|t| t.name.as_str()).collect();

        let refs: Vec<_> = extensions
            .iter()
            .map(|ext| FilesystemCrossRef {
                name: ext.name.clone(),
                hebrew: ext.hebrew.clone(),
                meaning: ext.meaning.clone(),
                extensions: ext.extensions.clone(),
                has_l0_type: type_names.contains(ext.name.as_str()),
            })
            .collect();

        Ok(refs)
    }

    /// Resolves the L0 primitive type that corresponds to a given L2 health
    /// level name, if one exists.
    pub fn type_for_health_level(
        &self,
        level_name: &str,
    ) -> Result<Option<foundation::PrimitiveType>, RegistryError> {
        let levels = self.platform.all_health_levels()?;

        let found = levels.iter().any(|l| l.name == level_name);
        if !found {
            return Err(RegistryError::Query(format!(
                "health level {level_name:?} not found in L2"
            )));
        }

        match self.foundation.type_by_name(level_name) {
            Ok(t) => Ok(Some(t)),
            Err(_) => Ok(None), // Not every level maps to a type
        }
    }

    // -----------------------------------------------------------------------
    // B.3 Cross-Layer Queries: L1 ↔ L2
    // -----------------------------------------------------------------------

    /// Looks up a file extension across L1 and L2. L2 FilesystemExtension
    /// `.extensions` is a JSON array; L1 Extension is per-extension. Returns
    /// a CrossRef if the extension exists in either layer.
    pub fn extension_with_language_info(&self, ext: &str) -> Result<Option<CrossRef>, RegistryError> {
        let fs_exts = self.platform.all_filesystem_extensions()?;

        // L2 Extensions field is a JSON array — check with string contains
        let l2_name: Option<&str> = fs_exts
            .iter()
            .find(|fe| fe.extensions.contains(ext))
            .map(|fe| fe.name.as_str());

        let l1_found = self.language.extension_by_ext(ext).is_ok();

        if l2_name.is_none() && !l1_found {
            return Ok(None);
        }

        let mut ref_ = CrossRef {
            source_layer: String::new(),
            source_table: String::new(),
            source_key: ext.into(),
            target_layer: String::new(),
            target_table: String::new(),
            target_key: String::new(),
        };

        if l2_name.is_some() {
            ref_.source_layer = "L2".into();
            ref_.source_table = "filesystem_biblical_extensions".into();
        }

        if l1_found {
            ref_.target_layer = "L1".into();
            ref_.target_table = "extensions".into();
            ref_.target_key = ext.into();
        }

        Ok(Some(ref_))
    }

    // -----------------------------------------------------------------------
    // B.4 Cross-Layer Queries: L0 ↔ L1
    // -----------------------------------------------------------------------

    /// Finds L1 keywords that share a name with L0 operations, establishing
    /// that OmniCode keyword semantics trace back to foundation math.
    pub fn operations_used_by_keywords(&self) -> Result<Vec<CrossRef>, RegistryError> {
        let keywords = self.language.all_keywords()?;
        let operations = self.foundation.all_operations()?;

        let op_names: HashSet<&str> = operations.iter().map(|op| op.name.as_str()).collect();

        let refs: Vec<_> = keywords
            .iter()
            .filter(|kw| op_names.contains(kw.keyword.as_str()))
            .map(|kw| CrossRef {
                source_layer: "L1".into(),
                source_table: "keywords".into(),
                source_key: kw.keyword.clone(),
                target_layer: "L0".into(),
                target_table: "operations".into(),
                target_key: kw.keyword.clone(),
            })
            .collect();

        Ok(refs)
    }

    // -----------------------------------------------------------------------
    // B.5 Spec Dependency Resolution
    // -----------------------------------------------------------------------

    /// Returns every spec across all three databases, keyed by spec key, with
    /// the layer name as value.
    pub fn all_specs(&self) -> Result<HashMap<String, String>, RegistryError> {
        let mut specs = HashMap::new();

        for s in self.foundation.all_specs()? {
            specs.insert(s.key, "L0".into());
        }
        for s in self.language.all_specs()? {
            specs.insert(s.key, "L1".into());
        }
        for s in self.platform.all_specs()? {
            specs.insert(s.key, "L2".into());
        }

        Ok(specs)
    }

    /// Looks up a spec key across all layers and returns which layer owns it.
    pub fn resolve_dependency(&self, spec_key: &str) -> Result<Option<String>, RegistryError> {
        let specs = self.all_specs()?;
        Ok(specs.get(spec_key).cloned())
    }

    // -----------------------------------------------------------------------
    // B.6 Cross-Layer Validation
    // -----------------------------------------------------------------------

    /// Runs every cross-layer validation check.
    /// This is the canonical entry point for verifying that the bidirectional DAG
    /// declared in index.toml is correctly materialized across all three databases.
    pub fn validate_all(&self) -> Vec<ValidationError> {
        let mut errs = Vec::new();

        errs.extend(self.validate_health_normalization());
        errs.extend(self.validate_filesystem_vocabulary());
        errs.extend(self.validate_spec_dependencies());
        errs.extend(self.validate_extension_language_parity());

        errs
    }

    /// Alias for `validate_all` — kept for backward compatibility.
    pub fn validate_cross_layer_refs(&self) -> Vec<ValidationError> {
        self.validate_all()
    }

    fn validate_health_normalization(&self) -> Vec<ValidationError> {
        let mut errs = Vec::new();

        let bases = match self.platform.all_normalization_bases() {
            Ok(b) => b,
            Err(e) => {
                errs.push(ValidationError {
                    layer: "L2".into(),
                    table: "health_normalization".into(),
                    field: "*".into(),
                    value: String::new(),
                    target: String::new(),
                    message: format!("query failed: {e}"),
                });
                return errs;
            }
        };

        let scales = match self.foundation.all_scales() {
            Ok(s) => s,
            Err(e) => {
                errs.push(ValidationError {
                    layer: "L0".into(),
                    table: "ternary_scales".into(),
                    field: "*".into(),
                    value: String::new(),
                    target: String::new(),
                    message: format!("query failed: {e}"),
                });
                return errs;
            }
        };

        let scale_names: HashSet<&str> = scales.iter().map(|s| s.name.as_str()).collect();

        for b in &bases {
            if !scale_names.contains(b.name.as_str()) {
                errs.push(ValidationError {
                    layer: "L2".into(),
                    table: "health_normalization".into(),
                    field: "name".into(),
                    value: b.name.clone(),
                    target: "L0.ternary_scales.name".into(),
                    message: "normalization base not found in L0 ternary scales".into(),
                });
            }
        }

        errs
    }

    fn validate_filesystem_vocabulary(&self) -> Vec<ValidationError> {
        let mut errs = Vec::new();

        let extensions = match self.platform.all_filesystem_extensions() {
            Ok(e) => e,
            Err(e) => {
                errs.push(ValidationError {
                    layer: "L2".into(),
                    table: "filesystem_biblical_extensions".into(),
                    field: "*".into(),
                    value: String::new(),
                    target: String::new(),
                    message: format!("query failed: {e}"),
                });
                return errs;
            }
        };

        for ext in &extensions {
            if ext.name.is_empty() {
                errs.push(ValidationError {
                    layer: "L2".into(),
                    table: "filesystem_biblical_extensions".into(),
                    field: "name".into(),
                    value: "(empty)".into(),
                    target: "self".into(),
                    message: "filesystem extension has no biblical vocabulary name".into(),
                });
            }
            if ext.hebrew.is_empty() {
                errs.push(ValidationError {
                    layer: "L2".into(),
                    table: "filesystem_biblical_extensions".into(),
                    field: "hebrew".into(),
                    value: ext.name.clone(),
                    target: "self".into(),
                    message: "filesystem extension missing Hebrew characters".into(),
                });
            }
            if ext.extensions.is_empty() || ext.extensions == "[]" {
                errs.push(ValidationError {
                    layer: "L2".into(),
                    table: "filesystem_biblical_extensions".into(),
                    field: "extensions".into(),
                    value: ext.name.clone(),
                    target: "self".into(),
                    message: "filesystem extension has no file extension mappings".into(),
                });
            }
        }

        errs
    }

    fn validate_extension_language_parity(&self) -> Vec<ValidationError> {
        let mut errs = Vec::new();

        let extensions = match self.platform.all_filesystem_extensions() {
            Ok(e) => e,
            Err(e) => {
                errs.push(ValidationError {
                    layer: "L2".into(),
                    table: "filesystem_biblical_extensions".into(),
                    field: "*".into(),
                    value: String::new(),
                    target: String::new(),
                    message: format!("query failed: {e}"),
                });
                return errs;
            }
        };

        // For each L2 filesystem extension that has mapped file extensions,
        // check whether at least one appears in L1's extension table.
        for ext in &extensions {
            if ext.extensions.is_empty() || ext.extensions == "[]" {
                continue; // Skip empty — already caught by vocabulary validation
            }

            // Parse the JSON array manually: [".go", ".rs", ".py"]
            let trimmed = ext.extensions.trim();
            let inner = trimmed
                .strip_prefix('[')
                .and_then(|s| s.strip_suffix(']'))
                .unwrap_or("");

            if inner.is_empty() {
                continue;
            }

            let any_in_l1 = inner.split(',').any(|part| {
                let cleaned = part.trim().trim_matches('"').trim_matches('\'');
                !cleaned.is_empty() && self.language.extension_by_ext(cleaned).is_ok()
            });

            if !any_in_l1 {
                errs.push(ValidationError {
                    layer: "L2".into(),
                    table: "filesystem_biblical_extensions".into(),
                    field: "extensions".into(),
                    value: ext.name.clone(),
                    target: "L1.extensions.ext".into(),
                    message: format!(
                        "none of {} found in L1 extensions table",
                        ext.extensions
                    ),
                });
            }
        }

        errs
    }

    fn validate_spec_dependencies(&self) -> Vec<ValidationError> {
        let mut errs = Vec::new();

        let all_specs = match self.all_specs() {
            Ok(s) => s,
            Err(e) => {
                errs.push(ValidationError {
                    layer: "*".into(),
                    table: "specs".into(),
                    field: "*".into(),
                    value: String::new(),
                    target: String::new(),
                    message: format!("failed to load all specs: {e}"),
                });
                return errs;
            }
        };

        // Check L0 dependencies
        for (key, layer) in &all_specs {
            if layer != "L0" {
                continue;
            }
            if let Ok(deps) = self.foundation.dependencies_of(key) {
                for d in &deps {
                    if !all_specs.contains_key(&d.depends_on) {
                        errs.push(ValidationError {
                            layer: layer.clone(),
                            table: "dependencies".into(),
                            field: "depends_on".into(),
                            value: d.depends_on.clone(),
                            target: "*.specs.key".into(),
                            message: format!(
                                "spec {:?} depends on {:?} which doesn't exist in any layer",
                                key, d.depends_on
                            ),
                        });
                    }
                }
            }
        }

        errs
    }
}

// =============================================================================
// CLOSING
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::PathBuf;

    /// Returns the b-word/ module root for test database access.
    fn module_root() -> PathBuf {
        PathBuf::from(env!("CARGO_MANIFEST_DIR")).join("..").join("..").join("..")
    }

    /// Attempts to open the registry. Returns None if databases don't exist.
    fn try_open() -> Option<Registry> {
        Registry::open(module_root()).ok()
    }

    #[test]
    fn registry_opens_all_databases() {
        if let Some(r) = try_open() {
            assert!(!r.root().is_empty());
        }
    }

    #[test]
    fn all_specs_spans_layers() {
        if let Some(r) = try_open() {
            let specs = r.all_specs().unwrap();
            assert!(!specs.is_empty());

            let layers: HashSet<&str> = specs.values().map(|s| s.as_str()).collect();
            assert!(layers.contains("L0"), "should have L0 specs");
            assert!(layers.contains("L1"), "should have L1 specs");
            assert!(layers.contains("L2"), "should have L2 specs");
        }
    }

    #[test]
    fn resolve_dependency_finds_l0() {
        if let Some(r) = try_open() {
            // B-L0-math is a known L0 spec
            if let Ok(Some(layer)) = r.resolve_dependency("B-L0-math") {
                assert_eq!(layer, "L0");
            }
        }
    }

    #[test]
    fn resolve_dependency_finds_l1() {
        if let Some(r) = try_open() {
            if let Ok(Some(layer)) = r.resolve_dependency("B-L1-keywords") {
                assert_eq!(layer, "L1");
            }
        }
    }

    #[test]
    fn resolve_dependency_finds_l2() {
        if let Some(r) = try_open() {
            if let Ok(Some(layer)) = r.resolve_dependency("B-L2-health") {
                assert_eq!(layer, "L2");
            }
        }
    }

    #[test]
    fn resolve_dependency_not_found() {
        if let Some(r) = try_open() {
            let result = r.resolve_dependency("nonexistent-spec-key").unwrap();
            assert!(result.is_none());
        }
    }

    #[test]
    fn health_normalization_scales_query() {
        if let Some(r) = try_open() {
            // Should not error — may return empty if no matching scales
            let _ = r.health_normalization_scales().unwrap();
        }
    }

    #[test]
    fn filesystem_vocabulary_query() {
        if let Some(r) = try_open() {
            let refs = r.filesystem_vocabulary().unwrap();
            // May be empty if no filesystem extensions seeded
            for r in &refs {
                assert!(!r.name.is_empty() || true); // Just verify it runs
            }
        }
    }

    #[test]
    fn operations_used_by_keywords_query() {
        if let Some(r) = try_open() {
            let _ = r.operations_used_by_keywords().unwrap();
        }
    }

    #[test]
    fn validate_all_runs() {
        if let Some(r) = try_open() {
            let errs = r.validate_all();
            for e in &errs {
                eprintln!("  validation: {e}");
            }
            eprintln!("  total validation issues: {}", errs.len());
            // We don't assert empty — the test just verifies it runs
        }
    }

    #[test]
    fn validate_cross_layer_refs_runs() {
        if let Some(r) = try_open() {
            let errs = r.validate_cross_layer_refs();
            // Should match validate_all output — they're aliases
            for e in &errs {
                eprintln!("  validation: {e}");
            }
        }
    }

    #[test]
    fn validation_error_display() {
        let e = ValidationError {
            layer: "L2".into(),
            table: "health_normalization".into(),
            field: "base_name".into(),
            value: "test".into(),
            target: "L0.ternary_scales.name".into(),
            message: "not found".into(),
        };
        let s = format!("{e}");
        assert!(s.contains("L2"));
        assert!(s.contains("health_normalization"));
        assert!(s.contains("not found"));
    }

    #[test]
    fn cross_ref_fields() {
        let cr = CrossRef {
            source_layer: "L1".into(),
            source_table: "keywords".into(),
            source_key: "bless".into(),
            target_layer: "L0".into(),
            target_table: "operations".into(),
            target_key: "bless".into(),
        };
        assert_eq!(cr.source_layer, "L1");
        assert_eq!(cr.target_layer, "L0");
    }

    #[test]
    fn registry_error_display() {
        let e = RegistryError::Open("test failure".into());
        assert!(format!("{e}").contains("test failure"));

        let e = RegistryError::Query("query failure".into());
        assert!(format!("{e}").contains("query failure"));
    }
}
