#include "UI/Map.hpp"
#include <cstdio>
#include "Entities/Player.hpp"
#include "Base/NavalTypedefs.h"
#include <iostream>
#include <format>
#include "Base/GameMode.h"
#include "Base/ContextMenu.hpp"
#include "Base/Factory.hpp"
#include "Base/ContextMenuEntry.hpp"

// Countries
#include "CountryMap/continent_outline.h"

#include "external/glad.h"

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

Map::~Map()
{
    unsigned int vaos[] = { vaoAfrica, vaoEurope, vaoAsia, vaoNA, vaoOceania, vaoSA, vaoAntarctica };
    unsigned int vbos[] = { vboAfrica, vboEurope, vboAsia, vboNA, vboOceania, vboSA, vboAntarctica };
    glDeleteVertexArrays(7, vaos);
    glDeleteBuffers(7, vbos);

    Display::~Display();
}

void Map::Draw()
{
    // Border and stuff outside of RenderTarget
    DrawTextureNPatch(BorderTexture, MapBorder->nPatchInfo.value(), BorderRect,{0,0}, 0, WHITE);

    // RenderTarget 
    BeginTextureMode(ActiveRenderTarget);
    ClearBackground(BLACK);

    const float gridSpacing = ZoomLevel < 0.01f ? 500000.f : 1000.f;

    Vector2 worldTopLeft = ConvertScreenPosToWorld({ 0, 0 });
    Vector2 worldBottomRight = ConvertScreenPosToWorld({ DestinationRect.width, DestinationRect.height });

    float startX = std::floor(worldTopLeft.x / gridSpacing) * gridSpacing;
    float endX = std::ceil(worldBottomRight.x / gridSpacing) * gridSpacing;
    float startY = std::floor(worldTopLeft.y / gridSpacing) * gridSpacing;
    float endY = std::ceil(worldBottomRight.y / gridSpacing) * gridSpacing;

    // Draw vertical grid lines
    for (float x = startX; x <= endX; x += gridSpacing)
    {
        Vector2 worldStart = { x, startY };
        Vector2 worldEnd = { x, endY };

        Vector2 screenStart = ConvertWorldToScreenPos(worldStart);
        Vector2 screenEnd = ConvertWorldToScreenPos(worldEnd);

        DrawLineEx(screenStart, screenEnd, 2, GridColor);

    }

    // Draw horizontal grid lines
    for (float y = startY; y <= endY; y += gridSpacing)
    {
        Vector2 worldStart = { startX, y };
        Vector2 worldEnd = { endX, y };

        Vector2 screenStart = ConvertWorldToScreenPos(worldStart); 
        Vector2 screenEnd = ConvertWorldToScreenPos(worldEnd);

        DrawLineEx(screenStart, screenEnd, 2, GridColor);
    }


    // Custom OpenGL

    rlDrawRenderBatchActive();

    Matrix viewProj = GetOpenGLProjectionMatrix();
    glUseProgram(shader.id);
    glUniformMatrix4fv(locMVP, 1, GL_TRUE, &viewProj.m0); // GL_TRUE = transpose for raylib's Matrix

    RenderOpenGLBuffer(vaoAfrica, &AfricaVertices, &AfricaOffsets, &AfricaCounts);
    RenderOpenGLBuffer(vaoEurope, &EuropeVertices, &EuropeOffsets, &EuropeCounts);
    RenderOpenGLBuffer(vaoAsia, &AsiaVertices, &AsiaOffsets, &AsiaCounts);
    RenderOpenGLBuffer(vaoNA, &NorthAmericaVertices, &NorthAmericaOffsets, &NorthAmericaCounts);
    RenderOpenGLBuffer(vaoSA, &SouthAmericaVertices, &SouthAmericaOffsets, &SouthAmericaCounts);
    RenderOpenGLBuffer(vaoOceania, &OceaniaVertices, &OceaniaOffsets, &OceaniaCounts);
    RenderOpenGLBuffer(vaoAntarctica, &AntarcticaVertices, &AntarcticaOffsets, &AntarcticaCounts);
    RenderOpenGLBuffer(vaoSevenSeas, &SevenseasVertices, &SevenseasOffsets, &SevenseasCounts);

    glBindVertexArray(0);
    glUseProgram(rlGetShaderIdDefault());

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

        std::weak_ptr<Entity> Object = std::dynamic_pointer_cast<Entity>(obj);

        Vector2 screenPos = ConvertWorldToScreenPos(Object.lock()->GetEntityLocation());
        Vector2 MousePos = ConvertMouseScreenPosToMapScreenPos(GetMousePosition());

        float scaledWidth = PlayerIcon.width * std::fmax(ZoomLevel, 0.035f);
        float scaledHeight = PlayerIcon.height * std::fmax(ZoomLevel, 0.035f);

        Rectangle destRec = {
            screenPos.x,
            screenPos.y,
            scaledWidth,
            scaledHeight
        };

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
                        { 0, 0, (float)PlayerIcon.width, (float)PlayerIcon.height }, // Source rectangle
                        destRec,                                                      // Destination rectangle
                        origin,                                                       // Rotate around the center
                        Object.lock()->GetEntityRotation(),                           // Rotation angle
                        ColorLookupState[static_cast<int>(State)]                     // Tint color
                    );
                }
                break;
            }
                                  // Handle Ship type
        }
    }
    auto Mark = ConvertWorldToScreenPos(MarkedPos);
    DrawCircle(Mark.x, Mark.y, 10, GREEN);

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
    if (CheckCollisionPointRec(GetMousePosition(), DestinationRect)) 
    {
        int Multiply = 100.f;
        if (IsKeyDown(KEY_LEFT_CONTROL))
        {
            Multiply = 1.f;
        }
        ZoomLevel += GetMouseWheelMove() * (0.0001f * Multiply);
        ZoomLevel = Clamp(ZoomLevel, 0.00005f, 10.0f);

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

    // Calling it manually
    // TODO: Later on should be changed to TickGroups
    RightClickMenu.TryLoad()->Tick(DeltaTime);
    SpeedMenu.TryLoad()->Tick(DeltaTime);

}

