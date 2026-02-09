// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-logging
// Purpose: Structured logging for CPI-SI - Detection layer of immune system
// Biblical: Proverbs 4:26 - "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-10
// Updated: 2025-12-21 - Config-driven using health loader
//
// Design Goals:
//   - Parsable AND readable output
//   - Health scoring with visual indicators (from TOML config)
//   - Compact (grep-friendly) and verbose modes
//   - Never blocks execution (fails gracefully)
//
// Config-Driven Design:
//   - ANCHOR: pkg/foundation/schema/config/l2/health/*.toml
//   - FRAMEWORK: pkg/health/loader.go
//   - Emoji, levels, ternary scale all from config
//
// ═══════════════════════════════════════════════════════════════════════════

package logging

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
	"sync"
	"time"

	"cws.studio/pkg/core/health"
)

// --- Constants ---

// Log levels
const (
	DEBUG Level = iota
	INFO
	WARN
	ERROR
	FATAL
)

// Mode constants
const (
	ModeCompact  Mode = "compact"
	ModeVerbose  Mode = "verbose"
)

// Ternary scale constants (from health.TernaryMin/Center/Max)
// These match the values in score.toml
const (
	HealthMin    = health.TernaryMin    // -100
	HealthCenter = health.TernaryCenter // 0 (even balance)
	HealthMax    = health.TernaryMax    // +100
)

// --- Types ---

// Level represents log severity
type Level int

// Mode represents output format mode
type Mode string

// Entry represents a single log entry
type Entry struct {
	Timestamp time.Time
	Level     Level
	Component string
	Message   string
	Event     string            // Machine-readable event name
	Health    int               // Ternary scale: -100 to +100 (0 = even balance)
	Delta     int               // Health change (+1 success, -1 failure)
	HealthLvl string            // Level name (broken, wanting, lacking, even, sound, whole, perfect)
	Details   map[string]string // Additional key-value pairs
}

// Logger handles structured logging
type Logger struct {
	mu           sync.Mutex
	out          io.Writer
	mode         Mode
	level        Level
	component    string
	health       int            // Current health state (ternary: -100 to +100)
	healthConfig *health.HealthConfig // Loaded from TOML
	logFile      *os.File       // File handle for persistent logging (nil if not enabled)
}

// --- Log Directory ---

// LogDir returns the directory for persistent logs
// Creates ~/.claude/logs/ if it doesn't exist
func LogDir() string {
	home, err := os.UserHomeDir()
	if err != nil {
		return "/tmp/claude-logs"
	}
	dir := filepath.Join(home, ".claude", "logs")
	_ = os.MkdirAll(dir, 0755)
	return dir
}

