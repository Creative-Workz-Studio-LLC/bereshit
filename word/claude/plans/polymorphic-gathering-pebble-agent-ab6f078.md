# Exploration: AsciiDoc Publishing Reference Data in build.config.yaml

## Date: 2026-02-07
## Task: Understand what publishing reference data should be encoded in the config file

---

## FINDINGS SUMMARY

### 1. Current Config State
- **Location:** `/media/seanje-lenox-wise/Project/Bereshit/company-docs/build/build.config.yaml`
- **Current Version:** `5.0.0` (line 1225)
- **File Size:** 1,226 lines
- **Current Highest Section:** §16 — Document Types (Register Mapping)
- **Sections Present:** 1-16 (complete coverage through Document Types)

### 2. What IS Currently Encoded
The config already contains extensive publishing reference data:

#### Section 2: Document Attributes (lines 46-111)
- Book structure attributes (doctype, book, title-page, toc, toclevels, sectnums, sectnumlevels, partnums)
- Rendering attributes (icons, experimental, reproducible, source-highlighter, rouge-style)
- Governance hierarchy labels (chapter-label: Article, part-label: Part, appendix-caption)
- Cross-references (xrefstyle: full)
- Caption labels (listing-caption, figure-caption, table-caption)
- Admonition captions (caution, example, important, note, tip, warning)
- Company attributes (company-name, company-short, manual-title, tagline, founding-scripture)
- Entity attributes (entity-type, jurisdiction, formation-date, filing-number, founding-year, manual-doc-id)

#### Section 11: Typography (lines 467-674)
- Type scale anchor (body_size: 11, scale_ratio: 1.333, line_height: 16.5)
- Governance depth hierarchy with complete mapping:
  - part: `=`, 30pt, scale_step: 6
  - h1 (Article): `==`, 26pt, scale_step: 5
  - h2 (Section): `===`, 22pt, scale_step: 4
  - h3 (Subsection): `====`, 17pt, scale_step: 3
  - h4 (Clause): `=====`, 14pt, scale_step: 2
  - h5 (Subclause): `======`, 12pt, scale_step: 1
  - h6: `=======`, 11pt, scale_step: 0
- Supporting sizes (lead, small, code, footnotes, title_page, header_footer, toc)
- Font stack (body, headings, code, hebrew, symbol)
- Font-family assignments by theme element (11 categories)
- Text alignment (heading: center, part: center, chapter: center)
- Semantic color assignments (14 categories)
- Custom roles (6 roles: gold, muted, blue, green, purple, red)
- Font-style assignments (7 categories)
- Legal typography (Butterick standards: body_serif, prose_indent, prose_justify, line_height_percent, line_length_target, uppercase_heading, uppercase_letter_spacing)

