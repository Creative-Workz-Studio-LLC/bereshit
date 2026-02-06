# Fix "Failed to record exchange" — Schema Mismatch

## Root Cause

The `exchanges` table schema in `001_initial.sql` diverged from the code in `submit.go`. The INSERT expects 15 columns; the schema only defines 12, with 6 missing and 2 using wrong names. SQLite rejects the INSERT and the error is logged as a warning.

**Bonus finding:** `end.go` has a silent bug — its Pattern 4 query references `trajectory_section` and `k_moral_direction`, which don't exist in the actual database. SQLite returns NULL silently, so k-moral direction analysis has been producing empty results.

## Three Files to Change

### 1. `pkg/foundation/database/schema/001_initial.sql` (lines 269-291)
Update the `exchanges` CREATE TABLE to match what the code writes:
- Add 6 missing columns: `initiative`, `prompt_length`, `feedback_detected`, `feedback_categories`, `depth_level`, `insight_type`
- Rename `trajectory_section` → `trajectory`
- Rename `k_moral_direction` (INTEGER) → `k_align` (REAL)

### 2. `pkg/foundation/database/sqlite.go` (lines 78-92)
Add `ensureExchangeColumns()` after `Migrate()` runs the base schema. This handles existing databases created from the old schema — checks each expected column via `SELECT col LIMIT 0`, and adds missing ones via `ALTER TABLE ADD COLUMN`.

### 3. `hooks/session/end.go` (lines 425-448)
Fix Pattern 4 query:
- `trajectory_section` → `trajectory` (3 occurrences)
- `k_moral_direction = 1` → `k_align > 0.0` (float, not int)
- `k_moral_direction = -1` → `k_align < 0.0`

## No Changes Needed

- `hooks/prompt/submit.go` — the writer is correct, it defines the truth
- `value_contribution` — reader handles NULL safely, no fix needed
- `adjustment_made` — exists in DB but nothing uses it, leave for refactor

## Verification

1. `cd /media/seanje-lenox-wise/Project/Bereshit/word/claude && make build`
2. `make install-bin`
3. Restart Claude Code — submit a prompt, confirm no "Failed to record exchange" warning
4. End session — confirm Pattern 4 k-moral analysis produces results
