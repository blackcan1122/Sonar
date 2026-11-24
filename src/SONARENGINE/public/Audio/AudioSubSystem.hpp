#pragma once

#include "Base/SSubSystem.hpp"
#include "Base/SonarTypes.hpp"

class SONARENGINE_API AudioSubSystem : public SSubSystem
{
    AUTOBODY(AudioSubSystem, SSubSystem)
public:
    AudioSubSystem() = default;
    virtual ~AudioSubSystem() = default;

    void OnInitialize() override;
    void OnShutdown() override;
    void OnUpdate() override;

    void InitAudioDevice(void);
    void CloseAudioDevice(void);
    Sonar::Music LoadMusicStream(const char *fileName);
    void UnloadMusicStream(Sonar::Music& resource);
    void PlayMusicStream(Sonar::Music music);
    void UpdateMusicStream(Sonar::Music music);
    void StopMusicStream(Sonar::Music music);
    void SetMusicVolume(Sonar::Music music, float volume);
    void SetMusicLooping(Sonar::Music& music, bool looping);
};
