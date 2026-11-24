#include "Base/SubSystemRegistry.hpp"

namespace {
class LifecycleGuard {
public:
    explicit LifecycleGuard(bool& Active) : m_Active(Active)
    {
        if (m_Active) throw std::logic_error("Recursive subsystem lifecycle operation");
        m_Active = true;
    }
    ~LifecycleGuard() { m_Active = false; }
private:
    bool& m_Active;
};
}

SubSystemRegistry::~SubSystemRegistry() { Shutdown(); }

void SubSystemRegistry::RegisterSubSystem(std::unique_ptr<SSubSystem> SubSystem)
{
    if (m_InLifecycle) throw std::logic_error("Cannot register subsystems during lifecycle callbacks");
    if (!SubSystem) throw std::invalid_argument("Subsystem registration requires an instance");
    const SClass* Class = SubSystem->GetStaticClass();
    if (!Class || Class == SSubSystem::StaticClass())
        throw std::invalid_argument("Subsystem must declare its own class metadata with AUTOBODY");
    if (FindSubSystem(Class)) throw std::invalid_argument("Subsystem already registered: " + Class->ClassName);
    m_SubSystems.emplace_back(Class, std::move(SubSystem));
    if (m_Initialized)
    {
        LifecycleGuard Guard(m_InLifecycle);
        try { m_SubSystems.back().second->Initialize(); }
        catch (...) { m_SubSystems.pop_back(); throw; }
    }
}

SSubSystem* SubSystemRegistry::FindSubSystem(const SClass* Class) const
{
    for (const auto& Entry : m_SubSystems)
        if (Entry.first == Class) return Entry.second.get();
    return nullptr;
}

SSubSystem& SubSystemRegistry::GetSubSystem(const SClass* Class) const
{
    auto* SubSystem = FindSubSystem(Class);
    if (!SubSystem) throw std::out_of_range("Subsystem class is not registered");
    return *SubSystem;
}

void SubSystemRegistry::Initialize()
{
    if (m_Initialized) return;
    LifecycleGuard Guard(m_InLifecycle);
    try
    {
        for (auto& Entry : m_SubSystems) Entry.second->Initialize();
        m_Initialized = true;
    }
    catch (...)
    {
        for (auto It = m_SubSystems.rbegin(); It != m_SubSystems.rend(); ++It) It->second->Shutdown();
        throw;
    }
}

void SubSystemRegistry::Update()
{
    if (!m_Initialized) return;
    LifecycleGuard Guard(m_InLifecycle);
    for (auto& Entry : m_SubSystems) Entry.second->Update();
}

void SubSystemRegistry::Shutdown()
{
    LifecycleGuard Guard(m_InLifecycle);
    for (auto It = m_SubSystems.rbegin(); It != m_SubSystems.rend(); ++It) It->second->Shutdown();
    m_Initialized = false;
}
