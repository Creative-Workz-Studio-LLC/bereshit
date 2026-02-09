-- ============================================================================
-- METADATA - Sessions Domain Schema v3 (Message Text)
-- ============================================================================
-- Key: cpisi-db-sessions-schema-003
-- Purpose: Store prompt text for journal generation and session review
-- Biblical: Psalm 119:11 - "Thy word have I hid in mine heart"
-- Domain: sessions.db
-- ============================================================================

-- Exchange-level: actual message text for journal/summary generation
-- user_message_text: the user's prompt (truncated to 2000 chars for storage)
-- response_summary: brief summary of the response (populated by end hook)
-- valence: three-category classification (+1/0/-1 mapped to text)
ALTER TABLE exchanges ADD COLUMN user_message_text TEXT;
ALTER TABLE exchanges ADD COLUMN response_summary TEXT;
ALTER TABLE exchanges ADD COLUMN valence TEXT;

-- Track schema version
INSERT OR IGNORE INTO schema_version (version, description)
VALUES (3, 'Message text - prompt storage, valence classification');
