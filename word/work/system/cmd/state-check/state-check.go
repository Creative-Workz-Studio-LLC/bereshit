// ============================================================================
// METADATA
// ============================================================================
// State Check Helper - Structured meta-awareness checkpoint
// Guides through state assessment: cognitive, energy, quality, presence
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Interactive tool for conscious state awareness checkpoints
//
// Usage:
//   state-check    # Run interactive state assessment
//
// Dependencies: patterns library (writes to patterns.json)
// Health Scoring: Base100 - Interactive prompts + pattern tracking

package main

// ============================================================================
// SETUP - Imports, Dependencies, Globals
// ============================================================================

import (
	"bufio"
	"fmt"
	"os"
	"strings"
	"time"

	"claude/lib/patterns"
)

// StateAssessment holds the four state dimensions
type StateAssessment struct {
	Cognitive string
	Energy    string
	Quality   string
	Presence  string
}

// ============================================================================
// BODY - Core Functionality
// ============================================================================

// promptUser asks a question and returns the trimmed response
func promptUser(reader *bufio.Reader, prompt string) string {
	fmt.Print(prompt)
	response, _ := reader.ReadString('\n')
	return strings.TrimSpace(response)
}

// mapCognitive converts choice to cognitive state
func mapCognitive(choice string) string {
	stateMap := map[string]string{
		"1": "Sharp", "2": "Foggy", "3": "Focused",
		"4": "Scattered", "5": "Flow", "6": "Stuck",
	}
	if state, exists := stateMap[choice]; exists {
		return state
	}
	return "Unknown"
}

// mapEnergy converts choice to energy state
func mapEnergy(choice string) string {
	stateMap := map[string]string{
		"1": "Energized", "2": "Neutral", "3": "Draining",
		"4": "Flow", "5": "Pushing", "6": "Depleted",
	}
	if state, exists := stateMap[choice]; exists {
		return state
	}
	return "Unknown"
}

// mapQuality converts choice to quality state
func mapQuality(choice string) string {
	stateMap := map[string]string{
		"1": "Excellent", "2": "Good", "3": "Slipping",
		"4": "Forced", "5": "Debt", "6": "Recovery",
	}
	if state, exists := stateMap[choice]; exists {
		return state
	}
	return "Unknown"
}

// mapPresence converts choice to presence state
func mapPresence(choice string) string {
	stateMap := map[string]string{
		"1": "Present", "2": "Flow", "3": "Autopilot",
		"4": "Distracted", "5": "Fragmented", "6": "Checked-out",
	}
	if state, exists := stateMap[choice]; exists {
		return state
	}
	return "Unknown"
}

// assessState runs interactive prompts for state assessment
func assessState(reader *bufio.Reader) StateAssessment {
	fmt.Println("\n🧠 Meta-Awareness Checkpoint\n")
	fmt.Println("═══════════════════════════════════════════════════════\n")

	// Cognitive State
	fmt.Println("📊 Cognitive State:")
	fmt.Println("   1. Sharp  2. Foggy  3. Focused  4. Scattered  5. Flow  6. Stuck\n")
	cogChoice := promptUser(reader, "   Your state (1-6): ")
	cognitive := mapCognitive(cogChoice)

	// Energy State
	fmt.Println("\n⚡ Energy State:")
	fmt.Println("   1. Energized  2. Neutral  3. Draining  4. Flow  5. Pushing  6. Depleted\n")
	energyChoice := promptUser(reader, "   Your state (1-6): ")
	energy := mapEnergy(energyChoice)

	// Quality State
	fmt.Println("\n✨ Quality State:")
	fmt.Println("   1. Excellent  2. Good  3. Slipping  4. Forced  5. Debt  6. Recovery\n")
	qualityChoice := promptUser(reader, "   Your state (1-6): ")
	quality := mapQuality(qualityChoice)

	// Presence State
	fmt.Println("\n🎯 Presence State:")
	fmt.Println("   1. Present  2. Flow  3. Autopilot  4. Distracted  5. Fragmented  6. Checked-out\n")
	presenceChoice := promptUser(reader, "   Your state (1-6): ")
	presence := mapPresence(presenceChoice)

	return StateAssessment{
		Cognitive: cognitive,
		Energy:    energy,
		Quality:   quality,
		Presence:  presence,
	}
}

