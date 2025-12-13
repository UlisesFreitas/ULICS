#include "capture/GifRecorder.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <cstring>

// msf_gif for simple GIF encoding
#define MSF_GIF_IMPL
#include "msf_gif.h"

GifRecorder::GifRecorder() 
    : recording(false), width(0), height(0), frameCount(0) {
}

GifRecorder::~GifRecorder() {
    if (recording) {
        StopRecording();
    }
}

std::string GifRecorder::GenerateFilename() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &time_t);
    
    std::ostringstream filename;
    filename << GIF_DIR << "/recording_"
             << std::setfill('0')
             << std::setw(4) << (tm.tm_year + 1900) << "-"
             << std::setw(2) << (tm.tm_mon + 1) << "-"
             << std::setw(2) << tm.tm_mday << "_"
             << std::setw(2) << tm.tm_hour << "-"
             << std::setw(2) << tm.tm_min << "-"
             << std::setw(2) << tm.tm_sec
             << ".gif";
    
    return filename.str();
}

void GifRecorder::StartRecording(int w, int h) {
    if (recording) {
        std::cout << "Already recording!" << std::endl;
        return;
    }
    
    width = w;
    height = h;
    frameCount = 0;
    frames.clear();
    frames.reserve(MAX_FRAMES);
    recording = true;
    
    std::cout << "GIF Recording started (5 seconds, 60fps)..." << std::endl;
}

void GifRecorder::AddFrame(const uint8_t* pixels) {
    if (!recording) return;
    
    // Stop if we hit max frames
    if (frameCount >= MAX_FRAMES) {
        StopRecording();
        return;
    }
    
    // Convert from ARGB32 (from pixelBuffer) to RGBA (for msf_gif)
    size_t frameSize = width * height * 4;
    std::vector<uint8_t> frame(frameSize);
    
    const uint32_t* argb = reinterpret_cast<const uint32_t*>(pixels);
    
    for (int i = 0; i < width * height; ++i) {
        uint32_t pixel = argb[i];
        
        // Extract ARGB components
        uint8_t a = (pixel >> 24) & 0xFF;
        uint8_t r = (pixel >> 16) & 0xFF;
        uint8_t g = (pixel >> 8) & 0xFF;
        uint8_t b = pixel & 0xFF;
        
        // Store as RGBA
        frame[i * 4 + 0] = r;
        frame[i * 4 + 1] = g;
        frame[i * 4 + 2] = b;
        frame[i * 4 + 3] = a;
    }
    
    frames.push_back(std::move(frame));
    frameCount++;
}

bool GifRecorder::StopRecording() {
    if (!recording) {
        return false;
    }
    
    recording = false;
    
    if (frames.empty()) {
        std::cerr << "No frames recorded!" << std::endl;
        return false;
    }
    
    // Create directory
    try {
        std::filesystem::create_directories(GIF_DIR);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to create directory: " << e.what() << std::endl;
    }
    
    std::string filename = GenerateFilename();
    
    // msf_gif context
    MsfGifState gifState = {};
    msf_gif_begin(&gifState, width, height);
    
    // Add all frames (centiseconds per frame: 100 / 60fps ≈ 2)
    int centiSecondsPerFrame = 2;  // ~60 FPS
    
    for (const auto& frame : frames) {
        // msf_gif expects RGBA, which we have! (const_cast needed for API)
        msf_gif_frame(&gifState, const_cast<uint8_t*>(frame.data()), centiSecondsPerFrame, 16, width * 4);
    }
    
    // Write to file
    MsfGifResult result = msf_gif_end(&gifState);
    if (result.data) {
        FILE* fp = nullptr;
        fopen_s(&fp, filename.c_str(), "wb");
        if (fp) {
            fwrite(result.data, result.dataSize, 1, fp);
            fclose(fp);
            std::cout << "GIF saved: " << filename << " (" << frameCount << " frames)" << std::endl;
            msf_gif_free(result);
            
            frames.clear();
            frameCount = 0;
            return true;
        } else {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            msf_gif_free(result);
        }
    } else {
        std::cerr << "Failed to encode GIF!" << std::endl;
    }
    
    frames.clear();
    frameCount = 0;
    return false;
}
