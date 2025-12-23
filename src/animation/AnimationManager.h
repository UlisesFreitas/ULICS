#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include "animation/Animation.h"
#include <vector>
#include <string>
#include <memory>

/**
 * @brief Manager for all animations in the game
 * 
 * Responsibilities:
 * - Load/Save animations from/to JSON file (animations.json)
 * - Add/Remove/Find animations
 * - Update all active animations each frame
 * - Provide API for Lua scripting
 */
class AnimationManager {
public:
    AnimationManager();
    ~AnimationManager();
    
    // === File I/O ===
    
    /**
     * @brief Load animations from JSON file
     * @param filepath Path to animations.json
     * @return true if loaded successfully, false on error
     */
    bool LoadFromFile(const std::string& filepath);
    
    /**
     * @brief Save animations to JSON file
     * @param filepath Path to animations.json
     * @return true if saved successfully, false on error
     */
    bool SaveToFile(const std::string& filepath);
    
    // === Animation Management ===
    
    /**
     * @brief Add a new animation
     * @param anim Animation to add
     * @return true if added, false if name already exists
     */
    bool AddAnimation(const Animation& anim);
    
    /**
     * @brief Remove animation by name
     * @param name Animation name
     * @return true if removed, false if not found
     */
    bool RemoveAnimation(const std::string& name);
    
    /**
     * @brief Get animation by name
     * @param name Animation name
     * @return Pointer to animation or nullptr if not found
     */
    Animation* GetAnimation(const std::string& name);
    
    /**
     * @brief Get animation by name (const version)
     */
    const Animation* GetAnimation(const std::string& name) const;
    
    /**
     * @brief Check if animation exists
     * @param name Animation name
     * @return true if exists
     */
    bool Exists(const std::string& name) const;
    
    /**
     * @brief Get all animation names
     * @return Vector of animation names
     */
    std::vector<std::string> GetAnimationNames() const;
    
    /**
     * @brief Get total count of animations
     */
    int GetAnimationCount() const;
    
    /**
     * @brief Clear all animations
     */
    void Clear();
    
    /**
     * @brief Get all animations (for UI display)
     * @return Const reference to animations vector
     */
    const std::vector<Animation>& GetAllAnimations() const { return animations; }
    
    // === Playback Control ===
    
    /**
     * @brief Start/resume animation
     * @param name Animation name
     * @return true if started, false if not found
     */
    bool Play(const std::string& name);
    
    /**
     * @brief Stop animation and reset to frame 0
     * @param name Animation name
     * @return true if stopped, false if not found
     */
    bool Stop(const std::string& name);
    
    /**
     * @brief Pause animation at current frame
     * @param name Animation name
     * @return true if paused, false if not found
     */
    bool Pause(const std::string& name);
    
    /**
     * @brief Reset animation to frame 0 without stopping
     * @param name Animation name
     * @return true if reset, false if not found
     */
    bool Reset(const std::string& name);
    
    // === Query Functions ===
    
    /**
     * @brief Get current sprite ID for animation
     * @param name Animation name
     * @return Sprite ID (0-255) or -1 if not found/invalid
     */
    int GetCurrentSpriteId(const std::string& name) const;
    
    /**
     * @brief Check if animation is playing
     * @param name Animation name
     * @return true if playing
     */
    bool IsPlaying(const std::string& name) const;
    
    /**
     * @brief Check if non-loop animation has finished
     * @param name Animation name
     * @return true if finished
     */
    bool IsFinished(const std::string& name) const;
    
    /**
     * @brief Get frame count for animation
     * @param name Animation name
     * @return Number of frames or 0 if not found
     */
    int GetFrameCount(const std::string& name) const;
    
    /**
     * @brief Get total duration for animation
     * @param name Animation name
     * @return Total duration in engine frames @60fps or 0 if not found
     */
    int GetTotalDuration(const std::string& name) const;
    
    // === Update ===
    
    /**
     * @brief Update all active animations (called each frame by Engine)
     * 
     * Increments frame timers, advances frames when needed, handles looping
     */
    void Update();
    
private:
    std::vector<Animation> animations;
    
    /**
     * @brief Update a single animation
     * @param anim Animation to update
     */
    void UpdateAnimation(Animation& anim);
    
    /**
     * @brief Find animation index by name
     * @return Index or -1 if not found
     */
    int FindAnimationIndex(const std::string& name) const;
};

#endif // ANIMATION_MANAGER_H
