#include "Engine/Renderer/SpriteAnimation.hpp"

#include "Engine/Core/StringUtils.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture2D.hpp"

SpriteAnimation::SpriteAnimation(SpriteSheet* spriteSheet,
                                 float durationSeconds,
                                 SpriteAnimation::SpriteAnimMode playbackMode,
                                 int startSpriteIndex,
                                 int endSpriteIndex)
: m_spriteSheet(spriteSheet)
, m_durationSeconds(durationSeconds)
, m_playbackMode(playbackMode)
, m_startIndex(startSpriteIndex)
, m_endIndex(endSpriteIndex)
{
    m_maxFrameDeltaSeconds = durationSeconds / (endSpriteIndex + 1 - startSpriteIndex);
}

SpriteAnimation::SpriteAnimation(SimpleRenderer& renderer, const XMLElement& elem)
{
    DataUtils::ValidateXmlElement(elem, "animation", "spritesheet,animationset", "", "", "name");
    auto xml_sheet = elem.FirstChildElement("spritesheet");
    DataUtils::ValidateXmlElement(*xml_sheet, "spritesheet", "", "src,dimensions");
    m_spriteSheet = renderer.CreateSpriteSheet(*xml_sheet);

    auto xml_animset = elem.FirstChildElement("animationset");
    DataUtils::ValidateXmlElement(*xml_animset, "animationset", "", "startindex,endindex,duration", "", "loop,reverse,pingpong");

    m_startIndex = DataUtils::ParseXmlAttribute(*xml_animset, "startindex", m_startIndex);
    m_endIndex = DataUtils::ParseXmlAttribute(*xml_animset, "endindex", m_endIndex);

    auto min_duration = 0.016f;
    m_durationSeconds = DataUtils::ParseXmlAttribute(*xml_animset, "duration", m_durationSeconds);
    if(m_durationSeconds < min_duration) {
        m_durationSeconds = min_duration;
    }

    bool is_looping = DataUtils::ParseXmlAttribute(*xml_animset, "loop", false);
    bool is_reverse = DataUtils::ParseXmlAttribute(*xml_animset, "reverse", false);
    bool is_pingpong = DataUtils::ParseXmlAttribute(*xml_animset, "pingpong", false);
    m_playbackMode = GetAnimModeFromOptions(is_looping, is_reverse, is_pingpong);

    m_maxFrameDeltaSeconds = m_durationSeconds / (m_endIndex + 1 - m_startIndex);
}

SpriteAnimation::~SpriteAnimation() {
    delete m_spriteSheet;
    m_spriteSheet = nullptr;
}

void SpriteAnimation::Update(float deltaSeconds) {

    m_elapsedFrameDeltaSeconds += deltaSeconds;
    while(m_elapsedFrameDeltaSeconds >= m_maxFrameDeltaSeconds) {
        m_elapsedFrameDeltaSeconds -= m_maxFrameDeltaSeconds;
    }
    switch(m_playbackMode) {
        case SpriteAnimMode::LOOPING:
            if(IsFinished()) {
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
        case SpriteAnimMode::PLAY_TO_BEGINNING:
            if(IsFinished()) {
                m_isPlaying = false;
                m_elapsedSeconds = 0.0f;
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
        default:
            break;
    }
    m_elapsedSeconds += deltaSeconds;
}

AABB2 SpriteAnimation::GetCurrentTexCoords() const {

    auto framesPerSecond = 1.0f / m_maxFrameDeltaSeconds;
    auto frameIndex = static_cast<int>(framesPerSecond * m_elapsedSeconds);

    switch(m_playbackMode) {
    case SpriteAnimMode::PLAY_TO_END:
        if(frameIndex > m_endIndex) {
            frameIndex = m_endIndex;
        }
        break;
    case SpriteAnimMode::PLAY_TO_BEGINNING:
        if(frameIndex < 0) {
            frameIndex = 0;
        }
        break;
    case SpriteAnimMode::LOOPING: /* FALLTHROUGH */
    case SpriteAnimMode::LOOPING_REVERSE:
        if(frameIndex > m_endIndex) {
            frameIndex = 0;
        }
        if(frameIndex < 0) {
            frameIndex = m_endIndex;
        }
        break;
    case SpriteAnimMode::PINGPONG:
        if(frameIndex > m_endIndex) {
            frameIndex = m_endIndex;
        }
        if(frameIndex < 0) {
            frameIndex = 0;
        }
        break;
    default:
        break;
    }

    return m_spriteSheet->GetTexCoordsForSpriteIndex(frameIndex);
}

const Texture* const SpriteAnimation::GetTexture() const {
    return &m_spriteSheet->GetTexture();
}

const Texture2D* const SpriteAnimation::GetTexture2D() const {
    return &m_spriteSheet->GetTexture2D();
}

void SpriteAnimation::TogglePause() {
    m_isPlaying = !m_isPlaying;
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

SpriteAnimation::SpriteAnimMode SpriteAnimation::GetAnimModeFromOptions(bool looping, bool backwards, bool ping_pong /*= false*/) {

    if(ping_pong) {
        return SpriteAnimMode::PINGPONG;
    }

    if(looping) {
        if(backwards) {
            return SpriteAnimMode::LOOPING_REVERSE;
        }
        return SpriteAnimMode::LOOPING;
    }
    if(backwards) {
        return SpriteAnimMode::PLAY_TO_BEGINNING;
    }
    return SpriteAnimMode::PLAY_TO_END;
}
