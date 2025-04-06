#include "StateMachines/GameModeSwitcher.h"
#include "Base/GameMode.h"

void GameModeSwitcher::RegisterState(const std::string& StateName, std::function<GameMode* ()> FactoryFunction)
{
	StateFactory[StateName] = FactoryFunction;
}

void GameModeSwitcher::ChangeState(const std::string& StateName)
{
	LOG_INFO(l_GAMEMODE, TEXT("GameMode: '{}' Requested Loading", StateName));
	if (CurrentGameMode != nullptr)
	{
		LastGameMode = CurrentGameMode;
		bPendingKillLastMode = true;
		LOG_INFO(l_GAMEMODE, TEXT("GameMode: '{}' Marked for Deletion", LastGameMode->GetName()));

	}
	

	CurrentGameMode = StateFactory[StateName]();
	CurrentGameMode->BeginPlay();
	LOG_INFO(l_GAMEMODE, TEXT("GameMode: '{}' Successfully Loaded and 'BeginPlay()' called", StateName));

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

			LOG_INFO(l_GAMEMODE, TEXT("Old GameMode: '{}' Deleted", LastName));

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
