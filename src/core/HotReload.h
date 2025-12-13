#ifndef HOTRELOAD_H
#define HOTRELOAD_H

#include <string>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * @brief HotReload - Monitors file changes and triggers reloads
 * 
 * Watches main.lua and other cartridge files for modifications.
 * When a change is detected, triggers Engine::ReloadCurrentCartridge()
 */
class HotReload {
public:
    HotReload();
    ~HotReload();

    /**
     * @brief Start monitoring a file for changes
     * @param filepath Path to file to watch
     */
    void WatchFile(const std::string& filepath);

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
     * @brief Get list of changed files
     */
    const std::vector<std::string>& GetChangedFiles() const { return changedFiles; }

    /**
     * @brief Enable/disable hot reload
     */
    void SetEnabled(bool enabled) { this->enabled = enabled; }
    bool IsEnabled() const { return enabled; }

private:
    struct WatchedFile {
        std::string path;
        fs::file_time_type lastModified;
    };

    bool enabled;
    std::vector<WatchedFile> watchedFiles;
    std::vector<std::string> changedFiles;

    /**
     * @brief Get last modification time of a file
     */
    fs::file_time_type GetFileModificationTime(const std::string& path);
};

#endif // HOTRELOAD_H
