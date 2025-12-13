#pragma once
#include "Base/Core.h"
#include "Base/GameMode.h"


class ShutDownGame : public GameMode
{
    
public:
    ShutDownGame();

    virtual void Update() override;
	virtual void BeginPlay() override;

};