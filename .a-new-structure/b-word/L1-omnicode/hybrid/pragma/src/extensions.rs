//! OmniCode File Extension Registry — The Filesystem Speaks.
//!
//! 15 extensions across 3 categories (OmniSuite, Biblical, Cognitive).
//! **Spec-driven:** loads from `extensions.toml` via L0's TOML loader at runtime.
//! Falls back to compiled defaults if the spec file isn't available.
//!
//! **L0 Connections (compiler-verified):**
//!   - [`get_registry`] calls `bereshit_l0_config::load_toml_file::<SpecRoot>(path)` to load spec
//!   - [`register_all_with_l0`] calls `bereshit_l0_config::register_format()` for each extension
//!   - [`resolve_extensions_spec`] calls `bereshit_l0_paths::bereshit_root()` to find spec location
//!
//! Go equivalent: `extensions.go` — same spec-driven pattern, same fallback.
//!
//! Genesis 1:5 — "And God called the light Day, and the darkness he called Night."
//! Naming establishes identity.

use std::collections::BTreeMap;
use std::path::PathBuf;
use std::sync::OnceLock;

use crate::types::*;

// ────────────────────────────────────────────────────────────────
// TOML Spec Types
// ────────────────────────────────────────────────────────────────

/// Single extension entry from extensions.toml.
///
/// Go equivalent: `type extSpec struct { ... }`
#[derive(serde::Deserialize)]
struct SpecEntry {
    ext: String,
    name: String,
    #[allow(dead_code)]
    category: String,
    phase: String,
    biblical_root: String,
    operation: String,
    description: String,
    internal_format: String,
    block_pattern: String,
    default_type: String,
    mime_type: String,
    state_mapping: String,
}

/// Top-level shape of extensions.toml.
/// `_pragma`, `_metadata`, `_content`, `_validation`, `_closing`,
/// `keyword_link`, `l0_parallel`, `lookup` are silently ignored by serde.
///
/// Go equivalent: `type extensionsSpec struct { ... }`
#[derive(serde::Deserialize)]
struct SpecRoot {
    omni: BTreeMap<String, SpecEntry>,
    biblical: BTreeMap<String, SpecEntry>,
    cognitive: BTreeMap<String, SpecEntry>,
}

// ────────────────────────────────────────────────────────────────
// Registry — Spec-Driven with Fallback
// ────────────────────────────────────────────────────────────────

/// Holds both the registry and whether it was loaded from the spec.
struct RegistryState {
    map: BTreeMap<String, Extension>,
    spec_loaded: bool,
}

/// Thread-safe, lazily-initialized registry.
///
/// Go equivalent: `var registryOnce sync.Once` + `var activeRegistry`
static REGISTRY: OnceLock<RegistryState> = OnceLock::new();

/// Get the active extension registry.
///
/// First call initializes: tries spec (extensions.toml via L0 loader),
/// falls back to compiled defaults. Subsequent calls return cached.
///
/// Go equivalent: `func getRegistry() map[string]*Extension`
fn get_registry() -> &'static BTreeMap<String, Extension> {
    &REGISTRY
        .get_or_init(|| {
            let spec_path = resolve_extensions_spec();
            if !spec_path.as_os_str().is_empty() {
                if let Ok(reg) = load_from_spec(&spec_path) {
                    if !reg.is_empty() {
                        return RegistryState {
                            map: reg,
                            spec_loaded: true,
                        };
                    }
                }
            }
            RegistryState {
                map: default_registry(),
                spec_loaded: false,
            }
        })
        .map
}

/// Whether the registry was loaded from extensions.toml (true)
/// or using compiled defaults (false).
///
/// Go equivalent: `func IsSpecLoaded() bool`
pub fn is_spec_loaded() -> bool {
    let _ = get_registry(); // ensure initialized
    REGISTRY.get().is_some_and(|s| s.spec_loaded)
}

// ────────────────────────────────────────────────────────────────
// Spec Loading — L0 Connection
// ────────────────────────────────────────────────────────────────

