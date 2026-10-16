#include "StateMachines/GameModeSwitcher.h"
#include "Base/GameMode.h"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"

void GameModeSwitcher::RegisterState(const std::string& StateName, std::function<GameMode* ()> FactoryFunction)
{
	StateFactory[StateName] = FactoryFunction;
}

void GameModeSwitcher::ChangeState(const std::string& StateName)
{
	LOG_INFO(l_GAMEMODE, SONAR_FMT("============================================="));
	LOG_INFO(l_GAMEMODE, SONAR_FMT("GameMode: '{}' Requested Loading", StateName));
	LOG_INFO(l_GAMEMODE, SONAR_FMT("============================================="));
	if (CurrentGameMode != nullptr)
	{
		LastGameMode = CurrentGameMode;
		bPendingKillLastMode = true;
		LOG_INFO(l_GAMEMODE, SONAR_FMT("GameMode: '{}' Marked for Deletion", LastGameMode->GetName()));

	}
	

	CurrentGameMode = StateFactory[StateName]();
	CurrentGameMode->BeginPlay();
	LOG_INFO(l_GAMEMODE, SONAR_FMT("============================================="));
	LOG_INFO(l_GAMEMODE, SONAR_FMT("GameMode: '{}' Successfully Loaded and 'BeginPlay()' called", StateName));
	LOG_INFO(l_GAMEMODE, SONAR_FMT("============================================="));

}

void GameModeSwitcher::KillLastGameMode()
{

    if (LastGameMode)
    {
        std::string LastName = LastGameMode->GetName();
        LOG_INFO(l_DISPATCHER, SONAR_FMT("=== Before deleting GameMode '{}' ===", LastName));
        
        delete LastGameMode;
        LastGameMode = nullptr;
        
        if (LastGameMode == nullptr)
        {
            bPendingKillLastMode = false;
            LOG_INFO(l_GAMEMODE, SONAR_FMT("Old GameMode: '{}' Deleted", LastName));
        }
    }
}

void GameModeSwitcher::ShutdownCurrentGameMode()
{
	if (CurrentGameMode)
	{
		std::string CurrentName = CurrentGameMode->GetName();
		LOG_INFO(l_GAMEMODE, SONAR_FMT("Shutting down active GameMode: '{}'", CurrentName));
		delete CurrentGameMode;
		CurrentGameMode = nullptr;
	}

	if (LastGameMode)
	{
		std::string LastName = LastGameMode->GetName();
		LOG_INFO(l_GAMEMODE, SONAR_FMT("Cleaning deferred GameMode during shutdown: '{}'", LastName));
		delete LastGameMode;
		LastGameMode = nullptr;
	}

	bPendingKillLastMode = false;
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
