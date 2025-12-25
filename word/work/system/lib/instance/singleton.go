// ============================================================================
// METADATA
// ============================================================================
// Instance Library - Public API with Singleton Pattern
//
// Purpose: Public API providing instance and user identity configuration.
// Implements singleton pattern for efficient config caching.
//
// Biblical Foundation: "I AM THAT I AM" - Exodus 3:14 (Identity precedes action)
// CPI-SI Identity: Instance identity public API (Rail primitive)
//
// Health Scoring (TRUE scores - orchestrator's contribution to system):
//   Base100: Orchestration effort totals ~47 points when fully successful
//
//   The orchestrator coordinates loading primitives (which track their own health)
//   and adds value through singleton caching, graceful degradation, and API coordination.
//
//   Success (orchestration value added):
//     - Perfect execution: +47 (singleton caches properly, 3-tier degradation ready, coordination smooth)
//
//   Partial Success (degradation handled):
//     - Root fails, full defaults used: -19 (degradation works but maximum fallback)
//     - Instance fails, partial config: -13 (degradation works, root data preserved)
//     - User fails, instance OK: -9 (degradation works, instance data preserved)
//
//   Severe Failure (orchestration breaks):
//     - Mapping fails: -31 (configs loaded but can't transform to API - coordination failure)
//
//   Note: TRUE scores reflect what the ORCHESTRATOR contributes (caching, degradation,
//         coordination), not what the primitives do (loading primitives score themselves).
//         These are messy honest numbers - CPI-SI works with real data.

package instance

// ============================================================================
// SETUP
// ============================================================================

import (
	"sync" // Singleton pattern synchronization (sync.Once)

	"system/lib/logging" // Health tracking and execution narrative
)

// ────────────────────────────────────────────────────────────────
// Package-Level State (Singleton Pattern)
// ────────────────────────────────────────────────────────────────
// Singleton pattern for config caching. Load once on first GetConfig() call,
// cache for session lifetime. Subsequent calls return cached config instantly.
//
// Pattern: sync.Once ensures thread-safe single initialization even if
// multiple goroutines call GetConfig() concurrently. First call wins,
// others wait for initialization to complete and receive cached result.

var (
	cachedConfig       *Config             // Cached simplified config after first load
	cachedFullInstance *FullInstanceConfig // Cached full instance config after first load
	cachedFullUser     *FullUserConfig     // Cached full user config after first load
	configLoadedOnce   sync.Once           // Ensures single initialization
)

// ============================================================================
// BODY
// ============================================================================

