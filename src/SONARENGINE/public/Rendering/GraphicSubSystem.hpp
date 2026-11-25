#pragma once

#include "Base/SSubSystem.hpp"
#include "Base/SonarTypes.hpp"

class SONARENGINE_API GraphicSubSystem : public SSubSystem
{
    AUTOBODY(GraphicSubSystem, SSubSystem)
public:
    GraphicSubSystem() = default;
    virtual ~GraphicSubSystem() = default;

    void OnInitialize() override;
    void OnShutdown() override;
    void OnUpdate() override;

    void InitWindow(int width, int height, const char *title);
    void CloseWindow(void);
    bool WindowShouldClose(void);
    int GetScreenWidth(void);
    int GetScreenHeight(void);
    void ClearBackground(Sonar::Color color);
    void BeginDrawing(void);
    void EndDrawing(void);
    void BeginTextureMode(Sonar::RenderTexture2D target);
    void EndTextureMode(void);
    Sonar::Shader LoadShader(const char *vsFileName, const char *fsFileName);
    void UnloadShader(Sonar::Shader& resource);
    void SetTargetFPS(int fps);
    float GetFrameTime(void);
    void SetConfigFlags(unsigned int flags);
    void SetTraceLogCallback(Sonar::TraceLogCallback callback);
    void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Sonar::Color color);
    void DrawLineEx(Sonar::Vector2 startPos, Sonar::Vector2 endPos, float thick, Sonar::Color color);
    void DrawCircle(int centerX, int centerY, float radius, Sonar::Color color);
    void DrawCircleLinesV(Sonar::Vector2 center, float radius, Sonar::Color color);
    void DrawRectangle(int posX, int posY, int width, int height, Sonar::Color color);
    void DrawRectangleRec(Sonar::Rectangle rec, Sonar::Color color);
    void DrawRectangleLinesEx(Sonar::Rectangle rec, float lineThick, Sonar::Color color);
    void DrawRectangleRounded(Sonar::Rectangle rec, float roundness, int segments, Sonar::Color color);
    void DrawRectangleRoundedLinesEx(Sonar::Rectangle rec, float roundness, int segments, float lineThick, Sonar::Color color);
    Sonar::Image LoadImageFromTexture(Sonar::Texture2D texture);
    Sonar::Image GenImageColor(int width, int height, Sonar::Color color);
    void UnloadImage(Sonar::Image& image);
    Sonar::Texture2D LoadTexture(const char *fileName);
    Sonar::Texture2D LoadTextureFromImage(Sonar::Image image);
    Sonar::RenderTexture2D LoadRenderTexture(int width, int height);
    void UnloadTexture(Sonar::Texture2D& resource);
    void UnloadRenderTexture(Sonar::RenderTexture2D& resource);
    void UpdateTexture(Sonar::Texture2D texture, const void *pixels);
    void GenTextureMipmaps(Sonar::Texture2D *texture);
    void SetTextureFilter(Sonar::Texture2D texture, int filter);
    void DrawTexture(Sonar::Texture2D texture, int posX, int posY, Sonar::Color tint);
    void DrawTexturePro(Sonar::Texture2D texture, Sonar::Rectangle source, Sonar::Rectangle dest, Sonar::Vector2 origin, float rotation, Sonar::Color tint);
    void DrawTextureNPatch(Sonar::Texture2D texture, Sonar::NPatchInfo nPatchInfo, Sonar::Rectangle dest, Sonar::Vector2 origin, float rotation, Sonar::Color tint);
    Sonar::Color ColorAlpha(Sonar::Color color, float alpha);
    void DrawFPS(int posX, int posY);
    void DrawText(const char *text, int posX, int posY, int fontSize, Sonar::Color color);
    int MeasureText(const char *text, int fontSize);

    void BeginLineRendering(const Sonar::Shader& shader, const Sonar::Matrix& projection, float offset);
    void EndLineRendering();
};
