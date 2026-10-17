#include "UI/Map.hpp"
#include <cstdio>
#include "Entities/SubmarinePlayer.hpp"
#include "SONAR/NavalUnits.hpp"
#include <iostream>
#include <format>
#include "Base/EventDispatcher.hpp"
#include "Base/GameMode.h"
#include "Base/ContextMenu.hpp"
#include "Base/Factory.hpp"
#include "Base/ContextMenuEntry.hpp"
#include "Base/ResourceManager.hpp"

// Countries
#include "CountryMap/continent_outline.h"

#define GridColor {5,18,36,255}

Map::Map(std::string Name, Sonar::Vector2 Pos)
    :Display(Pos.x, Pos.y)
{
    SourceRect.width = Pos.x;
    SourceRect.height = Pos.y;
}

Map::Map(int X, int Y)
    :Display(X, Y)
{
}

Map::~Map()
{
    Sonar::UnloadShader(shader);
}

long double Map::GetMinimumZoomLevel() const
{
    if (DestinationRect.height <= 0.0f)
    {
        return MinMapZoom;
    }

    const long double minimumZoomForHeight = static_cast<long double>(DestinationRect.height) /
        static_cast<long double>(WORLD_HEIGHT);
    return std::max(MinMapZoom, minimumZoomForHeight);
}

void Map::ClampZoomToWorldBounds()
{
    ZoomLevel = std::clamp(ZoomLevel, GetMinimumZoomLevel(), MaxMapZoom);
}

void Map::ClampCameraToWorldBounds()
{
    ClampZoomToWorldBounds();

    while (CameraWorldPosition.x < -WORLD_HALF_WIDTH)
    {
        CameraWorldPosition.x += WORLD_WIDTH;
    }
    while (CameraWorldPosition.x > WORLD_HALF_WIDTH)
    {
        CameraWorldPosition.x -= WORLD_WIDTH;
    }

    const float visibleHeight = DestinationRect.height / ZoomLevel;
    const float halfVisibleHeight = visibleHeight * 0.5f;

    if (halfVisibleHeight >= WORLD_HALF_HEIGHT)
    {
        CameraWorldPosition.y = 0.0f;
        return;
    }

    const float minCameraY = -WORLD_HALF_HEIGHT + halfVisibleHeight;
    const float maxCameraY = WORLD_HALF_HEIGHT - halfVisibleHeight;
    CameraWorldPosition.y = Sonar::Clamp(CameraWorldPosition.y, minCameraY, maxCameraY);
}

