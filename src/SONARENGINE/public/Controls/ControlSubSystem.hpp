#pragma once

#include "Base/SSubSystem.hpp"
#include "Base/SonarTypes.hpp"

class SONARENGINE_API ControlSubSystem : public SSubSystem
{
    AUTOBODY(ControlSubSystem, SSubSystem)
public:
    ControlSubSystem() = default;
    virtual ~ControlSubSystem() = default;

    void OnInitialize() override;
    void OnShutdown() override;
    void OnUpdate() override;

    bool IsKeyPressed(int key);
    bool IsKeyDown(int key);
    int GetKeyPressed(void);
    int GetCharPressed(void);
    bool IsMouseButtonPressed(int button);
    bool IsMouseButtonDown(int button);
    bool IsMouseButtonReleased(int button);
    Sonar::Vector2 GetMousePosition(void);
    float GetMouseWheelMove(void);
    bool IsCursorOnScreen(void);
    void SetMouseCursor(int cursor);
};
