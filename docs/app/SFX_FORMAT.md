# ULICS SFX Format Specification

## Overview

ULICS sound effects are synthesized waveforms inspired by PICO-8 and TIC-80 chiptune systems. SFX are defined in JSON format with waveform parameters and ADSR envelopes.

## File Format

```
cartridges/my_game/
└── sfx.json       # Sound effects library
```

## SFX Data Structure (sfx.json)

```json
{
  "version": "1.0",
  "sfx": [
    {
      "id": 0,
      "name": "jump",
      "waveform": "square",
      "frequency": 440,
      "duration": 0.2,
      "volume": 0.5,
      "envelope": {
        "attack": 0.01,
        "decay": 0.05,
        "sustain": 0.7,
        "release": 0.1
      },
      "frequency_sweep": {
        "start": 440,
        "end": 220,
        "duration": 0.2
      },
      "vibrato": {
        "rate": 5.0,
        "depth": 10.0
      }
    }
  ]
}
```

## Fields

### SFX Object

- **id**: Unique SFX ID (0-255)
- **name**: Human-readable name
- **waveform**: Waveform type
  - `"square"` - Square wave (50% duty cycle)
  - `"triangle"` - Triangle wave
  - `"sine"` - Sine wave
  - `"sawtooth"` - Sawtooth wave
  - `"noise"` - White noise
- **frequency**: Base frequency in Hz (e.g., 440 = A4)
- **duration**: Duration in seconds
- **volume**: Volume (0.0 to 1.0)

### ADSR Envelope (optional)

- **attack**: Attack time (seconds) - fade in
- **decay**: Decay time (seconds) - fade to sustain level
- **sustain**: Sustain level (0.0 to 1.0)
- **release**: Release time (seconds) - fade out

### Frequency Sweep (optional)

- **start**: Starting frequency (Hz)
- **end**: Ending frequency (Hz)
- **duration**: Sweep duration (seconds)

### Vibrato (optional)

- **rate**: Vibrato rate (Hz)
- **depth**: Vibrato depth (Hz)

## Example SFX Library

```json
{
  "version": "1.0",
  "sfx": [
    {
      "id": 0,
      "name": "coin",
      "waveform": "square",
      "frequency": 880,
      "duration": 0.15,
      "volume": 0.6,
      "envelope": {
        "attack": 0.01,
        "decay": 0.05,
        "sustain": 0.0,
        "release": 0.09
      }
    },
    {
      "id": 1,
      "name": "explosion",
      "waveform": "noise",
      "frequency": 0,
      "duration": 0.5,
      "volume": 0.8,
      "envelope": {
        "attack": 0.001,
        "decay": 0.2,
        "sustain": 0.3,
        "release": 0.3
      }
    },
    {
      "id": 2,
      "name": "laser",
      "waveform": "sawtooth",
      "frequency": 1200,
      "duration": 0.3,
      "volume": 0.5,
      "frequency_sweep": {
        "start": 1200,
        "end": 200,
        "duration": 0.3
      }
    }
  ]
}
```

## Lua API Usage

```lua
-- Play sound effect
sfx(0)           -- Play SFX 0 on auto channel
sfx(1, 2)        -- Play SFX 1 on channel 2
sfx(2, -1, 0.5)  -- Play SFX 2 with 0.5 volume offset

-- Stop SFX
sfx(-1)          -- Stop all SFX
sfx(-1, 2)       -- Stop SFX on channel 2
```

## Technical Notes

- **8 SFX channels** available (0-7)
- **Sample rate**: 44100 Hz
- **Synthesis**: Real-time procedural generation
- **No file loading**: All SFX synthesized from JSON specs
- **Memory efficient**: Only parameters stored, audio generated on-the-fly

## Comparison with PICO-8/TIC-80

**PICO-8:**
- 64 SFX slots
- 8 channels
- Built-in editor

**TIC-80:**
- Built-in tracker
- Waveform editing

**ULICS:**
- JSON-based (externally editable)
- 256 SFX IDs
- 8 channels
- Synthesized in real-time
- Editor planned for Phase 5.5

---

**Version**: 1.0  
**Status**: Specification Complete  
**Implementation**: Phase 5.12
