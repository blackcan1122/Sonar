#include "UI/Map.hpp"
#include <cstdio>
#include "Entities/Player.hpp"
#include <iostream>
#include <format>
#include "Base/GameMode.h"

Map::Map(std::string Name, Vector2 Pos)
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

    for (size_t i = 0; i < ObjectsToDraw.size(); i++)
    {
        auto& ObjectPair = ObjectsToDraw[i];
        auto Type = ObjectPair.second.first;
        auto State = ObjectPair.second.second.first;
        auto& InterActionState = ObjectPair.second.second.second;
        auto obj = ObjectPair.first.lock();

        

        if (!obj)
        {
            IndicesPendingKill.push_back(i);
            continue; 
        }

        std::weak_ptr<Entity> Object = std::dynamic_pointer_cast<Entity>(obj);



        Vector2 screenPos = ConvertWorldToScreenPos(Object.lock()->GetEntityLocation());
        Vector2 MousePos = ConvertMouseScreenPosToMapScreenPos(GetMousePosition());

        if (CheckCollisionPointCircle(MousePos, screenPos, PlayerIcon.width * ZoomLevel / 2))
        {
            if (InterActionState != InteractionState::Active)
            {
                InterActionState = InteractionState::Hovered;
            }
            if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
            {
                InterActionState = InteractionState::Active;
                ClickDataPayload->ClickedObject = obj->GetName();
                MapEventDispatcher->Dispatch(MapClickEvent);
            }
        }

        // Frustum culling: skip off-screen objects
        if (screenPos.x + (PlayerIcon.width / 2) < 0 
            || screenPos.x - (PlayerIcon.width / 2) > DestinationRect.width
            || screenPos.y + (PlayerIcon.height / 2) < 0
            || screenPos.y - (PlayerIcon.height / 2) > DestinationRect.height) 
        {
            continue;
        }

        switch (Type)
        {
            case ObjectType::Submarine: 
            {
                auto player = std::dynamic_pointer_cast<Player>(obj);
                if (player) 
                {
#if DEBUG
                    DrawCircleLines(screenPos.x, screenPos.y, PlayerIcon.width * ZoomLevel / 2, ColorLookupInteractivity[static_cast<int>(InterActionState)]);
#endif
                    DrawTextureEx(
                        PlayerIcon,
                        Vector2Subtract(screenPos, {
                            PlayerIcon.width * ZoomLevel / 2,
                            PlayerIcon.height * ZoomLevel / 2
                            }),
                        Object.lock()->GetEntityRotation(),
                        ZoomLevel,
                        ColorLookupState[static_cast<int>(State)]
                    );
                }
                break;
            }
                                  // Handle Ship type similarly
        }
    }

    for (const auto Index : IndicesPendingKill)
    {
        ObjectsToDraw.erase(ObjectsToDraw.begin() + Index);
    }
    IndicesPendingKill.clear();
    ObjectsToDraw.shrink_to_fit();

    EndTextureMode();
}

void Map::Tick(float DeltaTime)
{
    // Handle input (same as before)
    if (CheckCollisionPointRec(GetMousePosition(), DestinationRect)) {
        // Zoom
        ZoomLevel += GetMouseWheelMove() * 0.1f;
        ZoomLevel = Clamp(ZoomLevel, 0.1f, 10.0f);

        // Pan
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
        {
            IsDragging = true;
            LastMousePosition = GetMousePosition();
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) 
        {
            IsDragging = false;
        }
        if (IsDragging) 
        {
            Vector2 delta = Vector2Scale(
                Vector2Subtract(GetMousePosition(), LastMousePosition),
                1.0f / ZoomLevel
            );
            CameraWorldPosition = Vector2Subtract(CameraWorldPosition, delta);
            LastMousePosition = GetMousePosition();
        }
    }
    else
    {
        IsDragging = false;
    }



    Draw();
    RenderToMainBuffer();
}

void Map::Init()
{
    try
    {
        LoadRessources();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(l_RESOURCES, TEXT("{}", e.what()));
    }

    // Events
    MapClickEvent = std::make_shared<AllPurposeEvent>();
    MapEventDispatcher = std::make_shared<EventDispatcher>();
    ClickDataPayload = std::make_shared<MapClickEventData>();

    MapClickEvent->Payload = ClickDataPayload;



    CameraWorldPosition = { 0.0f, 0.0f };
    ZoomLevel = 1.0f;
}

void Map::AddObjectToDraw(std::weak_ptr<IObject> Object) 
{
    if (Object.lock()->GetStaticClass() == Player::StaticClass())
    {
        auto playerPtr = std::dynamic_pointer_cast<Player>(Object.lock());
        ObjectsToDraw.push_back({ Object, {ObjectType::Submarine, {ObjectState::Enemy, InteractionState::None } } });
    }
}

Vector2 Map::ConvertWorldToScreenPos(Vector2 worldPos) const
{
    Matrix transform = GetViewProjectionMatrix();
    Vector3 transformed = Vector3Transform({ worldPos.x, worldPos.y, 0 }, transform);
    return { transformed.x, transformed.y };
}

Matrix Map::GetViewProjectionMatrix() const
{
    Matrix translate = MatrixTranslate(-CameraWorldPosition.x, -CameraWorldPosition.y, 0);

    Matrix scale = MatrixScale(ZoomLevel, ZoomLevel, 1);

    Matrix offset = MatrixTranslate(
        DestinationRect.width / 2.0f,
        DestinationRect.height / 2.0f,
        0
    );

    return MatrixMultiply(MatrixMultiply(translate, scale), offset);
}

void Map::LoadRessources()
{
    // Player Icon
    Image ImageSubmarine = LoadImage(PlayerIconPath.c_str());
    if (!ImageSubmarine.data)
    {
        UnloadImage(ImageSubmarine);
        throw std::runtime_error("Failed To Load Player Icon: " + PlayerIconPath);
    }
    PlayerIcon = LoadTextureFromImage(ImageSubmarine);
    UnloadImage(ImageSubmarine);

    // Ship Icon
    Image ImageShip = LoadImage(ShipIconPath.c_str());
    if (!ImageShip.data)
    {
        UnloadImage(ImageShip);
        throw std::runtime_error("Failed To Load Ship Icon: " + ShipIconPath);
    }

    ShipIcon = LoadTextureFromImage(ImageShip);
    UnloadImage(ImageShip);

}


inline Vector2 Map::ConvertMouseScreenPosToMapScreenPos(Vector2 MouseAbsolutePos)
{
    Vector2 DestinationVector = { DestinationRect.x, DestinationRect.y };
    Vector2 DestinationVectorSize = { DestinationRect.width, DestinationRect.height };
    return Vector2Clamp(Vector2Subtract(GetMousePosition(), DestinationVector), Vector2{ 0,0 }, DestinationVectorSize);
}