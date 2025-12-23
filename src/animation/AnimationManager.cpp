#include "animation/AnimationManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

AnimationManager::AnimationManager() {
    std::cout << "[AnimationManager] Initialized" << std::endl;
}

AnimationManager::~AnimationManager() {
    std::cout << "[AnimationManager] Destroyed (" << animations.size() << " animations)" << std::endl;
}

// === File I/O ===

bool AnimationManager::LoadFromFile(const std::string& filepath) {
    std::cout << "[AnimationManager] Attempting to load: " << filepath << std::endl;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "[AnimationManager] File not found: " << filepath << " (this is OK for new cartridges)" << std::endl;
        return false;
    }
    
    std::cout << "[AnimationManager] File opened successfully" << std::endl;
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    if (content.empty()) {
        std::cout << "[AnimationManager] WARNING: Empty file: " << filepath << std::endl;
        return false;
    }
    
    // Manual JSON parsing (simple, no external libs)
    // Expected format:
    // {
    //   "animations": [
    //     {"name": "anim1", "frames": [0,1,2], "speeds": [8,8,8], "loop": true},
    //     ...
    //   ]
    // }
    
    Clear();  // Clear existing animations
    
    size_t animsStart = content.find("\"animations\"");
    if (animsStart == std::string::npos) {
        std::cout << "[AnimationManager] ERROR: No 'animations' key found in JSON" << std::endl;
        return false;
    }
    
    
    size_t arrayStart = content.find('[', animsStart);
    if (arrayStart == std::string::npos) {
        std::cout << "[AnimationManager] ERROR: No opening [ found for animations array" << std::endl;
        return false;
    }
    
    // Find matching closing ] by counting brackets
    int bracketCount = 1;
    size_t arrayEnd = arrayStart + 1;
    while (arrayEnd < content.size() && bracketCount > 0) {
        if (content[arrayEnd] == '[') bracketCount++;
        else if (content[arrayEnd] == ']') bracketCount--;
        arrayEnd++;
    }
    arrayEnd--;  // Back to the ]
    
    if (arrayEnd >= content.size() || bracketCount != 0) {
        std::cout << "[AnimationManager] ERROR: Unclosed animations array" << std::endl;
        return false;
    }
    
    std::cout << "[AnimationManager] Array range: [" << arrayStart << ", " << arrayEnd << "]" << std::endl;
    
    // Parse each animation object {...}
    size_t pos = arrayStart + 1;
    int loadedCount = 0;
    
    while (pos < arrayEnd) {
        // Find next animation object
        size_t objStart = content.find('{', pos);
        if (objStart == std::string::npos || objStart >= arrayEnd) break;
        
        // Find matching closing brace
        int braceCount = 1;
        size_t objEnd = objStart + 1;
        while (objEnd < arrayEnd && braceCount > 0) {
            if (content[objEnd] == '{') braceCount++;
            else if (content[objEnd] == '}') braceCount--;
            objEnd++;
        }
        objEnd--;  // Back to the closing brace
        
        if (objEnd >= arrayEnd) {
            std::cout << "[AnimationManager] WARNING: Unclosed object at pos " << objStart << std::endl;
            break;
        }
        
        // Extract animation object
        std::string obj = content.substr(objStart, objEnd - objStart + 1);
        
        // Parse fields
        Animation anim;
        
        // Parse name
        size_t namePos = obj.find("\"name\"");
        if (namePos != std::string::npos) {
            size_t nameStart = obj.find('\"', namePos + 6);
            size_t nameEnd = obj.find('\"', nameStart + 1);
            anim.name = obj.substr(nameStart + 1, nameEnd - nameStart - 1);
        }
        
        // Parse frames array
        size_t framesPos = obj.find("\"frames\"");
        if (framesPos != std::string::npos) {
            size_t framesStart = obj.find('[', framesPos);
            size_t framesEnd = obj.find(']', framesStart);
            std::string framesStr = obj.substr(framesStart + 1, framesEnd - framesStart - 1);
            
            // Remove all whitespace and newlines
            framesStr.erase(std::remove_if(framesStr.begin(), framesStr.end(), 
                [](char c) { return std::isspace(static_cast<unsigned char>(c)); }), 
                framesStr.end());
            
            std::stringstream ss(framesStr);
            int spriteId;
            while (ss >> spriteId) {
                anim.frames.push_back(AnimationFrame(spriteId, 8));  // Default duration
                if (ss.peek() == ',') ss.ignore();
            }
        }
        
        // Parse speeds array
        size_t speedsPos = obj.find("\"speeds\"");
        if (speedsPos != std::string::npos) {
            size_t speedsStart = obj.find('[', speedsPos);
            size_t speedsEnd = obj.find(']', speedsStart);
            std::string speedsStr = obj.substr(speedsStart + 1, speedsEnd - speedsStart - 1);
            
            // Remove all whitespace and newlines
            speedsStr.erase(std::remove_if(speedsStr.begin(), speedsStr.end(), 
                [](char c) { return std::isspace(static_cast<unsigned char>(c)); }), 
                speedsStr.end());
            
            std::stringstream ss(speedsStr);
            int speed;
            size_t idx = 0;
            while (ss >> speed && idx < anim.frames.size()) {
                anim.frames[idx].duration = speed;
                idx++;
                if (ss.peek() == ',') ss.ignore();
            }
        }
        
        // Parse loop
        size_t loopPos = obj.find("\"loop\"");
        if (loopPos != std::string::npos) {
            size_t truePos = obj.find("true", loopPos);
            size_t falsePos = obj.find("false", loopPos);
            anim.loop = (truePos != std::string::npos && (falsePos == std::string::npos || truePos < falsePos));
        }
        
        // Add animation if valid
        if (anim.IsValid()) {
            animations.push_back(anim);
            loadedCount++;
            std::cout << "[AnimationManager]   - Loaded: " << anim.name 
                      << " (" << anim.GetFrameCount() << " frames)" << std::endl;
        } else {
            std::cout << "[AnimationManager] WARNING: Invalid animation (name empty or no frames)" << std::endl;
        }
        
        pos = objEnd + 1;
    }
    
    std::cout << "[AnimationManager] Loaded " << loadedCount << " animations from " << filepath << std::endl;
    return loadedCount > 0;
}

