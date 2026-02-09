// ============================================================================
// METADATA - Data Skill Configuration
// ============================================================================
// Purpose: Load and provide access to data-config.toml
// Biblical: Proverbs 25:2 - "the honour of kings is to search out a matter"

package config

import (
	"os"

	pkgtoml "cws.studio/pkg/util/fs/toml"
)

// ============================================================================
// SETUP - Types
// ============================================================================

// Config represents the full data configuration
type Config struct {
	Formats      map[string]DataFormat  `toml:"formats"`
	DecisionTree DecisionTree           `toml:"decision_tree"`
	Blocks       map[string]Block       `toml:"blocks"`
	Workflow     Workflow               `toml:"workflow"`
}

// DataFormat represents a data format configuration
type DataFormat struct {
	Name        string   `toml:"name"`
	Extension   string   `toml:"extension"`
	Description string   `toml:"description"`
	Template    string   `toml:"template"`
	UseWhen     []string `toml:"use_when"`
	CommentStyle string  `toml:"comment_style"`
}

// DecisionTree holds the decision tree configuration
type DecisionTree struct {
	Root  string                  `toml:"root"`
	Nodes map[string]DecisionNode `toml:"-"`
}

// DecisionNode represents a node in the decision tree
type DecisionNode struct {
	Question string   `toml:"question"`
	Options  []Option `toml:"options"`
}

// Option represents a decision tree option
type Option struct {
	Answer string `toml:"answer"`
	Next   string `toml:"next"`
}

// Block represents a 3-block data block
type Block struct {
	Name     string   `toml:"name"`
	Purpose  string   `toml:"purpose"`
	Contains []string `toml:"contains"`
}

// Workflow defines the creation workflow
type Workflow struct {
	BlockByBlock BlockByBlock `toml:"block_by_block"`
}

// BlockByBlock defines block-by-block workflow
type BlockByBlock struct {
	Principle string   `toml:"principle"`
	Why       []string `toml:"why"`
}

// ============================================================================
// BODY - Configuration Loading
// ============================================================================

// Load reads the configuration from data-config.toml
func Load() (*Config, error) {
	searchPaths := pkgtoml.SkillConfigPaths("create-data")

	configPath, err := pkgtoml.FindConfig("data-config.toml", searchPaths)
	if err != nil {
		return nil, err
	}

	data, err := os.ReadFile(configPath)
	if err != nil {
		return nil, err
	}

	var cfg Config
	if err := pkgtoml.LoadBytes(data, &cfg); err != nil {
		return nil, err
	}

	// Load decision tree nodes from raw TOML
	cfg.DecisionTree.Nodes = make(map[string]DecisionNode)
	var raw map[string]interface{}
	if err := pkgtoml.LoadBytes(data, &raw); err != nil {
		return nil, err
	}

	if dt, ok := raw["decision_tree"].(map[string]interface{}); ok {
		for key, val := range dt {
			if key == "root" {
				continue
			}
			if nodeMap, ok := val.(map[string]interface{}); ok {
				node := DecisionNode{}
				if q, ok := nodeMap["question"].(string); ok {
					node.Question = q
				}
				if opts, ok := nodeMap["options"].([]interface{}); ok {
					for _, opt := range opts {
						if optMap, ok := opt.(map[string]interface{}); ok {
							option := Option{}
							if a, ok := optMap["answer"].(string); ok {
								option.Answer = a
							}
							if n, ok := optMap["next"].(string); ok {
								option.Next = n
							}
							node.Options = append(node.Options, option)
						}
					}
				}
				cfg.DecisionTree.Nodes[key] = node
			}
		}
	}

	return &cfg, nil
}

// ============================================================================
// BODY - Accessor Methods
// ============================================================================

// GetFormatNames returns all data format names
func (c *Config) GetFormatNames() []string {
	names := make([]string, 0, len(c.Formats))
	for name := range c.Formats {
		names = append(names, name)
	}
	return names
}

// GetFormat returns a specific data format
func (c *Config) GetFormat(name string) (DataFormat, bool) {
	df, ok := c.Formats[name]
	return df, ok
}

// GetDecisionNode returns a decision tree node
func (c *Config) GetDecisionNode(name string) (DecisionNode, bool) {
	node, ok := c.DecisionTree.Nodes[name]
	return node, ok
}

// GetRootNode returns the root decision tree node
func (c *Config) GetRootNode() (DecisionNode, bool) {
	return c.GetDecisionNode(c.DecisionTree.Root)
}

// GetBlockNames returns all block names in order (3-block)
func (c *Config) GetBlockNames() []string {
	return []string{"metadata", "content", "closing"}
}

// GetBlock returns a specific block definition
func (c *Config) GetBlock(name string) (Block, bool) {
	b, ok := c.Blocks[name]
	return b, ok
}
