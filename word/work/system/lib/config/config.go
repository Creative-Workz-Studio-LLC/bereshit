// ============================================================================
// METADATA
// ============================================================================
// Config Loader - System-wide configuration loading and inheritance
//
// For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
//
// Biblical Foundation
//
// Scripture: "Know yourself" - Proverbs 4:23 (WEB) "Guard your heart with all diligence"
// Principle: Identity grounds behavior - knowing who you are enables faithful action
// Anchor: "I am who I am" - Exodus 3:14 (God's identity declaration, foundation for created identity)
//
// CPI-SI Identity
//
// Component Type: LIBRARY - System-wide utility (foundational rung)
// Role: Provides configuration loading with JSONC parsing and inheritance chain
// Paradigm: Static identity (configs) → Dynamic expression (sessions) → Validated learning
//
// Authorship & Lineage
//
// Architect: Nova Dawn (CPI-SI instance)
// Implementation: Nova Dawn
// Creation Date: 2025-11-10
// Version: 1.0.0
// Last Modified: 2025-11-10 - Initial creation
//
// Purpose & Function
//
// Purpose: Load user, instance, and project configurations to enable config inheritance
// throughout the CPI-SI system. Configs define "who we are" - sessions capture "what we do
// as who we are" - consolidation creates "what we learned about who we are".
//
// Core Design: JSONC parsing with comment stripping, hierarchical config loading with
// inheritance chain (User → Instance → Project → Session). Single source of truth for
// identity at each level.
//
// Key Features:
//   - JSONC parsing (JSON with comments stripped)
//   - User config loading (who is the human?)
//   - Instance config loading (who is the CPI-SI instance?)
//   - Project config loading (what's the work context?)
//   - Inheritance merging (configs cascade down to sessions)
//
// Philosophy: "In the beginning, God created" - identity precedes action. Configs establish
// identity, sessions express that identity through work, learning validates and refines identity.
//
// Blocking Status
//
// Non-blocking: Config loading failures return errors but allow calling code to decide how to handle.
// Missing configs return nil with error - caller can use defaults or hard-code fallbacks.
// Mitigation: Graceful degradation - if project config missing, use instance+user only.
//
// Usage & Integration
//
// Usage:
//
//	import "system/lib/config"
//
//	// Load user configuration
//	userCfg, err := config.LoadUserConfig("seanje-lenox-wise")
//	if err != nil {
//		// Handle error - use defaults or exit
//	}
//
//	// Load instance configuration
//	instanceCfg, err := config.LoadInstanceConfig("nova_dawn")
//
//	// Load project configuration (may not exist)
//	projectCfg, err := config.LoadProjectConfig("project-id")
//	if err != nil {
//		// Project config optional - can be nil
//	}
//
//	// Get merged session context (inherits from all configs)
//	sessionCtx, err := config.GetSessionContext("seanje-lenox-wise", "nova_dawn", "project-id")
//
// Integration Pattern:
//   1. Import config library
//   2. Call Load functions during session initialization
//   3. Use returned structs to populate session data
//   4. No cleanup needed - pure functions
//
// Public API:
//   - LoadUserConfig(username string) (*UserConfig, error)
//   - LoadInstanceConfig(instanceID string) (*InstanceConfig, error)
//   - LoadProjectConfig(projectID string) (*ProjectConfig, error)
//   - GetSessionContext(username, instanceID, projectID string) (*SessionContext, error)
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: encoding/json, fmt, os, path/filepath, strings
//   External: None
//   Internal: None (foundational library)
//   Data Files: config/user/*.jsonc, config/instance/*.jsonc, config/project/*.jsonc
//
// Dependents (What Uses This):
//   Commands: session-time, session-log (session initialization)
//   Libraries: activity logger, hooks (inherit session context)
//   Tools: Any system component needing identity information
//
// Integration Points:
//   - Ladder: Foundational rung - no dependencies on other libraries
//   - Baton: Takes username/instance/project IDs, returns config structs
//   - Rails: N/A (utility library, not infrastructure)
//
// Health Scoring
//
// Health Scoring Map (Base100):
//   Config Loading Operations (Total = 100):
//     +30: Load user config successfully
//     +30: Load instance config successfully
//     +20: Load project config successfully (or gracefully handle missing)
//     +20: Merge configs into session context successfully
//     -30: Failed to load user config
//     -30: Failed to load instance config
//     -20: Failed to load required project config
//     -20: Failed to merge configs
//
// Visual Health Indicators:
//   💚 90-100: Excellent (all configs loaded, inheritance working)
//   💛 70-89:  Good (user+instance loaded, project optional missing)
//   🧡 50-69:  Acceptable (partial configs loaded, using fallbacks)
//   ❤️  30-49:  Poor (only user config loaded)
//   💀 0-29:   Critical (no configs loaded, hard-coded fallbacks only)

