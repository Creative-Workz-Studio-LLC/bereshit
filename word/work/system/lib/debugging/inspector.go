// Package debugging provides state inspection infrastructure for CPI-SI.
//
// Rails component for deep execution state capture. See inspector-api.md for complete documentation.
//
// Quick Start:
//   inspector := debugging.NewInspector("component-name", contextID)
//   inspector.Enable()
//   inspector.Snapshot("state-label", vars)
//   inspector.ExpectedState("check", expected, actual, vars)
//   inspector.Disable()
//
// CHUNK 1 PROCESSED - SEE inspector-api.md FOR COMPLETE DOCUMENTATION
package debugging

// ============================================================================
// SETUP
// ============================================================================

// ─── Imports ───

import (
	"bufio"         // Buffered I/O for efficient line-by-line file reading
	"fmt"           // Formatted output for debug entries
	"maps"          // Efficient map operations (Copy for state merging)
	"os"            // File operations, environment variables, process info
	"os/exec"       // System command execution for context capture
	"path/filepath" // Cross-platform path manipulation for debug file routing
	"runtime"       // Go runtime introspection (call stacks, memory, goroutines)
	"strings"       // String processing for output formatting and parsing
	"sync"          // Synchronization primitives for thread-safe config loading
	"time"          // Timestamps and duration tracking

	"github.com/BurntSushi/toml" // TOML parsing for debugging.toml config
)

// ─── Types ───

// Inspector provides state inspection capabilities for a component.
type Inspector struct {
	component    string    // Component name for identification and correlation
	enabled      bool      // Inspection active (false = instant no-ops, true = capture state)
	outputFile   *os.File  // Debug file handle (nil when disabled)
	contextID    string    // Correlation ID shared with logger for rail correlation
	startTime    time.Time // Inspector creation time (for elapsed time in entries)
	username     string    // Pre-computed username (static per process)
	hostname     string    // Pre-computed hostname (static per process)
	pid          int       // Pre-computed process ID (static per process)
	userHost     string    // Pre-computed user@host:pid (static per process)
	homeDir      string    // Pre-computed home directory (static per process)
	componentDir string    // Pre-computed debug directory path (static per process)
	goVersion    string    // Pre-computed Go runtime version (static per process)
	numCPU       int       // Pre-computed CPU count (static per process)
}

// inspectionEntry represents a debug output entry (internal).
type inspectionEntry struct {
	Timestamp time.Time      // Inspection moment (for time-based correlation)
	Type      string         // Entry type constant (entrySnapshot, entryDivergence, etc.)
	Label     string         // User label for this inspection point
	CallSite  string         // Code location (file:line from runtime.Caller)
	Data      map[string]any // Captured state (variables, metrics, expected vs actual)
}

// InspectionEntry represents a parsed debug entry (exported for reading).
type InspectionEntry struct {
	Timestamp time.Time      // Inspection moment
	Type      string         // Entry type (SNAPSHOT, DIVERGENCE, TIMING, etc.)
	Label     string         // User label for this inspection point
	CallSite  string         // Code location (file:line)
	Data      map[string]any // Captured state
}

// ─── Configuration Loading (Rails Pattern) ───

// debuggingConfig represents the debugging.toml configuration structure
type debuggingConfig struct {
	Paths struct {
		BaseDir       string `toml:"base_dir"`
		FileExtension string `toml:"file_extension"`
	} `toml:"paths"`
	Permissions struct {
		Directory os.FileMode `toml:"directory"`
		File      os.FileMode `toml:"file"`
	} `toml:"permissions"`
	Output struct {
		DefaultStackDepth int `toml:"default_stack_depth"`
	} `toml:"output"`
}

// Singleton config instance with thread-safe initialization (rails pattern)
var (
	debugConfig     *debuggingConfig
	debugConfigOnce sync.Once
	debugLoaded     bool
)

// loadDebugConfig loads debugging.toml using singleton pattern
func loadDebugConfig() {
	debugConfigOnce.Do(func() {
		home, err := os.UserHomeDir()
		if err != nil {
			debugLoaded = false
			return
		}

		configPath := filepath.Join(home, ".claude/cpi-si/system/config/debugging.toml")
		data, err := os.ReadFile(configPath)
		if err != nil {
			debugLoaded = false
			return
		}

		var cfg debuggingConfig
		if err := toml.Unmarshal(data, &cfg); err != nil {
			debugLoaded = false
			return
		}

		debugConfig = &cfg
		debugLoaded = true
	})
}

