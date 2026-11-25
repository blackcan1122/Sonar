#include "Base/GameInstance.h"
#include "Base/GameMode.h"
#include "Rendering/GraphicSubSystem.hpp"
#include "Controls/ControlSubSystem.hpp"
#include "Audio/AudioSubSystem.hpp"
#include <stdexcept>

static bool BeganPlay = false;
static bool StoppedService = false;
static bool DestroyedService = false;
static void require(bool Value) { if (!Value) throw std::runtime_error("Runtime contract failed"); }

class ConsumerService : public SSubSystem
{
    AUTOBODY(ConsumerService, SSubSystem)
public:
    explicit ConsumerService(std::unique_ptr<int> Value) : m_Value(std::move(Value)) {}
    ~ConsumerService() override { DestroyedService = true; }
protected:
    void OnInitialize() override { require(m_Value && *m_Value == 42); }
    void OnUpdate() override {}
    void OnShutdown() override
    {
        auto& context = GameInstance::GetRuntimeContext();
        require(context.GetSubSystem<GraphicSubSystem>().IsInitialized());
        require(context.GetSubSystem<AudioSubSystem>().IsInitialized());
        StoppedService = true;
    }
private:
    std::unique_ptr<int> m_Value;
};

class SmokeGameMode : public GameMode
{
public:
    void BeginPlay() override
    {
        auto* instance = GameInstance::GetInstance();
        auto& context = GetRuntimeContext();
        auto& graphics = context.GetSubSystem<GraphicSubSystem>();
        require(&graphics == &instance->GetSubSystem<GraphicSubSystem>());
        require(&graphics == &instance->GetSubSystem(GraphicSubSystem::StaticClass()));
        require(graphics.IsInitialized());
        require(context.GetSubSystem<ControlSubSystem>().IsInitialized());
        require(context.GetSubSystem<AudioSubSystem>().IsInitialized());
        auto& service = instance->RegisterSubSystem<ConsumerService>(std::make_unique<int>(42));
        require(&service == &context.GetSubSystem<ConsumerService>());
        require(context.GetSubSystem<ConsumerService>().IsInitialized());
        BeganPlay = true;
    }
    void Update() override { GetRuntimeContext().RequestShutdown(); }
};

int main()
{
    GameInstance::RegisterGameMode("RuntimeSmoke", [] { return new SmokeGameMode(); });
    GameInstance::SetStartupGameMode("RuntimeSmoke");
    GameInstance::InitGameInstance(WindowProperties(64, 64, 60, false, false));
    require(BeganPlay && StoppedService && DestroyedService);
    require(GameInstance::GetRuntimeContext().FindSubSystem(GraphicSubSystem::StaticClass()) == nullptr);
}
