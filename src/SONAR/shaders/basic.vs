#version 430

layout(location = 0) in vec2 aPos;

uniform mat4 uMVP; // matrix we'll send
uniform float uWorldOffsetX;

void main()
{
    gl_Position = uMVP * vec4(aPos.x + uWorldOffsetX, aPos.y, 0.0, 1.0);
}
