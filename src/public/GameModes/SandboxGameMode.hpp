#pragma once
#include "Base/Core.h"
#include "Base/GameMode.h"
#include "UI/WaterfallDisplay.hpp"

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

	std::shared_ptr<Waterfall> WaterfallDisplay;
	std::shared_ptr<Waterfall> WaterfallDisplay2;
	std::shared_ptr<Waterfall> WaterfallDisplay3;
	std::shared_ptr<Waterfall> WaterfallDisplay4;

};
