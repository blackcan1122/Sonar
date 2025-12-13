#pragma once
#include "Base/Core.h"

/**
 * @file RenderBufferArrayLine.hpp
 * @brief Temporary placeholder for render buffer line representation.
 *
 * @note This class is a provisional implementation and does not reflect the final targeted design.
 *       Significant changes are expected in the future as the rendering system evolves.
 */


// Could be inhertied from IObject im not sure yet
class RenderBufferArrayLine
{
public:
    RenderBufferArrayLine() = default;

    unsigned int vao = 0;
    unsigned int vbo = 0;
    

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

    void RenderBuffer();
    void LoadBuffer();


private:


    std::vector<float> Vertices; // All the Vertices
    std::vector<unsigned int> Offsets; // The Offsets between the Arrays
    std::vector<unsigned int> Counts; // The Amount of Lines

};