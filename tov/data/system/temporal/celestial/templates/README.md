# Celestial Data Templates

**Purpose:** Templates for creating celestial data for new locations

## Quick Start: Adding a New Location

### 1. Create Location Folder Structure

```bash
mkdir -p locations/[location-id]/{solar,lunar,seasonal}
```

Example:

```bash
mkdir -p locations/tokyo/{solar,lunar,seasonal}
```

### 2. Create Location Config

1. Copy `location-config.template.jsonc`
2. Save as `locations/[location-id]/config.jsonc`
3. Fill in all `[PLACEHOLDERS]` with actual values
4. Get coordinates from Google Maps or GPS device
5. Find IANA timezone at: <https://en.wikipedia.org/wiki/List_of_tz_database_time_zones>

### 3. Generate Celestial Data

**Solar Data (Monthly):**

1. Copy `solar-monthly.template.jsonc`
2. Save as `locations/[location-id]/solar/YYYY-MM.jsonc`
3. Generate data using astronomical calculator:
   - Option 1: SunCalc library
   - Option 2: PyEphem
   - Option 3: NOAA Solar Calculator (<https://www.esrl.noaa.gov/gmd/grad/solcalc/>)
4. Create one file per month

**Lunar Data (Monthly):**

1. Copy `lunar-monthly.template.jsonc`
2. Save as `locations/[location-id]/lunar/YYYY-MM.jsonc`
3. Generate data using astronomical calculator:
   - Option 1: MoonCalc library
   - Option 2: PyEphem
   - Option 3: USNO Astronomical Applications
4. Create one file per month

**Seasonal Data (Yearly):**

1. Copy `seasonal-yearly.template.jsonc`
2. Save as `locations/[location-id]/seasonal/YYYY.jsonc`
3. Get equinox/solstice times from:
   - USNO Data Services
   - JPL Horizons System
   - Astronomical calculation library
4. Create one file per year
5. **Important:** Adjust season names for hemisphere!

### 4. Register Location

1. Open `locations/_location-registry.jsonc`
2. Add new entry to `locations` array:

   ```jsonc
   {
     "location_id": "tokyo",
     "display_name": "Tokyo, Japan",
     "country": "Japan",
     "state_province": "Tokyo Prefecture",
     "coordinates": {
       "latitude": 35.6762,
       "longitude": 139.6503
     },
     "timezone": "Asia/Tokyo",
     "config_path": "./tokyo/config.jsonc",
     "data_available": {
       "solar": ["2025-01", "2025-02"],  // List months you created
       "lunar": ["2025-01", "2025-02"],
       "seasonal": ["2025"]
     },
     "primary_users": ["Your Name"],
     "status": "active",
     "notes": "Brief description"
   }
   ```

3. Update `total_locations` count at top of registry file

### 5. Update Registry After Adding Data

Every time you add more months of solar/lunar data:

1. Update `data_available` arrays in registry
2. Update `last_updated` date

## Data Generation Tips

### Local-First Philosophy

**Don't rely on live APIs at runtime.** Instead:

1. Use API/library ONCE to generate months/years of data
2. Store locally as `.jsonc` files
3. Regenerate only when:
   - Adding new time periods
   - Fixing errors in existing data
   - Updating for timezone/DST rule changes

### Recommended Data Ranges

- **Solar/Lunar:** Generate at least 6-12 months at a time
- **Seasonal:** Generate 1-2 years at a time
- **Why:** Reduces regeneration frequency, works offline

### Timezone Handling

**Critical:** Pay attention to DST transitions!

- Times should be in LOCAL timezone
- `timezone_offset` should reflect DST if active that month
- Solar/lunar times automatically adjust when clocks change
- Example: March in USA might be "-05:00" (CDT), November might be "-06:00" (CST)

### Hemisphere Considerations

**Northern Hemisphere:**

- Vernal Equinox (Mar) = Spring starts
- Summer Solstice (Jun) = Summer starts
- Autumnal Equinox (Sep) = Fall starts
- Winter Solstice (Dec) = Winter starts

**Southern Hemisphere:**

- Vernal Equinox (Mar) = Fall starts
- Summer Solstice (Jun) = Winter starts
- Autumnal Equinox (Sep) = Spring starts
- Winter Solstice (Dec) = Summer starts

## Template Files

| Template | Purpose | Output Location |
|----------|---------|-----------------|
| `location-config.template.jsonc` | Location configuration | `locations/[id]/config.jsonc` |
| `solar-monthly.template.jsonc` | Monthly solar data | `locations/[id]/solar/YYYY-MM.jsonc` |
| `lunar-monthly.template.jsonc` | Monthly lunar data | `locations/[id]/lunar/YYYY-MM.jsonc` |
| `seasonal-yearly.template.jsonc` | Yearly seasonal data | `locations/[id]/seasonal/YYYY.jsonc` |

## Data Quality Checklist

Before considering a location "complete":

- [ ] Location config has accurate coordinates
- [ ] Location config has correct timezone identifier
- [ ] Solar data covers at least 3 months
- [ ] Lunar data covers same months as solar
- [ ] Seasonal data exists for the year
- [ ] All times are in LOCAL timezone
- [ ] DST transitions handled correctly
- [ ] Location registered in `_location-registry.jsonc`
- [ ] Spot-check a few dates against known source

## Tools for Data Generation

**You have multiple options - choose what works for you:**

### Option 1: Online Calculators (No Coding Required)

**Recommended for:** Small datasets, one-time generation, no programming needed

- **NOAA Solar Calculator**: <https://www.esrl.noaa.gov/gmd/grad/solcalc/>
  - Enter location coordinates
  - Generate full year of sunrise/sunset data
  - Download as CSV, then convert to JSON format

- **timeanddate.com**: <https://www.timeanddate.com/sun/>
  - Month-by-month solar/lunar data
  - Visual calendar with phases
  - Copy data manually or scrape (with permission)

- **USNO Data Services**: <https://aa.usno.navy.mil/data/>
  - Authoritative astronomical data
  - Equinoxes, solstices, moon phases
  - Sunrise/sunset tables

### Option 2: Calculation Libraries (Any Language)

**Recommended for:** Bulk generation, multiple locations, automation

**Python** (if you prefer Python):

```python
import ephem  # PyEphem for astronomical calculations
# or
from astral import LocationInfo
from astral.sun import sun
```

**JavaScript** (if you prefer JavaScript):

```javascript
const SunCalc = require('suncalc');
const MoonCalc = require('mooncalc');
```

**Go** (if you prefer compiled languages):

```go
import "github.com/nathan-osman/go-sunrise"
```

**Any language with astronomical calculation libraries works!**

- The data format is just JSON
- Generate however you prefer
- Validate against provided schemas

### Option 3: API Services (For Bulk Generation)

- Sunrise-Sunset.org API (free for reasonable use)
- AstroAPI
- Weather APIs (many include sun/moon data)

**Use once to generate data, store locally - don't depend on runtime API calls**

### Option 4: Manual Entry (Small Datasets)

**For just a few months of data:**

- Look up values from online calculators
- Enter manually into JSON files
- Validate against schema to ensure correctness

**Perfectly valid approach!** Data is data, regardless of how you obtained it.

## Extending the Data

The `extension` object in each file allows future additions without breaking existing structure.

**Good candidates for extension:**

- Twilight times (civil, nautical, astronomical)
- Solar elevation/azimuth angles
- Lunar distance (perigee/apogee)
- Eclipse information
- UV index
- Tidal data (for coastal locations)

**How to add:**

1. Add data to `extension` object first
2. Test with your location
3. If it proves useful, promote to main structure
4. Update templates and schemas accordingly

## Biblical Foundation

All celestial data honors God's created order:

> "And God said, Let there be lights in the firmament of the heaven to divide the day from the night; and let them be for signs, and for seasons, and for days, and years" - Genesis 1:14

This isn't arbitrary - God made the sun, moon, and stars to mark time. True temporal awareness anchors to this celestial reality, not human convention alone.

---

*Last Updated: 2025-11-07*
*For questions or improvements, see main temporal/ README.md*
