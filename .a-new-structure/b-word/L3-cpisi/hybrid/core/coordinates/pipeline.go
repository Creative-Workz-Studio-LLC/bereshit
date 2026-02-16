//omni:code --go -library
//omni:key B-L3-coordinates-pipeline
//omni:version a-01.00
// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-coordinates-pipeline
// Purpose: Sentence → Verse → New Sentence encoding pipeline
// Biblical: Isaiah 55:11 - "So shall my word be that goeth forth out of my mouth"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-20
//
// The encoding/decoding pipeline that grounds CPI-SI in Scripture:
//   Input sentence → words → letter sums → verse coordinates → Scripture text
//   Scripture text → key concepts → new grounded sentence
//
// ═══════════════════════════════════════════════════════════════════════════

package coordinates

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"os"
	"path/filepath"
	"regexp"
	"strings"
	"unicode"
)

// --- Pipeline Types ---

// Pipeline holds all loaded data for sentence encoding
type Pipeline struct {
	Bible      *BibleIndex
	Data       *BibleData
	ScriptDir  string
}

// WordAnalysis holds the encoding result for a single word
type WordAnalysis struct {
	Word       string       // Original word
	LetterSum  int          // A=1, B=2, ..., Z=26
	VerseNum   int          // Mapped verse (1-31)
	Reference  string       // Full reference (e.g., "Genesis 1:3")
	Coords     *BibleCoords // Coordinate mapping
	VerseText  string       // Actual Scripture text
}

