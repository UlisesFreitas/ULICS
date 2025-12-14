#ifndef SFXSYNTHESIZER_H
#define SFXSYNTHESIZER_H

#include <cstdint>
#include <array>

/**
 * @brief SFXSynthesizer - Generates waveforms for sound effects
 * 
 * Phase 5.12: Waveform synthesis (square, triangle, sine, noise)
 * ADSR envelope, frequency sweep, vibrato
 */

// Simple SFX channel
struct SFXChannel {
    bool active = false;
    float phase = 0.0f;
    float frequency = 440.0f;  // Hz
    float amplitude = 0.3f;
    int duration = 0;          // Frames remaining
    int waveform = 0;          // 0=sine, 1=square, 2=triangle, 3=noise
};

class SFXSynthesizer {
public:
    explicit SFXSynthesizer(int sampleRate);
    ~SFXSynthesizer();

    /**
     * @brief Play a sound effect on a channel
     */
    void PlaySFX(int channel, int sfxId);

    /**
     * @brief Stop a channel
     */
    void StopChannel(int channel);

    /**
     * @brief Render audio samples for all active SFX channels
     */
    void RenderSamples(float* buffer, int frames, int channels);

private:
    int sampleRate;
    
    static constexpr int MAX_CHANNELS = 8;
    std::array<SFXChannel, MAX_CHANNELS> sfxChannels;
    
    // Helper to generate waveforms
    float GenerateWaveform(int waveform, float phase);
};

#endif // SFXSYNTHESIZER_H
