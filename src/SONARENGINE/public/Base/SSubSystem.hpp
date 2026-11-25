#pragma once

#include "Base/Export.hpp"
#include "Base/Macros.h"

// Engine service interface, independent of the IObject/reflection hierarchy.
class SONARENGINE_API SSubSystem
{
    ROOTBODY(SSubSystem)
public:
    SSubSystem() = default;
    virtual ~SSubSystem();
    SSubSystem(const SSubSystem&) = delete;
    SSubSystem& operator=(const SSubSystem&) = delete;

    void Initialize()
    {
        if (!m_Initialized)
        {
            OnInitialize();
            m_Initialized = true;
        }
    }

    void Shutdown()
    {
        if (m_Initialized)
        {
            OnShutdown();
            m_Initialized = false;
        }
    }

    void Update() { if (m_Initialized) OnUpdate(); }
    bool IsInitialized() const noexcept { return m_Initialized; }

protected:
    virtual void OnInitialize() = 0;
    virtual void OnShutdown() = 0;
    virtual void OnUpdate() = 0;

private:
    bool m_Initialized = false;
};
