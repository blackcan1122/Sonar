#pragma once
#include "Base/Core.h"

class GameMode
{
public:
	GameMode();
	virtual ~GameMode() = default;

	virtual void Update();
	virtual void SetName(std::string Name);
	virtual std::string GetName();

protected:
	float m_DeltaTime = 0;
	std::string m_Name;

};