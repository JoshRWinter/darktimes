#version 330 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texcoords;

out vec2 ftexcoords;

void main()
{
    ftexcoords = texcoords;
    gl_Position = projection * view * model * vec4(vertex.xy, 0.0, 1.0);
}
