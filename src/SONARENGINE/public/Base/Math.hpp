#pragma once

#include "Base/SonarTypes.hpp"

namespace Sonar {

SONARENGINE_API bool CheckCollisionPointRec(Vector2 point, Rectangle rec);
SONARENGINE_API bool CheckCollisionPointCircle(Vector2 point, Vector2 center, float radius);
SONARENGINE_API float Clamp(float value, float min, float max);
SONARENGINE_API Vector2 Vector2Add(Vector2 v1, Vector2 v2);
SONARENGINE_API Vector2 Vector2Subtract(Vector2 v1, Vector2 v2);
SONARENGINE_API Vector2 Vector2Scale(Vector2 v, float scale);
SONARENGINE_API Vector2 Vector2Normalize(Vector2 v);
SONARENGINE_API Vector2 Vector2Clamp(Vector2 v, Vector2 min, Vector2 max);
SONARENGINE_API Matrix MatrixMultiply(Matrix left, Matrix right);
SONARENGINE_API Matrix MatrixTranslate(float x, float y, float z);
SONARENGINE_API Matrix MatrixScale(float x, float y, float z);

}
