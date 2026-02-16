//omni:code --rust -library
//omni:key B-L3-schema
//omni:version a-01.00

//! # CPI-SI Schema Types
//!
//! Schema types for loading and validating CPI-SI TOML schema files that define
//! the cube geometry, state machine, encryption tables, and task halts.
//!
//! The Go schema package loads TOML schema files; these Rust types model the same
//! structures with compile-time safety and serde deserialization.
//!
//! # Usage
//!
//! ```
//! use bereshit_l3_schema::{CubePosition, HebrewState, CommandType, SchemaCategory};
//!
//! let pos = CubePosition::new(0, "origin", 0, 0, 0).unwrap();
//! assert_eq!(pos.name(), "origin");
//!
//! let state = HebrewState::new("shavar", "broken", 0, "Initial broken state");
//! assert_eq!(state.ordinal(), 0);
//!
//! let cmd = CommandType::Halt;
//! assert_eq!(format!("{cmd}"), "Halt");
//! ```
//!
//! See [`PRAGMA`] and [`METADATA`] for OmniCode identity.

// -------- M.1 Core Identity --------
//
// Key:       B-L3-schema
//
// Scripture: "Train up a child in the way he should go: and when he is old,
//            he will not depart from it."
//            -- Proverbs 22:6
// Principle: Schema defines the rails -- structure trained in from the start
//            ensures correct behavior throughout the system's lifetime.
// Anchor:    "In the beginning God created the heaven and the earth" -- Genesis 1:1
//
// Type:      Foundation (schema definitions ground all L3 components)
//
// Architect:       Seanje Lenox-Wise
// Implementation:  Nova Dawn
// Created:         2026-02-16
// Version:         b-01.00
//
// -------- M.2 Version History --------
//
// - b-01.00 (2026-02-16) -- Initial creation: cube, state, command, schema types
//
// -------- M.3 Interface --------
//
// Requires: std (fmt, path) | external: serde, toml | internal: bereshit-l0-config
// Used by:  L3-cpisi/spiral/statemachine, L3-cpisi/spiral/compute, orchestration
// Import:   use bereshit_l3_schema::{CubeSchema, StateSchema, SchemaLoader}
// Pattern:  define schema types -> load from TOML -> validate -> use
//
// -------- M.4 Public API --------
//
// Types:    SchemaCategory, SchemaEntry, SchemaTable, CubePosition, CubeSchema,
//           HebrewState, StateSchema, CommandType, Command
// Traits:   SchemaLoader (load, validate)
// Create:   CubePosition::new, HebrewState::new, Command::new
// Query:    name(), index(), ordinal(), meaning(), description()
// Identity: pragma, pragma_get, metadata, metadata_get
//
// -------- M.5 Operational --------
//
// Blocking: no -- in-memory type definitions, file I/O only via SchemaLoader
// Health:   Foundation | granted: loaded | deferred: n/a | denied: SchemaError

// ============================================================================
// METADATA
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Pragma
// ────────────────────────────────────────────────────────────────

/// OmniCode pragma identity (P1-P5). Static, zero-cost at runtime.
pub static PRAGMA: &[(&str, &str)] = &[
    // P1: Core Identity
    ("P1.key", "B-L3-schema"),
    ("P1.type", "code"),
    ("P1.format", "rust"),
    ("P1.style", "library"),
    // P2: Structure
    ("P2.blocks", "4-block"),
    // P3: Derivation
    ("P3.from", "b-word/seed/code/L0/rust/library.rs"),
    ("P3.derives", "B-seed-code-L0-rust-library"),
    // P4: Version
    ("P4.at", "b-01.00"),
    // P5: Summary
    ("P5.title", "CPI-SI Schema Types"),
    (
        "P5.summary",
        "Schema types and loader traits for CPI-SI TOML schemas",
    ),
];

// ────────────────────────────────────────────────────────────────
// Metadata
// ────────────────────────────────────────────────────────────────

