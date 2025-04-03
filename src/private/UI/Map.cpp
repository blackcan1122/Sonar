#include "UI/Map.hpp"
#include "Entities/Player.hpp"

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
            std::weak_ptr<Player> PlayerCast = std::dynamic_pointer_cast<Player>(Object.first.lock());
            Vector2 Position = PlayerCast.lock()->Position;
            DrawTextureEx(PlayerIcon, CenterMap - Position - CenterPlayer, 0.f, 1.f, BLUE);
        }
    }
    
    EndTextureMode();
}

void Map::Tick(float DeltaTime)
{
    CenterPlayer = Vector2{ (float)PlayerIcon.width / 2,  abs((float)PlayerIcon.height / 2) };
    Draw();
    RenderToMainBuffer();
}

void Map::Init()
{
    Image ImagePlayer = LoadImage(((GameInstance::GetInstance())->WorkingDirectory + "\\resources\\imgs\\PlayerMap.png").c_str());
    PlayerIcon = LoadTextureFromImage(ImagePlayer);
    PlayerIcon.height = 32;
    PlayerIcon.width = 32;

    CenterMap = Vector2{ SourceRect.width / 2, abs(SourceRect.height / 2) };
    UnloadImage(ImagePlayer);
}

void Map::AddObjectToDraw(std::weak_ptr<IObject> Object)
{
    if (Object.lock()->GetStaticClass() == Player::StaticClass())
    {
        ObjectsToDraw.push_back({ Object, ObjectType::Submarine });
    }
}
