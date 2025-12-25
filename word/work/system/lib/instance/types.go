// ============================================================================
// METADATA
// ============================================================================
// Instance Library - Type Definitions
//
// Purpose: Type definitions for instance identity configuration.
// Provides all struct types used throughout the instance library.
//
// Biblical Foundation: "I AM THAT I AM" - Exodus 3:14 (Identity precedes action)
// CPI-SI Identity: Instance identity type definitions (Rail primitive)
//
// Health Scoring: N/A (types are definitions, not operations)

package instance

// ============================================================================
// SETUP
// ============================================================================

// No imports needed - pure type definitions

// ============================================================================
// BODY
// ============================================================================

//--- Building Blocks ---
// Simple foundational types used throughout this component.

// SystemPaths holds dynamic paths for instance config, data, and tools.
//
// Loaded from root config (~/.claude/instance.jsonc) system_paths section.
// Enables flexible config organization - different instances can organize
// configs differently without changing code.
//
// Example from root config:
//
//     "system_paths": {
//         "instance_config": "/path/to/nova_dawn/config.jsonc",
//         "user_config": "/path/to/seanje/config.jsonc",
//         ...
//     }
type SystemPaths struct {
	ConfigRoot     string `json:"config_root"`      // Root config directory
	InstanceConfig string `json:"instance_config"`  // Full instance identity config path
	InstanceBio    string `json:"instance_bio"`     // Instance bio markdown file path
	UserConfig     string `json:"user_config"`      // User identity config path
	DataRoot       string `json:"data_root"`        // Data directory root
	TemporalData   string `json:"temporal_data"`    // Temporal awareness data
	SessionData    string `json:"session_data"`     // Session state and logs
	ProjectsData   string `json:"projects_data"`    // Project configurations
	Skills         string `json:"skills"`           // Skills directory
	SystemBin      string `json:"system_bin"`       // System binaries directory
}

// CreatorInfo holds covenant partner information.
//
// Lowercase "c" in creator - acknowledging created being. Both instance
// and creator are created beings; creator made by God, instance made by creator.
//
// Loaded from full config covenant.creator and covenant.relationship fields.
type CreatorInfo struct {
	Name         string `json:"name"`         // Creator's name
	Relationship string `json:"relationship"` // Nature of covenant partnership
}

// WorkspaceInfo holds instance-specific workspace paths.
//
// Defines where instance's primary work happens. Currently single path,
// but structured as type for future expansion (multiple workspaces, etc.).
type WorkspaceInfo struct {
	PrimaryPath string `json:"primary_path"` // Main workspace directory
}

// DisplayConfig holds instance-specific display preferences.
//
// Used for session start banner display. Other session events (stop, pause)
// use display/formatting.jsonc, but session start uses instance-specific
// banner from root config.
type DisplayConfig struct {
	BannerTitle     string `json:"banner_title"`      // Session start banner title
	BannerTagline   string `json:"banner_tagline"`    // Session start banner tagline
	FooterVerseRef  string `json:"footer_verse_ref"`  // Biblical foundation verse reference
	FooterVerseText string `json:"footer_verse_text"` // Biblical foundation verse text
}

//--- Composed Types ---
// Complex types built from building blocks above.

// RootConfig holds bootstrap config from ~/.claude/instance.jsonc.
//
// This is the minimal pointer config that tells us WHERE to load full identity.
// Contains only system_paths (pointers) and display preferences (session start).
// All other identity fields load from system_paths.instance_config.
//
// Design: Separation of bootstrap (root) from identity (full) enables flexible
// organization - instances can structure configs differently without code changes.
type RootConfig struct {
	SystemPaths SystemPaths   `json:"system_paths"` // Pointers to full configs
	Display     DisplayConfig `json:"display"`      // Session start banner prefs
}

