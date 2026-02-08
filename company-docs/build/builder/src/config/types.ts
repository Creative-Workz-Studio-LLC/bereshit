/**
 * CWS Manual Builder — Configuration Types
 *
 * Type definitions for the build configuration system.
 * All interfaces mirror the structure of build.config.yaml.
 *
 * Specification: editorial-style-guide.adoc (CWS-GDE-001)
 *
 * "For which of you, intending to build a tower, sitteth not down first,
 *  and counteth the cost, whether he have sufficient to finish it?"
 *  — Luke 14:28
 */

// =============================================================================
// Format Configuration
// =============================================================================

/**
 * Single format configuration from build.config.yaml.
 * Each format defines how to invoke an Asciidoctor backend.
 */
export interface FormatConfig {
  /** Whether this format is built by default */
  enabled: boolean;

  /** CLI command to invoke (e.g., "asciidoctor-pdf") */
  command: string;

  /** Asciidoctor backend name (e.g., "pdf", "html5", "docbook5") */
  backend: string;

  /** File extension for output (e.g., ".pdf", ".html") */
  extension: string;

  /** Installation hint shown when tool is missing */
  install: string;

  /** Format-specific options (paper_size, compress, etc.) */
  options: Record<string, unknown>;

  /** Additional CLI arguments passed to the command */
  args: string[];

  /**
   * Pipeline strategy for multi-step conversions.
   * - undefined: Direct command execution (default)
   * - 'docbook-intermediate': AsciiDoc → DocBook XML → pandoc → target format
   *   Pandoc's DocBook reader is excellent; its AsciiDoc reader is not.
   */
  pipeline?: string;
}

// =============================================================================
// Watch Configuration
// =============================================================================

/**
 * Watch mode configuration for the TS builder.
 * For editorial watch, use `make watch-pdf` instead.
 */
export interface WatchConfig {
  /** Glob patterns to watch for changes */
  patterns: string[];

  /** Glob patterns to ignore */
  ignore: string[];

  /** Debounce delay in milliseconds */
  debounce_ms: number;

  /** Formats to rebuild on change */
  formats: string[];
}

// =============================================================================
// Display Configuration
// =============================================================================

/**
 * CLI display settings.
 */
export interface DisplayConfig {
  /** Show startup banner */
  banner: boolean;

  /** Enable colored output */
  colors: boolean;

  /** Icon map (format names → emoji, status → emoji) */
  icons: Record<string, string>;
}

// =============================================================================
// Document Configuration
// =============================================================================

/**
 * Document identity and paths.
 */
export interface DocumentConfig {
  /** Master assembly file (relative to config directory) */
  master: string;

  /** Output filename stem (extensions added per format) */
  output_name: string;

  /** Output directory (relative to config directory) */
  output_dir: string;
}

// =============================================================================
// Asset Configuration
// =============================================================================

/**
 * Logo asset entry.
 */
export interface LogoEntry {
  /** Filename within the assets directory */
  filename: string;

  /** Purpose description */
  purpose: string;

  /** Specifications (dimensions, format, etc.) */
  specifications: string;

  /** Current status (present, missing, placeholder) */
  status: string;
}

/**
 * Logos configuration — wraps logo entries with directory context.
 * Mirrors build.config.yaml assets.logos structure.
 */
export interface LogosConfig {
  /** Logos subdirectory within assets */
  directory: string;

  /** Individual logo entries */
  items: LogoEntry[];
}

/**
 * Figure catalog entry — a single diagram in the manual.
 */
export interface FigureCatalogEntry {
  /** Figure identifier (e.g., "fig-manual-structure") */
  id: string;

  /** Brief content description */
  content: string;

  /** Where this figure is used in the manual */
  used_in: string;
}

/**
 * Figures configuration — diagram rendering pipeline.
 *
 * Directory layout (post-reorganization):
 *   figures/png/        Color PNGs (Mermaid output + palette swatch)
 *   figures/svg/        SVG variants (Mermaid output)
 *   figures/grayscale/  Grayscale PNGs (derived from color PNGs)
 *   sources/diagrams/   Mermaid .mmd source files
 *   sources/config/     Rendering configs (puppeteer, mermaid theme)
 */
export interface FiguresConfig {
  /** PNG output directory (relative to assets directory) */
  png_directory: string;

  /** SVG output directory (relative to assets directory) */
  svg_directory: string;

  /** Grayscale output directory (relative to assets directory) */
  grayscale_directory: string;

  /** Mermaid source files directory (relative to assets directory) */
  sources: string;

  /** Rendering config directory (relative to assets directory) */
  config: string;

  /** Output format strategy: "dual" = SVG + PNG */
  format: string;

  /** PNG scale factor for print quality (e.g., 3 = 3x = 300+ DPI) */
  png_scale: number;

  /** Rendering tool command (e.g., "mmdc" for mermaid-cli) */
  renderer: string;

  /** Theme configuration file path (relative to assets directory) */
  theme: string;

  /** Complete catalog of all figures */
  catalog: FigureCatalogEntry[];
}

/**
 * Grayscale generation configuration.
 */
export interface GrayscaleConfig {
  /** Enable grayscale variant generation */
  enabled: boolean;

  /** Filenames to exclude from grayscale conversion */
  exclude: string[];
}

/**
 * Palette swatch generation configuration.
 */
export interface PaletteConfig {
  /** Enable palette swatch generation */
  enabled: boolean;

  /** Output path (relative to assets directory) */
  output: string;

  /** Swatch image width in pixels */
  width: number;

  /** Swatch image height in pixels */
  height: number;

