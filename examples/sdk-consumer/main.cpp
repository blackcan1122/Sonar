#include "Base/Core.h"
#include "Base/GameInstance.h"
#include "Base/GameMode.h"
#include "Base/Object.hpp"
#include "Base/EventDispatcher.hpp"
#include "Base/Math.hpp"
#include "Events/UIEvent.h"
#include "Events/MouseEvent.hpp"
#include "Events/AllPurposeEvent.h"
#include "Events/WindowResizeData.hpp"
#include "UI/Button.h"
#include "Rendering/GraphicSubSystem.hpp"
#include "Controls/ControlSubSystem.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace
{
constexpr int InitialScreenWidth = 1280;
constexpr int InitialScreenHeight = 720;

class PongPaddle final : public Object
{
    AUTOBODY(PongPaddle, Object)

public:
    PongPaddle(Sonar::Rectangle rectangle, Sonar::KeyboardKey upKey, Sonar::KeyboardKey downKey)
        : paddleRect(rectangle), up(upKey), down(downKey)
    {
        m_Name = "PongPaddle";
    }

    void SetArenaHeight(int height)
    {
        arenaHeight = height;
    }

    void SetHorizontalPosition(float x)
    {
        paddleRect.x = x;
    }

    void Tick(float deltaTime) override
    {
        auto& controls = GetOutter()->GetRuntimeContext().GetSubSystem<ControlSubSystem>();
        if (controls.IsKeyDown(up)) paddleRect.y -= paddleSpeed * deltaTime;
        if (controls.IsKeyDown(down)) paddleRect.y += paddleSpeed * deltaTime;
        paddleRect.y = std::clamp(paddleRect.y, 20.f, static_cast<float>(arenaHeight - paddleRect.height - 20));

        GetOutter()->GetRuntimeContext().GetSubSystem<GraphicSubSystem>().DrawRectangleRec(paddleRect, Sonar::WHITE);
    }

    const Sonar::Rectangle& GetRectangle() const { return paddleRect; }

private:
    Sonar::Rectangle paddleRect;
    Sonar::KeyboardKey up;
    Sonar::KeyboardKey down;
    float paddleSpeed = 420.f;
    int arenaHeight = InitialScreenHeight;
};

class PongBall final : public Object
{
    AUTOBODY(PongBall, Object)

public:
    PongBall()
    {
        m_Name = "PongBall";
        Reset(360.f);
    }

    void SetArenaSize(int width, int height)
    {
        arenaWidth = width;
        arenaHeight = height;
    }

    void Reset(float velocityX)
    {
        position = {arenaWidth * 0.5f, arenaHeight * 0.5f};
        velocity = {velocityX, (std::rand() % 2 ? 260.f : -260.f)};
    }

    bool Touches(const Sonar::Rectangle& rectangle) const
    {
        const float closestX = std::clamp(position.x, rectangle.x, rectangle.x + rectangle.width);
        const float closestY = std::clamp(position.y, rectangle.y, rectangle.y + rectangle.height);
        const float differenceX = position.x - closestX;
        const float differenceY = position.y - closestY;
        return differenceX * differenceX + differenceY * differenceY <= radius * radius;
    }

    void Tick(float deltaTime) override
    {
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;

        if (position.y - radius <= 10.f)
        {
            position.y = 10.f + radius;
            velocity.y = std::abs(velocity.y);
        }
        if (position.y + radius >= arenaHeight - 10.f)
        {
            position.y = arenaHeight - 10.f - radius;
            velocity.y = -std::abs(velocity.y);
        }

        auto& graphics = GetOutter()->GetRuntimeContext().GetSubSystem<GraphicSubSystem>();
        graphics.DrawCircle(static_cast<int>(position.x), static_cast<int>(position.y), radius, Sonar::YELLOW);
    }

    Sonar::Vector2 position{InitialScreenWidth * 0.5f, InitialScreenHeight * 0.5f};
    Sonar::Vector2 velocity{360.f, 260.f};
    float radius = 10.f;

private:
    int arenaWidth = InitialScreenWidth;
    int arenaHeight = InitialScreenHeight;
};

class PongMenuMode final : public GameMode
{
public:
    PongMenuMode() { SetName("Menu"); }

    ~PongMenuMode() override
    {
        auto& context = GetRuntimeContext();
        context.GetMouseDispatcher().RemoveListener("PongMenuMouse", MouseEvent::StaticClass());
        context.GetAllPurposeDispatcher().RemoveListener("PongMenuResize", AllPurposeEvent::StaticClass());
    }

    void BeginPlay() override
    {
        menuDispatcher = std::make_shared<EventDispatcher>();
        menuDispatcher->m_Name = "PongMenuDispatcher";

        menuDispatcher->AddListener("PlayButton", UIEvent::StaticClass(), [this](std::shared_ptr<IEvent> event)
        {
            const auto uiEvent = std::dynamic_pointer_cast<UIEvent>(event);
            if (uiEvent && uiEvent->Payload == "Play")
                GetRuntimeContext().RequestGameModeChange("Pong");
        });

        menuDispatcher->AddListener("ExitButton", UIEvent::StaticClass(), [this](std::shared_ptr<IEvent> event)
        {
            const auto uiEvent = std::dynamic_pointer_cast<UIEvent>(event);
            if (uiEvent && uiEvent->Payload == "Exit")
                GetRuntimeContext().RequestShutdown();
        });

        playButton = NewObject<Button>();
        playButton.TryLoad()->Construct({490.f, 290.f, 300.f, 70.f}, "PLAY", Sonar::GREEN)
            .UpdateTextColor(Sonar::WHITE)
            .UpdateFontSize(30)
            .CenterText()
            .SetEventDispatcher(menuDispatcher)
            .SetEventPayload("Play")
            .OnHover([](Button* button) { button->UpdateColor(Sonar::LIME); })
            .OnHoverLeave([](Button* button) { button->UpdateColor(Sonar::GREEN); });

        exitButton = NewObject<Button>();
        exitButton.TryLoad()->Construct({490.f, 380.f, 300.f, 70.f}, "EXIT", Sonar::RED)
            .UpdateTextColor(Sonar::WHITE)
            .UpdateFontSize(30)
            .CenterText()
            .SetEventDispatcher(menuDispatcher)
            .SetEventPayload("Exit")
            .OnHover([](Button* button) { button->UpdateColor(Sonar::ORANGE); })
            .OnHoverLeave([](Button* button) { button->UpdateColor(Sonar::RED); });

        GetRuntimeContext().GetMouseDispatcher().AddListener("PongMenuMouse", MouseEvent::StaticClass(),
            [this](std::shared_ptr<IEvent> event)
            {
                const auto mouseEvent = std::dynamic_pointer_cast<MouseEvent>(event);
                if (!mouseEvent || mouseEvent->KeyPressed != Sonar::MOUSE_BUTTON_LEFT)
                    return;

                if (Sonar::CheckCollisionPointRec(mouseEvent->MousePos, {490.f, 290.f, 300.f, 70.f}))
                    GetRuntimeContext().RequestGameModeChange("Pong");
                if (Sonar::CheckCollisionPointRec(mouseEvent->MousePos, {490.f, 380.f, 300.f, 70.f}))
                    GetRuntimeContext().RequestShutdown();
            });

        GetRuntimeContext().GetAllPurposeDispatcher().AddListener("PongMenuResize", AllPurposeEvent::StaticClass(),
            [this](std::shared_ptr<IEvent> event)
            {
                const auto allPurpose = std::dynamic_pointer_cast<AllPurposeEvent>(event);
                if (!allPurpose || !allPurpose->Payload || allPurpose->Payload->GetStaticClass() != WindowResizeData::StaticClass())
                    return;

                const auto resize = std::static_pointer_cast<WindowResizeData>(allPurpose->Payload);
                const float centerX = resize->width * 0.5f;
                playButton.TryLoad()->UpdateButtonPosition(centerX - 150.f, resize->height * 0.40f);
                exitButton.TryLoad()->UpdateButtonPosition(centerX - 150.f, resize->height * 0.53f);
            });
    }

    void Update() override
    {
        auto& graphics = GetRuntimeContext().GetSubSystem<GraphicSubSystem>();
        auto& controls = GetRuntimeContext().GetSubSystem<ControlSubSystem>();

        graphics.ClearBackground(Sonar::DARKBLUE);

        const char* title = "SONAR PONG";
        graphics.DrawText(title, (GetRuntimeContext().GetScreenWidth() - graphics.MeasureText(title, 48)) / 2, 140, 48, Sonar::WHITE);

        const char* controlsText = "Left paddle: W / S     Right paddle: UP / DOWN";
        graphics.DrawText(controlsText, (GetRuntimeContext().GetScreenWidth() - graphics.MeasureText(controlsText, 20)) / 2, 230, 20, Sonar::LIGHTGRAY);

        if (controls.IsKeyPressed(Sonar::KEY_ESCAPE))
            GetRuntimeContext().RequestShutdown();

        GameMode::Update();
    }

private:
    std::shared_ptr<EventDispatcher> menuDispatcher;
    SoftObjectPath<Button> playButton;
    SoftObjectPath<Button> exitButton;
};

class PongGameMode final : public GameMode
{
public:
    PongGameMode() { SetName("Pong"); }

    ~PongGameMode() override
    {
        GetRuntimeContext().GetMouseDispatcher().RemoveListener("PongGameMouse", MouseEvent::StaticClass());
        GetRuntimeContext().GetAllPurposeDispatcher().RemoveListener("PongGameResize", AllPurposeEvent::StaticClass());
    }

    void BeginPlay() override
    {
        arenaWidth = GetRuntimeContext().GetScreenWidth();
        arenaHeight = GetRuntimeContext().GetScreenHeight();

        leftPaddle = NewObject<PongPaddle>(Sonar::Rectangle{40.f, 280.f, 24.f, 140.f}, Sonar::KEY_W, Sonar::KEY_S);
        rightPaddle = NewObject<PongPaddle>(Sonar::Rectangle{static_cast<float>(arenaWidth - 64), 280.f, 24.f, 140.f}, Sonar::KEY_UP, Sonar::KEY_DOWN);
        ball = NewObject<PongBall>();
        ball.TryLoad()->SetArenaSize(arenaWidth, arenaHeight);

        leftPaddle.TryLoad()->SetArenaHeight(arenaHeight);
        rightPaddle.TryLoad()->SetArenaHeight(arenaHeight);

        GetRuntimeContext().GetMouseDispatcher().AddListener("PongGameMouse", MouseEvent::StaticClass(),
            [this](std::shared_ptr<IEvent> event)
            {
                const auto mouseEvent = std::dynamic_pointer_cast<MouseEvent>(event);
                if (mouseEvent && mouseEvent->KeyPressed == Sonar::MOUSE_BUTTON_LEFT)
                {
                    if (auto pongBall = ball.TryLoad())
                        pongBall->Reset(pongBall->velocity.x < 0.f ? -360.f : 360.f);
                }
            });

        GetRuntimeContext().GetAllPurposeDispatcher().AddListener("PongGameResize", AllPurposeEvent::StaticClass(),
            [this](std::shared_ptr<IEvent> event)
            {
                const auto allPurpose = std::dynamic_pointer_cast<AllPurposeEvent>(event);
                if (!allPurpose || !allPurpose->Payload || allPurpose->Payload->GetStaticClass() != WindowResizeData::StaticClass())
                    return;

                const auto resize = std::static_pointer_cast<WindowResizeData>(allPurpose->Payload);
                arenaWidth = resize->width;
                arenaHeight = resize->height;

                if (auto left = leftPaddle.TryLoad()) left->SetArenaHeight(arenaHeight);
                if (auto right = rightPaddle.TryLoad())
                {
                    right->SetArenaHeight(arenaHeight);
                    right->SetHorizontalPosition(static_cast<float>(arenaWidth - 64));
                }
                if (auto pongBall = ball.TryLoad())
                    pongBall->SetArenaSize(arenaWidth, arenaHeight);
            });
    }

    void Update() override
    {
        auto& graphics = GetRuntimeContext().GetSubSystem<GraphicSubSystem>();
        auto& controls = GetRuntimeContext().GetSubSystem<ControlSubSystem>();
        graphics.ClearBackground(Sonar::BLACK);

        auto left = leftPaddle.TryLoad();
        auto right = rightPaddle.TryLoad();
        auto pongBall = ball.TryLoad();

        if (pongBall && left && right)
        {
            if (pongBall->Touches(left->GetRectangle()) && pongBall->velocity.x < 0.f)
            {
                pongBall->position.x = left->GetRectangle().x + left->GetRectangle().width + pongBall->radius;
                pongBall->velocity.x = std::abs(pongBall->velocity.x);
            }
            if (pongBall->Touches(right->GetRectangle()) && pongBall->velocity.x > 0.f)
            {
                pongBall->position.x = right->GetRectangle().x - pongBall->radius;
                pongBall->velocity.x = -std::abs(pongBall->velocity.x);
            }

            if (pongBall->position.x < -pongBall->radius)
                pongBall->Reset(-360.f);
            if (pongBall->position.x > arenaWidth + pongBall->radius)
                pongBall->Reset(360.f);
        }

        for (int y = 20; y < arenaHeight; y += 30)
            graphics.DrawRectangle(arenaWidth / 2 - 2, y, 4, 16, Sonar::LIGHTGRAY);

        if (controls.IsKeyPressed(Sonar::KEY_ESCAPE))
            GetRuntimeContext().RequestGameModeChange("Menu");

        GameMode::Update();
    }

private:
    SoftObjectPath<PongPaddle> leftPaddle;
    SoftObjectPath<PongPaddle> rightPaddle;
    SoftObjectPath<PongBall> ball;
    int arenaWidth = InitialScreenWidth;
    int arenaHeight = InitialScreenHeight;
};
}

int main()
{
    std::cout << "Registering Menu and Pong game modes..." << std::endl;
    GameInstance::RegisterGameMode("Menu", []() { return new PongMenuMode(); });
    GameInstance::RegisterGameMode("Pong", []() { return new PongGameMode(); });
    
    GameInstance::SetStartupGameMode("LEEEL");
    GameInstance::SetStartupGameMode("Menu");
    std::cout << "Calling InitGameInstance..." << std::endl;

    GameInstance::InitGameInstance(WindowProperties(InitialScreenWidth, InitialScreenHeight, 60, false, false));
    return 0;
}
