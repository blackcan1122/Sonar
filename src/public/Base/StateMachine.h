#pragma once
#include "Base/Core.h"



class StateMachine
{
private:

	std::map<std::string, std::function<GameMode*()>> StateFactory;
	GameMode* CurrentGameMode;

public:

	StateMachine() : CurrentGameMode(nullptr) {};

	void RegisterState(const std::string& StateName, std::function<GameMode*()> FactoryFunction);

	void ChangeState(const std::string& StateName);

	void UpdateGameMode() const;

	GameMode* GetCurrentGameMode();

};