/// OmniCode metadata (M1-M10). Static, zero-cost at runtime.
pub static METADATA: &[(&str, &str)] = &[
    // -- M1: Core Identity ------------------------------------------------
    ("M1.key", "B-L3-schema"),
    ("M1.component_type", "Foundation"),
    ("M1.architect", "Seanje Lenox-Wise"),
    ("M1.implementation", "Nova Dawn"),
    ("M1.created", "2026-02-16"),
    // -- M2: Version History ----------------------------------------------
    (
        "M2.b-01.00",
        "2026-02-16 -- Initial creation: cube, state, command, schema types",
    ),
    // -- M3: Interface ----------------------------------------------------
    ("M3.requires.stdlib", "fmt, path"),
    ("M3.requires.external", "serde 1.0, toml 0.8"),
    (
        "M3.requires.internal",
        "bereshit-l0-config (config loading)",
    ),
    (
        "M3.used_by",
        "L3-cpisi/spiral/statemachine, L3-cpisi/spiral/compute, orchestration",
    ),
    ("M3.import", "bereshit-l3-schema"),
    (
        "M3.pattern",
        "define schema -> load TOML -> validate -> use typed structures",
    ),
    // -- M4: Public API ---------------------------------------------------
    (
        "M4.types",
        "SchemaCategory, SchemaEntry, SchemaTable, CubePosition, CubeSchema, HebrewState, StateSchema, CommandType, Command",
    ),
    ("M4.traits", "SchemaLoader"),
    (
        "M4.create",
        "CubePosition::new, HebrewState::new, Command::new",
    ),
    (
        "M4.query",
        "name, index, ordinal, meaning, description, positions, dimension",
    ),
    ("M4.identity", "pragma, pragma_get, metadata, metadata_get"),
    // -- M5: Operational --------------------------------------------------
    (
        "M5.blocking",
        "no -- in-memory types, file I/O only via SchemaLoader",
    ),
    (
        "M5.health",
        "Foundation | granted: loaded | deferred: n/a | denied: SchemaError",
    ),
    // -- M6: Classification -----------------------------------------------
    (
        "M6.tags",
        "schema, cube, state-machine, hebrew, commands, toml, cpi-si",
    ),
    ("M6.category", "Foundation"),
    ("M6.domain", "schema"),
    ("M6.layer", "L3-cpisi"),
    ("M6.paradigm", "CPI-SI"),
    // -- M7: Intent -------------------------------------------------------
    (
        "M7.purpose",
        "Typed schema definitions for CPI-SI cube geometry, state machine, and commands",
    ),
    (
        "M7.philosophy",
        "Schema is structure made explicit -- types enforce correctness at compile time",
    ),
    (
        "M7.provides",
        "Cube positions, hebrew states, command types, schema loading trait",
    ),
    // -- M8: Grounding ----------------------------------------------------
    (
        "M8.scripture",
        "Proverbs 22:6 -- Train up a child in the way he should go",
    ),
    (
        "M8.principle",
        "Schema defines the rails -- structure trained in from the start ensures correct behavior",
    ),
    (
        "M8.anchor",
        "Genesis 1:1 -- In the beginning God created the heaven and the earth",
    ),
    // -- M9: Dependencies -------------------------------------------------
    ("M9.needs.stdlib", "fmt (Display), path (Path)"),
    (
        "M9.needs.external",
        "serde 1.0 (derive), toml 0.8 (parsing)",
    ),
    (
        "M9.needs.internal",
        "bereshit-l0-config (config file loading)",
    ),
    (
        "M9.used_by",
        "L3-cpisi statemachine, compute, orchestration",
    ),
    (
        "M9.layer_deps",
        "L0 (config) + L3 internal -- schema is L3 foundation",
    ),
    // -- M10: Roadmap -----------------------------------------------------
    (
        "M10.current",
        "b-01.00 -- Core schema types: cube, state, command, loader trait",
    ),
    (
        "M10.planned",
        "TOML schema file loading, validation rules, encryption table schema",
    ),
    (
        "M10.limitations",
        "Loader trait defined but no default implementation yet",
    ),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

// --- Standard Library ---
use std::fmt;
use std::path::Path;

// --- External Crates ---
use serde::{Deserialize, Serialize};

// ────────────────────────────────────────────────────────────────
// Error Types
// ────────────────────────────────────────────────────────────────

/// Errors that can occur during schema operations.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum SchemaError {
    /// Schema file could not be loaded from the given path.
    LoadError(String),
    /// Schema data failed validation checks.
    ValidationError(String),
    /// A cube position coordinate is out of the valid range (-1..1).
    BoundsError(String),
    /// A hebrew state ordinal is out of the valid range (0..6).
    OrdinalError(String),
}

impl fmt::Display for SchemaError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::LoadError(msg) => write!(f, "schema load: {msg}"),
            Self::ValidationError(msg) => write!(f, "schema validation: {msg}"),
            Self::BoundsError(msg) => write!(f, "schema bounds: {msg}"),
            Self::OrdinalError(msg) => write!(f, "schema ordinal: {msg}"),
        }
    }
}

impl std::error::Error for SchemaError {}

// ────────────────────────────────────────────────────────────────
// Core Types
// ────────────────────────────────────────────────────────────────

