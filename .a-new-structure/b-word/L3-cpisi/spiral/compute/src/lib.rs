//omni:code --rust -library
//omni:key B-L3-compute
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L3-compute
// Purpose: Numerical computations over the 3^3 cube — gematria and transforms
// Biblical: "The heavens declare the glory of God; and the firmament sheweth
//   his handywork" — Psalm 19:1
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-16
//
// Gematria: Hebrew letter-number correspondences used in biblical analysis.
// The 27-position cube (3^3) maps letters to positions in 3D space.
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
// B.1 Gematria Types
// ────────────────────────────────────────────────────────────────

/// A gematria value mapping a Hebrew letter to its numerical value
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct GematriaEntry {
    /// Hebrew letter (Unicode)
    pub letter: char,
    /// Transliterated name (e.g., "Aleph", "Bet")
    pub name: String,
    /// Standard gematria value (1-900)
    pub value: u32,
    /// Position in the 27-letter+final system (0-26)
    pub position: u8,
}

/// Result of a gematria calculation on a word or phrase
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct GematriaResult {
    /// The input text
    pub text: String,
    /// Total gematria value
    pub total: u64,
    /// Per-letter breakdown
    pub letters: Vec<GematriaEntry>,
    /// Digital root (recursive digit sum)
    pub digital_root: u32,
}

// ────────────────────────────────────────────────────────────────
// B.2 Transform Functions
// ────────────────────────────────────────────────────────────────

/// Calculate the digital root of a number (recursive digit sum until single digit)
/// This is fundamental to gematria reduction.
pub fn digital_root(mut n: u64) -> u32 {
    if n == 0 {
        return 0;
    }
    loop {
        let mut sum = 0u64;
        while n > 0 {
            sum += n % 10;
            n /= 10;
        }
        if sum < 10 {
            return sum as u32;
        }
        n = sum;
    }
}

/// Map a linear index (0-26) to 3D cube coordinates (x, y, z)
/// Each axis ranges from -1 to +1, giving 3^3 = 27 positions.
pub fn index_to_cube(index: u8) -> (i8, i8, i8) {
    let i = index.min(26) as i8;
    let x = (i % 3) - 1;
    let y = ((i / 3) % 3) - 1;
    let z = (i / 9) - 1;
    (x, y, z)
}

/// Map 3D cube coordinates back to a linear index (0-26)
pub fn cube_to_index(x: i8, y: i8, z: i8) -> u8 {
    let x = (x + 1).clamp(0, 2) as u8;
    let y = (y + 1).clamp(0, 2) as u8;
    let z = (z + 1).clamp(0, 2) as u8;
    z * 9 + y * 3 + x
}

/// Calculate Manhattan distance between two cube positions
pub fn manhattan_distance(a: (i8, i8, i8), b: (i8, i8, i8)) -> u8 {
    (a.0 - b.0).unsigned_abs() + (a.1 - b.1).unsigned_abs() + (a.2 - b.2).unsigned_abs()
}

/// Calculate the Z-depth value for trajectory analysis.
/// Z-depth represents vertical position in the cube: -1 (foundation),
/// 0 (middle ground), +1 (transcendent).
pub fn z_depth(index: u8) -> i8 {
    let (_, _, z) = index_to_cube(index);
    z
}

/// Normalize a value to the 0.0-1.0 range given min/max bounds
pub fn normalize(value: f64, min: f64, max: f64) -> f64 {
    if (max - min).abs() < f64::EPSILON {
        return 0.5;
    }
    ((value - min) / (max - min)).clamp(0.0, 1.0)
}

/// Scale a normalized (0.0-1.0) value to an integer range
pub fn scale(normalized: f64, min: i32, max: i32) -> i32 {
    let range = (max - min) as f64;
    min + (normalized * range) as i32
}

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn digital_root_single_digit() {
        assert_eq!(digital_root(5), 5);
        assert_eq!(digital_root(0), 0);
    }

    #[test]
    fn digital_root_multi_digit() {
        assert_eq!(digital_root(123), 6); // 1+2+3 = 6
        assert_eq!(digital_root(999), 9); // 9+9+9 = 27 → 2+7 = 9
        assert_eq!(digital_root(100), 1);
    }

    #[test]
    fn index_cube_roundtrip() {
        for i in 0..27u8 {
            let (x, y, z) = index_to_cube(i);
            assert_eq!(cube_to_index(x, y, z), i, "roundtrip failed for index {i}");
        }
    }

    #[test]
    fn cube_center_is_origin() {
        // Index 13 should be the center (0, 0, 0)
        let (x, y, z) = index_to_cube(13);
        assert_eq!((x, y, z), (0, 0, 0));
    }

    #[test]
    fn cube_corners() {
        assert_eq!(index_to_cube(0), (-1, -1, -1));
        assert_eq!(index_to_cube(26), (1, 1, 1));
    }

    #[test]
    fn manhattan_distance_same_point() {
        assert_eq!(manhattan_distance((0, 0, 0), (0, 0, 0)), 0);
    }

    #[test]
    fn manhattan_distance_corners() {
        // Opposite corners: (-1,-1,-1) to (1,1,1) = 2+2+2 = 6
        assert_eq!(manhattan_distance((-1, -1, -1), (1, 1, 1)), 6);
    }

    #[test]
    fn z_depth_layers() {
        // Indices 0-8 are z=-1 (foundation)
        assert_eq!(z_depth(0), -1);
        assert_eq!(z_depth(8), -1);
        // Indices 9-17 are z=0 (middle)
        assert_eq!(z_depth(9), 0);
        assert_eq!(z_depth(17), 0);
        // Indices 18-26 are z=+1 (transcendent)
        assert_eq!(z_depth(18), 1);
        assert_eq!(z_depth(26), 1);
    }

    #[test]
    fn normalize_range() {
        assert_eq!(normalize(50.0, 0.0, 100.0), 0.5);
        assert_eq!(normalize(0.0, 0.0, 100.0), 0.0);
        assert_eq!(normalize(100.0, 0.0, 100.0), 1.0);
    }

    #[test]
    fn scale_to_integer() {
        assert_eq!(scale(0.5, 0, 200), 100);
        assert_eq!(scale(0.0, -100, 100), -100);
        assert_eq!(scale(1.0, -100, 100), 100);
    }
}
