#version 330 core

uniform mat4 projection;

layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 texcoord;

out vec3 ftexcoord;

void main()
{
	gl_Position = projection * vec4(pos.xy, 0.0, 1.0);
	ftexcoord = texcoord;
}
