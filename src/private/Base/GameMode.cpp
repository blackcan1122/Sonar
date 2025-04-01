#include "Base/GameMode.h"

GameMode::GameMode()
{

}


void GameMode::Update()
{
	float Dt = GetFrameTime();
	for (auto& Object : m_Objects)
	{
		Object->Tick(Dt);
		if (Object->IsMarkedForDestruction())
		{
			m_PendingKill.push_back(Object);
		}
	}

	CleanUpPendingKill();
}

void GameMode::SetName(std::string Name)
{
	this->m_Name = Name;
}

std::string GameMode::GetName()
{
	return this->m_Name;
}

void GameMode::CollectPendingDestruction()
{
	for (auto& Object : m_Objects)
	{
		if (Object->IsMarkedForDestruction())
		{
			m_PendingKill.push_back(Object);
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
	m_Objects.push_back(Object);
}

void GameMode::UnregisterObject(IObject* inObject)
{
	m_Objects.erase(
		std::remove_if(m_Objects.begin(), m_Objects.end(),
			[inObject](const std::shared_ptr<IObject>& ptr) 
			{
				return ptr.get() == inObject;
			}),
		m_Objects.end()
	);
}

void GameMode::DestroyObjectExplicitly(std::shared_ptr<IObject> InObject)
{
	m_Objects.erase(
		std::remove(m_Objects.begin(), m_Objects.end(), InObject),
		m_Objects.end()
	);

	InObject.reset();
}
