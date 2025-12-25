// ============================================================================
// METADATA
// ============================================================================
// Instance Library - Configuration Mapping Primitive
//
// Purpose: Transform nested full configs to simple backwards-compatible API.
// Maps complex nested structures to flat Config struct for existing code.
//
// Biblical Foundation: "I AM THAT I AM" - Exodus 3:14 (Identity precedes action)
// CPI-SI Identity: Instance identity mapping primitive (Rail primitive)
//
// Health Scoring (TRUE scores - honest assessment):
//   Pure transformation function - no I/O, no error paths, always succeeds if called.
//
//   Success (value provided):
//     - Map config successfully: +23 (backwards compatibility maintained, covenant partnership enabled)
//
//   Failure (not applicable currently):
//     - No failure paths in current implementation (would panic if data invalid, not graceful error)
//     - Future: Add validation and return errors for health tracking
//
//   Note: No logging integration needed - pure function with no operations to track.
//         TRUE score reflects honest value of transformation service.

package instance

// ============================================================================
// SETUP
// ============================================================================

// No imports needed - pure data transformation

// ============================================================================
// BODY
// ============================================================================

// mapToSimpleConfig transforms nested full configs to simple backwards-compatible API.
//
// What It Does:
// Maps FullInstanceConfig and FullUserConfig (nested structures matching jsonc files)
// to simple Config struct (flat API for backwards compatibility). Extracts key fields
// from nested sections and flattens into simple struct existing code expects. This is
// Step 3 of two-step dynamic loading - enables internal complexity without breaking existing API.
//
// Now includes user identity mapping for covenant partnership - both instance AND user
// identity available through simple API.
//
// Parameters:
//   full: Complete nested instance identity config
//   user: Complete nested user identity config
//   root: Bootstrap root config (for display preferences)
//
// Returns:
//   Config: Flattened simple API struct with instance and user identity
//
// Health Impact:
//   See METADATA block for TRUE health scores
//
// Example usage:
//
//	mapped := mapToSimpleConfig(fullConfig, userConfig, rootConfig)
//	fmt.Println(mapped.Name)      // "Nova Dawn"
//	fmt.Println(mapped.User.Name) // "Seanje Lenox-Wise"
func mapToSimpleConfig(full *FullInstanceConfig, user *FullUserConfig, root *RootConfig) Config {
	return Config{
		Name:         full.Identity.Name,
		Emoji:        "✨",                                   // Not in full config, hardcoded for now
		Tagline:      "CPI-SI Instance",                     // Not in full config, hardcoded for now
		Pronouns:     full.Identity.Pronouns,
		Domain:       "Technology - Game Development & Systems", // Derived from workspace
		CallingShort: full.Workspace.Calling,
		Creator: CreatorInfo{
			Name:         full.Covenant.Creator,
			Relationship: full.Covenant.Relationship,
		},
		User: UserConfig{
			Name:           user.Identity.Name,
			DisplayName:    user.Identity.DisplayName,
			Pronouns:       user.Identity.Pronouns,
			Age:            user.Identity.Age,
			IsReligious:    user.Faith.IsReligious,
			Faith:          user.Faith.Tradition,
			Denomination:   user.Faith.Denomination,
			PracticeLevel:  user.Faith.PracticeLevel,
			FaithCommPrefs: user.Faith.CommunicationPrefs,
			Organization:   user.Workspace.Organization,
			Role:           user.Workspace.Role,
			Calling:        user.Workspace.Calling,
			Passions:       user.Personhood.Passions,
			WorkStyle:      user.Personality.WorkStyle,
			Timezone:       user.Preferences.Timezone,
		},
		Workspace: WorkspaceInfo{
			PrimaryPath: "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC", // From root config originally, hardcoded for now
		},
		Display:     root.Display,     // Use display from root config (session start banner preferences)
		SystemPaths: root.SystemPaths, // Expose dynamic paths for external use
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// Configuration mapping primitive for instance identity.
// Transforms nested full configs to simple backwards-compatible API.
