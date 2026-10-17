#pragma once
#include "Base/Macros.h"
#include "Rendering/Graphics.hpp"

#include <utility>
#include <vector>


/**
 * @file RenderBufferArrayLine.hpp
 * @brief Temporary placeholder for render buffer line representation.
 *
 * @note This class is a provisional implementation and does not reflect the final targeted design.
 *       Significant changes are expected in the future as the rendering system evolves.
 */


// Could be inhertied from IObject im not sure yet
class SONARENGINE_API RenderBufferArrayLine
{
public:
    RenderBufferArrayLine() = default;
    ~RenderBufferArrayLine();


    RenderBufferArrayLine(const RenderBufferArrayLine& Other);
    RenderBufferArrayLine& operator=(const RenderBufferArrayLine& Other);
    RenderBufferArrayLine(RenderBufferArrayLine&& Other) noexcept;
    RenderBufferArrayLine& operator=(RenderBufferArrayLine&& Other) noexcept;


    void AddVertices(std::vector<float> inVertex) noexcept
    {
        Vertices = std::move(inVertex);
    }

    void AddOffsets(std::vector<unsigned int> inOffsets) noexcept
    {
        Offsets = std::move(inOffsets);
    }

    void AddCounts(std::vector<unsigned int> inCounts) noexcept
    {
        Counts = std::move(inCounts);
    }

    void RenderBuffer(const Sonar::Shader& shader, const Sonar::Matrix& projection, float worldOffset);
    void LoadBuffer();


private:


    std::vector<float> Vertices; // All the Vertices
    std::vector<unsigned int> Offsets; // The Offsets between the Arrays
    std::vector<unsigned int> Counts; // The Amount of Lines

    unsigned int vao = 0, vbo = 0;
    int DrawCount = 0;
    std::vector<int>  GLOffsets;
    std::vector<int> GLCounts;

};