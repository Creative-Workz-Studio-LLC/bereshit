# CWS Manual Builder

Build system for the Creative Workz Studio Company Identity Manual.

Compiles AsciiDoc source files into multiple output formats:
- **HTML** - Web-ready single-page document
- **PDF** - Print-ready document
- **EPUB** - E-reader compatible format
- **DocBook** - XML interchange format

## Quick Start

```bash
# From company-docs directory:
make setup    # Install dependencies (one-time)
make html     # Build HTML version
make pdf      # Build PDF version
make all      # Build all formats
```

## Prerequisites

- **Node.js** >= 18.0.0
- **Ruby** with gem (for Asciidoctor tools)

## Installation

### Option 1: Using Make (Recommended)

```bash
make setup
```

This installs both Node.js dependencies and Asciidoctor tools.

### Option 2: Manual Installation

```bash
# Install Node.js dependencies
cd builder
npm install
npm run build

# Install Asciidoctor tools
gem install asciidoctor
gem install asciidoctor-pdf
gem install asciidoctor-epub3
gem install rouge
```

## Usage

### Command Line (Make)

```bash
make html       # Build HTML only
make pdf        # Build PDF only
make epub       # Build EPUB only
make all        # Build all formats
make watch      # Watch mode (auto-rebuild HTML)
make clean      # Clean output directory
make check      # Verify prerequisites
make info       # Show output file info
```

### Command Line (npm)

```bash
cd builder
npm run manual:html     # Build HTML
npm run manual:pdf      # Build PDF
npm run manual:epub     # Build EPUB
npm run manual:all      # Build all formats
npm run manual:watch    # Watch mode
npm run clean           # Clean output
```

### CLI Tool

```bash
cd builder
node dist/cli.js --help

# Examples:
node dist/cli.js --format html
node dist/cli.js --format pdf,epub
node dist/cli.js --format all --parallel
node dist/cli.js --watch
node dist/cli.js --clean
node dist/cli.js --check
```

## Output

Generated files are written to `company-docs/output/`:

```
output/
├── CWS-Company-Identity-Manual.html
├── CWS-Company-Identity-Manual.pdf
├── CWS-Company-Identity-Manual.epub
└── CWS-Company-Identity-Manual.xml
```

## Programmatic Usage

The build system can be used as a library:

```typescript
import { createBuilder, loadConfig, type OutputFormat } from '@cws/manual-builder';

const config = loadConfig({
  outputDir: './custom-output',
});

const builder = createBuilder(config, (event) => {
  console.log('Build event:', event.type);
});

// Build specific formats
const results = await builder.build(['html', 'pdf']);

// Or build all formats in parallel
const allResults = await builder.buildParallel();
```

## Configuration

Edit `builder/src/config.ts` to customize:

- Output filenames
- Document attributes
- Format-specific options (PDF themes, paper size, etc.)

## Troubleshooting

### "asciidoctor: command not found"

Install Asciidoctor:
```bash
gem install asciidoctor
```

Or use system package manager:
```bash
# Ubuntu/Debian
sudo apt install asciidoctor

# macOS
brew install asciidoctor
```

### "asciidoctor-pdf: command not found"

Install asciidoctor-pdf:
```bash
gem install asciidoctor-pdf
```

### Ruby gem permission errors

Use `--user-install` flag:
```bash
gem install asciidoctor --user-install
```

Or configure gem to install to user directory.

## Project Structure

```
builder/
├── src/
│   ├── cli.ts        # Command-line interface
│   ├── config.ts     # Build configuration
│   ├── formats.ts    # Format handlers
│   ├── builder.ts    # Build orchestration
│   └── index.ts      # Library exports
├── dist/             # Compiled JavaScript (generated)
├── package.json
├── tsconfig.json
└── README.md
```

## License

Proprietary - Creative Workz Studio LLC