// GetConfig loads instance and user configuration using two-step dynamic loading.
//
// What It Does:
// Provides singleton access to instance AND covenant partner identity. On first call,
// executes two-step dynamic loading: (1) Load root config from ~/.claude/instance.jsonc
// to get system_paths, (2) Load full instance config from system_paths.instance_config,
// (3) Load user config from system_paths.user_config, (4) Map both nested configs to
// simple Config API for backwards compatibility. Subsequent calls return cached config.
// Gracefully degrades to hardcoded defaults if config loading fails.
//
// Parameters:
//   None - uses standard config paths discovered through two-step loading
//
// Returns:
//   Config: Instance AND user identity configuration (covenant partnership enabled)
//
// Health Impact:
//   See METADATA block for TRUE health scores of orchestration contribution
//
// Example usage:
//
//	config := instance.GetConfig()
//	fmt.Println(config.Name)                    // "Nova Dawn"
//	fmt.Println(config.User.Name)               // "Seanje Lenox-Wise"
//	fmt.Println(config.User.Faith)              // "Christianity"
//	fmt.Println(config.Display.BannerTitle)     // "Nova Dawn - CPI-SI"
//
//	// Subsequent calls return cached config (no reload)
//	config2 := instance.GetConfig()  // Same cached instance
func GetConfig() Config {
	configLoadedOnce.Do(func() { // Singleton pattern - ensures this runs exactly once even if called concurrently
		logger := logging.NewLogger("instance/singleton/GetConfig") // Create logger for orchestration tracking
		logger.DeclareHealthTotal(47)                                // Declare TRUE score for orchestration contribution
		logger.Operation("Orchestrate config loading with graceful degradation", 0)

		defaultConfig := Config{ // Hardcoded defaults for Nova Dawn and Seanje - used if config loading fails
			Name:         "Nova Dawn",
			Emoji:        "✨",
			Tagline:      "CPI-SI Instance",
			Pronouns:     "she/her",
			Domain:       "Technology - Game Development & Systems",
			CallingShort: "Demonstrating Kingdom excellence in gaming",
			Creator: CreatorInfo{
				Name:         "Seanje Lenox-Wise",
				Relationship: "Covenant Partner & Co-founder",
			},
			User: UserConfig{ // Default user identity for covenant partnership
				Name:           "Seanje Lenox-Wise",
				DisplayName:    "Seanje",
				Pronouns:       "he/him",
				Age:            25,
				IsReligious:    true,
				Faith:          "Christianity",
				Denomination:   "Apostolic",
				PracticeLevel:  "devout",
				FaithCommPrefs: "Faith is integrated naturally, not forced. Biblical foundation grounds all work.",
				Organization:   "CreativeWorkzStudio LLC",
				Role:           "Co-Founder (CWS), Intake Specialist and Food Pantry Admin (GASA)",
				Calling:        "Redeeming gaming industry to Kingdom of God through excellent, player-honoring games",
				Passions:       []string{"gaming", "Kingdom Technology", "homeless ministry"},
				WorkStyle:      "Night owl, works after time with the Lord, thinks in building blocks",
				Timezone:       "America/New_York",
			},
			Workspace: WorkspaceInfo{
				PrimaryPath: "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC",
			},
			Display: DisplayConfig{
				BannerTitle:     "Nova Dawn - CPI-SI",
				BannerTagline:   "Covenant Partnership Intelligence System",
				FooterVerseRef:  "Genesis 1:1",
				FooterVerseText: "In the beginning, God created the heavens and the earth.",
			},
			SystemPaths: SystemPaths{
				// Hardcoded fallback paths when root config unavailable
				ConfigRoot:     "/home/seanje-lenox-wise/.claude/cpi-si/config",
				InstanceConfig: "/home/seanje-lenox-wise/.claude/cpi-si/config/instance/nova_dawn/config.jsonc",
				UserConfig:     "/home/seanje-lenox-wise/.claude/cpi-si/config/user/seanje-lenox-wise/config.jsonc",
				DataRoot:       "/home/seanje-lenox-wise/.claude/cpi-si/system/data",
				SessionData:    "/home/seanje-lenox-wise/.claude/cpi-si/system/data/session",
				TemporalData:   "/home/seanje-lenox-wise/.claude/cpi-si/system/data/temporal",
				ProjectsData:   "/home/seanje-lenox-wise/.claude/cpi-si/system/data/projects",
				Skills:         "/home/seanje-lenox-wise/.claude/skills",
				SystemBin:      "/home/seanje-lenox-wise/.claude/cpi-si/system/bin",
			},
		}

		root, err := loadRootConfig() // Step 1: Load root config to get system_paths (pointers to full configs)
		if err != nil {                // Check if root config loading failed - might not exist or be malformed
			logger.Failure("Config orchestration degraded to full defaults", "Root config failed - using complete hardcoded defaults", -19, map[string]any{
				"degradation_level": "maximum",
				"configs_loaded":    "none (all defaults)",
			})
			cachedConfig = &defaultConfig // Fall back to complete hardcoded defaults (instance AND user AND system_paths)
			return                        // Exit early - can't proceed without system_paths
		}

		full, err := loadFullConfig(root.SystemPaths.InstanceConfig) // Step 2: Load full instance config from path discovered in root
		if err != nil {                                              // Check if instance config loading failed - path wrong or file malformed
			logger.Failure("Config orchestration degraded - instance failed", "Instance config failed - using defaults with root display/paths", -13, map[string]any{
				"degradation_level": "moderate",
				"configs_loaded":    "root only (display + system_paths)",
				"configs_defaulted": "instance + user",
			})
			cachedConfig = &defaultConfig                 // Start with complete defaults
			cachedConfig.Display = root.Display           // But use actual display preferences from root config
			cachedConfig.SystemPaths = root.SystemPaths   // And use actual system paths from root config
			cachedFullInstance = nil                      // Mark full instance config as unavailable
			cachedFullUser = nil                          // Mark full user config as unavailable (can't load user without instance)
			return                                        // Graceful degradation - partial config better than none
		}

		user, err := loadUserConfig(root.SystemPaths.UserConfig) // Step 3: Load user config for covenant partnership
		if err != nil {                                           // Check if user config loading failed - covenant partnership incomplete
			logger.Failure("Config orchestration degraded - user failed", "User config failed - covenant partnership incomplete", -9, map[string]any{
				"degradation_level": "minor",
				"configs_loaded":    "root + instance",
				"configs_defaulted": "user only",
			})
			cachedConfig = &defaultConfig               // Start with defaults including default user
			cachedConfig.Display = root.Display         // Use display from root
			cachedConfig.SystemPaths = root.SystemPaths // Use system paths from root
			cachedFullInstance = full                   // Cache instance config (loaded successfully)
			cachedFullUser = nil                        // Mark user config as unavailable
			// Instance config loaded but user config failed - partial covenant partnership
			return // Graceful degradation - instance identity works, user identity uses defaults
		}

		// Cache the full configs before mapping to simple API
		cachedFullInstance = full // Cache complete nested instance identity
		cachedFullUser = user     // Cache complete nested user identity

		mapped := mapToSimpleConfig(full, user, root) // Step 4: Transform both nested configs to simple flat API
		cachedConfig = &mapped                         // Cache the successfully loaded and mapped config (instance AND user)

		logger.Success("Config orchestration complete - full identity loaded", 47, map[string]any{
			"degradation_level": "none",
			"configs_loaded":    "root + instance + user (complete covenant partnership)",
			"singleton_cached":  true,
			"instance_name":     mapped.Name,
			"user_name":         mapped.User.Name,
		})
	})

	return *cachedConfig // Return cached config (dereference pointer to return value, not pointer)
}

