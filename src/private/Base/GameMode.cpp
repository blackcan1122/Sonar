#include "Base/GameMode.h"

GameMode::GameMode()
{

}


void GameMode::Update()
{
	float Dt = GetFrameTime();
	std::vector<std::string> ObjectsToRemove;

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
	for (auto& Object : m_PendingKill)
	{
		DestroyObjectExplicitly(Object);
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

void GameMode::DestroyObjectExplicitly(std::shared_ptr<IObject> InObject)
{
	m_Objects.erase(InObject->GetName());

	InObject.reset();
}
