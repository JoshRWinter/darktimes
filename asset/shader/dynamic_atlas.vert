#version 330 core

layout (location = 0) in vec2 vert;
layout (location = 1) in ivec2 texcoord;

out vec2 ftexcoord;

uniform mat4 view_projection;
uniform vec2 position;
uniform vec2 size;
uniform float rotation;

void main()
{
    mat4 translate = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, position.x, position.y, 0.0, 1.0);
    mat4 scale = mat4(size.x, 0.0, 0.0, 0.0, 0.0, size.y, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    mat4 rotation = mat4(cos(rotation), sin(rotation), 0.0, 0.0, -sin(rotation), cos(rotation), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);

    ftexcoord = vec2(texcoord.st / 65535.0);
    gl_Position = view_projection * translate * scale * rotation * vec4(vert.xy, 0.0, 1.0);
}
