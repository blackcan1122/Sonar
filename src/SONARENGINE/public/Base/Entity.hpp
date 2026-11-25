#pragma once
#include "Base/Object.hpp"

#include <cmath>

/*
	Base class for all entities in the game world.
	TODO:
	need to parse JSON / XML data to populate entity attributes like:
	- Model/Texture
	- Speed
	- Armament
	- Sensors
	- Crew
	- etc.
*/
class SONARENGINE_API Entity : public Object
{
	AUTOBODY(Entity, Object)

public:

	virtual void Initialize() override;

	void SetEntityLocation(Sonar::Vector2 NewLocation) { m_Position = NewLocation; };
	void SetEntityVelocity(Sonar::Vector2 NewVelocity) { m_Velocity = NewVelocity; };
	void SetEntityRotation(float Angle)
	{ 
		m_Rotation = std::fmod(Angle, 360.f); 
	};


	virtual void ConvertAngleToVector();
	Sonar::Vector2 GetFacingVector() const { return m_FacingVector; };
	Sonar::Vector2 GetEntityLocation() const { return m_Position; };
	float GetEntityRotation() const { return m_Rotation; };

protected:

	Sonar::Vector2 m_Position = { 0,0 };

	Sonar::Vector2 m_Velocity = { 0,0 };
	Sonar::Vector2 m_FacingVector = { 0, -1 };
	float m_Rotation = 0.f;
	float m_Scale = 1.f;

	bool m_IsActive = true;
	bool m_IsVisible = true;
	bool m_CollisionEnabled = false;

	Sonar::Texture2D Texture; // don't know about this, since we should need a way to cicyle through a tileset maybe

};