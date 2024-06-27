#version 330 core

layout (location = 0) in vec2 pos;

uniform mat4 view_projection;

void main()
{
    gl_Position = view_projection * vec4(pos.xy, 0.0, 1.0);
}