void Map::Draw()
{
    // Border and stuff outside of RenderTarget
    Sonar::DrawTextureNPatch(BorderTexture, MapBorder->nPatchInfo.value(), BorderRect,{0,0}, 0, Sonar::WHITE);

	Sonar::Texture2D playerIconTexture = {};
	if (PlayerIcon.isValid())
	{
		playerIconTexture = PlayerIcon;
	}

    // RenderTarget 
    BeginDisplayDraw();
    Sonar::ClearBackground(Sonar::BLACK);

    const float gridSpacing = ZoomLevel < 0.01f ? 500000.f : 1000.f;

    Sonar::Vector2 worldTopLeft = ConvertScreenPosToWorld({ 0, 0 });
    Sonar::Vector2 worldBottomRight = ConvertScreenPosToWorld({ DestinationRect.width, DestinationRect.height });

    float startX = std::floor(worldTopLeft.x / gridSpacing) * gridSpacing;
    float endX = std::ceil(worldBottomRight.x / gridSpacing) * gridSpacing;
    float startY = std::floor(worldTopLeft.y / gridSpacing) * gridSpacing;
    float endY = std::ceil(worldBottomRight.y / gridSpacing) * gridSpacing;

    // Draw vertical grid lines
    for (float x = startX; x <= endX; x += gridSpacing)
    {
        Sonar::Vector2 worldStart = { x, startY };
        Sonar::Vector2 worldEnd = { x, endY };

        Sonar::Vector2 screenStart = ConvertWorldToScreenPos(worldStart);
        Sonar::Vector2 screenEnd = ConvertWorldToScreenPos(worldEnd);

        Sonar::DrawLineEx(screenStart, screenEnd, 2, GridColor);

    }

    // Draw horizontal grid lines
    for (float y = startY; y <= endY; y += gridSpacing)
    {
        Sonar::Vector2 worldStart = { startX, y };
        Sonar::Vector2 worldEnd = { endX, y };

        Sonar::Vector2 screenStart = ConvertWorldToScreenPos(worldStart);
        Sonar::Vector2 screenEnd = ConvertWorldToScreenPos(worldEnd);

        Sonar::DrawLineEx(screenStart, screenEnd, 2, GridColor);
    }


    // Custom OpenGL

    Sonar::Matrix viewProj = GetOpenGLProjectionMatrix();

    const std::array<float, 3> worldCopyOffsets = {
        -WORLD_WIDTH,
        0.0f,
        WORLD_WIDTH,
    };

    for (const float worldCopyOffset : worldCopyOffsets)
    {

        for (auto& Continent : Continents)
        {
            Continent.RenderBuffer(shader, viewProj, worldCopyOffset);
        }
    }

    // Raylib Drawing

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

        std::weak_ptr<Entity> TempObject = std::dynamic_pointer_cast<Entity>(obj);

        Sonar::Vector2 screenPos = ConvertWorldToScreenPos(TempObject.lock()->GetEntityLocation());
        Sonar::Vector2 MousePos = ConvertMouseScreenPosToMapScreenPos(Sonar::GetMousePosition());

        if (!PlayerIcon.isValid())
        {
            continue;
        }

        float scaledWidth = playerIconTexture.width * std::fmax(ZoomLevel, 0.035f);
        float scaledHeight = playerIconTexture.height * std::fmax(ZoomLevel, 0.035f);

        Sonar::Rectangle destRec = {
            screenPos.x,
            screenPos.y,
            scaledWidth,
            scaledHeight
        };

        Sonar::Vector2 origin = { scaledWidth / 2, scaledHeight / 2 };

        if (Sonar::CheckCollisionPointCircle(MousePos, screenPos, playerIconTexture.width * ZoomLevel / 2))
        {
            HoveredUnit = obj;
            if (Sonar::IsMouseButtonPressed(Sonar::MouseButton::MOUSE_BUTTON_LEFT))
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
        if (screenPos.x + (playerIconTexture.width * ZoomLevel / 2) < 0
            || screenPos.x - (playerIconTexture.width * ZoomLevel / 2) > DestinationRect.width
            || screenPos.y + (playerIconTexture.height * ZoomLevel / 2) < 0
            || screenPos.y - (playerIconTexture.height * ZoomLevel / 2) > DestinationRect.height)
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
                        Sonar::DrawCircleLinesV(screenPos, playerIconTexture.width * ZoomLevel / 2, Sonar::YELLOW);
                    }
                    if (FocusedUnit.lock() == obj)
                    {
                        Sonar::DrawCircleLinesV(screenPos, playerIconTexture.width * ZoomLevel / 2, Sonar::PURPLE);
                        std::string CourseString = std::to_string(Submarine->GetEntityRotation());
                        std::string SpeedString = std::to_string(Submarine->GetCurrentSpeed());
                        Sonar::DrawText(("Name: " + obj->GetDisplayName()).c_str(), screenPos.x + (playerIconTexture.width * ZoomLevel / 2) + 2, screenPos.y - 24, 12, Sonar::GREEN);
                        Sonar::DrawText(("Course: " + CourseString).c_str(), screenPos.x + (playerIconTexture.width * ZoomLevel / 2) + 2, screenPos.y, 12, Sonar::GREEN);
                        Sonar::DrawText(("Speed: " + SpeedString).c_str(), screenPos.x + (playerIconTexture.width * ZoomLevel / 2) + 2, screenPos.y + 12, 12, Sonar::GREEN);
                    }

                    Sonar::DrawTexturePro(
                        playerIconTexture,
                        { 0, 0, (float)playerIconTexture.width, (float)playerIconTexture.height }, // Source rectangle
                        destRec,                                                      // Destination rectangle
                        origin,                                                       // Rotate around the center
                        TempObject.lock()->GetEntityRotation(),                           // Rotation angle
                        ColorLookupState[static_cast<int>(State)]                     // Tint color
                    );
                }
                break;
            }
            // Handle Ship type
        }
    }
    auto Mark = ConvertWorldToScreenPos(MarkedPos);
    Sonar::DrawCircle(Mark.x, Mark.y, 10, Sonar::GREEN);

    for (const auto Index : IndicesPendingKill)
    {
        ObjectsToDraw.erase(ObjectsToDraw.begin() + Index);
    }
    IndicesPendingKill.clear();
    ObjectsToDraw.shrink_to_fit();

    EndDisplayDraw();

    // Overlay
}

