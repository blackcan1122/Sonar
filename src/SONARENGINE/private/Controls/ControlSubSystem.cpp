#include "Base/Logging.h"
#include "Base/SonarTypeConversions.hpp"
#include "Controls/ControlSubSystem.hpp"

using Sonar::Backend::FromRaylib;

void ControlSubSystem::OnInitialize()
{
    LOG_INFO("ControlSubSystem initialized");
}

void ControlSubSystem::OnShutdown()
{
    LOG_INFO("ControlSubSystem shutdown");
}

void ControlSubSystem::OnUpdate() {}

bool ControlSubSystem::IsKeyPressed(int key) { auto result=::IsKeyPressed(key); return result; }

bool ControlSubSystem::IsKeyDown(int key) { auto result=::IsKeyDown(key); return result; }

int ControlSubSystem::GetKeyPressed(void) { auto result=::GetKeyPressed(); return result; }

int ControlSubSystem::GetCharPressed(void) { auto result=::GetCharPressed(); return result; }

bool ControlSubSystem::IsMouseButtonPressed(int button) { auto result=::IsMouseButtonPressed(button); return result; }

bool ControlSubSystem::IsMouseButtonDown(int button) { auto result=::IsMouseButtonDown(button); return result; }

bool ControlSubSystem::IsMouseButtonReleased(int button) { auto result=::IsMouseButtonReleased(button); return result; }

Sonar::Vector2 ControlSubSystem::GetMousePosition(void) { auto result=FromRaylib(::GetMousePosition()); return result; }

float ControlSubSystem::GetMouseWheelMove(void) { auto result=::GetMouseWheelMove(); return result; }

bool ControlSubSystem::IsCursorOnScreen(void) { auto result=::IsCursorOnScreen(); return result; }

void ControlSubSystem::SetMouseCursor(int cursor) { ::SetMouseCursor(cursor); }
