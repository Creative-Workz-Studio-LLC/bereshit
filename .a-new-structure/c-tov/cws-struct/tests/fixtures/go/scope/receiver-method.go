// #!omni code --go -module->utility
//omni:code --go -module->utility
//omni:key B-test-fixture-scope-receiver
//omni:version a-01.00

// ============================================================================
// METADATA BLOCK [METADATA]
// ============================================================================
//
// Key:     B-test-fixture-scope-receiver
// Purpose: Fixture for R[3] self_access detection testing (Go receiver methods).
//

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

// Pragma — Go uses comment-based identity blocks.
//
// I1.key:       B-test-fixture-scope-receiver
// I1.format:    go
// I1.from:      tests/fixtures/go/scope/receiver-method.go
// I1.at:        a-01.00
// I2.type:      code
// I2.structure: 4-block
// I2.subtype:   module
// I2.purpose:   utility
// I2.role:      [role]
// I3.file:      receiver-method.go
// I3.title:     Receiver method fixture
// I3.component: scope-test
// I3.path:      tests/fixtures/go/scope/receiver-method.go
// I3.provides:  [provides]
// I3.brief:     Fixture for R[3] self_access detection testing.
// I4.layer:     [layer]
// I4.position:  [position]
// I4.pattern:   [pattern]

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

// Metadata — Go uses comment-based context blocks.
//
// C1.version:           a-01.00
// C1.status:            Active
// C1.created:           2026-02-23
// C1.updated:           2026-02-23
// C2.architect:         Seanje Lenox-Wise
// C2.implementation:    Nova Dawn (CPI-SI)
// C2.organization:      CreativeWorkzStudio LLC
// C2.copyright:         CreativeWorkzStudio LLC
// C3.scripture:         Genesis 1:1
// C3.principle:         [principle]
// C3.anchor:            Genesis 1:1
// C5.purpose:           Fixture for R[3] self_access detection testing.

// ============================================================================
// END METADATA [END]
// ============================================================================

package scope

// ============================================================================
// SETUP BLOCK [SETUP]
// ============================================================================
//
// SETUP makes things EXIST. BODY makes things HAPPEN.
//

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

import "fmt"

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

// Node represents a tree node with a name and children.
type Node struct {
	Name     string
	Children []*Node
	Depth    int
}

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
// Available (not needed in this module):
//   Constants, Statics, Type Aliases, Error Types, Trait Defs, Interfaces

// ============================================================================
// END SETUP [END]
// ============================================================================

// ============================================================================
// BODY BLOCK [BODY]
// ============================================================================
//
// BODY makes things HAPPEN. SETUP made things EXIST.
//

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────

// Print prints the node's name — receiver access on n.Name.
func (n *Node) Print() {
	fmt.Println(n.Name)
}

// AddChild adds a child to this node — receiver access on n.Children.
func (n *Node) AddChild(child *Node) {
	child.Depth = n.Depth + 1
	n.Children = append(n.Children, child)
}

// Walk traverses the tree depth-first — recursive + receiver access.
func (n *Node) Walk(visit func(string)) {
	visit(n.Name)
	for _, child := range n.Children {
		child.Walk(visit)
	}
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Free Functions
// ──────────────────────────────────────────────────────────────────────────

// CountNodes counts all nodes in a tree recursively (free function recursion).
func CountNodes(node *Node) int {
	if node == nil {
		return 0
	}
	count := 1
	for _, child := range node.Children {
		count += CountNodes(child)
	}
	return count
}

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
// Available (not needed in this module):
//   Identity Access, Trait Implementations, Constructors, Queries, Output

// ============================================================================
// END BODY [END]
// ============================================================================

// ============================================================================
// CLOSING BLOCK [CLOSING]
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
// policy: Test fixture — do not modify structure
// scripture: "Proverbs 22:28 (WEB) — Don't move the ancient boundary stone."
// never: Break 4-block structure
// careful: Function signatures
// safe: Function bodies, Comments

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
// note: "R[3] self_access detection test fixture."
// scripture: "Psalm 19:2 — Day unto day uttereth speech."
// anchor: "Receiver methods for scope analysis testing."

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
// Available (not needed in this module):
//   Cv, Ce, Cc, X2, X3, X4

// ============================================================================
// END CLOSING [END]
// ============================================================================
