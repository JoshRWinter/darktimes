#version 330 core

uniform mat4 projection;
uniform mat4 view;

layout (location = 0) in vec2 pos;

void main()
{
	gl_Position = view * projection * vec4(pos.xy, 0.0, 1.0);
}
