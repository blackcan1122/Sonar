#pragma once
#include "raylib.h"

#undef LIGHTGRAY
#undef GRAY
#undef DARKGRAY
#undef YELLOW
#undef GOLD
#undef ORANGE
#undef PINK
#undef RED
#undef MAROON
#undef GREEN
#undef LIME
#undef DARKGREEN
#undef SKYBLUE
#undef BLUE
#undef DARKBLUE
#undef PURPLE
#undef VIOLET
#undef DARKPURPLE
#undef BEIGE
#undef BROWN
#undef DARKBROWN
#undef WHITE
#undef BLACK
#undef BLANK
#undef MAGENTA
#undef RAYWHITE
#undef PI
#undef DEG2RAD
#undef RAD2DEG
#undef CloseWindow
#undef ShowCursor
#undef PlaySound
#include "Base/SonarTypes.hpp"

namespace Sonar::Backend {

::Vector2 ToRaylib(const Sonar::Vector2& v) noexcept;
Sonar::Vector2 FromRaylib(const ::Vector2& v) noexcept;
::Vector3 ToRaylib(const Sonar::Vector3& v) noexcept;
Sonar::Vector3 FromRaylib(const ::Vector3& v) noexcept;
::Vector4 ToRaylib(const Sonar::Vector4& v) noexcept;
Sonar::Vector4 FromRaylib(const ::Vector4& v) noexcept;
::Matrix ToRaylib(const Sonar::Matrix& v) noexcept;
Sonar::Matrix FromRaylib(const ::Matrix& v) noexcept;
::Color ToRaylib(const Sonar::Color& v) noexcept;
Sonar::Color FromRaylib(const ::Color& v) noexcept;
::Rectangle ToRaylib(const Sonar::Rectangle& v) noexcept;
Sonar::Rectangle FromRaylib(const ::Rectangle& v) noexcept;
::NPatchInfo ToRaylib(const Sonar::NPatchInfo& v) noexcept;
Sonar::NPatchInfo FromRaylib(const ::NPatchInfo& v) noexcept;
::Texture2D ToRaylib(const Sonar::Texture2D& v) noexcept;
Sonar::Texture2D FromRaylib(const ::Texture2D& v) noexcept;
::Shader ToRaylib(const Sonar::Shader& v) noexcept;
Sonar::Shader FromRaylib(const ::Shader& v) noexcept;
::AudioStream ToRaylib(const Sonar::AudioStream& v) noexcept;
Sonar::AudioStream FromRaylib(const ::AudioStream& v) noexcept;
::Music ToRaylib(const Sonar::Music& v) noexcept;
Sonar::Music FromRaylib(const ::Music& v) noexcept;
::RenderTexture2D ToRaylib(const Sonar::RenderTexture2D& v) noexcept;
Sonar::RenderTexture2D FromRaylib(const ::RenderTexture2D& v) noexcept;
::Image ToRaylib(const Sonar::Image& v) noexcept;
Sonar::Image FromRaylib(const ::Image& v) noexcept;

}
