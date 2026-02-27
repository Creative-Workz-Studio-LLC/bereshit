# cws-struct Code Standards

## TypeScript/Deno

- Strict TypeScript with Deno runtime
- Import maps for depth-independent imports
- `@lib/` -> `./lib/`, `@test/` -> `./tests/`, `@std/` -> `jsr:@std/`
- JSONC for configuration (Deno ignores `_` prefixed keys)

## File Structure

All code files follow 4-block structure:
```
METADATA -> SETUP -> BODY -> CLOSING
```

- SETUP: imports, constants, types
- BODY: implementation
- CLOSING: exports, validation

## Testing

- Test files in `tests/` directory mirror source structure
- Layer-based organization: R50 (existence), R25 (organization), R10 (content), R05 (pattern)
- Error codes in brackets: `[CWS-R50-020]` enables `deno test --filter`
- Two tests ignored (FFI): `database_test.ts`, `health_query_test.ts`

## Error Codes

210 error codes organized dimensionally: `[CWS-R50-020]`
- R50 = layer (existence), 020 = domain code
- Dimensional form: `R50.BLK.020` (computed via `dimensionalCode()`)
- Full catalog in `lib/data/errors.ts`

## Adding New Formats

1. Create handler directory in `lib/handlers/<format>/`
2. Implement handler interface
3. Register in `lib/handlers/mod.ts`
4. Add tests in `tests/` at each layer
5. Add deno.jsonc task entries
