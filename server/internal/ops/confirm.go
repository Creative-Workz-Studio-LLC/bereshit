// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-ops-confirm
// Purpose: Interactive confirmation for destructive operations
// Biblical: Proverbs 14:15 — "The simple believeth every word: but the
//           prudent man looketh well to his going"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-12
//
// ═══════════════════════════════════════════════════════════════════════════

package ops

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Confirm prompts the user for y/n confirmation on destructive operations.
// Returns true if the user types "y" or "yes" (case-insensitive).
// If noConfirm is true, skips the prompt and returns true immediately.
func Confirm(prompt string, noConfirm bool) bool {
	if noConfirm {
		return true
	}

	fmt.Printf("%s [y/N] ", prompt)
	reader := bufio.NewReader(os.Stdin)
	answer, err := reader.ReadString('\n')
	if err != nil {
		return false
	}

	answer = strings.TrimSpace(strings.ToLower(answer))
	return answer == "y" || answer == "yes"
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
