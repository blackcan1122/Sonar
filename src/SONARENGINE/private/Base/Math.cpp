#include "raylib.h"
#include "raymath.h"
#include "Base/SonarTypeConversions.hpp"
#include "Base/Math.hpp"

namespace Sonar {

using Backend::ToRaylib;
using Backend::FromRaylib;

bool CheckCollisionPointRec(Vector2 point, Rectangle rec) { auto result=::CheckCollisionPointRec(ToRaylib(point),ToRaylib(rec)); return result; }

bool CheckCollisionPointCircle(Vector2 point, Vector2 center, float radius) { auto result=::CheckCollisionPointCircle(ToRaylib(point),ToRaylib(center),radius); return result; }

float Clamp(float value, float min, float max) { auto result=::Clamp(value,min,max); return result; }

Vector2 Vector2Add(Vector2 v1, Vector2 v2) { auto result=FromRaylib(::Vector2Add(ToRaylib(v1),ToRaylib(v2))); return result; }

Vector2 Vector2Subtract(Vector2 v1, Vector2 v2) { auto result=FromRaylib(::Vector2Subtract(ToRaylib(v1),ToRaylib(v2))); return result; }

Vector2 Vector2Scale(Vector2 v, float scale) { auto result=FromRaylib(::Vector2Scale(ToRaylib(v),scale)); return result; }

Vector2 Vector2Normalize(Vector2 v) { auto result=FromRaylib(::Vector2Normalize(ToRaylib(v))); return result; }

Vector2 Vector2Clamp(Vector2 v, Vector2 min, Vector2 max) { auto result=FromRaylib(::Vector2Clamp(ToRaylib(v),ToRaylib(min),ToRaylib(max))); return result; }

Matrix MatrixMultiply(Matrix left, Matrix right) { auto result=FromRaylib(::MatrixMultiply(ToRaylib(left),ToRaylib(right))); return result; }

Matrix MatrixTranslate(float x, float y, float z) { auto result=FromRaylib(::MatrixTranslate(x,y,z)); return result; }

Matrix MatrixScale(float x, float y, float z) { auto result=FromRaylib(::MatrixScale(x,y,z)); return result; }

}
