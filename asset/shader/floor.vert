#version 330 core

uniform mat4 projection;
uniform mat4 view;

layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 texcoord;

out vec3 ftexcoord;

void main()
{
	gl_Position = projection * view * vec4(pos.xy, 0.0, 1.0);
	ftexcoord = texcoord;
}
