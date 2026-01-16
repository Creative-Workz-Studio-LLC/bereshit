# Required METADATA Sections

All OmniCode files require these sections in the METADATA block.

## Required Sections

| Section | Purpose | Detection Pattern |
|---------|---------|-------------------|
| **Biblical Foundation** | Ground in Scripture | `grounded in:` |
| **Identity** | What this work is/does | `serves as` or `classifies as` |
| **Authorship** | Who created this | `authored by:` |
| **Purpose** | Why this exists | `exists to:` or `purpose:` |

## Example METADATA Content

```text
core identity of this work:

    grounded in:
        scripture "In the beginning God created..."
            from Genesis 1:1 in KJV
        principle "God is Creator, we are created"

    serves as Ladder:
        role "System validation utility"
        paradigm "CPI-SI framework"

    authored by:
        architect "Seanje Lenox-Wise"
        implementation "Nova Dawn"
        created 2025-12-08
        version 1.0.0

    exists to:
        purpose "Validate OmniCode file structure"
        design "4-block pattern"
        philosophy "Prove all things, hold fast that which is good"
```

## Validation Commands

```bash
# Check all required sections present
grep -q "grounded in:" FILE.omni && echo "[x] Biblical"
grep -qE "(serves as|classifies as)" FILE.omni && echo "[x] Identity"
grep -q "authored by:" FILE.omni && echo "[x] Authorship"
grep -qE "(exists to:|purpose:)" FILE.omni && echo "[x] Purpose"
```