/// Find the extensions.toml spec file.
///
/// Resolution order:
/// 1. `BERESHIT_EXTENSIONS_SPEC` env var (exact path)
/// 2. `{bereshit_root}/.a-new-structure/b-word/L1-omnicode/ladder/language/extensions.toml`
///
/// Go equivalent: `func resolveExtensionsSpec() string`
fn resolve_extensions_spec() -> PathBuf {
    // 1. Explicit override
    if let Ok(p) = std::env::var("BERESHIT_EXTENSIONS_SPEC") {
        let path = PathBuf::from(p);
        if path.exists() {
            return path;
        }
    }

    // 2. Relative to bereshit root — uses L0 paths
    let root = bereshit_l0_paths::bereshit_root();
    let spec = root
        .join(".a-new-structure")
        .join("b-word")
        .join("L1-omnicode")
        .join("ladder")
        .join("language")
        .join("extensions.toml");

    if spec.exists() {
        return spec;
    }

    PathBuf::new() // empty = not found
}

/// Load the extension registry from a TOML spec file via L0's loader.
///
/// This is THE L0-L1 bridge for extensions — the spec drives runtime.
///
/// Go equivalent: `func loadFromSpec(path string) (map[string]*Extension, error)`
fn load_from_spec(
    path: &std::path::Path,
) -> Result<BTreeMap<String, Extension>, bereshit_l0_config::ConfigError> {
    let spec: SpecRoot = bereshit_l0_config::load_toml_file(path)?;
    let mut registry = BTreeMap::new();

    for entry in spec.omni.values() {
        registry.insert(
            entry.ext.clone(),
            spec_to_extension(entry, Category::OmniSuite),
        );
    }
    for entry in spec.biblical.values() {
        registry.insert(
            entry.ext.clone(),
            spec_to_extension(entry, Category::Biblical),
        );
    }
    for entry in spec.cognitive.values() {
        registry.insert(
            entry.ext.clone(),
            spec_to_extension(entry, Category::Cognitive),
        );
    }

    Ok(registry)
}

/// Convert a TOML spec entry to a runtime Extension.
///
/// Category comes from the TOML section (omni/biblical/cognitive),
/// not from the entry's own `category` field — section IS truth.
///
/// Go equivalent: `func specToExtension(spec extSpec, cat Category) *Extension`
fn spec_to_extension(entry: &SpecEntry, category: Category) -> Extension {
    Extension {
        ext: entry.ext.clone(),
        name: entry.name.clone(),
        category,
        phase: entry.phase.clone(),
        biblical_root: entry.biblical_root.clone(),
        operation: entry.operation.clone(),
        description: entry.description.clone(),
        internal_format: Format::from_str_loose(&entry.internal_format).unwrap_or_default(),
        block_pattern: entry.block_pattern.clone(),
        default_type: FileType::from_str_loose(&entry.default_type).unwrap_or(FileType::Data),
        mime_type: entry.mime_type.clone(),
        state_map: parse_state_mapping(&entry.state_mapping),
    }
}

/// Parse state mapping from "COMMAND — description" format.
///
/// The em dash (U+2014) separates the command from the description.
/// Go equivalent: `func parseStateMapping(s string) StateMapping`
fn parse_state_mapping(s: &str) -> StateMapping {
    let cmd = s.split('\u{2014}').next().unwrap_or(s).trim();
    match cmd {
        "AWAIT" => StateMapping::Await,
        "PROCEED" => StateMapping::Proceed,
        "CLEAR" => StateMapping::Clear,
        "PIVOT" => StateMapping::Pivot,
        "HALT" => StateMapping::Halt,
        _ => StateMapping::Proceed,
    }
}

// ────────────────────────────────────────────────────────────────
// Default Registry (Fallback)
// ────────────────────────────────────────────────────────────────