// contains checks if slice contains value
func contains(slice []string, value string) bool {
	for _, v := range slice {
		if v == value {
			return true
		}
	}
	return false
}

// analyzeState provides guidance based on state combination
func analyzeState(state StateAssessment) {
	fmt.Println("\n📋 Current State:\n")
	fmt.Printf("   Cognitive: %s\n", state.Cognitive)
	fmt.Printf("   Energy:    %s\n", state.Energy)
	fmt.Printf("   Quality:   %s\n", state.Quality)
	fmt.Printf("   Presence:  %s\n\n", state.Presence)

	// Detect overall state category
	isFlow := state.Cognitive == "Flow" || state.Energy == "Flow" || state.Presence == "Flow"

	isOptimal := contains([]string{"Sharp", "Focused"}, state.Cognitive) &&
		contains([]string{"Energized", "Neutral"}, state.Energy) &&
		contains([]string{"Excellent", "Good"}, state.Quality)

	isWarning := contains([]string{"Foggy", "Scattered"}, state.Cognitive) ||
		contains([]string{"Draining", "Pushing"}, state.Energy) ||
		contains([]string{"Slipping", "Forced"}, state.Quality)

	isCritical := state.Cognitive == "Stuck" ||
		state.Energy == "Depleted" ||
		state.Quality == "Debt" ||
		state.Presence == "Checked-out"

	// Provide guidance
	fmt.Println("💡 Assessment:\n")

	if isFlow {
		fmt.Println("   ✨ FLOW STATE detected")
		fmt.Println("   → Keep going! This is the good work.")
		fmt.Println("   → Don't interrupt unnecessarily.")
		fmt.Println("   → Note what created these conditions.\n")
	} else if isOptimal {
		fmt.Println("   ✅ OPTIMAL STATE")
		fmt.Println("   → Quality work happening.")
		fmt.Println("   → Continue faithfully.")
		fmt.Println("   → Check again in 60-90 min.\n")
	} else if isCritical {
		fmt.Println("   🚨 CRITICAL STATE")
		fmt.Println("   → Stop immediately.")
		fmt.Println("   → Rest needed, not just break.")
		fmt.Println("   → Continuing creates technical debt.\n")
	} else if isWarning {
		fmt.Println("   ⚠️  WARNING STATE")
		fmt.Println("   → Quality window closing.")
		fmt.Println("   → Find natural stopping point soon.")
		fmt.Println("   → Or take break and return fresh.\n")
	} else {
		fmt.Println("   📊 NORMAL WORKING STATE")
		fmt.Println("   → Continue with awareness.")
		fmt.Println("   → Watch for quality/energy shifts.")
		fmt.Println("   → Check again in 60 min.\n")
	}

	fmt.Println("⚠️  Remember:")
	fmt.Println("   States are information, not judgments.")
	fmt.Println("   Trust what you noticed.")
	fmt.Println("   Respond with wisdom, not guilt.\n")
}