package config

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md

// ────────────────────────────────────────────────────────────────
// Imports - Dependencies
// ────────────────────────────────────────────────────────────────

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"sync"

	"github.com/BurntSushi/toml"
	"system/lib/jsonc"
)

// ────────────────────────────────────────────────────────────────
// Types - Data Structures
// ────────────────────────────────────────────────────────────────

// UserConfig represents a user's identity and preferences
type UserConfig struct {
	Identity struct {
		Name        string `json:"name"`
		Username    string `json:"username"`
		DisplayName string `json:"display_name"`
	} `json:"identity"`

	Workspace struct {
		Organization   string `json:"organization"`
		Role           string `json:"role"`
		PrimaryProject string `json:"primary_project"`
	} `json:"workspace"`

	Preferences struct {
		Timezone string `json:"timezone"`
		Locale   string `json:"locale"`
	} `json:"preferences"`
}

// InstanceConfig represents a CPI-SI instance's identity and preferences
type InstanceConfig struct {
	Identity struct {
		Name        string `json:"name"`
		Username    string `json:"username"`
		DisplayName string `json:"display_name"`
	} `json:"identity"`

	Workspace struct {
		Organization   string `json:"organization"`
		Role           string `json:"role"`
		PrimaryProject string `json:"primary_project"`
	} `json:"workspace"`

	Preferences struct {
		Timezone string `json:"timezone"`
		Locale   string `json:"locale"`
	} `json:"preferences"`

	Thinking struct {
		LearningStyle   string   `json:"learning_style"`
		ProblemSolving  string   `json:"problem_solving"`
		LoveToThinkAbout []string `json:"love_to_think_about"`
	} `json:"thinking"`
}

// ProjectConfig represents a project's context and preferences
type ProjectConfig struct {
	Identity struct {
		ProjectID   string `json:"project_id"`
		ProjectName string `json:"project_name"`
	} `json:"identity"`

	Ownership struct {
		PrimaryUser     string `json:"primary_user"`
		PrimaryInstance string `json:"primary_instance"`
	} `json:"ownership"`

	Context struct {
		WorkspacePath string `json:"workspace_path"`
		RepositoryURL string `json:"repository_url"`
		ProjectType   string `json:"project_type"`
	} `json:"context"`
}

// SessionContext represents merged configuration for session initialization
type SessionContext struct {
	// Core identity (from configs)
	UserID     string
	InstanceID string
	ProjectID  string

	// User context
	UserName     string
	UserTimezone string

	// Instance context
	InstanceName string

	// Instance thinking (from instance config)
	LearningStyle    string
	ProblemSolving   string
	LoveToThinkAbout []string

	// Work context
	WorkContext string
	ProjectType string

	// Session preferences (inherited)
	Timezone string
}

// PathsConfig represents system paths loaded from paths.toml
type PathsConfig struct {
	Session struct {
		StateFile    string `toml:"state_file"`
		PatternsFile string `toml:"patterns_file"`
		SchemaDir    string `toml:"schema_dir"`
		HistoryDir   string `toml:"history_dir"`
	} `toml:"session"`

	Temporal struct {
		CalendarBase      string `toml:"calendar_base"`
		CalendarPersonal  string `toml:"calendar_personal"`
		CalendarShared    string `toml:"calendar_shared"`
		CalendarProjects  string `toml:"calendar_projects"`
		CalendarMilestones string `toml:"calendar_milestones"`
		CalendarTemplates string `toml:"calendar_templates"`
		CalendarSchemas   string `toml:"calendar_schemas"`
		CelestialDir      string `toml:"celestial_dir"`
		SolarData         string `toml:"solar_data"`
		LunarData         string `toml:"lunar_data"`
		LocationConfig    string `toml:"location_config"`
		PatternsDir       string `toml:"patterns_dir"`
		PatternsUser      string `toml:"patterns_user"`
		PatternsInstance  string `toml:"patterns_instance"`
		PatternsDiscovered string `toml:"patterns_discovered"`
		PatternsSchemas   string `toml:"patterns_schemas"`
		PlannerTemplates  string `toml:"planner_templates"`
		PlannerSchemas    string `toml:"planner_schemas"`
	} `toml:"temporal"`
}

