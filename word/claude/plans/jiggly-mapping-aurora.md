# Plan: Config-Driven Theme Variant Overrides

## Context

The witness report shows 17 errors and 6 warnings across 3 non-primary themes (`cws-manual-theme.yml`, `cws-plain-theme.yml`, `cws-informal-theme.yml`). ALL failures are **intentional design differences** — left-aligned headings, different margins, sans-serif body, different base font sizes. The validator currently checks all 5 themes against one canonical spec in `build.config.yaml` §11-12.

**Goal:** Add per-theme override declarations to the config. The runner merges overrides before passing to validators. Validators stay unchanged — they already handle missing expected values correctly.

## Failures Being Addressed

| Theme | Fail | Warn | Causes |
|-------|:----:|:----:|--------|
| `cws-manual-theme.yml` | 1 | 2 | heading align: left; part/chapter unset |
| `cws-plain-theme.yml` | 6 | 2 | body 12pt, h1 22pt, h2 18pt, heading align left, header/footer 0.5in |
| `cws-informal-theme.yml` | 10 | 2 | header/footer 8pt, sans body, heading align left, all margins 0.75in, header/footer 0.5in |

## Implementation

### 1. Add `§19 theme_variants` to `build.config.yaml`

**File:** `company-docs/build/build.config.yaml` (after §18, before version)

```yaml
# §19 Theme Variants — Per-theme expected overrides
theme_variants:
  cws-manual-theme:
    alignment:
      heading: left
      part: ~          # null = skip check (intentionally unset)
      chapter: ~

  cws-plain-theme:
    body_size: 12
    depth:
      h1: { font_size: 22 }
      h2: { font_size: 18 }
    alignment:
      heading: left
      part: ~
      chapter: ~
    page_layout:
      header_height: "0.5in"
      footer_height: "0.5in"

  cws-informal-theme:
    font_assignments:
      base: "Noto Sans"
    supporting:
      header_footer: 8
    alignment:
      heading: left
      part: ~
      chapter: ~
    page_layout:
      margin_top: "0.75in"
      margin_right: "0.75in"
      margin_bottom: "0.75in"
      margin_left: "0.75in"
      header_height: "0.5in"
      footer_height: "0.5in"
```

Themes NOT listed (`cws-manual-legal-theme.yml`, `cws-legal-theme.yml`) validate against the canonical spec unchanged.

### 2. Add types to `config/types.ts`

**File:** `company-docs/build/builder/src/config/types.ts`

```typescript
/** Per-theme overrides — partial overrides of canonical typography/page_layout */
export interface ThemeVariantOverrides {
  body_size?: number;
  depth?: Record<string, { font_size: number }>;
  supporting?: Partial<SupportingSizes>;
  font_assignments?: Partial<FontAssignments>;
  alignment?: Record<string, string | null>;  // null = skip check
  page_layout?: Partial<PageLayoutConfig>;
}

/** §19 — Theme variant config: theme filename stem → overrides */
export type ThemeVariantsConfig = Record<string, ThemeVariantOverrides>;
```

Add to `BuildConfig`:
```typescript
/** §19 — Theme variant overrides (per-theme expected values) */
theme_variants?: ThemeVariantsConfig;
```

### 3. Add merge utility to `validate/theme/index.ts`

**File:** `company-docs/build/builder/src/validate/theme/index.ts`

Add a `mergeThemeOverrides()` function that:
- Takes canonical `TypographyConfig` + `PageLayoutConfig` + `ThemeVariantOverrides`
- Returns `{ typography: TypographyConfig, pageLayout: PageLayoutConfig }` with overrides applied
- Deep-merges: `body_size`, `depth[key].font_size`, `supporting.*`, `font_assignments.*`, `alignment.*`
- For alignment: `null` override values become empty string (triggers the existing `if (!check.expected) continue;` skip at line 372)
- For page_layout: partial merge of margin/height values

### 4. Update `witness/runner.ts` `runThemeDomain()`

**File:** `company-docs/build/builder/src/witness/runner.ts` (lines 127-211)

In the theme iteration loop (line 169), before calling validators:
1. Derive theme stem from filename: `cws-manual-theme.yml` → `cws-manual-theme`
2. Look up `config.theme_variants?.[stem]`
3. If overrides exist, call `mergeThemeOverrides()` to produce per-theme config
4. Pass merged config to validator calls instead of canonical

```typescript
const stem = basename(themeFile, '.yml');
const overrides = config.theme_variants?.[stem];
const merged = overrides
  ? mergeThemeOverrides(config.typography!, config.page_layout!, overrides)
  : { typography: config.typography!, pageLayout: config.page_layout! };

findings.push(...validateThemeFontSizes(themeFile, merged.typography));
// ... etc
```

### 5. Update `validate/runner.ts` `runValidation()`

**File:** `company-docs/build/builder/src/validate/runner.ts` (lines 56-81)

The structural runner validates only the primary theme file (`cws-manual-legal-theme.yml`) so no changes are needed here — it doesn't iterate over themes. Confirm this is correct and leave unchanged.

## Files Modified

| File | Change |
|------|--------|
| `build/build.config.yaml` | Add §19 `theme_variants` section |
| `build/builder/src/config/types.ts` | Add `ThemeVariantOverrides`, `ThemeVariantsConfig`, update `BuildConfig` |
| `build/builder/src/validate/theme/index.ts` | Add `mergeThemeOverrides()` utility |
| `build/builder/src/witness/runner.ts` | Per-theme override lookup + merge in `runThemeDomain()` |

**No validator function signatures change.** The merge happens upstream in the runner.

## Verification

1. `npx tsc --noEmit` — clean compilation
2. `node dist/cli/index.js witness --domains theme` — 0 failures, 0 warnings expected
3. `node dist/cli/index.js validate` — primary theme still fully validates
4. `node dist/cli/index.js witness --domains theme --json --out /tmp/theme-check.json` — verify all themes show only pass/info findings
