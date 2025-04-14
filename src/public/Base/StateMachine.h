#pragma once
#include "Base/Core.h"


/**
 * @class StateMachine
 * @brief Manages transitions between game modes or states.
 *
 * Purpose:
 * - Provides a mechanism for switching between different game modes.
 * - Maintains the current active game mode and allows updates.
 *
 * Usage:
 * - Register states using `RegisterState()`.
 * - Transition to a new state with `ChangeState()`.
 * - Call `UpdateGameMode()` to update the active game mode.
 *
 * Notes:
 * - Ensure that states are properly registered before transitioning.
 * - Use `GetCurrentGameMode()` to retrieve the active game mode.
 */
class StateMachine
{
private:

	std::map<std::string, std::function<GameMode*()>> m_StateFactory;
	GameMode* m_CurrentGameMode;

public:

	StateMachine() : m_CurrentGameMode(nullptr) {};

	virtual void RegisterState(const std::string& StateName, std::function<GameMode*()> FactoryFunction);

	virtual void ChangeState(const std::string& StateName);

	virtual void UpdateGameMode() const;

	GameMode* GetCurrentGameMode();

};