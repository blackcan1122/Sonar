#pragma once
#include "Base/Core.h"

DECLARE_CLASS(Entity, Object)

public:
	void SetEntityLocation(Vector2 NewLocation) { m_Position = NewLocation; };
	void SetEntityVelocity(Vector2 NewVelocity) { m_Velocity = NewVelocity; };
	void SetEntityRotation(int Angle) { m_Rotation = Angle % 360; };
	// TODO More

	virtual void ConvertAngleToVector();
	Vector2 GetFacingVector() const { return m_FacingVector; };
	Vector2 GetEntityLocation() const { return m_Position; };
	float GetEntityRotation() const { return m_Rotation; };
	NavalUnits::Knot GetCurrentSpeed() const { return m_CurrentKnots; };

protected:
	Vector2 m_Position = { 0,0 };
	Vector2 m_Velocity = { 0,0 };
	NavalUnits::Knot m_CurrentKnots;
	NavalUnits::Knot m_DesiredKnots;
	Vector2 m_FacingVector = { 0, -1 };
	float m_Rotation = 0.f;
	float m_Scale = 1.f;

	bool m_IsActive = true;
	bool m_IsVisible = true;
	bool m_CollisionEnabled = false;

	Texture2D Texture; // don't know about this, since we should need a way to cicyle through a tileset maybe

END_CLASS