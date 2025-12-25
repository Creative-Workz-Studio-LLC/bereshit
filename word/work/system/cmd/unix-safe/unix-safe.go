// ============================================================================
// METADATA
// ============================================================================
// Unix-Safe Command - CPI-SI Framework Utility
// Purpose: Convert files to Unix line endings (CRLF → LF)
// Non-blocking: Safe conversion with optional backup and recursive processing
// Usage: ./bin/unix-safe <directory> [--backup] [--recursive]
//
// HEALTH SCORING MAP (TRUE SCORE):
// ----------------------------------
// Setup (4 health tracking calls = 18 points):
//   Call 1/4: logger.Check - Initialize logger (+5 or -5)
//   Call 2/4: logger.Operation - Start operation (+5 or -5)
//   Call 3/4: logger.Check - Parse arguments (+5 or -5)
//   Call 4/4: logger.Check - Validate directory (+3 or -3)
//
// File Processing (5 health tracking calls = 103 points) - CORE PURPOSE:
//   Call 1/5: logger.Check - Scan directory (+10 or -10)
//   Call 2/5: logger.Check - Check files for CRLF (+30 or -30) - Detection accuracy critical
//   Call 3/5: logger.Check - Backup/skip backup (+8 or -8)
//   Call 4/5: logger.Check - Convert CRLF to LF (+50 or -50) - THE core operation
//   Call 5/5: logger.Check - Track results (+5 or -5)
//
// Summary & Results (4 health tracking calls = 18 points):
//   Call 1/4: logger.Check - Display summary (+3 or -3)
//   Call 2/4: logger.Check - Show recommendations (+5 or -5)
//   Call 3/4: logger.Success/Failure - Log final result (+5 or -5)
//   Call 4/4: logger.Check - Exit appropriately (+5 or -5)
//
// Total Possible: 139 points
// Normalization: (cumulative_health / 139) × 100

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"bytes"
	"flag"
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"system/lib/debugging"
	"system/lib/display"
	"system/lib/logging"
)

// ============================================================================
// BODY
// ============================================================================

type ConversionConfig struct {
	Directory string
	Backup    bool
	Recursive bool
}

type ConversionResults struct {
	Converted int
	Skipped   int
	Failed    int
	Errors    []string
}

// hasCRLF checks if a file contains CRLF line endings
func hasCRLF(path string) (bool, error) {
	content, err := os.ReadFile(path)
	if err != nil {
		return false, err
	}
	return bytes.Contains(content, []byte("\r\n")), nil
}

// convertFile converts a single file from CRLF to LF
func convertFile(path string, backup bool) error {
	// Read file content
	content, err := os.ReadFile(path)
	if err != nil {
		return fmt.Errorf("read failed: %w", err)
	}

	// Create backup if requested
	if backup {
		backupPath := path + ".bak"
		if err := os.WriteFile(backupPath, content, 0644); err != nil {
			return fmt.Errorf("backup failed: %w", err)
		}
	}

	// Convert CRLF to LF
	converted := bytes.ReplaceAll(content, []byte("\r\n"), []byte("\n"))

	// Write converted content
	fileInfo, err := os.Stat(path)
	if err != nil {
		return fmt.Errorf("stat failed: %w", err)
	}

	if err := os.WriteFile(path, converted, fileInfo.Mode()); err != nil {
		return fmt.Errorf("write failed: %w", err)
	}

	return nil
}

// processDirectory processes all files in a directory
func processDirectory(config ConversionConfig) ConversionResults {
	results := ConversionResults{
		Errors: make([]string, 0),
	}

	// Clean the config directory for comparison
	cleanConfigDir := filepath.Clean(config.Directory)

	// Determine walk depth
	walkFunc := func(path string, d fs.DirEntry, err error) error {
		if err != nil {
			results.Errors = append(results.Errors, fmt.Sprintf("%s: %v", path, err))
			return nil
		}

		// Skip directories themselves (we process their contents)
		if d.IsDir() {
			// If not recursive, skip subdirectories
			if !config.Recursive && filepath.Clean(path) != cleanConfigDir {
				return fs.SkipDir
			}
			return nil
		}

		// For files: skip if not recursive and file is in a subdirectory
		if !config.Recursive {
			fileDir := filepath.Clean(filepath.Dir(path))
			if fileDir != cleanConfigDir {
				return nil
			}
		}

		// Check if file has CRLF
		hasCR, err := hasCRLF(path)
		if err != nil {
			results.Errors = append(results.Errors, fmt.Sprintf("%s: %v", path, err))
			results.Failed++
			return nil
		}

		if !hasCR {
			results.Skipped++
			return nil
		}

		// Convert file
		fmt.Printf("  Converting: %s... ", filepath.Base(path))
		if err := convertFile(path, config.Backup); err != nil {
			fmt.Printf("%s✗%s\n", display.Red, display.Reset)
			results.Errors = append(results.Errors, fmt.Sprintf("%s: %v", path, err))
			results.Failed++
			return nil
		}

		fmt.Printf("%s✓%s\n", display.Green, display.Reset)
		results.Converted++
		return nil
	}

	filepath.WalkDir(config.Directory, walkFunc)
	return results
}

