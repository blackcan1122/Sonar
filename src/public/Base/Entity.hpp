#pragma once
#include "Base/Core.h"

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
class Entity : public Object
{
	AUTOBODY(Entity, Object)

public:
	void SetEntityLocation(Vector2 NewLocation) { m_Position = NewLocation; };
	void SetEntityVelocity(Vector2 NewVelocity) { m_Velocity = NewVelocity; };
	void SetEntityRotation(float Angle) { m_Rotation = std::fmod(Angle, 360.f); };

	virtual void ConvertAngleToVector();
	Vector2 GetFacingVector() const { return m_FacingVector; };
	Vector2 GetEntityLocation() const { return m_Position; };
	float GetEntityRotation() const { return m_Rotation; };
	NavalUnits::Knot GetCurrentSpeed() const { return m_CurrentKnots; };

	void MoveEntityToPosition(Vector2 TargetPosition);


	void SetAccelerationRate(float NewAccelRate) { m_AccelerationRate = NewAccelRate; }
	void SetDampeningRate(float NewDampeningRate) { m_DampeningRate = NewDampeningRate; }

	void SetBaseTurningRate(float NewBaseTurningRate) { m_BaseTurningRate = NewBaseTurningRate; }
	void SetSpeedChangeDelay(float NewSpeedChangeDelay) { m_SpeedChangeDelay = NewSpeedChangeDelay; }
	void SetSpeedChangeTimer(float NewSpeedChangeTimer) { m_SpeedChangeTimer = NewSpeedChangeTimer; }

protected:

	Vector2 m_Position = { 0,0 };
	Vector2 m_Velocity = { 0,0 };
	NavalUnits::Knot m_CurrentKnots = 0.f;
	NavalUnits::Knot m_DesiredKnots = 0.f;
	float m_DesiredCourse = 0.f;
	float m_CurrentCourse = 0.f;
	Vector2 m_FacingVector = { 0, -1 };
	float m_Rotation = 0.f;
	float m_Scale = 1.f;

	bool m_IsActive = true;
	bool m_IsVisible = true;
	bool m_CollisionEnabled = false;

	virtual void CalculateSpeed(float Deltatime) = 0;
	virtual void CalculateRotation(float Deltatime) = 0;

	virtual void Accel(float Deltatime) = 0;
	virtual void Turning() = 0;

	float m_AccelerationRate = 2.f;
	float m_DampeningRate = 0.01f;

	float m_SpeedChangeDelay = 0.2f;
	float m_SpeedChangeTimer = 0.0f;
	float m_RotationChangeTimer = 0.0f;

	float m_FullSpeedZone = 45.0f;

	float m_BaseTurningRate = 6.f;



	Texture2D Texture; // don't know about this, since we should need a way to cicyle through a tileset maybe

};