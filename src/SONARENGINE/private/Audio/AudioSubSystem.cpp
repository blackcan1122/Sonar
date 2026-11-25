#include "Base/Logging.h"
#include "Base/SonarTypeConversions.hpp"
#include "Audio/AudioSubSystem.hpp"

using Sonar::Backend::ToRaylib;
using Sonar::Backend::FromRaylib;

void AudioSubSystem::OnInitialize()
{
    LOG_INFO("AudioSubSystem initialized");
}

void AudioSubSystem::OnShutdown()
{
    LOG_INFO("AudioSubSystem shutdown");
}

void AudioSubSystem::OnUpdate() {}

void AudioSubSystem::InitAudioDevice(void) { ::InitAudioDevice(); }

void AudioSubSystem::CloseAudioDevice(void) { ::CloseAudioDevice(); }

Sonar::Music AudioSubSystem::LoadMusicStream(const char *fileName) { auto result=FromRaylib(::LoadMusicStream(fileName)); return result; }

void AudioSubSystem::UnloadMusicStream(Sonar::Music& resource)
{
    if (resource.IsValid()) ::UnloadMusicStream(ToRaylib(resource));
    resource = {};
}

void AudioSubSystem::PlayMusicStream(Sonar::Music music) { ::PlayMusicStream(ToRaylib(music)); }

void AudioSubSystem::UpdateMusicStream(Sonar::Music music) { ::UpdateMusicStream(ToRaylib(music)); }

void AudioSubSystem::StopMusicStream(Sonar::Music music) { ::StopMusicStream(ToRaylib(music)); }

void AudioSubSystem::SetMusicVolume(Sonar::Music music, float volume) { ::SetMusicVolume(ToRaylib(music),volume); }

void AudioSubSystem::SetMusicLooping(Sonar::Music& music, bool looping) { music.looping = looping; }
