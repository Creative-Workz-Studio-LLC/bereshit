# Research: Deno Compile — Embedding Static Files in Binaries

## Question

How to embed default schemas in a `deno compile` binary while still allowing runtime filesystem overrides? What mechanisms does Deno provide, and what's the best pattern for cws-struct?

## Structure Observed

This research touches:
- Deno runtime documentation (official docs, blog posts)
- cws-struct codebase (schema loading in `lib/foundation/schema.ts` and `lib/foundation/code-schema.ts`)
- 12 JSONC schema files in `b-word/L1-omnicode/ladder/schemas/`

## Key Findings

### 1. JSON Import Assertions — YES, bundled into binary

**Syntax (Deno 1.17+, stable):**
```typescript
import data from "./schema.jsonc" with { type: "json" };
```

- JSON imports are part of the **module graph** — Deno statically analyzes them
- `deno compile` bundles the entire module graph into an **eszip** inside the binary
- **JSON files imported this way ARE automatically included** in the compiled binary — no extra flags needed
- **JSONC is NOT supported** via import assertions — only standard `.json` files work with `with { type: "json" }`
- For JSONC, you need `@std/jsonc` parse at runtime (which cws-struct already uses)

### 2. `--include` Flag — The File Embedding Mechanism (Deno 2.1+)

**Syntax:**
```bash
deno compile --include ./schemas/ --include ./config.jsonc main.ts
```

- Ships files/directories inside the binary's virtual filesystem
- Read at runtime with `Deno.readTextFile(import.meta.dirname + "/schemas/file.jsonc")`
- `Deno.readDir()` also works on included directories
- Entire directories can be included with a single flag
- `import.meta.dirname` resolves correctly inside the compiled binary — it points to the virtual filesystem location
- **This is the mechanism for JSONC files** since they can't use import assertions

### 3. `Deno.readTextFile()` in Compiled Binaries

- **For `--include`'d files:** YES, works transparently. The runtime intercepts the read and serves from the embedded virtual filesystem
- **For files NOT included:** Falls through to the real filesystem. If the file exists on disk, it reads normally. If not, throws `NotFound`
- **This is the key insight:** `Deno.readTextFile()` checks the virtual FS first, then falls through to real FS

### 4. `--unstable-raw-imports` (Deno 2.4+)

**Syntax:**
```typescript
import text from "./hello.txt" with { type: "text" };
import bytes from "./image.png" with { type: "bytes" };
```

- Imports non-JS files directly into the module graph as text or bytes
- Works with `deno compile` — assets embedded at compile time
- Currently **unstable** (`--unstable-raw-imports` flag required)
- Useful for embedding text files as string constants at compile time

### 5. Current cws-struct Schema Loading

**Pattern used today (both `schema.ts` and `code-schema.ts`):**
```typescript
const SCHEMA_REL_PATH = "../../../../b-word/L1-omnicode/ladder/schemas/data/format/toml-3block-schema.jsonc";

function resolveSchemaPath(): string {
  const thisDir = dirname(fromFileUrl(import.meta.url));
  return join(thisDir, SCHEMA_REL_PATH);
}

// Later:
text = await Deno.readTextFile(schemaPath);
parsed = parseJsonc(text);
```

- Uses `import.meta.url` to find the source file location
- Navigates with relative paths (4 levels up) to reach the schema directory
- Reads JSONC at runtime via `Deno.readTextFile()` + `@std/jsonc` parse
- Caches after first load (module-level `cached` variable)

### 6. Existing Compile Task

Already defined in `deno.jsonc` line 176:
```
"compile": "deno compile --allow-read --allow-write --allow-run --allow-net --output bin/cws-struct mod.ts"
```

**No `--include` flags** — schemas would NOT be available in the compiled binary today.

## Detailed Analysis

### The Problem

