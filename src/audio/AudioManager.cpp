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

void AudioManager::AudioCallback(void* userdata, Uint8* stream, int len) {
    AudioManager* manager = static_cast<AudioManager*>(userdata);
    
    // Convert byte stream to float buffer
    float* buffer = reinterpret_cast<float*>(stream);
    int frames = len / (sizeof(float) * manager->channels);
    
    // Mix audio
    manager->MixAudio(buffer, frames);
}

void AudioManager::MixAudio(float* buffer, int frames) {
    // Clear buffer
    std::memset(buffer, 0, frames * channels * sizeof(float));
    
    // TODO Phase 6: Fix audio threading issue causing input lag
    // Mix SFX channels (Phase 5.12.1 - DISABLED due to 6-8s input delay)
    // if (sfxSynthesizer) {
    //     sfxSynthesizer->RenderSamples(buffer, frames, channels);
    // }
    
    // TODO Phase 5.14: Mix music
    // if (musicPlayer) {
    //     musicPlayer->RenderSamples(buffer, frames, channels);
    // }
    
    // Apply master volume
    for (int i = 0; i < frames * channels; i++) {
        buffer[i] *= masterVolume;
    }
}