// getDebugBaseDir returns configured base directory with fallback
func getDebugBaseDir() string {
	loadDebugConfig()
	if debugLoaded && debugConfig.Paths.BaseDir != "" {
		return debugConfig.Paths.BaseDir
	}
	return filepath.Join(claudeBaseDir, systemSubdir, debugSubdir) // Fallback to hardcoded
}

// getDebugFileExt returns configured file extension with fallback
func getDebugFileExt() string {
	loadDebugConfig()
	if debugLoaded && debugConfig.Paths.FileExtension != "" {
		return debugConfig.Paths.FileExtension
	}
	return debugFileExtension // Fallback to hardcoded constant
}

// getDebugDirPerms returns configured directory permissions with fallback
func getDebugDirPerms() os.FileMode {
	loadDebugConfig()
	if debugLoaded && debugConfig.Permissions.Directory != 0 {
		return debugConfig.Permissions.Directory
	}
	return debugDirPermissions // Fallback to hardcoded constant
}

// getDebugFilePerms returns configured file permissions with fallback
func getDebugFilePerms() os.FileMode {
	loadDebugConfig()
	if debugLoaded && debugConfig.Permissions.File != 0 {
		return debugConfig.Permissions.File
	}
	return debugFilePermissions // Fallback to hardcoded constant
}

// getDefaultStackDepth returns configured default call stack depth with fallback
func getDefaultStackDepth() int {
	loadDebugConfig()
	if debugLoaded && debugConfig.Output.DefaultStackDepth > 0 {
		return debugConfig.Output.DefaultStackDepth
	}
	return defaultCallStackDepth // Fallback to hardcoded constant
}

// ─── Constants ───

const (
	// Entry type identifiers - State Capture

	entrySnapshot      = "SNAPSHOT"         // Simple state capture at execution point (variables, conditions, values)
	entryExpectedState = "EXPECTED_STATE"   // Expected vs actual state comparison matches (state verification passed)
	entryDivergence    = "DIVERGENCE"       // State diverged from expected (verification failed, requires investigation)
	entryConditional   = "CONDITIONAL"      // Conditional state capture (only recorded when condition true)

	// Entry type identifiers - Execution Analysis

	entryTiming         = "TIMING"          // Performance timing within expected bounds (acceptable duration)
	entrySlowTiming     = "SLOW_TIMING"     // Performance exceeded expected duration (slow execution warning)
	entryCounter        = "COUNTER"         // Execution count matches expected (iteration verification passed)
	entryCountDivergence = "COUNT_DIVERGENCE" // Count diverged from expected (unexpected iterations, requires investigation)
	entryCallStack      = "CALLSTACK"       // Call stack trace capture (function call chain for debugging)
	entryCheckpoint     = "CHECKPOINT"      // Execution checkpoint marker (waypoint for flow verification)
	entryFlow           = "FLOW"            // Execution flow matches expected path (correct branch taken)
	entryUnexpectedFlow = "UNEXPECTED_FLOW" // Flow diverged from expected path (unexpected branch, logic error)

	// Entry type identifiers - System State

	entryMemory        = "MEMORY"           // Memory state capture (allocations, heap usage, GC stats)
	entrySystemContext = "SYSTEM_CONTEXT"   // Full system state capture (CPU, memory, disk, environment)

	// Fallback values

	unknownValue = "unknown" // Graceful failure return value when information unavailable

	// Timestamp formats

	timestampFormat       = "2006-01-02 15:04:05.000" // Standard debug entry timestamp format (millisecond precision)
	timestampFormatHeader = "2006-01-02 15:04:05"     // Session header timestamp format (second precision, no milliseconds)
)

const (
	// Format strings for entry components

	userHostFormat        = "%s@%s:%d"                  // User identity format combining username, hostname, and process ID
	entryHeaderFormat     = "[%s] %s | %s | %s | %s\n"  // Entry header line format (timestamp | type | component | user@host | context)
	eventLabelFormat      = "  EVENT: %s\n"             // Event description label with 2-space indentation
	callSiteFormat        = "  CALL SITE: %s\n"         // Call site location label with 2-space indentation (file:line)
	stateSectionHeader    = "  STATE:\n"                // State data section header with 2-space indentation
	stateKeyValueFormat   = "    %s: %v\n"              // State key-value format with 4-space indentation (nested under STATE)
	callSiteLocationFormat = "%s:%d"                    // Call site file and line format (file.go:123)
	debugFilenameFormat    = "%s-%d"                    // Debug filename base format (component-timestamp for uniqueness)

	// Entry structure

	entrySeparator = "---\n" // Entry separator line (matches logging rail convention)

	// Box-drawing header template

	headerTemplate = `╔════════════════════════════════════════════════════════════════╗
║ CPI-SI Debug Session - %s
║ Context ID: %s
║ PID: %d
║ Started: %s
╚════════════════════════════════════════════════════════════════╝

` // Header template for debug session start
)