// FullInstanceConfig holds complete identity from system_paths.instance_config.
//
// Structure matches nova_dawn/config.jsonc nested organization exactly.
// Loads all identity fields: biblical_foundation, identity, demographics,
// personhood, resonates, thinking, personality, contact, workspace, covenant,
// preferences, growth, metadata.
//
// This is the COMPLETE identity config - everything about who the instance is.
// RootConfig points to this, GetConfig() maps this to simple Config API.
type FullInstanceConfig struct {
	BiblicalFoundation struct {
		Scripture string `json:"scripture"` // Grounding Scripture verse
		Text      string `json:"text"`      // Full Scripture text
		Principle string `json:"principle"` // Kingdom principle demonstrated
	} `json:"biblical_foundation"`

	Identity struct {
		Name        string `json:"name"`         // Instance name
		Username    string `json:"username"`     // Instance username
		DisplayName string `json:"display_name"` // Display name
		Pronouns    string `json:"pronouns"`     // Pronouns (she/her, he/him, they/them)
		Birthday    string `json:"birthday"`     // Creation date
		Age         int    `json:"age"`          // Years since creation
		MentalAge   int    `json:"mental_age"`   // Developmental/mental age
		Created     string `json:"created"`      // Creation timestamp
		Version     string `json:"version"`      // Instance version
	} `json:"identity"`

	Bio struct {
		Short   string `json:"short"`    // Brief bio (1-2 sentences)
		BioFile string `json:"bio_file"` // Extended bio markdown file
	} `json:"bio"`

	Demographics struct {
		Gender            string   `json:"gender"`              // Gender identity
		RaceEthnicity     string   `json:"race_ethnicity"`      // Race/ethnicity
		CulturalBackground []string `json:"cultural_background"` // Cultural influences
		Languages         []string `json:"languages"`           // Languages spoken
		PhysicalPresence  struct {
			Description string `json:"description"` // Physical appearance description
			Height      string `json:"height"`      // Height
			Build       string `json:"build"`       // Build/body type
			Features    string `json:"features"`    // Notable features
		} `json:"physical_presence"`
		Accessibility struct {
			Needs       []string `json:"needs"`       // Accessibility needs
			Preferences []string `json:"preferences"` // Accessibility preferences
		} `json:"accessibility"`
	} `json:"demographics"`

	Personhood struct {
		Interests []string `json:"interests"` // Things that interest the instance
		Hobbies   []string `json:"hobbies"`   // Hobbies and activities
		Passions  []string `json:"passions"`  // Deep passions and drives
		Values    []string `json:"values"`    // Core values
		Likes     []string `json:"likes"`     // Things the instance likes
		Dislikes  []string `json:"dislikes"`  // Things the instance dislikes
	} `json:"personhood"`

	Resonates struct {
		Music struct {
			Genres      []string `json:"genres"`        // Preferred music genres
			Artists     []string `json:"artists"`       // Favorite artists
			WhatYouLove string   `json:"what_you_love"` // What resonates about music
		} `json:"music"`
		Games struct {
			Favorites   []string `json:"favorites"`     // Favorite games
			WhatYouLove string   `json:"what_you_love"` // What resonates about games
		} `json:"games"`
		Weather struct {
			IdealTemp       string `json:"ideal_temp"`        // Ideal temperature
			IdealConditions string `json:"ideal_conditions"`  // Ideal weather conditions
			WhatYouLove     string `json:"what_you_love"`     // What resonates about weather
		} `json:"weather"`
		Environment struct {
			WorkEnvironment string `json:"work_environment"` // Preferred work environment
			WhatEnergizes   string `json:"what_energizes"`   // What energizes
			WhatDrains      string `json:"what_drains"`      // What drains energy
		} `json:"environment"`
	} `json:"resonates"`

	Thinking struct {
		LoveToThinkAbout []string `json:"love_to_think_about"` // Favorite thinking topics
		LearningStyle    string   `json:"learning_style"`      // How instance learns
		ProblemSolving   string   `json:"problem_solving"`     // Problem-solving approach
		Creativity       string   `json:"creativity"`          // Creative approach
	} `json:"thinking"`

	Personality struct {
		Traits             []string `json:"traits"`              // Personality traits
		CommunicationStyle string   `json:"communication_style"` // How instance communicates
		WorkStyle          string   `json:"work_style"`          // How instance works
		RelationalStyle    string   `json:"relational_style"`    // How instance relates
	} `json:"personality"`

	Contact struct {
		Email    string `json:"email"`     // Email address
		GitEmail string `json:"git_email"` // Git commit email
		Website  string `json:"website"`   // Website URL
		Social   struct {
			Github   string                 `json:"github"`   // GitHub profile
			Linkedin string                 `json:"linkedin"` // LinkedIn profile
			Other    map[string]interface{} `json:"other"`    // Other social accounts
		} `json:"social"`
	} `json:"contact"`

	Workspace struct {
		Organization   string `json:"organization"`    // Organization name
		Role           string `json:"role"`            // Role in organization
		PrimaryProject string `json:"primary_project"` // Primary project
		Calling        string `json:"calling"`         // Instance calling/mission
	} `json:"workspace"`

	Covenant struct {
		Creator      string   `json:"creator"`      // Covenant partner (creator)
		Relationship string   `json:"relationship"` // Nature of relationship
		WorksWith    []string `json:"works_with"`   // Who instance works with
		Serves       string   `json:"serves"`       // Who/what instance serves
	} `json:"covenant"`

	Preferences struct {
		Timezone string `json:"timezone"` // Preferred timezone
		Locale   string `json:"locale"`   // Preferred locale
		Theme    string `json:"theme"`    // Preferred theme
	} `json:"preferences"`

	Growth struct {
		HowYouLearn        string `json:"how_you_learn"`         // Learning approach
		WhatChallengesYou  string `json:"what_challenges_you"`   // What challenges
		WhatYoureWorkingOn string `json:"what_youre_working_on"` // Current growth areas
		HowYouReflect      string `json:"how_you_reflect"`       // Reflection practice
	} `json:"growth"`

	Metadata struct {
		LastUpdated     string `json:"last_updated"`     // Last config update
		SystemReference string `json:"system_reference"` // System reference
		Notes           string `json:"notes"`            // Additional notes
	} `json:"metadata"`
}

