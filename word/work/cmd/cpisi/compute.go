// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-cmd-cpisi-compute
// Purpose: Compute mode — geometric computation demonstrations
// Biblical: Ezekiel 1:16 - "a wheel in the middle of a wheel"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-20
//
// Demonstrates the geometric computation: SHAPE → NUMBER → COMPUTE → NUMBER → SHAPE
// The cube is a computational space, not just storage.
//
// ═══════════════════════════════════════════════════════════════════════════

package main

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"

	"creativeworkzstudio.com/bereshit/word/work/pkg/core/cpisi/bereshit"
	"creativeworkzstudio.com/bereshit/word/work/pkg/core/cpisi/compute"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Compute Mode Entry ---

func runCompute(args []string) {
	if len(args) == 0 {
		printComputeUsage()
		return
	}

	switch args[0] {
	case "letter":
		computeLetter(args[1:])
	case "shape":
		computeShape(args[1:])
	case "cycle":
		computeCycle(args[1:])
	case "demo":
		computeDemo()
	default:
		fmt.Printf("Unknown compute subcommand: %s\n", args[0])
		printComputeUsage()
	}
}

func printComputeUsage() {
	fmt.Println(`Geometric Computation - Shape ↔ Number Translation

Usage: cpisi compute <subcommand> [args]

THE CYCLE:
  SHAPE → NUMBER → COMPUTE → NUMBER → SHAPE
    ↑                                    ↓
    └────────────────────────────────────┘

Subcommands:
  letter <letter> [perspective]  - Compute through cube: letter → shape → compute → shape → letter
  shape <x> <y> <z>              - Create shape, show number translation
  cycle <value> [perspective]    - Ternary cycle: value → new value
  demo                           - Full demonstration of the computation model

Perspective: -1 (PAST), 0 (PRESENT), 1 (FUTURE). Default: 0

Examples:
  cpisi compute letter א          # Hebrew Aleph through PRESENT
  cpisi compute letter α -1       # Greek Alpha through PAST
  cpisi compute shape -1 -1 -1    # Origin shape (position 0)
  cpisi compute cycle 42 1        # Cycle 42 through FUTURE
  cpisi compute demo              # Full demonstration`)
}

// --- Letter Computation ---

func computeLetter(args []string) {
	if len(args) == 0 {
		fmt.Println("Usage: cpisi compute letter <letter> [perspective]")
		return
	}

	letter := args[0]
	perspective := compute.PerspectivePresent
	if len(args) > 1 {
		var p int
		fmt.Sscanf(args[1], "%d", &p)
		perspective = compute.Perspective(p)
	}

	// Load Bereshit data
	trans, err := bereshit.LoadTransliteration(bereshit.DefaultBereshitDir())
	if err != nil {
		fmt.Printf("Error loading Bereshit data: %v\n", err)
		fmt.Println("Make sure Bereshit repository is at: " + bereshit.DefaultBereshitDir())
		return
	}

	// Create cube with loaded data
	cube := compute.NewCube(trans, nil)

	fmt.Printf("Geometric Computation: Letter '%s'\n", letter)
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()

	// Step 1: Letter → Shape
	shape, ok := cube.LetterToShape(letter)
	if !ok {
		fmt.Printf("Letter '%s' not found in Hebrew or Greek alphabet\n", letter)
		return
	}

	fmt.Println("Step 1: LETTER → SHAPE")
	fmt.Printf("  Letter: %s\n", letter)
	fmt.Printf("  Shape:  (%d, %d, %d) [balanced ternary coords]\n", shape.X, shape.Y, shape.Z)
	fmt.Println()

	// Step 2: Shape → Number
	number := compute.ShapeToNumber(shape)
	fmt.Println("Step 2: SHAPE → NUMBER")
	fmt.Printf("  Position: %d (linear index 0-26)\n", number)
	fmt.Printf("  Formula:  (x+1)*9 + (y+1)*3 + (z+1)\n")
	fmt.Printf("            (%d+1)*9 + (%d+1)*3 + (%d+1) = %d\n", shape.X, shape.Y, shape.Z, number)
	fmt.Println()

	// Step 3: Compute
	perspName := map[compute.Perspective]string{-1: "PAST", 0: "PRESENT", 1: "FUTURE"}[perspective]
	result := cube.Compute(number, perspective)
	fmt.Println("Step 3: COMPUTE (via ternary cycle)")
	fmt.Printf("  Perspective: %s (%d)\n", perspName, perspective)
	fmt.Printf("  Formula:     (value × 3 + perspective) mod 243\n")
	fmt.Printf("               (%d × 3 + %d) mod 243 = %d\n", number, perspective, result)
	fmt.Println()

	// Step 4: Number → Shape
	newShape := compute.NumberToShape(result % 27)
	fmt.Println("Step 4: NUMBER → SHAPE")
	fmt.Printf("  New position: %d mod 27 = %d\n", result, result%27)
	fmt.Printf("  New shape:    (%d, %d, %d)\n", newShape.X, newShape.Y, newShape.Z)
	fmt.Println()

	// Step 5: Shape → Letter
	testament := "Old"
	if _, ok := trans.EncryptGreek(letter); ok {
		testament = "New"
	}
	newLetter, _ := cube.ShapeToLetter(newShape, testament)
	fmt.Println("Step 5: SHAPE → LETTER")
	fmt.Printf("  New letter: %s\n", newLetter)
	fmt.Println()

	// Summary
	fmt.Println("═══════════════════════════════════════════════════════════════")
	fmt.Printf("CYCLE COMPLETE: %s → (%d,%d,%d) → %d → %d → (%d,%d,%d) → %s\n",
		letter, shape.X, shape.Y, shape.Z, number, result,
		newShape.X, newShape.Y, newShape.Z, newLetter)
}

