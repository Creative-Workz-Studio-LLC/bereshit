// ============================================================================
// METADATA
// ============================================================================
// Log File Reading & Parsing - Logging Library
//
// Biblical Foundation
//
// Scripture: "Study to shew thyself approved unto God, a workman that needeth not to be ashamed, rightly dividing the word of truth" (2 Timothy 2:15, KJV)
// Principle: Careful examination reveals truth. Parsing log files reconstructs the narrative from written record.
// Anchor: Reading and parsing is the inverse of writing - reconstructing structured data from formatted text. Faithful parsing enables the debugging layer to analyze execution history.
//
// CPI-SI Identity
//
// Component Type: Parsing module within Rails infrastructure
// Role: Read and parse log files into structured data (enables Assessment layer)
// Paradigm: CPI-SI framework component
//
// Authorship & Lineage
//
// Architect: Seanje Lenox-Wise, Nova Dawn
// Implementation: Nova Dawn
// Creation Date: 2025-11-18
// Version: 1.0.0
// Last Modified: 2025-11-18 - Extracted from monolithic logger.go
//
// Purpose & Function
//
// Purpose: Read log files and parse them back into LogEntry structures for analysis. Enables the debugging layer to examine execution history by reconstructing the structured data from formatted log files.
//
// Core Design: Line-by-line state machine parser. Recognizes entry boundaries, header format, sections (EVENT, DETAILS, CONTEXT, INTERACTIONS), and reconstructs LogEntry structures.
//
// Key Features:
//   - Header parsing (timestamp, level, component, context ID, health)
//   - Section parsing (EVENT, DETAILS, CONTEXT, INTERACTIONS)
//   - Multi-line value handling
//   - Entry boundary detection (separator lines)
//   - Graceful error handling (returns partial data + error)
//
// Blocking Status
//
// Non-blocking: Parse errors return what was successfully parsed plus error. Partial data is useful for debugging even if complete parse fails.
// Mitigation: Best-effort parsing - extract what's valid, return partial results with error information.
//
// Usage & Integration
//
// Usage:
//
//	import "system/runtime/lib/logging"
//
// Integration Pattern:
//   1. Debugging layer calls ReadLogFile(path) with log file path
//   2. Parser reads file line-by-line with bufio.Scanner
//   3. State machine recognizes entry boundaries and sections
//   4. Returns []LogEntry slice with all parsed entries
//   5. Used by debugger command for log analysis
//
// Public API:
//   ReadLogFile(path string) ([]LogEntry, error) - Parse log file into entry slice
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: bufio, fmt, os, strings, time
//   Package Files: entry.go (LogEntry type, entrySeparator constant)
//
// Dependents (What Uses This):
//   External: system/runtime/lib/debugging (log analysis)
//   Commands: debugger command (health assessment)
//
// Health Scoring
//
// Base100 scoring algorithm (CPSI-ALG-001).
//
// Parsing Operations (100 pts):
//   - File open: +10 (success), 0 (failure)
//   - Header parsing: +30 (all fields), +15 (partial), 0 (failed)
//   - Section parsing: +40 (complete), +20 (partial), 0 (failed)
//   - Entry reconstruction: +20 (all entries), +10 (some entries), 0 (none)
//
// Note: This module's health is about parse accuracy, not log content quality.

package logging

// ============================================================================
// SETUP
// ============================================================================

// Imports

import (
	"bufio"   // Line-by-line file reading
	"fmt"     // String parsing (Sscanf)
	"os"      // File operations
	"strings" // String manipulation for parsing
	"time"    // Timestamp parsing
)

// Constants (from entry.go)
// entrySeparator is defined in entry.go and used here for boundary detection

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Core Operations - Log File Parsing
// ────────────────────────────────────────────────────────────────

