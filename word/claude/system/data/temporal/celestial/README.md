# Celestial Layer - Biblical Foundation

**Scripture:** Genesis 1:14-18

*"And God said, Let there be lights in the firmament of the heaven to divide the day from the night; and let them be for signs, and for seasons, and for days, and years... And God made two great lights; the greater light to rule the day, and the lesser light to rule the night: he made the stars also."*

## Purpose

God's created order for marking time through celestial bodies.

## What Belongs Here

### Solar Data

- Sunrise/sunset times by location and date
- Solar position calculations
- Daylight hours
- Solar year definitions

### Lunar Data

- Moon phases (new, waxing, full, waning)
- Lunar month cycles
- Moonrise/moonset times

### Seasonal Data

- Equinoxes and solstices
- Season transitions
- Earth's axial tilt effects

### Location Context

- Geographic coordinates for calculations
- Timezone mappings to solar position
- Latitude/longitude effects on day/night

## Why This Matters

**Not arbitrary definitions:** "Morning" isn't just "6 AM - 12 PM" universally. It's sunrise to solar noon, which varies by location and season.

**Circadian alignment:** Human rhythms follow God's created order. True temporal awareness respects this.

**Foundation for other layers:** Calendar and definitions should anchor to celestial reality, not human convention alone.

## Data Structure

```bash
celestial/
├── templates/                    # Templates for adding new locations
│   ├── README.md                # How to use templates
│   ├── location-config.template.jsonc
│   ├── solar-monthly.template.jsonc
│   ├── lunar-monthly.template.jsonc
│   └── seasonal-yearly.template.jsonc
├── locations/
│   ├── _location-registry.jsonc # Index of all locations
│   └── st-louis/                # Example: St. Louis, MO
│       ├── config.jsonc         # Location configuration
│       ├── solar/               # Monthly solar data (YYYY-MM.jsonc)
│       ├── lunar/               # Monthly lunar data (YYYY-MM.jsonc)
│       └── seasonal/            # Yearly seasonal data (YYYY.jsonc)
└── README.md
```

## Data Format

**Location Config** (`locations/[id]/config.jsonc`):

- Geographic coordinates (lat/long/elevation)
- Timezone information (IANA identifier, UTC offsets, DST rules)
- Context (who uses this location, for what purpose)
- Extension point for future location-specific data

**Solar Data** (`locations/[id]/solar/YYYY-MM.jsonc`):

- Daily sunrise/sunset times
- Solar noon (sun's highest point)
- Day length
- Monthly summaries
- Extension point for twilight, solar angles, etc.

**Lunar Data** (`locations/[id]/lunar/YYYY-MM.jsonc`):

- Major phases (New, First Quarter, Full, Last Quarter)
- Daily moonrise/moonset times
- Phase names and illumination percentages
- Lunar age (days since new moon)
- Monthly summaries with traditional full moon names
- Extension point for lunar distance, tidal data, etc.

**Seasonal Data** (`locations/[id]/seasonal/YYYY.jsonc`):

- Equinoxes and solstices (exact times)
- Season date ranges (astronomical, not meteorological)
- Cross-quarter days (traditional midpoints)
- Typical weather patterns for location
- Extension point for meteorological seasons, climate data, etc.

## Using This Data

1. **Check location registry** to see what locations are available
2. **Read location config** to get geographic/timezone info
3. **Load celestial data** for the time period you need
4. **Use in temporal awareness** - sunrise determines "morning", not arbitrary clock time

## Adding a New Location

See `templates/README.md` for complete instructions.

Quick summary:

1. Create location folder structure
2. Copy and fill templates
3. Generate celestial data using astronomical calculator
4. Register location in `_location-registry.jsonc`

## Current Locations

- **St. Louis, MO** - Primary location for CreativeWorkzStudio LLC
  - Solar: November-December 2025
  - Lunar: November-December 2025
  - Seasonal: 2025

---

*Status: Data structure proven, templates created, schemas pending*
*Last Updated: 2025-11-07*
