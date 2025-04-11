#pragma once
#include <typeindex>
#include <string>

class GameMode;
class Factory;

class IObject
{
	friend class Factory;
public:

	IObject() = default;

	virtual std::type_index GetStaticClass() const { return typeid(*this); };
	std::type_index StaticClass() { return typeid(IObject); };

	virtual void Tick(float DeltaTime) = 0;

	GameMode* GetOutter();

	virtual void MarkForDestruction();

	virtual bool IsMarkedForDestruction();

	virtual std::string GetName() const { return m_Name; };
	virtual std::string GetDisplayName() const { return m_DisplayName; };
	virtual void SetDisplayName(std::string NewName) { m_DisplayName = NewName; };

protected:

	std::string m_Name;
	std::string m_DisplayName = "Unit";

private:

	bool bIsMarkedForDestruction = false;



};

class Object : public IObject
{

};