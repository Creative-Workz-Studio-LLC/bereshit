//omni:code --rust -library
//omni:key B-L3-statemachine
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L3-statemachine
// Purpose: 27-position state machine — commands, operators, transitions
// Biblical: "There is a way which seemeth right unto a man, but the end
//   thereof are the ways of death" — Proverbs 14:12
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-16
//
// The state machine navigates the 3^3 cube using:
//   3 commands: HALT, AWAIT, PROCEED
//   9 operators: 3 per command (one per key direction)
//   7 Hebrew states: Days of Creation progression
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
// B.1 Commands
// ────────────────────────────────────────────────────────────────

/// The 3 fundamental commands
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum Command {
    /// HALT: Stop, evaluate, checkpoint
    Halt,
    /// AWAIT: Hold position, observe, gather
    Await,
    /// PROCEED: Move forward, execute, advance
    Proceed,
}

impl Command {
    pub fn as_str(&self) -> &str {
        match self {
            Self::Halt => "HALT",
            Self::Await => "AWAIT",
            Self::Proceed => "PROCEED",
        }
    }

    pub fn all() -> &'static [Self; 3] {
        &[Self::Halt, Self::Await, Self::Proceed]
    }
}

impl std::fmt::Display for Command {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.as_str())
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 Operators
// ────────────────────────────────────────────────────────────────

/// The 9 operators (3 per command x 3 key directions)
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum Operator {
    // HALT operators
    HaltRetreat, // <- (finality)
    HaltHold,    // @ (lateral)
    HaltAdvance, // -> (expansion)

    // AWAIT operators
    AwaitContract, // - (finality)
    AwaitBalance,  // = (lateral)
    AwaitExpand,   // + (expansion)

    // PROCEED operators
    ProceedDown,  // < (finality)
    ProceedLevel, // | (lateral)
    ProceedUp,    // > (expansion)
}

impl Operator {
    /// The command this operator belongs to
    pub fn command(&self) -> Command {
        match self {
            Self::HaltRetreat | Self::HaltHold | Self::HaltAdvance => Command::Halt,
            Self::AwaitContract | Self::AwaitBalance | Self::AwaitExpand => Command::Await,
            Self::ProceedDown | Self::ProceedLevel | Self::ProceedUp => Command::Proceed,
        }
    }

    /// Symbol for display
    pub fn symbol(&self) -> &str {
        match self {
            Self::HaltRetreat => "<-",
            Self::HaltHold => "@",
            Self::HaltAdvance => "->",
            Self::AwaitContract => "-",
            Self::AwaitBalance => "=",
            Self::AwaitExpand => "+",
            Self::ProceedDown => "<",
            Self::ProceedLevel => "|",
            Self::ProceedUp => ">",
        }
    }

    /// All 9 operators
    pub fn all() -> &'static [Self; 9] {
        &[
            Self::HaltRetreat,
            Self::HaltHold,
            Self::HaltAdvance,
            Self::AwaitContract,
            Self::AwaitBalance,
            Self::AwaitExpand,
            Self::ProceedDown,
            Self::ProceedLevel,
            Self::ProceedUp,
        ]
    }
}

// ────────────────────────────────────────────────────────────────
// B.3 State Transition
// ────────────────────────────────────────────────────────────────

/// A state transition record
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct Transition {
    /// Position index before transition
    pub from_position: u8,
    /// Position index after transition
    pub to_position: u8,
    /// Command that triggered the transition
    pub command: Command,
    /// Operator applied
    pub operator: Operator,
    /// Hebrew state before
    pub from_state: String,
    /// Hebrew state after
    pub to_state: String,
    /// Timestamp (ISO 8601)
    pub timestamp: String,
}

/// A trajectory: sequence of transitions through the cube
#[derive(Clone, Debug, Default, Serialize, Deserialize)]
pub struct Trajectory {
    /// Ordered transitions
    pub transitions: Vec<Transition>,
    /// Current position index
    pub current_position: u8,
    /// Total path length
    pub path_length: u32,
}

impl Trajectory {
    /// Create a new trajectory starting at a position
    pub fn new(start_position: u8) -> Self {
        Self {
            transitions: Vec::new(),
            current_position: start_position,
            path_length: 0,
        }
    }

