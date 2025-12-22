#pragma once
#include "raylib.h"
#include <typeindex>
#include <string>
#include "Base/SClass.hpp"
#include <memory>
#include <functional>	

class GameMode;
class Factory;
class EventDispatcher;

struct CallbackEntry
{
	std::weak_ptr<EventDispatcher> EventDispatcher;
	std::string Identifier;
	SClass* EventClass;
};	

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
	using EventCallback = std::function<void(std::shared_ptr<IEvent>)>;

	IObject() = default;
	~IObject();

	// Copy constructor - does NOT copy callbacks (new object has fresh callback state)
	IObject(const IObject& Other);

	// Copy assignment operator - does NOT copy callbacks, clears existing callbacks first
	IObject& operator=(const IObject& Other);

	// Move constructor - transfers callback ownership from source
	IObject(IObject&& Other) noexcept;

	// Move assignment operator - transfers callback ownership, clears existing callbacks first
	IObject& operator=(IObject&& Other) noexcept;

	virtual SClass* GetStaticClass() { return &m_SClass; };
	static SClass* StaticClass() { return  &m_SClass; };

	virtual void Tick(float DeltaTime) {};
	virtual void Initialize(){}; // TODO: Should be marked abstracted in future maybe?

	GameMode* GetOutter();

	virtual void MarkForDestruction();

	virtual bool IsMarkedForDestruction();


	virtual std::string GetName() const { return m_Name; };
	virtual std::string GetDisplayName() const { return m_DisplayName; };
	virtual void SetDisplayName(std::string NewName) { m_DisplayName = NewName; };

	// Maybe for later implementations
	// Right now i dont see the usecase sadly
	// My Thought was to have a easy cleanup method so no event dispatcher callbacks could leak
	// but in reality most of the objects subscribe to their own Dispatchers or Global ones, so they can't really leak
	//virtual void AddCallbackToEventDispatcher(std::weak_ptr<EventDispatcher> Dispatcher, const std::string& Identifier, SClass* EventClass, EventCallback Callback);

	//template <typename T>
	//void AddCallbackToEventDispatcherTemp(std::weak_ptr<EventDispatcher> Dispatcher, const std::string& Identifier, SClass* EventClass, T* Object, void (T::* MemberFunc)(std::shared_ptr<IEvent>))
	//{
	//	AddCallbackToEventDispatcher(Dispatcher, Identifier, EventClass, [Object, MemberFunc](std::shared_ptr<IEvent> Event)
	//		{
	//			(Object->*MemberFunc)(Event);
	//		});
	//}

protected:

	virtual void OnKeyStroke(KeyboardKey PressedKey, Vector2 MousePosition) {};
	virtual void OnMouseButtonPressed(MouseButton PressedKey, Vector2 MousePosition) {};


	std::string m_Name;
	std::string m_DisplayName = "Unit";

	std::vector<CallbackEntry> m_RegisteredCallbacks;

private:

	bool bIsMarkedForDestruction = false;



};


class Object : public IObject
{
	friend GameInstance;

private:
	static inline SClass m_SClass = SClass(IObject::StaticClass(), "Object");

};