void Map::Tick(float DeltaTime)
{
    // Handle resize interaction from base class
    Super::Tick(DeltaTime);
    ClampCameraToWorldBounds();

    const Sonar::Rectangle DisplayedRect = GetDisplayedRect();

    if (IsManipulatingDisplay())
    {
        BorderRect = { DisplayedRect.x - 15, DisplayedRect.y - 15, DisplayedRect.width + 30, DisplayedRect.height + 30 };
        Draw();
        RenderToMainBuffer();
        return;
    }

    if (Sonar::CheckCollisionPointRec(Sonar::GetMousePosition(), DestinationRect))
    {
        int Multiply = 100.f;
        if (Sonar::IsKeyDown(Sonar::KEY_LEFT_CONTROL))
        {
            Multiply = 1.f;
        }
        ZoomLevel += Sonar::GetMouseWheelMove() * (0.0001f * Multiply);
        ClampZoomToWorldBounds();

        // Pan
        if (Sonar::IsMouseButtonPressed(Sonar::MOUSE_BUTTON_LEFT))
        {
            IsDragging = true;
            LastMousePosition = Sonar::GetMousePosition();
        }
        if (Sonar::IsMouseButtonReleased(Sonar::MOUSE_BUTTON_LEFT))
        {
            IsDragging = false;
        }
        if (IsDragging)
        {
            Sonar::Vector2 delta = Sonar::Vector2Scale(
                Sonar::Vector2Subtract(Sonar::GetMousePosition(), LastMousePosition),
                static_cast<float>(1.0L / ZoomLevel));
            CameraWorldPosition = Sonar::Vector2Subtract(CameraWorldPosition, delta);

            LastMousePosition = Sonar::GetMousePosition();
        }
    }
    else
    {
        IsDragging = false;
    }

    ClampCameraToWorldBounds();

	BorderRect = { DisplayedRect.x - 15, DisplayedRect.y - 15, DisplayedRect.width + 30, DisplayedRect.height + 30 };

    Draw();
    RenderToMainBuffer();

    // Calling it manually
    // TODO: Later on should be changed to TickGroups
    RightClickMenu.TryLoad()->Tick(DeltaTime);
    SpeedMenu.TryLoad()->Tick(DeltaTime);

}

void Map::Initialize()
{
	Super::Initialize();
	Init();
}