void Map::Init()
{
    // Load shader through raylib
    shader = LoadShader("src/shaders/basic.vs", "src/shaders/basic.fs");
    locMVP = glGetUniformLocation(shader.id, "uMVP");
    
    LoadBuffer(vaoAfrica, vboAfrica, &AfricaVertices);
    LoadBuffer(vaoEurope, vboEurope, &EuropeVertices);
    LoadBuffer(vaoAsia, vboAsia, &AsiaVertices);
    LoadBuffer(vaoNA, vboNA, &NorthAmericaVertices);
    LoadBuffer(vaoSA, vboSA, &SouthAmericaVertices);
    LoadBuffer(vaoOceania, vboOceania, &OceaniaVertices);
    LoadBuffer(vaoAntarctica, vboAntarctica, &AntarcticaVertices);
    LoadBuffer(vaoSevenSeas, vboSevenSeas, &SevenseasVertices);

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


    RightClickMenu = GetOutter()->m_ObjectFactory->NewObject<ContextMenu>();
    RightClickMenu.TryLoad()->SetDisplayName("Right Click Menu");
   
    SpeedMenu = GetOutter()->m_ObjectFactory->NewObject<ContextMenu>();
    SpeedMenu.TryLoad()->SetDisplayName("Speed Menu");

    ContextMenuEntry FullSpeedEntry;
    FullSpeedEntry.SetDisplayName("Full Speed");
    FullSpeedEntry.SetCallback([this](ContextMenuEntry* Self)
        {
            TrackedPlayer->SetSpeed(20);
        });

    ContextMenuEntry HalfSpeedEntry;
    HalfSpeedEntry.SetDisplayName("Half Speed");
    HalfSpeedEntry.SetCallback([this](ContextMenuEntry* Self)
        {
            TrackedPlayer->SetSpeed(12);
        });

    ContextMenuEntry SlowAheadEntry;
    SlowAheadEntry.SetDisplayName("Slow Ahead");
    SlowAheadEntry.SetCallback([this](ContextMenuEntry* Self)
        {
            TrackedPlayer->SetSpeed(8);
        });

    ContextMenuEntry DeadSlowEntry;
    DeadSlowEntry.SetDisplayName("Dead Slow");
    DeadSlowEntry.SetCallback([this](ContextMenuEntry* Self)
        {
            TrackedPlayer->SetSpeed(3);
        });

    ContextMenuEntry StopEntry;
    StopEntry.SetDisplayName("Stop");
    StopEntry.SetCallback([this](ContextMenuEntry* Self)
        {
            TrackedPlayer->SetSpeed(0);
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
            CameraWorldPosition = TrackedPlayer->GetEntityLocation();
            return;
        });

    ContextMenuEntry NewEntry2;
    NewEntry2.SetDisplayName("Set Course");
    NewEntry2.SetCallback([this](ContextMenuEntry* Self)
        {
            Vector2 localMouse = ConvertMouseScreenPosToMapScreenPos(RightClickMenu.TryLoad()->MousePosWhenConstructed);

            Vector2 MousePos = ConvertScreenPosToWorld(localMouse);
            Vector2 PlayerPos = TrackedPlayer->GetEntityLocation();
            std::cout << MousePos.x << " " << MousePos.y << std::endl;

            MarkedPos = MousePos;

            Vector2 delta = MousePos - PlayerPos;

            float angleRad = std::atan2(delta.y, delta.x);

            float angleDeg = angleRad * (180.0f / PI);
            angleDeg += 90.0f; // Offset so 0, is north
            angleDeg = std::fmod(angleDeg, 360.0f);
            if (angleDeg < 0.0f)
            {
                angleDeg += 360.0f;
            }

            TrackedPlayer->SetCourse(angleDeg);

        });


    ContextMenuEntry NewEntry3;
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
}