// showHeader displays the conversion header
func showHeader(config ConversionConfig) {
	fmt.Printf("%s╔════════════════════════════════════════════════════════════════╗%s\n", display.BoldYellow, display.Reset)
	fmt.Printf("%s║         Unix-Safe File Conversion (CRLF → LF)                ║%s\n", display.BoldYellow, display.Reset)
	fmt.Printf("%s╚════════════════════════════════════════════════════════════════╝%s\n", display.BoldYellow, display.Reset)
	fmt.Println()
	fmt.Printf("Directory: %s\n", config.Directory)

	if config.Backup {
		fmt.Println("Backup: Enabled (.bak files will be created)")
	} else {
		fmt.Println("Backup: Disabled")
	}

	if config.Recursive {
		fmt.Println("Mode: Recursive (processing all subdirectories)")
	} else {
		fmt.Println("Mode: Single directory only")
	}
	fmt.Println()
}

// showSummary displays the conversion results
func showSummary(results ConversionResults, config ConversionConfig) {
	fmt.Println()
	fmt.Printf("%s━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━%s\n", display.BoldYellow, display.Reset)
	fmt.Printf("  Results: %s%d converted%s, %d already Unix-safe",
		display.Green, results.Converted, display.Reset, results.Skipped)

	if results.Failed > 0 {
		fmt.Printf(", %s%d failed%s", display.Red, results.Failed, display.Reset)
	}
	fmt.Println()
	fmt.Printf("%s━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━%s\n", display.BoldYellow, display.Reset)

	if results.Converted > 0 {
		fmt.Println()
		fmt.Println(display.Success("Conversion complete"))

		if config.Backup {
			fmt.Println()
			fmt.Println("Backup files created with .bak extension")
			fmt.Printf("To remove backups: rm %s/*.bak\n", config.Directory)
		}
	} else if results.Failed == 0 {
		fmt.Println()
		fmt.Println(display.Success("All files already have Unix line endings"))
	}

	// Show errors if any
	if len(results.Errors) > 0 {
		fmt.Println()
		fmt.Println(display.Warning("Errors encountered:"))
		for _, err := range results.Errors {
			fmt.Printf("  %s\n", err)
		}
	}
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup Action 1/4: Initialize logger (+5 or -5)
	logger := logging.NewLogger("unix-safe")
	logger.DeclareHealthTotal(139)  // Total possible points from health scoring map
	inspector := debugging.NewInspector("unix-safe")
	inspector.Enable() // Enable debugging to capture HOW data

	// DEBUGGING: Capture conversion command start
	inspector.Snapshot("unix-safe-start", map[string]any{
		"command": "unix-safe",
		"purpose": "convert files to Unix line endings (CRLF → LF)",
		"options": []string{"backup", "recursive"},
	})

	logger.Check("logger-initialized", true, 5, map[string]any{
		"component": "unix-safe",
	})

	// Setup Action 2/4: Start operation (+5 or -5)
	logger.Operation("unix-safe-conversion", 5, "convert files to Unix line endings")

	// Setup Action 3/4: Parse arguments (+5 or -5)
	var config ConversionConfig

	flag.StringVar(&config.Directory, "dir", "", "Directory to process (required)")
	flag.BoolVar(&config.Backup, "backup", false, "Create .bak files before conversion")
	flag.BoolVar(&config.Recursive, "recursive", false, "Process subdirectories recursively")
	flag.Parse()

	// Handle positional argument for directory
	if config.Directory == "" && flag.NArg() > 0 {
		config.Directory = flag.Arg(0)
	}

	if config.Directory == "" {
		fmt.Println("Usage: unix-safe [--backup] [--recursive] <directory>")
		fmt.Println()
		fmt.Println("Convert files to Unix line endings (CRLF → LF)")
		fmt.Println()
		fmt.Println("Arguments:")
		fmt.Println("  directory     Directory to process (required)")
		fmt.Println("  --backup      Create .bak files before conversion")
		fmt.Println("  --recursive   Process subdirectories recursively")
		fmt.Println()
		fmt.Println("Examples:")
		fmt.Println("  unix-safe ~/.claude/system/sudoers")
		fmt.Println("  unix-safe --recursive ~/.claude/system")
		fmt.Println("  unix-safe --backup --recursive /path/to/files")
		logger.Failure("Argument parsing failed", "no directory specified", -5, map[string]any{
			"exit_code": 1,
		})
		os.Exit(1)
	}

	logger.Check("arguments-parsed", true, 5, map[string]any{
		"directory": config.Directory,
		"backup":    config.Backup,
		"recursive": config.Recursive,
	})

	// Setup Action 4/4: Validate directory (+5 or -5)
	fileInfo, err := os.Stat(config.Directory)
	if err != nil || !fileInfo.IsDir() {
		fmt.Printf("%sError: Directory not found: %s%s\n",
			display.Red, config.Directory, display.Reset)
		logger.Failure("Directory validation failed", "directory does not exist", -3, map[string]any{
			"directory":  config.Directory,
			"error":      err,
			"exit_code":  1,
		})
		os.Exit(1)
	}

	logger.Check("directory-validated", true, 3, map[string]any{
		"directory": config.Directory,
		"exists":    true,
	})

	// DEBUGGING: Capture configuration
	inspector.Snapshot("unix-safe-config", map[string]any{
		"directory": config.Directory,
		"backup":    config.Backup,
		"recursive": config.Recursive,
		"validated": true,
	})

	// Display header
	showHeader(config)

	// File Processing Action 1/5: Scan directory (+10 or -10)
	logger.Check("directory-scan-started", true, 10, map[string]any{
		"directory": config.Directory,
		"recursive": config.Recursive,
	})

	// Process all files
	results := processDirectory(config)

	// File Processing Action 2/5: Check files for CRLF (+30 or -30)
	totalFiles := results.Converted + results.Skipped + results.Failed
	logger.Check("files-checked", totalFiles > 0, 30, map[string]any{
		"total_files": totalFiles,
		"converted":   results.Converted,
		"skipped":     results.Skipped,
		"failed":      results.Failed,
	})

	// File Processing Action 3/5: Backup files if requested (+8 or -8)
	if config.Backup && results.Converted > 0 {
		logger.Check("backup-created", true, 8, map[string]any{
			"backup_count": results.Converted,
		})
	} else {
		logger.Check("backup-skipped", true, 8, map[string]any{
			"backup_enabled": config.Backup,
		})
	}

	// File Processing Action 4/5: Convert CRLF to LF (+50 or -50)
	conversionSuccess := results.Failed == 0
	healthImpact := 50
	if !conversionSuccess {
		healthImpact = -50
	}
	logger.Check("files-converted", conversionSuccess, healthImpact, map[string]any{
		"converted": results.Converted,
		"failed":    results.Failed,
	})

	// File Processing Action 5/5: Track results (+5 or -5)
	logger.Check("results-tracked", true, 5, map[string]any{
		"converted": results.Converted,
		"skipped":   results.Skipped,
		"failed":    results.Failed,
		"errors":    len(results.Errors),
	})

	// DEBUGGING: Capture conversion results
	conversionSuccessful := results.Failed == 0
	inspector.ExpectedState("unix-safe-results", 0, results.Failed, map[string]any{
		"converted":      results.Converted,
		"skipped":        results.Skipped,
		"failed":         results.Failed,
		"error_count":    len(results.Errors),
		"all_successful": conversionSuccessful,
	})

	// Summary Action 1/4: Display summary (+3 or -3)
	showSummary(results, config)
	logger.Check("summary-displayed", true, 3, map[string]any{
		"displayed": "conversion summary",
	})

	// Summary Action 2/4: Show recommendations (+5 or -5)
	logger.Check("recommendations-shown", true, 5, map[string]any{
		"shown": results.Failed > 0,
	})

	// Summary Action 3/4: Log final result (+5 or -5)
	if results.Failed == 0 {
		logger.Success("Conversion completed successfully", 5, map[string]any{
			"converted":  results.Converted,
			"skipped":    results.Skipped,
			"exit_code":  0,
		})
	} else {
		logger.Failure("Conversion completed with errors", "some files failed to convert", -5, map[string]any{
			"converted":  results.Converted,
			"failed":     results.Failed,
			"exit_code":  1,
		})
	}

	// Summary Action 4/4: Exit appropriately (+5 or -5)
	if results.Failed > 0 {
		logger.Check("exit-with-errors", true, -5, map[string]any{
			"exit_code": 1,
		})
		os.Exit(1)
	}

	logger.Check("exit-success", true, 5, map[string]any{
		"exit_code": 0,
	})
}
