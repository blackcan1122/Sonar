#pragma once
#include "Base/SoftObject.hpp"
#include "Base/World.hpp"
#include "Base/Core.h"


/**
 * @class GameMode
 * @brief Represents a core game mode, responsible for object lifecycle, game logic, and frame-based updates.
 *
 * Purpose:
 * - Manages object creation and destruction via the embedded `Factory`.
 * - Handles frame-based updates and deferred cleanup of objects.
 * - Acts as the foundation for implementing game-specific logic and rules.
 *
 * Usage:
 * 1. Inherit from `GameMode` to define custom game mode logic:
 *    @code
 *    class MyGameMode : public GameMode {
 *        void Update() override;
 *        void BeginPlay() override;
 *    };
 *    @endcode
 * 2. Use `m_ObjectFactory.NewObject<T>()` to create objects.
 * 3. Override `Update` for per-frame logic and `BeginPlay` for initialization.
 *
 * Notes for Derived Classes:
 * - Use `MarkForDestruction` instead of directly deleting objects.
 * - Store SoftObjectPath unless explicit ownership is required.
 * - Use `SetName` and `GetName` for identifying game modes.
 */

class AssetRegistry;
class Factory;
class GameInstance;

class GameMode
{
friend IObject;
friend Factory;
friend GameInstance;
friend AssetRegistry;

public:
	GameMode();
	virtual ~GameMode();

	virtual void Update();
	virtual void SetName(std::string Name);
	virtual void BeginPlay();
	virtual SoftObjectPath<World> GetWorld();
	virtual std::string GetName();
	std::shared_ptr<Factory> m_ObjectFactory;

	virtual bool DestroyObjectExplicitly(std::shared_ptr<IObject> InObject);

	template<typename T>
	bool DestroyObjectExplicitly(SoftObjectPath<T> InObject)
	{
		std::shared_ptr<T> Obj = InObject.TryLoad();
		if (Obj)
		{
			return DestroyObjectExplicitly(Obj);
		}

		return false;
	}


protected:

	virtual void CollectPendingDestruction();
	virtual void CleanUpPendingKill();
	virtual void RegisterObject(std::shared_ptr<IObject> InObject);
	virtual void UnregisterObject(IObject* InObject);
	std::unordered_map<std::string, std::shared_ptr<IObject>> m_Objects;
	std::vector<std::shared_ptr<IObject>> m_PendingKill;
	float m_DeltaTime = 0;
	std::string m_Name;
	
	// Flag to prevent UnregisterObject calls during destruction
	bool m_IsDestroying = false;

	// Optional
	SoftObjectPath<World> m_World;

};