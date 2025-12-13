#pragma once
#include "Base/Core.h"
#include "external/glad.h"


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
    ~RenderBufferArrayLine();

    
    // Copy Constructor
    RenderBufferArrayLine(const RenderBufferArrayLine& Other)
        :Vertices(Other.Vertices),
        Offsets(Other.Offsets),
        Counts(Other.Counts)
    {
        this->LoadBuffer(); // Important to make sure we got a unique vao and vbo
    }

    // Copy Assignment
    RenderBufferArrayLine& operator=(const RenderBufferArrayLine& Other)
    {
        if (this == &Other)
        {
            return *this;
        }

        if (vbo != 0)
        {
            glDeleteBuffers(1, &vbo);
        }

        if (vao != 0)
        {
            glDeleteVertexArrays(1, &vao);
        }
        
        this->Vertices = Other.Vertices;
        this->Counts = Other.Counts;
        this->Offsets = Other.Offsets;

        LoadBuffer();

        return *this;
    }


    // Move Constructor
    RenderBufferArrayLine(RenderBufferArrayLine&& Other) noexcept
        :vao(Other.vao),
         vbo(Other.vbo),
         Vertices(std::move(Other.Vertices)),
         Offsets(std::move(Other.Offsets)),
         Counts(std::move(Other.Counts))
    {
        Other.vao = 0;
        Other.vbo = 0;
    }

    // Move Assignment
    RenderBufferArrayLine& operator=(RenderBufferArrayLine&& Other) noexcept
    {
        if (this == &Other)
        {
            return *this;
        }

        if (vbo != 0)
        {
            glDeleteBuffers(1, &vbo);
        }
        if (vao != 0)
        {
            glDeleteVertexArrays(1, &vao);
        }

        this->vao = Other.vao;
        this->vbo = Other.vbo;
        this->Vertices = std::move(Other.Vertices);
        this->Counts = std::move(Other.Counts);
        this->Offsets = std::move(Other.Offsets);

        Other.vao = 0;
        Other.vbo = 0;


        return *this;
    }

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