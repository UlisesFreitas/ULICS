#ifndef HOTRELOAD_H
#define HOTRELOAD_H

#include <string>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * @brief Resource types for hot reload
 */
enum class ResourceType {
    CODE,        // Lua scripts (main.lua)
    SPRITESHEET, // spritesheet.png
    FLAGS,       // spritesheet.flags
    MAP,         // map.json
    AUDIO        // sfx/music files (future)
};

/**
 * @brief HotReload - Monitors file changes and triggers reloads
 * 
 * Watches cartridge files (code, sprites, flags, maps) for modifications.
 * When a change is detected, triggers appropriate reload actions.
 */
class HotReload {
public:
    HotReload();
    ~HotReload();

    /**
     * @brief Start monitoring a file for changes
     * @param filepath Path to file to watch
     * @param type Type of resource (default: CODE for backwards compatibility)
     */
    void WatchFile(const std::string& filepath, ResourceType type = ResourceType::CODE);

    /**
     * @brief Stop monitoring all files
     */
    void StopWatching();

    /**
     * @brief Check if any watched files have changed
     * @return true if at least one file was modified
     */
    bool CheckForChanges();

    /**
     * @brief Get list of changed files with their types
     */
    struct ChangedResource {
        std::string filepath;
        ResourceType type;
    };
    const std::vector<ChangedResource>& GetChangedResources() const { return changedResources; }

    /**
     * @brief Enable/disable hot reload
     */
    void SetEnabled(bool enabled) { this->enabled = enabled; }
    bool IsEnabled() const { return enabled; }

private:
    struct WatchedFile {
        std::string path;
        ResourceType type;
        fs::file_time_type lastModified;
    };

    bool enabled;
    std::vector<WatchedFile> watchedFiles;
    std::vector<ChangedResource> changedResources;

    /**
     * @brief Get last modification time of a file
     */
    fs::file_time_type GetFileModificationTime(const std::string& path);
};

#endif // HOTRELOAD_H