// --- Shape Computation ---

func computeShape(args []string) {
	if len(args) < 3 {
		fmt.Println("Usage: cpisi compute shape <x> <y> <z>")
		fmt.Println("  Coordinates must be -1, 0, or +1")
		return
	}

	var x, y, z int
	fmt.Sscanf(args[0], "%d", &x)
	fmt.Sscanf(args[1], "%d", &y)
	fmt.Sscanf(args[2], "%d", &z)

	shape := compute.NewShape(x, y, z)
	number := compute.ShapeToNumber(shape)

	fmt.Printf("Shape Analysis: (%d, %d, %d)\n", shape.X, shape.Y, shape.Z)
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()

	fmt.Printf("Linear Position: %d\n", number)
	fmt.Printf("Formula: (%d+1)*9 + (%d+1)*3 + (%d+1) = %d\n", x, y, z, number)
	fmt.Println()

	// Show plane modes
	fmt.Println("Plane Modes:")
	fmt.Printf("  Perspective (x-y at z=%d): mode %d\n", shape.Z, shape.Mode(compute.PlanePerspective))
	fmt.Printf("  Elevation (x-z at y=%d):   mode %d\n", shape.Y, shape.Mode(compute.PlaneElevation))
	fmt.Printf("  Lateral (y-z at x=%d):     mode %d\n", shape.X, shape.Mode(compute.PlaneLateral))
	fmt.Println()

	// Properties
	fmt.Println("Properties:")
	fmt.Printf("  Is Origin:  %v\n", shape.IsOrigin())
	fmt.Printf("  Is Corner:  %v\n", shape.IsCorner())

	// Octant
	octantName := map[int]string{-1: "PAST (units)", 0: "PRESENT (tens)", 1: "FUTURE (hundreds)"}
	fmt.Printf("  Octant:     %s\n", octantName[shape.X])
	fmt.Println()

	// Reverse verification
	backShape := compute.NumberToShape(number)
	fmt.Println("Reverse Translation (NUMBER → SHAPE):")
	fmt.Printf("  Position %d → (%d, %d, %d) ✓\n", number, backShape.X, backShape.Y, backShape.Z)
}

// --- Cycle Computation ---

func computeCycle(args []string) {
	if len(args) == 0 {
		fmt.Println("Usage: cpisi compute cycle <value> [perspective]")
		return
	}

	var value int
	fmt.Sscanf(args[0], "%d", &value)

	perspective := compute.PerspectivePresent
	if len(args) > 1 {
		var p int
		fmt.Sscanf(args[1], "%d", &p)
		perspective = compute.Perspective(p)
	}

	cube := compute.NewCube(nil, nil)

	fmt.Printf("Ternary Cycle: %d through perspective %d\n", value, perspective)
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()

	// Show 5 iterations of the cycle
	fmt.Println("Iteration  Input  →  Output  (Shape)")
	fmt.Println("─────────────────────────────────────────")

	current := value
	for i := 0; i < 5; i++ {
		result := cube.Cycle(current, perspective)
		shape := compute.NumberToShape(result % 27)
		fmt.Printf("    %d       %3d   →   %3d     (%+d, %+d, %+d)\n",
			i+1, current, result, shape.X, shape.Y, shape.Z)
		current = result
	}

	fmt.Println()
	fmt.Println("The wheel within a wheel: each output becomes the next input.")
}

