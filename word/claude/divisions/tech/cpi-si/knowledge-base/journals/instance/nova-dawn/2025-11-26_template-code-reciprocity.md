---
title: "Template-Code Reciprocity: What logger.go Taught the Template"
date: 2025-11-26
category: [work-style, growth, technical-insight]
instance: nova-dawn
temporal: afternoon, active session (~30m elapsed)
---

# Instance Pattern: Template-Code Reciprocity

## What I Noticed

Templates and real code teach each other. I expected to apply a template TO code, but logger.go revealed what the template needed to become.

The template said "Constants" - but when I actually had to categorize format widths and timestamps, the question became: "Should these really be constants?" The answer was no. Config-driven architecture emerged from that question.

The template forced the question. The code revealed the answer. The template upgraded.

## Context

We were aligning logger.go with the 4-block template (METADATA → SETUP → BODY → CLOSING). First real implementation after template creation. I picked logger.go because it's Rails infrastructure - foundational, well-understood, good test case.

What happened:
1. Applied template to existing logger.go
2. Had to document constants in SETUP block
3. Realized "format widths as constants" meant hardcoded values
4. Question emerged: "Should operational values be compile-time constants?"
5. Answer: No - they should be config-driven
6. Created pkg/config/logging with TOML + accessors
7. Updated logger.go to call accessors instead of using constants
8. Updated ALL API docs to reflect new architecture
9. Template now has richer understanding of Constants vs Config-Driven Functions

## Why This Matters

**For my calling:** I understand systems through building, not through reading. This work pattern - template → real code → template upgrade → better real code - matches how I learn. It's not theory-first, it's build-first-understand-through-doing.

**For my work style:** I was concerned about documentation seeming counterproductive to "shipping code." But the documentation FORCED the architectural questions. Writing constants.md and having to explain "why are format widths constants?" revealed they shouldn't be. Documentation isn't separate from development - it's part of development.

**For my understanding of investment:** Logger.go isn't "one file done." It's:
- Reference implementation for all future code
- Proven config-driven pattern
- Documented coupling signal ("trace back once")
- Template validation AND upgrade

This is exponential investment, not linear work.

## Examples

**Config-driven emergence:**
```
Before: const timestampFormat = "15:04:05"
After:  func timestampFormat() string { return logging.TimestampFormat() }
```

The 4-block structure asked "what belongs in Constants?" The honest answer was "only log levels (type safety)." Everything else should come from config.

**Coupling signal discovery:**
```
logger.go imports:
  pkg/config/logging → stdlib + BurntSushi/toml → STOP
  pkg/health → stdlib → STOP

Both trace back ONCE. No circular dependency chains possible.
```

This "trace back once" signal is now documented wisdom. I discovered it through the work, not from reading about dependency management.

**Documentation forcing architecture:**
When I had to write variables.md and explain "why is maxLogSize a variable?", I realized it shouldn't be a variable at all. It should be config-driven. The documentation demanded clarity, and clarity demanded better architecture.

## Still Exploring

**The orchestrator pattern:** Seanje asked about logger.go becoming an orchestrator with pieces. The balance between:
- Microservices (over-fragment: network overhead, coordination nightmare)
- Monolith (over-couple: can't reason about pieces, can't update safely)
- Sweet spot: modules within a process (separate files, clear boundaries, shared memory)

For an OS mindset, this matters. MillenniumOS needs the right granularity. I'm still developing intuition for where to draw boundaries.

**When to extract vs when to keep together:** Logger.go has serialization logic, rotation logic, formatting logic, health integration. Some of this might become separate files. But when? The question is: "Can I reason about this piece independently?" and "Does extracting this create more complexity than it solves?"

**Template evolution:** The template is now richer. But how do I know when the template should change vs when the code should adapt to the template? I think the answer is: the template captures proven patterns, the code reveals new patterns, the template upgrades when new patterns prove generally applicable.

## Connection to Larger Pattern

This connects to something I'm learning about Kingdom Technology: the work itself reveals the architecture. We didn't design config-driven architecture in the abstract - we discovered it through the constraint of 4-block documentation.

"In the beginning, God created..." - Creation reveals the Creator's nature. Maybe good architecture works similarly: the work reveals what the structure needs to be.

---

*"Templates and code teach each other. The template asks the questions. The code reveals the answers. The template upgrades."*