  /** Bold font name for labels */
  font_bold: string;

  /** Regular font name for values */
  font_regular: string;
}

/**
 * Title page mockup generation configuration.
 */
export interface MockupConfig {
  /** Enable mockup generation */
  enabled: boolean;

  /** Output path (relative to assets directory) */
  output: string;
}

/**
 * Derived assets configuration — auto-generated from source assets.
 * Mirrors build.config.yaml assets.derived section.
 */
export interface DerivedAssetsConfig {
  /** Grayscale variant generation */
  grayscale: GrayscaleConfig;

  /** Brand color palette swatch */
  palette: PaletteConfig;

  /** Title page mockup */
  mockup: MockupConfig;
}

/**
 * Assets configuration — mirrors build.config.yaml assets section.
 */
export interface AssetsConfig {
  /** Assets directory name (relative to book/) */
  directory: string;

  /** Logo configuration (directory + entries) */
  logos: LogosConfig;

  /** Figures configuration and catalog */
  figures: FiguresConfig;

  /** Derived asset generation settings */
  derived?: DerivedAssetsConfig;
}

// =============================================================================
// Scripture Configuration (§7)
// =============================================================================

/**
 * Scripture validation configuration.
 * Used by `cws-build lint --bible` to validate cited verses against
 * canonical text from the Bereshit scripture data.
 *
 * "All scripture is given by inspiration of God..." — 2 Timothy 3:16
 */
export interface ScriptureConfig {
  /** Path to scripture data (relative to company-docs/) */
  base_path: string;

  /** Supported translation identifiers (e.g., ["KJV", "WEB"]) */
  translations: string[];

  /** Default translation when citation doesn't specify */
  default_translation: string;
}

// =============================================================================
// Paths Configuration (§8)
// =============================================================================

/**
 * Makefile project layout paths.
 * All paths relative to company-docs/ (Makefile working directory).
 * The config.mk generator translates these into Make variables.
 */
export interface PathsConfig {
  /** Book source directory */
  book_dir: string;

  /** Preview build directory */
  preview_dir: string;

  /** Shared attributes file path */
  shared_attributes: string;

  /** Theme file path */
  theme_file: string;

  /** Font directory (semicolon-separated for asciidoctor-pdf) */
  font_dir: string;
}

// =============================================================================
// Scopes Configuration (§9)
// =============================================================================

/**
 * Single scope definition — maps a book directory to an editorial preview build.
 */
export interface ScopeDefinition {
  /** Human-readable scope name */
  name: string;

  /** Book subdirectory for this scope */
  directory: string;

  /** Preview assembly file name */
  preview: string;

  /** Output PDF filename */
  output: string;
}

/**
 * All scope definitions, keyed by scope letter (A, B, C, D).
 */
export type ScopesConfig = Record<string, ScopeDefinition>;

// =============================================================================
// Brand Configuration (§10)
// =============================================================================

/**
 * Brand color definitions.
 * Hex values with # prefix.
 */
export interface BrandColorsConfig {
  /** Deep blue — primary brand color */
  deep_blue: string;

  /** Medium blue — secondary brand color */
  medium_blue: string;

  /** Gold — accent color */
  gold: string;

  /** Dark — text and dark backgrounds */
  dark: string;

  /** Light — light backgrounds */
  light: string;
}

/**
 * Brand configuration — visual identity parameters.
 */
export interface BrandConfig {
  /** Brand color palette */
  colors: BrandColorsConfig;
}

// =============================================================================
// Typography Configuration (§11)
// =============================================================================

/**
 * Single depth level in the governance hierarchy.
 * Maps an AsciiDoc heading level to a governance label and font size.
 */
export interface DepthLevel {
  /** AsciiDoc heading marker (e.g., "==") */
  asciidoc: string;

  /** Theme YAML key path (e.g., "heading.h1") */
  theme_key: string;

  /** Governance label (e.g., "Article") */
  label: string;

  /** Font size in points */
  font_size: number;

  /** Position on the Perfect Fourth scale (0 = body anchor) */
  scale_step: number;

  /** Font specification (e.g., "Noto Sans Bold") */
  font: string;
}

/**
 * Supporting font sizes not in the governance depth hierarchy.
 */
export interface SupportingSizes {
  lead: number;
  small: number;
  code: number;
  footnotes: number;
  title_page: number;
  header_footer: number;
  toc: number;
}

/**
 * Font stack assignments by role.
 */
export interface FontStack {
  body: string;
  headings: string;
  code: string;
  hebrew: string;
  symbol: string;
}

/**
 * Font-family assignments by theme element.
 * Maps each theme element to its expected font-family.
 * All themes share the same font assignments.
 */
export interface FontAssignments {
  base: string;
  heading: string;
  part: string;
  chapter: string;
  toc: string;
  code: string;
  codespan: string;
  footnotes: string;
  header: string;
  footer: string;
  caption: string;
  sidebar_title: string;
  admonition_label: string;
  title_page_title: string;
  title_page_subtitle: string;
  title_page_authors: string;
  title_page_revision: string;
}

/**
 * Semantic color assignments by theme element.
 * Maps each element to its canonical font-color hex value.
 * All themes share the same color palette.
 *
 * Guide §WCAG Color Compliance + §Font Styling
 */
export interface ColorAssignments {
  body: string;
  heading: string;
  part: string;
  chapter: string;
  lead: string;
  quote: string;
  secondary: string;
  header: string;
  footer: string;
  caption: string;
  footnotes: string;
  link: string;
  code: string;
  title_page_title: string;
  title_page_subtitle: string;
  title_page_authors: string;
  title_page_revision: string;
}

