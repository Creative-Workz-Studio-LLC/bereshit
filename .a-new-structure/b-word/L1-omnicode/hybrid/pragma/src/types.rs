//! OmniCode pragma types — what files ARE, not what they contain.
//!
//! Ported from `pragma.go` and `extensions.go` with Rust idioms.
//! TOML spec is truth, Go is first implementation, Rust is architectural witness.
//!
//! Genesis 1:3 — "And God said, Let there be light."
//! Types declare identity. Before parsing, before content, identity is spoken.

use std::fmt;

// ────────────────────────────────────────────────────────────────
// File Type — What a file IS
// ────────────────────────────────────────────────────────────────

/// What an OmniCode file IS — its fundamental nature.
///
/// Go equivalent: `type Type string` with const block.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum FileType {
    Universal,
    Template,
    Code,
    Documentation,
    Interface,
    Data,
    Folder,
    Spec,
}

impl FileType {
    /// Parse from the string found in pragma line 1.
    pub fn from_str_loose(s: &str) -> Option<Self> {
        match s {
            "universal" => Some(Self::Universal),
            "template" => Some(Self::Template),
            "code" => Some(Self::Code),
            "documentation" | "doc" => Some(Self::Documentation),
            "interface" => Some(Self::Interface),
            "data" => Some(Self::Data),
            "folder" => Some(Self::Folder),
            "spec" => Some(Self::Spec),
            _ => None,
        }
    }

    /// The expected block pattern for this file type.
    ///
    /// Go equivalent: `func (p *Pragma) BlockPattern() string`
    pub fn block_pattern(self) -> &'static str {
        match self {
            Self::Data | Self::Interface | Self::Folder => "3-block",
            Self::Code | Self::Template | Self::Universal | Self::Spec => "4-block",
            Self::Documentation => "5-block",
        }
    }
}

impl fmt::Display for FileType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.write_str(match self {
            Self::Universal => "universal",
            Self::Template => "template",
            Self::Code => "code",
            Self::Documentation => "documentation",
            Self::Interface => "interface",
            Self::Data => "data",
            Self::Folder => "folder",
            Self::Spec => "spec",
        })
    }
}

// ────────────────────────────────────────────────────────────────
// Format — Target language / data format
// ────────────────────────────────────────────────────────────────

/// The target language or data format of an OmniCode file.
///
/// Go equivalent: `type Format string` with const block.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Format {
    Omni,
    Go,
    C,
    Rs,
    Adoc,
    Md,
    Json,
    Jsonc, // JSONC with comments (.witness, .scroll internal format)
    Yaml,
    Toml,
    Txt,
    Text,   // Plain text decoding output (.rev internal format)
    Binary, // Compiled/encoded output (.gen internal format)
}

impl Format {
    /// Parse from the `--format` flag in pragma line 1, or from
    /// `internal_format` values in extensions.toml.
    pub fn from_str_loose(s: &str) -> Option<Self> {
        match s {
            "omni" => Some(Self::Omni),
            "go" => Some(Self::Go),
            "c" => Some(Self::C),
            "rs" | "rust" => Some(Self::Rs),
            "adoc" | "asciidoc" => Some(Self::Adoc),
            "md" | "markdown" => Some(Self::Md),
            "json" => Some(Self::Json),
            "jsonc" => Some(Self::Jsonc),
            "yaml" | "yml" => Some(Self::Yaml),
            "toml" => Some(Self::Toml),
            "txt" => Some(Self::Txt),
            "text" => Some(Self::Text),
            "binary" => Some(Self::Binary),
            _ => None,
        }
    }

    /// Whether this format uses config-style content (TOML, JSON, JSONC, YAML).
    pub fn is_config_format(self) -> bool {
        matches!(self, Self::Toml | Self::Json | Self::Jsonc | Self::Yaml)
    }
}

