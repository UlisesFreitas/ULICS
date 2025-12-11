#ifndef CARTRIDGE_CONFIG_H
#define CARTRIDGE_CONFIG_H

#include <string>

/**
 * @brief Configuration data for a ULICS cartridge.
 * 
 * This struct holds all metadata and settings parsed from a cartridge's config.json file.
 * All fields have sensible defaults that align with ULICS's high-performance fantasy console philosophy.
 * 
 * ULICS differentiates from PICO-8/TIC-80 by offering massive resources:
 * - 512MB-1GB RAM (vs 2MB in PICO-8)
 * - 1,000,000 lines of Lua (vs 8,192 tokens in PICO-8)
 * 
 * These generous limits enable ambitious projects (RPGs, roguelikes, simulations, AI) while
 * maintaining the fantasy console aesthetic and "everything is code" philosophy.
 */
struct CartridgeConfig {
    // ===== METADATA =====
    /// Display name of the cartridge
    std::string name = "Untitled";
    
    /// Author/creator name
    std::string author = "Unknown";
    
    /// Semantic version (e.g., "1.0.0")
    std::string version = "1.0.0";
    
    /// Brief description of the cartridge
    std::string description = "";
    
    // ===== RESOURCE LIMITS (CRITICAL - ULICS DIFFERENTIATOR) =====
    /// Maximum Lua memory allocation in megabytes (512-1024)
    /// This is a CORE feature that sets ULICS apart from PICO-8/TIC-80
    int memory_limit_mb = 512;
    
    /// Maximum lines of Lua code allowed (1 to 1,000,000)
    /// Default is the maximum to not limit creativity
    /// This is a CORE feature that sets ULICS apart from PICO-8/TIC-80
    int lua_code_limit_lines = 1000000;
    
    // ===== GRAPHICS CONFIGURATION =====
    /// Number of colors in the palette (must be 16, 32, 64, or 128)
    int palette_size = 16;
    
    /// Framebuffer width in pixels (128-512)
    int framebuffer_width = 256;
    
    /// Framebuffer height in pixels (128-512)
    int framebuffer_height = 256;
    
    // ===== PERFORMANCE =====
    /// Target frames per second
    int target_fps = 60;
    
    // ===== VALIDATION HELPERS =====
    
    /**
     * @brief Validates and clamps all configuration values to their allowed ranges.
     * 
     * This function ensures that all settings are within acceptable bounds.
     * Out-of-range values are clamped and a warning is logged.
     * 
     * @return true if all values were valid, false if any were clamped
     */
    bool Validate() {
        bool all_valid = true;
        
        // Validate memory limit (512-1024 MB)
        if (memory_limit_mb < 512 || memory_limit_mb > 1024) {
            // Log warning (will be implemented when we have a logger)
            memory_limit_mb = (memory_limit_mb < 512) ? 512 : 1024;
            all_valid = false;
        }
        
        // Validate Lua code lines (1 to 1,000,000)
        if (lua_code_limit_lines < 1 || lua_code_limit_lines > 1000000) {
            lua_code_limit_lines = (lua_code_limit_lines < 1) ? 1 : 1000000;
            all_valid = false;
        }
        
        // Validate palette size (must be 16, 32, 64, or 128)
        if (palette_size != 16 && palette_size != 32 && 
            palette_size != 64 && palette_size != 128) {
            palette_size = 16; // Default to 16
            all_valid = false;
        }
        
        // Validate framebuffer dimensions (128-512)
        if (framebuffer_width < 128 || framebuffer_width > 512) {
            framebuffer_width = (framebuffer_width < 128) ? 128 : 512;
            all_valid = false;
        }
        
        if (framebuffer_height < 128 || framebuffer_height > 512) {
            framebuffer_height = (framebuffer_height < 128) ? 128 : 512;
            all_valid = false;
        }
        
        // Validate target FPS (reasonable range: 30-120)
        if (target_fps < 30 || target_fps > 120) {
            target_fps = 60; // Default
            all_valid = false;
        }
        
        return all_valid;
    }
    
    /**
     * @brief Returns a default configuration with ULICS standard settings.
     * 
     * The default configuration represents a typical ULICS cartridge:
     * - 512MB RAM (generous but not maximum)
     * - 1M lines of code (maximum freedom)
     * - 16 color palette (classic fantasy console)
     * - 256x256 resolution (standard retro resolution)
     * 
     * @return CartridgeConfig with default values
     */
    static CartridgeConfig Default() {
        return CartridgeConfig(); // All fields have default initializers
    }
    
    /**
     * @brief Returns a configuration for massive/ambitious projects.
     * 
     * This preset is designed for large-scale projects like:
     * - Complex RPGs with vast worlds
     * - Roguelikes with extensive procedural generation
     * - Simulations with thousands of entities
     * - AI-driven games
     * 
     * @return CartridgeConfig optimized for high-performance projects
     */
    static CartridgeConfig HighPerformance() {
        CartridgeConfig config;
        config.memory_limit_mb = 1024;          // Maximum RAM
        config.lua_code_limit_lines = 1000000;  // Maximum code
        config.palette_size = 128;              // Maximum colors
        config.framebuffer_width = 384;         // Larger resolution
        config.framebuffer_height = 384;
        config.target_fps = 60;
        return config;
    }
    
    /**
     * @brief Returns a configuration for simple/small projects.
     * 
     * This preset is good for:
     * - Puzzle games
     * - Simple arcade games
     * - Prototypes
     * - Game jam entries
     * 
     * @return CartridgeConfig optimized for smaller projects
     */
    static CartridgeConfig Minimal() {
        CartridgeConfig config;
        config.memory_limit_mb = 512;           // Minimum RAM (still massive!)
        config.lua_code_limit_lines = 100000;   // 100K lines (still huge)
        config.palette_size = 16;               // Classic 16 colors
        config.framebuffer_width = 256;
        config.framebuffer_height = 256;
        config.target_fps = 60;
        return config;
    }
};

#endif // CARTRIDGE_CONFIG_H
