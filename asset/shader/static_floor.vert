#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in int layer;

out vec3 ftexcoord;

uniform mat4 view_projection;

void main()
{
    ftexcoord = vec3(texcoord.st, layer);
    gl_Position = view_projection * vec4(pos.xy, 0.0, 1.0);
}
