#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

/**
 * @brief Settings - Global application settings
 * 
 * Manages persistent settings stored in:
 * AppData/Roaming/ULICS/settings.json (Windows)
 * 
 * Settings include:
 * - Volume (0-100)
 * - Fullscreen mode
 * - Debug mode
 * - VSync
 */
class Settings {
public:
    Settings();
    ~Settings();
    
    // === Load/Save ===
    
    /**
     * @brief Load settings from settings.json
     * @return true if loaded successfully, false if file not found (uses defaults)
     */
    bool Load();
    
    /**
     * @brief Save settings to settings.json
     * @return true if saved successfully
     */
    bool Save();
    
    /**
     * @brief Reset all settings to default values
     */
    void ResetToDefaults();
    
    // === Getters/Setters ===
    
    int GetVolume() const { return volume; }
    void SetVolume(int vol);
    int* GetVolumePtr() { return &volume; }  // For MenuSystem sliders
    
    bool IsFullscreen() const { return fullscreen; }
    void SetFullscreen(bool fs) { fullscreen = fs; }
    bool* GetFullscreenPtr() { return &fullscreen; }  // For MenuSystem toggles
    
    bool IsDebugMode() const { return debugMode; }
    void SetDebugMode(bool dm) { debugMode = dm; }
    bool* GetDebugModePtr() { return &debugMode; }  // For MenuSystem toggles
    
    bool IsVSync() const { return vsync; }
    void SetVSync(bool vs) { vsync = vs; }
    bool* GetVSyncPtr() { return &vsync; }  // For MenuSystem toggles
    
private:
    // Settings values
    int volume;         // 0-100
    bool fullscreen;
    bool debugMode;
    bool vsync;
    
    /**
     * @brief Get path to settings.json file
     * Format: AppData/Roaming/ULICS/settings.json
     */
    std::string GetSettingsPath() const;
};

#endif // SETTINGS_H
