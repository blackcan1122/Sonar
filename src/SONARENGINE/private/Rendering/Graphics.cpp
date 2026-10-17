#include "raylib.h"

#include "raymath.h"

#include "rlgl.h"

#include "external/glad.h"

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
#include "Rendering/Graphics.hpp"

#include <cstring>

#include <type_traits>

namespace Sonar {

struct BackendAccess {

    template<class Native, class Resource> static Native& get(const Resource& r) {

        if (!r.native) { static Native empty{}; return empty; }

        return *static_cast<Native*>(r.native.get());

    }

    template<class Resource, class Native> static Resource wrap(Native value, bool owns = true) {

        Resource r;

        r.native = std::shared_ptr<Native>(new Native(value), [owns](Native* p) {
            if (owns) {
                if constexpr (std::is_same_v<Native, ::Texture2D>) { if (p->id && ::IsWindowReady()) ::UnloadTexture(*p); }
                else if constexpr (std::is_same_v<Native, ::Shader>) { if (p->id && ::IsWindowReady()) ::UnloadShader(*p); }
                else if constexpr (std::is_same_v<Native, ::RenderTexture2D>) { if (p->id && ::IsWindowReady()) ::UnloadRenderTexture(*p); }
                else if constexpr (std::is_same_v<Native, ::Music>) { if (p->stream.buffer && ::IsAudioDeviceReady()) ::UnloadMusicStream(*p); }
            }
            delete p;
        });

        return r;

    }

};

::Vector2 native(Vector2 v) { return { v.x, v.y }; }

Vector2 owned(::Vector2 v) { return { v.x, v.y }; }

::Vector3 native(Vector3 v) { return { v.x, v.y, v.z }; }

Vector3 owned(::Vector3 v) { return { v.x, v.y, v.z }; }

::Vector4 native(Vector4 v) { return { v.x, v.y, v.z, v.w }; }

Vector4 owned(::Vector4 v) { return { v.x, v.y, v.z, v.w }; }

::Matrix native(Matrix v) { return { v.m0, v.m4, v.m8, v.m12, v.m1, v.m5, v.m9, v.m13, v.m2, v.m6, v.m10, v.m14, v.m3, v.m7, v.m11, v.m15 }; }

Matrix owned(::Matrix v) { return { v.m0, v.m4, v.m8, v.m12, v.m1, v.m5, v.m9, v.m13, v.m2, v.m6, v.m10, v.m14, v.m3, v.m7, v.m11, v.m15 }; }

::Color native(Color v) { return { v.r, v.g, v.b, v.a }; }

Color owned(::Color v) { return { v.r, v.g, v.b, v.a }; }

::Rectangle native(Rectangle v) { return { v.x, v.y, v.width, v.height }; }

Rectangle owned(::Rectangle v) { return { v.x, v.y, v.width, v.height }; }

::NPatchInfo native(NPatchInfo v) { return { native(v.source), v.left, v.top, v.right, v.bottom, v.layout }; }

NPatchInfo owned(::NPatchInfo v) { return { owned(v.source), v.left, v.top, v.right, v.bottom, v.layout }; }

::Texture2D native(const Texture2D& r) { auto value=BackendAccess::get<::Texture2D>(r); value.width=r.width; value.height=r.height; return value; }

Texture2D owned(::Texture2D v) { auto r=BackendAccess::wrap<Texture2D>(v); r.width=v.width; r.height=v.height; return r; }

::Shader native(const Shader& r) { return BackendAccess::get<::Shader>(r); }

Shader owned(::Shader v) { auto r=BackendAccess::wrap<Shader>(v);  return r; }

::Music native(const Music& r) { return BackendAccess::get<::Music>(r); }

Music owned(::Music v) { auto r=BackendAccess::wrap<Music>(v);  return r; }

::RenderTexture2D native(const RenderTexture2D& r) { return BackendAccess::get<::RenderTexture2D>(r); }

RenderTexture2D owned(::RenderTexture2D v) { auto r=BackendAccess::wrap<RenderTexture2D>(v); r.texture=BackendAccess::wrap<Texture2D>(v.texture, false); r.texture.width=v.texture.width; r.texture.height=v.texture.height; return r; }

::Image native(const Image& image) { return {const_cast<Color*>(image.pixels.data()),image.width,image.height,1,PIXELFORMAT_UNCOMPRESSED_R8G8B8A8}; }

Image owned(::Image image) {

    ::ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    Image result; result.width=image.width; result.height=image.height;

    auto colors=static_cast<::Color*>(image.data);

    for (int i=0;i<image.width*image.height;++i) result.pixels.push_back(owned(colors[i]));

    ::UnloadImage(image); return result;

}

void InitWindow(int width, int height, const char *title) { ::InitWindow(width,height,title); }

void CloseWindow(void) { 
#if defined(_WIN32) && !defined(PLATFORM_DESKTOP_RGFW)
::RaylibCloseWindow()
#else
::CloseWindow()
#endif
; }

bool WindowShouldClose(void) { auto result=::WindowShouldClose(); return result; }

int GetScreenWidth(void) { auto result=::GetScreenWidth(); return result; }

int GetScreenHeight(void) { auto result=::GetScreenHeight(); return result; }

bool IsCursorOnScreen(void) { auto result=::IsCursorOnScreen(); return result; }

void ClearBackground(Color color) { ::ClearBackground(native(color)); }

void BeginDrawing(void) { ::BeginDrawing(); }

void EndDrawing(void) { ::EndDrawing(); }

void BeginTextureMode(RenderTexture2D target) { ::BeginTextureMode(native(target)); }

void EndTextureMode(void) { ::EndTextureMode(); }

Shader LoadShader(const char *vsFileName, const char *fsFileName) { auto result=owned(::LoadShader(vsFileName,fsFileName)); return result; }

void UnloadShader(Shader resource) { if(resource.IsValid()) { auto& value=BackendAccess::get<::Shader>(resource); if (value.id) ::UnloadShader(value); value={}; } }

void SetTargetFPS(int fps) { ::SetTargetFPS(fps); }

float GetFrameTime(void) { auto result=::GetFrameTime(); return result; }

int GetRandomValue(int min, int max) { auto result=::GetRandomValue(min,max); return result; }

void SetConfigFlags(unsigned int flags) { ::SetConfigFlags(flags); }

void SetTraceLogCallback(TraceLogCallback callback) { ::SetTraceLogCallback(callback); }

bool IsKeyPressed(int key) { auto result=::IsKeyPressed(key); return result; }

bool IsKeyDown(int key) { auto result=::IsKeyDown(key); return result; }

int GetKeyPressed(void) { auto result=::GetKeyPressed(); return result; }

int GetCharPressed(void) { auto result=::GetCharPressed(); return result; }

bool IsMouseButtonPressed(int button) { auto result=::IsMouseButtonPressed(button); return result; }

bool IsMouseButtonDown(int button) { auto result=::IsMouseButtonDown(button); return result; }

bool IsMouseButtonReleased(int button) { auto result=::IsMouseButtonReleased(button); return result; }

Vector2 GetMousePosition(void) { auto result=owned(::GetMousePosition()); return result; }

float GetMouseWheelMove(void) { auto result=::GetMouseWheelMove(); return result; }

void SetMouseCursor(int cursor) { ::SetMouseCursor(cursor); }

void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color) { ::DrawLine(startPosX,startPosY,endPosX,endPosY,native(color)); }

