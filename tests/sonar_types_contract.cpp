#include "Base/SonarTypeConversions.hpp"
#include <cassert>
#include <type_traits>
using Sonar::Backend::ToRaylib;
using Sonar::Backend::FromRaylib;

int main() {
    static_assert(std::is_trivially_copyable_v<Sonar::Image>);
    static_assert(std::is_trivially_copyable_v<Sonar::Texture2D>);
    static_assert(std::is_trivially_copyable_v<Sonar::Music>);
    int storage[4]{};
    ::Texture2D texture{17, 23, 31, 4, 7};
    auto t = ToRaylib(FromRaylib(texture));
    assert(t.id == 17 && t.width == 23 && t.height == 31 && t.mipmaps == 4 && t.format == 7);
    ::Image image{storage, 2, 2, 3, 7};
    auto descriptor = FromRaylib(image);
    auto i = ToRaylib(descriptor);
    assert(i.data == storage && i.width == 2 && i.height == 2 && i.mipmaps == 3 && i.format == 7);
    { auto copy = descriptor; assert(copy.data == descriptor.data); }
    assert(descriptor.data == storage); // Copy destruction does not free the pixels.
    ::Shader shader{19, storage};
    auto s = ToRaylib(FromRaylib(shader));
    assert(s.id == 19 && s.locs == storage);
    ::RenderTexture2D target{29, texture, {37, 41, 43, 2, 19}};
    auto r = ToRaylib(FromRaylib(target));
    assert(r.id == 29 && r.texture.id == 17 && r.depth.id == 37);
    assert(r.depth.width == 41 && r.depth.height == 43 && r.depth.mipmaps == 2 && r.depth.format == 19);
    ::AudioStream stream{reinterpret_cast<::rAudioBuffer*>(storage), reinterpret_cast<::rAudioProcessor*>(storage + 1), 48000, 32, 2};
    ::Music music{stream, 123, true, 5, storage + 2};
    auto m = ToRaylib(FromRaylib(music));
    assert(m.stream.buffer == stream.buffer && m.stream.processor == stream.processor);
    assert(m.stream.sampleRate == 48000 && m.stream.sampleSize == 32 && m.stream.channels == 2);
    assert(m.frameCount == 123 && m.looping && m.ctxType == 5 && m.ctxData == storage + 2);
    ::Matrix matrix{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    auto mat = ToRaylib(FromRaylib(matrix));
    assert(mat.m0 == 1 && mat.m4 == 2 && mat.m8 == 3 && mat.m12 == 4);
    assert(mat.m1 == 5 && mat.m5 == 6 && mat.m9 == 7 && mat.m13 == 8);
    assert(mat.m2 == 9 && mat.m6 == 10 && mat.m10 == 11 && mat.m14 == 12);
    assert(mat.m3 == 13 && mat.m7 == 14 && mat.m11 == 15 && mat.m15 == 16);
    auto v2 = ToRaylib(FromRaylib(::Vector2{1,2})); assert(v2.x == 1 && v2.y == 2);
    auto v3 = ToRaylib(FromRaylib(::Vector3{3,4,5})); assert(v3.x == 3 && v3.y == 4 && v3.z == 5);
    auto v4 = ToRaylib(FromRaylib(::Vector4{6,7,8,9})); assert(v4.x == 6 && v4.y == 7 && v4.z == 8 && v4.w == 9);
    auto c = ToRaylib(FromRaylib(::Color{11,22,33,44})); assert(c.r == 11 && c.g == 22 && c.b == 33 && c.a == 44);
    ::NPatchInfo patch{{1,2,3,4},5,6,7,8,2};
    auto n = ToRaylib(FromRaylib(patch));
    assert(n.source.x == 1 && n.source.y == 2 && n.source.width == 3 && n.source.height == 4);
    assert(n.left == 5 && n.top == 6 && n.right == 7 && n.bottom == 8 && n.layout == 2);
    assert(!Sonar::Image{}.IsValid() && !Sonar::Texture2D{}.IsValid());
    assert(!Sonar::Shader{}.IsValid() && !Sonar::Music{}.IsValid() && !Sonar::RenderTexture2D{}.IsValid());
}
