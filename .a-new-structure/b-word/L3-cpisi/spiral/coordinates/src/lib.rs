// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L3-coordinates
// Purpose: 27-position cube geometry — positions, perspectives, reachability
// Biblical: "And God said, Let there be a firmament in the midst of the
//   waters, and let it divide the waters from the waters" — Genesis 1:6
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-16
//
// The 3^3 cube has 27 positions organized by 9 named positions across
// 3 Z-layers. Perspective transforms map how one position "sees" another.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

use serde::{Deserialize, Serialize};

// ────────────────────────────────────────────────────────────────
// S.1 Constants
// ────────────────────────────────────────────────────────────────

/// Cube dimension (3x3x3)
pub const DIMENSION: u8 = 3;

/// Total positions in the cube
pub const TOTAL_POSITIONS: u8 = 27;

/// Named position count per Z-layer
pub const POSITIONS_PER_LAYER: u8 = 9;

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Position Types
// ────────────────────────────────────────────────────────────────

/// The 9 named positions in each Z-layer of the cube.
/// Names derive from their spatial relationship: center, edges, corners.
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum NamedPosition {
    /// Center (0,0): The axis point
    Center,
    /// North (0,1): Upper edge
    North,
    /// South (0,-1): Lower edge
    South,
    /// East (1,0): Right edge
    East,
    /// West (-1,0): Left edge
    West,
    /// NorthEast (1,1): Upper-right corner
    NorthEast,
    /// NorthWest (-1,1): Upper-left corner
    NorthWest,
    /// SouthEast (1,-1): Lower-right corner
    SouthEast,
    /// SouthWest (-1,-1): Lower-left corner
    SouthWest,
}

impl NamedPosition {
    /// Get the (x, y) offset for this named position
    pub fn offset(&self) -> (i8, i8) {
        match self {
            Self::Center => (0, 0),
            Self::North => (0, 1),
            Self::South => (0, -1),
            Self::East => (1, 0),
            Self::West => (-1, 0),
            Self::NorthEast => (1, 1),
            Self::NorthWest => (-1, 1),
            Self::SouthEast => (1, -1),
            Self::SouthWest => (-1, -1),
        }
    }

    /// Get all 9 named positions
    pub fn all() -> &'static [Self; 9] {
        &[
            Self::Center,
            Self::North,
            Self::South,
            Self::East,
            Self::West,
            Self::NorthEast,
            Self::NorthWest,
            Self::SouthEast,
            Self::SouthWest,
        ]
    }
}

impl std::fmt::Display for NamedPosition {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let name = match self {
            Self::Center => "Center",
            Self::North => "North",
            Self::South => "South",
            Self::East => "East",
            Self::West => "West",
            Self::NorthEast => "NorthEast",
            Self::NorthWest => "NorthWest",
            Self::SouthEast => "SouthEast",
            Self::SouthWest => "SouthWest",
        };
        write!(f, "{name}")
    }
}

/// Z-layer in the cube
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum ZLayer {
    /// z = -1: Foundation layer
    Foundation,
    /// z = 0: Middle ground
    Middle,
    /// z = +1: Transcendent layer
    Transcendent,
}

impl ZLayer {
    /// Get the z coordinate
    pub fn z(&self) -> i8 {
        match self {
            Self::Foundation => -1,
            Self::Middle => 0,
            Self::Transcendent => 1,
        }
    }

    /// Determine layer from z coordinate
    pub fn from_z(z: i8) -> Self {
        match z {
            z if z < 0 => Self::Foundation,
            0 => Self::Middle,
            _ => Self::Transcendent,
        }
    }
}

/// A full 3D position in the cube
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub struct CubePosition {
    /// Linear index (0-26)
    pub index: u8,
    /// X coordinate (-1 to 1)
    pub x: i8,
    /// Y coordinate (-1 to 1)
    pub y: i8,
    /// Z coordinate (-1 to 1)
    pub z: i8,
}

impl CubePosition {
    /// Create from a linear index
    pub fn from_index(index: u8) -> Self {
        let i = index.min(26) as i8;
        Self {
            index: index.min(26),
            x: (i % 3) - 1,
            y: ((i / 3) % 3) - 1,
            z: (i / 9) - 1,
        }
    }

    /// Create from 3D coordinates
    pub fn from_xyz(x: i8, y: i8, z: i8) -> Self {
        let cx = (x + 1).clamp(0, 2) as u8;
        let cy = (y + 1).clamp(0, 2) as u8;
        let cz = (z + 1).clamp(0, 2) as u8;
        let index = cz * 9 + cy * 3 + cx;
        Self { index, x, y, z }
    }

