#include "Base/GameMode.h"
#include "Base/Factory.hpp"
#include "Base/Core.h"

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
	for (auto& Object : m_Objects)
	{
		Object.second->Tick(Dt);
		if (Object.second->IsMarkedForDestruction())
		{
			m_PendingKill.push_back(Object.second);
		}
	}

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
		}
	}
}

void GameMode::CleanUpPendingKill()
{
	for (int i = 0; i < m_PendingKill.size(); i++)
	{
		DestroyObjectExplicitly(m_PendingKill[i]);
	}

	m_PendingKill.clear();
}

void GameMode::RegisterObject(std::shared_ptr<IObject> Object)
{
	if (m_Objects.insert({ Object->GetName(), Object }).second == false)
	{
		LOG_ERROR("Couldn't Add {}, as it already registred.", Object->GetName());
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
