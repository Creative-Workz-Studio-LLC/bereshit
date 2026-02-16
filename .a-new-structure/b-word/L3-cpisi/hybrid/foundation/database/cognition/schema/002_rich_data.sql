-- ============================================================================
-- METADATA - Cognition Domain Schema v2 (Rich Data)
-- ============================================================================
-- Key: cpisi-db-cognition-schema-002
-- Purpose: Add health score tracking per choice
-- Biblical: Proverbs 27:23 - "Be thou diligent to know the state of thy flocks"
-- Domain: cognition.db
-- ============================================================================

-- Choice-level: health at time of each tool use
ALTER TABLE choices ADD COLUMN health_score INTEGER;

-- Track schema version
INSERT OR IGNORE INTO schema_version (version, description)
VALUES (2, 'Rich data - health score per choice');
