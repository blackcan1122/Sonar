#include "Base/StateMachine.h"
#include "Base/GameMode.h"

void StateMachine::RegisterState(const std::string& StateName, std::function<GameMode* ()> FactoryFunction)
{
	StateFactory[StateName] = FactoryFunction;
}

void StateMachine::ChangeState(const std::string& StateName)
{
	if (CurrentGameMode)
	{
		std::cout << CurrentGameMode << std::endl;
		delete CurrentGameMode;
		CurrentGameMode = nullptr;
		if (CurrentGameMode == nullptr)
		{
			std::cout << "GameMode is now successfully Deleted" << std::endl;
		}
	}

	CurrentGameMode = StateFactory[StateName]();
	std::cout << "GameMode is now : " << StateName << std::endl;
}

void StateMachine::UpdateGameMode() const
{
	if (CurrentGameMode != nullptr)
	{
		CurrentGameMode->Update();
	}
}

GameMode* StateMachine::GetCurrentGameMode()
{
	return CurrentGameMode;
}
