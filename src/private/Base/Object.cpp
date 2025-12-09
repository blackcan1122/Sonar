#include "Base/Object.hpp"
#include "Base/GameInstance.h"
#include "Base/GameMode.h"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"

GameMode* IObject::GetOutter()
{
	// This just temporary, since in the current State only one GameMode can be Active
	// Will be replaced, when several gamemodes can run cocurrently
	// Right now the GameMode will always be the Outter for every IObject Derived Class
	return GameInstance::g_ActiveStateMachine.GetCurrentGameMode();
}

void IObject::MarkForDestruction()
{
	bIsMarkedForDestruction = true;
}

bool IObject::IsMarkedForDestruction()
{
	return bIsMarkedForDestruction;
}

Object::Object()
{

}
