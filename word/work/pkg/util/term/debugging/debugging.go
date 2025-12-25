// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-debugging
// Purpose: Structured debugging for CPI-SI - Assessment layer of immune system
// Biblical: Job 28:27 - "Then did he see it, and declare it; he prepared it"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-10
//
// Design Goals:
//   - Three verbosity levels (BASIC, DETAILED, VERBOSE)
//   - Clear section markers for parsing
//   - Event timeline with health tracking
//   - Context capture for reproducibility
//
// ═══════════════════════════════════════════════════════════════════════════

package debugging

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"io"
	"os"
	"os/user"
	"runtime"
	"strings"
	"time"
)

// --- Constants ---

// Debug verbosity levels
const (
	LevelBasic    Level = iota // State snapshots only
	LevelDetailed              // + Event timeline
	LevelVerbose               // + Analysis section
)

// Event types for timeline
const (
	EventInit       EventType = "INIT"
	EventValidate   EventType = "VALIDATE"
	EventOperation  EventType = "OPERATION"
	EventCheckpoint EventType = "CHECKPOINT"
	EventDecision   EventType = "DECISION"
	EventWarning    EventType = "WARNING"
	EventError      EventType = "ERROR"
)

// Event severities
const (
	SeverityDebug Severity = "DEBUG"
	SeverityInfo  Severity = "INFO"
	SeverityWarn  Severity = "WARN"
	SeverityError Severity = "ERROR"
)

// Result states
const (
	ResultSuccess Result = "SUCCESS"
	ResultPartial Result = "PARTIAL"
	ResultFailed  Result = "FAILED"
	ResultTimeout Result = "TIMEOUT"
)

// Health emoji map
var healthEmoji = []struct {
	min   int
	emoji string
	text  string
}{
	{90, "💚", "Excellent"},
	{70, "💛", "Good"},
	{50, "🧡", "Moderate"},
	{30, "❤️", "Poor"},
	{10, "🩸", "Critical"},
	{0, "☠️", "Failed"},
}

// --- Types ---

type Level int
type EventType string
type Severity string
type Result string

// Event represents a single debug event in the timeline
type Event struct {
	Timestamp    time.Time
	Severity     Severity
	Type         EventType
	Description  string
	Details      map[string]string
	HealthBefore int
	HealthAfter  int
}

