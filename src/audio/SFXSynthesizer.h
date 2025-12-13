#ifndef SFXSYNTHESIZER_H
#define SFXSYNTHESIZER_H

#include <cstdint>

/**
 * @brief SFXSynthesizer - Generates waveforms for sound effects
 * 
 * Phase 5.12: Waveform synthesis (square, triangle, sine, noise)
 * ADSR envelope, frequency sweep, vibrato
 */
class SFXSynthesizer {
public:
    explicit SFXSynthesizer(int sampleRate);
    ~SFXSynthesizer();

    /**
     * @brief Render audio samples for all active SFX channels
     */
    void RenderSamples(float* buffer, int frames, int channels);

private:
    int sampleRate;
    
    // TODO Phase 5.12: Add SFX channel state, waveform generators
};

#endif // SFXSYNTHESIZER_H