/// Categories of CPI-SI schemas that can be loaded from TOML files.
///
/// Each category corresponds to a distinct aspect of the CPI-SI system:
/// cube geometry, initialization sequences, state transitions, task
/// definitions, encryption mappings, and halt conditions.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum SchemaCategory {
    /// 3x3x3 cube geometry and position definitions.
    Cube,
    /// Initialization sequences and bootstrap schemas.
    Init,
    /// State machine transition definitions.
    State,
    /// Task execution schemas.
    Task,
    /// Encryption table mappings.
    Encrypt,
    /// Halt condition definitions.
    Halts,
}

/// All schema categories in definition order.
pub const SCHEMA_CATEGORIES: &[SchemaCategory] = &[
    SchemaCategory::Cube,
    SchemaCategory::Init,
    SchemaCategory::State,
    SchemaCategory::Task,
    SchemaCategory::Encrypt,
    SchemaCategory::Halts,
];

impl fmt::Display for SchemaCategory {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Cube => write!(f, "Cube"),
            Self::Init => write!(f, "Init"),
            Self::State => write!(f, "State"),
            Self::Task => write!(f, "Task"),
            Self::Encrypt => write!(f, "Encrypt"),
            Self::Halts => write!(f, "Halts"),
        }
    }
}

/// A single key-value entry within a schema table.
///
/// Represents one row in a TOML schema definition -- a named value
/// with its category classification and human-readable description.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct SchemaEntry {
    /// Unique key identifying this entry within its table.
    pub key: String,
    /// The entry's value as a string representation.
    pub value: String,
    /// Which schema category this entry belongs to.
    pub category: SchemaCategory,
    /// Human-readable description of this entry's purpose.
    pub description: String,
}

/// A named collection of schema entries under a single category.
///
/// Maps to a TOML table section in schema files. Each table groups
/// related entries (e.g., all cube positions, all state definitions).
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct SchemaTable {
    /// Name of this schema table (matches TOML section header).
    pub name: String,
    /// Ordered list of entries in this table.
    pub entries: Vec<SchemaEntry>,
    /// The category all entries in this table belong to.
    pub category: SchemaCategory,
}

/// A position within the 3x3x3 CPI-SI cube.
///
/// The cube has 27 positions (indices 0-26), each with x/y/z coordinates
/// in the range -1..1. The center position (0,0,0) is the origin.
/// Coordinates map to the cube's three axes of meaning.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct CubePosition {
    /// Position index (0-26) within the 27-position cube.
    index: u8,
    /// Human-readable name for this position.
    name: String,
    /// X-axis coordinate (-1, 0, or 1).
    x: i8,
    /// Y-axis coordinate (-1, 0, or 1).
    y: i8,
    /// Z-axis coordinate (-1, 0, or 1).
    z: i8,
}

/// The complete 3x3x3 cube schema.
///
/// Holds all 27 positions and the cube's dimensionality. The CPI-SI cube
/// is always 3-dimensional (dimension = 3), yielding 3^3 = 27 positions.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct CubeSchema {
    /// All positions in the cube, ordered by index.
    positions: Vec<CubePosition>,
    /// Cube dimensionality (always 3 for CPI-SI).
    dimension: u8,
}

/// A hebrew state in the CPI-SI state machine.
///
/// The state machine has 7 states corresponding to Hebrew concepts,
/// progressing from brokenness (shavar, ordinal 0) through restoration
/// to goodness (tov, ordinal 6). Each state carries its Hebrew meaning
/// and a description of what the system does in that state.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct HebrewState {
    /// Hebrew name of the state (e.g., "shavar", "tov").
    name: String,
    /// English meaning of the Hebrew word.
    meaning: String,
    /// Ordinal position (0-6) in the state progression.
    ordinal: u8,
    /// Description of the system's behavior in this state.
    description: String,
}

/// The complete state schema with all 7 Hebrew states.
///
/// The states progress through a fixed order from shavar (broken, 0)
/// to tov (good, 6), modeling the CPI-SI restoration arc.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct StateSchema {
    /// Shavar -- broken (ordinal 0). The initial broken state.
    pub shavar: HebrewState,
    /// Tohu -- formless (ordinal 1). Without structure or purpose.
    pub tohu: HebrewState,
    /// Bohu -- void (ordinal 2). Empty, awaiting filling.
    pub bohu: HebrewState,
    /// Choshek -- darkness (ordinal 3). Obscured, not yet illuminated.
    pub choshek: HebrewState,
    /// Or -- light (ordinal 4). Illumination begins.
    pub or: HebrewState,
    /// Ratzon -- will/desire (ordinal 5). Intentional direction emerges.
    pub ratzon: HebrewState,
    /// Tov -- good (ordinal 6). The restored, functional state.
    pub tov: HebrewState,
}

