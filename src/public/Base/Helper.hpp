#pragma once
#include "Base/Core.h"




static class Helper
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