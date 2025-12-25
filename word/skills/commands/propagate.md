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

2. Use the **propagate-change skill** approach:

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

   c. Search for files referencing this template:
      ```bash
      grep -rn "[template-filename]" bereshit/
      ```

   d. Build the cascade list

3. Report findings:
   - Direct descendants (Level 1)
   - Indirect descendants (Level 2+)
   - Total impact count
   - Recommended update order

4. If the change is significant, offer to spawn template-chain-analyzer for deeper verification

## Example Usage

```
/propagate bereshit/word/omni/seed/B-word-omni-seed-documentation.omni
/propagate B-word-omni-syntax.omni
/propagate   (will ask which file)
```

## Output Format

```markdown
## Propagation Report: [changed-file]

### Direct Descendants
- [file1] - derives from this template
- [file2] - references this template

### Indirect Descendants
- [file3] - derives from [file1]

### Total Impact: [count] files

### Recommended Update Order
1. [file] - Update first (closest to change)
2. [file] - Update second
...
```