/// The three command types that drive CPI-SI execution.
///
/// Commands control the state machine's flow: halt stops execution,
/// await pauses for a condition, and proceed advances forward.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum CommandType {
    /// Stop execution immediately.
    Halt,
    /// Pause and wait for a condition to be met.
    Await,
    /// Continue forward to the next state or operation.
    Proceed,
}

impl fmt::Display for CommandType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Halt => write!(f, "Halt"),
            Self::Await => write!(f, "Await"),
            Self::Proceed => write!(f, "Proceed"),
        }
    }
}

/// A concrete command instance with its type and descriptive context.
///
/// Commands are issued to the CPI-SI state machine to control transitions.
/// Each command has a type (halt/await/proceed), a human-readable name,
/// and a description explaining its purpose.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct Command {
    /// The type of command (Halt, Await, or Proceed).
    pub command_type: CommandType,
    /// Human-readable name for this command instance.
    pub name: String,
    /// Description of what this command does in context.
    pub description: String,
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Identity -- Static OmniCode Access
// ────────────────────────────────────────────────────────────────

/// Returns this crate's OmniCode pragma identity (P1-P5).
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    PRAGMA
}

/// Looks up a specific pragma key. Returns `None` if not found.
///
/// ```
/// assert_eq!(bereshit_l3_schema::pragma_get("P1.key"), Some("B-L3-schema"));
/// assert_eq!(bereshit_l3_schema::pragma_get("P99.nope"), None);
/// ```
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Returns this crate's OmniCode metadata (M1-M10).
pub fn metadata() -> &'static [(&'static str, &'static str)] {
    METADATA
}

/// Looks up a specific metadata key. Returns `None` if not found.
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

// ────────────────────────────────────────────────────────────────
// SchemaCategory
// ────────────────────────────────────────────────────────────────

impl SchemaCategory {
    /// Returns all schema categories in definition order.
    ///
    /// Useful for iteration over all categories without allocating.
    pub fn all() -> &'static [SchemaCategory] {
        SCHEMA_CATEGORIES
    }

    /// Returns the category name as a lowercase string slice.
    pub fn as_str(&self) -> &'static str {
        match self {
            Self::Cube => "cube",
            Self::Init => "init",
            Self::State => "state",
            Self::Task => "task",
            Self::Encrypt => "encrypt",
            Self::Halts => "halts",
        }
    }
}

// ────────────────────────────────────────────────────────────────
// SchemaEntry
// ────────────────────────────────────────────────────────────────

impl SchemaEntry {
    /// Creates a new schema entry.
    pub fn new(key: &str, value: &str, category: SchemaCategory, description: &str) -> Self {
        Self {
            key: key.to_owned(),
            value: value.to_owned(),
            category,
            description: description.to_owned(),
        }
    }
}

// ────────────────────────────────────────────────────────────────
// SchemaTable
// ────────────────────────────────────────────────────────────────

impl SchemaTable {
    /// Creates a new empty schema table.
    pub fn new(name: &str, category: SchemaCategory) -> Self {
        Self {
            name: name.to_owned(),
            entries: Vec::new(),
            category,
        }
    }

    /// Adds an entry to this table. Builder pattern.
    pub fn with_entry(mut self, entry: SchemaEntry) -> Self {
        self.entries.push(entry);
        self
    }

    /// Returns the number of entries in this table.
    pub fn len(&self) -> usize {
        self.entries.len()
    }

    /// Returns true if this table has no entries.
    pub fn is_empty(&self) -> bool {
        self.entries.is_empty()
    }
}

// ────────────────────────────────────────────────────────────────
// CubePosition
// ────────────────────────────────────────────────────────────────

impl CubePosition {
    /// Creates a new cube position with bounds validation.
    ///
    /// # Errors
    ///
    /// Returns [`SchemaError::BoundsError`] if:
    /// - `index` > 26
    /// - Any of `x`, `y`, `z` is outside the range -1..=1
    pub fn new(index: u8, name: &str, x: i8, y: i8, z: i8) -> Result<Self, SchemaError> {
        if index > 26 {
            return Err(SchemaError::BoundsError(format!(
                "cube index {index} exceeds maximum 26"
            )));
        }
        if !(-1..=1).contains(&x) || !(-1..=1).contains(&y) || !(-1..=1).contains(&z) {
            return Err(SchemaError::BoundsError(format!(
                "coordinates ({x},{y},{z}) out of range -1..1"
            )));
        }
        Ok(Self {
            index,
            name: name.to_owned(),
            x,
            y,
            z,
        })
    }

