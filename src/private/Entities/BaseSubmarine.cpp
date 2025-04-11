#include "Entities/BaseSubmarine.hpp"


void BaseSubmarine::Tick(float DeltaTime)
{

    CalculateRotation(DeltaTime);
    Turning();
    CalculateSpeed(DeltaTime);
    Accel(DeltaTime);
}

void BaseSubmarine::SetSpeed(NavalUnits::Knot DesiredKnots)
{
    m_DesiredKnots = DesiredKnots;
}

void BaseSubmarine::SetInitialSpeed(NavalUnits::Knot DesiredKnots)
{
    m_DesiredKnots = DesiredKnots;
    m_CurrentKnots = DesiredKnots;
}

void BaseSubmarine::SetCourse(int Course)
{
    m_DesiredCourse = Course % 360;
}

void BaseSubmarine::CalculateSpeed(float Deltatime)
{
    SpeedChangeTimer += Deltatime;
    if (SpeedChangeTimer < SpeedChangeDelay)
        return;

    // Reset timer once enough time has passed.
    SpeedChangeTimer = 0.0f;

    float diff = m_DesiredKnots - m_CurrentKnots;

    if (std::fabs(diff) < DampeningRate)
    {
        // If we're very close to the target, snap to the target speed
        m_CurrentKnots = m_DesiredKnots;
    }
    else
    {
        // Determine the sign of acceleration: + if we need to speed up, - if we need to slow down.
        float acceleration = (diff > 0.0f) ? AccelerationRate : -AccelerationRate;

        // Update the current speed.
        m_CurrentKnots += acceleration * Deltatime;

        // Clamp so that we do not overshoot the target speed.
        if ((acceleration > 0.0f && m_CurrentKnots > m_DesiredKnots)
            || (acceleration < 0.0f && m_CurrentKnots < m_DesiredKnots))
        {
            m_CurrentKnots = m_DesiredKnots;
        }
    }
}

void BaseSubmarine::CalculateRotation(float Deltatime)
{
    RotationChangeTimer += Deltatime;
    if (RotationChangeTimer < SpeedChangeDelay)
        return;

    // Reset timer once enough time has passed.
    RotationChangeTimer = 0.0f;

    float diff = m_DesiredCourse - m_CurrentCourse;

    if (std::fabs(diff) < DampeningRate)
    {
        m_CurrentCourse = m_DesiredCourse;
    }
    else
    {
        float RealTurningRate = BaseTurningRate * NavalUnits::KnotToMetersPerSecond(m_CurrentKnots);
        float RotationAngle = (diff > std::fabs(diff - 360)) ? -RealTurningRate : RealTurningRate;

        // Update the current speed.
        m_CurrentCourse += RotationAngle * Deltatime;

    }
}

void BaseSubmarine::Accel(float Deltatime)
{
    // Convert knots to meters per second.
    float speedMPS = NavalUnits::KnotToMetersPerSecond(m_CurrentKnots);

    // Calculate displacement vector (speed * deltaTime gives movement in meters for this frame)
    Vector2 displacement = Vector2Scale(GetFacingVector(), speedMPS * Deltatime);

    // Update the submarine's position.
    SetEntityLocation(Vector2Add(GetEntityLocation(), displacement));
}

void BaseSubmarine::Turning()
{
    SetEntityRotation(m_CurrentCourse);
    ConvertAngleToVector();
}