bool AnimationManager::SaveToFile(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cout << "[AnimationManager] ERROR: Could not create " << filepath << std::endl;
        return false;
    }
    
    // Write JSON manually
    file << "{\n";
    file << "  \"animations\": [\n";
    
    for (size_t i = 0; i < animations.size(); i++) {
        const Animation& anim = animations[i];
        
        file << "    {\n";
        file << "      \"name\": \"" << anim.name << "\",\n";
        
        // Frames array
        file << "      \"frames\": [";
        for (size_t j = 0; j < anim.frames.size(); j++) {
            file << anim.frames[j].spriteId;
            if (j < anim.frames.size() - 1) file << ", ";
        }
        file << "],\n";
        
        // Speeds array
        file << "      \"speeds\": [";
        for (size_t j = 0; j < anim.frames.size(); j++) {
            file << anim.frames[j].duration;
            if (j < anim.frames.size() - 1) file << ", ";
        }
        file << "],\n";
        
        // Loop
        file << "      \"loop\": " << (anim.loop ? "true" : "false") << "\n";
        
        file << "    }";
        if (i < animations.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    
    std::cout << "[AnimationManager] Saved " << animations.size() << " animations to " << filepath << std::endl;
    return true;
}

// === Animation Management ===

bool AnimationManager::AddAnimation(const Animation& anim) {
    if (!anim.IsValid()) {
        std::cout << "[AnimationManager] ERROR: Invalid animation (no name or no frames)" << std::endl;
        return false;
    }
    
    if (Exists(anim.name)) {
        std::cout << "[AnimationManager] WARNING: Animation '" << anim.name << "' already exists" << std::endl;
        return false;
    }
    
    animations.push_back(anim);
    std::cout << "[AnimationManager] Added animation '" << anim.name << "' (" << anim.GetFrameCount() << " frames)" << std::endl;
    return true;
}

bool AnimationManager::RemoveAnimation(const std::string& name) {
    int idx = FindAnimationIndex(name);
    if (idx == -1) return false;
    
    animations.erase(animations.begin() + idx);
    std::cout << "[AnimationManager] Removed animation '" << name << "'" << std::endl;
    return true;
}

Animation* AnimationManager::GetAnimation(const std::string& name) {
    int idx = FindAnimationIndex(name);
    return (idx != -1) ? &animations[idx] : nullptr;
}

const Animation* AnimationManager::GetAnimation(const std::string& name) const {
    int idx = FindAnimationIndex(name);
    return (idx != -1) ? &animations[idx] : nullptr;
}

bool AnimationManager::Exists(const std::string& name) const {
    return FindAnimationIndex(name) != -1;
}

std::vector<std::string> AnimationManager::GetAnimationNames() const {
    std::vector<std::string> names;
    for (const auto& anim : animations) {
        names.push_back(anim.name);
    }
    return names;
}

int AnimationManager::GetAnimationCount() const {
    return static_cast<int>(animations.size());
}

void AnimationManager::Clear() {
    animations.clear();
    std::cout << "[AnimationManager] Cleared all animations" << std::endl;
}

// === Playback Control ===

bool AnimationManager::Play(const std::string& name) {
    Animation* anim = GetAnimation(name);
    if (!anim) return false;
    anim->Play();
    return true;
}

bool AnimationManager::Stop(const std::string& name) {
    Animation* anim = GetAnimation(name);
    if (!anim) return false;
    anim->Stop();
    return true;
}

bool AnimationManager::Pause(const std::string& name) {
    Animation* anim = GetAnimation(name);
    if (!anim) return false;
    anim->Pause();
    return true;
}

bool AnimationManager::Reset(const std::string& name) {
    Animation* anim = GetAnimation(name);
    if (!anim) return false;
    anim->Reset();
    return true;
}

// === Query Functions ===

int AnimationManager::GetCurrentSpriteId(const std::string& name) const {
    const Animation* anim = GetAnimation(name);
    return anim ? anim->GetCurrentSpriteId() : -1;
}

bool AnimationManager::IsPlaying(const std::string& name) const {
    const Animation* anim = GetAnimation(name);
    return anim ? anim->playing : false;
}

bool AnimationManager::IsFinished(const std::string& name) const {
    const Animation* anim = GetAnimation(name);
    return anim ? anim->finished : false;
}

int AnimationManager::GetFrameCount(const std::string& name) const {
    const Animation* anim = GetAnimation(name);
    return anim ? anim->GetFrameCount() : 0;
}

int AnimationManager::GetTotalDuration(const std::string& name) const {
    const Animation* anim = GetAnimation(name);
    return anim ? anim->GetTotalDuration() : 0;
}

// === Update ===

void AnimationManager::Update() {
    static int frameCount = 0;
    frameCount++;
    
    if (frameCount % 60 == 0) {  // Log every second
        std::cout << "[AnimationManager::Update] Called (frame " << frameCount << ")" << std::endl;
    }
    
    for (auto& anim : animations) {
        if (anim.playing) {
            UpdateAnimation(anim);
        }
    }
}

void AnimationManager::UpdateAnimation(Animation& anim) {
    if (anim.frames.empty()) {
        anim.playing = false;
        return;
    }
    
    // Increment timer
    anim.frameTimer++;
    
    // Check if  we should advance to next frame
    int currentDuration = anim.frames[anim.currentFrame].duration;
    if (anim.frameTimer >= currentDuration) {
        anim.frameTimer = 0;
        anim.currentFrame++;
        
        // Check if animation finished
        if (anim.currentFrame >= static_cast<int>(anim.frames.size())) {
            if (anim.loop) {
                // Loop back to start
                anim.currentFrame = 0;
            } else {
                // Stop at last frame
                anim.currentFrame = static_cast<int>(anim.frames.size()) - 1;
                anim.playing = false;
                anim.finished = true;
            }
        }
    }
}

// === Private Methods ===

int AnimationManager::FindAnimationIndex(const std::string& name) const {
    for (size_t i = 0; i < animations.size(); i++) {
        if (animations[i].name == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
