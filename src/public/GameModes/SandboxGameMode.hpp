#pragma once
#include "Base/Core.h"
#include "Base/GameMode.h"
#include "UI/WaterfallDisplay.hpp"

/**
* Core game state manager and object lifecycle controller.
*
* Acts as the central authority for:
* - Object creation/destruction via embedded Factory
* - Frame-based Tick() execution hierarchy
* - Safe deferred object cleanup
* - Game-specific rule implementation foundation
*
* Key Functionality Demonstrated in SandboxGameMode:
* 1. Object Management:
*    - Creates entities via ObjectFactory.NewObject<T>() pattern
*    - Tracks objects using weak_ptr (safe observation) and shared_ptr (explicit ownership)
*    - Implements deferred destruction via MarkForDestruction() + CleanUpPendingKill()
*
* 2. Update Cycle:
*    - Base Update() handles core Tick() execution and cleanup
*    - Derived classes extend with custom logic (e.g., input handling, rendering)
*    - Maintains delta time for frame-consistent updates
*
* 3. Lifecycle Hooks:
*    - Provides virtual methods for game-specific customization
*    - Enables name-based identification through SetName/GetName
*
* Core Architecture:
* - Factory Pattern: Embedded ObjectFactory handles type-safe object creation
* - Double Buffering: m_Objects (active) vs m_PendingKill (destruction queue)
* - RAII Lifetime: Automatic cleanup via shared_ptr/weak_ptr mechanics
*
* Usage Pattern (see SandboxGameMode):
* 1. Inherit and implement game-specific logic:
*    > class MyGameMode : public GameMode
* 2. Create objects via factory:
*    > ObjectFactory.NewObject<MyObjectType>(constructor_args)
* 3. Store references appropriately:
*    > std::weak_ptr for observation (WaterfallDisplay)
*    > std::shared_ptr only when maintaining ownership
* 4. Modify object states:
*    > Access via weak_ptr::lock() during Update()
*    > Mark for destruction when needed
* 5. Extend Update():
*    > Add custom logic before/after base Update() call
*    > Handle input/rendering specific to game mode
*
* Critical Implementation Notes:
* - All Tick() methods of IObjects are called are automatic - do NOT call directly
* - Object destruction is deferred - use MarkForDestruction() instead of delete
* - Prefer weak_ptr for member references unless maintaining explicit ownership
* - Override cleanup logic via DestroyObjectExplicitly() when needed
*/

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

	std::weak_ptr<Waterfall> WaterfallDisplay;
	std::shared_ptr<Waterfall> WaterfallDisplay2;
	std::shared_ptr<Waterfall> WaterfallDisplay3;
	std::shared_ptr<Waterfall> WaterfallDisplay4;

};