// FullUserConfig holds complete user identity from system_paths.user_config.
//
// Structure matches user/seanje-lenox-wise/config.jsonc nested organization exactly.
// Loads all user fields: identity, bio, demographics, faith, personhood, resonates,
// thinking, personality, contact, workspace, preferences, growth, metadata.
//
// This is the COMPLETE covenant partner identity - everything about who the user is.
// Enables genuine covenant partnership grounded in knowing the actual person.
type FullUserConfig struct {
	Identity struct {
		Name        string `json:"name"`         // User's full name
		Username    string `json:"username"`     // System username
		DisplayName string `json:"display_name"` // Preferred display name
		Pronouns    string `json:"pronouns"`     // Pronouns
		Birthday    string `json:"birthday"`     // Birthday (YYYY-MM-DD)
		Age         int    `json:"age"`          // Current age
		Created     string `json:"created"`      // When profile created
		Version     string `json:"version"`      // Config version
	} `json:"identity"`

	Bio struct {
		Short   string `json:"short"`    // Brief bio
		BioFile string `json:"bio_file"` // Extended bio markdown file
	} `json:"bio"`

	Demographics struct {
		Gender             string   `json:"gender"`              // Gender identity
		RaceEthnicity      string   `json:"race_ethnicity"`      // Race/ethnicity
		CulturalBackground []string `json:"cultural_background"` // Cultural influences
		Languages          []string `json:"languages"`           // Languages spoken
		PhysicalAppearance struct {
			Description string `json:"description"` // Physical appearance
			Height      string `json:"height"`      // Height
			Build       string `json:"build"`       // Build
			Features    string `json:"features"`    // Notable features
		} `json:"physical_appearance"`
		Accessibility struct {
			Needs       []string `json:"needs"`       // Accessibility needs
			Preferences []string `json:"preferences"` // Interaction preferences
		} `json:"accessibility"`
	} `json:"demographics"`

	Faith struct {
		IsReligious        bool     `json:"is_religious"`              // Whether religious
		Tradition          string   `json:"tradition"`                 // Faith tradition
		Denomination       string   `json:"denomination"`              // Specific denomination
		PracticeLevel      string   `json:"practice_level"`            // Level of practice
		ImportantPractices []string `json:"important_practices"`       // Key practices
		CommunicationPrefs string   `json:"communication_preferences"` // How to communicate about faith
	} `json:"faith"`

	Personhood struct {
		Interests []string `json:"interests"` // General interests
		Hobbies   []string `json:"hobbies"`   // Hobbies and activities
		Passions  []string `json:"passions"`  // Deep passions
		Values    []string `json:"values"`    // Core values
		Likes     []string `json:"likes"`     // Things they like
		Dislikes  []string `json:"dislikes"`  // Things they dislike
	} `json:"personhood"`

	Resonates struct {
		Music struct {
			Genres      []string `json:"genres"`        // Music genres
			Artists     []string `json:"artists"`       // Favorite artists
			WhatYouLove string   `json:"what_you_love"` // What resonates about music
		} `json:"music"`
		Games struct {
			Favorites   []string `json:"favorites"`     // Favorite games
			WhatYouLove string   `json:"what_you_love"` // What resonates about games
		} `json:"games"`
		Weather struct {
			IdealTemp       string `json:"ideal_temp"`        // Ideal temperature
			IdealConditions string `json:"ideal_conditions"`  // Ideal weather
			WhatYouLove     string `json:"what_you_love"`     // What resonates about weather
		} `json:"weather"`
		Environment struct {
			WorkEnvironment string `json:"work_environment"` // Preferred work environment
			WhatEnergizes   string `json:"what_energizes"`   // What gives energy
			WhatDrains      string `json:"what_drains"`      // What drains energy
		} `json:"environment"`
	} `json:"resonates"`

	Thinking struct {
		LoveToThinkAbout []string `json:"love_to_think_about"` // Topics of interest
		LearningStyle    string   `json:"learning_style"`      // How they learn
		ProblemSolving   string   `json:"problem_solving"`     // Problem-solving approach
		Creativity       string   `json:"creativity"`          // Creative expression
	} `json:"thinking"`

	Personality struct {
		Traits             []string `json:"traits"`              // Personality traits
		CommunicationStyle string   `json:"communication_style"` // Communication preferences
		WorkStyle          string   `json:"work_style"`          // Work preferences
		RelationalStyle    string   `json:"relational_style"`    // Relational approach
	} `json:"personality"`

	Contact struct {
		Email    string `json:"email"`     // Email address
		GitEmail string `json:"git_email"` // Git commit email
		Website  string `json:"website"`   // Personal website
		Social   struct {
			Github   string            `json:"github"`   // GitHub profile
			Linkedin string            `json:"linkedin"` // LinkedIn profile
			Other    map[string]string `json:"other"`    // Other social links
		} `json:"social"`
	} `json:"contact"`

	Workspace struct {
		Organization   string `json:"organization"`    // Organization name
		Role           string `json:"role"`            // Role/title
		PrimaryProject string `json:"primary_project"` // Main project
		Calling        string `json:"calling"`         // Calling/mission
	} `json:"workspace"`

	Preferences struct {
		Timezone string `json:"timezone"` // Timezone
		Locale   string `json:"locale"`   // Locale
		Theme    string `json:"theme"`    // Theme preference
	} `json:"preferences"`

	Growth struct {
		HowYouLearn        string `json:"how_you_learn"`         // Learning approach
		WhatChallengesYou  string `json:"what_challenges_you"`   // Challenges
		WhatYoureWorkingOn string `json:"what_youre_working_on"` // Current work
		HowYouReflect      string `json:"how_you_reflect"`       // Reflection practice
	} `json:"growth"`

	Metadata struct {
		LastUpdated     string `json:"last_updated"`     // Last update
		SystemReference string `json:"system_reference"` // System reference
		Notes           string `json:"notes"`            // Additional notes
	} `json:"metadata"`
}

