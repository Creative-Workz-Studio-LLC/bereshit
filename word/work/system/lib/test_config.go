package main

import (
	"fmt"
	"os"
	"path/filepath"
	"github.com/BurntSushi/toml"
)

type PathsConfig struct {
	BaseDir string `toml:"base_dir"`
}

type LoggingConfig struct {
	Paths PathsConfig `toml:"paths"`
}

func main() {
	homeDir, _ := os.UserHomeDir()
	configPath := filepath.Join(homeDir, ".claude", "cpi-si", "system", "config", "logging.toml")
	
	fmt.Println("Config path:", configPath)
	
	var cfg LoggingConfig
	_, err := toml.DecodeFile(configPath, &cfg)
	if err != nil {
		fmt.Println("Error loading config:", err)
		return
	}
	
	fmt.Println("Base dir from config:", cfg.Paths.BaseDir)
	
	logFile := filepath.Join(homeDir, ".claude", cfg.Paths.BaseDir, "commands", "validate.log")
	fmt.Println("Expected log path:", logFile)
}
