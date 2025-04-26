#include "UI/Map.hpp"
#include <cstdio>
#include "Entities/Player.hpp"
#include <iostream>
#include <format>
#include "Base/GameMode.h"

// Countries
#include "Base/WorldData.hpp"
#include "CountryMap/NA.hpp"
#include "CountryMap/SA.hpp"
#include "CountryMap/Africa.hpp"
#include "CountryMap/Asia.hpp"
#include "CountryMap/Europe.hpp"

#define GridColor {5,18,36,255}

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
    // Border and stuff outside of RenderTarget
    DrawTextureNPatch(BorderTexture, MapBorder->nPatchInfo.value(), BorderRect,{0,0}, 0, WHITE);

    // RenderTarget 
    BeginTextureMode(ActiveRenderTarget);
    ClearBackground(BLACK);

    // Assume gridSpacing is in world units
    const float gridSpacing = ZoomLevel < 0.01f ? 500000.f : 1000.f;

    // Determine the world-space bounds of the viewport.
    // If you don't have ConvertScreenToWorldPos, you must compute the inverse of your view matrix.
    // For example, if you have this function defined, you could do:
    Vector2 worldTopLeft = ConvertScreenPosToWorld({ 0, 0 });
    Vector2 worldBottomRight = ConvertScreenPosToWorld({ DestinationRect.width, DestinationRect.height });

    // Calculate starting and ending grid positions (round down/up to the nearest grid spacing)
    float startX = std::floor(worldTopLeft.x / gridSpacing) * gridSpacing;
    float endX = std::ceil(worldBottomRight.x / gridSpacing) * gridSpacing;
    float startY = std::floor(worldTopLeft.y / gridSpacing) * gridSpacing;
    float endY = std::ceil(worldBottomRight.y / gridSpacing) * gridSpacing;

    // Draw vertical grid lines
    for (float x = startX; x <= endX; x += gridSpacing)
    {
        // Create points in world space for the vertical line
        Vector2 worldStart = { x, startY };
        Vector2 worldEnd = { x, endY };

        // Convert both endpoints to screen space
        Vector2 screenStart = ConvertWorldToScreenPos(worldStart);
        Vector2 screenEnd = ConvertWorldToScreenPos(worldEnd);

        DrawLineEx(screenStart, screenEnd, 2, GridColor);

    }

    // Draw horizontal grid lines
    for (float y = startY; y <= endY; y += gridSpacing)
    {
        // Create points in world space for the horizontal line
        Vector2 worldStart = { startX, y };
        Vector2 worldEnd = { endX, y };

        // Convert both endpoints to screen space
        Vector2 screenStart = ConvertWorldToScreenPos(worldStart);
        Vector2 screenEnd = ConvertWorldToScreenPos(worldEnd);

        DrawLineEx(screenStart, screenEnd, 2, GridColor);
    }

    // Map Borders <-- TODO: Optimize heavily and refactor just a POC
    // currently takes around 6 ms
    
    for (int i = 0; i < africaOutline.size(); i++)
    {
        if (i == africaOutline.size() - 1)
        {
            continue;
        }

        auto AfricaOutlinePointA = ConvertWorldToScreenPos(africaOutline[i]);
        auto AfricaOutlinePointB = ConvertWorldToScreenPos(africaOutline[i+1]);
        if (AfricaOutlinePointA.x < 0 && AfricaOutlinePointB.x < 0
            || AfricaOutlinePointA.x > DestinationRect.width && AfricaOutlinePointB.x > DestinationRect.width
            || AfricaOutlinePointA.y < 0 && AfricaOutlinePointB.y < 0
            || AfricaOutlinePointA.y > DestinationRect.height && AfricaOutlinePointB.y > DestinationRect.height)
        {
            continue;
        }

        DrawLineEx(AfricaOutlinePointA, AfricaOutlinePointB, 1, GREEN);    
    }

    for (int i = 0; i < EuropeOutline.size(); i++)
    {
        if (i == EuropeOutline.size() - 1)
        {
            continue;
        }

        auto AfricaOutlinePointA = ConvertWorldToScreenPos(EuropeOutline[i]);
        auto AfricaOutlinePointB = ConvertWorldToScreenPos(EuropeOutline[i + 1]);
        if (AfricaOutlinePointA.x < 0 && AfricaOutlinePointB.x < 0
            || AfricaOutlinePointA.x > DestinationRect.width && AfricaOutlinePointB.x > DestinationRect.width
            || AfricaOutlinePointA.y < 0 && AfricaOutlinePointB.y < 0
            || AfricaOutlinePointA.y > DestinationRect.height && AfricaOutlinePointB.y > DestinationRect.height)
        {
            continue;
        }

        DrawLineEx(AfricaOutlinePointA, AfricaOutlinePointB, 1, GREEN);
    }

    for (int i = 0; i < NAOutline.size(); i++)
    {
        if (i == NAOutline.size() - 1)
        {
            continue;
        }

        auto AfricaOutlinePointA = ConvertWorldToScreenPos(NAOutline[i]);
        auto AfricaOutlinePointB = ConvertWorldToScreenPos(NAOutline[i + 1]);
        if (AfricaOutlinePointA.x < 0 && AfricaOutlinePointB.x < 0
            || AfricaOutlinePointA.x > DestinationRect.width && AfricaOutlinePointB.x > DestinationRect.width
            || AfricaOutlinePointA.y < 0 && AfricaOutlinePointB.y < 0
            || AfricaOutlinePointA.y > DestinationRect.height && AfricaOutlinePointB.y > DestinationRect.height)
        {
            continue;
        }

        DrawLineEx(AfricaOutlinePointA, AfricaOutlinePointB, 1, GREEN);
    }

    for (int i = 0; i < SAOutline.size(); i++)
    {
        if (i == SAOutline.size() - 1)
        {
            continue;
        }

        auto AfricaOutlinePointA = ConvertWorldToScreenPos(SAOutline[i]);
        auto AfricaOutlinePointB = ConvertWorldToScreenPos(SAOutline[i + 1]);
        if (AfricaOutlinePointA.x < 0 && AfricaOutlinePointB.x < 0
            || AfricaOutlinePointA.x > DestinationRect.width && AfricaOutlinePointB.x > DestinationRect.width
            || AfricaOutlinePointA.y < 0 && AfricaOutlinePointB.y < 0
            || AfricaOutlinePointA.y > DestinationRect.height && AfricaOutlinePointB.y > DestinationRect.height)
        {
            continue;
        }

        DrawLineEx(AfricaOutlinePointA, AfricaOutlinePointB, 1, GREEN);
    }

    for (int i = 0; i < AsiaOutline.size(); i++)
    {
        if (i == AsiaOutline.size() - 1)
        {
            continue;
        }

        auto AfricaOutlinePointA = ConvertWorldToScreenPos(AsiaOutline[i]);
        auto AfricaOutlinePointB = ConvertWorldToScreenPos(AsiaOutline[i + 1]);
        if (AfricaOutlinePointA.x < 0 && AfricaOutlinePointB.x < 0
            || AfricaOutlinePointA.x > DestinationRect.width && AfricaOutlinePointB.x > DestinationRect.width
            || AfricaOutlinePointA.y < 0 && AfricaOutlinePointB.y < 0
            || AfricaOutlinePointA.y > DestinationRect.height && AfricaOutlinePointB.y > DestinationRect.height)
        {
            continue;
        }

        DrawLineEx(AfricaOutlinePointA, AfricaOutlinePointB, 1, GREEN);
    }



    for (size_t i = 0; i < ObjectsToDraw.size(); i++)
    {
        auto& ObjectPair = ObjectsToDraw[i];
        auto Type = ObjectPair.second.first;
        auto State = ObjectPair.second.second;
        auto obj = ObjectPair.first.lock();

        

        if (!obj)
        {
            IndicesPendingKill.push_back(i);
            continue; 
        }

        std::weak_ptr<Entity> Object = std::dynamic_pointer_cast<Entity>(obj);



        Vector2 screenPos = ConvertWorldToScreenPos(Object.lock()->GetEntityLocation());
        Vector2 MousePos = ConvertMouseScreenPosToMapScreenPos(GetMousePosition());

        // Calculate scaled texture dimensions
        float scaledWidth = PlayerIcon.width * std::fmax(ZoomLevel, 0.035f);
        float scaledHeight = PlayerIcon.height * std::fmax(ZoomLevel, 0.035f);

        // Define the destination rectangle (centered at screenPos)
        Rectangle destRec = {
            screenPos.x,  // Center horizontally
            screenPos.y, // Center vertically
            scaledWidth,
            scaledHeight
        };

        // Define the rotation origin (center of the sprite)
        Vector2 origin = { scaledWidth / 2, scaledHeight / 2 };

        if (CheckCollisionPointCircle(MousePos, screenPos, PlayerIcon.width * ZoomLevel / 2))
        {
            HoveredUnit = obj;
            if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
            {
                FocusedUnit = obj;
                ClickDataPayload->ClickedObject = obj->GetName();
                MapEventDispatcher->Dispatch(MapClickEvent);
            }
        }
        else
        {
            HoveredUnit.reset();
        }

        // Frustum culling: skip off-screen objects
        if (screenPos.x + (PlayerIcon.width * ZoomLevel / 2) < 0
            || screenPos.x - (PlayerIcon.width * ZoomLevel / 2) > DestinationRect.width
            || screenPos.y + (PlayerIcon.height * ZoomLevel / 2) < 0
            || screenPos.y - (PlayerIcon.height * ZoomLevel / 2) > DestinationRect.height)
        {
            continue;
        }

        switch (Type)
        {
            case ObjectType::Submarine: 
            {
                auto Submarine = std::dynamic_pointer_cast<BaseSubmarine>(obj);
                if (Submarine) 
                {
                    // BoundingBox Drawing for hover and focused
                    if (HoveredUnit.lock() == obj)
                    {
                        DrawCircleLinesV(screenPos, PlayerIcon.width * ZoomLevel / 2, YELLOW);
                    }
                    if (FocusedUnit.lock() == obj)
                    {
                        DrawCircleLinesV(screenPos, PlayerIcon.width * ZoomLevel / 2, PURPLE);
                        std::string CourseString = std::to_string(Submarine->GetEntityRotation());
                        std::string SpeedString = std::to_string(Submarine->GetCurrentSpeed());
                        DrawText(("Course: " + CourseString).c_str(), screenPos.x + (PlayerIcon.width * ZoomLevel / 2) + 2, screenPos.y, 12, GREEN);
                        DrawText(("Speed: " + SpeedString).c_str(), screenPos.x + (PlayerIcon.width * ZoomLevel / 2) + 2, screenPos.y + 12, 12, GREEN);
                    }
                    
                    DrawTexturePro(
                        PlayerIcon,
                        { 0, 0, (float)PlayerIcon.width, (float)PlayerIcon.height }, // Source rectangle (entire texture)
                        destRec,                                                      // Destination rectangle (position/size)
                        origin,                                                       // Rotate around the center
                        Object.lock()->GetEntityRotation(),                           // Rotation angle
                        ColorLookupState[static_cast<int>(State)]                     // Tint color
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

    // Overlay
}

void Map::Tick(float DeltaTime)
{
    // Handle input (same as before)
    if (CheckCollisionPointRec(GetMousePosition(), DestinationRect)) 
    {
        int Multiply = 100.f;
        if (IsKeyDown(KEY_LEFT_CONTROL))
        {
            Multiply = 1.f;
        }
        ZoomLevel += GetMouseWheelMove() * (0.0001f * Multiply);
        ZoomLevel = Clamp(ZoomLevel, 0.0001f, 10.0f);

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


    BorderRect = { DestinationRect.x - 15, DestinationRect.y - 15, DestinationRect.width + 30, DestinationRect.height + 30 };

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
    if (*Object.lock()->GetStaticClass()<<(Entity::StaticClass()))
    {
        std::cout << "Found Derived " << Object.lock()->GetDisplayName() << std::endl;
        std::shared_ptr<Player> PlayerPTR = std::dynamic_pointer_cast<Player>(Object.lock());
        if (PlayerPTR && TrackedPlayer == nullptr)
        {
            TrackedPlayer = PlayerPTR;
            ObjectsToDraw.push_back({ Object, {ObjectType::Submarine, ObjectState::EPlayer} });
        }
        else
        {
            auto EntityPtr = std::dynamic_pointer_cast<Entity>(Object.lock());
            ObjectsToDraw.push_back({ Object, {ObjectType::Submarine, ObjectState::EEnemy} });
        }

    }
}

Vector2 Map::ConvertWorldToScreenPos(Vector2 worldPos) const
{
    Matrix transform = GetViewProjectionMatrix();
    Vector3 transformed = Vector3Transform({ worldPos.x, worldPos.y, 0 }, transform);
    return { transformed.x, transformed.y };
}

Vector2 Map::ConvertScreenPosToWorld(Vector2 VectorToConver) const
{
    // Get the view projection matrix (maps world -> screen)
    Matrix viewProj = GetViewProjectionMatrix();
    // Invert it so we get the transformation from screen -> world
    Matrix invViewProj = MatrixInvert(viewProj);

    // Transform the screen position (using z=0) into world space
    Vector3 transformed = Vector3Transform({ VectorToConver.x, VectorToConver.y, 0 }, invViewProj);
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
    }
    ImageMipmaps(&ImageSubmarine);
    PlayerIcon = LoadTextureFromImage(ImageSubmarine);
    SetTextureFilter(PlayerIcon, TextureFilter::TEXTURE_FILTER_TRILINEAR);
    UnloadImage(ImageSubmarine);

    // Ship Icon
    Image ImageShip = LoadImage(ShipIconPath.c_str());
    if (!ImageShip.data)
    {
        UnloadImage(ImageShip);
    }

    ShipIcon = LoadTextureFromImage(ImageShip);
    UnloadImage(ImageShip);

    MapBorder = GameInstance::GetInstance()->GetResource("ButtonImage");
    MapBorder->GenerateMipMaps();
    BorderTexture = MapBorder->LoadTexture();
}


inline Vector2 Map::ConvertMouseScreenPosToMapScreenPos(Vector2 MouseAbsolutePos)
{
    Vector2 DestinationVector = { DestinationRect.x, DestinationRect.y };
    Vector2 DestinationVectorSize = { DestinationRect.width, DestinationRect.height };
    return Vector2Clamp(Vector2Subtract(GetMousePosition(), DestinationVector), Vector2{ 0,0 }, DestinationVectorSize);
}