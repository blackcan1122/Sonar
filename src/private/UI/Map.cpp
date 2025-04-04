#include "UI/Map.hpp"
#include "Entities/Player.hpp"
#include <cstdio>



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

    Vector2 CamPos = ConvertWorldToScreenPos(MapCenter);
    for (const auto& Object : ObjectsToDraw)
    {
        if (Object.second == ObjectType::Submarine)
        {
            auto CurrentPlayer = std::dynamic_pointer_cast<Player>(Object.first.lock());
            if (!CurrentPlayer) continue;

            // Use positive scale for the icon
            float iconScale = ZoomLevel;
            Vector2 iconSize = {
                PlayerIcon.width * iconScale,
                PlayerIcon.height * iconScale
            };

            // Convert player's world position to texture coordinates
            Vector2 posInTexture = Vector2Subtract(ConvertWorldToScreenPos(Vector2Scale((CurrentPlayer->Position), ZoomLevel)), CamPos);

            

            // Center the icon by subtracting half of its scaled size
            posInTexture.x -= iconSize.x / 2.0f;
            posInTexture.y -= iconSize.y / 2.0f;

            DrawTextureEx(PlayerIcon, posInTexture, 0.0f, iconScale, BLUE);
            
        }
    }

    EndTextureMode();
}

void Map::Tick(float DeltaTime)
{
    if (CheckCollisionPointRec(GetMousePosition(), DestinationRect))
    {
        float prevScale = ZoomLevel;
        ZoomLevel += GetMouseWheelMove() * 0.1f;

        ZoomLevel = std::clamp(ZoomLevel, 0.0001f, 1000.f);

        // Adjust map offset to keep the view centered
        Vector2 delta = {
            SourceRect.width * 0.5f * (1.0f / prevScale - 1.0f / ZoomLevel),
            SourceRect.height * 0.5f * (1.0f / prevScale - 1.0f / ZoomLevel)
        };

        MapOffset.x += delta.x;
        MapOffset.y += delta.y;

    }

        std::cout << "Test" << std::endl;

    Draw();
    RenderToMainBuffer();
}

void Map::Init()
{
    Image ImagePlayer = LoadImage(((GameInstance::GetInstance())->WorkingDirectory + "\\resources\\imgs\\PlayerMap.png").c_str());
    PlayerIcon = LoadTextureFromImage(ImagePlayer);
    PlayerIcon.height = 100;
    PlayerIcon.width = 100;
    std::freopen("log.txt", "w", stdout);

    MapOffset = { DestinationRect.width / 2, DestinationRect.height / 2 };
    UnloadImage(ImagePlayer);
}

void Map::AddObjectToDraw(std::weak_ptr<IObject> Object)
{
    if (Object.lock()->GetStaticClass() == Player::StaticClass())
    {
        ObjectsToDraw.push_back({ Object, ObjectType::Submarine });
    }
}

Vector2 Map::ConvertWorldToScreenPos(Vector2 VectorToConvert) const
{
    return Vector2{
        VectorToConvert.x + DestinationRect.width / 2,
        VectorToConvert.y + DestinationRect.height / 2
    };
}
