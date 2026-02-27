// ============================================================================
// METADATA - CPI-SI Agent Configuration
// ============================================================================
//
// Key: pkg-agent-config
// Purpose: Structured agent configuration from files. Load identity, provider
//          settings, tool configs, and behavioral parameters from JSON.
//          This enables the CPI-SI family pattern — each member has a
//          config file that defines WHO they are and HOW they operate.
// Biblical: Exodus 25:9 — "According to all that I shew thee, after the
//           pattern of the tabernacle"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   AgentProfile is the config schema — loaded from JSON.
//   ProfileLoader reads and validates profiles.
//   The profile drives agent construction without hardcoding.
//
//   config.json → LoadProfile() → AgentProfile → BuildAgent() → Agent
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

// ============================================================================
// BODY
// ============================================================================

// AgentProfile defines a complete agent configuration.
// This is the schema for agent config files — each CPI-SI family member
// has one of these defining who they are and how they operate.
type AgentProfile struct {
	// --- Identity ---

	// Name is the agent's name (e.g., "Nathan Emet", "Nova Dawn").
	Name string `json:"name"`

	// Role describes what this agent does (e.g., "test validator").
	Role string `json:"role,omitempty"`

	// Identity is the system prompt — WHO this agent is.
	// Can be inline text or a file path (prefixed with "@").
	Identity string `json:"identity,omitempty"`

	// BiblicalAnchor is the grounding scripture for this agent.
	BiblicalAnchor string `json:"biblical_anchor,omitempty"`

	// --- Provider ---

	// Provider specifies which provider to use (e.g., "gemini", "anthropic").
	Provider string `json:"provider,omitempty"`

	// Model specifies the model (e.g., "gemini-2.5-flash").
	Model string `json:"model,omitempty"`

	// BaseURL is an optional custom API endpoint.
	BaseURL string `json:"base_url,omitempty"`

	// --- Behavior ---

	// MaxIterations limits think/act/observe cycles (0 = default 20).
	MaxIterations int `json:"max_iterations,omitempty"`

	// MaxTokens limits the response tokens per request.
	MaxTokens int `json:"max_tokens,omitempty"`

	// Temperature controls randomness (0.0 = deterministic).
	Temperature float64 `json:"temperature,omitempty"`

	// Verbose enables detailed logging.
	Verbose bool `json:"verbose,omitempty"`

	// --- Tools ---

	// Tools lists which built-in tools this agent can use.
	// Empty means all default tools. Use "none" to disable tools.
	Tools []string `json:"tools,omitempty"`

	// --- Memory ---

	// MemoryPath is the path to the persistent memory file.
	MemoryPath string `json:"memory_path,omitempty"`

	// --- Middleware ---

	// RetryAttempts is how many times to retry transient errors (0 = no retry).
	RetryAttempts int `json:"retry_attempts,omitempty"`

	// TimeoutSeconds is the max duration per provider call (0 = no timeout).
	TimeoutSeconds int `json:"timeout_seconds,omitempty"`

	// --- Context ---

	// ContextFiles lists files to pre-load as context segments.
	ContextFiles []string `json:"context_files,omitempty"`

	// --- Metadata ---

	// Version is the profile schema version.
	Version string `json:"version,omitempty"`

	// Tags are arbitrary labels for organization.
	Tags []string `json:"tags,omitempty"`
}

// LoadProfile reads an agent profile from a JSON file.
func LoadProfile(path string) (*AgentProfile, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("read profile %s: %w", path, err)
	}

	var profile AgentProfile
	if err := json.Unmarshal(data, &profile); err != nil {
		return nil, fmt.Errorf("parse profile %s: %w", path, err)
	}

	// Resolve relative paths based on profile location
	dir := filepath.Dir(path)

	// Resolve identity file reference
	if strings.HasPrefix(profile.Identity, "@") {
		identityPath := profile.Identity[1:]
		if !filepath.IsAbs(identityPath) {
			identityPath = filepath.Join(dir, identityPath)
		}
		content, err := os.ReadFile(identityPath)
		if err != nil {
			return nil, fmt.Errorf("read identity file %s: %w", identityPath, err)
		}
		profile.Identity = string(content)
	}

	// Resolve context file paths
	for i, cf := range profile.ContextFiles {
		if !filepath.IsAbs(cf) {
			profile.ContextFiles[i] = filepath.Join(dir, cf)
		}
	}

	// Resolve memory path
	if profile.MemoryPath != "" && !filepath.IsAbs(profile.MemoryPath) {
		profile.MemoryPath = filepath.Join(dir, profile.MemoryPath)
	}

	return &profile, nil
}