void Map::Init()
{
    auto addContinent = [this](const auto& vertices, const auto& offsets, const auto& counts) {
        RenderBufferArrayLine buffer;
        buffer.AddVertices(vertices);
        buffer.AddOffsets(offsets);
        buffer.AddCounts(counts);
        buffer.LoadBuffer();
        Continents.push_back(std::move(buffer));
    };
    addContinent(AsiaVertices, AsiaOffsets, AsiaCounts);
    addContinent(SouthAmericaVertices, SouthAmericaOffsets, SouthAmericaCounts);
    addContinent(AfricaVertices, AfricaOffsets, AfricaCounts);
    addContinent(EuropeVertices, EuropeOffsets, EuropeCounts);
    addContinent(NorthAmericaVertices, NorthAmericaOffsets, NorthAmericaCounts);
    addContinent(OceaniaVertices, OceaniaOffsets, OceaniaCounts);
    addContinent(AntarcticaVertices, AntarcticaOffsets, AntarcticaCounts);
    addContinent(SevenseasVertices, SevenseasOffsets, SevenseasCounts);
    const EngineRuntimeContext& RuntimeContext = GetOutter()->GetRuntimeContext();
    const std::string VertexShaderPath = RuntimeContext.ResolveShaderPath("basic.vs");
    const std::string FragmentShaderPath = RuntimeContext.ResolveShaderPath("basic.fs");

    if (VertexShaderPath.empty() || FragmentShaderPath.empty())
    {
        LOG_ERROR(l_RESOURCES, SONAR_FMT("Failed to locate map shaders. Vertex: '{}' Fragment: '{}'",
            VertexShaderPath,
            FragmentShaderPath));
    }

    shader = Sonar::LoadShader(
        VertexShaderPath.empty() ? nullptr : VertexShaderPath.c_str(),
        FragmentShaderPath.empty() ? nullptr : FragmentShaderPath.c_str());
    try
    {
        LoadRessources();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(l_RESOURCES, SONAR_FMT("{}", e.what()));
    }

    // Events
    MapClickEvent = std::make_shared<AllPurposeEvent>();
    MapEventDispatcher = std::make_shared<EventDispatcher>();
    ClickDataPayload = std::make_shared<MapClickEventData>();
    MapClickEvent->Payload = ClickDataPayload;


    RightClickMenu = GetOutter()->NewObject<ContextMenu>();
    RightClickMenu.TryLoad()->SetDisplayName("Right Click Menu");

    SpeedMenu = GetOutter()->NewObject<ContextMenu>();
    SpeedMenu.TryLoad()->SetDisplayName("Speed Menu");

    ContextMenuEntry FullSpeedEntry;
    FullSpeedEntry.SetDisplayName("Full Speed");
    FullSpeedEntry.SetCallback([this](ContextMenuEntry* Self)
        {
            TrackedPlayer.lock()->SetSpeed(20);
        });

    ContextMenuEntry HalfSpeedEntry;
    HalfSpeedEntry.SetDisplayName("Half Speed");
    HalfSpeedEntry.SetCallback([this](ContextMenuEntry* Self)
        {
            TrackedPlayer.lock()->SetSpeed(12);
        });

    ContextMenuEntry SlowAheadEntry;
    SlowAheadEntry.SetDisplayName("Slow Ahead");
    SlowAheadEntry.SetCallback([this](ContextMenuEntry* Self)
        {
            TrackedPlayer.lock()->SetSpeed(8);
        });

    ContextMenuEntry DeadSlowEntry;
    DeadSlowEntry.SetDisplayName("Dead Slow");
    DeadSlowEntry.SetCallback([this](ContextMenuEntry* Self)
        {
            TrackedPlayer.lock()->SetSpeed(3);
        });

    ContextMenuEntry StopEntry;
    StopEntry.SetDisplayName("Stop");
    StopEntry.SetCallback([this](ContextMenuEntry* Self)
        {
            TrackedPlayer.lock()->SetSpeed(0);
        });

    SpeedMenu.TryLoad()->AddMenuEntry(FullSpeedEntry);
    SpeedMenu.TryLoad()->AddMenuEntry(HalfSpeedEntry);
    SpeedMenu.TryLoad()->AddMenuEntry(SlowAheadEntry);
    SpeedMenu.TryLoad()->AddMenuEntry(DeadSlowEntry);
    SpeedMenu.TryLoad()->AddMenuEntry(StopEntry);



    ContextMenuEntry NewEntry;
    NewEntry.SetDisplayName("Center Player");
    NewEntry.SetCallback([this](ContextMenuEntry* Self) -> void
        {
            CameraWorldPosition = TrackedPlayer.lock()->GetEntityLocation();
            ClampCameraToWorldBounds();
            return;
        });

    ContextMenuEntry NewEntry2;
    NewEntry2.SetDisplayName("Set Course");
    NewEntry2.SetCallback([this](ContextMenuEntry* Self)
        {
            Sonar::Vector2 localMouse = ConvertMouseScreenPosToMapScreenPos(RightClickMenu.TryLoad()->MousePosWhenConstructed);

            Sonar::Vector2 MousePos = ConvertScreenPosToWorld(localMouse);

            MarkedPos = MousePos;

            TrackedPlayer.lock()->MoveEntityToPosition(MarkedPos);

        });


    ContextMenuEntry NewEntry3;
	NewEntry3.CloseOnClick = false;
    NewEntry3.SetDisplayName("Set Speed");
    NewEntry3.SetCallback([this](ContextMenuEntry* Self)
        {
            auto Height = Self->ContextMenuEntryRec.y + Self->ContextMenuEntryRec.height;
            auto Width = Self->ContextMenuEntryRec.x + Self->ContextMenuEntryRec.width;
            this->SpeedMenu.TryLoad()->OnConstruct({ Width, Height });

            auto ptr = this->SpeedMenu.TryLoad();
        });


    RightClickMenu.TryLoad()->AddMenuEntry(NewEntry);
    RightClickMenu.TryLoad()->AddMenuEntry(NewEntry2);
    RightClickMenu.TryLoad()->AddMenuEntry(NewEntry3);




    CameraWorldPosition = { 0.0f, 0.0f };
    ZoomLevel = 1.0f;
    ClampCameraToWorldBounds();
}

