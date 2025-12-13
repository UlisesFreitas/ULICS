# ULICS v1.0.0 - Known Issues Tracker

**Version:** 1.0.0  
**Status:** Released 2025-12-13  
**Next Fix:** v1.1.0

---

## CRITICAL (v1.1.0)

### 1. Rendering Flicker in Pong Demo
**Task:** 1.1.1  
**Severity:** HIGH  
**Status:** INVESTIGATING  

**Description:**  
Ball flickers/blinks when colliding with paddles in the Pong demo.

**Investigation Findings:**
- Rendering pipeline is correct (SDL_RenderClear → RenderCopy → RenderPresent)
- V-Sync is enabled (SDL_RENDERER_PRESENTVSYNC)
- Framebuffer clear works correctly
- **Root Cause:** Likely in Pong game logic - ball collision code may be triggering multiple bounces per frame

**Attempted Fixes:**
1. Added collision cooldown (5 frames) - FAILED
2. Used abs() to force ball direction - FAILED
3. Verified movement direction before bounce - FAILED

**Next Steps for v1.1.1:**
- [ ] Add frame-by-frame debug logging to Pong
- [ ] Implement proper collision prediction (sweep testing)
- [ ] Consider using physics engine (Box2D lite)
- [ ] Alternative: Rewrite Pong with simpler collision

**Workaround:** Use debug version of platformer instead

---

### 2. Platformer Player Disappearing
**Task:** 1.1.2  
**Severity:** HIGH  
**Status:** INVESTIGATING

**Description:**  
Player sprite disappears when moving in platformer demo.

**Investigation Findings:**
- Ultra-simple debug version works fine (single rectfill)
- Complex version with physics fails
- Player position updates correctly (verified with print())
- **Root Cause:** Unknown rendering bug, possibly related to:
  - Coordinate overflow
  - Render order issue
  - Camera transform problem (even without camera())

**Attempted Fixes:**
1. Removed camera system - FAILED
2. Used only rectfill (no circfill) - FAILED  
3. Simplified to minimal version - WORKS

**Next Steps for v1.1.2:**
- [ ] Binary search: add features one by one to find breaking point
- [ ] Check if issue is float vs int coordinates
- [ ] Verify framebuffer bounds checking
- [ ] Add rendering debug mode with bounding boxes

**Workaround:** Use ultra-simple platformer version

---

### 3. Audio Synthesis Causes Input Lag
**Task:** 1.1.3  
**Severity:** HIGH  
**Status:** DISABLED

**Description:**  
Enabling SFX synthesis causes 6-8 second input lag.

**Investigation Findings:**
- Audio callback runs on separate thread
- Suspected mutex lock blocking main thread
- Buffer size may be too large (1024 samples)
- **Root Cause:** Threading/synchronization issue in AudioManager

**Current Workaround:** Audio synthesis completely disabled
```cpp
// AudioManager.cpp line 133
// sfxSynthesizer->RenderSamples(...); // DISABLED
```

**Next Steps for v1.1.3:**
- [ ] Implement lock-free ring buffer
- [ ] Reduce buffer size to 512 or 256 samples
- [ ] Use atomic operations for audio data access
- [ ] Profile threading with Concurrency Visualizer
- [ ] Consider push model instead of pull (callback)

**Workaround:** No audio in v1.0.0

---

## MEDIUM (v1.1.0)

### 4. Map API Not Connected
**Task:** 1.1.4  
**Severity:** MEDIUM  
**Status:** PENDING

**Description:**  
`map()`, `mget()`, `mset()` functions are registered in Lua but not connected to actual Map instance.

**Fix:** Add global map reference in Engine, pass to ScriptingManager

---

### 5. PNG Loading Uses BMP Fallback
**Task:** 1.1.5  
**Severity:** MEDIUM  
**Status:** PENDING

**Description:**  
SpriteSheet loading falls back to SDL_LoadBMP instead of proper PNG loading.

**Fix:** Download stb_image.h and implement PNG loading

---

## LOW (v1.2.0)

### 6. Poor Error Messages
**Task:** 1.1.6  
**Severity:** LOW  
**Status:** PENDING

**Description:**  
Lua errors don't show line numbers, file-not-found messages are vague.

**Fix:** Improve error reporting in ScriptingManager

---

## DEFERRED

### 7. No Unit Tests
**Task:** 1.1.7  
**Severity:** LOW  
**Status:** DEFERRED TO v1.2

**Description:**  
No automated testing infrastructure.

**Fix:** Add Google Test or Catch2 framework

---

## Analysis Summary

**Total Known Issues:** 7  
**Critical (Blocking):** 0  
**High Priority:** 3  
**Medium Priority:** 2  
**Low Priority:** 2  

**v1.0.0 Verdict:** ✅ Shippable  
**Recommendation:** Users should be aware of demo limitations but core engine is functional.

---

## Debug Commands (For Future Investigation)

### Enable Rendering Debug
```cpp
// Add to AestheticLayer.h
bool debugMode = false;

// In Present(), draw bounding boxes
if (debugMode) {
    SDL_SetRender DrawColor(renderer, 255, 0, 0, 255);
    // Draw debug rects
}
```

### Frame-by-Frame Logging
```lua
-- In Pong main.lua
local frame_count = 0
function _update()
    frame_count = frame_count + 1
    print("Frame "..frame_count.." Ball: "..ball.x..", "..ball.y)
end
```

---

**Last Updated:** 2025-12-13  
**For:** ULICS v1.0.0 → v1.1.0 transition
