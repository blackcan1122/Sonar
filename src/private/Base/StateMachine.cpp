#include "Base/StateMachine.h"
#include "Base/GameMode.h"

void StateMachine::RegisterState(const std::string& StateName, std::function<GameMode* ()> FactoryFunction)
{
	m_StateFactory[StateName] = FactoryFunction;
}

void StateMachine::ChangeState(const std::string& StateName)
{
	if (m_CurrentGameMode)
	{
		delete m_CurrentGameMode;
		m_CurrentGameMode = nullptr;
		if (m_CurrentGameMode == nullptr)
		{
			std::cout << "GameMode is now successfully Deleted" << std::endl;
		}
	}


	m_CurrentGameMode = m_StateFactory[StateName]();
	std::cout << "GameMode is now : " << StateName << std::endl;

}

void StateMachine::UpdateGameMode() const
{
	if (m_CurrentGameMode != nullptr)
	{
		m_CurrentGameMode->Update();
	}
}

GameMode* StateMachine::GetCurrentGameMode()
{
	return m_CurrentGameMode;
}
