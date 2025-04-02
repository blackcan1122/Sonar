#include "Base/Object.hpp"
#include "Base/GameInstance.h"
#include "Base/GameMode.h"


GameMode* IObject::GetOutter()
{
	return GameInstance::ActiveStateMachine.GetCurrentGameMode();
	//return nullptr
}

void IObject::MarkForDestruction()
{
	bIsMarkedForDestruction = true;
}

bool IObject::IsMarkedForDestruction()
{
	return bIsMarkedForDestruction;
}
