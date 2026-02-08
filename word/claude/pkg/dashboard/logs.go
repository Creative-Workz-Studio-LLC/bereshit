// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-dashboard-logs
// Purpose: Session log parser and tailer for real-time event streaming
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-07
//
// ═══════════════════════════════════════════════════════════════════════════

package dashboard

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"bufio"
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"time"

	"github.com/creativeworkzstudio/claude-global/pkg/util/fs/paths"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ReadRecentEvents reads the most recent N events from today's session log.
func ReadRecentEvents(limit int) ([]LogEvent, error) {
	logsDir := paths.LogsSessionDir()
	today := time.Now().Format("2006-01-02")
	logPath := filepath.Join(logsDir, fmt.Sprintf("session-%s.jsonl", today))

	return readEventsFromFile(logPath, limit)
}

// TailLogFile reads the last N events from a specific log file.
func TailLogFile(logPath string, limit int) ([]LogEvent, error) {
	return readEventsFromFile(logPath, limit)
}

// ListLogFiles returns available session log files sorted by date (newest first).
func ListLogFiles() ([]string, error) {
	logsDir := paths.LogsSessionDir()
	entries, err := os.ReadDir(logsDir)
	if err != nil {
		return nil, err
	}

	var files []string
	for _, entry := range entries {
		if !entry.IsDir() && filepath.Ext(entry.Name()) == ".jsonl" {
			files = append(files, entry.Name())
		}
	}

	// Sort newest first
	sort.Sort(sort.Reverse(sort.StringSlice(files)))
	return files, nil
}

// readEventsFromFile reads events from a JSONL file, returning the last N.
func readEventsFromFile(logPath string, limit int) ([]LogEvent, error) {
	f, err := os.Open(logPath)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	var allEvents []LogEvent
	scanner := bufio.NewScanner(f)
	// Increase buffer for long lines
	scanner.Buffer(make([]byte, 0, 64*1024), 256*1024)

	for scanner.Scan() {
		line := scanner.Bytes()
		if len(line) == 0 {
			continue
		}

		var evt LogEvent
		if err := json.Unmarshal(line, &evt); err != nil {
			// Skip malformed lines
			continue
		}
		allEvents = append(allEvents, evt)
	}

	// Return last N events
	if limit > 0 && len(allEvents) > limit {
		allEvents = allEvents[len(allEvents)-limit:]
	}

	return allEvents, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
