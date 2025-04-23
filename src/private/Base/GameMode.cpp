#include "Base/GameMode.h"
#include "Base/Factory.hpp"

GameMode::GameMode()
{
	m_ObjectFactory = std::make_shared<Factory>(this);
}

GameMode::~GameMode()
{
	for (auto& Objects : m_Objects)
	{
		Objects.second->MarkForDestruction();
	}

	CollectPendingDestruction();
	CleanUpPendingKill();
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
