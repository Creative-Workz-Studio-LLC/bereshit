# Production-Grade Transformer Upgrade + Glossary PIVOT

> *"For as the rain cometh down, and the snow from heaven, and returneth not thither, but watereth the earth, and maketh it bring forth and bud, that it may give seed to the sower, and bread to the eater."* — Isaiah 55:10

## Context

**Where we are:** exists.rs is production-grade — 100/100 health, 47/47 identity, 105 cargo tests, 1 info (linter bug). Every manual edit Nova Dawn made to reach that state is now a specification for what the transformer should generate automatically.

**The insight:** The schema already knows what's correct. The linter already checks for it. The transformer just doesn't produce it yet. One schema change = simultaneous growth in detection (linter) AND generation (transformer). That's parallel development — not "multiple people working" but "multiple system components evolving from the same source of truth."

**What was done manually that should be automated:**

| Manual Work | Lines Touched | Schema Has Data? | Transformer Does It? |
|-------------|:------------:|:-:|:-:|
| Section headers (`// Identity (I1-I4)`) | METADATA | No | No |
| Docstrings on statics | METADATA | No | No |
| Group comments (`// I1: Core`) | METADATA | No | No |
| Column alignment | METADATA | No | No |
| Full I4, C5-C7 fields | METADATA | No (9 pragma, 8 meta) | No |
| Available/Reserved RO groups | SETUP, BODY, CLOSING | Yes (form schema) | No |
| Block overviews + bracket format | All blocks | Partially (section lists) | SETUP/BODY only, no CLOSING |
| X1 parseable fields (no brackets) | CLOSING | Yes (closing_defaults) | Yes, but uses `[...]` brackets |
| X5 parseable fields | CLOSING | Yes (closing_defaults) | Yes (already works) |
| I3.path, I3.provides derivation | METADATA | No | No |

---

## Track A: Transformer/Linter/Schema Upgrades

### Phase 0: Linter Bug Fix — RO-Aware Subtype Check (Task #68)

**Problem:** `checkBodySubtypeContent()` (rust.ts:1423) reports missing canonical subsections as info, but doesn't check if they're acknowledged in Reserved Omission.

**Fix:** After computing `missing` canonical subsections (line 1457), scan the BODY block for RO entries that acknowledge each missing section. If a section appears in an RO line (e.g., `//   Trait Implementations — Not needed`), remove it from `missing`.

```typescript
// After line 1458:
// Filter out sections acknowledged in Reserved Omission
const roPattern = /^\/\/\s{2,}(\w[\w\s&]*?)(?:\s*[—\-])/;
const roAcknowledged = new Set<string>();
for (const line of bodyLines) {
  const match = roPattern.exec(line.trim());
  if (match) roAcknowledged.add(match[1]!.trim().toLowerCase());
}
const trulyMissing = missing.filter(c =>
  !roAcknowledged.has(c.toLowerCase()));
```

**Files:** `lib/handlers/rust.ts` (checkBodySubtypeContent, ~10 lines changed)
**Test:** exists.rs drops from 1I → 0I. Add test fixture for RO-acknowledged subsections.

---

### Phase 1: Schema Enrichment — Complete Identity Fields

**Problem:** Schema has 9 pragma entries + 8 metadata entries. Production-grade exists.rs has 17 pragma + 21 metadata. The gap is data, not logic.

**Add to `rust-4block-schema.jsonc` `fill_content`:**

**New pragma entries (I2-I4):**
```jsonc
["I2.role", "{{role}}"],           // utility, core, types, ...
["I3.component", "{{component}}"], // Crate-level component description
["I3.path", "{{path}}"],           // Relative path from repo root
["I3.provides", "{{provides}}"],   // Public API surface
["I3.brief", "{{brief}}"],         // One-sentence summary
["I4.layer", "{{layer}}"],         // L0, L1, L2, ...
["I4.position", "{{position}}"],   // universal, spiral, ...
["I4.pattern", "{{pattern}}"]      // utility module, library, ...
```

