// ============================================================================
// METADATA
// ============================================================================
//
// Key: cpisi-loganalyzer
// Purpose: Import and analyze CPI-SI JSONL logs
// Biblical: Proverbs 2:6 - "For the LORD giveth wisdom"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-24
//
// Commands:
//   import   - Import JSONL logs into database
//   analyze  - Run analysis queries
//   report   - Generate summary report
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"bufio"
	"database/sql"
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"time"

	_ "github.com/mattn/go-sqlite3"
)

// LogEvent represents a parsed JSONL log entry
type LogEvent struct {
	Timestamp   string                 `json:"timestamp"`
	Type        string                 `json:"type"`
	SessionID   string                 `json:"session_id"`
	Sequence    int                    `json:"sequence"`
	Level       string                 `json:"level"`
	Component   string                 `json:"component"`
	Message     string                 `json:"message"`
	Health      float64                `json:"health"`
	Delta       float64                `json:"delta"`
	HealthLevel string                 `json:"health_level"`
	Details     map[string]interface{} `json:"details"`
}

// Config holds runtime configuration
type Config struct {
	DBPath   string
	LogsDir  string
	Command  string
	LogType  string
	LogDate  string
	Verbose  bool
}

// ============================================================================
// BODY
// ============================================================================

func main() {
	cfg := parseFlags()

	switch cfg.Command {
	case "import":
		runImport(cfg)
	case "analyze":
		runAnalyze(cfg)
	case "report":
		runReport(cfg)
	default:
		fmt.Println("Usage: loganalyzer <command> [options]")
		fmt.Println("\nCommands:")
		fmt.Println("  import   Import JSONL logs into database")
		fmt.Println("  analyze  Run analysis queries")
		fmt.Println("  report   Generate summary report")
		os.Exit(1)
	}
}

func parseFlags() Config {
	cfg := Config{}

	// Default paths
	home, _ := os.UserHomeDir()
	defaultDB := filepath.Join(home, ".claude", "data", "cpisi.db")
	defaultLogs := filepath.Join(home, ".claude", "data", "logs")

	flag.StringVar(&cfg.DBPath, "db", defaultDB, "Path to SQLite database")
	flag.StringVar(&cfg.LogsDir, "logs", defaultLogs, "Path to logs directory")
	flag.StringVar(&cfg.LogType, "type", "all", "Log type to import: session, tools, context, all")
	flag.StringVar(&cfg.LogDate, "date", "", "Specific date to import (YYYY-MM-DD)")
	flag.BoolVar(&cfg.Verbose, "v", false, "Verbose output")
	flag.Parse()

	if flag.NArg() > 0 {
		cfg.Command = flag.Arg(0)
	}

	return cfg
}

// --- Import Command ---

func runImport(cfg Config) {
	db, err := sql.Open("sqlite3", cfg.DBPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error opening database: %v\n", err)
		os.Exit(1)
	}
	defer db.Close()

	logTypes := []string{"session", "tools", "context"}
	if cfg.LogType != "all" {
		logTypes = []string{cfg.LogType}
	}

	totalImported := 0
	for _, logType := range logTypes {
		dir := filepath.Join(cfg.LogsDir, logType)
		if _, err := os.Stat(dir); os.IsNotExist(err) {
			continue
		}

		files, _ := filepath.Glob(filepath.Join(dir, "*.jsonl"))
		for _, file := range files {
			// Extract date from filename
			base := filepath.Base(file)
			date := strings.TrimSuffix(base, ".jsonl")

			// Skip if specific date requested and doesn't match
			if cfg.LogDate != "" && date != cfg.LogDate {
				continue
			}

			// Check if already imported
			if isImported(db, logType, date) {
				if cfg.Verbose {
					fmt.Printf("Skipping %s/%s (already imported)\n", logType, date)
				}
				continue
			}

			count, err := importFile(db, file, logType, date)
			if err != nil {
				fmt.Fprintf(os.Stderr, "Error importing %s: %v\n", file, err)
				continue
			}

			// Record import
			recordImport(db, logType, date, file, count)
			totalImported += count

			if cfg.Verbose {
				fmt.Printf("Imported %d events from %s/%s\n", count, logType, date)
			}
		}
	}

	fmt.Printf("Total events imported: %d\n", totalImported)
}

func isImported(db *sql.DB, logType, date string) bool {
	var count int
	err := db.QueryRow("SELECT COUNT(*) FROM log_imports WHERE log_type = ? AND log_date = ?",
		logType, date).Scan(&count)
	return err == nil && count > 0
}

func recordImport(db *sql.DB, logType, date, path string, count int) {
	db.Exec(`INSERT OR REPLACE INTO log_imports (log_type, log_date, file_path, events_imported)
		VALUES (?, ?, ?, ?)`, logType, date, path, count)
}