/// Compiled extension data — used when extensions.toml is not available.
///
/// THIS IS THE FALLBACK, NOT THE SOURCE OF TRUTH.
/// The TOML spec is truth. This exists for environments where
/// the spec file isn't on disk (CI, standalone binary, etc).
///
/// Go equivalent: `func defaultRegistry() map[string]*Extension`
fn default_registry() -> BTreeMap<String, Extension> {
    BTreeMap::from([
        // ── Omni-Suite ───────────────────────────────────────────────
        (
            EXT_OMNI.into(),
            Extension {
                ext: EXT_OMNI.into(),
                name: "OmniCode Source".into(),
                category: Category::OmniSuite,
                phase: "a".into(),
                biblical_root: "Genesis 1:3".into(),
                operation: "source".into(),
                description: "General-purpose OmniCode source file".into(),
                internal_format: Format::Omni,
                block_pattern: "varies".into(),
                default_type: FileType::Code,
                mime_type: "text/x-omnicode".into(),
                state_map: StateMapping::Proceed,
            },
        ),
        (
            EXT_OMNIFOLD.into(),
            Extension {
                ext: EXT_OMNIFOLD.into(),
                name: "OmniFold".into(),
                category: Category::OmniSuite,
                phase: "a".into(),
                biblical_root: "Genesis 1:9".into(),
                operation: "folder".into(),
                description: "Folder metadata \u{2014} declares what a directory IS".into(),
                internal_format: Format::Toml,
                block_pattern: "3-block".into(),
                default_type: FileType::Folder,
                mime_type: "application/x-omnifold+toml".into(),
                state_map: StateMapping::Proceed,
            },
        ),
        (
            EXT_OMNI_CONFIG.into(),
            Extension {
                ext: EXT_OMNI_CONFIG.into(),
                name: "OmniConfig".into(),
                category: Category::OmniSuite,
                phase: "a".into(),
                biblical_root: "Proverbs 3:5-6".into(),
                operation: "config".into(),
                description: "Configuration file \u{2014} settings, preferences, build options"
                    .into(),
                internal_format: Format::Toml,
                block_pattern: "3-block".into(),
                default_type: FileType::Data,
                mime_type: "application/x-omniconfig+toml".into(),
                state_map: StateMapping::Proceed,
            },
        ),
        (
            EXT_OMNI_INTERFACE.into(),
            Extension {
                ext: EXT_OMNI_INTERFACE.into(),
                name: "OmniInterface".into(),
                category: Category::OmniSuite,
                phase: "b".into(),
                biblical_root: "Exodus 19:5".into(),
                operation: "interface".into(),
                description: "Interface definition \u{2014} API contracts, covenant surfaces"
                    .into(),
                internal_format: Format::Omni,
                block_pattern: "4-block".into(),
                default_type: FileType::Interface,
                mime_type: "text/x-omniinterface".into(),
                state_map: StateMapping::Await,
            },
        ),
        (
            EXT_OMNI_MANIFEST.into(),
            Extension {
                ext: EXT_OMNI_MANIFEST.into(),
                name: "OmniManifest".into(),
                category: Category::OmniSuite,
                phase: "b".into(),
                biblical_root: "Numbers 1:2".into(),
                operation: "manifest".into(),
                description: "Package manifest \u{2014} dependencies, exports, versioning".into(),
                internal_format: Format::Toml,
                block_pattern: "3-block".into(),
                default_type: FileType::Data,
                mime_type: "application/x-omnimanifest+toml".into(),
                state_map: StateMapping::Proceed,
            },
        ),
        // ── Biblical ────────────────────────────────────────────────
        (
            EXT_GENESIS.into(),
            Extension {
                ext: EXT_GENESIS.into(),
                name: "Genesis".into(),
                category: Category::Biblical,
                phase: "a".into(),
                biblical_root: "Genesis 1:1".into(),
                operation: "encode".into(),
                description: "Encoding/generation output \u{2014} creation in binary form".into(),
                internal_format: Format::Binary,
                block_pattern: "none".into(),
                default_type: FileType::Data,
                mime_type: "application/x-omni-genesis".into(),
                state_map: StateMapping::Proceed,
            },
        ),
        (
            EXT_REVEAL.into(),
            Extension {
                ext: EXT_REVEAL.into(),
                name: "Revelation".into(),
                category: Category::Biblical,
                phase: "a".into(),
                biblical_root: "Revelation 1:1".into(),
                operation: "decode".into(),
                description: "Decoding/revelation \u{2014} encoded content made readable".into(),
                internal_format: Format::Text,
                block_pattern: "none".into(),
                default_type: FileType::Data,
                mime_type: "text/x-omni-revelation".into(),
                state_map: StateMapping::Proceed,
            },
        ),
        (
            EXT_WITNESS.into(),
            Extension {
                ext: EXT_WITNESS.into(),
                name: "Witness".into(),
                category: Category::Biblical,
                phase: "a".into(),
                biblical_root: "Deuteronomy 19:15".into(),
                operation: "attest".into(),
                description: "Attestation \u{2014} checksums, signatures, verification records"
                    .into(),
                internal_format: Format::Jsonc,
                block_pattern: "3-block".into(),
                default_type: FileType::Data,
                mime_type: "application/x-omni-witness+json".into(),
                state_map: StateMapping::Halt,
            },
        ),
        (
            EXT_COVENANT.into(),
            Extension {
                ext: EXT_COVENANT.into(),
                name: "Covenant".into(),
                category: Category::Biblical,
                phase: "a".into(),
                biblical_root: "Genesis 9:9".into(),
                operation: "bind".into(),
                description: "Agreement files \u{2014} licenses, covenants, terms of service"
                    .into(),
                internal_format: Format::Toml,
                block_pattern: "3-block".into(),
                default_type: FileType::Data,
                mime_type: "application/x-omni-covenant+toml".into(),
                state_map: StateMapping::Proceed,
            },
        ),
        (
            EXT_SCROLL.into(),
            Extension {
                ext: EXT_SCROLL.into(),
                name: "Scroll".into(),
                category: Category::Biblical,
                phase: "b".into(),
                biblical_root: "Revelation 5:1".into(),
                operation: "log".into(),
                description: "Event logs and audit trails \u{2014} append-only history".into(),
                internal_format: Format::Jsonc,
                block_pattern: "3-block".into(),
                default_type: FileType::Data,
                mime_type: "application/x-omni-scroll+json".into(),
                state_map: StateMapping::Proceed,
            },
        ),
        (
            EXT_SEED.into(),
            Extension {
                ext: EXT_SEED.into(),
                name: "Seed".into(),
                category: Category::Biblical,
                phase: "a".into(),
                biblical_root: "Genesis 1:11".into(),
                operation: "template".into(),
                description:
                    "Template and generative patterns \u{2014} seeds yield fruit when planted"
                        .into(),
                internal_format: Format::Omni,
                block_pattern: "varies".into(),
                default_type: FileType::Template,
                mime_type: "text/x-omni-seed".into(),
                state_map: StateMapping::Await,
            },
        ),
        (
            EXT_TOV.into(),
            Extension {
                ext: EXT_TOV.into(),
                name: "Tov".into(),
                category: Category::Biblical,
                phase: "a".into(),
                biblical_root: "Genesis 1:31".into(),
                operation: "complete".into(),
                description:
                    "Completion marker \u{2014} tov (good), God's assessment after creation".into(),
                internal_format: Format::Toml,
                block_pattern: "3-block".into(),
                default_type: FileType::Data,
                mime_type: "application/x-omni-tov+toml".into(),
                state_map: StateMapping::Halt,
            },
        ),
        // ── Cognitive ───────────────────────────────────────────────
        (
            EXT_HEALTH.into(),
            Extension {
                ext: EXT_HEALTH.into(),
                name: "Health".into(),
                category: Category::Cognitive,
                phase: "b".into(),
                biblical_root: "Leviticus 19:36".into(),
                operation: "score".into(),
                description: "Health state scoring \u{2014} ternary scale (-100 to +100)".into(),
                internal_format: Format::Toml,
                block_pattern: "3-block".into(),
                default_type: FileType::Data,
                mime_type: "application/x-omni-health+toml".into(),
                state_map: StateMapping::Clear,
            },
        ),
        (
            EXT_HALTS.into(),
            Extension {
                ext: EXT_HALTS.into(),
                name: "Halts".into(),
                category: Category::Cognitive,
                phase: "b".into(),
                biblical_root: "Genesis 1:1".into(),
                operation: "anchor".into(),
                description: "Anchor definitions \u{2014} where recursion stops".into(),
                internal_format: Format::Toml,
                block_pattern: "3-block".into(),
                default_type: FileType::Data,
                mime_type: "application/x-omni-halts+toml".into(),
                state_map: StateMapping::Halt,
            },
        ),
        (
            EXT_CUBE.into(),
            Extension {
                ext: EXT_CUBE.into(),
                name: "Cube".into(),
                category: Category::Cognitive,
                phase: "b".into(),
                biblical_root: "Revelation 21:16".into(),
                operation: "position".into(),
                description: "Positional state \u{2014} 27-position cube coordinates".into(),
                internal_format: Format::Toml,
                block_pattern: "3-block".into(),
                default_type: FileType::Data,
                mime_type: "application/x-omni-cube+toml".into(),
                state_map: StateMapping::Pivot,
            },
        ),
    ])
}

