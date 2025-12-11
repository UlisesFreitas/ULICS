#ifndef CARTRIDGE_LOADER_H
#define CARTRIDGE_LOADER_H

#include "cartridge/CartridgeConfig.h"
#include <string>
#include <vector>
#include <optional>

/**
 * @brief Handles loading and parsing of ULICS cartridges.
 * 
 * The CartridgeLoader is responsible for:
 * - Discovering cartridges in the cartridges/ directory
 * - Parsing config.json files (optional)
 * - Validating cartridge structure
 * - Providing cartridge metadata
 * 
 * A valid cartridge can be:
 * 1. A directory with main.lua (and optional config.json)
 * 2. A single .lua file
 */
class CartridgeLoader {
public:
    /**
     * @brief Represents a discovered cartridge with its metadata.
     */
    struct CartridgeInfo {
        std::string name;           // Display name (from config or derived from path)
        std::string path;           // Full path to cartridge directory or .lua file
        std::string main_lua_path;  // Full path to main.lua
        bool has_config;            // True if config.json exists
        std::string config_path;    // Full path to config.json (if exists)
    };

    CartridgeLoader() = default;
    ~CartridgeLoader() = default;

    /**
     * @brief Lists all available cartridges in a directory.
     * 
     * Scans the specified directory for valid cartridges:
     * - Directories containing main.lua
     * - Standalone .lua files
     * 
     * @param directory_path Path to scan (e.g., "./cartridges")
     * @return Vector of CartridgeInfo for all discovered cartridges
     */
    std::vector<CartridgeInfo> ListAvailableCartridges(const std::string& directory_path);

    /**
     * @brief Parses a config.json file and returns the configuration.
     * 
     * If the file doesn't exist or parsing fails, returns a default configuration.
     * 
     * @param config_path Full path to config.json
     * @return CartridgeConfig with parsed values (or defaults if parsing failed)
     */
    CartridgeConfig ParseConfig(const std::string& config_path);

    /**
     * @brief Loads a cartridge's configuration from a cartridge path.
     * 
     * This method:
     * 1. Determines if path is a .lua file or directory
     * 2. Looks for config.json (if directory)
     * 3. Parses config.json or returns defaults
     * 4. Validates the configuration
     * 
     * @param cartridge_path Path to cartridge (directory or .lua file)
     * @return CartridgeConfig for the cartridge
     */
    CartridgeConfig LoadCartridge(const std::string& cartridge_path);

    /**
     * @brief Gets the path to main.lua for a given cartridge path.
     * 
     * @param cartridge_path Path to cartridge (directory or .lua file)
     * @return Full path to main.lua
     */
    std::string GetMainLuaPath(const std::string& cartridge_path);

    /**
     * @brief Checks if a path represents a valid cartridge.
     * 
     * A valid cartridge is either:
     * - A .lua file
     * - A directory containing main.lua
     * 
     * @param path Path to check
     * @return true if valid cartridge, false otherwise
     */
    bool IsValidCartridge(const std::string& path);

    /**
     * @brief Gets the last error message from parsing/loading operations.
     * 
     * @return Last error message (empty if no error)
     */
    const std::string& GetLastError() const { return last_error; }

private:
    std::string last_error;

    /**
     * @brief Helper to check if a file exists.
     */
    bool FileExists(const std::string& path);

    /**
     * @brief Helper to check if a directory exists.
     */
    bool DirectoryExists(const std::string& path);

    /**
     * @brief Helper to read entire file content as string.
     */
    std::string ReadFileContent(const std::string& path);

    /**
     * @brief Helper to extract cartridge name from path.
     * 
     * Examples:
     * - "cartridges/my_game" -> "my_game"
     * - "cartridges/pong.lua" -> "pong"
     * 
     * @param path Cartridge path
     * @return Extracted name
     */
    std::string ExtractCartridgeName(const std::string& path);
};

#endif // CARTRIDGE_LOADER_H