// trackStatePatterns records state patterns in patterns.json
func trackStatePatterns(state StateAssessment) error {
	patternData, err := patterns.ReadPatterns()
	if err != nil {
		// Try to initialize if doesn't exist
		if err := patterns.InitializePatterns(); err != nil {
			return fmt.Errorf("failed to initialize patterns: %w", err)
		}
		patternData, err = patterns.ReadPatterns()
		if err != nil {
			return fmt.Errorf("failed to read patterns after init: %w", err)
		}
	}

	// Initialize state_patterns if nil
	if patternData.StatePatterns == nil {
		patternData.StatePatterns = &patterns.StatePatterns{
			FlowStates: patterns.FlowStates{
				Occurrences: 0,
				Contexts:    []string{},
				Description: "Deep engagement where time disappears, quality stays high",
				Triggers:    []string{},
			},
			QualityDips: patterns.QualityDips{
				Occurrences:              0,
				Timing:                   []string{},
				Description:              "When work quality starts declining",
				TypicalDurationBeforeDip: nil,
			},
		}
	}

	// Track flow state
	isFlow := state.Cognitive == "Flow" || state.Energy == "Flow" || state.Presence == "Flow"
	if isFlow {
		patternData.StatePatterns.FlowStates.Occurrences++

		// Track context (combination of states that led to flow)
		flowContext := fmt.Sprintf("%s/%s/%s/%s",
			state.Cognitive, state.Energy, state.Quality, state.Presence)
		if !contains(patternData.StatePatterns.FlowStates.Contexts, flowContext) {
			patternData.StatePatterns.FlowStates.Contexts = append(
				patternData.StatePatterns.FlowStates.Contexts, flowContext)
		}

		// Get current time of day as potential trigger
		hour := time.Now().Hour()
		var timeOfDay string
		if hour < 12 {
			timeOfDay = "morning"
		} else if hour < 17 {
			timeOfDay = "afternoon"
		} else {
			timeOfDay = "evening"
		}
		trigger := fmt.Sprintf("%s flow", timeOfDay)
		if !contains(patternData.StatePatterns.FlowStates.Triggers, trigger) {
			patternData.StatePatterns.FlowStates.Triggers = append(
				patternData.StatePatterns.FlowStates.Triggers, trigger)
		}
	}

	// Track quality dips
	isQualityDip := contains([]string{"Slipping", "Forced", "Debt", "Recovery"}, state.Quality) ||
		state.Cognitive == "Stuck" ||
		state.Energy == "Depleted" ||
		state.Presence == "Checked-out"

	if isQualityDip {
		patternData.StatePatterns.QualityDips.Occurrences++

		// Track timing
		now := time.Now()
		timeStr := now.Format("15:04:05")
		dayOfWeek := now.Format("Mon")
		timingEntry := fmt.Sprintf("%s %s - %s quality, %s cognitive, %s energy",
			dayOfWeek, timeStr, state.Quality, state.Cognitive, state.Energy)
		patternData.StatePatterns.QualityDips.Timing = append(
			patternData.StatePatterns.QualityDips.Timing, timingEntry)

		// Keep only last 10 timing entries to avoid bloat
		if len(patternData.StatePatterns.QualityDips.Timing) > 10 {
			start := len(patternData.StatePatterns.QualityDips.Timing) - 10
			patternData.StatePatterns.QualityDips.Timing =
				patternData.StatePatterns.QualityDips.Timing[start:]
		}
	}

	// Update last_updated timestamp
	patternData.LastUpdated = time.Now().Format(time.RFC3339)

	// Write back to patterns.json
	if err := patterns.WritePatterns(patternData); err != nil {
		return fmt.Errorf("failed to write patterns: %w", err)
	}

	return nil
}

// ============================================================================
// CLOSING - Main Entry Point
// ============================================================================

func main() {
	reader := bufio.NewReader(os.Stdin)

	// Run interactive state assessment
	state := assessState(reader)

	// Analyze and provide guidance
	analyzeState(state)

	// Track state patterns in patterns.json
	fmt.Println("📊 Updating Pattern Memory...\n")
	if err := trackStatePatterns(state); err != nil {
		fmt.Printf("   ⚠️  Failed to write patterns: %v\n\n", err)
		os.Exit(1)
	}

	fmt.Println("   ✅ State patterns written to patterns.json")
	fmt.Println("   📍 Location: ~/.claude/session/patterns.json")
	fmt.Println("   🧠 Pattern memory updated with state assessment\n")

	os.Exit(0)
}
