#pragma once
#include "Base/Core.h"



class StateMachine
{
private:

	std::map<std::string, std::function<GameMode*()>> StateFactory;
	GameMode* CurrentGameMode;

public:

	StateMachine() : CurrentGameMode(nullptr) {};

	virtual void RegisterState(const std::string& StateName, std::function<GameMode*()> FactoryFunction);

	virtual void ChangeState(const std::string& StateName);

	virtual void UpdateGameMode() const;

	GameMode* GetCurrentGameMode();

};