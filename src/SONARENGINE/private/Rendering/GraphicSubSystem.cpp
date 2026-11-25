#include "Base/Logging.h"
#include "raylib.h"
#include "rlgl.h"
#include "external/glad.h"
#include "Base/SonarTypeConversions.hpp"
#include "Rendering/GraphicSubSystem.hpp"

using Sonar::Backend::ToRaylib;
using Sonar::Backend::FromRaylib;

void GraphicSubSystem::OnInitialize()
{
    LOG_INFO("GraphicSubSystem initialized");
}

void GraphicSubSystem::OnShutdown()
{
    LOG_INFO("GraphicSubSystem shutdown");
}

void GraphicSubSystem::OnUpdate() {}

void GraphicSubSystem::InitWindow(int width, int height, const char* title)
{
    ::InitWindow(width, height, title);
}

void GraphicSubSystem::CloseWindow(void) {
#if defined(_WIN32) && !defined(PLATFORM_DESKTOP_RGFW)
::RaylibCloseWindow()
#else
::CloseWindow()
#endif
; }

bool GraphicSubSystem::WindowShouldClose(void) { auto result=::WindowShouldClose(); return result; }

int GraphicSubSystem::GetScreenWidth(void) { auto result=::GetScreenWidth(); return result; }

int GraphicSubSystem::GetScreenHeight(void) { auto result=::GetScreenHeight(); return result; }

void GraphicSubSystem::ClearBackground(Sonar::Color color) { ::ClearBackground(ToRaylib(color)); }

void GraphicSubSystem::BeginDrawing(void) { ::BeginDrawing(); }

void GraphicSubSystem::EndDrawing(void) { ::EndDrawing(); }

void GraphicSubSystem::BeginTextureMode(Sonar::RenderTexture2D target) { ::BeginTextureMode(ToRaylib(target)); }

void GraphicSubSystem::EndTextureMode(void) { ::EndTextureMode(); }

Sonar::Shader GraphicSubSystem::LoadShader(const char *vsFileName, const char *fsFileName) { auto result=FromRaylib(::LoadShader(vsFileName,fsFileName)); return result; }

void GraphicSubSystem::UnloadShader(Sonar::Shader& resource)
{
    if (resource.IsValid()) ::UnloadShader(ToRaylib(resource));
    resource = {};
}

void GraphicSubSystem::SetTargetFPS(int fps) { ::SetTargetFPS(fps); }

float GraphicSubSystem::GetFrameTime(void) { auto result=::GetFrameTime(); return result; }

void GraphicSubSystem::SetConfigFlags(unsigned int flags) { ::SetConfigFlags(flags); }

void GraphicSubSystem::SetTraceLogCallback(Sonar::TraceLogCallback callback) { ::SetTraceLogCallback(callback); }

void GraphicSubSystem::DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Sonar::Color color) { ::DrawLine(startPosX,startPosY,endPosX,endPosY,ToRaylib(color)); }

void GraphicSubSystem::DrawLineEx(Sonar::Vector2 startPos, Sonar::Vector2 endPos, float thick, Sonar::Color color) { ::DrawLineEx(ToRaylib(startPos),ToRaylib(endPos),thick,ToRaylib(color)); }

void GraphicSubSystem::DrawCircle(int centerX, int centerY, float radius, Sonar::Color color) { ::DrawCircle(centerX,centerY,radius,ToRaylib(color)); }

void GraphicSubSystem::DrawCircleLinesV(Sonar::Vector2 center, float radius, Sonar::Color color) { ::DrawCircleLinesV(ToRaylib(center),radius,ToRaylib(color)); }

void GraphicSubSystem::DrawRectangle(int posX, int posY, int width, int height, Sonar::Color color) { ::DrawRectangle(posX,posY,width,height,ToRaylib(color)); }

void GraphicSubSystem::DrawRectangleRec(Sonar::Rectangle rec, Sonar::Color color) { ::DrawRectangleRec(ToRaylib(rec),ToRaylib(color)); }

void GraphicSubSystem::DrawRectangleLinesEx(Sonar::Rectangle rec, float lineThick, Sonar::Color color) { ::DrawRectangleLinesEx(ToRaylib(rec),lineThick,ToRaylib(color)); }

void GraphicSubSystem::DrawRectangleRounded(Sonar::Rectangle rec, float roundness, int segments, Sonar::Color color) { ::DrawRectangleRounded(ToRaylib(rec),roundness,segments,ToRaylib(color)); }

