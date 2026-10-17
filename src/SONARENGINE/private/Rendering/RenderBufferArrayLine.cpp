#include "external/glad.h"
#include "Rendering/Backend.hpp"
#include "Rendering/RenderBufferArrayLine.hpp"

void RenderBufferArrayLine::LoadBuffer()
{
    this->GLCounts.clear();
    this->GLOffsets.clear();
    if (vbo != 0)
    {
        glDeleteBuffers(1, &vbo);
    }

    if (vao != 0)
    {
        glDeleteVertexArrays(1, &vao);
    }

    // VBO and VAO setup
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Upload vertex data
    glBufferData(GL_ARRAY_BUFFER,
                 Vertices.size() * sizeof(Vertices[0]),
                 Vertices.data(),
                 GL_STATIC_DRAW);

    // Set vertex attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    DrawCount = GLsizei(Counts.size());
    GLOffsets.assign(Offsets.begin(), Offsets.end());
    GLCounts.assign(Counts.begin(), Counts.end());
}

void RenderBufferArrayLine::RenderBuffer(const Sonar::Shader& shader, const Sonar::Matrix& projection, float worldOffset)
{
    if (vao == 0) return;

    Sonar::BeginLineRendering(shader, projection, worldOffset);

    glBindVertexArray(vao);

    glMultiDrawArrays(
        GL_LINE_LOOP,
        GLOffsets.data(),    // array of starting vertex indices
        GLCounts.data(),    // array of vertex counts per loop
        DrawCount         // number of loops
    );
    Sonar::EndLineRendering();
}

RenderBufferArrayLine::~RenderBufferArrayLine()
{
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
}
    RenderBufferArrayLine::RenderBufferArrayLine(const RenderBufferArrayLine& Other)
        :Vertices(Other.Vertices),
        Offsets(Other.Offsets),
        Counts(Other.Counts)
    {
        this->LoadBuffer(); // Important to make sure we got a unique vao and vbo
    }

    RenderBufferArrayLine& RenderBufferArrayLine::operator=(const RenderBufferArrayLine& Other)
    {
        if (this == &Other)
        {
            return *this;
        }

        this->Vertices = Other.Vertices;
        this->Counts = Other.Counts;
        this->Offsets = Other.Offsets;

        LoadBuffer();

        return *this;
    }

    RenderBufferArrayLine::RenderBufferArrayLine(RenderBufferArrayLine&& Other) noexcept
        :vao(Other.vao),
         vbo(Other.vbo),
         DrawCount(Other.DrawCount),
         Vertices(std::move(Other.Vertices)),
         Offsets(std::move(Other.Offsets)),
         Counts(std::move(Other.Counts)),
         GLOffsets(std::move(Other.GLOffsets)),
         GLCounts(std::move(Other.GLCounts))
    {
        Other.vao = 0;
        Other.vbo = 0;
    }

    RenderBufferArrayLine& RenderBufferArrayLine::operator=(RenderBufferArrayLine&& Other) noexcept
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
        this->GLCounts = std::move(Other.GLCounts);
        this->GLOffsets = std::move(Other.GLOffsets);
        this->DrawCount = Other.DrawCount;

        Other.vao = 0;
        Other.vbo = 0;

        return *this;
    }