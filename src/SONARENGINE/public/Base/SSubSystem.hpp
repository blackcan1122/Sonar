#pragma once

#include "Base/Core.h"

class SSubSystem
{
    mutable bool m_Initialized = false;
public:

    SSubSystem() = default;
    virtual ~SSubSystem() = default;

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

    void Update()
    {
        OnUpdate();
    }

protected:
    virtual void OnInitialize(){};
    virtual void OnShutdown(){};
    virtual void OnUpdate(){};
};
