#pragma once

//---------------------------------------------------------------------------
#pragma comment( lib, "ThirdParty/fmod/fmodex_vc" ) // Link in the fmodex_vc.lib static library


//---------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"
#include "Engine/Core/EngineSubsystem.hpp"

#include <string>
#include <vector>
#include <map>


//---------------------------------------------------------------------------
typedef unsigned int SoundID;
typedef void* AudioChannelHandle;
const unsigned int MISSING_SOUND_ID = 0xffffffff;


/////////////////////////////////////////////////////////////////////////////
class AudioSystem : public EngineSubsystem
{
public:
	AudioSystem();
	virtual ~AudioSystem() override;

	SoundID CreateOrGetSound(const std::string& soundFileName);
    SoundID CreateSoundFromMemory(const std::string& name, const std::vector<uint8_t>& buffer);

	AudioChannelHandle Play(SoundID soundID, float volumeLevel = 1.0f);
	void StopChannel( AudioChannelHandle channel );
	virtual void BeginFrame() override;
    bool IsPlaying(SoundID id);
    void StopAll();
    void SetFrequency(SoundID id, float frequency);
    float GetFrequency(SoundID id);
    void SetPitch(SoundID id, float pitch);
    float GetPitch(SoundID id);
    AudioChannelHandle GetChannelFromSound(SoundID id);

    virtual bool ProcessSystemMessage(const SystemMessage& msg) override;
protected:
	void InitializeFMOD();
	void ShutDownFMOD();
	void ValidateResult(FMOD_RESULT result);

protected:
	FMOD::System* m_fmodSystem;
	std::map<std::string, SoundID> m_registeredSoundIDs;
	std::vector<FMOD::Sound*> m_registeredSounds;
    std::vector<AudioChannelHandle> m_registeredChannels;
};

