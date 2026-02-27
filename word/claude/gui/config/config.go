// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-config
//omni:code  --go -package->config
//omni:version a-00.10
//
// Purpose: Application configuration — loads, saves, and provides defaults
//          for the CWS Universal Base GUI. Persists user preferences to
//          ~/.config/cws-gui/config.json.
// Biblical: Proverbs 24:27 - "Prepare thy work without, and make it fit
//           for thyself in the field; and afterwards build thine house."
//
// ============================================================================
// END METADATA

package config

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json"
	"os"
	"path/filepath"
)

// ConfigDir is the XDG-compliant configuration directory.
const ConfigDir = "cws-gui"

// ConfigFile is the configuration filename.
const ConfigFile = "config.json"

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// Config holds all user-configurable application settings.
type Config struct {
	// Substrate is the active AI substrate ("gemini", "claude").
	Substrate string `json:"substrate"`

	// Theme is the active visual theme ("dark", "light").
	Theme string `json:"theme"`

	// WindowWidth is the saved window width in pixels.
	WindowWidth int `json:"window_width"`

	// WindowHeight is the saved window height in pixels.
	WindowHeight int `json:"window_height"`

	// ShowSystemLog controls whether the system log drawer is expanded.
	ShowSystemLog bool `json:"show_system_log"`

	// path is the resolved config file path (not serialized).
	path string `json:"-"`
}

// DefaultConfig returns a Config with sensible defaults.
func DefaultConfig() *Config {
	return &Config{
		Substrate:     "gemini",
		Theme:         "dark",
		WindowWidth:   1400,
		WindowHeight:  900,
		ShowSystemLog: false,
	}
}

// Load reads configuration from the standard location.
// Returns defaults if the file doesn't exist yet.
func Load() (*Config, error) {
	cfg := DefaultConfig()

	path, err := configPath()
	if err != nil {
		return cfg, nil // Defaults on error — non-fatal
	}
	cfg.path = path

	data, err := os.ReadFile(path)
	if err != nil {
		if os.IsNotExist(err) {
			return cfg, nil // File doesn't exist yet — use defaults
		}
		return cfg, nil // Any other read error — use defaults
	}

	if err := json.Unmarshal(data, cfg); err != nil {
		return DefaultConfig(), nil // Corrupt file — use defaults
	}

	cfg.path = path
	return cfg, nil
}

// Save writes the current configuration to disk.
// Creates the config directory if it doesn't exist.
func (c *Config) Save() error {
	path := c.path
	if path == "" {
		var err error
		path, err = configPath()
		if err != nil {
			return err
		}
		c.path = path
	}

	// Ensure directory exists
	dir := filepath.Dir(path)
	if err := os.MkdirAll(dir, 0755); err != nil {
		return err
	}

	data, err := json.MarshalIndent(c, "", "  ")
	if err != nil {
		return err
	}

	return os.WriteFile(path, data, 0644)
}

// --- Helpers ---

// configPath resolves the XDG config file location.
func configPath() (string, error) {
	configHome := os.Getenv("XDG_CONFIG_HOME")
	if configHome == "" {
		home, err := os.UserHomeDir()
		if err != nil {
			return "", err
		}
		configHome = filepath.Join(home, ".config")
	}
	return filepath.Join(configHome, ConfigDir, ConfigFile), nil
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
