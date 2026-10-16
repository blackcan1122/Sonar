#include "SONAR/GameBootstrap.hpp"

#include "Base/GameInstance.h"
#include "GameModes/Menu.hpp"
#include "GameModes/OptionsMode.hpp"
#include "GameModes/SandboxGameMode.hpp"

namespace SONAR
{
	void RegisterDefaultGameModes()
	{
		GameInstance::RegisterGameMode("Menu", []() { return new MenuMode(); });
		GameInstance::RegisterGameMode("Sandbox", []() { return new SandboxGameMode(); });
		GameInstance::RegisterGameMode("Options", []() { return new OptionsMode(); });
		GameInstance::SetStartupGameMode("Menu");
	}
}