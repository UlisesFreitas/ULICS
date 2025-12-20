#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include <string>

// Bootstrap system - ensures app has required boot files
// Creates .ulics_boot in AppData/Roaming with default main.lua and config.json
namespace Bootstrap {
    // Initialize bootstrap system (call from Engine::Initialize)
    void Initialize();
    
    // Get path to boot directory
    std::string GetBootPath();
    
    // Check if boot files exist, create if missing
    void EnsureBootFiles();
}

#endif // BOOTSTRAP_H