    /// Returns the position index (0-26).
    pub fn index(&self) -> u8 {
        self.index
    }

    /// Returns the position name.
    pub fn name(&self) -> &str {
        &self.name
    }

    /// Returns the x-axis coordinate (-1, 0, or 1).
    pub fn x(&self) -> i8 {
        self.x
    }

    /// Returns the y-axis coordinate (-1, 0, or 1).
    pub fn y(&self) -> i8 {
        self.y
    }

    /// Returns the z-axis coordinate (-1, 0, or 1).
    pub fn z(&self) -> i8 {
        self.z
    }

    /// Returns all three coordinates as a tuple (x, y, z).
    pub fn coordinates(&self) -> (i8, i8, i8) {
        (self.x, self.y, self.z)
    }
}

impl fmt::Display for CubePosition {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "{}[{}] ({},{},{})",
            self.name, self.index, self.x, self.y, self.z
        )
    }
}

// ────────────────────────────────────────────────────────────────
// CubeSchema
// ────────────────────────────────────────────────────────────────

impl CubeSchema {
    /// Creates a new cube schema with the given positions.
    ///
    /// Dimension defaults to 3 (the CPI-SI standard).
    pub fn new(positions: Vec<CubePosition>) -> Self {
        Self {
            positions,
            dimension: 3,
        }
    }

    /// Creates a cube schema with a custom dimension.
    pub fn with_dimension(positions: Vec<CubePosition>, dimension: u8) -> Self {
        Self {
            positions,
            dimension,
        }
    }

    /// Returns all positions in the cube.
    pub fn positions(&self) -> &[CubePosition] {
        &self.positions
    }

    /// Returns the cube dimensionality.
    pub fn dimension(&self) -> u8 {
        self.dimension
    }

    /// Returns the expected number of positions for this dimension (dim^3).
    pub fn expected_count(&self) -> usize {
        (self.dimension as usize).pow(3)
    }

    /// Returns the position at the given index, if it exists.
    pub fn position_at(&self, index: u8) -> Option<&CubePosition> {
        self.positions.iter().find(|p| p.index == index)
    }
}

// ────────────────────────────────────────────────────────────────
// HebrewState
// ────────────────────────────────────────────────────────────────

impl HebrewState {
    /// Creates a new hebrew state.
    ///
    /// Ordinal must be in the range 0-6 (7 states total).
    pub fn new(name: &str, meaning: &str, ordinal: u8, description: &str) -> Self {
        Self {
            name: name.to_owned(),
            meaning: meaning.to_owned(),
            ordinal,
            description: description.to_owned(),
        }
    }

    /// Returns the Hebrew name of this state.
    pub fn name(&self) -> &str {
        &self.name
    }

    /// Returns the English meaning of this state.
    pub fn meaning(&self) -> &str {
        &self.meaning
    }

    /// Returns the ordinal position (0-6).
    pub fn ordinal(&self) -> u8 {
        self.ordinal
    }

    /// Returns the description of this state.
    pub fn description(&self) -> &str {
        &self.description
    }
}

impl fmt::Display for HebrewState {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{} ({}) [{}]", self.name, self.meaning, self.ordinal)
    }
}

// ────────────────────────────────────────────────────────────────
// StateSchema
// ────────────────────────────────────────────────────────────────

impl StateSchema {
    /// Creates the default CPI-SI state schema with all 7 Hebrew states.
    ///
    /// The states progress from brokenness to goodness:
    /// shavar (0) -> tohu (1) -> bohu (2) -> choshek (3) -> or (4) -> ratzon (5) -> tov (6)
    pub fn default_states() -> Self {
        Self {
            shavar: HebrewState::new(
                "shavar",
                "broken",
                0,
                "Initial broken state -- the starting condition",
            ),
            tohu: HebrewState::new(
                "tohu",
                "formless",
                1,
                "Without structure or defined purpose",
            ),
            bohu: HebrewState::new("bohu", "void", 2, "Empty space awaiting filling"),
            choshek: HebrewState::new("choshek", "darkness", 3, "Obscured, not yet illuminated"),
            or: HebrewState::new(
                "or",
                "light",
                4,
                "Illumination begins, structure becomes visible",
            ),
            ratzon: HebrewState::new(
                "ratzon",
                "will",
                5,
                "Intentional direction and purpose emerge",
            ),
            tov: HebrewState::new("tov", "good", 6, "The restored, functional, complete state"),
        }
    }