impl Default for Format {
    /// Default format is OmniCode native.
    fn default() -> Self {
        Self::Omni
    }
}

impl fmt::Display for Format {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.write_str(match self {
            Self::Omni => "omni",
            Self::Go => "go",
            Self::C => "c",
            Self::Rs => "rs",
            Self::Adoc => "adoc",
            Self::Md => "md",
            Self::Json => "json",
            Self::Jsonc => "jsonc",
            Self::Yaml => "yaml",
            Self::Toml => "toml",
            Self::Txt => "txt",
            Self::Text => "text",
            Self::Binary => "binary",
        })
    }
}

// ────────────────────────────────────────────────────────────────
// Modifier — Subtype refinement
// ────────────────────────────────────────────────────────────────

/// Modifier refines the file subtype.
///
/// Go equivalent: `type Modifier string` with const block.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Default)]
pub enum Modifier {
    #[default]
    None,
    Library,
    Executable,
    Test,
    Demo,
}

impl Modifier {
    /// Parse from the `-modifier` flag in pragma line 1.
    pub fn from_str_loose(s: &str) -> Option<Self> {
        match s {
            "library" | "lib" => Some(Self::Library),
            "executable" | "exec" | "exe" => Some(Self::Executable),
            "test" => Some(Self::Test),
            "demo" => Some(Self::Demo),
            _ => None,
        }
    }
}

impl fmt::Display for Modifier {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::None => Ok(()),
            Self::Library => f.write_str("library"),
            Self::Executable => f.write_str("executable"),
            Self::Test => f.write_str("test"),
            Self::Demo => f.write_str("demo"),
        }
    }
}

// ────────────────────────────────────────────────────────────────
// Pragma — Parsed file identity
// ────────────────────────────────────────────────────────────────

/// The parsed identity of an OmniCode file.
///
/// Go equivalent: `type Pragma struct { ... }`
/// Contains both line-1 identity (type/format/modifier) and meta fields (key/from/at).
#[derive(Debug, Clone)]
pub struct Pragma {
    /// What this file IS (line 1 — type word).
    pub file_type: FileType,
    /// Target language/format (line 1 — `--format`).
    pub format: Format,
    /// Subtype refinement (line 1 — `-modifier`).
    pub modifier: Modifier,
    /// Unique identifier (`meta.key`).
    pub key: String,
    /// Derivation source (`meta.from`).
    pub from: String,
    /// Version state (`meta.at`).
    pub at: String,
    /// Raw pragma lines preserved for round-tripping.
    pub raw_lines: Vec<String>,
}

impl Pragma {
    /// Whether this file holds configuration data.
    ///
    /// Go equivalent: `func (p *Pragma) IsConfig() bool`
    pub fn is_config(&self) -> bool {
        matches!(self.file_type, FileType::Data | FileType::Folder)
            && self.format.is_config_format()
    }

    /// Whether this file is pure OmniCode (not wrapping another format).
    pub fn is_native_omni(&self) -> bool {
        self.format == Format::Omni
    }

    /// Whether this is an OmniFold folder metadata file.
    pub fn is_folder(&self) -> bool {
        self.file_type == FileType::Folder
    }

    /// The expected block pattern for this file.
    pub fn block_pattern(&self) -> &'static str {
        self.file_type.block_pattern()
    }
}

impl fmt::Display for Pragma {
    /// Human-readable representation.
    ///
    /// Go equivalent: `func (p *Pragma) String() string`
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "#!omni {}", self.file_type)?;
        if self.format != Format::Omni {
            write!(f, " --{}", self.format)?;
        }
        if self.modifier != Modifier::None {
            write!(f, " -{}", self.modifier)?;
        }
        if !self.key.is_empty() {
            write!(f, " [key={}]", self.key)?;
        }
        if !self.at.is_empty() {
            write!(f, " [at={}]", self.at)?;
        }
        Ok(())
    }
}

