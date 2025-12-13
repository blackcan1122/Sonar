#pragma once
#include "raylib.h"
#include <typeindex>
#include <string>
#include "Base/SClass.hpp"
#include <memory>

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

class IEvent;
class GameInstance;

class IObject
{
	friend class Factory;
private:
	static inline SClass m_SClass = SClass(nullptr, "IObject");

public:

	IObject() = default;

	virtual SClass* GetStaticClass() { return &m_SClass; };
	static SClass* StaticClass() { return  &m_SClass; };

	virtual void Tick(float DeltaTime) = 0;
	virtual void Initialize(){}; // TODO: Should be marked abstracted in future maybe?

	GameMode* GetOutter();

	virtual void MarkForDestruction();

	virtual bool IsMarkedForDestruction();


	virtual std::string GetName() const { return m_Name; };
	virtual std::string GetDisplayName() const { return m_DisplayName; };
	virtual void SetDisplayName(std::string NewName) { m_DisplayName = NewName; };

protected:

	virtual void OnKeyStroke(KeyboardKey PressedKey, Vector2 MousePosition) {};
	virtual void OnMouseButtonPressed(MouseButton PressedKey, Vector2 MousePosition) {};


	std::string m_Name;
	std::string m_DisplayName = "Unit";

private:

	bool bIsMarkedForDestruction = false;



};


class Object : public IObject
{
	friend GameInstance;

private:
	static inline SClass m_SClass = SClass(IObject::StaticClass(), "Object");
public:
	Object();



	
	virtual SClass* GetStaticClass() override { return &m_SClass; }
	static SClass* StaticClass() { return  &m_SClass; }
private:

protected:
};