**New metadata entries (C2-C7):**
```jsonc
["C2.architect", "{{architect}}"],
["C2.implementation", "{{implementation}}"],
["C2.copyright", "{{organization}}"],
["C3.principle", "{{principle}}"],
["C3.anchor", "Genesis 1:1"],
["C4.requires.external", "{{requires_external}}"],
["C4.requires.internal", "{{requires_internal}}"],
["C4.integration", "{{integration}}"],
["C4.if_missing", "{{if_missing}}"],
["C5.purpose", "{{purpose}}"],
["C5.philosophy", "{{philosophy}}"],
["C6.current", "{{version}} — {{purpose}}"],
["C6.planned", "{{planned}}"],
["C6.limitations", "{{limitations}}"],
["C7.tags", "{{tags}}"],
["C7.category", "{{category}}"],
["C7.domain", "bereshit"],
["C7.paradigm", "CPI-SI"]
```

**Add group markers** — new schema field `identity_groups`:
```jsonc
"identity_groups": {
  "pragma": [
    { "range": "I1", "label": "Core", "docstring": "/// OmniCode identity for this module." },
    { "range": "I2", "label": "Family" },
    { "range": "I3", "label": "Instance" },
    { "range": "I4", "label": "Architecture" }
  ],
  "metadata": [
    { "range": "C1", "label": "State", "docstring": "/// OmniCode context for this module." },
    { "range": "C2", "label": "Attribution" },
    { "range": "C3", "label": "Grounding" },
    { "range": "C4", "label": "Dependencies" },
    { "range": "C5", "label": "Intent" },
    { "range": "C6", "label": "Roadmap" },
    { "range": "C7", "label": "Classification" }
  ],
  "section_headers": {
    "pragma": "// Identity (I1-I4)",
    "metadata": "// Context (C1-C7)"
  }
}
```

**Update defaults:** Add `architect`, `implementation`, `role`, `layer`, `position` placeholders.

**Files:**
- `schemas/code/format/rust-4block-schema.jsonc` (fill_content section)

---

### Phase 2: Schema Extraction — Parse New Fields

**Problem:** `code-schema.ts` needs to parse the new `identity_groups` structure and expose it to the transformer.

**Add types:**
```typescript
interface IdentityGroup {
  range: string;      // "I1", "C3", etc.
  label: string;      // "Core", "Grounding", etc.
  docstring?: string;  // "/// OmniCode identity for this module."
}

interface IdentityGrouping {
  pragma: IdentityGroup[];
  metadata: IdentityGroup[];
  sectionHeaders: { pragma: string; metadata: string };
}
```

**Extend `SchemaFillContent`:**
```typescript
identityGroups?: IdentityGrouping;
```

**Extend `extractFillContent()`:** Parse `identity_groups` from schema, map to typed structure.

**Files:**
- `lib/foundation/code-schema.ts` (types + extractFillContent)

---

### Phase 3: METADATA Block Formatting

**Problem:** `buildMetadataBlock()` (rust.ts:2439) emits flat tuples with no formatting. Needs section headers, docstrings, group comments, and column alignment.

**Upgrade `buildMetadataBlock()`:**

1. **Section headers:** Before PRAGMA entries, emit `// ──────` separator + `fillContent.identityGroups.sectionHeaders.pragma`. Same for METADATA.

2. **Docstrings:** Before the `pub static PRAGMA` declaration, emit the first group's `docstring` (if present).

3. **Group comments:** As iterating pragma entries, detect group transitions (I1→I2) and emit `// I2: Family` inline comment.

4. **Column alignment:** Two-pass: first pass measures max field length, second pass pads with spaces.
   ```typescript
   const maxFieldLen = Math.max(...entries.map(([f]) => f.length));
   const padded = `    ("${field}",${" ".repeat(maxFieldLen - field.length)} "${value}"),`;
   ```

