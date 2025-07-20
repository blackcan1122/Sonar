#pragma once
#include "Base/Core.h"


class Helper
{
	Helper();
	~Helper() = default;

public:

	static float LerpFloat(float& Float1, float& Float2, float Alpha, float AlphaMax, float AlphaMin);
	static Vector2 LerpVector(Vector2& VectorFirst, Vector2& VectorSecond, float Alpha, float AlphaMax, float AlphaMin);
	static Vector2 NormalizeVector2(Vector2& VectorToNormalize);
	static Vector2 Vector2MultiplyFloat(Vector2& Vector, float FloatValue);
	static bool Vector2IsInRange(Vector2& Vector, Vector2 Min, Vector2 Max);

};


template<typename DurationUnit = std::chrono::milliseconds>
class Timer 
{
public:
    // Constructor starts the timer
    explicit Timer(std::string_view name = "",
        std::function<void(DurationUnit, std::string_view)> on_destroy = nullptr)
        : m_start(std::chrono::steady_clock::now()),
        m_name(name),
        m_on_destroy(on_destroy) {}

    // Destructor stops the timer and prints to log
    ~Timer() 
    {
        auto end = std::chrono::steady_clock::now();
        DurationUnit duration = std::chrono::duration_cast<DurationUnit>(end - m_start);

        if (m_on_destroy) 
        {
            m_on_destroy(duration, m_name);
        }
        else 
        {
            LOG_INFO(l_DEFAULT, TEXT("Timer: {}, took: {} {}", m_name, duration.count(), unit_suffix()));
        }
    }

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

private:
    const char* unit_suffix() const 
    {
        if constexpr (std::is_same_v<DurationUnit, std::chrono::nanoseconds>) {
            return "ns";
        }
        else if constexpr (std::is_same_v<DurationUnit, std::chrono::microseconds>) {
            return "μs";
        }
        else if constexpr (std::is_same_v<DurationUnit, std::chrono::milliseconds>) {
            return "ms";
        }
        else if constexpr (std::is_same_v<DurationUnit, std::chrono::seconds>) {
            return "s";
        }
        else {
            return "units";
        }
    }

    std::chrono::steady_clock::time_point m_start;
    std::string_view m_name;
    std::function<void(DurationUnit, std::string_view)> m_on_destroy;
};