// ReadLogFile reads and parses a log file into LogEntry structures.
//
// Parser design: State machine recognizing entry boundaries and sections.
// Entry format: [timestamp] LEVEL | component | user@host:pid | context-id | HEALTH: X% (raw: Y, ΔZ)
//               Followed by EVENT, DETAILS, CONTEXT, INTERACTIONS sections, then separator (---)
func ReadLogFile(path string) ([]LogEntry, error) {
	file, err := os.Open(path) // Open log file for reading
	if err != nil {             // File open failed
		return nil, err // Return error to caller
	}
	defer file.Close() // Ensure file closes when function exits

	var entries []LogEntry     // Slice to collect parsed entries
	var currentEntry *LogEntry // Current entry being parsed (nil between entries)
	scanner := bufio.NewScanner(file)

	for scanner.Scan() { // Read each line
		line := scanner.Text() // Get line text

		// NEW ENTRY DETECTION - Lines starting with [timestamp] mark new entries

		if strings.HasPrefix(line, "[") && strings.Contains(line, "|") { // Entry header line detected
			if currentEntry != nil { // Previous entry exists (not first entry)
				entries = append(entries, *currentEntry) // Save completed previous entry
			}

			// HEADER PARSING - Format: [timestamp] LEVEL | component | user@host:pid | context-id | HEALTH: X (ΔY)

			parts := strings.SplitN(line, "|", 5) // Split header by pipe separators
			if len(parts) >= 5 {                  // Valid header format (5+ parts)
				// Extract timestamp
				timestampStr := strings.TrimSpace(strings.Trim(strings.SplitN(parts[0], "]", 2)[0], "[")) // Extract timestamp between brackets
				timestamp, _ := time.Parse(timestampFormat, timestampStr)                                  // Parse using timestamp format constant

				// Extract level
				level := strings.TrimSpace(strings.SplitN(parts[0], "]", 2)[1]) // Extract level after ] bracket

				// Extract component
				component := strings.TrimSpace(parts[1]) // Component name from second part

				// Extract context ID
				contextID := strings.TrimSpace(parts[3]) // Context ID from fourth part

				// Extract health values from HEALTH: X% (raw: Y, ΔZ) pattern
				healthPart := parts[4]   // Fifth part contains health info
				normalizedHealth := 0    // Default normalized health
				rawHealth := 0           // Default raw health
				healthImpact := 0        // Default health impact
				// Extract normalized health, raw health, and delta from new format
				if strings.Contains(healthPart, "HEALTH:") { // Health info present
					// Extract normalized health (percentage after HEALTH:)
					normalizedStr := strings.TrimSpace(strings.Split(healthPart, "(")[0])            // Part before first parenthesis
					normalizedStr = strings.TrimSpace(strings.TrimPrefix(normalizedStr, "HEALTH:"))  // Remove prefix
					normalizedStr = strings.TrimSuffix(normalizedStr, "%")                           // Remove % sign
					fmt.Sscanf(normalizedStr, "%d", &normalizedHealth)                               // Parse integer

					// Extract raw health (number after "raw:")
					if strings.Contains(healthPart, "raw:") { // Raw health present
						rawStr := strings.Split(strings.Split(healthPart, "raw:")[1], ",")[0] // Extract between "raw:" and ","
						fmt.Sscanf(strings.TrimSpace(rawStr), "%d", &rawHealth)               // Parse integer
					}

					// Extract delta (number in parentheses with Δ)
					if strings.Contains(healthPart, "Δ") { // Delta present
						deltaStr := strings.Split(strings.Split(healthPart, "Δ")[1], ")")[0] // Extract between Δ and )
						fmt.Sscanf(deltaStr, "%d", &healthImpact)                            // Parse integer (handles +/-)
					}
				}

				currentEntry = &LogEntry{ // Create new entry
					Timestamp:        timestamp,        // Set parsed timestamp
					Level:            level,            // Set log level (OPERATION, SUCCESS, etc.)
					Component:        component,        // Set component name
					ContextID:        contextID,        // Set context ID for correlation
					NormalizedHealth: normalizedHealth, // Set normalized health percentage
					RawHealth:        rawHealth,        // Set cumulative health
					HealthImpact:     healthImpact,     // Set health delta
					Details:          make(map[string]any), // Initialize empty details map
				}
			}
		} else if currentEntry != nil { // Continuation line (part of current entry)
			// EVENT LINE PARSING - Captures event description

			trimmedLine := strings.TrimSpace(line)                                                // Trim once for reuse
			if eventText, found := strings.CutPrefix(trimmedLine, "EVENT:"); found {              // EVENT section line
				currentEntry.Event = strings.TrimSpace(eventText) // Extract event text
			}

			// DETAILS SECTION PARSING - Key-value pairs from DETAILS section

			if strings.Contains(line, ":") && !strings.HasPrefix(strings.TrimSpace(line), "EVENT:") && // Contains colon but not section header
				!strings.HasPrefix(strings.TrimSpace(line), "DETAILS:") &&     // Not DETAILS header
				!strings.HasPrefix(strings.TrimSpace(line), "CONTEXT:") &&     // Not CONTEXT header
				!strings.HasPrefix(strings.TrimSpace(line), "INTERACTIONS:") { // Not INTERACTIONS header
				parts := strings.SplitN(strings.TrimSpace(line), ":", 2) // Split key:value on first colon
				if len(parts) == 2 {                                     // Valid key-value format
					currentEntry.Details[strings.TrimSpace(parts[0])] = strings.TrimSpace(parts[1]) // Add to details map
				}
			}
		}

		// ENTRY BOUNDARY DETECTION - Separator marks end of entry

		if strings.TrimSpace(line) == strings.TrimSpace(entrySeparator) && currentEntry != nil { // Entry separator found
			entries = append(entries, *currentEntry) // Save completed entry
			currentEntry = nil                       // Reset for next entry
		}
	}

	// FINAL ENTRY HANDLING - File may not end with separator

	if currentEntry != nil { // Entry in progress when file ended
		entries = append(entries, *currentEntry) // Save final entry
	}

	return entries, scanner.Err() // Return entries and any scan error
}

// ============================================================================
// CLOSING
// ============================================================================
// Library module (no entry point). Import: "system/runtime/lib/logging"
//
// ============================================================================
// END CLOSING
// ============================================================================