    /// Returns all 7 states as an ordered slice.
    pub fn all_states(&self) -> [&HebrewState; 7] {
        [
            &self.shavar,
            &self.tohu,
            &self.bohu,
            &self.choshek,
            &self.or,
            &self.ratzon,
            &self.tov,
        ]
    }

    /// Looks up a state by its ordinal (0-6).
    pub fn by_ordinal(&self, ordinal: u8) -> Option<&HebrewState> {
        self.all_states().into_iter().find(|s| s.ordinal == ordinal)
    }

    /// Looks up a state by its Hebrew name.
    pub fn by_name(&self, name: &str) -> Option<&HebrewState> {
        self.all_states().into_iter().find(|s| s.name == name)
    }
}

// ────────────────────────────────────────────────────────────────
// Command
// ────────────────────────────────────────────────────────────────

impl Command {
    /// Creates a new command.
    pub fn new(command_type: CommandType, name: &str, description: &str) -> Self {
        Self {
            command_type,
            name: name.to_owned(),
            description: description.to_owned(),
        }
    }

    /// Returns the command type.
    pub fn command_type(&self) -> CommandType {
        self.command_type
    }

    /// Returns the command name.
    pub fn name(&self) -> &str {
        &self.name
    }

    /// Returns the command description.
    pub fn description(&self) -> &str {
        &self.description
    }
}

impl fmt::Display for Command {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}: {}", self.command_type, self.name)
    }
}

// ────────────────────────────────────────────────────────────────
// SchemaLoader Trait
// ────────────────────────────────────────────────────────────────

/// Trait for loading CPI-SI schemas from TOML files.
///
/// Implementors provide concrete loading logic for specific schema
/// categories or file layouts. The Go schema package's `Load()` and
/// `Validate()` functions map to this trait's methods.
pub trait SchemaLoader {
    /// The schema type this loader produces.
    type Output;

    /// Load a schema from the given file path.
    ///
    /// # Errors
    ///
    /// Returns [`SchemaError::LoadError`] if the file cannot be read or parsed.
    fn load(&self, path: &Path) -> Result<Self::Output, SchemaError>;

    /// Validate the loaded schema for internal consistency.
    ///
    /// # Errors
    ///
    /// Returns [`SchemaError::ValidationError`] if the schema is invalid.
    fn validate(&self, schema: &Self::Output) -> Result<(), SchemaError>;
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// Schema types for CPI-SI TOML schemas: cube geometry (27 positions),
// 7 Hebrew states (shavar through tov), 3 command types (halt/await/proceed),
// and a SchemaLoader trait for file-based schema loading and validation.
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   - Add new query/accessor methods on existing types
//   - Add new SchemaLoader implementations
//   - Add new SchemaEntry or SchemaTable instances
//   - Extend SchemaCategory with new variants
//
// Modify with Care:
//   - CubePosition coordinate ranges -- affects all cube consumers
//   - HebrewState ordinal mapping -- must stay 0-6
//   - CommandType variants -- breaks match exhaustiveness
//
// Never Modify:
//   - 4-block structure
//   - PRAGMA/METADATA static data format
//   - Public trait definitions (SchemaLoader)
//   - 3x3x3 cube dimensionality constant
//
// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Dependencies:  serde (serialization), toml (parsing), bereshit-l0-config (loading)
// Dependents:    L3-cpisi/spiral/statemachine, L3-cpisi/spiral/compute
// Supersedes:    Go schema package (L3-cpisi/foundation/schema)
// Parallel:      Go schema types in L3-cpisi/foundation/database/$domain
//
// ────────────────────────────────────────────────────────────────
// Validation
// ────────────────────────────────────────────────────────────────
//
// Build:    cargo build -p bereshit-l3-schema
// Test:     cargo test -p bereshit-l3-schema
// Clippy:   cargo clippy -p bereshit-l3-schema -- -D warnings
// Doc:      cargo doc -p bereshit-l3-schema --no-deps
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// "Train up a child in the way he should go: and when he is old, he will
// not depart from it." -- Proverbs 22:6 KJV
//
// ============================================================================
// END CLOSING
// ============================================================================

// ============================================================================
// Tests
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    // -- CubePosition ----------------------------------------------------

    #[test]
    fn cube_position_creation_and_accessors() {
        let pos = CubePosition::new(0, "origin", 0, 0, 0).unwrap();
        assert_eq!(pos.index(), 0);
        assert_eq!(pos.name(), "origin");
        assert_eq!(pos.x(), 0);
        assert_eq!(pos.y(), 0);
        assert_eq!(pos.z(), 0);
        assert_eq!(pos.coordinates(), (0, 0, 0));
    }

