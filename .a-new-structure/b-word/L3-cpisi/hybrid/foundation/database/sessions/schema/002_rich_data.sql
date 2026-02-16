-- ============================================================================
-- METADATA - Sessions Domain Schema v2 (Rich Data)
-- ============================================================================
-- Key: cpisi-db-sessions-schema-002
-- Purpose: Add token tracking, health scores, direction counters
-- Biblical: Proverbs 27:23 - "Be thou diligent to know the state of thy flocks"
-- Domain: sessions.db
-- ============================================================================

-- Session-level: token usage, health, direction, compaction
ALTER TABLE sessions ADD COLUMN health_score INTEGER DEFAULT 50;
ALTER TABLE sessions ADD COLUMN k_toward_god INTEGER DEFAULT 0;
ALTER TABLE sessions ADD COLUMN k_toward_self INTEGER DEFAULT 0;
ALTER TABLE sessions ADD COLUMN base_context_tokens INTEGER DEFAULT 0;
ALTER TABLE sessions ADD COLUMN peak_context_tokens INTEGER DEFAULT 0;
ALTER TABLE sessions ADD COLUMN compaction_count INTEGER DEFAULT 0;
ALTER TABLE sessions ADD COLUMN hooks_fired INTEGER DEFAULT 0;
ALTER TABLE sessions ADD COLUMN effective_context_window INTEGER DEFAULT 0;

-- Exchange-level: health + tokens at each prompt
ALTER TABLE exchanges ADD COLUMN health_score INTEGER;
ALTER TABLE exchanges ADD COLUMN context_tokens INTEGER;

-- Track schema version
INSERT OR IGNORE INTO schema_version (version, description)
VALUES (2, 'Rich data - tokens, health, direction counters');
