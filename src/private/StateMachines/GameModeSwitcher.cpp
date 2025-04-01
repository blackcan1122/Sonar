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
#if DEBUG
	std::cout << "GameMode is now : " << StateName << std::endl;
#endif
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
#if DEBUG
			std::cout << "Previous GameMode is now successfully Deleted" << std::endl;
#endif
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
