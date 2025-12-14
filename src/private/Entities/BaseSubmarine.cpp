#include "Entities/BaseSubmarine.hpp"
#include "Base/EventDispatcher.hpp"
#include "Events/SoundEvent.hpp"


void BaseSubmarine::Tick(float DeltaTime)
{

    CalculateRotation(DeltaTime);
    Turning();
    CalculateSpeed(DeltaTime);
    Accel(DeltaTime);
    if (m_CurrentKnots != 0)
    {
        std::shared_ptr<SoundEvent> CurrentSoundEvent = std::make_shared<SoundEvent>();
        CurrentSoundEvent->Sender = SoftObjectPath<BaseSubmarine>(this->GetName());
        CurrentSoundEvent->SignalStrength = 20;
        CurrentSoundEvent->SoundOrigin = m_Position;
        SoundDispatcher->Dispatch(CurrentSoundEvent, "Sound Event");
    }
}

void BaseSubmarine::CalculateSpeed(float Deltatime)
{
    m_SpeedChangeTimer += Deltatime;
    if (m_SpeedChangeTimer < m_SpeedChangeDelay)
        return;

    // Reset timer once enough time has passed.
    m_SpeedChangeTimer = 0.0f;

    float diff = m_DesiredKnots - m_CurrentKnots;

    if (std::fabs(diff) < m_DampeningRate)
    {
        // If we're very close to the target, snap to the target speed
        m_CurrentKnots = m_DesiredKnots;
    }
    else
    {
        // Determine the sign of acceleration: + if we need to speed up, - if we need to slow down.
        float acceleration = (diff > 0.0f) ? m_AccelerationRate : -m_AccelerationRate;

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
    m_RotationChangeTimer += Deltatime;
    if (m_RotationChangeTimer < m_SpeedChangeDelay)
        return;

    m_RotationChangeTimer = 0.0f;

    // Compute smallest signed difference in [–180°, +180°]
    float diff = m_DesiredCourse - m_CurrentCourse;
    // wrap into –180..+180
    if (diff > 180.0f)  diff -= 360.0f;
    if (diff < -180.0f) diff += 360.0f;

    //std::cout << "Heading error: " << diff << "°\n";

    // If we're "close enough", just snap on target
    if (std::fabs(diff) < m_DampeningRate)
    {
        m_CurrentCourse = m_DesiredCourse;
        return;
    }

    // Base turning speed (m/s → rad/s or deg/s as you prefer)
    float RealTurningRate = m_BaseTurningRate * NavalUnits::KnotToMetersPerSecond(m_CurrentKnots);

    float factor = std::clamp(std::fabs(diff) / m_FullSpeedZone, 0.0f, 1.0f);

    // Apply dampening factor
    float scaledTurnRate = RealTurningRate * factor;

    // Determine direction: + for CCW increase, – for CW decrease
    float turnDirection = (diff > 0 ? +1.0f : -1.0f);

    // Finally update the current course
    m_CurrentCourse += turnDirection * scaledTurnRate * Deltatime;

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
