#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <string>
#include <cstdint>

/**
 * @brief Screenshot capture system
 * 
 * Captures the current frame buffer and saves it as PNG.
 * Press F12 to save a screenshot to screenshots/ folder.
 */
class Screenshot {
public:
    /**
     * @brief Save the current frame buffer to PNG
     * @param pixels Pointer to pixel data (RGBA format)
     * @param width Screen width
     * @param height Screen height
     * @return true if saved successfully, false otherwise
     */
    static bool SavePNG(const uint8_t* pixels, int width, int height);

    /**
     * @brief Generate timestamped filename
     * @return Filename like "screenshot_2025-12-13_19-30-45.png"
     */
    static std::string GenerateFilename();

    /**
     * @brief Ensure screenshots directory exists
     */
    static void EnsureDirectoryExists();

private:
    static constexpr const char* SCREENSHOT_DIR = "screenshots";
};

#endif // SCREENSHOT_H
