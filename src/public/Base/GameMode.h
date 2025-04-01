#pragma once
#include "Base/Core.h"
#include "Base/Factory.hpp"



class GameMode
{
friend IObject;
friend Factory;
public:
	GameMode();
	virtual ~GameMode() = default;

	virtual void Update();
	virtual void SetName(std::string Name);
	virtual std::string GetName();
	Factory ObjectFactory = Factory(this);

	virtual void DestroyObjectExplicitly(std::shared_ptr<IObject> InObject);


protected:

	virtual void CollectPendingDestruction();
	virtual void CleanUpPendingKill();
	virtual void RegisterObject(std::shared_ptr<IObject> InObject);
	virtual void UnregisterObject(IObject* InObject);
	std::vector<std::shared_ptr<IObject>> m_Objects;
	std::vector<std::shared_ptr<IObject>> m_PendingKill;
	float m_DeltaTime = 0;
	std::string m_Name;

};