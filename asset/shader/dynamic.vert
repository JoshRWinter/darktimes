#version 330 core

uniform mat4 projection;
uniform mat4 view;

layout (std140) uniform transforms
{
    mat4 models[2];
};

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texcoords;
layout (location = 3) in int drawid;

out vec2 ftexcoords;

void main()
{
    ftexcoords = texcoords;
    gl_Position = projection * view * models[drawid] * vec4(vertex.xy, 0.0, 1.0);
}
