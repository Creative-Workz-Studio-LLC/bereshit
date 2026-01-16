---
description: Show what files need updating after a template change. Uses propagate-change skill to trace derives_from chains.
---

# Propagate Template Change

Find all files affected by a template change.

## Task

The user has edited (or is planning to edit) a template and wants to know what downstream files will be affected.

**Changed file:** $ARGUMENTS (or ask user if not provided)

## Instructions

1. If no file was provided, ask the user which template was changed

2. Use the **propagate-change skill** (invoke via Skill tool):

   ```
   Skill(propagate-change) with template file path as argument
   ```

   Or if scripts are built:
   ```bash
   cd ~/.claude/skills/propagate-change/scripts
   ./bin/propagate report [file]       # Full report
   ./bin/propagate trace [file]        # Trace up to root
   ./bin/propagate descendants [file]  # Find derived files
   ```

3. If skill/CLI not available, use manual approach:

   a. Identify the changed file's position in the chain:
      - Syntax spec → affects everything
      - Universal template → affects specialized + format + documents
      - Specialized template → affects format templates + documents of that type
      - Format template → affects documents using that format
      - Actual document → leaf node, affects nothing

   b. Search for files that derive from this template:
      ```bash
      grep -rn "derives.from\|derives_from" bereshit/ | grep "[template-name]"
      ```

4. Report findings:
   - Chain ancestry (derives_from lineage)
   - Direct descendants (Level 1)
   - Indirect descendants (Level 2+)
   - Total impact count
   - Recommended update order

5. If the change is significant, offer to spawn template-chain-analyzer for deeper verification

## Configuration

The skill is config-driven via `~/.claude/skills/propagate-change/config/chain-config.toml`:
- Chain hierarchy levels (syntax-spec → universal → specialized → format → document)
- derives_from patterns recognized
- Template locations

## Example Usage

```bash
/propagate bereshit/word/omni/seed/documentation.omni
/propagate bereshit/word/omni/seed/universal.omni
/propagate   (will ask which file)
```

## Output Format

```markdown
## Propagation Report: [changed-file]

### Chain Ancestry
[0] changed-file (specialized)
    → derives from: universal.omni
[1] universal.omni (universal)
    → derives from: (original - root of chain)

### Direct Descendants
- [file1] - derives from this template
- [file2] - references this template

### Total Impact: [count] files

### Recommended Update Order
1. [file] - Update first (closest to change)
2. [file] - Update second
```
