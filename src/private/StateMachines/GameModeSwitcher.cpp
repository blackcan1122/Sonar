#include "StateMachines/GameModeSwitcher.h"
#include "Base/GameMode.h"

void GameModeSwitcher::RegisterState(const std::string& StateName, std::function<GameMode* ()> FactoryFunction)
{
	StateFactory[StateName] = FactoryFunction;
}

void GameModeSwitcher::ChangeState(const std::string& StateName)
{
	if (CurrentGameMode != nullptr)
	{
		LastGameMode = CurrentGameMode;
		bPendingKillLastMode = true;
		LOG_INFO("GameMode: {} Marked for Deletion", LastGameMode->GetName());

	}
	
	LOG_INFO("GameMode: {} Requested Loading", StateName);
	CurrentGameMode = StateFactory[StateName]();
	LOG_INFO("GameMode: {} Sucessfully Loaded", StateName);


}

void GameModeSwitcher::KillLastGameMode()
{
	if (LastGameMode)
	{
		std::string LastName = LastGameMode->GetName();
		delete LastGameMode;
		LastGameMode = nullptr;
		if (LastGameMode == nullptr)
		{
			bPendingKillLastMode = false;

			LOG_INFO("GameMode: {} Deleted", LastName);

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

GameMode* GameModeSwitcher::GetLastGameMode()
{
	return LastGameMode;
}
