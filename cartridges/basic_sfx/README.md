# Basic SFX Demo

A simple ULICS cartridge demonstrating the audio API with sound effects.

## Features

- **Beep Sound**: Press `Z` to play a 440Hz square wave
- **Boop Sound**: Press `X` to play a 220Hz sine wave
- **Visual Feedback**: Circles light up when sounds play
- **Sound Stats**: Track which sounds have been played

## Controls

- **Z Key** (Button 4): Play beep sound (SFX 0)
- **X Key** (Button 5): Play boop sound (SFX 1)

## Audio System Overview

This demo showcases ULICS's procedural audio synthesis:

### SFX Format
Sound effects are defined in `sfx.json` with:
- **Waveform**: square, sine, triangle, sawtooth, noise
- **Frequency**: Base pitch in Hz
- **Duration**: Length in seconds
- **ADSR Envelope**: Attack, Decay, Sustain, Release

### Example: Beep Sound (SFX 0)
```json
{
  "id": 0,
  "name": "beep",
  "waveform": "square",
  "frequency": 440,
  "duration": 0.15,
  "volume": 0.5,
  "envelope": {
    "attack": 0.01,
    "decay": 0.05,
    "sustain": 0.7,
    "release": 0.09
  }
}
```

## Lua API

### Play Sound Effect
```lua
sfx(id)              -- Play SFX on auto channel
sfx(id, channel)     -- Play SFX on specific channel (0-7)
sfx(id, channel, offset)  -- Play with time offset
```

### Stop Sound Effect
```lua
sfx(-1)              -- Stop all SFX
sfx(-1, 2)           -- Stop SFX on channel 2
```

## Code Structure

### main.lua
```lua
function _update()
    if btnp(4) then
        sfx(0)  -- Play beep
    end
end
```

Simple and clean - just like PICO-8!

## Technical Details

- **Audio Backend**: SDL2 raw audio API
- **Sample Rate**: 44100 Hz
- **Channels**: 8 SFX channels (mono or stereo)
- **Synthesis**: Real-time waveform generation
- **No file loading**: All audio synthesized from JSON specs

## Try It Yourself

1. **Modify sfx.json**: Change frequencies, waveforms, or envelopes
2. **Add More Sounds**: Define SFX IDs 3, 4, 5...
3. **Create Music**: See `docs/MUSIC_FORMAT.md` for tracker-style music

## Next Steps

- Check `docs/SFX_FORMAT.md` for complete format specification
- Check `docs/MUSIC_FORMAT.md` for music patterns
- Explore `cartridges/api_test` for more API examples

---

**ULICS Audio System** - Procedural synthesis inspired by PICO-8 and TIC-80