#### Section 12: Page Layout (lines 677-720)
- Page dimensions (Letter, portrait)
- Page margins [0.875in, 1in, 0.875in, 1in]
- Prepress margins (inner: 0.75in, outer: 0.59in)
- Header and footer height (0.625in)
- Vertical rhythm (spacing: 12pt, thematic_break margins)
- Orphan prevention (heading_min_height_after: 0.5in)
- Heading spacing (margin_top: 18pt, margin_bottom: 10pt, line_height: 1.2)
- Thematic break styling (border_width: 0.25, border_color: #CBD5E0)

#### Section 13: Legal & Governance Language (lines 723-993)
- Obligation language (modern & traditional registers)
- Defined terms (format, capitalization, first_occurrence, glossary_required, inline_acceptable)
- Definition verbs (means, includes, means_and_includes, does_not_include, refers_to)
- Construction conventions (9 rules)
- Binding provisions (activation_mechanism, required_instances: 3, locations with pages)
- Amendment authority (6 tiers from typos to governance structure)
- Governance notation (terms & abbreviations: Part, Article, Section, Subsection, Clause, Sub-clause)
- Signature and approval format
- Cross-reference conventions (internal styles, traditional/modern self-reference)
- Traditional legal document structure (10-element anatomy)
- Recitals (traditional vs modern default)
- Numbering hierarchy (modern vs traditional, all levels defined)
- Enumeration conventions (tabulated_threshold: 3, punctuation rules)
- Required boilerplate provisions (severability, governing_law, entire_agreement, amendment, waiver, effective_date, notices)

#### Section 14: Industry-Specific Conventions (lines 995-1082)
- Game development IP protection (source_code, game_art_music_assets, game_design_documents, player_generated_content, apis_and_tools)
- EULA conventions (license_grant, ip_rights, ugc_policy, termination, liability, dispute_resolution)
- Three-license structure (patterns, implementation, commercial)
- Publishing house creator agreements (7 provisions)
- Publishing house governance (4 elements)

#### Section 15: Tone & Voice (lines 1085-1179)
- CWS Voice (5 attributes: professional, warm, grounded, clear, authoritative)
- Tone spectrum (5 contexts with formality levels)
- Covenant vs compulsion language (5 replacement pairs)
- Plain language principles (max_sentence_words: 50, target: [20, 30], 8 word replacements)
- Readability targets (5 audiences with grade levels)

#### Section 16: Document Types (lines 1182-1220)
- Register mapping (7 document types with register, theme, rationale)

---

### 3. What IS NOT Currently Encoded (But Exists in Editorial Guide)

#### From Editorial Style Guide §2380-2575 (AsciiDoc Publishing Reference)

**A. Section Roles (Book Doctype) — lines 2435-2471**
```
Pattern: [role]
== Heading
```

Required roles and their behaviors:
- `[dedication]` — Creates special section with boxed chapter-level heading
  - Included WITHOUT leveloffset
  - All internal headings use [discrete]
  - Single auto-TOC entry: "Dedication"
  
- `[preface]` — Creates special section with boxed chapter-level heading
  - Included WITHOUT leveloffset
  - Major subsections use NON-discrete === (appear in auto-TOC)
  - Minor subsections use [discrete] === (visual only)
  
- Cover and TOC — Use leveloffset=+1 in assembly
  - All headings [discrete] — none in auto-TOC
  - Forms preamble in book structure

**B. Auto-Generated TOC Rules — lines 2473-2505**
- Enabled with `:toc:` + `:toclevels: 3`
- Entry behaviors by element type:
  - `[dedication] == Dedication` → top-level entry
  - `[preface] == Preface` → top-level entry
  - Non-discrete `=== Subsection` → indented under parent
  - `[discrete] === Visual Heading` → NOT in auto-TOC
  - `== Glossary` → top-level entry

**Critical:** Never use `:preface-title:` — creates false TOC entries

- Auto-TOC (functional navigation, pages ii-iii, auto-generated)
- Hand-crafted TOC (visual presentation, pages 15-43, designed)
- PDF page offset: Title page + 2 auto-TOC pages = 3 pages before body page 1

**C. Page Flow Management — lines 2507-2575**
Three mechanisms control page flow:

1. **Explicit page breaks (`<<<`)**
   - Only ABSOLUTE page break
   - Use sparingly — each `<<<` guarantees new page
   - Required format: `// PDF p.N` comment + `<<<`
   - Unnecessary breaks create empty whitespace
   
2. **Heading levels in doctype: book**
   - `==` (chapter-level) force new pages automatically
   - Lower levels (`===`, `====`, `=====`) do NOT force breaks
   - Demoting `==` → `===` removes page break
   - Promoting `===` → `==` forces new page
   - In full builds, `==` still forces break even if `<<<` removed
   
3. **Section preview vs full book behavior**
   - Full book (`make book`): Both `<<<` AND `==` force breaks
   - Section preview (`make section`): Only `<<<` forces breaks

4. **Image placement and whitespace**
   - Images not fitting push to next page (natural whitespace)
   - Don't add filler
   - If whitespace exceeds 60%, consider:
     * Reordering (prose before image)
     * Adjusting image size
     * Moving related prose above image
   
5. **Table row overflow**
   - Table rows move as complete units
   - Cannot split across pages
   - Long tables naturally span multiple pages

---

### 4. Structure Pattern of Current Config

The config uses a clear organizational pattern:

```
# ============================================================================
# §N — Section Title
# =============================================================================
# 
# Brief description of what this section contains.
# Context and purpose.
# 

section_key:
  subsection_key:
    - item: value      # Comments explaining the value
```

**Comment Structure:**
- Top-level header: `# ===... §N — Title ...===`
- Pragmatic comments about purpose and usage
- Inline comments on complex values
- Biblical foundation comments when relevant
- Cross-references to related sections/docs

**YAML Structure:**
- 3-level hierarchy (section → subsection → items)
- Consistent indentation (2 spaces)
- Arrays for lists of related items
- Inline comments on values, not separate lines
- Quotes around values that look like numbers or keywords

**Version:** Living format `X.Y.Z` where:
- X = major schema (currently 5)
- Y = feature additions within schema
- Z = bug fixes and refinements

**Current highest section:** §16 (Document Types)

---

### 5. Required Data to Add (§17+)

**NEW SECTION: §17 — AsciiDoc Book Publishing Directives**

A. **Section Roles (Book Doctype)**
   ```yaml
   section_roles:
     dedication:
       role_name: "[dedication]"
       applies_to: "Chapter-level headings in frontmatter"
       behavior: "Creates special boxed section"
       include_directive: "include directive WITHOUT leveloffset"
       internal_headings: "[discrete] for all (visual only)"
       toc_entry: "Single top-level auto-TOC entry"
       
     preface:
       role_name: "[preface]"
       applies_to: "Chapter-level headings in frontmatter"
       behavior: "Creates special boxed section"
       include_directive: "include directive WITHOUT leveloffset"
       major_subsections: "NON-discrete === (appear in auto-TOC)"
       minor_subsections: "[discrete] === (visual only)"
       toc_entry: "Single top-level auto-TOC entry"
       
     cover_and_toc:
       role_name: "none (use leveloffset instead)"
       applies_to: "Cover and TOC files"
       include_directive: "include directive WITH leveloffset=+1"
       all_headings: "[discrete] (none in auto-TOC)"
       behavior: "Forms preamble in book structure"
   ```

B. **Auto-Generated TOC Configuration**
   ```yaml
   auto_toc:
     enabled: true
     attributes: [":toc:", ":toclevels: 3"]
     
     entry_rules:
       - element: "[dedication] == Dedication"
         behavior: "top-level entry"
         
       - element: "[preface] == Preface"
         behavior: "top-level entry"
         
       - element: "Non-discrete === Subsection"
         behavior: "indented entry under parent"
         parent_required: true
         
       - element: "[discrete] === Visual Heading"
         behavior: "NOT in auto-TOC"
         visual_only: true
         
       - element: "== Glossary"
         behavior: "top-level entry"
     
     pdf_page_offset: 3  # Title page + 2 auto-TOC pages before body p.1
     
     hand_crafted_toc:
       location: "Visual presentation pages"
       pages: [15, 43]
       purpose: "Designed content, distinct from auto-TOC"
     
     critical_rule: "Never use :preface-title: — creates false TOC entries"
   ```

C. **Page Flow Management**
   ```yaml
   page_flow:
     mechanisms: 3
     
     # 1. Explicit page breaks
     explicit_breaks:
       syntax: "<<<\n// PDF p.N comment"
       absoluteness: "Only absolute page break"
       usage: "Sparingly — each break guarantees new page"
       impact: "Unnecessary breaks create empty whitespace"
       required_comment: true
       comment_format: "// PDF p.N"
     
     # 2. Heading levels in doctype: book
     heading_levels_book:
       h1_chapter:
         syntax: "=="
         behavior: "Force new pages automatically"
         context: "Full book assembly"
       h2_section:
         syntax: "==="
         behavior: "Do NOT force page breaks"
       h3_subsection:
         syntax: "===="
         behavior: "Do NOT force page breaks"
       h4_clause:
         syntax: "====="
         behavior: "Do NOT force page breaks"
       
       heading_as_flow_tool: true
       demoting_removes_break: "== → === removes forced page break"
       promoting_forces_break: "=== → == forces new page"
       caveat: "In full builds, == forces break even if <<< removed"
     
     # 3. Section preview vs full book
     context_dependent:
       full_book:
         source: "make book"
         controls: ["<<<", "== headings"]
         both_force_breaks: true
       section_preview:
         source: "make section"
         controls: ["<<<"]
         headings_affect_flow: false
     
     # 4. Image placement and whitespace
     images:
       behavior: "Images not fitting push to next page"
       whitespace: "Natural, acceptable"
       dont_add_filler: true
       threshold: "If whitespace exceeds 60%, consider:"
       considerations:
         - "Reorder: prose before image (fills page)"
         - "Adjust: image size (width= or pdfwidth=)"
         - "Move: related prose above image"
     
     # 5. Table row overflow
     tables:
       behavior: "Table rows move as complete units"
       split_across_pages: false
       overflow_behavior: "Long tables naturally span multiple pages"
   ```

D. **Hierarchy Mapping (AsciiDoc → Legal → Filesystem)**
   ```yaml
   hierarchy_mapping:
     - level: 0
       asciidoc: "= Title (Part)"
       renders_as: "Part I, Part II..."
       legal_level: Part
       filesystem: "Part-{N}-{name}/"
       example: "= Part 1: Identity"
       
     - level: 1
       asciidoc: "== Title (Chapter)"
       renders_as: "Article 1, Article 2..."
       legal_level: Article
       filesystem: "Article-{NN}-{name}/"
       example: "== Article 1: Entity Overview"
       
     - level: 2
       asciidoc: "=== Title"
       renders_as: "§ 1.1, § 1.2..."
       legal_level: Section
       filesystem: "{LETTER}-{NAME}.adoc"
       example: "=== A-Entity-Overview"
       
     - level: 3
       asciidoc: "==== Title"
       renders_as: "§ 1.1.1, § 1.1.2..."
       legal_level: Subsection
       filesystem: "Heading within file"
       
     - level: 4
       asciidoc: "===== Title"
       renders_as: "—"
       legal_level: Clause
       filesystem: "Heading within file"
       
     - level: 5
       asciidoc: "====== Title"
       renders_as: "—"
       legal_level: Subclause
       filesystem: "Heading within file"
   ```

---

### 6. Document Structure Context

**Book Structure:**
- Part-1-identity (Article 1-2)
- Part-2-operations (Article 3-5)
- Part-3-business (Article 6-7)
- Part-4-direction (Article 8-11)

**Total Articles:** 11
**Total Sections (Files):** ~73 section-level files
**Total Subsections:** 200+ (headings within files)

**Frontmatter:**
- A-frontmatter/ (5 files: Cover, TOC, Dedication, Preface, Glossary)
- C-appendices/ (4 files: Document Map, Scripture Index, Decision Frameworks, Anti-Patterns)
- D-backmatter/ (3 files: Acknowledgments, Colophon, Contact)

---

## RECOMMENDATIONS

1. **Add §17 — AsciiDoc Book Publishing Directives**
   - Capture section roles ([dedication], [preface], no-role for cover/toc)
   - Document auto-TOC rules and behavior
   - Encode page flow management mechanisms
   - Map AsciiDoc syntax to legal hierarchy

2. **Keep Version at 5.Y.Z**
   - No schema breaking change (just adding §17)
   - Bump minor: 5.0.0 → 5.1.0

3. **Use Existing Comment Pattern**
   - Top-level header with § notation
   - Inline comments explaining complex values
   - Cross-references to related sections
   - Biblical/philosophical foundation where relevant

4. **Structure as Hierarchical Reference**
   - Not every detail needs encoding (would be too verbose)
   - Focus on: rules, thresholds, critical constraints
   - Editorial guide (§2380-2575) remains primary reference
   - Config supplements with machine-parseable structure

---

## CURRENT FILE STATE

- **Last modification date:** 2026-02-06
- **Total lines:** 1,226
- **Format:** YAML with pragmatic comments
- **Validation:** Used by both Makefile (via config.mk) and TS builder (direct read)
- **NO page flow data currently encoded**
- **NO section roles documentation**
- **NO auto-TOC rules**
- **NO hierarchy mapping reference**

All of these exist in the editorial guide but not in the config yet.