void Map::AddObjectToDraw(std::weak_ptr<IObject> inObject) 
{
    if (inObject.lock() != nullptr && *inObject.lock()->GetStaticClass() << (Entity::StaticClass()))
    {
        std::cout << "Found Derived " << inObject.lock()->GetDisplayName() << std::endl;
        std::weak_ptr<SubmarinePlayer> PlayerPTR;
        if (*(inObject.lock()->GetStaticClass()) << (SubmarinePlayer::StaticClass()))
        {
            PlayerPTR = std::dynamic_pointer_cast<SubmarinePlayer>(inObject.lock());
        }
        if (PlayerPTR.lock() && TrackedPlayer.lock() == nullptr)
        {
            TrackedPlayer = PlayerPTR;
            auto EntityPtr = std::dynamic_pointer_cast<Entity>(inObject.lock());
            ObjectsToDraw.push_back({ EntityPtr, {ObjectType::Submarine, ObjectState::EPlayer} });
        }
        else
        {
            auto EntityPtr = std::dynamic_pointer_cast<Entity>(inObject.lock());
            ObjectsToDraw.push_back({ EntityPtr, {ObjectType::Submarine, ObjectState::EEnemy} });
        }

    }
}

void Map::OnKeyStroke(Sonar::KeyboardKey Key, Sonar::Vector2 MousePos)
{
    if (Sonar::CheckCollisionPointRec(MousePos, DestinationRect)
        && Key == Sonar::KEY_C)
    {
        CameraWorldPosition = TrackedPlayer.lock()->GetEntityLocation();
        ZoomLevel = 1.f;
        ClampCameraToWorldBounds();
    }
}

void Map::OnMouseButtonPressed(Sonar::MouseButton Key, Sonar::Vector2 MousePos)
{
    if (!Sonar::CheckCollisionPointRec(MousePos, DestinationRect))
    {
        return;
    }

    if (Key == Sonar::MOUSE_BUTTON_RIGHT)
    {
        RightClickMenu.TryLoad()->OnConstruct(MousePos);
    }
}

Sonar::Vector2 Map::ConvertWorldToScreenPos(Sonar::Vector2 worldPos) const
{
    float relX = worldPos.x - CameraWorldPosition.x;
    float relY = worldPos.y - CameraWorldPosition.y;

    relX *= ZoomLevel;
    relY *= ZoomLevel;

    Sonar::Vector2 screenPos;
    screenPos.x = relX + (DestinationRect.width * 0.5f);
    screenPos.y = relY + (DestinationRect.height * 0.5f);

    return screenPos;
}

Sonar::Vector2 Map::ConvertScreenPosToWorld(Sonar::Vector2 VectorToConver) const
{
    // 1) Convert so that (0,0) is at the center:
    float centeredX = VectorToConver.x - (DestinationRect.width * 0.5f);
    float centeredY = VectorToConver.y - (DestinationRect.height * 0.5f);

    // 2) If you flipped Y above, flip back here:
    //    (uncomment if you used relY = -relY)
    // centeredY = -centeredY;

    // 3) Undo zoom:
    centeredX /= ZoomLevel;
    centeredY /= ZoomLevel;

    // 4) Translate so that camera-origin returns to its world position:
    Sonar::Vector2 worldPos;
    worldPos.x = centeredX + CameraWorldPosition.x;
    worldPos.y = centeredY + CameraWorldPosition.y;

    return worldPos;
}