func importFile(db *sql.DB, path, logType, date string) (int, error) {
	file, err := os.Open(path)
	if err != nil {
		return 0, err
	}
	defer file.Close()

	tx, err := db.Begin()
	if err != nil {
		return 0, err
	}

	count := 0
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		var event LogEvent
		if err := json.Unmarshal(scanner.Bytes(), &event); err != nil {
			continue
		}

		if err := insertEvent(tx, &event, logType, date); err != nil {
			// Skip duplicates silently
			continue
		}
		count++
	}

	if err := tx.Commit(); err != nil {
		return 0, err
	}

	return count, nil
}

func insertEvent(tx *sql.Tx, event *LogEvent, logType, date string) error {
	detailsJSON, _ := json.Marshal(event.Details)
	trajectory := ""
	if t, ok := event.Details["trajectory"].(string); ok {
		trajectory = t
	}

	switch logType {
	case "session":
		_, err := tx.Exec(`INSERT OR IGNORE INTO session_events
			(session_id, timestamp, sequence_num, event_type, level, component, message,
			 health_delta, health_after, health_level, trajectory, details, log_date)
			VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`,
			event.SessionID, event.Timestamp, event.Sequence, event.Type,
			event.Level, event.Component, event.Message,
			event.Delta, event.Health, event.HealthLevel, trajectory,
			string(detailsJSON), date)
		return err

	case "tools":
		intendedKey := 0
		keyLabel := "lateral"
		if k, ok := event.Details["intended_key"].(string); ok {
			switch {
			case strings.Contains(k, "+1"), strings.Contains(k, "growth"):
				intendedKey = 1
				keyLabel = "growth"
			case strings.Contains(k, "-1"), strings.Contains(k, "completion"):
				intendedKey = -1
				keyLabel = "completion"
			}
		}

		toolName := ""
		if t, ok := event.Details["tool"].(string); ok {
			toolName = t
		}

		_, err := tx.Exec(`INSERT OR IGNORE INTO tool_events
			(session_id, timestamp, sequence_num, event_type, tool_name,
			 intended_key, intended_key_label, health_delta, health_after, health_level,
			 trajectory, log_date)
			VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`,
			event.SessionID, event.Timestamp, event.Sequence, event.Type, toolName,
			intendedKey, keyLabel, event.Delta, event.Health, event.HealthLevel,
			trajectory, date)
		return err

	case "context":
		// Context events go to session_events
		_, err := tx.Exec(`INSERT OR IGNORE INTO session_events
			(session_id, timestamp, sequence_num, event_type, level, component, message,
			 trajectory, details, log_date)
			VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`,
			event.SessionID, event.Timestamp, event.Sequence, event.Type,
			event.Level, event.Component, event.Message, trajectory,
			string(detailsJSON), date)
		return err
	}

	return nil
}

// --- Analyze Command ---

