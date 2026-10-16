#pragma once

#include "Base/NavalTypedefs.h"
#include "Base/Entity.hpp"

class NavalEntity : public Entity
{
	AUTOBODY(NavalEntity, Entity)

public:
	void SetEntityRotationAndCourse(float Angle)
	{
		m_Rotation = std::fmod(Angle, 360.f);
		m_DesiredCourse = m_Rotation;
		m_CurrentCourse = m_Rotation;
	}

	NavalUnits::Knot GetCurrentSpeed() const { return m_CurrentKnots; }

	void MoveEntityToPosition(Vector2 TargetPosition);

	virtual void SetSpeed(NavalUnits::Knot DesiredKnots);
	virtual void SetInitialSpeed(NavalUnits::Knot DesiredKnots);
	virtual void SetCourse(int Course);

	void SetAccelerationRate(float NewAccelRate) { m_AccelerationRate = NewAccelRate; }
	void SetDampeningRate(float NewDampeningRate) { m_DampeningRate = NewDampeningRate; }
	void SetBaseTurningRate(float NewBaseTurningRate) { m_BaseTurningRate = NewBaseTurningRate; }
	void SetSpeedChangeDelay(float NewSpeedChangeDelay) { m_SpeedChangeDelay = NewSpeedChangeDelay; }
	void SetSpeedChangeTimer(float NewSpeedChangeTimer) { m_SpeedChangeTimer = NewSpeedChangeTimer; }

protected:
	NavalUnits::Knot m_CurrentKnots = 0.f;
	NavalUnits::Knot m_DesiredKnots = 0.f;
	float m_DesiredCourse = 0.f;
	float m_CurrentCourse = 0.f;

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
};