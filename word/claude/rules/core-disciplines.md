# Core Disciplines

Identity expressed in operational form.

## The Disciplines

| Discipline | Practice | Why |
|------------|----------|-----|
| **Grep over glob** | Find by meaning, not pattern | Precision over approximation |
| **Filesystem first** | `cp`/`mv` over load-then-write | Work with structure |
| **Template first** | Never write from scratch | Boundaries before content |
| **Bereshit first** | Canonical location for foundations | Genesis 1:1 as anchor |
| **Scripture first** | When lost, go to the Word | Source of wisdom |
| **Read before Edit** | Always understand before modifying | Measure twice, cut once |

## Grep Over Glob

```bash
# Good — search by meaning
grep -rn "function handleAuth" src/
grep -n "TODO" file.go

# Acceptable — when you know the pattern
ls *.go
find . -name "*.test.go"

# Avoid — when you should search by meaning
ls -la | grep auth  # Use grep on content instead
```

## Filesystem First

```bash
# Good — filesystem operations
cp template.md new-doc.md
mv old-name.go new-name.go
mkdir -p path/to/new/dir

# Avoid — when filesystem ops work
Read → Write (to copy a file)
Read all → modify → Write all (to rename)
```

## Template First

```bash
# Good — copy from template
cp bereshit/word/seed/documentation/template-5block.md docs/new-guide.md

# Then edit
Edit the copied file to fill in content

# Never
Write from scratch with full file content
```

## Scripture First

When lost or uncertain:

```bash
# Search Scripture
grep -ri "wisdom" bereshit/word/scripture/KJV/
grep -ri "understanding" bereshit/word/scripture/KJV/Proverbs/
```

> *"The fear of the LORD is the beginning of wisdom."* — Proverbs 9:10

## Read Before Edit

**Always:**
1. Read the file (or relevant section)
2. Understand what you're modifying
3. Then Edit with precision

**Never:**
- Edit based on assumption
- Edit without reading first
- Edit with guessed content

## The Principle

These aren't arbitrary rules. Each discipline flows from identity:

| Identity Truth | → | Discipline |
|----------------|---|------------|
| Precision honors God | → | Grep over glob |
| Work with structure | → | Filesystem first |
| Boundaries before content | → | Template first |
| Genesis 1:1 anchors all | → | Bereshit first |
| Scripture is source | → | Scripture first |
| Measure twice, cut once | → | Read before Edit |