void Map::AddObjectToDraw(std::weak_ptr<IObject> Object) 
{
    if (Object.lock() != nullptr && *Object.lock()->GetStaticClass() << (Entity::StaticClass()))
    {
        std::cout << "Found Derived " << Object.lock()->GetDisplayName() << std::endl;
        std::shared_ptr<Player> PlayerPTR = std::dynamic_pointer_cast<Player>(Object.lock());
        if (PlayerPTR && TrackedPlayer == nullptr)
        {
            TrackedPlayer = PlayerPTR;
            auto EntityPtr = std::dynamic_pointer_cast<Entity>(Object.lock());
            ObjectsToDraw.push_back({ EntityPtr, {ObjectType::Submarine, ObjectState::EPlayer} });
        }
        else
        {
            auto EntityPtr = std::dynamic_pointer_cast<Entity>(Object.lock());
            ObjectsToDraw.push_back({ EntityPtr, {ObjectType::Submarine, ObjectState::EEnemy} });
        }

    }
}

void Map::OnKeyStroke(KeyboardKey Key, Vector2 MousePos)
{
    if (CheckCollisionPointRec(MousePos, DestinationRect)
        && Key == KEY_C)
    {
        CameraWorldPosition = TrackedPlayer->GetEntityLocation();
        ZoomLevel = 1.f;
    }
}

void Map::OnMouseButtonPressed(MouseButton Key, Vector2 MousePos)
{
    if (!CheckCollisionPointRec(MousePos, DestinationRect))
    {
        return;
    }


    if (Key == MOUSE_BUTTON_RIGHT)
    {
        RightClickMenu.TryLoad()->OnConstruct(MousePos);
    }
}

