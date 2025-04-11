#include "Entities/BaseSubmarine.hpp"


void BaseSubmarine::Tick(float DeltaTime)
{
    // Optional: Use a timer delay to control how often speed updates occur.
    SpeedChangeTimer += DeltaTime;
    if (SpeedChangeTimer < SpeedChangeDelay)
        return;

    // Reset timer once enough time has passed.
    SpeedChangeTimer = 0.0f;

    float diff = m_DesiredKnots - m_CurrentKnots;

    if (std::fabs(diff) < DampeningRate) 
    {
        // If we're very close to the target, snap to the target speed
        m_CurrentKnots = m_DesiredKnots;
        std::cout << m_CurrentKnots << std::endl;
    }
    else 
    {
        // Determine the sign of acceleration: + if we need to speed up, - if we need to slow down.
        float acceleration = (diff > 0.0f) ? AccelerationRate : -AccelerationRate;

        // Update the current speed.
        m_CurrentKnots += acceleration * DeltaTime;

        std::cout << m_CurrentKnots << std::endl;

        // Clamp so that we do not overshoot the target speed.
        if ((acceleration > 0.0f && m_CurrentKnots > m_DesiredKnots) 
            || (acceleration < 0.0f && m_CurrentKnots < m_DesiredKnots))
        {
            m_CurrentKnots = m_DesiredKnots;
        }
    }

    Accel();
}

void BaseSubmarine::SetSpeed(NavalUnits::Knot DesiredKnots)
{
    m_DesiredKnots = DesiredKnots;
}

void BaseSubmarine::Accel()
{
    SetEntityLocation(Vector2Add(GetEntityLocation(), Vector2Scale(GetFacingVector(), NavalUnits::KnotToMetersPerSecond(m_CurrentKnots))));
}