    #[test]
    fn cube_position_valid_extremes() {
        // All corners of the cube should be valid
        let corner = CubePosition::new(26, "far-corner", 1, 1, 1).unwrap();
        assert_eq!(corner.index(), 26);
        assert_eq!(corner.coordinates(), (1, 1, 1));

        let neg_corner = CubePosition::new(0, "neg-corner", -1, -1, -1).unwrap();
        assert_eq!(neg_corner.coordinates(), (-1, -1, -1));
    }

    #[test]
    fn cube_position_bounds_index_too_high() {
        let result = CubePosition::new(27, "invalid", 0, 0, 0);
        assert!(result.is_err());
        assert!(matches!(result.unwrap_err(), SchemaError::BoundsError(_)));
    }

    #[test]
    fn cube_position_bounds_coordinate_out_of_range() {
        let result = CubePosition::new(0, "bad-x", 2, 0, 0);
        assert!(result.is_err());
        assert!(matches!(result.unwrap_err(), SchemaError::BoundsError(_)));

        let result = CubePosition::new(0, "bad-y", 0, -2, 0);
        assert!(result.is_err());

        let result = CubePosition::new(0, "bad-z", 0, 0, 3);
        assert!(result.is_err());
    }

    #[test]
    fn cube_position_display() {
        let pos = CubePosition::new(13, "center", 0, 0, 0).unwrap();
        assert_eq!(format!("{pos}"), "center[13] (0,0,0)");
    }

    // -- CubeSchema ------------------------------------------------------

    #[test]
    fn cube_schema_dimension_default() {
        let schema = CubeSchema::new(Vec::new());
        assert_eq!(schema.dimension(), 3);
        assert_eq!(schema.expected_count(), 27);
    }

    #[test]
    fn cube_schema_custom_dimension() {
        let schema = CubeSchema::with_dimension(Vec::new(), 2);
        assert_eq!(schema.dimension(), 2);
        assert_eq!(schema.expected_count(), 8);
    }

    #[test]
    fn cube_schema_position_lookup() {
        let positions = vec![
            CubePosition::new(0, "origin", 0, 0, 0).unwrap(),
            CubePosition::new(1, "right", 1, 0, 0).unwrap(),
            CubePosition::new(2, "left", -1, 0, 0).unwrap(),
        ];
        let schema = CubeSchema::new(positions);

        assert_eq!(schema.positions().len(), 3);
        assert_eq!(schema.position_at(1).unwrap().name(), "right");
        assert!(schema.position_at(99).is_none());
    }

    // -- HebrewState -----------------------------------------------------

    #[test]
    fn hebrew_state_ordinal_mapping() {
        let schema = StateSchema::default_states();

        // Verify all 7 ordinals map correctly
        assert_eq!(schema.shavar.ordinal(), 0);
        assert_eq!(schema.tohu.ordinal(), 1);
        assert_eq!(schema.bohu.ordinal(), 2);
        assert_eq!(schema.choshek.ordinal(), 3);
        assert_eq!(schema.or.ordinal(), 4);
        assert_eq!(schema.ratzon.ordinal(), 5);
        assert_eq!(schema.tov.ordinal(), 6);

        // Verify lookup by ordinal
        assert_eq!(schema.by_ordinal(0).unwrap().name(), "shavar");
        assert_eq!(schema.by_ordinal(6).unwrap().name(), "tov");
        assert!(schema.by_ordinal(7).is_none());
    }

    #[test]
    fn hebrew_state_name_and_meaning() {
        let state = HebrewState::new("shavar", "broken", 0, "Initial state");
        assert_eq!(state.name(), "shavar");
        assert_eq!(state.meaning(), "broken");
        assert_eq!(state.description(), "Initial state");
    }

    #[test]
    fn hebrew_state_display() {
        let state = HebrewState::new("tov", "good", 6, "Complete");
        assert_eq!(format!("{state}"), "tov (good) [6]");
    }

    #[test]
    fn state_schema_lookup_by_name() {
        let schema = StateSchema::default_states();
        assert_eq!(schema.by_name("or").unwrap().meaning(), "light");
        assert_eq!(schema.by_name("ratzon").unwrap().ordinal(), 5);
        assert!(schema.by_name("nonexistent").is_none());
    }

    #[test]
    fn state_schema_all_states_count() {
        let schema = StateSchema::default_states();
        assert_eq!(schema.all_states().len(), 7);
    }

