#pragma once

#include "Base/SSubSystem.hpp"

class GraphicSubSystem : public SSubSystem
{
public:
    GraphicSubSystem() = default;
    virtual ~GraphicSubSystem() = default;

    void OnInitialize() override;
    void OnShutdown() override;
    void OnUpdate() override;
};