// Session represents a complete debug session
type Session struct {
	// Identity
	Component string
	ContextID string
	PID       int
	Level     Level

	// Timing
	StartTime time.Time
	EndTime   time.Time

	// Context
	Username string
	Hostname string
	Shell    string
	Cwd      string
	Distro   string
	Arch     string
	Kernel   string

	// State
	InitialHealth int
	FinalHealth   int
	Parameters    map[string]string
	InitialState  map[string]string
	FinalState    map[string]string

	// Events (for DETAILED and VERBOSE)
	Events []Event

	// Analysis (for VERBOSE only)
	Issues          []string
	Performance     map[string]string
	Recommendations []string

	// Outcome
	Result   Result
	ExitCode int

	// Output
	out io.Writer
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Session Construction ---

// NewSession creates a new debug session
func NewSession(component string, level Level) *Session {
	now := time.Now()
	pid := os.Getpid()

	s := &Session{
		Component:     component,
		ContextID:     fmt.Sprintf("%s-%d-%d", component, pid, now.UnixNano()),
		PID:           pid,
		Level:         level,
		StartTime:     now,
		out:           os.Stdout,
		Parameters:    make(map[string]string),
		InitialState:  make(map[string]string),
		FinalState:    make(map[string]string),
		Performance:   make(map[string]string),
		InitialHealth: 0,
	}

	// Capture context
	s.captureContext()

	return s
}

// captureContext gathers execution environment
func (s *Session) captureContext() {
	// User info
	if u, err := user.Current(); err == nil {
		s.Username = u.Username
	} else {
		s.Username = "unknown"
	}

	// Hostname
	if h, err := os.Hostname(); err == nil {
		s.Hostname = h
	} else {
		s.Hostname = "unknown"
	}

	// Shell
	s.Shell = os.Getenv("SHELL")
	if s.Shell == "" {
		s.Shell = "/bin/sh"
	}

	// Working directory
	if cwd, err := os.Getwd(); err == nil {
		s.Cwd = cwd
	} else {
		s.Cwd = "unknown"
	}

	// System info
	s.Arch = runtime.GOARCH
	s.Kernel = runtime.GOOS

	// Try to get distro from /etc/os-release
	if data, err := os.ReadFile("/etc/os-release"); err == nil {
		lines := strings.Split(string(data), "\n")
		for _, line := range lines {
			if strings.HasPrefix(line, "PRETTY_NAME=") {
				s.Distro = strings.Trim(strings.TrimPrefix(line, "PRETTY_NAME="), "\"")
				break
			}
		}
	}
	if s.Distro == "" {
		s.Distro = runtime.GOOS
	}
}

// --- Session Configuration ---

// SetOutput changes the output destination
func (s *Session) SetOutput(w io.Writer) {
	s.out = w
}

// SetParameter sets an initial parameter
func (s *Session) SetParameter(key, value string) {
	s.Parameters[key] = value
}

// SetInitialState sets initial state key-value
func (s *Session) SetInitialState(key, value string) {
	s.InitialState[key] = value
}

// SetInitialHealth sets starting health
func (s *Session) SetInitialHealth(h int) {
	s.InitialHealth = clamp(h, 0, 100)
}

// --- Event Recording ---

// AddEvent records an event in the timeline
func (s *Session) AddEvent(severity Severity, eventType EventType, desc string, details map[string]string, healthBefore, healthAfter int) {
	if s.Level < LevelDetailed {
		return // BASIC level doesn't record events
	}

	s.Events = append(s.Events, Event{
		Timestamp:    time.Now(),
		Severity:     severity,
		Type:         eventType,
		Description:  desc,
		Details:      details,
		HealthBefore: healthBefore,
		HealthAfter:  healthAfter,
	})
}

// Info adds an INFO event
func (s *Session) Info(eventType EventType, desc string, details map[string]string, healthBefore, healthAfter int) {
	s.AddEvent(SeverityInfo, eventType, desc, details, healthBefore, healthAfter)
}

// Warn adds a WARN event
func (s *Session) Warn(eventType EventType, desc string, details map[string]string, healthBefore, healthAfter int) {
	s.AddEvent(SeverityWarn, eventType, desc, details, healthBefore, healthAfter)
}

// Error adds an ERROR event
func (s *Session) Error(eventType EventType, desc string, details map[string]string, healthBefore, healthAfter int) {
	s.AddEvent(SeverityError, eventType, desc, details, healthBefore, healthAfter)
}

// --- Analysis (VERBOSE only) ---

// AddIssue adds an issue to the analysis
func (s *Session) AddIssue(issue string) {
	if s.Level < LevelVerbose {
		return
	}
	s.Issues = append(s.Issues, issue)
}

// AddPerformance adds performance data
func (s *Session) AddPerformance(key, value string) {
	if s.Level < LevelVerbose {
		return
	}
	s.Performance[key] = value
}

// AddRecommendation adds a recommendation
func (s *Session) AddRecommendation(rec string) {
	if s.Level < LevelVerbose {
		return
	}
	s.Recommendations = append(s.Recommendations, rec)
}

// --- Session Completion ---

// End completes the session and writes output
func (s *Session) End(result Result, exitCode int, finalHealth int, finalState map[string]string) {
	s.EndTime = time.Now()
	s.Result = result
	s.ExitCode = exitCode
	s.FinalHealth = clamp(finalHealth, 0, 100)
	s.FinalState = finalState

	s.writeOutput()
}

// --- Output Formatting ---

func (s *Session) writeOutput() {
	var b strings.Builder

	// Header
	s.writeHeader(&b)

	// Context
	s.writeContext(&b)

	// Initial State
	s.writeInitialState(&b)

	// Events (DETAILED and VERBOSE)
	if s.Level >= LevelDetailed {
		s.writeEvents(&b)
	}

	// Final State
	s.writeFinalState(&b)

	// Analysis (VERBOSE only)
	if s.Level >= LevelVerbose {
		s.writeAnalysis(&b)
	}

	// Footer
	s.writeFooter(&b)

	fmt.Fprint(s.out, b.String())
}

func (s *Session) writeHeader(b *strings.Builder) {
	b.WriteString("# DEBUG_SESSION_START\n")
	b.WriteString("┌────────────────────────────────────────────────────────────────┐\n")
	b.WriteString("│ CPI-SI Debug Session                                          │\n")
	b.WriteString("├────────────────────────────────────────────────────────────────┤\n")
	fmt.Fprintf(b, "│ Component:  %-51s│\n", s.Component)
	fmt.Fprintf(b, "│ Context ID: %-51s│\n", truncate(s.ContextID, 51))
	fmt.Fprintf(b, "│ PID:        %-51d│\n", s.PID)
	fmt.Fprintf(b, "│ Level:      %-51s│\n", s.Level.String())
	fmt.Fprintf(b, "│ Started:    %-51s│\n", s.StartTime.Format("2006-01-02 15:04:05.000"))
	b.WriteString("└────────────────────────────────────────────────────────────────┘\n")
}

func (s *Session) writeContext(b *strings.Builder) {
	b.WriteString("\n# CONTEXT\n")
	b.WriteString("────────────────────────────────────────────────────────────────\n\n")
	fmt.Fprintf(b, "Identity: %s@%s:%d\n", s.Username, s.Hostname, s.PID)
	fmt.Fprintf(b, "Shell:    %s\n", s.Shell)
	fmt.Fprintf(b, "CWD:      %s\n\n", s.Cwd)
	fmt.Fprintf(b, "System:   %s (%s)\n", s.Distro, s.Arch)
	fmt.Fprintf(b, "Kernel:   %s\n\n", s.Kernel)
}

func (s *Session) writeInitialState(b *strings.Builder) {
	emoji, _ := healthInfo(s.InitialHealth)
	b.WriteString("# INITIAL_STATE\n")
	b.WriteString("────────────────────────────────────────────────────────────────\n\n")
	fmt.Fprintf(b, "Health: %d%% %s\n\n", s.InitialHealth, emoji)

	if len(s.Parameters) > 0 {
		b.WriteString("Parameters:\n")
		for k, v := range s.Parameters {
			fmt.Fprintf(b, "  %s: %s\n", k, v)
		}
		b.WriteString("\n")
	}

	if len(s.InitialState) > 0 {
		b.WriteString("Component State:\n")
		for k, v := range s.InitialState {
			fmt.Fprintf(b, "  %s: %s\n", k, v)
		}
		b.WriteString("\n")
	}
}

func (s *Session) writeEvents(b *strings.Builder) {
	b.WriteString("# EVENTS\n")
	b.WriteString("────────────────────────────────────────────────────────────────\n\n")

	if len(s.Events) == 0 {
		b.WriteString("(no events recorded)\n\n")
		return
	}

	for _, e := range s.Events {
		ts := e.Timestamp.Format("2006-01-02 15:04:05.000")
		fmt.Fprintf(b, "[%s] %s | %s | %s\n", ts, e.Severity, e.Type, e.Description)

		for k, v := range e.Details {
			fmt.Fprintf(b, "  %s: %s\n", k, v)
		}

		if e.HealthBefore != e.HealthAfter {
			delta := e.HealthAfter - e.HealthBefore
			fmt.Fprintf(b, "  HEALTH: %d%% → %d%% (Δ%+d)\n", e.HealthBefore, e.HealthAfter, delta)
		}
		b.WriteString("\n")
	}
}

func (s *Session) writeFinalState(b *strings.Builder) {
	emoji, assess := healthInfo(s.FinalHealth)
	delta := s.FinalHealth - s.InitialHealth
	duration := s.EndTime.Sub(s.StartTime)

	b.WriteString("# FINAL_STATE\n")
	b.WriteString("────────────────────────────────────────────────────────────────\n\n")
	fmt.Fprintf(b, "Result:   %s\n", s.Result)
	fmt.Fprintf(b, "Exit:     %d\n", s.ExitCode)
	fmt.Fprintf(b, "Duration: %s\n\n", formatDuration(duration))
	fmt.Fprintf(b, "Health:   %d%% %s\n", s.FinalHealth, emoji)
	fmt.Fprintf(b, "          Initial: %d%% → Final: %d%%\n", s.InitialHealth, s.FinalHealth)
	fmt.Fprintf(b, "          Delta: %+d (%s)\n\n", delta, assess)

	if len(s.FinalState) > 0 {
		b.WriteString("Component State:\n")
		for k, v := range s.FinalState {
			fmt.Fprintf(b, "  %s: %s\n", k, v)
		}
		b.WriteString("\n")
	}
}

func (s *Session) writeAnalysis(b *strings.Builder) {
	b.WriteString("# ANALYSIS\n")
	b.WriteString("────────────────────────────────────────────────────────────────\n\n")

	b.WriteString("Issues Detected:\n")
	if len(s.Issues) == 0 {
		b.WriteString("  (none)\n")
	} else {
		for _, issue := range s.Issues {
			fmt.Fprintf(b, "  - %s\n", issue)
		}
	}
	b.WriteString("\n")

	b.WriteString("Performance:\n")
	if len(s.Performance) == 0 {
		b.WriteString("  (not measured)\n")
	} else {
		for k, v := range s.Performance {
			fmt.Fprintf(b, "  %s: %s\n", k, v)
		}
	}
	b.WriteString("\n")

	b.WriteString("Recommendations:\n")
	if len(s.Recommendations) == 0 {
		b.WriteString("  (none)\n")
	} else {
		for _, rec := range s.Recommendations {
			fmt.Fprintf(b, "  - %s\n", rec)
		}
	}
	b.WriteString("\n")
}

func (s *Session) writeFooter(b *strings.Builder) {
	emoji, assess := healthInfo(s.FinalHealth)
	duration := s.EndTime.Sub(s.StartTime)

	b.WriteString("┌────────────────────────────────────────────────────────────────┐\n")
	b.WriteString("│ Session Complete                                              │\n")
	b.WriteString("├────────────────────────────────────────────────────────────────┤\n")
	fmt.Fprintf(b, "│ Context ID: %-51s│\n", truncate(s.ContextID, 51))
	fmt.Fprintf(b, "│ Duration:   %-51s│\n", formatDuration(duration))
	fmt.Fprintf(b, "│ Health:     %d%% %s %-43s│\n", s.FinalHealth, emoji, "("+assess+")")
	fmt.Fprintf(b, "│ Ended:      %-51s│\n", s.EndTime.Format("2006-01-02 15:04:05.000"))
	b.WriteString("└────────────────────────────────────────────────────────────────┘\n")
	b.WriteString("# DEBUG_SESSION_END\n")
}

// --- Level Methods ---

func (l Level) String() string {
	switch l {
	case LevelBasic:
		return "BASIC"
	case LevelDetailed:
		return "DETAILED"
	case LevelVerbose:
		return "VERBOSE"
	default:
		return "UNKNOWN"
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════

// --- Helpers ---

func healthInfo(h int) (emoji string, text string) {
	for _, hi := range healthEmoji {
		if h >= hi.min {
			return hi.emoji, hi.text
		}
	}
	return "☠️", "Failed"
}

func truncate(s string, max int) string {
	if len(s) <= max {
		return s
	}
	return s[:max-3] + "..."
}

func clamp(v, min, max int) int {
	if v < min {
		return min
	}
	if v > max {
		return max
	}
	return v
}

func formatDuration(d time.Duration) string {
	if d < time.Second {
		return fmt.Sprintf("%.3fs", d.Seconds())
	}
	if d < time.Minute {
		return fmt.Sprintf("%.2fs", d.Seconds())
	}
	return fmt.Sprintf("%dm %.1fs", int(d.Minutes()), d.Seconds()-float64(int(d.Minutes())*60))
}