    // -- CommandType -----------------------------------------------------

    #[test]
    fn command_type_display() {
        assert_eq!(format!("{}", CommandType::Halt), "Halt");
        assert_eq!(format!("{}", CommandType::Await), "Await");
        assert_eq!(format!("{}", CommandType::Proceed), "Proceed");
    }

    #[test]
    fn command_type_equality() {
        assert_eq!(CommandType::Halt, CommandType::Halt);
        assert_ne!(CommandType::Halt, CommandType::Await);
        assert_ne!(CommandType::Await, CommandType::Proceed);
    }

    #[test]
    fn command_creation_and_accessors() {
        let cmd = Command::new(CommandType::Halt, "emergency-stop", "Stop all processing");
        assert_eq!(cmd.command_type(), CommandType::Halt);
        assert_eq!(cmd.name(), "emergency-stop");
        assert_eq!(cmd.description(), "Stop all processing");
    }

    #[test]
    fn command_display() {
        let cmd = Command::new(CommandType::Proceed, "advance", "Move to next state");
        assert_eq!(format!("{cmd}"), "Proceed: advance");
    }

    // -- SchemaCategory --------------------------------------------------

    #[test]
    fn schema_category_iteration() {
        let categories = SchemaCategory::all();
        assert_eq!(categories.len(), 6);
        assert_eq!(categories[0], SchemaCategory::Cube);
        assert_eq!(categories[1], SchemaCategory::Init);
        assert_eq!(categories[2], SchemaCategory::State);
        assert_eq!(categories[3], SchemaCategory::Task);
        assert_eq!(categories[4], SchemaCategory::Encrypt);
        assert_eq!(categories[5], SchemaCategory::Halts);
    }

    #[test]
    fn schema_category_display() {
        assert_eq!(format!("{}", SchemaCategory::Cube), "Cube");
        assert_eq!(format!("{}", SchemaCategory::Encrypt), "Encrypt");
        assert_eq!(format!("{}", SchemaCategory::Halts), "Halts");
    }

    #[test]
    fn schema_category_as_str() {
        assert_eq!(SchemaCategory::Cube.as_str(), "cube");
        assert_eq!(SchemaCategory::State.as_str(), "state");
        assert_eq!(SchemaCategory::Halts.as_str(), "halts");
    }

    // -- SchemaEntry / SchemaTable ---------------------------------------

    #[test]
    fn schema_table_builder() {
        let table = SchemaTable::new("positions", SchemaCategory::Cube)
            .with_entry(SchemaEntry::new(
                "origin",
                "0,0,0",
                SchemaCategory::Cube,
                "Center of the cube",
            ))
            .with_entry(SchemaEntry::new(
                "corner-a",
                "1,1,1",
                SchemaCategory::Cube,
                "Far corner",
            ));

        assert_eq!(table.name, "positions");
        assert_eq!(table.len(), 2);
        assert!(!table.is_empty());
        assert_eq!(table.category, SchemaCategory::Cube);
    }

    #[test]
    fn schema_table_empty() {
        let table = SchemaTable::new("empty", SchemaCategory::Init);
        assert!(table.is_empty());
        assert_eq!(table.len(), 0);
    }

    // -- SchemaError -----------------------------------------------------

    #[test]
    fn schema_error_display() {
        let err = SchemaError::LoadError("file not found".to_string());
        assert_eq!(err.to_string(), "schema load: file not found");

        let err = SchemaError::ValidationError("missing field".to_string());
        assert_eq!(err.to_string(), "schema validation: missing field");

        let err = SchemaError::BoundsError("index 30".to_string());
        assert_eq!(err.to_string(), "schema bounds: index 30");

        let err = SchemaError::OrdinalError("ordinal 8".to_string());
        assert_eq!(err.to_string(), "schema ordinal: ordinal 8");
    }

    // -- Identity --------------------------------------------------------

    #[test]
    fn pragma_contains_key() {
        assert_eq!(pragma_get("P1.key"), Some("B-L3-schema"));
    }

    #[test]
    fn pragma_contains_format() {
        assert_eq!(pragma_get("P1.format"), Some("rust"));
    }

    #[test]
    fn pragma_get_returns_none_for_missing() {
        assert_eq!(pragma_get("P99.nonexistent"), None);
    }

    #[test]
    fn metadata_contains_grounding() {
        let scripture = metadata_get("M8.scripture").unwrap();
        assert!(scripture.contains("Proverbs 22:6"));
    }

    #[test]
    fn metadata_contains_layer() {
        assert_eq!(metadata_get("M6.layer"), Some("L3-cpisi"));
    }
}