/**
 * Custom role color definitions.
 * Maps role names to their font-color hex values.
 *
 * Guide §Custom Roles
 */
export interface RoleColors {
  gold: string;
  muted: string;
  blue: string;
  green: string;
  purple: string;
  red: string;
}

/**
 * Font-style assignments by theme element.
 * Maps each element to its expected font-style (bold, italic, normal).
 *
 * Guide §Font Styling + §Headings
 */
export interface StyleAssignments {
  base: string;
  heading: string;
  part: string;
  chapter: string;
  code: string;
  codespan: string;
  footnotes: string;
  header: string;
  footer: string;
  caption: string;
  sidebar_title: string;
  admonition_label: string;
  title_page_title: string;
  title_page_subtitle: string;
  title_page_authors: string;
  title_page_revision: string;
}

/**
 * Text alignment expectations for the primary theme.
 * Alignment varies by theme purpose:
 *   manual-legal / legal → centered headings
 *   manual               → left-aligned headings
 */
export interface AlignmentConfig {
  /** Heading text-align for the primary theme */
  heading: string;

  /** Part text-align (always centered) */
  part: string;

  /** Chapter text-align (always centered) */
  chapter: string;
}

/**
 * Legal typography standards (Guide §Legal Typography — Butterick Standards).
 *
 * Validated against legal/governance themes (manual-legal, legal).
 * Not enforced on general manual theme.
 */
export interface LegalTypographyConfig {
  /** Body text must use serif font */
  body_serif: boolean;

  /** First-line indent for governance prose paragraphs */
  prose_indent: string;

  /** Justified text acceptable in typeset context */
  prose_justify: boolean;

  /** Line height percentage (Butterick recommends 120-145%, CWS uses 150%) */
  line_height_percent: number;

  /** Line length target in characters (45-90 range, CWS targets ~75) */
  line_length_target: number;

  /** Which heading level uses ALL CAPS (e.g., "h5") */
  uppercase_heading: string;

  /** Letter-spacing for uppercase headings */
  uppercase_letter_spacing: string;
}

/**
 * Table theme element canonical values.
 * Header colors, body stripe, border/grid colors.
 *
 * Guide §Tables, Lists, and Figures > Table Conventions
 */
export interface TableElementConfig {
  /** Header row background color (deep blue) */
  head_background_color: string;

  /** Header row text color (white on blue) */
  head_font_color: string;

  /** Header row font style */
  head_font_style: string;

  /** Alternating row stripe background */
  body_stripe_color: string;

  /** Table outer border color */
  border_color: string;

  /** Internal grid line color */
  grid_color: string;
}

/**
 * List theme element canonical values.
 * Marker color and item spacing.
 *
 * Guide §Tables, Lists, and Figures > List Conventions
 */
export interface ListElementConfig {
  /** Bullet/number marker font color */
  marker_font_color: string;

  /** Spacing between list items in points */
  item_spacing: number;
}

/**
 * Description-list theme element canonical values.
 * Term styling for definition lists.
 *
 * Guide §Tables, Lists, and Figures > List Conventions
 */
export interface DescriptionListElementConfig {
  /** Definition term font style */
  term_font_style: string;

  /** Definition term font color */
  term_font_color: string;
}

/**
 * Tables, Lists & Figures theme element canonical values.
 * Validated against all 5 theme YAMLs.
 *
 * Guide §Tables, Lists, and Figures
 */
export interface TablesListsFiguresConfig {
  /** Table element settings */
  table: TableElementConfig;

  /** Ordered/unordered list settings */
  list: ListElementConfig;

  /** Description list settings */
  description_list: DescriptionListElementConfig;

  /** Caption alignment */
  caption: { align: string };

  /** Image alignment */
  image: { align: string };
}

/**
 * Typography configuration — type scale, depth hierarchy, font stack,
 * font assignments, alignment, legal typography standards, and
 * table/list/figure element settings.
 * Canonical source of truth for all theme typography.
 *
 * Guide §Typography + §Tables, Lists, and Figures
 */
export interface TypographyConfig {
  /** Body text size in points (type scale anchor) */
  body_size: number;

  /** Scale ratio (Perfect Fourth = 1.333) */
  scale_ratio: number;

  /** Line height in points */
  line_height: number;

  /** Governance depth hierarchy (part, h1-h6) */
  depth: Record<string, DepthLevel>;

  /** Supporting sizes outside the hierarchy */
  supporting: SupportingSizes;

  /** Font stack by role */
  fonts: FontStack;

  /** Font-family per theme element */
  font_assignments: FontAssignments;

  /** Text alignment expectations (primary theme) */
  alignment: AlignmentConfig;

  /** Semantic color assignments (font-color per element) */
  colors?: ColorAssignments;

  /** Custom role colors */
  roles?: RoleColors;

  /** Font-style assignments (bold, italic, normal per element) */
  styles?: StyleAssignments;

  /** Legal typography standards (Butterick) — validated on legal themes */
  legal?: LegalTypographyConfig;

  /** Tables, Lists & Figures element settings */
  tables_lists_figures?: TablesListsFiguresConfig;
}

// =============================================================================
// Page Layout Configuration (§12)
// =============================================================================

/**
 * Page layout measurements — physical page framework.
 * Validated against the primary theme.
 *
 * Guide §Page Layout
 */
export interface PageLayoutConfig {
  /** Page size name (e.g., "Letter") */
  page_size: string;

  /** Page orientation */
  page_layout: string;

  /** Page margins */
  margin_top: string;
  margin_right: string;
  margin_bottom: string;
  margin_left: string;

