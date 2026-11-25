#pragma once

#define KiloMeter *1000

namespace NavalUnits
{
    constexpr float METERS_PER_NAUTICAL_MILE = 1852.0f;
    constexpr float METERS_PER_KNOT = 0.514444f;

    using NauticalMile = float;
    using Knot = float;
    using Hz = double;

    // Sound frequency units
    constexpr Hz KHz = 1000 * 1;
    constexpr Hz MHz = 1000 * KHz;

    inline float NauticalMilesToMeters(NauticalMile nm)
    {
        return nm * 1852.0f;
    }

    constexpr inline float KnotToMetersPerSecond(Knot k)
    {
        return k * 0.514444f;
    }
}