    /// Record a transition
    pub fn record(&mut self, transition: Transition) {
        self.current_position = transition.to_position;
        self.path_length += 1;
        self.transitions.push(transition);
    }

    /// Get the last transition
    pub fn last(&self) -> Option<&Transition> {
        self.transitions.last()
    }

    /// Whether we've visited a position
    pub fn has_visited(&self, position: u8) -> bool {
        self.transitions.iter().any(|t| t.to_position == position)
    }

    /// Unique positions visited
    pub fn unique_positions(&self) -> Vec<u8> {
        let mut positions: Vec<u8> = self.transitions.iter().map(|t| t.to_position).collect();
        positions.sort();
        positions.dedup();
        positions
    }
}

// ────────────────────────────────────────────────────────────────
// B.4 Pattern Detection
// ────────────────────────────────────────────────────────────────

/// A detected pattern in the trajectory
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct Pattern {
    /// Pattern type name
    pub name: String,
    /// Positions involved
    pub positions: Vec<u8>,
    /// Confidence (0.0-1.0)
    pub confidence: f64,
    /// Description
    pub description: String,
}

/// Detect if the trajectory has a cycle (revisiting positions)
pub fn detect_cycle(trajectory: &Trajectory) -> Option<Pattern> {
    let positions = &trajectory.transitions;
    if positions.len() < 3 {
        return None;
    }

    // Simple cycle detection: if current position matches an earlier position
    for (i, t) in positions.iter().enumerate() {
        for t2 in positions.iter().skip(i + 2) {
            if t.to_position == t2.to_position {
                return Some(Pattern {
                    name: "cycle".to_string(),
                    positions: vec![t.to_position],
                    confidence: 0.8,
                    description: format!("Revisited position {}", t.to_position),
                });
            }
        }
    }
    None
}

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn command_count() {
        assert_eq!(Command::all().len(), 3);
    }

    #[test]
    fn operator_count() {
        assert_eq!(Operator::all().len(), 9);
    }

    #[test]
    fn operator_command_mapping() {
        assert_eq!(Operator::HaltRetreat.command(), Command::Halt);
        assert_eq!(Operator::AwaitBalance.command(), Command::Await);
        assert_eq!(Operator::ProceedUp.command(), Command::Proceed);
    }

    #[test]
    fn operators_per_command() {
        for cmd in Command::all() {
            let count = Operator::all()
                .iter()
                .filter(|op| op.command() == *cmd)
                .count();
            assert_eq!(count, 3, "{cmd} should have 3 operators");
        }
    }

    #[test]
    fn trajectory_new() {
        let t = Trajectory::new(13); // Center
        assert_eq!(t.current_position, 13);
        assert_eq!(t.path_length, 0);
        assert!(t.transitions.is_empty());
    }

    #[test]
    fn trajectory_record() {
        let mut t = Trajectory::new(13);
        t.record(Transition {
            from_position: 13,
            to_position: 14,
            command: Command::Proceed,
            operator: Operator::ProceedUp,
            from_state: "yashar".to_string(),
            to_state: "tamim".to_string(),
            timestamp: String::new(),
        });
        assert_eq!(t.current_position, 14);
        assert_eq!(t.path_length, 1);
    }

    #[test]
    fn trajectory_unique_positions() {
        let mut t = Trajectory::new(0);
        for pos in [1, 2, 1, 3] {
            t.record(Transition {
                from_position: t.current_position,
                to_position: pos,
                command: Command::Proceed,
                operator: Operator::ProceedLevel,
                from_state: String::new(),
                to_state: String::new(),
                timestamp: String::new(),
            });
        }
        let unique = t.unique_positions();
        assert_eq!(unique, vec![1, 2, 3]);
    }

    #[test]
    fn cycle_detection_no_cycle() {
        let t = Trajectory::new(0);
        assert!(detect_cycle(&t).is_none());
    }

    #[test]
    fn cycle_detection_with_cycle() {
        let mut t = Trajectory::new(0);
        for pos in [1, 2, 1] {
            t.record(Transition {
                from_position: t.current_position,
                to_position: pos,
                command: Command::Await,
                operator: Operator::AwaitBalance,
                from_state: String::new(),
                to_state: String::new(),
                timestamp: String::new(),
            });
        }
        assert!(detect_cycle(&t).is_some());
    }
}
