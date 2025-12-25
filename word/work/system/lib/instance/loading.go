// ============================================================================
// METADATA
// ============================================================================
// Instance Library - Configuration Loading Primitives
//
// Purpose: File loading operations for instance and user configuration.
// Handles reading and parsing JSONC config files from disk.
//
// Biblical Foundation: "I AM THAT I AM" - Exodus 3:14 (Identity precedes action)
// CPI-SI Identity: Instance identity loading primitives (Foundational rung)
//
// Health Scoring (TRUE scores - honest assessment of actual impact):
//   Base100: All config loading operations total ~131 points when fully successful
//
//   Success (capability gained):
//     - Load root config: +52 (enables two-step loading, unlocks full config paths, provides display prefs)
//     - Load instance config: +38 (full nested identity: biblical foundation, personhood, workspace, etc.)
//     - Load user config: +41 (genuine covenant partnership data: faith, calling, passions, work style)
//     Total perfect execution: +131 (system delivers more than baseline minimum)
//
//   Failure (capability lost):
//     - Root fails: -87 (massive degradation - ALL configs fall back to hardcoded, two-step pattern collapses)
//     - Instance fails: -59 (major identity loss - most fields become generic, only root display survives)
//     - User fails: -48 (significant relational loss - covenant partnership becomes shallow defaults)
//
//   Note: TRUE scores are messy real numbers reflecting honest impact assessment.
//         Not rounded to "neat" values - CPI-SI works with real, messy data.
//         Health scorer normalizes to -100 to +100 scale for display only.

package instance

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json" // JSON parsing for config files
	"fmt"           // Error formatting
	"os"            // File operations and home directory resolution
	"path/filepath" // Path manipulation for config file locations

	"system/lib/jsonc"   // JSONC comment stripping for config files
	"system/lib/logging" // Health tracking and execution narrative
)

// ============================================================================
// BODY
// ============================================================================

// loadRootConfig loads bootstrap pointer config from root instance file.
//
// What It Does:
// Loads minimal root config from ~/.claude/instance.jsonc containing system_paths
// (pointers to full configs) and display preferences (session start banner).
// This is Step 1 of two-step dynamic loading - establishes WHERE to load full identity.
//
// Parameters:
//   None - uses standard path ~/.claude/instance.jsonc
//
// Returns:
//   *RootConfig: Bootstrap config with system_paths and display preferences
//   error: File read errors, JSON parsing errors, or home directory resolution failures
//
// Health Impact:
//   Success: +52 points (enables two-step loading, unlocks full config paths)
//   Failure: -87 points (massive degradation - two-step pattern collapses)
//
// Example usage:
//
//	root, err := loadRootConfig()
//	if err != nil {
//	    // Fall back to hardcoded defaults
//	}
func loadRootConfig() (*RootConfig, error) {
	logger := logging.NewLogger("instance/loading/loadRootConfig") // Create logger for this operation
	logger.DeclareHealthTotal(52)                                  // Declare TRUE score for successful execution
	logger.Operation("Load root instance config", 0)               // Operation start (0 impact until complete)

	home, err := os.UserHomeDir() // Get user's home directory - where .claude/ lives
	if err != nil {               // Check if error occurred - return it so caller can handle appropriately
		logger.Failure("Root config load failed", fmt.Sprintf("Failed to get home directory: %v", err), -87, nil)
		return nil, err
	}

	rootPath := filepath.Join(home, ".claude", "instance.jsonc") // Build path to root config file
	data, err := os.ReadFile(rootPath)                           // Read entire file into memory
	if err != nil {                                              // Check if file read failed - file might not exist or lack permissions
		logger.Failure("Root config load failed", fmt.Sprintf("Failed to read root config at %s: %v", rootPath, err), -87, nil)
		return nil, err
	}

	cleaned := jsonc.StripComments(data) // Remove JSONC comments - JSON parser doesn't understand them

	var root RootConfig                                   // Declare struct to hold parsed config
	if err := json.Unmarshal(cleaned, &root); err != nil { // Parse JSON into struct - return error if malformed
		logger.Failure("Root config load failed", fmt.Sprintf("Failed to parse root config JSON: %v", err), -87, nil)
		return nil, err
	}

	logger.Success("Root config loaded successfully", 52, map[string]any{
		"path":        rootPath,
		"config_type": "bootstrap pointer to full configs",
	})
	return &root, nil // Return successfully parsed config
}