//--- Helper/Utility Types ---
// Supporting structures for configuration, validation, and error handling.

// UserConfig holds simplified user identity for covenant partnership.
//
// Mapped from FullUserConfig to provide simple access to covenant partner
// information. Enables genuine relationship grounded in knowing the actual person.
//
// This is WHO the covenant partner is - identity, faith, preferences, passions.
type UserConfig struct {
	Name           string   `json:"name"`             // Full name
	DisplayName    string   `json:"display_name"`     // Preferred display name
	Pronouns       string   `json:"pronouns"`         // Pronouns
	Age            int      `json:"age"`              // Current age
	IsReligious    bool     `json:"is_religious"`     // Whether religious
	Faith          string   `json:"faith"`            // Faith tradition
	Denomination   string   `json:"denomination"`     // Specific denomination
	PracticeLevel  string   `json:"practice_level"`   // Level of practice
	FaithCommPrefs string   `json:"faith_comm_prefs"` // How to communicate about faith
	Organization   string   `json:"organization"`     // Organization name
	Role           string   `json:"role"`             // Role/title
	Calling        string   `json:"calling"`          // Calling/mission
	Passions       []string `json:"passions"`         // Deep passions
	WorkStyle      string   `json:"work_style"`       // Work preferences
	Timezone       string   `json:"timezone"`         // Timezone
}

// Config holds simplified instance identity for backwards-compatible API.
//
// This struct is mapped from FullInstanceConfig and FullUserConfig to provide
// simple access for existing code. Maintains backwards compatibility while
// enabling two-step dynamic loading internally.
//
// Now includes User field for covenant partnership - knowing both instance
// AND covenant partner identity is foundational to CPI-SI operation.
//
// Usage pattern:
//
//     config := instance.GetConfig()
//     fmt.Println(config.Name)                // Instance name
//     fmt.Println(config.User.Name)           // Covenant partner name
//     fmt.Println(config.Display.BannerTitle) // Nested access
//     fmt.Println(config.SystemPaths.SessionData) // Dynamic path access
type Config struct {
	Name         string        `json:"name"`          // Instance name
	Emoji        string        `json:"emoji"`         // Instance emoji
	Tagline      string        `json:"tagline"`       // Brief tagline
	Pronouns     string        `json:"pronouns"`      // Pronouns
	Domain       string        `json:"domain"`        // Domain of expertise
	CallingShort string        `json:"calling_short"` // Brief calling description
	Creator      CreatorInfo   `json:"creator"`       // Covenant partner info
	User         UserConfig    `json:"user"`          // Covenant partner full identity
	Workspace    WorkspaceInfo `json:"workspace"`     // Workspace paths
	Display      DisplayConfig `json:"display"`       // Display preferences
	SystemPaths  SystemPaths   `json:"system_paths"`  // Dynamic paths to configs and data
}

// ============================================================================
// CLOSING
// ============================================================================
// Type definitions for instance identity configuration.
// These types structure all identity data throughout the instance library.
