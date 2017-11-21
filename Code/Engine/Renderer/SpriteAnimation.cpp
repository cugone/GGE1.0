#include "Engine/Renderer/SpriteAnimation.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture2D.hpp"

SpriteAnimation::SpriteAnimation(const SpriteSheet& spriteSheet,
                                 float durationSeconds,
                                 SpriteAnimation::SpriteAnimMode playbackMode,
                                 int startSpriteIndex,
                                 int endSpriteIndex)
: m_spriteSheet(spriteSheet)
, m_durationSeconds(durationSeconds)
, m_elapsedSeconds(0.0f)
, m_elapsedFrameDeltaSeconds(0.0f)
, m_maxFrameDeltaSeconds(0.0f)
, m_playbackMode(playbackMode)
, m_startIndex(startSpriteIndex)
, m_endIndex(endSpriteIndex)
, m_currentIndex(startSpriteIndex)
, m_isPlaying(true)
{
    m_maxFrameDeltaSeconds = durationSeconds / (endSpriteIndex + 1 - startSpriteIndex);
}

void SpriteAnimation::Update(float deltaSeconds) {

    m_elapsedFrameDeltaSeconds += deltaSeconds;
    if(m_elapsedFrameDeltaSeconds >= m_maxFrameDeltaSeconds) {
        ++m_currentIndex;
        while(m_elapsedFrameDeltaSeconds >= m_maxFrameDeltaSeconds) {
            m_elapsedFrameDeltaSeconds -= m_maxFrameDeltaSeconds;
        }
    }
    switch(m_playbackMode) {
        case SpriteAnimMode::LOOPING:
            if(IsFinished()) {
                while(m_elapsedSeconds >= m_durationSeconds) {
                    m_elapsedSeconds -= m_durationSeconds;
                }
            }
            break;
        case SpriteAnimMode::PLAY_TO_END:
            if(IsFinished()) {
                m_isPlaying = false;
                m_elapsedSeconds = m_durationSeconds;
            }
            break;
        case SpriteAnimMode::PINGPONG:
            if(m_elapsedSeconds < 0.0) {
                deltaSeconds *= -1.0f;
                while(m_elapsedSeconds < 0.0f) {
                    m_elapsedSeconds += m_durationSeconds;
                }
            } else if(m_elapsedSeconds >= m_durationSeconds) {
                deltaSeconds *= -1.0f;
                while(m_elapsedSeconds >= m_durationSeconds) {
                    m_elapsedSeconds -= m_durationSeconds;
                }
            }
            break;
        case SpriteAnimMode::LOOPING_REVERSE:
            if(IsFinished()) {
                m_elapsedSeconds = m_durationSeconds;
                deltaSeconds *= -1.0f;
            }
            break;
    }
    m_elapsedSeconds += deltaSeconds;
}

AABB2 SpriteAnimation::GetCurrentTexCoords() const {
    return m_spriteSheet.GetTexCoordsForSpriteIndex(m_currentIndex);
}

const Texture* const SpriteAnimation::GetTexture() const {
    return &m_spriteSheet.GetTexture();
}

const Texture2D* const SpriteAnimation::GetTexture2D() const {
    return &m_spriteSheet.GetTexture2D();
}
void SpriteAnimation::Pause() {
    m_isPlaying = false;
}

void SpriteAnimation::Resume() {
    m_isPlaying = true;
}

void SpriteAnimation::Reset() {
    m_elapsedSeconds = 0.0f;
}

bool SpriteAnimation::IsFinished() const {
    if(!m_isPlaying) {
        return false;
    }
    switch(m_playbackMode) {
        case SpriteAnimMode::LOOPING: /* FALL THROUGH */
        case SpriteAnimMode::PLAY_TO_END:
            return m_elapsedSeconds > m_durationSeconds;
        case SpriteAnimMode::LOOPING_REVERSE: /* FALL THROUGH */
        case SpriteAnimMode::PLAY_TO_BEGINNING:
            return m_elapsedSeconds < 0.0f;
        case SpriteAnimMode::PINGPONG:
            return false;
        default:
            return m_elapsedSeconds > m_durationSeconds;
    }
}

bool SpriteAnimation::IsPlaying() const {
    return m_isPlaying;
}

float SpriteAnimation::GetDurationSeconds() const {
    return m_durationSeconds;
}

float SpriteAnimation::GetSecondsElapsed() const {
    return m_elapsedSeconds;
}

float SpriteAnimation::GetSecondsRemaining() const {
    return m_durationSeconds - m_elapsedSeconds;
}

float SpriteAnimation::GetFractionElapsed() const {
    return m_elapsedSeconds / m_durationSeconds;
}

float SpriteAnimation::GetFractionRemaining() const {
    return m_durationSeconds - m_elapsedSeconds / m_durationSeconds;
}

void SpriteAnimation::SetSecondsElapsed(float secondsElapsed) {
    m_elapsedSeconds = secondsElapsed;
}

void SpriteAnimation::SetFractionElapsed(float fractionElapsed) {
    m_elapsedSeconds = m_durationSeconds * fractionElapsed;
}