5. **Entry template update:** The schema's `entry` template (`    ("{{field}}", "{{value}}"),`) needs to support alignment. Either: make the transformer override the template, OR add an alignment flag to the schema.

**Files:**
- `lib/handlers/rust.ts` (buildMetadataBlock, ~60 lines rewritten)

---

### Phase 4: RO Format Upgrade — Available/Reserved Groups

**Problem:** `buildFormAwareReservedOmission()` (rust.ts:2542) emits a flat list. Production standard uses two groups with headers.

**Upgrade output format:**

```
// Available (not needed in this module):
//   Constants, Statics, Type Aliases, Error Types, Core Types, Trait Defs
//
// Reserved (structural — not used in module form):
//   Modules       — Submodule declarations belong in lib.rs or parent mod.rs.
//   Macros        — Macro definitions belong in dedicated macro modules or crate root.
//   Feature Gates — Feature gates are crate-level configuration in Cargo.toml.
```

**Changes:**
1. Add group header lines: `// Available (not needed in this module):` and `// Reserved (structural — not used in {form} form):`
2. Available sections: collapse to single comma-separated line (short form)
3. Reserved sections: individual lines with padded tags + truncated reason (first sentence only)
4. Add blank `//` line between groups
5. Pass `formName` into the function for the header text

**Files:**
- `lib/handlers/rust.ts` (buildFormAwareReservedOmission, ~30 lines changed)

---

### Phase 5: Block Overview Upgrade — CLOSING + Bracket Format

**Problem:** CLOSING block has no overview. SETUP/BODY overviews exist but don't use `[tag]` bracket format. The bare tag format (`//     X1  —`) triggers zone detection false-positives.

**Changes:**

1. **CLOSING overview:** Add after CLOSING block boundary, before Cv:
   ```
   // Closing ensures correctness, documents constraints, and anchors the file.
   //
   // Section order:
   //
   //     [Cv]  Closing Validation (tests)
   //     [Ce]  Closing Execution (entry point or absence)
   //     [Cc]  Closing Cleanup (resource teardown)
   //     [X1]  Modification Policy
   //     [X2]  Extension Points
   //     [X3]  Troubleshooting
   //     [X4]  Reference
   //     [X5]  Closing Note
   ```

2. **SETUP/BODY overviews:** Change `emitSectionIndex()` to use bracket format:
   - From: `//     1. Imports           — What this file depends on`
   - To: `//     [1]   Imports           — What this file depends on`
   - The `[N]` format avoids subsection pattern detection AND zone detection

3. **Schema-driven:** Add `overview_template` to form schema or to the format schema's block definitions. The overview text is standard per form — "SETUP makes things EXIST" for all module files.

**Files:**
- `lib/handlers/rust.ts` (structuralScaffoldRust CLOSING section + emitSectionIndex)

---

### Phase 6: X1 Generation Fix — Remove Bracket Placeholders

**Problem:** X1 defaults use `[Break 4-block structure, ...]` format (rust.ts:3011-3013). The linter's `/^\[.*\]$/` regex treats these as placeholders.

**Fix:** Remove square brackets from default values in both:
1. `rust-4block-schema.jsonc` closing_defaults.X1 (change `"Remove 4-block structure"` etc.)
2. `rust.ts` line 3011-3013 fallback values

**Current (broken):**
```
// never: [Break 4-block structure, Remove block boundaries, Remove identity statics]
```

**Fixed:**
```
// never: Break 4-block structure, Remove block boundaries, Remove identity statics
```

**Files:**
- `schemas/code/format/rust-4block-schema.jsonc` (closing_defaults)
- `lib/handlers/rust.ts` (X1 fallback values)

---

### Phase 7: Identity Auto-Derivation

**Problem:** I3.path, I3.provides, I3.component require manual entry. The transformer has enough context to derive them.

