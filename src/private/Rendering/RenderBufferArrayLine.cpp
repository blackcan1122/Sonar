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

void RenderBufferArrayLine::RenderBuffer()
{
    if (vao == 0) return;

    glBindVertexArray(vao);

    glMultiDrawArrays(
        GL_LINE_LOOP,
        GLOffsets.data(),    // array of starting vertex indices
        GLCounts.data(),    // array of vertex counts per loop
        DrawCount         // number of loops
    );
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