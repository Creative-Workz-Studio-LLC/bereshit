// ============================================================================
// METADATA - Documentation Skill Configuration
// ============================================================================
// Purpose: Load and provide access to documentation-config.toml
// Biblical: Proverbs 25:2 - "the honour of kings is to search out a matter"

package config

import (
	"os"

	pkgtoml "cws.studio/pkg/util/fs/toml"
)

// ============================================================================
// SETUP - Types
// ============================================================================

// Config represents the full documentation configuration
type Config struct {
	Quality      Quality                `toml:"quality"`
	Types        map[string]DocType     `toml:"types"`
	DecisionTree DecisionTree           `toml:"decision_tree"`
	Blocks       map[string]Block       `toml:"blocks"`
	Toolkit      map[string]ToolkitItem `toml:"toolkit"`
	Reading      map[string]ReadingLevel `toml:"reading"`
}

// Quality defines the documentation quality standard
type Quality struct {
	Standard    string            `toml:"standard"`
	Dimensions  map[string]string `toml:"dimensions"`
	Insight     string            `toml:"insight"`
}

// DocType represents a document type configuration
type DocType struct {
	Name        string `toml:"name"`
	Description string `toml:"description"`
	Template    string `toml:"template"`
	Strictness  string `toml:"strictness"`
	WhenToUse   string `toml:"when_to_use"`
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

// Block represents a 5-block documentation block
type Block struct {
	Name     string   `toml:"name"`
	Purpose  string   `toml:"purpose"`
	Contains []string `toml:"contains"`
}

// ToolkitItem represents an AsciiDoc toolkit element
type ToolkitItem struct {
	Name      string `toml:"name"`
	Syntax    string `toml:"syntax"`
	WhenToUse string `toml:"when_to_use"`
}

// ReadingLevel represents a layered reading level
type ReadingLevel struct {
	Name    string `toml:"name"`
	Time    string `toml:"time"`
	Focus   string `toml:"focus"`
	Purpose string `toml:"purpose"`
}

// ============================================================================
// BODY - Configuration Loading
// ============================================================================

// Load reads the configuration from documentation-config.toml
func Load() (*Config, error) {
	searchPaths := pkgtoml.SkillConfigPaths("create-documentation")

	configPath, err := pkgtoml.FindConfig("documentation-config.toml", searchPaths)
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

// GetTypeNames returns all document type names
func (c *Config) GetTypeNames() []string {
	names := make([]string, 0, len(c.Types))
	for name := range c.Types {
		names = append(names, name)
	}
	return names
}

// GetType returns a specific document type
func (c *Config) GetType(name string) (DocType, bool) {
	dt, ok := c.Types[name]
	return dt, ok
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

// GetBlockNames returns all block names in order
func (c *Config) GetBlockNames() []string {
	return []string{"metadata", "header", "context", "content", "footer"}
}

// GetBlock returns a specific block definition
func (c *Config) GetBlock(name string) (Block, bool) {
	b, ok := c.Blocks[name]
	return b, ok
}