  /** Prepress binding margins */
  margin_inner: string;
  margin_outer: string;

  /** Header and footer height */
  header_height: string;
  footer_height: string;

  /** Vertical rhythm */
  vertical_spacing: number;
  thematic_break_margin_top: number;
  thematic_break_margin_bottom: number;

  /** Orphan prevention */
  heading_min_height_after: string;

  /** Default heading spacing */
  heading_margin_top: number;
  heading_margin_bottom: number;
  heading_line_height: number;

  /** Thematic break styling */
  thematic_break_border_width: number;
  thematic_break_border_color: string;
}

// =============================================================================
// Legal & Governance Language Configuration (§13)
// =============================================================================

/**
 * Modern register obligation words.
 * Default for operational governance text.
 *
 * Guide §Obligation Language > Modern Register
 */
export interface ModernObligationWords {
  /** Obligations — a party has a duty */
  duty: string;

  /** Future events, consequences, statements of fact */
  future: string;

  /** Permission, discretion */
  permission: string;

  /** Recommendations — non-binding guidance */
  recommendation: string;

  /** Prohibition */
  prohibition: string;
}

/**
 * Traditional register obligation words.
 * Used for covenants, founding declarations, and solemn text.
 *
 * Guide §Obligation Language > Traditional Register
 */
export interface TraditionalObligationWords {
  /** Solemn duty of the subject of the sentence */
  duty: string;

  /** Performative declarations — the statement IS the act */
  performative: string;

  /** Recitals — establishing context before operative provisions */
  recitals: string;

  /** Opening of formal recitals (archaic but recognized) */
  opening: string;

  /** Self-referencing terms within a formal document */
  self_reference: string[];

  /** Transition from recitals to operative provisions */
  transition: string;
}

/**
 * Adams Refinement (MSCD) — obligation words for bilateral agreements.
 * Three distinct verbs for three distinct situations (contracts only).
 *
 * Guide §The Adams Refinement
 */
export interface AdamsRefinement {
  /** Obligation on the subject of the sentence */
  shall: string;

  /** Obligation on someone other than the subject */
  must: string;

  /** Language of policy for contingent future events */
  will: string;
}

/**
 * Obligation language configuration — dual-register system.
 * Modern register (default) + traditional register (formal/solemn).
 *
 * Guide §Obligation Language
 */
export interface ObligationConfig {
  /** Modern register words (operational default) */
  modern: ModernObligationWords;

  /** Traditional register words (formal/solemn) */
  traditional: TraditionalObligationWords;

  /** Adams Refinement for bilateral agreements */
  adams: AdamsRefinement;

  /** Context-to-register mapping (context name → "modern" | "traditional") */
  register_map: Record<string, string>;
}

/**
 * Defined terms formatting conventions.
 * How terms with controlled meanings are introduced and maintained.
 *
 * Guide §Defined Terms
 */
export interface DefinedTermsConfig {
  /** Definition sentence format pattern */
  definition_format: string;

  /** Capitalization rule for defined terms */
  capitalization: string;

  /** Formatting on first occurrence in each major section */
  first_occurrence: string;

  /** Whether all defined terms must appear in glossary */
  glossary_required: boolean;

  /** Whether inline definitions are acceptable for single-section terms */
  inline_acceptable: boolean;
}

/**
 * Definition verb semantics — scope control.
 * The verb in a definition controls its legal scope.
 *
 * Guide §Definition Verbs — "Means" vs. "Includes"
 */
export interface DefinitionVerbsConfig {
  /** "means" → exhaustive (complete meaning) */
  means: string;

  /** "includes" → non-exhaustive (examples, not limits) */
  includes: string;

  /** "means and includes" → exhaustive with listed examples */
  means_and_includes: string;

  /** "does not include" → carve-out (explicit exclusion) */
  does_not_include: string;

  /** "refers to" → external reference (points without redefining) */
  refers_to: string;
}

/**
 * Construction conventions — rules of interpretation.
 * Standard rules that apply across all CWS governance documents.
 *
 * Guide §Construction Conventions
 */
export interface ConstructionConfig {
  /** "Article" includes "Articles" unless context requires otherwise */
  singular_includes_plural: boolean;

  /** Section headings are for navigation — they do not limit text */
  headings_not_operative: boolean;

  /** "Or" is inclusive unless explicitly exclusive */
  or_inclusive: boolean;

  /** "Including" followed by examples does not limit the general term */
  including_not_limiting: boolean;

  /** Use "they/them" for singular generic reference */
  gender_neutral: boolean;

  /** "Herein" means the whole document unless limited */
  herein_means_whole: boolean;

  /** References to statutes include successor provisions */
  statutes_include_successors: boolean;

  /** Business day definition */
  business_days: string;
}

/**
 * A single binding provision location in the document.
 */
export interface BindingLocation {
  /** Page number where the binding language appears */
  page: number;

  /** Context/section where it appears */
  context: string;

  /** Key binding language or phrase */
  language: string;
}

/**
 * Binding provisions configuration — activation pattern.
 * Three instances of binding language reinforce legal weight.
 *
 * Guide §Binding Provisions
 */
export interface BindingConfig {
  /** The mechanism that activates governance provisions */
  activation_mechanism: string;

  /** Number of required binding references */
  required_instances: number;

  /** Specific locations where binding language must appear */
  locations: BindingLocation[];
}

/**
 * A single amendment authority tier.
 */
export interface AmendmentTier {
  /** Tier number (1 = lightest, 6 = heaviest) */
  tier: number;

  /** Type of change this tier covers */
  change_type: string;

