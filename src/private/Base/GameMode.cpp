#include "Base/GameMode.h"
#include "Base/Factory.hpp"
#include "Base/Core.h"
#include "UI/GridLayoutManager.hpp"
#include "Base/GameInstance.h"
#include "Base/TickGroup.hpp"

// ObjectType Defintions

#include "Entities/Player.hpp"
#include "UI/Display.hpp"
#include "Base/World.hpp"
#include "Base/Entity.hpp"

GameMode::GameMode()
{
	m_ObjectFactory = std::make_shared<Factory>(this);
}

GameMode::~GameMode()
{
	#if DEBUG
	LogInfo(l_GAMEMODE, TEXT("Start Destructor of GameMode: {}", this->m_Name));
	LogInfo(l_GAMEMODE, TEXT("{} Objects belong to GameMode before Cleaning", this->m_Objects.size()));
	#endif
	// Set flag to prevent UnregisterObject calls during destruction
	m_IsDestroying = true;
	
	// Clear objects map - this will trigger shared_ptr destructors
	// but UnregisterObject calls will be ignored due to m_IsDestroying flag
	    // Remove all event listeners FIRST to break the circular reference
	for (auto& obj : m_Objects)
	{
		obj.second->MarkForDestruction();
	}

	m_Objects.clear();
	m_PendingKill.clear();
	#if DEBUG
	LogInfo(l_GAMEMODE, TEXT("Finished Destructor of GameMode: {}", this->m_Name));
	LogInfo(l_GAMEMODE, TEXT("{} Objects belong to GameMode after Cleaning", this->m_Objects.size()));

	#endif
}


void GameMode::Update()
{
	float Dt = GetFrameTime();
	for (auto& DefaultTickObj : m_ObjectsByTickGroup[ETickGroup::DefaultTick])
	{
		auto ObjPtr = DefaultTickObj.TryLoad();
		if (ObjPtr)
		{
			ObjPtr->Tick(Dt);
		}
	}

	for (auto& PostTickObj : m_ObjectsByTickGroup[ETickGroup::PostTick])
	{
		auto ObjPtr = PostTickObj.TryLoad();
		if (ObjPtr)
		{
			ObjPtr->Tick(Dt);
		}
	}

	for (auto& PreTickObj : m_ObjectsByTickGroup[ETickGroup::Rendering])
	{
		auto ObjPtr = PreTickObj.TryLoad();
		if (ObjPtr)
		{
			ObjPtr->Tick(Dt);
		}
	}

	for (auto& PhysicsTickObj : m_ObjectsByTickGroup[ETickGroup::MAX])
	{
		auto ObjPtr = PhysicsTickObj.TryLoad();
		if (ObjPtr)
		{
			ObjPtr->Tick(Dt);
		}
	}

	CollectPendingDestruction();
	CleanUpPendingKill();
}

void GameMode::SetName(std::string Name)
{
	this->m_Name = Name;
}

void GameMode::BeginPlay()
{
}

SoftObjectPath<World> GameMode::GetWorld()
{
	return m_World;
}

std::string GameMode::GetName()
{
	return this->m_Name;
}

void GameMode::CollectPendingDestruction()
{
	for (auto& Object : m_Objects)
	{
		if (Object.second->IsMarkedForDestruction())
		{
			m_PendingKill.push_back(Object.second);
			m_ObjectsToUnregister.push_back({ Object.second->GetName(), Object.second->GetStaticClass() });
		}
	}
}

void GameMode::CleanUpPendingKill()
{
	for (size_t i = 0; i < m_PendingKill.size(); i++)
	{
		m_ObjectsByTickGroup[m_PendingKill[i]->GetTickGroup()].erase(
			std::remove(
				m_ObjectsByTickGroup[m_PendingKill[i]->GetTickGroup()].begin(),
				m_ObjectsByTickGroup[m_PendingKill[i]->GetTickGroup()].end(),
				SoftObjectPath<IObject>(m_PendingKill[i]->GetName())
			),
			m_ObjectsByTickGroup[m_PendingKill[i]->GetTickGroup()].end()
		);

		m_ObjectsByType[m_ObjectsToUnregister[i].second].erase(m_ObjectsToUnregister[i].first);

		DestroyObjectExplicitly(m_PendingKill[i]);
	}

	m_PendingKill.clear();
	m_ObjectsToUnregister.clear();
}

void GameMode::RegisterObject(std::shared_ptr<IObject> Object)
{
	if (m_Objects.insert({ Object->GetName(), Object }).second == false)
	{
		LOG_ERROR("Couldn't Add {}, as it already registred.", Object->GetName());
	}

	if (m_ObjectsByType[Object->GetStaticClass()].insert({Object->GetName(), SoftObjectPath<IObject>(Object->GetName())}).second == false)
	{
		LOG_ERROR("Couldn't Add {}, as it already registred in Type Map.", Object->GetName());
	}

	if (m_ObjectsByTickGroup[Object->GetTickGroup()].size() == 0 ||
		std::find(m_ObjectsByTickGroup[Object->GetTickGroup()].begin(),
			m_ObjectsByTickGroup[Object->GetTickGroup()].end(),
			SoftObjectPath<IObject>(Object->GetName())) == m_ObjectsByTickGroup[Object->GetTickGroup()].end())
	{
		m_ObjectsByTickGroup[Object->GetTickGroup()].push_back(SoftObjectPath<IObject>(Object->GetName()));
	}
}

void GameMode::UnregisterObject(IObject* inObject)
{
	if (m_IsDestroying) {
		return;
	}
	
	m_Objects.erase(inObject->GetName());
}

bool GameMode::DestroyObjectExplicitly(std::shared_ptr<IObject> InObject)
{
	if (m_Objects.erase(InObject->GetName()) != 0)
	{
		InObject.reset();
		return true;
	}
	return false;
}
