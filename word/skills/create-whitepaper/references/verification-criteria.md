# Per-Block Verification Criteria

> **Reference for:** [create-whitepaper skill](../SKILL.md)
>
> **Purpose:** Explicit completion checklists for each 5-block structure unit

---

## The Principle

A block is complete when it meets ALL its criteria, not when you've written content in it. This prevents moving forward with incomplete blocks that destabilize later work.

---

## METADATA Block — Complete When:

| Criterion | Verification Question | How to Check |
|-----------|----------------------|--------------|
| Identity established | Does `:key:` uniquely identify this document? | No other doc should have same key |
| Biblical grounding present | Is `:biblical_foundation:` meaningful, not decorative? | Quote should relate to document purpose |
| Derivation chain traceable | Does `:derives_from:` point to real sources? | Can navigate to source files |
| All required attributes present | Are key/title/type/status/version complete? | Check each attribute exists and has value |

---

## HEADER Block — Complete When:

| Criterion | Verification Question | How to Check |
|-----------|----------------------|--------------|
| Title matches key | Does the `= {title}` render correctly? | Preview the document |
| Navigation functional | Do all `<<anchor>>` links resolve? | Click each link |
| Abstract distilled | Can a newcomer understand purpose in 30 seconds? | Read aloud, time it |
| Entry points clear | Are there multiple paths into the document? | Count distinct navigation options |

---

## CONTEXT Block — Complete When:

| Criterion | Verification Question | How to Check |
|-----------|----------------------|--------------|
| Scope defined | Can reader know what IS and ISN'T covered? | Look for explicit "In scope" / "Out of scope" |
| Prerequisites stated | Does reader know what they need before starting? | Check for prerequisites section |
| Terms defined | Are key terms defined before use? | Search for first use of technical terms |
| Quick start accessible | Can beginner get oriented without reading everything? | Follow quick start path yourself |

---

## CONTENT Block (Each Layer) — Complete When:

| Criterion | Verification Question | How to Check |
|-----------|----------------------|--------------|
| Anchor present | Does `[[layer-N-name]]` exist and is it meaningful? | `grep -n "^\[\[layer"` |
| "Stands on" explicit | Is dependency on previous layer stated? | Look for "Stands on: <<layer-N-1>>" |
| Claim distilled | Is main text precise without being cryptic? | Read claim aloud—is it quotable? |
| Evidence included | Does sidebar/footnote provide support? | Each claim should have evidence |
| Cross-refs functional | Do `<<>>` links work? | Click each reference |
| Index terms present | Are key concepts marked with `indexterm:[]`? | Search for indexterm in section |
| Academic prose proper | Does intro set epistemological frame? | First paragraph should name what's being claimed |
| Prior Understanding sidebar (if applicable) | Is source material traced with line numbers? | Check quotes have attribution |

---

## FOOTER Block — Complete When:

| Criterion | Verification Question | How to Check |
|-----------|----------------------|--------------|
| Appendices include full material | Is verbose content accessible? | Check `include::` paths resolve |
| Biblical foundation present | Is Scripture grounding explicit? | Look for quoted Scripture |
| Index terms collected | Would index be useful? | Count `indexterm:[]` in document |
| Document info complete | Can someone trace provenance? | Check version, date, authors |
| See Also connects to related work | Are related documents linked? | Check cross-document references |

---

## The Verification Cycle

```text
BLOCK: [Name]
├── PLAN ─────► What goes in this block? (before writing)
├── WRITE ────► Create the content (build up)
├── VERIFY ───► Check against criteria table above
│   ├── ALL criteria met? ──► Mark complete, move to next block
│   └── Criteria missing? ──► Return to WRITE, don't move forward
└── Only mark TODO complete when ALL criteria verified
```

---

## Why This Matters

| Without Criteria | With Criteria |
|------------------|---------------|
| "Feels done" → move forward | Checkable → move forward only when verified |
| Discover gaps late → expensive fixes | Discover gaps immediately → cheap fixes |
| Uncertain completion → anxiety | Clear completion → confidence |
| Block instability propagates | Block stability enables building |
