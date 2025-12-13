#include "audio/MusicPlayer.h"

MusicPlayer::MusicPlayer(SFXSynthesizer* sfxSynth)
    : sfxSynth(sfxSynth) {
}

MusicPlayer::~MusicPlayer() {
}

void MusicPlayer::RenderSamples(float* buffer, int frames, int channels) {
    // TODO Phase 5.14: Sequence patterns and trigger SFX
    (void)buffer;
    (void)frames;
    (void)channels;
}