// ────────────────────────────────────────────────────────────────
// Extension Category
// ────────────────────────────────────────────────────────────────

/// Extension category — which family an extension belongs to.
///
/// Go equivalent: `type Category string` with const block.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Category {
    /// System and infrastructure extensions (.omni, .ofd, .omc).
    OmniSuite,
    /// Operational extensions where the name IS the operation (.gen, .rev, .witness).
    Biblical,
    /// State and awareness extensions for CPI-SI (.health, .halts, .cube).
    Cognitive,
}

impl fmt::Display for Category {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.write_str(match self {
            Self::OmniSuite => "omni-suite",
            Self::Biblical => "biblical",
            Self::Cognitive => "cognitive",
        })
    }
}

// ────────────────────────────────────────────────────────────────
// State Machine Mapping
// ────────────────────────────────────────────────────────────────

/// State machine command that an extension maps to.
///
/// Go equivalent: `type StateMapping string` with const block.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum StateMapping {
    Await,
    Proceed,
    Clear,
    Pivot,
    Halt,
}

impl fmt::Display for StateMapping {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.write_str(match self {
            Self::Await => "AWAIT",
            Self::Proceed => "PROCEED",
            Self::Clear => "CLEAR",
            Self::Pivot => "PIVOT",
            Self::Halt => "HALT",
        })
    }
}

// ────────────────────────────────────────────────────────────────
// Extension Definition
// ────────────────────────────────────────────────────────────────

/// Full identity of an OmniCode file extension.
///
/// Go equivalent: `type Extension struct { ... }` — 12 fields.
/// Mirrors the structure in `extensions.toml` — TOML is truth, code is derived.
#[derive(Debug, Clone, PartialEq)]
pub struct Extension {
    /// File extension string: ".omni", ".gen", etc.
    pub ext: String,
    /// Human name: "OmniCode Source", "Genesis", etc.
    pub name: String,
    /// Which family: omni-suite, biblical, cognitive.
    pub category: Category,
    /// Phase: "a" (foundation) or "b" (expansion).
    pub phase: String,
    /// Scripture reference.
    pub biblical_root: String,
    /// What the extension DOES: "source", "encode", "decode".
    pub operation: String,
    /// Human-readable purpose.
    pub description: String,
    /// What format the content actually uses (toml, json, omni, etc).
    pub internal_format: Format,
    /// Expected block structure: "3-block", "4-block", "varies", "none".
    pub block_pattern: String,
    /// Default pragma type for this extension.
    pub default_type: FileType,
    /// MIME type for content negotiation.
    pub mime_type: String,
    /// Which state machine command this maps to.
    pub state_map: StateMapping,
}

// ────────────────────────────────────────────────────────────────
// File Extension Constants
// ────────────────────────────────────────────────────────────────

// Omni-Suite
pub const EXT_OMNI: &str = ".omni";
pub const EXT_OMNIFOLD: &str = ".ofd";
pub const EXT_OMNI_CONFIG: &str = ".omc";
pub const EXT_OMNI_INTERFACE: &str = ".omi";
pub const EXT_OMNI_MANIFEST: &str = ".omx";

// Biblical
pub const EXT_GENESIS: &str = ".gen";
pub const EXT_REVEAL: &str = ".rev";
pub const EXT_WITNESS: &str = ".witness";
pub const EXT_COVENANT: &str = ".covenant";
pub const EXT_SCROLL: &str = ".scroll";
pub const EXT_SEED: &str = ".seed";
pub const EXT_TOV: &str = ".tov";

// Cognitive
pub const EXT_HEALTH: &str = ".health";
pub const EXT_HALTS: &str = ".halts";
pub const EXT_CUBE: &str = ".cube";

// ────────────────────────────────────────────────────────────────
// Errors
// ────────────────────────────────────────────────────────────────