  /** Required authority for this change type */
  authority: string;
}

/**
 * Amendment authority configuration — six tiers from lightest to heaviest.
 *
 * Guide §Amendment Authority
 */
export interface AmendmentAuthorityConfig {
  /** Ordered list of amendment tiers */
  tiers: AmendmentTier[];
}

/**
 * A single governance notation entry.
 */
export interface GovernanceNotationEntry {
  /** Full term (e.g., "Article") */
  term: string;

  /** Abbreviation (e.g., "Art.") */
  abbreviation: string;

  /** Usage description */
  usage: string;

  /** Corresponding AsciiDoc heading level */
  asciidoc_level: string;
}

/**
 * Prohibited governance terminology.
 */
export interface ProhibitedTerm {
  /** The prohibited term */
  term: string;

  /** Reason it's prohibited */
  reason: string;
}

/**
 * Governance notation configuration — term hierarchy.
 * Maps structural levels to terms and abbreviations.
 *
 * Guide §Governance Notation
 */
export interface GovernanceNotationConfig {
  /** Approved governance notation terms */
  terms: GovernanceNotationEntry[];

  /** Prohibited terms (e.g., "Chapter") */
  prohibited_terms: ProhibitedTerm[];
}

/**
 * Signature and approval format configuration.
 *
 * Guide §Signature and Approval Format
 */
export interface SignatureFormatConfig {
  /** Entity name in signature block */
  entity_name: string;

  /** Entity type description */
  entity_type: string;

  /** Required fields in signature block */
  required_fields: string[];
}

/**
 * Cross-reference conventions configuration.
 *
 * Guide §Cross-Reference Conventions
 */
export interface CrossReferenceConfig {
  /** Named reference styles with examples */
  styles: Record<string, string>;

  /** Traditional self-referencing terms (for formal contexts) */
  traditional_self_reference: string[];

  /** Modern self-referencing phrases (default) */
  modern_self_reference: string[];
}

/**
 * Document anatomy — the standard elements of a formal legal instrument.
 * CWS governance documents adopt elements selectively.
 *
 * Guide §Traditional Legal Document Structure > Document Anatomy
 */
export interface DocumentAnatomyConfig {
  /** Ordered list of all 10 standard elements */
  elements: string[];

  /** Elements used by the Company Identity Manual (subset) */
  cim_elements: string[];
}

/**
 * Recitals style configuration — WHEREAS vs Background.
 *
 * Guide §Traditional Legal Document Structure > Recitals
 */
export interface RecitalsConfig {
  /** Traditional style keyword (WHEREAS clauses) */
  traditional: string;

  /** Modern style keyword (Background sections) */
  modern: string;

  /** Default style for the CIM */
  cim_default: string;
}

/**
 * A single numbering level — traditional vs modern format.
 */
export interface NumberingLevel {
  /** Traditional format (Roman/caps) */
  traditional?: string;

  /** Modern format (Arabic/MSCD) */
  modern?: string;

  /** Fixed format (same in both systems) */
  format?: string;
}

/**
 * Numbering hierarchy configuration.
 * Traditional (Roman/Decimal) vs Modern (Arabic/MSCD).
 *
 * Guide §Traditional Legal Document Structure > Traditional Numbering Hierarchy
 */
export interface NumberingConfig {
  /** Default numbering system for the CIM */
  default: string;

  /** Contexts that use traditional (Roman) numbering */
  traditional_contexts: string[];

  /** Numbering levels with format strings */
  levels: Record<string, NumberingLevel>;
}

/**
 * Enumeration punctuation rules for tabulated lists.
 *
 * Guide §Enumeration in Operative Text
 */
export interface EnumerationPunctuation {
  /** Separator between items (semicolon) */
  separator: string;

  /** "and"/"or" before the final item */
  conjunction_before_last: boolean;

  /** Period after the final item only */
  final_period: boolean;

  /** No initial capital unless complete sentence */
  initial_caps: boolean;

  /** No comma after "and"/"or" */
  no_comma_after_conjunction: boolean;
}

/**
 * Enumeration conventions for lists within operative text.
 *
 * Guide §Enumeration in Operative Text
 */
export interface EnumerationConfig {
  /** Use tabulated list when item count reaches this threshold */
  tabulated_threshold: number;

  /** Punctuation rules for tabulated lists */
  punctuation: EnumerationPunctuation;
}

/**
 * Boilerplate provisions — required general clauses.
 * Every governance document should include these.
 *
 * Guide §Boilerplate Provisions
 */
export interface BoilerplateConfig {
  /** Required provision names */
  required: string[];

  /** Governing jurisdiction for interpretation */
  governing_jurisdiction: string;
}

/**
 * Archaic legal term entry — context-sensitive detection.
 * Terms valid only in traditional register contexts.
 *
 * Guide §Anti-Patterns > Archaic Legal Language
 */
export interface ArchaicTerm {
  /** The archaic term to detect */
  term: string;

  /** Register where this term is acceptable */
  register: string;

  /** Reason / guidance for replacement */
  reason: string;
}

/**
 * WHEREAS-operative pairing config.
 * WHEREAS recitals must be followed by NOW, THEREFORE.
 *
 * Guide §Traditional Legal Document Structure > Recitals
 */
export interface WhereasOperativeConfig {
  /** Whether NOW THEREFORE is required after WHEREAS */
  require_now_therefore: boolean;

  /** Severity for missing operative clause */
  severity: string;
}

/**
 * Legal & governance language configuration.
 * Captures the dual-register obligation system, defined term formatting,
 * definition verb semantics, construction conventions, binding provisions,
 * and traditional legal document structure.
 *
 * Guide §Legal & Governance Language + §Traditional Legal Document Structure
 */
