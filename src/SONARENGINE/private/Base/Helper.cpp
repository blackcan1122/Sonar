#include "Base/Helper.hpp"

float Helper::LerpFloat(float& Float1, float& Float2, float Alpha, float AlphaMax, float AlphaMin)
{
	// Normalize Alpha
	float NormalizedAlpha;

	NormalizedAlpha = (Alpha - AlphaMin) / (AlphaMax - AlphaMin);

	NormalizedAlpha = std::clamp(NormalizedAlpha, 0.0f, 1.0f);

	return (1 - NormalizedAlpha) * Float1 + NormalizedAlpha * Float2;

}

Sonar::Vector2 Helper::LerpVector(Sonar::Vector2& VectorFirst, Sonar::Vector2& VectorSecond, float Alpha, float AlphaMax, float AlphaMin)
{

	// Normalize Alpha
	float NormalizedAlpha;
	NormalizedAlpha = (Alpha - AlphaMin) / (AlphaMax - AlphaMin);

	// Clamp NormalizedAlpha between 0 and 1
	NormalizedAlpha = std::clamp(NormalizedAlpha, 0.0f, 1.0f);

	// Interpolate between VectorFirst and VectorSecond
	Sonar::Vector2 result;
	result.x = (1 - NormalizedAlpha) * VectorFirst.x + NormalizedAlpha * VectorSecond.x;
	result.y = (1 - NormalizedAlpha) * VectorFirst.y + NormalizedAlpha * VectorSecond.y;
	return result;

}

Sonar::Vector2 Helper::NormalizeVector2(Sonar::Vector2& VectorToNormalize)
{
	Sonar::Vector2 ReturnVector = VectorToNormalize;
	float directionMagnitude = std::sqrt((VectorToNormalize.x * VectorToNormalize.x) + (VectorToNormalize.y * VectorToNormalize.y));
	if (directionMagnitude > 0)
	{
		ReturnVector.x /= directionMagnitude;
		ReturnVector.y /= directionMagnitude;
	}
	return ReturnVector;
}

Sonar::Vector2 Helper::Vector2MultiplyFloat(Sonar::Vector2& Vector, float FloatValue)
{
	Sonar::Vector2 ReturnVector;
	ReturnVector.x = Vector.x * FloatValue;
	ReturnVector.y = Vector.y * FloatValue;
	return ReturnVector;
}

bool Helper::Vector2IsInRange(Sonar::Vector2& Vector, Sonar::Vector2 Min, Sonar::Vector2 Max)
{
	if (Vector.x >= Min.x && Vector.x <= Max.x)
	{
		if (Vector.y >= Min.y && Vector.y <= Max.y)
		{
			return true;
		}
	}
	return false;
}