    /// The Z-layer this position belongs to
    pub fn layer(&self) -> ZLayer {
        ZLayer::from_z(self.z)
    }

    /// Named position within the layer (ignoring Z)
    pub fn named(&self) -> NamedPosition {
        match (self.x, self.y) {
            (0, 0) => NamedPosition::Center,
            (0, 1) => NamedPosition::North,
            (0, -1) => NamedPosition::South,
            (1, 0) => NamedPosition::East,
            (-1, 0) => NamedPosition::West,
            (1, 1) => NamedPosition::NorthEast,
            (-1, 1) => NamedPosition::NorthWest,
            (1, -1) => NamedPosition::SouthEast,
            (-1, -1) => NamedPosition::SouthWest,
            _ => NamedPosition::Center,
        }
    }

    /// Manhattan distance to another position
    pub fn distance_to(&self, other: &Self) -> u8 {
        (self.x - other.x).unsigned_abs()
            + (self.y - other.y).unsigned_abs()
            + (self.z - other.z).unsigned_abs()
    }

    /// Whether this position is adjacent (Manhattan distance = 1)
    pub fn is_adjacent(&self, other: &Self) -> bool {
        self.distance_to(other) == 1
    }

    /// The center of the cube
    pub fn center() -> Self {
        Self::from_index(13)
    }
}

impl std::fmt::Display for CubePosition {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "({},{},{})@{}", self.x, self.y, self.z, self.index)
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 Cube Operations
// ────────────────────────────────────────────────────────────────

/// Get all 27 positions in the cube
pub fn all_positions() -> Vec<CubePosition> {
    (0..27).map(CubePosition::from_index).collect()
}

/// Get positions in a specific Z-layer
pub fn positions_in_layer(layer: ZLayer) -> Vec<CubePosition> {
    all_positions()
        .into_iter()
        .filter(|p| p.layer() == layer)
        .collect()
}

/// Get neighbors of a position (adjacent by Manhattan distance 1)
pub fn neighbors(pos: &CubePosition) -> Vec<CubePosition> {
    all_positions()
        .into_iter()
        .filter(|p| p.is_adjacent(pos))
        .collect()
}

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn total_positions_is_27() {
        assert_eq!(all_positions().len(), 27);
    }

    #[test]
    fn center_is_origin() {
        let c = CubePosition::center();
        assert_eq!((c.x, c.y, c.z), (0, 0, 0));
        assert_eq!(c.index, 13);
    }

    #[test]
    fn index_roundtrip() {
        for i in 0..27u8 {
            let pos = CubePosition::from_index(i);
            let back = CubePosition::from_xyz(pos.x, pos.y, pos.z);
            assert_eq!(back.index, i, "roundtrip failed for {i}");
        }
    }

    #[test]
    fn layer_assignment() {
        assert_eq!(CubePosition::from_index(0).layer(), ZLayer::Foundation);
        assert_eq!(CubePosition::from_index(13).layer(), ZLayer::Middle);
        assert_eq!(CubePosition::from_index(26).layer(), ZLayer::Transcendent);
    }

    #[test]
    fn positions_per_layer_is_9() {
        assert_eq!(positions_in_layer(ZLayer::Foundation).len(), 9);
        assert_eq!(positions_in_layer(ZLayer::Middle).len(), 9);
        assert_eq!(positions_in_layer(ZLayer::Transcendent).len(), 9);
    }

    #[test]
    fn center_has_6_neighbors() {
        let c = CubePosition::center();
        let n = neighbors(&c);
        // Center (0,0,0) is adjacent to 6 face-neighbors
        assert_eq!(n.len(), 6);
    }

    #[test]
    fn corner_has_3_neighbors() {
        let corner = CubePosition::from_index(0); // (-1,-1,-1)
        let n = neighbors(&corner);
        assert_eq!(n.len(), 3);
    }

    #[test]
    fn named_position_mapping() {
        let center = CubePosition::from_xyz(0, 0, 0);
        assert_eq!(center.named(), NamedPosition::Center);

        let ne = CubePosition::from_xyz(1, 1, 0);
        assert_eq!(ne.named(), NamedPosition::NorthEast);
    }

    #[test]
    fn distance_same_point() {
        let a = CubePosition::center();
        assert_eq!(a.distance_to(&a), 0);
    }

    #[test]
    fn distance_opposite_corners() {
        let a = CubePosition::from_index(0);
        let b = CubePosition::from_index(26);
        assert_eq!(a.distance_to(&b), 6);
    }
}
