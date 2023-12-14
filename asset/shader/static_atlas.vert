#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in ivec2 texcoord;

out vec2 ftexcoord;

uniform mat4 view_projection;

void main()
{
    ftexcoord = vec2(texcoord.st / 65535.0);
    gl_Position = view_projection * vec4(position.xy, 0.0, 1.0);
}