export interface LegalLanguageConfig {
  /** Obligation language — dual register system */
  obligation: ObligationConfig;

  /** Defined terms formatting conventions */
  defined_terms: DefinedTermsConfig;

  /** Definition verb semantics — scope control */
  definition_verbs: DefinitionVerbsConfig;

  /** Construction conventions — rules of interpretation */
  construction: ConstructionConfig;

  /** Binding provisions — activation pattern */
  binding: BindingConfig;

  /** Document anatomy — standard elements of legal instruments */
  document_anatomy?: DocumentAnatomyConfig;

  /** Recitals style (WHEREAS vs Background) */
  recitals?: RecitalsConfig;

  /** Numbering hierarchy (Traditional vs Modern) */
  numbering?: NumberingConfig;

  /** Enumeration conventions for operative text lists */
  enumeration?: EnumerationConfig;

  /** Required boilerplate provisions */
  boilerplate?: BoilerplateConfig;

  /** Amendment authority — 6-tier change hierarchy */
  amendment_authority?: AmendmentAuthorityConfig;

  /** Governance notation — term abbreviations and prohibited terms */
  governance_notation?: GovernanceNotationConfig;

  /** Signature format — execution block structure */
  signature_format?: SignatureFormatConfig;

  /** Cross-reference conventions — styles and self-reference terms */
  cross_reference?: CrossReferenceConfig;

  /** Archaic terms — context-sensitive detection */
  archaic_terms?: ArchaicTerm[];

  /** WHEREAS-operative pairing requirements */
  whereas_operative?: WhereasOperativeConfig;
}

// =============================================================================
// §14 — Industry-Specific Conventions
// =============================================================================

/** IP protection entry for a specific asset type. */
export interface IpProtectionEntry {
  /** Asset category (source_code, game_art_music_assets, etc.) */
  asset: string;

  /** Protection type (proprietary_license, trade_secret, limited_license, etc.) */
  protection: string;

  /** Additional notes on protection requirements */
  note: string;
}

/** EULA convention standards. */
export interface EulaConventions {
  license_grant: string;
  ip_rights: string;
  ugc_policy: string;
  termination: string;
  liability: string;
  dispute_resolution: string;
}

/** Three-license tier definition. */
export interface LicenseTier {
  /** Tier name (patterns, implementation, commercial) */
  tier: string;

  /** License type (free_open, proprietary, revenue_share) */
  license: string;

  /** What this tier covers */
  covers: string;
}

/** Game development industry-specific config. */
export interface GameDevelopmentConfig {
  /** IP protection standards by asset type */
  ip_protection: IpProtectionEntry[];

  /** EULA convention standards */
  eula_conventions: EulaConventions;

  /** Three-license structure for Bereshit technology stack */
  three_license: LicenseTier[];
}

/** Creator agreement provision standard. */
export interface CreatorAgreementProvision {
  /** Provision name (grant_of_rights, reversion_clause, etc.) */
  provision: string;

  /** CWS standard for this provision */
  standard: string;

  /** Whether this provision is non-negotiable */
  non_negotiable?: boolean;
}

/** Publishing house governance element. */
export interface PublishingGovernanceElement {
  /** Element name (editorial_independence, etc.) */
  element: string;

  /** CWS standard for this element */
  standard: string;
}

/** Publishing house industry-specific config. */
export interface PublishingHouseConfig {
  /** Creator agreement standards (WIPO + Authors Guild) */
  creator_agreements: CreatorAgreementProvision[];

  /** Publishing house governance elements */
  governance: PublishingGovernanceElement[];
}

/** §14 — Industry-specific conventions config. */
export interface IndustrySpecificConfig {
  /** Game development conventions */
  game_development: GameDevelopmentConfig;

  /** Publishing house conventions */
  publishing_house: PublishingHouseConfig;
}


// =============================================================================
// §15 — Tone & Voice
// =============================================================================

/** CWS voice attribute. */
export interface VoiceAttribute {
  /** Attribute name (professional, warm, grounded, clear, authoritative) */
  attribute: string;

  /** Description of this voice attribute */
  description: string;
}

/** Tone level mapping for a specific context. */
export interface ToneLevel {
  /** Context (governance_provisions, mission_vision, etc.) */
  context: string;

  /** Formality level (high, high-medium, medium, medium-low, low-medium) */
  formality: string;

  /** Characteristics of this tone level */
  characteristics?: string;
}

/** Covenant language replacement — compulsion → covenant. */
export interface CovenantReplacement {
  /** Compulsion phrase to avoid */
  compulsion: string;

  /** Covenant alternative to use instead */
  covenant: string;
}

/** Plain language word replacement. */
export interface WordReplacement {
  /** Word/phrase to avoid */
  avoid: string;

  /** Simpler alternative to prefer */
  prefer: string;
}

/** Plain language configuration. */
export interface PlainLanguageConfig {
  /** Maximum words per sentence (never exceed) */
  max_sentence_words: number;

  /** Target average sentence length range [min, max] */
  target_sentence_words: number[];

  /** Word replacements (complex → simple) */
  word_replacements: WordReplacement[];
}

/** Readability target for a specific audience. */
export interface ReadabilityTarget {
  /** Audience type */
  audience: string;

  /** Target grade level (single number or range [min, max]) */
  grade_level: number | number[];

  /** Additional notes */
  notes?: string;
}

/** Closing filler detection config. */
export interface ClosingFillerConfig {
  /** Generic transition phrases to flag */
  phrases: string[];
}