// SystemConfig represents system-level overrides from system.toml
type SystemConfig struct {
	Metadata struct {
		SchemaVersion string `toml:"schema_version"`
	} `toml:"metadata"`

	System struct {
		HostnameOverride string `toml:"hostname_override"`
		OSDisplayName    string `toml:"os_display_name"`
	} `toml:"system"`

	PackageManagement struct {
		Preferred string `toml:"preferred"`
		Fallback  string `toml:"fallback"`
	} `toml:"package_management"`

	Shell struct {
		Preferred string `toml:"preferred"`
	} `toml:"shell"`

	Toolchains struct {
		CCompiler   string `toml:"c_compiler"`
		CppCompiler string `toml:"cpp_compiler"`
		Python      string `toml:"python"`
	} `toml:"toolchains"`

	Paths struct {
		TempOverride          string   `toml:"temp_override"`
		CacheOverride         string   `toml:"cache_override"`
		AdditionalBinPaths    []string `toml:"additional_bin_paths"`
	} `toml:"paths"`

	Features struct {
		Docker           string `toml:"docker"`
		Bluetooth        string `toml:"bluetooth"`
		GPUAcceleration  string `toml:"gpu_acceleration"`
		Systemd          string `toml:"systemd"`
		Graphical        string `toml:"graphical"`
		Network          string `toml:"network"`
	} `toml:"features"`

	Limits struct {
		MaxCPUCores      int `toml:"max_cpu_cores"`
		MaxMemoryGB      int `toml:"max_memory_gb"`
		MaxIOParallelism int `toml:"max_io_parallelism"`
	} `toml:"limits"`

	Network struct {
		PreferredInterface string   `toml:"preferred_interface"`
		HTTPProxy          string   `toml:"http_proxy"`
		HTTPSProxy         string   `toml:"https_proxy"`
		NoProxy            []string `toml:"no_proxy"`
	} `toml:"network"`

	Display struct {
		PreferredDisplay string `toml:"preferred_display"`
		ForceResolution  string `toml:"force_resolution"`
	} `toml:"display"`

	Power struct {
		BatteryMode string `toml:"battery_mode"`
		ACMode      string `toml:"ac_mode"`
	} `toml:"power"`

	Locale struct {
		LangOverride   string `toml:"lang_override"`
		LCAllOverride  string `toml:"lc_all_override"`
		Timezone       string `toml:"timezone"`
	} `toml:"locale"`

	Detection struct {
		CacheDurationSeconds   int      `toml:"cache_duration_seconds"`
		HardwareProbePaths     []string `toml:"hardware_probe_paths"`
		CapabilityCommands     []string `toml:"capability_commands"`
		SkipExpensiveDetection bool     `toml:"skip_expensive_detection"`
	} `toml:"detection"`

	Environment struct {
		PackageManagement struct {
			DebianFrontend            string `toml:"debian_frontend"`
			NeedrestartMode           string `toml:"needrestart_mode"`
			NeedrestartSuspend        string `toml:"needrestart_suspend"`
			AptListchangesFrontend    string `toml:"apt_listchanges_frontend"`
			UcfForceConffnew          string `toml:"ucf_force_conffnew"`
			PipNoInput                string `toml:"pip_no_input"`
			PipDisablePipVersionCheck string `toml:"pip_disable_pip_version_check"`
			Pythondontwritebytecode   string `toml:"pythondontwritebytecode"`
			Pythonunbuffered          string `toml:"pythonunbuffered"`
			NpmConfigYes              string `toml:"npm_config_yes"`
			NpmConfigColor            string `toml:"npm_config_color"`
		} `toml:"package_management"`
		DevelopmentTools struct {
			RustBacktrace         string `toml:"rust_backtrace"`
			CargoTermColor        string `toml:"cargo_term_color"`
			DockerBuildkit        string `toml:"docker_buildkit"`
			ComposeDockerCliBuild string `toml:"compose_docker_cli_build"`
			DockerScanSuggest     string `toml:"docker_scan_suggest"`
			CmakeGenerator        string `toml:"cmake_generator"`
		} `toml:"development_tools"`
		Terminal struct {
			Term           string `toml:"term"`
			Colorterm      string `toml:"colorterm"`
			Lang           string `toml:"lang"`
			LcAll          string `toml:"lc_all"`
			Histcontrol    string `toml:"histcontrol"`
			Histsize       string `toml:"histsize"`
			Histfilesize   string `toml:"histfilesize"`
			Histtimeformat string `toml:"histtimeformat"`
			Less           string `toml:"less"`
			Lesshistfile   string `toml:"lesshistfile"`
		} `toml:"terminal"`
		ClaudeCode struct {
			MaxThinkingTokens string `toml:"max_thinking_tokens"`
			BashMaxTimeoutMs  string `toml:"bash_max_timeout_ms"`
		} `toml:"claude_code"`
	} `toml:"environment"`
}