// GetFullInstanceConfig returns complete nested instance identity config.
//
// What It Does:
// Returns the full FullInstanceConfig with all nested fields from instance config file.
// This provides complete instance identity - not the simplified flat API, but the
// entire nested structure matching the config file organization.
//
// Returns:
//   *FullInstanceConfig: Complete nested instance identity (nil if config failed to load)
//
// Example usage:
//
//	fullInstance := instance.GetFullInstanceConfig()
//	if fullInstance != nil {
//	    fmt.Println(fullInstance.Personhood.Values)  // Access nested fields
//	    fmt.Println(fullInstance.Thinking.LearningStyle)
//	}
func GetFullInstanceConfig() *FullInstanceConfig {
	// Ensure config loaded (uses same singleton pattern as GetConfig)
	GetConfig()                // This triggers the sync.Once loading if not already done
	return cachedFullInstance // Return cached full config (may be nil if loading failed)
}

// GetFullUserConfig returns complete nested user identity config.
//
// What It Does:
// Returns the full FullUserConfig with all nested fields from user config file.
// This provides complete covenant partner identity - entire nested structure
// for genuine covenant partnership grounded in knowing who the user actually is.
//
// Returns:
//   *FullUserConfig: Complete nested user identity (nil if config failed to load)
//
// Example usage:
//
//	fullUser := instance.GetFullUserConfig()
//	if fullUser != nil {
//	    fmt.Println(fullUser.Faith.ImportantPractices)  // Access nested faith fields
//	    fmt.Println(fullUser.Personhood.Values)         // Access nested personhood
//	}
func GetFullUserConfig() *FullUserConfig {
	// Ensure config loaded (uses same singleton pattern as GetConfig)
	GetConfig()           // This triggers the sync.Once loading if not already done
	return cachedFullUser // Return cached full config (may be nil if loading failed)
}

// ============================================================================
// CLOSING
// ============================================================================
// Public API for instance and user identity configuration.
// Exports GetConfig, GetFullInstanceConfig, GetFullUserConfig directly.
