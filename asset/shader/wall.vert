#version 330 core

uniform mat4 projection;

layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 color;

out vec3 fcolor;

void main()
{
	fcolor = color;
	gl_Position = projection * vec4(pos.xy, 0.0, 1.0);
}
