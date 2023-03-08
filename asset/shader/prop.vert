#version 330 core

uniform mat4 projection;
uniform mat4 view;

layout (location = 0) in vec2 pos;

void main()
{
	gl_Position = projection * view * vec4(pos.x, pos.y, 0.0, 1.0);
}
