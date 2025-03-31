#pragma once
#include "Base/Core.h"
#include "Base/StateMachine.h"



class GameModeSwitcher : public StateMachine
{
private:

	std::map<std::string, std::function<GameMode*()>> StateFactory;
	GameMode* CurrentGameMode;
	GameMode* LastGameMode;

	bool bPendingKillLastMode = false;

public:

	GameModeSwitcher() : CurrentGameMode(nullptr), LastGameMode(nullptr) {};

	virtual void RegisterState(const std::string& StateName, std::function<GameMode*()> FactoryFunction);

	virtual void ChangeState(const std::string& StateName) override;

	virtual void KillLastGameMode();

	virtual void UpdateGameMode() const;

	virtual bool isPendingKillLastMode();

	GameMode* GetCurrentGameMode();

};