void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color) { ::DrawLineEx(native(startPos),native(endPos),thick,native(color)); }

void DrawCircle(int centerX, int centerY, float radius, Color color) { ::DrawCircle(centerX,centerY,radius,native(color)); }

void DrawCircleLinesV(Vector2 center, float radius, Color color) { ::DrawCircleLinesV(native(center),radius,native(color)); }

void DrawRectangle(int posX, int posY, int width, int height, Color color) { ::DrawRectangle(posX,posY,width,height,native(color)); }

void DrawRectangleRec(Rectangle rec, Color color) { ::DrawRectangleRec(native(rec),native(color)); }

void DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color) { ::DrawRectangleLinesEx(native(rec),lineThick,native(color)); }

void DrawRectangleRounded(Rectangle rec, float roundness, int segments, Color color) { ::DrawRectangleRounded(native(rec),roundness,segments,native(color)); }

void DrawRectangleRoundedLinesEx(Rectangle rec, float roundness, int segments, float lineThick, Color color) { ::DrawRectangleRoundedLinesEx(native(rec),roundness,segments,lineThick,native(color)); }

bool CheckCollisionPointRec(Vector2 point, Rectangle rec) { auto result=::CheckCollisionPointRec(native(point),native(rec)); return result; }

bool CheckCollisionPointCircle(Vector2 point, Vector2 center, float radius) { auto result=::CheckCollisionPointCircle(native(point),native(center),radius); return result; }

Image LoadImageFromTexture(Texture2D texture) { auto result=owned(::LoadImageFromTexture(native(texture))); return result; }

Image GenImageColor(int width, int height, Color color) { auto result=owned(::GenImageColor(width,height,native(color))); return result; }

Texture2D LoadTexture(const char *fileName) { auto result=owned(::LoadTexture(fileName)); return result; }

Texture2D LoadTextureFromImage(Image image) { auto result=owned(::LoadTextureFromImage(native(image))); return result; }

RenderTexture2D LoadRenderTexture(int width, int height) { auto result=owned(::LoadRenderTexture(width,height)); return result; }

void UnloadTexture(Texture2D resource) { if(resource.IsValid()) { auto& value=BackendAccess::get<::Texture2D>(resource); if (value.id) ::UnloadTexture(value); value={}; } }