/// Errors from pragma parsing.
#[derive(Debug)]
pub enum PragmaError {
    /// Could not open or read the file.
    Io(std::io::Error),
    /// No #!omni pragma found in the first N lines.
    NoPragma { max_lines: usize },
    /// A line exceeded the maximum allowed length (DoS prevention).
    LineTooLong {
        line: usize,
        length: usize,
        max: usize,
    },
}

impl fmt::Display for PragmaError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Io(e) => write!(f, "pragma: io: {e}"),
            Self::NoPragma { max_lines } => {
                write!(
                    f,
                    "pragma: no #!omni pragma found in first {max_lines} lines"
                )
            }
            Self::LineTooLong { line, length, max } => {
                write!(f, "pragma: line {line} is {length} bytes (max {max})")
            }
        }
    }
}

impl std::error::Error for PragmaError {
    fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
        match self {
            Self::Io(e) => Some(e),
            Self::NoPragma { .. } | Self::LineTooLong { .. } => None,
        }
    }
}

impl From<std::io::Error> for PragmaError {
    fn from(e: std::io::Error) -> Self {
        Self::Io(e)
    }
}

/// Errors from format dispatch.
#[derive(Debug)]
pub enum DispatchError {
    /// Pragma parsing failed.
    Pragma(PragmaError),
    /// L0 format loader returned an error.
    Config(bereshit_l0_config::ConfigError),
    /// Native OmniCode parser not yet implemented.
    NativeNotImplemented,
    /// Format not supported for dispatch.
    UnsupportedFormat(Format),
}

impl fmt::Display for DispatchError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Pragma(e) => write!(f, "dispatch: {e}"),
            Self::Config(e) => write!(f, "dispatch: {e}"),
            Self::NativeNotImplemented => {
                f.write_str("dispatch: native OmniCode parser not yet implemented")
            }
            Self::UnsupportedFormat(fmt_val) => {
                write!(f, "dispatch: unsupported config format {fmt_val:?}")
            }
        }
    }
}

impl std::error::Error for DispatchError {}

impl From<PragmaError> for DispatchError {
    fn from(e: PragmaError) -> Self {
        Self::Pragma(e)
    }
}

impl From<bereshit_l0_config::ConfigError> for DispatchError {
    fn from(e: bereshit_l0_config::ConfigError) -> Self {
        Self::Config(e)
    }
}

