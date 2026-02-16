//omni:code --go -library
//omni:key B-L2-logging-category
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L2-logging-category
// Purpose: Category-aware logging with multi-format output (.log + .jsonl)
// Biblical: Proverbs 4:26 - "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-16
//
// Generic category logger infrastructure — routes logs to named directories
// with both human-readable (.log) and machine-parseable (.jsonl) output.
//
// Specific categories (session, cognition, health, etc.) are defined by
// consuming layers, not here. This package provides the machinery.
//
// ============================================================================

package logging

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"sync"
	"time"
)

// ────────────────────────────────────────────────────────────────
// S.1 Types
// ────────────────────────────────────────────────────────────────

// CategoryLogger routes logs to category-specific files.
// Each category gets both .log (human) and .jsonl (machine) output.
type CategoryLogger struct {
	mu        sync.Mutex
	category  string
	sessionID string
	sequence  int
	logFile   *os.File
	jsonlFile *os.File
	logger    *Logger // Embedded base logger for formatting
}

// JSONLEntry represents a single JSONL log line.
type JSONLEntry struct {
	Timestamp string            `json:"timestamp"`
	Type      string            `json:"type"`
	SessionID string            `json:"session_id"`
	Sequence  int               `json:"sequence"`
	Level     string            `json:"level"`
	Component string            `json:"component"`
	Message   string            `json:"message"`
	Details   map[string]string `json:"details,omitempty"`
}

// SessionLoggers manages all category loggers for a session.
type SessionLoggers struct {
	mu        sync.Mutex
	sessionID string
	loggers   map[string]*CategoryLogger
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 CategoryLogger Constructor
// ────────────────────────────────────────────────────────────────

// NewCategoryLogger creates a logger for a specific category directory.
// Automatically creates both .log and .jsonl files in the given directory.
func NewCategoryLogger(category, sessionID, dir string) (*CategoryLogger, error) {
	// Ensure directory exists
	if err := os.MkdirAll(dir, 0755); err != nil {
		return nil, fmt.Errorf("create log directory %s: %w", dir, err)
	}

	// Create file paths for today
	date := time.Now().Format("2006-01-02")
	logPath := filepath.Join(dir, date+".log")
	jsonlPath := filepath.Join(dir, date+".jsonl")

	// Open log file (append mode)
	logFile, err := os.OpenFile(logPath, os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0644)
	if err != nil {
		return nil, fmt.Errorf("open log file %s: %w", logPath, err)
	}

	// Open jsonl file (append mode)
	jsonlFile, err := os.OpenFile(jsonlPath, os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0644)
	if err != nil {
		logFile.Close()
		return nil, fmt.Errorf("open jsonl file %s: %w", jsonlPath, err)
	}

	// Create base logger for formatting
	logger := New(category)

	return &CategoryLogger{
		category:  category,
		sessionID: sessionID,
		sequence:  0,
		logFile:   logFile,
		jsonlFile: jsonlFile,
		logger:    logger,
	}, nil
}

// ────────────────────────────────────────────────────────────────
// B.2 CategoryLogger Logging
// ────────────────────────────────────────────────────────────────

// Log writes an entry to both .log and .jsonl files.
func (cl *CategoryLogger) Log(level Level, eventType, message string, details map[string]string) {
	cl.mu.Lock()
	defer cl.mu.Unlock()

	cl.sequence++
	now := time.Now()

	entry := Entry{
		Timestamp: now,
		Level:     level,
		Component: cl.category,
		Message:   message,
		Event:     eventType,
		Details:   details,
	}

	cl.writeLog(entry)
	cl.writeJSONL(entry, eventType)
}

// Convenience methods.
func (cl *CategoryLogger) Info(eventType, msg string, details map[string]string) {
	cl.Log(INFO, eventType, msg, details)
}
func (cl *CategoryLogger) Warn(eventType, msg string, details map[string]string) {
	cl.Log(WARN, eventType, msg, details)
}
func (cl *CategoryLogger) Error(eventType, msg string, details map[string]string) {
	cl.Log(ERROR, eventType, msg, details)
}
func (cl *CategoryLogger) Debug(eventType, msg string, details map[string]string) {
	cl.Log(DEBUG, eventType, msg, details)
}

// Category returns this logger's category name.
func (cl *CategoryLogger) Category() string {
	return cl.category
}

// ────────────────────────────────────────────────────────────────
// B.3 Format Writers
// ────────────────────────────────────────────────────────────────

// writeLog writes human-readable format to .log file.
func (cl *CategoryLogger) writeLog(e Entry) {
	ts := e.Timestamp.Format("2006-01-02 15:04:05")
	lvl := PadRight(e.Level.String(), 5)
	comp := PadRight(e.Component, 12)

	line := fmt.Sprintf("[%s] %s | %s | %s\n",
		ts, lvl, comp, Truncate(e.Message, 60))

	_, _ = io.WriteString(cl.logFile, line)
}

// writeJSONL writes machine-parseable format to .jsonl file.
func (cl *CategoryLogger) writeJSONL(e Entry, eventType string) {
	jsonEntry := JSONLEntry{
		Timestamp: e.Timestamp.Format(time.RFC3339),
		Type:      eventType,
		SessionID: cl.sessionID,
		Sequence:  cl.sequence,
		Level:     e.Level.String(),
		Component: e.Component,
		Message:   e.Message,
		Details:   e.Details,
	}

	data, err := json.Marshal(jsonEntry)
	if err != nil {
		return // Non-fatal
	}

	_, _ = cl.jsonlFile.Write(data)
	_, _ = cl.jsonlFile.WriteString("\n")
}

// ────────────────────────────────────────────────────────────────
// B.4 Resource Management
// ────────────────────────────────────────────────────────────────

// Close releases file resources.
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

// ────────────────────────────────────────────────────────────────
// B.5 SessionLoggers
// ────────────────────────────────────────────────────────────────

// NewSessionLoggers creates a SessionLoggers manager.
// Consumers register their own categories using Register().
func NewSessionLoggers(sessionID string) *SessionLoggers {
	return &SessionLoggers{
		sessionID: sessionID,
		loggers:   make(map[string]*CategoryLogger),
	}
}

// Register adds a category logger to the session.
func (sl *SessionLoggers) Register(category string, cl *CategoryLogger) {
	sl.mu.Lock()
	defer sl.mu.Unlock()
	sl.loggers[category] = cl
}

// Get returns the logger for a specific category.
func (sl *SessionLoggers) Get(category string) *CategoryLogger {
	sl.mu.Lock()
	defer sl.mu.Unlock()
	return sl.loggers[category]
}

// Categories returns all registered category names.
func (sl *SessionLoggers) Categories() []string {
	sl.mu.Lock()
	defer sl.mu.Unlock()
	cats := make([]string, 0, len(sl.loggers))
	for cat := range sl.loggers {
		cats = append(cats, cat)
	}
	return cats
}

// Close closes all category loggers.
func (sl *SessionLoggers) Close() error {
	sl.mu.Lock()
	defer sl.mu.Unlock()

	var firstErr error
	for _, logger := range sl.loggers {
		if err := logger.Close(); err != nil && firstErr == nil {
			firstErr = err
		}
	}
	sl.loggers = make(map[string]*CategoryLogger)
	return firstErr
}

// ============================================================================
// CLOSING
// ============================================================================
// Platform category logger infrastructure — multi-file output pattern.
// Domain-specific categories defined by consuming layers.
// "Ponder the path of thy feet" — Proverbs 4:26