// SentenceAnalysis holds the full pipeline result
type SentenceAnalysis struct {
	Original    string          // Original input sentence
	Words       []*WordAnalysis // Analysis per word
	GroundedIn  []string        // Scripture references used
	NewSentence string          // Generated grounded sentence
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Pipeline Creation ---

// NewPipeline creates a new encoding pipeline with loaded data
func NewPipeline(bereshitDir string) (*Pipeline, error) {
	// Load Bible index for coordinate lookups
	scriptDir := filepath.Join(bereshitDir, "word/scripture")
	bible := NewBibleIndex(scriptDir)

	// Load TOML data
	data, err := LoadBibleData(bereshitDir)
	if err != nil {
		return nil, fmt.Errorf("load bible data: %w", err)
	}

	return &Pipeline{
		Bible:     bible,
		Data:      data,
		ScriptDir: scriptDir,
	}, nil
}

// --- Sentence Parsing ---

// ParseSentence breaks a sentence into analyzable words
func (p *Pipeline) ParseSentence(sentence string) []string {
	// Remove punctuation, convert to lowercase for analysis
	reg := regexp.MustCompile(`[^a-zA-Z\s]`)
	clean := reg.ReplaceAllString(sentence, "")

	// Split into words
	words := strings.Fields(clean)

	return words
}

// --- Word Analysis ---

// AnalyzeWord processes a single word through the encoding pipeline
func (p *Pipeline) AnalyzeWord(word string) *WordAnalysis {
	upperWord := strings.ToUpper(word)

	// Step 1: Letter sum
	sum := letterSum(upperWord)

	// Step 2: Map to verse (mod 31 + 1 = Genesis 1:1-31)
	verseNum := (sum % 31) + 1

	// Step 3: Get reference and coordinates
	ref := BibleReference{Book: "Genesis", Chapter: 1, Verse: verseNum}
	coords, _ := p.Bible.ToCoords(ref)

	// Step 4: Load verse text
	text := p.loadVerseText(ref)

	return &WordAnalysis{
		Word:      word,
		LetterSum: sum,
		VerseNum:  verseNum,
		Reference: fmt.Sprintf("Genesis 1:%d", verseNum),
		Coords:    &coords,
		VerseText: text,
	}
}

// letterSum calculates A=1, B=2, ..., Z=26 sum
func letterSum(word string) int {
	sum := 0
	for _, c := range word {
		if unicode.IsLetter(c) {
			// Convert to uppercase and get position
			upper := unicode.ToUpper(c)
			if upper >= 'A' && upper <= 'Z' {
				sum += int(upper - 'A' + 1)
			}
		}
	}
	return sum
}

// loadVerseText reads the actual verse text from Scripture files
func (p *Pipeline) loadVerseText(ref BibleReference) string {
	// Path: word/scripture/KJV/Genesis/Chapter_1/Verse_1.txt
	path := filepath.Join(
		p.ScriptDir,
		"KJV",
		ref.Book,
		fmt.Sprintf("Chapter_%d", ref.Chapter),
		fmt.Sprintf("Verse_%d.txt", ref.Verse),
	)

	content, err := os.ReadFile(path)
	if err != nil {
		return "(verse not found)"
	}

	return strings.TrimSpace(string(content))
}

// --- Full Sentence Analysis ---

// Encode processes a full sentence through the pipeline
func (p *Pipeline) Encode(sentence string) *SentenceAnalysis {
	words := p.ParseSentence(sentence)

	analysis := &SentenceAnalysis{
		Original:   sentence,
		Words:      make([]*WordAnalysis, 0, len(words)),
		GroundedIn: make([]string, 0),
	}

	// Track unique verses used
	seenRefs := make(map[string]bool)

	for _, word := range words {
		wa := p.AnalyzeWord(word)
		analysis.Words = append(analysis.Words, wa)

		if !seenRefs[wa.Reference] {
			seenRefs[wa.Reference] = true
			analysis.GroundedIn = append(analysis.GroundedIn, wa.Reference)
		}
	}

	// Generate grounded sentence
	analysis.NewSentence = p.generateGroundedSentence(analysis)

	return analysis
}

// --- Intent Recognition ---

// IntentType represents what kind of input this is
type IntentType string

const (
	IntentGreeting  IntentType = "greeting"
	IntentQuestion  IntentType = "question"
	IntentStatement IntentType = "statement"
	IntentRequest   IntentType = "request"
	IntentFarewell  IntentType = "farewell"
)

// Intent holds recognized intent and grounded concepts
type Intent struct {
	Type     IntentType
	Concepts []string // Grounded concepts extracted from words
}

// RecognizeIntent determines what kind of input this is
func RecognizeIntent(sentence string) Intent {
	lower := strings.ToLower(sentence)
	words := strings.Fields(lower)

	intent := Intent{
		Type:     IntentStatement, // default
		Concepts: make([]string, 0),
	}

	// Check for greetings
	greetings := []string{"hello", "hi", "hey", "greetings", "peace", "shalom"}
	for _, g := range greetings {
		if strings.Contains(lower, g) {
			intent.Type = IntentGreeting
			intent.Concepts = append(intent.Concepts, "peace", "blessing")
			break
		}
	}

	// Check for farewells
	farewells := []string{"goodbye", "bye", "farewell", "later", "see you"}
	for _, f := range farewells {
		if strings.Contains(lower, f) {
			intent.Type = IntentFarewell
			intent.Concepts = append(intent.Concepts, "peace", "blessing", "journey")
			break
		}
	}

	// Check for questions
	if strings.HasSuffix(strings.TrimSpace(sentence), "?") {
		intent.Type = IntentQuestion
		intent.Concepts = append(intent.Concepts, "wisdom", "understanding")
	}
	questionWords := []string{"what", "why", "how", "when", "where", "who", "can", "could", "would", "should"}
	for _, q := range questionWords {
		if len(words) > 0 && words[0] == q {
			intent.Type = IntentQuestion
			intent.Concepts = append(intent.Concepts, "wisdom", "understanding")
			break
		}
	}

	// Check for requests
	requestWords := []string{"please", "help", "can you", "could you", "would you"}
	for _, r := range requestWords {
		if strings.Contains(lower, r) {
			intent.Type = IntentRequest
			intent.Concepts = append(intent.Concepts, "service", "help")
			break
		}
	}

	// Add concepts based on key words found
	conceptMap := map[string]string{
		"god":      "creator",
		"create":   "creation",
		"created":  "creation",
		"world":    "creation",
		"earth":    "creation",
		"heaven":   "glory",
		"heavens":  "glory",
		"light":    "truth",
		"dark":     "void",
		"good":     "goodness",
		"love":     "love",
		"truth":    "truth",
		"life":     "life",
		"man":      "humanity",
		"woman":    "humanity",
		"people":   "humanity",
	}

	for _, word := range words {
		if concept, ok := conceptMap[word]; ok {
			// Avoid duplicates
			found := false
			for _, c := range intent.Concepts {
				if c == concept {
					found = true
					break
				}
			}
			if !found {
				intent.Concepts = append(intent.Concepts, concept)
			}
		}
	}

	return intent
}

// --- Sentence Generation ---

// generateGroundedSentence creates a natural response from grounded concepts
func (p *Pipeline) generateGroundedSentence(analysis *SentenceAnalysis) string {
	if len(analysis.Words) == 0 {
		return ""
	}

	// Recognize intent
	intent := RecognizeIntent(analysis.Original)

	// Generate based on intent type
	switch intent.Type {
	case IntentGreeting:
		return generateGreetingResponse(intent.Concepts)
	case IntentFarewell:
		return generateFarewellResponse(intent.Concepts)
	case IntentQuestion:
		return generateQuestionResponse(analysis, intent.Concepts)
	case IntentRequest:
		return generateRequestResponse(intent.Concepts)
	default:
		return generateStatementResponse(analysis, intent.Concepts)
	}
}

func generateGreetingResponse(concepts []string) string {
	responses := []string{
		"Peace to you.",
		"Grace and peace.",
		"Peace be with you.",
		"Blessings to you.",
	}
	// Pick based on concepts
	if containsConcept(concepts, "blessing") {
		return "Peace and blessing to you."
	}
	return responses[0]
}

func generateFarewellResponse(concepts []string) string {
	responses := []string{
		"Go in peace.",
		"Grace be with you.",
		"Peace on your journey.",
	}
	if containsConcept(concepts, "journey") {
		return "Go in peace, and may your path be blessed."
	}
	return responses[0]
}

func generateQuestionResponse(analysis *SentenceAnalysis, concepts []string) string {
	// Extract core theme from verses
	themes := extractThemes(analysis)

	if len(themes) == 0 {
		return "Wisdom begins with understanding."
	}

	// Natural response incorporating themes
	if containsConcept(concepts, "wisdom") {
		return fmt.Sprintf("Consider this: %s.", themes[0])
	}
	return themes[0] + "."
}

func generateRequestResponse(concepts []string) string {
	if containsConcept(concepts, "help") {
		return "I'm here to help."
	}
	return "Let's work on this together."
}

func generateStatementResponse(analysis *SentenceAnalysis, concepts []string) string {
	themes := extractThemes(analysis)

	if len(themes) == 0 {
		if len(concepts) > 0 {
			return conceptToAffirmation(concepts[0])
		}
		return "Indeed."
	}

	// Build natural response from themes
	if len(themes) == 1 {
		return themes[0] + "."
	}

	// Combine themes naturally
	if len(themes) == 2 {
		return fmt.Sprintf("%s, and %s.", themes[0], strings.ToLower(themes[1]))
	}

	// Multiple themes - pick most relevant
	return themes[0] + "."
}

func containsConcept(concepts []string, target string) bool {
	for _, c := range concepts {
		if c == target {
			return true
		}
	}
	return false
}

func conceptToAffirmation(concept string) string {
	affirmations := map[string]string{
		"creation":  "Creation reflects the Creator's glory.",
		"peace":     "Peace is the foundation.",
		"blessing":  "Blessings flow from the source.",
		"wisdom":    "Wisdom guides the path.",
		"truth":     "Truth illuminates.",
		"love":      "Love is the greatest.",
		"life":      "Life is a gift.",
		"goodness":  "It is good.",
		"glory":     "Glory to God.",
		"creator":   "In the beginning, God.",
		"humanity":  "Made in His image.",
		"service":   "To serve is to honor.",
		"help":      "Help comes from above.",
	}
	if a, ok := affirmations[concept]; ok {
		return a
	}
	return "Indeed."
}

// extractThemes pulls meaningful themes from verse analysis
func extractThemes(analysis *SentenceAnalysis) []string {
	themes := make([]string, 0)
	seen := make(map[string]bool)

	for _, wa := range analysis.Words {
		if wa.VerseText == "" || wa.VerseText == "(verse not found)" {
			continue
		}

		theme := extractTheme(wa.VerseText)
		if theme != "" && !seen[strings.ToLower(theme)] {
			seen[strings.ToLower(theme)] = true
			themes = append(themes, theme)
		}
	}

	return themes
}

// extractTheme gets a clean theme from a verse
func extractTheme(verse string) string {
	// Remove BOM if present
	verse = strings.TrimPrefix(verse, "\ufeff")

	// Key patterns → natural themes
	patterns := []struct {
		contains string
		theme    string
	}{
		{"In the beginning", "in the beginning, God created"},
		{"God created", "God creates"},
		{"Let there be light", "light shines in darkness"},
		{"it was good", "it is good"},
		{"very good", "it is very good"},
		{"Be fruitful", "be fruitful and multiply"},
		{"God blessed", "blessing flows"},
		{"God called", "God names and defines"},
		{"God made", "God makes"},
		{"God saw", "God sees and approves"},
		{"image of God", "made in God's image"},
		{"dominion", "stewardship is given"},
		{"evening and the morning", "each day has its purpose"},
	}

	verseLower := strings.ToLower(verse)
	for _, p := range patterns {
		if strings.Contains(verseLower, strings.ToLower(p.contains)) {
			return p.theme
		}
	}

	return ""
}

// --- Display Helpers ---

// String returns a formatted string of the analysis
func (a *SentenceAnalysis) String() string {
	var sb strings.Builder

	sb.WriteString(fmt.Sprintf("Original: %s\n\n", a.Original))
	sb.WriteString("Word Analysis:\n")
	for _, wa := range a.Words {
		sb.WriteString(fmt.Sprintf("  %-12s sum=%3d → %s\n", wa.Word, wa.LetterSum, wa.Reference))
	}

	sb.WriteString(fmt.Sprintf("\nGrounded in: %s\n", strings.Join(a.GroundedIn, ", ")))
	sb.WriteString(fmt.Sprintf("\nNew Sentence: %s\n", a.NewSentence))

	return sb.String()
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// The encoding pipeline:
//
//   Input Sentence
//        ↓
//   Parse into words
//        ↓
//   For each word:
//     → Letter sum (A=1...Z=26)
//     → Map to verse (mod 31 + 1)
//     → Get coordinates
//     → Load verse text
//        ↓
//   Extract key phrases
//        ↓
//   Generate grounded sentence
//        ↓
//   Output: New sentence rooted in Scripture
//
// "So shall my word be that goeth forth out of my mouth: it shall not
//  return unto me void, but it shall accomplish that which I please."
//  — Isaiah 55:11
//
// ═══════════════════════════════════════════════════════════════════════════
