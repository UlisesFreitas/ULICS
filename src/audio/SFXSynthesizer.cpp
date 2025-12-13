#include "audio/SFXSynthesizer.h"
#include <cstring>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

SFXSynthesizer::SFXSynthesizer(int sampleRate)
    : sampleRate(sampleRate) {
}

SFXSynthesizer::~SFXSynthesizer() {
}

void SFXSynthesizer::RenderSamples(float* buffer, int frames, int channels) {
    // TODO Phase 6: Load and manage SFX from JSON
    // TODO Phase 6: Track active SFX channels
    // TODO Phase 6: Mix multiple SFX
    
    // For now, generate a simple test tone when called
    // This is just to verify audio pipeline works
    
    // Simple 440Hz sine wave test (very quiet)
    static float phase = 0.0f;
    float frequency = 440.0f;
    float amplitude = 0.05f;  // Very quiet test tone
    
    for (int i = 0; i < frames; i++) {
        float sample = amplitude * std::sin(2.0f * M_PI * phase);
        phase += frequency / static_cast<float>(sampleRate);
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
        
        // Write to all channels (stereo)
        for (int ch = 0; ch < channels; ch++) {
            buffer[i * channels + ch] = sample;
        }
    }
    
    (void)buffer;
    (void)frames;
    (void)channels;
}

// TODO Phase 6: Implement these synthesis functions

/**
 * Generate square wave
 */
float GenerateSquareWave(float phase) {
    return (phase < 0.5f) ? 1.0f : -1.0f;
}

/**
 * Generate sine wave
 */
float GenerateSineWave(float phase) {
    return std::sin(2.0f * M_PI * phase);
}

/**
 * Generate triangle wave
 */
float GenerateTriangleWave(float phase) {
    if (phase < 0.25f) {
        return 4.0f * phase;
    } else if (phase < 0.75f) {
        return 2.0f - 4.0f * phase;
    } else {
        return 4.0f * phase - 4.0f;
    }
}

/**
 * Generate sawtooth wave
 */
float GenerateSawtoothWave(float phase) {
    return 2.0f * phase - 1.0f;
}

/**
 * Generate white noise
 */
float GenerateNoise() {
    return 2.0f * (static_cast<float>(rand()) / RAND_MAX) - 1.0f;
}

/**
 * Apply ADSR envelope
 */
float ApplyADSR(float time, float duration, float attack, float decay, float sustain, float release) {
    if (time < attack) {
        // Attack phase
        return time / attack;
    } else if (time < attack + decay) {
        // Decay phase
        float t = (time - attack) / decay;
        return 1.0f - t * (1.0f - sustain);
    } else if (time < duration - release) {
        // Sustain phase
        return sustain;
    } else {
        // Release phase
        float t = (time - (duration - release)) / release;
        return sustain * (1.0f - t);
    }
}