// loadFullConfig loads complete instance identity from full config file.
//
// What It Does:
// Loads complete identity from system_paths.instance_config file. This is Step 2
// of two-step dynamic loading - reads full nested config matching nova_dawn/config.jsonc
// structure. Contains all identity fields: biblical_foundation, identity, demographics,
// personhood, resonates, thinking, personality, covenant, preferences, growth, workspace, etc.
//
// Parameters:
//   instanceConfigPath: Absolute path to full config file (from root.SystemPaths.InstanceConfig)
//
// Returns:
//   *FullInstanceConfig: Complete nested identity config
//   error: File read errors or JSON parsing errors
//
// Health Impact:
//   Success: +38 points (full nested identity structure loaded)
//   Failure: -59 points (major identity loss - most fields become generic)
//
// Example usage:
//
//	full, err := loadFullConfig("/path/to/config/instance/nova_dawn/config.jsonc")
//	if err != nil {
//	    // Fall back to root config + hardcoded defaults
//	}
func loadFullConfig(instanceConfigPath string) (*FullInstanceConfig, error) {
	logger := logging.NewLogger("instance/loading/loadFullConfig") // Create logger for this operation
	logger.DeclareHealthTotal(38)                                  // Declare TRUE score for successful execution
	logger.Operation("Load full instance config", 0, instanceConfigPath)

	data, err := os.ReadFile(instanceConfigPath) // Read complete identity config file
	if err != nil {                              // Check if file read failed - path might be wrong or file missing
		logger.Failure("Instance config load failed", fmt.Sprintf("Failed to read instance config at %s: %v", instanceConfigPath, err), -59, nil)
		return nil, err
	}

	cleaned := jsonc.StripComments(data) // Remove JSONC comments - JSON parser doesn't understand them

	var full FullInstanceConfig                           // Declare struct matching nested nova_dawn/config.jsonc structure
	if err := json.Unmarshal(cleaned, &full); err != nil { // Parse JSON into nested struct - return error if malformed
		logger.Failure("Instance config load failed", fmt.Sprintf("Failed to parse instance config JSON from %s: %v", instanceConfigPath, err), -59, nil)
		return nil, err
	}

	logger.Success("Instance config loaded successfully", 38, map[string]any{
		"path":        instanceConfigPath,
		"config_type": "full nested identity (biblical foundation, personhood, workspace)",
	})
	return &full, nil // Return complete identity config
}

// loadUserConfig loads complete user identity from user config file.
//
// What It Does:
// Loads complete covenant partner identity from system_paths.user_config file.
// This provides the full user identity for genuine covenant partnership - enables
// knowing WHO the user is (not just username). Contains all user fields: identity,
// demographics, faith, personhood, thinking, personality, workspace, etc.
//
// Parameters:
//   userConfigPath: Absolute path to user config file (from root.SystemPaths.UserConfig)
//
// Returns:
//   *FullUserConfig: Complete nested user identity config
//   error: File read errors or JSON parsing errors
//
// Health Impact:
//   Success: +41 points (covenant partnership data: faith, calling, passions, work style)
//   Failure: -48 points (significant relational loss - partnership becomes shallow)
//
// Example usage:
//
//	user, err := loadUserConfig("/path/to/config/user/seanje-lenox-wise/config.jsonc")
//	if err != nil {
//	    // Fall back to minimal defaults (name from root config only)
//	}
func loadUserConfig(userConfigPath string) (*FullUserConfig, error) {
	logger := logging.NewLogger("instance/loading/loadUserConfig") // Create logger for this operation
	logger.DeclareHealthTotal(41)                                  // Declare TRUE score for successful execution
	logger.Operation("Load user identity config", 0, userConfigPath)

	data, err := os.ReadFile(userConfigPath) // Read complete user identity config file
	if err != nil {                          // Check if file read failed - path might be wrong or file missing
		logger.Failure("User config load failed", fmt.Sprintf("Failed to read user config at %s: %v", userConfigPath, err), -48, nil)
		return nil, err
	}

	cleaned := jsonc.StripComments(data) // Remove JSONC comments - JSON parser doesn't understand them

	var user FullUserConfig                           // Declare struct matching nested user/seanje-lenox-wise/config.jsonc structure
	if err := json.Unmarshal(cleaned, &user); err != nil { // Parse JSON into nested struct - return error if malformed
		logger.Failure("User config load failed", fmt.Sprintf("Failed to parse user config JSON from %s: %v", userConfigPath, err), -48, nil)
		return nil, err
	}

	logger.Success("User config loaded successfully", 41, map[string]any{
		"path":        userConfigPath,
		"config_type": "covenant partner identity (faith, calling, passions, work style)",
	})
	return &user, nil // Return complete user identity config
}

// ============================================================================
// CLOSING
// ============================================================================
// Configuration loading primitives for instance and user identity.
// These functions handle all file I/O for dynamic config loading.