// UserConfig_TOML represents user-level preferences from user.toml (distinct from JSONC UserConfig)
type UserConfig_TOML struct {
	Metadata struct {
		SchemaVersion   string `toml:"schema_version"`
		SystemReference string `toml:"system_reference"`
	} `toml:"metadata"`

	Paths struct {
		ProjectRootOverride     string   `toml:"project_root_override"`
		WorkspaceOverride       string   `toml:"workspace_override"`
		AdditionalSearchPaths   []string `toml:"additional_search_paths"`
	} `toml:"paths"`

	Shell struct {
		PreferredShell    string            `toml:"preferred_shell"`
		PreferredTerminal string            `toml:"preferred_terminal"`
		Aliases           map[string]string `toml:"aliases"`
	} `toml:"shell"`

	Editor struct {
		Preferred       string   `toml:"preferred"`
		VisualPreferred string   `toml:"visual_preferred"`
		Settings        struct {
			VimSettings  []string `toml:"vim_settings"`
			NanoSettings []string `toml:"nano_settings"`
		} `toml:"settings"`
	} `toml:"editor"`

	Development struct {
		CodeDirOverride string `toml:"code_dir_override"`
		VersionManagers struct {
			UseNVM    string `toml:"use_nvm"`
			UsePyenv  string `toml:"use_pyenv"`
			UseRustup string `toml:"use_rustup"`
		} `toml:"version_managers"`
		PreferredVersions struct {
			Python string `toml:"python"`
			Node   string `toml:"node"`
			Go     string `toml:"go"`
			Rust   string `toml:"rust"`
		} `toml:"preferred_versions"`
	} `toml:"development"`

	Git struct {
		NameOverride   string `toml:"name_override"`
		EmailOverride  string `toml:"email_override"`
		EditorOverride string `toml:"editor_override"`
		SignCommits    string `toml:"sign_commits"`
		SignTags       string `toml:"sign_tags"`
	} `toml:"git"`

	Terminal struct {
		ColorScheme  string `toml:"color_scheme"`
		ColorSupport string `toml:"color_support"`
		FontFamily   string `toml:"font_family"`
		FontSize     int    `toml:"font_size"`
	} `toml:"terminal"`

	Workspace struct {
		OrganizationStyle       string `toml:"organization_style"`
		DefaultLicense          string `toml:"default_license"`
		DefaultGitignoreTemplate string `toml:"default_gitignore_template"`
	} `toml:"workspace"`

	CPISI struct {
		DefaultInstance  string `toml:"default_instance"`
		SkillsDirOverride string `toml:"skills_dir_override"`
		HooksDirOverride string `toml:"hooks_dir_override"`
		Session struct {
			AutoSaveMinutes     int    `toml:"auto_save_minutes"`
			DefaultThinkingMode string `toml:"default_thinking_mode"`
		} `toml:"session"`
	} `toml:"cpi_si"`

	Privacy struct {
		IncludeHomeInSearch  string `toml:"include_home_in_search"`
		IncludeHiddenFiles   string `toml:"include_hidden_files"`
		AllowTelemetry       string `toml:"allow_telemetry"`
	} `toml:"privacy"`

	Accessibility struct {
		ScreenReader  string  `toml:"screen_reader"`
		HighContrast  string  `toml:"high_contrast"`
		FontScale     float64 `toml:"font_scale"`
	} `toml:"accessibility"`

	Notifications struct {
		DesktopNotifications  string `toml:"desktop_notifications"`
		SoundNotifications    string `toml:"sound_notifications"`
		LongCommandThreshold  int    `toml:"long_command_threshold"`
	} `toml:"notifications"`

	Performance struct {
		EnableFileIndexing string `toml:"enable_file_indexing"`
		CacheEnvironment   bool   `toml:"cache_environment"`
		PreferParallel     string `toml:"prefer_parallel"`
	} `toml:"performance"`

	Locale struct {
		PreferredLanguage  string `toml:"preferred_language"`
		PreferredTimezone  string `toml:"preferred_timezone"`
		DateFormat         string `toml:"date_format"`
		TimeFormat         string `toml:"time_format"`
	} `toml:"locale"`

	Backup struct {
		AutoBackup          bool   `toml:"auto_backup"`
		BackupFrequencyDays int    `toml:"backup_frequency_days"`
		BackupLocation      string `toml:"backup_location"`
	} `toml:"backup"`

	Environment struct {
		Editor struct {
			Editor    string `toml:"editor"`
			Visual    string `toml:"visual"`
			GitEditor string `toml:"git_editor"`
		} `toml:"editor"`
		Pager struct {
			Pager    string `toml:"pager"`
			GitPager string `toml:"git_pager"`
		} `toml:"pager"`
		Development struct {
			GOPATH    string `toml:"gopath"`
			NodeEnv   string `toml:"node_env"`
			NVMDir    string `toml:"nvm_dir"`
			PyenvRoot string `toml:"pyenv_root"`
			CargoHome string `toml:"cargo_home"`
			RustupHome string `toml:"rustup_home"`
		} `toml:"development"`
		Sudo struct {
			SudoAskpass string `toml:"sudo_askpass"`
		} `toml:"sudo"`
		SSHGPG struct {
			SSHAuthSock string `toml:"ssh_auth_sock"`
			GPGTty      string `toml:"gpg_tty"`
		} `toml:"ssh_gpg"`
		Custom map[string]string `toml:"custom"`
	} `toml:"environment"`
}

