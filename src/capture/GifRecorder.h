#ifndef GIFRECORDER_H
#define GIFRECORDER_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief GIF Recording system
 * 
 * Records gameplay to animated GIF.
 * Hold Ctrl+F12 to start 5-second recording.
 */
class GifRecorder {
public:
    GifRecorder();
    ~GifRecorder();

    /**
     * @brief Start recording a GIF
     * @param width Frame width
     * @param height Frame height
     */
    void StartRecording(int width, int height);

    /**
     * @brief Add a frame to the recording
     * @param pixels RGBA pixel data
     */
    void AddFrame(const uint8_t* pixels);

    /**
     * @brief Stop recording and save GIF
     * @return true if saved successfully
     */
    bool StopRecording();

    /**
     * @brief Check if currently recording
     */
    bool IsRecording() const { return recording; }

    /**
     * @brief Get number of frames recorded
     */
    int GetFrameCount() const { return frameCount; }

    /**
     * @brief Get max frames (5 seconds at 60fps = 300 frames)
     */
    static constexpr int MAX_FRAMES = 300;

private:
    bool recording;
    int width;
    int height;
    int frameCount;
    std::vector<std::vector<uint8_t>> frames;
    
    std::string GenerateFilename();
    static constexpr const char* GIF_DIR = "screenshots";
};

#endif // GIFRECORDER_H
