#pragma once
#include "Base/Core.h"
#include "Base/Factory.hpp"

/**
* Core game state manager and object lifecycle controller.
*
* Acts as the central authority for:
* - Object creation/destruction via embedded Factory
* - Frame-based Tick(float Deltatime) execution hierarchy
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
* - Constructor should be handled with care, Initialization should start in BeginPlay
* - All Tick() methods of IObjects are called are automatic - do NOT call directly
* - Object destruction is deferred - use MarkForDestruction() instead of delete
* - Prefer weak_ptr for member references unless maintaining explicit ownership
* - Override cleanup logic via DestroyObjectExplicitly() when needed
*/

class GameMode
{
friend IObject;
friend Factory;
friend GameInstance;
public:
	GameMode();
	virtual ~GameMode() = default;

	virtual void Update();
	virtual void SetName(std::string Name);
	virtual void BeginPlay();
	virtual std::string GetName();
	Factory m_ObjectFactory = Factory(this);

	virtual bool DestroyObjectExplicitly(std::shared_ptr<IObject> InObject);

	template<typename T>
	bool DestroyObjectExplicitly(SoftObjectPath<T> InObject);


protected:

	virtual void CollectPendingDestruction();
	virtual void CleanUpPendingKill();
	virtual void RegisterObject(std::shared_ptr<IObject> InObject);
	virtual void UnregisterObject(IObject* InObject);
	std::unordered_map<std::string, std::shared_ptr<IObject>> m_Objects;
	std::vector<std::shared_ptr<IObject>> m_PendingKill;
	float m_DeltaTime = 0;
	std::string m_Name;

};

template<typename T>
inline bool GameMode::DestroyObjectExplicitly(SoftObjectPath<T> InObject)
{
	std::shared_ptr<T> Obj = InObject.TryLoad();
	if (Obj)
	{
		return DestroyObjectExplicitly(Obj);
	}

	return false;
}