cws-struct loads 12+ JSONC schema files at runtime via relative paths from source location. When compiled to a binary, those relative paths point to... nothing (the source tree isn't there). The compile task exists but doesn't include the schemas.

### The Solution: Three Approaches (Ranked)

#### Approach A: `--include` the schemas directory (RECOMMENDED)

```bash
deno compile \
  --allow-read --allow-write --allow-run --allow-net \
  --include ../../b-word/L1-omnicode/ladder/schemas/ \
  --output bin/cws-struct \
  mod.ts
```

**Then in schema loaders, use `import.meta.dirname`:**
```typescript
function resolveSchemaPath(): string {
  const thisDir = import.meta.dirname!;
  return join(thisDir, SCHEMA_REL_PATH);
}
```

**Why this works:**
- `import.meta.dirname` resolves to the virtual FS location in compiled binary
- `Deno.readTextFile()` transparently reads from the virtual FS
- The same code works in dev mode (`deno run`) AND compiled mode
- JSONC files are included as-is, parsed at runtime with `@std/jsonc`
- Directory structure preserved — all 12 schemas in their subdirectories
- **No code changes needed** to the loading logic itself (just ensure `import.meta.dirname` is used)

**Override pattern:**
```typescript
// Check for user override first, fall back to embedded
async function loadSchemaWithOverride(embeddedPath: string, overridePath?: string): Promise<string> {
  if (overridePath) {
    try {
      return await Deno.readTextFile(overridePath);
    } catch { /* fall through to embedded */ }
  }
  return await Deno.readTextFile(embeddedPath);
}
```

#### Approach B: Import JSON directly (won't work for JSONC)

```typescript
import schema from "./toml-3block-schema.json" with { type: "json" };
```

**Problems:**
- Only works with standard JSON, not JSONC (our schemas have comments)
- Would require converting all schemas from JSONC to JSON (losing comments)
- Or maintaining parallel JSON copies
- Not worth it — `--include` handles JSONC natively

#### Approach C: `--unstable-raw-imports` (Deno 2.4+)

```typescript
import schemaText from "../../b-word/.../toml-3block-schema.jsonc" with { type: "text" };
const schema = parseJsonc(schemaText);
```

**Problems:**
- Currently **unstable** — not production-ready
- Requires flag on every invocation
- Would embed schemas as string constants (larger binary, less flexible)
- Can't be overridden at runtime without additional logic

### Recommended Implementation Pattern

**For cws-struct, the pattern should be:**

1. **Default:** Schemas embedded in binary via `--include`
2. **Override:** CLI flag `--schema-dir <path>` or env var `CWS_STRUCT_SCHEMA_DIR` to load from filesystem instead
3. **Resolution order:**
   - If override path specified, use it (real filesystem)
   - Otherwise, use `import.meta.dirname`-relative path (embedded in binary, or source tree in dev)

```typescript
function resolveSchemaDir(): string {
  // 1. Explicit override (CLI or env)
  const override = Deno.env.get("CWS_STRUCT_SCHEMA_DIR");
  if (override) return override;

  // 2. Default: relative to module (works both dev and compiled)
  return join(import.meta.dirname!, SCHEMA_REL_DIR);
}
```

**Compile command update:**
```
"compile": "deno compile --allow-read --allow-write --allow-run --allow-net --allow-env --include ../../b-word/L1-omnicode/ladder/schemas/ --output bin/cws-struct mod.ts"
```

## Files Examined

| File | Purpose |
|------|---------|
| `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/deno.jsonc` | Project config, existing compile task (line 176) |
| `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/mod.ts` | CLI entrypoint |
| `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/lib/foundation/schema.ts` | TOML schema loader — uses `import.meta.url` + relative path + `Deno.readTextFile` |
| `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/lib/foundation/code-schema.ts` | Go/Rust schema loader — same pattern as schema.ts |
| `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/lib/studio/serve.ts` | Studio server — also uses `import.meta.url` for static files |
| 12 `.jsonc` schema files in `b-word/L1-omnicode/ladder/schemas/` | The schemas that need embedding |

## Environment

- **Deno version:** 2.6.9 (stable) — supports `--include` (2.1+) and `--unstable-raw-imports` (2.4+)
- **All features needed are available** in the installed version

## Recommendations

1. **Use `--include` with the schemas directory** — simplest, most robust, production-stable
2. **Switch from `import.meta.url` to `import.meta.dirname`** in schema loaders (minor refactor, more idiomatic for Deno 2.x)
3. **Add `--allow-env` to compile task** to support `CWS_STRUCT_SCHEMA_DIR` override
4. **Update compile task** in `deno.jsonc` to include the schemas directory
5. **Don't convert JSONC to JSON** — `--include` handles JSONC files perfectly since we parse them ourselves
6. **Studio's `public/` directory should also be `--include`'d** — same pattern, same solution
7. **Consider `--unstable-raw-imports` for future** once it stabilizes — cleaner for truly static data

## Open Questions

1. **Schema directory depth in `--include`:** Need to verify that `--include ../../b-word/L1-omnicode/ladder/schemas/` preserves the full subdirectory structure (code/format/, data/format/, etc.) inside the binary. The docs say directories are included recursively, so this should work.
2. **`import.meta.url` vs `import.meta.dirname`:** Current code uses `dirname(fromFileUrl(import.meta.url))` — is this equivalent to `import.meta.dirname` in compiled binaries? The Deno 2.1 blog specifically recommends `import.meta.dirname` for this use case.
3. **Studio static files:** `lib/studio/serve.ts` uses `import.meta.url` to find `public/` and `cwe-creator-tools/`. These would also need `--include` flags for the compiled binary.

## Sources

- [Deno compile documentation](https://docs.deno.com/runtime/reference/cli/compile/)
- [Deno 2.1: Wasm Imports and other enhancements](https://deno.com/blog/v2.1) — introduced `--include`
- [Deno 2.4: deno bundle is back](https://deno.com/blog/v2.4) — introduced `--unstable-raw-imports`
- [Importing JSON — Deno examples](https://docs.deno.com/examples/importing_json/)
- [GitHub issue #20885 — Embed files when using deno compile](https://github.com/denoland/deno/issues/20885) — feature request (COMPLETED)
- [GitHub issue #21173 — import.meta.url in compiled binaries](https://github.com/denoland/deno/issues/21173) — path resolution concerns (pre-2.1)
- [Self-contained Executable Programs with Deno Compile](https://deno.com/blog/deno-compile-executable-programs) — overview