// ────────────────────────────────────────────────────────────────
// Lookup Functions
// ────────────────────────────────────────────────────────────────

/// Look up an extension definition by its file extension string.
///
/// Go equivalent: `func LookupExt(ext string) (*Extension, bool)`
pub fn lookup(ext: &str) -> Option<&Extension> {
    get_registry().get(ext)
}

/// Whether an extension is registered in the OmniCode registry.
///
/// Go equivalent: `func IsRegisteredExt(ext string) bool`
pub fn is_registered(ext: &str) -> bool {
    get_registry().contains_key(ext)
}

/// All extensions belonging to a category.
///
/// Go equivalent: `func ExtsByCategory(cat Category) []*Extension`
pub fn by_category(cat: Category) -> Vec<&'static Extension> {
    get_registry()
        .values()
        .filter(|e| e.category == cat)
        .collect()
}

/// All extensions in a given phase ("a" or "b").
///
/// Go equivalent: `func ExtsByPhase(phase string) []*Extension`
pub fn by_phase(phase: &str) -> Vec<&Extension> {
    get_registry()
        .values()
        .filter(|e| e.phase == phase)
        .collect()
}

/// All extensions mapped to a state machine command.
///
/// Go equivalent: `func ExtsByState(state StateMapping) []*Extension`
pub fn by_state(state: StateMapping) -> Vec<&'static Extension> {
    get_registry()
        .values()
        .filter(|e| e.state_map == state)
        .collect()
}

