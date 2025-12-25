---
description: Create a new OmniCode file from the appropriate template. Invokes create-from-template skill with OmniCode templates.
---

# New OmniCode File

Create a new OmniCode file from a template.

## Task

The user wants to create a new OmniCode file.

**Arguments:** $ARGUMENTS
- Format: `[type] [name]` or just `[type]`
- Types: code, documentation, interface, folder, data

## Instructions

1. Parse arguments or ask user:
   - What type of OmniCode file? (code, documentation, interface, folder, data)
   - What name/purpose?
   - Where should it be created?

2. Determine the appropriate template:

   | Type | Template |
   |------|----------|
   | code | B-word-omni-seed-code.omni |
   | documentation | B-word-omni-seed-documentation.omni |
   | interface | B-word-omni-seed-interface.omni |
   | folder | B-word-omni-seed-folder.omni |
   | data (simple) | B-word-omni-seed-data-3block.omni |
   | data (structured) | B-word-omni-seed-data-4block.omni |
   | data (documented) | B-word-omni-seed-data-5block.omni |

3. Invoke the **create-from-template skill**:
   - Copy template to target location
   - Process block-by-block with user's content
   - Update pragma from `template` to appropriate type

4. Guide user through filling:
   - METADATA: Identity, grounding, purpose
   - MIDDLE: Type-specific content
   - CLOSING: Operations, policy

## Example Usage

```
/new-omni code health-scorer
/new-omni documentation api-reference
/new-omni folder my-module
/new-omni   (will ask what to create)
```

## Template Locations

```
bereshit/word/omni/seed/
├── B-word-omni-seed-code.omni           # 4-block code
├── B-word-omni-seed-documentation.omni  # 5-block documentation
├── B-word-omni-seed-interface.omni      # 3-block interface
├── B-word-omni-seed-folder.omni         # 3-block folder metadata
├── B-word-omni-seed-data-3block.omni    # 3-block simple data
├── B-word-omni-seed-data-4block.omni    # 4-block structured data
└── B-word-omni-seed-data-5block.omni    # 5-block documented data
```