const (
	// Directory structure (parallel to logging structure)

	claudeBaseDir         = ".claude"   // Base directory for Claude system (hidden directory in $HOME)
	systemSubdir          = "system"    // System subdirectory containing all CPI-SI infrastructure
	debugSubdir           = "debug"     // Debug subdirectory (parallel to logs/, same level in hierarchy)
	debugFileExtension    = ".debug"    // Debug file extension (distinguishes from .log files)
	debugDirPermissions   = 0755        // Directory permissions for debug directories (world readable/executable, owner writable)
	debugFilePermissions  = 0644        // File permissions for debug files (world readable, owner writable only)

	// Context ID format (matches logging)

	contextIDFormat       = "%s-%d-%d"  // Context ID format combining component name, process ID, and Unix timestamp

	// Default values

	defaultCallStackDepth = 10          // Default call stack capture depth (balance between context and output size)
)

const (
	// Environment variables

	envUser  = "USER"   // Username environment variable (current user identity)
	envHome  = "HOME"   // Home directory environment variable (user's home path)
	envShell = "SHELL"  // Shell environment variable (current shell binary path)

	// Proc filesystem paths (Linux)

	procLoadAvg = "/proc/loadavg" // CPU load averages (1min, 5min, 15min + running/total processes)
	procMemInfo = "/proc/meminfo" // Memory information (total, available, buffers, cache, swap)

	// Meminfo field names

	meminfoMemTotal     = "MemTotal:"     // Total memory field name (includes colon for exact match)
	meminfoMemAvailable = "MemAvailable:" // Available memory field name (includes colon for exact match)

	// Commands and arguments

	dfCommand      = "df" // Disk free command (disk usage statistics)
	dfHumanFlag    = "-h" // Human-readable flag for df (shows KB/MB/GB instead of blocks)
	currentDirPath = "."  // Current directory path (for df to show current mount point)
)

