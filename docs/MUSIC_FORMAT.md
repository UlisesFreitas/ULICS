# ULICS Music Format Specification

## Overview

ULICS music uses a **tracker-style pattern system** similar to TIC-80. Music is composed of patterns that sequence notes/SFX across multiple channels.

## File Format

```
cartridges/my_game/
└── music.json     # Music tracks and patterns
```

## Music Data Structure (music.json)

```json
{
  "version": "1.0",
  "tempo": 120,
  "tracks": [
    {
      "id": 0,
      "name": "main_theme",
      "tempo": 140,
      "loop": true,
      "patterns": [0, 1, 0, 2]
    }
  ],
  "patterns": [
    {
      "id": 0,
      "name": "intro",
      "rows": 32,
      "channels": [
        {
          "channel": 0,
          "notes": [
            {"row": 0, "note": "C-4", "sfx": 5},
            {"row": 4, "note": "E-4", "sfx": 5},
            {"row": 8, "note": "G-4", "sfx": 5}
          ]
        }
      ]
    }
  ]
}
```

## Fields

### Track Object

- **id**: Unique track ID (0-255)
- **name**: Human-readable name
- **tempo**: BPM (beats per minute), default: 120
- **loop**: Whether track loops (true/false)
- **patterns**: Array of pattern IDs to sequence

### Pattern Object

- **id**: Unique pattern ID (0-255)
- **name**: Pattern name
- **rows**: Number of rows (typically 16, 32, or 64)
- **channels**: Array of channel data

### Channel Data

- **channel**: Channel index (0-3, 4 channels total)
- **notes**: Array of note events

### Note Event

- **row**: Row index (0-based)
- **note**: Note in format "NOTE-OCTAVE" or "---" for rest
  - Examples: `"C-4"`, `"A#3"`, `"Db5"`, `"---"`
- **sfx**: SFX ID to trigger (references sfx.json)
- **volume** (optional): Volume override (0.0-1.0)

## Note Format

Notes use tracker notation:
```
C-4  = Middle C (261.63 Hz)
C#4  = C sharp 4
Db4  = D flat 4 (same as C#4)
---  = Rest (no note)
OFF  = Note off (stop channel)
```

**Octave range**: 0-8 (C-0 to B-8)

## Example: Simple Melody

```json
{
  "version": "1.0",
  "tempo": 120,
  "tracks": [
    {
      "id": 0,
      "name": "melody",
      "tempo": 120,
      "loop": true,
      "patterns": [0]
    }
  ],
  "patterns": [
    {
      "id": 0,
      "name": "verse",
      "rows": 16,
      "channels": [
        {
          "channel": 0,
          "notes": [
            {"row": 0,  "note": "C-4", "sfx": 5},
            {"row": 2,  "note": "D-4", "sfx": 5},
            {"row": 4,  "note": "E-4", "sfx": 5},
            {"row": 6,  "note": "F-4", "sfx": 5},
            {"row": 8,  "note": "G-4", "sfx": 5},
            {"row": 10, "note": "A-4", "sfx": 5},
            {"row": 12, "note": "B-4", "sfx": 5},
            {"row": 14, "note": "C-5", "sfx": 5}
          ]
        }
      ]
    }
  ]
}
```

## Example: Multi-Channel Song

```json
{
  "version": "1.0",
  "tempo": 140,
  "tracks": [
    {
      "id": 0,
      "name": "boss_battle",
      "tempo": 140,
      "loop": true,
      "patterns": [0, 0, 1, 1]
    }
  ],
  "patterns": [
    {
      "id": 0,
      "name": "main_riff",
      "rows": 16,
      "channels": [
        {
          "channel": 0,
          "notes": [
            {"row": 0, "note": "E-3", "sfx": 10},
            {"row": 4, "note": "E-3", "sfx": 10},
            {"row": 8, "note": "G-3", "sfx": 10},
            {"row": 12, "note": "E-3", "sfx": 10}
          ]
        },
        {
          "channel": 1,
          "notes": [
            {"row": 0, "note": "C-5", "sfx": 12},
            {"row": 8, "note": "D-5", "sfx": 12}
          ]
        },
        {
          "channel": 2,
          "notes": [
            {"row": 0, "note": "C-1", "sfx": 15},
            {"row": 4, "note": "C-1", "sfx": 15},
            {"row": 8, "note": "C-1", "sfx": 15},
            {"row": 12, "note": "C-1", "sfx": 15}
          ]
        }
      ]
    }
  ]
}
```

## Lua API Usage

```lua
-- Play music track
music(0)              -- Play track 0 from start
music(0, 500)         -- Fade in over 500ms
music(0, 0, 0x03)     -- Play only channels 0 and 1

-- Stop music
music(-1)             -- Stop immediately
music(-1, 1000)       -- Fade out over 1000ms

-- Check if music is playing
if music_playing() then
    -- ...
end
```

## Technical Details

- **4 music channels** (independent from 8 SFX channels)
- **Tempo**: 60-240 BPM
- **Pattern length**: 16-64 rows typical
- **Rows per second**: `tempo / 60 * 4` (at 120 BPM = 8 rows/sec)
- **Note resolution**: Based on row timing

## Frequency Table (A440 Standard)

```
C-4  = 261.63 Hz (Middle C)
C#4  = 277.18 Hz
D-4  = 293.66 Hz
D#4  = 311.13 Hz
E-4  = 329.63 Hz
F-4  = 349.23 Hz
F#4  = 369.99 Hz
G-4  = 392.00 Hz
G#4  = 415.30 Hz
A-4  = 440.00 Hz
A#4  = 466.16 Hz
B-4  = 493.88 Hz
```

## Comparison with TIC-80

**TIC-80:**
- Built-in tracker editor
- 4 channels
- Pattern-based sequencing

**ULICS:**
- JSON-based (external editing)
- 4 music channels  
- Pattern sequencer
- SFX-based notes
- Tracker editor planned (Phase 5.5)

---

**Version**: 1.0  
**Status**: Specification Complete  
**Implementation**: Phase 5.14
