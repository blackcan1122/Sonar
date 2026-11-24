#pragma once

#include "Base/SSubSystem.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

class SONARENGINE_API SubSystemRegistry
{
public:
    SubSystemRegistry() = default;
    ~SubSystemRegistry();
    SubSystemRegistry(const SubSystemRegistry&) = delete;
    SubSystemRegistry& operator=(const SubSystemRegistry&) = delete;

    // Registration order determines initialization/update order; shutdown reverses it.
    void RegisterSubSystem(std::unique_ptr<SSubSystem> SubSystem);
    SSubSystem* FindSubSystem(const SClass* Class) const;
    SSubSystem& GetSubSystem(const SClass* Class) const;

    template<typename T>
    T& GetSubSystem(const SClass* Class = T::StaticClass()) const
    {
        static_assert(std::is_base_of_v<SSubSystem, T>);
        auto* SubSystem = dynamic_cast<T*>(&GetSubSystem(Class));
        if (!SubSystem) throw std::invalid_argument("Subsystem type mismatch");
        return *SubSystem;
    }

    void Initialize();
    void Update();
    void Shutdown();

private:
    std::vector<std::pair<const SClass*, std::unique_ptr<SSubSystem>>> m_SubSystems;
    bool m_Initialized = false;
    bool m_InLifecycle = false;
};
