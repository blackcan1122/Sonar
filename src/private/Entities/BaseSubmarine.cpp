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
    std::cout << "Course is now: " << Course << std::endl;
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

    RotationChangeTimer = 0.0f;

    // Compute smallest signed difference in [–180°, +180°]
    float diff = m_DesiredCourse - m_CurrentCourse;
    // wrap into –180..+180
    if (diff > 180.0f)  diff -= 360.0f;
    if (diff < -180.0f) diff += 360.0f;

    //std::cout << "Heading error: " << diff << "°\n";

    // If we're "close enough", just snap on target
    if (std::fabs(diff) < DampeningRate)
    {
        m_CurrentCourse = m_DesiredCourse;
        return;
    }

    // Base turning speed (m/s → rad/s or deg/s as you prefer)
    float RealTurningRate = BaseTurningRate * NavalUnits::KnotToMetersPerSecond(m_CurrentKnots);
    // Now scale it down the closer we are:
    // Choose a full-speed dead‑zone, e.g. 90°, beyond which you turn at full rate:
    constexpr float fullSpeedZone = 15.0f;
    float factor = std::clamp(std::fabs(diff) / fullSpeedZone, 0.0f, 1.0f);

    // Apply dampening factor
    float scaledTurnRate = RealTurningRate * factor;

    // Determine direction: + for CCW increase, – for CW decrease
    float turnDirection = (diff > 0 ? +1.0f : -1.0f);

    // Finally update the current course
    m_CurrentCourse += turnDirection * scaledTurnRate * Deltatime;

    // Optional: wrap m_CurrentCourse back into [0,360)
    if (m_CurrentCourse >= 360.0f) m_CurrentCourse -= 360.0f;
    if (m_CurrentCourse < 0.0f)   m_CurrentCourse += 360.0f;
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
