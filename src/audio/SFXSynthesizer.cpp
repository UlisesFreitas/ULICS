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

void SFXSynthesizer::PlaySFX(int channel, int sfxId) {
    if (channel < 0 || channel >= MAX_CHANNELS) {
        return;
    }
    
    // Simple mapping: sfxId determines waveform and frequency
    SFXChannel& ch = sfxChannels[channel];
    ch.active = true;
    ch.phase = 0.0f;
    ch.duration = sampleRate / 2;  // 0.5 seconds
    ch.amplitude = 0.15f;  // Reduced from 0.3f to prevent clipping when mixing multiple channels
    
    // Map sfxId to different sounds
    switch (sfxId % 4) {
        case 0:  // Sine
            ch.waveform = 0;
            ch.frequency = 440.0f;
            break;
        case 1:  // Square
            ch.waveform = 1;
            ch.frequency = 523.0f;  // C5
            break;
        case 2:  // Triangle
            ch.waveform = 2;
            ch.frequency = 659.0f;  // E5
            break;
        case 3:  // Noise
            ch.waveform = 3;
            ch.frequency = 0.0f;
            break;
    }
}

void SFXSynthesizer::StopChannel(int channel) {
    if (channel >= 0 && channel < MAX_CHANNELS) {
        sfxChannels[channel].active = false;
    }
}

void SFXSynthesizer::RenderSamples(float* buffer, int frames, int channels) {
    // Process each SFX channel
    for (int ch = 0; ch < MAX_CHANNELS; ch++) {
        SFXChannel& sfx = sfxChannels[ch];
        
        if (!sfx.active) {
            continue;
        }
        
        // Generate samples for this channel
        for (int i = 0; i < frames; i++) {
            // Generate waveform
            float sample = GenerateWaveform(sfx.waveform, sfx.phase);
            sample *= sfx.amplitude;
            
            // Apply simple envelope (fade out)
            float envelope = static_cast<float>(sfx.duration) / (sampleRate / 2);
            sample *= envelope;
            
            // Mix into buffer (stereo)
            for (int c = 0; c < channels; c++) {
                buffer[i * channels + c] += sample;
            }
            
            // Update phase
            if (sfx.waveform != 3) {  // Not noise
                sfx.phase += sfx.frequency / static_cast<float>(sampleRate);
                if (sfx.phase >= 1.0f) {
                    sfx.phase -= 1.0f;
                }
            }
        }
        
        // Update duration
        sfx.duration -= frames;
        if (sfx.duration <= 0) {
            sfx.active = false;
        }
    }
    
    (void)buffer;
    (void)frames;
    (void)channels;
}

float SFXSynthesizer::GenerateWaveform(int waveform, float phase) {
    switch (waveform) {
        case 0:  // Sine
            return std::sin(2.0f * M_PI * phase);
        
        case 1:  // Square
            return (phase < 0.5f) ? 1.0f : -1.0f;
        
        case 2:  // Triangle
            if (phase < 0.25f) {
                return 4.0f * phase;
            } else if (phase < 0.75f) {
                return 2.0f - 4.0f * phase;
            } else {
                return 4.0f * phase - 4.0f;
            }
        
        case 3:  // Noise
            return 2.0f * (static_cast<float>(rand()) / RAND_MAX) - 1.0f;
        
        default:
            return 0.0f;
    }
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
