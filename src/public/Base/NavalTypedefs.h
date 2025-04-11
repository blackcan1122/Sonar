#pragma once


namespace NavalUnits
{
    constexpr float METERS_PER_NAUTICAL_MILE = 1852.0f;
    constexpr float METERS_PER_KNOT = 0.514444f;

    using NauticalMile = float;
    using Knot = float;

    inline float NauticalMilesToMeters(NauticalMile nm)
    {
        return nm * 1852.0f;
    }

    constexpr inline  float KnotToMetersPerSecond(Knot k)
    {
        return k * 0.514444f;
    }
}

