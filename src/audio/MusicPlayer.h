#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

class SFXSynthesizer;

/**
 * @brief MusicPlayer - Plays pattern-based music tracks
 * 
 * Phase 5.14: Tracker-style music player
 * Sequences patterns, triggers SFX at tempo intervals
 */
class MusicPlayer {
public:
    explicit MusicPlayer(SFXSynthesizer* sfxSynth);
    ~MusicPlayer();

    /**
     * @brief Render music samples
     */
    void RenderSamples(float* buffer, int frames, int channels);

private:
    SFXSynthesizer* sfxSynth;
    
    // TODO Phase 5.14: Pattern sequencer, tempo, active track
};

#endif // MUSICPLAYER_H
