#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief Single frame in an animation
 * 
 * Represents one frame: which sprite to show and for how long
 */
struct AnimationFrame {
    int spriteId;      // Sprite ID (0-255)
    int duration;      // Duration in frames @60fps (default: 8 = ~133ms)
    
    AnimationFrame() : spriteId(0), duration(8) {}
    AnimationFrame(int id, int dur) : spriteId(id), duration(dur) {}
};

/**
 * @brief Complete animation definition
 * 
 * Contains all data needed to define and play an animation:
 * - Metadata (name, loop setting)
 * - Frame sequence (sprite IDs + durations)
 * - Runtime state (current frame, timer, playing status)
 */
struct Animation {
    // === Metadata ===
    std::string name;                      // Unique identifier (e.g., "player_walk")
    std::vector<AnimationFrame> frames;    // Sequence of frames
    bool loop;                             // Loop when finished?
    
    // === Runtime State ===
    int currentFrame;                      // Current frame index (0 to frames.size()-1)
    int frameTimer;                        // Timer for current frame (counts up to duration)
    bool playing;                          // Is animation currently playing?
    bool finished;                         // Has non-loop animation finished?
    
    // === Constructors ===
    Animation() 
        : name(""), 
          loop(true), 
          currentFrame(0), 
          frameTimer(0), 
          playing(false),
          finished(false) 
    {}
    
    Animation(const std::string& animName, bool shouldLoop = true)
        : name(animName),
          loop(shouldLoop),
          currentFrame(0),
          frameTimer(0),
          playing(false),
          finished(false)
    {}
    
    // === Helper Methods ===
    
    /**
     * @brief Get current sprite ID
     * @return Sprite ID (0-255) or -1 if no frames
     */
    int GetCurrentSpriteId() const {
        if (frames.empty()) return -1;
        if (currentFrame < 0 || currentFrame >= static_cast<int>(frames.size())) return -1;
        return frames[currentFrame].spriteId;
    }
    
    /**
     * @brief Get total number of frames
     */
    int GetFrameCount() const {
        return static_cast<int>(frames.size());
    }
    
    /**
     * @brief Get total duration in engine frames
     * @return Sum of all frame durations
     */
    int GetTotalDuration() const {
        int total = 0;
        for (const auto& frame : frames) {
            total += frame.duration;
        }
        return total;
    }
    
    /**
     * @brief Check if animation is valid (has frames)
     */
    bool IsValid() const {
        return !frames.empty() && !name.empty();
    }
    
    /**
     * @brief Reset animation to beginning
     */
    void Reset() {
        currentFrame = 0;
        frameTimer = 0;
        finished = false;
    }
    
    /**
     * @brief Start playing animation
     */
    void Play() {
        playing = true;
        if (finished) {
            Reset();
        }
    }
    
    /**
     * @brief Stop animation and reset
     */
    void Stop() {
        playing = false;
        Reset();
    }
    
    /**
     * @brief Pause animation (can be resumed)
     */
    void Pause() {
        playing = false;
    }
    
    /**
     * @brief Add a frame to the animation
     * @param spriteId Sprite ID (0-255)
     * @param duration Frame duration in engine frames @60fps (default: 8)
     */
    void AddFrame(int spriteId, int duration = 8) {
        frames.push_back(AnimationFrame(spriteId, duration));
    }
    
    /**
     * @brief Clear all frames
     */
    void ClearFrames() {
        frames.clear();
        Reset();
    }
};

#endif // ANIMATION_H
