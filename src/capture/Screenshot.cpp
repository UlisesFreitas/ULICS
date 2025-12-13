#include "capture/Screenshot.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>

// stb_image_write for PNG saving
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

std::string Screenshot::GenerateFilename() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &time_t);
    
    std::ostringstream filename;
    filename << SCREENSHOT_DIR << "/screenshot_"
             << std::setfill('0')
             << std::setw(4) << (tm.tm_year + 1900) << "-"
             << std::setw(2) << (tm.tm_mon + 1) << "-"
             << std::setw(2) << tm.tm_mday << "_"
             << std::setw(2) << tm.tm_hour << "-"
             << std::setw(2) << tm.tm_min << "-"
             << std::setw(2) << tm.tm_sec
             << ".png";
    
    return filename.str();
}

void Screenshot::EnsureDirectoryExists() {
    try {
        std::filesystem::create_directories(SCREENSHOT_DIR);
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: Failed to create screenshots directory: " << e.what() << std::endl;
    }
}

bool Screenshot::SavePNG(const uint8_t* pixels, int width, int height) {
    EnsureDirectoryExists();
    
    std::string filename = GenerateFilename();
    
    // stbi_write_png expects RGBA data, which we have
    // Stride is width * 4 bytes (RGBA)
    int success = stbi_write_png(filename.c_str(), width, height, 4, pixels, width * 4);
    
    if (success) {
        std::cout << "Screenshot saved: " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Failed to save screenshot: " << filename << std::endl;
        return false;
    }
}
