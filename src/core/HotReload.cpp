#include "core/HotReload.h"
#include <iostream>

HotReload::HotReload() : enabled(true) {
    // Constructor
}

HotReload::~HotReload() {
    StopWatching();
}

void HotReload::WatchFile(const std::string& filepath, ResourceType type) {
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
        wf.type = type;
        wf.lastModified = GetFileModificationTime(filepath);
        
        watchedFiles.push_back(wf);
        
        // Log resource type
        const char* typeName = "UNKNOWN";
        switch (type) {
            case ResourceType::CODE: typeName = "CODE"; break;
            case ResourceType::SPRITESHEET: typeName = "SPRITESHEET"; break;
            case ResourceType::FLAGS: typeName = "FLAGS"; break;
            case ResourceType::MAP: typeName = "MAP"; break;
            case ResourceType::AUDIO: typeName = "AUDIO"; break;
        }
        
        std::cout << "HotReload: Now watching " << filepath << " [" << typeName << "]" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "HotReload: Error watching file: " << e.what() << std::endl;
    }
}

void HotReload::StopWatching() {
    watchedFiles.clear();
    changedResources.clear();
}

bool HotReload::CheckForChanges() {
    if (!enabled) {
        return false;
    }

    changedResources.clear();

    try {
        for (auto& watched : watchedFiles) {
            if (!fs::exists(watched.path)) {
                continue;  // File was deleted
            }

            auto currentModTime = GetFileModificationTime(watched.path);
            
            if (currentModTime != watched.lastModified) {
                // File was modified!
                ChangedResource change;
                change.filepath = watched.path;
                change.type = watched.type;
                changedResources.push_back(change);
                
                watched.lastModified = currentModTime;
                
                std::cout << "HotReload: Detected change in " << watched.path << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "HotReload: Error checking for changes: " << e.what() << std::endl;
    }

    return !changedResources.empty();
}

fs::file_time_type HotReload::GetFileModificationTime(const std::string& path) {
    return fs::last_write_time(path);
}