inline Sonar::Vector2 Map::ConvertTextureSizeToWorldSize(TextureResource* UsedTexture, Sonar::Vector2 SizeInMeters)
{
    return { SizeInMeters.x / UsedTexture->width, SizeInMeters.y / UsedTexture->height };
}

Sonar::Matrix Map::GetViewProjectionMatrix() const
{
    Sonar::Matrix S = Sonar::MatrixScale(ZoomLevel, ZoomLevel, 1);
    Sonar::Matrix T = Sonar::MatrixTranslate(-CameraWorldPosition.x, -CameraWorldPosition.y, 0);
    Sonar::Matrix O = Sonar::MatrixTranslate(
        DestinationRect.width / 2.0f,
        DestinationRect.height / 2.0f,
        0
    );

    return Sonar::MatrixMultiply(Sonar::MatrixMultiply(T, S), O);
}

Sonar::Matrix Map::GetOpenGLProjectionMatrix() const
{
    float visibleWidth = DestinationRect.width / ZoomLevel;
    float visibleHeight = DestinationRect.height / ZoomLevel;

    // Bounds
    float left = CameraWorldPosition.x - visibleWidth / 2.0f;
    float right = CameraWorldPosition.x + visibleWidth / 2.0f;
    float bottom = -CameraWorldPosition.y - visibleHeight / 2.0f;
    float top = -CameraWorldPosition.y + visibleHeight / 2.0f;

    // OpenGL Matrix Projection
    Sonar::Matrix projection = { 0 };
    projection.m0 = 2.0f / (right - left); // X scale
    projection.m5 = 2.0f / (top - bottom); // Y scale
    projection.m12 = -(right + left) / (right - left); // X translation
    projection.m13 = -(top + bottom) / (top - bottom); // Y translation
    projection.m15 = 1.0f;

    return projection;
}

void Map::LoadRessources()
{
    const EngineRuntimeContext& RuntimeContext = GetOutter()->GetRuntimeContext();

    if (TextureResource* PlayerIconResource = RuntimeContext.GetResource("PlayerMapIcon"))
    {
        PlayerIcon = PlayerIconResource->LoadTexture();
        if (PlayerIcon.isValid())
        {
            Sonar::Texture2D LoadedPlayerIcon = PlayerIcon;
            Sonar::SetTextureFilter(LoadedPlayerIcon, Sonar::TextureFilter::TEXTURE_FILTER_TRILINEAR);
        }
    }
    else
    {
        LOG_ERROR(l_RESOURCES, SONAR_FMT("Missing texture resource definition for 'PlayerMapIcon'"));
    }

    if (TextureResource* ShipIconResource = RuntimeContext.GetResource("ShipMapIcon"))
    {
        ShipIcon = ShipIconResource->LoadTexture();
        if (ShipIcon.isValid())
        {
            Sonar::Texture2D LoadedShipIcon = ShipIcon;
            Sonar::SetTextureFilter(LoadedShipIcon, Sonar::TextureFilter::TEXTURE_FILTER_TRILINEAR);
        }
    }
    else
    {
        LOG_ERROR(l_RESOURCES, SONAR_FMT("Missing texture resource definition for 'ShipMapIcon'"));
    }

    MapBorder = GetOutter()->GetRuntimeContext().GetResource("ButtonImage");
    MapBorder->GenerateMipMaps();
    BorderTexture = MapBorder->LoadTexture();
}

inline Sonar::Vector2 Map::ConvertMouseScreenPosToMapScreenPos(Sonar::Vector2 MouseAbsolutePos)
{
    Sonar::Vector2 DestinationVector = { DestinationRect.x, DestinationRect.y };
    Sonar::Vector2 DestinationVectorSize = { DestinationRect.width, DestinationRect.height };
    return Sonar::Vector2Clamp(Sonar::Vector2Subtract(MouseAbsolutePos, DestinationVector), Sonar::Vector2{ 0,0 }, DestinationVectorSize);
}