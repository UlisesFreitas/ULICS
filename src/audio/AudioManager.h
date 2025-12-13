#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SDL.h>
#include <string>
#include <vector>
#include <memory>
#include "SFXSynthesizer.h"
#include "MusicPlayer.h"

/**
 * @brief AudioManager - Singleton for SDL2 raw audio management
 * 
 * Implements a fantasy console audio system using SDL2's raw audio API.
 * Inspired by PICO-8 and TIC-80 synthesized sound systems.
 * 
 * Features:
 * - Waveform synthesis (square, triangle, sine, noise)
 * - ADSR envelopes
 * - 8 SFX channels
 * - Pattern-based music player
 */
class AudioManager {
public:
    /**
     * @brief Get singleton instance
     */
    static AudioManager& getInstance();

    /**
     * @brief Initialize SDL2 audio subsystem
     * @return true if successful
     */
    bool Initialize();

    /**
     * @brief Cleanup and shutdown audio
     */
    void Shutdown();

    /**
     * @brief Check if audio system is initialized
     */
    bool IsInitialized() const { return initialized; }

    /**
     * @brief Set master volume (0.0 to 1.0)
     */
    void SetMasterVolume(float volume);

    /**
     * @brief Get master volume
     */
    float GetMasterVolume() const;

    /**
     * @brief Get audio sample rate
     */
    int GetSampleRate() const { return sampleRate; }

    /**
     * @brief Audio mixing callback (called by SDL)
     */
    static void AudioCallback(void* userdata, Uint8* stream, int len);

    // Delete copy constructor and assignment operator (singleton pattern)
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

private:
    AudioManager();
    ~AudioManager();

    /**
     * @brief Mix audio samples for one frame
     */
    void MixAudio(float* buffer, int frames);

    bool initialized;
    float masterVolume;
    
    // SDL Audio configuration
    SDL_AudioDeviceID audioDevice;
    SDL_AudioSpec audioSpec;
    int sampleRate;
    int channels;      // Stereo: 2
    int bufferSize;    // Samples per callback

    // Audio subsystems (to be implemented in Phase 5.12-5.14)
    std::unique_ptr<SFXSynthesizer> sfxSynthesizer;
    std::unique_ptr<MusicPlayer> musicPlayer;
};

#endif // AUDIOMANAGER_H
