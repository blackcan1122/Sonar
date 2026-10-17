#version 330
layout(location = 0) in vec2 position;
uniform mat4 uMVP;
uniform float uWorldOffsetX;
void main() { gl_Position = uMVP * vec4(position + vec2(uWorldOffsetX, 0), 0, 1); }