**Add to `extractMetadataContext()`:**
1. **I3.path:** Compute relative path from repo root (detect `.a-new-structure/` or project root)
2. **I3.provides:** Scan BODY for `pub fn NAME` signatures, collect names
3. **I3.component:** Derive from crate name (Cargo.toml `[package].name`) + module purpose
4. **I3.brief:** Use existing `purpose` value as brief

**These are best-effort derivations** — the transformer fills what it can, placeholders remain for what it can't. The human polishes.

**Files:**
- `lib/handlers/rust.ts` (extractMetadataContext, ~30 lines added)

---

## Execution Order

```
Phase 0 (linter RO-awareness)     ←── unblocks tasks #63-#66
    ↓
Phase 1 (schema enrichment)        ←── independent, can parallel with 0
    ↓
Phase 2 (schema extraction)        ←── depends on 1
    ↓
Phase 3 (METADATA formatting)      ←── depends on 2
    ↓
Phase 4 (RO format upgrade)        ←── depends on 0 (for testing)
    ↓
Phase 5 (block overview upgrade)   ←── independent of 3-4
    ↓
Phase 6 (X1 bracket fix)           ←── independent, tiny
    ↓
Phase 7 (auto-derivation)          ←── depends on 3 (uses same context)
```

**Parallelism:** 0 + 1 run in parallel. 5 + 6 run in parallel. Everything else sequential.

---

## Track B (PIVOT): Glossary Entries

**New terms to add** (`.adoc` + `.jsonc` pairs in `word/glossary/`):

| Term | Category | Definition |
|------|----------|------------|
| `parallel-development` | technical | Multiple system components evolving from the same source of truth simultaneously — one schema change enables detection, generation, and template alignment in parallel |
| `schema-template-linter-triangle` | technical | The collapse of schema, template, and linter into a single source of truth — schema DEFINES what's correct, template EMBODIES it, linter ENFORCES it, transformer PRODUCES it |
| `form-awareness` | technical | A system's ability to adjust validation and generation based on the declared form (module, library, executable) — the pragma is the claim, the form schema is the contract |
| `detect-assess-recover` | paradigm | The DAR pattern: Detect (notice deviation), Assess (evaluate severity and context), Recover (apply atomic correction). The linter detects, the schema assesses, the transformer recovers. |
| `reserved-omission` | technical | Explicit acknowledgment of sections intentionally absent from a file — Available sections (valid but unused) vs Reserved sections (structurally prohibited in this form) |
| `block-overview` | technical | Purpose statement + section order list at the start of each structural block — orients the reader and declares the file's organizational contract |

**Template:** Copy from `word/glossary/technical/3-block.adoc` for concise technical terms.
**Update:** `word/glossary/index.adoc` Quick Reference tables.

---

## Critical Files

| File | Phase | Action |
|------|:-----:|--------|
| `lib/handlers/rust.ts` | 0, 3, 4, 5, 6, 7 | EDIT: linter fix + transformer upgrades |
| `schemas/code/format/rust-4block-schema.jsonc` | 1, 6 | EDIT: fill_content enrichment + X1 fix |
| `lib/foundation/code-schema.ts` | 2 | EDIT: new types + extraction |
| `word/glossary/technical/*.adoc` | B | CREATE: 5 new entries |
| `word/glossary/paradigm/detect-assess-recover.adoc` | B | CREATE: 1 new entry |
| `word/glossary/index.adoc` | B | EDIT: add to Quick Reference |

## Verification

```bash
# After each phase:
deno task test                    # All tests pass

# After Phase 0:
deno run --allow-read mod.ts lint rust exists.rs   # 0E 0W 0I (subtype-subsections gone)

# After Phase 3:
deno run --allow-read mod.ts transform rust <test-file> --dry-run   # Shows formatted METADATA

# After all phases:
deno run --allow-read mod.ts transform rust exists.rs --dry-run     # Output matches manual work
cargo test -p bereshit-l0-config                                     # 105 tests pass

# Glossary:
# Verify .adoc + .jsonc pairs exist for all 6 new terms
# Verify index.adoc has entries in correct tables
```