void UnloadRenderTexture(RenderTexture2D resource) { if(resource.IsValid()) { auto& value=BackendAccess::get<::RenderTexture2D>(resource); if (value.id) ::UnloadRenderTexture(value); value={}; } }

void UpdateTexture(Texture2D texture, const void *pixels) { ::UpdateTexture(native(texture),pixels); }

void GenTextureMipmaps(Texture2D *texture) { auto& value=BackendAccess::get<::Texture2D>(*texture); ::GenTextureMipmaps(&value); }

void SetTextureFilter(Texture2D texture, int filter) { ::SetTextureFilter(native(texture),filter); }

void DrawTexture(Texture2D texture, int posX, int posY, Color tint) { ::DrawTexture(native(texture),posX,posY,native(tint)); }

void DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint) { ::DrawTexturePro(native(texture),native(source),native(dest),native(origin),rotation,native(tint)); }

void DrawTextureNPatch(Texture2D texture, NPatchInfo nPatchInfo, Rectangle dest, Vector2 origin, float rotation, Color tint) { ::DrawTextureNPatch(native(texture),native(nPatchInfo),native(dest),native(origin),rotation,native(tint)); }

Color ColorAlpha(Color color, float alpha) { auto result=owned(::ColorAlpha(native(color),alpha)); return result; }

void DrawFPS(int posX, int posY) { ::DrawFPS(posX,posY); }

void DrawText(const char *text, int posX, int posY, int fontSize, Color color) { ::DrawText(text,posX,posY,fontSize,native(color)); }

int MeasureText(const char *text, int fontSize) { auto result=::MeasureText(text,fontSize); return result; }

void InitAudioDevice(void) { ::InitAudioDevice(); }

void CloseAudioDevice(void) { ::CloseAudioDevice(); }

Music LoadMusicStream(const char *fileName) { auto result=owned(::LoadMusicStream(fileName)); return result; }

void UnloadMusicStream(Music resource) { if(resource.IsValid()) { auto& value=BackendAccess::get<::Music>(resource); if (value.stream.buffer) ::UnloadMusicStream(value); value={}; } }

void PlayMusicStream(Music music) { ::PlayMusicStream(native(music)); }

void UpdateMusicStream(Music music) { ::UpdateMusicStream(native(music)); }

void StopMusicStream(Music music) { ::StopMusicStream(native(music)); }

void SetMusicVolume(Music music, float volume) { ::SetMusicVolume(native(music),volume); }

float Clamp(float value, float min, float max) { auto result=::Clamp(value,min,max); return result; }

Vector2 Vector2Add(Vector2 v1, Vector2 v2) { auto result=owned(::Vector2Add(native(v1),native(v2))); return result; }

Vector2 Vector2Subtract(Vector2 v1, Vector2 v2) { auto result=owned(::Vector2Subtract(native(v1),native(v2))); return result; }

Vector2 Vector2Scale(Vector2 v, float scale) { auto result=owned(::Vector2Scale(native(v),scale)); return result; }

Vector2 Vector2Normalize(Vector2 v) { auto result=owned(::Vector2Normalize(native(v))); return result; }

Vector2 Vector2Clamp(Vector2 v, Vector2 min, Vector2 max) { auto result=owned(::Vector2Clamp(native(v),native(min),native(max))); return result; }

Matrix MatrixMultiply(Matrix left, Matrix right) { auto result=owned(::MatrixMultiply(native(left),native(right))); return result; }

Matrix MatrixTranslate(float x, float y, float z) { auto result=owned(::MatrixTranslate(x,y,z)); return result; }

Matrix MatrixScale(float x, float y, float z) { auto result=owned(::MatrixScale(x,y,z)); return result; }

bool Texture2D::IsValid() const { return native && BackendAccess::get<::Texture2D>(*this).id != 0; }
bool Shader::IsValid() const { return native && BackendAccess::get<::Shader>(*this).id != 0; }
bool RenderTexture2D::IsValid() const { return native && BackendAccess::get<::RenderTexture2D>(*this).id != 0; }
bool Music::IsValid() const { return native && BackendAccess::get<::Music>(*this).stream.buffer != 0; }
void BeginLineRendering(const Shader& shader, const Matrix& projection, float offset) {
    ::rlDrawRenderBatchActive(); auto s=native(shader);
    ::glUseProgram(s.id);
    ::glUniformMatrix4fv(::glGetUniformLocation(s.id,"uMVP"),1,GL_TRUE,&projection.m0);
    ::glUniform1f(::glGetUniformLocation(s.id,"uWorldOffsetX"),offset);
}
void EndLineRendering() { ::glBindVertexArray(0); ::glUseProgram(::rlGetShaderIdDefault()); }
void SetMusicLooping(Music music, bool looping) { BackendAccess::get<::Music>(music).looping=looping; }
}
