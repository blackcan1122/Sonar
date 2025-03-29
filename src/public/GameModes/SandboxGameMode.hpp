#pragma once
#include "Base/Core.h"
#include "Base/GameMode.h"

class SandboxGameMode : public GameMode
{
public:
	SandboxGameMode();
	~SandboxGameMode();

	void Update() override;
	void SetName(std::string Name) override;
	std::string GetName() override;

protected:
	float m_DeltaTime = 0;

};