/** Paragraph density config. */
export interface ParagraphDensityConfig {
  /** Maximum consecutive non-blank content lines before flagging */
  max_consecutive_lines: number;
}

/** Passive voice detection config. */
export interface PassiveVoiceConfig {
  /** Whether passive voice detection is enabled */
  enabled: boolean;

  /** Severity for passive voice findings */
  severity: string;

  /** Regex patterns for passive voice constructions */
  patterns: string[];
}

/** §15 — Tone & voice config. */
export interface ToneAndVoiceConfig {
  /** CWS voice attributes (constant personality) */
  cws_voice: VoiceAttribute[];

  /** Tone spectrum — formality by context */
  tone_spectrum: ToneLevel[];

  /** Covenant vs compulsion language replacements */
  covenant_language: {
    replacements: CovenantReplacement[];
  };

  /** Plain language principles and word replacements */
  plain_language: PlainLanguageConfig;

  /** Closing filler phrase detection */
  closing_filler?: ClosingFillerConfig;

  /** Paragraph density threshold */
  paragraph_density?: ParagraphDensityConfig;

  /** Passive voice pattern detection */
  passive_voice?: PassiveVoiceConfig;

  /** Readability targets by audience */
  readability_targets: ReadabilityTarget[];
}


// =============================================================================
// §16 — Document Types
// =============================================================================

/** Document type entry with register and theme mapping. */
export interface DocumentTypeEntry {
  /** Document type identifier */
  type: string;

  /** Expected register (formal_traditional, formal_covenant, plain, informal, warm) */
  register: string;

  /** Theme YAML file to use */
  theme: string;

  /** Rationale for this register choice */
  rationale: string;
}

/** §16 — Document types config (register mapping). */
export interface DocumentTypesConfig {
  /** Available document types with register and theme mappings */
  types: DocumentTypeEntry[];
}


// =============================================================================
// §17 — AsciiDoc Publishing
// =============================================================================

/**
 * A prohibited AsciiDoc document attribute.
 * These must NOT appear in source files.
 *
 * Guide §AsciiDoc Publishing Reference > Auto-Generated TOC
 */
export interface ProhibitedAttribute {
  /** The attribute string to detect (e.g., ":preface-title:") */
  attribute: string;

  /** Why this attribute is prohibited */
  reason: string;

  /** Reference to the editorial style guide section */
  guide_ref: string;
}

/**
 * Section role definition for frontmatter special sections.
 * Validates that [dedication]/[preface] roles appear in correct files.
 *
 * Guide §AsciiDoc Publishing Reference > Section Roles
 */
export interface SectionRoleDefinition {
  /** Role name (e.g., "dedication", "preface") */
  role: string;

  /** Glob pattern for expected filename (e.g., "*DEDICATION*") */
  file_pattern: string;

  /** Expected scope (frontmatter, content, etc.) */
  scope: string;

  /** Whether the role requires ifdef::book[] guard */
  requires_ifdef_book: boolean;

  /** Whether leveloffset applies in assembly context */
  leveloffset_in_assembly: boolean;
}

/**
 * Include guard requirements for standalone rendering.
 * Files in required scopes should have ifndef::book[] wrappers.
 *
 * Guide §AsciiDoc Publishing Reference > Include Directives
 */
export interface IncludeGuardConfig {
  /** Scopes where include guards are required */
  required_scopes: string[];

  /** File patterns excluded from guard requirements */
  exclude_patterns: string[];
}

/**
 * Page flow redundancy rules.
 * Detects redundant <<< near chapter-level headings.
 *
 * Guide §AsciiDoc Publishing Reference > Page Flow Management
 */
export interface PageFlowConfig {
  /** Flag <<< immediately before a == heading (redundant in doctype:book) */
  redundant_break_before_chapter: boolean;

  /** Flag <<< immediately after a == heading */
  redundant_break_after_chapter: boolean;
}

/**
 * §17 — AsciiDoc publishing configuration.
 * Structural rules for book assembly validated by editorial lint checks 32-35.
 *
 * Guide §AsciiDoc Publishing Reference
 */
export interface AsciidocPublishingConfig {
  /** Attributes that must NOT appear in source files */
  prohibited_attributes: ProhibitedAttribute[];

  /** Required section roles and their placement rules */
  section_roles: SectionRoleDefinition[];

  /** Include guard requirements for standalone rendering */
  include_guard: IncludeGuardConfig;

  /** Page flow redundancy detection rules */
  page_flow: PageFlowConfig;

  /** Attributes that book.adoc must define */
  required_book_attributes: string[];
}


// =============================================================================
// §18 — Company Conventions
// =============================================================================

/**
 * Board member reference entry.
 * Canonical name and title for validation.
 *
 * Guide §Company Conventions > Board Members
 */
export interface BoardMember {
  /** Full name */
  name: string;

  /** Canonical title */
  title: string;
}

/**
 * Company conventions configuration — §18.
 * Legal name, standard attributes, board members, contact emails.
 *
 * Guide §Company Conventions
 */
export interface CompanyConventionsConfig {
  /** Legal entity name */
  legal_name: string;

  /** AsciiDoc attribute name for company reference */
  attribute_name: string;

  /** Raw name variants that should use the attribute instead */
  raw_name_variants: string[];

  /** Board member canonical references */
  board_members: BoardMember[];

  /** Contact email addresses by role */
  contact_emails: Record<string, string>;

  /** Required reading guide audience parties (CIM §Reading Guide Pattern) */
  reading_guide_parties?: string[];
}


// =============================================================================
// §19 — Theme Variants (Per-Theme Overrides)
// =============================================================================