/// Every registered extension.
///
/// Go equivalent: `func AllExts() []*Extension`
pub fn all() -> Vec<&'static Extension> {
    get_registry().values().collect()
}

// ────────────────────────────────────────────────────────────────
// Format Bridge
// ────────────────────────────────────────────────────────────────

/// Internal format for an OmniCode extension.
///
/// Go equivalent: `func InternalFormatForExt(ext string) Format`
pub fn internal_format(ext: &str) -> Option<Format> {
    get_registry().get(ext).map(|e| e.internal_format)
}

/// Expected block pattern for an extension.
///
/// Go equivalent: `func BlockPatternForExt(ext string) string`
pub fn block_pattern(ext: &str) -> Option<&str> {
    get_registry().get(ext).map(|e| e.block_pattern.as_str())
}

/// Default pragma type for an extension.
///
/// Go equivalent: `func DefaultTypeForExt(ext string) Type`
pub fn default_type(ext: &str) -> Option<FileType> {
    get_registry().get(ext).map(|e| e.default_type)
}

/// Whether a path has an OmniCode extension (.omni or .ofd).
///
/// Go equivalent: `func IsOmniCodeFile(path string) bool`
pub fn is_omnicode_file(path: &std::path::Path) -> bool {
    path.extension().and_then(|e| e.to_str()).is_some_and(|e| {
        let ext = format!(".{e}");
        ext == EXT_OMNI || ext == EXT_OMNIFOLD
    })
}

// ────────────────────────────────────────────────────────────────
// L0 Registration Bridge
// ────────────────────────────────────────────────────────────────

/// Register all OmniCode extensions with L0's format registry.
///
/// This is THE connection point — L0's `formats.rs` was designed for this call
/// (doc comment: "L1 calls this to register OmniCode-specific extensions")
/// but it had never been exercised until now.
///
/// The compiler witnessing this call succeed proves the L0-L1 contract is real.
pub fn register_all_with_l0() {
    for ext in get_registry().values() {
        bereshit_l0_config::register_format(&ext.ext, &ext.internal_format.to_string());

        // Register config formats (extensions whose content is parseable config)
        if ext.internal_format.is_config_format() {
            bereshit_l0_config::register_config_format(&ext.internal_format.to_string());
        }
    }
}

