#include "audio/AudioManager.h"
#include <iostream>
#include <cstring>

AudioManager::AudioManager() 
    : initialized(false)
    , masterVolume(0.5f)
    , audioDevice(0)
    , sampleRate(44100)
    , channels(2)
    , bufferSize(1024)
    , sfxSynthesizer(nullptr)
    , musicPlayer(nullptr) {
}

AudioManager::~AudioManager() {
    Shutdown();
}

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

bool AudioManager::Initialize() {
    if (initialized) {
        std::cout << "AudioManager: Already initialized" << std::endl;
        return true;
    }

    // Initialize SDL audio subsystem
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cerr << "AudioManager: Failed to initialize SDL audio: " 
                  << SDL_GetError() << std::endl;
        return false;
    }

    // Configure audio specification
    SDL_AudioSpec desired;
    SDL_zero(desired);
    desired.freq = sampleRate;
    desired.format = AUDIO_F32SYS;  // 32-bit float samples
    desired.channels = channels;
    desired.samples = bufferSize;
    desired.callback = AudioManager::AudioCallback;
    desired.userdata = this;

    // Open audio device
    audioDevice = SDL_OpenAudioDevice(nullptr, 0, &desired, &audioSpec, 0);
    if (audioDevice == 0) {
        std::cerr << "AudioManager: Failed to open audio device: " 
                  << SDL_GetError() << std::endl;
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    // Update actual configuration
    sampleRate = audioSpec.freq;
    channels = audioSpec.channels;
    bufferSize = audioSpec.samples;

    std::cout << "AudioManager: Initialized successfully" << std::endl;
    std::cout << "  Sample rate: " << sampleRate << " Hz" << std::endl;
    std::cout << "  Channels: " << channels << " (stereo)" << std::endl;
    std::cout << "  Buffer size: " << bufferSize << " samples" << std::endl;

    // Initialize lock-free ring buffer (FIX for 1.1.3)
    audioRingBuffer = std::make_unique<RingBuffer<float>>(RING_BUFFER_SIZE);
    std::cout << "  Ring buffer: " << RING_BUFFER_SIZE << " samples ("
              << (RING_BUFFER_SIZE * 1000 / (sampleRate * channels)) << "ms)" << std::endl;

    // Initialize SFXSynthesizer (Phase 5.12.1)
    sfxSynthesizer = std::make_unique<SFXSynthesizer>(sampleRate);
    
    // TODO Phase 5.14: Initialize MusicPlayer  
    // musicPlayer = std::make_unique<MusicPlayer>(sfxSynthesizer.get());

    // Start audio playback
    SDL_PauseAudioDevice(audioDevice, 0);

    initialized = true;
    return true;
}

void AudioManager::Shutdown() {
    if (!initialized) {
        return;
    }

    std::cout << "AudioManager: Shutting down..." << std::endl;

    // Stop audio playback
    if (audioDevice != 0) {
        SDL_PauseAudioDevice(audioDevice, 1);
        SDL_CloseAudioDevice(audioDevice);
        audioDevice = 0;
    }

    // Cleanup subsystems
    musicPlayer.reset();
    sfxSynthesizer.reset();

    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    initialized = false;
    std::cout << "AudioManager: Shutdown complete" << std::endl;
}

void AudioManager::SetMasterVolume(float volume) {
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    
    masterVolume = volume;
    std::cout << "AudioManager: Master volume set to " << (volume * 100.0f) << "%" << std::endl;
}

float AudioManager::GetMasterVolume() const {
    return masterVolume;
}

void AudioManager::PlaySFX(int channel, int sfxId) {
    if (sfxSynthesizer) {
        sfxSynthesizer->PlaySFX(channel, sfxId);
    }
}

void AudioManager::GenerateAudio(int frames) {
    if (!initialized || !audioRingBuffer) {
        return;
    }

    // Allocate temporary buffer for synthesis
    std::vector<float> tempBuffer(frames * channels);
    
    // Clear buffer
    std::memset(tempBuffer.data(), 0, tempBuffer.size() * sizeof(float));
    
    // Synthesize audio (now safe to do in main thread!)
    if (sfxSynthesizer) {
        sfxSynthesizer->RenderSamples(tempBuffer.data(), frames, channels);
    }
    
    // TODO Phase 5.14: Mix music
    // if (musicPlayer) {
    //     musicPlayer->RenderSamples(tempBuffer.data(), frames, channels);
    // }
    
    // Apply master volume and soft limiter
    for (size_t i = 0; i < tempBuffer.size(); i++) {
        float sample = tempBuffer[i] * masterVolume;
        
        // Soft limiter using tanh (prevents harsh clipping)
        if (sample > 1.0f || sample < -1.0f) {
            sample = std::tanh(sample);  // Smooth saturation
        }
        
        tempBuffer[i] = sample;
    }
    
    // Write to ring buffer (non-blocking, lock-free)
    audioRingBuffer->Write(tempBuffer.data(), tempBuffer.size());
}

void AudioManager::AudioCallback(void* userdata, Uint8* stream, int len) {
    AudioManager* manager = static_cast<AudioManager*>(userdata);
    
    // Convert byte stream to float buffer
    float* buffer = reinterpret_cast<float*>(stream);
    int samples = len / sizeof(float);
    
    // Read from ring buffer (lock-free, no blocking!)
    // If buffer underruns, Read() fills with silence automatically
    if (manager->audioRingBuffer) {
        manager->audioRingBuffer->Read(buffer, samples);
    } else {
        // Fallback: silence
        std::memset(buffer, 0, len);
    }
}
