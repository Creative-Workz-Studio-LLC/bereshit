// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L3-alphabet
// Purpose: Hebrew/Greek alphabet tables and transliteration for the 27-letter
//   system (22 standard + 5 final forms)
// Biblical: "In the beginning was the Word" — John 1:1
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-16
//
// The Bereshit alphabet system maps Hebrew letters to positions in the
// 3^3 cube, enabling gematria computations and state machine navigation.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

use serde::{Deserialize, Serialize};

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Types
// ────────────────────────────────────────────────────────────────

/// A single entry in the alphabet table.
/// Maps a Hebrew letter to its name, position, and gematria value.
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct AlphabetEntry {
    /// Position in the 27-letter system (0-26)
    pub position: u8,
    /// Hebrew Unicode character
    pub hebrew: char,
    /// Transliterated name (e.g., "Aleph")
    pub name: String,
    /// Standard gematria value
    pub value: u32,
    /// Whether this is a final form (sofit)
    pub is_final: bool,
}

/// The complete 27-position alphabet table
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct AlphabetTable {
    /// All 27 entries, indexed by position
    pub entries: Vec<AlphabetEntry>,
}

/// The 7 Days of Creation mapped to Hebrew states
#[derive(Clone, Debug, PartialEq, Eq, Serialize, Deserialize)]
pub enum CreationDay {
    Day1, // Light (shavar → broken)
    Day2, // Firmament (chaser → lacking)
    Day3, // Land & Vegetation (ratsah → wanting)
    Day4, // Luminaries (yashar → even)
    Day5, // Fish & Birds (tamim → sound)
    Day6, // Animals & Man (shalem → whole)
    Day7, // Rest (tov → perfect/good)
}

impl CreationDay {
    /// The Hebrew state name for this day
    pub fn hebrew_state(&self) -> &str {
        match self {
            Self::Day1 => "shavar",
            Self::Day2 => "chaser",
            Self::Day3 => "ratsah",
            Self::Day4 => "yashar",
            Self::Day5 => "tamim",
            Self::Day6 => "shalem",
            Self::Day7 => "tov",
        }
    }

    /// English meaning
    pub fn meaning(&self) -> &str {
        match self {
            Self::Day1 => "BROKEN",
            Self::Day2 => "LACKING",
            Self::Day3 => "WANTING",
            Self::Day4 => "EVEN",
            Self::Day5 => "SOUND",
            Self::Day6 => "WHOLE",
            Self::Day7 => "PERFECT",
        }
    }

    /// Day number (1-7)
    pub fn number(&self) -> u8 {
        match self {
            Self::Day1 => 1,
            Self::Day2 => 2,
            Self::Day3 => 3,
            Self::Day4 => 4,
            Self::Day5 => 5,
            Self::Day6 => 6,
            Self::Day7 => 7,
        }
    }

    /// All 7 days in order
    pub fn all() -> &'static [Self; 7] {
        &[
            Self::Day1,
            Self::Day2,
            Self::Day3,
            Self::Day4,
            Self::Day5,
            Self::Day6,
            Self::Day7,
        ]
    }

    /// Find by hebrew state name
    pub fn from_state(state: &str) -> Option<Self> {
        match state {
            "shavar" => Some(Self::Day1),
            "chaser" => Some(Self::Day2),
            "ratsah" => Some(Self::Day3),
            "yashar" => Some(Self::Day4),
            "tamim" => Some(Self::Day5),
            "shalem" => Some(Self::Day6),
            "tov" => Some(Self::Day7),
            _ => None,
        }
    }
}

impl std::fmt::Display for CreationDay {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Day {} ({} = {})", self.number(), self.hebrew_state(), self.meaning())
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 AlphabetTable Operations
// ────────────────────────────────────────────────────────────────

impl AlphabetTable {
    /// Create an empty table with 27 positions
    pub fn new() -> Self {
        Self {
            entries: Vec::with_capacity(27),
        }
    }

    /// Look up an entry by position
    pub fn by_position(&self, pos: u8) -> Option<&AlphabetEntry> {
        self.entries.iter().find(|e| e.position == pos)
    }

    /// Look up an entry by Hebrew character
    pub fn by_hebrew(&self, ch: char) -> Option<&AlphabetEntry> {
        self.entries.iter().find(|e| e.hebrew == ch)
    }

    /// Look up an entry by name (case-insensitive)
    pub fn by_name(&self, name: &str) -> Option<&AlphabetEntry> {
        let lower = name.to_lowercase();
        self.entries.iter().find(|e| e.name.to_lowercase() == lower)
    }

    /// How many entries are loaded
    pub fn len(&self) -> usize {
        self.entries.len()
    }

    /// Whether the table is empty
    pub fn is_empty(&self) -> bool {
        self.entries.is_empty()
    }

    /// Whether the table is complete (27 entries)
    pub fn is_complete(&self) -> bool {
        self.entries.len() == 27
    }
}

impl Default for AlphabetTable {
    fn default() -> Self {
        Self::new()
    }
}

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn creation_days_count() {
        assert_eq!(CreationDay::all().len(), 7);
    }

    #[test]
    fn creation_day_from_state() {
        assert_eq!(CreationDay::from_state("tov"), Some(CreationDay::Day7));
        assert_eq!(CreationDay::from_state("shavar"), Some(CreationDay::Day1));
        assert_eq!(CreationDay::from_state("invalid"), None);
    }

    #[test]
    fn creation_day_meanings() {
        assert_eq!(CreationDay::Day1.meaning(), "BROKEN");
        assert_eq!(CreationDay::Day7.meaning(), "PERFECT");
    }

    #[test]
    fn creation_day_numbers() {
        for (i, day) in CreationDay::all().iter().enumerate() {
            assert_eq!(day.number() as usize, i + 1);
        }
    }

    #[test]
    fn empty_alphabet_table() {
        let t = AlphabetTable::new();
        assert!(t.is_empty());
        assert!(!t.is_complete());
        assert_eq!(t.len(), 0);
    }

    #[test]
    fn alphabet_lookup_by_position() {
        let mut t = AlphabetTable::new();
        t.entries.push(AlphabetEntry {
            position: 0,
            hebrew: '\u{05D0}', // Aleph
            name: "Aleph".to_string(),
            value: 1,
            is_final: false,
        });
        assert!(t.by_position(0).is_some());
        assert!(t.by_position(1).is_none());
    }

    #[test]
    fn alphabet_lookup_by_name() {
        let mut t = AlphabetTable::new();
        t.entries.push(AlphabetEntry {
            position: 1,
            hebrew: '\u{05D1}', // Bet
            name: "Bet".to_string(),
            value: 2,
            is_final: false,
        });
        assert!(t.by_name("bet").is_some()); // case-insensitive
        assert!(t.by_name("BET").is_some());
    }
}
