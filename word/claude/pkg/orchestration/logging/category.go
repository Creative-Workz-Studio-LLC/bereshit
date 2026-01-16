// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-logging-category
// Purpose: Category-aware logging with multi-format output (.log + .jsonl)
// Biblical: Proverbs 4:26 - "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-23
//
// Design Goals:
//   - Route logs to organized category directories
//   - Write both human-readable (.log) and machine-parseable (.jsonl)
//   - Integrate with claude-global/data/logs/ structure
//   - Build on existing Logger foundation
//
// ═══════════════════════════════════════════════════════════════════════════

package logging

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/json"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"sync"
	"time"

	"github.com/creativeworkzstudio/claude-global/pkg/util/fs/paths"
)

// --- Category Constants ---

// Category represents a log category for routing
type Category string

const (
	CategorySession    Category = "session"
	CategoryTrajectory Category = "trajectory"
	CategoryCognition  Category = "cognition"
	CategoryHealth     Category = "health"
	CategoryTools      Category = "tools"
	CategoryContext    Category = "context"
)

// --- CategoryLogger ---

// CategoryLogger routes logs to organized directories with multi-format output
type CategoryLogger struct {
	mu        sync.Mutex
	category  Category
	sessionID string
	sequence  int
	logFile   *os.File
	jsonlFile *os.File
	logger    *Logger // Embedded base logger for formatting
}