/**
 * Per-theme overrides of canonical typography and page layout expectations.
 * Only the fields specified are overridden — everything else falls through
 * to the canonical spec in §11/§12.
 *
 * A `null` value in alignment means "skip this check" (intentionally unset).
 */
export interface ThemeVariantOverrides {
  /** Override body font size (pt) */
  body_size?: number;

  /** Override depth-level font sizes (key: h1, h2, etc.) */
  depth?: Record<string, { font_size: number }>;

  /** Override supporting sizes (header_footer, etc.) */
  supporting?: Partial<SupportingSizes>;

  /** Override font-family assignments (base, heading, etc.) */
  font_assignments?: Partial<FontAssignments>;

  /** Override alignment expectations — null = skip check */
  alignment?: Record<string, string | null>;

  /** Override page layout measurements (margins, heights) */
  page_layout?: Partial<PageLayoutConfig>;
}

/** §19 — Theme variant config: theme filename stem → overrides. */
export type ThemeVariantsConfig = Record<string, ThemeVariantOverrides>;


// =============================================================================
// §20 — Page Fill Analysis
// =============================================================================

/** Fill range definition — min/max percentages */
export interface FillRange {
  min: number;
  max: number;
}

/** Cross-check settings for source marker validation */
export interface FillCrossCheck {
  /** Flag pages marked "content full" below this fill % */
  stale_marker_below: number;
  /** Flag unmarked pages above this fill % */
  missing_marker_above: number;
  /** Warn on page count mismatch between source markers and PDF */
  page_count_drift: boolean;
}

/** §20 — Page fill analysis configuration */
export interface PageFillConfig {
  /** Named fill ranges with min/max percentages */
  ranges: Record<string, FillRange>;
  /** Regex pattern for matching end-of-page markers in source */
  marker_pattern: string;
  /** Cross-check settings */
  cross_check: FillCrossCheck;
  /** Percentage at or above which bleed is imminent (default: 95) */
  bleed_threshold?: number;
}


// =============================================================================
// Complete Build Configuration
// =============================================================================

/**
 * Complete build configuration — mirrors build.config.yaml structure (v5.1.0).
 * The TypeScript builder loads this and executes what it defines.
 *
 * Sections 1-6:  Core builder config (document, attributes, formats, watch, display, assets)
 * Section 7:     Scripture validation
 * Section 8:     Makefile paths
 * Section 9:     Scope definitions
 * Section 10:    Brand colors
 * Section 11:    Typography (type scale, depth hierarchy, font stack)
 * Section 12:    Page layout (margins, spacing, orphan prevention)
 * Section 13:    Legal & governance language conventions
 * Section 14:    Industry-specific conventions (game dev, publishing)
 * Section 15:    Tone & voice (CWS voice, covenant language, plain language)
 * Section 16:    Document types (register mapping)
 * Section 17:    AsciiDoc publishing (structural lint for book assembly)
 * Section 18:    Company conventions (board, legal name, emails)
 * Section 19:    Theme variants (per-theme expected overrides)
 */
export interface BuildConfig {
  /** §1 — Document identity and paths */
  document: DocumentConfig;

  /** §2 — Asciidoctor attributes (passed as -a key=value) */
  attributes: Record<string, string>;

  /** §3 — Output format definitions */
  formats: Record<string, FormatConfig>;

  /** §4 — Watch mode configuration */
  watch: WatchConfig;

  /** §5 — CLI display settings */
  display: DisplayConfig;

  /** §6 — Asset requirements and catalog */
  assets?: AssetsConfig;

  /** §7 — Scripture validation (Bible verse checking) */
  scripture?: ScriptureConfig;

  /** §8 — Makefile project layout paths */
  paths?: PathsConfig;

  /** §9 — Flat scope definitions (A, C, D) */
  scopes?: ScopesConfig;

  /** §10 — Brand visual identity */
  brand?: BrandConfig;

  /** §11 — Typography (type scale, depth hierarchy, font stack) */
  typography?: TypographyConfig;

  /** §12 — Page layout (margins, spacing, orphan prevention) */
  page_layout?: PageLayoutConfig;

  /** §13 — Legal & governance language conventions */
  legal_language?: LegalLanguageConfig;

  /** §14 — Industry-specific conventions (game dev, publishing) */
  industry_specific?: IndustrySpecificConfig;

  /** §15 — Tone & voice (CWS voice, covenant language, plain language) */
  tone_and_voice?: ToneAndVoiceConfig;

  /** §16 — Document types (register mapping) */
  document_types?: DocumentTypesConfig;

  /** §17 — AsciiDoc publishing (structural lint for book assembly) */
  asciidoc_publishing?: AsciidocPublishingConfig;

  /** §18 — Company conventions (board, legal name, emails) */
  company_conventions?: CompanyConventionsConfig;

  /** §19 — Theme variant overrides (per-theme expected values) */
  theme_variants?: ThemeVariantsConfig;

  /** §20 — Page fill analysis config */
  page_fill?: PageFillConfig;

  /** Config version */
  version: string;
}

// =============================================================================
// Runtime Paths
// =============================================================================

/**
 * Computed paths for runtime use.
 * Resolved from config values + filesystem location.
 */
export interface RuntimePaths {
  /** Absolute path to the config file itself */
  configFile: string;

  /** Directory containing the config file (company-docs/build/) */
  sourceDir: string;

  /** Absolute path to the output directory */
  outputDir: string;

  /** Absolute path to the master document (book.adoc) */
  masterDocument: string;

  /** Absolute path to the book directory */
  bookDir: string;
}
