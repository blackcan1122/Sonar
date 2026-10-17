#include "Rendering/Graphics.hpp"
#include "Rendering/RenderBufferArrayLine.hpp"
#include <stdexcept>

static void require(bool value) { if (!value) throw std::runtime_error("Graphics contract failed"); }
int main() {
    using namespace Sonar;
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(64, 64, "Engine graphics contract");
    {
        auto image = GenImageColor(8, 8, GREEN);
        require(image.pixels.size() == 64 && image.pixels.front().g == GREEN.g);
        auto texture = LoadTextureFromImage(image);
        require(texture.IsValid() && texture.width == 8 && texture.height == 8);
        auto copy = texture;
        image.pixels.assign(64, RED);
        UpdateTexture(texture, image.pixels.data());
        auto readback = LoadImageFromTexture(copy);
        require(readback.pixels.front().r == RED.r && readback.pixels.front().g == RED.g);
        auto target = LoadRenderTexture(8, 8);
        BeginTextureMode(target); ClearBackground(BLUE); EndTextureMode();
        auto rendered = LoadImageFromTexture(target.texture);
        require(rendered.pixels.front().b == BLUE.b);
        UnloadTexture(texture);
        require(!copy.IsValid());
        UnloadTexture(copy); // Releasing another shared copy must be harmless.
        UnloadRenderTexture(target);
        require(!target.IsValid());
        auto linesTarget = LoadRenderTexture(64, 64);
        auto shader = LoadShader(CONTRACT_SHADER_DIR "/line.vs", CONTRACT_SHADER_DIR "/line.fs");
        require(shader.IsValid());
        RenderBufferArrayLine lines;
        lines.AddVertices({-.5f,-.5f, .5f,-.5f, .5f,.5f, -.5f,.5f});
        lines.AddOffsets({0}); lines.AddCounts({4}); lines.LoadBuffer();
        Matrix identity{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        BeginTextureMode(linesTarget);
        ClearBackground(BLACK);
        lines.RenderBuffer(shader, identity, 0);
        DrawRectangle(30,30,4,4,GREEN);
        EndTextureMode();
        auto lineImage = LoadImageFromTexture(linesTarget.texture);
        int redPixels=0, greenPixels=0;
        for (auto pixel : lineImage.pixels) {
            if (pixel.r > 200 && pixel.g < 30) ++redPixels;
            if (pixel.g > 150 && pixel.r < 30) ++greenPixels;
        }
        require(redPixels > 50 && greenPixels >= 16);
        UnloadRenderTexture(linesTarget);
        UnloadShader(shader);
    }
    CloseWindow();
}