// LogFilePath returns the path for today's log file
// Format: ~/.claude/logs/YYYY-MM-DD.log
func LogFilePath() string {
	return filepath.Join(LogDir(), time.Now().Format("2006-01-02")+".log")
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Logger Construction ---

// New creates a logger with default settings
// Health starts at 0 (even balance) per ternary system
func New(component string) *Logger {
	cfg, _ := health.Config() // Load from TOML, ignore error (use defaults)
	return &Logger{
		out:          os.Stdout,
		mode:         ModeCompact,
		level:        INFO,
		component:    truncate(component, 12),
		health:       HealthCenter, // 0 = even balance (not 100)
		healthConfig: cfg,
	}
}

// NewWithOptions creates a logger with custom settings
// Health starts at 0 (even balance) per ternary system
func NewWithOptions(component string, out io.Writer, mode Mode, level Level) *Logger {
	cfg, _ := health.Config()
	return &Logger{
		out:          out,
		mode:         mode,
		level:        level,
		component:    truncate(component, 12),
		health:       HealthCenter, // 0 = even balance
		healthConfig: cfg,
	}
}

// NewWithHealth creates a logger with custom initial health
func NewWithHealth(component string, initialHealth int) *Logger {
	cfg, _ := health.Config()
	return &Logger{
		out:          os.Stdout,
		mode:         ModeCompact,
		level:        INFO,
		component:    truncate(component, 12),
		health:       clamp(initialHealth, HealthMin, HealthMax),
		healthConfig: cfg,
	}
}

// NewWithFile creates a logger that writes to both stdout and a file
// File path: ~/.claude/logs/YYYY-MM-DD.log
// Returns logger (always usable) - file errors are non-fatal
func NewWithFile(component string) *Logger {
	log := New(component)
	log.EnableFileLogging()
	return log
}

// EnableFileLogging enables persistent logging to disk
// Writes to both stdout and ~/.claude/logs/YYYY-MM-DD.log
// Non-fatal: if file open fails, continues with stdout only
func (l *Logger) EnableFileLogging() {
	l.mu.Lock()
	defer l.mu.Unlock()

	// Open log file in append mode
	f, err := os.OpenFile(LogFilePath(), os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0644)
	if err != nil {
		// Non-fatal: continue with stdout only
		return
	}

	l.logFile = f
	l.out = io.MultiWriter(os.Stdout, f)
}

// Close releases file resources
// Call this when done with a logger that has file logging enabled
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

// --- Configuration ---

// SetOutput changes the output destination
func (l *Logger) SetOutput(w io.Writer) {
	l.mu.Lock()
	defer l.mu.Unlock()
	l.out = w
}

// SetMode changes output format mode
func (l *Logger) SetMode(m Mode) {
	l.mu.Lock()
	defer l.mu.Unlock()
	l.mode = m
}

// SetLevel changes minimum log level
func (l *Logger) SetLevel(lv Level) {
	l.mu.Lock()
	defer l.mu.Unlock()
	l.level = lv
}

// SetHealth sets current health state (ternary scale: -100 to +100)
func (l *Logger) SetHealth(h int) {
	l.mu.Lock()
	defer l.mu.Unlock()
	l.health = clamp(h, HealthMin, HealthMax)
}

// AdjustHealth modifies health by delta and returns new value
// Use +1 for success (narrow way), -1 for failure (broad way)
func (l *Logger) AdjustHealth(delta int) int {
	l.mu.Lock()
	defer l.mu.Unlock()
	l.health = clamp(l.health+delta, HealthMin, HealthMax)
	return l.health
}

// GetHealthLevel returns the current health level name
func (l *Logger) GetHealthLevel() string {
	l.mu.Lock()
	defer l.mu.Unlock()
	if l.healthConfig != nil {
		return l.healthConfig.LevelFromTrueScore(l.health)
	}
	return levelFromTrueScore(l.health)
}

// GetHealthEmoji returns the emoji for current health
func (l *Logger) GetHealthEmoji() string {
	l.mu.Lock()
	defer l.mu.Unlock()
	if l.healthConfig != nil {
		return l.healthConfig.EmojiFromTrueScore(l.health)
	}
	return emojiFromTrueScore(l.health)
}

// --- Logging Methods ---

// Log writes an entry at specified level
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
		Health:    l.health,
		Details:   details,
	}

	l.write(entry)
}

// LogWithHealth writes an entry and adjusts health
// Delta should be +1 for success (narrow way), -1 for failure (broad way)
func (l *Logger) LogWithHealth(level Level, msg string, delta int, details map[string]string) {
	if level < l.level {
		return
	}

	l.mu.Lock()
	defer l.mu.Unlock()

	oldHealth := l.health
	l.health = clamp(l.health+delta, HealthMin, HealthMax)

	// Get level name from config or fallback
	var lvlName string
	if l.healthConfig != nil {
		lvlName = l.healthConfig.LevelFromTrueScore(l.health)
	} else {
		lvlName = levelFromTrueScore(l.health)
	}

	entry := Entry{
		Timestamp: time.Now(),
		Level:     level,
		Component: l.component,
		Message:   msg,
		Health:    l.health,
		Delta:     l.health - oldHealth,
		HealthLvl: lvlName,
		Details:   details,
	}

	l.write(entry)
}

// LogSuccess logs a success with +1 health delta (narrow way)
func (l *Logger) LogSuccess(msg string, details map[string]string) {
	delta := 1
	if l.healthConfig != nil {
		delta = l.healthConfig.SuccessValue()
	}
	l.LogWithHealth(INFO, msg, delta, details)
}

// LogFailure logs a failure with -1 health delta (broad way)
func (l *Logger) LogFailure(msg string, details map[string]string) {
	delta := -1
	if l.healthConfig != nil {
		delta = l.healthConfig.FailureValue()
	}
	l.LogWithHealth(ERROR, msg, delta, details)
}

