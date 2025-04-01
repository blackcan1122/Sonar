#pragma once
#include <typeindex>

class GameMode;

class IObject
{
public:

	virtual std::type_index GetStaticClass() const { return typeid(*this); };
	std::type_index StaticClass() { return typeid(IObject); };

	virtual void Tick(float DeltaTime) = 0;

	GameMode* GetOutter();

	virtual void MarkForDestruction();

	virtual bool IsMarkedForDestruction();

private:

	bool bIsMarkedForDestruction = false;

};

class Object : public IObject
{

};