const (
	// Memory conversion

	bytesToMbDivisor = 1024 * 1024 // Bytes-to-megabytes divisor (1048576 converts runtime.MemStats bytes to MB)

	// Call stack formatting

	callStackEntryFormat = "%s (%s:%d)" // Call stack entry format combining function name with source location (function (file:line))
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ─── Helpers - Data ───

// ensureVarsMap ensures the vars map is initialized, creating it if nil.
func ensureVarsMap(vars map[string]any) map[string]any {
	if vars == nil {                          // User passed nil or didn't initialize
		return make(map[string]any)           // Create empty map to prevent panic
	}
	return vars                               // Already initialized - return as-is
}

// getEnvWithFallback retrieves environment variable with fallback to unknownValue.
func getEnvWithFallback(key string) string {
	value := os.Getenv(key)                   // Retrieve environment variable value
	if value == "" {                          // Empty or unset
		return unknownValue                   // Use "unknown" constant for consistency
	}
	return value                              // Return actual value
}

// mergeVars merges source map into destination map, overwriting existing keys.
func mergeVars(dest, src map[string]any) {
	if src == nil {                           // No source data to merge
		return                                // Safe no-op - dest unchanged
	}
	maps.Copy(dest, src)                      // Copy all src keys into dest (overwrites existing)
}

// ─── Helpers - Entry Writing ───

// writeInspectionEntry formats and writes inspection entry to debug file.
func (i *Inspector) writeInspectionEntry(entry inspectionEntry) {
	// CORRELATION POINTS - Same as logging for safe correlation

	timestamp := entry.Timestamp.Format(timestampFormat)                     // 1. Timestamp (moment)
	contextID := i.contextID                                                 // 2. Context ID (execution/baton)
	component := i.component                                                 // 3. Component name
	userHost := i.userHost                                                   // 4-5. User@hostname:pid (pre-computed at initialization)

	header := fmt.Sprintf(entryHeaderFormat, timestamp, entry.Type, component, userHost, contextID) // Write entry with ALL correlation points
	i.outputFile.WriteString(header)                                         // Write header to file

	fmt.Fprintf(i.outputFile, eventLabelFormat, entry.Label)                // Write event/label
	fmt.Fprintf(i.outputFile, callSiteFormat, entry.CallSite)               // Write call site as context

	if len(entry.Data) > 0 {                                                 // Write state data (matching DETAILS format from logging)
		i.outputFile.WriteString(stateSectionHeader)                         // Write STATE: section header
		for key, value := range entry.Data {
			fmt.Fprintf(i.outputFile, stateKeyValueFormat, key, value)      // Write each key-value pair indented
		}
	}

	i.outputFile.WriteString(entrySeparator)                                 // Write entry separator (---)
	i.outputFile.Sync()                                                      // Ensure written to disk
}

// writeHeader writes debug session header with session information.
func (i *Inspector) writeHeader() {
	if !i.enabled || i.outputFile == nil {                                  // Guard - no-op if disabled or file not open
		return
	}

	header := fmt.Sprintf(headerTemplate,                                   // Format header with component, context, PID, start time
		i.component, i.contextID, i.pid, i.startTime.Format(timestampFormatHeader))

	i.outputFile.WriteString(header)                                        // Write formatted header to file
}

// writeEntry writes a debug entry to output.
func (i *Inspector) writeEntry(entryType, label string, data map[string]any) {
	if !i.enabled || i.outputFile == nil {                                  // Guard - no-op if disabled or file not open
		return
	}

	// CORRELATION POINTS - Same as logging for safe correlation - Build structured entry first, then write it

	_, file, line, ok := runtime.Caller(2)                                   // 6. Call site (code location)
	callSite := unknownValue                                                 // Default to unknown if capture fails
	if ok {
		callSite = fmt.Sprintf(callSiteLocationFormat, filepath.Base(file), line) // Format as file:line
	}

	entry := inspectionEntry{                                                // Create structured inspection entry
		Timestamp: time.Now(),                                               // Current moment for time correlation
		Type:      entryType,                                                // Entry type constant (SNAPSHOT, DIVERGENCE, etc.)
		Label:     label,                                                    // User-provided label for this point
		CallSite:  callSite,                                                 // Code location (file:line)
		Data:      data,                                                     // Variable state captured by caller
	}

	i.writeInspectionEntry(entry)                                            // Write the entry to output file
}

// ─── Inspector Creation ───

// NewInspector creates a new inspector instance for a component.
func NewInspector(component string, contextID ...string) *Inspector {
	// CONTEXT ID SELECTION - Use provided or generate unique ID

	var ctx string                                                               // Context ID for this inspector instance
	if len(contextID) > 0 && contextID[0] != "" {                                // Check if contextID provided and non-empty
		ctx = contextID[0]                                                       // Use provided contextID for rail correlation
	} else {                                                                     // No contextID provided
		ctx = fmt.Sprintf(contextIDFormat, component, os.Getpid(), time.Now().UnixNano())  // Generate using same format as logging: component-pid-nanotime
	}

	// PRE-COMPUTE UNCHANGING CORRELATION POINTS
	// These values don't change during process lifetime - compute once, reuse everywhere

	username := getEnvWithFallback(envUser)                                      // Capture username with fallback to "unknown"
	hostname, err := os.Hostname()                                               // Capture hostname for user@host correlation
	if err != nil {                                                              // Handle hostname lookup failure gracefully
		hostname = unknownValue                                                  // Use constant fallback
	}
	pid := os.Getpid()                                                           // Capture PID for user@host:pid correlation
	userHost := fmt.Sprintf(userHostFormat, username, hostname, pid)             // Format user@host:pid string once
	homeDir := getEnvWithFallback(envHome)                                       // Capture home directory with fallback

	// PRE-COMPUTE UNCHANGING DEBUG DIRECTORY PATHS
	// Construct paths once at creation - reused in Enable for file operations

	debugBaseDir := getDebugBaseDir()                                             // Get configured base directory path
	debugDir := filepath.Join(homeDir, ".claude", debugBaseDir)                   // Full debug directory path
	componentDir := filepath.Join(debugDir, component)                            // Component-specific debug directory

	// PRE-COMPUTE UNCHANGING RUNTIME INFORMATION
	// System information that remains constant during process lifetime

	goVersion := runtime.Version()                                               // Go version doesn't change during execution
	numCPU := runtime.NumCPU()                                                   // CPU count doesn't change during execution

	// CONSTRUCT INSPECTOR INSTANCE

	inspector := &Inspector{                                                     // Create and initialize inspector
		component:    component,                                                 // Component name for identification
		enabled:      false,                                                     // Disabled by default (call Enable to activate)
		contextID:    ctx,                                                       // Context ID for rail correlation
		startTime:    time.Now(),                                                // Capture creation time
		username:     username,                                                  // Pre-computed username (reused for every entry)
		hostname:     hostname,                                                  // Pre-computed hostname (reused for every entry)
		pid:          pid,                                                       // Pre-computed PID (reused for every entry)
		userHost:     userHost,                                                  // Pre-computed user@host:pid string (reused for every entry)
		homeDir:      homeDir,                                                   // Pre-computed home directory (reused in SystemContext)
		componentDir: componentDir,                                              // Pre-computed component debug directory (reused in Enable)
		goVersion:    goVersion,                                                 // Pre-computed Go version (reused in SystemContext)
		numCPU:       numCPU,                                                    // Pre-computed CPU count (reused in SystemContext)
	}

	return inspector                                                             // Return initialized inspector
}

// ─── Lifecycle Management ───

// Enable activates debug output for this inspector.
func (i *Inspector) Enable() error {
	if i.enabled {                                                               // Already enabled - no-op
		return nil                                                               // Return success
	}

	// CREATE DEBUG OUTPUT DIRECTORY

	dirPerms := getDebugDirPerms()                                               // Get configured directory permissions
	if err := os.MkdirAll(i.componentDir, dirPerms); err != nil {                // Create directory using configured permissions
		return fmt.Errorf("failed to create debug directory: %w", err)           // Return wrapped error if creation fails
	}

	// CREATE DEBUG OUTPUT FILE

	filenameBase := fmt.Sprintf(debugFilenameFormat, i.component, time.Now().Unix())  // Format: component-timestamp
	fileExt := getDebugFileExt()                                                 // Get configured file extension
	filename := filenameBase + fileExt                                           // Add configured extension
	filePath := filepath.Join(i.componentDir, filename)                          // Full path to debug file

	filePerms := getDebugFilePerms()                                             // Get configured file permissions
	file, err := os.OpenFile(filePath, os.O_CREATE|os.O_WRONLY|os.O_APPEND, filePerms)  // Open for append with configured permissions
	if err != nil {                                                              // File creation failed
		return fmt.Errorf("failed to create debug file: %w", err)                // Return wrapped error
	}

	i.outputFile = file                                                          // Store file handle for writing
	i.enabled = true                                                             // Mark as enabled

	// WRITE SESSION HEADER

	i.writeHeader()                                                              // Write header with correlation points

	return nil                                                                   // Success
}

// Disable deactivates debug output for this inspector.
func (i *Inspector) Disable() error {
	if !i.enabled {                                                              // Already disabled - no-op
		return nil                                                               // Return success
	}

	if i.outputFile != nil {                                                     // Check if file handle exists
		i.outputFile.Close()                                                     // Close file handle cleanly
		i.outputFile = nil                                                       // Clear reference to prevent reuse
	}

	i.enabled = false                                                            // Mark as disabled
	return nil                                                                   // Success
}

// IsEnabled returns whether inspection is currently enabled.
func (i *Inspector) IsEnabled() bool {
	return i.enabled                                                             // Return current enabled state
}

// Close cleanly closes the inspector (defer-compatible).
func (i *Inspector) Close() error {
	return i.Disable()                                                           // Delegate to Disable for actual cleanup
}

// ─── File Reading ───

// ReadDebugFile reads and parses a debug file into InspectionEntry structures.
func ReadDebugFile(path string) ([]InspectionEntry, error) {
	file, err := os.Open(path)                                               // Open debug file for reading
	if err != nil {                                                          // File open failed
		return nil, err                                                      // Return error to caller
	}
	defer file.Close()                                                       // Ensure file closes when function exits

	var entries []InspectionEntry                                            // Slice to collect parsed entries
	var currentEntry *InspectionEntry                                        // Current entry being parsed (nil between entries)
	scanner := bufio.NewScanner(file)                                        // Line-by-line scanner

	for scanner.Scan() {                                                     // Read each line
		line := scanner.Text()                                               // Get line text

		// NEW ENTRY DETECTION - Lines starting with [timestamp] mark new entries

		if strings.HasPrefix(line, "[") && strings.Contains(line, "]") {     // Entry header line detected
			if currentEntry != nil {                                         // Previous entry exists (not first entry)
				entries = append(entries, *currentEntry)                     // Save completed previous entry
			}

			// HEADER PARSING - Format: [timestamp] TYPE | component | user@host | context

			parts := strings.SplitN(line, "|", 4)                            // Split header by pipe separators
			if len(parts) >= 4 {                                             // Valid header format (4+ parts)
				timestampStr := strings.TrimSpace(strings.Trim(strings.SplitN(parts[0], "]", 2)[0], "["))  // Extract timestamp between brackets
				timestamp, _ := time.Parse(timestampFormat, timestampStr)    // Parse using SETUP timestamp format constant

				entryType := strings.TrimSpace(strings.SplitN(parts[0], "]", 2)[1])  // Extract type after ] bracket

				currentEntry = &InspectionEntry{                             // Create new entry
					Timestamp: timestamp,                                    // Set parsed timestamp
					Type:      entryType,                                    // Set entry type (SNAPSHOT, TIMING, etc.)
					Data:      make(map[string]any),                         // Initialize empty state map
				}
			}
		} else if currentEntry != nil {                                      // Continuation line (part of current entry)
			// EVENT LINE PARSING - Captures user-provided label

			trimmedLine := strings.TrimSpace(line)                           // Trim once for reuse
			if labelText, found := strings.CutPrefix(trimmedLine, "EVENT:"); found {  // EVENT section line
				currentEntry.Label = strings.TrimSpace(labelText)            // Extract label text
			}

			// CALL SITE PARSING - Captures code location (file:line)

			if callSiteText, found := strings.CutPrefix(trimmedLine, "CALL SITE:"); found {  // CALL SITE section line
				currentEntry.CallSite = strings.TrimSpace(callSiteText)      // Extract call site
			}

			// STATE DATA PARSING - Key-value pairs from STATE section

			if strings.Contains(line, ":") && !strings.HasPrefix(strings.TrimSpace(line), "EVENT:") &&  // Contains colon but not section header
				!strings.HasPrefix(strings.TrimSpace(line), "CALL SITE:") &&  // Not CALL SITE header
				!strings.HasPrefix(strings.TrimSpace(line), "STATE:") {       // Not STATE header
				parts := strings.SplitN(strings.TrimSpace(line), ":", 2)     // Split key:value on first colon
				if len(parts) == 2 {                                         // Valid key-value format
					currentEntry.Data[strings.TrimSpace(parts[0])] = strings.TrimSpace(parts[1])  // Add to state map
				}
			}
		}

		// ENTRY BOUNDARY DETECTION - Separator marks end of entry

		if strings.TrimSpace(line) == strings.TrimSpace(entrySeparator) && currentEntry != nil {  // Entry separator found
			entries = append(entries, *currentEntry)                         // Save completed entry
			currentEntry = nil                                               // Reset for next entry
		}
	}

	// FINAL ENTRY HANDLING - File may not end with separator

	if currentEntry != nil {                                                 // Entry in progress when file ended
		entries = append(entries, *currentEntry)                             // Save final entry
	}

	return entries, scanner.Err()                                            // Return entries and any scan error
}

// ─── Public API - State Capture ───

// Snapshot captures current variable state at this execution point.
func (i *Inspector) Snapshot(label string, vars map[string]any) {
	i.writeEntry(entrySnapshot, label, vars)                             // Write SNAPSHOT entry with provided variables
}

// ExpectedState captures expected vs actual state comparison.
func (i *Inspector) ExpectedState(label string, expected, actual any, vars map[string]any) {
	vars = ensureVarsMap(vars)                                           // Ensure vars map initialized (create if nil)

	vars["expected"] = expected                                          // Add expected value to vars
	vars["actual"] = actual                                              // Add actual value to vars
	vars["matches"] = expected == actual                                 // Add boolean match result

	entryType := entryExpectedState                                      // Default to EXPECTED_STATE type (match)
	if expected != actual {                                              // Values don't match - divergence detected
		entryType = entryDivergence                                      // Use DIVERGENCE type (mismatch)
	}

	i.writeEntry(entryType, label, vars)                                 // Write entry with appropriate type
}

// ConditionalSnapshot captures state only when condition is true.
func (i *Inspector) ConditionalSnapshot(label string, condition bool, vars map[string]any) {
	if !condition {                                                      // Condition not met - skip capture
		return                                                           // Instant return (no entry written)
	}

	vars = ensureVarsMap(vars)                                           // Ensure vars map initialized
	vars["condition_met"] = true                                         // Mark that condition was met

	i.writeEntry(entryConditional, label, vars)                          // Write CONDITIONAL entry
}

// ═══ Execution Analysis ═══

// Timing captures performance metrics with expected vs actual duration comparison.
func (i *Inspector) Timing(label string, duration, expected time.Duration) {
	vars := map[string]any{                                              // Construct performance metrics map
		"duration_ms":     duration.Milliseconds(),                      // Actual duration in milliseconds
		"expected_ms":     expected.Milliseconds(),                      // Expected threshold in milliseconds
		"variance_ms":     (duration - expected).Milliseconds(),         // How much over/under expected
		"within_expected": duration <= expected,                         // Boolean: met performance threshold
	}

	entryType := entryTiming                                             // Default to TIMING type (within expected)
	if duration > expected {                                             // Duration exceeded threshold
		entryType = entrySlowTiming                                      // Use SLOW_TIMING type (performance issue)
	}

	i.writeEntry(entryType, label, vars)                                 // Write entry with appropriate type
}

// Counter tracks execution counts with expected vs actual comparison.
func (i *Inspector) Counter(label string, count, expected int) {
	vars := map[string]any{                                              // Construct count metrics map
		"count":          count,                                         // Actual execution count
		"expected":       expected,                                      // Expected execution count
		"variance":       count - expected,                              // How much over/under expected
		"matches":        count == expected,                             // Boolean: count matched expectation
	}

	entryType := entryCounter                                            // Default to COUNTER type (matched expected)
	if count != expected {                                               // Count didn't match expected
		entryType = entryCountDivergence                                 // Use COUNT_DIVERGENCE type (mismatch)
	}

	i.writeEntry(entryType, label, vars)                                 // Write entry with appropriate type
}

// CallStack captures the current call stack trace.
func (i *Inspector) CallStack(label string, depth int) {
	if !i.enabled {                                                      // Inspection disabled - skip
		return                                                           // Instant return
	}

	if depth <= 0 {                                                      // No depth specified or invalid
		depth = getDefaultStackDepth()                                   // Use configured default depth
	}

	// CAPTURE CALL STACK FRAMES

	stack := make([]string, 0, depth)                                    // Pre-allocate slice for frames
	for skip := 1; skip <= depth; skip++ {                               // Walk stack (skip=1 skips CallStack itself)
		pc, file, line, ok := runtime.Caller(skip)                       // Get frame at this depth
		if !ok {                                                         // No more frames available
			break                                                        // Stop walking stack
		}

		fn := runtime.FuncForPC(pc)                                      // Get function info for program counter
		funcName := unknownValue                                         // Default to unknown
		if fn != nil {                                                   // Function info available
			funcName = fn.Name()                                         // Extract function name
		}

		stack = append(stack, fmt.Sprintf(callStackEntryFormat, funcName, filepath.Base(file), line))  // Format and append frame
	}

	// CONSTRUCT ENTRY DATA

	data := map[string]any{                                              // Build entry data map
		"depth": len(stack),                                             // How many frames captured
		"stack": strings.Join(stack, " <- "),                            // Format as call chain
	}

	i.writeEntry(entryCallStack, label, data)                            // Write CALLSTACK entry
}

// Checkpoint marks an important execution waypoint with state capture.
func (i *Inspector) Checkpoint(label string, vars map[string]any) {
	i.writeEntry(entryCheckpoint, label, vars)                           // Write CHECKPOINT entry
}

// Flow tracks which execution path was taken with optional expected path comparison.
func (i *Inspector) Flow(label string, branch string, expected ...string) {
	data := map[string]any{                                              // Construct flow data map
		"branch_taken": branch,                                          // Which path was actually executed
	}

	entryType := entryFlow                                               // Default to FLOW type (no comparison or matched)
	if len(expected) > 0 && expected[0] != "" {                          // Expected branch provided
		data["expected_branch"] = expected[0]                            // Add expected path to data
		data["matches_expected"] = branch == expected[0]                 // Add boolean match result

		if branch != expected[0] {                                       // Execution took unexpected path
			entryType = entryUnexpectedFlow                              // Use UNEXPECTED_FLOW type (divergence)
		}
	}

	i.writeEntry(entryType, label, data)                                 // Write entry with appropriate type
}

// ═══ System State ═══

// Memory captures current memory allocation state for leak detection.
func (i *Inspector) Memory(label string, vars map[string]any) {
	var m runtime.MemStats                                               // Memory statistics structure
	runtime.ReadMemStats(&m)                                             // Read current memory stats from runtime

	data := map[string]any{                                              // Construct memory metrics map
		"alloc_mb":       m.Alloc / bytesToMbDivisor,                    // Currently allocated memory in MB
		"total_alloc_mb": m.TotalAlloc / bytesToMbDivisor,               // Cumulative allocated memory in MB
		"sys_mb":         m.Sys / bytesToMbDivisor,                      // System memory obtained in MB
		"num_gc":         m.NumGC,                                       // Number of GC cycles completed
		"goroutines":     runtime.NumGoroutine(),                        // Current goroutine count
	}

	// MERGE PROVIDED CONTEXT

	mergeVars(data, vars)                                                // Merge optional user-provided vars into data

	i.writeEntry(entryMemory, label, data)                               // Write MEMORY entry
}

// SystemContext captures full system environment snapshot matching logging's context capture.
func (i *Inspector) SystemContext(label string) {
	data := make(map[string]any)                                         // Initialize context data map

	// USER CONTEXT - Pre-computed and dynamic values

	data["user"] = i.username                                            // Pre-computed username (unchanging during execution)
	data["shell"] = os.Getenv(envShell)                                  // Current shell (dynamic - can change)
	data["home"] = i.homeDir                                             // Pre-computed home directory (unchanging)

	// WORKING DIRECTORY

	if cwd, err := os.Getwd(); err == nil {                              // Get current working directory
		data["cwd"] = cwd                                                // Add to context if successful
	}

	// SYSTEM LOAD (Linux)

	if loadavg, err := os.ReadFile(procLoadAvg); err == nil {            // Read /proc/loadavg
		data["load"] = strings.Fields(string(loadavg))[0:3]              // Extract 1, 5, 15 minute load averages
	}

	// MEMORY INFO (Linux)

	if meminfo, err := os.ReadFile(procMemInfo); err == nil {            // Read /proc/meminfo
		for line := range strings.SplitSeq(string(meminfo), "\n") {     // Parse each line
			if strings.HasPrefix(line, meminfoMemTotal) || strings.HasPrefix(line, meminfoMemAvailable) {  // Match Total/Available
				fields := strings.Fields(line)                           // Split into fields
				if len(fields) >= 2 {                                    // Ensure field exists
					data[strings.TrimSuffix(fields[0], ":")] = fields[1]  // Add to context (remove trailing colon)
				}
			}
		}
	}

	// DISK USAGE

	if output, err := exec.Command(dfCommand, dfHumanFlag, currentDirPath).Output(); err == nil {  // Run df -h .
		lines := strings.Split(string(output), "\n")                     // Split output into lines
		if len(lines) > 1 {                                              // Ensure data line exists (skip header)
			data["disk_usage"] = strings.Fields(lines[1])                // Parse disk usage fields
		}
	}

	// GO RUNTIME - Pre-computed values

	data["go_version"] = i.goVersion                                     // Pre-computed Go version (unchanging)
	data["num_cpu"] = i.numCPU                                           // Pre-computed CPU count (unchanging)

	i.writeEntry(entrySystemContext, label, data)                        // Write SYSTEM_CONTEXT entry
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// This is a LIBRARY, not an executable. Import and use:
//   import "system/lib/debugging"
//
// See inspector-api.md for complete documentation.
//
// Quick Start:
//   inspector := debugging.NewInspector("component-name")
//   inspector.Enable()
//   inspector.Snapshot("label", vars)
//   inspector.ExpectedState("check", expected, actual, vars)
//   inspector.Timing("operation", duration, expectedDuration)
//   inspector.Disable()
//
// Rails Architecture: Components create their own inspector directly.
// Never pass inspectors through function parameters.
//
// Rail Correlation: Share contextID with logger for cross-rail analysis:
//   inspector := debugging.NewInspector("component", logger.ContextID)
//
// CHUNK PROCESSING COMPLETE - SEE inspector-api.md FOR FULL DOCUMENTATION
//
// ============================================================================
// END CLOSING
// ============================================================================