// Convenience methods
func (l *Logger) Debug(msg string, details map[string]string) { l.Log(DEBUG, msg, details) }
func (l *Logger) Info(msg string, details map[string]string)  { l.Log(INFO, msg, details) }
func (l *Logger) Warn(msg string, details map[string]string)  { l.Log(WARN, msg, details) }
func (l *Logger) Error(msg string, details map[string]string) { l.Log(ERROR, msg, details) }
func (l *Logger) Fatal(msg string, details map[string]string) { l.Log(FATAL, msg, details) }

// --- Formatting ---

// write outputs the entry in configured format
func (l *Logger) write(e Entry) {
	var output string

	switch l.mode {
	case ModeVerbose:
		output = l.formatVerbose(e)
	default:
		output = l.formatCompact(e)
	}

	// Never block - ignore write errors
	fmt.Fprint(l.out, output)
}

// formatCompact produces single-line grep-friendly output
// Format: [timestamp] LEVEL | component | message (health emoji level)
func (l *Logger) formatCompact(e Entry) string {
	ts := e.Timestamp.Format("2006-01-02 15:04:05")
	lvl := padRight(e.Level.String(), 5)
	comp := padRight(e.Component, 12)

	// Get emoji from config or fallback
	var emoji string
	if l.healthConfig != nil {
		emoji = l.healthConfig.EmojiFromTrueScore(e.Health)
	} else {
		emoji = emojiFromTrueScore(e.Health)
	}

	// Show health with sign for ternary clarity
	healthStr := fmt.Sprintf("%+d", e.Health)

	return fmt.Sprintf("[%s] %s | %s | %s (%s %s)\n",
		ts, lvl, comp, truncate(e.Message, 60), emoji, healthStr)
}

// formatVerbose produces multi-line output with full context
func (l *Logger) formatVerbose(e Entry) string {
	var b strings.Builder

	ts := e.Timestamp.Format("2006-01-02 15:04:05.000")

	// Get emoji from config or fallback
	var emoji string
	if l.healthConfig != nil {
		emoji = l.healthConfig.EmojiFromTrueScore(e.Health)
	} else {
		emoji = emojiFromTrueScore(e.Health)
	}

	// Header line
	fmt.Fprintf(&b, "[%s] %s | %s | %s:%d\n",
		ts, e.Level.String(), e.Component, hostname(), os.Getpid())

	// Event (if set)
	if e.Event != "" {
		fmt.Fprintf(&b, "  EVENT: %s\n", e.Event)
	}

	// Health line - show true score with sign for ternary clarity
	if e.Delta != 0 {
		fmt.Fprintf(&b, "  HEALTH: %+d (Δ%+d) %s\n", e.Health, e.Delta, emoji)
	} else {
		fmt.Fprintf(&b, "  HEALTH: %+d %s\n", e.Health, emoji)
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

// --- Level Methods ---

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

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════

// --- Helpers ---

// levelFromTrueScore returns health level name from ternary score
// Fallback when config not available (matches score.toml)
func levelFromTrueScore(trueScore int) string {
	switch {
	case trueScore <= -67:
		return "broken"
	case trueScore <= -34:
		return "wanting"
	case trueScore <= -1:
		return "lacking"
	case trueScore == 0:
		return "even"
	case trueScore <= 33:
		return "sound"
	case trueScore <= 66:
		return "whole"
	default:
		return "perfect"
	}
}

// emojiFromTrueScore returns emoji for ternary score
// Fallback when config not available (matches score.toml display)
func emojiFromTrueScore(trueScore int) string {
	switch levelFromTrueScore(trueScore) {
	case "broken":
		return "💔"
	case "wanting":
		return "🩹"
	case "lacking":
		return "💛"
	case "even":
		return "⚖️"
	case "sound":
		return "💚"
	case "whole":
		return "💙"
	case "perfect":
		return "💜"
	default:
		return "❓"
	}
}

func truncate(s string, max int) string {
	if len(s) <= max {
		return s
	}
	return s[:max-1] + "…"
}

func padRight(s string, width int) string {
	if len(s) >= width {
		return s
	}
	return s + strings.Repeat(" ", width-len(s))
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

func hostname() string {
	h, err := os.Hostname()
	if err != nil {
		return "unknown"
	}
	return h
}