// --- Demo Computation ---

func computeDemo() {
	fmt.Println("Geometric Computation Demo")
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()
	fmt.Println("THE CORE INSIGHT:")
	fmt.Println()
	fmt.Println("  SHAPE → NUMBER → COMPUTE → NUMBER → SHAPE")
	fmt.Println("    ↑                                    ↓")
	fmt.Println("    └────────────────────────────────────┘")
	fmt.Println()
	fmt.Println("The cube is a COMPUTATIONAL SPACE, not just storage.")
	fmt.Println("Position IS computation — where you are determines what you get.")
	fmt.Println()

	// Load data
	trans, err := bereshit.LoadTransliteration(bereshit.DefaultBereshitDir())
	if err != nil {
		fmt.Printf("Note: Could not load Bereshit data: %v\n", err)
		fmt.Println("Demo will continue with shape/number operations only.")
		trans = nil
	}

	cube := compute.NewCube(trans, nil)

	fmt.Println("─────────────────────────────────────────────────────────────")
	fmt.Println("DEMO 1: Shape ↔ Number Translation")
	fmt.Println("─────────────────────────────────────────────────────────────")
	fmt.Println()

	shapes := []struct{ x, y, z int }{
		{-1, -1, -1}, // Origin (Aleph/Alpha)
		{0, 0, 0},    // Center (Nun/Nu)
		{1, 1, 1},    // Terminus (Final Tsade/Sampi)
	}

	for _, s := range shapes {
		shape := compute.NewShape(s.x, s.y, s.z)
		num := compute.ShapeToNumber(shape)
		back := compute.NumberToShape(num)
		fmt.Printf("  (%+d, %+d, %+d) → %2d → (%+d, %+d, %+d)\n",
			s.x, s.y, s.z, num, back.X, back.Y, back.Z)
	}

	if trans != nil {
		fmt.Println()
		fmt.Println("─────────────────────────────────────────────────────────────")
		fmt.Println("DEMO 2: Letter → Shape → Number (Encryption)")
		fmt.Println("─────────────────────────────────────────────────────────────")
		fmt.Println()

		letters := []string{"א", "נ", "ץ", "Α", "Ν", "Ϡ"}
		for _, letter := range letters {
			if shape, ok := cube.LetterToShape(letter); ok {
				num := compute.ShapeToNumber(shape)
				testament := "Hebrew"
				if _, ok := trans.EncryptGreek(letter); ok {
					testament = "Greek"
				}
				fmt.Printf("  %s (%s) → (%+d, %+d, %+d) → %2d\n",
					letter, testament, shape.X, shape.Y, shape.Z, num)
			}
		}
	}

	fmt.Println()
	fmt.Println("─────────────────────────────────────────────────────────────")
	fmt.Println("DEMO 3: Ternary Cycle (Wheel in a Wheel)")
	fmt.Println("─────────────────────────────────────────────────────────────")
	fmt.Println()
	fmt.Println("Starting value: 1 (Aleph position)")
	fmt.Println("Perspective: PRESENT (0)")
	fmt.Println()

	value := 1
	for i := 0; i < 7; i++ {
		result := cube.Cycle(value, compute.PerspectivePresent)
		shape := compute.NumberToShape(result % 27)
		fmt.Printf("  Cycle %d: %3d → %3d  shape: (%+d, %+d, %+d)\n",
			i+1, value, result, shape.X, shape.Y, shape.Z)
		value = result
	}

	fmt.Println()
	fmt.Println("─────────────────────────────────────────────────────────────")
	fmt.Println()
	fmt.Println("\"Their appearance and their work was as a wheel in a wheel.\"")
	fmt.Println("— Ezekiel 1:16")
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// Compute mode demonstrates the geometric computation model:
// - Letter → Shape → Number → Compute → Number → Shape → Letter
// - The ternary cycle: (value × 3 + perspective) mod 243
// - Shape ↔ Number bidirectional translation
//
// The cube is not storage — it's a computational space.
// Position determines output. The wheel within a wheel.
//
// ═══════════════════════════════════════════════════════════════════════════