func runAnalyze(cfg Config) {
	db, err := sql.Open("sqlite3", cfg.DBPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error opening database: %v\n", err)
		os.Exit(1)
	}
	defer db.Close()

	fmt.Println("═══════════════════════════════════════════════════════════════")
	fmt.Println("                    CPI-SI Log Analysis")
	fmt.Println("═══════════════════════════════════════════════════════════════")

	// Key Distribution
	fmt.Println("\n📊 Key Distribution (which keys are we picking?):")
	fmt.Println("─────────────────────────────────────────────────────────────────")
	rows, err := db.Query(`
		SELECT intended_key_label, COUNT(*) as picks,
		       ROUND(AVG(health_delta), 2) as avg_delta
		FROM tool_events
		WHERE event_type = 'key_picked'
		GROUP BY intended_key_label
		ORDER BY picks DESC`)
	if err == nil {
		defer rows.Close()
		for rows.Next() {
			var label string
			var picks int
			var avgDelta float64
			rows.Scan(&label, &picks, &avgDelta)
			fmt.Printf("  %-12s  %4d picks  (avg Δ: %+.2f)\n", label, picks, avgDelta)
		}
	}

	// Tool Patterns
	fmt.Println("\n🔧 Top Tools by Usage:")
	fmt.Println("─────────────────────────────────────────────────────────────────")
	rows, err = db.Query(`
		SELECT tool_name, COUNT(*) as uses,
		       SUM(CASE WHEN health_delta > 0 THEN 1 ELSE 0 END) as positive,
		       ROUND(AVG(health_delta), 2) as avg_delta
		FROM tool_events
		WHERE event_type = 'tool_completed' AND tool_name != ''
		GROUP BY tool_name
		ORDER BY uses DESC
		LIMIT 10`)
	if err == nil {
		defer rows.Close()
		for rows.Next() {
			var tool string
			var uses, positive int
			var avgDelta float64
			rows.Scan(&tool, &uses, &positive, &avgDelta)
			rate := float64(positive) / float64(uses) * 100
			fmt.Printf("  %-15s  %4d uses  %5.1f%% positive  (avg Δ: %+.2f)\n",
				tool, uses, rate, avgDelta)
		}
	}

	// Session Events
	fmt.Println("\n📋 Session Event Types:")
	fmt.Println("─────────────────────────────────────────────────────────────────")
	rows, err = db.Query(`
		SELECT event_type, COUNT(*) as count,
		       ROUND(AVG(health_delta), 2) as avg_delta
		FROM session_events
		GROUP BY event_type
		ORDER BY count DESC`)
	if err == nil {
		defer rows.Close()
		for rows.Next() {
			var eventType string
			var count int
			var avgDelta sql.NullFloat64
			rows.Scan(&eventType, &count, &avgDelta)
			deltaStr := "n/a"
			if avgDelta.Valid {
				deltaStr = fmt.Sprintf("%+.2f", avgDelta.Float64)
			}
			fmt.Printf("  %-25s  %4d  (avg Δ: %s)\n", eventType, count, deltaStr)
		}
	}

	// Feedback Analysis
	fmt.Println("\n⚠️  Feedback Events (user corrections):")
	fmt.Println("─────────────────────────────────────────────────────────────────")
	rows, err = db.Query(`
		SELECT COUNT(*) as total,
		       ROUND(AVG(health_delta), 2) as avg_impact
		FROM session_events
		WHERE event_type = 'feedback_detected'`)
	if err == nil {
		defer rows.Close()
		for rows.Next() {
			var total int
			var avgImpact sql.NullFloat64
			rows.Scan(&total, &avgImpact)
			impactStr := "n/a"
			if avgImpact.Valid {
				impactStr = fmt.Sprintf("%+.2f", avgImpact.Float64)
			}
			fmt.Printf("  Total corrections: %d  (avg impact: %s)\n", total, impactStr)
		}
	}
}

// --- Report Command ---

func runReport(cfg Config) {
	db, err := sql.Open("sqlite3", cfg.DBPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error opening database: %v\n", err)
		os.Exit(1)
	}
	defer db.Close()

	fmt.Println("═══════════════════════════════════════════════════════════════")
	fmt.Printf("           CPI-SI State Machine Report - %s\n", time.Now().Format("2006-01-02"))
	fmt.Println("═══════════════════════════════════════════════════════════════")

	// Summary stats
	var totalSessions, totalChoices, totalToolEvents, totalSessionEvents int
	db.QueryRow("SELECT COUNT(*) FROM sessions").Scan(&totalSessions)
	db.QueryRow("SELECT COUNT(*) FROM choices").Scan(&totalChoices)
	db.QueryRow("SELECT COUNT(*) FROM tool_events").Scan(&totalToolEvents)
	db.QueryRow("SELECT COUNT(*) FROM session_events").Scan(&totalSessionEvents)

	fmt.Println("\n📈 Database Summary:")
	fmt.Printf("  Sessions:       %d\n", totalSessions)
	fmt.Printf("  Choices:        %d\n", totalChoices)
	fmt.Printf("  Tool Events:    %d\n", totalToolEvents)
	fmt.Printf("  Session Events: %d\n", totalSessionEvents)

	// Key balance
	fmt.Println("\n⚖️  Key Balance (are we choosing wisely?):")
	var growth, lateral, completion int
	db.QueryRow(`SELECT
		SUM(CASE WHEN intended_key = 1 THEN 1 ELSE 0 END),
		SUM(CASE WHEN intended_key = 0 THEN 1 ELSE 0 END),
		SUM(CASE WHEN intended_key = -1 THEN 1 ELSE 0 END)
		FROM tool_events WHERE event_type = 'key_picked'`).Scan(&growth, &lateral, &completion)

	total := growth + lateral + completion
	if total > 0 {
		fmt.Printf("  +1 (growth):     %4d (%5.1f%%)\n", growth, float64(growth)/float64(total)*100)
		fmt.Printf("   0 (lateral):    %4d (%5.1f%%)\n", lateral, float64(lateral)/float64(total)*100)
		fmt.Printf("  -1 (completion): %4d (%5.1f%%)\n", completion, float64(completion)/float64(total)*100)
	}

	// Recommendations
	fmt.Println("\n💡 Recommendations:")
	if lateral > growth+completion {
		fmt.Println("  • High lateral key usage - consider more intentional growth/completion choices")
	}
	if growth > lateral*2 {
		fmt.Println("  • Very high growth bias - ensure natural stopping points aren't missed")
	}

	fmt.Println("\n═══════════════════════════════════════════════════════════════")
}

// ============================================================================
// CLOSING
// ============================================================================
