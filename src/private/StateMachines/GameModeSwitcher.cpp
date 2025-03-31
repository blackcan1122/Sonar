#include "StateMachines/GameModeSwitcher.h"
#include "Base/GameMode.h"

void GameModeSwitcher::RegisterState(const std::string& StateName, std::function<GameMode* ()> FactoryFunction)
{
	StateFactory[StateName] = FactoryFunction;
}

void GameModeSwitcher::ChangeState(const std::string& StateName)
{
	LastGameMode = CurrentGameMode;

	CurrentGameMode = StateFactory[StateName]();
	std::cout << "GameMode is now : " << StateName << std::endl;

	bPendingKillLastMode = true;

}

void GameModeSwitcher::KillLastGameMode()
{
	if (LastGameMode)
	{
		delete LastGameMode;
		LastGameMode = nullptr;
		if (LastGameMode == nullptr)
		{
			bPendingKillLastMode = false;
			std::cout << "GameMode is now successfully Deleted" << std::endl;
		}
	}
}

void GameModeSwitcher::UpdateGameMode() const
{
	if (CurrentGameMode != nullptr)
	{
		CurrentGameMode->Update();
	}
}

bool GameModeSwitcher::isPendingKillLastMode()
{
	return bPendingKillLastMode;
}

GameMode* GameModeSwitcher::GetCurrentGameMode()
{
	return CurrentGameMode;
}
