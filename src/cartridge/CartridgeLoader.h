#ifndef CARTRIDGE_LOADER_H
#define CARTRIDGE_LOADER_H

#include <string>
#include "nlohmann/json.hpp"

class CartridgeLoader {
public:
    CartridgeLoader();

    /**
     * @brief Loads a cartridge from the specified directory path.
     * 
     * This function reads the 'config.json' and 'main.lua' files from the
     * given directory, parsing the configuration and loading the script content.
     * @param cartridgeDirectoryPath The path to the cartridge's root folder.
     * @return True if the cartridge was loaded successfully, false otherwise.
     */
    bool loadCartridge(const std::string& cartridgeDirectoryPath);

    const nlohmann::json& getConfig() const { return config; }
    const std::string& getLuaScript() const { return luaScript; }
    bool isLoaded() const { return loaded; }

private:
    nlohmann::json config;
    std::string luaScript;
    bool loaded = false;
};

#endif // CARTRIDGE_LOADER_H