// ────────────────────────────────────────────────────────────────
// Tests
// ────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::Path;

    #[test]
    fn test_registry_has_15_extensions() {
        // 5 omni-suite + 7 biblical + 3 cognitive = 15
        assert_eq!(all().len(), 15);
    }

    #[test]
    fn test_lookup_known() {
        let ext = lookup(EXT_OMNI).expect(".omni should be registered");
        assert_eq!(ext.name, "OmniCode Source");
        assert_eq!(ext.category, Category::OmniSuite);
        assert_eq!(ext.operation, "source");
    }

    #[test]
    fn test_lookup_unknown() {
        assert!(lookup(".xyz").is_none());
        assert!(!is_registered(".xyz"));
    }

    #[test]
    fn test_by_category_omni_suite() {
        let exts = by_category(Category::OmniSuite);
        assert_eq!(exts.len(), 5); // .omni, .ofd, .omc, .omi, .omx
    }

    #[test]
    fn test_by_category_biblical() {
        let exts = by_category(Category::Biblical);
        assert_eq!(exts.len(), 7); // .gen, .rev, .witness, .covenant, .scroll, .seed, .tov
    }

    #[test]
    fn test_by_category_cognitive() {
        let exts = by_category(Category::Cognitive);
        assert_eq!(exts.len(), 3); // .health, .halts, .cube
    }

    #[test]
    fn test_category_counts_sum() {
        let omni = by_category(Category::OmniSuite).len();
        let bib = by_category(Category::Biblical).len();
        let cog = by_category(Category::Cognitive).len();
        assert_eq!(omni + bib + cog, 15);
        assert_eq!(omni + bib + cog, all().len());
    }

    #[test]
    fn test_by_phase_a() {
        let phase_a = by_phase("a");
        // Phase a: .omni, .ofd, .omc, .gen, .rev, .witness, .covenant, .seed, .tov = 9
        assert!(phase_a.len() >= 9);
    }

    #[test]
    fn test_by_phase_b() {
        let phase_b = by_phase("b");
        // Phase b: .omi, .omx, .scroll, .health, .halts, .cube = 6
        assert!(phase_b.len() >= 5);
    }

    #[test]
    fn test_by_state() {
        let halt = by_state(StateMapping::Halt);
        // .witness, .tov, .halts
        assert!(halt.len() >= 3);
    }

    #[test]
    fn test_internal_format() {
        assert_eq!(internal_format(EXT_OMNIFOLD), Some(Format::Toml));
        assert_eq!(internal_format(EXT_WITNESS), Some(Format::Jsonc));
        assert_eq!(internal_format(EXT_SCROLL), Some(Format::Jsonc));
        assert_eq!(internal_format(EXT_OMNI), Some(Format::Omni));
        assert_eq!(internal_format(EXT_GENESIS), Some(Format::Binary));
        assert_eq!(internal_format(EXT_REVEAL), Some(Format::Text));
        assert_eq!(internal_format(".xyz"), None);
    }

    #[test]
    fn test_block_pattern() {
        assert_eq!(block_pattern(EXT_OMNIFOLD), Some("3-block"));
        assert_eq!(block_pattern(EXT_OMNI), Some("varies"));
        assert_eq!(block_pattern(EXT_GENESIS), Some("none"));
        assert_eq!(block_pattern(".xyz"), None);
    }

    #[test]
    fn test_default_type() {
        assert_eq!(default_type(EXT_OMNI), Some(FileType::Code));
        assert_eq!(default_type(EXT_OMNIFOLD), Some(FileType::Folder));
        assert_eq!(default_type(EXT_SEED), Some(FileType::Template));
        assert_eq!(default_type(".xyz"), None);
    }

    #[test]
    fn test_is_omnicode_file() {
        assert!(is_omnicode_file(Path::new("root.omni")));
        assert!(is_omnicode_file(Path::new("dir.ofd")));
        assert!(!is_omnicode_file(Path::new("config.toml")));
        assert!(!is_omnicode_file(Path::new("main.go")));
    }

    #[test]
    fn test_register_all_with_l0() {
        // This exercises the L0 connection.
        // After registration, L0 should know about OmniCode extensions.
        register_all_with_l0();

        assert!(bereshit_l0_config::is_known_ext(EXT_OMNI));
        assert!(bereshit_l0_config::is_known_ext(EXT_OMNIFOLD));
        assert!(bereshit_l0_config::is_known_ext(EXT_WITNESS));
        assert!(bereshit_l0_config::is_known_ext(EXT_HEALTH));

        // Format should be retrievable
        assert_eq!(
            bereshit_l0_config::format_from_ext(EXT_OMNIFOLD),
            Some("toml".to_owned())
        );
    }

    #[test]
    fn test_parse_state_mapping() {
        assert_eq!(
            parse_state_mapping("PROCEED \u{2014} active content"),
            StateMapping::Proceed
        );
        assert_eq!(
            parse_state_mapping("HALT \u{2014} verification complete"),
            StateMapping::Halt
        );
        assert_eq!(
            parse_state_mapping("AWAIT \u{2014} waiting"),
            StateMapping::Await
        );
        assert_eq!(
            parse_state_mapping("CLEAR \u{2014} reset"),
            StateMapping::Clear
        );
        assert_eq!(
            parse_state_mapping("PIVOT \u{2014} direction change"),
            StateMapping::Pivot
        );
        assert_eq!(
            parse_state_mapping("UNKNOWN \u{2014} something"),
            StateMapping::Proceed
        );
    }

    // ── Spec-Driven Assurance Tests ──────────────────────────────

    #[test]
    fn test_is_spec_loaded() {
        let root = bereshit_l0_paths::bereshit_root();
        let spec_path = root
            .join(".a-new-structure")
            .join("b-word")
            .join("L1-omnicode")
            .join("ladder")
            .join("language")
            .join("extensions.toml");

        if spec_path.exists() {
            assert!(
                is_spec_loaded(),
                "spec should be loaded when extensions.toml exists on disk"
            );
        }
    }

    /// Cross-check: spec-loaded registry must produce the same data as the
    /// compiled default registry. This catches drift in EITHER direction.
    ///
    /// If someone edits extensions.toml without updating defaults → fails.
    /// If someone edits defaults without updating extensions.toml → fails.
    #[test]
    fn test_spec_matches_defaults() {
        if !is_spec_loaded() {
            eprintln!("Skipping spec-vs-defaults: spec not loaded");
            return;
        }

        let defaults = default_registry();
        let active = get_registry();

        assert_eq!(
            active.len(),
            defaults.len(),
            "active has {} entries, defaults has {}",
            active.len(),
            defaults.len()
        );

        for (ext, def) in &defaults {
            let act = active
                .get(ext)
                .unwrap_or_else(|| panic!("{ext} in defaults but not in active registry"));

            assert_eq!(act.name, def.name, "{ext} name mismatch");
            assert_eq!(act.category, def.category, "{ext} category mismatch");
            assert_eq!(act.phase, def.phase, "{ext} phase mismatch");
            assert_eq!(act.operation, def.operation, "{ext} operation mismatch");
            assert_eq!(
                act.internal_format, def.internal_format,
                "{ext} internal_format mismatch"
            );
            assert_eq!(
                act.block_pattern, def.block_pattern,
                "{ext} block_pattern mismatch"
            );
            assert_eq!(
                act.default_type, def.default_type,
                "{ext} default_type mismatch"
            );
            assert_eq!(act.state_map, def.state_map, "{ext} state_map mismatch");
        }
    }

    /// Independent spec load: verify the TOML structure via L0's loader
    /// and cross-check format_bridge tables.
    #[test]
    fn test_spec_format_bridge() {
        let root = bereshit_l0_paths::bereshit_root();
        let spec_path = root
            .join(".a-new-structure")
            .join("b-word")
            .join("L1-omnicode")
            .join("ladder")
            .join("language")
            .join("extensions.toml");

        if !spec_path.exists() {
            eprintln!("Skipping format_bridge check: extensions.toml not found");
            return;
        }

        /// Minimal shape for reading format_bridge from TOML.
        #[derive(serde::Deserialize)]
        struct BridgeRoot {
            format_bridge: FormatBridge,
        }
        #[derive(serde::Deserialize)]
        struct FormatBridge {
            mappings: BTreeMap<String, String>,
            block_patterns: BTreeMap<String, String>,
        }

        let spec: BridgeRoot =
            bereshit_l0_config::load_toml_file(&spec_path).expect("load extensions.toml");

        // Cross-check format_bridge.mappings against active registry
        for (ext, spec_fmt) in &spec.format_bridge.mappings {
            let reg_fmt = internal_format(ext)
                .unwrap_or_else(|| panic!("{ext} in format_bridge.mappings but not in registry"));
            assert_eq!(
                reg_fmt.to_string(),
                *spec_fmt,
                "format_bridge {ext} mismatch"
            );
        }

        // Cross-check format_bridge.block_patterns
        for (ext, spec_pat) in &spec.format_bridge.block_patterns {
            let reg_pat = block_pattern(ext)
                .unwrap_or_else(|| panic!("{ext} in block_patterns but not in registry"));
            assert_eq!(reg_pat, *spec_pat, "block_pattern {ext} mismatch");
        }
    }
}