void GraphicSubSystem::DrawRectangleRoundedLinesEx(Sonar::Rectangle rec, float roundness, int segments, float lineThick, Sonar::Color color) { ::DrawRectangleRoundedLinesEx(ToRaylib(rec),roundness,segments,lineThick,ToRaylib(color)); }

Sonar::Image GraphicSubSystem::LoadImageFromTexture(Sonar::Texture2D texture) { auto result=FromRaylib(::LoadImageFromTexture(ToRaylib(texture))); return result; }

Sonar::Image GraphicSubSystem::GenImageColor(int width, int height, Sonar::Color color) { auto result=FromRaylib(::GenImageColor(width,height,ToRaylib(color))); return result; }

void GraphicSubSystem::UnloadImage(Sonar::Image& image)
{
    if (image.IsValid()) ::UnloadImage(ToRaylib(image));
    image = {};
}

Sonar::Texture2D GraphicSubSystem::LoadTexture(const char *fileName) { auto result=FromRaylib(::LoadTexture(fileName)); return result; }

Sonar::Texture2D GraphicSubSystem::LoadTextureFromImage(Sonar::Image image) { auto result=FromRaylib(::LoadTextureFromImage(ToRaylib(image))); return result; }

Sonar::RenderTexture2D GraphicSubSystem::LoadRenderTexture(int width, int height) { auto result=FromRaylib(::LoadRenderTexture(width,height)); return result; }

void GraphicSubSystem::UnloadTexture(Sonar::Texture2D& resource)
{
    if (resource.IsValid()) ::UnloadTexture(ToRaylib(resource));
    resource = {};
}

void GraphicSubSystem::UnloadRenderTexture(Sonar::RenderTexture2D& resource)
{
    if (resource.IsValid()) ::UnloadRenderTexture(ToRaylib(resource));
    resource = {};
}

void GraphicSubSystem::UpdateTexture(Sonar::Texture2D texture, const void *pixels) { ::UpdateTexture(ToRaylib(texture),pixels); }

void GraphicSubSystem::GenTextureMipmaps(Sonar::Texture2D* texture)
{
    if (!texture) return;
    auto value = ToRaylib(*texture);
    ::GenTextureMipmaps(&value);
    *texture = FromRaylib(value);
}

void GraphicSubSystem::SetTextureFilter(Sonar::Texture2D texture, int filter) { ::SetTextureFilter(ToRaylib(texture),filter); }

void GraphicSubSystem::DrawTexture(Sonar::Texture2D texture, int posX, int posY, Sonar::Color tint) { ::DrawTexture(ToRaylib(texture),posX,posY,ToRaylib(tint)); }

void GraphicSubSystem::DrawTexturePro(Sonar::Texture2D texture, Sonar::Rectangle source, Sonar::Rectangle dest, Sonar::Vector2 origin, float rotation, Sonar::Color tint) { ::DrawTexturePro(ToRaylib(texture),ToRaylib(source),ToRaylib(dest),ToRaylib(origin),rotation,ToRaylib(tint)); }

void GraphicSubSystem::DrawTextureNPatch(Sonar::Texture2D texture, Sonar::NPatchInfo nPatchInfo, Sonar::Rectangle dest, Sonar::Vector2 origin, float rotation, Sonar::Color tint) { ::DrawTextureNPatch(ToRaylib(texture),ToRaylib(nPatchInfo),ToRaylib(dest),ToRaylib(origin),rotation,ToRaylib(tint)); }

Sonar::Color GraphicSubSystem::ColorAlpha(Sonar::Color color, float alpha) { auto result=FromRaylib(::ColorAlpha(ToRaylib(color),alpha)); return result; }

void GraphicSubSystem::DrawFPS(int posX, int posY) { ::DrawFPS(posX,posY); }

void GraphicSubSystem::DrawText(const char *text, int posX, int posY, int fontSize, Sonar::Color color) { ::DrawText(text,posX,posY,fontSize,ToRaylib(color)); }

int GraphicSubSystem::MeasureText(const char *text, int fontSize) { auto result=::MeasureText(text,fontSize); return result; }

void GraphicSubSystem::BeginLineRendering(const Sonar::Shader& shader, const Sonar::Matrix& projection, float offset) {
    ::rlDrawRenderBatchActive(); auto s=ToRaylib(shader);
    ::glUseProgram(s.id);
    ::glUniformMatrix4fv(::glGetUniformLocation(s.id,"uMVP"),1,GL_TRUE,&projection.m0);
    ::glUniform1f(::glGetUniformLocation(s.id,"uWorldOffsetX"),offset);
}
void GraphicSubSystem::EndLineRendering() { ::glBindVertexArray(0); ::glUseProgram(::rlGetShaderIdDefault()); }
