//omni:code --go -library
//omni:key B-L2-logging-logger
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L2-logging-logger
// Purpose: Generic structured logging — levels, modes, file I/O
// Biblical: Proverbs 4:26 - "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-16
//
// Platform-level structured logger usable by any layer.
// Health tracking and domain-specific categories belong in their own layers
// (L3 for CPI-SI health ternary scale, etc.).
//
// ============================================================================

package logging

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
	"sync"
	"time"
)

// ────────────────────────────────────────────────────────────────
// S.1 Types
// ────────────────────────────────────────────────────────────────

// Level represents log severity.
type Level int

const (
	DEBUG Level = iota
	INFO
	WARN
	ERROR
	FATAL
)

// Mode represents output format.
type Mode string

const (
	ModeCompact Mode = "compact"
	ModeVerbose Mode = "verbose"
)

// Entry represents a single log event.
type Entry struct {
	Timestamp time.Time
	Level     Level
	Component string
	Message   string
	Event     string            // Machine-readable event name
	Details   map[string]string // Additional key-value pairs
}

// Logger is a thread-safe structured logger.
type Logger struct {
	mu        sync.Mutex
	out       io.Writer
	mode      Mode
	level     Level
	component string
	logFile   *os.File // File handle for persistent logging
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Log Directory
// ────────────────────────────────────────────────────────────────

// LogDir returns the directory for persistent logs.
// Creates ~/.claude/logs/ if it doesn't exist.
func LogDir() string {
	home, err := os.UserHomeDir()
	if err != nil {
		return "/tmp/claude-logs"
	}
	dir := filepath.Join(home, ".claude", "logs")
	_ = os.MkdirAll(dir, 0755)
	return dir
}

// LogFilePath returns the path for today's log file.
// Format: ~/.claude/logs/YYYY-MM-DD.log
func LogFilePath() string {
	return filepath.Join(LogDir(), time.Now().Format("2006-01-02")+".log")
}

// ────────────────────────────────────────────────────────────────
// B.2 Constructors
// ────────────────────────────────────────────────────────────────

// New creates a logger with default settings (compact, INFO level).
func New(component string) *Logger {
	return &Logger{
		out:       os.Stdout,
		mode:      ModeCompact,
		level:     INFO,
		component: Truncate(component, 12),
	}
}

// NewWithOptions creates a logger with custom settings.
func NewWithOptions(component string, out io.Writer, mode Mode, level Level) *Logger {
	return &Logger{
		out:       out,
		mode:      mode,
		level:     level,
		component: Truncate(component, 12),
	}
}

// NewWithFile creates a logger that writes to both stdout and a file.
// File path: ~/.claude/logs/YYYY-MM-DD.log
// Non-fatal: if file open fails, continues with stdout only.
func NewWithFile(component string) *Logger {
	log := New(component)
	log.EnableFileLogging()
	return log
}

// ────────────────────────────────────────────────────────────────
// B.3 Configuration
// ────────────────────────────────────────────────────────────────

// SetOutput changes the output destination.
func (l *Logger) SetOutput(w io.Writer) {
	l.mu.Lock()
	defer l.mu.Unlock()
	l.out = w
}

// SetMode changes output format mode.
func (l *Logger) SetMode(m Mode) {
	l.mu.Lock()
	defer l.mu.Unlock()
	l.mode = m
}

// SetLevel changes minimum log level.
func (l *Logger) SetLevel(lv Level) {
	l.mu.Lock()
	defer l.mu.Unlock()
	l.level = lv
}

// GetLevel returns the current minimum log level.
func (l *Logger) GetLevel() Level {
	l.mu.Lock()
	defer l.mu.Unlock()
	return l.level
}

// GetMode returns the current output mode.
func (l *Logger) GetMode() Mode {
	l.mu.Lock()
	defer l.mu.Unlock()
	return l.mode
}

// Component returns the logger's component name.
func (l *Logger) Component() string {
	return l.component
}

// ────────────────────────────────────────────────────────────────
// B.4 File I/O
// ────────────────────────────────────────────────────────────────

// EnableFileLogging enables persistent logging to disk.
// Writes to both stdout and ~/.claude/logs/YYYY-MM-DD.log.
// Non-fatal: if file open fails, continues with stdout only.
func (l *Logger) EnableFileLogging() {
	l.mu.Lock()
	defer l.mu.Unlock()

	f, err := os.OpenFile(LogFilePath(), os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0644)
	if err != nil {
		return // Non-fatal
	}

	l.logFile = f
	l.out = io.MultiWriter(os.Stdout, f)
}

// Close releases file resources.
func (l *Logger) Close() error {
	l.mu.Lock()
	defer l.mu.Unlock()

	if l.logFile != nil {
		err := l.logFile.Close()
		l.logFile = nil
		l.out = os.Stdout
		return err
	}
	return nil
}

// ────────────────────────────────────────────────────────────────
// B.5 Logging Methods
// ────────────────────────────────────────────────────────────────

// Log writes an entry at the specified level.
func (l *Logger) Log(level Level, msg string, details map[string]string) {
	if level < l.level {
		return
	}

	l.mu.Lock()
	defer l.mu.Unlock()

	entry := Entry{
		Timestamp: time.Now(),
		Level:     level,
		Component: l.component,
		Message:   msg,
		Details:   details,
	}

	l.write(entry)
}

// LogEvent writes an entry with a machine-readable event name.
func (l *Logger) LogEvent(level Level, event, msg string, details map[string]string) {
	if level < l.level {
		return
	}

	l.mu.Lock()
	defer l.mu.Unlock()

	entry := Entry{
		Timestamp: time.Now(),
		Level:     level,
		Component: l.component,
		Message:   msg,
		Event:     event,
		Details:   details,
	}

	l.write(entry)
}

// Convenience methods.
func (l *Logger) Debug(msg string, details map[string]string) { l.Log(DEBUG, msg, details) }
func (l *Logger) Info(msg string, details map[string]string)  { l.Log(INFO, msg, details) }
func (l *Logger) Warn(msg string, details map[string]string)  { l.Log(WARN, msg, details) }
func (l *Logger) Error(msg string, details map[string]string) { l.Log(ERROR, msg, details) }
func (l *Logger) Fatal(msg string, details map[string]string) { l.Log(FATAL, msg, details) }

// ────────────────────────────────────────────────────────────────
// B.6 Formatting
// ────────────────────────────────────────────────────────────────

// write outputs the entry in configured format.
func (l *Logger) write(e Entry) {
	var output string

	switch l.mode {
	case ModeVerbose:
		output = l.FormatVerbose(e)
	default:
		output = l.FormatCompact(e)
	}

	// Never block — ignore write errors
	fmt.Fprint(l.out, output)
}

// FormatCompact produces single-line grep-friendly output.
// Format: [timestamp] LEVEL | component | message
func (l *Logger) FormatCompact(e Entry) string {
	ts := e.Timestamp.Format("2006-01-02 15:04:05")
	lvl := PadRight(e.Level.String(), 5)
	comp := PadRight(e.Component, 12)

	return fmt.Sprintf("[%s] %s | %s | %s\n",
		ts, lvl, comp, Truncate(e.Message, 80))
}

// FormatVerbose produces multi-line output with full context.
func (l *Logger) FormatVerbose(e Entry) string {
	var b strings.Builder

	ts := e.Timestamp.Format("2006-01-02 15:04:05.000")

	// Header line
	fmt.Fprintf(&b, "[%s] %s | %s | %s:%d\n",
		ts, e.Level.String(), e.Component, Hostname(), os.Getpid())

	// Event (if set)
	if e.Event != "" {
		fmt.Fprintf(&b, "  EVENT: %s\n", e.Event)
	}

	// Message
	fmt.Fprintf(&b, "  MESSAGE: %s\n", e.Message)

	// Details
	if len(e.Details) > 0 {
		fmt.Fprintf(&b, "  DETAILS:\n")
		for k, v := range e.Details {
			fmt.Fprintf(&b, "    %s: %s\n", k, v)
		}
	}

	// Entry delimiter
	fmt.Fprintf(&b, "---\n")

	return b.String()
}

// ────────────────────────────────────────────────────────────────
// B.7 Level Methods
// ────────────────────────────────────────────────────────────────

// String returns the level name.
func (l Level) String() string {
	switch l {
	case DEBUG:
		return "DEBUG"
	case INFO:
		return "INFO"
	case WARN:
		return "WARN"
	case ERROR:
		return "ERROR"
	case FATAL:
		return "FATAL"
	default:
		return "UNKN"
	}
}

// ParseLevel converts a string to a Level.
func ParseLevel(s string) Level {
	switch strings.ToUpper(s) {
	case "DEBUG":
		return DEBUG
	case "INFO":
		return INFO
	case "WARN", "WARNING":
		return WARN
	case "ERROR":
		return ERROR
	case "FATAL":
		return FATAL
	default:
		return INFO
	}
}

// ────────────────────────────────────────────────────────────────
// B.8 Helpers (exported for layer extensions)
// ────────────────────────────────────────────────────────────────

// Truncate shortens a string with ellipsis.
func Truncate(s string, max int) string {
	if len(s) <= max {
		return s
	}
	return s[:max-1] + "…"
}

// PadRight pads a string with spaces to the given width.
func PadRight(s string, width int) string {
	if len(s) >= width {
		return s
	}
	return s + strings.Repeat(" ", width-len(s))
}

// Clamp constrains a value between min and max.
func Clamp(v, min, max int) int {
	if v < min {
		return min
	}
	if v > max {
		return max
	}
	return v
}

// Hostname returns the system hostname.
func Hostname() string {
	h, err := os.Hostname()
	if err != nil {
		return "unknown"
	}
	return h
}

// ============================================================================
// CLOSING
// ============================================================================
// Platform logging infrastructure — generic structured logger.
// Health tracking and domain-specific categories belong in their layers.
// "Ponder the path of thy feet" — Proverbs 4:26