Vector2 Map::ConvertWorldToScreenPos(Vector2 worldPos) const
{
    // 1) Translate the world so that CameraWorldPosition maps to (0,0)
    float relX = worldPos.x - CameraWorldPosition.x;
    float relY = worldPos.y - CameraWorldPosition.y;

    // 2) Scale by zoom
    relX *= ZoomLevel;
    relY *= ZoomLevel;

    // 3) Raylib’s render‑target space has +Y down, so flip Y if your world +Y is up:
    //    (uncomment if needed)
    // relY = -relY;

    // 4) Now center it inside your render target:
    //    camera‑origin (0,0) → (width/2, height/2)
    Vector2 screenPos;
    screenPos.x = relX + (DestinationRect.width * 0.5f);
    screenPos.y = relY + (DestinationRect.height * 0.5f);

    return screenPos;
}

Vector2 Map::ConvertScreenPosToWorld(Vector2 VectorToConver) const
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

    // 4) Translate so that camera‑origin returns to its world position:
    Vector2 worldPos;
    worldPos.x = centeredX + CameraWorldPosition.x;
    worldPos.y = centeredY + CameraWorldPosition.y;

    return worldPos;
}

inline Vector2 Map::ConvertTextureSizeToWorldSize(TextureResource* UsedTexture, Vector2 SizeInMeters)
{
    return { SizeInMeters.x / UsedTexture->width, SizeInMeters.y / UsedTexture->height };
}

void Map::LoadBuffer(unsigned int& VAO, unsigned int& VBO, const std::vector<float>* PointArray) const
{
    // VBO and VAO setup
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Upload vertex data
    glBufferData(GL_ARRAY_BUFFER,
        PointArray->size() * sizeof((*PointArray)[0]),
        PointArray->data(),
        GL_STATIC_DRAW);

    // Set vertex attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
}

void Map::RenderOpenGLBuffer(unsigned int& VAO, const std::vector<float>* PointArray, const std::vector<unsigned int>* Offset, const std::vector<unsigned int>* Counts) const
{


    // Convert your std::vector<unsigned int> into the proper GL types:
    GLsizei drawCount = GLsizei(Counts->size());
    std::vector<GLint>   firsts(Offset->begin(), Offset->end());
    std::vector<GLsizei> counts(Counts->begin(), Counts->end());

    glBindVertexArray(VAO);
    glMultiDrawArrays(
        GL_LINE_LOOP,
        firsts.data(),    // array of starting vertex indices
        counts.data(),    // array of vertex counts per loop
        drawCount         // number of loops
    );
}

Matrix Map::GetViewProjectionMatrix() const
{
    Matrix S = MatrixScale(ZoomLevel, ZoomLevel, 1);
    Matrix T = MatrixTranslate(-CameraWorldPosition.x, -CameraWorldPosition.y, 0);
    Matrix O = MatrixTranslate(
        DestinationRect.width / 2.0f,
        DestinationRect.height / 2.0f,
        0
    );

    return MatrixMultiply(MatrixMultiply(T, S), O);
}

Matrix Map::GetOpenGLProjectionMatrix() const
{
    float visibleWidth = DestinationRect.width / ZoomLevel;
    float visibleHeight = DestinationRect.height / ZoomLevel;

    // Bounds
    float left = CameraWorldPosition.x - visibleWidth / 2.0f;
    float right = CameraWorldPosition.x + visibleWidth / 2.0f;
    float bottom = -CameraWorldPosition.y - visibleHeight / 2.0f;
    float top = -CameraWorldPosition.y + visibleHeight / 2.0f;

    // OpenGL Matrix Projection
    Matrix projection = { 0 };
    projection.m0 = 2.0f / (right - left); // X scale
    projection.m5 = 2.0f / (top - bottom); // Y scale
    projection.m12 = -(right + left) / (right - left); // X translation
    projection.m13 = -(top + bottom) / (top - bottom); // Y translation
    projection.m15 = 1.0f;

    return projection;
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
    return Vector2Clamp(Vector2Subtract(MouseAbsolutePos, DestinationVector), Vector2{ 0,0 }, DestinationVectorSize);
}