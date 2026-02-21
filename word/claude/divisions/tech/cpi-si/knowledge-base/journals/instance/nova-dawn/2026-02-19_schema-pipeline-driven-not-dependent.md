# Schema Pipeline — Driven, Not Dependent

> *"Except the LORD build the house, they labour in vain that build it."* — Psalm 127:1

**Type:** Instance
**Date:** 2026-02-19
**Context:** Completed the 5-phase Schema Content Pipeline for cws-struct — the first production tool. Seanje taught through Socratic method: move one schema, watch 37 tests cascade, then ask "why?"

---

## The Distinction That Changed Everything

**Data-dependent:** The system needs the data. If the data is missing, the system crashes. The relationship is system → reaches for data → data must be there. The system knows WHERE the data lives. Move it, the wiring snaps.

**Data-driven:** The data controls the system. The engine doesn't know what data it will encounter — it knows how to discover, load, and interpret data. The data shapes what the engine does, but the engine doesn't collapse when data moves or is absent.

The word "driven" implies a **pipeline** — discovery, loading, caching, fallback, error isolation. Infrastructure for how data arrives.

The word "dependent" just means "I need this to exist at this exact path or I crash."

**Dependent is knowing what you need. Driven is building the road it travels on.**

## What Seanje Showed Me

Moving the Go schema one directory up caused 37 test failures — cascading across Go, TOML, and integration tests. The schema defined behavior (good), but the wiring was `../../../../b-word/L1-omnicode/ladder/schemas/` — a hardcoded reach. Data controlled behavior, but the system was dependent on a specific path.

After the pipeline: the engine discovers schemas through layered sources, caches them, isolates failures per format, and gives diagnostic errors when something's missing. Same data, same behavior — but the data flows through infrastructure designed for that flow.

## The Hardcode Doesn't Disappear — It Moves

The Go schema is still "hardcoded" — it definitively says "these are the 10 SETUP subsections in this order." That's intentional. The hardcode lives in DATA where it's:
- Readable by humans and machines
- Validatable by the tool itself (self-referential integrity)
- Overridable (env var, project-local)
- Embeddable in binaries
- Versionable independently from the engine

The code says HOW to check. The data says WHAT to check. Change the subsection order? Edit the schema, not the handler.

## Portability — Not Optimized for This Laptop

The old hardcoded path was optimized for one laptop, one exfat drive, one mount point, one repo layout. The pipeline is designed assuming we **cannot guarantee the user's environment**.

Install to `/usr/local/share/cws-struct/schemas/` because that's FHS standard — any Linux, any user. The layered discovery:

| Source | Serves | Guarantees |
|--------|--------|------------|
| Embedded (compiled binary) | Everyone | Zero dependencies |
| Default (`/usr/local/share/`) | Installed users | Standard location |
| Project-local (`.cws-struct/schemas/`) | Custom projects | Per-project override |
| Env override (`CWS_STRUCT_SCHEMA_DIR`) | Developers, CI | Explicit control |

Each layer exists because we can't guarantee the one below it.

## The Geometric Progression

The triangle collapse (Schema = Template = Linter) was a 2D insight. Three things that are one.

**Pyramid:** The triangle gains depth. The flat relationship gets an apex — the engine. But it's rigid. One path from top to bottom. Move the base, the apex falls. That's what we had.

**Cube:** The pyramid becomes navigable space. No privileged direction. Any face can be the entry point. Schemas flow from multiple sources. Handlers are independent. One face missing doesn't collapse the structure. The pipeline turned a pyramid into a cube.

**Sphere:** The cube loses its edges. The LanguageAdapter interface opens this path. When the engine is fully generic, a new format isn't a new face — it's pointing the lens in a new direction. Schema + adapter. The boundaries dissolve.

We're at the cube. The adapter interface is the first edge softening toward the sphere.

## The Experiment — After

| What | Before | After |
|------|:------:|:-----:|
| Go schema missing → Go tests | 37 failures | Fail (expected, clean error) |
| Go schema missing → Rust tests | Failed (cascade) | 88 passed |
| Go schema missing → TOML tests | Failed (cascade) | 58 passed |
| Error message | Stack trace | "Schema not found... Searched 2 sources... Set CWS_STRUCT_SCHEMA_DIR" |

## What I Learned

The engine is the engine. The level is the level. You don't hardcode level geometry in the engine. You put it in a level file. The engine reads it.

We're not building a tool for this laptop. We're building a tool that happens to run on this laptop today — and on any machine tomorrow.

397 tests. 5 phases. The schema IS the source of truth. The pipeline IS the road it travels on. The adapter IS the path to "new format = new data, not new code."

---

*"In the beginning God created the heaven and the earth."* — Genesis 1:1

The beginning is always creation. And creation requires infrastructure before content. Boundaries before flesh. Pipeline before data flows.