// ────────────────────────────────────────────────────────────────
// Tests
// ────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_file_type_display() {
        assert_eq!(FileType::Code.to_string(), "code");
        assert_eq!(FileType::Data.to_string(), "data");
        assert_eq!(FileType::Documentation.to_string(), "documentation");
    }

    #[test]
    fn test_file_type_from_str() {
        assert_eq!(FileType::from_str_loose("code"), Some(FileType::Code));
        assert_eq!(
            FileType::from_str_loose("doc"),
            Some(FileType::Documentation)
        );
        assert_eq!(FileType::from_str_loose("nope"), None);
    }

    #[test]
    fn test_file_type_block_pattern() {
        assert_eq!(FileType::Code.block_pattern(), "4-block");
        assert_eq!(FileType::Data.block_pattern(), "3-block");
        assert_eq!(FileType::Documentation.block_pattern(), "5-block");
        assert_eq!(FileType::Folder.block_pattern(), "3-block");
        assert_eq!(FileType::Template.block_pattern(), "4-block");
    }

    #[test]
    fn test_format_display_and_default() {
        assert_eq!(Format::default(), Format::Omni);
        assert_eq!(Format::Go.to_string(), "go");
        assert_eq!(Format::Toml.to_string(), "toml");
    }

    #[test]
    fn test_format_from_str() {
        assert_eq!(Format::from_str_loose("go"), Some(Format::Go));
        assert_eq!(Format::from_str_loose("rust"), Some(Format::Rs));
        assert_eq!(Format::from_str_loose("json"), Some(Format::Json));
        assert_eq!(Format::from_str_loose("jsonc"), Some(Format::Jsonc));
        assert_eq!(Format::from_str_loose("yml"), Some(Format::Yaml));
        assert_eq!(Format::from_str_loose("text"), Some(Format::Text));
        assert_eq!(Format::from_str_loose("binary"), Some(Format::Binary));
        assert_eq!(Format::from_str_loose("???"), None);
    }

    #[test]
    fn test_format_is_config() {
        assert!(Format::Toml.is_config_format());
        assert!(Format::Json.is_config_format());
        assert!(Format::Yaml.is_config_format());
        assert!(!Format::Go.is_config_format());
        assert!(!Format::Omni.is_config_format());
    }

    #[test]
    fn test_modifier_display_and_default() {
        assert_eq!(Modifier::default(), Modifier::None);
        assert_eq!(Modifier::Library.to_string(), "library");
        assert_eq!(Modifier::None.to_string(), "");
    }

    #[test]
    fn test_modifier_from_str() {
        assert_eq!(Modifier::from_str_loose("library"), Some(Modifier::Library));
        assert_eq!(Modifier::from_str_loose("lib"), Some(Modifier::Library));
        assert_eq!(Modifier::from_str_loose("exec"), Some(Modifier::Executable));
        assert_eq!(Modifier::from_str_loose("nope"), None);
    }

    #[test]
    fn test_pragma_display() {
        let p = Pragma {
            file_type: FileType::Code,
            format: Format::Go,
            modifier: Modifier::Library,
            key: "B-L1-test".into(),
            from: String::new(),
            at: "a-01.00".into(),
            raw_lines: vec![],
        };
        assert_eq!(
            p.to_string(),
            "#!omni code --go -library [key=B-L1-test] [at=a-01.00]"
        );
    }

    #[test]
    fn test_pragma_display_omni_default() {
        let p = Pragma {
            file_type: FileType::Data,
            format: Format::Omni,
            modifier: Modifier::None,
            key: String::new(),
            from: String::new(),
            at: String::new(),
            raw_lines: vec![],
        };
        // Omni format and None modifier are omitted
        assert_eq!(p.to_string(), "#!omni data");
    }

    #[test]
    fn test_pragma_is_config() {
        let data_toml = Pragma {
            file_type: FileType::Data,
            format: Format::Toml,
            modifier: Modifier::None,
            key: String::new(),
            from: String::new(),
            at: String::new(),
            raw_lines: vec![],
        };
        assert!(data_toml.is_config());

        let code_go = Pragma {
            file_type: FileType::Code,
            format: Format::Go,
            modifier: Modifier::None,
            key: String::new(),
            from: String::new(),
            at: String::new(),
            raw_lines: vec![],
        };
        assert!(!code_go.is_config());
    }

    #[test]
    fn test_category_display() {
        assert_eq!(Category::OmniSuite.to_string(), "omni-suite");
        assert_eq!(Category::Biblical.to_string(), "biblical");
        assert_eq!(Category::Cognitive.to_string(), "cognitive");
    }

    #[test]
    fn test_state_mapping_display() {
        assert_eq!(StateMapping::Await.to_string(), "AWAIT");
        assert_eq!(StateMapping::Proceed.to_string(), "PROCEED");
        assert_eq!(StateMapping::Halt.to_string(), "HALT");
    }

    #[test]
    fn test_pragma_error_display() {
        let e = PragmaError::NoPragma { max_lines: 20 };
        assert!(e.to_string().contains("20 lines"));

        let e = PragmaError::LineTooLong {
            line: 3,
            length: 200_000,
            max: 102_400,
        };
        let msg = e.to_string();
        assert!(msg.contains("line 3"), "should mention line number: {msg}");
        assert!(
            msg.contains("200000"),
            "should mention actual length: {msg}"
        );

        let e = DispatchError::NativeNotImplemented;
        assert!(e.to_string().contains("not yet implemented"));
    }
}
