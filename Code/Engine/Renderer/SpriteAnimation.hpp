#pragma once

#include "Engine/Math/AABB2.hpp"

class SpriteSheet;
class Texture;
class Texture2D;

class SpriteAnimation {
public:
    enum class SpriteAnimMode {
        PLAY_TO_END,	// Play from time=0 to durationSeconds, then finish
        PLAY_TO_BEGINNING, // Play from time=durationSeconds to 0, then finish
        LOOPING,		// Play from time=0 to end then repeat (never finish)
        LOOPING_REVERSE, // Play from time=durationSeconds then repeat (never finish)
        PINGPONG, 	    // optional, play forwards, backwards, forwards...
        NUM_SPRITE_ANIM_MODES,
    };

    SpriteAnimation(const SpriteSheet& spriteSheet, float durationSeconds,
                    SpriteAnimMode playbackMode, int startSpriteIndex, int endSpriteIndex);

    void Update(float deltaSeconds);
    AABB2 GetCurrentTexCoords() const;	               // Based on the current elapsed time
    const Texture* const GetTexture() const;
    const Texture2D* const GetTexture2D() const;
    void Pause();					                   // Starts unpaused (playing) by default
    void Resume();				                       // Resume after pausing
    void Reset();					                   // Rewinds to time 0 and starts (re)playing
    bool IsFinished() const;                           //{ return m_isFinished; }
    bool IsPlaying() const;                            //{ return m_isPlaying; }
    float GetDurationSeconds() const;                  //{ return m_durationSeconds; }
    float GetSecondsElapsed() const;                   //{ return m_elapsedSeconds; }
    float GetSecondsRemaining() const;
    float GetFractionElapsed() const;
    float GetFractionRemaining() const;
    void SetSecondsElapsed(float secondsElapsed);	   // Jump to specific time
    void SetFractionElapsed(float fractionElapsed);    // e.g. 0.33f for one-third in
protected:
private:
    const SpriteSheet& m_spriteSheet;
    float m_durationSeconds;
    float m_elapsedSeconds;
    float m_elapsedFrameDeltaSeconds;
    float m_maxFrameDeltaSeconds;
    SpriteAnimMode m_playbackMode;
    int m_startIndex;
    int m_endIndex;
    int m_currentIndex;
    bool m_isPlaying;
};
