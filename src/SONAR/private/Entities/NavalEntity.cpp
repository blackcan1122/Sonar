#include "SONAR/NavalUnits.hpp"
#include "Entities/NavalEntity.hpp"

#include "Base/Logging.h"
#include "Rendering/Graphics.hpp"

void NavalEntity::MoveEntityToPosition(Sonar::Vector2 TargetPosition)
{
	Sonar::Vector2 Delta = Sonar::Vector2Subtract(TargetPosition, m_Position);
	double AngleRadians = std::atan2(Delta.y, Delta.x);
	double AngleDeg = AngleRadians * (180.0f / Sonar::PI);

	AngleDeg += 90.0f;
	AngleDeg = std::fmod(AngleDeg, 360.0f);
	if (AngleDeg < 0.0f)
	{
		AngleDeg += 360.0f;
	}

	SetCourse(static_cast<int>(AngleDeg) % 360);
}

void NavalEntity::SetSpeed(NavalUnits::Knot DesiredKnots)
{
	#if DEBUG
	LOG_INFO(l_GAMEMODE, SONAR_FMT("Setting Desired Speed to {}", static_cast<int>(DesiredKnots)));
	#endif
	m_DesiredKnots = DesiredKnots;
}

void NavalEntity::SetInitialSpeed(NavalUnits::Knot DesiredKnots)
{
	#if DEBUG
	LOG_INFO(l_GAMEMODE, SONAR_FMT("Setting Initial Speed to {}", static_cast<int>(DesiredKnots)));
	#endif
	m_DesiredKnots = DesiredKnots;
	m_CurrentKnots = DesiredKnots;
}

void NavalEntity::SetCourse(int Course)
{
	#if DEBUG
	LOG_INFO(l_GAMEMODE, SONAR_FMT("Setting Course To: {}", Course));
	#endif
	m_DesiredCourse = Course % 360;
}