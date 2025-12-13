#include "core/HotReload.h"
#include <iostream>

HotReload::HotReload() : enabled(true) {
    // Constructor
}

HotReload::~HotReload() {
    StopWatching();
}

void HotReload::WatchFile(const std::string& filepath) {
    try {
        if (!fs::exists(filepath)) {
            std::cerr << "HotReload: File doesn't exist, cannot watch: " << filepath << std::endl;
            return;
        }

        // Check if already watching
        for (const auto& watched : watchedFiles) {
            if (watched.path == filepath) {
                return;  // Already watching
            }
        }

        WatchedFile wf;
        wf.path = filepath;
        wf.lastModified = GetFileModificationTime(filepath);
        
        watchedFiles.push_back(wf);
        std::cout << "HotReload: Now watching " << filepath << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "HotReload: Error watching file: " << e.what() << std::endl;
    }
}

void HotReload::StopWatching() {
    watchedFiles.clear();
    changedFiles.clear();
}

bool HotReload::CheckForChanges() {
    if (!enabled) {
        return false;
    }

    changedFiles.clear();

    try {
        for (auto& watched : watchedFiles) {
            if (!fs::exists(watched.path)) {
                continue;  // File was deleted
            }

            auto currentModTime = GetFileModificationTime(watched.path);
            
            if (currentModTime != watched.lastModified) {
                // File was modified!
                changedFiles.push_back(watched.path);
                watched.lastModified = currentModTime;
                
                std::cout << "HotReload: Detected change in " << watched.path << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "HotReload: Error checking for changes: " << e.what() << std::endl;
    }

    return !changedFiles.empty();
}

fs::file_time_type HotReload::GetFileModificationTime(const std::string& path) {
    return fs::last_write_time(path);
}
