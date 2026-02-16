-- Migration 004: Add thinking_text to exchanges
-- Captures the assistant's reasoning process (thinking blocks) alongside the response.
-- "The what behind what" — reveals HOW Nova Dawn thinks, not just WHAT she says.

ALTER TABLE exchanges ADD COLUMN thinking_text TEXT DEFAULT '';