// JSONLEntry represents a single JSONL log entry
type JSONLEntry struct {
	Timestamp   string            `json:"timestamp"`
	Type        string            `json:"type"`
	SessionID   string            `json:"session_id"`
	Sequence    int               `json:"sequence"`
	Level       string            `json:"level"`
	Component   string            `json:"component"`
	Message     string            `json:"message"`
	Health      int               `json:"health,omitempty"`
	Delta       int               `json:"delta,omitempty"`
	HealthLevel string            `json:"health_level,omitempty"`
	Details     map[string]string `json:"details,omitempty"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Category Directory Resolution ---

// categoryDir returns the directory path for a category
func categoryDir(cat Category) string {
	switch cat {
	case CategorySession:
		return paths.LogsSessionDir()
	case CategoryTrajectory:
		return paths.LogsTrajectoryDir()
	case CategoryCognition:
		return paths.LogsCognitionDir()
	case CategoryHealth:
		return paths.LogsHealthDir()
	case CategoryTools:
		return paths.LogsToolsDir()
	case CategoryContext:
		return paths.LogsContextDir()
	default:
		return paths.LogsDir()
	}
}

// --- Constructor ---

// NewCategoryLogger creates a logger for a specific category
// Automatically creates both .log and .jsonl files in the category directory
func NewCategoryLogger(category Category, sessionID string) (*CategoryLogger, error) {
	// Ensure directory exists
	dir := categoryDir(category)
	if err := os.MkdirAll(dir, 0755); err != nil {
		return nil, fmt.Errorf("failed to create log directory: %w", err)
	}

	// Create file paths for today
	date := time.Now().Format("2006-01-02")
	logPath := filepath.Join(dir, date+".log")
	jsonlPath := filepath.Join(dir, date+".jsonl")

	// Open log file (append mode)
	logFile, err := os.OpenFile(logPath, os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0644)
	if err != nil {
		return nil, fmt.Errorf("failed to open log file: %w", err)
	}

	// Open jsonl file (append mode)
	jsonlFile, err := os.OpenFile(jsonlPath, os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0644)
	if err != nil {
		logFile.Close()
		return nil, fmt.Errorf("failed to open jsonl file: %w", err)
	}

	// Create base logger for formatting
	logger := New(string(category))

	return &CategoryLogger{
		category:  category,
		sessionID: sessionID,
		sequence:  0,
		logFile:   logFile,
		jsonlFile: jsonlFile,
		logger:    logger,
	}, nil
}

// --- Logging Methods ---

// Log writes an entry to both .log and .jsonl files
func (cl *CategoryLogger) Log(level Level, eventType, message string, details map[string]string) {
	cl.mu.Lock()
	defer cl.mu.Unlock()

	cl.sequence++
	now := time.Now()

	// Create entry
	entry := Entry{
		Timestamp: now,
		Level:     level,
		Component: string(cl.category),
		Message:   message,
		Event:     eventType,
		Health:    cl.logger.health,
		Details:   details,
	}

	// Write to .log file (human-readable)
	cl.writeLog(entry)

	// Write to .jsonl file (machine-parseable)
	cl.writeJSONL(entry, eventType)
}

// LogWithHealth writes an entry with health delta
func (cl *CategoryLogger) LogWithHealth(level Level, eventType, message string, delta int, details map[string]string) {
	cl.mu.Lock()
	defer cl.mu.Unlock()

	cl.sequence++
	now := time.Now()

	oldHealth := cl.logger.health
	cl.logger.health = clamp(cl.logger.health+delta, HealthMin, HealthMax)

	// Get level name
	var lvlName string
	if cl.logger.healthConfig != nil {
		lvlName = cl.logger.healthConfig.LevelFromTrueScore(cl.logger.health)
	} else {
		lvlName = levelFromTrueScore(cl.logger.health)
	}

	entry := Entry{
		Timestamp: now,
		Level:     level,
		Component: string(cl.category),
		Message:   message,
		Event:     eventType,
		Health:    cl.logger.health,
		Delta:     cl.logger.health - oldHealth,
		HealthLvl: lvlName,
		Details:   details,
	}

	// Write to both formats
	cl.writeLog(entry)
	cl.writeJSONL(entry, eventType)
}

// --- Format Writers ---

// writeLog writes human-readable format to .log file
func (cl *CategoryLogger) writeLog(e Entry) {
	ts := e.Timestamp.Format("2006-01-02 15:04:05")
	lvl := padRight(e.Level.String(), 5)
	comp := padRight(e.Component, 12)

	var emoji string
	if cl.logger.healthConfig != nil {
		emoji = cl.logger.healthConfig.EmojiFromTrueScore(e.Health)
	} else {
		emoji = emojiFromTrueScore(e.Health)
	}

	healthStr := fmt.Sprintf("%+d", e.Health)
	line := fmt.Sprintf("[%s] %s | %s | %s (%s %s)\n",
		ts, lvl, comp, truncate(e.Message, 60), emoji, healthStr)

	// Non-blocking write
	_, _ = io.WriteString(cl.logFile, line)
}

// writeJSONL writes machine-parseable format to .jsonl file
func (cl *CategoryLogger) writeJSONL(e Entry, eventType string) {
	jsonEntry := JSONLEntry{
		Timestamp:   e.Timestamp.Format(time.RFC3339),
		Type:        eventType,
		SessionID:   cl.sessionID,
		Sequence:    cl.sequence,
		Level:       e.Level.String(),
		Component:   e.Component,
		Message:     e.Message,
		Health:      e.Health,
		Delta:       e.Delta,
		HealthLevel: e.HealthLvl,
		Details:     e.Details,
	}

	data, err := json.Marshal(jsonEntry)
	if err != nil {
		return // Non-fatal
	}

	// Write line + newline
	_, _ = cl.jsonlFile.Write(data)
	_, _ = cl.jsonlFile.WriteString("\n")
}

// --- Convenience Methods ---

// Info logs at INFO level
func (cl *CategoryLogger) Info(eventType, message string, details map[string]string) {
	cl.Log(INFO, eventType, message, details)
}

// Warn logs at WARN level
func (cl *CategoryLogger) Warn(eventType, message string, details map[string]string) {
	cl.Log(WARN, eventType, message, details)
}

// Error logs at ERROR level
func (cl *CategoryLogger) Error(eventType, message string, details map[string]string) {
	cl.Log(ERROR, eventType, message, details)
}

// Debug logs at DEBUG level
func (cl *CategoryLogger) Debug(eventType, message string, details map[string]string) {
	cl.Log(DEBUG, eventType, message, details)
}

// Success logs a success with +1 health
func (cl *CategoryLogger) Success(eventType, message string, details map[string]string) {
	cl.LogWithHealth(INFO, eventType, message, 1, details)
}

// Failure logs a failure with -1 health
func (cl *CategoryLogger) Failure(eventType, message string, details map[string]string) {
	cl.LogWithHealth(ERROR, eventType, message, -1, details)
}

// --- Resource Management ---

// Close releases file resources
func (cl *CategoryLogger) Close() error {
	cl.mu.Lock()
	defer cl.mu.Unlock()

	var errs []error
	if cl.logFile != nil {
		if err := cl.logFile.Close(); err != nil {
			errs = append(errs, err)
		}
		cl.logFile = nil
	}
	if cl.jsonlFile != nil {
		if err := cl.jsonlFile.Close(); err != nil {
			errs = append(errs, err)
		}
		cl.jsonlFile = nil
	}

	if len(errs) > 0 {
		return errs[0]
	}
	return nil
}

// --- Session-Wide Logging ---

// SessionLoggers holds all category loggers for a session
type SessionLoggers struct {
	mu        sync.Mutex
	sessionID string
	loggers   map[Category]*CategoryLogger
}

// NewSessionLoggers creates loggers for all categories
func NewSessionLoggers(sessionID string) (*SessionLoggers, error) {
	// Ensure all directories exist
	if err := paths.EnsureLogsDir(); err != nil {
		return nil, fmt.Errorf("failed to create log directories: %w", err)
	}

	sl := &SessionLoggers{
		sessionID: sessionID,
		loggers:   make(map[Category]*CategoryLogger),
	}

	// Create loggers for each category
	categories := []Category{
		CategorySession,
		CategoryTrajectory,
		CategoryCognition,
		CategoryHealth,
		CategoryTools,
		CategoryContext,
	}

	for _, cat := range categories {
		logger, err := NewCategoryLogger(cat, sessionID)
		if err != nil {
			// Close any already-opened loggers
			sl.Close()
			return nil, fmt.Errorf("failed to create %s logger: %w", cat, err)
		}
		sl.loggers[cat] = logger
	}

	return sl, nil
}

// Get returns the logger for a specific category
func (sl *SessionLoggers) Get(cat Category) *CategoryLogger {
	sl.mu.Lock()
	defer sl.mu.Unlock()
	return sl.loggers[cat]
}

// Session returns the session category logger
func (sl *SessionLoggers) Session() *CategoryLogger {
	return sl.Get(CategorySession)
}

// Trajectory returns the trajectory category logger
func (sl *SessionLoggers) Trajectory() *CategoryLogger {
	return sl.Get(CategoryTrajectory)
}

// Cognition returns the cognition category logger
func (sl *SessionLoggers) Cognition() *CategoryLogger {
	return sl.Get(CategoryCognition)
}

// Health returns the health category logger
func (sl *SessionLoggers) Health() *CategoryLogger {
	return sl.Get(CategoryHealth)
}

// Tools returns the tools category logger
func (sl *SessionLoggers) Tools() *CategoryLogger {
	return sl.Get(CategoryTools)
}

// Context returns the context category logger
func (sl *SessionLoggers) Context() *CategoryLogger {
	return sl.Get(CategoryContext)
}

// Close closes all category loggers
func (sl *SessionLoggers) Close() error {
	sl.mu.Lock()
	defer sl.mu.Unlock()

	var firstErr error
	for _, logger := range sl.loggers {
		if err := logger.Close(); err != nil && firstErr == nil {
			firstErr = err
		}
	}
	sl.loggers = make(map[Category]*CategoryLogger)
	return firstErr
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// Usage:
//
//   // Create session loggers at session start
//   loggers, err := logging.NewSessionLoggers(sessionID)
//   if err != nil {
//       // Handle error
//   }
//   defer loggers.Close()
//
//   // Log to specific categories
//   loggers.Session().Info("session_start", "Session initialized", nil)
//   loggers.Trajectory().Info("trajectory_transition", "B.1 → B.2", map[string]string{
//       "from": "B.1",
//       "to": "B.2",
//   })
//   loggers.Tools().Success("tool_complete", "Edit completed", map[string]string{
//       "tool": "Edit",
//       "file": "/path/to/file.go",
//   })
//
// Files created:
//   claude-global/data/logs/session/2025-12-23.log
//   claude-global/data/logs/session/2025-12-23.jsonl
//   claude-global/data/logs/trajectory/2025-12-23.log
//   claude-global/data/logs/trajectory/2025-12-23.jsonl
//   ... etc for each category
//
