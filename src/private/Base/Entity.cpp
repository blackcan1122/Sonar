#include "Base/Entity.hpp"

void Entity::ConvertAngleToVector()
{
	// Rotate around 90, to accomodate for math radians
	float RadianAngle = (m_Rotation + 90.f) * (PI / 180.f);

	m_FacingVector.x = -cos(RadianAngle);
	m_FacingVector.y = -sin(RadianAngle);

	m_FacingVector = Vector2Normalize(m_FacingVector);
}

void Entity::MoveEntityToPosition(Vector2 TargetPosition)
{
	Vector2 Delta = TargetPosition - m_Position;
	double AngleRadians = std::atan2(Delta.y, Delta.x);
	double AngleDeg = AngleRadians * (180.0f / PI);

	AngleDeg += 90.0f;
	AngleDeg = std::fmod(AngleDeg, 360.0f);
	if (AngleDeg < 0.0f)
	{
		AngleDeg += 360.0f;
	}

	this->SetCourse(((int)AngleDeg % 360));
}
void Entity::SetSpeed(NavalUnits::Knot DesiredKnots)
{
	#if DEBUG
	LOG_INFO(l_GAMEMODE, TEXT("Setting Desired Speed to {}", static_cast<int>(DesiredKnots)));
	#endif
	m_DesiredKnots = DesiredKnots;
}

void Entity::SetInitialSpeed(NavalUnits::Knot DesiredKnots)
{
	#if DEBUG
	LOG_INFO(l_GAMEMODE, TEXT("Setting Initial Speed to {}", static_cast<int>(DesiredKnots)));
	#endif
	m_DesiredKnots = DesiredKnots;
    m_CurrentKnots = DesiredKnots;
}

void Entity::SetCourse(int Course)
{
	#if DEBUG
	LOG_INFO(l_GAMEMODE, TEXT("Setting Course To: {}", Course));
	#endif
	m_DesiredCourse = Course % 360;
}
