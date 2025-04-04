#include "UI/Map.hpp"
#include <cstdio>
#include "Entities/Player.hpp"
#include <iostream>

Map::Map(Vector2 Pos)
    :Display(Pos.x, Pos.y)
{
    SourceRect.width = Pos.x;
    SourceRect.height = Pos.y;
    Init();
}

Map::Map(int X, int Y)
    :Display(X, Y)
{
    Init();
}

void Map::Draw()
{
    BeginTextureMode(ActiveRenderTarget);
    ClearBackground(BLACK);

    for (const auto& Object : ObjectsToDraw)
    {
        if (Object.second == ObjectType::Submarine)
        {
            auto CurrentPlayer = std::dynamic_pointer_cast<Player>(Object.first.lock());
            if (!CurrentPlayer) continue;

            float iconScale = ZoomLevel;
            Vector2 iconSize = {
                PlayerIcon.width * iconScale,
                PlayerIcon.height * iconScale
            };

            Vector2 posInTexture = ConvertWorldToScreenPos(CurrentPlayer->Position);
            posInTexture.x -= iconSize.x / 2.0f;
            posInTexture.y -= iconSize.y / 2.0f;

            DrawTextureEx(PlayerIcon, posInTexture, 0.0f, iconScale, BLUE);
        }
    }

    EndTextureMode();
}

void Map::Tick(float DeltaTime)
{
    // Handle zoom and pan only if mouse is over the map
    if (CheckCollisionPointRec(GetMousePosition(), DestinationRect))
    {
        // Zoom
        ZoomLevel += GetMouseWheelMove() * 0.1f;
        ZoomLevel = std::clamp(ZoomLevel, 0.1f, 10.0f);

        // Pan
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            IsDragging = true;
            LastMousePosition = GetMousePosition();
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            IsDragging = false;
        }
        if (IsDragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 currentMousePos = GetMousePosition();
            Vector2 deltaMouse = Vector2Subtract(currentMousePos, LastMousePosition);
            Vector2 deltaWorld = Vector2Scale(deltaMouse, 1.0f / ZoomLevel);
            CameraWorldPosition.x -= deltaWorld.x;
            CameraWorldPosition.y -= deltaWorld.y;
            LastMousePosition = currentMousePos;
        }
    }
    else {
        IsDragging = false;
    }

    Draw();
    RenderToMainBuffer();
}

void Map::Init()
{
    Image ImagePlayer = LoadImage((GameInstance::GetInstance()->WorkingDirectory + "\\resources\\imgs\\PlayerMap.png").c_str());
    PlayerIcon = LoadTextureFromImage(ImagePlayer);
    UnloadImage(ImagePlayer);
    CameraWorldPosition = { 0.0f, 0.0f };
    ZoomLevel = 1.0f;
}

void Map::AddObjectToDraw(std::weak_ptr<IObject> Object)
{
    if (Object.lock()->GetStaticClass() == Player::StaticClass())
    {
        auto playerPtr = std::dynamic_pointer_cast<Player>(Object.lock());
        ObjectsToDraw.push_back({ Object, ObjectType::Submarine });
        TrackedPlayer = playerPtr;
        // Initialize camera to player's position
        if (!IsDragging) {
            CameraWorldPosition = playerPtr->Position;
        }
    }
}

Vector2 Map::ConvertWorldToScreenPos(Vector2 worldPos) const
{
    Vector2 relativePos = Vector2Subtract(worldPos, CameraWorldPosition);
    Vector2 scaledPos = Vector2Scale(relativePos, ZoomLevel);
    return Vector2{
        scaledPos.x + (DestinationRect.width / 2),
        scaledPos.y + (DestinationRect.height / 2)
    };
}