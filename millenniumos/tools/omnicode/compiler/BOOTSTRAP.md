# OmniCode Bootstrap Process

> *"In the beginning was the Word"* — John 1:1

**How OmniCode compiles itself.**

---

## The Self-Hosting Goal

OmniCode achieves self-hosting when:

1. The OmniCode compiler is written in OmniCode
2. That compiler can compile itself
3. The compiled compiler produces identical output to the original

This creates a **trust chain** back to Genesis 1:1 — the foundation.

---

## Bootstrap Stages

### Stage 0: Seed Compiler (C)

**What it is:** A minimal OmniCode compiler written in C.

**Location:** `bereshit/word/work/pkg/omni/`

**Components:**
- `omni_lexer.c` - Tokenize OmniCode source
- `omni_parser.c` - Build AST from tokens
- `omni_ir.c` - Convert AST to IR
- `omni_codegen.c` - Generate C from IR
- `omni_vm.c` - Execute IR (for interpreted mode)

**This is the "bootstrap compiler" — it comes from outside OmniCode.**

```
                    ┌─────────────────────┐
                    │   Stage 0 (C)       │
                    │   Seed Compiler     │
                    └──────────┬──────────┘
                               │ compiles
                               ▼
```

### Stage 1: First OmniCode Compiler

**What it is:** The OmniCode compiler written in OmniCode.

**Compiled by:** Stage 0 (C compiler)

```
                    ┌─────────────────────┐
                    │   Stage 1 (.omni)   │
                    │   OmniCode Compiler │
                    └──────────┬──────────┘
                               │ compiled by Stage 0
                               ▼
                    ┌─────────────────────┐
                    │   Stage 1 (binary)  │
                    │   omnic-stage1      │
                    └──────────┬──────────┘
                               │ compiles
                               ▼
```

### Stage 2: Self-Compiled Compiler

**What it is:** The same OmniCode compiler, compiled by Stage 1.

**Key test:** Output must match Stage 1.

```
                    ┌─────────────────────┐
                    │   Stage 1 (binary)  │
                    │   omnic-stage1      │
                    └──────────┬──────────┘
                               │ compiles Stage 1 source
                               ▼
                    ┌─────────────────────┐
                    │   Stage 2 (binary)  │
                    │   omnic-stage2      │
                    └──────────┬──────────┘
                               │ must be identical
                               ▼
                    ┌─────────────────────┐
                    │   VERIFICATION      │
                    │   diff stage1 stage2│
                    └─────────────────────┘
```

### Stage 3: Trust Verification

**What it is:** Stage 2 compiles itself to produce Stage 3.

**Key test:** Stage 3 must match Stage 2.

```
If Stage 2 == Stage 3:
    The compiler is STABLE
    Trust chain is COMPLETE

If Stage 2 != Stage 3:
    Something is WRONG
    Debug and iterate
```

---

## The Bootstrap Commands

```bash
# Stage 0: Build the C seed compiler
cd bereshit/word/work/pkg/omni
make omnic-c          # Produces: omnic-c (C binary)

# Stage 1: Compile OmniCode compiler with C compiler
./omnic-c compiler/omnic.omni -o omnic-stage1

# Stage 2: Compile OmniCode compiler with Stage 1
./omnic-stage1 compiler/omnic.omni -o omnic-stage2

# Stage 3: Compile again with Stage 2
./omnic-stage2 compiler/omnic.omni -o omnic-stage3

# Verify: Compare binaries
diff omnic-stage2 omnic-stage3
# If empty: SUCCESS - self-hosting achieved
```

---

## OmniCode Compiler Structure

The OmniCode compiler (`omnic.omni`) has these components:

```
omnic.omni
├── lexer.omni       # Tokenization
├── parser.omni      # AST construction
├── semantic.omni    # Type checking, validation
├── ir.omni          # IR generation
├── backend/
│   ├── c.omni       # C code generation
│   ├── ir.omni      # IR bytecode generation
│   └── x86.omni     # Native x86 (future)
└── main.omni        # Driver, CLI
```

---

## Minimal Self-Hosting Subset

For initial bootstrap, only a **subset** of OmniCode needs to be self-hosting:

| Feature | Required | Why |
|---------|:--------:|-----|
| Variables | ✓ | Store values |
| Functions | ✓ | Organize code |
| If/Else | ✓ | Conditionals |
| Loops | ✓ | Iteration |
| Strings | ✓ | Source code handling |
| Arrays | ✓ | Token lists, AST nodes |
| Structs | ✓ | AST node types |
| File I/O | ✓ | Read source, write output |
| Hebrew States | ○ | Nice to have |
| Witness | ○ | Debug but not required |

**○ = Optional for initial bootstrap**

---

## Trust Anchors

### Code Trust

Every stage traces back through the chain:

```
Stage N → Stage N-1 → ... → Stage 1 → Stage 0 (C) → libtrit → Genesis 1:1
```

### Content Trust

OmniCode source has structural guarantees:

- **PRAGMA** identifies the file
- **METADATA** documents purpose
- **4-BLOCK** structure ensures completeness
- **Biblical grounding** anchors meaning

---

## Verification Criteria

Self-hosting is verified when:

1. **Functional:** Stage 1 can compile valid OmniCode
2. **Stable:** Stage 2 == Stage 3 (binary identical)
3. **Correct:** Test suite passes on all stages
4. **Complete:** Compiler can compile itself without errors

---

## Current Status

| Component | Status | Notes |
|-----------|--------|-------|
| Lexer (C) | ✓ Complete | `omni_lexer.c` |
| Parser (C) | ✓ Complete | `omni_parser.c` |
| IR (C) | ✓ Complete | `omni_ir.c` |
| CodeGen (C) | ✓ Complete | `omni_codegen.c` |
| VM (C) | ✓ Complete | `omni_vm.c` |
| Compiler Driver | ⬜ Pending | `omnic.c` |
| Test Suite | ⬜ Pending | `test/` |
| OmniCode Compiler | ⬜ Future | `omnic.omni` |

---

## Building the Seed Compiler

```bash
# In millenniumos/tools/omnicode/compiler/
make omnic-c

# This produces:
#   omnic-c           - The seed compiler (C binary)
#   libomni.a         - Static library
```

---

## Next Steps

1. **Create compiler driver** (`omnic.c`) that ties together lexer + parser + codegen
2. **Build test suite** to verify compiler correctness
3. **Write minimal omnic.omni** in OmniCode
4. **Bootstrap** through Stage 0 → 1 → 2 → 3
5. **Verify** binary identity

---

*"In the beginning was the Word, and the Word was with God, and the Word was God."*
— John 1:1

The Word creates. OmniCode creates code. Self-hosting closes the loop.
