#include "Base/SonarTypes.hpp"
#include "Rendering/GraphicSubSystem.hpp"
#include "Base/GameInstance.h"
#include <algorithm>
#include <span>
#include "Rendering/RenderBufferArrayLine.hpp"
#include <stdexcept>

static void require(bool value) { if (!value) throw std::runtime_error("Graphics contract failed"); }
int main() {
    using namespace Sonar;
    SubSystemRegistry registry;
    registry.RegisterSubSystem(std::make_unique<GraphicSubSystem>());
    auto& graphics = registry.GetSubSystem<GraphicSubSystem>();
    GameInstance::GetRuntimeContext().Configure(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                               {}, {}, {}, {}, {}, {}, &registry);
    graphics.SetConfigFlags(FLAG_WINDOW_HIDDEN);
    graphics.InitWindow(64, 64, "Engine graphics contract");
    {
        auto image = graphics.GenImageColor(8, 8, GREEN);
        require(image.IsValid() && image.width * image.height == 64 && static_cast<Color*>(image.data)[0].g == GREEN.g);
        auto texture = graphics.LoadTextureFromImage(image);
        require(texture.IsValid() && texture.width == 8 && texture.height == 8);
        auto copy = texture;
        std::fill_n(static_cast<Color*>(image.data), 64, RED);
        graphics.UpdateTexture(texture, image.data);
        auto readback = graphics.LoadImageFromTexture(copy);
        require(static_cast<Color*>(readback.data)[0].r == RED.r && static_cast<Color*>(readback.data)[0].g == RED.g);
        auto target = graphics.LoadRenderTexture(8, 8);
        graphics.BeginTextureMode(target); graphics.ClearBackground(BLUE); graphics.EndTextureMode();
        auto rendered = graphics.LoadImageFromTexture(target.texture);
        require(static_cast<Color*>(rendered.data)[0].b == BLUE.b);
        graphics.UnloadTexture(texture);
        require(!texture.IsValid());
        require(copy.id != 0); // Non-owning copies are not automatically invalidated.
        copy = {}; // The GPU resource was already unloaded through texture.
        graphics.UnloadRenderTexture(target);
        require(!target.IsValid());
        auto linesTarget = graphics.LoadRenderTexture(64, 64);
        auto shader = graphics.LoadShader(CONTRACT_SHADER_DIR "/line.vs", CONTRACT_SHADER_DIR "/line.fs");
        require(shader.IsValid());
        RenderBufferArrayLine lines;
        lines.AddVertices({-.5f,-.5f, .5f,-.5f, .5f,.5f, -.5f,.5f});
        lines.AddOffsets({0}); lines.AddCounts({4}); lines.LoadBuffer();
        Matrix identity{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        graphics.BeginTextureMode(linesTarget);
        graphics.ClearBackground(BLACK);
        lines.RenderBuffer(shader, identity, 0);
        graphics.DrawRectangle(30,30,4,4,GREEN);
        graphics.EndTextureMode();
        auto lineImage = graphics.LoadImageFromTexture(linesTarget.texture);
        int redPixels=0, greenPixels=0;
        for (auto pixel : std::span(static_cast<const Color*>(lineImage.data), lineImage.width * lineImage.height)) {
            if (pixel.r > 200 && pixel.g < 30) ++redPixels;
            if (pixel.g > 150 && pixel.r < 30) ++greenPixels;
        }
        require(redPixels > 50 && greenPixels >= 16);
        graphics.UnloadImage(image);
        graphics.UnloadImage(readback);
        graphics.UnloadImage(rendered);
        graphics.UnloadImage(lineImage);
        graphics.UnloadRenderTexture(linesTarget);
        graphics.UnloadShader(shader);
    }
    graphics.CloseWindow();
}
