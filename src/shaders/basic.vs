#version 430

layout(location = 0) in vec2 aPos;

uniform mat4 uMVP; // matrix we'll send

void main()
{
    gl_Position = uMVP * vec4(aPos, 0.0, 1);
}