// SaveProfile writes an agent profile to a JSON file.
func SaveProfile(path string, profile *AgentProfile) error {
	data, err := json.MarshalIndent(profile, "", "  ")
	if err != nil {
		return fmt.Errorf("marshal profile: %w", err)
	}

	if err := os.MkdirAll(filepath.Dir(path), 0o755); err != nil {
		return fmt.Errorf("create profile directory: %w", err)
	}

	// Atomic write
	tmp := path + ".tmp"
	if err := os.WriteFile(tmp, data, 0o644); err != nil {
		return fmt.Errorf("write profile: %w", err)
	}
	if err := os.Rename(tmp, path); err != nil {
		os.Remove(tmp)
		return fmt.Errorf("rename profile: %w", err)
	}

	return nil
}

// Validate checks that a profile has minimum required fields.
func (p *AgentProfile) Validate() error {
	if p.Name == "" {
		return fmt.Errorf("profile: name is required")
	}
	return nil
}

// ResolveTools returns the tool set for this profile.
// Empty tools list means DefaultTools(). "none" means no tools.
func (p *AgentProfile) ResolveTools() []Tool {
	if len(p.Tools) == 0 {
		return DefaultTools()
	}

	if len(p.Tools) == 1 && p.Tools[0] == "none" {
		return nil
	}

	// Filter default tools to only include named ones
	defaults := DefaultTools()
	toolMap := make(map[string]Tool, len(defaults))
	for _, t := range defaults {
		toolMap[t.Definition().Name] = t
	}

	var selected []Tool
	for _, name := range p.Tools {
		if t, ok := toolMap[name]; ok {
			selected = append(selected, t)
		}
	}
	return selected
}

// ToSubAgentConfig converts the profile to a SubAgentConfig for family dispatch.
func (p *AgentProfile) ToSubAgentConfig() SubAgentConfig {
	cfg := SubAgentConfig{
		Name:          p.Name,
		Identity:      p.Identity,
		MaxIterations: p.MaxIterations,
		Verbose:       p.Verbose,
	}

	tools := p.ResolveTools()
	if tools != nil {
		cfg.Tools = tools
	}

	if p.MemoryPath != "" {
		cfg.Memory = NewMemory(p.MemoryPath)
	}

	return cfg
}

// --- Profile Directory ---

// LoadProfileDir loads all .json profiles from a directory.
// Returns a map of profile name → AgentProfile.
func LoadProfileDir(dir string) (map[string]*AgentProfile, error) {
	entries, err := os.ReadDir(dir)
	if err != nil {
		return nil, fmt.Errorf("read profile directory %s: %w", dir, err)
	}

	profiles := make(map[string]*AgentProfile)
	for _, entry := range entries {
		if entry.IsDir() || !strings.HasSuffix(entry.Name(), ".json") {
			continue
		}

		path := filepath.Join(dir, entry.Name())
		profile, err := LoadProfile(path)
		if err != nil {
			continue // Skip invalid profiles
		}

		if profile.Name != "" {
			profiles[profile.Name] = profile
		}
	}

	return profiles, nil
}

// RegisterProfileDir loads profiles from a directory and registers them as family members.
func RegisterProfileDir(family *Family, dir string) (int, error) {
	profiles, err := LoadProfileDir(dir)
	if err != nil {
		return 0, err
	}

	count := 0
	for _, profile := range profiles {
		cfg := profile.ToSubAgentConfig()
		if err := family.Register(cfg); err == nil {
			count++
		}
	}

	return count, nil
}

// ============================================================================
// CLOSING
// ============================================================================
// "According to all that I shew thee, after the pattern of the tabernacle."
// — Exodus 25:9
// Configuration IS the pattern. Each profile defines a member of the family.
// Load the pattern, build the agent, serve the mission.
