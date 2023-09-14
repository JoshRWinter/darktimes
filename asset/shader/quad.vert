#version 330 core

layout (location = 0) in vec2 vert;
layout (location = 1) in ivec2 texcoord;

out vec2 ftexcoord;

uniform mat4 view_projection;
uniform vec2 position;
uniform vec2 size;

void main()
{
    ftexcoord = vec2(texcoord.x / 65535.0, texcoord.y / 65535.0);
    mat4 trans = mat4(size.x, 0.0, 0.0, 0.0, 0.0, size.y, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, position.x, position.y, 0.0, 1.0);
    gl_Position = view_projection * trans * vec4(vert.xy, 0.0, 1.0);
}
