#include "Base/SonarTypeConversions.hpp"

namespace Sonar::Backend {

::Vector2 ToRaylib(const Sonar::Vector2& v) noexcept
{
    return { v.x, v.y };
}

Sonar::Vector2 FromRaylib(const ::Vector2& v) noexcept
{
    return { v.x, v.y };
}

::Vector3 ToRaylib(const Sonar::Vector3& v) noexcept
{
    return { v.x, v.y, v.z };
}

Sonar::Vector3 FromRaylib(const ::Vector3& v) noexcept
{
    return { v.x, v.y, v.z };
}

::Vector4 ToRaylib(const Sonar::Vector4& v) noexcept
{
    return { v.x, v.y, v.z, v.w };
}

Sonar::Vector4 FromRaylib(const ::Vector4& v) noexcept
{
    return { v.x, v.y, v.z, v.w };
}

::Matrix ToRaylib(const Sonar::Matrix& v) noexcept
{
    return { v.m0, v.m4, v.m8, v.m12, v.m1, v.m5, v.m9, v.m13, v.m2, v.m6, v.m10, v.m14, v.m3, v.m7, v.m11, v.m15 };
}

Sonar::Matrix FromRaylib(const ::Matrix& v) noexcept
{
    return { v.m0, v.m4, v.m8, v.m12, v.m1, v.m5, v.m9, v.m13, v.m2, v.m6, v.m10, v.m14, v.m3, v.m7, v.m11, v.m15 };
}

::Color ToRaylib(const Sonar::Color& v) noexcept
{
    return { v.r, v.g, v.b, v.a };
}

Sonar::Color FromRaylib(const ::Color& v) noexcept
{
    return { v.r, v.g, v.b, v.a };
}

::Rectangle ToRaylib(const Sonar::Rectangle& v) noexcept
{
    return { v.x, v.y, v.width, v.height };
}

Sonar::Rectangle FromRaylib(const ::Rectangle& v) noexcept
{
    return { v.x, v.y, v.width, v.height };
}

::NPatchInfo ToRaylib(const Sonar::NPatchInfo& v) noexcept
{
    return { ToRaylib(v.source), v.left, v.top, v.right, v.bottom, v.layout };
}

Sonar::NPatchInfo FromRaylib(const ::NPatchInfo& v) noexcept
{
    return { FromRaylib(v.source), v.left, v.top, v.right, v.bottom, v.layout };
}

::Texture2D ToRaylib(const Sonar::Texture2D& v) noexcept
{
    return { v.id, v.width, v.height, v.mipmaps, v.format };
}

Sonar::Texture2D FromRaylib(const ::Texture2D& v) noexcept
{
    return { v.id, v.width, v.height, v.mipmaps, v.format };
}

::Shader ToRaylib(const Sonar::Shader& v) noexcept
{
    return { v.id, v.locs };
}

Sonar::Shader FromRaylib(const ::Shader& v) noexcept
{
    return { v.id, v.locs };
}

::AudioStream ToRaylib(const Sonar::AudioStream& v) noexcept
{
    return { static_cast<::rAudioBuffer*>(v.buffer), static_cast<::rAudioProcessor*>(v.processor), v.sampleRate, v.sampleSize, v.channels };
}

Sonar::AudioStream FromRaylib(const ::AudioStream& v) noexcept
{
    return { v.buffer, v.processor, v.sampleRate, v.sampleSize, v.channels };
}

::Music ToRaylib(const Sonar::Music& v) noexcept
{
    return { ToRaylib(v.stream), v.frameCount, v.looping, v.ctxType, v.ctxData };
}

Sonar::Music FromRaylib(const ::Music& v) noexcept
{
    return { FromRaylib(v.stream), v.frameCount, v.looping, v.ctxType, v.ctxData };
}

::RenderTexture2D ToRaylib(const Sonar::RenderTexture2D& v) noexcept
{
    return { v.id, ToRaylib(v.texture), ToRaylib(v.depth) };
}

Sonar::RenderTexture2D FromRaylib(const ::RenderTexture2D& v) noexcept
{
    return { v.id, FromRaylib(v.texture), FromRaylib(v.depth) };
}

::Image ToRaylib(const Sonar::Image& v) noexcept
{
    return { v.data, v.width, v.height, v.mipmaps, v.format };
}

Sonar::Image FromRaylib(const ::Image& v) noexcept
{
    return { v.data, v.width, v.height, v.mipmaps, v.format };
}

}
