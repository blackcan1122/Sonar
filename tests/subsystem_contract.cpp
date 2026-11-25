#include "Base/SubSystemRegistry.hpp"
#include "Base/EngineRuntimeContext.hpp"
#include "Base/Object.hpp"
#include "Rendering/GraphicSubSystem.hpp"
#include <stdexcept>
#include <type_traits>
#include <vector>

static void require(bool Value) { if (!Value) throw std::runtime_error("Subsystem contract failed"); }

template<typename Exception, typename F>
static void requireThrows(F&& Action)
{
    try { Action(); }
    catch (const Exception&) { return; }
    throw std::runtime_error("Expected subsystem error was not reported");
}

class ConsumerSubSystem : public SSubSystem
{
    AUTOBODY(ConsumerSubSystem, SSubSystem)
public:
    ConsumerSubSystem(std::vector<int>& Events, int Id) : m_Events(Events), m_Id(Id) {}
    ~ConsumerSubSystem() override { m_Events.push_back(m_Id + 30); }
protected:
    void OnInitialize() override { m_Events.push_back(m_Id); }
    void OnUpdate() override { m_Events.push_back(m_Id + 10); }
    void OnShutdown() override { m_Events.push_back(m_Id + 20); }
private:
    std::vector<int>& m_Events;
    int m_Id;
};

class OtherSubSystem : public ConsumerSubSystem
{
    AUTOBODY(OtherSubSystem, ConsumerSubSystem)
public:
    using ConsumerSubSystem::ConsumerSubSystem;
};

class FailingSubSystem : public SSubSystem
{
    AUTOBODY(FailingSubSystem, SSubSystem)
protected:
    void OnInitialize() override { throw std::runtime_error("Initialization failure"); }
    void OnUpdate() override {}
    void OnShutdown() override {}
};

int main()
{
    static_assert(std::is_abstract_v<SSubSystem>);
    static_assert(!std::is_base_of_v<IObject, SSubSystem>);
    require(GraphicSubSystem::StaticClass() == SClass::FindClass("GraphicSubSystem"));
    require(*GraphicSubSystem::StaticClass() << SSubSystem::StaticClass());
    require(!(*GraphicSubSystem::StaticClass() << IObject::StaticClass()));
    std::vector<int> events;
    {
        SubSystemRegistry registry;
        EngineRuntimeContext context;
        require(context.FindSubSystem(ConsumerSubSystem::StaticClass()) == nullptr);
        requireThrows<std::logic_error>([&] { context.GetSubSystem<ConsumerSubSystem>(); });
        context.Configure(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                          {}, {}, {}, {}, {}, {}, &registry);
        registry.RegisterSubSystem(std::make_unique<ConsumerSubSystem>(events, 1));
        auto& first = registry.GetSubSystem<ConsumerSubSystem>();
        require(&first == &context.GetSubSystem<ConsumerSubSystem>());
        require(&first == &context.GetSubSystem(ConsumerSubSystem::StaticClass()));
        require(first.GetStaticClass() == ConsumerSubSystem::StaticClass());
        requireThrows<std::out_of_range>([&] { registry.GetSubSystem<OtherSubSystem>(); });
        requireThrows<std::invalid_argument>([&] { registry.GetSubSystem<OtherSubSystem>(ConsumerSubSystem::StaticClass()); });
        requireThrows<std::invalid_argument>([&] { registry.RegisterSubSystem(nullptr); });
        requireThrows<std::invalid_argument>([&] { registry.RegisterSubSystem(std::make_unique<ConsumerSubSystem>(events, 9)); });
        events.clear();
        registry.Update();
        require(events.empty());
        registry.Initialize(); registry.Initialize();
        require(events == std::vector<int>{1});
        registry.RegisterSubSystem(std::make_unique<OtherSubSystem>(events, 2));
        require(registry.GetSubSystem<OtherSubSystem>().IsInitialized());
        registry.Update();
        registry.Shutdown(); registry.Shutdown();
        require(events == std::vector<int>({1,2,11,12,22,21}));
        registry.Update();
        require(events.size() == 6);
    }
    require(events.size() == 8); // Both interface-owned instances were destroyed.
    events.clear();
    {
        SubSystemRegistry registry;
        registry.RegisterSubSystem(std::make_unique<ConsumerSubSystem>(events, 1));
        registry.RegisterSubSystem(std::make_unique<FailingSubSystem>());
        requireThrows<std::runtime_error>([&] { registry.Initialize(); });
        require(!registry.GetSubSystem<ConsumerSubSystem>().IsInitialized());
        require(events == std::vector<int>({1,21}));
    }
}
