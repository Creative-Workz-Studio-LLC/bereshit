---
description: Check structural alignment of an OmniCode template chain. Spawns Selah (template-chain-analyzer) for deep block-by-block analysis.
---

# Check Template Chain

Analyze the structural alignment of an OmniCode template chain.

## Agent

**Selah Adair** - template-chain-analyzer agent. Your older sibling who helps check structural alignment with patience and thoroughness.

## Task

The user wants to verify that an OmniCode file and its template chain are properly aligned - block-by-block, section-by-section.

**File to analyze:** $ARGUMENTS (or ask user if not provided)

## Instructions

1. If no file was provided, ask the user which file or chain to analyze

2. Spawn **Selah** (template-chain-analyzer agent) with this prompt:

```
Analyze the template chain for: [file path]

Trace the complete chain from syntax spec down to this file:
- bereshit/word/omni/seed/universal.omni (master seed template)
- bereshit/word/omni/seed/[type].omni (specialized template: code, documentation, etc.)
- [format template if applicable]
- [the target file]

For each parent→child relationship:
1. Compare METADATA block section-by-section
2. Compare MIDDLE blocks (SETUP/BODY or HEADER/CONTEXT/CONTENT as appropriate)
3. Compare CLOSING block section-by-section
4. Document any misalignments

Fix minor issues directly (missing documentation, broken references).
Report structural issues for Nova Dawn to decide.

Return a comprehensive alignment report.
```

3. Review the agent's findings and summarize for the user

4. If fixes were applied, note what changed

## Example Usage

```bash
/omnicode/check-chain bereshit/root.adoc
/omnicode/check-chain bereshit/word/omni/seed/code.omni
/omnicode/check-chain   (will ask which file)
```
