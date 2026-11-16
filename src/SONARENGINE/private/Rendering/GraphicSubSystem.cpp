#include "Rendering/GraphicSubSystem.hpp"

void GraphicSubSystem::OnInitialize()
{
    LOG_INFO("GraphicSubSystem initialized");
}

void GraphicSubSystem::OnShutdown()
{
    LOG_INFO("GraphicSubSystem shutdown");
}

void GraphicSubSystem::OnUpdate()
{
    LOG_INFO("GraphicSubSystem updated");
}