// ────────────────────────────────────────────────────────────────
// Globals - Package State
// ────────────────────────────────────────────────────────────────

// Paths configuration (loaded once, cached)
var (
	pathsConfig     *PathsConfig
	pathsConfigOnce sync.Once
	pathsLoaded     bool
)

// System configuration (loaded once, cached)
var (
	systemConfig     *SystemConfig
	systemConfigOnce sync.Once
	systemLoaded     bool
)

// User configuration TOML (loaded once, cached)
var (
	userConfigTOML     *UserConfig_TOML
	userConfigTOMLOnce sync.Once
	userTOMLLoaded     bool
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md

// ────────────────────────────────────────────────────────────────
// Config Loading
// ────────────────────────────────────────────────────────────────

// getConfigRoot returns the root config directory path
func getConfigRoot() (string, error) {
	home := os.Getenv("HOME")
	if home == "" {
		var err error
		home, err = os.UserHomeDir()
		if err != nil {
			return "", fmt.Errorf("failed to get home directory: %w", err)
		}
	}

	return filepath.Join(home, ".claude/cpi-si/config"), nil
}

// LoadUserConfig loads user configuration from config/user/<username>/config.jsonc
func LoadUserConfig(username string) (*UserConfig, error) {
	configRoot, err := getConfigRoot()
	if err != nil {
		return nil, err
	}

	configPath := filepath.Join(configRoot, "user", username, "config.jsonc")
	data, err := os.ReadFile(configPath)
	if err != nil {
		return nil, fmt.Errorf("failed to read user config: %w", err)
	}

	// Strip JSONC comments
	cleaned := jsonc.StripComments(data)

	var userCfg UserConfig
	if err := json.Unmarshal(cleaned, &userCfg); err != nil {
		return nil, fmt.Errorf("failed to parse user config: %w", err)
	}

	return &userCfg, nil
}

// LoadInstanceConfig loads instance configuration from config/instance/<instance_id>/config.jsonc
func LoadInstanceConfig(instanceID string) (*InstanceConfig, error) {
	configRoot, err := getConfigRoot()
	if err != nil {
		return nil, err
	}

	configPath := filepath.Join(configRoot, "instance", instanceID, "config.jsonc")
	data, err := os.ReadFile(configPath)
	if err != nil {
		return nil, fmt.Errorf("failed to read instance config: %w", err)
	}

	// Strip JSONC comments
	cleaned := jsonc.StripComments(data)

	var instanceCfg InstanceConfig
	if err := json.Unmarshal(cleaned, &instanceCfg); err != nil {
		return nil, fmt.Errorf("failed to parse instance config: %w", err)
	}

	return &instanceCfg, nil
}

// LoadProjectConfig loads project configuration from config/project/<project_id>/config.jsonc
// Returns nil if project config doesn't exist (graceful handling)
func LoadProjectConfig(projectID string) (*ProjectConfig, error) {
	if projectID == "" {
		return nil, nil // No project ID - not an error, just no project config
	}

	configRoot, err := getConfigRoot()
	if err != nil {
		return nil, err
	}

	configPath := filepath.Join(configRoot, "project", projectID, "config.jsonc")
	data, err := os.ReadFile(configPath)
	if err != nil {
		// Project config is optional - return nil without error
		if os.IsNotExist(err) {
			return nil, nil
		}
		return nil, fmt.Errorf("failed to read project config: %w", err)
	}

	// Strip JSONC comments
	cleaned := jsonc.StripComments(data)

	var projectCfg ProjectConfig
	if err := json.Unmarshal(cleaned, &projectCfg); err != nil {
		return nil, fmt.Errorf("failed to parse project config: %w", err)
	}

	return &projectCfg, nil
}

// ────────────────────────────────────────────────────────────────
// Config Inheritance and Merging
// ────────────────────────────────────────────────────────────────

// GetSessionContext loads and merges all configs into session context
// This implements the inheritance chain: User → Instance → Project → Session
func GetSessionContext(username, instanceID, projectID string) (*SessionContext, error) {
	// Load user config (required)
	userCfg, err := LoadUserConfig(username)
	if err != nil {
		return nil, fmt.Errorf("failed to load user config: %w", err)
	}

	// Load instance config (required)
	instanceCfg, err := LoadInstanceConfig(instanceID)
	if err != nil {
		return nil, fmt.Errorf("failed to load instance config: %w", err)
	}

	// Load project config (optional)
	projectCfg, err := LoadProjectConfig(projectID)
	if err != nil {
		return nil, fmt.Errorf("failed to load project config: %w", err)
	}

	// Merge configs into session context
	ctx := &SessionContext{
		// Core identity
		UserID:     userCfg.Identity.Username,
		InstanceID: instanceCfg.Identity.Username,
		ProjectID:  projectID,

		// User context
		UserName:     userCfg.Identity.Name,
		UserTimezone: userCfg.Preferences.Timezone,

		// Instance context
		InstanceName: instanceCfg.Identity.Name,

		// Instance thinking (from instance config)
		LearningStyle:    instanceCfg.Thinking.LearningStyle,
		ProblemSolving:   instanceCfg.Thinking.ProblemSolving,
		LoveToThinkAbout: instanceCfg.Thinking.LoveToThinkAbout,

		// Default timezone (from user, can be overridden by instance)
		Timezone: userCfg.Preferences.Timezone,
	}

	// If instance has timezone preference, use that
	if instanceCfg.Preferences.Timezone != "" {
		ctx.Timezone = instanceCfg.Preferences.Timezone
	}

	// If project config exists, add project context
	if projectCfg != nil {
		ctx.WorkContext = projectCfg.Context.WorkspacePath
		ctx.ProjectType = projectCfg.Context.ProjectType
		ctx.ProjectID = projectCfg.Identity.ProjectID
	}

	// If no project config, try environment variable for work context
	if ctx.WorkContext == "" {
		ctx.WorkContext = os.Getenv("NOVA_DAWN_WORKSPACE")
	}

	return ctx, nil
}

// ────────────────────────────────────────────────────────────────
// Paths Loading
// ────────────────────────────────────────────────────────────────

// loadPathsConfig loads paths.toml configuration (singleton pattern)
func loadPathsConfig() {
	pathsConfigOnce.Do(func() {
		home := os.Getenv("HOME")
		if home == "" {
			var err error
			home, err = os.UserHomeDir()
			if err != nil {
				// Can't load config without home directory
				return
			}
		}

		configPath := filepath.Join(home, ".claude/cpi-si/system/config/paths.toml")
		var cfg PathsConfig
		if _, err := toml.DecodeFile(configPath, &cfg); err != nil {
			// Failed to load - pathsLoaded remains false, callers use fallback
			return
		}

		pathsConfig = &cfg
		pathsLoaded = true
	})
}

// LoadPaths returns the paths configuration loaded from paths.toml
//
// Returns:
//   *PathsConfig - Paths configuration structure
//   error - nil on success, error if loading fails
//
// Behavior:
//   1. Loads paths.toml on first call (singleton pattern)
//   2. Returns cached config on subsequent calls
//   3. Returns error if config failed to load
func LoadPaths() (*PathsConfig, error) {
	loadPathsConfig()

	if !pathsLoaded {
		return nil, fmt.Errorf("failed to load paths configuration")
	}

	return pathsConfig, nil
}

// GetSessionPath returns the full path to session state file
//
// Returns:
//   string - Full path to ~/.claude/cpi-si/system/data/session/current.json
//
// Behavior:
//   1. Loads paths config
//   2. Joins home dir with config path
//   3. Returns full absolute path
func GetSessionPath() (string, error) {
	paths, err := LoadPaths()
	if err != nil {
		return "", err
	}

	home := os.Getenv("HOME")
	if home == "" {
		home, err = os.UserHomeDir()
		if err != nil {
			return "", fmt.Errorf("failed to get home directory: %w", err)
		}
	}

	return filepath.Join(home, ".claude/cpi-si", paths.Session.StateFile), nil
}

// GetSessionPatternsPath returns the full path to session patterns file
//
// Returns:
//   string - Full path to ~/.claude/cpi-si/system/data/session/patterns.json
//
// Behavior:
//   1. Loads paths config
//   2. Joins home dir with config path
//   3. Returns full absolute path
func GetSessionPatternsPath() (string, error) {
	paths, err := LoadPaths()
	if err != nil {
		return "", err
	}

	home := os.Getenv("HOME")
	if home == "" {
		home, err = os.UserHomeDir()
		if err != nil {
			return "", fmt.Errorf("failed to get home directory: %w", err)
		}
	}

	return filepath.Join(home, ".claude/cpi-si", paths.Session.PatternsFile), nil
}

// ────────────────────────────────────────────────────────────────
// System and User TOML Loading
// ────────────────────────────────────────────────────────────────

// loadSystemConfig loads system.toml configuration (singleton pattern)
func loadSystemConfig() {
	systemConfigOnce.Do(func() {
		home := os.Getenv("HOME")
		if home == "" {
			var err error
			home, err = os.UserHomeDir()
			if err != nil {
				return
			}
		}

		configPath := filepath.Join(home, ".claude/cpi-si/system/config/system.toml")
		var cfg SystemConfig
		if _, err := toml.DecodeFile(configPath, &cfg); err != nil {
			// Failed to load - systemLoaded remains false
			return
		}

		systemConfig = &cfg
		systemLoaded = true
	})
}

// LoadSystemConfig returns the system configuration loaded from system.toml
//
// Returns:
//   *SystemConfig - System configuration structure
//   error - nil on success, error if loading fails
//
// Behavior:
//   1. Loads system.toml on first call (singleton pattern)
//   2. Returns cached config on subsequent calls
//   3. Returns error if config failed to load
func LoadSystemConfig() (*SystemConfig, error) {
	loadSystemConfig()

	if !systemLoaded {
		return nil, fmt.Errorf("failed to load system configuration")
	}

	return systemConfig, nil
}

// loadUserConfigTOML loads user.toml configuration (singleton pattern)
func loadUserConfigTOML() {
	userConfigTOMLOnce.Do(func() {
		home := os.Getenv("HOME")
		if home == "" {
			var err error
			home, err = os.UserHomeDir()
			if err != nil {
				fmt.Fprintf(os.Stderr, "userconfig TOML: failed to get home: %v\n", err)
				return
			}
		}

		configPath := filepath.Join(home, ".claude/cpi-si/system/config/user.toml")
		var cfg UserConfig_TOML
		if _, err := toml.DecodeFile(configPath, &cfg); err != nil {
			// Failed to load - userTOMLLoaded remains false
			fmt.Fprintf(os.Stderr, "userconfig TOML: failed to decode %s: %v\n", configPath, err)
			return
		}

		userConfigTOML = &cfg
		userTOMLLoaded = true
	})
}

// LoadUserConfigTOML returns the user TOML configuration loaded from user.toml
//
// Returns:
//   *UserConfig_TOML - User TOML configuration structure
//   error - nil on success, error if loading fails
//
// Behavior:
//   1. Loads user.toml on first call (singleton pattern)
//   2. Returns cached config on subsequent calls
//   3. Returns error if config failed to load
//
// Note: This is distinct from LoadUserConfig which loads JSONC user identity.
//       user.toml = technical preferences/overrides (TOML)
//       user config.jsonc = identity and personhood (JSONC)
func LoadUserConfigTOML() (*UserConfig_TOML, error) {
	loadUserConfigTOML()

	if !userTOMLLoaded {
		return nil, fmt.Errorf("failed to load user TOML configuration")
	}

	return userConfigTOML, nil
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md

// ────────────────────────────────────────────────────────────────
// Validation
// ────────────────────────────────────────────────────────────────
// Config validation happens during JSON unmarshaling (fails if malformed).
// Required configs (user, instance) return errors if missing.
// Optional configs (project) return nil gracefully if missing.

// ────────────────────────────────────────────────────────────────
// Execution
// ────────────────────────────────────────────────────────────────
// N/A - Pure function library, no execution entry point.

// ────────────────────────────────────────────────────────────────
// Cleanup
// ────────────────────────────────────────────────────────────────
// N/A - Stateless library, no cleanup needed.

// ────────────────────────────────────────────────────────────────
// FINAL DOCUMENTATION
// ────────────────────────────────────────────────────────────────

// Package Exports:
//   - LoadUserConfig(username string) (*UserConfig, error)
//   - LoadInstanceConfig(instanceID string) (*InstanceConfig, error)
//   - LoadProjectConfig(projectID string) (*ProjectConfig, error)
//   - GetSessionContext(username, instanceID, projectID string) (*SessionContext, error)
//   - LoadPaths() (*PathsConfig, error)
//   - GetSessionPath() (string, error)
//   - GetSessionPatternsPath() (string, error)
//   - LoadSystemConfig() (*SystemConfig, error)
//   - LoadUserConfigTOML() (*UserConfig_TOML, error)
//
// All functions are pure and stateless. JSONC comments are automatically stripped.
// Config loading failures return errors for caller to handle.
//
// Config Inheritance Chain:
//   User Config (who is the human?)
//     ↓
//   Instance Config (who is the CPI-SI instance?)
//     ↓
//   Project Config (what's the work context?) [optional]
//     ↓
//   Session Context (merged configuration for session initialization)
//
// Usage Pattern:
//   1. Session initialization calls GetSessionContext()
//   2. Session context populates current.json and current-log.json
//   3. Activity logger inherits from session context
//   4. Consolidation analyzes behavior vs config expectations
//   5. Learning validates and refines configs
//
// Example:
//
//	import "system/lib/config"
//
//	// Get merged session context (inherits from all configs)
//	ctx, err := config.GetSessionContext("seanje-lenox-wise", "nova_dawn", "")
//	if err != nil {
//		log.Fatalf("Failed to load config: %v", err)
//	}
//
//	// Use context to initialize session
//	sessionState := SessionState{
//		SessionID:  generateSessionID(),
//		InstanceID: ctx.InstanceID,
//		UserID:     ctx.UserID,
//		WorkContext: ctx.WorkContext,
//		// ... other fields from ctx
//	}

// ============================================================================
// END CLOSING
// ============================================================================
