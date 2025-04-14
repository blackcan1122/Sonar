#pragma once
#include <typeindex>
#include <string>

class GameMode;
class Factory;

/**
 * @class IObject
 * @brief Base interface for all objects managed by the game.
 *
 * Purpose:
 * - Defines the interface for objects that can be ticked and managed.
 * - Provides lifecycle management (e.g., destruction marking).
 *
 * Usage:
 * - Inherit from `IObject` to define game-specific objects.
 * - Implement the `Tick(float DeltaTime)` method for frame updates.
 *
 * Notes for Derived Classes:
 * - Use `MarkForDestruction` to safely queue the object for cleanup.
 * - Override `GetName()` and `GetDisplayName()` as needed.
 */
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